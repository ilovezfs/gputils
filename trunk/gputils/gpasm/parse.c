/* A Bison parser, made from parse.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	LABEL	257
# define	IDENTIFIER	258
# define	CBLOCK	259
# define	ENDC	260
# define	FILL	261
# define	NUMBER	262
# define	STRING	263
# define	INCLUDE	264
# define	UPPER	265
# define	HIGH	266
# define	LOW	267
# define	LSH	268
# define	RSH	269
# define	GREATER_EQUAL	270
# define	LESS_EQUAL	271
# define	EQUAL	272
# define	NOT_EQUAL	273
# define	LOGICAL_AND	274
# define	LOGICAL_OR	275
# define	ASSIGN_PLUS	276
# define	ASSIGN_MINUS	277
# define	ASSIGN_MULTIPLY	278
# define	ASSIGN_DIVIDE	279
# define	ASSIGN_MODULUS	280
# define	ASSIGN_LSH	281
# define	ASSIGN_RSH	282
# define	ASSIGN_AND	283
# define	ASSIGN_OR	284
# define	ASSIGN_XOR	285
# define	INCREMENT	286
# define	DECREMENT	287
# define	TBL_NO_CHANGE	288
# define	TBL_POST_INC	289
# define	TBL_POST_DEC	290
# define	TBL_PRE_INC	291
# define	CONCAT	292
# define	VAR	293
# define	VARLAB_BEGIN	294
# define	VAR_BEGIN	295
# define	VAR_END	296

#line 1 "parse.y"

/* Parser for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 
   James Bowman, Craig Franklin

This file is part of gputils.

gputils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

gputils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with gputils; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "stdhdr.h"

#include "libgputils.h"
#include "gpasm.h"
#include "evaluate.h"
#include "gperror.h"
#include "directive.h"
#include "lst.h"
#include "macro.h"
#include "coff.h"
#include "scan.h"

void yyerror(char *message)
{
  gperror(103, message);
}

int yylex(void);
extern int _16bit_core;

/************************************************************************/

/* Some simple functions for building parse trees */

static struct pnode *mk_pnode(enum pnode_tag tag)
{
  struct pnode *new = malloc(sizeof(*new));
  new->tag = tag;
  return new;
}

struct pnode *mk_constant(int value)
{
  struct pnode *new = mk_pnode(constant);
  new->value.constant = value;
  return new;
}

static struct pnode *mk_symbol(char *value)
{
  struct pnode *new = mk_pnode(symbol);
  new->value.symbol = value;
  return new;
}

static struct pnode *mk_string(char *value)
{
  struct pnode *new = mk_pnode(string);
  new->value.string = value;
  return new;
}

struct pnode *mk_list(struct pnode *head, struct pnode *tail)
{
  struct pnode *new = mk_pnode(list);
  new->value.list.head = head;
  new->value.list.tail = tail;
  return new;
}

static struct pnode *mk_2op(int op, struct pnode *p0, struct pnode *p1)
{
  struct pnode *new = mk_pnode(binop);
  new->value.binop.op = op;
  new->value.binop.p0 = p0;
  new->value.binop.p1 = p1;
  return new;
}

static struct pnode *mk_1op(int op, struct pnode *p0)
{
  struct pnode *new = mk_pnode(unop);  new->value.unop.op = op;
  new->value.unop.p0 = p0;
  return new;
}

/************************************************************************/
/* shared functions */

gpasmVal set_label(char *label, struct pnode *parms)
{
  gpasmVal value = 0;

  if (asm_enabled()) {
    value = do_or_append_insn("set", parms);
    if (!state.mac_prev) {
      set_global(label, value, TEMPORARY, gvt_constant);
    }
  }

  return value;
}

int return_op(int operation) 
{
  /* returns an operator for the replacement of i+=1 with i=i+1*/
  switch(operation) {
  case ASSIGN_PLUS:     return '+';
  case ASSIGN_MINUS:    return '-';
  case ASSIGN_MULTIPLY: return '*';
  case ASSIGN_DIVIDE:   return '/';
  case ASSIGN_MODULUS:  return '%';
  case ASSIGN_LSH:      return LSH;
  case ASSIGN_RSH:      return RSH;
  case ASSIGN_AND:      return '&';
  case ASSIGN_OR:       return '|';
  case ASSIGN_XOR:      return '^'; 
  default:
    assert(0); /* Unhandled operator */ 
  }

  return 0;
}

void next_line(int value)
{
  char l[BUFSIZ];
  char *e = l;

  if ((state.src->type == src_macro) || 
      (state.src->type == src_while)) {
    /* while loops can be defined inside a macro or nested */
    if (state.mac_prev) {
      state.lst.line.linetype = none;
      if (state.mac_body)
	state.mac_body->src_line = strdup(state.src->lst.m->src_line);
    }

    if (((state.src->type == src_while) || (state.lst.expand)) &&
        (state.pass == 2)) {
      assert(state.src->lst.m->src_line != NULL);
      lst_format_line(state.src->lst.m->src_line, value);
    }
    
    if (state.src->lst.m->next) {
      state.src->lst.m = state.src->lst.m->next;
    }
  } else if ((state.src->type == src_file) &&
             (state.src->lst.f != NULL)) {
    fgets(l, BUFSIZ, state.src->lst.f);
    l[strlen(l) - 1] = '\0';	/* Eat the trailing newline */

    if (state.mac_prev) {
      state.lst.line.linetype = none;
      if (state.mac_body)
	state.mac_body->src_line = strdup(l);
    }

    if (state.pass == 2) {
      lst_format_line(e, value);
    }
  }

  state.src->line_number++;

  switch (state.next_state) {
    case state_exitmacro:
      execute_exitm();
      break;

    case state_include:
      open_src(state.next_buffer.file, 1);
      free(state.next_buffer.file);
      break;

    case state_macro:
      /* push the label for local directive */
      state.stTop = push_macro_symbol_table(state.stTop);
      execute_macro(state.next_buffer.macro, 0);
      break;

    case state_section:
      /* create a new coff section */
      coff_new_section(state.obj.new_sec_name, 
                       state.obj.new_sec_addr, 
                       state.obj.new_sec_flags);
      break;

    case state_while:
      execute_macro(state.next_buffer.macro, 1);
      break;

    default:
      break;
  }
 
}


/************************************************************************/


#line 218 "parse.y"
#ifndef YYSTYPE
typedef union {
  gpasmVal i;
  char *s;
  struct pnode *p;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 1
#endif



#define	YYFINAL		155
#define	YYFLAG		-32768
#define	YYNTBASE	62

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 296 ? yytranslate[x] : 96)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      56,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    54,     2,     2,    60,    53,    22,     2,
      61,    57,    51,    49,    58,    50,     2,    52,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    59,     2,
      20,    27,    21,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    24,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    23,     2,    55,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    25,    26,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     2,     6,    10,    15,    20,    24,    28,
      31,    33,    35,    38,    41,    45,    52,    60,    61,    69,
      70,    77,    82,    83,    86,    88,    91,    94,    98,   100,
     104,   106,   110,   112,   114,   116,   118,   120,   122,   124,
     126,   128,   130,   132,   135,   139,   141,   143,   145,   149,
     151,   153,   157,   159,   161,   165,   167,   169,   173,   175,
     177,   179,   181,   185,   187,   189,   191,   193,   195,   197,
     199,   203,   205,   207,   209,   213,   215,   217,   219,   223,
     225,   227,   229,   231,   234,   236,   238,   240,   242,   244,
     246,   248,   250,   252,   254,   258,   260,   262,   264,   266,
     268,   272,   276,   278,   282
};
static const short yyrhs[] =
{
      -1,     0,    62,    63,    64,     0,    62,     1,    56,     0,
      95,    72,    74,    56,     0,    95,    27,    74,    56,     0,
      95,    39,    56,     0,    95,    38,    56,     0,    95,    65,
       0,    65,     0,    56,     0,    10,    56,     0,     4,    56,
       0,     4,    73,    56,     0,     7,     4,    57,    58,    74,
      56,     0,     7,     4,    73,    57,    58,    74,    56,     0,
       0,     5,    74,    56,    66,    68,     6,    56,     0,     0,
       5,    56,    67,    68,     6,    56,     0,     5,     1,     6,
      56,     0,     0,    68,    69,     0,    56,     0,    70,    56,
       0,    95,    56,     0,    95,    74,    56,     0,    71,     0,
      70,    58,    71,     0,    94,     0,    94,    59,    74,     0,
      28,     0,    29,     0,    30,     0,    31,     0,    32,     0,
      33,     0,    34,     0,    35,     0,    36,     0,    37,     0,
      74,     0,    74,    58,     0,    74,    58,    73,     0,    75,
       0,     9,     0,    77,     0,    75,    76,    77,     0,    27,
       0,    79,     0,    77,    78,    79,     0,    26,     0,    81,
       0,    79,    80,    81,     0,    25,     0,    83,     0,    81,
      82,    83,     0,    22,     0,    23,     0,    24,     0,    85,
       0,    83,    84,    85,     0,    20,     0,    21,     0,    18,
       0,    19,     0,    16,     0,    17,     0,    87,     0,    85,
      86,    87,     0,    14,     0,    15,     0,    89,     0,    87,
      88,    89,     0,    49,     0,    50,     0,    91,     0,    89,
      90,    91,     0,    51,     0,    52,     0,    53,     0,    93,
       0,    92,    93,     0,    11,     0,    12,     0,    13,     0,
      50,     0,    54,     0,    55,     0,    49,     0,    94,     0,
       8,     0,    60,     0,    61,    74,    57,     0,    51,     0,
      41,     0,    42,     0,    43,     0,     4,     0,    47,    74,
      57,     0,    47,    74,    48,     0,     3,     0,    46,    74,
      57,     0,    46,    74,    48,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   257,   259,   259,   266,   272,   284,   293,   302,   311,
     376,   389,   398,   403,   408,   413,   429,   445,   445,   452,
     452,   459,   466,   467,   474,   476,   478,   483,   490,   492,
     496,   501,   508,   508,   508,   508,   509,   509,   509,   509,
     510,   510,   512,   517,   523,   530,   532,   539,   541,   548,
     550,   552,   559,   561,   563,   570,   572,   574,   581,   581,
     581,   583,   585,   592,   592,   592,   592,   592,   592,   594,
     596,   603,   603,   605,   607,   614,   614,   616,   618,   625,
     625,   625,   627,   629,   636,   636,   636,   636,   636,   636,
     636,   638,   643,   648,   653,   658,   663,   668,   673,   680,
     685,   690,   698,   703,   711
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "LABEL", "IDENTIFIER", "CBLOCK", "ENDC", 
  "FILL", "NUMBER", "STRING", "INCLUDE", "UPPER", "HIGH", "LOW", "LSH", 
  "RSH", "GREATER_EQUAL", "LESS_EQUAL", "EQUAL", "NOT_EQUAL", "'<'", 
  "'>'", "'&'", "'|'", "'^'", "LOGICAL_AND", "LOGICAL_OR", "'='", 
  "ASSIGN_PLUS", "ASSIGN_MINUS", "ASSIGN_MULTIPLY", "ASSIGN_DIVIDE", 
  "ASSIGN_MODULUS", "ASSIGN_LSH", "ASSIGN_RSH", "ASSIGN_AND", "ASSIGN_OR", 
  "ASSIGN_XOR", "INCREMENT", "DECREMENT", "TBL_NO_CHANGE", "TBL_POST_INC", 
  "TBL_POST_DEC", "TBL_PRE_INC", "CONCAT", "VAR", "VARLAB_BEGIN", 
  "VAR_BEGIN", "VAR_END", "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", 
  "'\\n'", "')'", "','", "':'", "'$'", "'('", "program", "@1", "line", 
  "statement", "@2", "@3", "const_block", "const_line", "const_def_list", 
  "const_def", "assign_equal_ops", "parameter_list", "expr", "e9", "e9op", 
  "e8", "e8op", "e7", "e7op", "e6", "e6op", "e5", "e5op", "e4", "e4op", 
  "e3", "e3op", "e2", "e2op", "e1", "e1op", "e0", "cidentifier", 
  "label_concat", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    62,    63,    62,    62,    64,    64,    64,    64,    64,
      64,    65,    65,    65,    65,    65,    65,    66,    65,    67,
      65,    65,    68,    68,    69,    69,    69,    69,    70,    70,
      71,    71,    72,    72,    72,    72,    72,    72,    72,    72,
      72,    72,    73,    73,    73,    74,    74,    75,    75,    76,
      77,    77,    78,    79,    79,    80,    81,    81,    82,    82,
      82,    83,    83,    84,    84,    84,    84,    84,    84,    85,
      85,    86,    86,    87,    87,    88,    88,    89,    89,    90,
      90,    90,    91,    91,    92,    92,    92,    92,    92,    92,
      92,    93,    93,    93,    93,    93,    93,    93,    93,    94,
      94,    94,    95,    95,    95
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     0,     3,     3,     4,     4,     3,     3,     2,
       1,     1,     2,     2,     3,     6,     7,     0,     7,     0,
       6,     4,     0,     2,     1,     2,     2,     3,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     1,     1,     1,     3,     1,
       1,     3,     1,     1,     3,     1,     1,     3,     1,     1,
       1,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     3,     1,     1,     1,     3,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     1,     1,     1,     1,
       3,     3,     1,     3,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     0,     0,     0,     4,   102,     0,     0,     0,     0,
       0,    11,     3,    10,     0,    99,    92,    46,    84,    85,
      86,    96,    97,    98,     0,    90,    87,    95,    88,    89,
      13,    93,     0,     0,    42,    45,    47,    50,    53,    56,
      61,    69,    73,    77,     0,    82,    91,     0,    19,     0,
       0,    12,     0,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,     0,     0,     9,     0,     0,     0,
      14,    43,    49,     0,    52,     0,    55,     0,    58,    59,
      60,     0,    67,    68,    65,    66,    63,    64,     0,    71,
      72,     0,    75,    76,     0,    79,    80,    81,     0,    83,
       0,    22,    17,     0,     0,   104,   103,     0,     8,     7,
       0,   101,   100,    94,    44,    48,    51,    54,    57,    62,
      70,    74,    78,    21,     0,    22,     0,     0,     6,     5,
       0,    24,    23,     0,    28,    30,     0,     0,     0,     0,
      20,    25,     0,     0,    26,     0,     0,    15,     0,    29,
      31,    27,    18,    16,     0,     0
};

static const short yydefgoto[] =
{
       1,     3,    12,    13,   125,   101,   124,   132,   133,   134,
      67,    33,    34,    35,    73,    36,    75,    37,    77,    38,
      81,    39,    88,    40,    91,    41,    94,    42,    98,    43,
      44,    45,    46,   136
};

static const short yypact[] =
{
  -32768,   238,   -35,    97,-32768,-32768,    22,     1,    37,   -11,
     159,-32768,-32768,-32768,   268,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,   159,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,   159,    10,    -9,    40,    44,    49,     5,   235,
      39,   -42,    34,-32768,   189,-32768,-32768,    69,-32768,    25,
      84,-32768,   -37,   159,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,    28,    35,-32768,   159,   -25,    41,
  -32768,   159,-32768,   174,-32768,   174,-32768,   174,-32768,-32768,
  -32768,   174,-32768,-32768,-32768,-32768,-32768,-32768,   174,-32768,
  -32768,   174,-32768,-32768,   174,-32768,-32768,-32768,   174,-32768,
      38,-32768,-32768,    32,    42,-32768,-32768,    47,-32768,-32768,
      50,-32768,-32768,-32768,-32768,    44,    49,     5,   235,    39,
     -42,    34,-32768,-32768,    12,-32768,   159,    52,-32768,-32768,
      55,-32768,-32768,   -18,-32768,    46,   105,    33,    56,   159,
  -32768,-32768,     0,   159,-32768,    59,    64,-32768,    65,-32768,
  -32768,-32768,-32768,-32768,   108,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,   109,-32768,-32768,    -3,-32768,-32768,   -14,
  -32768,   -49,    -7,-32768,-32768,    51,-32768,    62,-32768,    53,
  -32768,    61,-32768,    63,-32768,    58,-32768,    68,-32768,    60,
  -32768,    96,  -118,   147
};


#define	YYLAST		324


static const short yytable[] =
{
      49,   104,    47,    52,    15,    15,   135,    92,    93,    16,
      17,   105,    18,    19,    20,     5,    15,    68,   130,   135,
     106,     4,   114,   111,   135,    69,    15,    78,    79,    80,
      16,    17,   112,    18,    19,    20,     5,    15,   141,   146,
     142,    50,    21,    22,    23,    51,   107,    24,    24,    71,
      25,    26,    27,    89,    90,    28,    29,    48,    10,    24,
     110,    31,    32,    21,    22,    23,    70,    72,   131,    24,
      74,    25,    26,    27,    76,   100,    28,    29,    30,    10,
      24,   102,    31,    32,   108,    95,    96,    97,    15,   131,
     126,   109,    16,    17,   123,    18,    19,    20,   113,   127,
       5,     6,     7,   128,     8,   143,   129,     9,   155,    15,
     139,   140,   147,    16,    17,   151,    18,    19,    20,   138,
     152,   153,   137,    66,   115,    21,    22,    23,   149,   145,
     117,    24,   148,    25,    26,    27,   150,   116,    28,    29,
      99,   103,   118,    10,    31,    32,    21,    22,    23,   120,
      14,   119,    24,    11,    25,    26,    27,     0,   122,    28,
      29,   144,   121,    15,     0,    31,    32,    16,    17,     0,
      18,    19,    20,     0,     0,     0,     0,     0,    15,     0,
       0,     0,    16,     0,     0,    18,    19,    20,     0,     0,
       0,     0,     0,    15,     0,     0,     0,    16,     0,     0,
      21,    22,    23,     0,     0,     0,    24,     0,    25,    26,
      27,     0,     0,    28,    29,    21,    22,    23,     0,    31,
      32,    24,     0,    25,    26,    27,     0,     0,    28,    29,
      21,    22,    23,     0,    31,    32,    24,     0,   154,     2,
      27,    -2,    -2,    -2,     0,    -2,     0,     0,    -2,    31,
      32,    82,    83,    84,    85,    86,    87,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     6,     7,     0,     8,     0,     0,     9,     0,
       0,     0,     0,     0,    -2,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    -2,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    11
};

static const short yycheck[] =
{
       7,    50,     1,    10,     4,     4,   124,    49,    50,     8,
       9,    48,    11,    12,    13,     3,     4,    24,     6,   137,
      57,    56,    71,    48,   142,    32,     4,    22,    23,    24,
       8,     9,    57,    11,    12,    13,     3,     4,    56,     6,
      58,     4,    41,    42,    43,    56,    53,    47,    47,    58,
      49,    50,    51,    14,    15,    54,    55,    56,    46,    47,
      67,    60,    61,    41,    42,    43,    56,    27,    56,    47,
      26,    49,    50,    51,    25,     6,    54,    55,    56,    46,
      47,    56,    60,    61,    56,    51,    52,    53,     4,    56,
      58,    56,     8,     9,    56,    11,    12,    13,    57,    57,
       3,     4,     5,    56,     7,    59,    56,    10,     0,     4,
      58,    56,    56,     8,     9,    56,    11,    12,    13,   126,
      56,    56,   125,    14,    73,    41,    42,    43,   142,   136,
      77,    47,   139,    49,    50,    51,   143,    75,    54,    55,
      44,    57,    81,    46,    60,    61,    41,    42,    43,    91,
       3,    88,    47,    56,    49,    50,    51,    -1,    98,    54,
      55,    56,    94,     4,    -1,    60,    61,     8,     9,    -1,
      11,    12,    13,    -1,    -1,    -1,    -1,    -1,     4,    -1,
      -1,    -1,     8,    -1,    -1,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,     4,    -1,    -1,    -1,     8,    -1,    -1,
      41,    42,    43,    -1,    -1,    -1,    47,    -1,    49,    50,
      51,    -1,    -1,    54,    55,    41,    42,    43,    -1,    60,
      61,    47,    -1,    49,    50,    51,    -1,    -1,    54,    55,
      41,    42,    43,    -1,    60,    61,    47,    -1,     0,     1,
      51,     3,     4,     5,    -1,     7,    -1,    -1,    10,    60,
      61,    16,    17,    18,    19,    20,    21,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     4,     5,    -1,     7,    -1,    -1,    10,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    56,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    56
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 2:
#line 261 "parse.y"
{ 
          state.lst.line.was_org = state.org; 
          state.lst.line.linetype = none; 
          state.next_state = state_nochange;
        }
    break;
case 4:
#line 267 "parse.y"
{ 
	  next_line(0);
	}
    break;
case 5:
#line 274 "parse.y"
{
	  struct pnode *parms;
	  int exp_result;

          exp_result = do_insn("set", mk_list(yyvsp[-1].p, NULL));          
          parms = mk_list(mk_2op(return_op(yyvsp[-2].i), 
                                 mk_symbol(yyvsp[-3].s), 
                                 mk_constant(exp_result)), NULL);
	  next_line(set_label(yyvsp[-3].s, parms));
	}
    break;
case 6:
#line 286 "parse.y"
{
	  struct pnode *parms;

	  /* implements i = 6 + 1 */           
	  parms = mk_list(yyvsp[-1].p, NULL);
	  next_line(set_label(yyvsp[-3].s, parms));
	}
    break;
case 7:
#line 295 "parse.y"
{
	  struct pnode *parms;

	  /* implements i-- */           
	  parms = mk_list(mk_1op(DECREMENT, mk_symbol(yyvsp[-2].s)), NULL);
	  next_line(set_label(yyvsp[-2].s, parms));
	}
    break;
case 8:
#line 304 "parse.y"
{
          struct pnode *parms;

	  /* implements i++ */          
	  parms = mk_list(mk_1op(INCREMENT, mk_symbol(yyvsp[-2].s)), NULL);
	  next_line(set_label(yyvsp[-2].s, parms));
	}
    break;
case 9:
#line 313 "parse.y"
{
	  if (state.lst.line.linetype == none)
	    state.lst.line.linetype = insn;
	  
	  if (asm_enabled()) {
	    if (state.mac_head) {
	      /* This is a macro definition.  Set it up */
	      struct symbol *mac;
	      struct macro_head *h = NULL;

	      mac = get_symbol(state.stMacros, yyvsp[-1].s);
	      if (mac)
		h = get_symbol_annotation(mac);

	      /* It's not an error if macro was defined on pass 1 and
		 we're in pass 2. */
	      if (h &&
		  !((h->pass == 1) && (state.pass == 2))) {
		gperror(GPE_DUPLICATE_MACRO, NULL);
	      } else {
		if (!mac)
		  mac = add_symbol(state.stMacros, yyvsp[-1].s);
		annotate_symbol(mac, state.mac_head);
		h = state.mac_head;
		h->line_number = state.src->line_number;
		h->file_symbol = state.src->file_symbol;
	      }
	      h->pass = state.pass;
 	      
	      /* The macro is defined so allow calls. */
	      if (state.pass == 2)
	        h->defined = 1;

	      state.mac_head = NULL;
	    } else if (!state.mac_prev) {
	      /* Outside a macro, just define the label. */
	      switch (state.lst.line.linetype) {
	      case sec:
		strncpy(state.obj.new_sec_name, yyvsp[-1].s, 78);
		break;
	      case set:
		set_global(yyvsp[-1].s, yyvsp[0].i, TEMPORARY, gvt_constant);
		break;
              case org:
	      case equ:
		set_global(yyvsp[-1].s, yyvsp[0].i, PERMANENT, gvt_constant);
		break;
	      case insn:
		set_global(yyvsp[-1].s, yyvsp[0].i << _16bit_core, PERMANENT, gvt_address);
		break;
	      case res:
		set_global(yyvsp[-1].s, yyvsp[0].i, PERMANENT, gvt_static);
                break;
	      case dir:
                gperror(GPE_ILLEGAL_LABEL, NULL);
                break;
              default:
		break;
	      }
	    }
	  }
	  next_line(yyvsp[0].i);
	}
    break;
case 10:
#line 378 "parse.y"
{
	  if (state.mac_head) {
	    /* This is a macro definition, but the label was missing */
	    state.mac_head = NULL;
	    gperror(GPE_NO_MACRO_NAME, NULL);
	  } else {
	    next_line(0);
	  }
	}
    break;
case 11:
#line 391 "parse.y"
{
	  if (!state.mac_prev) {
	    yyval.i = state.org;
	  } else {
	    macro_append();
	  }
	}
    break;
case 12:
#line 400 "parse.y"
{
	  yyval.i = do_or_append_insn("include", mk_list(mk_string(yyvsp[-1].s), NULL));
	}
    break;
case 13:
#line 405 "parse.y"
{
	  yyval.i = do_or_append_insn(yyvsp[-1].s, NULL);
	}
    break;
case 14:
#line 410 "parse.y"
{
	  yyval.i = do_or_append_insn(yyvsp[-2].s, yyvsp[-1].p);
	}
    break;
case 15:
#line 415 "parse.y"
{
	  int number;
	  int i;

          if (!state.mac_prev) {
            number = eval_fill_number(yyvsp[-1].p);

            for (i = 0; i < number; i++) {
              yyval.i = do_insn(yyvsp[-4].s, NULL);
            }
          } else {
	    macro_append();
	  }
	}
    break;
case 16:
#line 431 "parse.y"
{
	  int number;
	  int i;

          if (!state.mac_prev) {
            number = eval_fill_number(yyvsp[-1].p);

            for (i = 0; i < number; i++) {
              yyval.i = do_insn(yyvsp[-5].s, yyvsp[-4].p);
            }
          } else {
	    macro_append();
	  }
	}
    break;
case 17:
#line 446 "parse.y"
{ begin_cblock(yyvsp[-1].p); next_line(0); }
    break;
case 18:
#line 449 "parse.y"
{
	  yyval.i = 0;
	}
    break;
case 19:
#line 453 "parse.y"
{ next_line(0); }
    break;
case 20:
#line 456 "parse.y"
{
           yyval.i = 0;
        }
    break;
case 21:
#line 461 "parse.y"
{
	  yyval.i = 0;
  	}
    break;
case 23:
#line 469 "parse.y"
{
	  next_line(0);
	}
    break;
case 26:
#line 480 "parse.y"
{
	  cblock_expr(mk_symbol(yyvsp[-1].s));
	}
    break;
case 27:
#line 485 "parse.y"
{
	  cblock_expr_incr(mk_symbol(yyvsp[-2].s), yyvsp[-1].p);
	}
    break;
case 30:
#line 498 "parse.y"
{
	  cblock_expr(yyvsp[0].p);
	}
    break;
case 31:
#line 503 "parse.y"
{
	  cblock_expr_incr(yyvsp[-2].p, yyvsp[0].p);
	}
    break;
case 42:
#line 514 "parse.y"
{
	  yyval.p = mk_list(yyvsp[0].p, NULL);
	}
    break;
case 43:
#line 519 "parse.y"
{
	  gperror(GPE_BADCHAR, "Illegal Character (,)");
          yyval.p = mk_list(yyvsp[-1].p, NULL);
	}
    break;
case 44:
#line 525 "parse.y"
{
	  yyval.p = mk_list(yyvsp[-2].p, yyvsp[0].p);
	}
    break;
case 46:
#line 534 "parse.y"
{
	  yyval.p = mk_string(yyvsp[0].s);
        }
    break;
case 48:
#line 543 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;
case 51:
#line 554 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;
case 54:
#line 565 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;
case 57:
#line 576 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;
case 62:
#line 587 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;
case 70:
#line 598 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;
case 74:
#line 609 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;
case 78:
#line 620 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;
case 83:
#line 631 "parse.y"
{
	  yyval.p = mk_1op(yyvsp[-1].i, yyvsp[0].p);
	}
    break;
case 91:
#line 640 "parse.y"
{
	  yyval.p = yyvsp[0].p;
        }
    break;
case 92:
#line 645 "parse.y"
{
	  yyval.p = mk_constant(yyvsp[0].i);
	}
    break;
case 93:
#line 650 "parse.y"
{
	  yyval.p = mk_symbol("$");
	}
    break;
case 94:
#line 655 "parse.y"
{
	  yyval.p = yyvsp[-1].p;
	}
    break;
case 95:
#line 660 "parse.y"
{
	  yyval.p = mk_constant(TBL_NO_CHANGE);
	}
    break;
case 96:
#line 665 "parse.y"
{
	  yyval.p = mk_constant(yyvsp[0].i);
	}
    break;
case 97:
#line 670 "parse.y"
{
	  yyval.p = mk_constant(yyvsp[0].i);
	}
    break;
case 98:
#line 675 "parse.y"
{
	  yyval.p = mk_constant(yyvsp[0].i);
	}
    break;
case 99:
#line 682 "parse.y"
{ 
          yyval.p = mk_symbol(yyvsp[0].s);
        }
    break;
case 100:
#line 687 "parse.y"
{
          yyval.p = mk_2op(CONCAT, mk_symbol(yyvsp[-2].s), mk_1op(VAR, yyvsp[-1].p));
        }
    break;
case 101:
#line 692 "parse.y"
{
          yyval.p = mk_2op(CONCAT, mk_symbol(yyvsp[-2].s), 
                        mk_2op(CONCAT, mk_1op(VAR, yyvsp[-1].p), mk_symbol(yyvsp[0].s)));
        }
    break;
case 102:
#line 700 "parse.y"
{ 
          yyval.s = yyvsp[0].s;
        }
    break;
case 103:
#line 705 "parse.y"
{
          if (!state.mac_prev) {
	    yyval.s = evaluate_concatenation(mk_2op(CONCAT,  mk_symbol(yyvsp[-2].s), 
                           mk_1op(VAR, yyvsp[-1].p)));
	  }
        }
    break;
case 104:
#line 713 "parse.y"
{
          if (!state.mac_prev) {
            yyval.s = evaluate_concatenation(mk_2op(CONCAT,  mk_symbol(yyvsp[-2].s), 
                      mk_2op(CONCAT, mk_1op(VAR, yyvsp[-1].p), mk_symbol(yyvsp[0].s))));
	  }
        }
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 721 "parse.y"


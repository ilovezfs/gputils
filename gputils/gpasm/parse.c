
/*  A Bison parser, made from parse.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	LABEL	257
#define	IDENTIFIER	258
#define	CBLOCK	259
#define	ENDC	260
#define	FILL	261
#define	NUMBER	262
#define	STRING	263
#define	HIGH	264
#define	LOW	265
#define	LSH	266
#define	RSH	267
#define	GREATER_EQUAL	268
#define	LESS_EQUAL	269
#define	EQUAL	270
#define	NOT_EQUAL	271
#define	LOGICAL_AND	272
#define	LOGICAL_OR	273
#define	ASSIGN_PLUS	274
#define	ASSIGN_MINUS	275
#define	ASSIGN_MULTIPLY	276
#define	ASSIGN_DIVIDE	277
#define	ASSIGN_MODULUS	278
#define	ASSIGN_LSH	279
#define	ASSIGN_RSH	280
#define	ASSIGN_AND	281
#define	ASSIGN_OR	282
#define	ASSIGN_XOR	283
#define	INCREMENT	284
#define	DECREMENT	285
#define	TBL_NO_CHANGE	286
#define	TBL_POST_INC	287
#define	TBL_POST_DEC	288
#define	TBL_PRE_INC	289
#define	CONCAT	290
#define	VAR	291
#define	VARLAB_BEGIN	292
#define	VAR_BEGIN	293
#define	VAR_END	294

#line 1 "parse.y"

/* Parser for gpasm
   Copyright (C) 1998,1999,2000,2001 James Bowman, Craig Franklin

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

#include "gpasm.h"

#include "evaluate.h"
#include "gpsymbol.h"
#include "gperror.h"
#include "directive.h"
#include "lst.h"
#include "macro.h"
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
    } else {
      state.mac_body->label = label;
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


  if (state.src->type == macro) {
    if ((state.lst.expand) && (state.lst.enabled) && (state.pass == 2)) {
      assert(state.src->lst.m->src_line != NULL);
      lst_format_line(state.src->lst.m->src_line, value);
    }
    if (state.src->lst.m->next)
      state.src->lst.m = state.src->lst.m->next;
  
  } else if (state.src->lst.f != NULL) {
    fgets(l, BUFSIZ, state.src->lst.f);
    l[strlen(l) - 1] = '\0';	/* Eat the trailing newline */

    if (state.mac_prev) {
      state.lst.line.linetype = none;
      if (state.mac_body)
	state.mac_body->src_line = strdup(l);
    }

    if (state.lst.enabled && (state.pass == 2)) {
      lst_format_line(e, value);
    }
  }

  state.src->line_number++;
  

  switch (state.next_state) {
    case _exitmacro:
      execute_exitm();
      break;

    case _macro:
      /* push the label for local directive */
      state.stTop = push_macro_symbol_table(state.stTop);
      execute_macro(state.next_buffer.macro);
      break;

    case _include:
      open_src(state.next_buffer.file, 1);
      free(state.next_buffer.file);
      break;

    default:
      break;
  }
 
}


/************************************************************************/


#line 199 "parse.y"
typedef union {
  gpasmVal i;
  char *s;
  struct pnode *p;
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		152
#define	YYFLAG		-32768
#define	YYNTBASE	60

#define YYTRANSLATE(x) ((unsigned)(x) <= 294 ? yytranslate[x] : 94)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    54,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    52,     2,     2,    58,    51,    20,     2,    59,
    55,    49,    47,    56,    48,     2,    50,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    57,     2,    18,
    25,    19,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    22,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    21,     2,    53,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    23,    24,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    43,    44,    45,    46
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     2,     6,    10,    15,    20,    24,    28,    31,
    33,    35,    38,    42,    49,    57,    58,    66,    67,    74,
    79,    80,    83,    85,    88,    91,    95,    97,   101,   103,
   107,   109,   111,   113,   115,   117,   119,   121,   123,   125,
   127,   129,   132,   136,   138,   140,   142,   146,   148,   150,
   154,   156,   158,   162,   164,   166,   170,   172,   174,   176,
   178,   182,   184,   186,   188,   190,   192,   194,   196,   200,
   202,   204,   206,   210,   212,   214,   216,   220,   222,   224,
   226,   228,   231,   233,   235,   237,   239,   241,   243,   245,
   247,   249,   253,   255,   257,   259,   261,   263,   267,   271,
   273,   277
};

static const short yyrhs[] = {    -1,
     0,    60,    61,    62,     0,    60,     1,    54,     0,    93,
    70,    72,    54,     0,    93,    25,    72,    54,     0,    93,
    37,    54,     0,    93,    36,    54,     0,    93,    63,     0,
    63,     0,    54,     0,     4,    54,     0,     4,    71,    54,
     0,     7,     4,    55,    56,    72,    54,     0,     7,     4,
    71,    55,    56,    72,    54,     0,     0,     5,    72,    54,
    64,    66,     6,    54,     0,     0,     5,    54,    65,    66,
     6,    54,     0,     5,     1,     6,    54,     0,     0,    66,
    67,     0,    54,     0,    68,    54,     0,    93,    54,     0,
    93,    72,    54,     0,    69,     0,    68,    56,    69,     0,
    92,     0,    92,    57,    72,     0,    26,     0,    27,     0,
    28,     0,    29,     0,    30,     0,    31,     0,    32,     0,
    33,     0,    34,     0,    35,     0,    72,     0,    72,    56,
     0,    72,    56,    71,     0,    73,     0,     9,     0,    75,
     0,    73,    74,    75,     0,    25,     0,    77,     0,    75,
    76,    77,     0,    24,     0,    79,     0,    77,    78,    79,
     0,    23,     0,    81,     0,    79,    80,    81,     0,    20,
     0,    21,     0,    22,     0,    83,     0,    81,    82,    83,
     0,    18,     0,    19,     0,    16,     0,    17,     0,    14,
     0,    15,     0,    85,     0,    83,    84,    85,     0,    12,
     0,    13,     0,    87,     0,    85,    86,    87,     0,    47,
     0,    48,     0,    89,     0,    87,    88,    89,     0,    49,
     0,    50,     0,    51,     0,    91,     0,    90,    91,     0,
    10,     0,    11,     0,    48,     0,    52,     0,    53,     0,
    47,     0,    92,     0,     8,     0,    58,     0,    59,    72,
    55,     0,    49,     0,    39,     0,    40,     0,    41,     0,
     4,     0,    45,    72,    55,     0,    45,    72,    46,     0,
     3,     0,    44,    72,    55,     0,    44,    72,    46,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   236,   238,   245,   245,   251,   263,   272,   281,   290,   348,
   361,   370,   375,   380,   396,   412,   415,   419,   422,   426,
   433,   434,   441,   443,   445,   450,   457,   459,   463,   468,
   475,   475,   475,   475,   476,   476,   476,   476,   477,   477,
   479,   484,   490,   497,   499,   519,   521,   528,   530,   532,
   539,   541,   543,   550,   552,   554,   561,   561,   561,   563,
   565,   572,   572,   572,   572,   572,   572,   574,   576,   583,
   583,   585,   587,   594,   594,   596,   598,   605,   605,   605,
   607,   609,   616,   616,   616,   616,   616,   616,   618,   623,
   628,   633,   638,   643,   648,   653,   660,   665,   670,   678,
   683,   691
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","LABEL",
"IDENTIFIER","CBLOCK","ENDC","FILL","NUMBER","STRING","HIGH","LOW","LSH","RSH",
"GREATER_EQUAL","LESS_EQUAL","EQUAL","NOT_EQUAL","'<'","'>'","'&'","'|'","'^'",
"LOGICAL_AND","LOGICAL_OR","'='","ASSIGN_PLUS","ASSIGN_MINUS","ASSIGN_MULTIPLY",
"ASSIGN_DIVIDE","ASSIGN_MODULUS","ASSIGN_LSH","ASSIGN_RSH","ASSIGN_AND","ASSIGN_OR",
"ASSIGN_XOR","INCREMENT","DECREMENT","TBL_NO_CHANGE","TBL_POST_INC","TBL_POST_DEC",
"TBL_PRE_INC","CONCAT","VAR","VARLAB_BEGIN","VAR_BEGIN","VAR_END","'+'","'-'",
"'*'","'/'","'%'","'!'","'~'","'\\n'","')'","','","':'","'$'","'('","program",
"@1","line","statement","@2","@3","const_block","const_line","const_def_list",
"const_def","assign_equal_ops","parameter_list","expr","e9","e9op","e8","e8op",
"e7","e7op","e6","e6op","e5","e5op","e4","e4op","e3","e3op","e2","e2op","e1",
"e1op","e0","cidentifier","label_concat", NULL
};
#endif

static const short yyr1[] = {     0,
    60,    61,    60,    60,    62,    62,    62,    62,    62,    62,
    63,    63,    63,    63,    63,    64,    63,    65,    63,    63,
    66,    66,    67,    67,    67,    67,    68,    68,    69,    69,
    70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
    71,    71,    71,    72,    72,    73,    73,    74,    75,    75,
    76,    77,    77,    78,    79,    79,    80,    80,    80,    81,
    81,    82,    82,    82,    82,    82,    82,    83,    83,    84,
    84,    85,    85,    86,    86,    87,    87,    88,    88,    88,
    89,    89,    90,    90,    90,    90,    90,    90,    91,    91,
    91,    91,    91,    91,    91,    91,    92,    92,    92,    93,
    93,    93
};

static const short yyr2[] = {     0,
     0,     0,     3,     3,     4,     4,     3,     3,     2,     1,
     1,     2,     3,     6,     7,     0,     7,     0,     6,     4,
     0,     2,     1,     2,     2,     3,     1,     3,     1,     3,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     2,     3,     1,     1,     1,     3,     1,     1,     3,
     1,     1,     3,     1,     1,     3,     1,     1,     1,     1,
     3,     1,     1,     1,     1,     1,     1,     1,     3,     1,
     1,     1,     3,     1,     1,     1,     3,     1,     1,     1,
     1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     3,     1,     1,     1,     1,     1,     3,     3,     1,
     3,     3
};

static const short yydefact[] = {     1,
     0,     0,     0,     4,   100,     0,     0,     0,     0,    11,
     3,    10,     0,    97,    90,    45,    83,    84,    94,    95,
    96,     0,    88,    85,    93,    86,    87,    12,    91,     0,
     0,    41,    44,    46,    49,    52,    55,    60,    68,    72,
    76,     0,    81,    89,     0,    18,     0,     0,     0,     0,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
     0,     0,     9,     0,     0,     0,    13,    42,    48,     0,
    51,     0,    54,     0,    57,    58,    59,     0,    66,    67,
    64,    65,    62,    63,     0,    70,    71,     0,    74,    75,
     0,    78,    79,    80,     0,    82,     0,    21,    16,     0,
     0,   102,   101,     0,     8,     7,     0,    99,    98,    92,
    43,    47,    50,    53,    56,    61,    69,    73,    77,    20,
     0,    21,     0,     0,     6,     5,     0,    23,    22,     0,
    27,    29,     0,     0,     0,     0,    19,    24,     0,     0,
    25,     0,     0,    14,     0,    28,    30,    26,    17,    15,
     0,     0
};

static const short yydefgoto[] = {     1,
     3,    11,    12,   122,    98,   121,   129,   130,   131,    64,
    31,    32,    33,    70,    34,    72,    35,    74,    36,    78,
    37,    85,    38,    88,    39,    91,    40,    95,    41,    42,
    43,    44,   133
};

static const short yypact[] = {-32768,
    96,   -21,   184,-32768,-32768,    21,     0,    30,   156,-32768,
-32768,-32768,   229,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   156,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   156,
    -8,    -6,    26,    31,    44,    43,     3,    15,   -35,    33,
-32768,   186,-32768,-32768,    50,-32768,    17,    83,   -41,   156,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    18,    24,-32768,   156,   -39,    40,-32768,   156,-32768,   171,
-32768,   171,-32768,   171,-32768,-32768,-32768,   171,-32768,-32768,
-32768,-32768,-32768,-32768,   171,-32768,-32768,   171,-32768,-32768,
   171,-32768,-32768,-32768,   171,-32768,    27,-32768,-32768,    29,
    47,-32768,-32768,    35,-32768,-32768,    51,-32768,-32768,-32768,
-32768,    31,    44,    43,     3,    15,   -35,    33,-32768,-32768,
    32,-32768,   156,    42,-32768,-32768,    52,-32768,-32768,    34,
-32768,    53,   104,   196,    55,   156,-32768,-32768,    -1,   156,
-32768,    57,    63,-32768,    64,-32768,-32768,-32768,-32768,-32768,
   107,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    91,-32768,-32768,    -3,-32768,-32768,   -19,-32768,
   -42,    -7,-32768,-32768,    67,-32768,    49,-32768,    60,-32768,
    61,-32768,    62,-32768,    37,-32768,    36,-32768,    59,-32768,
   106,   -97,   143
};


#define	YYLAST		283


static const short yytable[] = {    47,
    45,    49,    14,    14,   102,   101,   108,    15,    16,    17,
    18,    89,    90,   103,    65,   109,    79,    80,    81,    82,
    83,    84,    66,   132,    14,   111,    86,    87,    15,    16,
    17,    18,     4,    48,     5,    14,   132,   127,    19,    20,
    21,   132,   104,    22,    22,    67,    23,    24,    25,    68,
    69,    26,    27,    46,    71,    97,   107,    29,    30,    19,
    20,    21,    75,    76,    77,    22,    73,    23,    24,    25,
    99,   105,    26,    27,    28,     9,    22,   106,    29,    30,
   120,    92,    93,    94,   123,   128,    14,   138,   125,   139,
    15,    16,    17,    18,   110,   151,     2,   136,    -2,    -2,
    -2,   124,    -2,    63,   126,   137,   152,    14,   144,   140,
   148,    15,    16,    17,    18,   135,   149,   150,   134,   146,
   113,    19,    20,    21,   117,   142,   118,    22,   145,    23,
    24,    25,   147,   114,    26,    27,   112,   100,   115,    -2,
    29,    30,    19,    20,    21,    13,   116,    96,    22,    -2,
    23,    24,    25,   119,     0,    26,    27,   141,     0,    14,
     0,    29,    30,    15,    16,    17,    18,     0,     0,     0,
     0,     0,     0,     0,    14,     0,     0,     0,    15,     0,
    17,    18,     0,     0,     0,     0,     5,     6,     7,    14,
     8,     0,     0,    15,    19,    20,    21,     0,     5,    14,
    22,   143,    23,    24,    25,     0,     0,    26,    27,    19,
    20,    21,     0,    29,    30,    22,     0,    23,    24,    25,
     0,     0,    26,    27,    19,    20,    21,     9,    29,    30,
    22,     0,     6,     7,    25,     8,     0,    10,     0,     9,
    22,     0,     0,    29,    30,     0,     0,     0,     0,   128,
     0,     0,     0,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    10
};

static const short yycheck[] = {     7,
     1,     9,     4,     4,    46,    48,    46,     8,     9,    10,
    11,    47,    48,    55,    22,    55,    14,    15,    16,    17,
    18,    19,    30,   121,     4,    68,    12,    13,     8,     9,
    10,    11,    54,     4,     3,     4,   134,     6,    39,    40,
    41,   139,    50,    45,    45,    54,    47,    48,    49,    56,
    25,    52,    53,    54,    24,     6,    64,    58,    59,    39,
    40,    41,    20,    21,    22,    45,    23,    47,    48,    49,
    54,    54,    52,    53,    54,    44,    45,    54,    58,    59,
    54,    49,    50,    51,    56,    54,     4,    54,    54,    56,
     8,     9,    10,    11,    55,     0,     1,    56,     3,     4,
     5,    55,     7,    13,    54,    54,     0,     4,    54,    57,
    54,     8,     9,    10,    11,   123,    54,    54,   122,   139,
    72,    39,    40,    41,    88,   133,    91,    45,   136,    47,
    48,    49,   140,    74,    52,    53,    70,    55,    78,    44,
    58,    59,    39,    40,    41,     3,    85,    42,    45,    54,
    47,    48,    49,    95,    -1,    52,    53,    54,    -1,     4,
    -1,    58,    59,     8,     9,    10,    11,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,     4,    -1,    -1,    -1,     8,    -1,
    10,    11,    -1,    -1,    -1,    -1,     3,     4,     5,     4,
     7,    -1,    -1,     8,    39,    40,    41,    -1,     3,     4,
    45,     6,    47,    48,    49,    -1,    -1,    52,    53,    39,
    40,    41,    -1,    58,    59,    45,    -1,    47,    48,    49,
    -1,    -1,    52,    53,    39,    40,    41,    44,    58,    59,
    45,    -1,     4,     5,    49,     7,    -1,    54,    -1,    44,
    45,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    54,
    -1,    -1,    -1,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    54
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/lib/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
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
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
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

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 2:
#line 240 "parse.y"
{ 
          state.lst.line.was_org = state.org; 
          state.lst.line.linetype = none; 
          state.next_state = _nochange;
        ;
    break;}
case 4:
#line 246 "parse.y"
{ 
	  next_line(0);
	;
    break;}
case 5:
#line 253 "parse.y"
{
	  struct pnode *parms;
	  int exp_result;

          exp_result = do_insn("set", mk_list(yyvsp[-1].p, NULL));          
          parms = mk_list(mk_2op(return_op(yyvsp[-2].i), 
                                 mk_symbol(yyvsp[-3].s), 
                                 mk_constant(exp_result)), NULL);
	  next_line(set_label(yyvsp[-3].s, parms));
	;
    break;}
case 6:
#line 265 "parse.y"
{
	  struct pnode *parms;

	  /* implements i = 6 + 1 */           
	  parms = mk_list(yyvsp[-1].p, NULL);
	  next_line(set_label(yyvsp[-3].s, parms));
	;
    break;}
case 7:
#line 274 "parse.y"
{
	  struct pnode *parms;

	  /* implements i-- */           
	  parms = mk_list(mk_1op(DECREMENT, mk_symbol(yyvsp[-2].s)), NULL);
	  next_line(set_label(yyvsp[-2].s, parms));
	;
    break;}
case 8:
#line 283 "parse.y"
{
          struct pnode *parms;

	  /* implements i++ */          
	  parms = mk_list(mk_1op(INCREMENT, mk_symbol(yyvsp[-2].s)), NULL);
	  next_line(set_label(yyvsp[-2].s, parms));
	;
    break;}
case 9:
#line 292 "parse.y"
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
	      }
	      h->pass = state.pass;

	      state.mac_head = NULL;
	    } else if (!state.mac_prev) {
	      /* Outside a macro, just define the label. */
	      switch (state.lst.line.linetype) {
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
	      case dir:
                gperror(GPE_ILLEGAL_LABEL, NULL);
                break;
              default:
		break;
	      }
	    } else {
	      /* Inside a macro; add the label to the current line */
	      if (state.mac_body)
		state.mac_body->label = yyvsp[-1].s;
	    }
	  }
	  next_line(yyvsp[0].i);
	;
    break;}
case 10:
#line 350 "parse.y"
{
	  if (state.mac_head) {
	    /* This is a macro definition, but the label was missing */
	    state.mac_head = NULL;
	    gperror(GPE_NO_MACRO_NAME, NULL);
	  } else {
	    next_line(0);
	  }
	;
    break;}
case 11:
#line 363 "parse.y"
{
	  if (!state.mac_prev) {
	    yyval.i = state.org;
	  } else {
	    macro_append();
	  }
	;
    break;}
case 12:
#line 372 "parse.y"
{
	  yyval.i = do_or_append_insn(yyvsp[-1].s, NULL);
	;
    break;}
case 13:
#line 377 "parse.y"
{
	  yyval.i = do_or_append_insn(yyvsp[-2].s, yyvsp[-1].p);
	;
    break;}
case 14:
#line 382 "parse.y"
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
	;
    break;}
case 15:
#line 398 "parse.y"
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
	;
    break;}
case 16:
#line 413 "parse.y"
{ begin_cblock(yyvsp[-1].p); next_line(0); ;
    break;}
case 17:
#line 416 "parse.y"
{
	  yyval.i = 0;
	;
    break;}
case 18:
#line 420 "parse.y"
{ next_line(0); ;
    break;}
case 19:
#line 423 "parse.y"
{
           yyval.i = 0;
        ;
    break;}
case 20:
#line 428 "parse.y"
{
	  yyval.i = 0;
  	;
    break;}
case 22:
#line 436 "parse.y"
{
	  next_line(0);
	;
    break;}
case 25:
#line 447 "parse.y"
{
	  cblock_expr(mk_symbol(yyvsp[-1].s));
	;
    break;}
case 26:
#line 452 "parse.y"
{
	  cblock_expr_incr(mk_symbol(yyvsp[-2].s), yyvsp[-1].p);
	;
    break;}
case 29:
#line 465 "parse.y"
{
	  cblock_expr(yyvsp[0].p);
	;
    break;}
case 30:
#line 470 "parse.y"
{
	  cblock_expr_incr(yyvsp[-2].p, yyvsp[0].p);
	;
    break;}
case 41:
#line 481 "parse.y"
{
	  yyval.p = mk_list(yyvsp[0].p, NULL);
	;
    break;}
case 42:
#line 486 "parse.y"
{
	  gperror(GPE_BADCHAR, "Illegal Character (,)");
          yyval.p = mk_list(yyvsp[-1].p, NULL);
	;
    break;}
case 43:
#line 492 "parse.y"
{
	  yyval.p = mk_list(yyvsp[-2].p, yyvsp[0].p);
	;
    break;}
case 45:
#line 501 "parse.y"
{
	  /* Cook up the string, replacing \X with its code. */
	  char *ps, *pd;

	  ps = pd = yyvsp[0].s;
	  while (*ps) {
	    if (*ps != '\\') {
	      *pd++ = *ps++;
	    } else {
	      *pd++ = gpasm_magic(ps);
	      ps += 2;
	    }
	  }
	  *pd = 0;
	  yyval.p = mk_string(yyvsp[0].s);
        ;
    break;}
case 47:
#line 523 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	;
    break;}
case 50:
#line 534 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	;
    break;}
case 53:
#line 545 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	;
    break;}
case 56:
#line 556 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	;
    break;}
case 61:
#line 567 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	;
    break;}
case 69:
#line 578 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	;
    break;}
case 73:
#line 589 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	;
    break;}
case 77:
#line 600 "parse.y"
{
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	;
    break;}
case 82:
#line 611 "parse.y"
{
	  yyval.p = mk_1op(yyvsp[-1].i, yyvsp[0].p);
	;
    break;}
case 89:
#line 620 "parse.y"
{
	  yyval.p = yyvsp[0].p;
        ;
    break;}
case 90:
#line 625 "parse.y"
{
	  yyval.p = mk_constant(yyvsp[0].i);
	;
    break;}
case 91:
#line 630 "parse.y"
{
	  yyval.p = mk_symbol("$");
	;
    break;}
case 92:
#line 635 "parse.y"
{
	  yyval.p = yyvsp[-1].p;
	;
    break;}
case 93:
#line 640 "parse.y"
{
	  yyval.p = mk_constant(TBL_NO_CHANGE);
	;
    break;}
case 94:
#line 645 "parse.y"
{
	  yyval.p = mk_constant(yyvsp[0].i);
	;
    break;}
case 95:
#line 650 "parse.y"
{
	  yyval.p = mk_constant(yyvsp[0].i);
	;
    break;}
case 96:
#line 655 "parse.y"
{
	  yyval.p = mk_constant(yyvsp[0].i);
	;
    break;}
case 97:
#line 662 "parse.y"
{ 
          yyval.p = mk_symbol(yyvsp[0].s);
        ;
    break;}
case 98:
#line 667 "parse.y"
{
          yyval.p = mk_2op(CONCAT, mk_symbol(yyvsp[-2].s), mk_1op(VAR, yyvsp[-1].p));
        ;
    break;}
case 99:
#line 672 "parse.y"
{
          yyval.p = mk_2op(CONCAT, mk_symbol(yyvsp[-2].s), 
                        mk_2op(CONCAT, mk_1op(VAR, yyvsp[-1].p), mk_symbol(yyvsp[0].s)));
        ;
    break;}
case 100:
#line 680 "parse.y"
{ 
          yyval.s = yyvsp[0].s;
        ;
    break;}
case 101:
#line 685 "parse.y"
{
          if (!state.mac_prev) {
	    yyval.s = evaluate_concatenation(mk_2op(CONCAT,  mk_symbol(yyvsp[-2].s), 
                           mk_1op(VAR, yyvsp[-1].p)));
	  }
        ;
    break;}
case 102:
#line 693 "parse.y"
{
          if (!state.mac_prev) {
            yyval.s = evaluate_concatenation(mk_2op(CONCAT,  mk_symbol(yyvsp[-2].s), 
                      mk_2op(CONCAT, mk_1op(VAR, yyvsp[-1].p), mk_symbol(yyvsp[0].s))));
	  }
        ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

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

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 701 "parse.y"


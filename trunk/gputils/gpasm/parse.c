/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LABEL = 258,
     IDENTIFIER = 259,
     CBLOCK = 260,
     ENDC = 261,
     FILL = 262,
     NUMBER = 263,
     STRING = 264,
     INCLUDE = 265,
     UPPER = 266,
     HIGH = 267,
     LOW = 268,
     LSH = 269,
     RSH = 270,
     GREATER_EQUAL = 271,
     LESS_EQUAL = 272,
     EQUAL = 273,
     NOT_EQUAL = 274,
     LOGICAL_AND = 275,
     LOGICAL_OR = 276,
     ASSIGN_PLUS = 277,
     ASSIGN_MINUS = 278,
     ASSIGN_MULTIPLY = 279,
     ASSIGN_DIVIDE = 280,
     ASSIGN_MODULUS = 281,
     ASSIGN_LSH = 282,
     ASSIGN_RSH = 283,
     ASSIGN_AND = 284,
     ASSIGN_OR = 285,
     ASSIGN_XOR = 286,
     INCREMENT = 287,
     DECREMENT = 288,
     TBL_NO_CHANGE = 289,
     TBL_POST_INC = 290,
     TBL_POST_DEC = 291,
     TBL_PRE_INC = 292,
     CONCAT = 293,
     VAR = 294,
     DEBUG_FILE = 295,
     DEBUG_LINE = 296,
     VARLAB_BEGIN = 297,
     VAR_BEGIN = 298,
     VAR_END = 299
   };
#endif
#define LABEL 258
#define IDENTIFIER 259
#define CBLOCK 260
#define ENDC 261
#define FILL 262
#define NUMBER 263
#define STRING 264
#define INCLUDE 265
#define UPPER 266
#define HIGH 267
#define LOW 268
#define LSH 269
#define RSH 270
#define GREATER_EQUAL 271
#define LESS_EQUAL 272
#define EQUAL 273
#define NOT_EQUAL 274
#define LOGICAL_AND 275
#define LOGICAL_OR 276
#define ASSIGN_PLUS 277
#define ASSIGN_MINUS 278
#define ASSIGN_MULTIPLY 279
#define ASSIGN_DIVIDE 280
#define ASSIGN_MODULUS 281
#define ASSIGN_LSH 282
#define ASSIGN_RSH 283
#define ASSIGN_AND 284
#define ASSIGN_OR 285
#define ASSIGN_XOR 286
#define INCREMENT 287
#define DECREMENT 288
#define TBL_NO_CHANGE 289
#define TBL_POST_INC 290
#define TBL_POST_DEC 291
#define TBL_PRE_INC 292
#define CONCAT 293
#define VAR 294
#define DEBUG_FILE 295
#define DEBUG_LINE 296
#define VARLAB_BEGIN 297
#define VAR_BEGIN 298
#define VAR_END 299




/* Copy the first part of user declarations.  */
#line 1 "parse.y"

/* Parser for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004 
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



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 218 "parse.y"
typedef union YYSTYPE {
  gpasmVal i;
  char *s;
  struct pnode *p;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 385 "parse.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 397 "parse.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
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
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   310

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  64
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  35
/* YYNRULES -- Number of rules. */
#define YYNRULES  107
/* YYNRULES -- Number of states. */
#define YYNSTATES  161

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   299

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      58,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    56,     2,     2,    62,    55,    22,     2,
      63,    59,    53,    51,    60,    52,     2,    54,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    61,     2,
      20,    27,    21,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    24,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    23,     2,    57,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    25,    26,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     5,     9,    13,    17,    21,    26,
      31,    35,    39,    42,    44,    46,    49,    52,    56,    63,
      71,    72,    80,    81,    88,    93,    94,    97,    99,   102,
     105,   109,   111,   115,   117,   121,   123,   125,   127,   129,
     131,   133,   135,   137,   139,   141,   143,   146,   150,   152,
     154,   156,   160,   162,   164,   168,   170,   172,   176,   178,
     180,   184,   186,   188,   190,   192,   196,   198,   200,   202,
     204,   206,   208,   210,   214,   216,   218,   220,   224,   226,
     228,   230,   234,   236,   238,   240,   242,   245,   247,   249,
     251,   253,   255,   257,   259,   261,   263,   265,   269,   271,
     273,   275,   277,   279,   283,   287,   289,   293
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      65,     0,    -1,    -1,    -1,    65,    66,    67,    -1,    65,
       1,    58,    -1,    46,     9,    58,    -1,    47,     8,    58,
      -1,    98,    75,    77,    58,    -1,    98,    27,    77,    58,
      -1,    98,    39,    58,    -1,    98,    38,    58,    -1,    98,
      68,    -1,    68,    -1,    58,    -1,    10,    58,    -1,     4,
      58,    -1,     4,    76,    58,    -1,     7,     4,    59,    60,
      77,    58,    -1,     7,     4,    76,    59,    60,    77,    58,
      -1,    -1,     5,    77,    58,    69,    71,     6,    58,    -1,
      -1,     5,    58,    70,    71,     6,    58,    -1,     5,     1,
       6,    58,    -1,    -1,    71,    72,    -1,    58,    -1,    73,
      58,    -1,    98,    58,    -1,    98,    77,    58,    -1,    74,
      -1,    73,    60,    74,    -1,    97,    -1,    97,    61,    77,
      -1,    28,    -1,    29,    -1,    30,    -1,    31,    -1,    32,
      -1,    33,    -1,    34,    -1,    35,    -1,    36,    -1,    37,
      -1,    77,    -1,    77,    60,    -1,    77,    60,    76,    -1,
      78,    -1,     9,    -1,    80,    -1,    78,    79,    80,    -1,
      27,    -1,    82,    -1,    80,    81,    82,    -1,    26,    -1,
      84,    -1,    82,    83,    84,    -1,    25,    -1,    86,    -1,
      84,    85,    86,    -1,    22,    -1,    23,    -1,    24,    -1,
      88,    -1,    86,    87,    88,    -1,    20,    -1,    21,    -1,
      18,    -1,    19,    -1,    16,    -1,    17,    -1,    90,    -1,
      88,    89,    90,    -1,    14,    -1,    15,    -1,    92,    -1,
      90,    91,    92,    -1,    51,    -1,    52,    -1,    94,    -1,
      92,    93,    94,    -1,    53,    -1,    54,    -1,    55,    -1,
      96,    -1,    95,    96,    -1,    11,    -1,    12,    -1,    13,
      -1,    52,    -1,    56,    -1,    57,    -1,    51,    -1,    97,
      -1,     8,    -1,    62,    -1,    63,    77,    59,    -1,    53,
      -1,    41,    -1,    42,    -1,    43,    -1,     4,    -1,    49,
      77,    59,    -1,    49,    77,    50,    -1,     3,    -1,    48,
      77,    59,    -1,    48,    77,    50,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   310,   310,   314,   313,   319,   326,   334,   342,   354,
     363,   372,   381,   446,   459,   468,   473,   478,   483,   499,
     516,   515,   534,   533,   549,   555,   557,   564,   566,   573,
     582,   593,   595,   599,   606,   614,   614,   614,   615,   615,
     615,   615,   616,   616,   616,   619,   624,   630,   637,   639,
     646,   648,   654,   657,   659,   665,   668,   670,   676,   679,
     681,   687,   687,   687,   690,   692,   698,   698,   698,   698,
     698,   698,   701,   703,   709,   709,   712,   714,   720,   720,
     723,   725,   731,   731,   731,   734,   736,   742,   742,   742,
     742,   742,   742,   742,   745,   750,   755,   760,   765,   770,
     775,   780,   787,   792,   797,   805,   810,   818
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LABEL", "IDENTIFIER", "CBLOCK", "ENDC",
  "FILL", "NUMBER", "STRING", "INCLUDE", "UPPER", "HIGH", "LOW", "LSH",
  "RSH", "GREATER_EQUAL", "LESS_EQUAL", "EQUAL", "NOT_EQUAL", "'<'", "'>'",
  "'&'", "'|'", "'^'", "LOGICAL_AND", "LOGICAL_OR", "'='", "ASSIGN_PLUS",
  "ASSIGN_MINUS", "ASSIGN_MULTIPLY", "ASSIGN_DIVIDE", "ASSIGN_MODULUS",
  "ASSIGN_LSH", "ASSIGN_RSH", "ASSIGN_AND", "ASSIGN_OR", "ASSIGN_XOR",
  "INCREMENT", "DECREMENT", "TBL_NO_CHANGE", "TBL_POST_INC",
  "TBL_POST_DEC", "TBL_PRE_INC", "CONCAT", "VAR", "DEBUG_FILE",
  "DEBUG_LINE", "VARLAB_BEGIN", "VAR_BEGIN", "VAR_END", "'+'", "'-'",
  "'*'", "'/'", "'%'", "'!'", "'~'", "'\\n'", "')'", "','", "':'", "'$'",
  "'('", "$accept", "program", "@1", "line", "statement", "@2", "@3",
  "const_block", "const_line", "const_def_list", "const_def",
  "assign_equal_ops", "parameter_list", "expr", "e9", "e9op", "e8", "e8op",
  "e7", "e7op", "e6", "e6op", "e5", "e5op", "e4", "e4op", "e3", "e3op",
  "e2", "e2op", "e1", "e1op", "e0", "cidentifier", "label_concat", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      60,    62,    38,   124,    94,   275,   276,    61,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     299,    43,    45,    42,    47,    37,    33,   126,    10,    41,
      44,    58,    36,    40
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    64,    65,    66,    65,    65,    67,    67,    67,    67,
      67,    67,    67,    67,    68,    68,    68,    68,    68,    68,
      69,    68,    70,    68,    68,    71,    71,    72,    72,    72,
      72,    73,    73,    74,    74,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    76,    76,    76,    77,    77,
      78,    78,    79,    80,    80,    81,    82,    82,    83,    84,
      84,    85,    85,    85,    86,    86,    87,    87,    87,    87,
      87,    87,    88,    88,    89,    89,    90,    90,    91,    91,
      92,    92,    93,    93,    93,    94,    94,    95,    95,    95,
      95,    95,    95,    95,    96,    96,    96,    96,    96,    96,
      96,    96,    97,    97,    97,    98,    98,    98
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     0,     3,     3,     3,     3,     4,     4,
       3,     3,     2,     1,     1,     2,     2,     3,     6,     7,
       0,     7,     0,     6,     4,     0,     2,     1,     2,     2,
       3,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     3,     1,     1,
       1,     3,     1,     1,     3,     1,     1,     3,     1,     1,
       3,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     3,     1,     1,
       1,     3,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     1,
       1,     1,     1,     3,     3,     1,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,     5,   105,     0,     0,     0,
       0,     0,     0,     0,    14,     4,    13,     0,   102,    95,
      49,    87,    88,    89,    99,   100,   101,     0,    93,    90,
      98,    91,    92,    16,    96,     0,     0,    45,    48,    50,
      53,    56,    59,    64,    72,    76,    80,     0,    85,    94,
       0,    22,     0,     0,    15,     0,     0,     0,     0,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,     0,
       0,    12,     0,     0,     0,    17,    46,    52,     0,    55,
       0,    58,     0,    61,    62,    63,     0,    70,    71,    68,
      69,    66,    67,     0,    74,    75,     0,    78,    79,     0,
      82,    83,    84,     0,    86,     0,    25,    20,     0,     0,
       6,     7,   107,   106,     0,    11,    10,     0,   104,   103,
      97,    47,    51,    54,    57,    60,    65,    73,    77,    81,
      24,     0,    25,     0,     0,     9,     8,     0,    27,    26,
       0,    31,    33,     0,     0,     0,     0,    23,    28,     0,
       0,    29,     0,     0,    18,     0,    32,    34,    30,    21,
      19
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     4,    15,    16,   132,   106,   131,   139,   140,
     141,    72,    36,    37,    38,    78,    39,    80,    40,    82,
      41,    86,    42,    93,    43,    96,    44,    99,    45,   103,
      46,    47,    48,    49,   143
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -111
static const short yypact[] =
{
    -111,   248,  -111,   -51,    13,  -111,  -111,    24,     0,     6,
     -10,     5,    46,   167,  -111,  -111,  -111,   252,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,   167,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,   167,    -3,     9,    43,    48,
      53,    22,   165,    10,   -22,    30,  -111,    64,  -111,  -111,
      73,  -111,    32,    88,  -111,    35,    36,   -44,   167,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,    37,
      40,  -111,   167,   -28,    44,  -111,   167,  -111,   184,  -111,
     184,  -111,   184,  -111,  -111,  -111,   184,  -111,  -111,  -111,
    -111,  -111,  -111,   184,  -111,  -111,   184,  -111,  -111,   184,
    -111,  -111,  -111,   184,  -111,    50,  -111,  -111,    42,    45,
    -111,  -111,  -111,  -111,    51,  -111,  -111,    52,  -111,  -111,
    -111,  -111,    48,    53,    22,   165,    10,   -22,    30,  -111,
    -111,    85,  -111,   167,    56,  -111,  -111,    60,  -111,  -111,
     -20,  -111,    67,   111,   108,    63,   167,  -111,  -111,    -2,
     167,  -111,    74,    78,  -111,    90,  -111,  -111,  -111,  -111,
    -111
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -111,  -111,  -111,  -111,   129,  -111,  -111,    17,  -111,  -111,
      12,  -111,   -50,    -8,  -111,  -111,    77,  -111,    79,  -111,
      76,  -111,    84,  -111,    72,  -111,    81,  -111,    91,  -111,
      69,  -111,   140,  -110,   185
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -4
static const short yytable[] =
{
      52,    50,    18,   109,    18,    57,   112,     5,    19,    20,
      53,    21,    22,    23,    55,   113,     6,     7,     8,    73,
       9,   142,   118,    10,    94,    95,   121,    74,    18,    97,
      98,   119,    19,    20,   142,    21,    22,    23,   148,   142,
     149,    24,    25,    26,    83,    84,    85,    27,    54,    27,
     114,    28,    29,    30,    56,    75,    31,    32,    51,    11,
      12,    13,    34,    35,   117,    24,    25,    26,    18,    76,
      77,    14,    19,    27,    79,    28,    29,    30,    81,   105,
      31,    32,    33,   100,   101,   102,    34,    35,     6,    18,
     107,   137,    18,   110,   111,   115,    19,    20,   116,    21,
      22,    23,   133,   120,   134,    24,    25,    26,   130,   135,
     136,     6,    18,    27,   153,    18,   146,    30,   147,    19,
      20,   154,    21,    22,    23,   145,    34,    35,   150,    24,
      25,    26,   158,    13,    27,   152,   159,    27,   155,    28,
      29,    30,   157,   138,    31,    32,    71,   108,   160,   144,
      34,    35,    24,    25,    26,   122,    13,    27,   124,   123,
      27,   156,    28,    29,    30,   126,   138,    31,    32,   151,
     125,    18,   129,    34,    35,    19,    20,   127,    21,    22,
      23,    87,    88,    89,    90,    91,    92,   104,    18,    17,
     128,     0,    19,     0,     0,    21,    22,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    24,    25,
      26,     0,     0,     0,     0,     0,    27,     0,    28,    29,
      30,     0,     0,    31,    32,    24,    25,    26,     0,    34,
      35,     0,     0,    27,     0,    28,    29,    30,     0,     0,
      31,    32,     0,     0,     0,     0,    34,    35,     2,     3,
       0,    -3,    -3,    -3,     0,    -3,     7,     8,    -3,     9,
       0,     0,    10,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,     0,     0,    -3,    -3,    -3,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    -3,     0,     0,     0,
      14
};

static const short yycheck[] =
{
       8,     1,     4,    53,     4,    13,    50,    58,     8,     9,
       4,    11,    12,    13,     9,    59,     3,     4,     5,    27,
       7,   131,    50,    10,    14,    15,    76,    35,     4,    51,
      52,    59,     8,     9,   144,    11,    12,    13,    58,   149,
      60,    41,    42,    43,    22,    23,    24,    49,    58,    49,
      58,    51,    52,    53,     8,    58,    56,    57,    58,    46,
      47,    48,    62,    63,    72,    41,    42,    43,     4,    60,
      27,    58,     8,    49,    26,    51,    52,    53,    25,     6,
      56,    57,    58,    53,    54,    55,    62,    63,     3,     4,
      58,     6,     4,    58,    58,    58,     8,     9,    58,    11,
      12,    13,    60,    59,    59,    41,    42,    43,    58,    58,
      58,     3,     4,    49,     6,     4,    60,    53,    58,     8,
       9,    58,    11,    12,    13,   133,    62,    63,    61,    41,
      42,    43,    58,    48,    49,   143,    58,    49,   146,    51,
      52,    53,   150,    58,    56,    57,    17,    59,    58,   132,
      62,    63,    41,    42,    43,    78,    48,    49,    82,    80,
      49,   149,    51,    52,    53,    93,    58,    56,    57,    58,
      86,     4,   103,    62,    63,     8,     9,    96,    11,    12,
      13,    16,    17,    18,    19,    20,    21,    47,     4,     4,
      99,    -1,     8,    -1,    -1,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,
      43,    -1,    -1,    -1,    -1,    -1,    49,    -1,    51,    52,
      53,    -1,    -1,    56,    57,    41,    42,    43,    -1,    62,
      63,    -1,    -1,    49,    -1,    51,    52,    53,    -1,    -1,
      56,    57,    -1,    -1,    -1,    -1,    62,    63,     0,     1,
      -1,     3,     4,     5,    -1,     7,     4,     5,    10,     7,
      -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    -1,    -1,    46,    47,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    58,    -1,    -1,    -1,
      58
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    65,     0,     1,    66,    58,     3,     4,     5,     7,
      10,    46,    47,    48,    58,    67,    68,    98,     4,     8,
       9,    11,    12,    13,    41,    42,    43,    49,    51,    52,
      53,    56,    57,    58,    62,    63,    76,    77,    78,    80,
      82,    84,    86,    88,    90,    92,    94,    95,    96,    97,
       1,    58,    77,     4,    58,     9,     8,    77,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    68,    75,    77,    77,    58,    60,    27,    79,    26,
      81,    25,    83,    22,    23,    24,    85,    16,    17,    18,
      19,    20,    21,    87,    14,    15,    89,    51,    52,    91,
      53,    54,    55,    93,    96,     6,    70,    58,    59,    76,
      58,    58,    50,    59,    77,    58,    58,    77,    50,    59,
      59,    76,    80,    82,    84,    86,    88,    90,    92,    94,
      58,    71,    69,    60,    59,    58,    58,     6,    58,    72,
      73,    74,    97,    98,    71,    77,    60,    58,    58,    60,
      61,    58,    77,     6,    58,    77,    74,    77,    58,    58,
      58
};

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
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


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
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

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

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
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

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

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

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
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

  if (yyss + yystacksize - 1 <= yyssp)
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
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


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

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 314 "parse.y"
    { 
          state.lst.line.was_org = state.org; 
          state.lst.line.linetype = none; 
          state.next_state = state_nochange;
        }
    break;

  case 5:
#line 320 "parse.y"
    { 
	  next_line(0);
	}
    break;

  case 6:
#line 327 "parse.y"
    {
          if ((state.mode == relocatable) && (state.debug_info)) {
	    state.obj.debug_file = coff_add_filesym(yyvsp[-1].s, 0);
          }
	  next_line(0);
	}
    break;

  case 7:
#line 335 "parse.y"
    {
	  if ((state.mode == relocatable) && (state.debug_info)) {
	    state.obj.debug_line = yyvsp[-1].i;
          }
          next_line(0);
	}
    break;

  case 8:
#line 343 "parse.y"
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

  case 9:
#line 355 "parse.y"
    {
	  struct pnode *parms;

	  /* implements i = 6 + 1 */           
	  parms = mk_list(yyvsp[-1].p, NULL);
	  next_line(set_label(yyvsp[-3].s, parms));
	}
    break;

  case 10:
#line 364 "parse.y"
    {
	  struct pnode *parms;

	  /* implements i-- */           
	  parms = mk_list(mk_1op(DECREMENT, mk_symbol(yyvsp[-2].s)), NULL);
	  next_line(set_label(yyvsp[-2].s, parms));
	}
    break;

  case 11:
#line 373 "parse.y"
    {
          struct pnode *parms;

	  /* implements i++ */          
	  parms = mk_list(mk_1op(INCREMENT, mk_symbol(yyvsp[-2].s)), NULL);
	  next_line(set_label(yyvsp[-2].s, parms));
	}
    break;

  case 12:
#line 382 "parse.y"
    {
	  if (asm_enabled() && (state.lst.line.linetype == none))
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

  case 13:
#line 447 "parse.y"
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

  case 14:
#line 460 "parse.y"
    {
	  if (!state.mac_prev) {
	    yyval.i = state.org;
	  } else {
	    macro_append();
	  }
	}
    break;

  case 15:
#line 469 "parse.y"
    {
	  yyval.i = do_or_append_insn("include", mk_list(mk_string(yyvsp[-1].s), NULL));
	}
    break;

  case 16:
#line 474 "parse.y"
    {
	  yyval.i = do_or_append_insn(yyvsp[-1].s, NULL);
	}
    break;

  case 17:
#line 479 "parse.y"
    {
	  yyval.i = do_or_append_insn(yyvsp[-2].s, yyvsp[-1].p);
	}
    break;

  case 18:
#line 484 "parse.y"
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

  case 19:
#line 500 "parse.y"
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

  case 20:
#line 516 "parse.y"
    {
	  if (!state.mac_prev) {
	    begin_cblock(yyvsp[-1].p);
	  } else {
	    macro_append();
	  }
	  next_line(0);
	}
    break;

  case 21:
#line 526 "parse.y"
    {
	  if (state.mac_prev) {
	    macro_append();
	  }
	  yyval.i = 0;
	}
    break;

  case 22:
#line 534 "parse.y"
    {
	  if (state.mac_prev) {
	    macro_append();
	  }
	  next_line(0);
	}
    break;

  case 23:
#line 542 "parse.y"
    {
	  if (state.mac_prev) {
	    macro_append();
	  }
          yyval.i = 0;
	}
    break;

  case 24:
#line 550 "parse.y"
    {
	  yyval.i = 0;
  	}
    break;

  case 26:
#line 558 "parse.y"
    {
	  next_line(0);
	}
    break;

  case 28:
#line 567 "parse.y"
    {
	  if (state.mac_prev) {
	    macro_append();
	  }
	}
    break;

  case 29:
#line 574 "parse.y"
    {
	  if (!state.mac_prev) {
	    cblock_expr(mk_symbol(yyvsp[-1].s));
	  } else {
	    macro_append();
	  }
	}
    break;

  case 30:
#line 583 "parse.y"
    {
	  if (!state.mac_prev) {
	    cblock_expr_incr(mk_symbol(yyvsp[-2].s), yyvsp[-1].p);
	  } else {
	    macro_append();
	  }
	}
    break;

  case 33:
#line 600 "parse.y"
    {
	  if (!state.mac_prev) {
	    cblock_expr(yyvsp[0].p);
	  }
	}
    break;

  case 34:
#line 607 "parse.y"
    {
	  if (!state.mac_prev) {
	    cblock_expr_incr(yyvsp[-2].p, yyvsp[0].p);
	  }
	}
    break;

  case 45:
#line 620 "parse.y"
    {
	  yyval.p = mk_list(yyvsp[0].p, NULL);
	}
    break;

  case 46:
#line 625 "parse.y"
    {
	  gperror(GPE_BADCHAR, "Illegal Character (,)");
          yyval.p = mk_list(yyvsp[-1].p, NULL);
	}
    break;

  case 47:
#line 631 "parse.y"
    {
	  yyval.p = mk_list(yyvsp[-2].p, yyvsp[0].p);
	}
    break;

  case 49:
#line 640 "parse.y"
    {
	  yyval.p = mk_string(yyvsp[0].s);
        }
    break;

  case 51:
#line 649 "parse.y"
    {
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;

  case 54:
#line 660 "parse.y"
    {
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;

  case 57:
#line 671 "parse.y"
    {
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;

  case 60:
#line 682 "parse.y"
    {
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;

  case 65:
#line 693 "parse.y"
    {
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;

  case 73:
#line 704 "parse.y"
    {
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;

  case 77:
#line 715 "parse.y"
    {
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;

  case 81:
#line 726 "parse.y"
    {
	  yyval.p = mk_2op(yyvsp[-1].i, yyvsp[-2].p, yyvsp[0].p);
	}
    break;

  case 86:
#line 737 "parse.y"
    {
	  yyval.p = mk_1op(yyvsp[-1].i, yyvsp[0].p);
	}
    break;

  case 94:
#line 746 "parse.y"
    {
	  yyval.p = yyvsp[0].p;
        }
    break;

  case 95:
#line 751 "parse.y"
    {
	  yyval.p = mk_constant(yyvsp[0].i);
	}
    break;

  case 96:
#line 756 "parse.y"
    {
	  yyval.p = mk_symbol("$");
	}
    break;

  case 97:
#line 761 "parse.y"
    {
	  yyval.p = yyvsp[-1].p;
	}
    break;

  case 98:
#line 766 "parse.y"
    {
	  yyval.p = mk_constant(TBL_NO_CHANGE);
	}
    break;

  case 99:
#line 771 "parse.y"
    {
	  yyval.p = mk_constant(yyvsp[0].i);
	}
    break;

  case 100:
#line 776 "parse.y"
    {
	  yyval.p = mk_constant(yyvsp[0].i);
	}
    break;

  case 101:
#line 781 "parse.y"
    {
	  yyval.p = mk_constant(yyvsp[0].i);
	}
    break;

  case 102:
#line 788 "parse.y"
    { 
          yyval.p = mk_symbol(yyvsp[0].s);
        }
    break;

  case 103:
#line 793 "parse.y"
    {
          yyval.p = mk_2op(CONCAT, mk_symbol(yyvsp[-2].s), mk_1op(VAR, yyvsp[-1].p));
        }
    break;

  case 104:
#line 798 "parse.y"
    {
          yyval.p = mk_2op(CONCAT, mk_symbol(yyvsp[-2].s), 
                        mk_2op(CONCAT, mk_1op(VAR, yyvsp[-1].p), mk_symbol(yyvsp[0].s)));
        }
    break;

  case 105:
#line 806 "parse.y"
    { 
          yyval.s = yyvsp[0].s;
        }
    break;

  case 106:
#line 811 "parse.y"
    {
          if (asm_enabled() && !state.mac_prev) {
	    yyval.s = evaluate_concatenation(mk_2op(CONCAT,  mk_symbol(yyvsp[-2].s), 
                           mk_1op(VAR, yyvsp[-1].p)));
	  }
        }
    break;

  case 107:
#line 819 "parse.y"
    {
          if (asm_enabled() && !state.mac_prev) {
            yyval.s = evaluate_concatenation(mk_2op(CONCAT,  mk_symbol(yyvsp[-2].s), 
                      mk_2op(CONCAT, mk_1op(VAR, yyvsp[-1].p), mk_symbol(yyvsp[0].s))));
	  }
        }
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2006 "parse.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 827 "parse.y"



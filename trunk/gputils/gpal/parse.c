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
     ALIAS = 258,
     ARRAY = 259,
     AT = 260,
     CASE = 261,
     CONSTANT = 262,
     BEGIN_TOK = 263,
     ELSE = 264,
     ELSIF = 265,
     END = 266,
     FOR = 267,
     FUNCTION = 268,
     GOTO = 269,
     IF = 270,
     IN = 271,
     INOUT = 272,
     IS = 273,
     LOOP = 274,
     MODULE = 275,
     NULL_TOK = 276,
     OF = 277,
     OTHERS = 278,
     PRAGMA = 279,
     PROCEDURE = 280,
     PUBLIC = 281,
     RETURN = 282,
     THEN = 283,
     TO = 284,
     TYPE = 285,
     OUT = 286,
     WHEN = 287,
     WHILE = 288,
     WITH = 289,
     ASM = 290,
     IDENT = 291,
     LABEL = 292,
     NUMBER = 293,
     STRING = 294,
     LSH = 295,
     RSH = 296,
     ARROW = 297,
     GREATER_EQUAL = 298,
     LESS_EQUAL = 299,
     EQUAL = 300,
     NOT_EQUAL = 301,
     LOGICAL_AND = 302,
     LOGICAL_OR = 303
   };
#endif
#define ALIAS 258
#define ARRAY 259
#define AT 260
#define CASE 261
#define CONSTANT 262
#define BEGIN_TOK 263
#define ELSE 264
#define ELSIF 265
#define END 266
#define FOR 267
#define FUNCTION 268
#define GOTO 269
#define IF 270
#define IN 271
#define INOUT 272
#define IS 273
#define LOOP 274
#define MODULE 275
#define NULL_TOK 276
#define OF 277
#define OTHERS 278
#define PRAGMA 279
#define PROCEDURE 280
#define PUBLIC 281
#define RETURN 282
#define THEN 283
#define TO 284
#define TYPE 285
#define OUT 286
#define WHEN 287
#define WHILE 288
#define WITH 289
#define ASM 290
#define IDENT 291
#define LABEL 292
#define NUMBER 293
#define STRING 294
#define LSH 295
#define RSH 296
#define ARROW 297
#define GREATER_EQUAL 298
#define LESS_EQUAL 299
#define EQUAL 300
#define NOT_EQUAL 301
#define LOGICAL_AND 302
#define LOGICAL_OR 303




/* Copy the first part of user declarations.  */
#line 1 "parse.y"

/* Parser
   Copyright (C) 2003, 2004
   Craig Franklin

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
#include "gpal.h"
#include "scan.h"

#define YYERROR_VERBOSE

void
yyerror(char *message)
{

  gp_num_errors++;

  if (gp_quiet != 0)
    return;

  assert(message != NULL);
  
  if (state.src) {
    printf("%s:%d:%s\n", state.src->name, state.src->line_number, message);
  } else {
    printf("error: %s\n", message);
  }

  return;
}

int yylex(void);

static gp_linked_list *case_stack = NULL;
static tree *case_ident = NULL;



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
#line 60 "parse.y"
typedef union YYSTYPE {
  int i;
  struct {
    tree *start;
    tree *end;
  } r;
  char *s;
  tree *t;
  enum node_dir d;
  enum node_op o;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 241 "parse.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 253 "parse.c"

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
#define YYLAST   248

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  43
/* YYNRULES -- Number of rules. */
#define YYNRULES  116
/* YYNRULES -- Number of states. */
#define YYNSTATES  238

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   303

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      64,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    62,     2,     2,     2,    61,    51,     2,
      65,    66,    59,    57,    67,    58,     2,    60,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    41,    40,
      49,    56,    50,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    68,     2,    69,    53,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    52,     2,    63,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    42,    43,    44,    45,    46,
      47,    48,    54,    55
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     7,    11,    15,    23,    31,    33,
      36,    40,    42,    44,    49,    55,    59,    67,    73,    79,
      88,    96,    98,   103,   105,   109,   114,   116,   118,   120,
     126,   127,   129,   131,   134,   139,   146,   153,   162,   170,
     171,   173,   175,   178,   181,   183,   187,   191,   194,   202,
     211,   212,   221,   227,   231,   233,   236,   241,   247,   250,
     255,   261,   266,   268,   272,   274,   276,   282,   286,   288,
     292,   294,   296,   300,   302,   304,   308,   310,   312,   314,
     318,   320,   322,   324,   326,   330,   332,   334,   336,   338,
     340,   342,   344,   348,   350,   352,   354,   358,   360,   362,
     364,   368,   370,   372,   374,   376,   379,   381,   383,   385,
     387,   389,   391,   396,   400,   405,   407
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      71,     0,    -1,    -1,    71,    72,    -1,    71,     1,    64,
      -1,    34,    36,    40,    -1,    20,    36,    18,    73,    11,
      20,    40,    -1,    26,    36,    18,    73,    11,    26,    40,
      -1,    74,    -1,    74,    73,    -1,    24,    95,    40,    -1,
      75,    -1,    83,    -1,     3,    36,    36,    40,    -1,    25,
      76,    80,    25,    40,    -1,    25,    76,    40,    -1,    13,
      76,    27,    36,    80,    13,    40,    -1,    13,    76,    27,
      36,    40,    -1,    30,    36,    18,    36,    40,    -1,    30,
      36,    18,     4,    93,    22,    36,    40,    -1,    30,    36,
      18,    65,    94,    66,    40,    -1,    36,    -1,    36,    65,
      77,    66,    -1,    78,    -1,    78,    67,    77,    -1,    36,
      41,    79,    36,    -1,    16,    -1,    17,    -1,    31,    -1,
      18,    81,     8,    84,    11,    -1,    -1,    82,    -1,    83,
      -1,    83,    82,    -1,    36,    41,    36,    40,    -1,    36,
      41,    36,     5,    95,    40,    -1,    36,    41,    36,    56,
      95,    40,    -1,    36,    41,    36,    56,    95,     5,    95,
      40,    -1,    36,    41,     7,    36,    56,    95,    40,    -1,
      -1,    85,    -1,    86,    -1,    86,    85,    -1,    21,    40,
      -1,    37,    -1,    14,    36,    40,    -1,    27,    95,    40,
      -1,    35,    40,    -1,    15,    95,    28,    84,    11,    15,
      40,    -1,    15,    95,    28,    84,    88,    11,    15,    40,
      -1,    -1,     6,    36,    87,    18,    89,    11,     6,    40,
      -1,    12,    36,    16,    93,    92,    -1,    33,    95,    92,
      -1,    92,    -1,    95,    40,    -1,    10,    95,    28,    84,
      -1,    10,    95,    28,    84,    88,    -1,     9,    84,    -1,
      32,    90,    44,    84,    -1,    32,    90,    44,    84,    89,
      -1,    32,    23,    44,    84,    -1,    91,    -1,    91,    52,
      90,    -1,    36,    -1,    38,    -1,    19,    84,    11,    19,
      40,    -1,    95,    29,    95,    -1,    95,    -1,    95,    67,
      94,    -1,    96,    -1,    98,    -1,    96,    97,    98,    -1,
      56,    -1,   100,    -1,    98,    99,   100,    -1,    54,    -1,
      55,    -1,   102,    -1,   100,   101,   102,    -1,    51,    -1,
      52,    -1,    53,    -1,   104,    -1,   102,   103,   104,    -1,
      49,    -1,    50,    -1,    47,    -1,    48,    -1,    45,    -1,
      46,    -1,   106,    -1,   104,   105,   106,    -1,    42,    -1,
      43,    -1,   108,    -1,   106,   107,   108,    -1,    57,    -1,
      58,    -1,   110,    -1,   108,   109,   110,    -1,    59,    -1,
      60,    -1,    61,    -1,   112,    -1,   111,   112,    -1,    58,
      -1,    62,    -1,    63,    -1,    57,    -1,    39,    -1,    36,
      -1,    36,    68,    95,    69,    -1,    36,    65,    66,    -1,
      36,    65,    94,    66,    -1,    38,    -1,    65,    95,    66,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   186,   186,   189,   191,   195,   200,   205,   212,   217,
     224,   229,   234,   239,   244,   249,   254,   259,   266,   272,
     278,   286,   291,   298,   303,   310,   317,   318,   319,   323,
     331,   335,   342,   347,   354,   359,   364,   369,   374,   382,
     386,   393,   398,   405,   410,   415,   420,   425,   430,   435,
     441,   440,   461,   480,   485,   490,   497,   503,   508,   516,
     522,   527,   535,   540,   547,   552,   559,   566,   574,   579,
     585,   588,   590,   596,   599,   601,   607,   608,   611,   613,
     619,   620,   621,   624,   626,   632,   633,   634,   635,   636,
     637,   640,   642,   648,   649,   652,   654,   660,   661,   664,
     666,   672,   673,   674,   677,   679,   685,   686,   687,   688,
     691,   696,   701,   706,   712,   718,   723
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"alias\"", "\"array\"", "\"at\"",
  "\"case\"", "\"constant\"", "\"begin\"", "\"else\"", "\"elsif\"",
  "\"end\"", "\"for\"", "\"function\"", "\"goto\"", "\"if\"", "\"in\"",
  "\"inout\"", "\"is\"", "\"loop\"", "\"module\"", "\"null\"", "\"of\"",
  "\"others\"", "\"pragma\"", "\"procedure\"", "\"public\"", "\"return\"",
  "\"then\"", "\"to\"", "\"type\"", "\"out\"", "\"when\"", "\"while\"",
  "\"with\"", "\"asm\"", "\"symbol\"", "\"label\"", "\"number\"",
  "\"string\"", "';'", "':'", "\"<<\"", "\">>\"", "\"=>\"", "\">=\"",
  "\"<=\"", "\"==\"", "\"!=\"", "'<'", "'>'", "'&'", "'|'", "'^'",
  "\"&&\"", "\"||\"", "'='", "'+'", "'-'", "'*'", "'/'", "'%'", "'!'",
  "'~'", "'\\n'", "'('", "')'", "','", "'['", "']'", "$accept", "program",
  "entity", "element_list", "element", "type", "head", "arg_list", "arg",
  "arg_direction", "body", "decl_start", "decl_block", "decl",
  "statement_start", "statement_block", "statement", "@1", "if_body",
  "case_body", "case_element_list", "case_element", "loop_statement",
  "range", "parameter_list", "expr", "e8", "e8op", "e7", "e7op", "e6",
  "e6op", "e5", "e5op", "e4", "e4op", "e3", "e3op", "e2", "e2op", "e1",
  "e1op", "e0", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
      59,    58,   295,   296,   297,   298,   299,   300,   301,    60,
      62,    38,   124,    94,   302,   303,    61,    43,    45,    42,
      47,    37,    33,   126,    10,    40,    41,    44,    91,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    70,    71,    71,    71,    72,    72,    72,    73,    73,
      74,    74,    74,    74,    74,    74,    74,    74,    75,    75,
      75,    76,    76,    77,    77,    78,    79,    79,    79,    80,
      81,    81,    82,    82,    83,    83,    83,    83,    83,    84,
      84,    85,    85,    86,    86,    86,    86,    86,    86,    86,
      87,    86,    86,    86,    86,    86,    88,    88,    88,    89,
      89,    89,    90,    90,    91,    91,    92,    93,    94,    94,
      95,    96,    96,    97,    98,    98,    99,    99,   100,   100,
     101,   101,   101,   102,   102,   103,   103,   103,   103,   103,
     103,   104,   104,   105,   105,   106,   106,   107,   107,   108,
     108,   109,   109,   109,   110,   110,   111,   111,   111,   111,
     112,   112,   112,   112,   112,   112,   112
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     3,     3,     7,     7,     1,     2,
       3,     1,     1,     4,     5,     3,     7,     5,     5,     8,
       7,     1,     4,     1,     3,     4,     1,     1,     1,     5,
       0,     1,     1,     2,     4,     6,     6,     8,     7,     0,
       1,     1,     2,     2,     1,     3,     3,     2,     7,     8,
       0,     8,     5,     3,     1,     2,     4,     5,     2,     4,
       5,     4,     1,     3,     1,     1,     5,     3,     1,     3,
       1,     1,     3,     1,     1,     3,     1,     1,     1,     3,
       1,     1,     1,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       3,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     4,     3,     4,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     3,     4,     0,
       0,     0,     0,     0,     5,     0,     0,     0,     0,     0,
       0,     0,     8,    11,    12,     0,     0,    21,     0,   111,
     115,   110,   109,   106,   107,   108,     0,     0,    70,    71,
      74,    78,    83,    91,    95,    99,     0,   104,     0,     0,
       0,     0,     9,     0,     0,     0,     0,     0,     0,     0,
      10,    73,     0,    76,    77,     0,    80,    81,    82,     0,
      89,    90,    87,    88,    85,    86,     0,    93,    94,     0,
      97,    98,     0,   101,   102,   103,     0,   105,    30,    15,
       0,     0,     0,     0,     0,     0,    13,     0,     0,    23,
       0,   113,     0,    68,     0,   116,    72,    75,    79,    84,
      92,    96,   100,     0,    31,    32,     0,     0,     0,     0,
       0,     0,    34,     0,     6,     7,     0,    22,     0,    17,
       0,   114,     0,   112,    39,    33,    14,     0,     0,    18,
       0,     0,     0,     0,    26,    27,    28,     0,    24,     0,
      69,     0,     0,     0,     0,    39,     0,     0,     0,     0,
      44,     0,    40,    41,    54,     0,     0,     0,     0,     0,
      35,     0,    36,    25,    16,    50,     0,     0,     0,     0,
      43,     0,     0,    47,    29,    42,    55,     0,    67,    20,
      38,     0,     0,     0,    45,    39,     0,    46,    53,    19,
      37,     0,     0,     0,     0,     0,     0,    52,    39,     0,
       0,     0,    66,     0,    64,    65,     0,    62,     0,    58,
       0,     0,     0,    39,    39,     0,     0,    39,    48,     0,
      61,    59,    63,    51,    56,    49,    60,    57
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     7,    21,    22,    23,    28,    98,    99,   147,
      90,   113,   114,    24,   161,   162,   163,   192,   211,   206,
     216,   217,   164,   137,   102,   165,    38,    62,    39,    65,
      40,    69,    41,    76,    42,    79,    43,    82,    44,    86,
      45,    46,    47
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -154
static const short yypact[] =
{
    -154,    56,  -154,   -51,   -18,    27,    29,  -154,  -154,    17,
      41,    49,    83,    83,  -154,    33,    45,    73,    45,    55,
      60,    92,    83,  -154,  -154,    94,    74,    61,    87,   -48,
    -154,  -154,  -154,  -154,  -154,  -154,    73,    85,    71,    23,
      42,    97,    78,    65,    38,  -154,   -31,  -154,   -12,   110,
       8,   109,  -154,   106,    93,    98,   103,    22,    73,    82,
    -154,  -154,    73,  -154,  -154,    73,  -154,  -154,  -154,    73,
    -154,  -154,  -154,  -154,  -154,  -154,    73,  -154,  -154,    73,
    -154,  -154,    73,  -154,  -154,  -154,    73,  -154,   113,  -154,
     126,    -3,   116,    -2,   115,   117,  -154,   112,    90,    91,
      -8,  -154,    95,    96,    99,  -154,    23,    42,    97,    78,
      65,    38,  -154,   151,  -154,   113,   120,    73,   122,    73,
     108,    73,  -154,    73,  -154,  -154,    -5,  -154,    98,  -154,
     152,  -154,    73,  -154,    10,  -154,  -154,   144,   138,  -154,
     104,    73,   129,    -1,  -154,  -154,  -154,   135,  -154,   132,
    -154,   137,   139,   141,    73,    10,   134,    73,    73,   140,
    -154,   167,  -154,    10,  -154,   142,   143,    73,   145,   146,
    -154,    73,  -154,  -154,  -154,  -154,   165,   147,   155,   173,
    -154,   148,   170,  -154,  -154,  -154,  -154,   150,  -154,  -154,
    -154,   153,   176,    73,  -154,    10,   172,  -154,  -154,  -154,
    -154,   163,   170,   107,   156,    28,   186,  -154,    10,    73,
     183,   188,  -154,   157,  -154,  -154,   158,   154,   194,  -154,
     175,   164,   190,    10,    10,   -15,   168,    10,  -154,   169,
    -154,   163,  -154,  -154,    43,  -154,  -154,  -154
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -154,  -154,  -154,    70,  -154,  -154,   189,    84,  -154,  -154,
     111,  -154,   100,   -79,  -153,    47,  -154,  -154,   -21,   -14,
     -11,  -154,  -152,    25,  -105,   -17,  -154,  -154,   159,  -154,
     160,  -154,   161,  -154,   162,  -154,   149,  -154,   166,  -154,
     130,  -154,   174
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      37,   117,   179,   121,   171,    29,    88,    30,    31,   115,
      88,   144,   145,     8,   140,    92,   151,    57,     9,    59,
      58,   214,   152,   215,   153,   154,   146,   150,    89,   155,
     198,   156,   129,   118,    36,    12,   115,   157,   122,   172,
     103,   104,   203,   158,    93,   159,    29,   160,    30,    31,
     207,   213,   208,   209,   123,   219,     2,     3,    29,    13,
      30,    31,   119,    10,   214,    11,   215,    32,    33,    26,
     230,   231,    34,    35,   234,    36,     4,    63,    64,    32,
      33,    27,     5,    25,    34,    35,    15,    36,   101,    14,
       6,    49,    52,    66,    67,    68,    16,    83,    84,    85,
     138,    50,   103,    51,   142,    53,   143,    17,    18,    29,
      54,    30,    31,    19,    56,   103,   208,   209,   210,    20,
      77,    78,    80,    81,   169,    60,    55,    61,    91,    94,
      32,    33,    95,    96,    97,    34,    35,   178,    36,   100,
     181,   182,    70,    71,    72,    73,    74,    75,   105,    20,
     188,   116,   120,   126,   191,   124,   127,   125,   128,   134,
     136,   131,   139,   132,   141,   149,   166,   167,   133,   170,
     168,   173,   174,   175,   180,   176,   138,   177,   184,   187,
     183,   193,   186,   195,   196,   189,   190,   194,   197,   155,
     199,   204,   220,   200,   201,   205,   212,   218,   221,   222,
     226,   223,   224,   227,   228,   229,   225,    48,   233,   235,
     185,   130,   148,   237,   232,   135,   112,   236,   202,     0,
      87,   106,     0,     0,     0,   107,     0,     0,   110,     0,
     108,     0,     0,     0,     0,     0,     0,     0,   109,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   111
};

static const short yycheck[] =
{
      17,     4,   155,     5,     5,    36,    18,    38,    39,    88,
      18,    16,    17,    64,   119,     7,     6,    65,    36,    36,
      68,    36,    12,    38,    14,    15,    31,   132,    40,    19,
     182,    21,    40,    36,    65,    18,   115,    27,    40,    40,
      57,    58,   195,    33,    36,    35,    36,    37,    38,    39,
     202,    23,     9,    10,    56,   208,     0,     1,    36,    18,
      38,    39,    65,    36,    36,    36,    38,    57,    58,    36,
     223,   224,    62,    63,   227,    65,    20,    54,    55,    57,
      58,    36,    26,    13,    62,    63,     3,    65,    66,    40,
      34,    36,    22,    51,    52,    53,    13,    59,    60,    61,
     117,    41,   119,    11,   121,    11,   123,    24,    25,    36,
      36,    38,    39,    30,    27,   132,     9,    10,    11,    36,
      42,    43,    57,    58,   141,    40,    65,    56,    18,    20,
      57,    58,    26,    40,    36,    62,    63,   154,    65,    36,
     157,   158,    45,    46,    47,    48,    49,    50,    66,    36,
     167,    25,    36,    41,   171,    40,    66,    40,    67,     8,
      40,    66,    40,    67,    56,    13,    22,    29,    69,    40,
      66,    36,    40,    36,    40,    36,   193,    36,    11,    36,
      40,    16,    40,    28,    11,    40,    40,    40,    40,    19,
      40,    19,   209,    40,    18,    32,    40,    11,    15,    11,
       6,    44,    44,    28,    40,    15,    52,    18,    40,    40,
     163,   100,   128,   234,   225,   115,    86,   231,   193,    -1,
      46,    62,    -1,    -1,    -1,    65,    -1,    -1,    79,    -1,
      69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    71,     0,     1,    20,    26,    34,    72,    64,    36,
      36,    36,    18,    18,    40,     3,    13,    24,    25,    30,
      36,    73,    74,    75,    83,    73,    36,    36,    76,    36,
      38,    39,    57,    58,    62,    63,    65,    95,    96,    98,
     100,   102,   104,   106,   108,   110,   111,   112,    76,    36,
      41,    11,    73,    11,    36,    65,    27,    65,    68,    95,
      40,    56,    97,    54,    55,    99,    51,    52,    53,   101,
      45,    46,    47,    48,    49,    50,   103,    42,    43,   105,
      57,    58,   107,    59,    60,    61,   109,   112,    18,    40,
      80,    18,     7,    36,    20,    26,    40,    36,    77,    78,
      36,    66,    94,    95,    95,    66,    98,   100,   102,   104,
     106,   108,   110,    81,    82,    83,    25,     4,    36,    65,
      36,     5,    40,    56,    40,    40,    41,    66,    67,    40,
      80,    66,    67,    69,     8,    82,    40,    93,    95,    40,
      94,    56,    95,    95,    16,    17,    31,    79,    77,    13,
      94,     6,    12,    14,    15,    19,    21,    27,    33,    35,
      37,    84,    85,    86,    92,    95,    22,    29,    66,    95,
      40,     5,    40,    36,    40,    36,    36,    36,    95,    84,
      40,    95,    95,    40,    11,    85,    40,    36,    95,    40,
      40,    95,    87,    16,    40,    28,    11,    40,    92,    40,
      40,    18,    93,    84,    19,    32,    89,    92,     9,    10,
      11,    88,    40,    23,    36,    38,    90,    91,    11,    84,
      95,    15,    11,    44,    44,    52,     6,    28,    40,    15,
      84,    84,    90,    40,    84,    40,    89,    88
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
        case 5:
#line 196 "parse.y"
    {
	  open_src(yyvsp[-1].s, source_with);
	}
    break;

  case 6:
#line 201 "parse.y"
    {
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;

  case 7:
#line 206 "parse.y"
    {
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;

  case 8:
#line 213 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 9:
#line 218 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 10:
#line 225 "parse.y"
    {
	  yyval.t = mk_pragma(yyvsp[-1].t);
	}
    break;

  case 11:
#line 230 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
        }
    break;

  case 12:
#line 235 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
        }
    break;

  case 13:
#line 240 "parse.y"
    {
	  yyval.t = mk_alias(yyvsp[-2].s, yyvsp[-1].s);
        }
    break;

  case 14:
#line 245 "parse.y"
    { 
	  yyval.t = mk_proc(yyvsp[-3].t, yyvsp[-2].t);
     	}
    break;

  case 15:
#line 250 "parse.y"
    { 
	  yyval.t = mk_proc(yyvsp[-1].t, NULL);
	}
    break;

  case 16:
#line 255 "parse.y"
    { 
	  yyval.t = mk_func(yyvsp[-5].t, yyvsp[-3].s, yyvsp[-2].t);
	}
    break;

  case 17:
#line 260 "parse.y"
    { 
	  yyval.t = mk_func(yyvsp[-3].t, yyvsp[-1].s, NULL);
	}
    break;

  case 18:
#line 267 "parse.y"
    {
	  /* alias */
	  yyval.t = mk_type(yyvsp[-3].s, NULL, NULL, NULL, yyvsp[-1].s);
        }
    break;

  case 19:
#line 273 "parse.y"
    {
	  /* array */
	  yyval.t = mk_type(yyvsp[-6].s, yyvsp[-3].r.start, yyvsp[-3].r.end, NULL, yyvsp[-1].s);
        }
    break;

  case 20:
#line 279 "parse.y"
    {
	  /* enumerated type */
	  yyval.t = mk_type(yyvsp[-5].s, NULL, NULL, yyvsp[-2].t, NULL);
        }
    break;

  case 21:
#line 287 "parse.y"
    {
 	  yyval.t = mk_head(yyvsp[0].s, NULL);
     	}
    break;

  case 22:
#line 292 "parse.y"
    {
 	  yyval.t = mk_head(yyvsp[-3].s, yyvsp[-1].t);
     	}
    break;

  case 23:
#line 299 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 24:
#line 304 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 25:
#line 311 "parse.y"
    {	  
	  yyval.t = mk_arg(yyvsp[-3].s, yyvsp[-1].d, yyvsp[0].s);
        }
    break;

  case 26:
#line 317 "parse.y"
    { yyval.d = dir_in; }
    break;

  case 27:
#line 318 "parse.y"
    { yyval.d = dir_inout; }
    break;

  case 28:
#line 319 "parse.y"
    { yyval.d = dir_out; }
    break;

  case 29:
#line 324 "parse.y"
    {
	  yyval.t = mk_body(yyvsp[-3].t, yyvsp[-1].t);
	}
    break;

  case 30:
#line 331 "parse.y"
    {
	  yyval.t = NULL;
	}
    break;

  case 31:
#line 336 "parse.y"
    {
          yyval.t = yyvsp[0].t;
	}
    break;

  case 32:
#line 343 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 33:
#line 348 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 34:
#line 355 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-3].s, false, yyvsp[-1].s, NULL, NULL);
        }
    break;

  case 35:
#line 360 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-5].s, false, yyvsp[-3].s, NULL, yyvsp[-1].t);
        }
    break;

  case 36:
#line 365 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-5].s, false, yyvsp[-3].s, yyvsp[-1].t, NULL);
        }
    break;

  case 37:
#line 370 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-7].s, false, yyvsp[-5].s, yyvsp[-3].t, yyvsp[-1].t);
        }
    break;

  case 38:
#line 375 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-6].s, true, yyvsp[-3].s, yyvsp[-1].t, NULL);
        }
    break;

  case 39:
#line 382 "parse.y"
    {
	  yyval.t = NULL;
	}
    break;

  case 40:
#line 387 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
	}
    break;

  case 41:
#line 394 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 42:
#line 399 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 43:
#line 406 "parse.y"
    {
	  yyval.t = mk_assembly(strdup("  nop"));
	}
    break;

  case 44:
#line 411 "parse.y"
    {
	  yyval.t = mk_label(yyvsp[0].s);
	}
    break;

  case 45:
#line 416 "parse.y"
    {
	  yyval.t = mk_goto(yyvsp[-1].s);
	}
    break;

  case 46:
#line 421 "parse.y"
    {
	  yyval.t = mk_return(yyvsp[-1].t);
	}
    break;

  case 47:
#line 426 "parse.y"
    {
	  yyval.t = mk_assembly(yyvsp[-1].s);
	}
    break;

  case 48:
#line 431 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-5].t, yyvsp[-3].t, NULL);
	}
    break;

  case 49:
#line 436 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-6].t, yyvsp[-4].t, yyvsp[-3].t);
	}
    break;

  case 50:
#line 441 "parse.y"
    {
          gp_linked_list *new = gp_list_make();

          new->prev = case_stack;
          case_stack = new;

	  case_ident = mk_symbol(yyvsp[0].s, NULL);
          gp_list_annotate(case_stack, case_ident);
	}
    break;

  case 51:
#line 451 "parse.y"
    {
	  yyval.t = yyvsp[-3].t;
          case_stack = case_stack->prev;
          if (case_stack) {
            case_ident = gp_list_get(case_stack);
          } else {
            case_ident = NULL;
          }
	}
    break;

  case 52:
#line 462 "parse.y"
    {
	  tree *init;
          tree *exit;
          tree *increment;
          
          /* IDENT = range.start; */
          init = mk_binop(op_assign, mk_symbol(yyvsp[-3].s, NULL), yyvsp[-1].r.start);
          
          /* IDENT = IDENT + 1; */
          increment = mk_binop(op_assign, mk_symbol(yyvsp[-3].s, NULL), 
                               mk_binop(op_add, mk_symbol(yyvsp[-3].s, NULL), mk_constant(1)));
          
          /* while (IDENT <= range.end) then loop */
          exit = mk_binop(op_lte, mk_symbol(yyvsp[-3].s, NULL), yyvsp[-1].r.end);

	  yyval.t = mk_loop(init, exit, increment, yyvsp[0].t);
	}
    break;

  case 53:
#line 481 "parse.y"
    {
	  yyval.t= mk_loop(NULL, yyvsp[-1].t, NULL, yyvsp[0].t);	
	}
    break;

  case 54:
#line 486 "parse.y"
    {
	  yyval.t= mk_loop(NULL, NULL, NULL, yyvsp[0].t);
	}
    break;

  case 55:
#line 491 "parse.y"
    {
	  yyval.t = yyvsp[-1].t;
	}
    break;

  case 56:
#line 498 "parse.y"
    {
	  /* last statement is elsif */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;

  case 57:
#line 504 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 58:
#line 509 "parse.y"
    {
	  /* last statement is else */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;

  case 59:
#line 517 "parse.y"
    {
	  /* last statement is elsif equivalent */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;

  case 60:
#line 523 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 61:
#line 528 "parse.y"
    {
	  /* last statement is else equivalent */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;

  case 62:
#line 536 "parse.y"
    {
	  yyval.t = mk_binop(op_eq, case_ident, yyvsp[0].t);
	}
    break;

  case 63:
#line 541 "parse.y"
    {
	  yyval.t = mk_binop(op_lor, mk_binop(op_eq, case_ident, yyvsp[-2].t), yyvsp[0].t);
	}
    break;

  case 64:
#line 548 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;

  case 65:
#line 553 "parse.y"
    {
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;

  case 66:
#line 560 "parse.y"
    {
	  yyval.t = yyvsp[-3].t;
	}
    break;

  case 67:
#line 567 "parse.y"
    {
	  yyval.r.start = yyvsp[-2].t;
	  yyval.r.end = yyvsp[0].t;
        }
    break;

  case 68:
#line 575 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 69:
#line 580 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 72:
#line 591 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 73:
#line 596 "parse.y"
    { yyval.o = op_assign; }
    break;

  case 75:
#line 602 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 76:
#line 607 "parse.y"
    { yyval.o = op_land; }
    break;

  case 77:
#line 608 "parse.y"
    { yyval.o = op_lor; }
    break;

  case 79:
#line 614 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 80:
#line 619 "parse.y"
    { yyval.o = op_and; }
    break;

  case 81:
#line 620 "parse.y"
    { yyval.o = op_or; }
    break;

  case 82:
#line 621 "parse.y"
    { yyval.o = op_xor; }
    break;

  case 84:
#line 627 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 85:
#line 632 "parse.y"
    { yyval.o = op_lt; }
    break;

  case 86:
#line 633 "parse.y"
    { yyval.o = op_gt; }
    break;

  case 87:
#line 634 "parse.y"
    { yyval.o = op_eq; }
    break;

  case 88:
#line 635 "parse.y"
    { yyval.o = op_ne; }
    break;

  case 89:
#line 636 "parse.y"
    { yyval.o = op_gte; }
    break;

  case 90:
#line 637 "parse.y"
    { yyval.o = op_lte; }
    break;

  case 92:
#line 643 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 93:
#line 648 "parse.y"
    { yyval.o = op_lsh; }
    break;

  case 94:
#line 649 "parse.y"
    { yyval.o = op_rsh; }
    break;

  case 96:
#line 655 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 97:
#line 660 "parse.y"
    { yyval.o = op_add; }
    break;

  case 98:
#line 661 "parse.y"
    { yyval.o = op_sub; }
    break;

  case 100:
#line 667 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 101:
#line 672 "parse.y"
    { yyval.o = op_mult; }
    break;

  case 102:
#line 673 "parse.y"
    { yyval.o = op_div; }
    break;

  case 103:
#line 674 "parse.y"
    { yyval.o = op_mod; }
    break;

  case 105:
#line 680 "parse.y"
    {
	  yyval.t = mk_unop(yyvsp[-1].o, yyvsp[0].t);
	}
    break;

  case 106:
#line 685 "parse.y"
    { yyval.o = op_neg; }
    break;

  case 107:
#line 686 "parse.y"
    { yyval.o = op_not; }
    break;

  case 108:
#line 687 "parse.y"
    { yyval.o = op_com; }
    break;

  case 109:
#line 688 "parse.y"
    { yyval.o = op_add; }
    break;

  case 110:
#line 692 "parse.y"
    {
	  yyval.t = mk_string(yyvsp[0].s);
        }
    break;

  case 111:
#line 697 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;

  case 112:
#line 702 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[-3].s, yyvsp[-1].t);
        }
    break;

  case 113:
#line 707 "parse.y"
    {
	  /* function or procedure call */
	  yyval.t = mk_call(yyvsp[-2].s, NULL);
	}
    break;

  case 114:
#line 713 "parse.y"
    {
	  /* function or procedure call with arguments */
	  yyval.t = mk_call(yyvsp[-3].s, yyvsp[-1].t);
	}
    break;

  case 115:
#line 719 "parse.y"
    {
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;

  case 116:
#line 724 "parse.y"
    {
	  yyval.t = yyvsp[-1].t;
	}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2063 "parse.c"

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


#line 729 "parse.y"



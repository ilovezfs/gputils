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
#define YYLAST   282

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  45
/* YYNRULES -- Number of rules. */
#define YYNRULES  123
/* YYNRULES -- Number of states. */
#define YYNSTATES  257

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
       0,     0,     3,     4,     7,    11,    15,    23,    31,    32,
      34,    36,    39,    43,    45,    47,    52,    58,    62,    70,
      76,    84,    93,    98,   104,   107,   113,   122,   130,   132,
     137,   139,   143,   148,   150,   152,   154,   160,   161,   163,
     165,   168,   173,   180,   187,   196,   204,   205,   207,   209,
     212,   215,   217,   221,   225,   228,   236,   245,   246,   255,
     261,   265,   267,   270,   275,   281,   284,   289,   295,   300,
     302,   306,   308,   310,   316,   320,   322,   326,   328,   330,
     334,   336,   338,   342,   344,   346,   348,   352,   354,   356,
     358,   360,   364,   366,   368,   370,   372,   374,   376,   378,
     382,   384,   386,   388,   392,   394,   396,   398,   402,   404,
     406,   408,   410,   413,   415,   417,   419,   421,   423,   425,
     430,   434,   439,   441
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      71,     0,    -1,    -1,    71,    72,    -1,    71,     1,    64,
      -1,    34,    36,    40,    -1,    20,    36,    18,    73,    11,
      20,    40,    -1,    26,    36,    18,    73,    11,    26,    40,
      -1,    -1,    74,    -1,    75,    -1,    75,    74,    -1,    24,
      97,    40,    -1,    77,    -1,    85,    -1,     3,    36,    36,
      40,    -1,    25,    78,    82,    25,    40,    -1,    25,    78,
      40,    -1,    13,    78,    27,    36,    82,    13,    40,    -1,
      13,    78,    27,    36,    40,    -1,    15,    97,    28,    73,
      11,    15,    40,    -1,    15,    97,    28,    73,    76,    11,
      15,    40,    -1,    10,    97,    28,    73,    -1,    10,    97,
      28,    73,    76,    -1,     9,    73,    -1,    30,    36,    18,
      36,    40,    -1,    30,    36,    18,     4,    95,    22,    36,
      40,    -1,    30,    36,    18,    65,    96,    66,    40,    -1,
      36,    -1,    36,    65,    79,    66,    -1,    80,    -1,    80,
      67,    79,    -1,    36,    41,    81,    36,    -1,    16,    -1,
      17,    -1,    31,    -1,    18,    83,     8,    86,    11,    -1,
      -1,    84,    -1,    85,    -1,    85,    84,    -1,    36,    41,
      36,    40,    -1,    36,    41,    36,     5,    97,    40,    -1,
      36,    41,    36,    56,    97,    40,    -1,    36,    41,    36,
      56,    97,     5,    97,    40,    -1,    36,    41,     7,    36,
      56,    97,    40,    -1,    -1,    87,    -1,    88,    -1,    88,
      87,    -1,    21,    40,    -1,    37,    -1,    14,    36,    40,
      -1,    27,    97,    40,    -1,    35,    40,    -1,    15,    97,
      28,    86,    11,    15,    40,    -1,    15,    97,    28,    86,
      90,    11,    15,    40,    -1,    -1,     6,    36,    89,    18,
      91,    11,     6,    40,    -1,    12,    36,    16,    95,    94,
      -1,    33,    97,    94,    -1,    94,    -1,    97,    40,    -1,
      10,    97,    28,    86,    -1,    10,    97,    28,    86,    90,
      -1,     9,    86,    -1,    32,    92,    44,    86,    -1,    32,
      92,    44,    86,    91,    -1,    32,    23,    44,    86,    -1,
      93,    -1,    93,    52,    92,    -1,    36,    -1,    38,    -1,
      19,    86,    11,    19,    40,    -1,    97,    29,    97,    -1,
      97,    -1,    97,    67,    96,    -1,    98,    -1,   100,    -1,
      98,    99,   100,    -1,    56,    -1,   102,    -1,   100,   101,
     102,    -1,    54,    -1,    55,    -1,   104,    -1,   102,   103,
     104,    -1,    51,    -1,    52,    -1,    53,    -1,   106,    -1,
     104,   105,   106,    -1,    49,    -1,    50,    -1,    47,    -1,
      48,    -1,    45,    -1,    46,    -1,   108,    -1,   106,   107,
     108,    -1,    42,    -1,    43,    -1,   110,    -1,   108,   109,
     110,    -1,    57,    -1,    58,    -1,   112,    -1,   110,   111,
     112,    -1,    59,    -1,    60,    -1,    61,    -1,   114,    -1,
     113,   114,    -1,    58,    -1,    62,    -1,    63,    -1,    57,
      -1,    39,    -1,    36,    -1,    36,    68,    97,    69,    -1,
      36,    65,    66,    -1,    36,    65,    96,    66,    -1,    38,
      -1,    65,    97,    66,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   188,   188,   191,   193,   197,   202,   207,   215,   219,
     226,   231,   238,   243,   248,   253,   258,   263,   268,   273,
     278,   283,   290,   296,   301,   309,   315,   321,   329,   334,
     341,   346,   353,   360,   361,   362,   366,   374,   378,   385,
     390,   397,   402,   407,   412,   417,   425,   429,   436,   441,
     448,   453,   458,   463,   468,   473,   478,   484,   483,   504,
     523,   528,   533,   540,   546,   551,   559,   565,   570,   578,
     583,   590,   595,   602,   609,   617,   622,   628,   631,   633,
     639,   642,   644,   650,   651,   654,   656,   662,   663,   664,
     667,   669,   675,   676,   677,   678,   679,   680,   683,   685,
     691,   692,   695,   697,   703,   704,   707,   709,   715,   716,
     717,   720,   722,   728,   729,   730,   731,   734,   739,   744,
     749,   755,   761,   766
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
  "entity", "element_start", "element_list", "element", "element_cond",
  "type", "head", "arg_list", "arg", "arg_direction", "body", "decl_start",
  "decl_block", "decl", "statement_start", "statement_block", "statement",
  "@1", "if_body", "case_body", "case_element_list", "case_element",
  "loop_statement", "range", "parameter_list", "expr", "e8", "e8op", "e7",
  "e7op", "e6", "e6op", "e5", "e5op", "e4", "e4op", "e3", "e3op", "e2",
  "e2op", "e1", "e1op", "e0", 0
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
      74,    74,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    76,    76,    76,    77,    77,    77,    78,    78,
      79,    79,    80,    81,    81,    81,    82,    83,    83,    84,
      84,    85,    85,    85,    85,    85,    86,    86,    87,    87,
      88,    88,    88,    88,    88,    88,    88,    89,    88,    88,
      88,    88,    88,    90,    90,    90,    91,    91,    91,    92,
      92,    93,    93,    94,    95,    96,    96,    97,    98,    98,
      99,   100,   100,   101,   101,   102,   102,   103,   103,   103,
     104,   104,   105,   105,   105,   105,   105,   105,   106,   106,
     107,   107,   108,   108,   109,   109,   110,   110,   111,   111,
     111,   112,   112,   113,   113,   113,   113,   114,   114,   114,
     114,   114,   114,   114
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     3,     3,     7,     7,     0,     1,
       1,     2,     3,     1,     1,     4,     5,     3,     7,     5,
       7,     8,     4,     5,     2,     5,     8,     7,     1,     4,
       1,     3,     4,     1,     1,     1,     5,     0,     1,     1,
       2,     4,     6,     6,     8,     7,     0,     1,     1,     2,
       2,     1,     3,     3,     2,     7,     8,     0,     8,     5,
       3,     1,     2,     4,     5,     2,     4,     5,     4,     1,
       3,     1,     1,     5,     3,     1,     3,     1,     1,     3,
       1,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     3,     1,     1,     1,     3,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     4,
       3,     4,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     3,     4,     0,
       0,     0,     8,     8,     5,     0,     0,     0,     0,     0,
       0,     0,     0,     9,    10,    13,    14,     0,     0,    28,
       0,   118,   122,   117,   116,   113,   114,   115,     0,     0,
      77,    78,    81,    85,    90,    98,   102,   106,     0,   111,
       0,     0,     0,     0,     0,    11,     0,     0,     0,     0,
       0,     0,     0,     8,    80,     0,    83,    84,     0,    87,
      88,    89,     0,    96,    97,    94,    95,    92,    93,     0,
     100,   101,     0,   104,   105,     0,   108,   109,   110,     0,
     112,    12,    37,    17,     0,     0,     0,     0,     0,     0,
      15,     0,     0,    30,     0,   120,     0,    75,     0,   123,
       0,    79,    82,    86,    91,    99,   103,   107,     0,    38,
      39,     0,     0,     0,     0,     0,     0,    41,     0,     6,
       7,     0,    29,     0,    19,     0,   121,     0,   119,     8,
       0,     0,     0,    46,    40,    16,     0,     0,    25,     0,
       0,     0,     0,    33,    34,    35,     0,    31,     0,    76,
      24,     0,     0,     0,     0,     0,     0,     0,    46,     0,
       0,     0,     0,    51,     0,    47,    48,    61,     0,     0,
       0,     0,     0,    42,     0,    43,    32,    18,     8,    20,
       0,    57,     0,     0,     0,     0,    50,     0,     0,    54,
      36,    49,    62,     0,    74,    27,    45,     0,    22,    21,
       0,     0,    52,    46,     0,    53,    60,    26,    44,    23,
       0,     0,     0,     0,     0,     0,    59,    46,     0,     0,
       0,    73,     0,    71,    72,     0,    69,     0,    65,     0,
       0,     0,    46,    46,     0,     0,    46,    55,     0,    68,
      66,    70,    58,    63,    56,    67,    64
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     7,    22,    23,    24,   142,    25,    30,   102,
     103,   156,    94,   118,   119,    26,   174,   175,   176,   210,
     230,   225,   235,   236,   177,   146,   106,   178,    40,    65,
      41,    68,    42,    72,    43,    79,    44,    82,    45,    85,
      46,    89,    47,    48,    49
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -190
static const short yypact[] =
{
    -190,    11,  -190,   -42,    -7,    15,    23,  -190,  -190,    29,
      45,    34,    10,    10,  -190,    46,    50,    37,    37,    50,
      54,    52,    86,  -190,    10,  -190,  -190,    90,    70,    38,
      81,   -51,  -190,  -190,  -190,  -190,  -190,  -190,    37,    99,
      82,    67,   -33,    98,    13,    72,    58,  -190,   -12,  -190,
     101,   -10,   131,     3,   132,  -190,   125,   116,   119,   121,
      22,    37,    92,    10,  -190,    37,  -190,  -190,    37,  -190,
    -190,  -190,    37,  -190,  -190,  -190,  -190,  -190,  -190,    37,
    -190,  -190,    37,  -190,  -190,    37,  -190,  -190,  -190,    37,
    -190,  -190,   123,  -190,   135,     0,   126,     1,   124,   128,
    -190,   120,   100,   102,    -2,  -190,   104,   105,    96,  -190,
      61,    67,   -33,    98,    13,    72,    58,  -190,   163,  -190,
     123,   133,    37,   134,    37,   129,    37,  -190,    37,  -190,
    -190,    33,  -190,   119,  -190,   162,  -190,    37,  -190,    10,
      37,   161,   167,    77,  -190,  -190,   157,   151,  -190,   117,
      37,   141,     2,  -190,  -190,  -190,   146,  -190,   144,  -190,
    -190,   158,   147,   173,   153,   154,   155,    37,    77,   152,
      37,    37,   156,  -190,   182,  -190,    77,  -190,   159,   164,
      37,   165,   166,  -190,    37,  -190,  -190,  -190,    10,  -190,
     168,  -190,   179,   169,   170,   186,  -190,   172,   183,  -190,
    -190,  -190,  -190,   174,  -190,  -190,  -190,   175,   122,  -190,
     185,    37,  -190,    77,   188,  -190,  -190,  -190,  -190,  -190,
     178,   183,   115,   176,    31,   190,  -190,    77,    37,   189,
     202,  -190,   177,  -190,  -190,   180,   171,   211,  -190,   191,
     187,   203,    77,    77,    30,   192,    77,  -190,   193,  -190,
     178,  -190,  -190,   127,  -190,  -190,  -190
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -190,  -190,  -190,   -11,   196,  -190,    14,  -190,   206,    93,
    -190,  -190,   130,  -190,   108,   -87,  -165,    53,  -190,  -190,
     -23,   -19,    -9,  -190,  -189,    25,  -109,   -17,  -190,  -190,
     181,  -190,   184,  -190,   194,  -190,   160,  -190,   195,  -190,
     197,  -190,   148,  -190,   199
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short yytable[] =
{
      39,    50,    27,   195,   122,   120,   126,   184,    92,   216,
      96,     2,     3,    15,    60,   149,    92,    61,    69,    70,
      71,    62,     8,    16,    31,    17,    32,    33,   159,     9,
      93,     4,   226,   120,    18,    19,   123,     5,   134,    97,
      20,   127,   185,   107,   108,     6,    21,    12,   222,   153,
     154,    10,   110,    38,   232,    80,    81,   128,    31,    11,
      32,    33,   238,    13,   155,   124,   233,   233,   234,   234,
     139,   140,   141,    31,    14,    32,    33,   249,   250,    34,
      35,   253,    28,   164,    36,    37,    29,    38,   105,   165,
      52,   166,   167,    53,    34,    35,   168,    54,   169,    36,
      37,    56,    38,    58,   170,   147,    57,   107,    59,   151,
     171,   152,   172,    31,   173,    32,    33,    86,    87,    88,
     107,    66,    67,   161,   227,   228,   229,    63,   160,    83,
      84,   139,   140,   182,    34,    35,   227,   228,    64,    36,
      37,    91,    38,    73,    74,    75,    76,    77,    78,    95,
     194,    99,    98,   197,   198,   101,   100,   104,   109,    21,
     121,   131,   125,   204,   129,   138,   132,   207,   130,   133,
     136,   143,   137,   145,   148,   158,   162,   208,   163,   179,
     180,   183,   186,   181,   187,   150,   188,   189,   190,   191,
     192,   193,   196,   200,   147,   211,   199,   214,   213,   202,
     203,   237,   168,   220,   240,   205,   206,   223,   209,   212,
     224,   239,   215,   241,   217,   218,   231,   245,   248,   246,
      55,   242,   219,   244,   243,    51,   157,   247,   144,   201,
     256,   255,   252,   254,   135,   251,   221,   117,     0,   114,
       0,     0,     0,     0,     0,     0,   111,    90,     0,     0,
       0,     0,   112,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   113,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   115,     0,     0,
       0,     0,   116
};

static const short yycheck[] =
{
      17,    18,    13,   168,     4,    92,     5,     5,    18,   198,
       7,     0,     1,     3,    65,   124,    18,    68,    51,    52,
      53,    38,    64,    13,    36,    15,    38,    39,   137,    36,
      40,    20,   221,   120,    24,    25,    36,    26,    40,    36,
      30,    40,    40,    60,    61,    34,    36,    18,   213,    16,
      17,    36,    63,    65,    23,    42,    43,    56,    36,    36,
      38,    39,   227,    18,    31,    65,    36,    36,    38,    38,
       9,    10,    11,    36,    40,    38,    39,   242,   243,    57,
      58,   246,    36,     6,    62,    63,    36,    65,    66,    12,
      36,    14,    15,    41,    57,    58,    19,    11,    21,    62,
      63,    11,    65,    65,    27,   122,    36,   124,    27,   126,
      33,   128,    35,    36,    37,    38,    39,    59,    60,    61,
     137,    54,    55,   140,     9,    10,    11,    28,   139,    57,
      58,     9,    10,   150,    57,    58,     9,    10,    56,    62,
      63,    40,    65,    45,    46,    47,    48,    49,    50,    18,
     167,    26,    20,   170,   171,    36,    40,    36,    66,    36,
      25,    41,    36,   180,    40,    69,    66,   184,    40,    67,
      66,     8,    67,    40,    40,    13,    15,   188,    11,    22,
      29,    40,    36,    66,    40,    56,    28,    40,    15,    36,
      36,    36,    40,    11,   211,    16,    40,    11,    28,    40,
      36,    11,    19,    18,    15,    40,    40,    19,    40,    40,
      32,   228,    40,    11,    40,    40,    40,     6,    15,    28,
      24,    44,   208,    52,    44,    19,   133,    40,   120,   176,
     253,   250,    40,    40,   104,   244,   211,    89,    -1,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    65,    48,    -1,    -1,
      -1,    -1,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    72,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,
      -1,    -1,    85
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    71,     0,     1,    20,    26,    34,    72,    64,    36,
      36,    36,    18,    18,    40,     3,    13,    15,    24,    25,
      30,    36,    73,    74,    75,    77,    85,    73,    36,    36,
      78,    36,    38,    39,    57,    58,    62,    63,    65,    97,
      98,   100,   102,   104,   106,   108,   110,   112,   113,   114,
      97,    78,    36,    41,    11,    74,    11,    36,    65,    27,
      65,    68,    97,    28,    56,    99,    54,    55,   101,    51,
      52,    53,   103,    45,    46,    47,    48,    49,    50,   105,
      42,    43,   107,    57,    58,   109,    59,    60,    61,   111,
     114,    40,    18,    40,    82,    18,     7,    36,    20,    26,
      40,    36,    79,    80,    36,    66,    96,    97,    97,    66,
      73,   100,   102,   104,   106,   108,   110,   112,    83,    84,
      85,    25,     4,    36,    65,    36,     5,    40,    56,    40,
      40,    41,    66,    67,    40,    82,    66,    67,    69,     9,
      10,    11,    76,     8,    84,    40,    95,    97,    40,    96,
      56,    97,    97,    16,    17,    31,    81,    79,    13,    96,
      73,    97,    15,    11,     6,    12,    14,    15,    19,    21,
      27,    33,    35,    37,    86,    87,    88,    94,    97,    22,
      29,    66,    97,    40,     5,    40,    36,    40,    28,    40,
      15,    36,    36,    36,    97,    86,    40,    97,    97,    40,
      11,    87,    40,    36,    97,    40,    40,    97,    73,    40,
      89,    16,    40,    28,    11,    40,    94,    40,    40,    76,
      18,    95,    86,    19,    32,    91,    94,     9,    10,    11,
      90,    40,    23,    36,    38,    92,    93,    11,    86,    97,
      15,    11,    44,    44,    52,     6,    28,    40,    15,    86,
      86,    92,    40,    86,    40,    91,    90
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
#line 198 "parse.y"
    {
	  open_src(yyvsp[-1].s, source_with);
	}
    break;

  case 6:
#line 203 "parse.y"
    {
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;

  case 7:
#line 208 "parse.y"
    {
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;

  case 8:
#line 215 "parse.y"
    {
	  yyval.t = NULL;
	}
    break;

  case 9:
#line 220 "parse.y"
    {
          yyval.t = yyvsp[0].t;
	}
    break;

  case 10:
#line 227 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 11:
#line 232 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 12:
#line 239 "parse.y"
    {
	  yyval.t = mk_pragma(yyvsp[-1].t);
	}
    break;

  case 13:
#line 244 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
        }
    break;

  case 14:
#line 249 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
        }
    break;

  case 15:
#line 254 "parse.y"
    {
	  yyval.t = mk_alias(yyvsp[-2].s, yyvsp[-1].s);
        }
    break;

  case 16:
#line 259 "parse.y"
    { 
	  yyval.t = mk_proc(yyvsp[-3].t, yyvsp[-2].t);
     	}
    break;

  case 17:
#line 264 "parse.y"
    { 
	  yyval.t = mk_proc(yyvsp[-1].t, NULL);
	}
    break;

  case 18:
#line 269 "parse.y"
    { 
	  yyval.t = mk_func(yyvsp[-5].t, yyvsp[-3].s, yyvsp[-2].t);
	}
    break;

  case 19:
#line 274 "parse.y"
    { 
	  yyval.t = mk_func(yyvsp[-3].t, yyvsp[-1].s, NULL);
	}
    break;

  case 20:
#line 279 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-5].t, yyvsp[-3].t, NULL);
	}
    break;

  case 21:
#line 284 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-6].t, yyvsp[-4].t, yyvsp[-3].t);
	}
    break;

  case 22:
#line 291 "parse.y"
    {
	  /* last statement is elsif */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;

  case 23:
#line 297 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 24:
#line 302 "parse.y"
    {
	  /* last statement is else */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;

  case 25:
#line 310 "parse.y"
    {
	  /* alias */
	  yyval.t = mk_type(yyvsp[-3].s, NULL, NULL, NULL, yyvsp[-1].s);
        }
    break;

  case 26:
#line 316 "parse.y"
    {
	  /* array */
	  yyval.t = mk_type(yyvsp[-6].s, yyvsp[-3].r.start, yyvsp[-3].r.end, NULL, yyvsp[-1].s);
        }
    break;

  case 27:
#line 322 "parse.y"
    {
	  /* enumerated type */
	  yyval.t = mk_type(yyvsp[-5].s, NULL, NULL, yyvsp[-2].t, NULL);
        }
    break;

  case 28:
#line 330 "parse.y"
    {
 	  yyval.t = mk_head(yyvsp[0].s, NULL);
     	}
    break;

  case 29:
#line 335 "parse.y"
    {
 	  yyval.t = mk_head(yyvsp[-3].s, yyvsp[-1].t);
     	}
    break;

  case 30:
#line 342 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 31:
#line 347 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 32:
#line 354 "parse.y"
    {	  
	  yyval.t = mk_arg(yyvsp[-3].s, yyvsp[-1].d, yyvsp[0].s);
        }
    break;

  case 33:
#line 360 "parse.y"
    { yyval.d = dir_in; }
    break;

  case 34:
#line 361 "parse.y"
    { yyval.d = dir_inout; }
    break;

  case 35:
#line 362 "parse.y"
    { yyval.d = dir_out; }
    break;

  case 36:
#line 367 "parse.y"
    {
	  yyval.t = mk_body(yyvsp[-3].t, yyvsp[-1].t);
	}
    break;

  case 37:
#line 374 "parse.y"
    {
	  yyval.t = NULL;
	}
    break;

  case 38:
#line 379 "parse.y"
    {
          yyval.t = yyvsp[0].t;
	}
    break;

  case 39:
#line 386 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 40:
#line 391 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 41:
#line 398 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-3].s, false, yyvsp[-1].s, NULL, NULL);
        }
    break;

  case 42:
#line 403 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-5].s, false, yyvsp[-3].s, NULL, yyvsp[-1].t);
        }
    break;

  case 43:
#line 408 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-5].s, false, yyvsp[-3].s, yyvsp[-1].t, NULL);
        }
    break;

  case 44:
#line 413 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-7].s, false, yyvsp[-5].s, yyvsp[-3].t, yyvsp[-1].t);
        }
    break;

  case 45:
#line 418 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-6].s, true, yyvsp[-3].s, yyvsp[-1].t, NULL);
        }
    break;

  case 46:
#line 425 "parse.y"
    {
	  yyval.t = NULL;
	}
    break;

  case 47:
#line 430 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
	}
    break;

  case 48:
#line 437 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 49:
#line 442 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 50:
#line 449 "parse.y"
    {
	  yyval.t = mk_assembly(strdup("  nop"));
	}
    break;

  case 51:
#line 454 "parse.y"
    {
	  yyval.t = mk_label(yyvsp[0].s);
	}
    break;

  case 52:
#line 459 "parse.y"
    {
	  yyval.t = mk_goto(yyvsp[-1].s);
	}
    break;

  case 53:
#line 464 "parse.y"
    {
	  yyval.t = mk_return(yyvsp[-1].t);
	}
    break;

  case 54:
#line 469 "parse.y"
    {
	  yyval.t = mk_assembly(yyvsp[-1].s);
	}
    break;

  case 55:
#line 474 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-5].t, yyvsp[-3].t, NULL);
	}
    break;

  case 56:
#line 479 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-6].t, yyvsp[-4].t, yyvsp[-3].t);
	}
    break;

  case 57:
#line 484 "parse.y"
    {
          gp_linked_list *new = gp_list_make();

          new->prev = case_stack;
          case_stack = new;

	  case_ident = mk_symbol(yyvsp[0].s, NULL);
          gp_list_annotate(case_stack, case_ident);
	}
    break;

  case 58:
#line 494 "parse.y"
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

  case 59:
#line 505 "parse.y"
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

  case 60:
#line 524 "parse.y"
    {
	  yyval.t= mk_loop(NULL, yyvsp[-1].t, NULL, yyvsp[0].t);	
	}
    break;

  case 61:
#line 529 "parse.y"
    {
	  yyval.t= mk_loop(NULL, NULL, NULL, yyvsp[0].t);
	}
    break;

  case 62:
#line 534 "parse.y"
    {
	  yyval.t = yyvsp[-1].t;
	}
    break;

  case 63:
#line 541 "parse.y"
    {
	  /* last statement is elsif */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;

  case 64:
#line 547 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 65:
#line 552 "parse.y"
    {
	  /* last statement is else */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;

  case 66:
#line 560 "parse.y"
    {
	  /* last statement is elsif equivalent */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;

  case 67:
#line 566 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 68:
#line 571 "parse.y"
    {
	  /* last statement is else equivalent */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;

  case 69:
#line 579 "parse.y"
    {
	  yyval.t = mk_binop(op_eq, case_ident, yyvsp[0].t);
	}
    break;

  case 70:
#line 584 "parse.y"
    {
	  yyval.t = mk_binop(op_lor, mk_binop(op_eq, case_ident, yyvsp[-2].t), yyvsp[0].t);
	}
    break;

  case 71:
#line 591 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;

  case 72:
#line 596 "parse.y"
    {
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;

  case 73:
#line 603 "parse.y"
    {
	  yyval.t = yyvsp[-3].t;
	}
    break;

  case 74:
#line 610 "parse.y"
    {
	  yyval.r.start = yyvsp[-2].t;
	  yyval.r.end = yyvsp[0].t;
        }
    break;

  case 75:
#line 618 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 76:
#line 623 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 79:
#line 634 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 80:
#line 639 "parse.y"
    { yyval.o = op_assign; }
    break;

  case 82:
#line 645 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 83:
#line 650 "parse.y"
    { yyval.o = op_land; }
    break;

  case 84:
#line 651 "parse.y"
    { yyval.o = op_lor; }
    break;

  case 86:
#line 657 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 87:
#line 662 "parse.y"
    { yyval.o = op_and; }
    break;

  case 88:
#line 663 "parse.y"
    { yyval.o = op_or; }
    break;

  case 89:
#line 664 "parse.y"
    { yyval.o = op_xor; }
    break;

  case 91:
#line 670 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 92:
#line 675 "parse.y"
    { yyval.o = op_lt; }
    break;

  case 93:
#line 676 "parse.y"
    { yyval.o = op_gt; }
    break;

  case 94:
#line 677 "parse.y"
    { yyval.o = op_eq; }
    break;

  case 95:
#line 678 "parse.y"
    { yyval.o = op_ne; }
    break;

  case 96:
#line 679 "parse.y"
    { yyval.o = op_gte; }
    break;

  case 97:
#line 680 "parse.y"
    { yyval.o = op_lte; }
    break;

  case 99:
#line 686 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 100:
#line 691 "parse.y"
    { yyval.o = op_lsh; }
    break;

  case 101:
#line 692 "parse.y"
    { yyval.o = op_rsh; }
    break;

  case 103:
#line 698 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 104:
#line 703 "parse.y"
    { yyval.o = op_add; }
    break;

  case 105:
#line 704 "parse.y"
    { yyval.o = op_sub; }
    break;

  case 107:
#line 710 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 108:
#line 715 "parse.y"
    { yyval.o = op_mult; }
    break;

  case 109:
#line 716 "parse.y"
    { yyval.o = op_div; }
    break;

  case 110:
#line 717 "parse.y"
    { yyval.o = op_mod; }
    break;

  case 112:
#line 723 "parse.y"
    {
	  yyval.t = mk_unop(yyvsp[-1].o, yyvsp[0].t);
	}
    break;

  case 113:
#line 728 "parse.y"
    { yyval.o = op_neg; }
    break;

  case 114:
#line 729 "parse.y"
    { yyval.o = op_not; }
    break;

  case 115:
#line 730 "parse.y"
    { yyval.o = op_com; }
    break;

  case 116:
#line 731 "parse.y"
    { yyval.o = op_add; }
    break;

  case 117:
#line 735 "parse.y"
    {
	  yyval.t = mk_string(yyvsp[0].s);
        }
    break;

  case 118:
#line 740 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;

  case 119:
#line 745 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[-3].s, yyvsp[-1].t);
        }
    break;

  case 120:
#line 750 "parse.y"
    {
	  /* function or procedure call */
	  yyval.t = mk_call(yyvsp[-2].s, NULL);
	}
    break;

  case 121:
#line 756 "parse.y"
    {
	  /* function or procedure call with arguments */
	  yyval.t = mk_call(yyvsp[-3].s, yyvsp[-1].t);
	}
    break;

  case 122:
#line 762 "parse.y"
    {
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;

  case 123:
#line 767 "parse.y"
    {
	  yyval.t = yyvsp[-1].t;
	}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2135 "parse.c"

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


#line 772 "parse.y"



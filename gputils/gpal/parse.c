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
     ARRAY = 258,
     CASE = 259,
     CONSTANT = 260,
     BEGIN_TOK = 261,
     ELSE = 262,
     ELSIF = 263,
     END = 264,
     FOR = 265,
     FUNCTION = 266,
     GOTO = 267,
     IF = 268,
     IN = 269,
     INOUT = 270,
     IS = 271,
     LOOP = 272,
     MODULE = 273,
     NULL_TOK = 274,
     OF = 275,
     OTHERS = 276,
     PRAGMA = 277,
     PROCEDURE = 278,
     PUBLIC = 279,
     RETURN = 280,
     THEN = 281,
     TO = 282,
     TYPE = 283,
     OUT = 284,
     WHEN = 285,
     WHILE = 286,
     WITH = 287,
     ASM = 288,
     IDENT = 289,
     LABEL = 290,
     NUMBER = 291,
     STRING = 292,
     LSH = 293,
     RSH = 294,
     ARROW = 295,
     GREATER_EQUAL = 296,
     LESS_EQUAL = 297,
     EQUAL = 298,
     NOT_EQUAL = 299,
     LOGICAL_AND = 300,
     LOGICAL_OR = 301
   };
#endif
#define ARRAY 258
#define CASE 259
#define CONSTANT 260
#define BEGIN_TOK 261
#define ELSE 262
#define ELSIF 263
#define END 264
#define FOR 265
#define FUNCTION 266
#define GOTO 267
#define IF 268
#define IN 269
#define INOUT 270
#define IS 271
#define LOOP 272
#define MODULE 273
#define NULL_TOK 274
#define OF 275
#define OTHERS 276
#define PRAGMA 277
#define PROCEDURE 278
#define PUBLIC 279
#define RETURN 280
#define THEN 281
#define TO 282
#define TYPE 283
#define OUT 284
#define WHEN 285
#define WHILE 286
#define WITH 287
#define ASM 288
#define IDENT 289
#define LABEL 290
#define NUMBER 291
#define STRING 292
#define LSH 293
#define RSH 294
#define ARROW 295
#define GREATER_EQUAL 296
#define LESS_EQUAL 297
#define EQUAL 298
#define NOT_EQUAL 299
#define LOGICAL_AND 300
#define LOGICAL_OR 301




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
#line 59 "parse.y"
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
#line 236 "parse.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 248 "parse.c"

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
#define YYLAST   234

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  68
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  41
/* YYNRULES -- Number of rules. */
#define YYNRULES  109
/* YYNRULES -- Number of states. */
#define YYNSTATES  223

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   301

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      62,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    60,     2,     2,     2,    59,    49,     2,
      63,    64,    57,    55,    65,    56,     2,    58,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    39,    38,
      47,    54,    48,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    66,     2,    67,    51,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    50,     2,    61,     2,     2,     2,
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
      35,    36,    37,    40,    41,    42,    43,    44,    45,    46,
      52,    53
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     7,    11,    15,    23,    31,    33,
      36,    40,    42,    44,    50,    54,    62,    68,    74,    83,
      91,    93,    98,   100,   104,   109,   111,   113,   115,   121,
     122,   124,   126,   129,   134,   141,   149,   150,   152,   154,
     157,   160,   162,   166,   170,   173,   181,   190,   191,   200,
     206,   210,   212,   215,   220,   226,   229,   234,   240,   245,
     251,   255,   257,   261,   263,   265,   269,   271,   273,   277,
     279,   281,   283,   287,   289,   291,   293,   295,   299,   301,
     303,   305,   307,   309,   311,   313,   317,   319,   321,   323,
     327,   329,   331,   333,   337,   339,   341,   343,   345,   348,
     350,   352,   354,   356,   358,   360,   365,   369,   374,   376
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      69,     0,    -1,    -1,    69,    70,    -1,    69,     1,    62,
      -1,    32,    34,    38,    -1,    18,    34,    16,    71,     9,
      18,    38,    -1,    24,    34,    16,    71,     9,    24,    38,
      -1,    72,    -1,    72,    71,    -1,    22,    91,    38,    -1,
      73,    -1,    81,    -1,    23,    74,    78,    23,    38,    -1,
      23,    74,    38,    -1,    11,    74,    25,    34,    78,    11,
      38,    -1,    11,    74,    25,    34,    38,    -1,    28,    34,
      16,    34,    38,    -1,    28,    34,    16,     3,    89,    20,
      34,    38,    -1,    28,    34,    16,    63,    90,    64,    38,
      -1,    34,    -1,    34,    63,    75,    64,    -1,    76,    -1,
      76,    65,    75,    -1,    34,    39,    77,    34,    -1,    14,
      -1,    15,    -1,    29,    -1,    16,    79,     6,    82,     9,
      -1,    -1,    80,    -1,    81,    -1,    81,    80,    -1,    34,
      39,    34,    38,    -1,    34,    39,    34,    54,    91,    38,
      -1,    34,    39,     5,    34,    54,    91,    38,    -1,    -1,
      83,    -1,    84,    -1,    84,    83,    -1,    19,    38,    -1,
      35,    -1,    12,    34,    38,    -1,    25,    91,    38,    -1,
      33,    38,    -1,    13,    91,    26,    82,     9,    13,    38,
      -1,    13,    91,    26,    82,    86,     9,    13,    38,    -1,
      -1,     4,    34,    85,    16,    87,     9,     4,    38,    -1,
      10,    34,    14,    89,    88,    -1,    31,    91,    88,    -1,
      88,    -1,    91,    38,    -1,     8,    91,    26,    82,    -1,
       8,    91,    26,    82,    86,    -1,     7,    82,    -1,    30,
     108,    42,    82,    -1,    30,   108,    42,    82,    87,    -1,
      30,    21,    42,    82,    -1,    17,    82,     9,    17,    38,
      -1,    91,    27,    91,    -1,    91,    -1,    91,    65,    90,
      -1,    92,    -1,    94,    -1,    92,    93,    94,    -1,    54,
      -1,    96,    -1,    94,    95,    96,    -1,    52,    -1,    53,
      -1,    98,    -1,    96,    97,    98,    -1,    49,    -1,    50,
      -1,    51,    -1,   100,    -1,    98,    99,   100,    -1,    47,
      -1,    48,    -1,    45,    -1,    46,    -1,    43,    -1,    44,
      -1,   102,    -1,   100,   101,   102,    -1,    40,    -1,    41,
      -1,   104,    -1,   102,   103,   104,    -1,    55,    -1,    56,
      -1,   106,    -1,   104,   105,   106,    -1,    57,    -1,    58,
      -1,    59,    -1,   108,    -1,   107,   108,    -1,    56,    -1,
      60,    -1,    61,    -1,    55,    -1,    37,    -1,    34,    -1,
      34,    66,    91,    67,    -1,    34,    63,    64,    -1,    34,
      63,    90,    64,    -1,    36,    -1,    63,    91,    64,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   159,   159,   162,   164,   168,   173,   178,   185,   190,
     197,   202,   207,   212,   217,   222,   227,   234,   240,   246,
     254,   259,   266,   271,   278,   285,   286,   287,   291,   299,
     303,   310,   315,   322,   327,   332,   340,   344,   351,   356,
     363,   368,   373,   378,   383,   388,   393,   399,   398,   411,
     430,   435,   440,   447,   453,   458,   466,   472,   477,   485,
     492,   500,   505,   511,   514,   516,   522,   525,   527,   533,
     534,   537,   539,   545,   546,   547,   550,   552,   558,   559,
     560,   561,   562,   563,   566,   568,   574,   575,   578,   580,
     586,   587,   590,   592,   598,   599,   600,   603,   605,   611,
     612,   613,   614,   617,   622,   627,   632,   638,   644,   649
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"array\"", "\"case\"", "\"constant\"",
  "\"begin\"", "\"else\"", "\"elsif\"", "\"end\"", "\"for\"",
  "\"function\"", "\"goto\"", "\"if\"", "\"in\"", "\"inout\"", "\"is\"",
  "\"loop\"", "\"module\"", "\"null\"", "\"of\"", "\"others\"",
  "\"pragma\"", "\"procedure\"", "\"public\"", "\"return\"", "\"then\"",
  "\"to\"", "\"type\"", "\"out\"", "\"when\"", "\"while\"", "\"with\"",
  "\"asm\"", "\"symbol\"", "\"label\"", "\"number\"", "\"string\"", "';'",
  "':'", "\"<<\"", "\">>\"", "\"=>\"", "\">=\"", "\"<=\"", "\"==\"",
  "\"!=\"", "'<'", "'>'", "'&'", "'|'", "'^'", "\"&&\"", "\"||\"", "'='",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", "'\\n'", "'('", "')'",
  "','", "'['", "']'", "$accept", "program", "entity", "element_list",
  "element", "type", "head", "arg_list", "arg", "arg_direction", "body",
  "decl_start", "decl_block", "decl", "statement_start", "statement_block",
  "statement", "@1", "if_body", "case_body", "loop_statement", "range",
  "parameter_list", "expr", "e8", "e8op", "e7", "e7op", "e6", "e6op", "e5",
  "e5op", "e4", "e4op", "e3", "e3op", "e2", "e2op", "e1", "e1op", "e0", 0
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
     285,   286,   287,   288,   289,   290,   291,   292,    59,    58,
     293,   294,   295,   296,   297,   298,   299,    60,    62,    38,
     124,    94,   300,   301,    61,    43,    45,    42,    47,    37,
      33,   126,    10,    40,    41,    44,    91,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    68,    69,    69,    69,    70,    70,    70,    71,    71,
      72,    72,    72,    72,    72,    72,    72,    73,    73,    73,
      74,    74,    75,    75,    76,    77,    77,    77,    78,    79,
      79,    80,    80,    81,    81,    81,    82,    82,    83,    83,
      84,    84,    84,    84,    84,    84,    84,    85,    84,    84,
      84,    84,    84,    86,    86,    86,    87,    87,    87,    88,
      89,    90,    90,    91,    92,    92,    93,    94,    94,    95,
      95,    96,    96,    97,    97,    97,    98,    98,    99,    99,
      99,    99,    99,    99,   100,   100,   101,   101,   102,   102,
     103,   103,   104,   104,   105,   105,   105,   106,   106,   107,
     107,   107,   107,   108,   108,   108,   108,   108,   108,   108
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     3,     3,     7,     7,     1,     2,
       3,     1,     1,     5,     3,     7,     5,     5,     8,     7,
       1,     4,     1,     3,     4,     1,     1,     1,     5,     0,
       1,     1,     2,     4,     6,     7,     0,     1,     1,     2,
       2,     1,     3,     3,     2,     7,     8,     0,     8,     5,
       3,     1,     2,     4,     5,     2,     4,     5,     4,     5,
       3,     1,     3,     1,     1,     3,     1,     1,     3,     1,
       1,     1,     3,     1,     1,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     3,
       1,     1,     1,     3,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     4,     3,     4,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     3,     4,     0,
       0,     0,     0,     0,     5,     0,     0,     0,     0,     0,
       0,     8,    11,    12,     0,    20,     0,   104,   108,   103,
     102,    99,   100,   101,     0,     0,    63,    64,    67,    71,
      76,    84,    88,    92,     0,    97,     0,     0,     0,     0,
       9,     0,     0,     0,     0,     0,     0,    10,    66,     0,
      69,    70,     0,    73,    74,    75,     0,    82,    83,    80,
      81,    78,    79,     0,    86,    87,     0,    90,    91,     0,
      94,    95,    96,     0,    98,    29,    14,     0,     0,     0,
       0,     0,     0,     0,     0,    22,     0,   106,     0,    61,
       0,   109,    65,    68,    72,    77,    85,    89,    93,     0,
      30,    31,     0,     0,     0,     0,     0,    33,     0,     6,
       7,     0,    21,     0,    16,     0,   107,     0,   105,    36,
      32,    13,     0,     0,    17,     0,     0,     0,    25,    26,
      27,     0,    23,     0,    62,     0,     0,     0,     0,    36,
       0,     0,     0,     0,    41,     0,    37,    38,    51,     0,
       0,     0,     0,     0,    34,    24,    15,    47,     0,     0,
       0,     0,    40,     0,     0,    44,    28,    39,    52,     0,
      60,    19,    35,     0,     0,    42,    36,     0,    43,    50,
      18,     0,     0,     0,     0,     0,     0,    49,    36,     0,
       0,     0,    59,     0,     0,     0,    55,     0,     0,     0,
      36,    36,     0,    36,    45,     0,    58,    56,    48,    53,
      46,    57,    54
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     7,    20,    21,    22,    26,    94,    95,   141,
      87,   109,   110,    23,   155,   156,   157,   183,   201,   196,
     158,   132,    98,   159,    36,    59,    37,    62,    38,    66,
      39,    73,    40,    76,    41,    79,    42,    83,    43,    44,
      45
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -163
static const short yypact[] =
{
    -163,    50,  -163,   -46,   -12,    22,    29,  -163,  -163,     9,
      57,    28,    95,    95,  -163,    36,    35,    36,    47,    44,
      84,    95,  -163,  -163,    85,    37,    76,   -42,  -163,  -163,
    -163,  -163,  -163,  -163,    35,    71,    56,    -8,    54,    94,
       7,    -2,    67,  -163,   -28,  -163,    -9,    98,     8,   101,
    -163,    97,    88,    93,    52,    35,    64,  -163,  -163,    35,
    -163,  -163,    35,  -163,  -163,  -163,    35,  -163,  -163,  -163,
    -163,  -163,  -163,    35,  -163,  -163,    35,  -163,  -163,    35,
    -163,  -163,  -163,    35,  -163,    96,  -163,   108,    -1,    99,
     -27,   105,   106,   110,    70,    86,    -6,  -163,    89,    90,
      83,  -163,    -8,    54,    94,     7,    -2,    67,  -163,   148,
    -163,    96,   118,    35,   119,    35,   104,  -163,    35,  -163,
    -163,    63,  -163,    88,  -163,   149,  -163,    35,  -163,    24,
    -163,  -163,   141,   132,  -163,   100,    35,   124,  -163,  -163,
    -163,   129,  -163,   127,  -163,   133,   135,   136,    35,    24,
     128,    35,    35,   134,  -163,   162,  -163,    24,  -163,   137,
     140,    35,   138,   142,  -163,  -163,  -163,  -163,   159,   143,
     151,   169,  -163,   144,   167,  -163,  -163,  -163,  -163,   147,
    -163,  -163,  -163,   163,    35,  -163,    24,   170,  -163,  -163,
    -163,   156,   167,   139,   150,   -17,   180,  -163,    24,    35,
     177,   182,  -163,   152,   153,   188,  -163,   171,   155,   183,
      24,    24,   160,    24,  -163,   161,  -163,   156,  -163,    68,
    -163,  -163,  -163
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -163,  -163,  -163,     2,  -163,  -163,   184,    77,  -163,  -163,
     107,  -163,    91,   -80,  -146,    48,  -163,  -163,   -15,   -11,
    -162,    23,  -101,   -16,  -163,  -163,   154,  -163,   146,  -163,
     145,  -163,   157,  -163,   158,  -163,   130,  -163,   131,  -163,
     -43
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      35,    84,   113,   171,   203,   111,    27,    85,    28,    29,
      85,   117,   189,    89,   135,    24,     8,    27,    56,    28,
      29,    54,     9,    50,    55,    12,   144,   118,   145,    86,
     197,   111,   124,   114,   146,    34,   147,   148,    99,   100,
     193,   149,    90,   150,    60,    61,    34,    74,    75,   151,
       2,     3,   206,    77,    78,   152,    10,   153,    27,   154,
      28,    29,   115,    11,   216,   217,    14,   219,     4,    27,
      25,    28,    29,    13,     5,   198,   199,   138,   139,    30,
      31,    47,     6,    48,    32,    33,    27,    34,    28,    29,
      30,    31,   140,    49,    51,    32,    33,   133,    34,    99,
      52,    53,   137,    63,    64,    65,    15,    30,    31,    57,
      58,    99,    32,    33,    88,    34,    97,    16,    17,    91,
     163,    92,    93,    18,    80,    81,    82,    96,   101,    19,
      19,   112,   170,   116,   122,   173,   174,    67,    68,    69,
      70,    71,    72,   119,   120,   180,   198,   199,   200,   121,
     128,   123,   204,   126,   129,   127,   131,   134,   136,   161,
     143,   160,   164,   165,   162,   166,   172,   167,   133,   168,
     169,   176,   175,   184,   179,   178,   181,   186,   187,   191,
     182,   185,   188,   207,   149,   190,   195,   194,   202,   205,
     208,   209,   212,   214,   210,   211,   215,   213,   218,   220,
     142,    46,   130,   125,   222,   177,   221,   192,   103,   107,
       0,   104,     0,   102,   108,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     105,     0,     0,     0,   106
};

static const short yycheck[] =
{
      16,    44,     3,   149,    21,    85,    34,    16,    36,    37,
      16,    38,   174,     5,   115,    13,    62,    34,    34,    36,
      37,    63,    34,    21,    66,    16,   127,    54,     4,    38,
     192,   111,    38,    34,    10,    63,    12,    13,    54,    55,
     186,    17,    34,    19,    52,    53,    63,    40,    41,    25,
       0,     1,   198,    55,    56,    31,    34,    33,    34,    35,
      36,    37,    63,    34,   210,   211,    38,   213,    18,    34,
      34,    36,    37,    16,    24,     7,     8,    14,    15,    55,
      56,    34,    32,    39,    60,    61,    34,    63,    36,    37,
      55,    56,    29,     9,     9,    60,    61,   113,    63,   115,
      63,    25,   118,    49,    50,    51,    11,    55,    56,    38,
      54,   127,    60,    61,    16,    63,    64,    22,    23,    18,
     136,    24,    34,    28,    57,    58,    59,    34,    64,    34,
      34,    23,   148,    34,    64,   151,   152,    43,    44,    45,
      46,    47,    48,    38,    38,   161,     7,     8,     9,    39,
      67,    65,   195,    64,     6,    65,    38,    38,    54,    27,
      11,    20,    38,    34,    64,    38,    38,    34,   184,    34,
      34,     9,    38,    14,    34,    38,    38,    26,     9,    16,
      38,    38,    38,   199,    17,    38,    30,    17,    38,     9,
      13,     9,     4,    38,    42,    42,    13,    26,    38,    38,
     123,    17,   111,    96,   219,   157,   217,   184,    62,    79,
      -1,    66,    -1,    59,    83,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    -1,    -1,    -1,    76
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    69,     0,     1,    18,    24,    32,    70,    62,    34,
      34,    34,    16,    16,    38,    11,    22,    23,    28,    34,
      71,    72,    73,    81,    71,    34,    74,    34,    36,    37,
      55,    56,    60,    61,    63,    91,    92,    94,    96,    98,
     100,   102,   104,   106,   107,   108,    74,    34,    39,     9,
      71,     9,    63,    25,    63,    66,    91,    38,    54,    93,
      52,    53,    95,    49,    50,    51,    97,    43,    44,    45,
      46,    47,    48,    99,    40,    41,   101,    55,    56,   103,
      57,    58,    59,   105,   108,    16,    38,    78,    16,     5,
      34,    18,    24,    34,    75,    76,    34,    64,    90,    91,
      91,    64,    94,    96,    98,   100,   102,   104,   106,    79,
      80,    81,    23,     3,    34,    63,    34,    38,    54,    38,
      38,    39,    64,    65,    38,    78,    64,    65,    67,     6,
      80,    38,    89,    91,    38,    90,    54,    91,    14,    15,
      29,    77,    75,    11,    90,     4,    10,    12,    13,    17,
      19,    25,    31,    33,    35,    82,    83,    84,    88,    91,
      20,    27,    64,    91,    38,    34,    38,    34,    34,    34,
      91,    82,    38,    91,    91,    38,     9,    83,    38,    34,
      91,    38,    38,    85,    14,    38,    26,     9,    38,    88,
      38,    16,    89,    82,    17,    30,    87,    88,     7,     8,
       9,    86,    38,    21,   108,     9,    82,    91,    13,     9,
      42,    42,     4,    26,    38,    13,    82,    82,    38,    82,
      38,    87,    86
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
#line 169 "parse.y"
    {
	  open_src(yyvsp[-1].s, source_with);
	}
    break;

  case 6:
#line 174 "parse.y"
    {
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;

  case 7:
#line 179 "parse.y"
    {
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;

  case 8:
#line 186 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 9:
#line 191 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 10:
#line 198 "parse.y"
    {
	  yyval.t = mk_pragma(yyvsp[-1].t);
	}
    break;

  case 11:
#line 203 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
        }
    break;

  case 12:
#line 208 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
        }
    break;

  case 13:
#line 213 "parse.y"
    { 
	  yyval.t = mk_proc(yyvsp[-3].t, yyvsp[-2].t);
     	}
    break;

  case 14:
#line 218 "parse.y"
    { 
	  yyval.t = mk_proc(yyvsp[-1].t, NULL);
	}
    break;

  case 15:
#line 223 "parse.y"
    { 
	  yyval.t = mk_func(yyvsp[-5].t, yyvsp[-3].s, yyvsp[-2].t);
	}
    break;

  case 16:
#line 228 "parse.y"
    { 
	  yyval.t = mk_func(yyvsp[-3].t, yyvsp[-1].s, NULL);
	}
    break;

  case 17:
#line 235 "parse.y"
    {
	  /* alias */
	  yyval.t = mk_type(yyvsp[-3].s, NULL, NULL, NULL, yyvsp[-1].s);
        }
    break;

  case 18:
#line 241 "parse.y"
    {
	  /* array */
	  yyval.t = mk_type(yyvsp[-6].s, yyvsp[-3].r.start, yyvsp[-3].r.end, NULL, yyvsp[-1].s);
        }
    break;

  case 19:
#line 247 "parse.y"
    {
	  /* enumerated type */
	  yyval.t = mk_type(yyvsp[-5].s, NULL, NULL, yyvsp[-2].t, NULL);
        }
    break;

  case 20:
#line 255 "parse.y"
    {
 	  yyval.t = mk_head(yyvsp[0].s, NULL);
     	}
    break;

  case 21:
#line 260 "parse.y"
    {
 	  yyval.t = mk_head(yyvsp[-3].s, yyvsp[-1].t);
     	}
    break;

  case 22:
#line 267 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 23:
#line 272 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 24:
#line 279 "parse.y"
    {	  
	  yyval.t = mk_arg(yyvsp[-3].s, yyvsp[-1].d, yyvsp[0].s);
        }
    break;

  case 25:
#line 285 "parse.y"
    { yyval.d = dir_in; }
    break;

  case 26:
#line 286 "parse.y"
    { yyval.d = dir_inout; }
    break;

  case 27:
#line 287 "parse.y"
    { yyval.d = dir_out; }
    break;

  case 28:
#line 292 "parse.y"
    {
	  yyval.t = mk_body(yyvsp[-3].t, yyvsp[-1].t);
	}
    break;

  case 29:
#line 299 "parse.y"
    {
	  yyval.t = NULL;
	}
    break;

  case 30:
#line 304 "parse.y"
    {
          yyval.t = yyvsp[0].t;
	}
    break;

  case 31:
#line 311 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 32:
#line 316 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 33:
#line 323 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-3].s, false, yyvsp[-1].s, NULL);
        }
    break;

  case 34:
#line 328 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-5].s, false, yyvsp[-3].s, yyvsp[-1].t);
        }
    break;

  case 35:
#line 333 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-6].s, true, yyvsp[-3].s, yyvsp[-1].t);
        }
    break;

  case 36:
#line 340 "parse.y"
    {
	  yyval.t = NULL;
	}
    break;

  case 37:
#line 345 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
	}
    break;

  case 38:
#line 352 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 39:
#line 357 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 40:
#line 364 "parse.y"
    {
	  yyval.t = mk_assembly(strdup("  nop"));
	}
    break;

  case 41:
#line 369 "parse.y"
    {
	  yyval.t = mk_label(yyvsp[0].s);
	}
    break;

  case 42:
#line 374 "parse.y"
    {
	  yyval.t = mk_goto(yyvsp[-1].s);
	}
    break;

  case 43:
#line 379 "parse.y"
    {
	  yyval.t = mk_return(yyvsp[-1].t);
	}
    break;

  case 44:
#line 384 "parse.y"
    {
	  yyval.t = mk_assembly(yyvsp[-1].s);
	}
    break;

  case 45:
#line 389 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-5].t, yyvsp[-3].t, NULL);
	}
    break;

  case 46:
#line 394 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-6].t, yyvsp[-4].t, yyvsp[-3].t);
	}
    break;

  case 47:
#line 399 "parse.y"
    {
	  if (case_ident) {
	    yyerror("nested case statements are not yet supported");
	  }
	  case_ident = mk_symbol(yyvsp[0].s, NULL);
	}
    break;

  case 48:
#line 406 "parse.y"
    {
	  yyval.t = yyvsp[-3].t;
	  case_ident = NULL;
	}
    break;

  case 49:
#line 412 "parse.y"
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

  case 50:
#line 431 "parse.y"
    {
	  yyval.t= mk_loop(NULL, yyvsp[-1].t, NULL, yyvsp[0].t);	
	}
    break;

  case 51:
#line 436 "parse.y"
    {
	  yyval.t= mk_loop(NULL, NULL, NULL, yyvsp[0].t);
	}
    break;

  case 52:
#line 441 "parse.y"
    {
	  yyval.t = yyvsp[-1].t;
	}
    break;

  case 53:
#line 448 "parse.y"
    {
	  /* last statement is elsif */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;

  case 54:
#line 454 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 55:
#line 459 "parse.y"
    {
	  /* last statement is else */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;

  case 56:
#line 467 "parse.y"
    {
	  /* last statement is elsif equivalent */
	  yyval.t = mk_cond(mk_binop(op_eq, case_ident, yyvsp[-2].t), yyvsp[0].t, NULL);
	}
    break;

  case 57:
#line 473 "parse.y"
    {
	  yyval.t = mk_cond(mk_binop(op_eq, case_ident, yyvsp[-3].t), yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 58:
#line 478 "parse.y"
    {
	  /* last statement is else equivalent */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;

  case 59:
#line 486 "parse.y"
    {
	  yyval.t = yyvsp[-3].t;
	}
    break;

  case 60:
#line 493 "parse.y"
    {
	  yyval.r.start = yyvsp[-2].t;
	  yyval.r.end = yyvsp[0].t;
        }
    break;

  case 61:
#line 501 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 62:
#line 506 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 65:
#line 517 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 66:
#line 522 "parse.y"
    { yyval.o = op_assign; }
    break;

  case 68:
#line 528 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 69:
#line 533 "parse.y"
    { yyval.o = op_land; }
    break;

  case 70:
#line 534 "parse.y"
    { yyval.o = op_lor; }
    break;

  case 72:
#line 540 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 73:
#line 545 "parse.y"
    { yyval.o = op_and; }
    break;

  case 74:
#line 546 "parse.y"
    { yyval.o = op_or; }
    break;

  case 75:
#line 547 "parse.y"
    { yyval.o = op_xor; }
    break;

  case 77:
#line 553 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 78:
#line 558 "parse.y"
    { yyval.o = op_lt; }
    break;

  case 79:
#line 559 "parse.y"
    { yyval.o = op_gt; }
    break;

  case 80:
#line 560 "parse.y"
    { yyval.o = op_eq; }
    break;

  case 81:
#line 561 "parse.y"
    { yyval.o = op_ne; }
    break;

  case 82:
#line 562 "parse.y"
    { yyval.o = op_gte; }
    break;

  case 83:
#line 563 "parse.y"
    { yyval.o = op_lte; }
    break;

  case 85:
#line 569 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 86:
#line 574 "parse.y"
    { yyval.o = op_lsh; }
    break;

  case 87:
#line 575 "parse.y"
    { yyval.o = op_rsh; }
    break;

  case 89:
#line 581 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 90:
#line 586 "parse.y"
    { yyval.o = op_add; }
    break;

  case 91:
#line 587 "parse.y"
    { yyval.o = op_sub; }
    break;

  case 93:
#line 593 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 94:
#line 598 "parse.y"
    { yyval.o = op_mult; }
    break;

  case 95:
#line 599 "parse.y"
    { yyval.o = op_div; }
    break;

  case 96:
#line 600 "parse.y"
    { yyval.o = op_mod; }
    break;

  case 98:
#line 606 "parse.y"
    {
	  yyval.t = mk_unop(yyvsp[-1].o, yyvsp[0].t);
	}
    break;

  case 99:
#line 611 "parse.y"
    { yyval.o = op_neg; }
    break;

  case 100:
#line 612 "parse.y"
    { yyval.o = op_not; }
    break;

  case 101:
#line 613 "parse.y"
    { yyval.o = op_com; }
    break;

  case 102:
#line 614 "parse.y"
    { yyval.o = op_add; }
    break;

  case 103:
#line 618 "parse.y"
    {
	  yyval.t = mk_string(yyvsp[0].s);
        }
    break;

  case 104:
#line 623 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;

  case 105:
#line 628 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[-3].s, yyvsp[-1].t);
        }
    break;

  case 106:
#line 633 "parse.y"
    {
	  /* function or procedure call */
	  yyval.t = mk_call(yyvsp[-2].s, NULL);
	}
    break;

  case 107:
#line 639 "parse.y"
    {
	  /* function or procedure call with arguments */
	  yyval.t = mk_call(yyvsp[-3].s, yyvsp[-1].t);
	}
    break;

  case 108:
#line 645 "parse.y"
    {
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;

  case 109:
#line 650 "parse.y"
    {
	  yyval.t = yyvsp[-1].t;
	}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 1986 "parse.c"

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


#line 655 "parse.y"



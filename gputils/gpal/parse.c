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
     CASE = 260,
     CONSTANT = 261,
     BEGIN_TOK = 262,
     ELSE = 263,
     ELSIF = 264,
     END = 265,
     FOR = 266,
     FUNCTION = 267,
     GOTO = 268,
     IF = 269,
     IN = 270,
     INOUT = 271,
     IS = 272,
     LOOP = 273,
     MODULE = 274,
     NULL_TOK = 275,
     OF = 276,
     OTHERS = 277,
     PRAGMA = 278,
     PROCEDURE = 279,
     PUBLIC = 280,
     RETURN = 281,
     THEN = 282,
     TO = 283,
     TYPE = 284,
     OUT = 285,
     WHEN = 286,
     WHILE = 287,
     WITH = 288,
     ASM = 289,
     IDENT = 290,
     LABEL = 291,
     NUMBER = 292,
     STRING = 293,
     LSH = 294,
     RSH = 295,
     ARROW = 296,
     GREATER_EQUAL = 297,
     LESS_EQUAL = 298,
     EQUAL = 299,
     NOT_EQUAL = 300,
     LOGICAL_AND = 301,
     LOGICAL_OR = 302
   };
#endif
#define ALIAS 258
#define ARRAY 259
#define CASE 260
#define CONSTANT 261
#define BEGIN_TOK 262
#define ELSE 263
#define ELSIF 264
#define END 265
#define FOR 266
#define FUNCTION 267
#define GOTO 268
#define IF 269
#define IN 270
#define INOUT 271
#define IS 272
#define LOOP 273
#define MODULE 274
#define NULL_TOK 275
#define OF 276
#define OTHERS 277
#define PRAGMA 278
#define PROCEDURE 279
#define PUBLIC 280
#define RETURN 281
#define THEN 282
#define TO 283
#define TYPE 284
#define OUT 285
#define WHEN 286
#define WHILE 287
#define WITH 288
#define ASM 289
#define IDENT 290
#define LABEL 291
#define NUMBER 292
#define STRING 293
#define LSH 294
#define RSH 295
#define ARROW 296
#define GREATER_EQUAL 297
#define LESS_EQUAL 298
#define EQUAL 299
#define NOT_EQUAL 300
#define LOGICAL_AND 301
#define LOGICAL_OR 302




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
#line 239 "parse.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 251 "parse.c"

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
#define YYLAST   240

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  69
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  43
/* YYNRULES -- Number of rules. */
#define YYNRULES  114
/* YYNRULES -- Number of states. */
#define YYNSTATES  232

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   302

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      63,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    61,     2,     2,     2,    60,    50,     2,
      64,    65,    58,    56,    66,    57,     2,    59,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    40,    39,
      48,    55,    49,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    67,     2,    68,    52,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    51,     2,    62,     2,     2,     2,
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
      35,    36,    37,    38,    41,    42,    43,    44,    45,    46,
      47,    53,    54
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     7,    11,    15,    23,    31,    33,
      36,    40,    42,    44,    49,    55,    59,    67,    73,    79,
      88,    96,    98,   103,   105,   109,   114,   116,   118,   120,
     126,   127,   129,   131,   134,   139,   146,   154,   155,   157,
     159,   162,   165,   167,   171,   175,   178,   186,   195,   196,
     205,   211,   215,   217,   220,   225,   231,   234,   239,   245,
     250,   252,   256,   258,   260,   266,   270,   272,   276,   278,
     280,   284,   286,   288,   292,   294,   296,   298,   302,   304,
     306,   308,   310,   314,   316,   318,   320,   322,   324,   326,
     328,   332,   334,   336,   338,   342,   344,   346,   348,   352,
     354,   356,   358,   360,   363,   365,   367,   369,   371,   373,
     375,   380,   384,   389,   391
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      70,     0,    -1,    -1,    70,    71,    -1,    70,     1,    63,
      -1,    33,    35,    39,    -1,    19,    35,    17,    72,    10,
      19,    39,    -1,    25,    35,    17,    72,    10,    25,    39,
      -1,    73,    -1,    73,    72,    -1,    23,    94,    39,    -1,
      74,    -1,    82,    -1,     3,    35,    35,    39,    -1,    24,
      75,    79,    24,    39,    -1,    24,    75,    39,    -1,    12,
      75,    26,    35,    79,    12,    39,    -1,    12,    75,    26,
      35,    39,    -1,    29,    35,    17,    35,    39,    -1,    29,
      35,    17,     4,    92,    21,    35,    39,    -1,    29,    35,
      17,    64,    93,    65,    39,    -1,    35,    -1,    35,    64,
      76,    65,    -1,    77,    -1,    77,    66,    76,    -1,    35,
      40,    78,    35,    -1,    15,    -1,    16,    -1,    30,    -1,
      17,    80,     7,    83,    10,    -1,    -1,    81,    -1,    82,
      -1,    82,    81,    -1,    35,    40,    35,    39,    -1,    35,
      40,    35,    55,    94,    39,    -1,    35,    40,     6,    35,
      55,    94,    39,    -1,    -1,    84,    -1,    85,    -1,    85,
      84,    -1,    20,    39,    -1,    36,    -1,    13,    35,    39,
      -1,    26,    94,    39,    -1,    34,    39,    -1,    14,    94,
      27,    83,    10,    14,    39,    -1,    14,    94,    27,    83,
      87,    10,    14,    39,    -1,    -1,     5,    35,    86,    17,
      88,    10,     5,    39,    -1,    11,    35,    15,    92,    91,
      -1,    32,    94,    91,    -1,    91,    -1,    94,    39,    -1,
       9,    94,    27,    83,    -1,     9,    94,    27,    83,    87,
      -1,     8,    83,    -1,    31,    89,    43,    83,    -1,    31,
      89,    43,    83,    88,    -1,    31,    22,    43,    83,    -1,
      90,    -1,    90,    51,    89,    -1,    35,    -1,    37,    -1,
      18,    83,    10,    18,    39,    -1,    94,    28,    94,    -1,
      94,    -1,    94,    66,    93,    -1,    95,    -1,    97,    -1,
      95,    96,    97,    -1,    55,    -1,    99,    -1,    97,    98,
      99,    -1,    53,    -1,    54,    -1,   101,    -1,    99,   100,
     101,    -1,    50,    -1,    51,    -1,    52,    -1,   103,    -1,
     101,   102,   103,    -1,    48,    -1,    49,    -1,    46,    -1,
      47,    -1,    44,    -1,    45,    -1,   105,    -1,   103,   104,
     105,    -1,    41,    -1,    42,    -1,   107,    -1,   105,   106,
     107,    -1,    56,    -1,    57,    -1,   109,    -1,   107,   108,
     109,    -1,    58,    -1,    59,    -1,    60,    -1,   111,    -1,
     110,   111,    -1,    57,    -1,    61,    -1,    62,    -1,    56,
      -1,    38,    -1,    35,    -1,    35,    67,    94,    68,    -1,
      35,    64,    65,    -1,    35,    64,    93,    65,    -1,    37,
      -1,    64,    94,    65,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   185,   185,   188,   190,   194,   199,   204,   211,   216,
     223,   228,   233,   238,   243,   248,   253,   258,   265,   271,
     277,   285,   290,   297,   302,   309,   316,   317,   318,   322,
     330,   334,   341,   346,   353,   358,   363,   371,   375,   382,
     387,   394,   399,   404,   409,   414,   419,   424,   430,   429,
     450,   469,   474,   479,   486,   492,   497,   505,   511,   516,
     524,   529,   536,   541,   548,   555,   563,   568,   574,   577,
     579,   585,   588,   590,   596,   597,   600,   602,   608,   609,
     610,   613,   615,   621,   622,   623,   624,   625,   626,   629,
     631,   637,   638,   641,   643,   649,   650,   653,   655,   661,
     662,   663,   666,   668,   674,   675,   676,   677,   680,   685,
     690,   695,   701,   707,   712
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"alias\"", "\"array\"", "\"case\"",
  "\"constant\"", "\"begin\"", "\"else\"", "\"elsif\"", "\"end\"",
  "\"for\"", "\"function\"", "\"goto\"", "\"if\"", "\"in\"", "\"inout\"",
  "\"is\"", "\"loop\"", "\"module\"", "\"null\"", "\"of\"", "\"others\"",
  "\"pragma\"", "\"procedure\"", "\"public\"", "\"return\"", "\"then\"",
  "\"to\"", "\"type\"", "\"out\"", "\"when\"", "\"while\"", "\"with\"",
  "\"asm\"", "\"symbol\"", "\"label\"", "\"number\"", "\"string\"", "';'",
  "':'", "\"<<\"", "\">>\"", "\"=>\"", "\">=\"", "\"<=\"", "\"==\"",
  "\"!=\"", "'<'", "'>'", "'&'", "'|'", "'^'", "\"&&\"", "\"||\"", "'='",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", "'\\n'", "'('", "')'",
  "','", "'['", "']'", "$accept", "program", "entity", "element_list",
  "element", "type", "head", "arg_list", "arg", "arg_direction", "body",
  "decl_start", "decl_block", "decl", "statement_start", "statement_block",
  "statement", "@1", "if_body", "case_body", "case_element_list",
  "case_element", "loop_statement", "range", "parameter_list", "expr",
  "e8", "e8op", "e7", "e7op", "e6", "e6op", "e5", "e5op", "e4", "e4op",
  "e3", "e3op", "e2", "e2op", "e1", "e1op", "e0", 0
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
     285,   286,   287,   288,   289,   290,   291,   292,   293,    59,
      58,   294,   295,   296,   297,   298,   299,   300,    60,    62,
      38,   124,    94,   301,   302,    61,    43,    45,    42,    47,
      37,    33,   126,    10,    40,    41,    44,    91,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    69,    70,    70,    70,    71,    71,    71,    72,    72,
      73,    73,    73,    73,    73,    73,    73,    73,    74,    74,
      74,    75,    75,    76,    76,    77,    78,    78,    78,    79,
      80,    80,    81,    81,    82,    82,    82,    83,    83,    84,
      84,    85,    85,    85,    85,    85,    85,    85,    86,    85,
      85,    85,    85,    85,    87,    87,    87,    88,    88,    88,
      89,    89,    90,    90,    91,    92,    93,    93,    94,    95,
      95,    96,    97,    97,    98,    98,    99,    99,   100,   100,
     100,   101,   101,   102,   102,   102,   102,   102,   102,   103,
     103,   104,   104,   105,   105,   106,   106,   107,   107,   108,
     108,   108,   109,   109,   110,   110,   110,   110,   111,   111,
     111,   111,   111,   111,   111
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     3,     3,     7,     7,     1,     2,
       3,     1,     1,     4,     5,     3,     7,     5,     5,     8,
       7,     1,     4,     1,     3,     4,     1,     1,     1,     5,
       0,     1,     1,     2,     4,     6,     7,     0,     1,     1,
       2,     2,     1,     3,     3,     2,     7,     8,     0,     8,
       5,     3,     1,     2,     4,     5,     2,     4,     5,     4,
       1,     3,     1,     1,     5,     3,     1,     3,     1,     1,
       3,     1,     1,     3,     1,     1,     1,     3,     1,     1,
       1,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     3,     1,     1,     1,     3,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       4,     3,     4,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     3,     4,     0,
       0,     0,     0,     0,     5,     0,     0,     0,     0,     0,
       0,     0,     8,    11,    12,     0,     0,    21,     0,   109,
     113,   108,   107,   104,   105,   106,     0,     0,    68,    69,
      72,    76,    81,    89,    93,    97,     0,   102,     0,     0,
       0,     0,     9,     0,     0,     0,     0,     0,     0,     0,
      10,    71,     0,    74,    75,     0,    78,    79,    80,     0,
      87,    88,    85,    86,    83,    84,     0,    91,    92,     0,
      95,    96,     0,    99,   100,   101,     0,   103,    30,    15,
       0,     0,     0,     0,     0,     0,    13,     0,     0,    23,
       0,   111,     0,    66,     0,   114,    70,    73,    77,    82,
      90,    94,    98,     0,    31,    32,     0,     0,     0,     0,
       0,    34,     0,     6,     7,     0,    22,     0,    17,     0,
     112,     0,   110,    37,    33,    14,     0,     0,    18,     0,
       0,     0,    26,    27,    28,     0,    24,     0,    67,     0,
       0,     0,     0,    37,     0,     0,     0,     0,    42,     0,
      38,    39,    52,     0,     0,     0,     0,     0,    35,    25,
      16,    48,     0,     0,     0,     0,    41,     0,     0,    45,
      29,    40,    53,     0,    65,    20,    36,     0,     0,    43,
      37,     0,    44,    51,    19,     0,     0,     0,     0,     0,
       0,    50,    37,     0,     0,     0,    64,     0,    62,    63,
       0,    60,     0,    56,     0,     0,     0,    37,    37,     0,
       0,    37,    46,     0,    59,    57,    61,    49,    54,    47,
      58,    55
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     7,    21,    22,    23,    28,    98,    99,   145,
      90,   113,   114,    24,   159,   160,   161,   187,   205,   200,
     210,   211,   162,   136,   102,   163,    38,    62,    39,    65,
      40,    69,    41,    76,    42,    79,    43,    82,    44,    86,
      45,    46,    47
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -167
static const short yypact[] =
{
    -167,     9,  -167,   -43,    17,    29,    39,  -167,  -167,    59,
      61,    47,    58,    58,  -167,    53,    55,    54,    55,    62,
      68,    91,    58,  -167,  -167,   107,    96,    57,   106,   -49,
    -167,  -167,  -167,  -167,  -167,  -167,    54,    94,    79,   -40,
       8,    81,    13,    56,    25,  -167,   -32,  -167,   -13,   119,
       1,   118,  -167,   115,   102,   108,   109,    42,    54,    77,
    -167,  -167,    54,  -167,  -167,    54,  -167,  -167,  -167,    54,
    -167,  -167,  -167,  -167,  -167,  -167,    54,  -167,  -167,    54,
    -167,  -167,    54,  -167,  -167,  -167,    54,  -167,   110,  -167,
     122,    -2,   112,   -28,   113,   114,  -167,   116,    84,    85,
       0,  -167,    89,    92,    82,  -167,   -40,     8,    81,    13,
      56,    25,  -167,   148,  -167,   110,   120,    54,   121,    54,
     117,  -167,    54,  -167,  -167,    41,  -167,   108,  -167,   145,
    -167,    54,  -167,    11,  -167,  -167,   140,   134,  -167,    98,
      54,   125,  -167,  -167,  -167,   130,  -167,   127,  -167,   132,
     133,   135,    54,    11,   136,    54,    54,   137,  -167,   159,
    -167,    11,  -167,   138,   139,    54,   141,   142,  -167,  -167,
    -167,  -167,   158,   143,   151,   169,  -167,   144,   166,  -167,
    -167,  -167,  -167,   146,  -167,  -167,  -167,   170,    54,  -167,
      11,   171,  -167,  -167,  -167,   157,   166,    86,   152,    87,
     180,  -167,    11,    54,   178,   183,  -167,   153,  -167,  -167,
     154,   147,   189,  -167,   168,   160,   186,    11,    11,   -14,
     162,    11,  -167,   163,  -167,   157,  -167,  -167,   111,  -167,
    -167,  -167
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -167,  -167,  -167,    31,  -167,  -167,   185,    78,  -167,  -167,
     104,  -167,    93,   -80,  -152,    45,  -167,  -167,   -21,   -16,
      -9,  -167,  -166,    23,   -68,   -17,  -167,  -167,   150,  -167,
     149,  -167,   155,  -167,   156,  -167,   161,  -167,   131,  -167,
     129,  -167,   172
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      37,   175,   117,    29,    88,    30,    31,    92,   115,     2,
       3,   121,   193,    63,    64,    57,   149,    88,    58,    59,
       8,   208,   150,   209,   151,   152,    89,   122,     4,   153,
     201,   154,    36,   118,     5,   115,    93,   155,   197,   128,
     103,   104,     6,   156,    25,   157,    29,   158,    30,    31,
     213,   139,     9,    52,    77,    78,   142,   143,    66,    67,
      68,    15,   119,   148,    10,   224,   225,    32,    33,   228,
      16,   144,    34,    35,    11,    36,    12,    29,    13,    30,
      31,    17,    18,    83,    84,    85,    14,    19,    26,    29,
      27,    30,    31,    20,   202,   203,   204,    49,    32,    33,
     137,    51,   103,    34,    35,   141,    36,   101,    50,   207,
      32,    33,    80,    81,   103,    34,    35,    53,    36,   202,
     203,    55,   208,   167,   209,    70,    71,    72,    73,    74,
      75,    54,    56,    60,    61,   174,    91,    94,   177,   178,
      95,    96,   105,    97,   100,    20,   116,   120,   184,   126,
     132,   127,   123,   124,   130,   133,   125,   147,   131,   135,
     138,   164,   165,   166,   168,   169,   170,   171,   172,   180,
     173,   137,   140,   188,   183,   176,   179,   182,   190,   191,
     185,   186,   189,   192,   153,   194,   214,   195,   199,   198,
     212,   206,   215,   216,   220,   221,   217,   218,   219,   222,
     223,   227,   229,    48,   129,   146,   181,   231,   134,   230,
     226,   196,   106,   111,   107,   112,     0,     0,    87,     0,
       0,     0,     0,     0,   108,     0,     0,     0,     0,     0,
       0,     0,   109,     0,     0,     0,     0,     0,     0,     0,
     110
};

static const short yycheck[] =
{
      17,   153,     4,    35,    17,    37,    38,     6,    88,     0,
       1,    39,   178,    53,    54,    64,     5,    17,    67,    36,
      63,    35,    11,    37,    13,    14,    39,    55,    19,    18,
     196,    20,    64,    35,    25,   115,    35,    26,   190,    39,
      57,    58,    33,    32,    13,    34,    35,    36,    37,    38,
     202,   119,    35,    22,    41,    42,    15,    16,    50,    51,
      52,     3,    64,   131,    35,   217,   218,    56,    57,   221,
      12,    30,    61,    62,    35,    64,    17,    35,    17,    37,
      38,    23,    24,    58,    59,    60,    39,    29,    35,    35,
      35,    37,    38,    35,     8,     9,    10,    35,    56,    57,
     117,    10,   119,    61,    62,   122,    64,    65,    40,    22,
      56,    57,    56,    57,   131,    61,    62,    10,    64,     8,
       9,    64,    35,   140,    37,    44,    45,    46,    47,    48,
      49,    35,    26,    39,    55,   152,    17,    19,   155,   156,
      25,    39,    65,    35,    35,    35,    24,    35,   165,    65,
      68,    66,    39,    39,    65,     7,    40,    12,    66,    39,
      39,    21,    28,    65,    39,    35,    39,    35,    35,    10,
      35,   188,    55,    15,    35,    39,    39,    39,    27,    10,
      39,    39,    39,    39,    18,    39,   203,    17,    31,    18,
      10,    39,    14,    10,     5,    27,    43,    43,    51,    39,
      14,    39,    39,    18,   100,   127,   161,   228,   115,   225,
     219,   188,    62,    82,    65,    86,    -1,    -1,    46,    -1,
      -1,    -1,    -1,    -1,    69,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      79
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    70,     0,     1,    19,    25,    33,    71,    63,    35,
      35,    35,    17,    17,    39,     3,    12,    23,    24,    29,
      35,    72,    73,    74,    82,    72,    35,    35,    75,    35,
      37,    38,    56,    57,    61,    62,    64,    94,    95,    97,
      99,   101,   103,   105,   107,   109,   110,   111,    75,    35,
      40,    10,    72,    10,    35,    64,    26,    64,    67,    94,
      39,    55,    96,    53,    54,    98,    50,    51,    52,   100,
      44,    45,    46,    47,    48,    49,   102,    41,    42,   104,
      56,    57,   106,    58,    59,    60,   108,   111,    17,    39,
      79,    17,     6,    35,    19,    25,    39,    35,    76,    77,
      35,    65,    93,    94,    94,    65,    97,    99,   101,   103,
     105,   107,   109,    80,    81,    82,    24,     4,    35,    64,
      35,    39,    55,    39,    39,    40,    65,    66,    39,    79,
      65,    66,    68,     7,    81,    39,    92,    94,    39,    93,
      55,    94,    15,    16,    30,    78,    76,    12,    93,     5,
      11,    13,    14,    18,    20,    26,    32,    34,    36,    83,
      84,    85,    91,    94,    21,    28,    65,    94,    39,    35,
      39,    35,    35,    35,    94,    83,    39,    94,    94,    39,
      10,    84,    39,    35,    94,    39,    39,    86,    15,    39,
      27,    10,    39,    91,    39,    17,    92,    83,    18,    31,
      88,    91,     8,     9,    10,    87,    39,    22,    35,    37,
      89,    90,    10,    83,    94,    14,    10,    43,    43,    51,
       5,    27,    39,    14,    83,    83,    89,    39,    83,    39,
      88,    87
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
#line 195 "parse.y"
    {
	  open_src(yyvsp[-1].s, source_with);
	}
    break;

  case 6:
#line 200 "parse.y"
    {
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;

  case 7:
#line 205 "parse.y"
    {
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;

  case 8:
#line 212 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 9:
#line 217 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 10:
#line 224 "parse.y"
    {
	  yyval.t = mk_pragma(yyvsp[-1].t);
	}
    break;

  case 11:
#line 229 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
        }
    break;

  case 12:
#line 234 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
        }
    break;

  case 13:
#line 239 "parse.y"
    {
	  yyval.t = mk_alias(yyvsp[-2].s, yyvsp[-1].s);
        }
    break;

  case 14:
#line 244 "parse.y"
    { 
	  yyval.t = mk_proc(yyvsp[-3].t, yyvsp[-2].t);
     	}
    break;

  case 15:
#line 249 "parse.y"
    { 
	  yyval.t = mk_proc(yyvsp[-1].t, NULL);
	}
    break;

  case 16:
#line 254 "parse.y"
    { 
	  yyval.t = mk_func(yyvsp[-5].t, yyvsp[-3].s, yyvsp[-2].t);
	}
    break;

  case 17:
#line 259 "parse.y"
    { 
	  yyval.t = mk_func(yyvsp[-3].t, yyvsp[-1].s, NULL);
	}
    break;

  case 18:
#line 266 "parse.y"
    {
	  /* alias */
	  yyval.t = mk_type(yyvsp[-3].s, NULL, NULL, NULL, yyvsp[-1].s);
        }
    break;

  case 19:
#line 272 "parse.y"
    {
	  /* array */
	  yyval.t = mk_type(yyvsp[-6].s, yyvsp[-3].r.start, yyvsp[-3].r.end, NULL, yyvsp[-1].s);
        }
    break;

  case 20:
#line 278 "parse.y"
    {
	  /* enumerated type */
	  yyval.t = mk_type(yyvsp[-5].s, NULL, NULL, yyvsp[-2].t, NULL);
        }
    break;

  case 21:
#line 286 "parse.y"
    {
 	  yyval.t = mk_head(yyvsp[0].s, NULL);
     	}
    break;

  case 22:
#line 291 "parse.y"
    {
 	  yyval.t = mk_head(yyvsp[-3].s, yyvsp[-1].t);
     	}
    break;

  case 23:
#line 298 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 24:
#line 303 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 25:
#line 310 "parse.y"
    {	  
	  yyval.t = mk_arg(yyvsp[-3].s, yyvsp[-1].d, yyvsp[0].s);
        }
    break;

  case 26:
#line 316 "parse.y"
    { yyval.d = dir_in; }
    break;

  case 27:
#line 317 "parse.y"
    { yyval.d = dir_inout; }
    break;

  case 28:
#line 318 "parse.y"
    { yyval.d = dir_out; }
    break;

  case 29:
#line 323 "parse.y"
    {
	  yyval.t = mk_body(yyvsp[-3].t, yyvsp[-1].t);
	}
    break;

  case 30:
#line 330 "parse.y"
    {
	  yyval.t = NULL;
	}
    break;

  case 31:
#line 335 "parse.y"
    {
          yyval.t = yyvsp[0].t;
	}
    break;

  case 32:
#line 342 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 33:
#line 347 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 34:
#line 354 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-3].s, false, yyvsp[-1].s, NULL);
        }
    break;

  case 35:
#line 359 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-5].s, false, yyvsp[-3].s, yyvsp[-1].t);
        }
    break;

  case 36:
#line 364 "parse.y"
    { 
	  yyval.t = mk_decl(yyvsp[-6].s, true, yyvsp[-3].s, yyvsp[-1].t);
        }
    break;

  case 37:
#line 371 "parse.y"
    {
	  yyval.t = NULL;
	}
    break;

  case 38:
#line 376 "parse.y"
    {
	  yyval.t = yyvsp[0].t;
	}
    break;

  case 39:
#line 383 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 40:
#line 388 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 41:
#line 395 "parse.y"
    {
	  yyval.t = mk_assembly(strdup("  nop"));
	}
    break;

  case 42:
#line 400 "parse.y"
    {
	  yyval.t = mk_label(yyvsp[0].s);
	}
    break;

  case 43:
#line 405 "parse.y"
    {
	  yyval.t = mk_goto(yyvsp[-1].s);
	}
    break;

  case 44:
#line 410 "parse.y"
    {
	  yyval.t = mk_return(yyvsp[-1].t);
	}
    break;

  case 45:
#line 415 "parse.y"
    {
	  yyval.t = mk_assembly(yyvsp[-1].s);
	}
    break;

  case 46:
#line 420 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-5].t, yyvsp[-3].t, NULL);
	}
    break;

  case 47:
#line 425 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-6].t, yyvsp[-4].t, yyvsp[-3].t);
	}
    break;

  case 48:
#line 430 "parse.y"
    {
          gp_linked_list *new = gp_list_make();

          new->prev = case_stack;
          case_stack = new;

	  case_ident = mk_symbol(yyvsp[0].s, NULL);
          gp_list_annotate(case_stack, case_ident);
	}
    break;

  case 49:
#line 440 "parse.y"
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

  case 50:
#line 451 "parse.y"
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

  case 51:
#line 470 "parse.y"
    {
	  yyval.t= mk_loop(NULL, yyvsp[-1].t, NULL, yyvsp[0].t);	
	}
    break;

  case 52:
#line 475 "parse.y"
    {
	  yyval.t= mk_loop(NULL, NULL, NULL, yyvsp[0].t);
	}
    break;

  case 53:
#line 480 "parse.y"
    {
	  yyval.t = yyvsp[-1].t;
	}
    break;

  case 54:
#line 487 "parse.y"
    {
	  /* last statement is elsif */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;

  case 55:
#line 493 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 56:
#line 498 "parse.y"
    {
	  /* last statement is else */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;

  case 57:
#line 506 "parse.y"
    {
	  /* last statement is elsif equivalent */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;

  case 58:
#line 512 "parse.y"
    {
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;

  case 59:
#line 517 "parse.y"
    {
	  /* last statement is else equivalent */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;

  case 60:
#line 525 "parse.y"
    {
	  yyval.t = mk_binop(op_eq, case_ident, yyvsp[0].t);
	}
    break;

  case 61:
#line 530 "parse.y"
    {
	  yyval.t = mk_binop(op_lor, mk_binop(op_eq, case_ident, yyvsp[-2].t), yyvsp[0].t);
	}
    break;

  case 62:
#line 537 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;

  case 63:
#line 542 "parse.y"
    {
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;

  case 64:
#line 549 "parse.y"
    {
	  yyval.t = yyvsp[-3].t;
	}
    break;

  case 65:
#line 556 "parse.y"
    {
	  yyval.r.start = yyvsp[-2].t;
	  yyval.r.end = yyvsp[0].t;
        }
    break;

  case 66:
#line 564 "parse.y"
    {
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;

  case 67:
#line 569 "parse.y"
    {
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 70:
#line 580 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 71:
#line 585 "parse.y"
    { yyval.o = op_assign; }
    break;

  case 73:
#line 591 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 74:
#line 596 "parse.y"
    { yyval.o = op_land; }
    break;

  case 75:
#line 597 "parse.y"
    { yyval.o = op_lor; }
    break;

  case 77:
#line 603 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 78:
#line 608 "parse.y"
    { yyval.o = op_and; }
    break;

  case 79:
#line 609 "parse.y"
    { yyval.o = op_or; }
    break;

  case 80:
#line 610 "parse.y"
    { yyval.o = op_xor; }
    break;

  case 82:
#line 616 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 83:
#line 621 "parse.y"
    { yyval.o = op_lt; }
    break;

  case 84:
#line 622 "parse.y"
    { yyval.o = op_gt; }
    break;

  case 85:
#line 623 "parse.y"
    { yyval.o = op_eq; }
    break;

  case 86:
#line 624 "parse.y"
    { yyval.o = op_ne; }
    break;

  case 87:
#line 625 "parse.y"
    { yyval.o = op_gte; }
    break;

  case 88:
#line 626 "parse.y"
    { yyval.o = op_lte; }
    break;

  case 90:
#line 632 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 91:
#line 637 "parse.y"
    { yyval.o = op_lsh; }
    break;

  case 92:
#line 638 "parse.y"
    { yyval.o = op_rsh; }
    break;

  case 94:
#line 644 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 95:
#line 649 "parse.y"
    { yyval.o = op_add; }
    break;

  case 96:
#line 650 "parse.y"
    { yyval.o = op_sub; }
    break;

  case 98:
#line 656 "parse.y"
    {
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;

  case 99:
#line 661 "parse.y"
    { yyval.o = op_mult; }
    break;

  case 100:
#line 662 "parse.y"
    { yyval.o = op_div; }
    break;

  case 101:
#line 663 "parse.y"
    { yyval.o = op_mod; }
    break;

  case 103:
#line 669 "parse.y"
    {
	  yyval.t = mk_unop(yyvsp[-1].o, yyvsp[0].t);
	}
    break;

  case 104:
#line 674 "parse.y"
    { yyval.o = op_neg; }
    break;

  case 105:
#line 675 "parse.y"
    { yyval.o = op_not; }
    break;

  case 106:
#line 676 "parse.y"
    { yyval.o = op_com; }
    break;

  case 107:
#line 677 "parse.y"
    { yyval.o = op_add; }
    break;

  case 108:
#line 681 "parse.y"
    {
	  yyval.t = mk_string(yyvsp[0].s);
        }
    break;

  case 109:
#line 686 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;

  case 110:
#line 691 "parse.y"
    {
	  yyval.t = mk_symbol(yyvsp[-3].s, yyvsp[-1].t);
        }
    break;

  case 111:
#line 696 "parse.y"
    {
	  /* function or procedure call */
	  yyval.t = mk_call(yyvsp[-2].s, NULL);
	}
    break;

  case 112:
#line 702 "parse.y"
    {
	  /* function or procedure call with arguments */
	  yyval.t = mk_call(yyvsp[-3].s, yyvsp[-1].t);
	}
    break;

  case 113:
#line 708 "parse.y"
    {
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;

  case 114:
#line 713 "parse.y"
    {
	  yyval.t = yyvsp[-1].t;
	}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2044 "parse.c"

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


#line 718 "parse.y"



/* A Bison parser, made from parse.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	ARRAY	257
# define	CASE	258
# define	BEGIN_TOK	259
# define	ELSE	260
# define	ELSIF	261
# define	END	262
# define	FOR	263
# define	FUNCTION	264
# define	IF	265
# define	IN	266
# define	INOUT	267
# define	IS	268
# define	LOOP	269
# define	MODULE	270
# define	NULL_TOK	271
# define	OF	272
# define	OTHERS	273
# define	PRAGMA	274
# define	PROCEDURE	275
# define	PUBLIC	276
# define	RETURN	277
# define	THEN	278
# define	TO	279
# define	TYPE	280
# define	OUT	281
# define	WHEN	282
# define	WHILE	283
# define	WITH	284
# define	ASM	285
# define	IDENT	286
# define	NUMBER	287
# define	STRING	288
# define	LSH	289
# define	RSH	290
# define	ARROW	291
# define	GREATER_EQUAL	292
# define	LESS_EQUAL	293
# define	EQUAL	294
# define	NOT_EQUAL	295
# define	LOGICAL_AND	296
# define	LOGICAL_OR	297

#line 1 "parse.y"

/* Parser
   Copyright (C) 2003 
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


#line 59 "parse.y"
#ifndef YYSTYPE
typedef union {
  int i;
  struct {
    tree *start;
    tree *end;
  } r;
  char *s;
  tree *t;
  enum node_dir d;
  enum node_op o;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 1
#endif



#define	YYFINAL		214
#define	YYFLAG		-32768
#define	YYNTBASE	65

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 297 ? yytranslate[x] : 105)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      59,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    57,     2,     2,     2,    56,    46,     2,
      60,    61,    54,    52,    62,    53,     2,    55,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    36,    35,
      44,    51,    45,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    63,     2,    64,    48,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    47,     2,    58,     2,     2,     2,
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
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    37,
      38,    39,    40,    41,    42,    43,    49,    50
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     8,    12,    20,    28,    30,    33,
      37,    39,    41,    47,    51,    59,    65,    71,    80,    88,
      90,    95,    97,   101,   106,   108,   110,   112,   118,   119,
     121,   123,   126,   131,   138,   139,   141,   143,   146,   149,
     153,   156,   164,   173,   174,   183,   189,   193,   195,   198,
     203,   209,   212,   217,   223,   228,   234,   238,   240,   244,
     246,   248,   252,   254,   256,   260,   262,   264,   266,   270,
     272,   274,   276,   278,   282,   284,   286,   288,   290,   292,
     294,   296,   300,   302,   304,   306,   310,   312,   314,   316,
     320,   322,   324,   326,   328,   331,   333,   335,   337,   339,
     341,   343,   348,   352,   357,   359
};
static const short yyrhs[] =
{
      -1,    65,    66,     0,    65,     1,    59,     0,    30,    32,
      35,     0,    16,    32,    14,    67,     8,    16,    35,     0,
      22,    32,    14,    67,     8,    22,    35,     0,    68,     0,
      68,    67,     0,    20,    87,    35,     0,    69,     0,    77,
       0,    21,    70,    74,    21,    35,     0,    21,    70,    35,
       0,    10,    70,    23,    32,    74,    10,    35,     0,    10,
      70,    23,    32,    35,     0,    26,    32,    14,    32,    35,
       0,    26,    32,    14,     3,    85,    18,    32,    35,     0,
      26,    32,    14,    60,    86,    61,    35,     0,    32,     0,
      32,    60,    71,    61,     0,    72,     0,    72,    62,    71,
       0,    32,    36,    73,    32,     0,    12,     0,    13,     0,
      27,     0,    14,    75,     5,    78,     8,     0,     0,    76,
       0,    77,     0,    77,    76,     0,    32,    36,    32,    35,
       0,    32,    36,    32,    51,    87,    35,     0,     0,    79,
       0,    80,     0,    80,    79,     0,    17,    35,     0,    23,
      87,    35,     0,    31,    35,     0,    11,    87,    24,    78,
       8,    11,    35,     0,    11,    87,    24,    78,    82,     8,
      11,    35,     0,     0,     4,    32,    81,    14,    83,     8,
       4,    35,     0,     9,    32,    12,    85,    84,     0,    29,
      87,    84,     0,    84,     0,    87,    35,     0,     7,    87,
      24,    78,     0,     7,    87,    24,    78,    82,     0,     6,
      78,     0,    28,   104,    39,    78,     0,    28,   104,    39,
      78,    83,     0,    28,    19,    39,    78,     0,    15,    78,
       8,    15,    35,     0,    87,    25,    87,     0,    87,     0,
      87,    62,    86,     0,    88,     0,    90,     0,    88,    89,
      90,     0,    51,     0,    92,     0,    90,    91,    92,     0,
      49,     0,    50,     0,    94,     0,    92,    93,    94,     0,
      46,     0,    47,     0,    48,     0,    96,     0,    94,    95,
      96,     0,    44,     0,    45,     0,    42,     0,    43,     0,
      40,     0,    41,     0,    98,     0,    96,    97,    98,     0,
      37,     0,    38,     0,   100,     0,    98,    99,   100,     0,
      52,     0,    53,     0,   102,     0,   100,   101,   102,     0,
      54,     0,    55,     0,    56,     0,   104,     0,   103,   104,
       0,    53,     0,    57,     0,    58,     0,    52,     0,    34,
       0,    32,     0,    32,    63,    87,    64,     0,    32,    60,
      61,     0,    32,    60,    86,    61,     0,    33,     0,    60,
      87,    61,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   156,   158,   160,   164,   169,   174,   181,   186,   193,
     198,   203,   208,   213,   218,   223,   230,   236,   242,   250,
     255,   262,   267,   274,   281,   283,   284,   287,   294,   299,
     306,   311,   318,   323,   330,   335,   342,   347,   354,   359,
     364,   369,   374,   379,   379,   392,   411,   416,   421,   428,
     434,   439,   447,   453,   458,   466,   473,   481,   486,   493,
     495,   497,   504,   506,   508,   515,   516,   518,   520,   527,
     528,   529,   531,   533,   540,   541,   542,   543,   544,   545,
     547,   549,   556,   557,   559,   561,   568,   569,   571,   573,
     580,   581,   582,   584,   586,   593,   594,   595,   596,   598,
     603,   608,   613,   619,   625,   630
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "\"array\"", "\"case\"", "\"begin\"", 
  "\"else\"", "\"elsif\"", "\"end\"", "\"for\"", "\"function\"", "\"if\"", 
  "\"in\"", "\"inout\"", "\"is\"", "\"loop\"", "\"module\"", "\"null\"", 
  "\"of\"", "\"others\"", "\"pragma\"", "\"procedure\"", "\"public\"", 
  "\"return\"", "\"then\"", "\"to\"", "\"type\"", "\"out\"", "\"when\"", 
  "\"while\"", "\"with\"", "ASM", "IDENT", "NUMBER", "STRING", "';'", 
  "':'", "\"<<\"", "\">>\"", "\"=>\"", "\">=\"", "\"<=\"", "\"==\"", 
  "\"!=\"", "'<'", "'>'", "'&'", "'|'", "'^'", "\"&&\"", "\"||\"", "'='", 
  "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", "'\\n'", "'('", "')'", 
  "','", "'['", "']'", "program", "entity", "element_list", "element", 
  "type", "head", "arg_list", "arg", "arg_direction", "body", 
  "decl_start", "decl_block", "decl", "statement_start", 
  "statement_block", "statement", "@1", "if_body", "case_body", 
  "loop_statement", "range", "parameter_list", "expr", "e8", "e8op", "e7", 
  "e7op", "e6", "e6op", "e5", "e5op", "e4", "e4op", "e3", "e3op", "e2", 
  "e2op", "e1", "e1op", "e0", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    65,    65,    65,    66,    66,    66,    67,    67,    68,
      68,    68,    68,    68,    68,    68,    69,    69,    69,    70,
      70,    71,    71,    72,    73,    73,    73,    74,    75,    75,
      76,    76,    77,    77,    78,    78,    79,    79,    80,    80,
      80,    80,    80,    81,    80,    80,    80,    80,    80,    82,
      82,    82,    83,    83,    83,    84,    85,    86,    86,    87,
      88,    88,    89,    90,    90,    91,    91,    92,    92,    93,
      93,    93,    94,    94,    95,    95,    95,    95,    95,    95,
      96,    96,    97,    97,    98,    98,    99,    99,   100,   100,
     101,   101,   101,   102,   102,   103,   103,   103,   103,   104,
     104,   104,   104,   104,   104,   104
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     3,     3,     7,     7,     1,     2,     3,
       1,     1,     5,     3,     7,     5,     5,     8,     7,     1,
       4,     1,     3,     4,     1,     1,     1,     5,     0,     1,
       1,     2,     4,     6,     0,     1,     1,     2,     2,     3,
       2,     7,     8,     0,     8,     5,     3,     1,     2,     4,
       5,     2,     4,     5,     4,     5,     3,     1,     3,     1,
       1,     3,     1,     1,     3,     1,     1,     1,     3,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     1,     1,     3,     1,     1,     1,     3,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     4,     3,     4,     1,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     0,     0,     0,     0,     0,     2,     3,     0,     0,
       0,     0,     0,     4,     0,     0,     0,     0,     0,     0,
       7,    10,    11,     0,    19,     0,   100,   104,    99,    98,
      95,    96,    97,     0,     0,    59,    60,    63,    67,    72,
      80,    84,    88,     0,    93,     0,     0,     0,     0,     8,
       0,     0,     0,     0,     0,     0,     9,    62,     0,    65,
      66,     0,    69,    70,    71,     0,    78,    79,    76,    77,
      74,    75,     0,    82,    83,     0,    86,    87,     0,    90,
      91,    92,     0,    94,    28,    13,     0,     0,     0,     0,
       0,     0,     0,    21,     0,   102,     0,    57,     0,   105,
      61,    64,    68,    73,    81,    85,    89,     0,    29,    30,
       0,     0,     0,     0,    32,     0,     5,     6,     0,    20,
       0,    15,     0,   103,     0,   101,    34,    31,    12,     0,
       0,    16,     0,     0,    24,    25,    26,     0,    22,     0,
      58,     0,     0,     0,    34,     0,     0,     0,     0,     0,
      35,    36,    47,     0,     0,     0,     0,    33,    23,    14,
      43,     0,     0,     0,    38,     0,     0,    40,    27,    37,
      48,     0,    56,    18,     0,     0,    34,     0,    39,    46,
      17,     0,     0,     0,     0,     0,     0,    45,    34,     0,
       0,     0,    55,     0,     0,     0,    51,     0,     0,     0,
      34,    34,     0,    34,    41,     0,    54,    52,    44,    49,
      42,    53,    50,     0,     0
};

static const short yydefgoto[] =
{
       1,     6,    19,    20,    21,    25,    92,    93,   137,    86,
     107,   108,    22,   149,   150,   151,   174,   191,   186,   152,
     129,    96,   153,    35,    58,    36,    61,    37,    65,    38,
      72,    39,    75,    40,    78,    41,    82,    42,    43,    44
};

static const short yypact[] =
{
  -32768,    14,   -47,   -12,    13,    16,-32768,-32768,    42,    55,
      37,    47,    47,-32768,    45,    60,    45,    57,    59,    66,
      47,-32768,-32768,    89,    39,    96,   -44,-32768,-32768,-32768,
  -32768,-32768,-32768,    60,    91,    76,    -7,    38,    61,    70,
      58,    67,-32768,   -23,-32768,   -10,   115,    98,   117,-32768,
     112,   103,   104,    30,    60,    77,-32768,-32768,    60,-32768,
  -32768,    60,-32768,-32768,-32768,    60,-32768,-32768,-32768,-32768,
  -32768,-32768,    60,-32768,-32768,    60,-32768,-32768,    60,-32768,
  -32768,-32768,    60,-32768,   105,-32768,   120,     0,   -28,   107,
     109,   110,    78,    83,    -9,-32768,    86,    87,    84,-32768,
      -7,    38,    61,    70,    58,    67,-32768,   145,-32768,   105,
     116,    60,   119,    60,-32768,    60,-32768,-32768,    53,-32768,
     103,-32768,   142,-32768,    60,-32768,    18,-32768,-32768,   135,
     130,-32768,    95,   122,-32768,-32768,-32768,   126,-32768,   124,
  -32768,   129,   131,    60,    18,   127,    60,    60,   132,   156,
  -32768,    18,-32768,   133,   134,    60,   136,-32768,-32768,-32768,
  -32768,   153,   146,   161,-32768,   137,   158,-32768,-32768,-32768,
  -32768,   140,-32768,-32768,   162,    60,    18,   163,-32768,-32768,
  -32768,   149,   158,   108,   144,    21,   172,-32768,    18,    60,
     170,   174,-32768,   147,   148,   179,-32768,   160,   150,   177,
      18,    18,   154,    18,-32768,   155,-32768,   149,-32768,   118,
  -32768,-32768,-32768,   191,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,     1,-32768,-32768,   176,    73,-32768,-32768,   100,
  -32768,    88,   -78,  -142,    44,-32768,-32768,   -13,    -8,  -158,
      23,   -96,   -15,-32768,-32768,   143,-32768,   139,-32768,   138,
  -32768,   141,-32768,   151,-32768,   128,-32768,   123,-32768,   -42
};


#define	YYLAST		226


static const short yytable[] =
{
      34,    83,   163,   111,    84,    84,   109,   114,   179,    26,
      27,    28,     7,    23,   213,     2,    53,   132,    55,    54,
       8,    49,   141,   115,   187,    85,   121,   142,   140,   143,
       3,   109,   112,   144,   183,   145,     4,    33,    97,    98,
     193,   146,    59,    60,     5,     9,   196,   147,    10,   148,
      26,    27,    28,    26,    27,    28,    11,    14,   206,   207,
     113,   209,    26,    27,    28,   134,   135,    15,    16,    12,
      29,    30,    13,    17,    48,    31,    32,    24,    33,    18,
     136,    33,    29,    30,    62,    63,    64,    31,    32,    46,
      33,    95,    26,    27,    28,    47,   130,    50,    97,    51,
     133,    66,    67,    68,    69,    70,    71,    73,    74,    97,
      76,    77,    29,    30,   188,   189,   190,    31,    32,    52,
      33,    79,    80,    81,   188,   189,    56,    57,   162,    87,
      88,   165,   166,    89,    90,    91,    94,    18,    99,   119,
     172,   110,   116,   194,   117,   120,   118,   123,   125,   124,
     126,   128,   139,   154,   131,   155,   156,   157,   158,   159,
     130,   160,   164,   161,   168,   175,   171,   167,   170,   177,
     176,   173,   178,   144,   197,   180,   181,   185,   184,   192,
     195,   198,   199,   202,   203,   204,   200,   201,   205,   208,
     210,   214,    45,   138,   122,   169,   212,   127,   182,   211,
     101,   100,     0,   102,     0,   106,   105,     0,     0,     0,
       0,     0,     0,   103,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   104
};

static const short yycheck[] =
{
      15,    43,   144,     3,    14,    14,    84,    35,   166,    32,
      33,    34,    59,    12,     0,     1,    60,   113,    33,    63,
      32,    20,     4,    51,   182,    35,    35,     9,   124,    11,
      16,   109,    32,    15,   176,    17,    22,    60,    53,    54,
      19,    23,    49,    50,    30,    32,   188,    29,    32,    31,
      32,    33,    34,    32,    33,    34,    14,    10,   200,   201,
      60,   203,    32,    33,    34,    12,    13,    20,    21,    14,
      52,    53,    35,    26,     8,    57,    58,    32,    60,    32,
      27,    60,    52,    53,    46,    47,    48,    57,    58,    32,
      60,    61,    32,    33,    34,    36,   111,     8,   113,    60,
     115,    40,    41,    42,    43,    44,    45,    37,    38,   124,
      52,    53,    52,    53,     6,     7,     8,    57,    58,    23,
      60,    54,    55,    56,     6,     7,    35,    51,   143,    14,
      32,   146,   147,    16,    22,    32,    32,    32,    61,    61,
     155,    21,    35,   185,    35,    62,    36,    61,    64,    62,
       5,    35,    10,    18,    35,    25,    61,    35,    32,    35,
     175,    32,    35,    32,     8,    12,    32,    35,    35,     8,
      24,    35,    35,    15,   189,    35,    14,    28,    15,    35,
       8,    11,     8,     4,    24,    35,    39,    39,    11,    35,
      35,     0,    16,   120,    94,   151,   209,   109,   175,   207,
      61,    58,    -1,    65,    -1,    82,    78,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    75
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

case 4:
#line 166 "parse.y"
{
	  open_src(yyvsp[-1].s, source_with);
	}
    break;
case 5:
#line 171 "parse.y"
{
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;
case 6:
#line 176 "parse.y"
{
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;
case 7:
#line 183 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 8:
#line 188 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 9:
#line 195 "parse.y"
{
	  yyval.t = mk_pragma(yyvsp[-1].t);
	}
    break;
case 10:
#line 200 "parse.y"
{
	  yyval.t = yyvsp[0].t;
        }
    break;
case 11:
#line 205 "parse.y"
{
	  yyval.t = yyvsp[0].t;
        }
    break;
case 12:
#line 210 "parse.y"
{ 
	  yyval.t = mk_proc(yyvsp[-3].t, yyvsp[-2].t);
     	}
    break;
case 13:
#line 215 "parse.y"
{ 
	  yyval.t = mk_proc(yyvsp[-1].t, NULL);
	}
    break;
case 14:
#line 220 "parse.y"
{ 
	  yyval.t = mk_func(yyvsp[-5].t, yyvsp[-3].s, yyvsp[-2].t);
	}
    break;
case 15:
#line 225 "parse.y"
{ 
	  yyval.t = mk_func(yyvsp[-3].t, yyvsp[-1].s, NULL);
	}
    break;
case 16:
#line 232 "parse.y"
{
	  /* alias */
	  yyval.t = mk_type(yyvsp[-3].s, NULL, NULL, NULL, yyvsp[-1].s);
        }
    break;
case 17:
#line 238 "parse.y"
{
	  /* array */
	  yyval.t = mk_type(yyvsp[-6].s, yyvsp[-3].r.start, yyvsp[-3].r.end, NULL, yyvsp[-1].s);
        }
    break;
case 18:
#line 244 "parse.y"
{
	  /* enumerated type */
	  yyval.t = mk_type(yyvsp[-5].s, NULL, NULL, yyvsp[-2].t, NULL);
        }
    break;
case 19:
#line 252 "parse.y"
{
 	  yyval.t = mk_head(yyvsp[0].s, NULL);
     	}
    break;
case 20:
#line 257 "parse.y"
{
 	  yyval.t = mk_head(yyvsp[-3].s, yyvsp[-1].t);
     	}
    break;
case 21:
#line 264 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 22:
#line 269 "parse.y"
{
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 23:
#line 276 "parse.y"
{	  
	  yyval.t = mk_arg(yyvsp[-3].s, yyvsp[-1].d, yyvsp[0].s);
        }
    break;
case 24:
#line 282 "parse.y"
{ yyval.d = dir_in; }
    break;
case 25:
#line 283 "parse.y"
{ yyval.d = dir_inout; }
    break;
case 26:
#line 284 "parse.y"
{ yyval.d = dir_out; }
    break;
case 27:
#line 289 "parse.y"
{
	  yyval.t = mk_body(yyvsp[-3].t, yyvsp[-1].t);
	}
    break;
case 28:
#line 296 "parse.y"
{
	  yyval.t = NULL;
	}
    break;
case 29:
#line 301 "parse.y"
{
          yyval.t = yyvsp[0].t;
	}
    break;
case 30:
#line 308 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 31:
#line 313 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 32:
#line 320 "parse.y"
{ 
	  yyval.t = mk_decl(yyvsp[-3].s, yyvsp[-1].s, NULL);
        }
    break;
case 33:
#line 325 "parse.y"
{ 
	  yyval.t = mk_decl(yyvsp[-5].s, yyvsp[-3].s, yyvsp[-1].t);
        }
    break;
case 34:
#line 332 "parse.y"
{
	  yyval.t = NULL;
	}
    break;
case 35:
#line 337 "parse.y"
{
	  yyval.t = yyvsp[0].t;
	}
    break;
case 36:
#line 344 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 37:
#line 349 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 38:
#line 356 "parse.y"
{
	  yyval.t = mk_assembly(strdup("  nop"));
	}
    break;
case 39:
#line 361 "parse.y"
{
	  yyval.t = mk_return(yyvsp[-1].t);
	}
    break;
case 40:
#line 366 "parse.y"
{
	  yyval.t = mk_assembly(yyvsp[-1].s);
	}
    break;
case 41:
#line 371 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-5].t, yyvsp[-3].t, NULL);
	}
    break;
case 42:
#line 376 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-6].t, yyvsp[-4].t, yyvsp[-3].t);
	}
    break;
case 43:
#line 381 "parse.y"
{
	  if (case_ident) {
	    yyerror("nested case statements are not yet supported");
	  }
	  case_ident = mk_symbol(yyvsp[0].s, NULL);
	}
    break;
case 44:
#line 388 "parse.y"
{
	  yyval.t = yyvsp[-3].t;
	  case_ident = NULL;
	}
    break;
case 45:
#line 394 "parse.y"
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
case 46:
#line 413 "parse.y"
{
	  yyval.t= mk_loop(NULL, yyvsp[-1].t, NULL, yyvsp[0].t);	
	}
    break;
case 47:
#line 418 "parse.y"
{
	  yyval.t= mk_loop(NULL, NULL, NULL, yyvsp[0].t);
	}
    break;
case 48:
#line 423 "parse.y"
{
	  yyval.t = yyvsp[-1].t;
	}
    break;
case 49:
#line 430 "parse.y"
{
	  /* last statement is elsif */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;
case 50:
#line 436 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 51:
#line 441 "parse.y"
{
	  /* last statement is else */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;
case 52:
#line 449 "parse.y"
{
	  /* last statement is elsif equivalent */
	  yyval.t = mk_cond(mk_binop(op_eq, case_ident, yyvsp[-2].t), yyvsp[0].t, NULL);
	}
    break;
case 53:
#line 455 "parse.y"
{
	  yyval.t = mk_cond(mk_binop(op_eq, case_ident, yyvsp[-3].t), yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 54:
#line 460 "parse.y"
{
	  /* last statement is else equivalent */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;
case 55:
#line 468 "parse.y"
{
	  yyval.t = yyvsp[-3].t;
	}
    break;
case 56:
#line 475 "parse.y"
{
	  yyval.r.start = yyvsp[-2].t;
	  yyval.r.end = yyvsp[0].t;
        }
    break;
case 57:
#line 483 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 58:
#line 488 "parse.y"
{
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 61:
#line 499 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 62:
#line 504 "parse.y"
{ yyval.o = op_assign; }
    break;
case 64:
#line 510 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 65:
#line 515 "parse.y"
{ yyval.o = op_land; }
    break;
case 66:
#line 516 "parse.y"
{ yyval.o = op_lor; }
    break;
case 68:
#line 522 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 69:
#line 527 "parse.y"
{ yyval.o = op_and; }
    break;
case 70:
#line 528 "parse.y"
{ yyval.o = op_or; }
    break;
case 71:
#line 529 "parse.y"
{ yyval.o = op_xor; }
    break;
case 73:
#line 535 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 74:
#line 540 "parse.y"
{ yyval.o = op_lt; }
    break;
case 75:
#line 541 "parse.y"
{ yyval.o = op_gt; }
    break;
case 76:
#line 542 "parse.y"
{ yyval.o = op_eq; }
    break;
case 77:
#line 543 "parse.y"
{ yyval.o = op_ne; }
    break;
case 78:
#line 544 "parse.y"
{ yyval.o = op_gte; }
    break;
case 79:
#line 545 "parse.y"
{ yyval.o = op_lte; }
    break;
case 81:
#line 551 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 82:
#line 556 "parse.y"
{ yyval.o = op_lsh; }
    break;
case 83:
#line 557 "parse.y"
{ yyval.o = op_rsh; }
    break;
case 85:
#line 563 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 86:
#line 568 "parse.y"
{ yyval.o = op_add; }
    break;
case 87:
#line 569 "parse.y"
{ yyval.o = op_sub; }
    break;
case 89:
#line 575 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 90:
#line 580 "parse.y"
{ yyval.o = op_mult; }
    break;
case 91:
#line 581 "parse.y"
{ yyval.o = op_div; }
    break;
case 92:
#line 582 "parse.y"
{ yyval.o = op_mod; }
    break;
case 94:
#line 588 "parse.y"
{
	  yyval.t = mk_unop(yyvsp[-1].o, yyvsp[0].t);
	}
    break;
case 95:
#line 593 "parse.y"
{ yyval.o = op_neg; }
    break;
case 96:
#line 594 "parse.y"
{ yyval.o = op_not; }
    break;
case 97:
#line 595 "parse.y"
{ yyval.o = op_com; }
    break;
case 98:
#line 596 "parse.y"
{ yyval.o = op_add; }
    break;
case 99:
#line 600 "parse.y"
{
	  yyval.t = mk_string(yyvsp[0].s);
        }
    break;
case 100:
#line 605 "parse.y"
{
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;
case 101:
#line 610 "parse.y"
{
	  yyval.t = mk_symbol(yyvsp[-3].s, yyvsp[-1].t);
        }
    break;
case 102:
#line 615 "parse.y"
{
	  /* function or procedure call */
	  yyval.t = mk_call(yyvsp[-2].s, NULL);
	}
    break;
case 103:
#line 621 "parse.y"
{
	  /* function or procedure call with arguments */
	  yyval.t = mk_call(yyvsp[-3].s, yyvsp[-1].t);
	}
    break;
case 104:
#line 627 "parse.y"
{
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;
case 105:
#line 632 "parse.y"
{
	  yyval.t = yyvsp[-1].t;
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
#line 637 "parse.y"


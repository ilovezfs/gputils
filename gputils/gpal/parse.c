/* A Bison parser, made from parse.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	ARRAY	257
# define	CASE	258
# define	CONSTANT_KEY	259
# define	BEGIN_KEY	260
# define	ELSE	261
# define	ELSIF	262
# define	END	263
# define	FOR	264
# define	FUNCTION_TOK	265
# define	IF	266
# define	IN	267
# define	INOUT	268
# define	IS	269
# define	LOOP	270
# define	MODULE	271
# define	NULL_TOK	272
# define	OF	273
# define	OTHERS	274
# define	PRAGMA	275
# define	PROCEDURE	276
# define	PUBLIC_STORAGE	277
# define	RETURN	278
# define	THEN	279
# define	TO	280
# define	TYPE	281
# define	OUT	282
# define	VARIABLE_KEY	283
# define	WHEN	284
# define	WHILE	285
# define	WITH	286
# define	ASM	287
# define	IDENT	288
# define	NUMBER	289
# define	STRING	290
# define	LSH	291
# define	RSH	292
# define	ARROW	293
# define	GREATER_EQUAL	294
# define	LESS_EQUAL	295
# define	EQUAL	296
# define	NOT_EQUAL	297
# define	LOGICAL_AND	298
# define	LOGICAL_OR	299

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

static tree *case_ident;


#line 57 "parse.y"
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
  enum node_key k;
  enum node_op o;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 1
#endif



#define	YYFINAL		224
#define	YYFLAG		-32768
#define	YYNTBASE	67

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 299 ? yytranslate[x] : 106)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      61,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    59,     2,     2,     2,    58,    48,     2,
      62,    63,    56,    54,    64,    55,     2,    57,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    38,    37,
      46,    53,    47,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    65,     2,    66,    50,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    49,     2,    60,     2,     2,     2,
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
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    39,    40,    41,    42,    43,    44,    45,    51,    52
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     8,    12,    19,    27,    34,    42,
      44,    47,    51,    53,    55,    61,    65,    73,    79,    85,
      94,   102,   104,   109,   111,   115,   120,   122,   124,   126,
     132,   137,   139,   142,   148,   156,   158,   160,   162,   165,
     168,   172,   175,   183,   192,   193,   202,   208,   212,   214,
     217,   222,   228,   231,   236,   242,   247,   253,   257,   259,
     263,   265,   267,   271,   273,   275,   279,   281,   283,   285,
     289,   291,   293,   295,   297,   301,   303,   305,   307,   309,
     311,   313,   315,   319,   321,   323,   325,   329,   331,   333,
     335,   339,   341,   343,   345,   347,   350,   352,   354,   356,
     358,   360,   362,   367,   371,   376,   378
};
static const short yyrhs[] =
{
      -1,    67,    68,     0,    67,     1,    61,     0,    32,    34,
      37,     0,    17,    34,    15,     9,    17,    37,     0,    17,
      34,    15,    69,     9,    17,    37,     0,    23,    34,    15,
       9,    23,    37,     0,    23,    34,    15,    69,     9,    23,
      37,     0,    70,     0,    70,    69,     0,    21,    88,    37,
       0,    71,     0,    78,     0,    22,    72,    76,    22,    37,
       0,    22,    72,    37,     0,    11,    72,    24,    34,    76,
      11,    37,     0,    11,    72,    24,    34,    37,     0,    27,
      34,    15,    34,    37,     0,    27,    34,    15,     3,    86,
      19,    34,    37,     0,    27,    34,    15,    62,    87,    63,
      37,     0,    34,     0,    34,    62,    73,    63,     0,    74,
       0,    74,    64,    73,     0,    34,    38,    75,    34,     0,
      13,     0,    14,     0,    28,     0,    15,    77,     6,    80,
       9,     0,    15,     6,    80,     9,     0,    78,     0,    78,
      77,     0,    79,    34,    38,    34,    37,     0,    79,    34,
      38,    34,    53,    88,    37,     0,     5,     0,    29,     0,
      81,     0,    81,    80,     0,    18,    37,     0,    24,    88,
      37,     0,    33,    37,     0,    12,    88,    25,    80,     9,
      12,    37,     0,    12,    88,    25,    80,    83,     9,    12,
      37,     0,     0,     4,    34,    82,    15,    84,     9,     4,
      37,     0,    10,    34,    13,    86,    85,     0,    31,    88,
      85,     0,    85,     0,    88,    37,     0,     8,    88,    25,
      80,     0,     8,    88,    25,    80,    83,     0,     7,    80,
       0,    30,   105,    41,    80,     0,    30,   105,    41,    80,
      84,     0,    30,    20,    41,    80,     0,    16,    80,     9,
      16,    37,     0,    88,    26,    88,     0,    88,     0,    88,
      64,    87,     0,    89,     0,    91,     0,    89,    90,    91,
       0,    53,     0,    93,     0,    91,    92,    93,     0,    51,
       0,    52,     0,    95,     0,    93,    94,    95,     0,    48,
       0,    49,     0,    50,     0,    97,     0,    95,    96,    97,
       0,    46,     0,    47,     0,    44,     0,    45,     0,    42,
       0,    43,     0,    99,     0,    97,    98,    99,     0,    39,
       0,    40,     0,   101,     0,    99,   100,   101,     0,    54,
       0,    55,     0,   103,     0,   101,   102,   103,     0,    56,
       0,    57,     0,    58,     0,   105,     0,   104,   105,     0,
      55,     0,    59,     0,    60,     0,    54,     0,    36,     0,
      34,     0,    34,    65,    88,    66,     0,    34,    62,    63,
       0,    34,    62,    87,    63,     0,    35,     0,    62,    88,
      63,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   120,   122,   124,   128,   133,   138,   143,   148,   155,
     160,   167,   172,   177,   182,   187,   192,   197,   204,   210,
     216,   224,   229,   236,   241,   248,   255,   257,   258,   261,
     266,   273,   278,   285,   291,   298,   303,   310,   315,   322,
     327,   332,   337,   342,   347,   347,   352,   371,   376,   381,
     388,   394,   399,   407,   413,   418,   426,   433,   441,   446,
     453,   455,   457,   464,   466,   468,   475,   476,   478,   480,
     487,   488,   489,   491,   493,   500,   501,   502,   503,   504,
     505,   507,   509,   516,   517,   519,   521,   528,   529,   531,
     533,   540,   541,   542,   544,   546,   553,   554,   555,   556,
     558,   563,   568,   573,   579,   585,   590
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "ARRAY", "CASE", "CONSTANT_KEY", "BEGIN_KEY", 
  "ELSE", "ELSIF", "END", "FOR", "FUNCTION_TOK", "IF", "IN", "INOUT", 
  "IS", "LOOP", "MODULE", "NULL_TOK", "OF", "OTHERS", "PRAGMA", 
  "PROCEDURE", "PUBLIC_STORAGE", "RETURN", "THEN", "TO", "TYPE", "OUT", 
  "VARIABLE_KEY", "WHEN", "WHILE", "WITH", "ASM", "IDENT", "NUMBER", 
  "STRING", "';'", "':'", "LSH", "RSH", "ARROW", "GREATER_EQUAL", 
  "LESS_EQUAL", "EQUAL", "NOT_EQUAL", "'<'", "'>'", "'&'", "'|'", "'^'", 
  "LOGICAL_AND", "LOGICAL_OR", "'='", "'+'", "'-'", "'*'", "'/'", "'%'", 
  "'!'", "'~'", "'\\n'", "'('", "')'", "','", "'['", "']'", "program", 
  "entity", "element_list", "element", "type", "head", "arg_list", "arg", 
  "arg_direction", "body", "decl_block", "decl", "decl_key", 
  "statement_block", "statement", "@1", "if_body", "case_body", 
  "loop_statement", "range", "parameter_list", "expr", "e8", "e8op", "e7", 
  "e7op", "e6", "e6op", "e5", "e5op", "e4", "e4op", "e3", "e3op", "e2", 
  "e2op", "e1", "e1op", "e0", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    67,    67,    67,    68,    68,    68,    68,    68,    69,
      69,    70,    70,    70,    70,    70,    70,    70,    71,    71,
      71,    72,    72,    73,    73,    74,    75,    75,    75,    76,
      76,    77,    77,    78,    78,    79,    79,    80,    80,    81,
      81,    81,    81,    81,    82,    81,    81,    81,    81,    81,
      83,    83,    83,    84,    84,    84,    85,    86,    87,    87,
      88,    89,    89,    90,    91,    91,    92,    92,    93,    93,
      94,    94,    94,    95,    95,    96,    96,    96,    96,    96,
      96,    97,    97,    98,    98,    99,    99,   100,   100,   101,
     101,   102,   102,   102,   103,   103,   104,   104,   104,   104,
     105,   105,   105,   105,   105,   105,   105
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     3,     3,     6,     7,     6,     7,     1,
       2,     3,     1,     1,     5,     3,     7,     5,     5,     8,
       7,     1,     4,     1,     3,     4,     1,     1,     1,     5,
       4,     1,     2,     5,     7,     1,     1,     1,     2,     2,
       3,     2,     7,     8,     0,     8,     5,     3,     1,     2,
       4,     5,     2,     4,     5,     4,     5,     3,     1,     3,
       1,     1,     3,     1,     1,     3,     1,     1,     1,     3,
       1,     1,     1,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       3,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     4,     3,     4,     1,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     0,     0,     0,     0,     0,     2,     3,     0,     0,
       0,     0,     0,     4,    35,     0,     0,     0,     0,     0,
      36,     0,     9,    12,    13,     0,     0,     0,     0,    21,
       0,   101,   105,   100,    99,    96,    97,    98,     0,     0,
      60,    61,    64,    68,    73,    81,    85,    89,     0,    94,
       0,     0,     0,    10,     0,     0,     0,     5,     0,     0,
       0,     0,     0,    11,    63,     0,    66,    67,     0,    70,
      71,    72,     0,    79,    80,    77,    78,    75,    76,     0,
      83,    84,     0,    87,    88,     0,    91,    92,    93,     0,
      95,     0,    15,     0,     0,     0,     0,     7,     0,     0,
       0,    23,     0,   103,     0,    58,     0,   106,    62,    65,
      69,    74,    82,    86,    90,     0,     0,    31,     0,     0,
       0,     0,     6,     0,     8,     0,    22,     0,    17,     0,
     104,     0,   102,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    37,    48,     0,     0,    32,    14,     0,     0,
      18,     0,    33,     0,    26,    27,    28,     0,    24,     0,
      59,    44,     0,     0,     0,    39,     0,     0,    41,    30,
      38,    49,     0,     0,     0,     0,     0,    25,    16,     0,
       0,     0,     0,    40,    47,    29,     0,    57,    20,    34,
       0,     0,     0,     0,    19,     0,     0,    46,     0,     0,
       0,     0,    56,     0,     0,     0,    52,     0,     0,     0,
       0,     0,     0,     0,    42,     0,    55,    53,    45,    50,
      43,    54,    51,     0,     0
};

static const short yydefgoto[] =
{
       1,     6,    21,    22,    23,    30,   100,   101,   157,    93,
     116,    24,    25,   141,   142,   179,   201,   196,   143,   148,
     104,   144,    40,    65,    41,    68,    42,    72,    43,    79,
      44,    82,    45,    85,    46,    89,    47,    48,    49
};

static const short yypact[] =
{
  -32768,    35,   -48,   -15,    14,    21,-32768,-32768,    42,    48,
      32,    98,   123,-32768,-32768,    61,    50,    46,    50,    55,
  -32768,    83,   132,-32768,-32768,    63,    71,   104,    89,    69,
     109,   -45,-32768,-32768,-32768,-32768,-32768,-32768,    46,   103,
      82,    -1,    37,   122,    84,    75,    54,-32768,   -30,-32768,
      -6,   126,   125,-32768,   111,   110,   128,-32768,   112,   121,
      36,    46,    93,-32768,-32768,    46,-32768,-32768,    46,-32768,
  -32768,-32768,    46,-32768,-32768,-32768,-32768,-32768,-32768,    46,
  -32768,-32768,    46,-32768,-32768,    46,-32768,-32768,-32768,    46,
  -32768,     9,-32768,   136,     0,   133,   137,-32768,   135,   124,
      97,   113,     8,-32768,   115,   116,   107,-32768,    -1,    37,
     122,    84,    75,    54,-32768,     6,   168,    20,   138,    46,
     139,    46,-32768,    22,-32768,    60,-32768,   112,-32768,   170,
  -32768,    46,-32768,   145,   149,    46,     6,   147,    46,    46,
     148,   177,     6,-32768,   150,     6,-32768,-32768,   169,   163,
  -32768,   127,-32768,    46,-32768,-32768,-32768,   157,-32768,   155,
  -32768,-32768,   180,   171,   185,-32768,   158,   181,-32768,-32768,
  -32768,-32768,   189,   165,    46,   164,   166,-32768,-32768,   187,
      46,     6,   184,-32768,-32768,-32768,   167,-32768,-32768,-32768,
     175,   181,   108,   172,-32768,    -8,   197,-32768,     6,    46,
     195,   199,-32768,   173,   174,   206,-32768,   186,   176,   200,
       6,     6,   179,     6,-32768,   182,-32768,   175,-32768,   131,
  -32768,-32768,-32768,   217,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,    34,-32768,-32768,   202,    91,-32768,-32768,   119,
     105,   -84,-32768,  -134,-32768,-32768,     4,     7,  -138,    45,
     -38,   -17,-32768,-32768,   161,-32768,   159,-32768,   156,-32768,
     151,-32768,   152,-32768,   144,-32768,   142,-32768,   -47
};


#define	YYLAST		234


static const short yytable[] =
{
      39,    90,   164,   119,    31,    32,    33,   117,   170,    91,
     133,   172,   203,     7,    14,   115,   134,    60,   135,     8,
      61,    62,   136,    91,   137,    14,    31,    32,    33,   184,
     138,    92,    38,   117,   120,   223,     2,   139,    20,   140,
      31,    32,    33,   105,   106,   128,    27,   192,     9,    20,
      66,    67,     3,   197,    38,    10,    53,    11,     4,   152,
      34,    35,   121,    12,   206,    36,    37,     5,    38,    13,
      31,    32,    33,   154,   155,   153,   216,   217,    28,   219,
      31,    32,    33,   151,    29,    69,    70,    71,   156,    51,
      34,    35,    52,   160,    55,    36,    37,    54,    38,   103,
      34,    35,   149,    14,   105,    36,    37,    15,    38,    16,
      86,    87,    88,    56,   105,   198,   199,   200,   163,    17,
      18,   166,   167,    80,    81,    19,    57,    20,    14,    83,
      84,    58,    26,    59,    16,    64,   176,    14,   198,   199,
      63,    94,    95,    16,    17,    18,    99,    97,   204,    96,
      19,    98,    20,    17,    18,   102,   107,   187,   118,    19,
     126,    20,   125,   149,    73,    74,    75,    76,    77,    78,
     122,   123,   124,   132,   145,   147,   150,   127,   130,   161,
     131,   159,   207,   162,   165,   168,   169,   171,   173,   174,
     175,   177,   178,   180,   182,   183,   181,   136,   185,   186,
     193,   188,   190,   189,   194,   195,   205,   208,   209,   202,
     212,   213,   215,   214,   210,   211,   218,   224,   158,   220,
      50,   129,   146,   222,   221,   191,   108,   109,   110,   113,
     111,   114,     0,     0,   112
};

static const short yycheck[] =
{
      17,    48,   136,     3,    34,    35,    36,    91,   142,    15,
       4,   145,    20,    61,     5,     6,    10,    62,    12,    34,
      65,    38,    16,    15,    18,     5,    34,    35,    36,   167,
      24,    37,    62,   117,    34,     0,     1,    31,    29,    33,
      34,    35,    36,    60,    61,    37,    12,   181,    34,    29,
      51,    52,    17,   191,    62,    34,    22,    15,    23,    37,
      54,    55,    62,    15,   198,    59,    60,    32,    62,    37,
      34,    35,    36,    13,    14,    53,   210,   211,    17,   213,
      34,    35,    36,   121,    34,    48,    49,    50,    28,    34,
      54,    55,     9,   131,    23,    59,    60,    34,    62,    63,
      54,    55,   119,     5,   121,    59,    60,     9,    62,    11,
      56,    57,    58,     9,   131,     7,     8,     9,   135,    21,
      22,   138,   139,    39,    40,    27,    37,    29,     5,    54,
      55,    62,     9,    24,    11,    53,   153,     5,     7,     8,
      37,    15,    17,    11,    21,    22,    34,    37,   195,    38,
      27,    23,    29,    21,    22,    34,    63,   174,    22,    27,
      63,    29,    38,   180,    42,    43,    44,    45,    46,    47,
      37,    34,    37,    66,     6,    37,    37,    64,    63,    34,
      64,    11,   199,    34,    37,    37,     9,    37,    19,    26,
      63,    34,    37,    13,     9,    37,    25,    16,     9,    34,
      16,    37,    15,    37,    37,    30,     9,    12,     9,    37,
       4,    25,    12,    37,    41,    41,    37,     0,   127,    37,
      18,   102,   117,   219,   217,   180,    65,    68,    72,    85,
      79,    89,    -1,    -1,    82
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
#line 130 "parse.y"
{
	  open_src(yyvsp[-1].s, source_with);
	}
    break;
case 5:
#line 135 "parse.y"
{
	  gp_warning("empty module");
	}
    break;
case 6:
#line 140 "parse.y"
{
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;
case 7:
#line 145 "parse.y"
{
	  gp_warning("empty public");
	}
    break;
case 8:
#line 150 "parse.y"
{
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;
case 9:
#line 157 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 10:
#line 162 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 11:
#line 169 "parse.y"
{
	  yyval.t = mk_pragma(yyvsp[-1].t);
	}
    break;
case 12:
#line 174 "parse.y"
{
	  yyval.t = yyvsp[0].t;
        }
    break;
case 13:
#line 179 "parse.y"
{
	  yyval.t = yyvsp[0].t;
        }
    break;
case 14:
#line 184 "parse.y"
{ 
	  yyval.t = mk_proc(yyvsp[-3].t, yyvsp[-2].t);
     	}
    break;
case 15:
#line 189 "parse.y"
{ 
	  yyval.t = mk_proc(yyvsp[-1].t, NULL);
     	}
    break;
case 16:
#line 194 "parse.y"
{ 
	  yyval.t = mk_func(yyvsp[-5].t, yyvsp[-3].s, yyvsp[-2].t);
     	}
    break;
case 17:
#line 199 "parse.y"
{ 
	  yyval.t = mk_func(yyvsp[-3].t, yyvsp[-1].s, NULL);
     	}
    break;
case 18:
#line 206 "parse.y"
{
	  /* alias */
	  yyval.t = mk_type(yyvsp[-3].s, NULL, NULL, NULL, yyvsp[-1].s);
        }
    break;
case 19:
#line 212 "parse.y"
{
	  /* array */
	  yyval.t = mk_type(yyvsp[-6].s, yyvsp[-3].r.start, yyvsp[-3].r.end, NULL, yyvsp[-1].s);
        }
    break;
case 20:
#line 218 "parse.y"
{
	  /* enumerated type */
	  yyval.t = mk_type(yyvsp[-5].s, NULL, NULL, yyvsp[-2].t, NULL);
        }
    break;
case 21:
#line 226 "parse.y"
{
 	  yyval.t = mk_head(yyvsp[0].s, NULL);
     	}
    break;
case 22:
#line 231 "parse.y"
{
 	  yyval.t = mk_head(yyvsp[-3].s, yyvsp[-1].t);
     	}
    break;
case 23:
#line 238 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 24:
#line 243 "parse.y"
{
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 25:
#line 250 "parse.y"
{	  
	  yyval.t = mk_arg(yyvsp[-3].s, yyvsp[-1].d, yyvsp[0].s);
        }
    break;
case 26:
#line 256 "parse.y"
{ yyval.d = dir_in; }
    break;
case 27:
#line 257 "parse.y"
{ yyval.d = dir_inout; }
    break;
case 28:
#line 258 "parse.y"
{ yyval.d = dir_out; }
    break;
case 29:
#line 263 "parse.y"
{
 	  yyval.t = mk_body(yyvsp[-3].t, yyvsp[-1].t);
     	}
    break;
case 30:
#line 268 "parse.y"
{
 	  yyval.t = mk_body(NULL, yyvsp[-1].t);
     	}
    break;
case 31:
#line 275 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 32:
#line 280 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 33:
#line 288 "parse.y"
{ 
	  yyval.t = mk_decl(yyvsp[-4].k, yyvsp[-1].s, yyvsp[-3].s, NULL);
        }
    break;
case 34:
#line 293 "parse.y"
{ 
	  yyval.t = mk_decl(yyvsp[-6].k, yyvsp[-3].s, yyvsp[-5].s, yyvsp[-1].t);
        }
    break;
case 35:
#line 300 "parse.y"
{
	  yyval.k = key_constant;
	}
    break;
case 36:
#line 305 "parse.y"
{
	  yyval.k = key_variable;
	}
    break;
case 37:
#line 312 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 38:
#line 317 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 39:
#line 324 "parse.y"
{
	  yyval.t = mk_assembly(strdup("  nop"));
	}
    break;
case 40:
#line 329 "parse.y"
{
	  yyval.t = mk_return(yyvsp[-1].t);
	}
    break;
case 41:
#line 334 "parse.y"
{
	  yyval.t = mk_assembly(yyvsp[-1].s);
	}
    break;
case 42:
#line 339 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-5].t, yyvsp[-3].t, NULL);
	}
    break;
case 43:
#line 344 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-6].t, yyvsp[-4].t, yyvsp[-3].t);
	}
    break;
case 44:
#line 348 "parse.y"
{ case_ident = mk_symbol(yyvsp[0].s, NULL); }
    break;
case 45:
#line 349 "parse.y"
{
	  yyval.t = yyvsp[-3].t;
	}
    break;
case 46:
#line 354 "parse.y"
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
case 47:
#line 373 "parse.y"
{
	  yyval.t= mk_loop(NULL, yyvsp[-1].t, NULL, yyvsp[0].t);	
	}
    break;
case 48:
#line 378 "parse.y"
{
	  yyval.t= mk_loop(NULL, NULL, NULL, yyvsp[0].t);
	}
    break;
case 49:
#line 383 "parse.y"
{
	  yyval.t = yyvsp[-1].t;
	}
    break;
case 50:
#line 390 "parse.y"
{
	  /* last statement is elsif */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;
case 51:
#line 396 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 52:
#line 401 "parse.y"
{
	  /* last statement is else */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;
case 53:
#line 409 "parse.y"
{
	  /* last statement is elsif equivalent */
	  yyval.t = mk_cond(mk_binop(op_eq, case_ident, yyvsp[-2].t), yyvsp[0].t, NULL);
	}
    break;
case 54:
#line 415 "parse.y"
{
	  yyval.t = mk_cond(mk_binop(op_eq, case_ident, yyvsp[-3].t), yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 55:
#line 420 "parse.y"
{
	  /* last statement is else equivalent */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;
case 56:
#line 428 "parse.y"
{
	  yyval.t = yyvsp[-3].t;
	}
    break;
case 57:
#line 435 "parse.y"
{
	  yyval.r.start = yyvsp[-2].t;
	  yyval.r.end = yyvsp[0].t;
        }
    break;
case 58:
#line 443 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 59:
#line 448 "parse.y"
{
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 62:
#line 459 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 63:
#line 464 "parse.y"
{ yyval.o = op_assign; }
    break;
case 65:
#line 470 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 66:
#line 475 "parse.y"
{ yyval.o = op_land; }
    break;
case 67:
#line 476 "parse.y"
{ yyval.o = op_lor; }
    break;
case 69:
#line 482 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 70:
#line 487 "parse.y"
{ yyval.o = op_and; }
    break;
case 71:
#line 488 "parse.y"
{ yyval.o = op_or; }
    break;
case 72:
#line 489 "parse.y"
{ yyval.o = op_xor; }
    break;
case 74:
#line 495 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 75:
#line 500 "parse.y"
{ yyval.o = op_lt; }
    break;
case 76:
#line 501 "parse.y"
{ yyval.o = op_gt; }
    break;
case 77:
#line 502 "parse.y"
{ yyval.o = op_eq; }
    break;
case 78:
#line 503 "parse.y"
{ yyval.o = op_ne; }
    break;
case 79:
#line 504 "parse.y"
{ yyval.o = op_gte; }
    break;
case 80:
#line 505 "parse.y"
{ yyval.o = op_lte; }
    break;
case 82:
#line 511 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 83:
#line 516 "parse.y"
{ yyval.o = op_lsh; }
    break;
case 84:
#line 517 "parse.y"
{ yyval.o = op_rsh; }
    break;
case 86:
#line 523 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 87:
#line 528 "parse.y"
{ yyval.o = op_add; }
    break;
case 88:
#line 529 "parse.y"
{ yyval.o = op_sub; }
    break;
case 90:
#line 535 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 91:
#line 540 "parse.y"
{ yyval.o = op_mult; }
    break;
case 92:
#line 541 "parse.y"
{ yyval.o = op_div; }
    break;
case 93:
#line 542 "parse.y"
{ yyval.o = op_mod; }
    break;
case 95:
#line 548 "parse.y"
{
	  yyval.t = mk_unop(yyvsp[-1].o, yyvsp[0].t);
	}
    break;
case 96:
#line 553 "parse.y"
{ yyval.o = op_neg; }
    break;
case 97:
#line 554 "parse.y"
{ yyval.o = op_not; }
    break;
case 98:
#line 555 "parse.y"
{ yyval.o = op_com; }
    break;
case 99:
#line 556 "parse.y"
{ yyval.o = op_add; }
    break;
case 100:
#line 560 "parse.y"
{
	  yyval.t = mk_string(yyvsp[0].s);
        }
    break;
case 101:
#line 565 "parse.y"
{
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;
case 102:
#line 570 "parse.y"
{
	  yyval.t = mk_symbol(yyvsp[-3].s, yyvsp[-1].t);
        }
    break;
case 103:
#line 575 "parse.y"
{
	  /* function or procedure call */
	  yyval.t = mk_call(yyvsp[-2].s, NULL);
	}
    break;
case 104:
#line 581 "parse.y"
{
	  /* function or procedure call with arguments */
	  yyval.t = mk_call(yyvsp[-3].s, yyvsp[-1].t);
	}
    break;
case 105:
#line 587 "parse.y"
{
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;
case 106:
#line 592 "parse.y"
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
#line 597 "parse.y"


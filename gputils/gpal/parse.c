/* A Bison parser, made from parse.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	ARRAY	257
# define	CASE	258
# define	CONSTANT	259
# define	BEGIN_TOK	260
# define	ELSE	261
# define	ELSIF	262
# define	END	263
# define	FOR	264
# define	FUNCTION	265
# define	GOTO	266
# define	IF	267
# define	IN	268
# define	INOUT	269
# define	IS	270
# define	LOOP	271
# define	MODULE	272
# define	NULL_TOK	273
# define	OF	274
# define	OTHERS	275
# define	PRAGMA	276
# define	PROCEDURE	277
# define	PUBLIC	278
# define	RETURN	279
# define	THEN	280
# define	TO	281
# define	TYPE	282
# define	OUT	283
# define	WHEN	284
# define	WHILE	285
# define	WITH	286
# define	ASM	287
# define	IDENT	288
# define	LABEL	289
# define	NUMBER	290
# define	STRING	291
# define	LSH	292
# define	RSH	293
# define	ARROW	294
# define	GREATER_EQUAL	295
# define	LESS_EQUAL	296
# define	EQUAL	297
# define	NOT_EQUAL	298
# define	LOGICAL_AND	299
# define	LOGICAL_OR	300

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



#define	YYFINAL		223
#define	YYFLAG		-32768
#define	YYNTBASE	68

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 300 ? yytranslate[x] : 108)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    40,    41,    42,    43,    44,    45,    46,    52,
      53
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     8,    12,    20,    28,    30,    33,
      37,    39,    41,    47,    51,    59,    65,    71,    80,    88,
      90,    95,    97,   101,   106,   108,   110,   112,   118,   119,
     121,   123,   126,   131,   138,   146,   147,   149,   151,   154,
     157,   159,   163,   167,   170,   178,   187,   188,   197,   203,
     207,   209,   212,   217,   223,   226,   231,   237,   242,   248,
     252,   254,   258,   260,   262,   266,   268,   270,   274,   276,
     278,   280,   284,   286,   288,   290,   292,   296,   298,   300,
     302,   304,   306,   308,   310,   314,   316,   318,   320,   324,
     326,   328,   330,   334,   336,   338,   340,   342,   345,   347,
     349,   351,   353,   355,   357,   362,   366,   371,   373
};
static const short yyrhs[] =
{
      -1,    68,    69,     0,    68,     1,    62,     0,    32,    34,
      38,     0,    18,    34,    16,    70,     9,    18,    38,     0,
      24,    34,    16,    70,     9,    24,    38,     0,    71,     0,
      71,    70,     0,    22,    90,    38,     0,    72,     0,    80,
       0,    23,    73,    77,    23,    38,     0,    23,    73,    38,
       0,    11,    73,    25,    34,    77,    11,    38,     0,    11,
      73,    25,    34,    38,     0,    28,    34,    16,    34,    38,
       0,    28,    34,    16,     3,    88,    20,    34,    38,     0,
      28,    34,    16,    63,    89,    64,    38,     0,    34,     0,
      34,    63,    74,    64,     0,    75,     0,    75,    65,    74,
       0,    34,    39,    76,    34,     0,    14,     0,    15,     0,
      29,     0,    16,    78,     6,    81,     9,     0,     0,    79,
       0,    80,     0,    80,    79,     0,    34,    39,    34,    38,
       0,    34,    39,    34,    54,    90,    38,     0,    34,    39,
       5,    34,    54,    90,    38,     0,     0,    82,     0,    83,
       0,    83,    82,     0,    19,    38,     0,    35,     0,    12,
      34,    38,     0,    25,    90,    38,     0,    33,    38,     0,
      13,    90,    26,    81,     9,    13,    38,     0,    13,    90,
      26,    81,    85,     9,    13,    38,     0,     0,     4,    34,
      84,    16,    86,     9,     4,    38,     0,    10,    34,    14,
      88,    87,     0,    31,    90,    87,     0,    87,     0,    90,
      38,     0,     8,    90,    26,    81,     0,     8,    90,    26,
      81,    85,     0,     7,    81,     0,    30,   107,    42,    81,
       0,    30,   107,    42,    81,    86,     0,    30,    21,    42,
      81,     0,    17,    81,     9,    17,    38,     0,    90,    27,
      90,     0,    90,     0,    90,    65,    89,     0,    91,     0,
      93,     0,    91,    92,    93,     0,    54,     0,    95,     0,
      93,    94,    95,     0,    52,     0,    53,     0,    97,     0,
      95,    96,    97,     0,    49,     0,    50,     0,    51,     0,
      99,     0,    97,    98,    99,     0,    47,     0,    48,     0,
      45,     0,    46,     0,    43,     0,    44,     0,   101,     0,
      99,   100,   101,     0,    40,     0,    41,     0,   103,     0,
     101,   102,   103,     0,    55,     0,    56,     0,   105,     0,
     103,   104,   105,     0,    57,     0,    58,     0,    59,     0,
     107,     0,   106,   107,     0,    56,     0,    60,     0,    61,
       0,    55,     0,    37,     0,    34,     0,    34,    66,    90,
      67,     0,    34,    63,    64,     0,    34,    63,    89,    64,
       0,    36,     0,    63,    90,    64,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   159,   161,   163,   167,   172,   177,   184,   189,   196,
     201,   206,   211,   216,   221,   226,   233,   239,   245,   253,
     258,   265,   270,   277,   284,   286,   287,   290,   297,   302,
     309,   314,   321,   326,   331,   338,   343,   350,   355,   362,
     367,   372,   377,   382,   387,   392,   397,   397,   410,   429,
     434,   439,   446,   452,   457,   465,   471,   476,   484,   491,
     499,   504,   511,   513,   515,   522,   524,   526,   533,   534,
     536,   538,   545,   546,   547,   549,   551,   558,   559,   560,
     561,   562,   563,   565,   567,   574,   575,   577,   579,   586,
     587,   589,   591,   598,   599,   600,   602,   604,   611,   612,
     613,   614,   616,   621,   626,   631,   637,   643,   648
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "\"array\"", "\"case\"", "\"constant\"", 
  "\"begin\"", "\"else\"", "\"elsif\"", "\"end\"", "\"for\"", 
  "\"function\"", "\"goto\"", "\"if\"", "\"in\"", "\"inout\"", "\"is\"", 
  "\"loop\"", "\"module\"", "\"null\"", "\"of\"", "\"others\"", 
  "\"pragma\"", "\"procedure\"", "\"public\"", "\"return\"", "\"then\"", 
  "\"to\"", "\"type\"", "\"out\"", "\"when\"", "\"while\"", "\"with\"", 
  "\"asm\"", "\"symbol\"", "\"label\"", "\"number\"", "\"string\"", "';'", 
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
       0,    68,    68,    68,    69,    69,    69,    70,    70,    71,
      71,    71,    71,    71,    71,    71,    72,    72,    72,    73,
      73,    74,    74,    75,    76,    76,    76,    77,    78,    78,
      79,    79,    80,    80,    80,    81,    81,    82,    82,    83,
      83,    83,    83,    83,    83,    83,    84,    83,    83,    83,
      83,    83,    85,    85,    85,    86,    86,    86,    87,    88,
      89,    89,    90,    91,    91,    92,    93,    93,    94,    94,
      95,    95,    96,    96,    96,    97,    97,    98,    98,    98,
      98,    98,    98,    99,    99,   100,   100,   101,   101,   102,
     102,   103,   103,   104,   104,   104,   105,   105,   106,   106,
     106,   106,   107,   107,   107,   107,   107,   107,   107
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     3,     3,     7,     7,     1,     2,     3,
       1,     1,     5,     3,     7,     5,     5,     8,     7,     1,
       4,     1,     3,     4,     1,     1,     1,     5,     0,     1,
       1,     2,     4,     6,     7,     0,     1,     1,     2,     2,
       1,     3,     3,     2,     7,     8,     0,     8,     5,     3,
       1,     2,     4,     5,     2,     4,     5,     4,     5,     3,
       1,     3,     1,     1,     3,     1,     1,     3,     1,     1,
       1,     3,     1,     1,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     1,     1,     3,     1,
       1,     1,     3,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     4,     3,     4,     1,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     0,     0,     0,     0,     0,     2,     3,     0,     0,
       0,     0,     0,     4,     0,     0,     0,     0,     0,     0,
       7,    10,    11,     0,    19,     0,   103,   107,   102,   101,
      98,    99,   100,     0,     0,    62,    63,    66,    70,    75,
      83,    87,    91,     0,    96,     0,     0,     0,     0,     8,
       0,     0,     0,     0,     0,     0,     9,    65,     0,    68,
      69,     0,    72,    73,    74,     0,    81,    82,    79,    80,
      77,    78,     0,    85,    86,     0,    89,    90,     0,    93,
      94,    95,     0,    97,    28,    13,     0,     0,     0,     0,
       0,     0,     0,     0,    21,     0,   105,     0,    60,     0,
     108,    64,    67,    71,    76,    84,    88,    92,     0,    29,
      30,     0,     0,     0,     0,     0,    32,     0,     5,     6,
       0,    20,     0,    15,     0,   106,     0,   104,    35,    31,
      12,     0,     0,    16,     0,     0,     0,    24,    25,    26,
       0,    22,     0,    61,     0,     0,     0,     0,    35,     0,
       0,     0,     0,    40,     0,    36,    37,    50,     0,     0,
       0,     0,     0,    33,    23,    14,    46,     0,     0,     0,
       0,    39,     0,     0,    43,    27,    38,    51,     0,    59,
      18,    34,     0,     0,    41,    35,     0,    42,    49,    17,
       0,     0,     0,     0,     0,     0,    48,    35,     0,     0,
       0,    58,     0,     0,     0,    54,     0,     0,     0,    35,
      35,     0,    35,    44,     0,    57,    55,    47,    52,    45,
      56,    53,     0,     0
};

static const short yydefgoto[] =
{
       1,     6,    19,    20,    21,    25,    93,    94,   140,    86,
     108,   109,    22,   154,   155,   156,   182,   200,   195,   157,
     131,    97,   158,    35,    58,    36,    61,    37,    65,    38,
      72,    39,    75,    40,    78,    41,    82,    42,    43,    44
};

static const short yypact[] =
{
  -32768,    50,   -46,   -13,   -10,    22,-32768,-32768,    47,    57,
      28,    95,    95,-32768,    36,    35,    36,    49,    42,    84,
      95,-32768,-32768,    85,    37,    76,   -41,-32768,-32768,-32768,
  -32768,-32768,-32768,    35,    71,    56,    -8,    54,    94,     7,
      -2,    67,-32768,   -28,-32768,    -9,    98,     8,   101,-32768,
      97,    88,    93,    52,    35,    64,-32768,-32768,    35,-32768,
  -32768,    35,-32768,-32768,-32768,    35,-32768,-32768,-32768,-32768,
  -32768,-32768,    35,-32768,-32768,    35,-32768,-32768,    35,-32768,
  -32768,-32768,    35,-32768,    96,-32768,   108,    -1,    99,   -27,
     105,   106,   110,    70,    86,    -6,-32768,    89,    90,    83,
  -32768,    -8,    54,    94,     7,    -2,    67,-32768,   148,-32768,
      96,   118,    35,   119,    35,   104,-32768,    35,-32768,-32768,
      63,-32768,    88,-32768,   149,-32768,    35,-32768,    24,-32768,
  -32768,   141,   132,-32768,   100,    35,   124,-32768,-32768,-32768,
     129,-32768,   127,-32768,   133,   135,   136,    35,    24,   128,
      35,    35,   134,-32768,   162,-32768,    24,-32768,   137,   140,
      35,   138,   142,-32768,-32768,-32768,-32768,   159,   143,   151,
     169,-32768,   144,   167,-32768,-32768,-32768,-32768,   147,-32768,
  -32768,-32768,   163,    35,-32768,    24,   170,-32768,-32768,-32768,
     156,   167,   139,   150,   -17,   180,-32768,    24,    35,   177,
     182,-32768,   152,   153,   188,-32768,   171,   155,   183,    24,
      24,   160,    24,-32768,   161,-32768,   156,-32768,    68,-32768,
  -32768,-32768,   200,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,     3,-32768,-32768,   185,    80,-32768,-32768,   109,
  -32768,   102,   -79,  -145,    51,-32768,-32768,   -12,   -11,  -161,
      20,  -100,   -15,-32768,-32768,   157,-32768,   158,-32768,   145,
  -32768,   146,-32768,   154,-32768,   130,-32768,   131,-32768,   -42
};


#define	YYLAST		229


static const short yytable[] =
{
      34,    83,   112,   170,   202,   110,    26,    84,    27,    28,
      84,   116,   188,    88,   134,    23,     7,    26,    55,    27,
      28,     8,    53,    49,     9,    54,   143,   117,   144,    85,
     196,   110,   123,   113,   145,    33,   146,   147,    98,    99,
     192,   148,    89,   149,    59,    60,    33,    73,    74,   150,
     222,     2,   205,    76,    77,   151,    10,   152,    26,   153,
      27,    28,   114,    11,   215,   216,    13,   218,     3,    26,
      24,    27,    28,    12,     4,   197,   198,   137,   138,    29,
      30,    47,     5,    46,    31,    32,    26,    33,    27,    28,
      29,    30,   139,    48,    50,    31,    32,   132,    33,    98,
      51,    52,   136,    62,    63,    64,    14,    29,    30,    56,
      57,    98,    31,    32,    87,    33,    96,    15,    16,    90,
     162,    91,    92,    17,    79,    80,    81,    95,   100,    18,
      18,   111,   169,   115,   121,   172,   173,    66,    67,    68,
      69,    70,    71,   118,   119,   179,   197,   198,   199,   120,
     127,   122,   203,   125,   128,   126,   130,   133,   135,   160,
     142,   159,   163,   164,   161,   165,   171,   166,   132,   167,
     168,   175,   174,   183,   178,   177,   180,   185,   186,   190,
     181,   184,   187,   206,   148,   189,   194,   193,   201,   204,
     207,   208,   211,   213,   209,   210,   214,   212,   217,   219,
     223,    45,   141,   191,   124,   220,   221,   176,   106,     0,
     103,     0,   129,   107,     0,   101,     0,     0,   104,   102,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   105
};

static const short yycheck[] =
{
      15,    43,     3,   148,    21,    84,    34,    16,    36,    37,
      16,    38,   173,     5,   114,    12,    62,    34,    33,    36,
      37,    34,    63,    20,    34,    66,   126,    54,     4,    38,
     191,   110,    38,    34,    10,    63,    12,    13,    53,    54,
     185,    17,    34,    19,    52,    53,    63,    40,    41,    25,
       0,     1,   197,    55,    56,    31,    34,    33,    34,    35,
      36,    37,    63,    16,   209,   210,    38,   212,    18,    34,
      34,    36,    37,    16,    24,     7,     8,    14,    15,    55,
      56,    39,    32,    34,    60,    61,    34,    63,    36,    37,
      55,    56,    29,     9,     9,    60,    61,   112,    63,   114,
      63,    25,   117,    49,    50,    51,    11,    55,    56,    38,
      54,   126,    60,    61,    16,    63,    64,    22,    23,    18,
     135,    24,    34,    28,    57,    58,    59,    34,    64,    34,
      34,    23,   147,    34,    64,   150,   151,    43,    44,    45,
      46,    47,    48,    38,    38,   160,     7,     8,     9,    39,
      67,    65,   194,    64,     6,    65,    38,    38,    54,    27,
      11,    20,    38,    34,    64,    38,    38,    34,   183,    34,
      34,     9,    38,    14,    34,    38,    38,    26,     9,    16,
      38,    38,    38,   198,    17,    38,    30,    17,    38,     9,
      13,     9,     4,    38,    42,    42,    13,    26,    38,    38,
       0,    16,   122,   183,    95,   216,   218,   156,    78,    -1,
      65,    -1,   110,    82,    -1,    58,    -1,    -1,    72,    61,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75
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
#line 169 "parse.y"
{
	  open_src(yyvsp[-1].s, source_with);
	}
    break;
case 5:
#line 174 "parse.y"
{
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;
case 6:
#line 179 "parse.y"
{
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;
case 7:
#line 186 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 8:
#line 191 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 9:
#line 198 "parse.y"
{
	  yyval.t = mk_pragma(yyvsp[-1].t);
	}
    break;
case 10:
#line 203 "parse.y"
{
	  yyval.t = yyvsp[0].t;
        }
    break;
case 11:
#line 208 "parse.y"
{
	  yyval.t = yyvsp[0].t;
        }
    break;
case 12:
#line 213 "parse.y"
{ 
	  yyval.t = mk_proc(yyvsp[-3].t, yyvsp[-2].t);
     	}
    break;
case 13:
#line 218 "parse.y"
{ 
	  yyval.t = mk_proc(yyvsp[-1].t, NULL);
	}
    break;
case 14:
#line 223 "parse.y"
{ 
	  yyval.t = mk_func(yyvsp[-5].t, yyvsp[-3].s, yyvsp[-2].t);
	}
    break;
case 15:
#line 228 "parse.y"
{ 
	  yyval.t = mk_func(yyvsp[-3].t, yyvsp[-1].s, NULL);
	}
    break;
case 16:
#line 235 "parse.y"
{
	  /* alias */
	  yyval.t = mk_type(yyvsp[-3].s, NULL, NULL, NULL, yyvsp[-1].s);
        }
    break;
case 17:
#line 241 "parse.y"
{
	  /* array */
	  yyval.t = mk_type(yyvsp[-6].s, yyvsp[-3].r.start, yyvsp[-3].r.end, NULL, yyvsp[-1].s);
        }
    break;
case 18:
#line 247 "parse.y"
{
	  /* enumerated type */
	  yyval.t = mk_type(yyvsp[-5].s, NULL, NULL, yyvsp[-2].t, NULL);
        }
    break;
case 19:
#line 255 "parse.y"
{
 	  yyval.t = mk_head(yyvsp[0].s, NULL);
     	}
    break;
case 20:
#line 260 "parse.y"
{
 	  yyval.t = mk_head(yyvsp[-3].s, yyvsp[-1].t);
     	}
    break;
case 21:
#line 267 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 22:
#line 272 "parse.y"
{
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 23:
#line 279 "parse.y"
{	  
	  yyval.t = mk_arg(yyvsp[-3].s, yyvsp[-1].d, yyvsp[0].s);
        }
    break;
case 24:
#line 285 "parse.y"
{ yyval.d = dir_in; }
    break;
case 25:
#line 286 "parse.y"
{ yyval.d = dir_inout; }
    break;
case 26:
#line 287 "parse.y"
{ yyval.d = dir_out; }
    break;
case 27:
#line 292 "parse.y"
{
	  yyval.t = mk_body(yyvsp[-3].t, yyvsp[-1].t);
	}
    break;
case 28:
#line 299 "parse.y"
{
	  yyval.t = NULL;
	}
    break;
case 29:
#line 304 "parse.y"
{
          yyval.t = yyvsp[0].t;
	}
    break;
case 30:
#line 311 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 31:
#line 316 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 32:
#line 323 "parse.y"
{ 
	  yyval.t = mk_decl(yyvsp[-3].s, false, yyvsp[-1].s, NULL);
        }
    break;
case 33:
#line 328 "parse.y"
{ 
	  yyval.t = mk_decl(yyvsp[-5].s, false, yyvsp[-3].s, yyvsp[-1].t);
        }
    break;
case 34:
#line 333 "parse.y"
{ 
	  yyval.t = mk_decl(yyvsp[-6].s, true, yyvsp[-3].s, yyvsp[-1].t);
        }
    break;
case 35:
#line 340 "parse.y"
{
	  yyval.t = NULL;
	}
    break;
case 36:
#line 345 "parse.y"
{
	  yyval.t = yyvsp[0].t;
	}
    break;
case 37:
#line 352 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 38:
#line 357 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 39:
#line 364 "parse.y"
{
	  yyval.t = mk_assembly(strdup("  nop"));
	}
    break;
case 40:
#line 369 "parse.y"
{
	  yyval.t = mk_label(yyvsp[0].s);
	}
    break;
case 41:
#line 374 "parse.y"
{
	  yyval.t = mk_goto(yyvsp[-1].s);
	}
    break;
case 42:
#line 379 "parse.y"
{
	  yyval.t = mk_return(yyvsp[-1].t);
	}
    break;
case 43:
#line 384 "parse.y"
{
	  yyval.t = mk_assembly(yyvsp[-1].s);
	}
    break;
case 44:
#line 389 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-5].t, yyvsp[-3].t, NULL);
	}
    break;
case 45:
#line 394 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-6].t, yyvsp[-4].t, yyvsp[-3].t);
	}
    break;
case 46:
#line 399 "parse.y"
{
	  if (case_ident) {
	    yyerror("nested case statements are not yet supported");
	  }
	  case_ident = mk_symbol(yyvsp[0].s, NULL);
	}
    break;
case 47:
#line 406 "parse.y"
{
	  yyval.t = yyvsp[-3].t;
	  case_ident = NULL;
	}
    break;
case 48:
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
case 49:
#line 431 "parse.y"
{
	  yyval.t= mk_loop(NULL, yyvsp[-1].t, NULL, yyvsp[0].t);	
	}
    break;
case 50:
#line 436 "parse.y"
{
	  yyval.t= mk_loop(NULL, NULL, NULL, yyvsp[0].t);
	}
    break;
case 51:
#line 441 "parse.y"
{
	  yyval.t = yyvsp[-1].t;
	}
    break;
case 52:
#line 448 "parse.y"
{
	  /* last statement is elsif */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;
case 53:
#line 454 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 54:
#line 459 "parse.y"
{
	  /* last statement is else */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;
case 55:
#line 467 "parse.y"
{
	  /* last statement is elsif equivalent */
	  yyval.t = mk_cond(mk_binop(op_eq, case_ident, yyvsp[-2].t), yyvsp[0].t, NULL);
	}
    break;
case 56:
#line 473 "parse.y"
{
	  yyval.t = mk_cond(mk_binop(op_eq, case_ident, yyvsp[-3].t), yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 57:
#line 478 "parse.y"
{
	  /* last statement is else equivalent */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;
case 58:
#line 486 "parse.y"
{
	  yyval.t = yyvsp[-3].t;
	}
    break;
case 59:
#line 493 "parse.y"
{
	  yyval.r.start = yyvsp[-2].t;
	  yyval.r.end = yyvsp[0].t;
        }
    break;
case 60:
#line 501 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 61:
#line 506 "parse.y"
{
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 64:
#line 517 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 65:
#line 522 "parse.y"
{ yyval.o = op_assign; }
    break;
case 67:
#line 528 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 68:
#line 533 "parse.y"
{ yyval.o = op_land; }
    break;
case 69:
#line 534 "parse.y"
{ yyval.o = op_lor; }
    break;
case 71:
#line 540 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 72:
#line 545 "parse.y"
{ yyval.o = op_and; }
    break;
case 73:
#line 546 "parse.y"
{ yyval.o = op_or; }
    break;
case 74:
#line 547 "parse.y"
{ yyval.o = op_xor; }
    break;
case 76:
#line 553 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 77:
#line 558 "parse.y"
{ yyval.o = op_lt; }
    break;
case 78:
#line 559 "parse.y"
{ yyval.o = op_gt; }
    break;
case 79:
#line 560 "parse.y"
{ yyval.o = op_eq; }
    break;
case 80:
#line 561 "parse.y"
{ yyval.o = op_ne; }
    break;
case 81:
#line 562 "parse.y"
{ yyval.o = op_gte; }
    break;
case 82:
#line 563 "parse.y"
{ yyval.o = op_lte; }
    break;
case 84:
#line 569 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 85:
#line 574 "parse.y"
{ yyval.o = op_lsh; }
    break;
case 86:
#line 575 "parse.y"
{ yyval.o = op_rsh; }
    break;
case 88:
#line 581 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 89:
#line 586 "parse.y"
{ yyval.o = op_add; }
    break;
case 90:
#line 587 "parse.y"
{ yyval.o = op_sub; }
    break;
case 92:
#line 593 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 93:
#line 598 "parse.y"
{ yyval.o = op_mult; }
    break;
case 94:
#line 599 "parse.y"
{ yyval.o = op_div; }
    break;
case 95:
#line 600 "parse.y"
{ yyval.o = op_mod; }
    break;
case 97:
#line 606 "parse.y"
{
	  yyval.t = mk_unop(yyvsp[-1].o, yyvsp[0].t);
	}
    break;
case 98:
#line 611 "parse.y"
{ yyval.o = op_neg; }
    break;
case 99:
#line 612 "parse.y"
{ yyval.o = op_not; }
    break;
case 100:
#line 613 "parse.y"
{ yyval.o = op_com; }
    break;
case 101:
#line 614 "parse.y"
{ yyval.o = op_add; }
    break;
case 102:
#line 618 "parse.y"
{
	  yyval.t = mk_string(yyvsp[0].s);
        }
    break;
case 103:
#line 623 "parse.y"
{
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;
case 104:
#line 628 "parse.y"
{
	  yyval.t = mk_symbol(yyvsp[-3].s, yyvsp[-1].t);
        }
    break;
case 105:
#line 633 "parse.y"
{
	  /* function or procedure call */
	  yyval.t = mk_call(yyvsp[-2].s, NULL);
	}
    break;
case 106:
#line 639 "parse.y"
{
	  /* function or procedure call with arguments */
	  yyval.t = mk_call(yyvsp[-3].s, yyvsp[-1].t);
	}
    break;
case 107:
#line 645 "parse.y"
{
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;
case 108:
#line 650 "parse.y"
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
#line 655 "parse.y"


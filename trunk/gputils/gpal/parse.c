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
# define	PUBLIC	277
# define	RETURN	278
# define	THEN	279
# define	TO	280
# define	TYPE	281
# define	OUT	282
# define	WHEN	283
# define	WHILE	284
# define	WITH	285
# define	ASM	286
# define	IDENT	287
# define	NUMBER	288
# define	STRING	289
# define	LSH	290
# define	RSH	291
# define	ARROW	292
# define	GREATER_EQUAL	293
# define	LESS_EQUAL	294
# define	EQUAL	295
# define	NOT_EQUAL	296
# define	LOGICAL_AND	297
# define	LOGICAL_OR	298

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



#define	YYFINAL		219
#define	YYFLAG		-32768
#define	YYNTBASE	66

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 298 ? yytranslate[x] : 106)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      60,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    58,     2,     2,     2,    57,    47,     2,
      61,    62,    55,    53,    63,    54,     2,    56,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,    36,
      45,    52,    46,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    64,     2,    65,    49,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    48,     2,    59,     2,     2,     2,
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
      38,    39,    40,    41,    42,    43,    44,    50,    51
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     8,    12,    20,    28,    30,    33,
      37,    39,    41,    47,    51,    59,    65,    71,    80,    88,
      90,    95,    97,   101,   106,   108,   110,   112,   118,   119,
     121,   123,   126,   131,   138,   146,   147,   149,   151,   154,
     157,   161,   164,   172,   181,   182,   191,   197,   201,   203,
     206,   211,   217,   220,   225,   231,   236,   242,   246,   248,
     252,   254,   256,   260,   262,   264,   268,   270,   272,   274,
     278,   280,   282,   284,   286,   290,   292,   294,   296,   298,
     300,   302,   304,   308,   310,   312,   314,   318,   320,   322,
     324,   328,   330,   332,   334,   336,   339,   341,   343,   345,
     347,   349,   351,   356,   360,   365,   367
};
static const short yyrhs[] =
{
      -1,    66,    67,     0,    66,     1,    60,     0,    31,    33,
      36,     0,    17,    33,    15,    68,     9,    17,    36,     0,
      23,    33,    15,    68,     9,    23,    36,     0,    69,     0,
      69,    68,     0,    21,    88,    36,     0,    70,     0,    78,
       0,    22,    71,    75,    22,    36,     0,    22,    71,    36,
       0,    11,    71,    24,    33,    75,    11,    36,     0,    11,
      71,    24,    33,    36,     0,    27,    33,    15,    33,    36,
       0,    27,    33,    15,     3,    86,    19,    33,    36,     0,
      27,    33,    15,    61,    87,    62,    36,     0,    33,     0,
      33,    61,    72,    62,     0,    73,     0,    73,    63,    72,
       0,    33,    37,    74,    33,     0,    13,     0,    14,     0,
      28,     0,    15,    76,     6,    79,     9,     0,     0,    77,
       0,    78,     0,    78,    77,     0,    33,    37,    33,    36,
       0,    33,    37,    33,    52,    88,    36,     0,    33,    37,
       5,    33,    52,    88,    36,     0,     0,    80,     0,    81,
       0,    81,    80,     0,    18,    36,     0,    24,    88,    36,
       0,    32,    36,     0,    12,    88,    25,    79,     9,    12,
      36,     0,    12,    88,    25,    79,    83,     9,    12,    36,
       0,     0,     4,    33,    82,    15,    84,     9,     4,    36,
       0,    10,    33,    13,    86,    85,     0,    30,    88,    85,
       0,    85,     0,    88,    36,     0,     8,    88,    25,    79,
       0,     8,    88,    25,    79,    83,     0,     7,    79,     0,
      29,   105,    40,    79,     0,    29,   105,    40,    79,    84,
       0,    29,    20,    40,    79,     0,    16,    79,     9,    16,
      36,     0,    88,    26,    88,     0,    88,     0,    88,    63,
      87,     0,    89,     0,    91,     0,    89,    90,    91,     0,
      52,     0,    93,     0,    91,    92,    93,     0,    50,     0,
      51,     0,    95,     0,    93,    94,    95,     0,    47,     0,
      48,     0,    49,     0,    97,     0,    95,    96,    97,     0,
      45,     0,    46,     0,    43,     0,    44,     0,    41,     0,
      42,     0,    99,     0,    97,    98,    99,     0,    38,     0,
      39,     0,   101,     0,    99,   100,   101,     0,    53,     0,
      54,     0,   103,     0,   101,   102,   103,     0,    55,     0,
      56,     0,    57,     0,   105,     0,   104,   105,     0,    54,
       0,    58,     0,    59,     0,    53,     0,    35,     0,    33,
       0,    33,    64,    88,    65,     0,    33,    61,    62,     0,
      33,    61,    87,    62,     0,    34,     0,    61,    88,    62,
       0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   157,   159,   161,   165,   170,   175,   182,   187,   194,
     199,   204,   209,   214,   219,   224,   231,   237,   243,   251,
     256,   263,   268,   275,   282,   284,   285,   288,   295,   300,
     307,   312,   319,   324,   329,   336,   341,   348,   353,   360,
     365,   370,   375,   380,   385,   385,   398,   417,   422,   427,
     434,   440,   445,   453,   459,   464,   472,   479,   487,   492,
     499,   501,   503,   510,   512,   514,   521,   522,   524,   526,
     533,   534,   535,   537,   539,   546,   547,   548,   549,   550,
     551,   553,   555,   562,   563,   565,   567,   574,   575,   577,
     579,   586,   587,   588,   590,   592,   599,   600,   601,   602,
     604,   609,   614,   619,   625,   631,   636
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "\"array\"", "\"case\"", "\"constant\"", 
  "\"begin\"", "\"else\"", "\"elsif\"", "\"end\"", "\"for\"", 
  "\"function\"", "\"if\"", "\"in\"", "\"inout\"", "\"is\"", "\"loop\"", 
  "\"module\"", "\"null\"", "\"of\"", "\"others\"", "\"pragma\"", 
  "\"procedure\"", "\"public\"", "\"return\"", "\"then\"", "\"to\"", 
  "\"type\"", "\"out\"", "\"when\"", "\"while\"", "\"with\"", "\"asm\"", 
  "\"symbol\"", "\"number\"", "\"string\"", "';'", "':'", "\"<<\"", 
  "\">>\"", "\"=>\"", "\">=\"", "\"<=\"", "\"==\"", "\"!=\"", "'<'", 
  "'>'", "'&'", "'|'", "'^'", "\"&&\"", "\"||\"", "'='", "'+'", "'-'", 
  "'*'", "'/'", "'%'", "'!'", "'~'", "'\\n'", "'('", "')'", "','", "'['", 
  "']'", "program", "entity", "element_list", "element", "type", "head", 
  "arg_list", "arg", "arg_direction", "body", "decl_start", "decl_block", 
  "decl", "statement_start", "statement_block", "statement", "@1", 
  "if_body", "case_body", "loop_statement", "range", "parameter_list", 
  "expr", "e8", "e8op", "e7", "e7op", "e6", "e6op", "e5", "e5op", "e4", 
  "e4op", "e3", "e3op", "e2", "e2op", "e1", "e1op", "e0", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    66,    66,    66,    67,    67,    67,    68,    68,    69,
      69,    69,    69,    69,    69,    69,    70,    70,    70,    71,
      71,    72,    72,    73,    74,    74,    74,    75,    76,    76,
      77,    77,    78,    78,    78,    79,    79,    80,    80,    81,
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
       0,     0,     2,     3,     3,     7,     7,     1,     2,     3,
       1,     1,     5,     3,     7,     5,     5,     8,     7,     1,
       4,     1,     3,     4,     1,     1,     1,     5,     0,     1,
       1,     2,     4,     6,     7,     0,     1,     1,     2,     2,
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
       0,     0,     0,     4,     0,     0,     0,     0,     0,     0,
       7,    10,    11,     0,    19,     0,   101,   105,   100,    99,
      96,    97,    98,     0,     0,    60,    61,    64,    68,    73,
      81,    85,    89,     0,    94,     0,     0,     0,     0,     8,
       0,     0,     0,     0,     0,     0,     9,    63,     0,    66,
      67,     0,    70,    71,    72,     0,    79,    80,    77,    78,
      75,    76,     0,    83,    84,     0,    87,    88,     0,    91,
      92,    93,     0,    95,    28,    13,     0,     0,     0,     0,
       0,     0,     0,     0,    21,     0,   103,     0,    58,     0,
     106,    62,    65,    69,    74,    82,    86,    90,     0,    29,
      30,     0,     0,     0,     0,     0,    32,     0,     5,     6,
       0,    20,     0,    15,     0,   104,     0,   102,    35,    31,
      12,     0,     0,    16,     0,     0,     0,    24,    25,    26,
       0,    22,     0,    59,     0,     0,     0,    35,     0,     0,
       0,     0,     0,    36,    37,    48,     0,     0,     0,     0,
       0,    33,    23,    14,    44,     0,     0,     0,    39,     0,
       0,    41,    27,    38,    49,     0,    57,    18,    34,     0,
       0,    35,     0,    40,    47,    17,     0,     0,     0,     0,
       0,     0,    46,    35,     0,     0,     0,    56,     0,     0,
       0,    52,     0,     0,     0,    35,    35,     0,    35,    42,
       0,    55,    53,    45,    50,    43,    54,    51,     0,     0
};

static const short yydefgoto[] =
{
       1,     6,    19,    20,    21,    25,    93,    94,   140,    86,
     108,   109,    22,   152,   153,   154,   179,   196,   191,   155,
     131,    97,   156,    35,    58,    36,    61,    37,    65,    38,
      72,    39,    75,    40,    78,    41,    82,    42,    43,    44
};

static const short yypact[] =
{
  -32768,    50,   -52,   -16,   -14,     9,-32768,-32768,    32,    38,
      23,    67,    67,-32768,    24,    51,    24,    57,    61,    78,
      67,-32768,-32768,    92,    58,    79,     7,-32768,-32768,-32768,
  -32768,-32768,-32768,    51,    93,    80,   -38,    59,    72,    54,
      42,    66,-32768,   -28,-32768,    -6,   115,    -2,   116,-32768,
     113,   104,   105,    21,    51,    77,-32768,-32768,    51,-32768,
  -32768,    51,-32768,-32768,-32768,    51,-32768,-32768,-32768,-32768,
  -32768,-32768,    51,-32768,-32768,    51,-32768,-32768,    51,-32768,
  -32768,-32768,    51,-32768,   107,-32768,   119,     1,   109,   -20,
     108,   110,   112,    83,    84,    13,-32768,    88,    89,    86,
  -32768,   -38,    59,    72,    54,    42,    66,-32768,   147,-32768,
     107,   118,    51,   121,    51,   103,-32768,    51,-32768,-32768,
      63,-32768,   104,-32768,   145,-32768,    51,-32768,    11,-32768,
  -32768,   139,   133,-32768,    98,    51,   125,-32768,-32768,-32768,
     129,-32768,   127,-32768,   131,   134,    51,    11,   130,    51,
      51,   132,   160,-32768,    11,-32768,   135,   137,    51,   136,
     138,-32768,-32768,-32768,-32768,   162,   148,   167,-32768,   141,
     164,-32768,-32768,-32768,-32768,   142,-32768,-32768,-32768,   166,
      51,    11,   168,-32768,-32768,-32768,   153,   164,   117,   149,
      -9,   174,-32768,    11,    51,   175,   177,-32768,   150,   151,
     184,-32768,   169,   156,   181,    11,    11,   159,    11,-32768,
     161,-32768,   153,-32768,   120,-32768,-32768,-32768,   189,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,    46,-32768,-32768,   180,    76,-32768,-32768,   106,
  -32768,    90,   -70,  -145,    45,-32768,-32768,   -12,    -8,  -150,
      25,  -104,   -15,-32768,-32768,   152,-32768,   146,-32768,   143,
  -32768,   140,-32768,   128,-32768,   144,-32768,   124,-32768,   -42
};


#define	YYLAST		222


static const short yytable[] =
{
      34,    83,   167,    88,   112,    26,    27,    28,     7,    84,
     134,   198,    59,    60,   110,   144,   116,     8,    55,     9,
     184,   145,   143,   146,    26,    27,    28,   147,    84,   148,
      85,    89,   117,    33,   113,   149,   188,   192,    98,    99,
     110,   150,    10,   151,    26,    27,    28,    11,   201,   123,
     218,     2,    33,    12,    26,    27,    28,    24,    23,    13,
     211,   212,   114,   214,    29,    30,    49,     3,    53,    31,
      32,    54,    33,     4,    29,    30,   137,   138,    14,    31,
      32,     5,    33,    96,    26,    27,    28,    48,    15,    16,
      46,   139,    73,    74,    17,    76,    77,   132,    47,    98,
      18,    50,   136,    52,    29,    30,    62,    63,    64,    31,
      32,    98,    33,    66,    67,    68,    69,    70,    71,    51,
     160,    79,    80,    81,   193,   194,   195,   193,   194,    56,
      87,   166,    57,    90,   169,   170,    91,    92,    95,   100,
      18,   111,   115,   176,   118,   121,   119,   122,   199,   120,
     125,   127,   126,   128,   130,   135,   142,   133,   157,   158,
     159,   161,   162,   163,   164,   132,   168,   165,   171,   172,
     175,   174,   177,   181,   178,   180,   182,   183,   185,   202,
     147,   186,   190,   200,   189,   197,   204,   203,   207,   219,
     205,   206,   209,   210,   208,   213,    45,   215,   141,   173,
     129,   124,   217,   105,   216,   187,   107,   102,   103,     0,
     101,     0,   104,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   106
};

static const short yycheck[] =
{
      15,    43,   147,     5,     3,    33,    34,    35,    60,    15,
     114,    20,    50,    51,    84,     4,    36,    33,    33,    33,
     170,    10,   126,    12,    33,    34,    35,    16,    15,    18,
      36,    33,    52,    61,    33,    24,   181,   187,    53,    54,
     110,    30,    33,    32,    33,    34,    35,    15,   193,    36,
       0,     1,    61,    15,    33,    34,    35,    33,    12,    36,
     205,   206,    61,   208,    53,    54,    20,    17,    61,    58,
      59,    64,    61,    23,    53,    54,    13,    14,    11,    58,
      59,    31,    61,    62,    33,    34,    35,     9,    21,    22,
      33,    28,    38,    39,    27,    53,    54,   112,    37,   114,
      33,     9,   117,    24,    53,    54,    47,    48,    49,    58,
      59,   126,    61,    41,    42,    43,    44,    45,    46,    61,
     135,    55,    56,    57,     7,     8,     9,     7,     8,    36,
      15,   146,    52,    17,   149,   150,    23,    33,    33,    62,
      33,    22,    33,   158,    36,    62,    36,    63,   190,    37,
      62,    65,    63,     6,    36,    52,    11,    36,    19,    26,
      62,    36,    33,    36,    33,   180,    36,    33,    36,     9,
      33,    36,    36,    25,    36,    13,     9,    36,    36,   194,
      16,    15,    29,     9,    16,    36,     9,    12,     4,     0,
      40,    40,    36,    12,    25,    36,    16,    36,   122,   154,
     110,    95,   214,    75,   212,   180,    82,    61,    65,    -1,
      58,    -1,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    78
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
#line 167 "parse.y"
{
	  open_src(yyvsp[-1].s, source_with);
	}
    break;
case 5:
#line 172 "parse.y"
{
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;
case 6:
#line 177 "parse.y"
{
	  add_entity(mk_file(yyvsp[-3].t, yyvsp[-5].s, state.src->type));
	}
    break;
case 7:
#line 184 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 8:
#line 189 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 9:
#line 196 "parse.y"
{
	  yyval.t = mk_pragma(yyvsp[-1].t);
	}
    break;
case 10:
#line 201 "parse.y"
{
	  yyval.t = yyvsp[0].t;
        }
    break;
case 11:
#line 206 "parse.y"
{
	  yyval.t = yyvsp[0].t;
        }
    break;
case 12:
#line 211 "parse.y"
{ 
	  yyval.t = mk_proc(yyvsp[-3].t, yyvsp[-2].t);
     	}
    break;
case 13:
#line 216 "parse.y"
{ 
	  yyval.t = mk_proc(yyvsp[-1].t, NULL);
	}
    break;
case 14:
#line 221 "parse.y"
{ 
	  yyval.t = mk_func(yyvsp[-5].t, yyvsp[-3].s, yyvsp[-2].t);
	}
    break;
case 15:
#line 226 "parse.y"
{ 
	  yyval.t = mk_func(yyvsp[-3].t, yyvsp[-1].s, NULL);
	}
    break;
case 16:
#line 233 "parse.y"
{
	  /* alias */
	  yyval.t = mk_type(yyvsp[-3].s, NULL, NULL, NULL, yyvsp[-1].s);
        }
    break;
case 17:
#line 239 "parse.y"
{
	  /* array */
	  yyval.t = mk_type(yyvsp[-6].s, yyvsp[-3].r.start, yyvsp[-3].r.end, NULL, yyvsp[-1].s);
        }
    break;
case 18:
#line 245 "parse.y"
{
	  /* enumerated type */
	  yyval.t = mk_type(yyvsp[-5].s, NULL, NULL, yyvsp[-2].t, NULL);
        }
    break;
case 19:
#line 253 "parse.y"
{
 	  yyval.t = mk_head(yyvsp[0].s, NULL);
     	}
    break;
case 20:
#line 258 "parse.y"
{
 	  yyval.t = mk_head(yyvsp[-3].s, yyvsp[-1].t);
     	}
    break;
case 21:
#line 265 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 22:
#line 270 "parse.y"
{
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 23:
#line 277 "parse.y"
{	  
	  yyval.t = mk_arg(yyvsp[-3].s, yyvsp[-1].d, yyvsp[0].s);
        }
    break;
case 24:
#line 283 "parse.y"
{ yyval.d = dir_in; }
    break;
case 25:
#line 284 "parse.y"
{ yyval.d = dir_inout; }
    break;
case 26:
#line 285 "parse.y"
{ yyval.d = dir_out; }
    break;
case 27:
#line 290 "parse.y"
{
	  yyval.t = mk_body(yyvsp[-3].t, yyvsp[-1].t);
	}
    break;
case 28:
#line 297 "parse.y"
{
	  yyval.t = NULL;
	}
    break;
case 29:
#line 302 "parse.y"
{
          yyval.t = yyvsp[0].t;
	}
    break;
case 30:
#line 309 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 31:
#line 314 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 32:
#line 321 "parse.y"
{ 
	  yyval.t = mk_decl(yyvsp[-3].s, false, yyvsp[-1].s, NULL);
        }
    break;
case 33:
#line 326 "parse.y"
{ 
	  yyval.t = mk_decl(yyvsp[-5].s, false, yyvsp[-3].s, yyvsp[-1].t);
        }
    break;
case 34:
#line 331 "parse.y"
{ 
	  yyval.t = mk_decl(yyvsp[-6].s, true, yyvsp[-3].s, yyvsp[-1].t);
        }
    break;
case 35:
#line 338 "parse.y"
{
	  yyval.t = NULL;
	}
    break;
case 36:
#line 343 "parse.y"
{
	  yyval.t = yyvsp[0].t;
	}
    break;
case 37:
#line 350 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 38:
#line 355 "parse.y"
{
	  yyval.t = node_list(yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 39:
#line 362 "parse.y"
{
	  yyval.t = mk_assembly(strdup("  nop"));
	}
    break;
case 40:
#line 367 "parse.y"
{
	  yyval.t = mk_return(yyvsp[-1].t);
	}
    break;
case 41:
#line 372 "parse.y"
{
	  yyval.t = mk_assembly(yyvsp[-1].s);
	}
    break;
case 42:
#line 377 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-5].t, yyvsp[-3].t, NULL);
	}
    break;
case 43:
#line 382 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-6].t, yyvsp[-4].t, yyvsp[-3].t);
	}
    break;
case 44:
#line 387 "parse.y"
{
	  if (case_ident) {
	    yyerror("nested case statements are not yet supported");
	  }
	  case_ident = mk_symbol(yyvsp[0].s, NULL);
	}
    break;
case 45:
#line 394 "parse.y"
{
	  yyval.t = yyvsp[-3].t;
	  case_ident = NULL;
	}
    break;
case 46:
#line 400 "parse.y"
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
#line 419 "parse.y"
{
	  yyval.t= mk_loop(NULL, yyvsp[-1].t, NULL, yyvsp[0].t);	
	}
    break;
case 48:
#line 424 "parse.y"
{
	  yyval.t= mk_loop(NULL, NULL, NULL, yyvsp[0].t);
	}
    break;
case 49:
#line 429 "parse.y"
{
	  yyval.t = yyvsp[-1].t;
	}
    break;
case 50:
#line 436 "parse.y"
{
	  /* last statement is elsif */
	  yyval.t = mk_cond(yyvsp[-2].t, yyvsp[0].t, NULL);
	}
    break;
case 51:
#line 442 "parse.y"
{
	  yyval.t = mk_cond(yyvsp[-3].t, yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 52:
#line 447 "parse.y"
{
	  /* last statement is else */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;
case 53:
#line 455 "parse.y"
{
	  /* last statement is elsif equivalent */
	  yyval.t = mk_cond(mk_binop(op_eq, case_ident, yyvsp[-2].t), yyvsp[0].t, NULL);
	}
    break;
case 54:
#line 461 "parse.y"
{
	  yyval.t = mk_cond(mk_binop(op_eq, case_ident, yyvsp[-3].t), yyvsp[-1].t, yyvsp[0].t);
	}
    break;
case 55:
#line 466 "parse.y"
{
	  /* last statement is else equivalent */
	  yyval.t = mk_cond(NULL, yyvsp[0].t, NULL);
	}
    break;
case 56:
#line 474 "parse.y"
{
	  yyval.t = yyvsp[-3].t;
	}
    break;
case 57:
#line 481 "parse.y"
{
	  yyval.r.start = yyvsp[-2].t;
	  yyval.r.end = yyvsp[0].t;
        }
    break;
case 58:
#line 489 "parse.y"
{
	  yyval.t = node_list(yyvsp[0].t, NULL);
	}
    break;
case 59:
#line 494 "parse.y"
{
	  yyval.t = node_list(yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 62:
#line 505 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 63:
#line 510 "parse.y"
{ yyval.o = op_assign; }
    break;
case 65:
#line 516 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 66:
#line 521 "parse.y"
{ yyval.o = op_land; }
    break;
case 67:
#line 522 "parse.y"
{ yyval.o = op_lor; }
    break;
case 69:
#line 528 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 70:
#line 533 "parse.y"
{ yyval.o = op_and; }
    break;
case 71:
#line 534 "parse.y"
{ yyval.o = op_or; }
    break;
case 72:
#line 535 "parse.y"
{ yyval.o = op_xor; }
    break;
case 74:
#line 541 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 75:
#line 546 "parse.y"
{ yyval.o = op_lt; }
    break;
case 76:
#line 547 "parse.y"
{ yyval.o = op_gt; }
    break;
case 77:
#line 548 "parse.y"
{ yyval.o = op_eq; }
    break;
case 78:
#line 549 "parse.y"
{ yyval.o = op_ne; }
    break;
case 79:
#line 550 "parse.y"
{ yyval.o = op_gte; }
    break;
case 80:
#line 551 "parse.y"
{ yyval.o = op_lte; }
    break;
case 82:
#line 557 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 83:
#line 562 "parse.y"
{ yyval.o = op_lsh; }
    break;
case 84:
#line 563 "parse.y"
{ yyval.o = op_rsh; }
    break;
case 86:
#line 569 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 87:
#line 574 "parse.y"
{ yyval.o = op_add; }
    break;
case 88:
#line 575 "parse.y"
{ yyval.o = op_sub; }
    break;
case 90:
#line 581 "parse.y"
{
	  yyval.t = mk_binop(yyvsp[-1].o, yyvsp[-2].t, yyvsp[0].t);
	}
    break;
case 91:
#line 586 "parse.y"
{ yyval.o = op_mult; }
    break;
case 92:
#line 587 "parse.y"
{ yyval.o = op_div; }
    break;
case 93:
#line 588 "parse.y"
{ yyval.o = op_mod; }
    break;
case 95:
#line 594 "parse.y"
{
	  yyval.t = mk_unop(yyvsp[-1].o, yyvsp[0].t);
	}
    break;
case 96:
#line 599 "parse.y"
{ yyval.o = op_neg; }
    break;
case 97:
#line 600 "parse.y"
{ yyval.o = op_not; }
    break;
case 98:
#line 601 "parse.y"
{ yyval.o = op_com; }
    break;
case 99:
#line 602 "parse.y"
{ yyval.o = op_add; }
    break;
case 100:
#line 606 "parse.y"
{
	  yyval.t = mk_string(yyvsp[0].s);
        }
    break;
case 101:
#line 611 "parse.y"
{
	  yyval.t = mk_symbol(yyvsp[0].s, NULL);
        }
    break;
case 102:
#line 616 "parse.y"
{
	  yyval.t = mk_symbol(yyvsp[-3].s, yyvsp[-1].t);
        }
    break;
case 103:
#line 621 "parse.y"
{
	  /* function or procedure call */
	  yyval.t = mk_call(yyvsp[-2].s, NULL);
	}
    break;
case 104:
#line 627 "parse.y"
{
	  /* function or procedure call with arguments */
	  yyval.t = mk_call(yyvsp[-3].s, yyvsp[-1].t);
	}
    break;
case 105:
#line 633 "parse.y"
{
	  yyval.t = mk_constant(yyvsp[0].i);
	}
    break;
case 106:
#line 638 "parse.y"
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
#line 643 "parse.y"


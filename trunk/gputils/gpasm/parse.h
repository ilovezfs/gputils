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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 218 "parse.y"
typedef union YYSTYPE {
  gpasmVal i;
  char *s;
  struct pnode *p;
} YYSTYPE;
/* Line 1275 of yacc.c.  */
#line 131 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;




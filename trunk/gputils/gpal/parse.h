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
/* Line 1275 of yacc.c.  */
#line 141 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;




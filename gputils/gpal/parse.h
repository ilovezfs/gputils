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
/* Line 1275 of yacc.c.  */
#line 143 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;




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
#line 145 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;




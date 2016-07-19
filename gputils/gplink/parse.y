%{
/* Parser for linker scripts
   Copyright (C) 2001, 2002, 2003, 2004, 2005
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
#include "gplink.h"
#include "scan.h"
#include "script.h"

void yyerror(char *Message)
{
  script_error(Message, NULL);
}

int yylex(void);

/************************************************************************/

/* Some simple functions for building parse trees */

static pnode_t *
_mk_pnode(enum pnode_tag Tag)
{
  pnode_t *new = GP_Malloc(sizeof(*new));

  new->tag = Tag;
  return new;
}

static pnode_t *
_mk_constant(long Value)
{
  pnode_t *new = _mk_pnode(PTAG_CONSTANT);

  PnConstant(new) = Value;
  return new;
}

static pnode_t *
_mk_symbol(const char *String)
{
  pnode_t *new = _mk_pnode(PTAG_SYMBOL);

  PnSymbol(new) = String;
  return new;
}

/*
static pnode_t *
_mk_string(const char *String)
{
  pnode_t *new = _mk_pnode(PTAG_STRING);

  PnString(new) = String;
  return new;
}

*/

static pnode_t *
_mk_list(pnode_t *Head, pnode_t *Tail)
{
  pnode_t *new = _mk_pnode(PTAG_LIST);

  PnListHead(new) = Head;
  PnListTail(new) = Tail;
  return new;
}

static pnode_t *
_mk_2op(int Op, pnode_t *Pnode0, pnode_t *Pnode1)
{
  pnode_t *new = _mk_pnode(PTAG_BINOP);

  PnBinOpOp(new) = Op;
  PnBinOpP0(new) = Pnode0;
  PnBinOpP1(new) = Pnode1;
  return new;
}

/*
static pnode_t *
_mk_1op(int Op, pnode_t *Pnode)
{
  pnode_t *new = _mk_pnode(PTAG_UNOP);

  PnUnOpOp(new) = Op;
  PnUnOpP0(new) = Pnode;
  return new;
}

*/

%}

/* Bison declarations */

%union {
  int           i;
  long          l;
  char         *s;
  struct pnode *p;
}

%token <s> SYMBOL
%token <s> LIBPATH
%token <s> LKRPATH
%token <s> STRING
%token <s> LEXEOF 0 "end of file"
%token <s> ERROR
%token <l> NUMBER
%token DEFINE
%token IFDEF
%token ELSE
%token FI
%type <i> '='
%type <i> '+'
%type <i> '-'
%type <i> '/'
%type <i> '*'
%type <i> e1op
%type <l> macroval
%type <i> aop

%type <p> e0
%type <p> e1
%type <p> parameter_list
%type <p> path_list 

%%
/* Grammar rules */

program:
        line
        | error
        | program '\n' {
          ++state.src->line_number;
        } line
        ;

line:
        /* empty */
        |
        LIBPATH path_list
        {
          if ((state.ifdef == NULL) || (state.ifdef->is_true)) {
            script_add_path($2);
          }
        }
        |
        LKRPATH path_list
        {
          if ((state.ifdef == NULL) || (state.ifdef->is_true)) {
            script_add_path($2);
          }
        }
        |
        SYMBOL parameter_list
        {
          if ((state.ifdef == NULL) || (state.ifdef->is_true)) {
            script_execute_command($1, $2);
          }
        }
        |
        STRING parameter_list
        {
          if ((state.ifdef == NULL) || (state.ifdef->is_true)) {
            script_execute_command($1, $2);
          }
        }
        |
        ERROR
        {
          if ((state.ifdef == NULL) || (state.ifdef->is_true)) {
            yyerror($1);
          }
        }
        |
        DEFINE SYMBOL macroval aop macroval
        {
          /* Contrary to documentation, the mplink 4.38 does not seem
             to care if the macro has already been defined or if the
             parameters to the operation are undefined. */
          if ((state.ifdef == NULL) || (state.ifdef->is_true)) {
            long newval = 0, lh = $3, rh = $5;

            switch ($4) {
              case '+': newval = lh + rh; break;
              case '-': newval = lh - rh; break;
              case '*': newval = lh * rh; break;
              case '/': {
                if (rh == 0) {
                  yyerror("Division by zero.");
                }
                else {
                  newval = lh / rh;
                }
                break;
              }
            }
            script_add_macro($2, newval);
          }
        }
        |
        IFDEF SYMBOL
        {
          struct ifdef *ifdef = GP_Malloc(sizeof *ifdef);

          ifdef->is_true = (((state.ifdef == NULL) || state.ifdef->is_true) &&
                            gp_sym_get_symbol(state.script_symbols, $2));
          ifdef->in_else = false;
          ifdef->prev = state.ifdef;
          state.ifdef = ifdef;
        }
        |
        ELSE
        {
          if ((state.ifdef == NULL) || (state.ifdef->in_else)) {
            yyerror("#ELSE without #IFDEF in linker script.");
          }
          else {
            state.ifdef->is_true = ((!state.ifdef->is_true) &&
                                    (state.ifdef->prev == NULL || state.ifdef->prev->is_true));
            state.ifdef->in_else = true;
          }
        }
        |
        FI
        {
          if (state.ifdef == NULL)
            yyerror("#FI without #IFDEF in linker script.");
          else {
            struct ifdef *ifdef = state.ifdef;

            state.ifdef = state.ifdef->prev;
            free(ifdef);
          }
        }
        ;

path_list:
        SYMBOL
        {
          $$ = _mk_list(_mk_symbol($1), NULL);
        }
        |
        SYMBOL ';' path_list
        {
          $$ = _mk_list(_mk_symbol($1), $3);
        }
        |
        STRING
        {
          $$ = _mk_list(_mk_symbol($1), NULL);
        }
        |
        STRING ';' path_list
        {
          $$ = _mk_list(_mk_symbol($1), $3);
        }
        ;

parameter_list:
        e1
        {
          $$ = _mk_list($1, NULL);
        }
        |
        e1  parameter_list
        {
          $$ = _mk_list($1, $2);
        }
        ;

e1:
        e0
        |
        e1 e1op e0
        {
          $$ = _mk_2op($2, $1, $3);
        }
        ;

e1op:   '=' ;

e0:
        SYMBOL
        {
          $$ = _mk_symbol($1);
        }
        |
        STRING
        {
          $$ = _mk_symbol($1);
        }
        |
        NUMBER
        {
          $$ = _mk_constant($1);
        }
        ;

macroval:
        SYMBOL
        {
          $$ = script_get_macro($1);
        }
        |
        NUMBER
        ;

aop:    '+' | '-' | '/' | '*' ;

%%

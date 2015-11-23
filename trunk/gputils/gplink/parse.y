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

void yyerror(char *message)
{
  script_error(message, NULL);
}

int yylex(void);

/************************************************************************/

/* Some simple functions for building parse trees */

static struct pnode *mk_pnode(enum pnode_tag tag)
{
  struct pnode *new = GP_Malloc(sizeof(*new));

  new->tag = tag;
  return new;
}

struct pnode *mk_constant(long value)
{
  struct pnode *new = mk_pnode(PTAG_CONSTANT);

  new->value.constant = value;
  return new;
}

static struct pnode *mk_symbol(const char *value)
{
  struct pnode *new = mk_pnode(PTAG_SYMBOL);

  new->value.symbol = value;
  return new;
}

/*
static struct pnode *mk_string(const char *value)
{
  struct pnode *new = mk_pnode(PTAG_STRING);

  new->value.string = value;
  return new;
}

*/

struct pnode *mk_list(struct pnode *head, struct pnode *tail)
{
  struct pnode *new = mk_pnode(PTAG_LIST);

  new->value.list.head = head;
  new->value.list.tail = tail;
  return new;
}

static struct pnode *mk_2op(int op, struct pnode *p0, struct pnode *p1)
{
  struct pnode *new = mk_pnode(PTAG_BINOP);

  new->value.binop.op = op;
  new->value.binop.p0 = p0;
  new->value.binop.p1 = p1;
  return new;
}

/*
static struct pnode *mk_1op(int op, struct pnode *p0)
{
  struct pnode *new = mk_pnode(PTAG_UNOP);

  new->value.unop.op = op;
  new->value.unop.p0 = p0;
  return new;
}

*/

%}

/* Bison declarations */

%union {
  int i;
  long l;
  char *s;
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
	  if (state.ifdef == NULL || state.ifdef->istrue)
	    add_path($2);
	}
	|
	LKRPATH path_list
	{
	  if (state.ifdef == NULL || state.ifdef->istrue)
	    add_path($2);
	}
	|
	SYMBOL parameter_list
	{
	  if (state.ifdef == NULL || state.ifdef->istrue)
	    execute_command($1, $2);
	}
	|
	STRING parameter_list
	{
	  if (state.ifdef == NULL || state.ifdef->istrue)
	    execute_command($1, $2);
	}
	|
	ERROR
	{
	  if (state.ifdef == NULL || state.ifdef->istrue)
	    yyerror($1);
	}
	|
	DEFINE SYMBOL macroval aop macroval
	{
	  /* Contrary to documentation, the mplink 4.38 does not seem
	     to care if the macro has already been defined or if the
	     parameters to the operation are undefined. */
	  if (state.ifdef == NULL || state.ifdef->istrue) {
	    long newval = 0, lh = $3, rh = $5;

	    switch($4) {
	    case '+': newval = lh + rh; break;
	    case '-': newval = lh - rh; break;
	    case '*': newval = lh * rh; break;
	    case '/':
	      if (rh == 0)
		yyerror("Division by zero.");
	      else
		newval = lh / rh;
	      break;
	    }
	    add_script_macro($2, newval);
	  }
	}
	|
	IFDEF SYMBOL
	{
	  struct ifdef *ifdef = GP_Malloc(sizeof *ifdef);

	  ifdef->istrue = (((state.ifdef == NULL) || state.ifdef->istrue) &&
			   get_symbol(state.script_symbols, $2));
	  ifdef->inelse = false;
	  ifdef->prev = state.ifdef;
	  state.ifdef = ifdef;
	}
	|
	ELSE
	{
	  if (state.ifdef == NULL || state.ifdef->inelse)
	    yyerror("#ELSE without #IFDEF in linker script.");
	  else {
	    state.ifdef->istrue = (!state.ifdef->istrue &&
				   (state.ifdef->prev == NULL ||
				    state.ifdef->prev->istrue));
	    state.ifdef->inelse = true;
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
	  $$ = mk_list(mk_symbol($1), NULL);
	}
	|
	SYMBOL ';' path_list
	{
	  $$ = mk_list(mk_symbol($1), $3);
	}
	|
	STRING
	{
	  $$ = mk_list(mk_symbol($1), NULL);
	}
	|
	STRING ';' path_list
	{
	  $$ = mk_list(mk_symbol($1), $3);
	}
	;

parameter_list:
	e1
	{
	  $$ = mk_list($1, NULL);
	}
	|
	e1  parameter_list
	{
	  $$ = mk_list($1, $2);
	}
	;

e1:
	e0
	|
	e1 e1op e0
	{
	  $$ = mk_2op($2, $1, $3);
	}
	;

e1op:	'=' ;

e0:
	SYMBOL
        {
	  $$ = mk_symbol($1);
        }
	|
	STRING
        {
	  $$ = mk_symbol($1);
        }
	|
	NUMBER
	{
	  $$ = mk_constant($1);
	}
	;

macroval:
	SYMBOL
	{
	  $$ = get_script_macro($1);
	}
	|
	NUMBER
	;

aop:	'+' | '-' | '/' | '*' ;

%%

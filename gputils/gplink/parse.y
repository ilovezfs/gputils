%{
/* Parser for linker scripts
   Copyright (C) 2001 Craig Franklin

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
  gp_error(message);
}

int yylex(void);

/************************************************************************/

/* Some simple functions for building parse trees */

static struct pnode *mk_pnode(enum pnode_tag tag)
{
  struct pnode *new = malloc(sizeof(*new));
  new->tag = tag;
  return new;
}

struct pnode *mk_constant(int value)
{
  struct pnode *new = mk_pnode(constant);
  new->value.constant = value;
  return new;
}

static struct pnode *mk_symbol(char *value)
{
  struct pnode *new = mk_pnode(symbol);
  new->value.symbol = value;
  return new;
}

/*
static struct pnode *mk_string(char *value)
{
  struct pnode *new = mk_pnode(string);
  new->value.string = value;
  return new;
}

*/

struct pnode *mk_list(struct pnode *head, struct pnode *tail)
{
  struct pnode *new = mk_pnode(list);
  new->value.list.head = head;
  new->value.list.tail = tail;
  return new;
}

static struct pnode *mk_2op(int op, struct pnode *p0, struct pnode *p1)
{
  struct pnode *new = mk_pnode(binop);
  new->value.binop.op = op;
  new->value.binop.p0 = p0;
  new->value.binop.p1 = p1;
  return new;
}

/*
static struct pnode *mk_1op(int op, struct pnode *p0)
{
  struct pnode *new = mk_pnode(unop);  new->value.unop.op = op;
  new->value.unop.p0 = p0;
  return new;
}

*/

%}

/* Bison declarations */

%union {
  int i;
  char *s;
  struct pnode *p;
}

%token <s> SYMBOL, LIBPATH, LKRPATH
%token <s> PATH
%token <i> NUMBER
%type <i> '='
%type <i> e1op

%type <s> line
%type <p> e0, e1
%type <p> parameter_list, path_list 

%%
/* Grammar rules */

program:
	/* can be nothing */
	|
	program 
        { 
          /* do nothing */
        } line
	| program error '\n'
	{ 
          /* do nothing */
	}
	;

line:
	'\n'
	{
          /* do nothing */
	}
	|
	LIBPATH path_list '\n'
	{

	}
	|
	LKRPATH path_list '\n'
	{

	}
	|
	SYMBOL parameter_list '\n'
	{
	  execute_command($1, $2);
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
	NUMBER
	{
	  $$ = mk_constant($1);
	}
	;

%%

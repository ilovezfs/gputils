%{
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

%}

/* Bison declarations */

%union {
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
}

/* keywords */
%token <i> ARRAY, CASE, CONSTANT_KEY, BEGIN_KEY, ELSE, ELSIF
%token <i> END, FOR, FUNCTION_TOK, IF, IN, INOUT, IS, LOOP
%token <i> MODULE, NULL_TOK, OF, OTHERS, PRAGMA, PROCEDURE
%token <i> PUBLIC_STORAGE, RETURN, THEN, TO, TYPE, OUT, VARIABLE_KEY
%token <i> WHEN, WHILE, WITH

/* general */
%token <s> ASM
%token <s> IDENT
%token <i> NUMBER
%token <s> STRING
%token <i> ';', ':'

/* operators */
%token <i> LSH, RSH, ARROW
%token <i> GREATER_EQUAL, LESS_EQUAL, EQUAL, NOT_EQUAL, '<', '>'
%token <i> '&', '|', '^'
%token <i> LOGICAL_AND, LOGICAL_OR
%token <i> '='

/* types */
%type <s> entity
%type <t> element_list
%type <t> element
%type <i> '+', '-', '*', '/', '%', '!', '~'
%type <t> expr, e0, e1, e2, e3, e4, e5, e6, e7, e8,
%type <o> e1op, e2op, e3op, e4op, e5op, e6op, e7op, e8op
%type <t> type
%type <t> head
%type <t> arg_list
%type <t> arg
%type <d> arg_direction
%type <t> body
%type <t> decl_block
%type <t> decl
%type <k> decl_key
%type <t> statement_block
%type <t> statement
%type <r> range
%type <t> if_body
%type <t> case_body
%type <t> loop_statement
%type <t> parameter_list

%start program

%%
/* Grammar rules */

program:
	/* can be nothing */
	|
	program entity
	| 
	program error '\n'
	;

entity:
	WITH IDENT ';'
	{
	  open_src($2, source_with);
	}
	|
	MODULE IDENT IS END MODULE ';'
	{
	  gp_warning("empty module");
	}
	|
	MODULE IDENT IS element_list END MODULE ';'
	{
	  add_entity(mk_file($4, $2, state.src->type));
	}
	|
	PUBLIC_STORAGE IDENT IS END PUBLIC_STORAGE ';'
	{
	  gp_warning("empty public");
	}
	|
	PUBLIC_STORAGE IDENT IS element_list END PUBLIC_STORAGE ';'
	{
	  add_entity(mk_file($4, $2, state.src->type));
	}
        ;

element_list:
	element
	{
	  $$ = node_list($1, NULL);
	}
	|
	element element_list
	{
	  $$ = node_list($1, $2);
	}
	;

element:
	PRAGMA expr ';'
	{
	  $$ = mk_pragma($2);
	}
	|
	type
	{
	  $$ = $1;
        }
	|
	decl
	{
	  $$ = $1;
        }
	|
	PROCEDURE head body PROCEDURE ';'
	{ 
	  $$ = mk_proc($2, $3);
     	}
	|
	PROCEDURE head ';'
	{ 
	  $$ = mk_proc($2, NULL);
     	}
        |
	FUNCTION_TOK head RETURN IDENT body FUNCTION_TOK ';'
	{ 
	  $$ = mk_func($2, $4, $5);
     	}
        |
	FUNCTION_TOK head RETURN IDENT ';'
	{ 
	  $$ = mk_func($2, $4, NULL);
     	}
	;

type:
	TYPE IDENT IS IDENT ';'
	{
	  /* alias */
	  $$ = mk_type($2, NULL, NULL, NULL, $4);
        }
	|
	TYPE IDENT IS ARRAY range OF IDENT ';'
	{
	  /* array */
	  $$ = mk_type($2, $5.start, $5.end, NULL, $7);
        }
	|
	TYPE IDENT IS '(' parameter_list ')' ';'
	{
	  /* enumerated type */
	  $$ = mk_type($2, NULL, NULL, $5, NULL);
        }
        ;

head:
	IDENT
	{
 	  $$ = mk_head($1, NULL);
     	}
	|
	IDENT '(' arg_list ')'
	{
 	  $$ = mk_head($1, $3);
     	}
        ;

arg_list:
	arg
	{
	  $$ = node_list($1, NULL);
	}
	|
	arg ',' arg_list
	{
	  $$ = node_list($1, $3);
	}
	;	

arg:
	IDENT ':' arg_direction IDENT 
	{	  
	  $$ = mk_arg($1, $3, $4);
        }
	;

arg_direction:
	  IN    { $$ = dir_in; }
	| INOUT { $$ = dir_inout; }
	| OUT   { $$ = dir_out; }
	;

body:
	IS decl_block BEGIN_KEY statement_block END
	{
 	  $$ = mk_body($2, $4);
     	}
	|
	IS BEGIN_KEY statement_block END
	{
 	  $$ = mk_body(NULL, $3);
     	}
        ;

decl_block:
	decl
	{
	  $$ = node_list($1, NULL);
	}
	|
        decl decl_block
	{
	  $$ = node_list($1, $2);
	}
	;

decl:

	decl_key IDENT ':' IDENT ';'
	{ 
	  $$ = mk_decl($1, $4, $2, NULL);
        }
	|
	decl_key IDENT ':' IDENT '=' expr ';'
	{ 
	  $$ = mk_decl($1, $4, $2, $6);
        }
	;

decl_key:
	CONSTANT_KEY
	{
	  $$ = key_constant;
	}
	|
	VARIABLE_KEY
	{
	  $$ = key_variable;
	}
	;

statement_block:
	statement
	{
	  $$ = node_list($1, NULL);
	}
	|
	statement statement_block
	{
	  $$ = node_list($1, $2);
	}
	;

statement:
	NULL_TOK ';'
	{
	  $$ = mk_assembly(strdup("  nop"));
	}
	|
	RETURN expr ';'
	{
	  $$ = mk_return($2);
	}
	|
	ASM ';'
	{
	  $$ = mk_assembly($1);
	}
	|
	IF expr THEN statement_block END IF ';'
	{
	  $$ = mk_cond($2, $4, NULL);
	}
	|
	IF expr THEN statement_block if_body END IF ';'
	{
	  $$ = mk_cond($2, $4, $5);
	}
	|
	CASE IDENT { case_ident = mk_symbol($2, NULL); } IS case_body END CASE ';'
	{
	  $$ = $5;
	}
	|
	FOR IDENT IN range loop_statement
	{
	  tree *init;
          tree *exit;
          tree *increment;
          
          /* IDENT = range.start; */
          init = mk_binop(op_assign, mk_symbol($2, NULL), $4.start);
          
          /* IDENT = IDENT + 1; */
          increment = mk_binop(op_assign, mk_symbol($2, NULL), 
                               mk_binop(op_add, mk_symbol($2, NULL), mk_constant(1)));
          
          /* while (IDENT <= range.end) then loop */
          exit = mk_binop(op_lte, mk_symbol($2, NULL), $4.end);

	  $$ = mk_loop(init, exit, increment, $5);
	}
	|
	WHILE expr loop_statement
	{
	  $$= mk_loop(NULL, $2, NULL, $3);	
	}
	|
	loop_statement
	{
	  $$= mk_loop(NULL, NULL, NULL, $1);
	}
	|
	expr ';'
	{
	  $$ = $1;
	}
	;

if_body:
	ELSIF expr THEN statement_block
	{
	  /* last statement is elsif */
	  $$ = mk_cond($2, $4, NULL);
	}
	|
	ELSIF expr THEN statement_block if_body
	{
	  $$ = mk_cond($2, $4, $5);
	}
	|
	ELSE statement_block
	{
	  /* last statement is else */
	  $$ = mk_cond(NULL, $2, NULL);
	}	
	;

case_body:
	WHEN e0 ARROW statement_block
	{
	  /* last statement is elsif equivalent */
	  $$ = mk_cond(mk_binop(op_eq, case_ident, $2), $4, NULL);
	}
	|
	WHEN e0 ARROW statement_block case_body
	{
	  $$ = mk_cond(mk_binop(op_eq, case_ident, $2), $4, $5);
	}
	|
	WHEN OTHERS ARROW statement_block
	{
	  /* last statement is else equivalent */
	  $$ = mk_cond(NULL, $4, NULL);
	}	
	;

loop_statement:
	LOOP statement_block END LOOP ';'
	{
	  $$ = $2;
	}
	;

range:
	expr TO expr
	{
	  $$.start = $1;
	  $$.end = $3;
        }
	;

parameter_list:
	expr
	{
	  $$ = node_list($1, NULL);
	}
	|
	expr ',' parameter_list
	{
	  $$ = node_list($1, $3);
	}
	;

expr:	e8;

e8:
	e7
	|
	e8 e8op e7
	{
	  $$ = mk_binop($2, $1, $3);
	}
	;

e8op:	'=' { $$ = op_assign; };

e7:
	e6
	|
	e7 e7op e6
	{
	  $$ = mk_binop($2, $1, $3);
	}
	;

e7op:	  LOGICAL_AND { $$ = op_land; }
	| LOGICAL_OR  { $$ = op_lor; };

e6:
	e5
	|
	e6 e6op e5
	{
	  $$ = mk_binop($2, $1, $3);
	}
	;

e6op:	  '&' { $$ = op_and; }
	| '|' { $$ = op_or; }
	| '^' { $$ = op_xor; };

e5:
	e4
	|
	e5 e5op e4
	{
	  $$ = mk_binop($2, $1, $3);
	}
	;

e5op:	  '<'           { $$ = op_lt; }
	| '>'           { $$ = op_gt; }
	| EQUAL         { $$ = op_eq; }
	| NOT_EQUAL     { $$ = op_ne; }
	| GREATER_EQUAL { $$ = op_gte; }
	| LESS_EQUAL    { $$ = op_lte; };

e4:
	e3
	|
	e4 e4op e3
	{
	  $$ = mk_binop($2, $1, $3);
	}
	;

e4op:	  LSH { $$ = op_lsh; }
	| RSH { $$ = op_rsh; };

e3:
	e2
	|
	e3 e3op e2
	{
	  $$ = mk_binop($2, $1, $3);
	}
	;

e3op:     '+' { $$ = op_add; }
	| '-' { $$ = op_sub; };

e2:
	e1
	|
	e2 e2op e1
	{
	  $$ = mk_binop($2, $1, $3);
	}
	;

e2op:     '*' { $$ = op_mult; }
	| '/' { $$ = op_div; }
	| '%' { $$ = op_mod; };

e1:
	e0
	|
	e1op e0
	{
	  $$ = mk_unop($1, $2);
	}
	;

e1op:	  '-' { $$ = op_neg; }
	| '!' { $$ = op_not; }
	| '~' { $$ = op_com; }
	| '+' { $$ = op_add; };

e0:
	STRING
        {
	  $$ = mk_string($1);
        }
	|
	IDENT
        {
	  $$ = mk_symbol($1, NULL);
        }
	|
	IDENT '[' expr ']'
        {
	  $$ = mk_symbol($1, $3);
        }
	|
	IDENT '(' ')'
	{
	  /* function or procedure call */
	  $$ = mk_call($1, NULL);
	}
	|
	IDENT '(' parameter_list ')'
	{
	  /* function or procedure call with arguments */
	  $$ = mk_call($1, $3);
	}
	|
	NUMBER
	{
	  $$ = mk_constant($1);
	}
	|
	'(' expr ')'
	{
	  $$ = $2;
	}
	;

%%

%{
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
  enum node_op o;
}

/* keywords */
%token <i> ALIAS     "alias"
%token <i> ARRAY     "array"
%token <i> CASE      "case"
%token <i> CONSTANT  "constant"
%token <i> BEGIN_TOK "begin"
%token <i> ELSE      "else"
%token <i> ELSIF     "elsif"
%token <i> END       "end"
%token <i> FOR       "for"
%token <i> FUNCTION  "function"
%token <i> GOTO      "goto"
%token <i> IF        "if"
%token <i> IN        "in"
%token <i> INOUT     "inout"
%token <i> IS        "is"
%token <i> LOOP      "loop"
%token <i> MODULE    "module"
%token <i> NULL_TOK  "null"
%token <i> OF        "of"
%token <i> OTHERS    "others"
%token <i> PRAGMA    "pragma"
%token <i> PROCEDURE "procedure"
%token <i> PUBLIC    "public"
%token <i> RETURN    "return"
%token <i> THEN      "then"
%token <i> TO        "to"
%token <i> TYPE      "type"
%token <i> OUT       "out"
%token <i> WHEN      "when"
%token <i> WHILE     "while"
%token <i> WITH      "with"

/* general */
%token <s> ASM       "asm"
%token <s> IDENT     "symbol"
%token <s> LABEL     "label"
%token <i> NUMBER    "number"
%token <s> STRING    "string"
%token <i> ';'
%token <i> ':'

/* operators */
%token <i> LSH           "<<"
%token <i> RSH           ">>"
%token <i> ARROW         "=>"
%token <i> GREATER_EQUAL ">="
%token <i> LESS_EQUAL    "<="
%token <i> EQUAL         "=="
%token <i> NOT_EQUAL     "!="
%token <i> '<'
%token <i> '>'
%token <i> '&'
%token <i> '|'
%token <i> '^'
%token <i> LOGICAL_AND   "&&"
%token <i> LOGICAL_OR    "||"
%token <i> '='

/* types */
%type <s> entity
%type <t> element_list
%type <t> element
%type <t> type
%type <t> head
%type <t> arg_list
%type <t> arg
%type <d> arg_direction
%type <t> body
%type <t> decl_start
%type <t> decl_block
%type <t> decl
%type <t> statement_start
%type <t> statement_block
%type <t> statement
%type <r> range
%type <t> if_body
%type <t> case_body
%type <t> loop_statement
%type <t> parameter_list
%type <i> '+'
%type <i> '-'
%type <i> '*'
%type <i> '/'
%type <i> '%'
%type <i> '!'
%type <i> '~'
%type <t> expr
%type <t> e0
%type <t> e1
%type <t> e2
%type <t> e3
%type <t> e4
%type <t> e5
%type <t> e6
%type <t> e7
%type <t> e8
%type <o> e1op
%type <o> e2op
%type <o> e3op
%type <o> e4op
%type <o> e5op
%type <o> e6op
%type <o> e7op
%type <o> e8op

%start program

%%
/* Grammar rules */

program:
	/* empty */
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
	MODULE IDENT IS element_list END MODULE ';'
	{
	  add_entity(mk_file($4, $2, state.src->type));
	}
	|
	PUBLIC IDENT IS element_list END PUBLIC ';'
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
	ALIAS IDENT IDENT ';'
	{
	  $$ = mk_alias($2, $3);
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
	FUNCTION head RETURN IDENT body FUNCTION ';'
	{ 
	  $$ = mk_func($2, $4, $5);
	}
	|
	FUNCTION head RETURN IDENT ';'
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
	IS decl_start BEGIN_TOK statement_start END
	{
	  $$ = mk_body($2, $4);
	}
	;

decl_start:
	/* empty */
	{
	  $$ = NULL;
	}
	|
	decl_block
	{
          $$ = $1;
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
	IDENT ':' IDENT ';'
	{ 
	  $$ = mk_decl($1, false, $3, NULL);
        }
	|
	IDENT ':' IDENT '=' expr ';'
	{ 
	  $$ = mk_decl($1, false, $3, $5);
        }
	|
	IDENT ':' CONSTANT IDENT '=' expr ';'
	{ 
	  $$ = mk_decl($1, true, $4, $6);
        }
	;

statement_start:
	/* empty */
	{
	  $$ = NULL;
	}
	|
	statement_block
	{
	  $$ = $1;
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
	LABEL
	{
	  $$ = mk_label($1);
	}
	|
	GOTO IDENT ';'
	{
	  $$ = mk_goto($2);
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
	IF expr THEN statement_start END IF ';'
	{
	  $$ = mk_cond($2, $4, NULL);
	}
	|
	IF expr THEN statement_start if_body END IF ';'
	{
	  $$ = mk_cond($2, $4, $5);
	}
	|
	CASE IDENT
	{
	  if (case_ident) {
	    yyerror("nested case statements are not yet supported");
	  }
	  case_ident = mk_symbol($2, NULL);
	}
	IS case_body END CASE ';'
	{
	  $$ = $5;
	  case_ident = NULL;
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
	ELSIF expr THEN statement_start
	{
	  /* last statement is elsif */
	  $$ = mk_cond($2, $4, NULL);
	}
	|
	ELSIF expr THEN statement_start if_body
	{
	  $$ = mk_cond($2, $4, $5);
	}
	|
	ELSE statement_start
	{
	  /* last statement is else */
	  $$ = mk_cond(NULL, $2, NULL);
	}	
	;

case_body:
	WHEN e0 ARROW statement_start
	{
	  /* last statement is elsif equivalent */
	  $$ = mk_cond(mk_binop(op_eq, case_ident, $2), $4, NULL);
	}
	|
	WHEN e0 ARROW statement_start case_body
	{
	  $$ = mk_cond(mk_binop(op_eq, case_ident, $2), $4, $5);
	}
	|
	WHEN OTHERS ARROW statement_start
	{
	  /* last statement is else equivalent */
	  $$ = mk_cond(NULL, $4, NULL);
	}	
	;

loop_statement:
	LOOP statement_start END LOOP ';'
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

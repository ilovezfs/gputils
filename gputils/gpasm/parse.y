%{
/* Parser for gpasm
   Copyright (C) 1998,1999,2000,2001 James Bowman, Craig Franklin

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

#include "gpasm.h"
#include "gpsymbol.h"
#include "gperror.h"
#include "directive.h"
#include "lst.h"

void yyerror(char *message)
{
  gperror(103, message);
}

int yylex(void);
extern int _16bit_core;

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

static struct pnode *mk_string(char *value)
{
  struct pnode *new = mk_pnode(string);
  new->value.string = value;
  return new;
}

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

static struct pnode *mk_1op(int op, struct pnode *p0)
{
  struct pnode *new = mk_pnode(unop);  new->value.unop.op = op;
  new->value.unop.p0 = p0;
  return new;
}

/************************************************************************/
/* shared functions */

gpasmVal set_label(char *label, struct pnode *parms)
{
  gpasmVal value = 0;

  if (asm_enabled()) {
    value = do_or_append_insn("set", parms);
    if (!state.mac_prev) {
      set_global(label, value, TEMPORARY, gvt_constant);
    } else {
      state.mac_body->label = label;
    }
  }

  return value;
}

int return_op(int operation) 
{
  /* returns an operator for the replacement of i+=1 with i=i+1*/
  switch(operation) {
  case ASSIGN_PLUS:     return '+';
  case ASSIGN_MINUS:    return '-';
  case ASSIGN_MULTIPLY: return '*';
  case ASSIGN_DIVIDE:   return '/';
  case ASSIGN_MODULUS:  return '%';
  case ASSIGN_LSH:      return LSH;
  case ASSIGN_RSH:      return RSH;
  case ASSIGN_AND:      return '&';
  case ASSIGN_OR:       return '|';
  case ASSIGN_XOR:      return '^'; 
  default:
    assert(0); /* Unhandled operator */ 
  }

  return 0;
}

void next_line(int value)
{
  char l[BUFSIZ];
  char *e = l;

  if (state.pass == 2) {
    fgets(l, BUFSIZ, state.src->f2);
    l[strlen(l) - 1] = '\0';	/* Eat the trailing newline */

    if (state.mac_prev) {
      state.lst.line.linetype = none;
      if (state.mac_body)
	state.mac_body->src_line = strdup(l);
    }

    if (state.lst.enabled) {
      lst_format_line(e, value);
    }
  }

  state.src->line_number++;
}


/************************************************************************/

%}

/* Bison declarations */

%union {
  gpasmVal i;
  char *s;
  struct pnode *p;
}

%token <s> LABEL
%token <s> IDENTIFIER
%token <s> CBLOCK, ENDC, FILL
%token <i> NUMBER
%token <s> STRING
%token <i> HIGH
%token <i> LOW
%token <i> LSH, RSH
%token <i> GREATER_EQUAL, LESS_EQUAL, EQUAL, NOT_EQUAL, '<', '>'
%token <i> '&', '|', '^'
%token <i> LOGICAL_AND, LOGICAL_OR
%token <i> '=', ASSIGN_PLUS, ASSIGN_MINUS, ASSIGN_MULTIPLY, ASSIGN_DIVIDE
%token <i> ASSIGN_MODULUS, ASSIGN_LSH, ASSIGN_RSH
%token <i> ASSIGN_AND, ASSIGN_OR, ASSIGN_XOR, INCREMENT, DECREMENT
%token <i> TBL_NO_CHANGE, TBL_POST_INC, TBL_POST_DEC, TBL_PRE_INC
%token <i> CONCAT, VAR
%token <s> VARLAB_BEGIN, VAR_BEGIN, VAR_END

%type <i> '+', '-', '*', '/', '%', '!', '~'
%type <s> line
%type <s> label_concat
%type <i> statement
%type <p> parameter_list
%type <p> expr, e0, e1, e2, e3, e4, e5, e6, e7, e8, e9
%type <p> cidentifier
%type <i> e1op, e2op, e3op, e4op, e5op, e6op, e7op, e8op, e9op
%type <i> assign_equal_ops

%%
/* Grammar rules */

program:
	/* can be nothing */
	|
	program 
        { 
          state.lst.line.was_org = state.org; 
          state.lst.line.linetype = none; 
        } line
	| program error '\n'
	{ 
	  next_line(0);
	}
	;

line:
	label_concat assign_equal_ops expr '\n'
	{
	  struct pnode *parms;
	  int exp_result;

          exp_result = do_or_append_insn("set", mk_list($3, NULL));          
          parms = mk_list(mk_2op(return_op($2), 
                                 mk_symbol($1), 
                                 mk_constant(exp_result)), NULL);
	  next_line(set_label($1, parms));
	}
	|
	label_concat '=' expr '\n'
	{
	  struct pnode *parms;

	  /* implements i = 6 + 1 */           
	  parms = mk_list($3, NULL);
	  next_line(set_label($1, parms));
	}
	|
	label_concat DECREMENT '\n'
	{
	  struct pnode *parms;

	  /* implements i-- */           
	  parms = mk_list(mk_1op(DECREMENT, mk_symbol($1)), NULL);
	  next_line(set_label($1, parms));
	}
	|
	label_concat INCREMENT '\n'
	{
          struct pnode *parms;

	  /* implements i++ */          
	  parms = mk_list(mk_1op(INCREMENT, mk_symbol($1)), NULL);
	  next_line(set_label($1, parms));
	}
	|
	label_concat statement
	{
	  if (state.lst.line.linetype == none)
	    state.lst.line.linetype = insn;
	  
	  if (asm_enabled()) {
	    if (state.mac_head) {
	      /* This is a macro definition.  Set it up */
	      struct symbol *mac;
	      struct macro_head *h = NULL;

	      mac = get_symbol(state.stMacros, $1);
	      if (mac)
		h = get_symbol_annotation(mac);

	      /* It's not an error if macro was defined on pass 1 and
		 we're in pass 2. */
	      if (h &&
		  !((h->pass == 1) && (state.pass == 2))) {
		gperror(GPE_DUPLICATE_MACRO, NULL);
	      } else {
		if (!mac)
		  mac = add_symbol(state.stMacros, $1);
		annotate_symbol(mac, state.mac_head);
		h = state.mac_head;
		h->line_number = state.src->line_number;
	      }
	      h->pass = state.pass;

	      state.mac_head = NULL;
	    } else if (!state.mac_prev) {
	      /* Outside a macro, just define the label. */
	      switch (state.lst.line.linetype) {
	      case set:
		set_global($1, $2, TEMPORARY, gvt_constant);
		break;
              case org:
	      case equ:
		set_global($1, $2, PERMANENT, gvt_constant);
		break;
	      case insn:
		set_global($1, $2 << _16bit_core, PERMANENT, gvt_address);
		break;
	      case dir:
                gperror(GPE_ILLEGAL_LABEL, NULL);
                break;
              default:
		break;
	      }
	    } else {
	      /* Inside a macro; add the label to the current line */
	      if (state.mac_body)
		state.mac_body->label = $1;
	    }
	  }
	  next_line($2);
	}
	|
	statement
	{
	  if (state.mac_head) {
	    /* This is a macro definition, but the label was missing */
	    state.mac_head = NULL;
	    gperror(GPE_NO_MACRO_NAME, NULL);
	  } else {
	    next_line(0);
	  }
	}
	;

label_concat:
	LABEL
        { 
          $$ = $1;
        }
        |
        VARLAB_BEGIN expr ')'
        {
          if (!state.mac_prev) {
            $$ = evaluate_concatenation(mk_2op(CONCAT,  mk_symbol($1), 
                         mk_1op(VAR, $2)));
          } else {
            gperror(GPE_UNKNOWN, 
                    "This operation is not yet supported in macros");
          }
        }
        |
        VARLAB_BEGIN expr VAR_END
        {
          if (!state.mac_prev) {
            $$ = evaluate_concatenation(mk_2op(CONCAT,  mk_symbol($1), 
                      mk_2op(CONCAT, mk_1op(VAR, $2), mk_symbol($3))));
          } else {
            gperror(GPE_UNKNOWN, 
                    "This operation is not yet supported in macros");
          }
        }        
        ;

statement:
	'\n'
	{
	  if (!state.mac_prev) {
	    $$ = state.org;
	  } else {
	    macro_append();
	  }
	}
	|
	IDENTIFIER '\n'
	{
	  $$ = do_or_append_insn($1, NULL);
	}
	|
	IDENTIFIER parameter_list '\n'
	{
	  $$ = do_or_append_insn($1, $2);
	}
	|
	FILL IDENTIFIER ')' ',' expr '\n'
	{
	  int number = eval_fill_number($5);
	  int i;

         if (!state.mac_prev) {
           for (i = 0; i < number; i++) {
             $$ = do_insn($2, NULL);
           }
         } else {
           gperror(GPE_UNKNOWN, 
                   "This operation is not yet supported in macros");
         }
	}
	|
	FILL IDENTIFIER parameter_list ')' ',' expr '\n'
	{
	  int number = eval_fill_number($6);
	  int i;

         if (!state.mac_prev) {
           for (i = 0; i < number; i++) {
             $$ = do_insn($2, $3);
           }
         } else {
           gperror(GPE_UNKNOWN, 
                   "This operation is not yet supported in macros");
         }
	}
	|
	CBLOCK expr '\n' { begin_cblock($2); next_line(0); }
        const_block
	ENDC '\n'
	{
	  $$ = 0;
	}
        |
        CBLOCK '\n' { next_line(0); }
        const_block
        ENDC '\n'
        {
           $$ = 0;
        }
	|
	CBLOCK error ENDC '\n'
	{
	  $$ = 0;
  	}
	;

const_block:
	|
	const_block const_line
	{
	  next_line(0);
	}
	;

const_line:
	'\n'
	|
	const_def_list '\n'
	|
	LABEL '\n'
	{
	  cblock_expr($1);
	}
	|
	LABEL expr '\n'
	{
	  cblock_expr_incr($1, $2);
	}

	;

const_def_list:
	const_def
	|
	const_def_list ',' const_def
	;

const_def:
	IDENTIFIER
	{
	  cblock_expr($1);
	}
	|
	IDENTIFIER ':' expr
	{
	  cblock_expr_incr($1, $3);
	}
	;

assign_equal_ops: ASSIGN_PLUS | ASSIGN_MINUS | ASSIGN_MULTIPLY | 
                  ASSIGN_DIVIDE | ASSIGN_MODULUS | ASSIGN_LSH | ASSIGN_RSH |
                  ASSIGN_AND | ASSIGN_OR | ASSIGN_XOR; 

parameter_list:
	expr
	{
	  $$ = mk_list($1, NULL);
	}
	|
	expr ','
	{
	  gperror(GPE_BADCHAR, "Illegal Character (,)");
          $$ = mk_list($1, NULL);
	}
	|
	expr ',' parameter_list
	{
	  $$ = mk_list($1, $3);
	}
	;

expr:
	e9
	|
	STRING
        {
	  /* Cook up the string, replacing \X with its code. */
	  char *ps, *pd;

	  ps = pd = $1;
	  while (*ps) {
	    if (*ps != '\\') {
	      *pd++ = *ps++;
	    } else {
	      *pd++ = gpasm_magic(ps);
	      ps += 2;
	    }
	  }
	  *pd = 0;
	  $$ = mk_string($1);
        }
	;

e9:
	e8
	|
	e9 e9op e8
	{
	  $$ = mk_2op($2, $1, $3);
	}
	;

e9op:	'=' ;

e8:
	e7
	|
	e8 e8op e7
	{
	  $$ = mk_2op($2, $1, $3);
	}
	;

e8op:	LOGICAL_OR ;

e7:
	e6
	|
	e7 e7op e6
	{
	  $$ = mk_2op($2, $1, $3);
	}
	;

e7op:	LOGICAL_AND;

e6:
	e5
	|
	e6 e6op e5
	{
	  $$ = mk_2op($2, $1, $3);
	}
	;

e6op:	'&' | '|' | '^' ;

e5:
	e4
	|
	e5 e5op e4
	{
	  $$ = mk_2op($2, $1, $3);
	}
	;

e5op:	'<' | '>' | EQUAL | NOT_EQUAL | GREATER_EQUAL | LESS_EQUAL ;

e4:
	e3
	|
	e4 e4op e3
	{
	  $$ = mk_2op($2, $1, $3);
	}
	;

e4op:	LSH | RSH ;

e3:
	e2
	|
	e3 e3op e2
	{
	  $$ = mk_2op($2, $1, $3);
	}
	;

e3op:   '+' | '-' ;

e2:
	e1
	|
	e2 e2op e1
	{
	  $$ = mk_2op($2, $1, $3);
	}
	;

e2op:   '*' | '/' | '%';

e1:
	e0
	|
	e1op e0
	{
	  $$ = mk_1op($1, $2);
	}
	;

e1op:	HIGH | LOW | '-' | '!' | '~' | '+';

e0:
	cidentifier
        {
	  $$ = $1;
        }
	|
	NUMBER
	{
	  $$ = mk_constant($1);
	}
	|
        '$'
        {
	  $$ = mk_symbol("$");
	}
	|
	'(' expr ')'
	{
	  $$ = $2;
	}
	|
	'*' 
	{
	  $$ = mk_constant(TBL_NO_CHANGE);
	}
	| 
	TBL_POST_INC
	{
	  $$ = mk_constant($1);
	}
	| 
	TBL_POST_DEC
	{
	  $$ = mk_constant($1);
	}
	| 
	TBL_PRE_INC
	{
	  $$ = mk_constant($1);
	}
	;

cidentifier:
        IDENTIFIER
        { 
          $$ = mk_symbol($1);
        }
        |
        VAR_BEGIN expr ')'
        {
          $$ = mk_2op(CONCAT, mk_symbol($1), mk_1op(VAR, $2));
        }
        |
        VAR_BEGIN expr VAR_END
        {
          $$ = mk_2op(CONCAT, mk_symbol($1), 
                        mk_2op(CONCAT, mk_1op(VAR, $2), mk_symbol($3)));
        }
        ;

%%

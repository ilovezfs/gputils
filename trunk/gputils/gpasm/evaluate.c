/* evaluates variables
   Copyright (C) 2002 Craig Franklin

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

#include "evaluate.h"
#include "directive.h"
#include "gperror.h"
#include "gpsymbol.h"
#include "parse.h"

extern int _16bit_core;

int enforce_arity(int arity, int must_be)
{
  if (arity == must_be)
    return 1;
  else {
    if (arity < must_be) {
      gperror(GPE_MISSING_ARGU, NULL);
    } else {
      gperror(GPE_TOO_MANY_ARGU, NULL);
    }
    return 0;
  }
}

int enforce_simple(struct pnode *p)
{
  if (p->tag == symbol) {
    return 1;
  } else {
    gperror(GPE_ILLEGAL_ARGU, NULL);
    return 0;
  }
}

int list_length(struct pnode *L)
{
  if (L == NULL) {
    return 0;
  } else {
    return 1 + list_length(TAIL(L));
  }
}

int can_evaluate_concatenation(struct pnode *p)
{
  char buf[BUFSIZ];

  switch (p->tag) {
  case constant:
    return 1;
  case symbol:
    return 1;
  case unop:
    return can_evaluate_concatenation(p->value.unop.p0);
  case binop:
    return can_evaluate_concatenation(p->value.binop.p0) 
           && can_evaluate_concatenation(p->value.binop.p1);
  case string:
    sprintf(buf, "Illegal argument (%s).", p->value.string);
    gperror(GPE_ILLEGAL_ARGU, buf);
    return 0;
  default:
    assert(0);
  }

  return 0;
}

int can_evaluate(struct pnode *p)
{
  char buf[BUFSIZ];

  if ((p->tag == binop) && (p->value.binop.op == CONCAT)) {
    return can_evaluate_concatenation(p);
  }
  switch (p->tag) {
  case constant:
    return 1;
  case symbol:
    {
      struct symbol *s;

      /* '$' means current org, which we can always evaluate */
      if (strcmp(p->value.symbol, "$") == 0) {
	return 1;
      } else {
        struct variable *var = NULL;

	/* Otherwise look it up */
	s = get_symbol(state.stTop, p->value.symbol);

        if (s == NULL) {
          sprintf(buf, "Symbol not previously defined (%s).", p->value.symbol);
          gperror(GPE_NOSYM, buf);    
	} else {
          var = get_symbol_annotation(s);

          if (var == NULL) {
            sprintf(buf, "Symbol not assigned a value (%s).", p->value.symbol);
            gpwarning(GPW_UNKNOWN, buf);    
          }
        }  
 
	return ((s != NULL) && (var != NULL));
      }
    }
  case unop:
    return can_evaluate(p->value.unop.p0);
  case binop:
    return can_evaluate(p->value.binop.p0) && can_evaluate(p->value.binop.p1);
  case string:
    sprintf(buf, "Illegal argument (%s).", p->value.string);
    gperror(GPE_ILLEGAL_ARGU, buf);
    return 0;
  default:
    assert(0);
  }

  return 0;
}

char *evaluate_concatenation(struct pnode *p)
{
  switch (p->tag) {
  case symbol:
    return p->value.symbol;
  case binop:
    assert(p->value.binop.op == CONCAT);
    {
      char *s[2], *new;

      s[0] = evaluate_concatenation(p->value.binop.p0);
      s[1] = evaluate_concatenation(p->value.binop.p1);
      new = malloc(strlen(s[0]) + 1 + strlen(s[1]) + 1);
      strcpy(new, s[0]);
      strcat(new, s[1]);
      return new;
    }
  case unop:
    assert(p->value.unop.op == VAR);
    {
      char buf[80];
      sprintf(buf, "%d", maybe_evaluate(p->value.unop.p0));
      return (strdup(buf));
    }
  default:
    assert(0);
  }

  return NULL;
}

gpasmVal evaluate(struct pnode *p)
{
  struct variable *var;
  gpasmVal p0, p1;

  if (((p->tag == binop) && (p->value.binop.op == CONCAT)) ||
      ((p->tag == unop) && (p->value.unop.op == VAR))) {
    char *string = evaluate_concatenation(p);
    struct symbol *s;

    s = get_symbol(state.stTop, string);
    if (s == NULL) {
      char buf[BUFSIZ];
      sprintf(buf, "Symbol not previously defined (%s).", string);
      gperror(GPE_NOSYM, buf); 
      return 0;
    } else {
      var = get_symbol_annotation(s);
      assert(var != NULL);
      return var->value;
    }
  }
  
  switch (p->tag) {
  case constant:
    return p->value.constant;
  case symbol:
    {
      struct symbol *s;

      if (strcmp(p->value.symbol, "$") == 0) {
	return state.org << _16bit_core;
      } else {
	s = get_symbol(state.stTop, p->value.symbol);
	var = get_symbol_annotation(s);
	assert(var != NULL);
	return var->value;
      }
    }
  case unop:
    switch (p->value.unop.op) {
    case '!':
      return !evaluate(p->value.unop.p0);
    case '+':
      return  evaluate(p->value.unop.p0);
    case '-':
      return -evaluate(p->value.unop.p0);
    case '~':
      return ~evaluate(p->value.unop.p0);
    case HIGH:
      return (evaluate(p->value.unop.p0) >> 8) & 0xff;
    case LOW:
      return evaluate(p->value.unop.p0) & 0xff;
    case INCREMENT:  
      return evaluate(p->value.unop.p0) + 1;
    case DECREMENT:          
      return evaluate(p->value.unop.p0) - 1;
    default:
      assert(0);
    }
  case binop:
    p0 = evaluate(p->value.binop.p0);
    p1 = evaluate(p->value.binop.p1);
    switch (p->value.binop.op) {
    case '+':      return p0 + p1;
    case '-':      return p0 - p1;
    case '*':      return p0 * p1;
    case '/':
      if (p1 == 0){
        gperror(GPE_DIVBY0, NULL);
        return 0;
      } else {
        return p0 / p1;
      }
    case '%':      return p0 % p1;
    case '&':      return p0 & p1;
    case '|':      return p0 | p1;
    case '^':      return p0 ^ p1;
    case LSH:      return p0 << p1;
    case RSH:      return p0 >> p1;
    case EQUAL:    return p0 == p1;
    case '<':      return p0 < p1;
    case '>':      return p0 > p1;
    case NOT_EQUAL:          return p0 != p1;
    case GREATER_EQUAL:      return p0 >= p1;
    case LESS_EQUAL:         return p0 <= p1;
    case LOGICAL_AND:        return p0 && p1;
    case LOGICAL_OR:         return p0 || p1;
    case '=': 
      gperror(GPE_BADCHAR, "Illegal character (=)");     
      return 0;
    default:
      assert(0); /* Unhandled binary operator */
    }
  default:
    assert(0); /* Unhandled parse node tag */
  }
  return (0); /* Should never reach here */
}

/* Attempt to evaluate expression 'p'.  Return its value if
 * successful, otherwise generate an error message and return 0.  */

gpasmVal maybe_evaluate(struct pnode *p)
{
  gpasmVal r;

  if (p && can_evaluate(p)) {
    r = evaluate(p);
  } else {
    r = 0;
  }

  return r;
}

/* evaluate the number of passes for the "fill" directive*/
int eval_fill_number(struct pnode *p)
{
  int number;

  number = maybe_evaluate(p);
  if(_16bit_core) {
    /* For 16 bit core devices number is bytes not words */ 
    if ((number & 0x1) == 1){
      /* The number is divided by two, so it can't be odd */
      gperror(GPE_FILL_ODD, NULL);
    } else {
      number = number / 2;
    }	
  }

  return number;
}

/* Implements macros
   Copyright (C) 2002, 2003, 2004, 2005
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
#include "gpasm.h"
#include "directive.h"
#include "evaluate.h"
#include "gperror.h"
#include "macro.h"
#include "parse.h"

#define BUFFER_SIZE 520
static char arg_buffer[BUFFER_SIZE];
static int arg_index;

static void
cat_string(char *string)
{
  char *ptr;
  int length;
  
  ptr = &arg_buffer[arg_index];
  length = strlen(string);
  if (arg_index + length + 1 < BUFFER_SIZE) {
    strcpy(ptr, string);
    arg_index += length;
  } else {
    gperror(GPE_UNKNOWN, "macro argument exceeds buffer size");  
  }

}

static void
cat_symbol(int op)
{
  switch (op) {
  case '+':
    cat_string("+");
    break;  
  case '-':
    cat_string("-");
    break;  
  case '*':
    cat_string("*");
    break;  
  case '/':
    cat_string("/");
    break;  
  case '%':
    cat_string("%");
    break;  
  case '&':
    cat_string("&");
    break;  
  case '|':
    cat_string("|");
    break;  
  case '^':
    cat_string("^");
    break;  
  case LSH:
    cat_string("<<");
    break;  
  case RSH:
    cat_string(">>");
    break;  
  case '<':
    cat_string("<");
    break;  
  case '>':
    cat_string(">");
    break;
  case '!':
    cat_string("!");
    break;
  case '~':
    cat_string("~");
    break;
  case EQUAL:
    cat_string("==");
    break;
  case NOT_EQUAL:
    cat_string("!=");
    break;
  case GREATER_EQUAL:
    cat_string(">=");
    break;
  case LESS_EQUAL:
    cat_string("<=");
    break;
  case LOGICAL_AND:
    cat_string("&&");
    break;
  case LOGICAL_OR:
    cat_string("||");
    break;
  case '=': 
    cat_string("=");
    break;
  case UPPER:
    cat_string("UPPER");
    break;
  case HIGH:
    cat_string("HIGH");
    break;
  case LOW:
    cat_string("LOW");
    break;
  case INCREMENT:  
    cat_string("++");
    break;
  case DECREMENT:          
    cat_string("--");
    break;
  default:
    assert(0);
  }

} 

/* Must convert the parm to a plain string, this will allow substitutions
   of labels and strings.  This is a kludge.  It would be better to store
   a copy of the raw string in the parse node. */

static void
node_to_string(struct pnode *p)
{
  char constant_buffer[64];

  switch(p->tag) {
  case constant:
    if (p->value.constant < 0) {
      snprintf(constant_buffer,
               sizeof(constant_buffer),
               "-%#x", -p->value.constant);
    } else {
      snprintf(constant_buffer,
               sizeof(constant_buffer),
               "%#x", p->value.constant);
    }
    cat_string(constant_buffer);
    break;
  case symbol:
    cat_string(p->value.symbol);
    break;
  case unop:
    cat_string("(");
    cat_symbol(p->value.unop.op);
    cat_string(" ");
    node_to_string(p->value.unop.p0);
    cat_string(")");
    break;
  case binop:
    if (p->value.binop.op == CONCAT) {
      cat_string(evaluate_concatenation(p));
    } else {
      cat_string("(");
      node_to_string(p->value.binop.p0);
      cat_symbol(p->value.binop.op);
      node_to_string(p->value.binop.p1);
      cat_string(")");
    }
    break;
  case string:
    cat_string("\"");
    cat_string(p->value.string);
    cat_string("\"");
    break;
  case list:
  default:
    assert(0);
  }

  return; 
}

/* Create a new defines table and place the macro parms in it. */

void setup_macro(struct macro_head *h, int arity, struct pnode *parms)
{
  if (enforce_arity(arity, list_length(h->parms))) {
    /* push table for the marco parms */
    state.stTopDefines = push_symbol_table(state.stTopDefines, 
                                        state.case_insensitive);

    /* Now add the macro's declared parameter list to the new 
       defines table. */
    if (arity > 0) {
      struct pnode *pFrom, *pFromH;
      struct pnode *pTo, *pToH;
      struct symbol *sym;

      pTo = parms;

      for (pFrom = h->parms; pFrom; pFrom = TAIL(pFrom)) {
	pToH = HEAD(pTo);
	pFromH = HEAD(pFrom);
	assert(pFromH->tag == symbol);
 
        arg_index = 0;
        arg_buffer[0] = '\0';
        node_to_string(pToH);
        sym = add_symbol(state.stTopDefines, pFromH->value.symbol);	
        annotate_symbol(sym, strdup(arg_buffer));
        pTo = TAIL(pTo);
      }
    }

    state.next_state = state_macro;  
    state.next_buffer.macro = h;
    state.lst.line.linetype = none;
  }
}

/* Copy the macro body to a buffer. */

void copy_macro_body(struct macro_body *b, char *buffer, size_t sizeof_buffer)
{
  int len = 0;
  while (b) {
    if (b->src_line != NULL) {
      /* sizeof_buffer is always larger than len because it was
	 measured by caller */
      len += snprintf(buffer + len, sizeof_buffer - len, "%s\n", b->src_line);
    }
    b = b->next;
  }
}


/* Create a buffer for parser from the macro definition. */

char *
make_macro_buffer(struct macro_head *h)
{
  struct macro_body *b;
  unsigned int macro_src_size = 0;
  char *macro_src;

  /* determine the length of the macro body */
  b = h->body;
  while (b) {
    if (b->src_line != NULL)
      macro_src_size += strlen(b->src_line) + 1; /* add one for \n */
    b = b->next;
  }

  macro_src_size += 2; /* flex requires two extra chars at the end */

  /* Allocate memory for the new buffer. yy_delete_buffer frees it */
  macro_src = (char *)malloc(macro_src_size);
  if (macro_src) {
    /* build the string to be scanned */  
    copy_macro_body(h->body, macro_src, macro_src_size);
    /* Flex requires the two extra chars at end to be nuls */
    macro_src[macro_src_size-2] = 0;
    macro_src[macro_src_size-1] = 0;
  }

  return macro_src;
}

/* The symbol table is pushed at each macro call.  This makes local symbols
   possible.  Each symbol table is created once on pass 1.  On pass two the
   old symbol table is reloaded so forward references to the local symbols
   are possible */

struct macro_table {
  struct symbol_table *table;
  int line_number;            /* sanity check, better not change */
  struct macro_table *next;
};

static struct macro_table * macro_table_list = NULL;

static void 
add_macro_table(struct symbol_table *table)
{
  struct macro_table *new;
    
  new = (struct macro_table *)malloc(sizeof(*new));
  new->table = table;
  new->line_number = state.src->line_number;
  new->next = NULL;

  if (macro_table_list == NULL) {
    macro_table_list = new;  
  } else {
    struct macro_table *list = macro_table_list;

    /* find the end of the list */  
    while(list->next != NULL) {
      list = list->next;
    }
  
    list->next = new;
  }

  return;
}

struct symbol_table *
push_macro_symbol_table(struct symbol_table *table)
{
  struct symbol_table *new = NULL;

  if (state.pass == 1) {
    new = push_symbol_table(table, state.case_insensitive);
    add_macro_table(new);
  } else if (macro_table_list->line_number != state.src->line_number) {
    /* The user must have conditionally assembled a macro using a forward
       reference to a label.  This is a very bad practice. It means that
       a macro wasn't executed on the first pass, but it was on the second.
       Probably errors will be generated.  Forward references to local 
       symbols probably won't be correct.  */
    new = push_symbol_table(table, state.case_insensitive);
    gpwarning(GPW_UNKNOWN, "macro not executed on pass 1");
  } else {
    assert(macro_table_list != NULL);
    new = macro_table_list->table;
    new->prev = table;
    macro_table_list = macro_table_list->next; /* setup for next macro */
  }

  return new;
}


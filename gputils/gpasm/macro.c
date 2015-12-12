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
#include "preprocess.h"
#include "lst.h"

/* Create a new defines table and place the macro parms in it. */

void setup_macro(struct macro_head *h, int arity, struct pnode *parms)
{
  if (enforce_arity(arity, list_length(h->parms))) {
    /* push table for the marco parms */
    state.stMacroParams = push_symbol_table(state.stMacroParams, state.case_insensitive);

    /* Now add the macro's declared parameter list to the new
       defines table. */
    if (arity > 0) {
      struct pnode *pFrom, *pFromH;
      struct pnode *pTo, *pToH;
      struct symbol *sym;

      pTo = parms;

      for (pFrom = h->parms; pFrom != NULL; pFrom = TAIL(pFrom)) {
        pToH = HEAD(pTo);
        pFromH = HEAD(pFrom);
        assert(pFromH->tag == PTAG_SYMBOL);
        assert(pToH->tag == PTAG_SYMBOL);

        sym = add_symbol(state.stMacroParams, pFromH->value.symbol);
        annotate_symbol(sym, mk_list(mk_string(GP_Strdup(pToH->value.symbol)), NULL));
        pTo = TAIL(pTo);
      }
    }

    state.next_state = STATE_MACRO;
    state.next_buffer.macro = h;
    state.lst.line.linetype = LTY_NONE;
  }
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

static struct macro_table *macro_table_list = NULL;

static void
add_macro_table(struct symbol_table *table)
{
  struct macro_table *new;

  new = (struct macro_table *)GP_Malloc(sizeof(*new));
  new->table = table;
  new->line_number = state.src->line_number;
  new->next = NULL;

  if (macro_table_list == NULL) {
    macro_table_list = new;
  } else {
    struct macro_table *list = macro_table_list;

    /* find the end of the list */
    while (list->next != NULL) {
      list = list->next;
    }

    list->next = new;
  }
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

void
list_macro(struct macro_body *p)
{
  unsigned int old_line_number = state.src->line_number;

  /* Never executed: list the macro body */
  state.lst.line.linetype = LTY_DIR;
  state.src->line_number = state.while_head->line_number;
  while (p != NULL) {
    ++state.src->line_number;
    lst_format_line(p->src_line, 0);
    p = p->next;
  }
  state.src->line_number = old_line_number;
}

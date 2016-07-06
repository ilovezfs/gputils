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
#include "gpmsg.h"
#include "macro.h"
#include "parse.h"
#include "preprocess.h"
#include "lst.h"

/* The symbol table is pushed at each macro call. This makes local symbols
   possible. Each symbol table is created once on pass 1. On pass two the
   old symbol table is reloaded so forward references to the local symbols
   are possible. */

typedef struct macro_table {
  symbol_table_t     *table;
  int                 line_number;      /* sanity check, better not change */
  struct macro_table *next;
} macro_table_t;

static macro_table_t *macro_table_list      = NULL;
static macro_table_t *macro_table_list_last = NULL;
static macro_table_t *macro_table_ptr       = NULL;

/*------------------------------------------------------------------------------------------------*/

static void
_add_macro_table(symbol_table_t *Table)
{
  macro_table_t *new;

  new = (macro_table_t *)GP_Calloc(1, sizeof(*new));
  new->table       = Table;
  new->line_number = state.src->line_number;

  if (macro_table_list == NULL) {
    macro_table_list = new;
    macro_table_ptr  = new;
  }
  else {
    macro_table_list_last->next = new;
  }

  macro_table_list_last = new;
}

/*------------------------------------------------------------------------------------------------*/

/* Create a new defines table and place the macro parms in it. */

void macro_setup(macro_head_t *Head, int Arity, const pnode_t *Parms)
{
  const pnode_t *pFrom;
  const pnode_t *pFromH;
  const pnode_t *pTo;
  const pnode_t *pToH;
  symbol_t      *sym;

  if (eval_enforce_arity(Arity, eval_list_length(Head->parms))) {
    /* push table for the marco parms */
    state.stMacroParams = sym_push_table(state.stMacroParams, state.case_insensitive);

    /* Now add the macro's declared parameter list to the new defines table. */
    if (Arity > 0) {
      pTo = Parms;

      for (pFrom = Head->parms; pFrom != NULL; pFrom = PnListTail(pFrom)) {
        pToH   = PnListHead(pTo);
        pFromH = PnListHead(pFrom);
        assert(PnIsSymbol(pFromH));
        assert(PnIsSymbol(pToH));

        sym = sym_add_symbol(state.stMacroParams, PnSymbol(pFromH));
        sym_annotate_symbol(sym, mk_list(mk_string(GP_Strdup(PnSymbol(pToH))), NULL));
        pTo = PnListTail(pTo);
      }
    }

    state.next_state        = STATE_MACRO;
    state.next_buffer.macro = Head;
    state.lst.line.linetype = LTY_NONE;
  }
}

/*------------------------------------------------------------------------------------------------*/

symbol_table_t *
macro_push_symbol_table(symbol_table_t *Table)
{
  symbol_table_t *new = NULL;

  if (state.pass == 1) {
    new = sym_push_table(Table, state.case_insensitive);
    _add_macro_table(new);
  }
  else {
    if (macro_table_ptr == NULL) {
      gpmsg_verror(GPE_UNKNOWN, "An error occurred during a macro execution on pass %i.", state.pass);
      exit(1);
    }

    if (macro_table_ptr->line_number != state.src->line_number) {
    /* The user must have conditionally assembled a macro using a forward
       reference to a label. This is a very bad practice. It means that
       a macro wasn't executed on the first pass, but it was on the second.
       Probably errors will be generated. Forward references to local
       symbols probably won't be correct. */
      new = sym_push_table(Table, state.case_insensitive);
      gpmsg_warning(GPW_UNKNOWN, "Macro not executed on pass 1.");
    }
    else {
      assert(macro_table_ptr != NULL);
      new = macro_table_ptr->table;
      sym_set_guest_table(new, Table);
      macro_table_ptr = macro_table_ptr->next; /* setup for next macro */
    }
  }

  return new;
}

/*------------------------------------------------------------------------------------------------*/

void
macro_list(macro_body_t *Body)
{
  unsigned int old_line_number = state.src->line_number;

  /* Never executed: list the macro body */
  state.lst.line.linetype = LTY_DIR;
  state.src->line_number  = state.while_head->line_number;
  while (Body != NULL) {
    ++state.src->line_number;
    lst_format_line(Body->src_line, 0);
    Body = Body->next;
  }
  state.src->line_number = old_line_number;
}

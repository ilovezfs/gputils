/* Symbol list support.

   Copyright (C) 2016 Molnar Karoly

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
#include "coff.h"
#include "symbol_list.h"

/*------------------------------------------------------------------------------------------------*/

/* Auxiliary function for this module. */

static void
_node_free(void *Node)
{
  symbol_node_t *n;

  n = (symbol_node_t *)Node;

  if (n->name != NULL) {
    free(n->name);
  }

  free(n);
}

/*------------------------------------------------------------------------------------------------*/

/* Initialize the symbol list. */

void
symbol_list_init(void)
{
  gp_list_set_delete_node_func(&state.obj.symbol_fifo, _node_free);
}

/*------------------------------------------------------------------------------------------------*/

/* Add a new symbol to the list (fifo). */

void
symbol_list_add_symbol(const symbol_t *Symbol, const char *Name, unsigned int Symbol_number,
                       int Section_number, unsigned int Base_class, unsigned int Byte_address)
{
  symbol_node_t *node;


  if (state.pass != 1) {
    return;
  }

  node = (symbol_node_t *)gp_list_node_append(&state.obj.symbol_fifo, gp_list_node_new(sizeof(symbol_node_t)));
  node->symbol         = Symbol;
  node->name           = GP_Strdup(Name);
  node->symbol_number  = Symbol_number;
  node->section_number = Section_number;
  node->base_class     = Base_class;
  node->byte_address   = Byte_address;
}

/*------------------------------------------------------------------------------------------------*/

/* Empties the list up to this symbol. */

void
symbol_list_flush_symbols(const char *End_symbol_name)
{
  symbol_node_t    *node;
  const symbol_t   *symbol;
  const char       *name;
  const variable_t *var;
  char             *coff_name;

  if (state.obj.symbol_fifo.first == NULL) {
    return;
  }

  node = state.obj.symbol_fifo.curr;
  while (node != NULL) {
    symbol = node->symbol;

    if (symbol != NULL) {
      name = gp_sym_get_symbol_name(symbol);
      var  = (const variable_t *)gp_sym_get_symbol_annotation(symbol);
      assert(var != NULL);
      coff_name = coff_local_name(name);
      coff_add_sym(coff_name, var->value, var->type, node->section_number);

      if (coff_name != NULL) {
        free(coff_name);
      }
    }

    if ((End_symbol_name != NULL) && (strcmp(node->name, End_symbol_name) == 0)) {
      node = node->next;
      break;
    }

    node = node->next;
  }

  state.obj.symbol_fifo.curr = node;
}

/*------------------------------------------------------------------------------------------------*/

/* Restores the current pointer of list to the beginning. */

void
symbol_list_reset(void)
{
  gp_list_reset(&state.obj.symbol_fifo);
}

/*------------------------------------------------------------------------------------------------*/

/* Freeing the entire list. */

void
symbol_list_free(void)
{
  gp_list_delete(&state.obj.symbol_fifo);
}

/* Source file functions for gpasm

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

/*------------------------------------------------------------------------------------------------*/

void
file_delete_node(void *Node)
{
  file_context_t *n;

  if (Node == NULL) {
    return;
  }

  n = (file_context_t *)Node;
  free(n->name);
  free(n);
}

/*------------------------------------------------------------------------------------------------*/

/* file_add: Add a file of type 'type' to the file_context stack. */

file_context_t *
file_add(unsigned int Type, const char *Name)
{
  static unsigned int  file_id = 0;

  file_context_t      *new;

  /* First check to make sure this file is not already in the list. */
  new = state.file_list.last;
  while (new != NULL) {
    if (strcmp(new->name, Name) == 0) {
      return new;
    }
    new = new->prev;
  }

  new = gp_list_node_new(sizeof(*new));

  new->name = GP_Strdup(Name);
  new->ft   = Type;
  new->id   = file_id++;

  if (state.file_list.first == NULL) {
    gp_list_set_delete_node_func(&state.file_list, file_delete_node);
  }

  gp_list_node_append(&state.file_list, new);
  return new;
}

/*------------------------------------------------------------------------------------------------*/

void
file_search_paths(source_context_t *Context, const char *Name)
{
  char tryname[PATH_MAX + 1];
  int  i;

  for (i = 0; i < state.num_paths; i++) {
    snprintf(tryname, sizeof(tryname), "%s" PATH_SEPARATOR_STR "%s", state.paths[i], Name);
    Context->f = fopen(tryname, "rt");

    if (Context->f != NULL) {
      Context->name = GP_Strdup(tryname);
      break;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* file_free: free memory allocated to the file_context stack */

void
file_free(void)
{
  gp_list_delete(&state.file_list);
}

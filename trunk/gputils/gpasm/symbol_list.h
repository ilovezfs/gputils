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

#ifndef __SYMBOL_LIST_H__
#define __SYMBOL_LIST_H__

#include "gpasm.h"

typedef struct symbol_node {
  /* This always should be the first item! (gplist.c) */
  GPNodeHeader(struct symbol_node);

  const symbol_t *symbol;
  char           *name;
  unsigned int    symbol_number;
  int             section_number;
  unsigned int    base_class;
  unsigned int    byte_address;
} symbol_node_t;

typedef struct symbol_list {
  /* head of symbol nodes
   * tail of symbol nodes
   * number of symbol nodes */
  GPListHeader(symbol_node_t);
} symbol_list_t;

extern void symbol_list_init(void);

extern void symbol_list_add_symbol(const symbol_t *Symbol, const char *Name, unsigned int Symbol_number,
                                   int Section_number, unsigned int Base_class, unsigned int Byte_address);

extern void symbol_list_flush_symbols(const char *End_symbol_name);
extern void symbol_list_reset(void);
extern void symbol_list_free(void);

#endif /* __SYMBOL_LIST_H__ */

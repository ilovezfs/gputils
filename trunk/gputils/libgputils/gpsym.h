/* Symbol table support
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman

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

#ifndef __GPSYM_H__
#define __GPSYM_H__

struct symbol;
typedef struct symbol symbol_t;

struct symbol_table;
typedef struct symbol_table symbol_table_t;

typedef int (*symbol_compare_t)(const void *, const void *);

extern size_t sym_get_symbol_count(const symbol_table_t *Table);

extern symbol_table_t *sym_push_table(symbol_table_t *Table, gp_boolean Case_insensitive);
extern symbol_table_t *sym_pop_table(symbol_table_t *Table);
extern void sym_set_guest_table(symbol_table_t *Table_host, symbol_table_t *Table_guest);
extern symbol_table_t *sym_get_guest_table(symbol_table_t *Table);

extern symbol_t *sym_add_symbol(symbol_table_t *Table, const char *Name);
extern gp_boolean sym_remove_symbol_with_index(symbol_table_t *Table, size_t Index);
extern gp_boolean sym_remove_symbol(symbol_table_t *Table, const char *Name);
extern symbol_t *sym_get_symbol(const symbol_table_t *Table, const char *Name);
extern symbol_t *sym_get_symbol_len(const symbol_table_t *Table, const char *Name, size_t Len);
extern symbol_t *sym_get_symbol_with_index(const symbol_table_t *Table, size_t Index);

extern const symbol_t **sym_clone_symbol_array(const symbol_table_t *Table, symbol_compare_t Cmp);

extern void sym_annotate_symbol(symbol_t *Sym, void *Value);

extern const char *sym_get_symbol_name(const symbol_t *Sym);
extern void *sym_get_symbol_annotation(const symbol_t *Sym);

extern int sym_compare_fn(const void *P0, const void *P1);
extern int sym_version_compare_fn(const void *P0, const void *P1);

#endif

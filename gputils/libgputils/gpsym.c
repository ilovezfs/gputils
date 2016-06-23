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

#include "stdhdr.h"
#include "libgputils.h"

#define HASH_TABLE_SIZE_MIN             5

struct symbol {
  const char *name;
  void       *annotation;
  hash128_t   hash;
};

struct symbol_table {
  symbol_table_t  *prev;
  symbol_t       **symbol_array;
  size_t           symbol_array_size;
  size_t           count;
  gp_boolean       case_insensitive;
};

/*------------------------------------------------------------------------------------------------*/

static symbol_t *
_make_symbol(const char *String, hash128_t *Hash)
{
  symbol_t *sym;

  if (String == NULL) {
    return NULL;
  }

  sym                = GP_Malloc(sizeof(symbol_t));
  sym->name          = GP_Strdup(String);
  sym->hash.low.u64  = Hash->low.u64;
  sym->hash.high.u64 = Hash->high.u64;
  sym->annotation    = NULL;
  return sym;
}

/*------------------------------------------------------------------------------------------------*/

static symbol_t *
_get_symbol_from_table(symbol_table_t *Table, hash128_t *Hash)
{
  symbol_t **base;
  symbol_t **current;
  size_t     mid;
  size_t     len;

  assert(Table != NULL);
  assert(Hash != NULL);

  if ((Table->symbol_array == NULL) || (Table->count == 0)) {
    return NULL;
  }

  base = Table->symbol_array;
  len  = Table->count;
  do {
    mid     = len >> 1;
    current = &base[mid];

    if ((Hash->high.u64 == (*current)->hash.high.u64) && (Hash->low.u64 == (*current)->hash.low.u64)) {
      /* Found the symbol. */
      return *current;
    }

    if (len == 1) {
      /* This is different int the least from the sought element. */
      break;
    }
    else if ((Hash->high.u64 < (*current)->hash.high.u64) ||
	     ((Hash->high.u64 == (*current)->hash.high.u64) && (Hash->low.u64 < (*current)->hash.low.u64))) {
      len = mid;
    }
    else {
      len  -= mid;
      base  = current;
    }
  }
  while (len > 0);

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

symbol_table_t *
sym_push_table(symbol_table_t *Table, gp_boolean Case_insensitive)
{
  symbol_table_t *new_table;

  new_table = GP_Calloc(1, sizeof(symbol_table_t));
  new_table->prev             = Table;
  new_table->case_insensitive = Case_insensitive;
  return new_table;
}

/*------------------------------------------------------------------------------------------------*/

symbol_table_t *
sym_pop_table(symbol_table_t *Table)
{
  assert(Table != NULL);

  return Table->prev;
}

/*------------------------------------------------------------------------------------------------*/

void
sym_set_guest_table(symbol_table_t *Table_host, symbol_table_t *Table_guest)
{
  assert(Table_host != NULL);

  Table_host->prev = Table_guest;
}

/*------------------------------------------------------------------------------------------------*/

symbol_table_t *
sym_get_guest_table(symbol_table_t *Table)
{
  assert(Table != NULL);

  return Table->prev;
}

/*------------------------------------------------------------------------------------------------*/

symbol_t *
sym_add_symbol(symbol_table_t *Table, const char *Name)
{
  symbol_t  **base;
  symbol_t  **current;
  symbol_t   *sym;
  size_t      mid;
  size_t      idx;
  size_t      len;
  hash128_t   hash;

  assert(Table != NULL);
  assert(Name != NULL);
  assert(Table->count <= UINT32_MAX);

  if (Table->symbol_array == NULL) {
    Table->symbol_array      = (symbol_t **)GP_Malloc(HASH_TABLE_SIZE_MIN * sizeof(symbol_t *));
    Table->symbol_array_size = HASH_TABLE_SIZE_MIN;
    Table->count             = 0;
  }
  else if (Table->count >= Table->symbol_array_size) {
    /* Doubles the size of the table. */
    len = Table->symbol_array_size * 2;
    Table->symbol_array      = (symbol_t **)GP_Realloc(Table->symbol_array, len * sizeof(symbol_t *));
    Table->symbol_array_size = len;
  }

  gp_hash_init(&hash);
  gp_hash_str(&hash, Name, Table->case_insensitive);

  if (Table->count == 0) {
    /* Empty the table. */
    sym = _make_symbol(Name, &hash);
    Table->symbol_array[0] = sym;
    Table->count           = 1;
    return sym;
  }

  base = Table->symbol_array;
  len  = Table->count;
  do {
    mid     = len >> 1;
    current = &base[mid];

    if ((hash.high.u64 == (*current)->hash.high.u64) && (hash.low.u64 == (*current)->hash.low.u64)) {
      /* Found the symbol. */
      return *current;
    }

    if (len == 1) {
      /* This is different in the least from the sought element. */
      base = Table->symbol_array;
      idx  = current - base;

      if ((hash.high.u64 > (*current)->hash.high.u64) ||
          ((hash.high.u64 == (*current)->hash.high.u64) && (hash.low.u64 > (*current)->hash.low.u64))) {
	/* The new element is greather than this. */
        ++idx;
      }

      len = (Table->count - idx) * sizeof(symbol_t *);
      if (len > 0) {
	/* The new element will not be the end of the table. */
        memmove(&base[idx + 1], &base[idx], len);
      }

      sym = _make_symbol(Name, &hash);
      base[idx] = sym;
      ++Table->count;
      return sym;
    }
    else if ((hash.high.u64 < (*current)->hash.high.u64) ||
	     ((hash.high.u64 == (*current)->hash.high.u64) && (hash.low.u64 < (*current)->hash.low.u64))) {
      len = mid;
    }
    else {
      len  -= mid;
      base  = current;
    }
  }
  while (len > 0);

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
sym_remove_symbol_with_index(symbol_table_t *Table, size_t Index)
{
  symbol_t **base;
  symbol_t  *sym;
  size_t     len;

  assert(Table != NULL);

  if ((Table->symbol_array == NULL) || (Table->count == 0)) {
    return false;
  }

  if (Index >= Table->count) {
    return false;
  }

  len  = (Table->count - Index - 1) * sizeof(symbol_t *);
  base = Table->symbol_array;
  sym  = base[Index];

  if (len > 0) {
    memmove(&base[Index], &base[Index + 1], len);
  }

  --Table->count;

  if (sym->name != NULL) {
    free((void *)sym->name);
  }

  free(sym);
  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* FIXME: sym_remove_symbol does not search all of the symbol tables in the stack.
          Maybe this is ok, but it seems wrong. */

gp_boolean
sym_remove_symbol(symbol_table_t *Table, const char *Name)
{
  symbol_t **base;
  symbol_t **current;
  size_t     mid;
  size_t     len;
  hash128_t  hash;

  assert(Table != NULL);
  assert(Name != NULL);

  if ((Table->symbol_array == NULL) || (Table->count == 0)) {
    return false;
  }

  gp_hash_init(&hash);
  gp_hash_str(&hash, Name, Table->case_insensitive);
  base = Table->symbol_array;
  len  = Table->count;
  do {
    mid     = len >> 1;
    current = &base[mid];

    if ((hash.high.u64 == (*current)->hash.high.u64) && (hash.low.u64 == (*current)->hash.low.u64)) {
      /* Found the symbol. */
      return sym_remove_symbol_with_index(Table, current - Table->symbol_array);
    }

    if (len == 1) {
      /* This is different int the least from the sought element. */
      return false;
    }
    else if ((hash.high.u64 < (*current)->hash.high.u64) ||
	     ((hash.high.u64 == (*current)->hash.high.u64) && (hash.low.u64 < (*current)->hash.low.u64))) {
      len = mid;
    }
    else {
      len  -= mid;
      base  = current;
    }
  }
  while (len > 0);

  return false;
}

/*------------------------------------------------------------------------------------------------*/

size_t
sym_get_symbol_count(const symbol_table_t *Table)
{
  assert(Table != NULL);

  return Table->count;
}

/*------------------------------------------------------------------------------------------------*/

symbol_t *
sym_get_symbol(symbol_table_t *Table, const char *Name)
{
  symbol_t  *sym;
  hash128_t  hash;

  assert(Table != NULL);
  assert(Name != NULL);

  while (Table != NULL) {
    gp_hash_init(&hash);
    gp_hash_str(&hash, Name, Table->case_insensitive);
    sym = _get_symbol_from_table(Table, &hash);
    if (sym != NULL) {
      return sym;
    }

    /* If sym is still NULL, we didn't match. Try the prev table on the stack. */
    Table = Table->prev;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

symbol_t *
sym_get_symbol_len(symbol_table_t *Table, const char *Name, size_t Len)
{
  symbol_t  *sym;
  hash128_t  hash;

  assert(Table != NULL);
  assert(Name != NULL);

  while (Table != NULL) {
    gp_hash_init(&hash);
    gp_hash_str_len(&hash, Name, Len, Table->case_insensitive);
    sym = _get_symbol_from_table(Table, &hash);
    if (sym != NULL) {
      return sym;
    }

    /* If sym is still NULL, we didn't match. Try the prev table on the stack. */
    Table = Table->prev;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

symbol_t *
sym_get_symbol_with_index(symbol_table_t *Table, size_t Index)
{
  assert(Table != NULL);
  assert(Index < Table->count);

  return Table->symbol_array[Index];
}

/*------------------------------------------------------------------------------------------------*/

const symbol_t **
sym_clone_symbol_array(const symbol_table_t *Table, symbol_compare_t Cmp)
{
  size_t           size;
  const symbol_t **symbol_array;

  assert(Table != NULL);

  if (Table->count == 0) {
    return NULL;
  }

  size = Table->count * sizeof(symbol_t *);
  symbol_array = (const symbol_t **)GP_Malloc(size);
  memcpy(symbol_array, Table->symbol_array, size);

  if (Cmp != NULL) {
    qsort(symbol_array, Table->count, sizeof(symbol_t *), Cmp);
  }

  return symbol_array;
}

/*------------------------------------------------------------------------------------------------*/

void
sym_annotate_symbol(symbol_t *Sym, void *Value)
{ 
  assert(Sym != NULL);

  Sym->annotation = Value;
}

/*------------------------------------------------------------------------------------------------*/

const char *
sym_get_symbol_name(const symbol_t *Sym)
{
  assert(Sym != NULL);

  return Sym->name;
}

/*------------------------------------------------------------------------------------------------*/

void *
sym_get_symbol_annotation(const symbol_t *Sym)
{
  assert(Sym != NULL);

  return Sym->annotation;
}

/*------------------------------------------------------------------------------------------------*/

int
sym_compare_fn(const void *P0, const void *P1)
{
  const symbol_t *sym0 = *(const symbol_t **)P0;
  const symbol_t *sym1 = *(const symbol_t **)P1;

  return strcmp(sym0->name, sym1->name);
}

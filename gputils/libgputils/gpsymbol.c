
/* GP Symbol table support.

   Copyright (C) 2014-2016 Molnar Karoly

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

/*------------------------------------------------------------------------------------------------*/

static int
_hash_sort_cmp(const void *P0, const void *P1)
{
  const gp_hash_type *h0 = (const gp_hash_type *)P0;
  const gp_hash_type *h1 = (const gp_hash_type *)P1;
  const char         *s0;
  const char         *s1;

  if (h0->hash.high.u64 < h1->hash.high.u64) {
    return -1;
  }

  if (h0->hash.high.u64 > h1->hash.high.u64) {
    return 1;
  }

  if (h0->hash.low.u64 < h1->hash.low.u64) {
    return -1;
  }

  if (h0->hash.low.u64 > h1->hash.low.u64) {
    return 1;
  }

  s0 = (h0->symbol != NULL) ? h0->symbol->name : NULL;
  s1 = (h1->symbol != NULL) ? h1->symbol->name : NULL;

  if ((s0 == NULL) && (s1 == NULL)) {
    return 0;
  }

  if ((s0 != NULL) && (s1 != NULL)) {
    return strcmp(s0, s1);
  }

  if (s0 != NULL) {
    return 1;
  }

  return -1;
}

/*------------------------------------------------------------------------------------------------*/

static int
_hash_find_cmp(const void *P0, const void *P1)
{
  const gp_hash_type *h0 = (const gp_hash_type *)P0;
  const gp_hash_type *h1 = (const gp_hash_type *)P1;

  if (h0->hash.high.u64 < h1->hash.high.u64) {
    return -1;
  }

  if (h0->hash.high.u64 > h1->hash.high.u64) {
    return 1;
  }

  if (h0->hash.low.u64 < h1->hash.low.u64) {
    return -1;
  }

  if (h0->hash.low.u64 > h1->hash.low.u64) {
    return 1;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

gp_hash_type *
gp_symbol_make_hash_table(gp_object_type *Object)
{
  gp_symbol_type *symbol;
  gp_hash_type   *table;
  gp_hash_type   *tp;
  unsigned int    n_symbols;
  hash128_t      *h;

  if (Object == NULL) {
    return NULL;
  }

  n_symbols = 0;
  symbol    = Object->symbols;
  while (symbol != NULL) {
    FlagClr(symbol->opt_flags, OPT_FLAGS_GPSYMBOL_MODULE);

    if ((symbol->class != C_FILE)    && (symbol->class != C_EOF) &&
        (symbol->class != C_SECTION) && (symbol->section_name != NULL)) {
      ++n_symbols;
      FlagSet(symbol->opt_flags, OPT_FLAGS_GPSYMBOL_MODULE);
    }
    symbol = symbol->next;
  }

  table = (gp_hash_type *)GP_Calloc(n_symbols, sizeof(gp_hash_type));

  Object->symbol_hashtable      = table;
  Object->symbol_hashtable_size = n_symbols;

  tp     = table;
  symbol = Object->symbols;
  while (symbol != NULL) {
    if (FlagIsSet(symbol->opt_flags, OPT_FLAGS_GPSYMBOL_MODULE)) {
      h = &tp->hash;
      gp_hash_init(h);
      gp_hash_str(h, symbol->section_name, false);
      gp_hash_mem(h, &symbol->value, sizeof(symbol->value));
      tp->symbol = symbol;
      ++tp;
    }
    symbol = symbol->next;
  }

  qsort(table, n_symbols, sizeof(gp_hash_type), _hash_sort_cmp);
  return table;
}

/*------------------------------------------------------------------------------------------------*/

const gp_symbol_type *
gp_symbol_find(const gp_object_type *Object, const char *Section_name, gp_symvalue_t Symbol_value)
{
  gp_hash_type  gp_hash;
  gp_hash_type *ret;

  if ((Object == NULL) || (Section_name == NULL)) {
    return NULL;
  }

  if ((Object->symbol_hashtable == NULL) || (Object->symbol_hashtable_size == 0)) {
    return NULL;
  }

  gp_hash_init(&gp_hash.hash);
  gp_hash_str(&gp_hash.hash, Section_name, false);
  gp_hash_mem(&gp_hash.hash, &Symbol_value, sizeof(Symbol_value));

  ret = (gp_hash_type *)bsearch(&gp_hash, Object->symbol_hashtable, Object->symbol_hashtable_size,
                                sizeof(gp_hash_type), _hash_find_cmp);

  return ((ret != NULL) ? ret->symbol : NULL);
}

/*------------------------------------------------------------------------------------------------*/

static int
_value_cmp(const void *P0, const void *P1)
{
  const gp_symbol_type *s0 = *(const gp_symbol_type **)P0;
  const gp_symbol_type *s1 = *(const gp_symbol_type **)P1;
  gp_symvalue_t         v0 = s0->value;
  gp_symvalue_t         v1 = s1->value;

  if (v0 < v1) {
    return -1;
  }

  if (v0 > v1) {
    return 1;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

gp_symbol_type **
gp_symbol_make_label_array(gp_symbol_type *First_symbol, const char *Section_name, unsigned int *Num_labels)
{
  gp_symbol_type  *symbol;
  gp_symbol_type **array;
  unsigned int     i;
  unsigned int     n_labels;

  if ((First_symbol == NULL) || (Num_labels == NULL)) {
    return NULL;
  }

  n_labels = 0;
  symbol   = First_symbol;
  while (symbol != NULL) {
    FlagClr(symbol->opt_flags, OPT_FLAGS_GPSYMBOL_MODULE);

    if ((symbol->class == C_EXT) || (symbol->class == C_LABEL)) {
      if (Section_name != NULL) {
        if ((symbol->section_name != NULL) && (strcmp(symbol->section_name, Section_name) == 0)) {
          ++n_labels;
          FlagSet(symbol->opt_flags, OPT_FLAGS_GPSYMBOL_MODULE);
        }
      }
      else {
        ++n_labels;
        FlagSet(symbol->opt_flags, OPT_FLAGS_GPSYMBOL_MODULE);
      }
    }
    symbol = symbol->next;
  }

  if (n_labels == 0) {
    return NULL;
  }

  array = (gp_symbol_type **)GP_Malloc(n_labels * sizeof(gp_symbol_type *));

  i      = 0;
  symbol = First_symbol;
  while (symbol != NULL) {
    if (FlagIsSet(symbol->opt_flags, OPT_FLAGS_GPSYMBOL_MODULE)) {
      array[i] = symbol;
      ++i;
    }
    symbol = symbol->next;
  }

  qsort(array, n_labels, sizeof(gp_symbol_type *), _value_cmp);
  *Num_labels = n_labels;
  return array;
}

/*------------------------------------------------------------------------------------------------*/

gp_symbol_type **
gp_symbol_make_section_array(gp_symbol_type *First_symbol, unsigned int *Num_sections)
{
  gp_symbol_type  *current;
  gp_symbol_type **array;
  unsigned int     i;
  unsigned int     n_sections;

  if ((First_symbol == NULL) || (Num_sections == NULL)) {
    return NULL;
  }

  current  = First_symbol;
  n_sections = 0;
  while (current != NULL) {
    FlagClr(current->opt_flags, OPT_FLAGS_GPSYMBOL_MODULE);

    if (current->class == C_SECTION) {
      ++n_sections;
      FlagSet(current->opt_flags, OPT_FLAGS_GPSYMBOL_MODULE);
    }
    current = current->next;
  }

  if (n_sections == 0) {
    return NULL;
  }

  array = (gp_symbol_type **)GP_Malloc(n_sections * sizeof(gp_symbol_type *));

  current = First_symbol;
  i       = 0;
  while (current != NULL) {
    if (FlagIsSet(current->opt_flags, OPT_FLAGS_GPSYMBOL_MODULE)) {
      array[i] = current;
      ++i;
    }
    current = current->next;
  }

  qsort(array, n_sections, sizeof(gp_symbol_type *), _value_cmp);
  *Num_sections = n_sections;
  return array;
}

/*------------------------------------------------------------------------------------------------*/

gp_symbol_type *
gp_symbol_find_by_value(gp_symbol_type **Array, unsigned int Num_symbols, gp_symvalue_t Value)
{
  gp_symbol_type   symbol;
  gp_symbol_type  *sptr;
  gp_symbol_type **ret;

  if ((Array == NULL) || (Num_symbols == 0)) {
    return NULL;
  }

  symbol.value = Value;
  sptr         = &symbol;
  ret = (gp_symbol_type **)bsearch(&sptr, Array, Num_symbols, sizeof(gp_symbol_type *), _value_cmp);
  return ((ret != NULL) ? *ret : NULL);
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_symbol_delete_by_value(gp_symbol_type **Array, unsigned int *Num_symbols, gp_symvalue_t Value)
{
  unsigned int     n_symbols;
  gp_symbol_type   symbol;
  gp_symbol_type  *sptr;
  gp_symbol_type **ret;
  unsigned int     dst;
  unsigned int     src;
  unsigned int     num;

  if ((Array == NULL) || (Num_symbols == NULL) || ((n_symbols = *Num_symbols) == 0)) {
    return false;
  }

  symbol.value = Value;
  sptr         = &symbol;
  ret = (gp_symbol_type **)bsearch(&sptr, Array, n_symbols, sizeof(gp_symbol_type *), _value_cmp);

  if (ret != NULL) {
    dst = ret - Array;
    src = dst + 1;
    num = n_symbols - src;
    memmove(&Array[dst], &Array[src], num * sizeof(gp_symbol_type *));
    *Num_symbols = n_symbols - 1;
    return true;
  }

  return false;
}

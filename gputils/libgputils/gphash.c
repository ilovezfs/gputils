
/* Supports Fowler–Noll–Vo hash making.

   Copyright (C) 2014 Molnar Karoly

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

unsigned int
gp_fnv1a_hash(const void *Ptr, unsigned int Size, unsigned int Hash) {
  const unsigned char *p;

  if ((Ptr == NULL) || (Size == 0)) {
    return Hash;
  }

  p = (const unsigned char *)Ptr;

  while (Size--) {
    Hash ^= *p;
    Hash *= FNV1A32_PRIME;
    ++p;
  }

  return Hash;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_fnv1a_hash_str(const char *String, unsigned int Hash) {
  const unsigned char *s;
  unsigned char c;

  if (String == NULL) {
    return Hash;
  }

  s = (const unsigned char *)String;

  while ((c = *s)) {
    Hash ^= c;
    Hash *= FNV1A32_PRIME;
    ++s;
  }

  return Hash;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_fnv1a_hash_symbol(const gp_symbol_type *Symbol) {
  unsigned int hash;

  if (Symbol == NULL) {
    return 0;
  }

  hash = gp_fnv1a_hash_str(Symbol->section_name, FNV1A32_INIT);
  return gp_fnv1a_hash(&Symbol->value, sizeof(Symbol->value), hash);
}

/*------------------------------------------------------------------------------------------------*/

static int
hash_sort_cmp(const void *P0, const void *P1) {
  const gp_hash_type *h0 = (const gp_hash_type *)P0;
  const gp_hash_type *h1 = (const gp_hash_type *)P1;
  unsigned int hash0 = h0->hash;
  unsigned int hash1 = h1->hash;

  if (hash0 < hash1) {
    return -1;
  }
  else if (hash0 > hash1) {
    return 1;
  }
  else {
    const char *s0 = (h0->symbol != NULL) ? h0->symbol->name : NULL;
    const char *s1 = (h1->symbol != NULL) ? h1->symbol->name : NULL;

    if ((s0 != NULL) && (s1 != NULL)) {
      return strcmp(s0, s1);
    }
    else {
      return 0;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

static int
hash_find_cmp(const void *P0, const void *P1) {
  const gp_hash_type *h0 = (const gp_hash_type *)P0;
  const gp_hash_type *h1 = (const gp_hash_type *)P1;
  unsigned int hash0 = h0->hash;
  unsigned int hash1 = h1->hash;

  if (hash0 < hash1) {
    return -1;
  }
  else if (hash0 > hash1) {
    return 1;
  }
  else {
    return 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

gp_hash_type *
gp_make_hash_table(gp_object_type *Object) {
  gp_symbol_type *current = NULL;
  gp_hash_type *table = NULL;
  unsigned int num_symbols;
  unsigned int idx;

  if (Object == NULL) {
    return NULL;
  }

  current = Object->symbols;
  num_symbols = 0;
  while (current != NULL) {
    if ((current->class != C_FILE)    && (current->class != C_EOF) &&
        (current->class != C_SECTION) && (current->section_name != NULL)) {
      ++num_symbols;
    }
    current = current->next;
  }

  table = (gp_hash_type *)GP_Calloc(num_symbols, sizeof(gp_hash_type));

  Object->symbol_hashtable      = table;
  Object->symbol_hashtable_size = num_symbols;

  current = Object->symbols;
  idx = 0;
  while (current != NULL) {
    if ((current->class != C_FILE)    && (current->class != C_EOF) &&
        (current->class != C_SECTION) && (current->section_name != NULL)) {
      table[idx].hash   = gp_fnv1a_hash_symbol(current);
      table[idx].symbol = current;
      ++idx;
    }
    current = current->next;
  }

  qsort(table, num_symbols, sizeof(gp_hash_type), hash_sort_cmp);
  return table;
}

/*------------------------------------------------------------------------------------------------*/

const gp_symbol_type *
gp_find_symbol_hash_table(const gp_object_type *Object, const char *Section_name, gp_symvalue_t Symbol_value) {
  gp_hash_type gp_hash;
  gp_hash_type *ret;

  if ((Object == NULL) || (Section_name == NULL)) {
    return NULL;
  }

  if ((Object->symbol_hashtable == NULL) || (Object->symbol_hashtable_size == 0)) {
    return NULL;
  }

  gp_hash.hash = gp_fnv1a_hash_str(Section_name, FNV1A32_INIT);
  gp_hash.hash = gp_fnv1a_hash(&Symbol_value, sizeof(Symbol_value), gp_hash.hash);

  ret = (gp_hash_type *)bsearch(&gp_hash, Object->symbol_hashtable, Object->symbol_hashtable_size,
                                sizeof(gp_hash_type), hash_find_cmp);

  return ((ret != NULL) ? ret->symbol : NULL);
}

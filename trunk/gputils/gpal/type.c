/* types
   Copyright (C) 2004
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
#include "gpal.h"
#include "analyze.h"

struct type *
get_type(char *name)
{
  struct symbol *sym;
  struct type *type = NULL;

  sym = get_symbol(state.type_top, name);
  if (sym != NULL) {
    type = get_symbol_annotation(sym);
  }

  return type;
}

static struct type *
default_type(void)
{
  struct type *new = malloc(sizeof(*new));

  new->tag = type_unknown;
  new->size = size_unknown;
  new->nelts = 0;
  new->start = 0;
  new->end = 0;
  new->prim = NULL;
  new->access = NULL;
  new->record_bits = 0;
  new->list = NULL;

  return new;
}

void
add_type_access(char *name, char *type, char *access)
{
  struct symbol *sym;
  struct symbol *prim;
  struct type *prim_type;
  struct type *access_type;
  struct type *new;

  /* the type of the access variable */
  prim = get_symbol(state.type, type);
  assert(prim != NULL);
  prim_type = get_symbol_annotation(prim);
  assert(prim_type != NULL);

  /* the type the access variable is pointing to */
  prim = get_symbol(state.type, access);
  if (prim == NULL) {
    analyze_error(NULL, "unknown type %s", access);
    return;
  } else {
    access_type = get_symbol_annotation(prim);
    assert(access_type != NULL);
  }

  sym = get_symbol(state.type, name);
  if (sym == NULL) {
    sym = add_symbol(state.type, name);
    new = default_type();
    annotate_symbol(sym, new);
    new->tag = type_access;
    new->prim = prim_type;
    new->access = access_type;
  } else {
    analyze_error(NULL, "redefinition of type %s", name);
  }
  
  return;
}

void
add_type_alias(char *name, char *type)
{
  struct symbol *sym;
  struct symbol *prim;
  struct type *prim_type;

  prim = get_symbol(state.type, type);
  if (prim == NULL) {
    analyze_error(NULL, "unknown type %s", type);
    return;
  } else {
    prim_type = get_symbol_annotation(prim);
    assert(prim_type != NULL);
  }

  sym = get_symbol(state.type_top, name);
  if (sym == NULL) {
    sym = add_symbol(state.type_top, name);
    annotate_symbol(sym, prim_type);
  } else {
    analyze_error(NULL, "redefinition of type %s", name);
  }
  
  return;
}

void
add_type_array(char *name, int start, int end, char *type)
{
  struct symbol *sym;
  struct type *new;
  struct symbol *prim;
  struct type *prim_type = NULL;

  prim = get_symbol(state.type, type);
  if (prim == NULL) {
    analyze_error(NULL, "unknown type %s", type);
    return;
  } else {
    prim_type = get_symbol_annotation(prim);
    assert(prim_type != NULL);
    if (prim_type->tag != type_prim) {
      analyze_error(NULL, "arrays can't be of derived type %s", type);
      return;
    }
  }

  sym = get_symbol(state.type, name);
  if (sym == NULL) {
    sym = add_symbol(state.type, name);
    new = default_type();
    annotate_symbol(sym, new);
    new->tag = type_array;
    new->nelts = end - start + 1;
    new->start = start;
    new->end = end;
    new->prim = prim_type;
  } else {
    analyze_error(NULL, "redefinition of type %s", name);
  }
  
  return;
}

void
add_type_enum(char *name)
{
  struct symbol *sym;
  struct type *new;
  struct symbol *prim;
  struct type *prim_type = NULL;

  prim = get_symbol(state.type, "uint8");
  if (prim == NULL) {
    assert(0);
  } else {
    prim_type = get_symbol_annotation(prim);
    assert(prim_type != NULL);
  }

  sym = get_symbol(state.type, name);
  if (sym == NULL) {
    sym = add_symbol(state.type, name);
    new = default_type();
    annotate_symbol(sym, new);
    new->tag = type_enum;
    new->prim = prim_type;
  } else {
    analyze_error(NULL, "redefinition of type %s", name);
  }
  
  return ;
}

static void
add_type_prim(char *name, enum size_tag size)
{
  struct symbol *sym;
  struct type *new;

  sym = get_symbol(state.type, name);
  if (sym == NULL) {
    sym = add_symbol(state.type, name);
    new = default_type();
    annotate_symbol(sym, new);
    new->tag = type_prim;
    new->size = size;
  } else {
    analyze_error(NULL, "redefinition of type \"%s\"", name);
  }
  
  return;
}

void
add_type_record(char *name, tree *list)
{
  char *element_name;
  char *element_type;
  struct symbol *prim;
  struct type *prim_type;
  struct symbol *sym;
  struct type *new;
  tree *current;
  int bit_size = 0;
  int record_bits = 0;

  current = list;
  while (list) {
    assert(list->tag == node_decl);
    element_name = DECL_NAME(list);
    element_type = DECL_TYPE(list);
    prim = get_symbol(state.type, element_type);
    if (prim == NULL) {
      analyze_error(NULL, "unknown type %s", element_type);
      return;
    } else {
      prim_type = get_symbol_annotation(prim);
      assert(prim_type != NULL);
      bit_size = type_bits(prim_type);
      if ((record_bits & 0x7) && (bit_size > 7)) {
        /* align data on the byte boundary */
        record_bits &= ~7;
        record_bits += 8;
      }
      record_bits += bit_size;
    }    
    list = list->next;
  }

  sym = get_symbol(state.type, name);
  if (sym == NULL) {
    sym = add_symbol(state.type, name);
    new = default_type();
    annotate_symbol(sym, new);
    new->tag = type_record;
    new->size = size_unknown;
    new->record_bits = record_bits;
    new->list = list;
  } else {
    analyze_error(NULL, "redefinition of type %s", name);
  }
  
  return;
}

/* determine the primative size */

enum size_tag
prim_size(struct type *type)
{
  enum size_tag size = size_unknown;

  switch (type->tag) {
  case type_prim:
    size = type->size;
    break;
  case type_access:
  case type_array:
  case type_enum:
    size = type->prim->size;
    break;
  default:
    assert(0);
  }

  return size;
}

/* determine the size of a primative in bits */

int
prim_bits(enum size_tag size)
{
  int byte_size = 0;

  switch (size) {
  case size_unknown:
    assert(0);
    break;
  case size_bit:
    byte_size = 1;
    break;
  case size_uint8:
    byte_size = 8;
    break;
  case size_int8:
    byte_size = 8;
    break;
  case size_uint16:
    byte_size = 16;
    break;
  case size_int16:
    byte_size = 16;
    break;
  case size_uint24:
    byte_size = 24;
    break;
  case size_int24:
    byte_size = 24;
    break;
  case size_uint32:
    byte_size = 32;
    break;
  case size_int32:
    byte_size = 32;
    break;
  case size_float:
    byte_size = 32;
    break;
  default:
    assert(0);
  }

  return byte_size;
}

/* determine the size of a primative in bytes */

int
prim_bytes(enum size_tag size)
{
  int bit_size = prim_bits(size);
  int byte_size;
  
  if (bit_size < 8)
    byte_size = 1;
  else
    byte_size = bit_size >> 3;

  return byte_size;
}

/* determine the size of a type in bits */

int
type_bits(struct type *type)
{
  int size = 0;

  switch (type->tag) {
  case type_access:
    size = type_bits(type->prim);
    break;
  case type_array:
    size = type->nelts * type_bits(type->prim);
    break;
  case type_enum:
    size = type_bits(type->prim);
    break;
  case type_prim:
    size = prim_bits(type->size);
    break;
  case type_record:
    size = type->record_bits;
    break;
  default:
    assert(0);
  }

  return size;
}

/* determine the size of a type in bytes */

int
type_bytes(struct type *type)
{
  int bit_size = type_bits(type);
  int byte_size;
  
  if (bit_size < 8)
    byte_size = 1;
  else
    byte_size = bit_size >> 3;

  return byte_size;
}

void
add_type_prims(void)
{
  add_type_prim("bit",      size_bit);
  add_type_prim("boolean",  size_uint8);
  add_type_prim("uint8",    size_uint8);
  add_type_prim("int8",     size_int8);
  add_type_prim("uint16",   size_uint16);
  add_type_prim("int16",    size_int16);
  add_type_prim("uint24",   size_uint24);
  add_type_prim("int24",    size_int24);
  add_type_prim("uint32",   size_uint32);
  add_type_prim("int32",    size_int32);
  add_type_prim("float",    size_float);

  add_constant(strdup("false"), 0, NULL, "boolean");
  add_constant(strdup("true"), 1, NULL, "boolean");

  return;
}

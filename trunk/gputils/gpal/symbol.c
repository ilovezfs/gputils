/* symbol table
   Copyright (C) 2003, 2004
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
#include "symbol.h"
#include "analyze.h"

/* Add an pointer to an existing symbol at the top of the symbol table stack */

void
add_symbol_pointer(char *name, tree *symbol, struct variable *var)
{
  struct symbol *sym;

  sym = get_symbol(state.top, name);
  if (sym == NULL) {
    sym = add_symbol(state.top, name);
    annotate_symbol(sym, var);
  } else {
    var = get_symbol_annotation(sym);
    analyze_error(symbol,
                  "redefinition of %s,\n\talso defined in %s:%i:",
                  name,
                  get_file_name(var->file_id),
                  var->line_number);
  }

  return;
}

char *
mangle_name1(char *first)
{
  return gp_lower_case(first);
}

char *
mangle_name2(char *first, char *second)
{
  char buffer[BUFSIZ];

  snprintf(buffer, sizeof(buffer), "%s.%s", first, second);

  return gp_lower_case(buffer);
}

char *
mangle_name3(char *first, char *second, char *third)
{
  char buffer[BUFSIZ];

  snprintf(buffer, sizeof(buffer), "%s.%s.%s", first, second, third);

  return gp_lower_case(buffer);
}


static struct variable *
default_var(char *name,
            tree *symbol,
            enum sym_tag tag,
            enum node_storage storage,
            char *type)
{
  struct variable *var;

  var = malloc(sizeof(*var));
  var->name = name;
  var->tag = tag;
  var->storage = storage;
  if (type) {
    var->type = get_type(type);   
    if (var->type == NULL) {
      analyze_error(symbol, "unknown symbol type %s", type);
    }
  } else {
    var->type = NULL;
  }
  var->is_used = false;
  var->is_assigned = false;
  var->is_absolute = false;
  var->value = 0;
  var->address = 0;
  if (symbol) {
    var->file_id = symbol->file_id;
    var->line_number = symbol->line_number;
  } else {
    var->file_id = 0;
    var->line_number = 0;
  }
  var->node = symbol;
  var->module = state.module;

  return var;
}

/* Add one symbol to the top table at the top of the symbol table
   stack.  */

struct variable *
add_top_symbol(char *name,
               tree *symbol,
               enum sym_tag tag,
               enum node_storage storage,
               char *type)
{
  struct symbol *sym;
  struct variable *var;

  sym = get_symbol(state.top, name);
  if (sym == NULL) {
    sym = add_symbol(state.top, name);
    var = default_var(name, symbol, tag, storage, type);
    annotate_symbol(sym, var);
  } else {
    var = get_symbol_annotation(sym);
    analyze_error(symbol,
                  "redefinition of %s,\n\talso defined in %s:%i:",
                  name,
                  get_file_name(var->file_id),
                  var->line_number);
  }

  return var;
}

/* Add one symbol to the global table at the base of the symbol table
   stack.  */

struct variable *
add_global_symbol(char *name,
                  tree *symbol,
                  enum sym_tag tag,
                  enum node_storage storage,
                  char *type)
{
  struct symbol *sym;
  struct variable *var;

  sym = get_symbol(state.global, name);
  if (sym == NULL) {
    sym = add_symbol(state.global, name);
    var = default_var(name, symbol, tag, storage, type);
    annotate_symbol(sym, var);
  } else {
    var = get_symbol_annotation(sym);
    analyze_error(symbol,
                  "redefinition of %s,\n\talso defined in %s:%i:",
                  name,
                  get_file_name(var->file_id),
                  var->line_number);
  }

  return var;
}

struct variable *
get_global(char *name)
{
  struct symbol *sym;
  struct variable *var = NULL;

  sym = get_symbol(state.top, name);
  if (sym != NULL) {
    var = get_symbol_annotation(sym);
  }

  return var;
}

struct variable *
add_constant(char *name, int value, tree *node, char *type)
{
  struct variable *var;

  var = add_global_symbol(name,
                          node,
                          sym_const,
                          storage_unknown,
                          type);
  if (var) {
    var->value = value;
  }

  return var;
}

static void
add_type_prim(char *name, enum size_tag size)
{
  struct symbol *sym;
  struct type *new;

  sym = get_symbol(state.type, name);
  if (sym == NULL) {
    sym = add_symbol(state.type, name);
    new = malloc(sizeof(*new));
    annotate_symbol(sym, new);
    new->tag = type_prim;
    new->size = size;
    new->nelts = 0;
    new->start = 0;
    new->end = 0;
    new->prim = NULL;
  } else {
    analyze_error(NULL, "redefinition of type \"%s\"", name);
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
    new = malloc(sizeof(*new));
    annotate_symbol(sym, new);
    new->tag = type_array;
    new->size = size_unknown;
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
    new = malloc(sizeof(*new));
    annotate_symbol(sym, new);
    new->tag = type_enum;
    new->size = size_unknown;
    new->nelts = 0;
    new->start = 0;
    new->end = 0;
    new->prim = prim_type;
  } else {
    analyze_error(NULL, "redefinition of type %s", name);
  }
  
  return ;
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

  sym = get_symbol(state.type, name);
  if (sym == NULL) {
    sym = add_symbol(state.type, name);
    annotate_symbol(sym, prim_type);
  } else {
    analyze_error(NULL, "redefinition of type %s", name);
  }
  
  return;
}

struct type *
get_type(char *name)
{
  struct symbol *sym;
  struct type *type = NULL;

  sym = get_symbol(state.type, name);
  if (sym != NULL) {
    type = get_symbol_annotation(sym);
  }

  return type;
}

/* determine the size of a primative in bytes */

int
prim_size(enum size_tag size)
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
    byte_size = 1;
    break;
  case size_int8:
    byte_size = 1;
    break;
  case size_uint16:
    byte_size = 2;
    break;
  case size_int16:
    byte_size = 2;
    break;
  case size_uint24:
    byte_size = 3;
    break;
  case size_int24:
    byte_size = 3;
    break;
  case size_uint32:
    byte_size = 4;
    break;
  case size_int32:
    byte_size = 4;
    break;
  case size_float:
    byte_size = 4;
    break;
  default:
    assert(0);
  }

  return byte_size;
}

/* determine the primative size */

enum size_tag
prim_type(struct type *type)
{
  enum size_tag size = size_unknown;

  switch (type->tag) {
  case type_prim:
    size = type->size;
    break;
  case type_array:
  case type_enum:
    size = type->prim->size;
    break;
  default:
    assert(0);
  }

  return size;
}

/* determine the size of a type in bytes */

int
type_size(struct type *type)
{
  int size = 0;

  switch (type->tag) {
  case type_prim:
    size = prim_size(type->size);
    break;
  case type_array:
    size = type->nelts * type_size(type->prim);
    break;
  case type_enum:
    size = type_size(type->prim);
    break;
  default:
    assert(0);
  }

  return size;
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

/* return address of the bank in the variable */

char *
var_bank(struct variable *var)
{
  char *bank = NULL;
 
  switch (var->tag) {
  case sym_idata:
  case sym_udata:
    if (var->is_absolute) {
      bank = var->name;
    } else {
      bank = FILE_DATA_ADDR(var->module);
    }
    break;
  default:
    assert(0);
  }

  return bank;
}

/* return address of the bank in the variable */

char *
var_page(struct variable *var)
{
  char *page = NULL;
  
  switch (var->tag) {
  case sym_func:
  case sym_label:
  case sym_proc:
    if (var->is_absolute) {
      page = var->name;
    } else {
      page = FILE_CODE_ADDR(var->module);
    } 
    break;
  default:
    assert(0);
  }

  return page;
}


/* return true if the symbol is data memory */

gp_boolean
is_data(struct variable *var)
{
  assert(var != NULL);

  switch (var->tag) {
  case sym_idata:
  case sym_udata:
    return true;
  default:
    return false;
  }

}

/* return true if the symbol is in the current module */

gp_boolean
in_module(struct variable *var)
{
  assert(var != NULL);

  switch (var->storage) {
  case storage_public:
  case storage_private:
  case storage_local:
    return true;
  default:
    return false;
  }

}

/* return true if the symbol is external to current module */

gp_boolean
is_extern(struct variable *var)
{
  assert(var != NULL);

  switch (var->storage) {
  case storage_near:
  case storage_far:
    return true;
  default:
    return false;
  }

}

/* return true if the symbol needs a banksel */

gp_boolean
is_far(struct variable *var)
{
  if ((var->storage == storage_far) ||
      (var->is_absolute)) {
    return true;
  } else {
    return false;
  }
}

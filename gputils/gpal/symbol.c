/* symbol table
   Copyright (C) 2003, 2004, 2005
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

char *
mangle_name1(char *first)
{
  return strdup(first);
}

char *
mangle_name2(char *first, char *second)
{
  char buffer[BUFSIZ];

  snprintf(buffer, sizeof(buffer), "%s.%s", first, second);

  return strdup(buffer);
}

char *
mangle_name3(char *first, char *second, char *third)
{
  char buffer[BUFSIZ];

  snprintf(buffer, sizeof(buffer), "%s.%s.%s", first, second, third);

  return strdup(buffer);
}

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
  var->is_volatile = false;
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
    } else if (var->storage == storage_near) {
      bank = FILE_DATA_ADDR(state.module);
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
  case sym_label:
  case sym_subprogram:
    if (var->is_absolute) {
      page = var->name;
    } else if (var->storage == storage_near) {
      page = FILE_CODE_ADDR(state.module);
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

static int
prim_coff_type(struct type *type)
{
  int coff_type = T_NULL;

  switch (type->tag) {
  case type_access:
    coff_type = (DT_PTR << 4) | prim_coff_type(type->prim);
    break;
  case type_array:
    coff_type = (DT_ARY << 4) | prim_coff_type(type->prim);
    break;
  case type_enum:
    coff_type = T_ENUM;
    break;
  case type_prim:
    switch (type->size) {
    case size_unknown:
      assert(0);
      break;
    case size_bit:
      coff_type = T_NULL;
      break;
    case size_uint8:
      coff_type = T_UCHAR;
      break;
    case size_int8:
      coff_type = T_CHAR;
      break;
    case size_uint16:
      coff_type = T_USHORT;
      break;
    case size_int16:
      coff_type = T_SHORT;
      break;
    case size_uint24:
      coff_type = T_USLONG;
      break;
    case size_int24:
      coff_type = T_SLONG;
      break;
    case size_uint32:
      coff_type = T_ULONG;
      break;
    case size_int32:
      coff_type = T_LONG;
      break;
    case size_float:
      coff_type = T_FLOAT;
      break;
    default:
      assert(0);
    }
    break;
  case type_record:
    coff_type = T_STRUCT;
    break;
  default:
    assert(0);
  }

  return coff_type;
}

int
var_coff_type(struct variable *var)
{
  int coff_type = T_NULL;

  assert(var != NULL);

  if (var->tag == sym_subprogram)
    return (DT_FCN << 4);

  if (var->type == NULL)
    return T_NULL;

  coff_type = prim_coff_type(var->type);

  return coff_type;
}

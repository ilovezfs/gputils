/* symbol table
   Copyright (C) 2003
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

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

/* primative types */

enum size_tag {
  size_unknown,
  size_bit,				/* one bit packed */
  size_uint8,				/* one byte unsigned */
  size_int8,				/* one byte signed */
  size_uint16,				/* two bytes unsigned */
  size_int16,				/* two bytes signed */
  size_uint24,				/* three bytes unsigned */
  size_int24,				/* three bytes signed */
  size_uint32,				/* four bytes unsigned */
  size_int32,				/* four bytes signed */
  size_float				/* IEEE 754 float (four bytes) */
};

/* type data */

enum type_tag {
  type_unknown,
  type_prim,				/* primative (bit, uint8, ...) */
  type_array,				/* array of other types */
  type_enum				/* enumeration */
};

struct type {
  enum type_tag tag;                    /* which type */
  enum size_tag size;			/* size, if it is a primative */
  int nelts;				/* number of array elements  */
  int start;				/* first element number or value */
  int end;				/* last element number or value */
  struct type *prim;			/* primative type it is derived from */  
};

/* symbol data */

enum sym_tag {
  sym_unknown,
  sym_const,				/* constant */
  sym_func,				/* function */
  sym_idata,				/* initialized data */
  sym_proc,				/* procedure */
  sym_udata				/* uninitialized data */
};

struct variable {
  char *name;				/* symbol name */
  enum sym_tag tag;			/* symbol tag */
  enum node_storage storage;		/* storage class */
  struct type *type;			/* symbol type */
  gp_boolean is_used;			/* is used in expr */
  gp_boolean is_assigned;		/* has been assigned a value */
  int value;				/* value if constant symbol */
  int file_id;				/* file symbol was defined in */
  int line_number;			/* line number symbol was defined on */
  tree *node;				/* parse tree */
  tree *module;				/* module the node was located in */
};

void add_symbol_alias(char *name, tree *symbol, struct variable *var);
char *mangle_name1(char *first);
char *mangle_name2(char *first, char *second);
char *mangle_name3(char *first, char *second, char *third);
struct variable *add_global_symbol(char *name,
                                   tree *symbol,
                                   enum sym_tag tag,
                                   enum node_storage storage,
                                   char *type);
struct variable *get_global(char *name);
struct variable *add_constant(char *name, int value, tree *node, char *type);
struct variable *add_label(char *name, tree *node);

void add_type_array(char *name, int start, int end, char *type);
void add_type_enum(char *name);
void add_type_alias(char *name, char *type);
struct type *get_type(char *name);
enum size_tag prim_type(struct type *type);
int prim_size(enum size_tag size);
int type_size(struct type *type);
void add_type_prims(void);

gp_boolean is_data(struct variable *var);
gp_boolean in_module(struct variable *var);
gp_boolean is_extern(struct variable *var);

#define SYM_TYPE(x) (x->type->tag)

#endif

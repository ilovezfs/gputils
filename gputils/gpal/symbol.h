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

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

/* symbol data */

enum sym_tag {
  sym_unknown,
  sym_addr,				/* symbol address */
  sym_alias,				/* symbol alias */
  sym_const,				/* constant */
  sym_idata,				/* initialized data */
  sym_label,				/* label */
  sym_subprogram,			/* subprogram */
  sym_udata				/* uninitialized data */
};

struct variable {
  char *name;				/* symbol name */
  enum sym_tag tag;			/* symbol tag */
  enum node_storage storage;		/* storage class */
  struct type *type;			/* symbol type */
  gp_boolean is_used;			/* is used in expr */
  gp_boolean is_assigned;		/* has been assigned a value */
  gp_boolean is_absolute;		/* has an absolute address */
  gp_boolean is_volatile;		/* is volatile */
  int value;				/* value if constant symbol */
  int address;				/* address if abosolute */
  int file_id;				/* file symbol was defined in */
  int line_number;			/* line number symbol was defined on */
  tree *node;				/* parse tree */
  tree *module;				/* module the node was located in */
};

char *mangle_name1(char *first);
char *mangle_name2(char *first, char *second);
char *mangle_name3(char *first, char *second, char *third);
void add_symbol_pointer(char *name, tree *symbol, struct variable *var);
struct variable *get_global(char *name);
struct variable *add_top_symbol(char *name,
                                tree *symbol,
                                enum sym_tag tag,
                                enum node_storage storage,
                                char *type);
struct variable *add_global_symbol(char *name,
                                   tree *symbol,
                                   enum sym_tag tag,
                                   enum node_storage storage,
                                   char *type);
struct variable *add_constant(char *name, int value, tree *node, char *type);

char *var_bank(struct variable *var);
char *var_page(struct variable *var);
gp_boolean is_data(struct variable *var);
gp_boolean in_module(struct variable *var);
gp_boolean is_extern(struct variable *var);
gp_boolean is_far(struct variable *var);

int var_coff_type(struct variable *var);

#endif

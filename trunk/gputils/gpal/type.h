/* types
   Copyright (C) 2004, 2005
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

#ifndef __TYPE_H__
#define __TYPE_H__

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
  type_access,				/* access types */
  type_array,				/* array of other types */
  type_enum,				/* enumeration */
  type_prim,				/* primative (bit, uint8, ...) */
  type_record				/* record */
};

struct type {
  enum type_tag tag;                    /* which type */
  enum size_tag size;			/* size, if it is a primative */
  int nelts;				/* number of array elements  */
  int start;				/* first element number or value */
  int end;				/* last element number or value */
  struct type *prim;			/* primative type it is derived from */  
  struct type *access;			/* primative type it is accessing */  
  int record_bits;                      /* size in bits if a record */
  tree *list;                           /* list of delaration if a record */
};

struct type *get_type(char *name);

void add_type_access(char *name, char *type, char *access);
void add_type_alias(char *name, char *type);
void add_type_array(char *name, int start, int end, char *type);
void add_type_enum(char *name);
void add_type_record(char *name, tree *list);

enum size_tag prim_size(struct type *type);
int prim_bites(enum size_tag size);
int prim_bytes(enum size_tag size);
int type_bits(struct type *type);
int type_bytes(struct type *type);
void add_type_prims(void);

#define SYM_TYPE(x) (x->type->tag)

#endif

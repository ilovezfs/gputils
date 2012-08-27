/* Symbol table support
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman

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

#ifndef __GPSYMBOL_H__
#define __GPSYMBOL_H__

#define HASH_SIZE 173  /* Too small and we get collisions.  Too big
                        * and we use up memory and run slow.. */

struct symbol {
  const char *name;
  void *annotation;
  struct symbol *next;
};

struct symbol_table {
  int count;
  gp_boolean case_insensitive;
  int (*compare)(const char *__s1, const char *__s2);
  int (*compare_len)(const char *__s1, const char *__s2, size_t len);
  struct symbol *hash_table[HASH_SIZE];
  struct symbol_table *prev;
};

struct symbol_table *push_symbol_table(struct symbol_table *,
                                       gp_boolean case_insensitive);
struct symbol_table *pop_symbol_table(struct symbol_table *);

struct symbol *add_symbol(struct symbol_table *, const char *name);
int remove_symbol(struct symbol_table *table, const char *name);
struct symbol *get_symbol(struct symbol_table *, const char *name);
struct symbol *get_symbol_len(struct symbol_table *, const char *name, size_t len);

void annotate_symbol(struct symbol *, void *);

const char *get_symbol_name(const struct symbol *);
void *get_symbol_annotation(const struct symbol *);

int symbol_compare(const void *p0, const void *p1);

#endif

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

#include "stdhdr.h"
#include "libgputils.h"

/* Base the hash func on the 1st, 2nd, 3rd and last characters of the
   string, and its length. */

static int hashfunc_len(struct symbol_table *t, const char *s, size_t len)
{
  union {
    char b[4];
    unsigned long ul;
  } change;

  change.ul = 0;

  change.b[0] = s[0];
  if (len > 1) {
    change.b[1] = s[1];
    change.b[3] = s[len - 1];
    if (len > 2)
      change.b[2] = s[2];
  }

  if (t->case_insensitive) {
    change.ul &= 0x1f1f1f1f;
  }
  change.ul += (len << 3);

  return (change.ul % HASH_SIZE);
}

static int hashfunc(struct symbol_table *t, const char *s)
{
  return hashfunc_len(t, s, strlen(s));
}

static int compare_len_case(const char *s1, const char *s2, size_t n)
{
  return (n == strlen(s2)) ? strncasecmp(s1, s2, n) : -1;
}

static int compare_len(const char *s1, const char *s2, size_t n)
{
  return (n == strlen(s2)) ? strncmp(s1, s2, n) : -1;
}

struct symbol_table *push_symbol_table(struct symbol_table * table,
                                       gp_boolean case_insensitive)
{
  struct symbol_table *new = GP_Calloc(1, sizeof(struct symbol_table));

  new->case_insensitive = case_insensitive;

  if (case_insensitive) {
    new->compare = strcasecmp;
    new->compare_len = compare_len_case;
  }
  else {
    new->compare = strcmp;
    new->compare_len = compare_len;
  }

  new->prev = table;
  return new;
}

struct symbol_table *pop_symbol_table(struct symbol_table *table)
{
  struct symbol_table *prev;

  prev = table->prev;

  return prev;
}

struct symbol *add_symbol(struct symbol_table *table, const char *name)
{
  struct symbol *r;
  int index;

  assert(name != NULL);
  assert(table != NULL);

  index = hashfunc(table, name);

  r = table->hash_table[index];
  while ((r != NULL) && ((*table->compare)(name, r->name) != 0)) {
    r = r->next;
  }

  if (r == NULL) {     /* No match. */
    r = GP_Malloc(sizeof(*r));
    r->name = GP_Strdup(name);
    r->next = table->hash_table[index];
    r->annotation = NULL;
    table->hash_table[index] = r;
    table->count++;
  }

  return r;
}

/* FIXME: remove_symbol does not search all of the symbol tables in the stack.
Maybe this is ok, but it seems wrong. */

int remove_symbol(struct symbol_table *table, const char *name)
{
  struct symbol *r = NULL;
  struct symbol *last = NULL;
  int index;
  int found_symbol = 0;

  assert(name != NULL);
  assert(table != NULL);

  index = hashfunc(table, name);

  /* Search for the symbol */
  if (table != NULL) {
    r = table->hash_table[index];
    while ((r != NULL) && ((*table->compare)(name, r->name) != 0)) {
      last = r;
      r = r->next;
    }
  }

  if (r != NULL) {
    /* remove the symbol */
    if (last != NULL) {
      last->next = r->next;
    } else {
      /* r was first in the list */
      table->hash_table[index] = r->next;
    }
    table->count--;
    found_symbol = 1;

    if (r->name != NULL) {
      free((void *)r->name);
    }

    free(r);
  }

  return found_symbol;
}

struct symbol *get_symbol(struct symbol_table *table, const char *name)
{
  struct symbol *r = NULL;

  assert(name != NULL);

  while (table != NULL) {
    int index = hashfunc(table, name);

    r = table->hash_table[index];
    while ((r != NULL) && ((*table->compare)(name, r->name) != 0)) {
      r = r->next;
    }

    if (r != NULL) {
      break;
    }

    /* If r is still NULL, we didn't match.  Try the prev table on the stack */
    table = table->prev;
  }

  return r;
}

struct symbol *get_symbol_len(struct symbol_table *table, const char *name, size_t len)
{
  struct symbol *r = NULL;

  assert(name != NULL);

  while (table != NULL) {
    int index = hashfunc_len(table, name, len);

    r = table->hash_table[index];
    while ((r != NULL) && ((*table->compare_len)(name, r->name, len) != 0)) {
      r = r->next;
    }

    if (r != NULL) {
      break;
    }

    /* If r is still NULL, we didn't match.  Try the prev table on the stack */
    table = table->prev;
  }

  return r;
}

void annotate_symbol(struct symbol *s, void *a)
{
  s->annotation = a;
}

const char *get_symbol_name(const struct symbol *s)
{
  return s->name;
}

void *get_symbol_annotation(const struct symbol *s)
{
  return s->annotation;
}

int symbol_compare(const void *p0, const void *p1)
{
  struct symbol *s0 = *(struct symbol **)p0;
  struct symbol *s1 = *(struct symbol **)p1;

  return strcmp(s0->name, s1->name);
}

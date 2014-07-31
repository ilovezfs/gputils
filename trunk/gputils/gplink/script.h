/* Linker script processing
   Copyright (C) 2001, 2002, 2003, 2004, 2005
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

#ifndef SCRIPT_H
#define SCRIPT_H

/* Parse node: created by the parser, interpreted by the 'backend' */

struct pnode {
  enum pnode_tag {
    PTAG_CONSTANT,
    PTAG_SYMBOL,
    PTAG_STRING,
    PTAG_LIST,
    PTAG_BINOP,
    PTAG_UNOP
    } tag;

  union {
    long constant;
    const char *symbol;

    struct {
      struct pnode *head, *tail;
    } list;

    struct {
      int op;
      struct pnode *p0, *p1;
    } binop;

    struct {
      int op;
      struct pnode *p0;
    } unop;

    const char *string;
  } value;
};

void script_error(const char *messg, const char *detail);
int add_path(struct pnode *parms);
void add_script_macro(const char *name, long value);
long get_script_macro(const char *name);
int execute_command(const char *name, struct pnode *parms);

#endif

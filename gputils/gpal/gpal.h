/* Common definitions for gpal
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

#ifndef __GPAL_H__
#define __GPAL_H__

#define GPAL_VERSION_STRING ("gpal-" VERSION " pre-alpha")

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

#include "tree.h"

extern struct gpal_state {
  gp_boolean compile_only;		/* compile, but don't assemble or link */
  gp_boolean no_link;			/* compile and assemble, but don't link */
  gp_boolean archive;			/* compile and assemble, then link */
  gp_boolean delete_temps;		/* delete temporary files */
  char *options;			/* extra link or lib options */
  struct {
    int level;
    gp_boolean auto_inline;
    gp_boolean constant_folding;
    gp_boolean dead_code;
    gp_boolean peep;
    gp_boolean second_pass;
    gp_boolean strength_reduction;
    gp_boolean tail_calls;
    gp_boolean tree_shape;
    gp_boolean trival_expressions;
    gp_boolean unused_mem;
  } optimize;
  gp_linked_list *path;			/* list of include paths */
  gp_linked_list *file;			/* list of compiled files */
  gp_boolean cmd_processor;		/* processor chose on command line */
  enum pic_processor processor;
  enum proc_class class;      		/* Processor class */
  struct px *processor_info;    	/* Processor identifiers (e.g. name) */
  gp_boolean processor_chosen;		/* Nonzero after processor-specific init */
  struct {				/* Processor data */
    char *code;      			/* code section name */
    int code_addr;			/* absolute address of code section */
    gp_boolean code_addr_valid;		/* is code address valid? */
    char *udata;      			/* uninitialized data section name */
    int udata_addr;			/* absolute address of udata section */
    gp_boolean udata_addr_valid;	/* is udata address valid? */
  } section;
  struct symbol_table
    *global,				/* Base of Global symbols */
    *top,				/* Top of Global symbols */
    *memory,				/* All data memory */
    *type;				/* Symbol Types */
  tree *root;				/* start of tree */
  tree *module;				/* current tree being processed  */
  struct source_context *src;		/* Top of the stack of source files */
  char *basefilename;			/* base filename */
  char *srcfilename;			/* source filename */
  char *outfilename;			/* output filename */
  char asmfilename[BUFSIZ];		/* asm output filename */
  struct {
    FILE *f;  				/* output assembly file */
  } output;
} state;

struct source_context {
  char *name;
  enum source_type type;
  int file_id;
  FILE *f;
  struct yy_buffer_state *yybuf;
  unsigned int line_number;
  struct source_context *prev;
};

struct file_struct {
  char *name;			/* full name including path, if used */
  int file_id;			/* the number of the current file */
  gp_boolean is_temp;		/* this is a temporary file */
  gp_boolean is_link;		/* a file to add to the link list */
};

void add_entity(tree *node);
int add_file(char *name,
             char *extension,
             gp_boolean is_temp,
             gp_boolean is_link);
char *get_file_name(int id);

#endif

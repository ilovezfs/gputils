/* gplink - GNU PIC Linker
   Copyright (C) 2001 Craig Franklin
 
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

#ifndef GPLINK_H
#define GPLINK_H

#define GPLINK_VERSION_STRING ("gplink-" VERSION " pre-alpha")

#define MAX_NAMES 256
#define MAX_PATHS 100

#include "gpmemory.h"
#include "gpwritehex.h"

enum modes { _hex, _object};

extern struct gplink_state {
  enum modes mode;                 /* default mode */ 
  enum formats hex_format;         /* format of the output */
  int quiet;			   /* suppress linker outputs when 1 */ 
  int debug;                       /* enable gplink debug features */
  char *paths[MAX_PATHS];          /* the list of include paths */
  int numpaths;                    /* number of paths in the list */
  struct {			   /* Totals for errors, warnings, messages */
    int errors;
    int warnings;
    int messages;
  } num;
  char  *srcfilename;		   /* Script file name */
  char  *basefilename;		   /* Script file name */
  struct source_context *src;	   /* Top of the stack of the script files */
  struct objectlist     *objects;
  struct archivelist    *archives;
  struct {
    struct symbol_table *definition; /* section definitions from script */
    struct symbol_table *logical;    /* logical definitions from script */
  } section;
  struct {
    struct symbol_table *definition; /* external symbol definition */
    struct symbol_table *missing;    /* missing external symbols */
    struct symbol_table *archive;    /* archive symbol index */
  } symbol;
  struct objectfile *output;         /* output object file */
} state;

struct source_context {
  char *name;
  FILE *f;
  unsigned int line_number;
  struct yy_buffer_state *yybuf;
  struct source_context *prev;
};

void gplink_error(char *messg);
void gplink_warning(char *messg);
void gplink_debug(char *messg);
void gplink_open_coff(char *name);
void gplink_add_path(char *path);

#endif

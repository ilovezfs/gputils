/* gplink - GNU PIC Linker
   Copyright (C) 2001, 2002, 2003
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

#ifndef GPLINK_H
#define GPLINK_H

#define GPLINK_VERSION_STRING ("gplink-" VERSION " pre-alpha")

#define MAX_PATHS 100

struct archivelist {
  char                *name;
  gp_archive_type     *archive;
  struct archivelist  *next;
};

enum outfile { normal, suppress, named };

extern struct gplink_state {
  enum formats hex_format;         /* format of the output */
  char *paths[MAX_PATHS];          /* the list of include paths */
  int numpaths;                    /* number of paths in the list */
  enum pic_processor processor;
  enum proc_class class;
  enum outfile
    codfile,			   /* Symbol output file control */
    hexfile,			   /* Hex output file control */
    mapfile,			   /* Map output file control */
    objfile;			   /* Executable object file control */
  int fill_enable;		   /* Fill unused program memory with value */
  unsigned int fill_value;	   /* Value to fill program memory with */
  char  *srcfilename,		   /* Script file name */
    basefilename[BUFSIZ],	   /* basename for generating hex,list,symbol filenames */
    codfilename[BUFSIZ],	   /* Symbol (.cod) file name */
    hexfilename[BUFSIZ],	   /* Hex (.hex) file name */
    mapfilename[BUFSIZ],	   /* List (.map) file name */
    objfilename[BUFSIZ];	   /* Object (.o) file name */
  struct source_context *src;	   /* Top of the stack of the script files */
  struct {			   /* Map file state: */
    FILE *f;			     /*   Map file output */
    char startdate[80];		     /*   When assembly started */
  } map;
  struct {			   /* Symbol file state: */
    FILE *f;			     /*   Symbol file output */
    int enabled;		     /*   nonzero if symbol file is enabled */
    int emitting;                    /*   flag indicating when an opcode is emitted */
  } cod;
  struct {
    struct symbol_table *definition; /* section definitions from script */
    struct symbol_table *logical;    /* logical definitions from script */
  } section;
  struct {
    struct symbol_table *definition; /* external symbol definition */
    struct symbol_table *missing;    /* missing external symbols */
    struct symbol_table *archive;    /* archive symbol index */
  } symbol;
  struct archivelist    *archives;
  gp_object_type        *object;     /* object files */
  MemBlock              *i_memory;   /* Instruction memory linked list */
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

/* GNU PIC general coff functions
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

#ifndef __GPCOFFGEN_H__
#define __GPCOFFGEN_H__

typedef struct gp_reloc_type {
  struct reloc relocation;
  struct gp_reloc_type *next;
} gp_reloc_type;

typedef struct gp_linenum_type {
  struct lineno linenumber;
  struct gp_linenum_type *next;
} gp_linenum_type;

typedef struct gp_symbol_type {
  struct syment symbol;            /* symbol */
  struct gp_symbol_type *next;
} gp_symbol_type;

typedef struct gp_section_type {
  unsigned int     number;          /* section number */
  struct scnhdr    header;
  MemBlock        *data;            /* Memory linked list */
  gp_reloc_type   *relocations;
  gp_linenum_type *line_numbers;
  struct gp_section_type *next;
} gp_section_type;

/* String table offsets are 16 bits so this coff has a limit on the 
   maximum string table size. */
#define MAX_STRING_TABLE 0xffff

typedef struct gp_object_type {
  char            *filename;                        /* object filename */
  struct filehdr   file_header;                     /* coff file header */
  struct opthdr    opt_header;                      /* coff optional header */
  gp_section_type *sections;                        /* sections */
  gp_symbol_type  *sym_table;                       /* Symbol table */
  char             string_table[MAX_STRING_TABLE];  /* String table */
} gp_object_type;

/* annotation for symbol tables used by the linker and archive tool */
typedef struct gp_coffsymbol_type {
  struct syment  *coffsym;  /* the coff symbol */
  gp_object_type *file;     /* the object file the symbol is defined in */
} gp_coffsymbol_type;

void _fput_16(short data, FILE *fp);
void _fput_32(long data, FILE *fp); 
short _get_16(char *addr);
long _get_32(char *addr);
void _put_16(char *addr, short data);
void _put_32(char *addr, long data);
gp_object_type *
gp_coffgen_init(char * name, enum pic_processor processor);
void gp_coffgen_fetch_symbol_name(gp_object_type *object, 
                                  struct syment *symbol,
			          char *string);
void gp_coffgen_fetch_section_name(gp_object_type *object, 
                                   struct scnhdr *header,
			           char *string);
gp_section_type *gp_coffgen_findsection(gp_object_type *object, char *name);
int gp_coffgen_addstring(gp_object_type *object, char *name);                                   
void gp_coffgen_addname(gp_object_type *object, char *ptr, char *name);
gp_section_type *gp_coffgen_addsection(gp_object_type *object, char *name);
gp_reloc_type *gp_coffgen_addreloc(gp_section_type *section);
gp_linenum_type *gp_coffgen_addlinenum(gp_section_type *section);
gp_symbol_type *
gp_coffgen_findsymbol(gp_object_type *object, char *name);
gp_symbol_type *
gp_coffgen_findsymbolnum(gp_object_type *object, unsigned int number);
gp_symbol_type *
gp_coffgen_addsymbol(gp_object_type *object, char *name, int num_aux);
int gp_coffgen_free(gp_object_type *object);
int gp_coffgen_updateptr(gp_object_type *object);
int gp_coffgen_writecoff(gp_object_type *object);

#endif

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

/* These definitions are for the COFF as stored in memory.  gpcoff.h defines
   the COFF as stored in a file. */

/* String table offsets are 16 bits so this coff has a limit on the 
   maximum string table size. */
#define MAX_STRING_TABLE 0xffff

/* relocation linked list */

typedef struct gp_reloc_type 
{
  /* entry relative address */
  long address;

  /* symbol */
  struct gp_symbol_type *symbol;

  /* symbol number, only valid when generating a coff file */
  long symbol_number;

  /* offset added to address of symbol */
  short offset;

  /* relocation type */
  unsigned short type;

  struct gp_reloc_type *next;
} gp_reloc_type;

/* line number linked list */

typedef struct gp_linenum_type 
{
  /* source file symbol */
  struct gp_symbol_type *symbol;

  /* line number */
  unsigned short line_number; 

  /* address of code for this line number */
  unsigned long address;     

  struct gp_linenum_type *next;
} gp_linenum_type;

/* auxilary symbol linked list */

typedef struct gp_aux_type 
{
  /* auxilary symbol type */
  long type;

  /* FIXME: Finish the aux entries. */
  union {
    struct {
      char *filename;
      unsigned long line_number; 
    } _aux_file;
    struct {
      unsigned long  length; 
      unsigned short nreloc; 
      unsigned short nlineno;
    } _aux_scn;
    char data[SYMBOL_SIZE];
  } _aux_symbol;
  
  struct gp_aux_type *next;
} gp_aux_type;

/* symbol linked list */

typedef struct gp_symbol_type 
{
  /* symbol name */
  char *name;

  /* symbol value */ 
  long value;

  /* section number, only for used for determining symbol type: 
     N_DEBUG = -2, N_ABS = -1, N_UNDEF = 0, or N_SCNUM = 1 if defined */
  short section_number;
  
  /* defining section */ 
  struct gp_section_type *section;

  /* type */
  unsigned short type;

  /* storage class */
  char class;

   /* number of auxiliary symbols */
  char num_auxsym;

  /* auxilary symbols */
  struct gp_aux_type *aux_list;

  /* symbol number, only valid when writing coff file */  
  unsigned long number;

  struct gp_symbol_type *next;
} gp_symbol_type;

/* section linked list */

typedef struct gp_section_type 
{
  /* section name */
  char *name;

  /* section symbol */
  struct gp_symbol_type *symbol;

  /* flags */
  unsigned long flags;
  
  /* section address */
  unsigned long address;
  
  /* section size in bytes */
  unsigned long size;
  
  /* memory linked list */
  MemBlock *data;

  /* number of relocations */
  unsigned short num_reloc;

  /* head of relocations */
  gp_reloc_type *relocations;

  /* tail of relocations */
  gp_reloc_type *relocations_tail;

  /* number of line numbers */
  unsigned short num_lineno;

  /* head of line numbers */
  gp_linenum_type *line_numbers;

  /* tail of line numbers */
  gp_linenum_type *line_numbers_tail;

  /* section number, only valid when writing coff file */  
  unsigned long number;

  /* data pointer, only valid when writing coff file */  
  unsigned long data_ptr;

  /* relocations pointer, only valid when writing coff file */  
  unsigned long reloc_ptr;

  /* linenumber pointer, only valid when writing coff file */  
  unsigned long lineno_ptr;

  struct gp_section_type *next;
} gp_section_type;

typedef struct gp_object_type 
{
  /* object filename */
  char *filename;

  /* processor */
  enum pic_processor processor;
  
  /* processor class */
  enum proc_class class;
  
  /* time object was created */
  long time;
  
  /* flags */
  unsigned short flags;
  
  /* number of sections */
  long num_sections;

  /* head of sections */
  gp_section_type *sections;

  /* tail of sections */
  gp_section_type *sections_tail;

  /* number of symbols */
  long num_symbols;
  
  /* head of symbols */
  gp_symbol_type *symbols;

  /* tail of symbols */
  gp_symbol_type *symbols_tail;

  /* symbol table pointer, only valid when writing coff file */  
  unsigned long symbol_ptr;

  /* next object in the linked list */
  struct gp_object_type *next;
} gp_object_type;

/* annotation for symbol tables used by the linker and archive tool */
typedef struct gp_coffsymbol_type {
  gp_symbol_type *symbol;   /* the coff symbol */
  gp_object_type *file;     /* the object file the symbol is defined in */
} gp_coffsymbol_type;

gp_object_type *gp_coffgen_init(void);
gp_section_type *gp_coffgen_findsection(gp_object_type *object, 
                                        gp_section_type *start,
                                        char *name);
gp_section_type *gp_coffgen_addsection(gp_object_type *object, char *name);
gp_reloc_type *gp_coffgen_addreloc(gp_section_type *section);
gp_linenum_type *gp_coffgen_addlinenum(gp_section_type *section);
gp_symbol_type *
gp_coffgen_findsymbol(gp_object_type *object, char *name);
gp_aux_type *gp_coffgen_addaux(gp_object_type *object, gp_symbol_type *symbol);
gp_symbol_type *gp_coffgen_addsymbol(gp_object_type *object);

gp_section_type *gp_coffgen_blocksec(unsigned int number);
gp_reloc_type *gp_coffgen_blockrel(unsigned int number);
gp_linenum_type *gp_coffgen_blockline(unsigned int number);
gp_symbol_type *gp_coffgen_blocksym(unsigned int number);
gp_aux_type *gp_coffgen_blockaux(unsigned int number);

int gp_coffgen_free_section(gp_section_type *section);
int gp_coffgen_free_symbol(gp_symbol_type *symbol);
int gp_coffgen_free(gp_object_type *object);
int gp_coffgen_free(gp_object_type *object);

int gp_determine_aux(gp_symbol_type *symbol);

#endif

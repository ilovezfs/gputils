/* GNU PIC coff linker functions
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2016 Molnar Karoly

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

#ifndef __GPCOFFLINK_H__
#define __GPCOFFLINK_H__

/*

SECT_ACCESSBANK - used for access registers in internal ram for 18CXX
SECT_CODEPAGE   - used for program code, initialized data values, and constants
SECT_DATABANK   - used for banked registers in internal ram
SECT_SHAREBANK  - used for unbanked registers in internal ram
SECT_LINEARMEM  - used for linear RAM access

*/

enum section_type {
  SECT_NONE = 0,
  SECT_ACCESSBANK,
  SECT_CODEPAGE,
  SECT_DATABANK,
  SECT_SHAREBANK,
  SECT_LINEARMEM
};

/* Section definitions from the linker script are stored in the following
   structure.  These structures are placed in a symbol table.  */

typedef struct linker_section {
  enum section_type  type;
  int                start;
  int                end;
  int                fill;
  gp_boolean         use_fill;
  gp_boolean         protected;
  int                shadow_val;
  const char        *shadow_sym;
} linker_section_t;

extern void gp_cofflink_add_symbol(symbol_table_t *Table, gp_symbol_t *Symbol, gp_object_t *File);

extern void gp_cofflink_remove_symbol(symbol_table_t *Table, const char *Name);

extern gp_boolean gp_cofflink_add_symbols(symbol_table_t *Table, symbol_table_t *Missing, gp_object_t *Object);

extern void gp_cofflink_combine_objects(gp_object_t *Object);

extern void gp_cofflink_clean_table(gp_object_t *Object, symbol_table_t *Symbols);

extern void gp_cofflink_combine_overlay(gp_object_t *Object, gp_boolean Remove_symbol);

extern void gp_cofflink_make_stack(gp_object_t *Object, unsigned int Num_bytes);

extern void gp_cofflink_merge_sections(gp_object_t *Object);

extern void gp_cofflink_make_cinit(gp_object_t *Object);

extern void gp_cofflink_make_idata(gp_object_t *Object, gp_boolean Force_cinit);

extern void gp_cofflink_add_cinit_section(gp_object_t *Object);

extern void gp_cofflink_reloc_abs(gp_object_t *Object, MemBlock_t *M, unsigned int Org_to_byte_shift,
                                  uint32_t Flags);

extern void gp_cofflink_reloc_assigned(gp_object_t *Object, MemBlock_t *M, unsigned int Org_to_byte_shift,
                                       uint32_t Flags, symbol_table_t *Sections,
                                       symbol_table_t *Logical_sections);

extern void gp_cofflink_reloc_cinit(gp_object_t *Object, MemBlock_t *M, unsigned int Org_to_byte_shift,
                                    gp_section_t *Cinit_section, const symbol_table_t *Sections);

extern void gp_cofflink_reloc_unassigned(gp_object_t *Object, MemBlock_t *M, unsigned int Org_to_byte_shift,
                                         uint32_t Flags, const symbol_table_t *Sections);

extern void gp_cofflink_update_table(gp_object_t *Object, unsigned int Org_to_byte_shift);

extern void gp_cofflink_fill_pages(gp_object_t *Object, MemBlock_t *M, const symbol_table_t *Sections);

extern void gp_cofflink_patch(gp_object_t *Object, gp_boolean Mplink_compatible);

extern MemBlock_t *gp_cofflink_make_memory(gp_object_t *Object);

extern gp_boolean gp_relocate_to_shared;

#endif

/* GNU PIC coff linker functions
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

#ifndef __GPCOFFLINK_H__
#define __GPCOFFLINK_H__

/*

accessbank - used for access registers in internal ram for 18CXX
codepage - used for program code, initialized data values, and constants
databank - used for banked registers in internal ram
sharebank - used for unbanked registers in internal ram

*/

enum section_type { 
  accessbank,
  codepage,
  databank,
  sharebank
};

/* Section definitions from the linker script are stored in the following
   structure.  These structures are placed in a symbol table.  */

struct linker_section {
  enum section_type type;
  int start;
  int end;
  int fill;
  int use_fill;
  int protected;
};

int gp_link_add_symbols(struct symbol_table *,
                        struct symbol_table *missing,
                        gp_object_type *object);

void gp_cofflink_combine_overlay(gp_object_type *object, int remove_symbol);

void gp_cofflink_reloc(gp_object_type *object,
                       struct symbol_table *sections,
                       struct symbol_table *logical_sections);

void gp_cofflink_patch(gp_object_type *object, struct symbol_table *symbols);

void gp_cofflink_clean_table(gp_object_type *object);

#endif

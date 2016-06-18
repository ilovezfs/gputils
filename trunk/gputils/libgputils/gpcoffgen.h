/* GNU PIC general coff functions
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

#ifndef __GPCOFFGEN_H__
#define __GPCOFFGEN_H__

/* annotation for symbol tables used by the linker and archive tool */
typedef struct gp_coffsymbol_type {
  gp_symbol_type *symbol;   /* the coff symbol */
  gp_object_type *file;     /* the object file the symbol is defined in */
} gp_coffsymbol_type;

extern gp_object_type *gp_coffgen_init(void);

extern gp_section_type *gp_coffgen_find_section(gp_object_type *object, gp_section_type *start,
                                                const char *name);

extern gp_section_type *gp_coffgen_new_section(const char *name, MemBlock *data);
extern gp_section_type *gp_coffgen_add_section(gp_object_type *object, const char *name, MemBlock *data);
extern void gp_coffgen_del_section_symbols(gp_object_type *object, gp_section_type *section);
extern gp_section_type *gp_coffgen_del_section(gp_object_type *object, gp_section_type *section);
extern gp_reloc_type *gp_coffgen_add_reloc(gp_section_type *section);
extern gp_reloc_type *gp_coffgen_del_reloc(gp_section_type *section, gp_reloc_type *relocation);
extern const char *gp_coffgen_reloc_type_to_str(uint16_t type);
extern gp_linenum_type *gp_coffgen_add_linenum(gp_section_type *section);
extern gp_linenum_type *gp_coffgen_find_linenum_by_address(gp_section_type *section, unsigned int address);
extern gp_linenum_type *gp_coffgen_del_linenum(gp_section_type *section, gp_linenum_type *linenum);
extern gp_linenum_type *gp_coffgen_del_linenum_by_address(gp_section_type *section, unsigned int address);
extern gp_boolean gp_coffgen_del_linenum_by_address_area(gp_section_type *section, unsigned int address_start,
                                                         unsigned int address_end);

extern gp_symbol_type *gp_coffgen_find_symbol(gp_object_type *object, const char *name);
extern gp_symbol_type *gp_coffgen_find_section_symbol(gp_object_type *object, const char *name);

extern gp_symbol_type *gp_coffgen_find_symbol_section_value(gp_object_type *object,
                                                            const char *section_section, long value);

extern gp_aux_type *gp_coffgen_add_aux(gp_object_type *object, gp_symbol_type *symbol);
extern gp_symbol_type *gp_coffgen_add_symbol(gp_object_type *object);
extern gp_symbol_type *gp_coffgen_del_symbol(gp_object_type *object, gp_symbol_type *symbol);
extern gp_boolean gp_coffgen_symbol_has_reloc(const gp_symbol_type *Symbol);
extern gp_boolean gp_coffgen_is_global_symbol(const gp_symbol_type *Symbol);
extern gp_boolean gp_coffgen_is_external_symbol(const gp_symbol_type *Symbol);
extern gp_boolean gp_coffgen_is_debug_symbol(const gp_symbol_type *Symbol);
extern gp_boolean gp_coffgen_is_absolute_symbol(const gp_symbol_type *Symbol);
extern gp_section_type *gp_coffgen_make_block_section(unsigned int number);
extern gp_reloc_type *gp_coffgen_make_block_reloc(unsigned int number);
extern gp_linenum_type *gp_coffgen_make_block_linenum(unsigned int number);
extern gp_symbol_type *gp_coffgen_make_block_symbol(unsigned int number);
extern gp_aux_type *gp_coffgen_make_block_aux(unsigned int number);

extern void gp_coffgen_free_section(gp_section_type *section);
extern unsigned int gp_coffgen_free_symbol(gp_symbol_type *symbol);
extern gp_boolean gp_coffgen_free_object(gp_object_type *object);

extern unsigned int gp_coffgen_determine_aux_symbol(const gp_symbol_type *symbol);

#endif

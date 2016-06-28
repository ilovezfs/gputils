/* GNU PIC general coff functions
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2016 Molnar Karoly <molnarkaroly@users.sf.net>

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
typedef struct gp_coffsymbol {
  gp_symbol_t *symbol;   /* the coff symbol */
  gp_object_t *file;     /* the object file the symbol is defined in */
} gp_coffsymbol_t;

extern gp_object_t *gp_coffgen_new_object(const char *File_name);
extern void gp_coffgen_transfer_object_data(gp_object_t *Receiver, gp_object_t *Sender);
extern gp_boolean gp_coffgen_update_all_object_id(gp_object_t *Object);

extern gp_section_t *gp_coffgen_find_section(gp_object_t *Object, gp_section_t *Start,
                                             const char *Name);

extern gp_section_t *gp_coffgen_new_section(const char *Name, MemBlock_t *Data);
extern gp_section_t *gp_coffgen_make_block_section(gp_object_t *Object);
extern gp_section_t *gp_coffgen_add_exists_section(gp_object_t *Object, gp_section_t *Section);

extern gp_section_t *gp_coffgen_insert_section(gp_object_t *Object, gp_section_t *Ancestor,
                                               gp_section_t *Following);

extern gp_section_t *gp_coffgen_add_section(gp_object_t *Object, const char *Name, MemBlock_t *Data);
extern void gp_coffgen_transfer_section_data(gp_section_t *Receiver, gp_section_t *Sender);
extern gp_boolean gp_coffgen_update_all_section_id(gp_section_t *Section);
extern gp_boolean gp_coffgen_update_section_id(gp_object_t *Object);
extern void gp_coffgen_del_section_symbols(gp_object_t *Object, gp_section_t *Section);
extern void gp_coffgen_move_reserve_section_symbols(gp_object_t *Object, gp_section_t *Section);
extern gp_section_t *gp_coffgen_move_reserve_section(gp_object_t *Object, gp_section_t *Section);
extern gp_boolean gp_coffgen_del_section(gp_object_t *Object, gp_section_t *Section);

extern gp_section_t **gp_coffgen_make_section_array(gp_object_t *Object, unsigned int *Num_sections,
                                                    unsigned int Page_addr, uint32_t Flags);

extern gp_symbol_t *gp_coffgen_find_symbol(gp_object_t *Object, const char *Name);
extern gp_symbol_t *gp_coffgen_find_section_symbol(gp_object_t *Object, const char *Name);

extern gp_symbol_t *gp_coffgen_find_symbol_section_value(gp_object_t *Object,
                                                         const char *Section_section, long Value);

extern gp_symbol_t *gp_coffgen_make_block_symbol(gp_object_t *Object);
extern gp_symbol_t *gp_coffgen_add_symbol(gp_object_t *Object);
extern gp_aux_t *gp_coffgen_add_aux(gp_object_t *Object, gp_symbol_t *Symbol);
extern gp_aux_t *gp_coffgen_make_block_aux(gp_symbol_t *Symbol);
extern gp_symbol_t *gp_coffgen_move_reserve_symbol(gp_object_t *Object, gp_symbol_t *Symbol);
extern gp_boolean gp_coffgen_del_symbol(gp_object_t *Object, gp_symbol_t *Symbol);

extern gp_reloc_t *gp_coffgen_make_block_reloc(gp_section_t *Section);
extern gp_reloc_t *gp_coffgen_add_reloc(gp_section_t *Section);
extern void gp_coffgen_check_relocations(const gp_object_t *Object, gp_boolean Enable_cinit_warns);
extern gp_boolean gp_coffgen_del_reloc(gp_section_t *Section, gp_reloc_t *Relocation);
extern const char *gp_coffgen_reloc_type_to_str(uint16_t Type);

extern gp_linenum_t *gp_coffgen_make_block_linenum(gp_section_t *Section);
extern gp_linenum_t *gp_coffgen_add_linenum(gp_section_t *Section);
extern gp_linenum_t *gp_coffgen_find_linenum_by_address(gp_section_t *Section, unsigned int Address);
extern gp_boolean gp_coffgen_del_linenum(gp_section_t *Section, gp_linenum_t *Linenum);
extern gp_boolean gp_coffgen_del_linenum_by_address(gp_section_t *Section, unsigned int Address);
extern unsigned int gp_coffgen_del_linenum_by_address_area(gp_section_t *Section, unsigned int Address_start,
                                                           unsigned int Address_end);

extern void gp_coffgen_make_linenum_arrays(gp_object_t *Object);

extern gp_linenum_t *gp_coffgen_find_linenum(const gp_section_t *Section, const gp_symbol_t *Symbol,
                                             unsigned int Line_number);


/* gp_coffgen_symbol_has_reloc() "Type" parameter */
#define COFF_SYM_RELOC_ALL          0
#define COFF_SYM_RELOC_OWN          1
#define COFF_SYM_RELOC_OTHER        2

extern gp_boolean gp_coffgen_symbol_has_reloc(const gp_symbol_t *Symbol, int Type);

extern gp_boolean gp_coffgen_is_global_symbol(const gp_symbol_t *Symbol);
extern gp_boolean gp_coffgen_is_external_symbol(const gp_symbol_t *Symbol);
extern gp_boolean gp_coffgen_is_debug_symbol(const gp_symbol_t *Symbol);
extern gp_boolean gp_coffgen_is_absolute_symbol(const gp_symbol_t *Symbol);

extern void gp_coffgen_free_section(gp_section_t *Section);
extern unsigned int gp_coffgen_free_symbol(gp_symbol_t *Symbol);
extern gp_boolean gp_coffgen_free_object(gp_object_t *Object);

extern unsigned int gp_coffgen_determine_aux_symbol(const gp_symbol_t *Symbol);

#endif

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

gp_object_type *gp_coffgen_init(void);
gp_section_type *gp_coffgen_findsection(gp_object_type *object, gp_section_type *start, const char *name);
gp_section_type *gp_coffgen_newsection(const char *name, MemBlock *data);
gp_section_type *gp_coffgen_addsection(gp_object_type *object, const char *name, MemBlock *data);
void gp_coffgen_delsectionsyms(gp_object_type *object, gp_section_type *section);
gp_section_type *gp_coffgen_delsection(gp_object_type *object, gp_section_type *section);
gp_reloc_type *gp_coffgen_addreloc(gp_section_type *section);
gp_linenum_type *gp_coffgen_addlinenum(gp_section_type *section);
gp_symbol_type * gp_coffgen_findsymbol(gp_object_type *object, const char *name);
gp_symbol_type * gp_coffgen_findsectionsymbol(gp_object_type *object, const char *name);
gp_symbol_type *gp_coffgen_findsymbol_sect_val(gp_object_type *object, const char *section_section, long value);
gp_aux_type *gp_coffgen_addaux(gp_object_type *object, gp_symbol_type *symbol);
gp_symbol_type *gp_coffgen_addsymbol(gp_object_type *object);
gp_symbol_type *gp_coffgen_delsymbol(gp_object_type *object, gp_symbol_type *symbol);
gp_boolean gp_coffgen_has_reloc(gp_object_type *object, gp_symbol_type *symbol);
gp_boolean gp_coffgen_is_global(gp_symbol_type *symbol);
gp_boolean gp_coffgen_is_external(gp_symbol_type *symbol);
gp_boolean gp_coffgen_is_debug(gp_symbol_type *symbol);
gp_boolean gp_coffgen_is_absolute(gp_symbol_type *symbol);
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

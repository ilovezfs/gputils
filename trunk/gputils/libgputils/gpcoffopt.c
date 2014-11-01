/* GNU PIC coff optimizing functions
   Copyright (C) 2005
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

#include "stdhdr.h"
#include "libgputils.h"

/* Remove any weak symbols in the object. */

void
gp_coffopt_remove_weak(gp_object_type *object)
{
  gp_symbol_type *symbol;

  gp_debug("Removing weak symbols from \"%s\".", object->filename);

  /* Search the symbol table for extern symbols. */
  symbol = object->symbols;
  while (symbol != NULL) {
    if ((symbol->class == C_EXT) && 
        (symbol->section_number == N_UNDEF) &&
        (!gp_coffgen_has_reloc(object, symbol))) {
      gp_debug("  removed weak symbol \"%s\"", symbol->name);
      gp_coffgen_delsymbol(object, symbol);
    }

    symbol = symbol->next;
  }
}

/* Remove any relocatable section that doesn't have a symbol pointed to by
   a relocation. */

void
gp_coffopt_remove_dead_sections(gp_object_type *object, int pass)
{
  gp_section_type *section;
  gp_reloc_type *relocation;
  gp_section_type *rel_sect;
  gp_boolean section_removed = false;

  gp_debug("Removing dead sections pass %i.", pass);

  section = object->sections;
  while (section != NULL) {
    /* Mark all sections as unused. */
    section->is_used = false;
    section = section->next;
  }

  section = object->sections;
  while (section != NULL) {
    /* Mark all sections that relocations point to as used. */
/*    gp_debug("  section_name: %s", section->name);*/
    relocation = section->relocations;
    while (relocation != NULL) {
      if ((rel_sect = relocation->symbol->section) != NULL) {
        if (rel_sect != section) {
/*          gp_debug("    reloc_section_name: %s", rel_sect->name);*/
          rel_sect->is_used = true;
        }
      }
      else {
        gp_warning("Relocation symbol %s has no section.", relocation->symbol->name);
      }
      relocation = relocation->next;
    }
    section = section->next;
  }

  section = object->sections;
  while (section != NULL) {
    /* FIXME: Maybe don't remove if it is in protected memory. */
    if ((!section->is_used) && !(section->flags & STYP_ABS)) {
      gp_debug("Removing section \"%s\".", section->name);
      gp_coffgen_delsectionsyms(object, section);
      gp_coffgen_delsection(object, section);
      section_removed = true;
    }

    section = section->next;
  }

  if (section_removed) {
    /* take another pass */
    gp_coffopt_remove_dead_sections(object, pass++);
  }
}

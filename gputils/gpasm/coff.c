/* Generate coff file
   Copyright (C) 2002 Craig Franklin

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
#include "gpasm.h"
#include "gperror.h"
#include "coff.h"

extern int _16bit_core;

void 
coff_init(void)
{
  if (state.objfile != named) {
    strcpy(state.objfilename, state.basefilename);
    strcat(state.objfilename, ".o");  
  }

  if (state.objfile == suppress) {
    state.obj.enabled = 0;
    unlink(state.objfilename);
  } else {
    if (state.processor_chosen == 0) {
      state.obj.enabled = 0;
    } else {
      state.obj.object = gp_coffgen_init();
      state.obj.object->filename = strdup(state.objfilename);
      state.obj.object->processor = state.processor;
      state.obj.object->class = state.device.class;
      state.obj.enabled = 1;
    }
  }
  
  return;
}

static void
_update_section_size(void)
{
  
  if (state.obj.section == NULL)
    return;
  
  if (state.obj.section->flags & STYP_TEXT) {
    /* the section is executable, so each word is two bytes */
    state.obj.section->size = 
      (state.org - state.obj.section->address) * 2;
  } else {
    /* the section is data, so each word is one byte */
    state.obj.section->size = 
      (state.org - state.obj.section->address);
  }

  /* write data to the auxiliary section symbol */
  state.obj.section->symbol->aux_list->_aux_symbol._aux_scn.length = 
    state.obj.section->size;
    
  state.obj.section->symbol->aux_list->_aux_symbol._aux_scn.nreloc = 
    state.obj.section->num_reloc;

  state.obj.section->symbol->aux_list->_aux_symbol._aux_scn.nlineno = 
    state.obj.section->num_lineno;

  return;
}

static void
_update_reloc_ptr(void)
{
  gp_section_type *section = state.obj.object->sections;
  gp_symbol_type *symbol;
  gp_reloc_type *reloc;

  while (section != NULL) {
    reloc = section->relocations;
    while (reloc != NULL) {
      symbol = state.obj.object->symbols;
      while (symbol != NULL) {
        if (reloc->symbol_number == symbol->number) {
          reloc->symbol = symbol;
          break;
        }
        symbol = symbol->next;
      }
      reloc = reloc->next;
    }
    section = section->next;
  }

  return;
}

void 
coff_close_file(void)
{

  if(!state.obj.enabled)
    return;

  /* store data from the last section */
  _update_section_size();

  /* update relocation symbol pointers */
  _update_reloc_ptr();

  /* combine overlayed sections */
  gp_cofflink_combine_overlay(state.obj.object, 1);

  if (gp_write_coff(state.obj.object, (state.num.errors + gp_num_errors)) == 1)
    gperror(GPE_UNKNOWN, "system error while writing object file");
    
  gp_coffgen_free(state.obj.object);
}

void 
coff_new_section(char *name, int addr, int flags)
{
  gp_section_type *found = NULL;
  gp_symbol_type *new = NULL;
  gp_aux_type *new_aux;

  state.obj.symbol_num += 2;

  /* store the flags so they are available for pass 1 */
  state.obj.flags = flags;

  if (!state.obj.enabled) {
    state.org = addr;
    return;
  }

  assert(state.obj.object != NULL);

  /* store data from the last section */
  _update_section_size();

  found = gp_coffgen_findsection(state.obj.object, 
                                 state.obj.object->sections,
                                 name);    

  if (found != NULL) {
    if ((flags & STYP_OVERLAY) && 
        (found->flags & STYP_OVERLAY)) {
      /* Overlayed sections can be duplicated.  This allows multiple code 
         sections in the same source file to share the same data memory. */
      if ((flags != found->flags) ||
          (addr != found->address)) {
        gperror(GPE_CONTIG_SECTION, NULL);
        return;
      }    
    } else {
      gperror(GPE_CONTIG_SECTION, NULL);
      return;
    }
  }

  state.obj.section = gp_coffgen_addsection(state.obj.object, name);    
  state.obj.section->address = addr; 
  state.obj.section->flags = flags;
  
  /* add a section symbol */
  new = gp_coffgen_addsymbol(state.obj.object);
  new->name           = strdup(name);
  new->value          = addr;
  new->section_number = 1;
  new->section        = state.obj.section;
  new->type           = T_NULL;
  new->class          = C_SECTION;

  state.obj.section->symbol = new;

  new_aux = gp_coffgen_addaux(state.obj.object, new);
  new_aux->type = AUX_SCN;
  
  state.i_memory = state.obj.section->data;
  state.org = addr;

  return;
}

/* All coff data is generated on the second pass.  To support forward
   references to symbols in the relocation table, the symbol index
   is stored in relocations instead of a pointer to the symbol.  Before
   the coff is written the symbol pointer is updated. */ 

void 
coff_reloc(int symbol, short offset, unsigned short type)
{
  gp_reloc_type *new = NULL;
  int origin;

  if ((!state.obj.enabled) || (state.obj.section == NULL))
    return;

  origin = state.org - state.obj.section->address;
  
  new = gp_coffgen_addreloc(state.obj.section);
  new->address       = origin * 2;    /* byte address not word */
  new->symbol_number = symbol;
  new->offset        = offset;    
  new->type          = type;    

  return;
}

void 
coff_linenum(int emitted)
{
  gp_linenum_type *new = NULL;
  int i;
  int origin;

  if ((!state.obj.enabled) || (state.obj.section == NULL))
    return;

  if (state.obj.section->flags & STYP_ABS) {
    /* If the section is absolute, use the abolute address. */
    origin = state.lst.line.was_org << _16bit_core;
  } else {
    /* use the relative address */
    origin = (state.lst.line.was_org - state.obj.section->address) << _16bit_core;
  }
  
  for (i = 0; i < emitted; i++) {
     
    new = gp_coffgen_addlinenum(state.obj.section);
    new->symbol      = state.src->file_symbol;
    new->line_number = state.src->line_number;
    new->address     = origin + (i << _16bit_core);
  }

  return;
}

/* Add a symbol to the coff symbol table.  The calling function must
   increment the global symbol number. */

void
coff_add_sym(char *name, int value, int section_number, int type, int class)
{
  gp_symbol_type *new;
  char message[BUFSIZ];

  if(!state.obj.enabled)
    return;

  new = gp_coffgen_findsymbol(state.obj.object, name);
  if (new != NULL) {
    sprintf(message,
            "Duplicate label or redefining symbol that cannot be redefined. (%s)",
            name);    
    gperror(GPE_DUPLAB, message);
  } else {
    new = gp_coffgen_addsymbol(state.obj.object);
    new->name           = strdup(name);
    new->value          = value;
    new->section_number = section_number;
    new->section        = state.obj.section;
    new->type           = type;
    new->class          = class;
  }
 
  return;
}

/* add a file symbol to the coff symbol table */

gp_symbol_type *
coff_add_filesym(char *name, int isinclude)
{
  gp_symbol_type *new = NULL;
  gp_aux_type *new_aux;

  state.obj.symbol_num += 2;

  if(!state.obj.enabled)
    return NULL;

  /* add .file symbol */
  new = gp_coffgen_addsymbol(state.obj.object);
  new->name           = strdup(".file");
  new->value          = 0;
  new->section_number = N_DEBUG;
  new->section        = NULL;
  new->type           = T_NULL;
  new->class          = C_FILE;

  new_aux = gp_coffgen_addaux(state.obj.object, new);
  new_aux->type = AUX_FILE;
  new_aux->_aux_symbol._aux_file.filename = strdup(name);
  if (isinclude == 1)
    new_aux->_aux_symbol._aux_file.line_number = state.src->line_number - 1;
  else
    new_aux->_aux_symbol._aux_file.line_number = 0;
 
  return new;
}

/* add an eof symbol to the coff symbol table */

void
coff_add_eofsym(void)
{
  gp_symbol_type *new;

  state.obj.symbol_num++;

  if(!state.obj.enabled)
    return;

  /* add .eof symbol */
  new = gp_coffgen_addsymbol(state.obj.object);
  new->name           = strdup(".eof");
  new->value          = 0;
  new->section_number = N_DEBUG;
  new->section        = NULL;
  new->type           = T_NULL;
  new->class          = C_EOF;  

  return;
}

/* add an eof symbol to the coff symbol table */

void
coff_add_listsym(void)
{
  gp_symbol_type *new;

  state.obj.symbol_num++;

  if(!state.obj.enabled)
    return;

  /* add .eof symbol */
  new = gp_coffgen_addsymbol(state.obj.object);
  new->name           = strdup(".list");
  new->value          = state.src->line_number;
  new->section_number = N_DEBUG;
  new->section        = NULL;
  new->type           = T_NULL;
  new->class          = C_LIST;

  return;
}

void
coff_add_nolistsym(void)
{
  gp_symbol_type *new;

  state.obj.symbol_num++;

  if(!state.obj.enabled)
    return;

  /* add .nolist symbol */
  new = gp_coffgen_addsymbol(state.obj.object);
  new->name           = strdup(".nolist");
  new->value          = state.src->line_number;
  new->section_number = N_DEBUG;
  new->section        = NULL;
  new->type           = T_NULL;
  new->class          = C_LIST;  

  return;
}

/* If the symbol is local, generate a modified name for the coff symbol 
   table. */

char *
coff_local_name(char *name)
{
  struct symbol *local;
  gp_symbol_type *symbol;
  char buffer[BUFSIZ];
  int count = 1;

  if(!state.obj.enabled)
    return NULL;

  local = get_symbol(state.stGlobal, name);
  if (local == NULL) {
    /* It isn't in the stGlobal so it must be in stTop. It's local. */
    while(1) {
      sprintf(buffer, "_%d%s", count, name);
      symbol = gp_coffgen_findsymbol(state.obj.object, buffer);
      if (symbol == NULL)
        break;
      count++;
    } 
  } else {
    strcpy(buffer, name);
  }

  return strdup(buffer);

}

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
    state.obj.object = NULL;
    state.obj.enabled = 0;
    unlink(state.objfilename);
  } else {
    if (state.processor_chosen == 0) {
      state.obj.enabled = 0;
    } else {
      state.obj.object = gp_coffgen_init(state.objfilename, state.processor);
      state.obj.enabled = 1;
    }
  }
     
  state.obj.section = NULL;
  state.obj.section_num = 0;
  
  return;
}

static void
_update_section_size(void)
{
  
  if (state.obj.section == NULL)
    return;
  
  if (state.obj.section->header.s_flags & STYP_TEXT) {
    /* the section is executable, so each word is two bytes */
    state.obj.section->header.s_size = 
      (state.org - state.obj.section->header.s_paddr) * 2;
  } else {
    /* the section is data, so each word is one byte */
    state.obj.section->header.s_size = 
      (state.org - state.obj.section->header.s_paddr);
  }

  return;
}

void 
coff_close_file(void)
{
  unsigned int num_sections;

  if(!state.obj.enabled)
    return;

  if (state.num.errors > 0) {
    unlink(state.objfilename);
    return;
  }

  /* store data from the last section */
  _update_section_size();

  /* combine overlayed sections */
  num_sections = state.obj.object->file_header.f_nscns;
  gp_cofflink_combine_overlay(state.obj.object);

  /* remove duplicate section entries in the symbol table */
  if (num_sections != state.obj.object->file_header.f_nscns)
    gp_cofflink_remove_dupsecsyms(state.obj.object);

  /* update file pointers in the coff */
  gp_coffgen_updateptr(state.obj.object);

  if (gp_coffgen_writecoff(state.obj.object) == 1)
    gperror(GPE_UNKNOWN, "system error while writing object file");
    
  gp_coffgen_free(state.obj.object);
}

void 
coff_new_section(char *name, int addr, int flags)
{
  gp_section_type *found = NULL;
  gp_symbol_type *new = NULL;
  struct aux_scn *aux = NULL;

  state.obj.symbol_num += 2;

  /* store the flags so they are available for pass 1 */
  state.obj.flags = flags;

  /* increment section number */
  state.obj.section_num++;

  if(!state.obj.enabled) {
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
        (found->header.s_flags & STYP_OVERLAY)) {
      /* Overlayed sections can be duplicated.  This allows multiple code 
         sections in the same source file to share the same data memory. */
      if ((flags != found->header.s_flags) ||
          (addr != found->header.s_paddr)) {
        gperror(GPE_CONTIG_SECTION, NULL);
        return;
      }    
    } else {
      gperror(GPE_CONTIG_SECTION, NULL);
      return;
    }
  }

  state.obj.section = gp_coffgen_addsection(state.obj.object, name);    
  state.obj.section->header.s_paddr = addr; 
  state.obj.section->header.s_vaddr = addr; 
  state.obj.section->header.s_flags = flags;
  
  /* add a section symbol */
  new = gp_coffgen_addsymbol(state.obj.object, name, 1);
  new->symbol.value    = addr;
  new->symbol.sec_num  = state.obj.section_num;
  new->symbol.type     = T_NULL;
  new->symbol.st_class = C_SECTION;

  /* add the aux section symbol */
  /* FIXME: why is this aux entry needed? */
  new = new->next;
  aux = (struct aux_scn *)&new->symbol;
  aux->x_scnlen = 0;
  aux->x_nreloc = 0;
  aux->x_nlinno = 0;

  state.i_memory = state.obj.section->data;
  state.org = addr;

  return;
}

void 
coff_reloc(int symbol, short offset, unsigned short type)
{
  gp_reloc_type *new = NULL;
  int origin;

  if ((!state.obj.enabled) || (state.obj.section == NULL))
    return;

  origin = state.org - state.obj.section->header.s_paddr;
  
  new = gp_coffgen_addreloc(state.obj.section);
  new->relocation.r_vaddr  = origin * 2;    /* byte address not word */
  new->relocation.r_symndx = symbol;
  new->relocation.r_offset = offset;    
  new->relocation.r_type   = type;    

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

  if (state.obj.section->header.s_flags & STYP_ABS) {
    /* If the section is absolute, use the abolute address. */
    origin = state.lst.line.was_org << _16bit_core;
  } else {
    /* use the relative address */
    origin = (state.lst.line.was_org - state.obj.section->header.s_paddr) << _16bit_core;
  }
  
  for (i = 0; i < emitted; i++) {
     
    new = gp_coffgen_addlinenum(state.obj.section);
    new->linenumber.l_srcndx = state.src->coff_number;
    new->linenumber.l_lnno   = state.src->line_number;
    new->linenumber.l_paddr  = origin + (i << _16bit_core);
  }

  return;
}

/* Add a symbol to the coff symbol table.  The calling function must
   increment the global symbol number. */

void
coff_add_sym(char *name, int value, int section, int type, int class)
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
    new = gp_coffgen_addsymbol(state.obj.object, name, 0);
    new->symbol.value    = value;
    new->symbol.sec_num  = section;
    new->symbol.type     = type;
    new->symbol.st_class = class;
  }
 
  return;
}

/* add a file symbol to the coff symbol table */

void
coff_add_filesym(char *name, int isinclude)
{
  gp_symbol_type *new;
  struct aux_file *aux;

  state.obj.symbol_num += 2;

  if(!state.obj.enabled)
    return;

  /* add .file symbol */
  new = gp_coffgen_addsymbol(state.obj.object, ".file", 1);
  new->symbol.sec_num  = N_DEBUG;
  new->symbol.type     = T_NULL;
  new->symbol.st_class = C_FILE;

  /* add auxilary symbol */
  new = new->next;
  aux = (struct aux_file *)&new->symbol;
  aux->x_offset = gp_coffgen_addstring(state.obj.object, name);
  
  if (isinclude == 1)
    aux->x_incline = state.src->line_number;
  else
    aux->x_incline = 0;
 
  return;
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
  new = gp_coffgen_addsymbol(state.obj.object, ".eof", 0);
  new->symbol.sec_num  = N_DEBUG;
  new->symbol.type     = T_NULL;
  new->symbol.st_class = C_EOF;

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
  new = gp_coffgen_addsymbol(state.obj.object, ".list", 0);
  new->symbol.value    = state.src->line_number;
  new->symbol.sec_num  = N_DEBUG;
  new->symbol.type     = T_NULL;
  new->symbol.st_class = C_LIST;

  return;
}

void
coff_add_nolistsym(void)
{
  gp_symbol_type *new;

  state.obj.symbol_num++;

  if(!state.obj.enabled)
    return;

  /* add .eof symbol */
  new = gp_coffgen_addsymbol(state.obj.object, ".nolist", 0);
  new->symbol.value    = state.src->line_number;
  new->symbol.sec_num  = N_DEBUG;
  new->symbol.type     = T_NULL;
  new->symbol.st_class = C_LIST;

  return;
}

/* locate a symbol in the COFF table and return its section number */

int
coff_symbol_section(char *name)
{
  gp_symbol_type *symbol;
  int number;

  if(!state.obj.enabled)
    return 0;

  symbol = gp_coffgen_findsymbol(state.obj.object, name);
  if (symbol == NULL)
    return 0;
  
  number = symbol->symbol.sec_num;

  return number;
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

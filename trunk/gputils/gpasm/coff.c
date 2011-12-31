/* Generate coff file
   Copyright (C) 2002, 2003, 2004, 2005
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
#include "gpasm.h"
#include "gperror.h"
#include "coff.h"

void
coff_init(void)
{
  if (state.objfile != named) {
    snprintf(state.objfilename, sizeof(state.objfilename),
       "%s.o", state.basefilename);
  }

  if (state.objfile == suppress) {
    state.obj.enabled = false;
    unlink(state.objfilename);
  } else {
    if (state.processor_chosen == 0) {
      state.obj.enabled = false;
    } else {
      state.obj.object = gp_coffgen_init();
      state.obj.object->filename = strdup(state.objfilename);
      state.obj.object->processor = state.processor;
      state.obj.object->class = state.device.class;
      state.obj.object->isnew = state.obj.newcoff;
      state.obj.enabled = true;
    }
  }
}

static void
_update_section_symbol(gp_section_type *section)
{

  /* write data to the auxiliary section symbol */
  section->symbol->aux_list->_aux_symbol._aux_scn.length =
    section->size;

  section->symbol->aux_list->_aux_symbol._aux_scn.nreloc =
    section->num_reloc;

  section->symbol->aux_list->_aux_symbol._aux_scn.nlineno =
    section->num_lineno;

}

static void
_update_section_size(void)
{

  if (state.obj.section == NULL)
    return;

  state.obj.section->size =
    (state.org - state.obj.section->address);

  _update_section_symbol(state.obj.section);
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
}

/* FIXME: Now that gpasm stores bytes, this should go away. */
/* Copy data from c_memory into the coff section.  This kludge is for the
   18xx devices.  Their memory is byte addressable, but words are stored
   in gpasm memory.  The user isn't guaranteed to put the config directives
   in accending order.  They might also skip addresses.  */

static void
_copy_config(void)
{
  gp_section_type *config_section = NULL;
  int i;
  int start;
  int stop;
  unsigned char byte;
  gp_boolean found_break;

  if(!state.obj.enabled)
    return;

  if (state.found_config) {
    config_section = gp_coffgen_findsection(state.obj.object,
                                            state.obj.object->sections,
                                            ".config");

    assert(config_section != NULL);

    start = gp_processor_org_to_byte(state.device.class, state.processor->config_addrs[0]);
    stop = gp_processor_org_to_byte(state.device.class, state.processor->config_addrs[1] + 1);
    /* find the last used byte */
    {
      int first_used = stop, last_used = start;

      for (i = start; i < stop; ++i)
        if (b_memory_get(state.c_memory, i, &byte)) {
          if (first_used == stop)
            first_used = i;
          last_used = i;
        }
      /* stop and start should be at even address */
      start = first_used & ~1;
      stop = (last_used + 2) & ~1;
    }
    config_section->shadow_address = config_section->address = start;
    config_section->size = stop - start;
    for (i = start; i < stop; i++) {
      if (b_memory_get(state.c_memory, i, &byte)) {
        b_memory_put(config_section->data, i, byte);
      } else {
        /* fill undefined configuration registers with core_size mask */
        b_memory_put(config_section->data, i, (i & 1) ? state.device.class->core_size >> 8 : state.device.class->core_size);
      }
    }
    _update_section_symbol(config_section);
  }

  if (state.found_devid) {
    config_section = gp_coffgen_findsection(state.obj.object,
                                            state.obj.object->sections,
                                            ".devid");

    assert(config_section != NULL);
    assert(IS_16BIT_CORE);

    if (b_memory_get(state.c_memory, DEVID1, &byte))
      b_memory_put(config_section->data, DEVID1, byte);
    else
      assert(0);
    if (b_memory_get(state.c_memory, DEVID2, &byte))
      b_memory_put(config_section->data, DEVID2, byte);
    else
      assert(0);

    config_section->size = 2;

    _update_section_symbol(config_section);
  }

  if (state.found_idlocs) {
    config_section = gp_coffgen_findsection(state.obj.object,
                                            state.obj.object->sections,
                                            ".idlocs");

    assert(config_section != NULL);

    if (IS_16BIT_CORE) {
      config_section->size = 0;
      start = config_section->address;
      stop = IDLOC7;
      found_break = false;
      for (i = start; i <= stop; i++) {
        printf("address = %x\n", i);
        if (b_memory_get(state.c_memory, i, &byte)) {
          if (found_break) {
            gperror(GPE_CONTIG_IDLOC, NULL);
          }
          b_memory_put(config_section->data, i, byte);
          config_section->size += 1;
        } else {
          found_break = true;
        }
      }

    } else {
      start = gp_processor_id_location(state.processor);
      stop = start + 8;
      for (i = start; i < stop; i++) {
        b_memory_assert_get(state.c_memory, i, &byte);
  b_memory_put(config_section->data, i, byte);
      }
      config_section->size = 8;
    }

    _update_section_symbol(config_section);
  }
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

  _copy_config();

  if (gp_write_coff(state.obj.object, (state.num.errors + gp_num_errors)) == 1)
    gperror(GPE_UNKNOWN, "system error while writing object file");

  gp_coffgen_free(state.obj.object);
}

void
coff_new_section(const char *name, int addr, int flags)
{
  gp_section_type *found = NULL;
  gp_symbol_type *new = NULL;
  gp_aux_type *new_aux;

  state.obj.symbol_num += 2;

  /* increment the section number */
  state.obj.section_num++;

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
  state.obj.section->shadow_address = state.obj.section->address = addr;
  state.obj.section->flags = flags;

  /* add a section symbol */
  new = gp_coffgen_addsymbol(state.obj.object);
  new->name           = strdup(name);
  new->value          = IS_RAM_ORG ? addr : gp_processor_byte_to_org(state.device.class, addr);
  new->section_number = state.obj.section_num;  /* Modified later. */
  new->section        = state.obj.section;
  new->type           = T_NULL;
  new->class          = C_SECTION;

  state.obj.section->symbol = new;

  new_aux = gp_coffgen_addaux(state.obj.object, new);
  new_aux->type = AUX_SCN;

  state.i_memory = state.obj.section->data;
  state.org = addr;
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
  new->address       = origin;
  new->symbol_number = symbol;
  new->offset        = offset;
  new->type          = type;
}

void
coff_linenum(int emitted)
{
  gp_linenum_type *new = NULL;
  int i;
  int origin;
  static gp_boolean show_bad_debug = true;

  if ((!state.obj.enabled) || (state.obj.section == NULL))
    return;

  /* If the section is absolute, use the abolute address. */
  origin = state.lst.line.was_org;
  if (!(state.obj.section->flags & STYP_ABS)) {
    /* else use the relative address */
    origin -= state.obj.section->address;
  }

  if (state.debug_info && (state.obj.debug_file == NULL)) {
    if (show_bad_debug) {
      gperror(GPE_UNKNOWN, ".file directive required to generate debug info");
      show_bad_debug = false;
    }
    return;
  }

  for (i = 0; i < emitted; i += 2) {

    new = gp_coffgen_addlinenum(state.obj.section);
    if (state.debug_info) {
      new->symbol = state.obj.debug_file;
      new->line_number = state.obj.debug_line;
    } else {
      new->symbol = state.src->file_symbol;
      new->line_number = state.src->line_number;
    }

    new->address = origin + i;
  }
}

/* Add a symbol to the coff symbol table.  The calling function must
   increment the global symbol number. */

gp_symbol_type *
coff_add_sym(const char *name, int value, enum gpasmValTypes type)
{
  gp_symbol_type *new = NULL;
  char message[BUFSIZ];
  int section_number = 0;
  int class = C_EXT;

  if(!state.obj.enabled)
    return NULL;

  switch (type) {
  case gvt_extern:
    section_number = N_UNDEF;
    class = C_EXT;
    break;
  case gvt_global:
    section_number = state.obj.section_num;
    class = C_EXT;
    break;
  case gvt_static:
    section_number = state.obj.section_num;
    class = C_STAT;
    break;
  case gvt_address:
    section_number = state.obj.section_num;
    class = C_LABEL;
    break;
  case gvt_debug:
    section_number = N_DEBUG;
    class = C_NULL;
    break;
  case gvt_absolute:
    section_number = N_ABS;
    class = C_NULL;
    break;
  default:
    return new;
  }

  new = gp_coffgen_findsymbol(state.obj.object, name);

  /* verify the duplicate extern has the same properties */
  if ((new != NULL) && (type == gvt_extern))  {
    if ((new->type != class) ||
        (new->section_number != section_number)) {
      snprintf(message, sizeof(message),
               "Duplicate label or redefining symbol that cannot be redefined. (%s)",
               name);
      gperror(GPE_UNKNOWN, message);
    }
  }

  if ((new != NULL) && (type != gvt_extern) && (type != gvt_debug))  {
    snprintf(message, sizeof(message),
             "Duplicate label or redefining symbol that cannot be redefined. (%s)",
             name);
    gperror(GPE_DUPLAB, message);
  } else {
    new = gp_coffgen_addsymbol(state.obj.object);
    new->name           = strdup(name);
    new->value          = value;
    new->section_number = section_number;
    new->section        = state.obj.section;
    new->type           = T_NULL;
    new->class          = class;
  }

  return new;
}

/* add a file symbol to the coff symbol table */

gp_symbol_type *
coff_add_filesym(const char *name, int isinclude)
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
  new_aux->_aux_symbol._aux_file.flags = 0;

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
}

/* add an eof symbol to the coff symbol table */

void
coff_add_listsym(void)
{
  gp_symbol_type *new;

  if (state.debug_info)
    return;

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
}

void
coff_add_nolistsym(void)
{
  gp_symbol_type *new;

  if (state.debug_info)
    return;

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
}

/* add a direct symbol to the coff symbol table */

void
coff_add_directsym(unsigned char command, const char *string)
{
  gp_symbol_type *new = NULL;
  gp_aux_type *new_aux;

  state.obj.symbol_num += 2;

  if(!state.obj.enabled)
    return;

  /* add .cod symbol */
  new = gp_coffgen_addsymbol(state.obj.object);
  new->name           = strdup(".direct");
  new->value          = gp_processor_byte_to_org(state.device.class, state.org);
  new->section_number = state.obj.section_num;
  new->section        = state.obj.section;
  new->type           = T_NULL;
  new->class          = C_NULL;

  new_aux = gp_coffgen_addaux(state.obj.object, new);
  new_aux->type = AUX_DIRECT;
  new_aux->_aux_symbol._aux_direct.command = command;
  new_aux->_aux_symbol._aux_direct.string = strdup(string);
}

/* add a cod symbol to the coff symbol table */

void
coff_add_identsym(const char *string)
{
  gp_symbol_type *new = NULL;
  gp_aux_type *new_aux;

  state.obj.symbol_num += 2;

  if(!state.obj.enabled)
    return;

  /* add .cod symbol */
  new = gp_coffgen_addsymbol(state.obj.object);
  new->name           = strdup(".ident");
  new->value          = 0;
  new->section_number = N_DEBUG;
  new->section        = NULL;
  new->type           = T_NULL;
  new->class          = C_NULL;

  new_aux = gp_coffgen_addaux(state.obj.object, new);
  new_aux->type = AUX_IDENT;
  new_aux->_aux_symbol._aux_ident.string = strdup(string);
}

/* If the symbol is local, generate a modified name for the coff symbol
   table. */

char *
coff_local_name(const char *name)
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
      snprintf(buffer, sizeof(buffer), "_%d%s", count, name);
      symbol = gp_coffgen_findsymbol(state.obj.object, buffer);
      if (symbol == NULL)
        break;
      count++;
    }
  } else {
    strncpy(buffer, name, sizeof(buffer));
  }

  return strdup(buffer);
}

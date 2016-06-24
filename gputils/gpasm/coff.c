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

/*------------------------------------------------------------------------------------------------*/

static void
_update_section_symbol(gp_section_t *section)
{
  /* write data to the auxiliary section symbol */
  section->symbol->aux_list->_aux_symbol._aux_scn.length  = section->size;
  section->symbol->aux_list->_aux_symbol._aux_scn.nreloc  = section->num_reloc;
  section->symbol->aux_list->_aux_symbol._aux_scn.nlineno = section->num_lineno;
}

/*------------------------------------------------------------------------------------------------*/

static void
_update_reloc_ptr(void)
{
  gp_section_t *section;
  gp_symbol_t  *symbol;
  gp_reloc_t   *reloc;

  section = state.obj.object->section_list;
  while (section != NULL) {
    reloc = section->relocation_list;
    while (reloc != NULL) {
      symbol = state.obj.object->symbol_list;
      while (symbol != NULL) {
        if (reloc->symbol_number == symbol->number) {
          reloc->symbol = symbol;
          goto l1;
        }
        symbol = symbol->next;
      }
/*      assert(0);*/
      exit(1);
l1:
      reloc = reloc->next;
    }
    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_new_config_section(const char *name, unsigned int addr, unsigned int flags, MemBlock_t *data,
                    gp_boolean new_config)
{
  gp_symbol_t *new;
  gp_aux_t    *new_aux;

  state.obj.symbol_num += 2;

  /* increment the section number */
  state.obj.section_num++;

  /* store the flags so they are available for pass 1 */
  state.obj.flags = flags;

  if (!state.obj.enabled) {
    state.byte_addr = addr;
    return;
  }

  assert(state.obj.object != NULL);

  state.obj.section = gp_coffgen_add_section(state.obj.object, name, data);
  state.obj.section->shadow_address = state.obj.section->address = addr;
  state.obj.section->flags = flags;

  /* add a section symbol */
  new = gp_coffgen_add_symbol(state.obj.object);
  new->name           = GP_Strdup(name);
  new->value          = IS_RAM_ORG ? addr : gp_processor_byte_to_org(state.device.class, addr);
  new->section_number = state.obj.section_num;  /* Modified later. */
  new->section        = state.obj.section;
  new->type           = T_NULL;
  new->class          = C_SECTION;

  state.obj.section->symbol = new;

  new_aux = gp_coffgen_add_aux(state.obj.object, new);
  new_aux->type = AUX_SCN;
}

/*------------------------------------------------------------------------------------------------*/

static void
_create_config_sections(void)
{
  const conf_mem_block_t *conf_sec_mem;
  gp_linenum_t           *linenum;
  char                    section_name[BUFSIZ];
  char                   *upper;

  for (conf_sec_mem = state.conf_sec_mem; conf_sec_mem != NULL; conf_sec_mem = conf_sec_mem->next) {
    upper = gp_strdup_upper_case(state.objfilename);
    snprintf(section_name, sizeof(section_name), ".config_%0*X_%s", state.device.class->addr_digits,
             gp_processor_byte_to_org(state.device.class, conf_sec_mem->addr), upper);
    free(upper);

    _new_config_section(section_name, conf_sec_mem->addr,
                        STYP_ABS | (conf_sec_mem->new_config ? STYP_DATA_ROM : STYP_TEXT),
                        conf_sec_mem->m, conf_sec_mem->new_config);

    if (!state.obj.enabled) {
      return;
    }

    state.obj.section->size      = conf_sec_mem->new_config ? gp_processor_org_to_byte(state.device.class, 1) : 2;
    state.lst.line.was_byte_addr = conf_sec_mem->addr;

    if ((!state.obj.enabled) || (state.obj.section == NULL)) {
      return;
    }

    if (state.debug_info && (state.obj.debug_file == NULL)) {
      gperror_error(GPE_UNKNOWN, ".file directive required to generate debug info");
      return;
    }

    linenum = gp_coffgen_add_linenum(state.obj.section);
    linenum->symbol = conf_sec_mem->file_symbol;
    /* MPASM(X) bug compatibility */
    linenum->line_number = state.mpasm_compatible ? (state.src->line_number - 1) : conf_sec_mem->line_number;
    linenum->address = conf_sec_mem->addr;

    _update_section_symbol(state.obj.section);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
coff_init(void)
{
  if (state.objfile != OUT_NAMED) {
    snprintf(state.objfilename, sizeof(state.objfilename), "%s.o", state.basefilename);
  }

  if (state.objfile == OUT_SUPPRESS) {
    state.obj.enabled = false;
    unlink(state.objfilename);
  }
  else {
    if (!state.processor_chosen) {
      state.obj.enabled = false;
    }
    else {
      state.obj.object            = gp_coffgen_new_object(state.objfilename);
      state.obj.object->processor = state.processor;
      state.obj.object->class     = state.device.class;
      state.obj.object->isnew     = state.obj.newcoff;
      state.obj.enabled           = true;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
coff_close_section(void)
{
  if (state.obj.section == NULL) {
    return;
  }

  state.obj.section->size = state.byte_addr - state.obj.section->address;
  _update_section_symbol(state.obj.section);
}

/*------------------------------------------------------------------------------------------------*/

void
coff_cleanup_before_eof(void)
{
  if (!state.obj.enabled) {
    return;
  }

  /* store data from the last section */
  coff_close_section();

  /* update relocation symbol pointers */
  _update_reloc_ptr();

  /* combine overlayed sections */
  gp_cofflink_combine_overlay(state.obj.object, 1);

  if (state.mode == MODE_RELOCATABLE) {
    /* create config_sections */
    _create_config_sections();
  }
}

/*------------------------------------------------------------------------------------------------*/

void
coff_close_file(void)
{
  if (!state.obj.enabled) {
    return;
  }

  if (!gp_write_coff(state.obj.object, (state.num.errors + gp_num_errors))) {
    gperror_error(GPE_UNKNOWN, "system error while writing object file");
    exit(1);
  }

  gp_coffgen_free_object(state.obj.object);
}

/*------------------------------------------------------------------------------------------------*/

void
coff_new_section(const char *name, unsigned int addr, unsigned int flags)
{
  gp_section_t *found;
  gp_symbol_t  *new;
  gp_aux_t     *new_aux;

  state.obj.symbol_num += 2;

  /* increment the section number */
  state.obj.section_num++;

  /* store the flags so they are available for pass 1 */
  state.obj.flags = flags;

  if (!state.obj.enabled) {
    state.byte_addr = addr;
    return;
  }

  assert(state.obj.object != NULL);

  /* store data from the last section */
  coff_close_section();

  found = gp_coffgen_find_section(state.obj.object, state.obj.object->section_list, name);

  if (found != NULL) {
    if ((flags & STYP_OVERLAY) && (found->flags & STYP_OVERLAY)) {
      /* Overlayed sections can be duplicated.  This allows multiple code
         sections in the same source file to share the same data memory. */
      if ((flags != found->flags) || (addr != found->address)) {
        gperror_verror(GPE_CONTIG_SECTION, NULL, name);
        return;
      }
    } else {
      gperror_verror(GPE_CONTIG_SECTION, NULL, name);
      return;
    }
  }

  state.obj.section                 = gp_coffgen_add_section(state.obj.object, name, NULL);
  state.obj.section->address        = addr;
  state.obj.section->shadow_address = addr;
  state.obj.section->flags          = flags;

  /* add a section symbol */
  new = gp_coffgen_add_symbol(state.obj.object);
  new->name           = GP_Strdup(name);
  new->value          = IS_RAM_ORG ? addr : gp_processor_byte_to_org(state.device.class, addr);
  new->section_number = state.obj.section_num;  /* Modified later. */
  new->section        = state.obj.section;
  new->type           = T_NULL;
  new->class          = C_SECTION;

  state.obj.section->symbol = new;

  new_aux       = gp_coffgen_add_aux(state.obj.object, new);
  new_aux->type = AUX_SCN;

  state.i_memory  = state.obj.section->data;
  state.byte_addr = addr;
}

/*------------------------------------------------------------------------------------------------*/

/* All coff data is generated on the second pass.  To support forward
   references to symbols in the relocation table, the symbol index
   is stored in relocations instead of a pointer to the symbol.  Before
   the coff is written the symbol pointer is updated. */

void
coff_reloc(unsigned int symbol_number, int16_t offset, enum gpasmValTypes type)
{
  gp_reloc_t   *new;
  unsigned int  origin;

  if ((!state.obj.enabled) || (state.obj.section == NULL)) {
    return;
  }

  origin = state.byte_addr - state.obj.section->address;

  new = gp_coffgen_add_reloc(state.obj.section);
  new->address       = origin;
  new->symbol_number = symbol_number;
  new->offset        = offset;
  new->type          = type;
}

/*------------------------------------------------------------------------------------------------*/

void
coff_linenum(unsigned int emitted)
{
  static gp_boolean  show_bad_debug = true;

  gp_linenum_t      *new;
  unsigned int       end;
  unsigned int       origin;

  if ((!state.obj.enabled) || (state.obj.section == NULL)) {
    return;
  }

  /* If the section is absolute, use the abolute address. */
  origin = state.lst.line.was_byte_addr;
  if ((state.obj.section->flags & STYP_ABS) == 0) {
    /* else use the relative address */
    origin -= state.obj.section->address;
  }

  if (state.debug_info && (state.obj.debug_file == NULL)) {
    if (show_bad_debug) {
      gperror_error(GPE_UNKNOWN, ".file directive required to generate debug info");
      show_bad_debug = false;
    }
    return;
  }

  end = origin + emitted;
  while (origin < end) {
    new = gp_coffgen_add_linenum(state.obj.section);

    if (state.debug_info) {
      new->symbol      = state.obj.debug_file;
      new->line_number = state.obj.debug_line;
    } else {
      new->symbol      = state.src->file_symbol;
      new->line_number = state.src->line_number;
    }

    new->address = origin;

    origin += (origin & 1) ? 1 : 2;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Add a symbol to the coff symbol table.  The calling function must
   increment the global symbol number. */

gp_symbol_t *
coff_add_sym(const char *name, gp_symvalue_t value, enum gpasmValTypes type)
{
  gp_symbol_t  *new;
  unsigned int  section_number;
  unsigned int  class;
  char          message[BUFSIZ];

  if (!state.obj.enabled) {
    return NULL;
  }

  switch (type) {
  case GVT_EXTERN:
    section_number = N_UNDEF;
    class          = C_EXT;
    break;

  case GVT_GLOBAL:
    section_number = state.obj.section_num;
    class          = C_EXT;
    break;

  case GVT_STATIC:
    section_number = state.obj.section_num;
    class          = C_STAT;
    break;

  case GVT_ADDRESS:
    section_number = state.obj.section_num;
    class          = C_LABEL;
    break;

  case GVT_DEBUG:
    section_number = N_DEBUG;
    class          = C_NULL;
    break;

  case GVT_ABSOLUTE:
    section_number = N_ABS;
    class          = C_NULL;
    break;

  default:
    return NULL;
  }

  new = gp_coffgen_find_symbol(state.obj.object, name);

  /* verify the duplicate extern has the same properties */
  if ((new != NULL) && (type == GVT_EXTERN))  {
    if ((new->type != type) || (new->class != class) || (new->section_number != section_number)) {
      snprintf(message, sizeof(message),
               "Duplicate label or redefining symbol that cannot be redefined. (%s)", name);
      gperror_error(GPE_UNKNOWN, message);
    }
  }

  if ((new != NULL) && (type != GVT_EXTERN) && (type != GVT_DEBUG))  {
    snprintf(message, sizeof(message),
             "Duplicate label or redefining symbol that cannot be redefined. (%s)", name);
    gperror_error(GPE_DUPLAB, message);
  }
  else {
    new = gp_coffgen_add_symbol(state.obj.object);
    new->name           = GP_Strdup(name);
    new->value          = value;
    new->section_number = section_number;
    new->section        = state.obj.section;
    new->type           = T_NULL;
    new->class          = class;
  }

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* add a file symbol to the coff symbol table */

gp_symbol_t *
coff_add_filesym(const char *name, gp_boolean isinclude)
{
  gp_symbol_t *new;
  gp_aux_t    *new_aux;

  state.obj.symbol_num += 2;

  if (!state.obj.enabled) {
    return NULL;
  }

  /* add .file symbol */
  new = gp_coffgen_add_symbol(state.obj.object);
  new->name           = GP_Strdup(".file");
  new->value          = 0;
  new->section_number = N_DEBUG;
  new->section        = NULL;
  new->type           = T_NULL;
  new->class          = C_FILE;

  new_aux = gp_coffgen_add_aux(state.obj.object, new);
  new_aux->type = AUX_FILE;
  new_aux->_aux_symbol._aux_file.filename = GP_Strdup(name);

  if (isinclude) {
    new_aux->_aux_symbol._aux_file.line_number = state.src->line_number - 1;
  }
  else {
    new_aux->_aux_symbol._aux_file.line_number = 0;
  }

  new_aux->_aux_symbol._aux_file.flags = 0;
  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* add an eof symbol to the coff symbol table */

void
coff_add_eofsym(void)
{
  gp_symbol_t *new;

  state.obj.symbol_num++;

  if (!state.obj.enabled) {
    return;
  }

  /* add .eof symbol */
  new = gp_coffgen_add_symbol(state.obj.object);
  new->name           = GP_Strdup(".eof");
  new->value          = 0;
  new->section_number = N_DEBUG;
  new->section        = NULL;
  new->type           = T_NULL;
  new->class          = C_EOF;
}

/*------------------------------------------------------------------------------------------------*/

/* add a list symbol to the coff symbol table */

void
coff_add_listsym(void)
{
  gp_symbol_t *new;

  if (state.debug_info) {
    return;
  }

  state.obj.symbol_num++;

  if (!state.obj.enabled) {
    return;
  }

  /* add .list symbol */
  new = gp_coffgen_add_symbol(state.obj.object);
  new->name           = GP_Strdup(".list");
  new->value          = state.src->line_number;
  new->section_number = N_DEBUG;
  new->section        = NULL;
  new->type           = T_NULL;
  new->class          = C_LIST;
}

/*------------------------------------------------------------------------------------------------*/

/* add a nolist symbol to the coff symbol table */

void
coff_add_nolistsym(void)
{
  gp_symbol_t *new;

  if (state.debug_info) {
    return;
  }

  state.obj.symbol_num++;

  if (!state.obj.enabled) {
    return;
  }

  /* add .nolist symbol */
  new = gp_coffgen_add_symbol(state.obj.object);
  new->name           = GP_Strdup(".nolist");
  new->value          = state.src->line_number;
  new->section_number = N_DEBUG;
  new->section        = NULL;
  new->type           = T_NULL;
  new->class          = C_LIST;
}

/*------------------------------------------------------------------------------------------------*/

/* add a direct symbol to the coff symbol table */

void
coff_add_directsym(uint8_t command, const char *string)
{
  gp_symbol_t *new;
  gp_aux_t    *new_aux;

  state.obj.symbol_num += 2;

  if (!state.obj.enabled) {
    return;
  }

  /* add .direct symbol */
  new = gp_coffgen_add_symbol(state.obj.object);
  new->name           = GP_Strdup(".direct");
  new->value          = gp_processor_byte_to_org(state.device.class, state.byte_addr);
  new->section_number = state.obj.section_num;
  new->section        = state.obj.section;
  new->type           = T_NULL;
  new->class          = C_NULL;

  new_aux = gp_coffgen_add_aux(state.obj.object, new);
  new_aux->type = AUX_DIRECT;
  new_aux->_aux_symbol._aux_direct.command = command;
  new_aux->_aux_symbol._aux_direct.string  = GP_Strdup(string);
}

/*------------------------------------------------------------------------------------------------*/

/* add an ident symbol to the coff symbol table */

void
coff_add_identsym(const char *string)
{
  gp_symbol_t *new;
  gp_aux_t    *new_aux;

  state.obj.symbol_num += 2;

  if (!state.obj.enabled) {
    return;
  }

  /* add .ident symbol */
  new = gp_coffgen_add_symbol(state.obj.object);
  new->name           = GP_Strdup(".ident");
  new->value          = 0;
  new->section_number = N_DEBUG;
  new->section        = NULL;
  new->type           = T_NULL;
  new->class          = C_NULL;

  new_aux = gp_coffgen_add_aux(state.obj.object, new);
  new_aux->type = AUX_IDENT;
  new_aux->_aux_symbol._aux_ident.string = GP_Strdup(string);
}

/*------------------------------------------------------------------------------------------------*/

/* If the symbol is local, generate a modified name for the coff symbol table. */

char *
coff_local_name(const char *name)
{
  symbol_t    *local;
  gp_symbol_t *symbol;
  int          count;
  char         buffer[BUFSIZ];

  if (!state.obj.enabled) {
    return NULL;
  }

  count = 1;
  local = sym_get_symbol(state.stGlobal, name);
  if (local == NULL) {
    /* It isn't in the stGlobal so it must be in stTop. It's local. */
    while (true) {
      snprintf(buffer, sizeof(buffer), "_%d%s", count, name);
      symbol = gp_coffgen_find_symbol(state.obj.object, buffer);

      if (symbol == NULL) {
        break;
      }

      count++;
    }
  } else {
    gp_strncpy(buffer, name, sizeof(buffer));
  }

  return GP_Strdup(buffer);
}

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
#include "gpmsg.h"
#include "coff.h"

/*------------------------------------------------------------------------------------------------*/

static void
_update_section_symbol(gp_section_t *Section)
{
  /* write data to the auxiliary section symbol */
  Section->symbol->aux_list.first->_aux_symbol._aux_scn.length  = Section->size;
  Section->symbol->aux_list.first->_aux_symbol._aux_scn.nreloc  = Section->relocation_list.num_nodes;
  Section->symbol->aux_list.first->_aux_symbol._aux_scn.nlineno = Section->line_number_list.num_nodes;
}

/*------------------------------------------------------------------------------------------------*/

static void
_update_reloc_ptr(void)
{
  gp_section_t *section;
  gp_symbol_t  *symbol;
  gp_reloc_t   *reloc;
  char          buffer[BUFSIZ];

  section = state.obj.object->section_list.first;
  while (section != NULL) {
    reloc = section->relocation_list.first;
    while (reloc != NULL) {
      symbol = state.obj.object->symbol_list.first;
      while (symbol != NULL) {
        if (reloc->symbol_number == symbol->number) {
          reloc->symbol = symbol;
          goto rel_next;
        }
        symbol = symbol->next;
      }

      snprintf(buffer, sizeof(buffer), "%s() -- Unknown relocation symbol number: %u\n", __func__, reloc->symbol_number);
      gpmsg_error(GPE_INTERNAL, buffer);
      exit(1);

rel_next:
      reloc = reloc->next;
    }

    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_new_config_section(const char *Name, unsigned int Address, unsigned int Flags, MemBlock_t *Data)
{
  gp_symbol_t *new;
  gp_aux_t    *new_aux;

  state.obj.symbol_num += 2;

  /* increment the section number */
  state.obj.section_num++;

  /* store the flags so they are available for pass 1 */
  state.obj.flags = Flags;

  if (!state.obj.enabled) {
    symbol_list_add_symbol(NULL, Name, state.obj.symbol_num - 2, state.obj.section_num, C_SECTION,
                           state.byte_addr);
    state.byte_addr = Address;
    return;
  }

  assert(state.obj.object != NULL);

  symbol_list_flush_symbols(Name);

  state.obj.section                 = gp_coffgen_add_section(state.obj.object, Name, Data);
  state.obj.section->address        = Address;
  state.obj.section->shadow_address = Address;
  state.obj.section->flags          = Flags;

  /* Add a section symbol, the section number modified later. */
  new = gp_coffgen_add_symbol(state.obj.object, Name, state.obj.section_num);
  new->value   = IS_RAM_ORG ? Address : gp_processor_insn_from_byte_c(state.device.class, Address);
  new->section = state.obj.section;
  new->class   = C_SECTION;

  state.obj.section->symbol = new;

  new_aux = gp_coffgen_add_aux(state.obj.object, new);
  new_aux->type = AUX_SECTION;
}

/*------------------------------------------------------------------------------------------------*/

static void
_create_config_sections(void)
{
  const conf_mem_block_t *conf_sec_mem;
  gp_linenum_t           *linenum;
  char                    section_name[BUFSIZ];
  char                   *upper;

  conf_sec_mem = state.conf_sec_mem;
  while (conf_sec_mem != NULL) {
    upper = gp_strdup_upper_case(state.obj_file_name);
    snprintf(section_name, sizeof(section_name), ".config_%0*X_%s", state.device.class->addr_digits,
             gp_processor_insn_from_byte_c(state.device.class, conf_sec_mem->addr), upper);
    free(upper);

    _new_config_section(section_name, conf_sec_mem->addr,
                        STYP_ABS | ((conf_sec_mem->new_config) ? STYP_DATA_ROM : STYP_TEXT),
                        conf_sec_mem->m);

    if (!state.obj.enabled) {
      return;
    }

    state.obj.section->size      = (conf_sec_mem->new_config) ? gp_processor_byte_from_insn_c(state.device.class, 1) : 2;
    state.lst.line.was_byte_addr = conf_sec_mem->addr;

    if (state.debug_info && (state.obj.debug_file == NULL)) {
      gpmsg_error(GPE_UNKNOWN, "The .file directive required to generate debug info.");
      return;
    }

    linenum = gp_coffgen_add_linenum(state.obj.section);
    linenum->symbol      = conf_sec_mem->file_symbol;
    /* MPASM(X) bug compatibility */
    linenum->line_number = (state.mpasm_compatible) ? (state.src_list.last->line_number - 1) : conf_sec_mem->line_number;
    linenum->address     = conf_sec_mem->addr;

    _update_section_symbol(state.obj.section);
    conf_sec_mem = conf_sec_mem->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
coff_init(void)
{
  if (state.obj_file != OUT_NAMED) {
    snprintf(state.obj_file_name, sizeof(state.obj_file_name), "%s.o", state.base_file_name);
  }

  if (state.obj_file == OUT_SUPPRESS) {
    state.obj.enabled = false;
    unlink(state.obj_file_name);
  }
  else {
    if (!state.processor_chosen) {
      state.obj.enabled = false;
    }
    else {
      state.obj.object = gp_coffgen_new_object(state.obj_file_name);
      state.obj.object->processor = state.processor;
      state.obj.object->class     = state.device.class;
      state.obj.object->isnew     = state.obj.newcoff;
      state.obj.enabled           = true;
    }
  }
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

  symbol_list_flush_symbols(NULL);

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

  if (!gp_writeobj_write_coff(state.obj.object, (state.num.errors + gp_num_errors))) {
    gpmsg_error(GPE_UNKNOWN, "System error while writing object file.");
    exit(1);
  }

  gp_coffgen_free_object(state.obj.object);
}

/*------------------------------------------------------------------------------------------------*/

void
coff_new_section(const char *Name, unsigned int Address, unsigned int Flags)
{
  gp_section_t *found;
  gp_symbol_t  *new;
  gp_aux_t     *new_aux;

  state.obj.symbol_num += 2;

  /* increment the section number */
  state.obj.section_num++;

  /* store the flags so they are available for pass 1 */
  state.obj.flags = Flags;

  if (!state.obj.enabled) {
    symbol_list_add_symbol(NULL, Name, state.obj.symbol_num - 2, state.obj.section_num, C_SECTION,
                           state.byte_addr);
    state.byte_addr = Address;
    return;
  }

  assert(state.obj.object != NULL);

  symbol_list_flush_symbols(Name);

  /* store data from the last section */
  coff_close_section();

  found = gp_coffgen_find_section(state.obj.object, state.obj.object->section_list.first, Name);
  if (found != NULL) {
    if (FlagIsSet(Flags, STYP_OVERLAY) && FlagIsSet(found->flags, STYP_OVERLAY)) {
      /* Overlayed sections can be duplicated. This allows multiple code
         sections in the same source file to share the same data memory. */
      if ((Flags != found->flags) || (Address != found->address)) {
        gpmsg_verror(GPE_CONTIG_SECTION, NULL, Name);
        return;
      }
    }
     else {
      gpmsg_verror(GPE_CONTIG_SECTION, NULL, Name);
      return;
    }
  }

  state.obj.section = gp_coffgen_add_section(state.obj.object, Name, NULL);
  state.obj.section->address        = Address;
  state.obj.section->shadow_address = Address;
  state.obj.section->flags          = Flags;

  /* Add a section symbol, the section number modified later. */
  new = gp_coffgen_add_symbol(state.obj.object, Name, state.obj.section_num);
  new->value   = IS_RAM_ORG ? Address : gp_processor_insn_from_byte_c(state.device.class, Address);
  new->section = state.obj.section;
  new->class   = C_SECTION;

  state.obj.section->symbol = new;

  new_aux       = gp_coffgen_add_aux(state.obj.object, new);
  new_aux->type = AUX_SECTION;

  state.i_memory  = state.obj.section->data;
  state.byte_addr = Address;
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

/* Add a symbol to the coff symbol table.  The calling function must
   increment the global symbol number. */

gp_symbol_t *
coff_add_sym(const char *Name, gpasmVal Value, enum gpasmValTypes Type, int Section_number)
{
  gp_symbol_t  *symbol;
  char          message[BUFSIZ];
  unsigned int  class;

  if (!state.obj.enabled) {
    return NULL;
  }

  if (!set_symbol_attr(NULL, &class, Type)) {
    return NULL;
  }

  symbol = gp_coffgen_find_symbol(state.obj.object, Name);

  /* verify the duplicate extern has the same properties */
  if (symbol != NULL) {
    if (Type == VAL_EXTERNAL)  {
      if ((symbol->type != Type) || (symbol->class != class) || (symbol->section_number != Section_number)) {
        snprintf(message, sizeof(message),
                 "Duplicate label or redefining symbol that cannot be redefined: \"%s\"", Name);
        gpmsg_error(GPE_UNKNOWN, message);
      }
    }
    /* (Type != VAL_EXTERNAL) */
    else if (Type != VAL_DEBUG) {
      snprintf(message, sizeof(message),
               "Duplicate label or redefining symbol that cannot be redefined: \"%s\"", Name);
      gpmsg_error(GPE_DUPLAB, message);
    }
  }
  else {
    symbol = gp_coffgen_add_symbol(state.obj.object, Name, Section_number);
    symbol->value   = Value;
    symbol->section = state.obj.section;
    symbol->class   = class;
  }

  return symbol;
}

/*------------------------------------------------------------------------------------------------*/

/* All coff data is generated on the second pass. To support forward
   references to symbols in the relocation table, the symbol index
   is stored in relocations instead of a pointer to the symbol. Before
   the coff is written the symbol pointer is updated. */

void
coff_add_reloc(unsigned int Symbol_number, int16_t Offset, uint16_t Type)
{
  gp_reloc_t   *reloc;
  unsigned int  origin;

  if ((!state.obj.enabled) || (state.obj.section == NULL)) {
    return;
  }

  origin = state.byte_addr - state.obj.section->address;

  reloc = gp_coffgen_add_reloc(state.obj.section);
  reloc->address       = origin;
  reloc->symbol_number = Symbol_number;
  reloc->offset        = Offset;
  reloc->type          = Type;
}

/*------------------------------------------------------------------------------------------------*/

void
coff_add_linenum(unsigned int Emitted)
{
  static gp_boolean  show_bad_debug = true;

  gp_linenum_t      *linenum;
  unsigned int       end;
  unsigned int       origin;

  if ((!state.obj.enabled) || (state.obj.section == NULL)) {
    return;
  }

  /* If the section is absolute, use the abolute address. */
  origin = state.lst.line.was_byte_addr;
  if (FlagIsClr(state.obj.section->flags, STYP_ABS)) {
    /* else use the relative address */
    origin -= state.obj.section->address;
  }

  if (state.debug_info && (state.obj.debug_file == NULL)) {
    if (show_bad_debug) {
      gpmsg_error(GPE_UNKNOWN, "The .file directive required to generate debug info.");
      show_bad_debug = false;
    }
    return;
  }

  end = origin + Emitted;
  while (origin < end) {
    linenum = gp_coffgen_add_linenum(state.obj.section);

    if (state.debug_info) {
      linenum->symbol      = state.obj.debug_file;
      linenum->line_number = state.obj.debug_line;
    }
     else {
      linenum->symbol      = state.src_list.last->file_symbol;
      linenum->line_number = state.src_list.last->line_number;
    }

    linenum->address = origin;

    origin += (origin & 1) ? 1 : 2;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* add a file symbol to the coff symbol table */

gp_symbol_t *
coff_add_file_sym(const char *Name, gp_boolean Is_include)
{
  gp_symbol_t *symbol;
  gp_aux_t    *aux;

  state.obj.symbol_num += 2;

  if (!state.obj.enabled) {
    symbol_list_add_symbol(NULL, ".file", state.obj.symbol_num - 2, 0, C_FILE, state.byte_addr);
    return NULL;
  }

  /* add .file symbol */
  symbol = gp_coffgen_add_symbol(state.obj.object, ".file", N_DEBUG);
  symbol->class = C_FILE;

  aux = gp_coffgen_add_aux(state.obj.object, symbol);
  aux->type                           = AUX_FILE;
  aux->_aux_symbol._aux_file.filename = GP_Strdup(Name);

  if (Is_include) {
    aux->_aux_symbol._aux_file.line_number = state.src_list.last->line_number - 1;
  }
  else {
    aux->_aux_symbol._aux_file.line_number = 0;
  }

  aux->_aux_symbol._aux_file.flags = 0;
  return symbol;
}

/*------------------------------------------------------------------------------------------------*/

/* add an eof symbol to the coff symbol table */

void
coff_add_eof_sym(void)
{
  gp_symbol_t *symbol;

  state.obj.symbol_num++;

  if (!state.obj.enabled) {
    symbol_list_add_symbol(NULL, ".eof", state.obj.symbol_num - 1, 0, C_EOF, state.byte_addr);
    return;
  }

  symbol_list_flush_symbols(".eof");

  /* add .eof symbol */
  symbol = gp_coffgen_add_symbol(state.obj.object, ".eof", N_DEBUG);
  symbol->class = C_EOF;
}

/*------------------------------------------------------------------------------------------------*/

/* add a list symbol to the coff symbol table */

void
coff_add_list_sym(void)
{
  gp_symbol_t *symbol;

  if (state.debug_info) {
    return;
  }

  state.obj.symbol_num++;

  if (!state.obj.enabled) {
    symbol_list_add_symbol(NULL, ".list", state.obj.symbol_num - 1, state.obj.section_num, C_LIST,
                           state.byte_addr);
    return;
  }

  symbol_list_flush_symbols(".list");

  /* add .list symbol */
  symbol = gp_coffgen_add_symbol(state.obj.object, ".list", N_DEBUG);
  symbol->value = state.src_list.last->line_number;
  symbol->class = C_LIST;
}

/*------------------------------------------------------------------------------------------------*/

/* add a nolist symbol to the coff symbol table */

void
coff_add_nolist_sym(void)
{
  gp_symbol_t *symbol;

  if (state.debug_info) {
    return;
  }

  state.obj.symbol_num++;

  if (!state.obj.enabled) {
    symbol_list_add_symbol(NULL, ".nolist", state.obj.symbol_num - 1, state.obj.section_num, C_LIST,
                           state.byte_addr);
    return;
  }

  symbol_list_flush_symbols(".nolist");

  /* add .nolist symbol */
  symbol = gp_coffgen_add_symbol(state.obj.object, ".nolist", N_DEBUG);
  symbol->value = state.src_list.last->line_number;
  symbol->class = C_LIST;
}

/*------------------------------------------------------------------------------------------------*/

/* add a direct symbol to the coff symbol table */

void
coff_add_direct_sym(uint8_t Command, const char *String)
{
  gp_symbol_t *symbol;
  gp_aux_t    *aux;

  state.obj.symbol_num += 2;

  if (!state.obj.enabled) {
    symbol_list_add_symbol(NULL, ".direct", state.obj.symbol_num - 2, state.obj.section_num, C_NULL,
                           state.byte_addr);
    return;
  }

  symbol_list_flush_symbols(".direct");

  /* add .direct symbol */
  symbol = gp_coffgen_add_symbol(state.obj.object, ".direct", state.obj.section_num);
  symbol->value   = gp_processor_insn_from_byte_c(state.device.class, state.byte_addr);
  symbol->section = state.obj.section;

  aux = gp_coffgen_add_aux(state.obj.object, symbol);
  aux->type                            = AUX_DIRECT;
  aux->_aux_symbol._aux_direct.command = Command;
  aux->_aux_symbol._aux_direct.string  = GP_Strdup(String);
}

/*------------------------------------------------------------------------------------------------*/

/* add an ident symbol to the coff symbol table */

void
coff_add_ident_sym(const char *String)
{
  gp_symbol_t *symbol;
  gp_aux_t    *aux;

  state.obj.symbol_num += 2;
  if (!state.obj.enabled) {
    symbol_list_add_symbol(NULL, ".ident", state.obj.symbol_num - 2, state.obj.section_num, C_NULL,
                           state.byte_addr);
    return;
  }

  symbol_list_flush_symbols(".ident");

  /* add .ident symbol */
  symbol = gp_coffgen_add_symbol(state.obj.object, ".ident", N_DEBUG);

  aux = gp_coffgen_add_aux(state.obj.object, symbol);
  aux->type                          = AUX_IDENT;
  aux->_aux_symbol._aux_ident.string = GP_Strdup(String);
}

/*------------------------------------------------------------------------------------------------*/

/* If the symbol is local, generate a modified name for the coff symbol table. */

char *
coff_local_name(const char *Name)
{
  symbol_t     *local;
  gp_symbol_t  *symbol;
  unsigned int  count;
  char          buffer[BUFSIZ];

  if (!state.obj.enabled) {
    return NULL;
  }

  count = 1;
  local = gp_sym_get_symbol(state.stGlobal, Name);
  if (local == NULL) {
    /* It isn't in the stGlobal so it must be in stTop. It's local. */
    while (true) {
      snprintf(buffer, sizeof(buffer), "_%u%s", count, Name);
      symbol = gp_coffgen_find_symbol(state.obj.object, buffer);

      if (symbol == NULL) {
        break;
      }

      count++;
    }
  }
  else {
    gp_strncpy(buffer, Name, sizeof(buffer));
  }

  return GP_Strdup(buffer);
}

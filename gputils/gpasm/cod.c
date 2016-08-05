/* ".COD" file output for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Scott Dattalo
   Copyright (C) 2012 Borut Razem
   Copyright (C) 2016 Molnar Karoly

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
#include "lst.h"
#include "cod.h"

#define COD_FILE_MAX_NUM            1000

static DirBlockInfo *main_dir  = NULL;
static DirBlockInfo *dbi       = NULL;
static unsigned int  _64k_base = 0;

/*------------------------------------------------------------------------------------------------*/

/*
 * _write_file_block - Write a code block that contains a list of the source files.
 */

static void
_write_source_file_block(void)
{
  const file_context_t *fc;
  BlockList            *fb;
  unsigned int          length;
  gp_boolean            truncated;

  if (state.file_list.first == NULL) {
    return;
  }

  if (state.file_list.num_nodes >= COD_FILE_MAX_NUM) {
    /* Too many files to handle in the .cod file. */
    assert(0);
  }

  fb = NULL;
  fc = state.file_list.first;
  while (fc != NULL) {
    if ((fb == NULL) || (main_dir->file.offset >= (COD_FILE_NAMES_PER_BLOCK * COD_FILE_NAME_SIZE))) {
      fb = gp_cod_block_append(&main_dir->file, gp_cod_block_new());
    }

    /* The file id is used to define the index at which the file name is written within
     * the file code block. (The id's are sequentially assigned when the files are opened.)
     * If there are too many files, then gpasm will abort. Note: .cod files can handle
     * larger file lists...
     */

    length = gp_Pstr_from_str(&fb->block[main_dir->file.offset], COD_FILE_NAME_SIZE, fc->name, &truncated);

    if (truncated && (state.strict_level > 0)) {
      gpmsg_vwarning(GPW_STRING_TRUNCATE, "(.COD)", fc->name, length);
    }

    main_dir->file.offset += COD_FILE_NAME_SIZE;

    fc = fc->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* cod_init - Initialize the .cod file. */

void
cod_init(void)
{
  if (state.cod_file != OUT_NAMED) {
    snprintf(state.cod_file_name, sizeof(state.cod_file_name), "%s.cod", state.base_file_name);
  }

  if (state.cod_file == OUT_SUPPRESS) {
    state.cod.f       = NULL;
    state.cod.enabled = false;
    unlink(state.cod_file_name);
  }
  else {
    state.cod.f = fopen(state.cod_file_name, "wb");
    if (state.cod.f == NULL) {
      perror(state.cod_file_name);
      exit(1);
    }
    state.cod.enabled = true;
  }

  if (!state.cod.enabled) {
    main_dir = NULL;
    return;
  }

  main_dir  = gp_cod_init_dir_block(state.cod_file_name, "gpasm");
  dbi       = NULL;
  _64k_base = 0;
}

/*------------------------------------------------------------------------------------------------*/

/* cod_lst_line - Add a line of information that cross references the
 *                the opcode's address, the source file, and the list file.
 */

#define COD_LST_FIRST_LINE              7

void
cod_lst_line(unsigned int List_line)
{
  source_context_t *ctx;
  uint8_t           smod_flag;
  BlockList        *lb;
  gp_boolean        first_time;
  unsigned int      address;
  unsigned int      high_address;

  if (!state.cod.enabled) {
    return;
  }

  ctx = state.src_list.last;

  /* Don't start until after the source is open. */
  if (ctx == NULL) {
    return;
  }

  /* Ignore the first few line numbers. */
/*  if (state.lst.line_number < COD_LST_FIRST_LINE) {*/
  if (state.lst.line_number < List_line) {
    return;
  }

  address      = gp_processor_insn_from_byte_c(state.device.class, state.lst.line.was_byte_addr);
  high_address = IMemBaseFromAddr(address);

  if ((dbi == NULL) || (high_address != _64k_base)) {
    _64k_base = high_address;
    dbi       = gp_cod_find_dir_block_by_high_addr(main_dir, _64k_base);
  }

  first_time = (gp_cod_block_get_last(&dbi->list) == NULL) ? true : false;

  lb = gp_cod_block_get_last_or_new(&dbi->list);

  if (dbi->list.offset >= (COD_MAX_LINE_SYM * COD_LINE_SYM_SIZE)) {
    lb = gp_cod_block_append(&dbi->list, gp_cod_block_new());
  }

  assert(ctx->fc != NULL);

  smod_flag = (first_time) ? COD_LS_SMOD_FLAG_ALL :
                             ((state.cod.emitting != 0) ? COD_LS_SMOD_FLAG_C1 :
                                                          (COD_LS_SMOD_FLAG_C1 | COD_LS_SMOD_FLAG_D));

  dbi->list.offset += gp_cod_put_line_number(&lb->block[dbi->list.offset], ctx->fc->id, ctx->line_number,
                                             address, smod_flag);
}

/*------------------------------------------------------------------------------------------------*/

/* cod_write_symbols - Write the symbol table to the .cod file.
 *
 * This routine will read the symbol table that gpasm has created and convert it into
 * a format suitable for .cod files. So far, only three variable types are supported:
 * address, register and constants
 *
 */

void
cod_write_symbols(const symbol_t **Symbol_list, size_t Num_symbols)
{
  size_t            i;
  unsigned int      length;
  unsigned int      type;
  const variable_t *var;
  const char       *name;
  BlockList        *sb;
  gp_boolean        truncated;

  if ((Symbol_list == NULL) || (Num_symbols == 0)) {
    return;
  }

  if (!state.cod.enabled) {
    return;
  }

  sb = NULL;
  for (i = 0; i < Num_symbols; i++) {
    name = gp_sym_get_symbol_name(Symbol_list[i]);
    var  = (const variable_t *)gp_sym_get_symbol_annotation(Symbol_list[i]);
    assert(var != NULL);

    if (FlagIsSet(var->flags, VATRR_HAS_NO_VALUE)) {
      msg_has_no_value("(.COD)", name);
    }

    length = gp_strlen_Plimit(name, COD_LSYMBOL_NAME_MAX_SIZE, &truncated);

    if (truncated && (state.strict_level > 0)) {
      /* This symbol name is too long. */
      gpmsg_vwarning(GPW_STRING_TRUNCATE, "(.COD)", name, length);
    }

    /* If this symbol extends past the end of the cod block then write this block out. */

    if ((sb == NULL) || ((main_dir->lsym.offset + length + COD_LSYMBOL_EXTRA) >= COD_BLOCK_SIZE)) {
      sb = gp_cod_block_append(&main_dir->lsym, gp_cod_block_new());
    }

    switch (var->type) {
      case VAL_CBLOCK:
        type = COD_ST_C_SHORT;  /* Byte Craft's nomenclature for a memory byte. */
        break;

      case VAL_ADDRESS:
        type = COD_ST_ADDRESS;
        break;

      case VAL_CONSTANT:
      case VAL_VARIABLE:
      default:
        type = COD_ST_CONSTANT;
    }

    main_dir->lsym.offset += gp_cod_put_long_symbol(&sb->block[main_dir->lsym.offset], name, var->value, type);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
cod_close_file(void)
{
  unsigned int  length;
  gp_boolean    truncated;
  const char   *name;

  if (!state.cod.enabled) {
    return;
  }

  name = gp_processor_name(state.processor, 2);
  /* The processor is unknown if not defined in command line at cod_init() call
     so it should be set here. */
  length = gp_Pstr_from_str(&main_dir->dir[COD_DIR_PROCESSOR], COD_DIR_PROCESSOR_SIZE, name, &truncated);

  if (truncated && (state.strict_level > 0)) {
    gpmsg_vwarning(GPW_STRING_TRUNCATE, "(.COD)", name, length);
  }

  _write_source_file_block();
  gp_cod_write_code(state.device.class, state.i_memory, main_dir);
  gp_cod_enumerate_directory(main_dir);
  gp_cod_write_directory(state.cod.f, main_dir);
  gp_cod_free_directory(main_dir);
  fclose(state.cod.f);

  main_dir  = NULL;
  dbi       = NULL;
  _64k_base = 0;
}

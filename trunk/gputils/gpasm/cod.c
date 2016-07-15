/* ".COD" file output for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Scott Dattalo
   Copyright (C) 2012 Borut Razem

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
#include "lst.h"
#include "cod.h"

#define COD_FILE_MAX_NUM            1000

static DirBlockInfo *main_dir;

/*------------------------------------------------------------------------------------------------*/

/*
 * _write_file_block - Write a code block that contains a list of the source files.
 */

static void
_write_file_block(void)
{
  const file_context_t *fc;
  BlockList            *fb;

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
    if ((fb == NULL) || (main_dir->src.offset >= (FILES_PER_BLOCK * FILE_SIZE))) {
      fb = gp_blocks_append(&main_dir->src, gp_blocks_new());
    }

    /* The file id is used to define the index at which the file
     * name is written within the file code block. (The id's are
     * sequentially assigned when the files are opened.) If there
     * are too many files, then gpasm will abort. Note: .cod files
     * can handle larger file lists...
     */

    gp_cod_strncpy(&fb->block[main_dir->src.offset + 1], fc->name, FILE_SIZE - 1);
    main_dir->src.offset += FILE_SIZE;

    fc = fc->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* cod_init - initialize the cod file */

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
    return;
  }

  main_dir = gp_cod_init_dir_block(state.cod_file_name, "gpasm");
}

/*------------------------------------------------------------------------------------------------*/

/* cod_lst_line - Add a line of information that cross references the
 *                the opcode's address, the source file, and the list file.
 */

#define COD_LST_FIRST_LINE              7

void
cod_lst_line(unsigned int List_line)
{
  static DirBlockInfo *dbi = NULL;
  static int           _64k_base = 0;

  source_context_t    *ctx;
  uint8_t              smod_flag;
  BlockList           *lb;
  gp_boolean           first_time;
  int                  address;
  int                  high_address;

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

  first_time = (gp_blocks_get_last(&dbi->lst) == NULL) ? true : false;

  lb = gp_blocks_get_last_or_new(&dbi->lst);

  if (dbi->lst.offset >= (COD_MAX_LINE_SYM * COD_LINE_SYM_SIZE)) {
    lb = gp_blocks_append(&dbi->lst, gp_blocks_new());
  }

  assert(ctx->fc != NULL);
  lb->block[dbi->lst.offset + COD_LS_SFILE] = ctx->fc->id;

  smod_flag = (first_time) ? COD_LS_SMOD_FLAG_ALL :
                             ((state.cod.emitting != 0) ? COD_LS_SMOD_FLAG_C1 :
                                                          (COD_LS_SMOD_FLAG_C1 | COD_LS_SMOD_FLAG_D));

  lb->block[dbi->lst.offset + COD_LS_SMOD] = smod_flag;

  /* Write the source file line number corresponding to the list file line number. */
  gp_putl16(&lb->block[dbi->lst.offset + COD_LS_SLINE], ctx->line_number);

  /* Write the address of the opcode. */
  gp_putl16(&lb->block[dbi->lst.offset + COD_LS_SLOC], address);

  dbi->lst.offset += COD_LINE_SYM_SIZE;
}

/*------------------------------------------------------------------------------------------------*/

/* cod_write_symbols - write the symbol table to the .cod file
 *
 * This routine will read the symbol table that gpasm has created
 * and convert it into a format suitable for .cod files. So far, only
 * three variable types are supported: address, register, and constants.
 *
 */

void
cod_write_symbols(const symbol_t **Symbol_list, size_t Num_symbols)
{
  size_t            i;
  int               len;
  int               type;
  const variable_t *var;
  const char       *name;
  BlockList        *sb;

  if ((Symbol_list == NULL) || (Num_symbols == 0)) {
    return;
  }

  if (!state.cod.enabled) {
    return;
  }

  sb = NULL;
  for (i = 0; i < Num_symbols; i++) {
    name = gp_sym_get_symbol_name(Symbol_list[i]);
    var  = gp_sym_get_symbol_annotation(Symbol_list[i]);
    len  = strlen(name);

    /* If this symbol extends past the end of the cod block then write this block out. */

    if ((sb == NULL) || ((main_dir->sym.offset + len + COD_SYM_EXTRA) >= COD_BLOCK_SIZE)) {
      sb = gp_blocks_append(&main_dir->sym, gp_blocks_new());
    }

    gp_cod_strncpy(&sb->block[main_dir->sym.offset + 1], name, MAX_SYM_LEN);

    switch (var->type) {
      case VAL_CBLOCK:
        type = COD_ST_C_SHORT;  /* Byte craft's nomenclature for a memory byte. */
        break;

      case VAL_ADDRESS:
        type = COD_ST_ADDRESS;
        break;

      case VAL_CONSTANT:
      case VAL_VARIABLE:
      default:
        type = COD_ST_CONSTANT;
    }

    gp_putl16(&sb->block[main_dir->sym.offset + len + COD_SYM_TYPE], type);

    /* write 32 bits, big endian */
    gp_putb32(&sb->block[main_dir->sym.offset + len + COD_SYM_VALUE], var->value);

    main_dir->sym.offset += len + COD_SYM_EXTRA;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
cod_close_file(void)
{
  if (!state.cod.enabled) {
    return;
  }

  /* processor is unknown if not defined in command line at cod_init() call
     so it should be set here */
  gp_cod_strncpy(&main_dir->dir[COD_DIR_PROCESSOR], gp_processor_name(state.processor, 2),
                 COD_DIR_LSYMTAB - COD_DIR_PROCESSOR);
  _write_file_block();
  gp_cod_write_code(state.device.class, state.i_memory, main_dir);
  gp_blocks_enumerate_directory(main_dir);
  gp_blocks_write_directory(state.cod.f, main_dir);
  gp_blocks_free_directory(main_dir);
  main_dir = NULL;
  fclose(state.cod.f);
}

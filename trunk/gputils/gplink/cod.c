/* ".COD" file output for gplink
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
#include "gplink.h"
#include "cod.h"

static DirBlockInfo *main_dir;

/*------------------------------------------------------------------------------------------------*/

/* Assign each file name unique file number. A file may appear in the symbol table more than once. */

static void
_assign_file_id(void)
{
  symbol_table_t *file_table;
  gp_symbol_t    *symbol;
  gp_aux_t       *aux;
  symbol_t       *sym;
  int             file_id = 0;
  int            *value;

  /* build a case sensitive file table */
  file_table = gp_sym_push_table(NULL, false);

  symbol = state.object->symbol_list.first;
  while (symbol != NULL) {
    if (symbol->class == C_FILE) {
      aux = symbol->aux_list.first;
      assert(aux != NULL);
      sym = gp_sym_get_symbol(file_table, aux->_aux_symbol._aux_file.filename);

      if (sym != NULL) {
        /* fetch the file number */
        value = (int *)gp_sym_get_symbol_annotation(sym);
      }
      else {
        /* the file hasn't been assigned a value */
        value  = (int *)GP_Malloc(sizeof(int));
        *value = file_id++;
        sym = gp_sym_add_symbol(file_table, aux->_aux_symbol._aux_file.filename);
        gp_sym_annotate_symbol(sym, value);
      }
      symbol->number = *value;
    }
    symbol = symbol->next;
  }

  /* destory the table */
  file_table = gp_sym_pop_table(file_table);
}

/*------------------------------------------------------------------------------------------------*/

/* _write_file_block - Write a code block that contains a list of the source files. */

static void
_write_file_block(void)
{
  const gp_symbol_t *symbol;
  BlockList         *fb = NULL;
  int                file_id = 0;

  symbol = state.object->symbol_list.first;
  while (symbol != NULL) {
    if ((fb == NULL) || (main_dir->src.offset >= (FILES_PER_BLOCK * COD_DIR_SOURCE_P_SIZE))) {
      fb = gp_blocks_append(&main_dir->src, gp_blocks_new());
    }

    if ((symbol->class == C_FILE) && (symbol->number == file_id)) {
      /* skip the duplicate file symbols */
      file_id++;

      /* The file id is used to define the index at which the file
       * name is written within the file code block. (The id's are
       * sequentially assigned when the files are opened.) If there
       * are too many files, then gpasm will abort. note: .cod files
       * can handle larger file lists...
       */

      gp_cod_Pstrncpy(&fb->block[main_dir->src.offset + 1],
                      symbol->aux_list.first->_aux_symbol._aux_file.filename, COD_DIR_SOURCE_C_SIZE);

      main_dir->src.offset += COD_DIR_SOURCE_P_SIZE;
    }

    symbol = symbol->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* cod_write_symbols - write the symbol table to the .cod file
 *
 * This routine will read the symbol table that gplink has created
 * and convert it into a format suitable for .cod files. So far, only
 * three variable types are supported: address, register, and constants.
 *
 */

static void
_write_symbols(const symbol_t **symbol_list, size_t num_symbols)
{
  size_t                 i;
  int                    len;
  int                    type;
  const gp_coffsymbol_t *var;
  const gp_symbol_t     *symbol;
  const gp_section_t    *section;
  const char            *name;
  BlockList             *sb;

  if ((symbol_list == NULL) || (num_symbols == 0)) {
    return;
  }

  sb = NULL;
  for (i = 0; i < num_symbols; i++) {
    name = gp_sym_get_symbol_name(symbol_list[i]);
    var  = gp_sym_get_symbol_annotation(symbol_list[i]);
    len  = strlen(name);

    /* If this symbol extends past the end of the cod block
     * then write this block out */

    if ((sb == NULL) || ((main_dir->sym.offset + len + COD_SYM_EXTRA) >= COD_BLOCK_SIZE)) {
      sb = gp_blocks_append(&main_dir->sym, gp_blocks_new());
    }

    gp_cod_Pstrncpy(&sb->block[main_dir->sym.offset + 1], name, MAX_SYM_LEN);

    symbol = var->symbol;
    assert(symbol != NULL);
    section = symbol->section;
    assert(section != NULL);

    if (FlagIsSet(section->flags, STYP_TEXT)) {
      type = COD_ST_ADDRESS;
    }
    else if (FlagIsSet(section->flags, STYP_RAM_AREA)) {
      type = COD_ST_C_SHORT;
    }
    else {
      type = COD_ST_CONSTANT;
    }

    gp_putl16(&sb->block[main_dir->sym.offset + len + COD_SYM_TYPE], type);

    /* write 32 bits, big endian */
    gp_putb32(&sb->block[main_dir->sym.offset + len + COD_SYM_VALUE], symbol->value);

    main_dir->sym.offset += len + COD_SYM_EXTRA;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_write_symbol_table(const symbol_table_t *Table)
{
  const symbol_t **lst;
  size_t           sym_count;

  sym_count = gp_sym_get_symbol_count(Table);

  if (sym_count == 0) {
    return;
  }

  lst = gp_sym_clone_symbol_array(Table, gp_sym_compare_fn);
  _write_symbols(lst, sym_count);
  free(lst);
}

/*------------------------------------------------------------------------------------------------*/

/* _write_debug - write debug symbols to the .cod file */

static void
_write_debug(void)
{
  int                len;
  const gp_symbol_t *symbol;
  const gp_aux_t    *aux;
  BlockList         *db;
  char               command;
  const char        *string;

  db     = NULL;
  symbol = state.object->symbol_list.first;
  while (symbol != NULL) {
    if (strcasecmp(".direct", symbol->name) == 0) {
      assert(symbol->aux_list.num_nodes == 1);
      aux = symbol->aux_list.first;
      assert(aux != NULL);

      command = aux->_aux_symbol._aux_direct.command;
      string  = aux->_aux_symbol._aux_direct.string;

      len = strlen(string);

      /* If this message extends past the end of the cod block then write this block out. */

      if ((db == NULL) || ((main_dir->dbg.offset + len + COD_DEBUG_EXTRA) >= COD_BLOCK_SIZE)) {
        db = gp_blocks_append(&main_dir->dbg, gp_blocks_new());
      }

      /* write 32 bits, big endian */
      gp_putb32(&db->block[main_dir->dbg.offset + COD_DEBUG_ADDR], symbol->value);

      db->block[main_dir->dbg.offset + COD_DEBUG_CMD] = command;
      gp_cod_Pstrncpy(&db->block[main_dir->dbg.offset + COD_DEBUG_MSG], string, MAX_STRING_LEN);

      main_dir->dbg.offset += len + COD_DEBUG_EXTRA;
    }
    symbol = symbol->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* init_cod - initialize the cod file */

void
cod_init(void)
{
  if (state.cod_file != OUT_NAMED) {
    snprintf(state.cod_file_name, sizeof(state.cod_file_name), "%s.cod", state.base_file_name);
  }

  if ((gp_num_errors > 0) || (state.cod_file == OUT_SUPPRESS)) {
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

  main_dir = gp_cod_init_dir_block(state.cod_file_name, "gplink");

  _assign_file_id();
}

/*------------------------------------------------------------------------------------------------*/

/* cod_lst_line - Add a line of information that cross references the
 *                the opcode's address, the source file, and the list file.
 */

void
cod_lst_line(int line_type)
{
  static DirBlockInfo *dbi = NULL;
  static int           _64k_base = 0;

  uint8_t              smod_flag;
  BlockList           *lb;
  gp_boolean           first_time;
  int                  address;
  int                  high_address;

  if (!state.cod.enabled) {
    return;
  }

  address      = gp_processor_insn_from_byte_c(state.class, state.lst.was_byte_addr);
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

  assert(state.lst.src != NULL);
  assert(state.lst.src->symbol != NULL);
  lb->block[dbi->lst.offset + COD_LS_SFILE] = state.lst.src->symbol->number;

  smod_flag = (first_time) ? COD_LS_SMOD_FLAG_ALL :
                             ((state.cod.emitting) ? COD_LS_SMOD_FLAG_C1 :
                                                     (COD_LS_SMOD_FLAG_C1 | COD_LS_SMOD_FLAG_D));

  lb->block[dbi->lst.offset + COD_LS_SMOD] = smod_flag;

  /* Write the source file line number corresponding to the list file line number. */
  gp_putl16(&lb->block[dbi->lst.offset + COD_LS_SLINE], state.lst.src->line_number);

  /* Write the address of the opcode. */
  gp_putl16(&lb->block[dbi->lst.offset + COD_LS_SLOC], address);

  dbi->lst.offset += COD_LINE_SYM_SIZE;
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
  gp_cod_Pstrncpy(&main_dir->dir[COD_DIR_PROCESSOR + 1], gp_processor_name(state.processor, 2),
                  COD_DIR_PROCESSOR_C_SIZE);

  /* All the global symbols are written.  Need to figure out what to do about the local symbols. */
  _write_symbol_table(state.symbol.definition);
  _write_file_block();
  gp_cod_write_code(state.class, state.i_memory, main_dir);
  _write_debug();
  gp_blocks_enumerate_directory(main_dir);
  gp_blocks_write_directory(state.cod.f, main_dir);
  gp_blocks_free_directory(main_dir);
  main_dir = NULL;
  fclose(state.cod.f);
}

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

static DirBlockInfo *main_dir;

static DirBlockInfo *
new_dir_block(void)
{
  /* initialize eveything to zero */
  DirBlockInfo *dir = GP_Calloc(1, sizeof(DirBlockInfo));

  gp_putl16(&dir->dir[COD_DIR_CODTYPE], 1);
  return dir;
}

static DirBlockInfo *
init_dir_block(void)
{
  DirBlockInfo *dir = new_dir_block();

  /* Initialize the directory block with known data. It'll be written
   * to the .cod file after everything else. */
  gp_cod_strncpy(&dir->dir[COD_DIR_SOURCE],
                 state.codfilename, COD_DIR_DATE - COD_DIR_SOURCE);
  gp_cod_date(&dir->dir[COD_DIR_DATE], COD_DIR_TIME - COD_DIR_DATE);
  gp_cod_time(&dir->dir[COD_DIR_TIME], COD_DIR_VERSION - COD_DIR_TIME);
  gp_cod_strncpy(&dir->dir[COD_DIR_VERSION],
                 VERSION, COD_DIR_COMPILER - COD_DIR_VERSION);
  gp_cod_strncpy(&dir->dir[COD_DIR_COMPILER],
                 "gpasm", COD_DIR_NOTICE - COD_DIR_COMPILER);
  gp_cod_strncpy(&dir->dir[COD_DIR_NOTICE],
                 GPUTILS_COPYRIGHT_STRING,
                 COD_DIR_SYMTAB - COD_DIR_NOTICE);

  /* The address is always two shorts or 4 bytes long. */
  dir->dir[COD_DIR_ADDRSIZE] = 0;

  return dir;
}

/*
 * init_cod - initialize the cod file
 */
void
cod_init(void)
{
  if (state.codfile != OUT_NAMED) {
    snprintf(state.codfilename, sizeof(state.codfilename),
             "%s.cod", state.basefilename);
  }

  if (state.codfile == OUT_SUPPRESS) {
    state.cod.f = NULL;
    state.cod.enabled = false;
    unlink(state.codfilename);
  } else {
    state.cod.f = fopen(state.codfilename, "wb");
    if (state.cod.f == NULL) {
      perror(state.codfilename);
      exit(1);
    }
    state.cod.enabled = true;
  }

  if (!state.cod.enabled) {
    return;
  }

  main_dir = init_dir_block();
}

/*
 * write_file_block - write a code block that contains a list of the
 * source files.
 */
static void
write_file_block(void)
{
  BlockList *fb = NULL;
  int id_number = 0;
  const struct file_context *fc;

  if (state.files == NULL) {
    return;
  }

  /* Find the head of the file list: */

  fc = state.files;
  while ((fc->prev != NULL) && (id_number++ < 1000)) {
    fc = fc->prev;
  }

  if (id_number >= 1000) {
    /* Too many files to handle in the .cod file. */
    assert(0);
  }

  id_number = 0;

  while (fc != NULL) {
    if ((fb == NULL) || (main_dir->src.offset >= (FILES_PER_BLOCK * FILE_SIZE))) {
      fb = gp_blocks_append(&main_dir->src, gp_blocks_new());
    }

    /* The file id is used to define the index at which the file
     * name is written within the file code block. (The id's are
     * sequentially assigned when the files are opened.) If there
     * are too many files, then gpasm will abort. note: .cod files
     * can handle larger file lists...
     */

    gp_cod_strncpy(&fb->block[main_dir->src.offset + 1], fc->name, FILE_SIZE - 1);
    main_dir->src.offset += FILE_SIZE;

    fc = fc->next;
  }
}

static DirBlockInfo *
find_dir_block_by_high_addr(int high_addr)
{
  DirBlockInfo *dbi = main_dir;

  /* find the directory containing high_addr 64k segment */
  while (gp_getl16(&dbi->dir[COD_DIR_HIGHADDR]) != high_addr) {
    /* If the next directory block (in the linked list of directory
       blocks) is NULL, then this is the first time to encounter this
       _64k segment. So we need to create a new segment. */
    if (dbi->next == NULL) {
      dbi->next = new_dir_block();
      gp_putl16(&dbi->next->dir[COD_DIR_HIGHADDR], high_addr);
      dbi = dbi->next;
      break;
    }
    else {
      dbi = dbi->next;
    }
  }
  return dbi;
}

/* cod_lst_line - add a line of information that cross references the
 * the opcode's address, the source file, and the list file.
 */

void
cod_lst_line(int line_type)
{
#define COD_LST_FIRST_LINE  7
  static DirBlockInfo *dbi = NULL;
  static int _64k_base = 0;

  unsigned char smod_flag;
  BlockList *lb;
  int first_time;
  int address, high_address;

  if (!state.cod.enabled) {
    return;
  }

  /* Don't start until after the source is open. */
  if (state.src == NULL) {
    return;
  }

  /* Ignore the first few line numbers. */
  if (state.lst.line_number < COD_LST_FIRST_LINE) {
    return;
  }

  address = gp_processor_byte_to_org(state.device.class, state.lst.line.was_byte_addr);
  high_address = (address >> 16) & 0xffff;

  if ((dbi == NULL) || (high_address != _64k_base)) {
    _64k_base = high_address;
    dbi = find_dir_block_by_high_addr(_64k_base);
  }

  first_time = (gp_blocks_get_last(&dbi->lst) == NULL) ? true : false;

  lb = gp_blocks_get_last_or_new(&dbi->lst);

  if (dbi->lst.offset >= (COD_MAX_LINE_SYM * COD_LINE_SYM_SIZE)) {
    lb = gp_blocks_append(&dbi->lst, gp_blocks_new());
  }

  assert(state.src->fc != NULL);
  lb->block[dbi->lst.offset + COD_LS_SFILE] = state.src->fc->id;

  smod_flag = (first_time) ? COD_LS_SMOD_FLAG_ALL :
                             ((state.cod.emitting != 0) ? COD_LS_SMOD_FLAG_C1 :
                                                          (COD_LS_SMOD_FLAG_C1 | COD_LS_SMOD_FLAG_D));

  lb->block[dbi->lst.offset + COD_LS_SMOD] = smod_flag;

  /* Write the source file line number corresponding to the list file line number. */
  gp_putl16(&lb->block[dbi->lst.offset + COD_LS_SLINE], state.src->line_number);

  /* Write the address of the opcode. */
  gp_putl16(&lb->block[dbi->lst.offset + COD_LS_SLOC], address);

  dbi->lst.offset += COD_LINE_SYM_SIZE;
}

/* cod_write_symbols - write the symbol table to the .cod file
 *
 * This routine will read the symbol table that gpasm has created
 * and convert it into a format suitable for .cod files. So far, only
 * three variable types are supported: address, register, and constants.
 *
 */

void
cod_write_symbols(struct symbol **symbol_list, int num_symbols)
{
  int i, len, type;
  const struct variable *var;
  const char *name;
  BlockList *sb = NULL;

  if (!state.cod.enabled) {
    return;
  }

  for (i = 0; i < num_symbols; i++) {
    var = get_symbol_annotation(symbol_list[i]);
    name = get_symbol_name(symbol_list[i]);
    len = strlen(name);

    /* If this symbol extends past the end of the cod block
     * then write this block out */

    if ((sb == NULL) || ((main_dir->sym.offset + len + COD_SYM_EXTRA) >= COD_BLOCK_SIZE)) {
      sb = gp_blocks_append(&main_dir->sym, gp_blocks_new());
    }

    gp_cod_strncpy(&sb->block[main_dir->sym.offset + 1], name, MAX_SYM_LEN);

    switch (var->type) {
    case GVT_CBLOCK:
      type = COD_ST_C_SHORT;    /* byte craft's nomenclature for a memory byte. */
      break;

    case GVT_ADDRESS:
      type = COD_ST_ADDRESS;
      break;

    case GVT_CONSTANT:
    default:
      type = COD_ST_CONSTANT;
    }

    gp_putl16(&sb->block[main_dir->sym.offset + len + COD_SYM_TYPE], type);

    /* write 32 bits, big endian */
    gp_putb32(&sb->block[main_dir->sym.offset + len + COD_SYM_VALUE], var->value);

    main_dir->sym.offset += len + COD_SYM_EXTRA;
  }
}

/* cod_emit_opcode - write one opcode to a cod_image_block
 */
static void
cod_emit_opcode(DirBlockInfo *dbi, int address, int opcode)
{
  int block_index;

  if (!state.cod.enabled) {
    return;
  }

  /* The code image blocks are handled in a different manner than the
   * other cod blocks. In theory, it's possible to emit opcodes in a
   * non-sequential manner. Furthermore, it's possible that there may
   * be gaps in the program memory. These cases are handled by an array
   * of code blocks. The lower 8 bits of the opcode's address form an
   * index into the code block, while bits 9-15 are an index into the
   * array of code blocks. The code image blocks are not written until
   * all of the opcodes have been emitted.
   */

  block_index = (address >> COD_BLOCK_BITS) & (COD_CODE_IMAGE_BLOCKS - 1);

  if (dbi->cod_image_blocks[block_index].block == NULL) {
    gp_cod_create(&dbi->cod_image_blocks[block_index]);
  }

  gp_putl16(&dbi->cod_image_blocks[block_index].block[address & (COD_BLOCK_SIZE - 1)], opcode);
}

/* cod_write_code - write all of the assembled pic code to the .cod file
 */
static void
cod_write_code(void)
{
  static DirBlockInfo *dbi = NULL;
  static int _64k_base = 0;

  const MemBlock *m = state.i_memory;
  int mem_base, i;
  int start_address = 0, used_flag = 0;
  BlockList *rb = NULL;

  while (m != NULL) {
    int high_addr;

    mem_base = m->base << I_MEM_BITS;
    high_addr = (mem_base >> 16) & 0xffff;

    if ((dbi == NULL) || (high_addr != _64k_base)) {
      _64k_base = high_addr;
      dbi = find_dir_block_by_high_addr(_64k_base);
    }

    for (i = mem_base; (i - mem_base) <= MAX_I_MEM; i += 2) {
      unsigned short insn;

      if (((i - mem_base) < MAX_I_MEM) &&
          state.device.class->i_memory_get(state.i_memory, i, &insn, NULL, NULL)) {
        cod_emit_opcode(dbi, i, insn);

        if (used_flag == 0) {
          /* Save the start address in a range of opcodes */
          start_address = i;
          used_flag = 1;
        }
      } else {
        /* No code at address i, but we need to check if this is the
           first empty address after a range of address. */
        if (used_flag == 1) {
          rb = gp_blocks_get_last_or_new(&dbi->rng);

          if ((rb == NULL) || ((dbi->rng.offset + COD_MAPENTRY_SIZE) >= COD_BLOCK_SIZE)) {
            /* If there are a whole bunch of non-contiguous pieces of
               code then we'll get here. But most pic apps will only need
               one directory block (that will give you 64 ranges or non-
               contiguous chunks of pic code). */
            rb = gp_blocks_append(&dbi->rng, gp_blocks_new());
          }
          /* We need to update dir map indicating a range of memory that
             is needed. This is done by writing the start and end address to
             the directory map. */
          gp_putl16(&rb->block[dbi->rng.offset + COD_MAPTAB_START], start_address);
          gp_putl16(&rb->block[dbi->rng.offset + COD_MAPTAB_LAST], i - 1);

          used_flag = 0;

          dbi->rng.offset += COD_MAPENTRY_SIZE;
        }
      }
    }

    m = m->next;
  }
}

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
  write_file_block();
  cod_write_code();
  gp_blocks_enumerate_directory(main_dir);
  gp_blocks_write_directory(state.cod.f, main_dir);
  gp_blocks_free_directory(main_dir);
  main_dir = NULL;
  fclose(state.cod.f);
}

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
  gp_cod_date(&dir->dir[COD_DIR_DATE],
              COD_DIR_TIME - COD_DIR_DATE);
  gp_cod_time(&dir->dir[COD_DIR_TIME],
              COD_DIR_VERSION - COD_DIR_TIME);
  gp_cod_strncpy(&dir->dir[COD_DIR_VERSION],
                 VERSION, COD_DIR_COMPILER - COD_DIR_VERSION);
  gp_cod_strncpy(&dir->dir[COD_DIR_COMPILER],
                 "gplink", COD_DIR_NOTICE - COD_DIR_COMPILER);
  gp_cod_strncpy(&dir->dir[COD_DIR_NOTICE],
                 GPUTILS_COPYRIGHT_STRING,
                 COD_DIR_SYMTAB - COD_DIR_NOTICE);

  /* The address is always two shorts or 4 bytes long. */
  dir->dir[COD_DIR_ADDRSIZE] = 0;

  return dir;
}

/* Assign each file name unique file number. A file may appear in the
   symbol table more than once. */
static void
assign_file_id(void)
{
  struct symbol_table *file_table;
  gp_symbol_type *symbol;
  gp_aux_type *aux;
  struct symbol *s;
  int file_id = 0;
  int *value;

  /* build a case sensitive file table */
  file_table = push_symbol_table(NULL, false);

  symbol = state.object->symbols;
  while (symbol != NULL) {
    if (symbol->class == C_FILE) {
      aux = symbol->aux_list;
      assert(aux != NULL);
      s = get_symbol(file_table, aux->_aux_symbol._aux_file.filename);

      if (s != NULL) {
        /* fetch the file number */
        value = get_symbol_annotation(s);
      } else {
        /* the file hasn't been assigned a value */
        value = GP_Malloc(sizeof(int));
        *value = file_id++;
        s = add_symbol(file_table, aux->_aux_symbol._aux_file.filename);
        annotate_symbol(s, value);
      }
      symbol->number = *value;
    }
    symbol = symbol->next;
  }

  /* destory the table */
  file_table = pop_symbol_table(file_table);
}

/*
 * init_cod - initialize the cod file
 */
void
cod_init(void)
{
  if (state.codfile != OUT_NAMED) {
    snprintf(state.codfilename, sizeof (state.codfilename),
             "%s.cod", state.basefilename);
  }

  if ((gp_num_errors > 0) || (state.codfile == OUT_SUPPRESS)) {
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

  assign_file_id();
}

/*
 * write_file_block - Write a code block that contains a list of the
 * source files.
 */
static void
write_file_block(void)
{
  gp_symbol_type *symbol;
  BlockList *fb = NULL;
  int file_id = 0;

  symbol = state.object->symbols;
  while (symbol != NULL) {
    if ((fb == NULL) || (main_dir->src.offset >= (FILES_PER_BLOCK * FILE_SIZE))) {
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

      gp_cod_strncpy(&fb->block[main_dir->src.offset + 1],
                     symbol->aux_list->_aux_symbol._aux_file.filename,
                     FILE_SIZE - 1);

      main_dir->src.offset += FILE_SIZE;
    }

    symbol = symbol->next;
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
  unsigned char smod_flag;
  BlockList *lb;
  gp_boolean first_time;
  int address, high_address;
  static DirBlockInfo *dbi = NULL;
  static int _64k_base = 0;

  if (!state.cod.enabled) {
    return;
  }

  address = gp_processor_byte_to_org(state.class, state.lst.was_org);
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

  assert(state.lst.src != NULL);
  assert(state.lst.src->symbol != NULL);
  lb->block[dbi->lst.offset + COD_LS_SFILE] = state.lst.src->symbol->number;

  smod_flag = (first_time) ? 0xff : ((state.cod.emitting != 0) ? 0x80 : 0x90);

  lb->block[dbi->lst.offset + COD_LS_SMOD] = smod_flag;

  /* Write the source file line number corresponding to the list file line
     number */
  gp_putl16(&lb->block[dbi->lst.offset + COD_LS_SLINE],
            state.lst.src->line_number);

  /* Write the address of the opcode. */
  gp_putl16(&lb->block[dbi->lst.offset + COD_LS_SLOC],
            gp_processor_byte_to_org(state.class, state.lst.was_org));

  dbi->lst.offset += COD_LINE_SYM_SIZE;
}

/* cod_write_symbols - write the symbol table to the .cod file
 *
 * This routine will read the symbol table that gplink has created
 * and convert it into a format suitable for .cod files. So far, only
 * three variable types are supported: address, register, and constants.
 *
 */
void
cod_write_symbols(struct symbol **symbol_list, int num_symbols)
{
  int i, len, type;
  gp_coffsymbol_type *var;
  const char *s;
  BlockList *sb = NULL;

  if (!state.cod.enabled) {
    return;
  }

  for (i = 0; i < num_symbols; i++) {
    var = get_symbol_annotation(symbol_list[i]);
    s = get_symbol_name(symbol_list[i]);
    len = strlen(s);

    /* If this symbol extends past the end of the cod block
     * then write this block out */

    if ((sb == NULL) || ((main_dir->sym.offset + len + COD_SYM_EXTRA) >= COD_BLOCK_SIZE)) {
      sb = gp_blocks_append(&main_dir->sym, gp_blocks_new());
    }

    gp_cod_strncpy(&sb->block[main_dir->sym.offset + 1], s, MAX_SYM_LEN);

    assert(var->symbol != NULL);
    assert(var->symbol->section != NULL);

    if (var->symbol->section->flags & STYP_TEXT) {
      type = COD_ST_ADDRESS;
    } else if (var->symbol->section->flags & STYP_DATA) {
      type = COD_ST_C_SHORT;
    } else if (var->symbol->section->flags & STYP_BSS) {
      type = COD_ST_C_SHORT;
    } else {
      type = COD_ST_CONSTANT;
    }

    gp_putl16(&sb->block[main_dir->sym.offset + len + COD_SYM_TYPE], type);

    /* write 32 bits, big endian */
    gp_putb32(&sb->block[main_dir->sym.offset + len + COD_SYM_VALUE], var->symbol->value);

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

  if (dbi->cod_image_blocks[block_index].block == NULL)
    gp_cod_create(&dbi->cod_image_blocks[block_index]);

  gp_putl16(&dbi->cod_image_blocks[block_index].block[address & (COD_BLOCK_SIZE - 1)], opcode);
}

/* cod_write_code - write all of the assembled pic code to the .cod file
 */
static void
cod_write_code(void)
{
  MemBlock *m = state.i_memory;
  int mem_base, i;
  int start_address = 0, used_flag = 0;
  BlockList *rb = NULL;
  static DirBlockInfo *dbi = NULL;
  int _64k_base = 0;

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
          state.class->i_memory_get(state.i_memory, i, &insn, NULL, NULL)) {
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

/* cod_write_debug - write debug symbols to the .cod file
 */
static void
cod_write_debug(void)
{
  int len;
  gp_symbol_type *symbol;
  gp_aux_type *aux;
  BlockList *db = NULL;
  char command;
  char *string;

  if (!state.cod.enabled) {
    return;
  }

  symbol = state.object->symbols;
  while (symbol != NULL) {
    if (strcasecmp(".direct", symbol->name) == 0) {
      assert(symbol->num_auxsym == 1);
      aux = symbol->aux_list;
      assert(aux != NULL);

      command = aux->_aux_symbol._aux_direct.command;
      string = aux->_aux_symbol._aux_direct.string;

      len = strlen(string);

      /* If this message extends past the end of the cod block
       * then write this block out */

      if ((db == NULL) || ((main_dir->dbg.offset + len + COD_DEBUG_EXTRA) >= COD_BLOCK_SIZE)) {
        db = gp_blocks_append(&main_dir->dbg, gp_blocks_new());
      }

      /* write 32 bits, big endian */
      gp_putb32(&db->block[main_dir->dbg.offset + COD_DEBUG_ADDR], symbol->value);

      db->block[main_dir->dbg.offset + COD_DEBUG_CMD] = command;
      gp_cod_strncpy(&db->block[main_dir->dbg.offset + COD_DEBUG_MSG], string,
                     MAX_STRING_LEN);

      main_dir->dbg.offset += len + COD_DEBUG_EXTRA;
    }
    symbol = symbol->next;
  }
}

static void
enumerate_blocks(DirBlockInfo *dir, int offset, Blocks *bl, unsigned int *block_num)
{
  if (bl->blocks != NULL) {
    /* enumerate block list */
    gp_putl16(&dir->dir[offset], ++(*block_num));
    *block_num += gp_blocks_count(bl) - 1;
    gp_putl16(&dir->dir[offset + 2], *block_num);
  }
}

static void
enumerate_directory(void)
{
  DirBlockInfo *dbi;
  unsigned int block_num = 0;

  /* enumerate directory blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_putl16(&dbi->dir[COD_DIR_NEXTDIR], (dbi->next != NULL) ? ++block_num : 0);
  }

  /* enumerate code blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    int i;

    for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
      if (dbi->cod_image_blocks[i].block != NULL) {
        gp_putl16(&dbi->dir[i * 2], ++block_num);
      }
    }
  }

  /* enumerate surce files blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    enumerate_blocks(dbi, COD_DIR_NAMTAB, &dbi->src, &block_num);
  }

  /* enumerate list lines blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    enumerate_blocks(dbi, COD_DIR_LSTTAB, &dbi->lst, &block_num);
  }

  /* enumerate memory map blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    enumerate_blocks(dbi, COD_DIR_MEMMAP, &dbi->rng, &block_num);
  }

  /* enumerate long symbol table blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    enumerate_blocks(dbi, COD_DIR_LSYMTAB, &dbi->sym, &block_num);
  }

  /* enumerate debug messages table blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    enumerate_blocks(dbi, COD_DIR_MESSTAB, &dbi->dbg, &block_num);
  }
}

static void
write_blocks(Blocks *bl)
{
  BlockList *p = bl->blocks;

  /* write block list */
  while (p != NULL) {
    fwrite(p->block, 1, COD_BLOCK_SIZE, state.cod.f);
    p = p->next;
  }
}

static void
write_directory(void)
{
  DirBlockInfo *dbi;

  /* write directory blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    fwrite(dbi->dir, 1, COD_BLOCK_SIZE, state.cod.f);
  }

  /* write code blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    int i;

    for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
      if (dbi->cod_image_blocks[i].block != NULL) {
        fwrite(dbi->cod_image_blocks[i].block, 1, COD_BLOCK_SIZE, state.cod.f);
      }
    }
  }

  /* write surce files blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    write_blocks(&dbi->src);
  }

  /* write list lines blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    write_blocks(&dbi->lst);
  }

  /* write memory map blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    write_blocks(&dbi->rng);
  }

  /* write long symbol table blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    write_blocks(&dbi->sym);
  }

  /* write debug messages table blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    write_blocks(&dbi->dbg);
  }
}

static void
free_blocks(Blocks *bl)
{
  BlockList *b = bl->blocks;

  while (b != NULL) {
    BlockList *next = b->next;

    free(b);
    b = next;
  }
}

static void
free_directory(void)
{
  DirBlockInfo *dbi = main_dir;

  while (dbi != NULL) {
    int i;
    DirBlockInfo *next;

    /* free code blocks */
    for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
      if (dbi->cod_image_blocks[i].block != NULL) {
        free(dbi->cod_image_blocks[i].block);
      }
    }

    /* free surce files blocks */
    free_blocks(&dbi->src);

    /* free list lines blocks */
    free_blocks(&dbi->lst);

    /* free memory map blocks */
    free_blocks(&dbi->rng);

    /* free long symbol table blocks */
    free_blocks(&dbi->sym);

    /* free debug messages table blocks */
    free_blocks(&dbi->dbg);

    next = dbi->next;
    /* free directory blocks */
    free(dbi);
    dbi = next;
  }

  main_dir = NULL;
}

static void
cod_symbol_table(struct symbol_table *table)
{
  int i;
  struct symbol **sym, **ps, *s;

  ps = sym = GP_Malloc(table->count * sizeof(sym[0]));

  for (i = 0; i < HASH_SIZE; i++) {
    for (s = table->hash_table[i]; s != NULL; s = s->next) {
      *ps++ = s;
    }
  }

  assert(ps == &sym[table->count]);

  qsort(sym, table->count, sizeof (sym[0]), symbol_compare);

  cod_write_symbols(sym, table->count);
}

void
cod_close_file(void)
{
  if (!state.cod.enabled) {
    return;
  }

  /* processor is unknown if not defined in command line at cod_init() call
     so it should be set here */
  gp_cod_strncpy(&main_dir->dir[COD_DIR_PROCESSOR],
                 gp_processor_name(state.processor, 2),
                 COD_DIR_LSYMTAB - COD_DIR_PROCESSOR);

  /* All the global symbols are written.  Need to figure out what to do about
     the local symbols. */
  cod_symbol_table(state.symbol.definition);

  write_file_block();

  cod_write_code();

  cod_write_debug();

  enumerate_directory();
  write_directory();
  free_directory();
  fclose(state.cod.f);
}

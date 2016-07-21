/* .cod file support
   Copyright (C) 2003, 2004, 2005 Craig Franklin
   Copyright (C) 2012 Borut Razem
   Copyright (C) 2015-2016 Molnar Karoly

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

/*------------------------------------------------------------------------------------------------*/

static void
_cod_time(uint8_t *Buffer, size_t Sizeof_buffer)
{
  time_t        now;
  struct tm    *local;
  unsigned int  value;

  time(&now);
  local = localtime(&now);
  value = (local->tm_hour * 100) + local->tm_min;
  gp_putl16(Buffer, value);
  /* No space for the seconds. */
}

/*------------------------------------------------------------------------------------------------*/

static void
_cod_Pdate(uint8_t *Pascal_str, size_t Pascal_max_size)
{
  time_t now;
  char   temp[16];
  size_t length;

  time(&now);
#ifdef HAVE_LOCALE_H
  setlocale(LC_ALL, "C");
#endif
  length = strftime(temp, sizeof(temp), "%d%b%g", localtime(&now));
  assert(length < Pascal_max_size);

  *Pascal_str = (uint8_t)length;
  ++Pascal_str;
  memcpy(Pascal_str, temp, length);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cod_create(Block *B)
{
  assert(B != NULL);

  B->block = GP_Calloc(1, COD_BLOCK_SIZE);
}

/*------------------------------------------------------------------------------------------------*/

DirBlockInfo *
gp_cod_new_dir_block(void)
{
  DirBlockInfo *dir;

  /* initialize eveything to zero */
  dir = GP_Calloc(1, sizeof(DirBlockInfo));
  gp_putl16(&dir->dir[COD_DIR_CODTYPE], 1);
  return dir;
}

/*------------------------------------------------------------------------------------------------*/

DirBlockInfo *
gp_cod_init_dir_block(const char *File_name, const char *Compiler)
{
  DirBlockInfo *dir;
  uint8_t      *block;

  dir   = gp_cod_new_dir_block();
  block = dir->dir;
  /* Initialize the directory block with known data. It'll be written
   * to the .cod file after everything else. */
  gp_Pstr_from_str(&block[COD_DIR_SOURCE], COD_DIR_SOURCE_P_SIZE, File_name);
  _cod_Pdate(&block[COD_DIR_DATE], COD_DIR_DATE_P_SIZE);
  _cod_time(&block[COD_DIR_TIME], COD_DIR_TIME_SIZE);
  gp_Pstr_from_str(&block[COD_DIR_VERSION], COD_DIR_VERSION_P_SIZE, VERSION);
  gp_Pstr_from_str(&block[COD_DIR_COMPILER], COD_DIR_COMPILER_P_SIZE, Compiler);
  gp_Pstr_from_str(&block[COD_DIR_NOTICE], COD_DIR_NOTICE_P_SIZE, GPUTILS_COPYRIGHT_STRING);

  /* The address is always two shorts or 4 bytes long. */
  block[COD_DIR_ADDRSIZE] = 0;

  return dir;
}

/*------------------------------------------------------------------------------------------------*/

DirBlockInfo *
gp_cod_find_dir_block_by_high_addr(DirBlockInfo *Main, unsigned int High_addr)
{
  DirBlockInfo *dbi;

  dbi = Main;
  /* find the directory containing high_addr 64k segment */
  while (gp_getl16(&dbi->dir[COD_DIR_HIGHADDR]) != High_addr) {
    /* If the next directory block (in the linked list of directory
       blocks) is NULL, then this is the first time to encounter this
       _64k segment. So we need to create a new segment. */
    if (dbi->next == NULL) {
      dbi->next = gp_cod_new_dir_block();
      gp_putl16(&dbi->next->dir[COD_DIR_HIGHADDR], High_addr);
      dbi = dbi->next;
      break;
    }
    else {
      dbi = dbi->next;
    }
  }

  return dbi;
}

/*------------------------------------------------------------------------------------------------*/

/* gp_cod_emit_opcode - write one opcode to a cod_image_block */

void
gp_cod_emit_opcode(DirBlockInfo *Dbi, unsigned int Address, unsigned int Opcode)
{
  unsigned int block_index;

  /* The code image blocks are handled in a different manner than the
   * other cod blocks. In theory, it's possible to emit opcodes in a
   * non-sequential manner. Furthermore, it's possible that there may
   * be gaps in the program memory. These cases are handled by an array
   * of code blocks. The lower 8 bits of the opcode's address form an
   * index into the code block, while bits 9-15 are an index into the
   * array of code blocks. The code image blocks are not written until
   * all of the opcodes have been emitted.
   */

  block_index = (Address >> COD_BLOCK_BITS) & (COD_CODE_IMAGE_BLOCKS - 1);

  if (Dbi->cod_image_blocks[block_index].block == NULL) {
    gp_cod_create(&Dbi->cod_image_blocks[block_index]);
  }

  gp_putl16(&Dbi->cod_image_blocks[block_index].block[Address & (COD_BLOCK_SIZE - 1)], Opcode);
}

/*------------------------------------------------------------------------------------------------*/

/* gp_cod_write_code - write all of the assembled pic code to the .cod file */

void
gp_cod_write_code(proc_class_t Class, const MemBlock_t *Mem, DirBlockInfo *Main)
{
  DirBlockInfo     *dbi;
  const MemBlock_t *m;
  int               i;
  int               mem_base;
  int               high_addr;
  int               start_address;
  gp_boolean        used_flag;
  BlockList        *rb;
  int               _64k_base;
  uint16_t          insn;
  uint8_t          *record;

  start_address = 0;
  used_flag     = false;
  dbi           = NULL;
  _64k_base     = 0;
  m             = Mem;

  while (m != NULL) {
    mem_base  = IMemAddrFromBase(m->base);
    high_addr = IMemBaseFromAddr(mem_base);

    if ((dbi == NULL) || (high_addr != _64k_base)) {
      _64k_base = high_addr;
      dbi       = gp_cod_find_dir_block_by_high_addr(Main, _64k_base);
    }

    for (i = mem_base; (i - mem_base) <= I_MEM_MAX; i += 2) {
      if (((i - mem_base) < I_MEM_MAX) &&
          (Class->i_memory_get(Mem, i, &insn, NULL, NULL) == W_USED_ALL)) {
        gp_cod_emit_opcode(dbi, i, insn);

        if (!used_flag) {
          /* Save the start address in a range of opcodes */
          start_address = i;
          used_flag     = true;
        }
      }
      else {
        /* No code at address i, but we need to check if this is the
           first empty address after a range of address. */
        if (used_flag) {
          rb = gp_cod_block_get_last_or_new(&dbi->range);

          if ((rb == NULL) || ((dbi->range.offset + COD_MAPENTRY_SIZE) >= COD_BLOCK_SIZE)) {
            /* If there are a whole bunch of non-contiguous pieces of
               code then we'll get here. But most pic apps will only need
               one directory block (that will give you 64 ranges or non-
               contiguous chunks of pic code). */
            rb = gp_cod_block_append(&dbi->range, gp_cod_block_new());
          }
          /* We need to update dir map indicating a range of memory that is needed.
             This is done by writing the start and end address to the directory map. */
          record = &rb->block[dbi->range.offset];
          gp_putl16(&record[COD_MAPTAB_START], start_address);
          gp_putl16(&record[COD_MAPTAB_LAST], i - 1);

          used_flag = false;

          dbi->range.offset += COD_MAPENTRY_SIZE;
        }
      }
    }

    m = m->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

size_t
gp_cod_put_long_symbol(uint8_t *Record, const char *Name, gp_symvalue_t Value, unsigned int Type)
{
  size_t length;

  length = gp_Pstr_from_str(&Record[COD_LSYMBOL_NAME], COD_LSYMBOL_PSTRING_MAX_LEN, Name);
  gp_putl16(&Record[length + COD_LSYMBOL_TYPE], Type);
  /* write 32 bits, big endian */
  gp_putb32(&Record[length + COD_LSYMBOL_VALUE], Value);
  return length;
}

/*------------------------------------------------------------------------------------------------*/

size_t
gp_cod_put_debug_symbol(uint8_t *Record, const char *String, gp_symvalue_t Value, char Command)
{
  /* write 32 bits, big endian */
  gp_putb32(&Record[COD_DEBUG_ADDR], Value);
  Record[COD_DEBUG_CMD] = Command;
  return gp_Pstr_from_str(&Record[COD_DEBUG_MSG], COD_DEBUG_PSTRING_MAX_LEN, String);
}

/*------------------------------------------------------------------------------------------------*/

BlockList *
gp_cod_block_new(void)
{
  return (BlockList *)GP_Calloc(1, sizeof(BlockList));
}

/*------------------------------------------------------------------------------------------------*/

BlockList *
gp_cod_block_append(Blocks *Bl, BlockList *B)
{
  if (Bl->first == NULL) {
    Bl->first = B;
    Bl->count = 1;
  }
  else {
    Bl->last->next = B;
    (Bl->count)++;
  }

  Bl->last   = B;
  Bl->offset = 0;
  return B;
}

/*------------------------------------------------------------------------------------------------*/

BlockList *
gp_cod_block_get_last(Blocks *Bl)
{
  return Bl->last;
}

/*------------------------------------------------------------------------------------------------*/

BlockList *
gp_cod_block_get_last_or_new(Blocks *Bl)
{
  if (Bl->first != NULL) {
    return gp_cod_block_get_last(Bl);
  }
  else {
    Bl->first  = gp_cod_block_new();
    Bl->last   = Bl->first;
    Bl->offset = 0;
    Bl->count  = 1;
    return Bl->first;
  }
}

/*------------------------------------------------------------------------------------------------*/

int
gp_cod_block_count(const Blocks *Bl)
{
  return ((Bl->first == NULL) ? 0 : Bl->count);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cod_block_enumerate(DirBlockInfo *Dir, unsigned int Offset, Blocks *Bl, unsigned int *Block_num)
{
  if (Bl->first != NULL) {
    /* enumerate block list */
    gp_putl16(&Dir->dir[Offset], ++(*Block_num));
    *Block_num += gp_cod_block_count(Bl) - 1;
    gp_putl16(&Dir->dir[Offset + 2], *Block_num);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cod_enumerate_directory(DirBlockInfo *Main_dir)
{
  DirBlockInfo *dbi;
  unsigned int  block_num;
  unsigned int  i;

  block_num = 0;

  /* enumerate directory blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_putl16(&dbi->dir[COD_DIR_NEXTDIR], (dbi->next != NULL) ? ++block_num : 0);
  }

  /* enumerate code blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
      if (dbi->cod_image_blocks[i].block != NULL) {
        gp_putl16(&dbi->dir[i * 2], ++block_num);
      }
    }
  }

  /* enumerate surce files blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_cod_block_enumerate(dbi, COD_DIR_NAMTAB, &dbi->file, &block_num);
  }

  /* enumerate list lines blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_cod_block_enumerate(dbi, COD_DIR_LSTTAB, &dbi->list, &block_num);
  }

  /* enumerate memory map blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_cod_block_enumerate(dbi, COD_DIR_MEMMAP, &dbi->range, &block_num);
  }

  /* enumerate long symbol table blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_cod_block_enumerate(dbi, COD_DIR_LSYMTAB, &dbi->lsym, &block_num);
  }

  /* enumerate debug messages table blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_cod_block_enumerate(dbi, COD_DIR_MESSTAB, &dbi->debug, &block_num);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cod_block_write(FILE *F, const Blocks *Bl)
{
  const BlockList *curr;

  curr = Bl->first;
  /* write block list */
  while (curr != NULL) {
    if (fwrite(curr->block, 1, COD_BLOCK_SIZE, F) != COD_BLOCK_SIZE) {
      fprintf(stderr, "%s() -- Could not write cod file.\n", __func__);
      exit(1);
    }
    curr = curr->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cod_write_directory(FILE *F, const DirBlockInfo *Main_dir)
{
  const DirBlockInfo *dbi;
  unsigned int        i;

  /* write directory blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    if (fwrite(dbi->dir, 1, COD_BLOCK_SIZE, F) != COD_BLOCK_SIZE) {
      fprintf(stderr, "%s() -- Could not write cod file.\n", __func__);
      exit(1);
    }
  }

  /* write code blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
      if (dbi->cod_image_blocks[i].block != NULL) {
        if (fwrite(dbi->cod_image_blocks[i].block, 1, COD_BLOCK_SIZE, F) != COD_BLOCK_SIZE) {
          fprintf(stderr, "%s() -- Could not write cod file.\n", __func__);
          exit(1);
        }
      }
    }
  }

  /* write source files blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_cod_block_write(F, &dbi->file);
  }

  /* write list lines blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_cod_block_write(F, &dbi->list);
  }

  /* write memory map blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_cod_block_write(F, &dbi->range);
  }

  /* write long symbol table blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_cod_block_write(F, &dbi->lsym);
  }

  /* write debug messages table blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_cod_block_write(F, &dbi->debug);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cod_block_free(Blocks *Bl)
{
  BlockList *curr;
  BlockList *next;

  curr = Bl->first;
  while (curr != NULL) {
    next = curr->next;
    free(curr);
    curr = next;
  }

  Bl->first  = NULL;
  Bl->last   = NULL;
  Bl->offset = 0;
  Bl->count  = 0;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cod_free_directory(DirBlockInfo *Main_dir)
{
  DirBlockInfo *dbi;
  DirBlockInfo *next;
  unsigned int  i;

  dbi = Main_dir;
  while (dbi != NULL) {
    /* free code blocks */
    for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
      if (dbi->cod_image_blocks[i].block != NULL) {
        free(dbi->cod_image_blocks[i].block);
      }
    }

    /* free surce files blocks */
    gp_cod_block_free(&dbi->file);

    /* free list lines blocks */
    gp_cod_block_free(&dbi->list);

    /* free memory map blocks */
    gp_cod_block_free(&dbi->range);

    /* free long symbol table blocks */
    gp_cod_block_free(&dbi->lsym);

    /* free debug messages table blocks */
    gp_cod_block_free(&dbi->debug);

    next = dbi->next;
    /* free directory blocks */
    free(dbi);
    dbi = next;
  }
}

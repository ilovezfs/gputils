/* .cod file support
   Copyright (C) 2003, 2004, 2005 Craig Franklin
   Copyright (C) 2012 Borut Razem
   Copyright (C) 2015 Molnar Karoly

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

/* copy a string to a cod block using the pascal convention, i.e. the 
   string length occupies the first string location */

void 
gp_cod_strncpy(uint8_t *Dest, const char *Src, int Max_len)
{
  int len;

  len = strlen(Src);
  Dest[-1] = (Max_len > len) ? len : Max_len;
  memcpy(Dest, Src, Dest[-1]);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cod_date(uint8_t *Buffer, size_t Sizeof_buffer)
{
  char       temp[32];
  time_t     now;
  struct tm *now_tm;

  static const char mon_name[12][4] = {
    "Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0", "Jun\0",
    "Jul\0", "Aug\0", "Sep\0", "Oct\0", "Nov\0", "Dec\0"
  };

  time(&now);
  now_tm = localtime(&now);
  snprintf(temp, sizeof(temp), "%02d%3s%02d", now_tm->tm_mday, mon_name[now_tm->tm_mon],
           now_tm->tm_year % 100);

  memcpy(Buffer, temp, Sizeof_buffer);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cod_time(uint8_t *Buffer, size_t Sizeof_buffer)
{
  time_t     now;
  struct tm *now_tm;
  int        value;

  time(&now);
  now_tm = localtime(&now);

  value = (now_tm->tm_hour * 100) + now_tm->tm_min;

  Buffer[0] = value & 0xff;
  Buffer[1] = (value >> 8) & 0xff;
  Buffer[2] = now_tm->tm_sec & 0xff;
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
  DirBlockInfo *dir = gp_cod_new_dir_block();

  /* Initialize the directory block with known data. It'll be written
   * to the .cod file after everything else. */
  gp_cod_strncpy(&dir->dir[COD_DIR_SOURCE], File_name, COD_DIR_DATE - COD_DIR_SOURCE);
  gp_cod_date(&dir->dir[COD_DIR_DATE], COD_DIR_TIME - COD_DIR_DATE);
  gp_cod_time(&dir->dir[COD_DIR_TIME], COD_DIR_VERSION - COD_DIR_TIME);
  gp_cod_strncpy(&dir->dir[COD_DIR_VERSION], VERSION, COD_DIR_COMPILER - COD_DIR_VERSION);
  gp_cod_strncpy(&dir->dir[COD_DIR_COMPILER], Compiler, COD_DIR_NOTICE - COD_DIR_COMPILER);
  gp_cod_strncpy(&dir->dir[COD_DIR_NOTICE], GPUTILS_COPYRIGHT_STRING, COD_DIR_SYMTAB - COD_DIR_NOTICE);

  /* The address is always two shorts or 4 bytes long. */
  dir->dir[COD_DIR_ADDRSIZE] = 0;

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

          used_flag = false;

          dbi->rng.offset += COD_MAPENTRY_SIZE;
        }
      }
    }

    m = m->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

BlockList *
gp_blocks_new(void)
{
  return (BlockList *)GP_Calloc(1, sizeof(BlockList));
}

/*------------------------------------------------------------------------------------------------*/

BlockList *
gp_blocks_append(Blocks *Bl, BlockList *B)
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
gp_blocks_get_last(Blocks *Bl)
{
  return Bl->last;
}

/*------------------------------------------------------------------------------------------------*/

BlockList *
gp_blocks_get_last_or_new(Blocks *Bl)
{
  if (Bl->first != NULL) {
    return gp_blocks_get_last(Bl);
  }
  else {
    Bl->first  = gp_blocks_new();
    Bl->last   = Bl->first;
    Bl->offset = 0;
    Bl->count  = 1;
    return Bl->first;
  }
}

/*------------------------------------------------------------------------------------------------*/

int
gp_blocks_count(const Blocks *Bl)
{
  return ((Bl->first == NULL) ? 0 : Bl->count);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_blocks_enumerate(DirBlockInfo *Dir, unsigned int Offset, Blocks *Bl, unsigned int *Block_num)
{
  if (Bl->first != NULL) {
    /* enumerate block list */
    gp_putl16(&Dir->dir[Offset], ++(*Block_num));
    *Block_num += gp_blocks_count(Bl) - 1;
    gp_putl16(&Dir->dir[Offset + 2], *Block_num);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_blocks_enumerate_directory(DirBlockInfo *Main_dir)
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
    gp_blocks_enumerate(dbi, COD_DIR_NAMTAB, &dbi->src, &block_num);
  }

  /* enumerate list lines blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_enumerate(dbi, COD_DIR_LSTTAB, &dbi->lst, &block_num);
  }

  /* enumerate memory map blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_enumerate(dbi, COD_DIR_MEMMAP, &dbi->rng, &block_num);
  }

  /* enumerate long symbol table blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_enumerate(dbi, COD_DIR_LSYMTAB, &dbi->sym, &block_num);
  }

  /* enumerate debug messages table blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_enumerate(dbi, COD_DIR_MESSTAB, &dbi->dbg, &block_num);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_blocks_write(FILE *F, Blocks *Bl)
{
  BlockList *curr = Bl->first;

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
gp_blocks_write_directory(FILE *F, DirBlockInfo *Main_dir)
{
  DirBlockInfo *dbi;
  unsigned int  i;

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
    gp_blocks_write(F, &dbi->src);
  }

  /* write list lines blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_write(F, &dbi->lst);
  }

  /* write memory map blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_write(F, &dbi->rng);
  }

  /* write long symbol table blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_write(F, &dbi->sym);
  }

  /* write debug messages table blocks */
  for (dbi = Main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_write(F, &dbi->dbg);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_blocks_free(Blocks *Bl)
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
gp_blocks_free_directory(DirBlockInfo *Main_dir)
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
    gp_blocks_free(&dbi->src);

    /* free list lines blocks */
    gp_blocks_free(&dbi->lst);

    /* free memory map blocks */
    gp_blocks_free(&dbi->rng);

    /* free long symbol table blocks */
    gp_blocks_free(&dbi->sym);

    /* free debug messages table blocks */
    gp_blocks_free(&dbi->dbg);

    next = dbi->next;
    /* free directory blocks */
    free(dbi);
    dbi = next;
  }
}

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

/* copy a string to a cod block using the pascal convention, i.e. the 
   string length occupies the first string location */

void 
gp_cod_strncpy(unsigned char *dest, const char *src, int max_len)
{
  int len = strlen(src);

  dest[-1] = (max_len > len) ? len : max_len;
  memcpy(dest, src, dest[-1]);
}

void
gp_cod_date(unsigned char *buffer, size_t sizeof_buffer)
{
  #define TEMP_SIZE 32
  char temp[TEMP_SIZE];
  time_t now;
  struct tm *now_tm;

  static const char mon_name[12][4] = {
    "Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0", "Jun\0",
    "Jul\0", "Aug\0", "Sep\0", "Oct\0", "Nov\0", "Dec\0"
  };

  time(&now);
  now_tm = localtime(&now);
  snprintf(temp, TEMP_SIZE,
           "%02d%3s%02d",
           now_tm->tm_mday,
           &mon_name[now_tm->tm_mon][0],
           now_tm->tm_year % 100);

  memcpy(buffer, temp, sizeof_buffer);
}

void
gp_cod_time(unsigned char *buffer, size_t sizeof_buffer)
{
  time_t now;
  struct tm *now_tm;
  int value;

  time(&now);
  now_tm = localtime(&now);

  value = (now_tm->tm_hour * 100) + now_tm->tm_min;
  
  buffer[0] = value & 0xff;
  buffer[1] = (value >> 8) & 0xff;
  buffer[2] = now_tm->tm_sec & 0xff;
}

/*********************************************************************/
void
gp_cod_create(Block *b)
{
  assert(b != NULL);

  b->block = GP_Calloc(1, COD_BLOCK_SIZE);
}

BlockList *
gp_blocks_new(void)
{
  return GP_Calloc(1, sizeof(BlockList));
}

BlockList *
gp_blocks_append(Blocks *bl, BlockList *b)
{
  if (bl->first == NULL) {
    bl->first = b;
    bl->count = 1;
  } else {
    bl->last->next = b;
    (bl->count)++;
  }

  bl->last   = b;
  bl->offset = 0;
  return b;
}

BlockList *
gp_blocks_get_last(Blocks *bl)
{
  return bl->last;
}

BlockList *
gp_blocks_get_last_or_new(Blocks *bl)
{
  if (bl->first != NULL) {
    return gp_blocks_get_last(bl);
  }
  else {
    bl->first  = gp_blocks_new();
    bl->last   = bl->first;
    bl->offset = 0;
    bl->count  = 1;
    return bl->first;
  }
}

int
gp_blocks_count(const Blocks *bl)
{
  return ((bl->first == NULL) ? 0 : bl->count);
}

void
gp_blocks_enumerate(DirBlockInfo *dir, unsigned int offset, Blocks *bl, unsigned int *block_num)
{
  if (bl->first != NULL) {
    /* enumerate block list */
    gp_putl16(&dir->dir[offset], ++(*block_num));
    *block_num += gp_blocks_count(bl) - 1;
    gp_putl16(&dir->dir[offset + 2], *block_num);
  }
}

void
gp_blocks_enumerate_directory(DirBlockInfo *main_dir)
{
  DirBlockInfo *dbi;
  unsigned int block_num = 0;
  unsigned int i;

  /* enumerate directory blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_putl16(&dbi->dir[COD_DIR_NEXTDIR], (dbi->next != NULL) ? ++block_num : 0);
  }

  /* enumerate code blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
      if (dbi->cod_image_blocks[i].block != NULL) {
        gp_putl16(&dbi->dir[i * 2], ++block_num);
      }
    }
  }

  /* enumerate surce files blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_enumerate(dbi, COD_DIR_NAMTAB, &dbi->src, &block_num);
  }

  /* enumerate list lines blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_enumerate(dbi, COD_DIR_LSTTAB, &dbi->lst, &block_num);
  }

  /* enumerate memory map blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_enumerate(dbi, COD_DIR_MEMMAP, &dbi->rng, &block_num);
  }

  /* enumerate long symbol table blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_enumerate(dbi, COD_DIR_LSYMTAB, &dbi->sym, &block_num);
  }

  /* enumerate debug messages table blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_enumerate(dbi, COD_DIR_MESSTAB, &dbi->dbg, &block_num);
  }
}

void
gp_blocks_write(FILE *f, Blocks *bl)
{
  BlockList *p = bl->first;

  /* write block list */
  while (p != NULL) {
    if (fwrite(p->block, 1, COD_BLOCK_SIZE, f) != COD_BLOCK_SIZE) {
      fprintf(stderr, "%s() -- Could not write cod file.\n", __func__);
      exit(1);
    }
    p = p->next;
  }
}

void
gp_blocks_write_directory(FILE *f, DirBlockInfo *main_dir)
{
  DirBlockInfo *dbi;
  unsigned int i;

  /* write directory blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    if (fwrite(dbi->dir, 1, COD_BLOCK_SIZE, f) != COD_BLOCK_SIZE) {
      fprintf(stderr, "%s() -- Could not write cod file.\n", __func__);
      exit(1);
    }
  }

  /* write code blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
      if (dbi->cod_image_blocks[i].block != NULL) {
        if (fwrite(dbi->cod_image_blocks[i].block, 1, COD_BLOCK_SIZE, f) != COD_BLOCK_SIZE) {
          fprintf(stderr, "%s() -- Could not write cod file.\n", __func__);
          exit(1);
        }
      }
    }
  }

  /* write source files blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_write(f, &dbi->src);
  }

  /* write list lines blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_write(f, &dbi->lst);
  }

  /* write memory map blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_write(f, &dbi->rng);
  }

  /* write long symbol table blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_write(f, &dbi->sym);
  }

  /* write debug messages table blocks */
  for (dbi = main_dir; dbi != NULL; dbi = dbi->next) {
    gp_blocks_write(f, &dbi->dbg);
  }
}

void
gp_blocks_free(Blocks *bl)
{
  BlockList *b = bl->first;

  while (b != NULL) {
    BlockList *next = b->next;

    free(b);
    b = next;
  }

  bl->first  = NULL;
  bl->last   = NULL;
  bl->offset = 0;
  bl->count  = 0;
}

void
gp_blocks_free_directory(DirBlockInfo *main_dir)
{
  DirBlockInfo *dbi = main_dir;
  DirBlockInfo *next;
  unsigned int i;

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

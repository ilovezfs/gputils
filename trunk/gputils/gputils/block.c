/* Displays contents of ".COD" files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Scott Dattalo

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
#include "gpvc.h"
#include "dump.h"
#include "block.h"

void
read_block(unsigned char * block, int block_number)
{
  fseek(codefile, block_number * COD_BLOCK_SIZE, SEEK_SET);
  fread(block, COD_BLOCK_SIZE, 1, codefile);
}

/* TODO: should be replaced by gp_cod_create() */
static void
create_block(Block *b)
{
  assert(b != NULL);

  b->block = malloc(COD_BLOCK_SIZE);
  gp_cod_clear(b);
}

DirBlockInfo *
read_directory(void)
{
  DirBlockInfo *dbi = NULL, *start = NULL;
  int next_dir_block = 0;

  do {
    DirBlockInfo *p = (DirBlockInfo *)malloc(sizeof (DirBlockInfo));
    assert(p);
    if (NULL == dbi)
      start = dbi = p;
    else {
      dbi->next_dir_block_info = p;
      dbi = p;
    }
    create_block(&dbi->dir);
    read_block(dbi->dir.block, next_dir_block);
  } while (0 != (next_dir_block = gp_getl16(&dbi->dir.block[COD_DIR_NEXTDIR])));

  dbi->next_dir_block_info = NULL;

  return start;
}

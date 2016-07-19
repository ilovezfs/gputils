/* Displays contents of ".COD" files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Scott Dattalo
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
#include "gpvc.h"
#include "dump.h"
#include "block.h"

/*------------------------------------------------------------------------------------------------*/

void
read_block(FILE *Code_file, uint8_t *Block, unsigned int Block_number)
{
  size_t n;

  fseek(Code_file, Block_number * COD_BLOCK_SIZE, SEEK_SET);
  n = fread(Block, 1, COD_BLOCK_SIZE, Code_file);

  if (n != COD_BLOCK_SIZE) {
    gp_error("bad block number %u", Block_number);
  }
}

/*------------------------------------------------------------------------------------------------*/

DirBlockInfo *
read_directory(FILE *Code_file)
{
  DirBlockInfo *dbi;
  DirBlockInfo *start;
  DirBlockInfo *p;
  uint16_t      next_dir_block;

  dbi            = NULL;
  start          = NULL;
  next_dir_block = 0;
  do {
    p = GP_Malloc(sizeof(DirBlockInfo));

    if (dbi == NULL) {
      start = p;
      dbi   = p;
    }
    else {
      dbi->next = p;
      dbi       = p;
    }

    read_block(Code_file, dbi->dir, next_dir_block);
  } while ((next_dir_block = gp_getl16(&dbi->dir[COD_DIR_NEXTDIR])) != 0);

  dbi->next = NULL;

  return start;
}

/* Displays contents of ".COD" files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Scott Dattalo
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
#include "gpvc.h"
#include "dump.h"
#include "block.h"

/*------------------------------------------------------------------------------------------------*/

void
read_block(uint8_t *block, unsigned int block_number)
{
  size_t n;

  fseek(codefile, block_number * COD_BLOCK_SIZE, SEEK_SET);
  n = fread(block, 1, COD_BLOCK_SIZE, codefile);

  if (n != COD_BLOCK_SIZE) {
    gp_error("bad block number %u", block_number);
  }
}

/*------------------------------------------------------------------------------------------------*/

DirBlockInfo *
read_directory(void)
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

    read_block(dbi->dir, next_dir_block);
  } while ((next_dir_block = gp_getl16(&dbi->dir[COD_DIR_NEXTDIR])) != 0);

  dbi->next = NULL;

  return start;
}

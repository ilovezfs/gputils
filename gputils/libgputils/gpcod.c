/* .cod file support
   Copyright (C) 2003
   Craig Franklin

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
gp_cod_strncpy(char *dest, char *src, int max_len)
{

  *(dest-1) = ( (max_len>strlen(src)) ? strlen(src) : max_len );
  strncpy(dest, src, *(dest-1));

}

/* gp_cod_clear - write zeroes to a code block, unless the code block ptr 
   is null. */

void
gp_cod_clear(Block *b)
{
  int i;

  if(b && b->block)
    for(i=0; i<COD_BLOCK_SIZE; i++)
      b->block[i] = 0;
  else
    assert(0);
}

void
gp_cod_delete(Block *b)
{

  if(b && b->block) {
    free(b->block);
    b->block = NULL;
  }
  else
    assert(0);

}

void
gp_cod_next(Block *b, int *block_number)
{
  assert(b != NULL);
  gp_cod_clear(b);
  b->block_number = *block_number;
  *block_number = *block_number + 1;
}

void
gp_cod_create(Block *b, int *block_number)
{

  assert(b != NULL);

  b->block = malloc(COD_BLOCK_SIZE);
  gp_cod_clear(b);
  b->block_number = *block_number;
  *block_number = *block_number + 1;

}

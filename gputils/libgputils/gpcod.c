/* .cod file support
   Copyright (C) 2003, 2004, 2005 Craig Franklin
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

/* copy a string to a cod block using the pascal convention, i.e. the 
   string length occupies the first string location */

void 
gp_cod_strncpy(unsigned char *dest, const char *src, int max_len)
{
  int len = strlen(src);
  dest[-1] = ( (max_len>len) ? len : max_len );
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

  value = ((now_tm->tm_hour) * 100) + now_tm->tm_min;
  
  buffer[0] = value & 0xff;
  buffer[1] = (value >> 8) & 0xff;
  buffer[2] = now_tm->tm_sec & 0xff;
}

/*********************************************************************/
void
gp_cod_create(Block *b)
{
  assert(b != NULL);

  b->block = malloc(COD_BLOCK_SIZE);
  memset(b->block, 0, COD_BLOCK_SIZE);
}

BlockList *
gp_blocks_new(void)
{
  BlockList *b = malloc(sizeof (BlockList));
  assert(b);
  memset(b, 0, sizeof (BlockList));

  return b;
}

BlockList *
gp_blocks_append(Blocks *bl, BlockList *b)
{
  if (NULL == bl->blocks)
    bl->blocks = b;
  else {
    BlockList *p;
    
    for (p = bl->blocks; p->next; p = p->next)
      ;

    p->next = b;
  }
  bl->offset = 0;

  return b;
}

BlockList *
gp_blocks_get_last(Blocks *bl)
{
  if (NULL == bl->blocks)
    return NULL;
  else {
    BlockList *p;

    for (p = bl->blocks; p->next; p = p->next)
      ;

    return p;
  }
}

BlockList *
gp_blocks_get_last_or_new(Blocks *bl)
{
  if (NULL != bl->blocks)
    return gp_blocks_get_last(bl);
  else {
    bl->offset = 0;
    return bl->blocks = gp_blocks_new();
  }
}

int
gp_blocks_count(Blocks *bl)
{
  if (NULL == bl->blocks)
    return 0;
  else {
    int n;
    BlockList *p;

    for (n = 1, p = bl->blocks; p->next; ++n, p = p->next)
      ;

    return n;
  }
}
/*********************************************************************/

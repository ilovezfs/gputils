/* General system functions
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

/* little endian functions */

void 
gp_fputl16(short data, FILE *fp) 
{
  fputc((int)(data & 255), fp);
  fputc((int)((data >> 8) & 255), fp);

  return;
}

void 
gp_fputl32(long data, FILE *fp) 
{
  fputc((int)(data & 255), fp);
  fputc((int)((data >> 8) & 255), fp);
  fputc((int)((data >> 16) & 255), fp);
  fputc((int)((data >> 24) & 255), fp);

  return;
}

void 
gp_fputvar(char *data, int number, FILE *fp) 
{
  int i;
  
  for(i = 0; i < number; i++)
    fputc(data[i], fp);

  return;
}

short 
gp_getl16(char *addr)
{
  short value;
  
  value  = (unsigned char)addr[0];
  value |= (unsigned char)addr[1] << 8;
  
  return value;
}

unsigned short 
gp_getu16(char *addr)
{
  return (unsigned short) gp_getl16(addr);
}

long 
gp_getl32(char *addr)
{
  long value;

  value  = (unsigned char)addr[0];
  value |= (unsigned char)addr[1] << 8;
  value |= (unsigned char)addr[2] << 16;
  value |= (unsigned char)addr[3] << 24;

  return value;
}

void 
gp_putl16(char *addr, short data)
{
  addr[1] = (data >> 8) & 0xff;
  addr[0] = data & 0xff;

  return;
}

void 
gp_putl32(char *addr, long data)
{
  addr[0] = data & 0xff;
  addr[1] = (data >> 8)  & 0xff;
  addr[2] = (data >> 16) & 0xff;
  addr[3] = (data >> 24) & 0xff;

  return;
}

/* big endian functions */

long 
gp_getb32(char *addr)
{
  long value;

  value  = (unsigned char)addr[0] << 24;
  value |= (unsigned char)addr[1] << 16;
  value |= (unsigned char)addr[2] << 8;
  value |= (unsigned char)addr[3];

  return value;
}

void 
gp_putb32(char *addr, long data)
{
  addr[0] = (data >> 24) & 0xff;
  addr[1] = (data >> 16) & 0xff;
  addr[2] = (data >> 8)  & 0xff;
  addr[3] = data & 0xff;

  return;
}

void
gp_date_string(char *buffer)
{
  time_t now;
  struct tm *now_tm;

  time(&now);
  now_tm = localtime(&now);
  sprintf(buffer,
          "%d-%d-%d  %02d:%02d:%02d",
          now_tm->tm_mon + 1,
          now_tm->tm_mday,
          1900 + now_tm->tm_year,
          now_tm->tm_hour,
          now_tm->tm_min,
          now_tm->tm_sec);

  return;
}

char *
gp_lower_case(char *name)
{
  char *new;
  char *ptr;

  ptr = new = strdup(name);

  while (*ptr != '\0') {
    *ptr = tolower(*ptr);
    ptr++;
  }

  return new;
}

/* linked list functions */

gp_linked_list *
gp_list_make(void)
{
  gp_linked_list *new;

  new = malloc(sizeof(*new));
  new->annotation = NULL;
  new->prev = NULL;
  new->next = NULL;

  return new;
}

void
gp_list_annotate(gp_linked_list *link, void *a)
{
  link->annotation = a;
}

void *
gp_list_get(gp_linked_list *link)
{
  return link->annotation;
}

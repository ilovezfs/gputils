/* General system functions
   Copyright (C) 2003, 2004
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

#ifndef __GPSYSTEM_H__
#define __GPSYSTEM_H__

extern char *gp_header_path;
extern char *gp_lkr_path;
extern char *gp_lib_path;
extern char *gp_pub_path;

void gp_init(void);

void  gp_fputl16(short data, FILE *fp);
void  gp_fputl32(long data, FILE *fp); 
void  gp_fputvar(char *data, int number, FILE *fp); 
short gp_getl16(char *addr);
unsigned short gp_getu16(char *addr);
long  gp_getl32(char *addr);
void  gp_putl16(char *addr, short data);
void  gp_putl32(char *addr, long data);

long  gp_getb32(char *addr);
void  gp_putb32(char *addr, long data);

void  gp_date_string(char *buffer, size_t sizeof_buffer);
char *gp_lower_case(char *name);
char *gp_upper_case(char *name);

typedef struct gp_list_struct gp_linked_list;

struct gp_list_struct {
  void *annotation;
  struct gp_list_struct *prev;
  struct gp_list_struct *next;
};

gp_linked_list *gp_list_make(void);
void gp_list_annotate(gp_linked_list *link, void *a);
void *gp_list_get(gp_linked_list *link);

#endif

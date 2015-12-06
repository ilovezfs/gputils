/* General system functions
   Copyright (C) 2003, 2004, 2005
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

/* Used in disasm. */
#define EXPLANATION_DISTANCE            60
#define TABULATOR_SIZE                  8
#define ARRAY_SIZE(a)                   (sizeof(a) / sizeof((a)[0]))

extern const char *gp_header_path;
extern const char *gp_lkr_path;
extern const char *gp_lib_path;

void gp_init(void);

void  gp_fputl16(short data, FILE *fp);
void  gp_fputl32(long data, FILE *fp); 
void  gp_fputvar(const void *data, int number, FILE *fp); 
short gp_getl16(const unsigned char *addr);
unsigned short gp_getu16(const unsigned char *addr);
long  gp_getl32(const unsigned char *addr);
void  gp_putl16(unsigned char *addr, unsigned short data);
void  gp_putl32(unsigned char *addr, unsigned long data);

long  gp_getb32(const unsigned char *addr);
void  gp_putb32(unsigned char *addr, unsigned long data);

void  gp_date_string(char *buffer, size_t sizeof_buffer);

void *gp_malloc(size_t Size, const char *File, size_t Line, const char *Func);
void *gp_calloc(size_t Nmemb, size_t Size, const char *File, size_t Line, const char *Func);
void *gp_realloc(void *Mem, size_t Size, const char *File, size_t Line, const char *Func);
char *gp_strdup(const char *String, const char *File, size_t Line, const char *Func);
char *gp_strndup(const char *String, size_t Length, const char *File, size_t Line, const char *Func);

#define GP_Malloc(Size)                 gp_malloc(Size, __FILE__, __LINE__, __func__)
#define GP_Calloc(Nmemb, Size)          gp_calloc(Nmemb, Size, __FILE__, __LINE__, __func__)
#define GP_Realloc(Mem, Size)           gp_realloc(Mem, Size, __FILE__, __LINE__, __func__)
#define GP_Strdup(String)               gp_strdup(String, __FILE__, __LINE__, __func__)
#define GP_Strndup(String, Length)      gp_strndup(String, Length, __FILE__, __LINE__, __func__)

char *gp_lower_case(const char *name);
char *gp_upper_case(const char *name);
char *gp_strncpy(char *Dest, const char *Src, size_t Maxlen);
char *gp_stptoupper(char *Dest, const char *Src, size_t Maxlen);

size_t gp_align_text(char *Buffer, size_t Buffer_length, size_t Current_length, size_t Aligned_to_length);
size_t gp_exclamation(char *Buffer, size_t Buffer_length, size_t Current_length, const char *Format, ...);

typedef struct gp_list_struct gp_linked_list;

struct gp_list_struct {
  void *annotation;
  struct gp_list_struct *prev;
  struct gp_list_struct *next;
};

gp_linked_list *gp_list_make(void);
void gp_list_annotate(gp_linked_list *link, void *a);
void *gp_list_get(gp_linked_list *link);

char *gp_absolute_path(char *filename);

void gp_exit_if_arg_an_option(const struct option *options, int opt_max_index, int opt_index,
                              const char *opt_string, int opt_char, const char *command);
#endif

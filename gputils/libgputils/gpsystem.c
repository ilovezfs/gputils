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

#include "stdhdr.h"
#include "libgputils.h"

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

const char *gp_header_path;
const char *gp_lkr_path;
const char *gp_lib_path;

gp_boolean absolute_path_warning = true;

/*------------------------------------------------------------------------------------------------*/

/* initialize the library */

void
gp_init(void)
{

#ifdef USE_DEFAULT_PATHS
  /* load the environmental variables */
  gp_header_path = getenv("GPUTILS_HEADER_PATH");
  gp_lkr_path = getenv("GPUTILS_LKR_PATH");
  gp_lib_path = getenv("GPUTILS_LIB_PATH");

  #ifndef HAVE_DOS_BASED_FILE_SYSTEM
    if (gp_header_path == NULL) {
      gp_header_path = GP_Strdup(HEADER_PATH);
    }
    if (gp_lkr_path == NULL) {
      gp_lkr_path = GP_Strdup(LKR_PATH);
    }
    if (gp_lib_path == NULL) {
      gp_lib_path = GP_Strdup(LIB_PATH);
    }
  #endif

#else
  gp_header_path = NULL;
  gp_lkr_path = NULL;
  gp_lib_path = NULL;
#endif

}

/*------------------------------------------------------------------------------------------------*/

/* little endian functions */

void 
gp_fputl16(short data, FILE *fp) 
{
  fputc(data & 0xff, fp);
  fputc((data >> 8) & 0xff, fp);
}

/*------------------------------------------------------------------------------------------------*/

void 
gp_fputl32(long data, FILE *fp) 
{
  fputc(data & 0xff, fp);
  fputc((data >> 8) & 0xff, fp);
  fputc((data >> 16) & 0xff, fp);
  fputc((data >> 24) & 0xff, fp);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_fputvar(const void *data_, int number, FILE *fp)
{
  const unsigned char *data = (const unsigned char *)data_;
  int i;
  
  for (i = 0; i < number; i++) {
    fputc(data[i], fp);
  }
}

/*------------------------------------------------------------------------------------------------*/

short 
gp_getl16(const unsigned char *addr)
{
  short value;
  
  value  = addr[0];
  value |= (short)addr[1] << 8;
  
  return value;
}

/*------------------------------------------------------------------------------------------------*/

unsigned short 
gp_getu16(const unsigned char *addr)
{
  return (unsigned short)gp_getl16(addr);
}

/*------------------------------------------------------------------------------------------------*/

long 
gp_getl32(const unsigned char *addr)
{
  long value;

  value  = addr[0];
  value |= (long)addr[1] << 8;
  value |= (long)addr[2] << 16;
  value |= (long)addr[3] << 24;

  return value;
}

/*------------------------------------------------------------------------------------------------*/

void 
gp_putl16(unsigned char *addr, unsigned short data)
{
  addr[1] = (data >> 8) & 0xff;
  addr[0] = data & 0xff;
}

/*------------------------------------------------------------------------------------------------*/

void 
gp_putl32(unsigned char *addr, unsigned long data)
{
  addr[0] = data & 0xff;
  addr[1] = (data >> 8)  & 0xff;
  addr[2] = (data >> 16) & 0xff;
  addr[3] = (data >> 24) & 0xff;
}

/*------------------------------------------------------------------------------------------------*/

/* big endian functions */

long 
gp_getb32(const unsigned char *addr)
{
  long value;

  value  = addr[0] << 24;
  value |= addr[1] << 16;
  value |= addr[2] << 8;
  value |= addr[3];

  return value;
}

/*------------------------------------------------------------------------------------------------*/

void 
gp_putb32(unsigned char *addr, unsigned long data)
{
  addr[0] = (data >> 24) & 0xff;
  addr[1] = (data >> 16) & 0xff;
  addr[2] = (data >> 8)  & 0xff;
  addr[3] = data & 0xff;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_date_string(char *buffer, size_t sizeof_buffer)
{
  time_t now;
  const struct tm *now_tm;

  time(&now);
  now_tm = localtime(&now);
  if (now_tm != NULL) {
    snprintf(buffer, sizeof_buffer,
             "%d-%d-%d  %02d:%02d:%02d",
             now_tm->tm_mon + 1,
             now_tm->tm_mday,
             1900 + now_tm->tm_year,
             now_tm->tm_hour,
             now_tm->tm_min,
             now_tm->tm_sec);
  } else if (sizeof_buffer > 0) {
    buffer[0] = '\0';
  }
}

/*------------------------------------------------------------------------------------------------*/

void *
gp_malloc(size_t Size, const char *File, size_t Line, const char *Func)
{
  void *m;

  if (Size == 0) {
    return NULL;
  }

  if ((m = malloc(Size)) == NULL) {
    fprintf(stderr, "%s() -- Could not allocate %zu bytes of memory. {%s.LINE-%zu, %s()}\n",
            __func__, Size, File, Line, Func);
    exit(1);
  }

  return m;
}

/*------------------------------------------------------------------------------------------------*/

void *
gp_calloc(size_t Nmemb, size_t Size, const char *File, size_t Line, const char *Func)
{
  void *m;

  if ((Nmemb == 0) || (Size == 0)) {
    return NULL;
  }

  if ((m = calloc(Nmemb, Size)) == NULL) {
    fprintf(stderr, "%s() -- Could not allocate %zu bytes of memory. {%s.LINE-%zu, %s()}\n",
            __func__, Nmemb * Size, File, Line, Func);
    exit(1);
  }

  return m;
}

/*------------------------------------------------------------------------------------------------*/

void *
gp_realloc(void *Mem, size_t Size, const char *File, size_t Line, const char *Func)
{
  void *m;

  if (Size == 0) {
    return NULL;
  }

  if ((m = realloc(Mem, Size)) == NULL) {
    fprintf(stderr, "%s() -- Could not reallocate %zu bytes of memory. {%s.LINE-%zu, %s()}\n",
            __func__, Size, File, Line, Func);
    exit(1);
  }

  return m;
}

/*------------------------------------------------------------------------------------------------*/

char *
gp_strdup(const char *String, const char *File, size_t Line, const char *Func)
{
  char *s;

  if (String == NULL) {
    return NULL;
  }

  if ((s = strdup(String)) == NULL) {
    fprintf(stderr, "%s(\"%s\") -- Could not allocate string {%s.LINE-%zu, %s()}, error: %s.\n",
            __func__, String, File, Line, Func, strerror(errno));
    exit(1);
  }

  return s;
}

/*------------------------------------------------------------------------------------------------*/

char *
gp_strndup(const char *String, size_t Length, const char *File, size_t Line, const char *Func)
{
  char *s;

  if (String == NULL) {
    return NULL;
  }

  if ((s = strndup(String, Length)) == NULL) {
    fprintf(stderr, "%s(\"%s\", %zu) -- Could not allocate string {%s.LINE-%zu, %s()}, error: %s.\n",
            __func__, String, Length, File, Line, Func, strerror(errno));
    exit(1);
  }

  return s;
}

/*------------------------------------------------------------------------------------------------*/

char *
gp_lower_case(const char *name)
{
  char ch;
  char *new;
  char *ptr;

  ptr = new = GP_Strdup(name);

  while ((ch = *ptr) != '\0') {
    *ptr = tolower(ch);
    ptr++;
  }

  return new;
}

/*------------------------------------------------------------------------------------------------*/

char *
gp_upper_case(const char *name)
{
  char ch;
  char *new;
  char *ptr;

  ptr = new = GP_Strdup(name);

  while ((ch = *ptr) != '\0') {
    *ptr = toupper(ch);
    ptr++;
  }

  return new;
}

/*------------------------------------------------------------------------------------------------*/

char *
gp_strncpy(char *Dest, const char *Src, size_t Maxlen)
{
  char ch;

  if ((Dest == NULL) || (Src == NULL) || (Maxlen == 0)) {
    return NULL;
  }

  do {
    if (--Maxlen == 0) {
      *Dest = '\0';
      break;
    }

    ch = *Src++;
    *Dest++ = ch;
  } while (ch != '\0');

  return Dest;
}

/*------------------------------------------------------------------------------------------------*/

char *
gp_stptoupper(char *Dest, const char *Src, size_t Maxlen)
{
  char ch;

  if ((Dest == NULL) || (Src == NULL) || (Maxlen == 0)) {
    return NULL;
  }

  do {
    if (--Maxlen == 0) {
      *Dest = '\0';
      break;
    }

    ch = *Src++;
    *Dest++ = toupper(ch);
  } while (ch != '\0');

  return Dest;
}

/*------------------------------------------------------------------------------------------------*/

size_t
gp_align_text(char *Buffer, size_t Buffer_length, size_t Current_length, size_t Aligned_to_length)
{
  size_t len;

  if ((Current_length < (Buffer_length - 1)) && (Aligned_to_length > Current_length)) {
    Buffer_length -= Current_length;
    len = Aligned_to_length - Current_length;

    if (len >= Buffer_length) {
      len = Buffer_length - 1;
    }

    len = snprintf(&Buffer[Current_length], Buffer_length - Current_length, "%*s", (int)len, " ");
    return ((len > 0) ? (Current_length + len) : Current_length);
  }

  return Current_length;
}

/*------------------------------------------------------------------------------------------------*/

size_t
gp_exclamation(char *Buffer, size_t Buffer_length, size_t Current_length, const char *Format, ...)
{
  size_t l;
  size_t length;
  va_list(ap);

  length = gp_align_text(Buffer, Buffer_length, Current_length, EXPLANATION_DISTANCE);
  va_start(ap, Format);
  l = vsnprintf(&Buffer[length], Buffer_length - length, Format, ap);
  va_end(ap);
  return ((l >= 0) ? (length + l) : Current_length);
}

/*------------------------------------------------------------------------------------------------*/

/* linked list functions */

gp_linked_list *
gp_list_make(void)
{
  gp_linked_list *new;

  new = GP_Malloc(sizeof(*new));
  new->annotation = NULL;
  new->prev = NULL;
  new->next = NULL;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_list_annotate(gp_linked_list *link, void *a)
{
  link->annotation = a;
}

/*------------------------------------------------------------------------------------------------*/

void *
gp_list_get(gp_linked_list *link)
{
  return link->annotation;
}

/*------------------------------------------------------------------------------------------------*/

/* fetch the absolute path of the filename */
char *
gp_absolute_path(char *file_name)
{
#ifdef HAVE_WINDOWS_H

  /* It would be better to test for GetFullPathName, but the test won't
     work with a cross compiler. So if windows.h exists, we assume that
     GetFullPathName is available. */ 

  #define FILE_BUFFER_SIZE 512
  char file_buffer[FILE_BUFFER_SIZE];
  char *file_ptr;
  int num_chars;

  num_chars = GetFullPathName(file_name, FILE_BUFFER_SIZE, file_buffer, &file_ptr);
  if (num_chars == 0) {
    gp_error("Can't fetch full path of %s.", file_name);
    return file_name;
  } else {
    return GP_Strdup(file_buffer);
  }
#else
  #ifdef HAVE_REALPATH
  char *resolved_name;

  resolved_name = realpath(file_name, NULL);
  if (resolved_name == NULL) {
    gp_error("Can't fetch full path of %s.", file_name);
    return file_name;
  } else {
    return resolved_name;
  }

  #else

  if (absolute_path_warning) {
    gp_warning("Host system does not support absolute paths.");
    absolute_path_warning = false;
  }
  
  return file_name;

  #endif
#endif
}

/*------------------------------------------------------------------------------------------------*/

void
gp_exit_if_arg_an_option(const struct option *options, int opt_max_index, int opt_index,
                         const char *opt_string, int opt_char, const char *command)
{
  const struct option *opt;
  int i;
  char cmd[8];

  if (opt_index < 0) {
    /* This is likely a short option. */
    opt = options;
    i = 0;

    while (opt->name != NULL) {
      if (opt->val == opt_char) {
        opt_index = i;
        cmd[0] = '-';
        cmd[1] = (char)opt_char;
        cmd[2] = '\0';
        command = cmd;
        break;
      }

      ++opt;
      ++i;
    }
  }

  if (opt_index < 0) {
    /* This not an option. */
    return;
  }

  if (opt_index >= opt_max_index) {
    fprintf(stderr, "%s() -- Fatal error: opt_index == %i (Only valid if opt_index < %i.)\n",
            __func__, opt_index, opt_max_index);
    exit(1);
  }

  if (options[opt_index].has_arg == no_argument) {
    return;
  }

  if ((opt_string == NULL) || (opt_string[0] != '-') || (opt_string[1] == '\0')) {
    /* This opt_string can not be option. */
    return;
  }

  /* opt_string == "-." */
  opt = options;
  while (opt->name != NULL) {
    if ((opt_string[1] == '-') && (opt_string[2] != '\0')) {
      /* opt_string == "--." This is a long option? */
      if (strcmp(&opt_string[2], opt->name) == 0) {
        fprintf(stderr, "Error: This option may not be parameter of the \"%s\" option: \"--%s\" (\"%s\")\n",
                command, opt->name, opt_string);
        exit(1);
      }
    }

    if (isalnum(opt->val)) {
      /* opt_string == "-X" This is a short option? */
      if (opt_string[1] == (char)opt->val) {
        fprintf(stderr, "Error: This option may not be parameter of the \"%s\" option: \"-%c\" (\"%s\")\n",
                command, (char)opt->val, opt_string);
        exit(1);
      }
    }

    ++opt;
  }
}

/* General system functions
   Copyright (C) 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2015-2016 Molnar Karoly

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
  gp_lkr_path    = getenv("GPUTILS_LKR_PATH");
  gp_lib_path    = getenv("GPUTILS_LIB_PATH");

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
  gp_lkr_path    = NULL;
  gp_lib_path    = NULL;
#endif

}

/*------------------------------------------------------------------------------------------------*/

/* little endian functions */

void 
gp_fputl16(int16_t Data, FILE *Fp)
{
  fputc(Data & 0xff, Fp);
  fputc((Data >> 8) & 0xff, Fp);
}

/*------------------------------------------------------------------------------------------------*/

void 
gp_fputl32(int32_t Data, FILE *Fp)
{
  fputc(Data & 0xff, Fp);
  fputc((Data >>  8) & 0xff, Fp);
  fputc((Data >> 16) & 0xff, Fp);
  fputc((Data >> 24) & 0xff, Fp);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_fputvar(const void *Data, size_t Number, FILE *Fp)
{
  const uint8_t *d;
  size_t         i;

  d = (const uint8_t *)Data;
  for (i = 0; i < Number; i++) {
    fputc(d[i], Fp);
  }
}

/*------------------------------------------------------------------------------------------------*/

int16_t
gp_getl16(const uint8_t *Addr)
{
  int16_t value;
  
  value  = (int16_t)Addr[0];
  value |= (int16_t)Addr[1] << 8;
  
  return value;
}

/*------------------------------------------------------------------------------------------------*/

uint16_t
gp_getu16(const uint8_t *Addr)
{
  return (uint16_t)gp_getl16(Addr);
}

/*------------------------------------------------------------------------------------------------*/

int32_t
gp_getl32(const uint8_t *Addr)
{
  int32_t value;

  value  = (int32_t)Addr[0];
  value |= (int32_t)Addr[1] << 8;
  value |= (int32_t)Addr[2] << 16;
  value |= (int32_t)Addr[3] << 24;

  return value;
}

/*------------------------------------------------------------------------------------------------*/

uint32_t
gp_getu32(const uint8_t *Addr)
{
  return (uint32_t)gp_getl32(Addr);
}

/*------------------------------------------------------------------------------------------------*/

void 
gp_putl16(uint8_t *Addr, uint16_t Data)
{
  Addr[0] = Data & 0xff;
  Addr[1] = (Data >> 8) & 0xff;
}

/*------------------------------------------------------------------------------------------------*/

void 
gp_putl32(uint8_t *Addr, uint32_t Data)
{
  Addr[0] = Data & 0xff;
  Addr[1] = (Data >>  8) & 0xff;
  Addr[2] = (Data >> 16) & 0xff;
  Addr[3] = (Data >> 24) & 0xff;
}

/*------------------------------------------------------------------------------------------------*/

/* big endian functions */

int32_t 
gp_getb32(const uint8_t *Addr)
{
  int32_t value;

  value  = (int32_t)Addr[0] << 24;
  value |= (int32_t)Addr[1] << 16;
  value |= (int32_t)Addr[2] <<  8;
  value |= (int32_t)Addr[3];

  return value;
}

/*------------------------------------------------------------------------------------------------*/

void 
gp_putb32(uint8_t *Addr, uint32_t Data)
{
  Addr[0] = (Data >> 24) & 0xff;
  Addr[1] = (Data >> 16) & 0xff;
  Addr[2] = (Data >>  8) & 0xff;
  Addr[3] = Data & 0xff;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_num_range_is_overlapped(int Area_start, int Area_end, int Ref_start, int Ref_end)
{
  int min;
  int max;
  int w0;
  int w1;
  int width0;
  int width1;

  if (Area_start > Area_end) {
    /* Swap these values. */
    w0         = Area_start;
    Area_start = Area_end;
    Area_end   = w0;
  }

  if (Ref_start > Ref_end) {
    /* Swap these values. */
    w0        = Ref_start;
    Ref_start = Ref_end;
    Ref_end   = w0;
  }

  min = (Area_start < Ref_start) ? Area_start : Ref_start;
  max = (Area_end > Ref_end) ? Area_end : Ref_end;

  /*
     min  Area                         max
      |    |                            |
      V    V                  Ref       |
      +----------+             |        |
           w0                  V        V
                      +-----------------+
                              w1

      +---------------------------------+
                    width0
  */

  w0 = Area_end - Area_start + 1;
  w1 = Ref_end - Ref_start + 1;

  width0 = max - min + 1;
  width1 = w0 + w1;

  return ((width0 < width1) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_find_lowest_bit(uint64_t Bits)
{
  unsigned int idx;

  if (Bits == 0) {
    return 0;
  }

  idx = 1;

  if ((Bits & 0x00000000FFFFFFFFul) == 0) {
    idx += 32;
    Bits >>= 32;
  }

  if ((Bits & 0x000000000000FFFFul) == 0) {
    idx += 16;
    Bits >>= 16;
  }

  if ((Bits & 0x00000000000000FFul) == 0) {
    idx += 8;
    Bits >>= 8;
  }

  if ((Bits & 0x000000000000000Ful) == 0) {
    idx += 4;
    Bits >>= 4;
  }

  if ((Bits & 0x0000000000000003ul) == 0) {
    idx += 2;
    Bits >>= 2;
  }

  if ((Bits & 0x0000000000000001ul) == 0) {
    idx += 1;
  }

  return idx;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_find_highest_bit(uint64_t Bits)
  {
  unsigned int idx;

  if (Bits == 0) {
    return 0;
  }

  idx = 64;

  if ((Bits & 0xFFFFFFFF00000000ul) == 0) {
    idx -= 32;
    Bits <<= 32;
  }

  if ((Bits & 0xFFFF000000000000ul) == 0) {
    idx -= 16;
    Bits <<= 16;
  }

  if ((Bits & 0xFF00000000000000ul) == 0) {
    idx -= 8;
    Bits <<= 8;
  }

  if ((Bits & 0xF000000000000000ul) == 0) {
    idx -= 4;
    Bits <<= 4;
  }

  if ((Bits & 0xC000000000000000ul) == 0) {
    idx -= 2;
    Bits <<= 2;
  }

  if ((Bits & 0x8000000000000000ul) == 0) {
    idx -= 1;
  }

  return idx;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_date_string(char *Buffer, size_t Sizeof_buffer)
{
  time_t           now;
  const struct tm *now_tm;
  char             format[60];

  time(&now);
  now_tm = localtime(&now);
  if (now_tm != NULL) {
#if defined(HAVE_LOCALE_H) && defined(HAVE_LANGINFO_H)
    setlocale(LC_ALL, "");
    snprintf(format, sizeof(format), "%s  %s", nl_langinfo(D_FMT), nl_langinfo(T_FMT));
    strftime(Buffer, Sizeof_buffer, format, now_tm);
    setlocale(LC_ALL, "C");
#else
    snprintf(Buffer, Sizeof_buffer,
             "%d-%d-%d  %02d:%02d:%02d",
             now_tm->tm_mon + 1,
             now_tm->tm_mday,
             1900 + now_tm->tm_year,
             now_tm->tm_hour,
             now_tm->tm_min,
             now_tm->tm_sec);
#endif
  }
  else if (Sizeof_buffer > 0) {
    Buffer[0] = '\0';
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
    fprintf(stderr, "%s() -- Could not allocate %lu bytes of memory. {%s.LINE-%lu, %s()}\n",
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
    fprintf(stderr, "%s() -- Could not allocate %lu bytes of memory. {%s.LINE-%lu, %s()}\n",
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
    free(Mem);
    return NULL;
  }

  if ((m = realloc(Mem, Size)) == NULL) {
    fprintf(stderr, "%s() -- Could not reallocate %lu bytes of memory. {%s.LINE-%lu, %s()}\n",
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

  assert(String != NULL);

  if ((s = strdup(String)) == NULL) {
    fprintf(stderr, "%s(\"%s\") -- Could not allocate string {%s.LINE-%lu, %s()}, error: %s.\n",
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

  assert(String != NULL);

  if ((s = strndup(String, Length)) == NULL) {
    fprintf(stderr, "%s(\"%s\", %lu) -- Could not allocate string {%s.LINE-%lu, %s()}, error: %s.\n",
            __func__, String, Length, File, Line, Func, strerror(errno));
    exit(1);
  }

  return s;
}

/*------------------------------------------------------------------------------------------------*/

char *
gp_strdup_lower_case(const char *Name)
{
  char  ch;
  char *new;
  char *ptr;

  ptr = new = GP_Strdup(Name);

  while ((ch = *ptr) != '\0') {
    *ptr = tolower(ch);
    ptr++;
  }

  return new;
}

/*------------------------------------------------------------------------------------------------*/

char *
gp_strdup_upper_case(const char *Name)
{
  char  ch;
  char *new;
  char *ptr;

  assert(Name != NULL);

  ptr = new = GP_Strdup(Name);

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

  assert(Dest != NULL);
  assert(Src != NULL);

  if (Maxlen == 0) {
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
gp_arch_strncpy(char *Dest, const char *Src, size_t Maxlen)
{
  char ch;

  assert(Dest != NULL);
  assert(Src != NULL);

  if (Maxlen == 0) {
    return NULL;
  }

  while (true) {
    if (--Maxlen == 0) {
      break;
    }

    ch = *Src;
    if (ch == '\0') {
      break;
    }

    *Dest = ch;
    ++Src;
    ++Dest;
  };

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

/*
    C_str          : Beginning address of C style string area.
    C_max_size     : Size of C style string area.
    Pascal_str     : Beginning address of Pascal style string area.
    Pascal_max_size: Size of Pascal style string area, included the string length.
*/

char *
gp_str_from_Pstr(char *C_str, size_t C_max_size, const uint8_t *Pascal_str, size_t Pascal_max_size)
{
  size_t length;

  assert(C_str != NULL);
  assert(Pascal_str != NULL);

  /* This is a Pascal style string, read the length. */
  length = *Pascal_str;
  /* The real beginning of string. */
  ++Pascal_str;

  if (length >= Pascal_max_size) {
    /* The string too long, corrects the size. */
    length = Pascal_max_size - 1;
  }

  if (length >= C_max_size) {
    /* The storage area too little. */
    length = C_max_size - 1;
  }

  memcpy(C_str, Pascal_str, length);
  C_str[length] = '\0';
  return C_str;
}

/*------------------------------------------------------------------------------------------------*/

/*
    Pascal_str     : Beginning address of Pascal style string area.
    Pascal_max_size: Size of Pascal style string area, included the string length.
    C_str          : Beginning address of C style string area.
*/

size_t
gp_Pstr_from_str(uint8_t *Pascal_str, size_t Pascal_max_size, const char *C_str)
{
  size_t length;

  assert(Pascal_str != NULL);
  assert(C_str != NULL);

  length = strlen(C_str);

  if (length >= Pascal_max_size) {
    /* Truncate the length. */
    length = Pascal_max_size - 1;
  }

  /* Store the string length. */
  *Pascal_str = length;
  /* The real beginning of string. */
  ++Pascal_str;
  memcpy(Pascal_str, C_str, length);
  return length;
}

/*------------------------------------------------------------------------------------------------*/

/*
    C_str            : Beginning address of C style string area.
    Pascal_max_size  : Size of Pascal style string area, included the string length.
    Is_limited_length: This a pointer wherewith the function indicates that limitation occurred.
*/

size_t
gp_strlen_Plimit(const char *C_str, size_t Pascal_max_size, gp_boolean *Is_limited_length)
{
  size_t     length;
  gp_boolean limit;

  assert(C_str != NULL);

  length = strlen(C_str);

  if (length >= Pascal_max_size) {
    length = Pascal_max_size - 1;
    limit = true;
  }
  else {
    limit = false;
  }

  if (Is_limited_length != NULL) {
    *Is_limited_length = limit;
  }

  return length;
}

/*------------------------------------------------------------------------------------------------*/

size_t
gp_align_text(char *Buffer, size_t Buffer_length, size_t Current_length, size_t Aligned_to_length)
{
  int    lp;
  size_t length;

  if ((Current_length < (Buffer_length - 1)) && (Aligned_to_length > Current_length)) {
    Buffer_length -= Current_length;
    length = Aligned_to_length - Current_length;

    if (length >= Buffer_length) {
      length = Buffer_length - 1;
    }

    lp = snprintf(&Buffer[Current_length], Buffer_length - Current_length, "%*s", (int)length, " ");
    return ((lp > 0) ? (Current_length + lp) : Current_length);
  }

  return Current_length;
}

/*------------------------------------------------------------------------------------------------*/

size_t
gp_exclamation(char *Buffer, size_t Buffer_length, size_t Current_length, const char *Format, ...)
{
  int     lp;
  size_t  length;
  va_list ap;

  length = gp_align_text(Buffer, Buffer_length, Current_length, EXPLANATION_DISTANCE);
  va_start(ap, Format);
  lp = vsnprintf(&Buffer[length], Buffer_length - length, Format, ap);
  va_end(ap);
  return ((lp >= 0) ? (length + lp) : Current_length);
}

/*------------------------------------------------------------------------------------------------*/

/* fetch the absolute path of the filename */

char *
gp_absolute_path(char *File_name)
{
#ifdef HAVE_WINDOWS_H

  /* It would be better to test for GetFullPathName, but the test won't
     work with a cross compiler. So if windows.h exists, we assume that
     GetFullPathName is available. */ 

  #define FILE_BUFFER_SIZE 512
  char  file_buffer[FILE_BUFFER_SIZE];
  char *file_ptr;
  int   num_chars;

  num_chars = GetFullPathName(File_name, FILE_BUFFER_SIZE, file_buffer, &file_ptr);
  if (num_chars == 0) {
    gp_error("Can't fetch full path of %s.", File_name);
    return File_name;
  }
  else {
    return GP_Strdup(file_buffer);
  }
#else
  #ifdef HAVE_REALPATH
  char *resolved_name;

  resolved_name = realpath(File_name, NULL);
  if (resolved_name == NULL) {
    gp_error("Can't fetch full path of \"%s\".", File_name);
    return File_name;
  }
  else {
    return resolved_name;
  }

  #else

  if (absolute_path_warning) {
    gp_warning("Host system does not support absolute paths.");
    absolute_path_warning = false;
  }
  
  return File_name;

  #endif
#endif
}

/*------------------------------------------------------------------------------------------------*/

void
gp_exit_if_arg_an_option(const struct option *Options, int Opt_max_index, int Opt_index,
                         const char *Opt_string, int Opt_char, const char *Command)
{
  const struct option *opt;
  int                  i;
  char                 cmd[8];

  if (Opt_index < 0) {
    /* This is likely a short option. */
    opt = Options;
    i = 0;

    while (opt->name != NULL) {
      if (opt->val == Opt_char) {
        Opt_index = i;
        cmd[0] = '-';
        cmd[1] = (char)Opt_char;
        cmd[2] = '\0';
        Command = cmd;
        break;
      }

      ++opt;
      ++i;
    }
  }

  if (Opt_index < 0) {
    /* This not an option. */
    return;
  }

  if (Opt_index >= Opt_max_index) {
    fprintf(stderr, "%s() -- Fatal error: opt_index == %i (Only valid if opt_index < %i.)\n",
            __func__, Opt_index, Opt_max_index);
    exit(1);
  }

  if (Options[Opt_index].has_arg == no_argument) {
    return;
  }

  if ((Opt_string == NULL) || (Opt_string[0] != '-') || (Opt_string[1] == '\0')) {
    /* This opt_string can not be option. */
    return;
  }

  /* opt_string == "-." */
  opt = Options;
  while (opt->name != NULL) {
    if ((Opt_string[1] == '-') && (Opt_string[2] != '\0')) {
      /* Opt_string == "--." This is a long option? */
      if (strcmp(&Opt_string[2], opt->name) == 0) {
        fprintf(stderr, "Error: This option may not be parameter of the \"%s\" option: \"--%s\" (\"%s\")\n",
                Command, opt->name, Opt_string);
        exit(1);
      }
    }

    if (isalnum(opt->val)) {
      /* Opt_string == "-X" This is a short option? */
      if (Opt_string[1] == (char)opt->val) {
        fprintf(stderr, "Error: This option may not be parameter of the \"%s\" option: \"-%c\" (\"%s\")\n",
                Command, (char)opt->val, Opt_string);
        exit(1);
      }
    }

    ++opt;
  }
}

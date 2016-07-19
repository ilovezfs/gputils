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

#ifndef __GPSYSTEM_H__
#define __GPSYSTEM_H__

/* Used in disasm. */
#define EXPLANATION_DISTANCE            60
#define TABULATOR_SIZE                  8
#define ARRAY_SIZE(a)                   (sizeof(a) / sizeof((a)[0]))

#define FlagSet(v, f)                   ((v) |= (f))
#define FlagClr(v, f)                   ((v) &= ~(f))
#define FlagIsSet(v, f)                 (((v) & (f)) != 0)
#define FlagIsClr(v, f)                 (((v) & (f)) == 0)

#define FlagsIsAllSet(v, f)             (((v) & (f)) == (f))
#define FlagsIsNotAllSet(v, f)          (((v) & (f)) != (f))
#define FlagsIsAllClr(v, f)             (((v) & (f)) == 0)
#define FlagsIsNotAllClr(v, f)          (((v) & (f)) != 0)

extern const char *gp_header_path;
extern const char *gp_lkr_path;
extern const char *gp_lib_path;

extern void gp_init(void);

extern void     gp_fputl16(int16_t Data, FILE *Fp);
extern void     gp_fputl32(int32_t Data, FILE *Fp);
extern void     gp_fputvar(const void *Data, size_t Number, FILE *Fp);
extern int16_t  gp_getl16(const uint8_t *Addr);
extern uint16_t gp_getu16(const uint8_t *Addr);
extern int32_t  gp_getl32(const uint8_t *Addr);
extern uint32_t gp_getu32(const uint8_t *Addr);
extern void     gp_putl16(uint8_t *Addr, uint16_t Data);
extern void     gp_putl32(uint8_t *Addr, uint32_t Data);

extern int32_t  gp_getb32(const uint8_t *Addr);
extern void     gp_putb32(uint8_t *Addr, uint32_t Data);

extern gp_boolean gp_num_range_is_overlapped(int Area_start, int Area_end, int Ref_start, int Ref_end);

extern unsigned int gp_find_lowest_bit(uint64_t Bits);
extern unsigned int gp_find_highest_bit(uint64_t Bits);

extern void  gp_date_string(char *Buffer, size_t Sizeof_buffer);

extern void *gp_malloc(size_t Size, const char *File, size_t Line, const char *Func);
extern void *gp_calloc(size_t Nmemb, size_t Size, const char *File, size_t Line, const char *Func);
extern void *gp_realloc(void *Mem, size_t Size, const char *File, size_t Line, const char *Func);
extern char *gp_strdup(const char *String, const char *File, size_t Line, const char *Func);
extern char *gp_strndup(const char *String, size_t Length, const char *File, size_t Line, const char *Func);

#define GP_Malloc(Size)                 gp_malloc(Size, __FILE__, __LINE__, __func__)
#define GP_Calloc(Nmemb, Size)          gp_calloc(Nmemb, Size, __FILE__, __LINE__, __func__)
#define GP_Realloc(Mem, Size)           gp_realloc(Mem, Size, __FILE__, __LINE__, __func__)
#define GP_Strdup(String)               gp_strdup(String, __FILE__, __LINE__, __func__)
#define GP_Strndup(String, Length)      gp_strndup(String, Length, __FILE__, __LINE__, __func__)

extern char *gp_strdup_lower_case(const char *Name);
extern char *gp_strdup_upper_case(const char *Name);
extern char *gp_strncpy(char *Dest, const char *Src, size_t Maxlen);
extern char *gp_arch_strncpy(char *Dest, const char *Src, size_t Maxlen);
extern char *gp_stptoupper(char *Dest, const char *Src, size_t Maxlen);

extern char *gp_str_from_Pstr(char *C_str, size_t C_max_size, const uint8_t *Pascal_str, size_t Pascal_max_size);
extern void gp_Pstr_from_str(uint8_t *Pascal_str, size_t Pascal_max_size, const char *C_str);

extern size_t gp_align_text(char *Buffer, size_t Buffer_length, size_t Current_length, size_t Aligned_to_length);
extern size_t gp_exclamation(char *Buffer, size_t Buffer_length, size_t Current_length, const char *Format, ...);

extern char *gp_absolute_path(char *File_name);

extern void gp_exit_if_arg_an_option(const struct option *Options, int Opt_max_index, int Opt_index,
                                     const char *Opt_string, int Opt_char, const char *Command);
#endif

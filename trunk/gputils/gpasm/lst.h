/* ".LST" file output for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin
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

#ifndef __LST_H__
#define __LST_H__
#include <stdarg.h>

/* MPASM(X) compatible listing */
#define MPASM_LIST

#ifdef MPASM_LIST
#define LST_LINENUM_POS 22
#else
#define LST_LINENUM_POS 15
#endif
#define LST_SRC_POS (LST_LINENUM_POS + 6)

void lst_line(const char *format, ...);
void lst_err_line(const char *type, unsigned int code, const char *format, va_list args);
void lst_throw(void);
void lst_init(void);
void lst_memory_map(MemBlock *m);
void lst_close(void);
void lst_format_line(const char *line, int value);
void lst_symbol_table(void);
void lst_page_start(void);
void preproc_init(void);
void preproc_emit(void);
#endif

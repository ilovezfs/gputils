/* Displays contents of ".COD" files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Scott Dattalo

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

#ifndef __DUMP_H__
#define __DUMP_H__

void dump_directory_blocks(void);
void dump_memmap(proc_class_t proc_class);
void dump_code(proc_class_t proc_class);
void dump_symbols(void);
void dump_lsymbols(void);
void dump_source_files(void);
void dump_line_symbols(void);
void dump_message_area(void);
void dump_local_vars(proc_class_t proc_class);

unsigned short get_short_int(char *buff);
char *substr(char *a, size_t sizeof_a, unsigned char *b, size_t n);

#endif

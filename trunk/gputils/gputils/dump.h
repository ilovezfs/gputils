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

extern void dump_directory_blocks(proc_class_t class);
extern void dump_memmap(proc_class_t class, gp_boolean make_list);
extern void dump_code(proc_class_t class, pic_processor_t processor);
extern void dump_symbols(void);
extern void dump_lsymbols(void);
extern void dump_source_files(void);
extern void dump_line_symbols();
extern void dump_message_area(void);
extern void dump_local_vars(proc_class_t class);

extern char *substr(char *a, size_t sizeof_a, const uint8_t *b, size_t n);

#endif

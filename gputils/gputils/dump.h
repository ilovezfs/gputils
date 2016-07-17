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

extern void dump_directory_blocks(const DirBlockInfo *Main_dir, proc_class_t Class);
extern void dump_memmap(FILE *Code_file, const DirBlockInfo *Main_dir, proc_class_t Class, gp_boolean Make_list);
extern void dump_code(FILE *Code_file, const DirBlockInfo *Main_dir, pic_processor_t Processor, gp_boolean Wide_dump);
extern void dump_symbols(FILE *Code_file, const DirBlockInfo *Main_dir);
extern void dump_lsymbols(FILE *Code_file, const DirBlockInfo *Main_dir);
extern void dump_source_files(FILE *Code_file, const DirBlockInfo *Main_dir);
extern void dump_line_symbols(FILE *Code_file, const DirBlockInfo *Main_dir);
extern void dump_message_area(FILE *Code_file, const DirBlockInfo *Main_dir);
extern void dump_local_vars(FILE *Code_file, const DirBlockInfo *Main_dir, proc_class_t Class);

extern char *substr(char *Dst, size_t Sizeof_dst, const uint8_t *Src, size_t Sizeof_src);

#endif

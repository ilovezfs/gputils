/* Generate coff file
   Copyright (C) 2002 Craig Franklin

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

#ifndef __COFF_H__
#define __COFF_H__

void coff_init(void);
void coff_close_file(void);
void coff_new_section(char *name, int addr, int flags);
void coff_add_sym(char *name, int value, int section, int type, int class);
void coff_reloc(int symbol, short offset, unsigned short type);
void coff_linenum(int emitted);
void coff_add_filesym(char *name, int isinclude);
void coff_add_eofsym(void);
void coff_add_listsym(void);
void coff_add_nolistsym(void);
int  coff_symbol_section(char *name);
char *coff_local_name(char *name);

#endif

/* Generate coff file
   Copyright (C) 2002, 2003, 2004, 2005
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

#ifndef __COFF_H__
#define __COFF_H__

#define SECTION_FLAGS state.obj.flags
#define IS_RAM_ORG (SECTION_FLAGS & (STYP_BSS | STYP_DATA))

void coff_init(void);
void coff_cleanup_before_eof(void);
void coff_close_file(void);
void coff_new_section(const char *name, int byte_addr, int flags);
void coff_close_section(void);
gp_symbol_type *coff_add_sym(const char *name, int value, enum gpasmValTypes type);
void coff_reloc(int symbol, short offset, enum gpasmValTypes type);
void coff_linenum(int emitted);
gp_symbol_type *coff_add_filesym(const char *name, gp_boolean isinclude);
void coff_add_eofsym(void);
void coff_add_listsym(void);
void coff_add_nolistsym(void);
void coff_add_directsym(unsigned char command, const char *string);
void coff_add_identsym(const char *string);
char *coff_local_name(const char *name);

#endif

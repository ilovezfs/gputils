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

#define SECTION_FLAGS           state.obj.flags
#define IS_RAM_ORG              (SECTION_FLAGS & STYP_RAM_AREA)

extern void coff_init(void);
extern void coff_cleanup_before_eof(void);
extern void coff_close_file(void);
extern void coff_new_section(const char *name, unsigned int byte_addr, unsigned int flags);
extern void coff_close_section(void);
extern gp_symbol_t *coff_add_sym(const char *name, gp_symvalue_t value, enum gpasmValTypes type);
extern void coff_reloc(unsigned int symbol_number, int16_t offset, enum gpasmValTypes type);
extern void coff_linenum(unsigned int emitted);
extern gp_symbol_t *coff_add_filesym(const char *name, gp_boolean is_include);
extern void coff_add_eofsym(void);
extern void coff_add_listsym(void);
extern void coff_add_nolistsym(void);
extern void coff_add_directsym(uint8_t command, const char *string);
extern void coff_add_identsym(const char *string);
extern char *coff_local_name(const char *name);

#endif

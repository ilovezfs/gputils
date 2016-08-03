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
extern void coff_new_section(const char *Name, unsigned int Byte_addr, unsigned int Flags);
extern void coff_close_section(void);

extern gp_symbol_t *coff_add_sym(const char *Name, gpasmVal Value, enum gpasmValTypes Type,
                                 int Section_number);

extern void coff_add_reloc(unsigned int Symbol_number, int16_t Offset, uint16_t Type);
extern void coff_add_linenum(unsigned int Emitted);
extern gp_symbol_t *coff_add_file_sym(const char *File_name, gp_boolean Is_include);
extern void coff_add_eof_sym(void);
extern void coff_add_list_sym(void);
extern void coff_add_nolist_sym(void);
extern void coff_add_direct_sym(uint8_t Command, const char *String);
extern void coff_add_ident_sym(const char *String);
extern char *coff_local_name(const char *Name);

#endif

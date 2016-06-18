
/* GP Symbol table support header file.

   Copyright (C) 2014-2016 Molnar Karoly

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

#ifndef __GPSYMBOL_H__
#define __GPSYMBOL_H__

extern gp_hash_type *gp_symbol_make_hash_table(gp_object_type *Object);

extern const gp_symbol_type *gp_symbol_find(const gp_object_type *Object, const char *Section_name,
                                            gp_symvalue_t Symbol_value);

extern gp_symbol_type **gp_symbol_make_label_array(gp_section_type *Section, unsigned int Org_to_byte_shift,
                                                   unsigned int *Num_labels);

extern gp_symbol_type *gp_symbol_find_by_value(gp_symbol_type **Array, unsigned int Num_symbols, gp_symvalue_t Value);

extern gp_boolean gp_symbol_delete_by_value(gp_symbol_type **Array, unsigned int *Num_symbols, gp_symvalue_t Value);

#endif /* __GPSYMBOL_H__ */

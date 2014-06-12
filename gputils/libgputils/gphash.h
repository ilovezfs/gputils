
/* Fowler–Noll–Vo hash making header file.

   Copyright (C) 2014 Molnar Karoly

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

#ifndef __GPHASH_H__
#define __GPHASH_H__

#define FNV1A32_INIT			0x811C9DC5UL
#define FNV1A32_PRIME			0x01000193UL

unsigned int gp_fnv1a_hash(const void *Ptr, unsigned int Size, unsigned int Hash);

unsigned int gp_fnv1a_hash_str(const char *String, unsigned int Hash);

unsigned int gp_fnv1a_hash_symbol(const gp_symbol_type *Symbol);

gp_hash_type *gp_make_hash_table(gp_object_type *Object);

const gp_symbol_type *gp_find_symbol_hash_table(const gp_object_type *Object, const char *Section_name, gp_symvalue_t Symbol_value);

#endif /* __GPHASH_H__ */

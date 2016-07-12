/* Bit array support

   Copyright (C) 2016 Molnar Karoly

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

#ifndef __GPBITARRAY_H__
#define __GPBITARRAY_H__

#include "stdhdr.h"

typedef struct gp_bit_array {
  size_t    bit_length;                 /* Number of stored bits. */
  size_t    byte_length;                /* Real size of storage in bytes. */
  size_t    group_length;               /* Real size of storage in groups. */
  uint64_t *array;
} gp_bit_array_t;

extern void gp_bitarray_create(gp_bit_array_t *Bits, size_t Bit_size);
extern void gp_bitarray_delete(gp_bit_array_t *Bits);
extern gp_boolean gp_bitarray_write(gp_bit_array_t *Bits, size_t Bit_index, gp_boolean Value);

extern gp_boolean gp_bitarray_write_range(gp_bit_array_t *Bits, size_t Bit_index_start,
                                          size_t Bit_index_end, gp_boolean Value);

extern gp_boolean gp_bitarray_clear(gp_bit_array_t *Bits);

extern gp_boolean gp_bitarray_read(const gp_bit_array_t *Bits, size_t Bit_index);

extern gp_boolean gp_bitarray_get_range_borders(const gp_bit_array_t *Bits, size_t Bit_index,
                                                size_t *Start, size_t *End);

#endif /* __GPBITARRAY_H__ */

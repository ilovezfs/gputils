
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

#include "libgputils.h"

#define BITARRAY_GROUP_SIZE             (sizeof(uint64_t) * 8)

/*------------------------------------------------------------------------------------------------*/

void
gp_bitarray_create(gp_bit_array_t *Bits, size_t Bit_size)
{
  assert(Bits != NULL);

  if (Bit_size == 0) {
    Bits->bit_length   = 0;
    Bits->group_length = 0;
    Bits->byte_length  = 0;
    Bits->array        = NULL;
  }

  Bits->bit_length   = Bit_size;
  Bits->group_length = (Bit_size + BITARRAY_GROUP_SIZE - 1) / BITARRAY_GROUP_SIZE;
  Bits->byte_length  = Bits->group_length * (sizeof(uint64_t) / sizeof(uint8_t));
  Bits->array        = GP_Calloc(sizeof(uint64_t), Bits->group_length);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_bitarray_delete(gp_bit_array_t *Bits)
{
  assert(Bits != NULL);

  if (Bits->array != NULL) {
    free(Bits->array);
    Bits->array = NULL;
  }

  Bits->bit_length   = 0;
  Bits->byte_length  = 0;
  Bits->group_length = 0;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_bitarray_write(gp_bit_array_t *Bits, size_t Bit_index, gp_boolean Value)
{
  size_t   group_index;
  size_t   bit_index;
  uint64_t cluster;

  assert(Bits != NULL);

  if ((Bits->array == NULL) || (Bit_index >= Bits->bit_length)) {
    return false;
  }

  group_index = Bit_index / BITARRAY_GROUP_SIZE;
  bit_index   = Bit_index % BITARRAY_GROUP_SIZE;
  cluster     = Bits->array[group_index];

  if (Value) {
    cluster |= 1 << bit_index;
  }
  else {
    cluster &= ~(1 << bit_index);
  }

  Bits->array[group_index] = cluster;
  return true;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_bitarray_write_range(gp_bit_array_t *Bits, size_t Bit_index_start, size_t Bit_index_end,
                        gp_boolean Value)
{
  size_t   group_index_start;
  size_t   bit_index_start;
  size_t   group_index_end;
  size_t   bit_index_end;
  size_t   i;
  uint64_t mask_start;
  uint64_t mask_end;
  uint64_t cluster;

  assert(Bits != NULL);

  if ((Bits->array == NULL) || (Bit_index_end >= Bits->bit_length)) {
    return false;
  }

  if (Bit_index_start > Bit_index_end) {
    i               = Bit_index_end;
    Bit_index_end   = Bit_index_start;
    Bit_index_start = i;
  }

  group_index_start = Bit_index_start / BITARRAY_GROUP_SIZE;
  bit_index_start   = Bit_index_start % BITARRAY_GROUP_SIZE;
  group_index_end   = Bit_index_end   / BITARRAY_GROUP_SIZE;
  bit_index_end     = Bit_index_end   % BITARRAY_GROUP_SIZE;

  mask_start = (uint64_t)(-1);
  mask_end   = (uint64_t)(-1);
  mask_start <<= bit_index_start;
  mask_end   >>= BITARRAY_GROUP_SIZE - bit_index_end - 1;

  i = group_index_end - group_index_start;
  cluster = Bits->array[group_index_start];

  if (i == 0) {
    mask_start &= mask_end;

    if (Value) {
      cluster |= mask_start;
    }
    else {
      cluster &= ~(mask_start);
    }

    Bits->array[group_index_start] = cluster;
    return true;
  }

  if (Value) {
    cluster |= mask_start;
  }
  else {
    cluster &= ~(mask_start);
  }

  Bits->array[group_index_start] = cluster;

  if (i > 1) {
    for (i = group_index_start + 1; i < group_index_end; ++i) {
      cluster = Bits->array[i];

      if (Value) {
	cluster = (uint64_t)(-1);
      }
      else {
	cluster = 0;
      }

      Bits->array[i] = cluster;
    }
  }

  cluster = Bits->array[group_index_end];

  if (Value) {
    cluster |= mask_end;
  }
  else {
    cluster &= ~(mask_end);
  }

  Bits->array[group_index_end] = cluster;
  return true;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_bitarray_clear(gp_bit_array_t *Bits)
{
  assert(Bits != NULL);

  if (Bits->array == NULL) {
    return false;
  }

  memset(Bits->array, 0, Bits->byte_length);
  return true;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_bitarray_read(const gp_bit_array_t *Bits, size_t Bit_index)
{
  size_t   group_index;
  size_t   bit_index;
  uint64_t cluster;

  assert(Bits != NULL);

  if ((Bits->array == NULL) || (Bit_index >= Bits->bit_length)) {
    return false;
  }

  group_index = Bit_index / BITARRAY_GROUP_SIZE;
  bit_index   = Bit_index % BITARRAY_GROUP_SIZE;
  cluster     = Bits->array[group_index];

  return ((cluster & (1 << bit_index)) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_find_lowest_bit(const uint64_t *Array, size_t Group_index, size_t *Find)
  {
  gp_boolean first;
  size_t     num;
  uint64_t   cluster;

  first = true;
  num   = 0;
  while (true) {
    cluster = Array[Group_index];
    if (cluster == 0) {
      if (first) {
        return false;
      }

      ++Group_index;
      *Find = num - 1 + (Group_index * BITARRAY_GROUP_SIZE);
      return true;
    }

    num = gp_find_lowest_bit(cluster);
    if (num > 1) {
      *Find = num - 1 + (Group_index * BITARRAY_GROUP_SIZE);
      return true;
    }

    if (Group_index == 0) {
      break;
    }

    --Group_index;
    first = false;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_find_highest_bit(const uint64_t *Array, size_t Group_index, size_t Group_end, size_t *Find)
  {
  gp_boolean first;
  size_t     num;
  uint64_t   cluster;

  first = true;
  num   = 0;
  while (true) {
    cluster = Array[Group_index];
    if (cluster == 0) {
      if (first) {
        return false;
      }

      --Group_index;
      *Find = num - 1 + (Group_index * BITARRAY_GROUP_SIZE);
      return true;
    }

    num = gp_find_highest_bit(cluster);
    if (num < 64) {
      *Find = num - 1 + (Group_index * BITARRAY_GROUP_SIZE);
      return true;
    }

    if (Group_index >= Group_end) {
      break;
    }

    ++Group_index;
    first = false;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_bitarray_get_range_borders(const gp_bit_array_t *Bits, size_t Bit_index, size_t *Start, size_t *End)
  {
  size_t     group_index;
  gp_boolean ret;

  assert(Bits != NULL);
  assert(Start != NULL);
  assert(End != NULL);

  if ((Bits->array == NULL) || (Bit_index >= Bits->bit_length)) {
    return false;
  }

  group_index = Bit_index / BITARRAY_GROUP_SIZE;
  ret  = _find_lowest_bit(Bits->array, group_index, Start);
  ret |= _find_highest_bit(Bits->array, group_index, Bits->group_length, End);
  return ret;
}

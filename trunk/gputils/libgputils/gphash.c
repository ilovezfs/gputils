/* Symbol table support

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

#include "stdhdr.h"
#include "libgputils.h"

#define HASH32_INIT0			0x510E3743ul
#define HASH32_INIT1			0x210F61E7ul
#define HASH32_INIT2			0xA9E47333ul
#define HASH32_INIT3			0x811CA7F5ul

#define HASH32_PRIME0			0x01023FD3ul
#define HASH32_PRIME1			0x0103E49Dul
#define HASH32_PRIME2			0x01015469ul
#define HASH32_PRIME3			0x01041943ul

/*------------------------------------------------------------------------------------------------*/

void
gp_hash_init(hash128_t *Hash)
{
  Hash->low.u32[0]  = HASH32_INIT0;
  Hash->low.u32[1]  = HASH32_INIT1;
  Hash->high.u32[0] = HASH32_INIT2;
  Hash->high.u32[1] = HASH32_INIT3;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_hash_mem(hash128_t *Hash, const void *Array, size_t Length)
{
  const uint8_t *p;
  uint32_t       ch;
  hash128_t      h;

  if ((Array == NULL) || (Length == 0)) {
    return;
  }

  p = (const uint8_t *)Array;
  h.low.u64  = Hash->low.u64;
  h.high.u64 = Hash->high.u64;

  while (Length > 0) {
    ch = (uint32_t)(*p);

    h.low.u32[0]  ^= ch;
    h.low.u32[0]  *= HASH32_PRIME0;
    h.low.u32[1]  ^= ch;
    h.low.u32[1]  *= HASH32_PRIME1;
    h.high.u32[0] ^= ch;
    h.high.u32[0] *= HASH32_PRIME2;
    h.high.u32[1] ^= ch;
    h.high.u32[1] *= HASH32_PRIME3;
    ++p;
    --Length;
  }

  Hash->low.u64  = h.low.u64;
  Hash->high.u64 = h.high.u64;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_hash_str_len(hash128_t *Hash, const char *String, size_t Length, gp_boolean Case_insensitive)
{
  const uint8_t *str;
  uint32_t       ch;
  hash128_t      h;

  if ((String == NULL) || (Length == 0)) {
    return;
  }

  str = (const uint8_t *)String;
  h.low.u64  = Hash->low.u64;
  h.high.u64 = Hash->high.u64;

  while ((Length > 0) && ((ch = (uint32_t)(*str)) != 0)) {
    if (Case_insensitive && isupper(ch)) {
      ch = _tolower(ch);
    }

    h.low.u32[0]  ^= ch;
    h.low.u32[0]  *= HASH32_PRIME0;
    h.low.u32[1]  ^= ch;
    h.low.u32[1]  *= HASH32_PRIME1;
    h.high.u32[0] ^= ch;
    h.high.u32[0] *= HASH32_PRIME2;
    h.high.u32[1] ^= ch;
    h.high.u32[1] *= HASH32_PRIME3;
    ++str;
    --Length;
  }

  Hash->low.u64  = h.low.u64;
  Hash->high.u64 = h.high.u64;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_hash_str(hash128_t *Hash, const char *String, gp_boolean Case_insensitive)
{
  gp_hash_str_len(Hash, String, (size_t)(-1), Case_insensitive);
}

/* gpregister.c - Handler of the processor registers and bits.

    Copyright (C) 2014 Molnar Karoly <molnarkaroly@users.sf.net>

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

/*------------------------------------------------------------------------------------------------*/

static int
mcu_cmp(const void *P1, const void *P2) {
  const gp_register_table_t *t1 = (const gp_register_table_t *)P1;
  const gp_register_table_t *t2 = (const gp_register_table_t *)P2;

  return strcasecmp(t1->name, t2->name);
}

/*------------------------------------------------------------------------------------------------*/

static int
reg_cmp(const void *P1, const void *P2) {
  const gp_register_t **r1 = (const gp_register_t **)P1;
  const gp_register_t **r2 = (const gp_register_t **)P2;
  unsigned int a1 = (*r1)->address;
  unsigned int a2 = (*r2)->address;

  if (a1 > a2) {
    return 1;
  }
  else if (a1 < a2) {
    return -1;
  }
  else {
    return 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

static int bit_cmp(const void *P1, const void *P2) {
  const gp_bit_t *r1 = (const gp_bit_t *)P1;
  const gp_bit_t *r2 = (const gp_bit_t *)P2;
  unsigned int a1 = r1->address;
  unsigned int a2 = r2->address;

  if (a1 > a2) {
    return 1;
  }
  else if (a1 < a2) {
    return -1;
  }
  else {
    return 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

const gp_register_table_t *
gp_register_find_mcu(const gp_register_table_t *Table, int Table_size, const char *McuName) {
  gp_register_table_t mcu;

  if ((Table == NULL) || (McuName == NULL)) {
    return NULL;
  }

  mcu.name = McuName;
  return (const gp_register_table_t *)bsearch(&mcu, Table, Table_size, sizeof(gp_register_table_t),
                                              mcu_cmp);
}

/*------------------------------------------------------------------------------------------------*/

const gp_register_t *
gp_register_find_reg(const gp_register_table_t *Mcu, unsigned int Address) {
  gp_register_t reg;
  const gp_register_t *rptr = &reg;
  const gp_register_t **ret;

  if (Mcu == NULL) {
    return NULL;
  }

  reg.address = Address;
  ret = (const gp_register_t **)bsearch(&rptr, Mcu->registers, Mcu->register_number,
                                        sizeof(gp_register_t *), reg_cmp);
  return ((ret != NULL) ? *ret : NULL);
}

/*------------------------------------------------------------------------------------------------*/

const char *
gp_register_find_reg_name(const gp_register_table_t *Mcu, unsigned int Address) {
  const gp_register_t *ret;

  ret = gp_register_find_reg(Mcu, Address);
  return ((ret != NULL) ? (ret->name) : NULL);
}

/*------------------------------------------------------------------------------------------------*/

const gp_bit_t *
gp_register_find_bit(const gp_register_t *Reg, unsigned int Address) {
  gp_bit_t bit;

  if (Reg == NULL) {
    return NULL;
  }

  bit.address = Address;
  return (const gp_bit_t *)bsearch(&bit, Reg->bits, Reg->bit_number, sizeof(gp_bit_t), bit_cmp);
}

/*------------------------------------------------------------------------------------------------*/

const char *
gp_register_find_bit_name(const gp_register_t *Reg, unsigned int Address) {
  const gp_bit_t *ret;

  ret = gp_register_find_bit(Reg, Address);
  return ((ret != NULL) ? (ret->name) : NULL);
}

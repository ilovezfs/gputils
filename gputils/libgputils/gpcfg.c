/* gpcfg.c - header file for pic object files
   Copyright (C) 2006
   Michael Ballbach

    Copyright (C) 2014 Molnar Karoly <molnarkaroly@users.sf.net>

This file is part of gputils.

Gputils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Gputils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with gputils; see the file COPYING. If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "stdhdr.h"

#include "gpcfg.h"

/*------------------------------------------------------------------------------------------------*/

static int
cmp(void const *P1, void const *P2) {
  gp_cfg_device_t const *d1 = P1;
  gp_cfg_device_t const *d2 = P2;

  return strcasecmp(d1->name, d2->name);
}

/*------------------------------------------------------------------------------------------------*/

/* Locate a PIC configuration device structure by name. */

const gp_cfg_device_t *
gp_cfg_find_pic(const char *Pic) {
  gp_cfg_device_t fake_dev = { NULL, 0, NULL };

  fake_dev.name = Pic;
  return (const gp_cfg_device_t *)bsearch(&fake_dev, gp_cfg_devices, gp_cfg_device_count,
                                          sizeof(gp_cfg_device_t), cmp);
}

/*------------------------------------------------------------------------------------------------*/

/* Locate a PIC by name, pass a list of names to use, try each in order. */

const gp_cfg_device_t *
gp_cfg_find_pic_multi_name(unsigned int Count, const char *const *Pics) {
  unsigned int t;

  for (t = 0; t < Count; t++) {
    const gp_cfg_device_t *dev = gp_cfg_find_pic(Pics[t]);

    if (dev != NULL) {
      return dev;
    }
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Give the real boundaries of the config address section. */

void
gp_cfg_real_config_boundaries(const gp_cfg_device_t *Device, int *Address_low, int *Address_high) {
  const gp_cfg_addr_t *addr;

  addr = Device->addresses;
  *Address_low  = addr[0].address;
  *Address_high = addr[Device->address_count - 1].address;
}

/*------------------------------------------------------------------------------------------------*/

/* Find a configuration directive in a processor's config db. */

const gp_cfg_directive_t *
gp_cfg_find_directive(const gp_cfg_device_t *Device, const char *Dname,
                      unsigned int *Out_config_addr, unsigned short *Out_def_value) {
  unsigned int t, u;
  const gp_cfg_addr_t *addr;
  const gp_cfg_directive_t *dir;

  for (t = Device->address_count, addr = Device->addresses; t; ++addr, --t) {
    for (u = addr->directive_count, dir = addr->directives; u; ++dir, --u) {
      if (strcasecmp(Dname, dir->name) == 0) {
        if (Out_config_addr != NULL) {
          *(Out_config_addr) = addr->address;
        }

        if (Out_def_value != NULL) {
          *(Out_def_value) = addr->def_value;
        }

        return dir;
      }
    }
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Lists in brief all configuration in a processor's config db. */

void
gp_cfg_brief_all_address(const gp_cfg_device_t *Device, const char *Head, int Addr_digits,
                         int Word_digits, unsigned int Mask_defaults) {
  unsigned int i, j, mask, def_value, xinst_mask;
  const gp_cfg_addr_t *addr;
  const gp_cfg_directive_t *dir;

  for (i = Device->address_count, addr = Device->addresses; i; ++addr, --i) {
    mask = 0;
    xinst_mask = 0;
    for (j = addr->directive_count, dir = addr->directives; j; ++dir, --j) {
      mask |= dir->mask;

      if (strcasecmp(dir->name, "XINST") == 0) {
        xinst_mask = dir->mask;
      }
    }

    def_value = addr->def_value;

    if (Mask_defaults) {
      def_value &= mask;
    }

    printf("%s0x%0*X 0x%0*X 0x%0*X", Head, Addr_digits, addr->address, Word_digits, mask, Word_digits, def_value);

    if (xinst_mask > 0) {
      printf(" 0x%0*X", Word_digits, (~xinst_mask) & ((UINT_MAX << (Word_digits * 4)) ^ UINT_MAX));
    }

    printf("\n");
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Locate an option for a directive. Return it or NULL. */

const gp_cfg_option_t *
gp_cfg_find_option(const gp_cfg_directive_t *Directive, const char *Option) {
  unsigned int t;
  const gp_cfg_option_t **opt;

  for (t = Directive->option_count, opt = Directive->options; t; ++opt, --t) {
    if (strcasecmp((*opt)->name, Option) == 0) {
      return *opt;
    }
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Find a configuration in a processor's config db. */

const gp_cfg_addr_t *
gp_cfg_find_config(const gp_cfg_device_t *Device, unsigned int Address) {
  unsigned int i;
  const gp_cfg_addr_t *addr;

  for (i = Device->address_count, addr = Device->addresses; i; ++addr, --i) {
    if (addr->address == Address) {
      return addr;
    }
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Return 0xffff or the default for the address and device passed. */

unsigned short
gp_cfg_get_default(const gp_cfg_device_t *Device, unsigned int Address) {
  const gp_cfg_addr_t *addr;

  addr = gp_cfg_find_config(Device, Address);
  return ((addr != NULL) ? addr->def_value : 0xFFFF);
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_cfg_decode_directive(const gp_cfg_device_t *Device, unsigned int Address, unsigned int Value,
                        gp_cfg_addr_hit_t *Hit) {
  unsigned int i, j, count;
  const gp_cfg_addr_t *addr;
  const gp_cfg_directive_t *dir;
  const gp_cfg_option_t **opt;
  unsigned int val;
  unsigned int max, len;

  if ((addr = gp_cfg_find_config(Device, Address)) == NULL) {
    return 0;
  }

  Hit->def_value = addr->def_value;
  count = 0;
  max = 0;
  for (i = addr->directive_count, dir = addr->directives; i; ++dir, --i) {
    val = Value & dir->mask;
    for (j = dir->option_count, opt = dir->options; j; ++opt, --j) {
      if (count >= GP_CFG_ADDR_HIT_MAX) {
        fprintf(stderr, "%s(): The size of gp_cfg_addr_hit_t too small!\n", __FUNCTION__);
        break;
      }

      if (val == (*opt)->value) {
        Hit->pairs[count].directive = dir;
        Hit->pairs[count].option    = *opt;
        len = strlen(dir->name);

        if (max < len) {
          max = len;
        }

        ++count;
        break;
      }
    }
  }

  Hit->pair_count    = count;
  Hit->max_dir_width = max;
  return count;
}

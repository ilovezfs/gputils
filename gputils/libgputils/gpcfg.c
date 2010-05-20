/* gpcfg.c - header file for pic object files
   Copyright (C) 2006
   Michael Ballbach

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

#include "gpcfg.h"

static int cb_find_pic(const void *p1, const void *p2)
{
  const struct gp_cfg_device *d1 = p1;
  const struct gp_cfg_device *d2 = p2;
  return strcasecmp(d1->device, d2->device);
}

/*
 * Locate a PIC configuration device structure by name
 */
const struct gp_cfg_device *gp_cfg_find_pic(const char *pic)
{
  struct gp_cfg_device fake_dev = { NULL, 0, NULL };
  fake_dev.device = pic;
  return bsearch(&fake_dev, gp_cfg_devices, gp_cfg_device_count, sizeof(gp_cfg_devices[0]), cb_find_pic);
}

/*
 * Locate a PIC by name, pass a list of names to use, try each in order.
 */
const struct gp_cfg_device *gp_cfg_find_pic_multi(int count, const char * const *pics)
{
  int t;
  for(t=0; t<count; t++) {
    const struct gp_cfg_device *d = gp_cfg_find_pic(pics[t]);
    if(d)
      return d;
  }

  return NULL;
}

/*
 * Find a configuration directive in a processor's config db.
 */
const struct gp_cfg_directive *gp_cfg_find_directive(const struct gp_cfg_device *device,
                            const char *dname, int *config_addr, unsigned char *def_value)
{
  size_t t, u;

  for(t=0; t<device->addr_count; t++) {
    const struct gp_cfg_addr *addr = device->config_addrs + t;
    for(u=0; u<addr->directive_count; u++) {
      const struct gp_cfg_directive *directive = addr->directives + u;
      if(strcasecmp(dname, directive->name) == 0) {
        if(config_addr)
          *(config_addr) = addr->addr;
        if(def_value)
          *(def_value) = addr->defval;
        return directive;
      }
    }
  }

  return NULL;
}

/*
 * Locate an option for a directive. Return it or NULL.
 */
const struct gp_cfg_option *gp_cfg_find_option(const struct gp_cfg_directive *directive, const char *option)
{
  size_t t;

  for(t=0; t<directive->option_count; t++) {
    const struct gp_cfg_option *p = directive->options + t;
    if(strcasecmp(p->name, option) == 0)
      return p;
  }

  return NULL;
}

/*
 * Return 0xff or the default for the address and device passed.
 */
unsigned char gp_cfg_get_default(const struct gp_cfg_device *device, int address)
{
  size_t t;
  for(t=0; t<device->addr_count; t++) {
    const struct gp_cfg_addr *addr = device->config_addrs + t;
    if(addr->addr == address)
      return addr->defval;
  }

  return 0xff;
}



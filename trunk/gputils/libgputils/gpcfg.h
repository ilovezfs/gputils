/* gpcfg.h - header file for pic object files
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

#ifndef __GPCFG_H__
#define __GPCFG_H__

/* a directive value */
struct gp_cfg_option
{
  const char *name;        /* name of the value */
  unsigned char byte;      /* it's data value */
};

/* a directive, i.e., FOSC */
struct gp_cfg_directive
{
  const char *name;                /* directive name */
  unsigned char mask;              /* mask of bytes in the config address that apply to its value */
  unsigned char option_count;      /* number of possible values */
  struct gp_cfg_option *options;   /* array of values */
};

/* one particular configuration address, i.e., 0x300001 */
struct gp_cfg_addr
{
  int addr;                              /* the address */
  unsigned char defval;                  /* its default */
  unsigned char directive_count;         /* count of relavent directives */
  struct gp_cfg_directive *directives;   /* array of directives */
};

/* a device - that is, a colllection of configuration addresses */
struct gp_cfg_device
{
  const char *device;                  /* device name */
  unsigned char addr_count;            /* number of configuration addresses */
  struct gp_cfg_addr *config_addrs;    /* array of configuration addresses */
};

/* the big table */
extern struct gp_cfg_device gp_cfg_devices[];
extern int gp_cfg_device_count;

/* helper API */
const struct gp_cfg_device *gp_cfg_find_pic(const char *pic);
const struct gp_cfg_device *gp_cfg_find_pic_multi(int count, const char * const *pics);
const struct gp_cfg_directive *gp_cfg_find_directive(const struct gp_cfg_device *device,
                    const char *directive, int *out_config_addr, unsigned char *out_def_value);
const struct gp_cfg_option *gp_cfg_find_option(const struct gp_cfg_directive *directive, const char *option);
unsigned char gp_cfg_get_default(const struct gp_cfg_device *dev, int address);

#endif /* __GPCFG_H__ */


/* libgputils typedefs
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2016 Molnar Karoly <molnarkaroly@users.sf.net>

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

#ifndef __GPTYPES_H__
#define __GPTYPES_H__

#include "stdhdr.h"

typedef enum {
  false = (0 == 1),
  true  = (0 == 0)
} gp_boolean;

typedef long    gp_symvalue_t;

typedef struct __attribute__ ((packed))
{
  union __attribute__ ((packed))
  {
    uint32_t u32[2];
    uint64_t u64;
  } low;

  union __attribute__ ((packed))
  {
    uint32_t u32[2];
    uint64_t u64;
  } high;
} hash128_t;

#endif

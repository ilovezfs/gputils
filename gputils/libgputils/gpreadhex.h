/* Read ".HEX" files and store it in memory
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

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

#ifndef __GPREADHEX_H__
#define __GPREADHEX_H__

#define IHEX_RECTYPE_DATA               0
#define IHEX_RECTYPE_EOF                1
#define IHEX_RECTYPE_EXT_SEG_ADDR       2
#define IHEX_RECTYPE_START_SEG_ADDR     3
#define IHEX_RECTYPE_EXT_LIN_ADDR       4
#define IHEX_RECTYPE_START_LIN_ADDR     5

/*
  INHX8   Have 16 bit addresses.
          Used types: IHEX_RECTYPE_DATA
                      IHEX_RECTYPE_EOF

  INHX16  Have 20 bit addresses.
          Used types: IHEX_RECTYPE_DATA
                      IHEX_RECTYPE_EOF
                      IHEX_RECTYPE_EXT_SEG_ADDR
                      IHEX_RECTYPE_START_SEG_ADDR

  INHX32  Have 32 bit addresses.
          Used types: IHEX_RECTYPE_DATA
                      IHEX_RECTYPE_EOF
                      IHEX_RECTYPE_EXT_LIN_ADDR
                      IHEX_RECTYPE_START_LIN_ADDR
*/

typedef struct hex_data {
  enum formats hex_format;
  int          size;
  gp_boolean   error;
} hex_data_t;

extern hex_data_t *gp_readhex(const char *filename, MemBlock_t *m);

#endif

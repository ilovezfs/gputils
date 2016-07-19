/* Read coff objects
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

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

#ifndef __GPREADOBJ_H__
#define __GPREADOBJ_H__

typedef enum gp_coff { 
  GP_COFF_ARCHIVE, 
  GP_COFF_OBJECT, 
  GP_COFF_OBJECT_V2, 
  GP_COFF_SYS_ERR,
  GP_COFF_UNKNOWN
} gp_coff_t;

typedef struct gp_binary {
  off_t    size;                /* size of the file in bytes */
  uint8_t *file;                /* file contents */
} gp_binary_t;

extern gp_coff_t gp_identify_coff_file(const char *File_name);
extern gp_binary_t *gp_read_file(const char *File_name);
extern void gp_free_file(gp_binary_t *Data);
extern gp_object_t *gp_convert_file(const char *File_name, const gp_binary_t *Data);
extern gp_object_t *gp_read_coff(const char *File_name);

#endif

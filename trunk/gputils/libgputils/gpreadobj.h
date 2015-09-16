/* Read coff objects
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

#ifndef __GPREADOBJ_H__
#define __GPREADOBJ_H__

typedef enum gp_coff_type { 
  GP_COFF_ARCHIVE, 
  GP_COFF_OBJECT, 
  GP_COFF_OBJECT_V2, 
  GP_COFF_SYS_ERR,
  GP_COFF_UNKNOWN
} gp_coff_type;

typedef struct gp_binary_type {
  long int        size;         /* size of the file in bytes */
  unsigned char  *file;         /* file contents */
} gp_binary_type;

gp_coff_type gp_identify_coff_file(const char *filename);
gp_binary_type *gp_read_file(const char *filename);
void gp_free_file(gp_binary_type *data);
gp_object_type *gp_convert_file(const char *filename, gp_binary_type *data);
gp_object_type *gp_read_coff(const char *filename);

#endif

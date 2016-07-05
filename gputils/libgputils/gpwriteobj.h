/* Write coff objects
   Copyright (C) 2003, 2004, 2005
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

#ifndef __GPWRITEOBJ_H__
#define __GPWRITEOBJ_H__

extern gp_boolean gp_writeobj_has_data(const gp_section_t *Section);
extern gp_boolean gp_writeobj_write_coff(gp_object_t *Object, int Num_errors);
extern gp_boolean gp_writeobj_is_absolute_object(const gp_object_t *Object);

#endif

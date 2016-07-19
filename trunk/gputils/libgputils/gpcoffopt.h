/* GNU PIC coff optimizing functions
   Copyright (C) 2005
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

#ifndef __GPCOFFOPT_H__
#define __GPCOFFOPT_H__

extern void gp_coffopt_remove_weak(gp_object_t *Object);
extern void gp_coffopt_remove_dead_sections(gp_object_t *Object, int Pass, gp_boolean Enable_cinit_wanings);
extern void gp_coffopt_remove_unnecessary_pagesel(gp_object_t *Object);
extern void gp_coffopt_remove_unnecessary_banksel(gp_object_t *Object);

#endif

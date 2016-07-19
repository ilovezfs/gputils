/* ".COD" file output for gplink
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

#ifndef __COD_H__
#define __COD_H__

/* line types for cod_lst_line */
#define COD_FIRST_LST_LINE      1
#define COD_NORMAL_LST_LINE     2
#define COD_LAST_LST_LINE       3

extern void cod_init(void);
extern void cod_close_file(void);
extern void cod_lst_line(int line_type);

#endif

/* Definitions for functions exported by the lexical analyser
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 
   James Bowman, Craig Franklin

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

#ifndef __SCAN_H__
#define __SCAN_H__

void open_src(char *name, int is_include_file);
void close_file();
void execute_exitm();
void execute_macro(struct macro_head *h, int is_while);

#endif

/* Definitions for functions exported by the lexical analyser
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
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

int yylex_destroy(void);

void open_src(const char *name, gp_boolean is_include_file);
void close_file(void);
void execute_exitm(void);
void execute_macro(struct macro_head *h, gp_boolean is_while);
int found_end(void);

extern gp_boolean force_ident;
extern gp_boolean force_decimal;

#endif

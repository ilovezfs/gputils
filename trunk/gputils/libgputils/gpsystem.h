/* General system functions
   Copyright (C) 2003
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

#ifndef __GPSYSTEM_H__
#define __GPSYSTEM_H__

void _fput_16(short data, FILE *fp);
void _fput_32(long data, FILE *fp); 
void _fput_var(char *data, int number, FILE *fp); 
short _get_16(char *addr);
long _get_32(char *addr);
void _put_16(char *addr, short data);
void _put_32(char *addr, long data);
void gp_data_string(char *buffer);

#endif

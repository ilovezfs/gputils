/* Displays contents of ".COD" files
   Copyright (C) 2001 Scott Dattalo

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

#ifndef __DUMP_H__
#define __DUMP_H__

void dump_hex(char *chunk, int length);
void dump_memmap( void);
void dump_code(void);
void dump_symbols( void );
void dump_lsymbols( void );
void dump_source_files( void );
void dump_line_symbols(void);
void dump_message_area(void);
void dump_local_vars(void);

unsigned short get_short_int( char * buff);
char *substr(char *a, char *b, int n);

#endif

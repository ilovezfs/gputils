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

#ifndef __GPVC_H__
#define __GPVC_H__


#include "gpcod.h"

#define GPVC_VERSION_STRING ("gpvc-" VERSION " alpha")
#define BUFFER_LENGTH 256
#define BLOCK_SIZE    512

extern FILE *codefile;  
extern char filename[BUFFER_LENGTH];
extern char temp[BLOCK_SIZE];
extern char *source_file_names[20];
extern FILE *source_files[20];
extern DirBlockInfo main_dir;

extern int addrsize;

extern char directory_block_data[BLOCK_SIZE];
extern char * SymbolType4[154];

#endif

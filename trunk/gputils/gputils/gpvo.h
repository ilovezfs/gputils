/* gpvo - GNU PIC view object
   Copyright (C) 2001, 2002, 2003
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

#ifndef GPVO_H
#define GPVO_H

#define GPVO_VERSION_STRING ("gpvo-" VERSION " alpha")

/* Dump Flags */
#define PRINT_HEADER   1 << 1
#define PRINT_SECTIONS 1 << 2
#define PRINT_SYMTBL   1 << 3
#define PRINT_BINARY   1 << 4

extern struct gpvo_state {
  int quiet;			/* suppress outputs when 1 */ 
  int dump_flags; 		/* when bit set then print that section */	
  int suppress_names;		/* suppress filenames when 1 */ 
  char *filename;		/* object file name */
  gp_object_type *object;	/* formatted object file */
  gp_binary_type *file;		/* binary object file */
} state;

#endif

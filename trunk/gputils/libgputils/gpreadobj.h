/* Read coff objects
   Copyright (C) 2001 Craig Franklin

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

/* FIXME: Change the section data from int to char.  All addresses are chars.
Changing it will reduce the >> 1 in the source code */

struct section {
  struct scnhdr  header;
  unsigned int  *data;  
  struct reloc  *relocations;
  struct lineno *linenumbers;
};

struct objectfile {
  char            *filename;    /* object filename */
  struct filehdr  file_header;  /* file header */
  struct opthdr   opt_header;   /* optional header */
  struct section  *sections;
  struct syment   *symtbl;
  char            *strtbl;
};

struct binaryfile {
  long int        size;         /* size of the file in bytes */
  char           *file;         /* file contents */
};

unsigned short get_16(unsigned char *addr);
unsigned long get_32(unsigned char *addr);
struct binaryfile *readfile(char *filename, char *message);
struct objectfile *readobj(char *filename, char *message);
struct objectfile *convert_object(unsigned char *file, char *message);
void freeobj(struct objectfile *object);

enum coff_types { 
  archive, 
  object, 
  sys_err,
  unknown
};

enum coff_types identify_coff_file(char *filename);
void gp_fetch_symbol_name(struct objectfile *object, 
                          struct syment *coffsym,
			  char *string);
void gp_fetch_section_name(struct objectfile *object, 
                           struct scnhdr *header,
			   char *string);

#endif

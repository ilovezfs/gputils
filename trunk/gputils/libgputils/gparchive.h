/* GNU PIC archive functions
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

#ifndef __GPARCHIVE_H__
#define __GPARCHIVE_H__

/* gputils uses Microchip's library file format.  It is basically a coff 
   archive with a few exceptions.  

   - There are no user id and mode entries in the header.
   - The archive member name has been increased from 16 to 256 characters.
   - The strings for data and size are terminated with "l"
   - The first member in the archive is not a symbol table.

   The first 8 bytes in the archive are ARMAG.  This is followed by the 
   archive members.  Each member starts with an ar_hdr.  This is followed
   by the complete object file.
   
   gputils uses the complete object filename for the member name.

   The only intentional deviation from Microchip's format, is a symbol 
   index.  This index provides all global symbols names from the archive.
   This simplifies the task of determining which archive members are 
   required for linking.  
   
   The symbol index is always the first coff member in the archive.  It
   can be identified because ar_name[0] == '/'.

*/

/* Note that the usual '\n' in magic strings may translate to different
   characters, as allowed by ANSI.  '\012' has a fixed value. */

#define ARMAG  "!<arch>\012"
#define SARMAG 8
#define ARFMAG "`\012"

/* archive file member header */
struct ar_hdr 
{
  char ar_name[256];		/* name of this member */
  char ar_date[12];		/* file mtime */
  char ar_size[10];		/* file size, printed as decimal */
  char ar_fmag[2];		/* should contain ARFMAG */
};

#define AR_HDR_SIZ 280

struct coff_archive {
  struct ar_hdr        header; /* archive header file */
  char                *file;   /* object file */
  int                  offset; /* offset from the begining of the archive */
  struct coff_archive *next;   /* next file in linked list */
};

/* symbol index data */
#define AR_INDEX_NUMBER_SIZ 4  /* number of symbols is 4 bytes long */
#define AR_INDEX_OFFSET_SIZ 4  /* symbol index offsets are 4 bytes long */

int count_archive_members(struct coff_archive *archive);
void list_archive_members(struct coff_archive *archive);
struct coff_archive *find_archive_member(struct coff_archive *archive, 
                                         char *objectname);
int free_archive_member(struct coff_archive *archive);
struct coff_archive *delete_archive_member(struct coff_archive *archive, 
                                           char *objectname, 
                                           char *message);
struct coff_archive *add_archive_member(struct coff_archive *archive, 
                                        char *objectname, 
                                        char *message);
int extract_archive_member(struct coff_archive *archive, 
                           char *objectname, 
                           char *message);
int write_archive(struct coff_archive *archive, 
                  char *archivename, 
                  char *message);
struct coff_archive *read_coff_archive(char *filename, 
                                       char *message);

int gp_archive_make_index(struct coff_archive *archive, 
                          struct symbol_table *,
			  char *message);
struct coff_archive *gp_archive_add_index(struct symbol_table *table,
                                          struct coff_archive *archive, 
                                          char *message);
void gp_archive_read_index(struct symbol_table *table,
                           struct coff_archive *archive);
void gp_archive_print_table(struct symbol_table *table);
int gp_archive_have_index(struct coff_archive *archive);
struct coff_archive *gp_archive_remove_index(struct coff_archive *archive);
char *gp_archive_member_name(struct coff_archive *archive);

#endif

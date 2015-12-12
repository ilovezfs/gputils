/* GNU PIC archive functions
   Copyright (C) 2001, 2002, 2003, 2004, 2005
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

#ifndef __GPARCHIVE_H__
#define __GPARCHIVE_H__

#include "gpreadobj.h"

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
  char ar_name[256];            /* name of this member */
  char ar_date[12];             /* file mtime */
  char ar_size[10];             /* file size, printed as decimal */
  char ar_fmag[2];              /* should contain ARFMAG */
};

#define AR_HDR_SIZ 280

typedef struct gp_archive_type {
  struct ar_hdr           header; /* archive header file */
  gp_binary_type          data;   /* binary data */
  int                     offset; /* offset from the begining of the archive */
  struct gp_archive_type *next;   /* next file in linked list */
} gp_archive_type;

/* symbol index data */
#define AR_INDEX_NUMBER_SIZ 4  /* number of symbols is 4 bytes long */
#define AR_INDEX_OFFSET_SIZ 4  /* symbol index offsets are 4 bytes long */

int gp_archive_count_members(const gp_archive_type *archive);
char *gp_archive_member_name(const gp_archive_type *archive);
void gp_archive_list_members(const gp_archive_type *archive);
gp_archive_type *gp_archive_find_member(gp_archive_type *archive, const char *objectname);
void gp_archive_free_member(gp_archive_type *archive);
gp_archive_type *gp_archive_delete_member(gp_archive_type *archive, const char *objectname);
gp_archive_type *gp_archive_add_member(gp_archive_type *archive,
                                       const char *filename,
                                       const char *objectname);
int gp_archive_extract_member(gp_archive_type *archive, const char *objectname);
int gp_archive_write(gp_archive_type *archive, const char *archivename);
void gp_archive_update_offsets(gp_archive_type *archive);
gp_archive_type *gp_archive_read(const char *filename);
gp_boolean gp_archive_have_index(const gp_archive_type *archive);
gp_archive_type *gp_archive_remove_index(gp_archive_type *archive);
int gp_archive_make_index(gp_archive_type *archive,
                          struct symbol_table *);
gp_archive_type *gp_archive_add_index(struct symbol_table *table,
                                      gp_archive_type *archive);
gp_boolean gp_archive_add_symbol(struct symbol_table *table,
                          const char *name,
                          gp_archive_type *member);
void gp_archive_read_index(struct symbol_table *table,
                           gp_archive_type *archive);
void gp_archive_print_table(struct symbol_table *table);

#endif

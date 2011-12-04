/* GNU PIC object symbol strip
   Copyright (C) 2005
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

#ifndef GPSTRIP_H
#define GPSTRIP_H

#define GPSTRIP_VERSION_STRING ("gpstrip-" VERSION " (" __DATE__ ")")

extern struct gpstrip_state {
  gp_boolean strip_debug;               /* strip debug symbols */
  gp_boolean preserve_dates;            /* preserve dates */
  gp_boolean strip_all;                 /* remove all symbols */
  gp_boolean strip_unneeded;            /* Strip symbols not need for relocations */
  gp_boolean discard_all;               /* Remove non-global symbols */
  char *input_file;                     /* input file name */
  char *output_file;                    /* output file name */
  struct symbol_table *symbol_keep;     /* force these symbols to be keep */
  struct symbol_table *symbol_remove;   /* force these symbols to be removed */
  struct symbol_table *section_remove;  /* remove these sections */
  gp_object_type *object;               /* formatted object file */
} state;

#endif

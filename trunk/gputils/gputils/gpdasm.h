/* Disassembles ".HEX" files
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

#ifndef __GPDASM_H__
#define __GPDASM_H__

#define GPDASM_VERSION_STRING ("gpdasm-" VERSION " alpha")

extern struct gpdasm_state {
  enum pic_processor processor; /* selected processor */
  enum proc_class class;        /* processor class */
  int format;                   /* format of the output */
  int pass;                     /* 1 or 2 */
  MemBlock *i_memory;           /* Instruction memory linked list */
  char *srcfilename,            /* Source (.asm) file name */
    basefilename[BUFSIZ],       /* basename for generating hex,list,symbol filenames */
    hexfilename[BUFSIZ];        /* Hex (.hex) file name */
  struct hex_data *hex_info;    /* information on input hex file */
  struct {			/* Totals for errors, warnings, messages */
    int errors;
    int warnings;
    int messages;
  } num;
} state;

#endif

/* Convert one word from memory into an equivalent assembly instruction
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

#ifndef __GPDIS_H__
#define __GPDIS_H__

extern gp_boolean gp_decode_mnemonics;
extern gp_boolean gp_decode_extended;

int gp_disassemble(MemBlock *m,
                   int byte_address,
                   proc_class_t class,
                   char *buffer,
                   size_t sizeof_buffer);
int gp_disassemble_byte(MemBlock *m,
                    int byte_address,
                    proc_class_t class,
                    char *buffer,
                    size_t sizeof_buffer);
int gp_disassemble_word(MemBlock *m,
                    int byte_address,
                    proc_class_t class,
                    char *buffer,
                    size_t sizeof_buffer);
int gp_disassemble_size(MemBlock *m,
                    int byte_address,
                    proc_class_t class,
                    char *buffer,
                    size_t sizeof_buffer,
                    unsigned int size);

#endif

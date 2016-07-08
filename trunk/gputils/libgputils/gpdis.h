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

#include "gpregister.h"

extern gp_boolean gp_decode_mnemonics;
extern gp_boolean gp_decode_extended;

typedef struct {
  int                        wreg;
  int                        pclath;
  unsigned int               pclath_valid;
  int                        bank;
  unsigned int               bank_valid;
  const gp_register_table_t *proc_regs;
  unsigned int               bsr_boundary;      /* Only case of PIC16E. */
  gp_boolean                 need_sfr_equ;
} gpdasm_fstate_t;

/* Values of the "Behavior". */
#define GPDIS_SHOW_NOTHING      0
#define GPDIS_SHOW_NAMES        (1 << 0)
#define GPDIS_SHOW_FSRN         (1 << 1)
#define GPDIS_SHOW_BYTES        (1 << 2)
#define GPDIS_SHOW_ALL_BRANCH   (1 << 3)
#define GPDIS_SHOW_EXCLAMATION  (1 << 4)

extern unsigned int gp_disassemble_mark_false_addresses(MemBlock_t *M, unsigned int Byte_address,
                                                        pic_processor_t Processor);

extern unsigned int gp_disassemble_find_labels(MemBlock_t *M, unsigned int Byte_address,
                                               pic_processor_t Processor, gpdasm_fstate_t *Fstate);

extern unsigned int gp_disassemble_find_registers(MemBlock_t *M, unsigned int Byte_address,
                                                  pic_processor_t Processor, gpdasm_fstate_t *Fstate,
                                                  void (*User_data_finder)(MemArg_t *));

extern void gp_disassemble_show_data(MemBlock_t *M, unsigned int Byte_address, proc_class_t Class,
                                     unsigned int Behavior, char *Buffer, size_t Buffer_length,
                                     size_t Current_length);

extern unsigned int gp_disassemble(MemBlock_t *M, unsigned int Byte_address, proc_class_t Class,
                                   unsigned int Bsr_boundary, unsigned int Prog_mem_size,
                                   unsigned int Behavior, char *Buffer, size_t Buffer_length,
                                   size_t Current_length);

extern unsigned int gp_disassemble_byte(MemBlock_t *M, unsigned int Byte_address,
                                        char *Buffer, size_t Buffer_length);

extern unsigned int gp_disassemble_word(MemBlock_t *M, unsigned int Byte_address, proc_class_t Class,
                                        char *Buffer, size_t Buffer_length);

extern unsigned int gp_disassemble_size(MemBlock_t *M, unsigned int Byte_address, proc_class_t Class,
                                        unsigned int Bsr_boundary, unsigned int Prog_mem_size,
                                        unsigned int Behavior, char *Buffer, size_t Buffer_length,
                                        unsigned int Size);
#endif

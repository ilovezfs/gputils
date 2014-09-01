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
  int wreg;
  int pclath;
  int pclath_valid;
  int bank;
  int bank_valid;
  const gp_register_table_t *proc_regs;
  int bsr_boundary;                     /* Only case of PIC16E. */
  gp_boolean need_sfr_equ;
} gpdasm_fstate_t;

/* Values of the "behavior". */
#define GPDIS_SHOW_NOTHING      0
#define GPDIS_SHOW_NAMES        (1 << 0)
#define GPDIS_SHOW_FSRN         (1 << 1)
#define GPDIS_SHOW_BYTES        (1 << 2)
#define GPDIS_SHOW_ALL_BRANCH   (1 << 3)
#define GPDIS_SHOW_EXCLAMATION  (1 << 4)

int gp_disassemble_mark_false_addresses(MemBlock *m, int byte_address, pic_processor_t processor);

int gp_disassemble_find_labels(MemBlock *m, int byte_address, pic_processor_t processor,
                               gpdasm_fstate_t *fstate);

int gp_disassemble_find_registers(MemBlock *m, int byte_address, pic_processor_t processor,
                                  gpdasm_fstate_t *fstate, void (*user_data_finder)(MemArg *));

void gp_disassemble_show_data(MemBlock *m, int byte_address, proc_class_t class, int behavior,
                              char *buffer, size_t buffer_length, size_t current_length);

int gp_disassemble(MemBlock *m, int byte_address, proc_class_t class, int bsr_boundary,
                   int prog_mem_size, int behavior, char *buffer, size_t buffer_length, size_t current_length);

int gp_disassemble_byte(MemBlock *m, int byte_address, proc_class_t class,
                        char *buffer, size_t buffer_length);

int gp_disassemble_word(MemBlock *m, int byte_address, proc_class_t class,
                        char *buffer, size_t buffer_length);

int gp_disassemble_size(MemBlock *m, int byte_address, proc_class_t class, int bsr_boundary,
                        int prog_mem_size, int behavior, char *buffer, size_t buffer_length,
                        unsigned int size);
#endif

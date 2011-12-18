/* GNU PIC opcode definitions
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

#ifndef __GPOPCODE_H__
#define __GPOPCODE_H__

enum insn_class {
  INSN_CLASS_LIT1,      /* bit 0 contains a 1 bit literal               */
  INSN_CLASS_LIT4S,     /* Bits 7:4 contain a 4 bit literal, bits 3:0 are unused   */
  INSN_CLASS_LIT5,      /* bits 4:0 contain an 4 bit literal            */
  INSN_CLASS_LIT6,      /* bits 5:0 contain an 6 bit literal            */
  INSN_CLASS_LIT7,      /* bits 6:0 contain an 7 bit literal            */
  INSN_CLASS_LIT8,      /* bits 7:0 contain an 8 bit literal            */
  INSN_CLASS_LIT8C12,   /* bits 7:0 contain an 8 bit literal, 12 bit CALL */
  INSN_CLASS_LIT8C16,   /* bits 7:0 contain an 8 bit literal, 16 bit CALL */
  INSN_CLASS_LIT9,      /* bits 8:0 contain a 9 bit literal             */
  INSN_CLASS_LIT11,     /* bits 10:0 contain an 11 bit literal          */
  INSN_CLASS_LIT13,     /* bits 12:0 contain an 11 bit literal          */
  INSN_CLASS_LITFSR_14, /* bits 5:0 contain an 6 bit literal for fsr (14bit enh. cores) */
  INSN_CLASS_LITFSR_16, /* bits 5:0 contain an 6 bit literal for fsr 7:6 (16bit cores)  */
  INSN_CLASS_IMPLICIT,  /* instruction has no variable bits at all      */
  INSN_CLASS_OPF5,      /* bits 4:0 contain a register address          */
  INSN_CLASS_OPWF5,     /* as above, but bit 5 has a destination flag   */
  INSN_CLASS_B5,        /* as for OPF5, but bits 7:5 have bit number    */
  INSN_CLASS_OPF7,      /* bits 6:0 contain a register address          */
  INSN_CLASS_OPWF7,     /* as above, but bit 7 has destination flag     */
  INSN_CLASS_B7,        /* as for OPF7, but bits 9:7 have bit number    */

  INSN_CLASS_OPF8,      /* bits 7:0 contain a register address  */
  INSN_CLASS_OPFA8,     /* bits 7:0 contain a register address & bit has access flag  */
  INSN_CLASS_OPWF8,     /* as above, but bit 8 has dest flag    */
  INSN_CLASS_OPWFA8,    /* as above, but bit 9 has dest flag & bit 8 has access flag            */
  INSN_CLASS_B8,        /* like OPF7, but bits 9:11 have bit number     */
  INSN_CLASS_BA8,       /* like OPF7, but bits 9:11 have bit number & bit 8 has access flag     */
  INSN_CLASS_LIT20,     /* 20bit lit, bits 7:0 in first word bits 19:8 in second                */
  INSN_CLASS_CALL20,    /* Like LIT20, but bit 8 has fast push flag                             */
  INSN_CLASS_RBRA8,     /* Bits 7:0 contain a relative branch address                           */
  INSN_CLASS_RBRA9,     /* Bits 8:0 contain a relative branch address                           */
  INSN_CLASS_RBRA11,    /* Bits 10:0 contain a relative branch address                          */
  INSN_CLASS_FLIT12,    /* LFSR, 12bit lit loaded into 1 of 4 FSRs                              */
  INSN_CLASS_FF,        /* two 12bit file addresses                                             */
  INSN_CLASS_FP,        /* Bits 7:0 contain a register address, bits 12:8 contains a peripheral address    */
  INSN_CLASS_PF,        /* Bits 7:0 contain a register address, bits 12:8 contains a peripheral address    */

  INSN_CLASS_SF,        /* 7 bit offset added to FSR2, fetched memory placed at 12 bit address */
  INSN_CLASS_SS,        /* two 7 bit offsets, memory moved using FSR2 */

  INSN_CLASS_TBL,       /* a table read or write instruction            */
  INSN_CLASS_TBL2,      /* a table read or write instruction.
                           Bits 7:0 contains a register address; Bit 8 is unused;
                           Bit 9, table byte select. (0:lower ; 1:upper) */
  INSN_CLASS_TBL3,      /* a table read or write instruction.
                           Bits 7:0 contains a register address;
                           Bit 8, 1 if increment pointer, 0 otherwise;
                           Bit 9, table byte select. (0:lower ; 1:upper) */
  INSN_CLASS_FUNC,      /* instruction is an assembler function         */
  INSN_CLASS_LIT3_BANK, /* SX: bits 3:0 contain a 3 bit literal, shifted 5 bits */
  INSN_CLASS_LIT3_PAGE, /* SX: bits 3:0 contain a 3 bit literal, shifted 9 bits */
  INSN_CLASS_LIT4,      /* SX: bits 3:0 contain a 4 bit literal         */
  INSN_CLASS_MOVINDF    /* enhanced 14-bit moviw and movwi insn arguments */
};

typedef int gpasmVal;   /* The type that internal arithmetic uses */

struct pnode;           /* forward declaration; defined in
                         * gpasm.h for assembler and in
                         * script.h for linker */

struct insn {
  char *name;
  long int mask;
  long int opcode;
  enum insn_class class;
  int attribs;
  gpasmVal (*doer)(gpasmVal r, char *name, int arity, struct pnode *parms);
};

struct strict_insn {
  char *name;
  long int mask;
};

#define TABLE_SIZE(X)       (sizeof(X) / sizeof(X[0]))

extern const struct insn op_12c5xx[];
extern const int num_op_12c5xx;

extern const struct insn op_sx[];
extern const int num_op_sx;

extern struct insn op_16cxx[];
extern const int num_op_16cxx;

extern const struct strict_insn op_16cxx_strict_mask[];
extern const int num_op_16cxx_strict_mask;

extern const struct insn op_16cxx_enh[];
extern const int num_op_16cxx_enh;

extern const struct insn op_17cxx[];
extern const int num_op_17cxx;

extern const struct insn op_18cxx[];
extern const int num_op_18cxx;

extern const struct insn op_18cxx_sp[];
extern const int num_op_18cxx_sp;

extern const struct insn op_18cxx_ext[];
extern const int num_op_18cxx_ext;

#endif

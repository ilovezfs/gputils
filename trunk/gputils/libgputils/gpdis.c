/* Disassemble memory
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

#include "stdhdr.h"
#include "libgputils.h"

#define DECODE_ARG0 \
        snprintf(buffer, sizeof_buffer, "%s", instruction->name)

#define DECODE_ARG1_N(ARG1) \
        snprintf(buffer, sizeof_buffer, "%s\t%#x", instruction->name, ARG1)

#define DECODE_ARG1_S(ARG1) \
        snprintf(buffer, sizeof_buffer, "%s\t%s", instruction->name, ARG1)

#define DECODE_ARG2_N_N(ARG1, ARG2) \
        snprintf(buffer, sizeof_buffer, "%s\t%#x, %#x", instruction->name, ARG1, ARG2)

#define DECODE_ARG2_N_S(ARG1, ARG2) \
        snprintf(buffer, sizeof_buffer, "%s\t%#x, %s", instruction->name, ARG1, ARG2)

#define DECODE_ARG2_S_N(ARG1, ARG2) \
        snprintf(buffer, sizeof_buffer, "%s\t%s, %#x", instruction->name, ARG1, ARG2)

#define DECODE_ARG2_S_S(ARG1, ARG2) \
        snprintf(buffer, sizeof_buffer, "%s\t%s, %s", instruction->name, ARG1, ARG2)

#define DECODE_ARG3_N_N_N(ARG1, ARG2, ARG3) \
        snprintf(buffer, sizeof_buffer, "%s\t%#x, %#x, %#x", instruction->name, ARG1, ARG2, ARG3)

#define DECODE_ARG3_N_N_S(ARG1, ARG2, ARG3) \
        snprintf(buffer, sizeof_buffer, "%s\t%#x, %#x, %s", instruction->name, ARG1, ARG2, ARG3)

#define DECODE_ARG3_N_S_S(ARG1, ARG2, ARG3) \
        snprintf(buffer, sizeof_buffer, "%s\t%#x, %s, %s", instruction->name, ARG1, ARG2, ARG3)

#define DECODE_ARG3_S_N_S(ARG1, ARG2, ARG3) \
        snprintf(buffer, sizeof_buffer, "%s\t%s, %#x, %s", instruction->name, ARG1, ARG2, ARG3)

#define DECODE_ARG3_S_S_S(ARG1, ARG2, ARG3) \
        snprintf(buffer, sizeof_buffer, "%s\t%s, %s, %s", instruction->name, ARG1, ARG2, ARG3)

#define DECODE_MOVINDF_S_N_S(ARG1, ARG2, ARG3) \
        snprintf(buffer, sizeof_buffer, "%s\t%s%#x%s", instruction->name, ARG1, ARG2, ARG3)

#define DECODE_MOVINDF_S_S_S(ARG1, ARG2, ARG3) \
        snprintf(buffer, sizeof_buffer, "%s\t%s%s%s", instruction->name, ARG1, ARG2, ARG3)

#define FLAG_FW(Flag)   ((Flag) ? "f" : "w")
#define FLAG_BA(Flag)   ((Flag) ? "b" : "a")

gp_boolean gp_decode_mnemonics = false;
gp_boolean gp_decode_extended = false;

int
gp_disassemble(MemBlock *m,
               int byte_address,
               proc_class_t class,
               int bsr_boundary,
               gp_boolean show_core_sfrs,
               char *buffer,
               size_t sizeof_buffer)
{
  int value;
  unsigned short opcode;
  const struct insn *instruction = NULL;
  int num_words = 1;
  unsigned short file1;
  unsigned short file2;
  unsigned int tmp;
  unsigned int ram_acc;
  const core_sfr_t *sfr;

  if (class->find_insn == NULL) {
    snprintf(buffer, sizeof_buffer, "unsupported processor class");
    return 0;
  }

  class->i_memory_get(m, byte_address, &opcode, NULL, NULL);

  /* Special case for pic14 enhanced moviw k[FSRn] & movwi k[FSRn]. */
  if (class == PROC_CLASS_PIC14E) {
    const char *instr = NULL;

    tmp = opcode & 0x3f80;

    if (tmp == INSN_PIC14E_MOVIW_IDX) {
      instr = "moviw";
    }
    else if (tmp == INSN_PIC14E_MOVWI_IDX) {
      instr = "movwi";
    }

    if (instr != NULL) {
      /* twos complement number */
      const char *neg;

      value = opcode & 0x3f;

      if (value & 0x20) {
        value = (value ^ 0x3f) + 1;
        neg = "-";
      } else {
        neg = "";
      }

      if (show_core_sfrs) {
        snprintf(buffer, sizeof_buffer, "%s\t%s.%d[%s]", instr, neg, value,
                 (opcode & 0x40) ? "FSR1" : "FSR0");
      }
      else {
        snprintf(buffer, sizeof_buffer, "%s\t%s.%d[%u]", instr, neg, value,
                 (opcode & 0x40) ? 6 : 4);
      }

      return num_words;
    }
  }

  instruction = class->find_insn(class, opcode);

  if (instruction == NULL)  {
    snprintf(buffer, sizeof_buffer, "dw\t%#x", opcode);
    return num_words;
  }

  switch (instruction->class)
    {
    case INSN_CLASS_LIT3_BANK:
      /* SX bank */
      DECODE_ARG1_N((opcode & 0x7) << 5);
      break;

    case INSN_CLASS_LIT3_PAGE:
      /* SX page */
      DECODE_ARG1_N((opcode & 0x7) << 9);
      break;

    case INSN_CLASS_LIT1:
      /* PIC16E (retfie, return) */
      DECODE_ARG1_N(opcode & 1);
      break;

    case INSN_CLASS_LIT3:
      /* PIC12E movlb */
      DECODE_ARG1_N(opcode & 0x07);
      break;

    case INSN_CLASS_LIT4:
      /* SX mode */
      DECODE_ARG1_N(opcode & 0x0f);
      break;

    case INSN_CLASS_LIT4S:
      /* PIC16 movlr */
      DECODE_ARG1_N((opcode & 0xf0) >> 4);
      break;

    case INSN_CLASS_LIT5:
      /* PIC14E movlb */
      DECODE_ARG1_N(opcode & MASK_PIC14E_BANK);
      break;

    case INSN_CLASS_LIT6:
      /* PIC16E (addulnk, subulnk) */
      DECODE_ARG1_N(opcode & 0x3f);
      break;

    case INSN_CLASS_LIT7:
      /* PIC14E movlp */
      DECODE_ARG1_N(opcode & MASK_PIC14E_PAGE);
      break;

    case INSN_CLASS_LIT8:
      /* PIC1xx (addlw, andlw, iorlw, movlw, retlw, sublw, xorlw), PIC16x movlb, PIC16x mullw, PIC16E pushl */
    case INSN_CLASS_LIT8C12:
      /* PIC12x call, SX call */
    case INSN_CLASS_LIT8C16:
      /* PIC16 lcall */
      DECODE_ARG1_N(opcode & 0xff);
      break;

    case INSN_CLASS_LIT9:
      /* PIC12 goto, SX goto */
      DECODE_ARG1_N(opcode & MASK_PIC12_GOTO);
      break;

    case INSN_CLASS_LIT11:
      /* PIC14x (call, goto) */
      DECODE_ARG1_N(opcode & MASK_PIC14_BRANCH);
      break;

    case INSN_CLASS_LIT13:
      /* PIC16 (call, goto) */
      DECODE_ARG1_N(opcode & MASK_PIC16_BRANCH);
      break;

    case INSN_CLASS_LITFSR_14:
      /* PIC14E addfsr */
      {
        const char *neg;

        value = opcode & 0x3f;

        if (value & 0x20) {
          value = (value ^ 0x3f) + 1;
          neg = "-";
        } else {
          neg = "";
        }

        if (show_core_sfrs) {
          snprintf(buffer, sizeof_buffer, "%s\t%s, %s.%d", instruction->name,
                   (opcode & 0x40) ? "FSR1" : "FSR0", neg, value);
        }
        else {
          snprintf(buffer, sizeof_buffer, "%s\t%u, %s.%d", instruction->name,
                   (opcode & 0x40) ? 6 : 4, neg, value);
        }
      }
      break;

    case INSN_CLASS_LITFSR_16:
      /* PIC16E (addfsr, subfsr) */
      DECODE_ARG2_N_N(((opcode >> 6) & 0x3), (opcode & 0x3f));
      break;

    case INSN_CLASS_RBRA8:
      /* PIC16E (bc, bn, bnc, bnn, bnov, bnz, bov, bz) */
      value = opcode & 0xff;
      /* twos complement number */
      if (value & 0x80) {
        value = -((value ^ 0xff) + 1);
      }

      value = gp_processor_byte_to_org(class, byte_address + value * 2 + 2);
      DECODE_ARG1_N(value);
      break;

    case INSN_CLASS_RBRA9:
      /* PIC14E bra */
      value = opcode & MASK_PIC14E_RBRA9;
      /* twos complement number */
      if (value & 0x100) {
        value = -((value ^ MASK_PIC14E_RBRA9) + 1);
      }

      value = gp_processor_byte_to_org(class, byte_address + value * 2 + 2);
      DECODE_ARG1_N(value);
      break;

    case INSN_CLASS_RBRA11:
      /* PIC16E (bra, rcall) */
      value = opcode & MASK_PIC16E_RBRA11;
      /* twos complement number */
      if (value & 0x400) {
        value = -((value ^ MASK_PIC16E_RBRA11) + 1);
      }

      value = gp_processor_byte_to_org(class, byte_address + value * 2 + 2);
      DECODE_ARG1_N(value);
      break;

    case INSN_CLASS_LIT20:
      /* PIC16E goto */
      {
        unsigned short dest;

        num_words = 2;
        class->i_memory_get(m, byte_address + 2, &dest, NULL, NULL);
        dest = (dest & MASK_PIC16E_BRANCH_HIGHER) << 8;
        dest |= opcode & MASK_PIC16E_BRANCH_LOWER;
        DECODE_ARG1_N(gp_processor_byte_to_org(class, dest * 2));
      }
      break;

    case INSN_CLASS_CALL20:
      /* PIC16E call */
      {
        unsigned short dest;

        num_words = 2;
        class->i_memory_get(m, byte_address + 2, &dest, NULL, NULL);
        dest  = (dest & MASK_PIC16E_BRANCH_HIGHER) << 8;
        dest |= opcode & MASK_PIC16E_BRANCH_LOWER;
        DECODE_ARG2_N_N(gp_processor_byte_to_org(class, dest * 2), (opcode >> 8) & 1);
      }
      break;

    case INSN_CLASS_FLIT12:
      /* PIC16E lfsr */
      {
        unsigned short k;

        num_words = 2;
        class->i_memory_get(m, byte_address + 2, &k, NULL, NULL);
        k = ((opcode & 0xf) << 8) | (k & 0xff);
        file1 = (opcode >> 4) & 3;
        DECODE_ARG2_N_N(file1, k);
      }
      break;

    case INSN_CLASS_FF:
      /* PIC16E movff */
      {
        num_words = 2;
        file1 = opcode & 0xfff;
        class->i_memory_get(m, byte_address + 2, &file2, NULL, NULL);
        file2 &= 0xfff;

        if (show_core_sfrs) {
          const core_sfr_t *sfr2;

          sfr  = gp_processor_find_sfr(class, file1);
          sfr2 = gp_processor_find_sfr(class, file2);

          if (sfr != NULL) {
            if (sfr2 != NULL) {
              DECODE_ARG2_S_S(sfr->name, sfr2->name);
            }
            else {
              DECODE_ARG2_S_N(sfr->name, file2);
            }
          }
          else {
            if (sfr2 != NULL) {
              DECODE_ARG2_N_S(file1, sfr2->name);
            }
            else {
              DECODE_ARG2_N_N(file1, file2);
            }
          }
        }
        else {
          DECODE_ARG2_N_N(file1, file2);
        }
      }
      break;

    case INSN_CLASS_FP:
      /* PIC16 movfp */
      {
        file1 = opcode & MASK_PIC16_FILE;
        file2 = (opcode >> 8) & 0x1f;

        if (show_core_sfrs) {
          const core_sfr_t *sfr2;

          sfr  = gp_processor_find_sfr(class, file1);
          sfr2 = gp_processor_find_sfr(class, file2);

          if (sfr != NULL) {
            if (sfr2 != NULL) {
              DECODE_ARG2_S_S(sfr->name, sfr2->name);
            }
            else {
              DECODE_ARG2_S_N(sfr->name, file2);
            }
          }
          else {
            if (sfr2 != NULL) {
              DECODE_ARG2_N_S(file1, sfr2->name);
            }
            else {
              DECODE_ARG2_N_N(file1, file2);
            }
          }
        }
        else {
          DECODE_ARG2_N_N(file1, file2);
        }
      }
      break;

    case INSN_CLASS_PF:
      /* PIC16 movpf */
      {
        file1 = (opcode >> 8) & 0x1f;
        file2 = opcode & MASK_PIC16_FILE;

        if (show_core_sfrs) {
          const core_sfr_t *sfr2;

          sfr  = gp_processor_find_sfr(class, file1);
          sfr2 = gp_processor_find_sfr(class, file2);

          if (sfr != NULL) {
            if (sfr2 != NULL) {
              DECODE_ARG2_S_S(sfr->name, sfr2->name);
            }
            else {
              DECODE_ARG2_S_N(sfr->name, file2);
            }
          }
          else {
            if (sfr2 != NULL) {
              DECODE_ARG2_N_S(file1, sfr2->name);
            }
            else {
              DECODE_ARG2_N_N(file1, file2);
            }
          }
        }
        else {
          DECODE_ARG2_N_N(file1, file2);
        }
      }
      break;

    case INSN_CLASS_SF:
      /* PIC16E movsf */
      {
        unsigned short offset;

        num_words = 2;
        offset = opcode & 0x7f;
        class->i_memory_get(m, byte_address + 2, &file1, NULL, NULL);
        file1 &= 0xfff;
        sfr    = ((show_core_sfrs) && (bsr_boundary > 0) && (file1 >= (0xF00 + bsr_boundary))) ?
                      gp_processor_find_sfr(class, file1) : NULL;

        if (sfr != NULL) {
          DECODE_ARG2_N_S(offset, sfr->name);
        }
        else {
          DECODE_ARG2_N_N(offset, file1);
        }
      }
      break;

    case INSN_CLASS_SS:
      /* PIC16E movss */
      {
        unsigned short offset2;

        num_words = 2;
        class->i_memory_get(m, byte_address + 2, &offset2, NULL, NULL);
        DECODE_ARG2_N_N(opcode & 0x7f, offset2 & 0x7f);
      }
      break;

    case INSN_CLASS_OPF3:
      /* PIC12 tris */
      DECODE_ARG1_N(opcode & MASK_PIC12_TRIS);
      break;

    case INSN_CLASS_OPF5:
      /* {PIC12x, SX} (clrf, movwf), SX tris */
      file1 = opcode & MASK_PIC12_FILE;
      sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG1_S(sfr->name);
      }
      else {
        DECODE_ARG1_N(file1);
      }
      break;

    case INSN_CLASS_OPWF5:
      /* {PIC12E, SX} (addwf, andwf, comf, decf, decfsz, incf, incfsz, iorwf, movf, rlf, rrf, subwf, swapf, xorwf) */
      file1 = opcode & MASK_PIC12_FILE;
      tmp   = (opcode >> 5) & 1;
      sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_S(sfr->name, FLAG_FW(tmp));
      }
      else if (show_core_sfrs) {
        DECODE_ARG2_N_S(file1, FLAG_FW(tmp));
      }
      else {
        DECODE_ARG2_N_N(file1, tmp);
      }
      break;

    case INSN_CLASS_B5:
      /* {PIC12x, SX} (bcf, bsf, btfsc, btfss) */
      file1 = opcode & MASK_PIC12_FILE;
      tmp   = (opcode >> 5) & 7;
      sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_N(sfr->name, tmp);
      }
      else {
        DECODE_ARG2_N_N(file1, tmp);
      }
      break;

    case INSN_CLASS_B8:
      /* PIC16 (bcf, bsf, btfsc, btfss, btg) */
      file1 = opcode & MASK_PIC16_FILE;
      tmp   = (opcode >> 8) & 7;
      sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_N(sfr->name, tmp);
      }
      else {
        DECODE_ARG2_N_N(file1, tmp);
      }
      break;

    case INSN_CLASS_OPF7:
      /* PIC14x (clrf, movwf, tris) */
      file1 = opcode & MASK_PIC14_FILE;
      sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG1_S(sfr->name);
      }
      else {
        DECODE_ARG1_N(file1);
      }
      break;

    case INSN_CLASS_OPF8:
      /* PIC16 (cpfseq, cpfsgt, cpfslt, movwf, mulwf, tstfsz) */
      file1 = opcode & MASK_PIC16_FILE;
      sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG1_S(sfr->name);
      }
      else {
        DECODE_ARG1_N(file1);
      }
      break;

    case INSN_CLASS_OPWF7:
      /* PIC14x (addwf, andwf, comf, decf, decfsz, incf, incfsz, iorwf, movf, rlf, rrf, subwf, swapf, xorwf)
         PIC14E (addwfc, asrf, lslf, lsrf, subwfb) */
      file1 = opcode & MASK_PIC14_FILE;
      tmp   = (opcode >> 7) & 1;
      sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_S(sfr->name, FLAG_FW(tmp));
      }
      else if (show_core_sfrs) {
        DECODE_ARG2_N_S(file1, FLAG_FW(tmp));
      }
      else {
        DECODE_ARG2_N_N(file1, tmp);
      }
      break;

    case INSN_CLASS_OPWF8:
      /* PIC16 (addwf, addwfc, andwf, clrf, comf, daw, decf, decfsz, dcfsnz, incf,
                incfsz, infsnz, iorwf, rlcf, rlncf, rrcf, rrncf, setf, subwf, subwfb,
                swapf, xorwf) */
      file1 = opcode & MASK_PIC16_FILE;
      tmp   = (opcode >> 8) & 1;
      sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_S(sfr->name, FLAG_FW(tmp));
      }
      else if (show_core_sfrs) {
        DECODE_ARG2_N_S(file1, FLAG_FW(tmp));
      }
      else {
        DECODE_ARG2_N_N(file1, tmp);
      }
      break;

    case INSN_CLASS_B7:
      /* PIC14x (bcf, bsf, btfsc, btfss) */
      file1 = opcode & MASK_PIC14_FILE;
      tmp   = (opcode >> 7) & 7;
      sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_N(sfr->name, tmp);
      }
      else {
        DECODE_ARG2_N_N(file1, tmp);
      }
      break;

    case INSN_CLASS_OPFA8:
      /* PIC16E (clrf, cpfseq, cpfsgt, cpfslt, movwf, mulwf, negf, setf, tstfsz) */
      file1   = opcode & MASK_PIC16_FILE;
      ram_acc = (opcode >> 8) & 1;
      sfr     = ((show_core_sfrs) && (ram_acc == 0) && (bsr_boundary > 0) && (file1 >= bsr_boundary)) ?
                     gp_processor_find_sfr(class, 0xF00 + file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_S(sfr->name, FLAG_BA(ram_acc));
      }
      else if (show_core_sfrs) {
        DECODE_ARG2_N_S(file1, FLAG_BA(ram_acc));
      }
      else {
        DECODE_ARG2_N_N(file1, ram_acc);
      }
      break;

    case INSN_CLASS_BA8:
      /* PIC16E (bcf, bsf, btfsc, btfss, btg) */
      file1   = opcode & MASK_PIC16_FILE;
      tmp     = (opcode >> 9) & 7;
      ram_acc = (opcode >> 8) & 1;
      sfr     = ((show_core_sfrs) && (ram_acc == 0) && (bsr_boundary > 0) && (file1 >= bsr_boundary)) ?
                     gp_processor_find_sfr(class, 0xF00 + file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG3_S_N_S(sfr->name, tmp, FLAG_BA(ram_acc));
      }
      else if (show_core_sfrs) {
        DECODE_ARG3_N_N_S(file1, tmp, FLAG_BA(ram_acc));
      }
      else {
        DECODE_ARG3_N_N_N(file1, tmp, ram_acc);
      }
      break;

    case INSN_CLASS_OPWFA8:
      /* PIC16E (addwf, addwfc, andwf, comf, decf, decfsz, dcfsnz, incf, incfsz,
                 infsnz, iorwf, movf, rlcf, rlncf, rrcf, rrncf, subfwb, subwf,
                 subwfb, swapf, xorwf) */
      file1   = opcode & MASK_PIC16_FILE;
      tmp     = (opcode >> 9) & 1;
      ram_acc = (opcode >> 8) & 1;
      sfr     = ((show_core_sfrs) && (ram_acc == 0) && (bsr_boundary > 0) && (file1 >= bsr_boundary)) ?
                     gp_processor_find_sfr(class, 0xF00 + file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG3_S_S_S(sfr->name, FLAG_FW(tmp), FLAG_BA(ram_acc));
      }
      else if (show_core_sfrs) {
        DECODE_ARG3_N_S_S(file1, FLAG_FW(tmp), FLAG_BA(ram_acc));
      }
      else {
        DECODE_ARG3_N_N_N(file1, tmp, ram_acc);
      }
      break;

    case INSN_CLASS_IMPLICIT:
      /* PIC12x  (clrw, clrwdt, nop, option, return, sleep)
         PIC12E  (retfie, return)
         SX      (iread, movmw, movwm, reti, retiw, retp, return)
         PIC14x  (clrw, clrwdt, halt, nop, option, retfie, return, sleep)
         PIC14E  (brw, callw, reset)
         PIC16   (clrwdt, nop, retfie, return, sleep)
         PIC16E  (clrwdt, daw, halt, nop, pop, push, reset, sleep, trap, tret)
         PIX16EX callw */
      DECODE_ARG0;
      break;

    case INSN_CLASS_TBL:
      /* PIC16E (tblrd, tblwt) */
      {
        const char *op[] = { "*", "*+", "*-", "+*" };

        snprintf(buffer, sizeof_buffer, "%s%s", instruction->name, op[opcode & 0x03]);
      }
      break;

    case INSN_CLASS_TBL2:
      /* PIC16 (tlrd, tlwt) */
      file1 = opcode & MASK_PIC16_FILE;
      tmp   = (opcode >> 9) & 1;
      sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_N_S(tmp, sfr->name);
      }
      else {
        DECODE_ARG2_N_N(tmp, file1);
      }
      break;

    case INSN_CLASS_TBL3:
      /* PIC16 (tablrd, tablwt) */
      {
        unsigned int inc;

        file1 = opcode & MASK_PIC16_FILE;
        tmp   = (opcode >> 9) & 1;
        inc   = (opcode >> 8) & 1;
        sfr   = (show_core_sfrs) ? gp_processor_find_sfr(class, file1) : NULL;

        if (sfr != NULL) {
          DECODE_ARG3_N_N_S(tmp, inc, sfr->name);
        }
        else {
          DECODE_ARG3_N_N_N(tmp, inc, file1);
        }
      }
      break;

    case INSN_CLASS_MOVINDF:
      /* PIC14E (moviw, movwi) */
      {
        const char *op_pre[]  = { "++", "--", "",   ""   };
        const char *op_post[] = { "",   "",   "++", "--" };

        file1 = opcode & 0x04;
        tmp   = opcode & 0x03;

        if (show_core_sfrs) {
          DECODE_MOVINDF_S_S_S(op_pre[tmp], (file1) ? "FSR1" : "FSR0", op_post[tmp]);
        }
        else {
          DECODE_MOVINDF_S_N_S(op_pre[tmp], (file1) ? 6 : 4, op_post[tmp]);
        }
      }
      break;

    default:
      assert(0);
    }

  return num_words;
}

int
gp_disassemble_byte(MemBlock *m,
                    int byte_address,
                    proc_class_t class,
                    char *buffer,
                    size_t sizeof_buffer)
{
  unsigned char byte;

  b_memory_assert_get(m, byte_address, &byte, NULL, NULL);
  snprintf(buffer, sizeof_buffer, "db\t0x%02x", (unsigned int)byte);
  return 1;
}

int
gp_disassemble_word(MemBlock *m,
                    int byte_address,
                    proc_class_t class,
                    char *buffer,
                    size_t sizeof_buffer)
{
  unsigned short word;

  class->i_memory_get(m, byte_address, &word, NULL, NULL);
  snprintf(buffer, sizeof_buffer, "dw\t0x%04x", (unsigned int)word);
  return 2;
}

int
gp_disassemble_size(MemBlock *m,
                    int byte_address,
                    proc_class_t class,
                    int bsr_boundary,
                    gp_boolean show_core_sfrs,
                    char *buffer,
                    size_t sizeof_buffer,
                    unsigned int size)
{
  if (size == 1) {
    return gp_disassemble_byte(m, byte_address, class, buffer, sizeof_buffer);
  }
  else if (size == 2) {
    char dasmbuf[512];
    int num_words;

    num_words = gp_disassemble(m, byte_address, class, bsr_boundary, show_core_sfrs, dasmbuf, sizeof(dasmbuf));

    if (num_words != 1) {
      return gp_disassemble_word(m, byte_address, class, buffer, sizeof_buffer);
    }
    else {
      strncpy(buffer, dasmbuf, sizeof_buffer);
      return (2 * num_words);
    }
  }
  return (2 * gp_disassemble(m, byte_address, class, bsr_boundary, show_core_sfrs, buffer, sizeof_buffer));
}

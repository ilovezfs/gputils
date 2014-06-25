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

#define DECODE_ARG1_Nw(ARG1) \
        snprintf(buffer, sizeof_buffer, "%s\t0x%0*x", instruction->name, addr_digits, ARG1)

#define DECODE_ARG1_S(ARG1) \
        snprintf(buffer, sizeof_buffer, "%s\t%s", instruction->name, ARG1)

#define DECODE_ARG1_S_Mw(ARG1, ARG2) \
        snprintf(buffer, sizeof_buffer, "%s\t%s\t\t\t; dest: 0x%0*x", instruction->name, ARG1, addr_digits, ARG2)

#define DECODE_ARG2_N_N(ARG1, ARG2) \
        snprintf(buffer, sizeof_buffer, "%s\t%#x, %#x", instruction->name, ARG1, ARG2)

#define DECODE_ARG2_Nw_N(ARG1, ARG2) \
        snprintf(buffer, sizeof_buffer, "%s\t0x%0*x, %#x", instruction->name, addr_digits, ARG1, ARG2)

#define DECODE_ARG2_N_S(ARG1, ARG2) \
        snprintf(buffer, sizeof_buffer, "%s\t%#x, %s", instruction->name, ARG1, ARG2)

#define DECODE_ARG2_S_N(ARG1, ARG2) \
        snprintf(buffer, sizeof_buffer, "%s\t%s, %#x", instruction->name, ARG1, ARG2)

#define DECODE_ARG2_S_N_Mw(ARG1, ARG2, ARG3) \
        snprintf(buffer, sizeof_buffer, "%s\t%s, %#x\t\t\t; dest: 0x%0*x", instruction->name, ARG1, ARG2, addr_digits, ARG3)

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

/*------------------------------------------------------------------------------------------------*/

int
gp_disassemble_find_labels(MemBlock *m, int byte_address, pic_processor_t processor,
                           gpdasm_fstate_t *fstate)
{
  proc_class_t class;
  int prog_max_org;
  int value;
  int org;
  unsigned short opcode;
  const struct insn *instruction;
  enum common_insn icode;
  int num_words = 1;
  unsigned short file1;
  unsigned short file2;
  unsigned int tmp;
  int dest_byte_addr;
  unsigned int type;
  int wreg;
  int pclath;
  int pclath_valid;

  class = processor->class;

  if (class->find_insn == NULL) {
    return 0;
  }

  if (class->i_memory_get(m, byte_address, &opcode, NULL, NULL) != W_USED_ALL) {
    return num_words;
  }

  instruction = class->find_insn(class, opcode);

  if (instruction == NULL)  {
    return num_words;
  }

  icode = instruction->icode;

  wreg         = fstate->wreg_prev;
  pclath       = fstate->pclath_prev;
  pclath_valid = fstate->pclath_valid_mask;
  prog_max_org = (processor->prog_mem_size > 0) ? (processor->prog_mem_size - 1) : 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"

  switch (instruction->class) {
    case INSN_CLASS_LIT7:
      /* PIC14E movlp */
      pclath = opcode & MASK_PIC14E_PAGE;
      pclath_valid = 0xff;
      break;

    case INSN_CLASS_LIT8C12:
      /* PIC12x call, SX call */
      value = opcode & MASK_PIC12_CALL;

      if ((prog_max_org > 0) && (prog_max_org <= (0x100 + MASK_PIC12_CALL))) {
        /* The PCLATH is not required. */
        org = value;
        goto _class_lit8c12;
      }
      else if ((pclath_valid & 0x06) == 0x06) {
        /* The value of the PCLATH is known. */
        org = ((pclath & 0x06) << 8) | value;

_class_lit8c12:

        if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
          dest_byte_addr = gp_processor_org_to_byte(class, org);
          b_memory_set_addr_type(m, byte_address, W_ADDR_T_BRANCH_SRC, dest_byte_addr);
          b_memory_set_addr_type(m, dest_byte_addr, W_ADDR_T_FUNC, 0);
        }
      }

      wreg = -1;
      pclath_valid = 0;
      break;

    case INSN_CLASS_LIT8C16:
      /* PIC16 lcall */
      if ((pclath_valid & 0xff) == 0xff) {
        /* The value of the PCLATH is known. */
        value = opcode & 0x00ff;
        org = ((pclath & 0xff) << 8) | value;

        if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
          dest_byte_addr = gp_processor_org_to_byte(class, org);
          b_memory_set_addr_type(m, byte_address, W_ADDR_T_BRANCH_SRC, dest_byte_addr);
          b_memory_set_addr_type(m, dest_byte_addr, W_ADDR_T_FUNC, 0);
        }
      }

      wreg = -1;
      pclath_valid = 0;
      break;

    case INSN_CLASS_LIT8:
      /* PIC1xx (addlw, andlw, iorlw, movlw, retlw, sublw, xorlw),
         PIC16  movlb,
         PIC16x mullw,
         PIC16E pushl */
      if ((class == PROC_CLASS_PIC14) || (class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC16)) {
        tmp = opcode & 0x00ff;

        if (icode == ICODE_MOVLW) {
          wreg = tmp;
        }
        else if (wreg >= 0) {
          if (icode == ICODE_ADDLW) {
            wreg = (wreg + tmp) & 0xff;
          }
          else if (icode == ICODE_ANDLW) {
            wreg = (wreg & tmp) & 0xff;
          }
          else if (icode == ICODE_IORLW) {
            wreg = (wreg | tmp) & 0xff;
          }
          else if (icode == ICODE_SUBLW) {
            wreg = (tmp - wreg) & 0xff;
          }
          else if (icode == ICODE_XORLW) {
            wreg = (wreg ^ tmp) & 0xff;
          }
        }
      }
      break;

    case INSN_CLASS_LIT9:
      /* PIC12 goto, SX goto */
      value = opcode & MASK_PIC12_GOTO;

      if ((prog_max_org > 0) && (prog_max_org <= MASK_PIC12_GOTO)) {
        /* The PCLATH is not required. */
        org = value;
        goto _class_lit9;
      }
      else if ((pclath_valid & 0x06) == 0x06) {
        /* The value of the PCLATH is known. */
        org = ((pclath & 0x06) << 8) | value;

_class_lit9:

        if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
          dest_byte_addr = gp_processor_org_to_byte(class, org);
          b_memory_set_addr_type(m, byte_address, W_ADDR_T_BRANCH_SRC, dest_byte_addr);
          b_memory_set_addr_type(m, dest_byte_addr, W_ADDR_T_FUNC, 0);
        }
      }
      break;

    case INSN_CLASS_LIT11:
      /* PIC14x (call, goto) */
      tmp  = (class == PROC_CLASS_PIC14E) ? 0x78 : 0x18;

      if ((prog_max_org > 0) && (prog_max_org <= 0x07ff)) {
        /* The PCLATH is not required. */
        value = opcode & MASK_PIC14_BRANCH;
        org = value;
        goto _class_lit11;
      }
      else if ((pclath_valid & tmp) == tmp) {
        /* The value of the PCLATH is known. */
        value = opcode & MASK_PIC14_BRANCH;
        org = ((pclath & tmp) << 8) | value;

_class_lit11:

        if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
          dest_byte_addr = gp_processor_org_to_byte(class, org);
          type = (icode == ICODE_CALL) ? W_ADDR_T_FUNC : W_ADDR_T_LABEL;
          b_memory_set_addr_type(m, byte_address, W_ADDR_T_BRANCH_SRC, dest_byte_addr);
          b_memory_set_addr_type(m, dest_byte_addr, type, 0);
        }
      }

      if (icode == ICODE_CALL) {
        wreg = -1;
        pclath_valid = 0;
      }
      break;

    case INSN_CLASS_RBRA8:
      /* PIC16E (bc, bn, bnc, bnn, bnov, bnz, bov, bz) */
      value = opcode & MASK_PIC16E_RBRA8;
      /* twos complement number */
      if (value & 0x80) {
        value = -((value ^ MASK_PIC16E_RBRA8) + 1);
      }

      dest_byte_addr = byte_address + value * 2 + 2;
      org = gp_processor_byte_to_org(class, dest_byte_addr);

      if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
        b_memory_set_addr_type(m, byte_address, W_ADDR_T_BRANCH_SRC, dest_byte_addr);
        b_memory_set_addr_type(m, dest_byte_addr, W_ADDR_T_LABEL, 0);
      }
      break;

    case INSN_CLASS_RBRA9:
      /* PIC14E bra */
      value = opcode & MASK_PIC14E_RBRA9;
      /* twos complement number */
      if (value & 0x100) {
        value = -((value ^ MASK_PIC14E_RBRA9) + 1);
      }

      dest_byte_addr = byte_address + value * 2 + 2;
      org = gp_processor_byte_to_org(class, dest_byte_addr);

      if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
        b_memory_set_addr_type(m, byte_address, W_ADDR_T_BRANCH_SRC, dest_byte_addr);
        b_memory_set_addr_type(m, dest_byte_addr, W_ADDR_T_LABEL, 0);
      }
      break;

    case INSN_CLASS_RBRA11:
      /* PIC16E (bra, rcall) */
      value = opcode & MASK_PIC16E_RBRA11;
      /* twos complement number */
      if (value & 0x400) {
        value = -((value ^ MASK_PIC16E_RBRA11) + 1);
      }

      dest_byte_addr = byte_address + value * 2 + 2;
      org = gp_processor_byte_to_org(class, dest_byte_addr);

      if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
        type = (icode == ICODE_RCALL) ? W_ADDR_T_FUNC : W_ADDR_T_LABEL;
        b_memory_set_addr_type(m, byte_address, W_ADDR_T_BRANCH_SRC, dest_byte_addr);
        b_memory_set_addr_type(m, dest_byte_addr, type, 0);
      }
      break;

    case INSN_CLASS_LIT20:
      /* PIC16E goto */
    case INSN_CLASS_CALL20:
      /* PIC16E call */
      {
        unsigned short dest;

        num_words = 2;
        class->i_memory_get(m, byte_address + 2, &dest, NULL, NULL);
        dest  = (dest & MASK_PIC16E_BRANCH_HIGHER) << 8;
        dest |= opcode & MASK_PIC16E_BRANCH_LOWER;
        dest_byte_addr = dest * 2;
        org = gp_processor_org_to_byte(class, dest_byte_addr);

        if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
          type = (icode == ICODE_CALL) ? W_ADDR_T_FUNC : W_ADDR_T_LABEL;
          b_memory_set_addr_type(m, byte_address, W_ADDR_T_BRANCH_SRC, dest_byte_addr);
          b_memory_set_addr_type(m, dest_byte_addr, type, 0);
        }
      }
      break;

    case INSN_CLASS_FP:
      /* PIC16 movfp */
      file1 = opcode & MASK_PIC16_FILE;
      file2 = (opcode >> 8) & 0x1f;

      if ((file1 == REG_PIC16_WREG) && (file2 == REG_PIC16_PCLATH)) {
        pclath = wreg;
        pclath_valid = (wreg >= 0) ? 0xff : 0;
      }
      else if (file2 == REG_PIC16_WREG) {
        /* The destination the WREG. */
        wreg = -1;
      }
      break;

    case INSN_CLASS_PF:
      /* PIC16 movpf */
      file1 = (opcode >> 8) & 0x1f;
      file2 = opcode & MASK_PIC16_FILE;

      if ((file1 == REG_PIC16_WREG) && (file2 == REG_PIC16_PCLATH)) {
        pclath = wreg;
        pclath_valid = (wreg >= 0) ? 0xff : 0;
      }
      else if (file2 == REG_PIC16_WREG) {
        /* The destination the WREG. */
        wreg = -1;
      }
      break;

    case INSN_CLASS_OPWF5:
      /* {PIC12E, SX} (addwf, andwf, comf, decf, decfsz, incf, incfsz,
                       iorwf, movf, rlf, rrf, subwf, swapf, xorwf) */
      tmp = (opcode >> 5) & 1;

      if (tmp == 0) {
        /* The destination the WREG. */
        wreg = -1;
      }
      break;

    case INSN_CLASS_B5:
      /* {PIC12x, SX} (bcf, bsf, btfsc, btfss) */
      file1 = opcode & MASK_PIC12_FILE;
      tmp   = (opcode >> 5) & 7;

      if ((file1 == REG_PIC12_STATUS) && ((tmp == 5) || (tmp == 6))) {
        tmp = 1 << (tmp - 4);

        if (icode == ICODE_BCF) {
          pclath &= ~tmp;
          pclath_valid |= tmp;
        }
        else if (icode == ICODE_BSF) {
          pclath |= tmp;
          pclath_valid |= tmp;
        }
      }
      break;

    case INSN_CLASS_OPF7:
      /* PIC14x (clrf, movwf, tris) */
      file1 = opcode & MASK_PIC14_FILE;

      if (icode == ICODE_CLRF) {
        if (file1 == REG_PIC14_PCLATH) {
          pclath = 0;
          pclath_valid = 0xff;
        }
        else if ((class == PROC_CLASS_PIC14E) && (file1 == REG_PIC14E_WREG)) {
          wreg = 0;
        }
      }
      break;

    case INSN_CLASS_OPWF7:
      /* PIC14x (addwf, andwf, comf, decf, decfsz, incf, incfsz, iorwf, movf,
                 rlf, rrf, subwf, swapf, xorwf)
         PIC14E (addwfc, asrf, lslf, lsrf, subwfb) */
      tmp = (opcode >> 7) & 1;

      if (tmp == 0) {
        wreg = -1;
      }
      break;

    case INSN_CLASS_B7:
      /* PIC14x (bcf, bsf, btfsc, btfss) */
      file1 = opcode & MASK_PIC14_FILE;
      tmp   = (opcode >> 7) & 7;

      if (class == PROC_CLASS_PIC14) {
        if ((file1 == REG_PIC14_PCLATH) && ((tmp == 3) || (tmp == 4))) {
          tmp = 1 << tmp;

          if (icode == ICODE_BCF) {
            pclath &= ~tmp;
            pclath_valid |= tmp;
          }
          else if (icode == ICODE_BSF) {
            pclath |= tmp;
            pclath_valid |= tmp;
          }
        }
      }
      else {
        /* PROC_CLASS_PIC14E */
        if ((file1 == REG_PIC14_PCLATH) && ((tmp >= 3) || (tmp <= 6))) {
          tmp = 1 << tmp;

          if (icode == ICODE_BCF) {
            pclath &= ~tmp;
            pclath_valid |= tmp;
          }
          else if (icode == ICODE_BSF) {
            pclath |= tmp;
            pclath_valid |= tmp;
          }
        }
      }
      break;

    case INSN_CLASS_OPF8:
      /* PIC16 (cpfseq, cpfsgt, cpfslt, movwf, mulwf, tstfsz) */
      file1 = opcode & MASK_PIC16_FILE;

      if ((file1 == REG_PIC16_PCLATH) && (icode == ICODE_MOVWF)) {
        pclath = wreg;
        pclath_valid = (wreg >= 0) ? 0xff : 0;
      }
      break;

    case INSN_CLASS_OPWF8:
      /* PIC16 (addwf, addwfc, andwf, clrf, comf, daw, decf, decfsz, dcfsnz, incf,
                incfsz, infsnz, iorwf, rlcf, rlncf, rrcf, rrncf, setf, subwf, subwfb,
                swapf, xorwf) */
      file1 = opcode & MASK_PIC16_FILE;
      tmp   = (opcode >> 8) & 1;

      if (icode == ICODE_SETF) {
        if ((tmp == 0) || (file1 == REG_PIC16_WREG)) {
          wreg = 0xff;
        }
      }
      else if (icode == ICODE_CLRF) {
        if (tmp == 0) {
          /* The destination the WREG. */
          wreg = 0;
        }
      }
      else if (tmp == 0) {
        /* The destination the WREG. */
        wreg = -1;
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
      if (icode == ICODE_CLRW) {
        wreg = 0;
      }
      break;

    case INSN_CLASS_FLIT12:
      /* PIC16E lfsr */
    case INSN_CLASS_FF:
      /* PIC16E movff */
    case INSN_CLASS_SF:
      /* PIC16E movsf */
    case INSN_CLASS_SS:
      /* PIC16E movss */
      num_words = 2;
      break;
    } /* switch (instruction->class) */

#pragma GCC diagnostic pop

  fstate->wreg_prev         = wreg;
  fstate->pclath_prev       = pclath;
  fstate->pclath_valid_mask = pclath_valid;
  return num_words;
}

static int
print_word(unsigned short opcode, int behavior, char *buffer, size_t sizeof_buffer) {
  if (behavior & GPDIS_SHOW_BYTES) {
    if (isprint(opcode)) {
      snprintf(buffer, sizeof_buffer, "dw\t0x%04x\t\t\t; '%c'", (unsigned int)opcode,
               (unsigned char)opcode);
      return 1;
    }
    else if (isprint(opcode & 0xFF) && isprint((opcode >> 8) & 0xFF)) {
      snprintf(buffer, sizeof_buffer, "dw\t0x%04x\t\t\t; '%c%c'", (unsigned int)opcode,
               (unsigned char)(opcode & 0xFF), (unsigned char)((opcode >> 8) & 0xFF));
      return 1;
    }
  }

  snprintf(buffer, sizeof_buffer, "dw\t0x%04x", (unsigned int)opcode);
  return 1;
}

/*------------------------------------------------------------------------------------------------*/

int
gp_disassemble(MemBlock *m, int byte_address, proc_class_t class, int bsr_boundary,
               int prog_mem_size, int behavior, char *buffer, size_t sizeof_buffer)
{
  int value;
  unsigned short opcode;
  const struct insn *instruction = NULL;
  int prog_max_org;
  unsigned int type;
  const char *dest_name;
  int org;
  int num_words = 1;
  unsigned short file1;
  unsigned short file2;
  unsigned int tmp;
  unsigned int ram_acc;
  const core_sfr_t *sfr;
  int addr_digits;

  if (class->find_insn == NULL) {
    snprintf(buffer, sizeof_buffer, "unsupported processor class");
    return 0;
  }

  prog_max_org = (prog_mem_size > 0) ? (prog_mem_size - 1) : 0;

  addr_digits = class->addr_digits;
  type = 0;
  dest_name = NULL;

  if (class->i_memory_get(m, byte_address, &opcode, NULL, NULL) == W_USED_ALL) {
    unsigned int dest_byte_addr;

    org = gp_processor_byte_to_org(class, byte_address);
    type = b_memory_get_addr_type(m, byte_address, NULL, &dest_byte_addr);

    if ((behavior & GPDIS_SHOW_NAMES) && (type & W_ADDR_T_BRANCH_SRC)) {
      b_memory_get_addr_type(m, dest_byte_addr, &dest_name, NULL);
    }
  }

  /* Special case for pic14 enhanced moviw k[FSRn] & movwi k[FSRn]. */
  if (class == PROC_CLASS_PIC14E) {
    const char *instr = NULL;

    tmp = opcode & IMSK_PIC14E_MOVIW_IDX;

    if (tmp == INSN_PIC14E_MOVIW_IDX) {
      instr = "moviw";
    }
    else if (tmp == INSN_PIC14E_MOVWI_IDX) {
      instr = "movwi";
    }

    if (instr != NULL) {
      /* twos complement number */
      const char *neg;

      value = opcode & 0x003f;
      tmp   = (opcode & 0x0040) >> 6;

      if (value & 0x20) {
        value = (value ^ 0x3f) + 1;
        neg = "-";
      } else {
        neg = "";
      }

      if (behavior & GPDIS_SHOW_NAMES) {
        snprintf(buffer, sizeof_buffer, "%s\t%s.%d[FSR%i]", instr, neg, value, tmp);
      }
      else {
        snprintf(buffer, sizeof_buffer, "%s\t%s.%d[%u]", instr, neg, value,
                 (tmp) ? REG_PIC14E_FSR1 : REG_PIC14E_FSR0);
      }

      return num_words;
    }
  }

  instruction = class->find_insn(class, opcode);

  if (instruction == NULL)  {
    return print_word(opcode, behavior, buffer, sizeof_buffer);
  }

  switch (instruction->class) {
    case INSN_CLASS_LIT3_BANK:
      /* SX bank */
      DECODE_ARG1_N((opcode & 0x0007) << 5);
      break;

    case INSN_CLASS_LIT3_PAGE:
      /* SX page */
      DECODE_ARG1_N((opcode & 0x0007) << 9);
      break;

    case INSN_CLASS_LIT1:
      /* PIC16E (retfie, return) */
      DECODE_ARG1_N(opcode & 0x0001);
      break;

    case INSN_CLASS_LIT3:
      /* PIC12E movlb */
      DECODE_ARG1_N(opcode & 0x0007);
      break;

    case INSN_CLASS_LIT4:
      /* SX mode */
    case INSN_CLASS_LIT4L:
      /* PIC16x movlb */
      DECODE_ARG1_N(opcode & 0x000f);
      break;

    case INSN_CLASS_LIT4S:
      /* PIC16 movlr */
      DECODE_ARG1_N((opcode & 0x00f0) >> 4);
      break;

    case INSN_CLASS_LIT5:
      /* PIC14E movlb */
      DECODE_ARG1_N(opcode & MASK_PIC14E_BANK);
      break;

    case INSN_CLASS_LIT6:
      /* PIC16E (addulnk, subulnk) */
      DECODE_ARG1_N(opcode & 0x003f);
      break;

    case INSN_CLASS_LIT7:
      /* PIC14E movlp */
      DECODE_ARG1_N(opcode & MASK_PIC14E_PAGE);
      break;

    case INSN_CLASS_LIT8:
      /* PIC1xx (addlw, andlw, iorlw, movlw, retlw, sublw, xorlw),
         PIC16  movlb,
         PIC16x mullw,
         PIC16E pushl */
      DECODE_ARG1_N(opcode & 0x00ff);
      break;

    case INSN_CLASS_LIT8C12:
      /* PIC12x call, SX call */
    case INSN_CLASS_LIT8C16:
      /* PIC16 lcall */
      org = opcode & 0x00ff;

      if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
        if (dest_name != NULL) {
          DECODE_ARG1_S(dest_name);
        }
        else {
          DECODE_ARG1_Nw(org);
        }
      }
      else {
        return print_word(opcode, behavior, buffer, sizeof_buffer);
      }
      break;

    case INSN_CLASS_LIT9:
      /* PIC12 goto, SX goto */
      org = opcode & MASK_PIC12_GOTO;

      if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
        if (dest_name != NULL) {
          DECODE_ARG1_S(dest_name);
        }
        else {
          DECODE_ARG1_Nw(org);
        }
      }
      else {
        return print_word(opcode, behavior, buffer, sizeof_buffer);
      }
      break;

    case INSN_CLASS_LIT11:
      /* PIC14x (call, goto) */
      org = opcode & MASK_PIC14_BRANCH;

      if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
        if (dest_name != NULL) {
          DECODE_ARG1_S(dest_name);
        }
        else {
          DECODE_ARG1_Nw(org);
        }
      }
      else {
        return print_word(opcode, behavior, buffer, sizeof_buffer);
      }
      break;

    case INSN_CLASS_LIT13:
      /* PIC16 (call, goto) */
      org = opcode & MASK_PIC16_BRANCH;

      if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
        DECODE_ARG1_Nw(opcode & MASK_PIC16_BRANCH);
      }
      else {
        return print_word(opcode, behavior, buffer, sizeof_buffer);
      }
      break;

    case INSN_CLASS_LITFSR_14:
      /* PIC14E addfsr */
      {
        const char *neg;

        value = opcode & 0x003f;
        tmp   = (opcode & 0x0040) >> 6;

        if (value & 0x20) {
          value = (value ^ 0x3f) + 1;
          neg = "-";
        } else {
          neg = "";
        }

        if (behavior & GPDIS_SHOW_NAMES) {
          snprintf(buffer, sizeof_buffer, "%s\tFSR%i, %s.%d", instruction->name,
                   tmp, neg, value);
        }
        else {
          snprintf(buffer, sizeof_buffer, "%s\t%u, %s.%d", instruction->name,
                   (tmp) ? REG_PIC14E_FSR1 : REG_PIC14E_FSR0, neg, value);
        }
      }
      break;

    case INSN_CLASS_LITFSR_16:
      /* PIC16E (addfsr, subfsr) */
      DECODE_ARG2_N_N(((opcode >> 6) & 0x3), (opcode & 0x003f));
      break;

    case INSN_CLASS_RBRA8:
      /* PIC16E (bc, bn, bnc, bnn, bnov, bnz, bov, bz) */
      value = opcode & MASK_PIC16E_RBRA8;
      /* twos complement number */
      if (value & 0x80) {
        value = -((value ^ MASK_PIC16E_RBRA8) + 1);
      }

      org = gp_processor_byte_to_org(class, byte_address + value * 2 + 2);

      if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
        if (dest_name != NULL) {
          DECODE_ARG1_S(dest_name);
        }
        else {
          DECODE_ARG1_Nw(org);
        }
      }
      else {
        return print_word(opcode, behavior, buffer, sizeof_buffer);
      }
      break;

    case INSN_CLASS_RBRA9:
      /* PIC14E bra */
      value = opcode & MASK_PIC14E_RBRA9;
      /* twos complement number */
      if (value & 0x100) {
        value = -((value ^ MASK_PIC14E_RBRA9) + 1);
      }

      org = gp_processor_byte_to_org(class, byte_address + value * 2 + 2);

      if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
        DECODE_ARG1_Nw(org);
      }
      else {
        return print_word(opcode, behavior, buffer, sizeof_buffer);
      }
      break;

    case INSN_CLASS_RBRA11:
      /* PIC16E (bra, rcall) */
      value = opcode & MASK_PIC16E_RBRA11;
      /* twos complement number */
      if (value & 0x400) {
        value = -((value ^ MASK_PIC16E_RBRA11) + 1);
      }

      org = gp_processor_byte_to_org(class, byte_address + value * 2 + 2);

      if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
        if (dest_name != NULL) {
          DECODE_ARG1_S_Mw(dest_name, org);
        }
        else {
          DECODE_ARG1_Nw(org);
        }
      }
      else {
        return print_word(opcode, behavior, buffer, sizeof_buffer);
      }
      break;

    case INSN_CLASS_LIT20:
      /* PIC16E goto */
      {
        unsigned short dest;

        num_words = 2;
        class->i_memory_get(m, byte_address + 2, &dest, NULL, NULL);
        dest  = (dest & MASK_PIC16E_BRANCH_HIGHER) << 8;
        dest |= opcode & MASK_PIC16E_BRANCH_LOWER;

        org = gp_processor_byte_to_org(class, dest * 2);

        if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
          if (dest_name != NULL) {
            DECODE_ARG1_S_Mw(dest_name, org);
          }
          else {
            DECODE_ARG1_Nw(gp_processor_byte_to_org(class, org));
          }
        }
        else {
          return print_word(opcode, behavior, buffer, sizeof_buffer);
        }
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
	tmp   = (opcode >> 8) & 1;

        org = gp_processor_byte_to_org(class, dest * 2);

        if ((prog_max_org > 0) && (org >= 0) && (org <= prog_max_org)) {
          if (dest_name != NULL) {
            DECODE_ARG2_S_N_Mw(dest_name, tmp, org);
          }
          else {
            DECODE_ARG2_Nw_N(org, tmp);
          }
        }
        else {
          return print_word(opcode, behavior, buffer, sizeof_buffer);
        }
      }
      break;

    case INSN_CLASS_FLIT12:
      /* PIC16E lfsr */
      {
        unsigned short k;

        num_words = 2;
        class->i_memory_get(m, byte_address + 2, &k, NULL, NULL);
        k = ((opcode & 0x000f) << 8) | (k & 0x00ff);
        file1 = (opcode >> 4) & 3;
        DECODE_ARG2_N_N(file1, k);
      }
      break;

    case INSN_CLASS_FF:
      /* PIC16E movff */
      num_words = 2;
      file1 = opcode & 0x0fff;
      class->i_memory_get(m, byte_address + 2, &file2, NULL, NULL);
      file2 &= 0xfff;

      if (behavior & GPDIS_SHOW_NAMES) {
        const core_sfr_t *sfr2;

        sfr  = gp_processor_find_sfr(class, file1);
        sfr2 = gp_processor_find_sfr(class, file2);

        tmp  = (sfr  != NULL) ? 2 : 0;
        tmp |= (sfr2 != NULL) ? 1 : 0;

        switch (tmp) {
        case 3:
          DECODE_ARG2_S_S(sfr->name, sfr2->name);
          break;

        case 2:
          DECODE_ARG2_S_N(sfr->name, file2);
          break;

        case 1:
          DECODE_ARG2_N_S(file1, sfr2->name);
          break;

        default:
          DECODE_ARG2_N_N(file1, file2);
        }
      }
      else {
        DECODE_ARG2_N_N(file1, file2);
      }
      break;

    case INSN_CLASS_FP:
      /* PIC16 movfp */
      file1 = opcode & MASK_PIC16_FILE;
      file2 = (opcode >> 8) & 0x1f;

      if (behavior & GPDIS_SHOW_NAMES) {
        const core_sfr_t *sfr2;

        sfr  = gp_processor_find_sfr(class, file1);
        sfr2 = gp_processor_find_sfr(class, file2);

        tmp  = (sfr  != NULL) ? 2 : 0;
        tmp |= (sfr2 != NULL) ? 1 : 0;

        switch (tmp) {
        case 3:
          DECODE_ARG2_S_S(sfr->name, sfr2->name);
          break;

        case 2:
          DECODE_ARG2_S_N(sfr->name, file2);
          break;

        case 1:
          DECODE_ARG2_N_S(file1, sfr2->name);
          break;

        default:
          DECODE_ARG2_N_N(file1, file2);
        }
      }
      else {
        DECODE_ARG2_N_N(file1, file2);
      }
      break;

    case INSN_CLASS_PF:
      /* PIC16 movpf */
      file1 = (opcode >> 8) & 0x1f;
      file2 = opcode & MASK_PIC16_FILE;

      if (behavior & GPDIS_SHOW_NAMES) {
        const core_sfr_t *sfr2;

        sfr  = gp_processor_find_sfr(class, file1);
        sfr2 = gp_processor_find_sfr(class, file2);

        tmp  = (sfr  != NULL) ? 2 : 0;
        tmp |= (sfr2 != NULL) ? 1 : 0;

        switch (tmp) {
        case 3:
          DECODE_ARG2_S_S(sfr->name, sfr2->name);
          break;

        case 2:
          DECODE_ARG2_S_N(sfr->name, file2);
          break;

        case 1:
          DECODE_ARG2_N_S(file1, sfr2->name);
          break;

        default:
          DECODE_ARG2_N_N(file1, file2);
        }
      }
      else {
        DECODE_ARG2_N_N(file1, file2);
      }
      break;

    case INSN_CLASS_SF:
      /* PIC16E movsf */
      {
        unsigned short offset;

        num_words = 2;
        offset = opcode & 0x007f;
        class->i_memory_get(m, byte_address + 2, &file1, NULL, NULL);
        file1 &= 0xfff;
        sfr    = ((behavior & GPDIS_SHOW_NAMES) && (bsr_boundary > 0) && (file1 >= (0xF00 + bsr_boundary))) ?
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
        DECODE_ARG2_N_N(opcode & 0x007f, offset2 & 0x007f);
      }
      break;

    case INSN_CLASS_OPF3:
      /* PIC12 tris */
      DECODE_ARG1_N(opcode & MASK_PIC12_TRIS);
      break;

    case INSN_CLASS_OPF5:
      /* {PIC12x, SX} (clrf, movwf), SX tris */
      file1 = opcode & MASK_PIC12_FILE;
      sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG1_S(sfr->name);
      }
      else {
        DECODE_ARG1_N(file1);
      }
      break;

    case INSN_CLASS_OPWF5:
      /* {PIC12E, SX} (addwf, andwf, comf, decf, decfsz, incf, incfsz,
                       iorwf, movf, rlf, rrf, subwf, swapf, xorwf) */
      file1 = opcode & MASK_PIC12_FILE;
      tmp   = (opcode >> 5) & 1;
      sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_S(sfr->name, FLAG_FW(tmp));
      }
      else if (behavior & GPDIS_SHOW_NAMES) {
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
      sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

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
      sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

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
      sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

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
      sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG1_S(sfr->name);
      }
      else {
        DECODE_ARG1_N(file1);
      }
      break;

    case INSN_CLASS_OPWF7:
      /* PIC14x (addwf, andwf, comf, decf, decfsz, incf, incfsz, iorwf, movf,
                 rlf, rrf, subwf, swapf, xorwf)
         PIC14E (addwfc, asrf, lslf, lsrf, subwfb) */
      file1 = opcode & MASK_PIC14_FILE;
      tmp   = (opcode >> 7) & 1;
      sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_S(sfr->name, FLAG_FW(tmp));
      }
      else if (behavior & GPDIS_SHOW_NAMES) {
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
      sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_S(sfr->name, FLAG_FW(tmp));
      }
      else if (behavior & GPDIS_SHOW_NAMES) {
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
      sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

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
      sfr     = ((behavior & GPDIS_SHOW_NAMES) && (ram_acc == 0) && (bsr_boundary > 0) && (file1 >= bsr_boundary)) ?
                     gp_processor_find_sfr(class, 0xF00 + file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG2_S_S(sfr->name, FLAG_BA(ram_acc));
      }
      else if (behavior & GPDIS_SHOW_NAMES) {
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
      sfr     = ((behavior & GPDIS_SHOW_NAMES) && (ram_acc == 0) && (bsr_boundary > 0) && (file1 >= bsr_boundary)) ?
                     gp_processor_find_sfr(class, 0xF00 + file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG3_S_N_S(sfr->name, tmp, FLAG_BA(ram_acc));
      }
      else if (behavior & GPDIS_SHOW_NAMES) {
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
      sfr     = ((behavior & GPDIS_SHOW_NAMES) && (ram_acc == 0) && (bsr_boundary > 0) && (file1 >= bsr_boundary)) ?
                     gp_processor_find_sfr(class, 0xF00 + file1) : NULL;

      if (sfr != NULL) {
        DECODE_ARG3_S_S_S(sfr->name, FLAG_FW(tmp), FLAG_BA(ram_acc));
      }
      else if (behavior & GPDIS_SHOW_NAMES) {
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

        snprintf(buffer, sizeof_buffer, "%s%s", instruction->name, op[opcode & 0x0003]);
      }
      break;

    case INSN_CLASS_TBL2:
      /* PIC16 (tlrd, tlwt) */
      file1 = opcode & MASK_PIC16_FILE;
      tmp   = (opcode >> 9) & 1;
      sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

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
        sfr   = (behavior & GPDIS_SHOW_NAMES) ? gp_processor_find_sfr(class, file1) : NULL;

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

        file1 = opcode & 0x0004;
        tmp   = opcode & 0x0003;

        if (behavior & GPDIS_SHOW_NAMES) {
          DECODE_MOVINDF_S_S_S(op_pre[tmp], (file1) ? "FSR1" : "FSR0", op_post[tmp]);
        }
        else {
          DECODE_MOVINDF_S_N_S(op_pre[tmp], (file1) ? REG_PIC14E_FSR1 : REG_PIC14E_FSR0, op_post[tmp]);
        }
      }
      break;

    default:
      assert(0);
    } /* switch (instruction->class) */

  return num_words;
}

/*------------------------------------------------------------------------------------------------*/

int
gp_disassemble_byte(MemBlock *m, int byte_address, proc_class_t class,
                    char *buffer, size_t sizeof_buffer)
{
  unsigned char byte;

  b_memory_assert_get(m, byte_address, &byte, NULL, NULL);
  snprintf(buffer, sizeof_buffer, "db\t0x%02x", (unsigned int)byte);
  return 1;
}

/*------------------------------------------------------------------------------------------------*/

int
gp_disassemble_word(MemBlock *m, int byte_address, proc_class_t class,
                    char *buffer, size_t sizeof_buffer)
{
  unsigned short word;

  class->i_memory_get(m, byte_address, &word, NULL, NULL);
  snprintf(buffer, sizeof_buffer, "dw\t0x%04x", (unsigned int)word);
  return 2;
}

/*------------------------------------------------------------------------------------------------*/

int
gp_disassemble_size(MemBlock *m, int byte_address, proc_class_t class, int bsr_boundary,
                    int prog_mem_size, int behavior, char *buffer, size_t sizeof_buffer,
                    unsigned int size)
{
  if (size == 1) {
    return gp_disassemble_byte(m, byte_address, class, buffer, sizeof_buffer);
  }
  else if (size == 2) {
    char dasmbuf[512];
    int num_words;

    num_words = gp_disassemble(m, byte_address, class, bsr_boundary, prog_mem_size, behavior,
                               dasmbuf, sizeof(dasmbuf));

    if (num_words != 1) {
      return gp_disassemble_word(m, byte_address, class, buffer, sizeof_buffer);
    }
    else {
      strncpy(buffer, dasmbuf, sizeof_buffer);
      return (2 * num_words);
    }
  }

  return (2 * gp_disassemble(m, byte_address, class, bsr_boundary, prog_mem_size, behavior,
                             buffer, sizeof_buffer));
}

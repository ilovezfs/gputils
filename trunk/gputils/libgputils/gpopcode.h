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
  INSN_CLASS_LIT3,      /* bits 2:0 contains a 3 bit literal            */
  INSN_CLASS_LIT4H,     /* bits 7:4 contain a 4 bit literal, bits 3:0 are unused   */
  INSN_CLASS_LIT4L,     /* bits 3:0 contain a 4 bit literal             */
  INSN_CLASS_LIT5,      /* bits 4:0 contain an 4 bit literal            */
  INSN_CLASS_LIT6,      /* bits 5:0 contain an 6 bit literal            */
  INSN_CLASS_LIT7,      /* bits 6:0 contain an 7 bit literal            */
  INSN_CLASS_LIT8,      /* bits 7:0 contain an 8 bit literal            */
  INSN_CLASS_LIT8C12,   /* bits 7:0 contain an 8 bit literal, 12 bit CALL */
  INSN_CLASS_LIT8C16,   /* bits 7:0 contain an 8 bit literal, 16 bit CALL */
  INSN_CLASS_LIT9,      /* bits 8:0 contain a 9 bit literal             */
  INSN_CLASS_LIT11,     /* bits 10:0 contain an 11 bit literal          */
  INSN_CLASS_LIT13,     /* bits 12:0 contain an 13 bit literal          */
  INSN_CLASS_LITFSR_14, /* bits 5:0 contain an 6 bit literal for fsr (14bit enh. cores) */
  INSN_CLASS_LITFSR_16, /* bits 5:0 contain an 6 bit literal for fsr 7:6 (16bit cores)  */
  INSN_CLASS_IMPLICIT,  /* instruction has no variable bits at all      */
  INSN_CLASS_OPF3,      /* bits 2:0 contain a register address          */
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

/******************************************
        PIC12 definitions
******************************************/

#define PIC12_CORE_MASK         ((1u << 12) - 1)
#define PIC12_PC_MASK           ((1u << 11) - 1)

    /* General file bitmask. */
#define PIC12_BMSK_FILE         0x1F

    /* addwf : 0001 11df ffff
               1111 1100 0000 */
#define PIC12_INSN_ADDWF        0x1C0
#define PIC12_MASK_ADDWF        0xFC0

    /* andlw : 1110 kkkk kkkk
               1111 0000 0000 */
#define PIC12_INSN_ANDLW        0xE00
#define PIC12_MASK_ANDLW        0xF00

    /* andwf : 0001 01df ffff
               1111 1100 0000 */
#define PIC12_INSN_ANDWF        0x140
#define PIC12_MASK_ANDWF        0xFC0

    /* bcf   : 0100 bbbf ffff
               1111 0000 0000 */
#define PIC12_INSN_BCF          0x400
#define PIC12_MASK_BCF          0xF00

    /* bsf   : 0101 bbbf ffff
               1111 0000 0000 */
#define PIC12_INSN_BSF          0x500
#define PIC12_MASK_BSF          0xF00

    /* btfsc : 0110 bbbf ffff
               1111 0000 0000 */
#define PIC12_INSN_BTFSC        0x600
#define PIC12_MASK_BTFSC        0xF00

    /* btfss : 0111 bbbf ffff
               1111 0000 0000 */
#define PIC12_INSN_BTFSS        0x700
#define PIC12_MASK_BTFSS        0xF00

    /* call  : 1001 kkkk kkkk
               1111 0000 0000 */
#define PIC12_INSN_CALL         0x900
#define PIC12_MASK_CALL         0xF00
    /* Address mask. */
#define PIC12_BMSK_CALL         (PIC12_MASK_CALL ^ PIC12_CORE_MASK)

    /* clrf  : 0000 011f ffff
               1111 1110 0000 */
#define PIC12_INSN_CLRF         0x060
#define PIC12_MASK_CLRF         0xFE0

    /* clrw  : 0000 0100 0000
               1111 1111 1111 */
#define PIC12_INSN_CLRW         0x040
#define PIC12_MASK_CLRW         0xFFF

    /* clrwdt: 0000 0000 0100
               1111 1111 1111 */
#define PIC12_INSN_CLRWDT       0x004
#define PIC12_MASK_CLRWDT       0xFFF

    /* comf  : 0010 01df ffff
               1111 1100 0000 */
#define PIC12_INSN_COMF         0x240
#define PIC12_MASK_COMF         0xFC0

    /* decf  : 0000 11df ffff
               1111 1100 0000 */
#define PIC12_INSN_DECF         0x0C0
#define PIC12_MASK_DECF         0xFC0

    /* decfsz: 0010 11df ffff
               1111 1100 0000 */
#define PIC12_INSN_DECFSZ       0x2C0
#define PIC12_MASK_DECFSZ       0xFC0

    /* goto  : 101k kkkk kkkk
               1110 0000 0000 */
#define PIC12_INSN_GOTO         0xA00
#define PIC12_MASK_GOTO         0xE00
    /* Address mask. */
#define PIC12_BMSK_GOTO         (PIC12_MASK_GOTO ^ PIC12_CORE_MASK)

    /* incf  : 0010 10df ffff
               1111 1100 0000 */
#define PIC12_INSN_INCF         0x280
#define PIC12_MASK_INCF         0xFC0

    /* incfsz: 0011 11df ffff
               1111 1100 0000 */
#define PIC12_INSN_INCFSZ       0x3C0
#define PIC12_MASK_INCFSZ       0xFC0

    /* iorlw : 1101 kkkk kkkk
               1111 0000 0000 */
#define PIC12_INSN_IORLW        0xD00
#define PIC12_MASK_IORLW        0xF00

    /* iorwf : 0001 00df ffff
               1111 1100 0000 */
#define PIC12_INSN_IORWF        0x100
#define PIC12_MASK_IORWF        0xFC0

    /* movf  : 0010 00df ffff
               1111 1100 0000 */
#define PIC12_INSN_MOVF         0x200
#define PIC12_MASK_MOVF         0xFC0

    /* movlw : 1100 kkkk kkkk
               1111 0000 0000 */
#define PIC12_INSN_MOVLW        0xC00
#define PIC12_MASK_MOVLW        0xF00

    /* movwf : 0000 001f ffff
               1111 1110 0000 */
#define PIC12_INSN_MOVWF        0x020
#define PIC12_MASK_MOVWF        0xFE0

    /* nop   : 0000 0000 0000
               1111 1111 1111 */
#define PIC12_INSN_NOP          0x000
#define PIC12_MASK_NOP          0xFFF

    /* option: 0000 0000 0010
               1111 1111 1111 */
#define PIC12_INSN_OPTION       0x002
#define PIC12_MASK_OPTION       0xFFF

    /* retlw : 1000 kkkk kkkk
               1111 0000 0000 */
#define PIC12_INSN_RETLW        0x800
#define PIC12_MASK_RETLW        0xF00

    /* return: 1000 kkkk kkkk (Alias of retlw instruction.)
               1111 1111 1111 */
#define PIC12_INSN_RETURN       0x800
#define PIC12_MASK_RETURN       0xFFF

    /* rlf   : 0011 01df ffff
               1111 1100 0000 */
#define PIC12_INSN_RLF          0x340
#define PIC12_MASK_RLF          0xFC0

    /* rrf   : 0011 00df ffff
               1111 1100 0000 */
#define PIC12_INSN_RRF          0x300
#define PIC12_MASK_RRF          0xFC0

    /* sleep : 0000 0000 0011
               1111 1111 1111 */
#define PIC12_INSN_SLEEP        0x003
#define PIC12_MASK_SLEEP        0xFFF

    /* subwf : 0000 10df ffff
               1111 1100 0000 */
#define PIC12_INSN_SUBWF        0x080
#define PIC12_MASK_SUBWF        0xFC0

    /* swapf : 0011 10df ffff
               1111 1100 0000 */
#define PIC12_INSN_SWAPF        0x380
#define PIC12_MASK_SWAPF        0xFC0

    /* tris  : 0000 0000 0fff
               1111 1111 1000 */
#define PIC12_INSN_TRIS         0x000
#define PIC12_MASK_TRIS         0xFF8
#define PIC12_BMSK_TRIS         (PIC12_MASK_TRIS ^ PIC12_CORE_MASK)

    /* xorlw : 1111 kkkk kkkk
               1111 0000 0000 */
#define PIC12_INSN_XORLW        0xF00
#define PIC12_MASK_XORLW        0xF00

    /* xorwf : 0001 10df ffff
               1111 1100 0000 */
#define PIC12_INSN_XORWF        0x180
#define PIC12_MASK_XORWF        0xFC0

    /*   PC mask: 111 1111 1111 
       goto mask:   1 1111 1111 */
#define PIC12_PAGE_BITS         (PIC12_PC_MASK & (~PIC12_BMSK_GOTO))

/******************************************
        PIC12E definitions
******************************************/

    /* movlb : 0000 0001 0kkk
               1111 1111 1000 */
#define PIC12E_INSN_MOVLB       0x010
#define PIC12E_MASK_MOVLB       0xFF8

    /* retfie: 0000 0001 1111
               1111 1111 1111 */
#define PIC12E_INSN_RETFIE      0x01F
#define PIC12E_MASK_RETFIE      0xFFF

    /* return: 0000 0001 1110
               1111 1111 1111 */
#define PIC12E_INSN_RETURN      0x01E
#define PIC12E_MASK_RETURN      0xFFF


/******************************************
        SX definitions
******************************************/

#define SX_PC_MASK              ((1u << 12) - 1)

    /* page  : 0000 0001 1nnn
               1111 1111 1000 */
#define SX_INSN_BANK            0x018
#define SX_MASK_BANK            0xFF8
#define SX_BMSK_BANK            (SX_MASK_BANK ^ PIC12_CORE_MASK)

    /* iread : 0000 0100 0001
               1111 1111 1111 */
#define SX_INSN_IREAD           0x041
#define SX_MASK_IREAD           0xFFF

    /* mov M,#lit : 0000 0101 kkkk
                    1111 1111 0000 */
#define SX_INSN_MODE            0x050
#define SX_MASK_MODE            0xFF0
#define SX_BMSK_MODE            (SX_MASK_MODE ^ PIC12_CORE_MASK)

    /* mov W,M : 0000 0100 0010
                 1111 1111 1111
       M ==> W */
#define SX_INSN_MOVMW           0x042
#define SX_MASK_MOVMW           0xFFF

    /* mov M,W : 0000 0100 0011
                 1111 1111 1111
       W ==> M */
#define SX_INSN_MOVWM           0x043
#define SX_MASK_MOVWM           0xFFF

    /* page  : 0000 0001 0nnn
               1111 1111 1000 */
#define SX_INSN_PAGE            0x010
#define SX_MASK_PAGE            0xFF8
#define SX_BMSK_PAGE            (SX_MASK_PAGE ^ PIC12_CORE_MASK)

    /* reti  : 0000 0000 1110
               1111 1111 1111 */
#define SX_INSN_RETI            0x00E
#define SX_MASK_RETI            0xFFF

    /* retiw : 0000 0000 1111
               1111 1111 1111 */
#define SX_INSN_RETIW           0x00F
#define SX_MASK_RETIW           0xFFF

    /* retp  : 0000 0000 1101
               1111 1111 1111 */
#define SX_INSN_RETP            0x00D
#define SX_MASK_RETP            0xFFF

    /* ret   : 0000 0000 1100
               1111 1111 1111 */
#define SX_INSN_RETURN          0x00C
#define SX_MASK_RETURN          0xFFF

    /*   PC mask: 1111 1111 1111 
       goto mask:    1 1111 1111 */
#define SX_PAGE_BITS            (SX_PC_MASK & (~PIC12_BMSK_GOTO))

/******************************************
        PIC14 definitions
******************************************/

#define PIC14_CORE_MASK         ((1u << 14) - 1)
#define PIC14_PC_MASK           ((1u << 13) - 1)

    /* General file bitmask. */
#define PIC14_BMSK_FILE         0x7F
#define PIC14_BMSK_TRIS         0x07

#define PIC14_INSN_BxF_BITSHIFT     7

    /* addlw : 11 111x kkkk kkkk
               11 1110 0000 0000 */
#define PIC14_INSN_ADDLW        0x3E00
#define PIC14_MASK_ADDLW        0x3E00

    /* addwf : 00 0111 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_ADDWF        0x0700
#define PIC14_MASK_ADDWF        0x3F00

    /* andlw : 11 1001 kkkk kkkk
               11 1111 0000 0000 */
#define PIC14_INSN_ANDLW        0x3900
#define PIC14_MASK_ANDLW        0x3F00

    /* andwf : 00 0101 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_ANDWF        0x0500
#define PIC14_MASK_ANDWF        0x3F00

    /* bcf   : 01 00bb bfff ffff
               11 1100 0000 0000 */
#define PIC14_INSN_BCF          0x1000
#define PIC14_MASK_BCF          0x3C00

    /* bsf   : 01 01bb bfff ffff
               11 1100 0000 0000 */
#define PIC14_INSN_BSF          0x1400
#define PIC14_MASK_BSF          0x3C00

    /* btfsc : 01 10bb bfff ffff
               11 1100 0000 0000 */
#define PIC14_INSN_BTFSC        0x1800
#define PIC14_MASK_BTFSC        0x3C00

    /* btfss : 01 11bb bfff ffff
               11 1100 0000 0000 */
#define PIC14_INSN_BTFSS        0x1C00
#define PIC14_MASK_BTFSS        0x3C00

    /* call  : 10 0kkk kkkk kkkk
               11 1000 0000 0000 */
#define PIC14_INSN_CALL         0x2000
#define PIC14_MASK_CALL         0x3800
#define PIC14_BMSK_CALL         (PIC14_MASK_CALL ^ PIC14_CORE_MASK)

    /* clrf  : 00 0001 1fff ffff
               11 1111 1000 0000 */
#define PIC14_INSN_CLRF         0x0180
#define PIC14_MASK_CLRF         0x3F80

    /* clrw  : 00 0001 0xxx xxxx
               11 1111 1111 1111 */
#define PIC14_INSN_CLRW         0x0103
#define PIC14_MASK_CLRW         0x3FFF

    /* clrwdt: 00 0000 0110 0100
               11 1111 1111 1111 */
#define PIC14_INSN_CLRWDT       0x0064
#define PIC14_MASK_CLRWDT       0x3FFF

    /* comf  : 00 1001 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_COMF         0x0900
#define PIC14_MASK_COMF         0x3F00

    /* decf  : 00 0011 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_DECF         0x0300
#define PIC14_MASK_DECF         0x3F00

    /* decfsz: 00 1011 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_DECFSZ       0x0B00
#define PIC14_MASK_DECFSZ       0x3F00

    /* goto  : 10 1kkk kkkk kkkk
               11 1000 0000 0000 */
#define PIC14_INSN_GOTO         0x2800
#define PIC14_MASK_GOTO         0x3800
#define PIC14_BMSK_GOTO         (PIC14_MASK_GOTO ^ PIC14_CORE_MASK)

    /* halt  : 00 0000 0110 0001
               11 1111 1111 1111 */
#define PIC14_INSN_HALT         0x0061
#define PIC14_MASK_HALT         0x3FFF

    /* incf  : 00 1010 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_INCF         0x0A00
#define PIC14_MASK_INCF         0x3F00

    /* incfsz: 00 1111 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_INCFSZ       0x0F00
#define PIC14_MASK_INCFSZ       0x3F00

    /* iorlw : 11 1000 kkkk kkkk
               11 1111 0000 0000 */
#define PIC14_INSN_IORLW        0x3800
#define PIC14_MASK_IORLW        0x3F00

    /* iorwf : 00 0100 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_IORWF        0x0400
#define PIC14_MASK_IORWF        0x3F00

    /* movf  : 00 1000 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_MOVF         0x0800
#define PIC14_MASK_MOVF         0x3F00

    /* movlw : 11 00xx kkkk kkkk
               11 1100 0000 0000 */
#define PIC14_INSN_MOVLW        0x3000
#define PIC14_MASK_MOVLW        0x3C00

    /* movwf : 00 0000 1fff ffff
               11 1111 1000 0000 */
#define PIC14_INSN_MOVWF        0x0080
#define PIC14_MASK_MOVWF        0x3F80

    /* nop   : 00 0000 0xx0 0000
               11 1111 1001 1111 */
#define PIC14_INSN_NOP          0x0000
#define PIC14_MASK_NOP          0x3F9F

    /* option: 00 0000 0110 0010
               11 1111 1111 1111 */
#define PIC14_INSN_OPTION       0x0062
#define PIC14_MASK_OPTION       0x3FFF

    /* retfie: 00 0000 0000 1001
               11 1111 1111 1111 */
#define PIC14_INSN_RETFIE       0x0009
#define PIC14_MASK_RETFIE       0x3FFF

    /* retlw : 11 01xx kkkk kkkk
               11 1100 0000 0000 */
#define PIC14_INSN_RETLW        0x3400
#define PIC14_MASK_RETLW        0x3C00

    /* return: 00 0000 0000 1000
               11 1111 1111 1111 */
#define PIC14_INSN_RETURN       0x0008
#define PIC14_MASK_RETURN       0x3FFF

    /* rlf   : 00 1101 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_RLF          0x0D00
#define PIC14_MASK_RLF          0x3F00

    /* rrf   : 00 1100 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_RRF          0x0C00
#define PIC14_MASK_RRF          0x3F00

    /* sleep : 00 0000 0110 0011
               11 1111 1111 1111 */
#define PIC14_INSN_SLEEP        0x0063
#define PIC14_MASK_SLEEP        0x3FFF

    /* sublw : 11 110x kkkk kkkk
               11 1110 0000 0000 */
#define PIC14_INSN_SUBLW        0x3C00
#define PIC14_MASK_SUBLW        0x3E00

    /* subwf : 00 0010 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_SUBWF        0x0200
#define PIC14_MASK_SUBWF        0x3F00

    /* swapf : 00 1110 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_SWAPF        0x0E00
#define PIC14_MASK_SWAPF        0x3F00

    /* tris  : 00 0000 0110 0fff
               11 1111 1111 1000 */
#define PIC14_INSN_TRIS         0x0060
#define PIC14_MASK_TRIS         0x3FF8

    /* xorlw : 11 1010 kkkk kkkk
               11 1111 0000 0000 */
#define PIC14_INSN_XORLW        0x3A00
#define PIC14_MASK_XORLW        0x3F00

    /* xorwf : 00 0110 dfff ffff
               11 1111 0000 0000 */
#define PIC14_INSN_XORWF        0x0600
#define PIC14_MASK_XORWF        0x3F00

    /* Same the mask of call and goto. */
#define PIC14_BMSK_BRANCH       PIC14_BMSK_CALL

    /*     PC mask:  1 1111 1111 1111 
       branch mask:     111 1111 1111 */
#define PIC14_PAGE_BITS         (PIC14_PC_MASK & (~PIC14_BMSK_BRANCH))

/******************************************
        PIC14E definitions
******************************************/

#define PIC14E_PC_MASK          ((1u << 15) - 1)

    /* addfsr: 11 0001 0nkk kkkk
               11 1111 1000 0000 */
#define PIC14E_INSN_ADDFSR      0x3100
#define PIC14E_MASK_ADDFSR      0x3F80

    /* addlw : 11 1110 kkkk kkkk
               11 1111 0000 0000 */
#define PIC14E_INSN_ADDLW       0x3E00
#define PIC14E_MASK_ADDLW       0x3F00

    /* addwfc: 11 1101 dfff ffff
               11 1111 0000 0000 */
#define PIC14E_INSN_ADDWFC      0x3D00
#define PIC14E_MASK_ADDWFC      0x3F00

    /* asrf  : 11 0111 dfff ffff
               11 1111 0000 0000 */
#define PIC14E_INSN_ASRF        0x3700
#define PIC14E_MASK_ASRF        0x3F00

    /* bra   : 11 001k kkkk kkkk
               11 1110 0000 0000 */
#define PIC14E_INSN_BRA         0x3200
#define PIC14E_MASK_BRA         0x3E00
#define PIC14E_BMSK_RBRA9       (PIC14E_MASK_BRA ^ PIC14_CORE_MASK)

    /* brw   : 00 0000 0000 1011
               11 1111 1111 1111 */
#define PIC14E_INSN_BRW         0x000B
#define PIC14E_MASK_BRW         0x3FFF

    /* callw : 00 0000 0000 1010
               11 1111 1111 1111 */
#define PIC14E_INSN_CALLW       0x000A
#define PIC14E_MASK_CALLW       0x3FFF

    /* lslf  : 11 0101 dfff ffff
               11 1111 0000 0000 */
#define PIC14E_INSN_LSLF        0x3500
#define PIC14E_MASK_LSLF        0x3F00

    /* lsrf  : 11 0110 dfff ffff
               11 1111 0000 0000 */
#define PIC14E_INSN_LSRF        0x3600
#define PIC14E_MASK_LSRF        0x3F00

    /* moviw : 00 0000 0001 0nmm
               11 1111 1111 1000 */
#define PIC14E_INSN_MOVIW       0x0010
#define PIC14E_MASK_MOVIW       0x3FF8

    /* moviw : 11 1111 0nkk kkkk
               11 1111 1000 0000 */
#define PIC14E_INSN_MOVIW_IDX   0x3F00
#define PIC14E_MASK_MOVIW_IDX   0x3F80

    /* movlw : 11 0000 kkkk kkkk
               11 1111 0000 0000 */
#define PIC14E_INSN_MOVLW       0x3000
#define PIC14E_MASK_MOVLW       0x3F00

    /* movlb : 00 0000 001k kkkk
               11 1111 1110 0000 */
#define PIC14E_INSN_MOVLB       0x0020
#define PIC14E_MASK_MOVLB       0x3FE0
#define PIC14E_BMSK_MOVLB       (PIC14E_MASK_BRA ^ PIC14_CORE_MASK)

    /* movlp : 11 0001 1kkk kkkk
               11 1111 1000 0000 */
#define PIC14E_INSN_MOVLP       0x3180
#define PIC14E_MASK_MOVLP       0x3F80

    /* movwi : 00 0000 0001 1nmm
               11 1111 1111 1000 */
#define PIC14E_INSN_MOVWI       0x0018
#define PIC14E_MASK_MOVWI       0x3FF8

    /* movwi : 11 1111 1nkk kkkk
               11 1111 1000 0000 */
#define PIC14E_INSN_MOVWI_IDX   0x3F80
#define PIC14E_MASK_MOVWI_IDX   0x3F80

    /* nop   : 00 0000 0000 0000
               11 1111 1111 1111 */
#define PIC14E_INSN_NOP         0x0000
#define PIC14E_MASK_NOP         0x3FFF

    /* reset : 00 0000 0000 0001
               11 1111 1111 1111 */
#define PIC14E_INSN_RESET       0x0001
#define PIC14E_MASK_RESET       0x3FFF

    /* retlw : 11 0100 kkkk kkkk
               11 1111 0000 0000 */
#define PIC14E_INSN_RETLW       0x3400
#define PIC14E_MASK_RETLW       0x3F00

    /* sublw : 11 1100 kkkk kkkk
               11 1111 0000 0000 */
#define PIC14E_INSN_SUBLW       0x3C00
#define PIC14E_MASK_SUBLW       0x3F00

    /* subwfb: 11 1011 dfff ffff
               11 1111 0000 0000 */
#define PIC14E_INSN_SUBWFB      0x3B00
#define PIC14E_MASK_SUBWFB      0x3F00

    /*     PC mask:  111 1111 1111 1111 
       branch mask:       111 1111 1111 */
#define PIC14E_PAGE_BITS        (PIC14E_PC_MASK & (~PIC14_BMSK_BRANCH))

/******************************************
        PIC16 definitions
******************************************/

#define PIC16_CORE_MASK         ((1u << 16) - 1)
#define PIC16_PC_MASK           ((1u << 16) - 1)

    /* General file bitmask. */
#define PIC16_BMSK_FILE         0xFF

    /* addlw : 1011 0001 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16_INSN_ADDLW        0xB100
#define PIC16_MASK_ADDLW        0xFF00

    /* addwf : 0000 111d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_ADDWF        0x0E00
#define PIC16_MASK_ADDWF        0xFE00

    /* addwf : 0001 000d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_ADDWFC       0x1000
#define PIC16_MASK_ADDWFC       0xFE00

    /* andlw : 1011 0101 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16_INSN_ANDLW        0xB500
#define PIC16_MASK_ANDLW        0xFF00

    /* andwf : 0000 101d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_ANDWF        0x0A00
#define PIC16_MASK_ANDWF        0xFE00

    /* bcf   : 1000 1bbb ffff ffff
               1111 1000 0000 0000 */
#define PIC16_INSN_BCF          0x8800
#define PIC16_MASK_BCF          0xF800

    /* bsf   : 1000 0bbb ffff ffff
               1111 1000 0000 0000 */
#define PIC16_INSN_BSF          0x8000
#define PIC16_MASK_BSF          0xF800

    /* btfsc : 1001 1bbb ffff ffff
               1111 1000 0000 0000 */
#define PIC16_INSN_BTFSC        0x9800
#define PIC16_MASK_BTFSC        0xF800

    /* btfss : 1001 0bbb ffff ffff
               1111 1000 0000 0000 */
#define PIC16_INSN_BTFSS        0x9000
#define PIC16_MASK_BTFSS        0xF800

    /* btg   : 0011 1bbb ffff ffff
               1111 1000 0000 0000 */
#define PIC16_INSN_BTG          0x3800
#define PIC16_MASK_BTG          0xF800

    /* call  : 111k kkkk kkkk kkkk
               1110 0000 0000 0000 */
#define PIC16_INSN_CALL         0xE000
#define PIC16_MASK_CALL         0xE000
#define PIC16_BMSK_CALL         (PIC16_MASK_CALL ^ PIC16_CORE_MASK)

    /* clrf  : 0010 100s ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_CLRF         0x2800
#define PIC16_MASK_CLRF         0xFE00

    /* clrwdt: 0000 0000 0000 0100
               1111 1111 1111 1111 */
#define PIC16_INSN_CLRWDT       0x0004
#define PIC16_MASK_CLRWDT       0xFFFF

    /* comf  : 0001 001d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_COMF         0x1200
#define PIC16_MASK_COMF         0xFE00

    /* cpfseq: 0011 0001 ffff ffff
               1111 1111 0000 0000 */
#define PIC16_INSN_CPFSEQ       0x3100
#define PIC16_MASK_CPFSEQ       0xFF00

    /* cpfsgt: 0011 0010 ffff ffff
               1111 1111 0000 0000 */
#define PIC16_INSN_CPFSGT       0x3200
#define PIC16_MASK_CPFSGT       0xFF00

    /* cpfslt: 0011 0000 ffff ffff
               1111 1111 0000 0000 */
#define PIC16_INSN_CPFSLT       0x3000
#define PIC16_MASK_CPFSLT       0xFF00

    /* daw   : 0010 111s ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_DAW          0x2E00
#define PIC16_MASK_DAW          0xFE00

    /* dcfsnz: 0010 011d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_DCFSNZ       0x2600
#define PIC16_MASK_DCFSNZ       0xFE00

    /* decf  : 0000 011d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_DECF         0x0600
#define PIC16_MASK_DECF         0xFE00

    /* decfsz: 0001 011d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_DECFSZ       0x1600
#define PIC16_MASK_DECFSZ       0xFE00

    /* goto  : 110k kkkk kkkk kkkk
               1110 0000 0000 0000 */
#define PIC16_INSN_GOTO         0xC000
#define PIC16_MASK_GOTO         0xE000
#define PIC16_BMSK_GOTO         (PIC16_MASK_GOTO ^ PIC16_CORE_MASK)

    /* incf  : 0001 010d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_INCF         0x1400
#define PIC16_MASK_INCF         0xFE00

    /* incfsz: 0001 111d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_INCFSZ       0x1E00
#define PIC16_MASK_INCFSZ       0xFE00

    /* infsnz: 0010 010d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_INFSNZ       0x2400
#define PIC16_MASK_INFSNZ       0xFE00

    /* iorlw : 1011 0011 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16_INSN_IORLW        0xB300
#define PIC16_MASK_IORLW        0xFF00

    /* iorwf : 0000 100d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_IORWF        0x0800
#define PIC16_MASK_IORWF        0xFE00

    /* lcall : 1011 0111 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16_INSN_LCALL        0xB700
#define PIC16_MASK_LCALL        0xFF00

    /* movfp : 011p pppp ffff ffff
               1110 0000 0000 0000 */
#define PIC16_INSN_MOVFP        0x6000
#define PIC16_MASK_MOVFP        0xE000

    /* movfp : 010p pppp ffff ffff
               1110 0000 0000 0000 */
#define PIC16_INSN_MOVPF        0x4000
#define PIC16_MASK_MOVPF        0xE000

    /* movlb : 1011 1000 uuuu kkkk
               1111 1111 0000 0000 */
#define PIC16_INSN_MOVLB        0xB800
#define PIC16_MASK_MOVLB        0xFF00
#define PIC16_BMSK_MOVLB        0x000F

    /* movlr : 1011 101x kkkk uuuu
               1111 1110 0000 0000 */
#define PIC16_INSN_MOVLR        0xBA00
#define PIC16_MASK_MOVLR        0xFE00
#define PIC16_BMSK_MOVLR        0x00F0

    /* movlw : 1011 0000 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16_INSN_MOVLW        0xB000
#define PIC16_MASK_MOVLW        0xFF00

    /* movwf : 0000 0001 ffff ffff
               1111 1111 0000 0000 */
#define PIC16_INSN_MOVWF        0x0100
#define PIC16_MASK_MOVWF        0xFF00

    /* mullw : 1011 1100 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16_INSN_MULLW        0xBC00
#define PIC16_MASK_MULLW        0xFF00

    /* mulwf : 0011 0100 ffff ffff
               1111 1111 0000 0000 */
#define PIC16_INSN_MULWF        0x3400
#define PIC16_MASK_MULWF        0xFF00

    /* negw  : 0010 110s ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_NEGW         0x2C00
#define PIC16_MASK_NEGW         0xFE00

    /* nop   : 0000 0000 0000 0000
               1111 1111 1111 1111 */
#define PIC16_INSN_NOP          0x0000
#define PIC16_MASK_NOP          0xFFFF

    /* retfie: 0000 0000 0000 0101
               1111 1111 1111 1111 */
#define PIC16_INSN_RETFIE       0x0005
#define PIC16_MASK_RETFIE       0xFFFF

    /* retlw : 1011 0110 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16_INSN_RETLW        0xB600
#define PIC16_MASK_RETLW        0xFF00

    /* return: 0000 0000 0000 0010
               1111 1111 1111 1111 */
#define PIC16_INSN_RETURN       0x0002
#define PIC16_MASK_RETURN       0xFFFF

    /* rlcf  : 0001 101d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_RLCF         0x1A00
#define PIC16_MASK_RLCF         0xFE00

    /* rlncf : 0010 001d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_RLNCF        0x2200
#define PIC16_MASK_RLNCF        0xFE00

    /* rrcf  : 0001 100d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_RRCF         0x1800
#define PIC16_MASK_RRCF         0xFE00

    /* rrncf : 0010 000d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_RRNCF        0x2000
#define PIC16_MASK_RRNCF        0xFE00

    /* setf  : 0010 101s ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_SETF         0x2A00
#define PIC16_MASK_SETF         0xFE00

    /* sleep : 0000 0000 0000 0011
               1111 1111 1111 1111 */
#define PIC16_INSN_SLEEP        0x0003
#define PIC16_MASK_SLEEP        0xFFFF

    /* sublw : 1011 0010 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16_INSN_SUBLW        0xB200
#define PIC16_MASK_SUBLW        0xFF00

    /* subwf : 0000 010d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_SUBWF        0x0400
#define PIC16_MASK_SUBWF        0xFE00

    /* subwfb: 0000 001d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_SUBWFB       0x0200
#define PIC16_MASK_SUBWFB       0xFE00

    /* swapf : 0001 110d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_SWAPF        0x1C00
#define PIC16_MASK_SWAPF        0xFE00

    /* tablrd: 1010 10ti ffff ffff
               1111 1100 0000 0000 */
#define PIC16_INSN_TABLRD       0xA800
#define PIC16_MASK_TABLRD       0xFC00

    /* tablwt: 1010 11ti ffff ffff
               1111 1100 0000 0000 */
#define PIC16_INSN_TABLWT       0xAC00
#define PIC16_MASK_TABLWT       0xFC00

    /* tlrd  : 1010 00tx ffff ffff
               1111 1100 0000 0000 */
#define PIC16_INSN_TLRD         0xA000
#define PIC16_MASK_TLRD         0xFC00

    /* tlwt  : 1010 01tx ffff ffff
               1111 1100 0000 0000 */
#define PIC16_INSN_TLWT         0xA400
#define PIC16_MASK_TLWT         0xFC00

    /* tstfsz: 0011 0011 ffff ffff
               1111 1111 0000 0000 */
#define PIC16_INSN_TSTFSZ       0x3300
#define PIC16_MASK_TSTFSZ       0xFF00

    /* xorlw : 1011 0100 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16_INSN_XORLW        0xB400
#define PIC16_MASK_XORLW        0xFF00

    /* xorwf : 0000 110d ffff ffff
               1111 1110 0000 0000 */
#define PIC16_INSN_XORWF        0x0C00
#define PIC16_MASK_XORWF        0xFE00

    /* Same the mask of call and goto. */
#define PIC16_BMSK_BRANCH       PIC16_BMSK_CALL

    /*     PC mask:  1111 1111 1111 1111 
       branch mask:     1 1111 1111 1111 */
#define PIC16_PAGE_BITS         (PIC16_PC_MASK & (~PIC16_BMSK_BRANCH))

/******************************************
        PIC16E definitions
******************************************/

    /* addlw : 0000 1111 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16E_INSN_ADDLW       0x0F00
#define PIC16E_MASK_ADDLW       0xFF00

    /* addwf : 0010 01da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_ADDWF       0x2400
#define PIC16E_MASK_ADDWF       0xFC00

    /* addwfc: 0010 00da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_ADDWFC      0x2000
#define PIC16E_MASK_ADDWFC      0xFC00

    /* andlw : 0000 1011 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16E_INSN_ANDLW       0x0B00
#define PIC16E_MASK_ANDLW       0xFF00

    /* andwf : 0001 01da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_ANDWF       0x1400
#define PIC16E_MASK_ANDWF       0xFC00

    /* bc    : 1110 0010 nnnn nnnn
               1111 1111 0000 0000 */
#define PIC16E_INSN_BC          0xE200
#define PIC16E_MASK_BC          0xFF00
#define PIC16E_BMSK_BC          (PIC16E_MASK_BC ^ PIC16_CORE_MASK)

    /* bcf   : 1001 bbba ffff ffff
               1111 0000 0000 0000 */
#define PIC16E_INSN_BCF         0x9000
#define PIC16E_MASK_BCF         0xF000

    /* bn    : 1110 0110 nnnn nnnn
               1111 1111 0000 0000 */
#define PIC16E_INSN_BN          0xE600
#define PIC16E_MASK_BN          0xFF00
#define PIC16E_BMSK_BN          (PIC16E_MASK_BN ^ PIC16_CORE_MASK)

    /* bnc   : 1110 0011 nnnn nnnn
               1111 1111 0000 0000 */
#define PIC16E_INSN_BNC         0xE300
#define PIC16E_MASK_BNC         0xFF00
#define PIC16E_BMSK_BNC         (PIC16E_MASK_BNC ^ PIC16_CORE_MASK)

    /* bnn   : 1110 0111 nnnn nnnn
               1111 1111 0000 0000 */
#define PIC16E_INSN_BNN         0xE700
#define PIC16E_MASK_BNN         0xFF00
#define PIC16E_BMSK_BNN         (PIC16E_MASK_BNN ^ PIC16_CORE_MASK)

    /* bnov  : 1110 0101 nnnn nnnn
               1111 1111 0000 0000 */
#define PIC16E_INSN_BNOV        0xE500
#define PIC16E_MASK_BNOV        0xFF00
#define PIC16E_BMSK_BNOV        (PIC16E_MASK_BNOV ^ PIC16_CORE_MASK)

    /* bnz   : 1110 0001 nnnn nnnn
               1111 1111 0000 0000 */
#define PIC16E_INSN_BNZ         0xE100
#define PIC16E_MASK_BNZ         0xFF00
#define PIC16E_BMSK_BNZ         (PIC16E_MASK_BNZ ^ PIC16_CORE_MASK)

    /* bov   : 1110 0100 nnnn nnnn
               1111 1111 0000 0000 */
#define PIC16E_INSN_BOV         0xE400
#define PIC16E_MASK_BOV         0xFF00
#define PIC16E_BMSK_BOV         (PIC16E_MASK_BOV ^ PIC16_CORE_MASK)

    /* bra   : 1101 0nnn nnnn nnnn
               1111 1000 0000 0000 */
#define PIC16E_INSN_BRA         0xD000
#define PIC16E_MASK_BRA         0xF800
#define PIC16E_BMSK_RBRA11      (PIC16E_MASK_BRA ^ PIC16_CORE_MASK)

    /* bsf   : 1000 bbba ffff ffff
               1111 0000 0000 0000 */
#define PIC16E_INSN_BSF         0x8000
#define PIC16E_MASK_BSF         0xF000

    /* btfsc : 1011 bbba ffff ffff
               1111 0000 0000 0000 */
#define PIC16E_INSN_BTFSC       0xB000
#define PIC16E_MASK_BTFSC       0xF000

    /* btfss : 1010 bbba ffff ffff
               1111 0000 0000 0000 */
#define PIC16E_INSN_BTFSS       0xA000
#define PIC16E_MASK_BTFSS       0xF000

    /* btg   : 0111 bbba ffff ffff
               1111 0000 0000 0000 */
#define PIC16E_INSN_BTG         0x7000
#define PIC16E_MASK_BTG         0xF000

    /* bz    : 1110 0000 nnnn nnnn
               1111 1111 0000 0000 */
#define PIC16E_INSN_BZ          0xE000
#define PIC16E_MASK_BZ          0xFF00
#define PIC16E_BMSK_BZ          (PIC16E_MASK_BZ ^ PIC16_CORE_MASK)

    /* call  : 1110 110s kkkk kkkk
               1111 kkkk kkkk kkkk

               1111 1110 0000 0000 */
#define PIC16E_INSN_CALL        0xEC00
#define PIC16E_MASK_CALL        0xFE00

    /* clrf  : 0110 101a ffff ffff
               1111 1110 0000 0000 */
#define PIC16E_INSN_CLRF        0x6A00
#define PIC16E_MASK_CLRF        0xFE00

    /* clrwdt: 0000 0000 0000 0100
               1111 1111 1111 1111 */
#define PIC16E_INSN_CLRWDT      0x0004
#define PIC16E_MASK_CLRWDT      0xFFFF

    /* comf  : 0001 11da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_COMF        0x1C00
#define PIC16E_MASK_COMF        0xFC00

    /* cpfseq: 0110 001a ffff ffff
               1111 1110 0000 0000 */
#define PIC16E_INSN_CPFSEQ      0x6200
#define PIC16E_MASK_CPFSEQ      0xFE00

    /* cpfsgt: 0110 010a ffff ffff
               1111 1110 0000 0000 */
#define PIC16E_INSN_CPFSGT      0x6400
#define PIC16E_MASK_CPFSGT      0xFE00

    /* cpfslt: 0110 000a ffff ffff
               1111 1110 0000 0000 */
#define PIC16E_INSN_CPFSLT      0x6000
#define PIC16E_MASK_CPFSLT      0xFE00

    /* daw   : 0000 0000 0000 0111
               1111 1111 1111 1111 */
#define PIC16E_INSN_DAW         0x0007
#define PIC16E_MASK_DAW         0xFFFF

    /* dcfsnz: 0100 11da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_DCFSNZ      0x4C00
#define PIC16E_MASK_DCFSNZ      0xFC00

    /* decf  : 0000 01da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_DECF        0x0400
#define PIC16E_MASK_DECF        0xFC00

    /* decfsz: 0010 11da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_DECFSZ      0x2C00
#define PIC16E_MASK_DECFSZ      0xFC00

    /* goto  : 1110 1111 kkkk kkkk
               1111 kkkk kkkk kkkk

               1111 1111 0000 0000 */
#define PIC16E_INSN_GOTO        0xEF00
#define PIC16E_MASK_GOTO        0xFF00

    /* halt  : 0000 0000 0000 0001
               1111 1111 1111 1111 */
#define PIC16E_INSN_HALT        0x0001
#define PIC16E_MASK_HALT        0xFFFF

    /* incf  : 0010 10da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_INCF        0x2800
#define PIC16E_MASK_INCF        0xFC00

    /* incfsz: 0011 11da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_INCFSZ      0x3C00
#define PIC16E_MASK_INCFSZ      0xFC00

    /* infsnz: 0100 10da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_INFSNZ      0x4800
#define PIC16E_MASK_INFSNZ      0xFC00

    /* iorlw : 0000 1001 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16E_INSN_IORLW       0x0900
#define PIC16E_MASK_IORLW       0xFF00

    /* iorwf : 0001 00da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_IORWF       0x1000
#define PIC16E_MASK_IORWF       0xFC00

    /* lfsr  : 1110 1110 00ff kkkk
               1111 0000 kkkk kkkk

               1111 1111 1100 0000 */
#define PIC16E_INSN_LFSR        0xEE00
#define PIC16E_MASK_LFSR        0xFFC0

    /* movf  : 0101 00da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_MOVF        0x5000
#define PIC16E_MASK_MOVF        0xFC00

    /* movff : 1100 ffff ffff ffff
               1111 ffff ffff ffff

               1111 0000 0000 0000 */
#define PIC16E_INSN_MOVFF       0xC000
#define PIC16E_MASK_MOVFF       0xF000

    /* movlb : 0000 0001 0000 kkkk
               1111 1111 1111 0000 */
#define PIC16E_INSN_MOVLB       0x0100
#define PIC16E_MASK_MOVLB       0xFFF0
#define PIC16E_BMSK_MOVLB       (PIC16E_MASK_MOVLB ^ PIC16_CORE_MASK)

    /* movlw : 0000 1110 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16E_INSN_MOVLW       0x0E00
#define PIC16E_MASK_MOVLW       0xFF00

    /* movwf : 0110 111a ffff ffff
               1111 1110 0000 0000 */
#define PIC16E_INSN_MOVWF       0x6E00
#define PIC16E_MASK_MOVWF       0xFE00

    /* mullw : 0000 1101 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16E_INSN_MULLW       0x0D00
#define PIC16E_MASK_MULLW       0xFF00

    /* mulwf : 0000 001a ffff ffff
               1111 1110 0000 0000 */
#define PIC16E_INSN_MULWF       0x0200
#define PIC16E_MASK_MULWF       0xFE00

    /* negf  : 0110 110a ffff ffff
               1111 1110 0000 0000 */
#define PIC16E_INSN_NEGF        0x6C00
#define PIC16E_MASK_NEGF        0xFE00

    /* nop   : 0000 0000 0000 0000 */
    /* nop   : 1111 xxxx xxxx xxxx

               1111 1111 1111 1111 */
#define PIC16E_INSN_NOP         0x0000
#define PIC16E_MASK_NOP         0xFFFF

    /* pop   : 0000 0000 0000 0110
               1111 1111 1111 1111 */
#define PIC16E_INSN_POP         0x0006
#define PIC16E_MASK_POP         0xFFFF

    /* push  : 0000 0000 0000 0101
               1111 1111 1111 1111 */
#define PIC16E_INSN_PUSH        0x0005
#define PIC16E_MASK_PUSH        0xFFFF

    /* rcall : 1101 1nnn nnnn nnnn
               1111 1000 0000 0000 */
#define PIC16E_INSN_RCALL       0xD800
#define PIC16E_MASK_RCALL       0xF800

    /* reset : 0000 0000 1111 1111
               1111 1111 1111 1111 */
#define PIC16E_INSN_RESET       0x00FF
#define PIC16E_MASK_RESET       0xFFFF

    /* retfie: 0000 0000 0001 000s
               1111 1111 1111 1110 */
#define PIC16E_INSN_RETFIE      0x0010
#define PIC16E_MASK_RETFIE      0xFFFE

    /* retlw : 0000 1100 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16E_INSN_RETLW       0x0C00
#define PIC16E_MASK_RETLW       0xFF00

    /* return: 0000 0000 0001 001s
               1111 1111 1111 1110 */
#define PIC16E_INSN_RETURN      0x0012
#define PIC16E_MASK_RETURN      0xFFFE

    /* rlcf  : 0011 01da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_RLCF        0x3400
#define PIC16E_MASK_RLCF        0xFC00

    /* rlncf : 0100 01da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_RLNCF       0x4400
#define PIC16E_MASK_RLNCF       0xFC00

    /* rrcf  : 0011 00da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_RRCF        0x3000
#define PIC16E_MASK_RRCF        0xFC00

    /* rrncf : 0100 00da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_RRNCF       0x4000
#define PIC16E_MASK_RRNCF       0xFC00

    /* setf  : 0110 100a ffff ffff
               1111 1110 0000 0000 */
#define PIC16E_INSN_SETF        0x6800
#define PIC16E_MASK_SETF        0xFE00

    /* sleep : 0000 0000 0000 0011
               1111 1111 1111 1111 */
#define PIC16E_INSN_SLEEP       0x0003
#define PIC16E_MASK_SLEEP       0xFFFF

    /* subfwb: 0101 01da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_SUBFWB      0x5400
#define PIC16E_MASK_SUBFWB      0xFC00

    /* sublw : 0000 1000 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16E_INSN_SUBLW       0x0800
#define PIC16E_MASK_SUBLW       0xFF00

    /* subwf : 0101 11da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_SUBWF       0x5C00
#define PIC16E_MASK_SUBWF       0xFC00

    /* subwfb: 0101 10da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_SUBWFB      0x5800
#define PIC16E_MASK_SUBWFB      0xFC00

    /* swapf : 0011 10da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_SWAPF       0x3800
#define PIC16E_MASK_SWAPF       0xFC00

    /* tblrd : 0000 0000 0000 10ii
               1111 1111 1111 1100 */
#define PIC16E_INSN_TBLRD       0x0008
#define PIC16E_MASK_TBLRD       0xFFFC

    /* tblwt : 0000 0000 0000 11ii
               1111 1111 1111 1100 */
#define PIC16E_INSN_TBLWT       0x000C
#define PIC16E_MASK_TBLWT       0xFFFC

    /* trap  : 0000 0000 1110 0000
               1111 1111 1111 1111 */
#define PIC16E_INSN_TRAP        0x00E0
#define PIC16E_MASK_TRAP        0xFFFF

    /* tret  : 0000 0000 1110 0001
               1111 1111 1111 1111 */
#define PIC16E_INSN_TRET        0x00E1
#define PIC16E_MASK_TRET        0xFFFF

    /* tstfsz: 0110 011a ffff ffff
               1111 1110 0000 0000 */
#define PIC16E_INSN_TSTFSZ      0x6600
#define PIC16E_MASK_TSTFSZ      0xFE00

    /* xorlw : 0000 1010 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16E_INSN_XORLW       0x0A00
#define PIC16E_MASK_XORLW       0xFF00

    /* xorwf : 0001 10da ffff ffff
               1111 1100 0000 0000 */
#define PIC16E_INSN_XORWF       0x1800
#define PIC16E_MASK_XORWF       0xFC00

    /* This is identical to the following cases:
       PIC16E_BMSK_BC,  PIC16E_BMSK_BN,   PIC16E_BMSK_BNC,
       PIC16E_BMSK_BNN, PIC16E_BMSK_BNOV, PIC16E_BMSK_BNZ,
       PIC16E_BMSK_BOV, PIC16E_BMSK_BZ */
#define PIC16E_BMSK_RBRA8       PIC16E_BMSK_BC

/******************************************
        PIC16E special instructions
******************************************/

#define PIC16ES_INSN_CLRC       0x90D8
#define PIC16ES_INSN_CLRDC      0x92D8
#define PIC16ES_INSN_CLRN       0x98D8
#define PIC16ES_INSN_CLROV      0x96D8
#define PIC16ES_INSN_CLRW       0x6AE8
#define PIC16ES_INSN_CLRZ       0x94D8
#define PIC16ES_INSN_SETC       0x80D8
#define PIC16ES_INSN_SETDC      0x82D8
#define PIC16ES_INSN_SETN       0x88D8
#define PIC16ES_INSN_SETOV      0x86D8
#define PIC16ES_INSN_SETZ       0x84D8
#define PIC16ES_INSN_SKPC       0xA0D8
#define PIC16ES_INSN_SKPDC      0xA2D8
#define PIC16ES_INSN_SKPN       0xA8D8
#define PIC16ES_INSN_SKPOV      0xA6D8
#define PIC16ES_INSN_SKPZ       0xA4D8
#define PIC16ES_INSN_SKPNC      0xB0D8
#define PIC16ES_INSN_SKPNDC     0xB2D8
#define PIC16ES_INSN_SKPNN      0xB8D8
#define PIC16ES_INSN_SKPNOV     0xB6D8
#define PIC16ES_INSN_SKPNZ      0xB4D8
#define PIC16ES_INSN_TGC        0x70D8
#define PIC16ES_INSN_TGDC       0x72D8
#define PIC16ES_INSN_TGN        0x78D8
#define PIC16ES_INSN_TGOV       0x76D8
#define PIC16ES_INSN_TGZ        0x74D8


/******************************************
        PIC16E extended instructions
******************************************/

    /* addfsr: 1110 1000 ffkk kkkk
               1111 1111 0000 0000 */
#define PIC16EX_INSN_ADDFSR     0xE800
#define PIC16EX_MASK_ADDFSR     0xFF00

    /* addulnk: 1110 1000 11kk kkkk
                1111 1111 1100 0000 */
#define PIC16EX_INSN_ADDULNK    0xE8C0
#define PIC16EX_MASK_ADDULNK    0xFFC0
#define PIC16EX_BMSK_ADDULNK    (PIC16EX_MASK_ADDULNK ^ PIC16_CORE_MASK)

    /* callw : 0000 0000 0001 0100
               1111 1111 1111 1111 */
#define PIC16EX_INSN_CALLW      0x0014
#define PIC16EX_MASK_CALLW      0xFFFF

    /* movsf : 1110 1011 0zzz zzzz
               1111 ffff ffff ffff

               1111 1111 1000 0000 */
#define PIC16EX_INSN_MOVSF      0xEB00
#define PIC16EX_MASK_MOVSF      0xFF80

    /* movss : 1110 1011 1zzz zzzz
               1111 xxxx xzzz zzzz

               1111 1111 1000 0000 */
#define PIC16EX_INSN_MOVSS      0xEB80
#define PIC16EX_MASK_MOVSS      0xFF80

    /* pushl : 1110 1010 kkkk kkkk
               1111 1111 0000 0000 */
#define PIC16EX_INSN_PUSHL      0xEA00
#define PIC16EX_MASK_PUSHL      0xFF00

    /* subfsr: 1110 1001 ffkk kkkk
               1111 1111 0000 0000 */
#define PIC16EX_INSN_SUBFSR     0xE900
#define PIC16EX_MASK_SUBFSR     0xFF00

    /* subulnk: 1110 1001 11kk kkkk
                1111 1111 1100 0000 */
#define PIC16EX_INSN_SUBULNK    0xE9C0
#define PIC16EX_MASK_SUBULNK    0xFFC0
#define PIC16EX_BMSK_SUBULNK    (PIC16EX_MASK_SUBULNK ^ PIC16_CORE_MASK)

    /* This is identical to PIC16EX_BMSK_SUBULNK. */
#define PIC16EX_BMSK_xxxULNK    PIC16EX_BMSK_ADDULNK


enum common_insn {
  ICODE_ADDFSR,
  ICODE_ADDLW,
  ICODE_ADDULNK,
  ICODE_ADDWF,
  ICODE_ADDWFC,
  ICODE_ANDLW,
  ICODE_ANDWF,
  ICODE_ASRF,
  ICODE_BANK,
  ICODE_BC,
  ICODE_BCF,
  ICODE_BN,
  ICODE_BNC,
  ICODE_BNN,
  ICODE_BNOV,
  ICODE_BNZ,
  ICODE_BOV,
  ICODE_BRA,
  ICODE_BRW,
  ICODE_BSF,
  ICODE_BTFSC,
  ICODE_BTFSS,
  ICODE_BTG,
  ICODE_BZ,
  ICODE_CALL,
  ICODE_CALLW,
  ICODE_CLRF,
  ICODE_CLRW,
  ICODE_CLRWDT,
  ICODE_COMF,
  ICODE_CPFSEQ,
  ICODE_CPFSGT,
  ICODE_CPFSLT,
  ICODE_DAW,
  ICODE_DCFSNZ,
  ICODE_DECF,
  ICODE_DECFSZ,
  ICODE_GOTO,
  ICODE_HALT,
  ICODE_INCF,
  ICODE_INCFSZ,
  ICODE_INFSNZ,
  ICODE_IORLW,
  ICODE_IORWF,
  ICODE_IREAD,
  ICODE_LCALL,
  ICODE_LFSR,
  ICODE_LSLF,
  ICODE_LSRF,
  ICODE_MODE,
  ICODE_MOVF,
  ICODE_MOVFF,
  ICODE_MOVFP,
  ICODE_MOVIW,
  ICODE_MOVLB,
  ICODE_MOVLP,
  ICODE_MOVLR,
  ICODE_MOVLW,
  ICODE_MOVMW,
  ICODE_MOVPF,
  ICODE_MOVSF,
  ICODE_MOVSS,
  ICODE_MOVWF,
  ICODE_MOVWI,
  ICODE_MOVWM,
  ICODE_MULLW,
  ICODE_MULWF,
  ICODE_NEGF,
  ICODE_NEGW,
  ICODE_NOP,
  ICODE_OPTION,
  ICODE_PAGE,
  ICODE_POP,
  ICODE_PUSH,
  ICODE_PUSHL,
  ICODE_RCALL,
  ICODE_RESET,
  ICODE_RETFIE,
  ICODE_RETI,
  ICODE_RETIW,
  ICODE_RETLW,
  ICODE_RETP,
  ICODE_RETURN,
  ICODE_RLCF,
  ICODE_RLF,
  ICODE_RLNCF,
  ICODE_RRCF,
  ICODE_RRF,
  ICODE_RRNCF,
  ICODE_SETF,
  ICODE_SLEEP,
  ICODE_SUBFSR,
  ICODE_SUBFWB,
  ICODE_SUBLW,
  ICODE_SUBULNK,
  ICODE_SUBWF,
  ICODE_SUBWFB,
  ICODE_SWAPF,
  ICODE_TABLRD,
  ICODE_TABLWT,
  ICODE_TBLRD,
  ICODE_TBLWT,
  ICODE_TLRD,
  ICODE_TLWT,
  ICODE_TRAP,
  ICODE_TRET,
  ICODE_TRIS,
  ICODE_TSTFSZ,
  ICODE_XORLW,
  ICODE_XORWF
};

typedef int gpasmVal;   /* The type that internal arithmetic uses. */

struct pnode;           /* forward declaration; defined in
                         * gpasm.h for assembler and in
                         * script.h for linker */

struct insn {
  const char *name;
  unsigned int mask;
  unsigned int opcode;
  enum common_insn icode;
  enum insn_class class;
  unsigned int attribs;
  gpasmVal (*doer)(gpasmVal r, const char *name, int arity, struct pnode *parms);
};

struct strict_insn {
  const char *name;
  unsigned int mask;
};

#define TABLE_SIZE(X)       (sizeof(X) / sizeof((X)[0]))

extern const struct insn op_12c5xx[];
extern const int num_op_12c5xx;

extern const struct insn op_16c5xx_enh[];
extern const int num_op_16c5xx_enh;

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

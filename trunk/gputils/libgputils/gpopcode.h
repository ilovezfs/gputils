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
  INSN_CLASS_LIT4S,     /* bits 7:4 contain a 4 bit literal, bits 3:0 are unused   */
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

enum insn_pic12 {
    /* addwf : 0001 11df ffff */
  INSN_PIC12_ADDWF      = 0x1C0,
    /* andlw : 0001 01df ffff */
  INSN_PIC12_ANDLW      = 0xE00,
    /* andwf : 0000 011f ffff */
  INSN_PIC12_ANDWF      = 0x140,
    /* bcf   : 0100 bbbf ffff */
  INSN_PIC12_BCF        = 0x400,
    /* bsf   : 0101 bbbf ffff */
  INSN_PIC12_BSF        = 0x500,
    /* btfsc : 0110 bbbf ffff */
  INSN_PIC12_BTFSC      = 0x600,
    /* btfss : 0111 bbbf ffff */
  INSN_PIC12_BTFSS      = 0x700,
    /* call  : 1001 kkkk kkkk */
  INSN_PIC12_CALL       = 0x900,
    /* clrf  : 0000 011f ffff */
  INSN_PIC12_CLRF       = 0x060,
    /* clrw  : 0000 0100 0000 */
  INSN_PIC12_CLRW       = 0x040,
    /* clrwdt: 0000 0000 0100 */
  INSN_PIC12_CLRWDT     = 0x004,
    /* comf  : 0010 01df ffff */
  INSN_PIC12_COMF       = 0x240,
    /* decf  : 0000 11df ffff */
  INSN_PIC12_DECF       = 0x0C0,
    /* decfsz: 0010 11df ffff */
  INSN_PIC12_DECFSZ     = 0x2C0,
    /* goto  : 101k kkkk kkkk */
  INSN_PIC12_GOTO       = 0xA00,
    /* incf  : 0010 10df ffff */
  INSN_PIC12_INCF       = 0x280,
    /* incfsz: 0011 11df ffff */
  INSN_PIC12_INCFSZ     = 0x3C0,
    /* iorlw : 1101 kkkk kkkk */
  INSN_PIC12_IORLW      = 0xD00,
    /* iorwf : 0001 00df ffff */
  INSN_PIC12_IORWF      = 0x100,
    /* movf  : 0010 00df ffff */
  INSN_PIC12_MOVF       = 0x200,
    /* movlw : 1100 kkkk kkkk */
  INSN_PIC12_MOVLW      = 0xC00,
    /* movwf : 0000 001f ffff */
  INSN_PIC12_MOVWF      = 0x020,
    /* nop   : 0000 0000 0000 */
  INSN_PIC12_NOP        = 0x000,
    /* option: 0000 0000 0010 */
  INSN_PIC12_OPTION     = 0x002,
    /* retlw : 1000 kkkk kkkk */
  INSN_PIC12_RETLW      = 0x800,
    /* return: 1000 kkkk kkkk */
  INSN_PIC12_RETURN     = 0x800,
    /* rlf   : 0011 01df ffff */
  INSN_PIC12_RLF        = 0x340,
    /* rrf   : 0011 00df ffff */
  INSN_PIC12_RRF        = 0x300,
    /* sleep : 0000 0000 0011 */
  INSN_PIC12_SLEEP      = 0x003,
    /* subwf : 0000 10df ffff */
  INSN_PIC12_SUBWF      = 0x080,
    /* swapf : 0011 10df ffff */
  INSN_PIC12_SWAPF      = 0x380,
    /* tris  : 0000 0000 0fff */
  INSN_PIC12_TRIS       = 0x000,
    /* xorlw : 1111 kkkk kkkk */
  INSN_PIC12_XORLW      = 0xF00,
    /* xorwf : 0001 10df ffff */
  INSN_PIC12_XORWF      = 0x180
};

/******************************************
        PIC12E definitions
******************************************/

enum insn_pic12e {
    /* movlb : 0000 0001 0kkk */
  INSN_PIC12E_MOVLB     = 0x010,
    /* retfie: 0000 0001 1111 */
  INSN_PIC12E_RETFIE    = 0x01F,
    /* return: 0000 0001 1110 */
  INSN_PIC12E_RETURN    = 0x01E
};

/******************************************
        SX definitions
******************************************/

enum insn_sx {
  INSN_SX_BANK          = 0x018,
  INSN_SX_IREAD         = 0x041,
  INSN_SX_MODE          = 0x050,
  INSN_SX_MOVMW         = 0x042,
  INSN_SX_MOVWM         = 0x043,
  INSN_SX_PAGE          = 0x010,
  INSN_SX_RETI          = 0x00E,
  INSN_SX_RETIW         = 0x00F,
  INSN_SX_RETP          = 0x00D,
  INSN_SX_RETURN        = 0x00C
};

/******************************************
        PIC14 definitions
******************************************/

#define INSN_PIC14_BxF_BITSHIFT     7

enum insn_pic14 {
    /* addlw : 11 111x kkkk kkkk */
  INSN_PIC14_ADDLW      = 0x3E00,
    /* addwf : 00 0111 dfff ffff */
  INSN_PIC14_ADDWF      = 0x0700,
    /* andlw : 11 1001 kkkk kkkk */
  INSN_PIC14_ANDLW      = 0x3900,
    /* andwf : 00 0101 dfff ffff */
  INSN_PIC14_ANDWF      = 0x0500,
    /* bcf   : 01 00bb bfff ffff */
  INSN_PIC14_BCF        = 0x1000,
    /* bsf   : 01 01bb bfff ffff */
  INSN_PIC14_BSF        = 0x1400,
    /* btfsc : 01 10bb bfff ffff */
  INSN_PIC14_BTFSC      = 0x1800,
    /* btfss : 01 11bb bfff ffff */
  INSN_PIC14_BTFSS      = 0x1C00,
    /* call  : 10 0kkk kkkk kkkk */
  INSN_PIC14_CALL       = 0x2000,
    /* clrf  : 00 0001 1fff ffff */
  INSN_PIC14_CLRF       = 0x0180,
    /* clrw  : 00 0001 0xxx xxxx */
  INSN_PIC14_CLRW       = 0x0103,
    /* clrwdt: 00 0000 0110 0100 */
  INSN_PIC14_CLRWDT     = 0x0064,
    /* comf  : 00 1001 dfff ffff */
  INSN_PIC14_COMF       = 0x0900,
    /* decf  : 00 0011 dfff ffff */
  INSN_PIC14_DECF       = 0x0300,
    /* decfsz: 00 1011 dfff ffff */
  INSN_PIC14_DECFSZ     = 0x0B00,
    /* goto  : 10 1kkk kkkk kkkk */
  INSN_PIC14_GOTO       = 0x2800,
    /* halt  : 00 0000 0110 0001 */
  INSN_PIC14_HALT       = 0x0061,
    /* incf  : 00 1010 dfff ffff */
  INSN_PIC14_INCF       = 0x0A00,
    /* incfsz: 00 1111 dfff ffff */
  INSN_PIC14_INCFSZ     = 0x0F00,
    /* iorlw : 11 1000 kkkk kkkk */
  INSN_PIC14_IORLW      = 0x3800,
    /* iorwf : 00 0100 dfff ffff */
  INSN_PIC14_IORWF      = 0x0400,
    /* movf  : 00 1000 dfff ffff */
  INSN_PIC14_MOVF       = 0x0800,
    /* movlw : 11 00xx kkkk kkkk */
  INSN_PIC14_MOVLW      = 0x3000,
    /* movwf : 00 0000 1fff ffff */
  INSN_PIC14_MOVWF      = 0x0080,
    /* nop   : 00 0000 0xx0 0000 */
  INSN_PIC14_NOP        = 0x0000,
    /* option: 00 0000 0110 0010 */
  INSN_PIC14_OPTION     = 0x0062,
    /* retfie: 00 0000 0000 1001 */
  INSN_PIC14_RETFIE     = 0x0009,
    /* retlw : 11 01xx kkkk kkkk */
  INSN_PIC14_RETLW      = 0x3400,
    /* return: 00 0000 0000 1000 */
  INSN_PIC14_RETURN     = 0x0008,
    /* rlf   : 00 1101 dfff ffff */
  INSN_PIC14_RLF        = 0x0D00,
    /* rrf   : 00 1100 dfff ffff */
  INSN_PIC14_RRF        = 0x0C00,
    /* sleep : 00 0000 0110 0011 */
  INSN_PIC14_SLEEP      = 0x0063,
    /* sublw : 11 110x kkkk kkkk */
  INSN_PIC14_SUBLW      = 0x3C00,
    /* subwf : 00 0010 dfff ffff */
  INSN_PIC14_SUBWF      = 0x0200,
    /* swapf : 00 1110 dfff ffff */
  INSN_PIC14_SWAPF      = 0x0E00,
    /* tris  : 00 0000 0110 0000 */
  INSN_PIC14_TRIS       = 0x0060,
    /* xorlw : 11 1010 kkkk kkkk */
  INSN_PIC14_XORLW      = 0x3A00,
    /* xorwf : 00 0110 dfff ffff */
  INSN_PIC14_XORWF      = 0x0600
};

/******************************************
        PIC14E definitions
******************************************/

enum insn_pic14e {
    /* addfsr: 11 0001 0nkk kkkk */
  INSN_PIC14E_ADDFSR    = 0x3100,
    /* addwfc: 11 1101 dfff ffff */
  INSN_PIC14E_ADDWFC    = 0x3D00,
    /* asrf  : 11 0111 dfff ffff */
  INSN_PIC14E_ASRF      = 0x3700,
    /* bra   : 11 001k kkkk kkkk */
  INSN_PIC14E_BRA       = 0x3200,
    /* brw   : 00 0000 0000 1011 */
  INSN_PIC14E_BRW       = 0x000B,
    /* callw : 00 0000 0000 1010 */
  INSN_PIC14E_CALLW     = 0x000A,
    /* lslf  : 11 0101 dfff ffff */
  INSN_PIC14E_LSLF      = 0x3500,
    /* lsrf  : 11 0110 dfff ffff */
  INSN_PIC14E_LSRF      = 0x3600,
    /* moviw : 00 0000 0001 0nmm */
  INSN_PIC14E_MOVIW     = 0x0010,
    /* moviw : 11 1111 0nkk kkkk */
  INSN_PIC14E_MOVIW_IDX = 0x3F00,
    /* movwi : 00 0000 0001 1nmm */
  INSN_PIC14E_MOVWI     = 0x0018,
    /* movwi : 11 1111 1nkk kkkk */
  INSN_PIC14E_MOVWI_IDX = 0x3F80,
    /* movlb : 00 0000 001k kkkk */
  INSN_PIC14E_MOVLB     = 0x0020,
    /* movlp : 11 0001 1kkk kkkk */
  INSN_PIC14E_MOVLP     = 0x3180,
    /* reset : 00 0000 0000 0001 */
  INSN_PIC14E_RESET     = 0x0001,
    /* subwfb: 11 1011 dfff ffff */
  INSN_PIC14E_SUBWFB    = 0x3B00
};

/******************************************
        PIC16 definitions
******************************************/

enum insn_pic16 {
    /* addlw : 1011 0001 kkkk kkkk */
  INSN_PIC16_ADDLW      = 0xB100,
    /* addwf : 0000 111d ffff ffff */
  INSN_PIC16_ADDWF      = 0x0E00,
    /* addwf : 0001 000d ffff ffff */
  INSN_PIC16_ADDWFC     = 0x1000,
    /* andlw : 1011 0101 kkkk kkkk */
  INSN_PIC16_ANDLW      = 0xB500,
    /* andwf : 0000 101d ffff ffff */
  INSN_PIC16_ANDWF      = 0x0A00,
    /* bcf   : 1000 1bbb ffff ffff */
  INSN_PIC16_BCF        = 0x8800,
    /* bsf   : 1000 0bbb ffff ffff */
  INSN_PIC16_BSF        = 0x8000,
    /* btfsc : 1001 1bbb ffff ffff */
  INSN_PIC16_BTFSC      = 0x9800,
    /* btfss : 1001 0bbb ffff ffff */
  INSN_PIC16_BTFSS      = 0x9000,
    /* btg   : 0011 1bbb ffff ffff */
  INSN_PIC16_BTG        = 0x3800,
    /* call  : 111k kkkk kkkk kkkk */
  INSN_PIC16_CALL       = 0xE000,
    /* clrf  : 0010 100s ffff ffff */
  INSN_PIC16_CLRF       = 0x2800,
    /* clrwdt: 0000 0000 0000 0100 */
  INSN_PIC16_CLRWDT     = 0x0004,
    /* comf  : 0001 001d ffff ffff */
  INSN_PIC16_COMF       = 0x1200,
    /* cpfseq: 0011 0001 ffff ffff */
  INSN_PIC16_CPFSEQ     = 0x3100,
    /* cpfsgt: 0011 0010 ffff ffff */
  INSN_PIC16_CPFSGT     = 0x3200,
    /* cpfslt: 0011 0000 ffff ffff */
  INSN_PIC16_CPFSLT     = 0x3000,
    /* daw   : 0010 111s ffff ffff */
  INSN_PIC16_DAW        = 0x2E00,
    /* decf  : 0000 011d ffff ffff */
  INSN_PIC16_DECF       = 0x0600,
    /* decfsz: 0001 011d ffff ffff */
  INSN_PIC16_DECFSZ     = 0x1600,
    /* dcfsnz: 0010 011d ffff ffff */
  INSN_PIC16_DCFSNZ     = 0x2600,
    /* goto  : 110k kkkk kkkk kkkk */
  INSN_PIC16_GOTO       = 0xC000,
    /* incf  : 0001 010d ffff ffff */
  INSN_PIC16_INCF       = 0x1400,
    /* incfsz: 0001 111d ffff ffff */
  INSN_PIC16_INCFSZ     = 0x1E00,
    /* infsnz: 0010 010d ffff ffff */
  INSN_PIC16_INFSNZ     = 0x2400,
    /* iorlw : 1011 0011 kkkk kkkk */
  INSN_PIC16_IORLW      = 0xB300,
    /* iorwf : 0000 100d ffff ffff */
  INSN_PIC16_IORWF      = 0x0800,
    /* lcall : 1011 0111 kkkk kkkk */
  INSN_PIC16_LCALL      = 0xB700,
    /* movfp : 011p pppp ffff ffff */
  INSN_PIC16_MOVFP      = 0x6000,
    /* movfp : 010p pppp ffff ffff */
  INSN_PIC16_MOVPF      = 0x4000,
    /* movlb : 1011 1000 uuuu kkkk */
  INSN_PIC16_MOVLB      = 0xB800,
    /* movlr : 1011 101x kkkk uuuu */
  INSN_PIC16_MOVLR      = 0xBA00,
    /* movlw : 1011 0000 kkkk kkkk */
  INSN_PIC16_MOVLW      = 0xB000,
    /* movwf : 0000 0001 ffff ffff */
  INSN_PIC16_MOVWF      = 0x0100,
    /* mullw : 1011 1100 kkkk kkkk */
  INSN_PIC16_MULLW      = 0xBC00,
    /* mulwf : 0011 0100 ffff ffff */
  INSN_PIC16_MULWF      = 0x3400,
    /* negw  : 0010 110s ffff ffff */
  INSN_PIC16_NEGW       = 0x2C00,
    /* nop   : 0000 0000 0000 0000 */
  INSN_PIC16_NOP        = 0x0000,
    /* retfie: 0000 0000 0000 0101 */
  INSN_PIC16_RETFIE     = 0x0005,
    /* retlw : 1011 0110 kkkk kkkk */
  INSN_PIC16_RETLW      = 0xB600,
    /* return: 0000 0000 0000 0010 */
  INSN_PIC16_RETURN     = 0x0002,
    /* rlcf  : 0001 101d ffff ffff */
  INSN_PIC16_RLCF       = 0x1A00,
    /* rlncf : 0010 001d ffff ffff */
  INSN_PIC16_RLNCF      = 0x2200,
    /* rrcf  : 0001 100d ffff ffff */
  INSN_PIC16_RRCF       = 0x1800,
    /* rrncf : 0010 000d ffff ffff */
  INSN_PIC16_RRNCF      = 0x2000,
    /* setf  : 0010 101s ffff ffff */
  INSN_PIC16_SETF       = 0x2A00,
    /* sleep : 0000 0000 0000 0011 */
  INSN_PIC16_SLEEP      = 0x0003,
    /* sublw : 1011 0010 kkkk kkkk */
  INSN_PIC16_SUBLW      = 0xB200,
    /* subwf : 0000 010d ffff ffff */
  INSN_PIC16_SUBWF      = 0x0400,
    /* subwfb: 0000 001d ffff ffff */
  INSN_PIC16_SUBWFB     = 0x0200,
    /* swapf : 0001 110d ffff ffff */
  INSN_PIC16_SWAPF      = 0x1C00,
    /* tablrd: 1010 10ti ffff ffff */
  INSN_PIC16_TABLRD     = 0xA800,
    /* tablwt: 1010 11ti ffff ffff */
  INSN_PIC16_TABLWT     = 0xAC00,
    /* tlrd  : 1010 00tx ffff ffff */
  INSN_PIC16_TLRD       = 0xA000,
    /* tlwt  : 1010 01tx ffff ffff */
  INSN_PIC16_TLWT       = 0xA400,
    /* tstfsz: 0011 0011 ffff ffff */
  INSN_PIC16_TSTFSZ     = 0x3300,
    /* xorlw : 1011 0100 kkkk kkkk */
  INSN_PIC16_XORLW      = 0xB400,
    /* xorwf : 0000 110d ffff ffff */
  INSN_PIC16_XORWF      = 0x0C00
};

/******************************************
        PIC16E definitions
******************************************/

enum insn_pic16e {
    /* addlw : 0000 1111 kkkk kkkk */
  INSN_PIC16E_ADDLW     = 0x0F00,
    /* addwf : 0010 01da ffff ffff */
  INSN_PIC16E_ADDWF     = 0x2400,
    /* addwfc: 0010 00da ffff ffff */
  INSN_PIC16E_ADDWFC    = 0x2000,
    /* andlw : 0000 1011 kkkk kkkk */
  INSN_PIC16E_ANDLW     = 0x0B00,
    /* andwf : 0001 01da ffff ffff */
  INSN_PIC16E_ANDWF     = 0x1400,
    /* bc    : 1110 0010 nnnn nnnn */
  INSN_PIC16E_BC        = 0xE200,
    /* bcf   : 1001 bbba ffff ffff */
  INSN_PIC16E_BCF       = 0x9000,
    /* bn    : 1110 0110 nnnn nnnn */
  INSN_PIC16E_BN        = 0xE600,
    /* bnc   : 1110 0011 nnnn nnnn */
  INSN_PIC16E_BNC       = 0xE300,
    /* bnn   : 1110 0111 nnnn nnnn */
  INSN_PIC16E_BNN       = 0xE700,
    /* bnov  : 1110 0101 nnnn nnnn */
  INSN_PIC16E_BNOV      = 0xE500,
    /* bnz   : 1110 0001 nnnn nnnn */
  INSN_PIC16E_BNZ       = 0xE100,
    /* bov   : 1110 0100 nnnn nnnn */
  INSN_PIC16E_BOV       = 0xE400,
    /* bra   : 1101 0nnn nnnn nnnn */
  INSN_PIC16E_BRA       = 0xD000,
    /* bsf   : 1000 bbba ffff ffff */
  INSN_PIC16E_BSF       = 0x8000,
    /* btfsc : 1011 bbba ffff ffff */
  INSN_PIC16E_BTFSC     = 0xB000,
    /* btfss : 1010 bbba ffff ffff */
  INSN_PIC16E_BTFSS     = 0xA000,
    /* btg   : 0111 bbba ffff ffff */
  INSN_PIC16E_BTG       = 0x7000,
    /* bz    : 1110 0000 nnnn nnnn */
  INSN_PIC16E_BZ        = 0xE000,
    /* call  : 1110 110s kkkk kkkk
               1111 kkkk kkkk kkkk */
  INSN_PIC16E_CALL      = 0xEC00,
    /* clrf  : 0110 101a ffff ffff */
  INSN_PIC16E_CLRF      = 0x6A00,
    /* clrwdt: 0000 0000 0000 0100 */
  INSN_PIC16E_CLRWDT    = 0x0004,
    /* comf  : 0001 11da ffff ffff */
  INSN_PIC16E_COMF      = 0x1C00,
    /* cpfseq: 0110 001a ffff ffff */
  INSN_PIC16E_CPFSEQ    = 0x6200,
    /* cpfsgt: 0110 010a ffff ffff */
  INSN_PIC16E_CPFSGT    = 0x6400,
    /* cpfslt: 0110 000a ffff ffff */
  INSN_PIC16E_CPFSLT    = 0x6000,
    /* daw   : 0000 0000 0000 0111 */
  INSN_PIC16E_DAW       = 0x0007,
    /* decf  : 0000 01da ffff ffff */
  INSN_PIC16E_DECF      = 0x0400,
    /* decfsz: 0010 11da ffff ffff */
  INSN_PIC16E_DECFSZ    = 0x2C00,
    /* dcfsnz: 0100 11da ffff ffff */
  INSN_PIC16E_DCFSNZ    = 0x4C00,
    /* goto  : 1110 1111 kkkk kkkk
               1111 kkkk kkkk kkkk */
  INSN_PIC16E_GOTO      = 0xEF00,
    /* halt  : 0000 0000 0000 0001 */
  INSN_PIC16E_HALT      = 0x0001,
    /* incf  : 0010 10da ffff ffff */
  INSN_PIC16E_INCF      = 0x2800,
    /* incfsz: 0011 11da ffff ffff */
  INSN_PIC16E_INCFSZ    = 0x3C00,
    /* infsnz: 0100 10da ffff ffff */
  INSN_PIC16E_INFSNZ    = 0x4800,
    /* iorlw : 0000 1001 kkkk kkkk */
  INSN_PIC16E_IORLW     = 0x0900,
    /* iorwf : 0001 00da ffff ffff */
  INSN_PIC16E_IORWF     = 0x1000,
    /* lfsr  : 1110 1110 00ff kkkk
               1111 0000 kkkk kkkk */
  INSN_PIC16E_LFSR      = 0xEE00,
    /* movf  : 0101 00da ffff ffff */
  INSN_PIC16E_MOVF      = 0x5000,
    /* movff : 1100 ffff ffff ffff
               1111 ffff ffff ffff */
  INSN_PIC16E_MOVFF     = 0xC000,
    /* movlb : 0000 0001 0000 kkkk */
  INSN_PIC16E_MOVLB     = 0x0100,
    /* movlw : 0000 1110 kkkk kkkk */
  INSN_PIC16E_MOVLW     = 0x0E00,
    /* movwf : 0110 111a ffff ffff */
  INSN_PIC16E_MOVWF     = 0x6E00,
    /* mullw : 0000 1101 kkkk kkkk */
  INSN_PIC16E_MULLW     = 0x0D00,
    /* mulwf : 0000 001a ffff ffff */
  INSN_PIC16E_MULWF     = 0x0200,
    /* negf  : 0110 110a ffff ffff */
  INSN_PIC16E_NEGF      = 0x6C00,
    /* nop   : 0000 0000 0000 0000 */
    /* nop   : 1111 xxxx xxxx xxxx */
  INSN_PIC16E_NOP       = 0x0000,
    /* pop   : 0000 0000 0000 0110 */
  INSN_PIC16E_POP       = 0x0006,
    /* push  : 0000 0000 0000 0101 */
  INSN_PIC16E_PUSH      = 0x0005,
    /* rcall : 1101 1nnn nnnn nnnn */
  INSN_PIC16E_RCALL     = 0xD800,
    /* reset : 0000 0000 1111 1111 */
  INSN_PIC16E_RESET     = 0x00FF,
    /* retfie: 0000 0000 0001 000s */
  INSN_PIC16E_RETFIE    = 0x0010,
    /* retlw : 0000 1100 kkkk kkkk */
  INSN_PIC16E_RETLW     = 0x0C00,
    /* return: 0000 0000 0001 001s */
  INSN_PIC16E_RETURN    = 0x0012,
    /* rlcf  : 0011 01da ffff ffff */
  INSN_PIC16E_RLCF      = 0x3400,
    /* rlncf : 0100 01da ffff ffff */
  INSN_PIC16E_RLNCF     = 0x4400,
    /* rrcf  : 0011 00da ffff ffff */
  INSN_PIC16E_RRCF      = 0x3000,
    /* rrncf : 0100 00da ffff ffff */
  INSN_PIC16E_RRNCF     = 0x4000,
    /* setf  : 0110 100a ffff ffff */
  INSN_PIC16E_SETF      = 0x6800,
    /* sleep : 0000 0000 0000 0011 */
  INSN_PIC16E_SLEEP     = 0x0003,
    /* subfwb: 0101 01da ffff ffff */
  INSN_PIC16E_SUBFWB    = 0x5400,
    /* sublw : 0000 1000 kkkk kkkk */
  INSN_PIC16E_SUBLW     = 0x0800,
    /* subwf : 0101 11da ffff ffff */
  INSN_PIC16E_SUBWF     = 0x5C00,
    /* subwfb: 0101 10da ffff ffff */
  INSN_PIC16E_SUBWFB    = 0x5800,
    /* swapf : 0011 10da ffff ffff */
  INSN_PIC16E_SWAPF     = 0x3800,
    /* tblrd : 0000 0000 0000 1000 */
  INSN_PIC16E_TBLRD     = 0x0008,
    /* tblwt : 0000 0000 0000 1100 */
  INSN_PIC16E_TBLWT     = 0x000C,
    /* trap  : 0000 0000 1110 0000 */
  INSN_PIC16E_TRAP      = 0x00E0,
    /* tret  : 0000 0000 1110 0001 */
  INSN_PIC16E_TRET      = 0x00E1,
    /* tstfsz: 0110 011a ffff ffff */
  INSN_PIC16E_TSTFSZ    = 0x6600,
    /* xorlw : 0000 1010 kkkk kkkk */
  INSN_PIC16E_XORLW     = 0x0A00,
    /* xorwf : 0001 10da ffff ffff */
  INSN_PIC16E_XORWF     = 0x1800
};


enum insn_pic16es {
  INSN_PIC16ES_CLRC     = 0x90D8,
  INSN_PIC16ES_CLRDC    = 0x92D8,
  INSN_PIC16ES_CLRN     = 0x98D8,
  INSN_PIC16ES_CLROV    = 0x96D8,
  INSN_PIC16ES_CLRW     = 0x6AE8,
  INSN_PIC16ES_CLRZ     = 0x94D8,
  INSN_PIC16ES_SETC     = 0x80D8,
  INSN_PIC16ES_SETDC    = 0x82D8,
  INSN_PIC16ES_SETN     = 0x88D8,
  INSN_PIC16ES_SETOV    = 0x86D8,
  INSN_PIC16ES_SETZ     = 0x84D8,
  INSN_PIC16ES_SKPC     = 0xA0D8,
  INSN_PIC16ES_SKPDC    = 0xA2D8,
  INSN_PIC16ES_SKPN     = 0xA8D8,
  INSN_PIC16ES_SKPOV    = 0xA6D8,
  INSN_PIC16ES_SKPZ     = 0xA4D8,
  INSN_PIC16ES_SKPNC    = 0xB0D8,
  INSN_PIC16ES_SKPNDC   = 0xB2D8,
  INSN_PIC16ES_SKPNN    = 0xB8D8,
  INSN_PIC16ES_SKPNOV   = 0xB6D8,
  INSN_PIC16ES_SKPNZ    = 0xB4D8,
  INSN_PIC16ES_TGC      = 0x70D8,
  INSN_PIC16ES_TGDC     = 0x72D8,
  INSN_PIC16ES_TGN      = 0x78D8,
  INSN_PIC16ES_TGOV     = 0x76D8,
  INSN_PIC16ES_TGZ      = 0x74D8
};

enum insn_pic16ex {
    /* addfsr: 1110 1000 ffkk kkkk */
  INSN_PIC16EX_ADDFSR   = 0xE800,
    /* addulnk: 1110 1000 11kk kkkk */
  INSN_PIC16EX_ADDULNK  = 0xE8C0,
    /* callw : 0000 0000 0001 0100 */
  INSN_PIC16EX_CALLW    = 0x0014,
    /* movsf : 1110 1011 0zzz zzzz
               1111 ffff ffff ffff */
  INSN_PIC16EX_MOVSF    = 0xEB00,
    /* movss : 1110 1011 1zzz zzzz
               1111 xxxx xzzz zzzz */
  INSN_PIC16EX_MOVSS    = 0xEB80,
    /* pushl : 1110 1010 kkkk kkkk */
  INSN_PIC16EX_PUSHL    = 0xEA00,
    /* subfsr: 1110 1001 ffkk kkkk */
  INSN_PIC16EX_SUBFSR   = 0xE900,
    /* subulnk: 1110 1001 11kk kkkk */
  INSN_PIC16EX_SUBULNK  = 0xE9C0
};

typedef int gpasmVal;   /* The type that internal arithmetic uses */

struct pnode;           /* forward declaration; defined in
                         * gpasm.h for assembler and in
                         * script.h for linker */

struct insn {
  const char *name;
  long int mask;
  long int opcode;
  enum insn_class class;
  int attribs;
  gpasmVal (*doer)(gpasmVal r, char *name, int arity, struct pnode *parms);
};

struct strict_insn {
  const char *name;
  long int mask;
};

#define TABLE_SIZE(X)       (sizeof(X) / sizeof(X[0]))

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

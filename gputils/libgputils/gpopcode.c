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

#include "stdhdr.h"
#include "libgputils.h"

/* PIC 12-bit instruction set */
const struct insn op_12c5xx[] = {
  { "addwf",   0xfc0,  INSN_PIC12_ADDWF,     INSN_CLASS_OPWF5     },
  { "andlw",   0xf00,  INSN_PIC12_ANDLW,     INSN_CLASS_LIT8      },
  { "andwf",   0xfc0,  INSN_PIC12_ANDWF,     INSN_CLASS_OPWF5     },
  { "bcf",     0xf00,  INSN_PIC12_BCF,       INSN_CLASS_B5        },
  { "bsf",     0xf00,  INSN_PIC12_BSF,       INSN_CLASS_B5        },
  { "btfsc",   0xf00,  INSN_PIC12_BTFSC,     INSN_CLASS_B5        },
  { "btfss",   0xf00,  INSN_PIC12_BTFSS,     INSN_CLASS_B5        },
  { "call",    0xf00,  INSN_PIC12_CALL,      INSN_CLASS_LIT8C12   },
  { "clrf",    0xfe0,  INSN_PIC12_CLRF,      INSN_CLASS_OPF5      },
  { "clrw",    0xfff,  INSN_PIC12_CLRW,      INSN_CLASS_IMPLICIT  },
  { "clrwdt",  0xfff,  INSN_PIC12_CLRWDT,    INSN_CLASS_IMPLICIT  },
  { "comf",    0xfc0,  INSN_PIC12_COMF,      INSN_CLASS_OPWF5     },
  { "decf",    0xfc0,  INSN_PIC12_DECF,      INSN_CLASS_OPWF5     },
  { "decfsz",  0xfc0,  INSN_PIC12_DECFSZ,    INSN_CLASS_OPWF5     },
  { "goto",    0xe00,  INSN_PIC12_GOTO,      INSN_CLASS_LIT9      },
  { "incf",    0xfc0,  INSN_PIC12_INCF,      INSN_CLASS_OPWF5     },
  { "incfsz",  0xfc0,  INSN_PIC12_INCFSZ,    INSN_CLASS_OPWF5     },
  { "iorlw",   0xf00,  INSN_PIC12_IORLW,     INSN_CLASS_LIT8      },
  { "iorwf",   0xfc0,  INSN_PIC12_IORWF,     INSN_CLASS_OPWF5     },
  { "movf",    0xfc0,  INSN_PIC12_MOVF,      INSN_CLASS_OPWF5     },
  { "movlw",   0xf00,  INSN_PIC12_MOVLW,     INSN_CLASS_LIT8      },
  { "movwf",   0xfe0,  INSN_PIC12_MOVWF,     INSN_CLASS_OPF5      },
  { "nop",     0xfff,  INSN_PIC12_NOP,       INSN_CLASS_IMPLICIT  },
  { "option",  0xfff,  INSN_PIC12_OPTION,    INSN_CLASS_IMPLICIT  },
  { "retlw",   0xf00,  INSN_PIC12_RETLW,     INSN_CLASS_LIT8      },
  { "return",  0xfff,  INSN_PIC12_RETURN,    INSN_CLASS_IMPLICIT  }, /* FIXME: special mnemonic */
  { "rlf",     0xfc0,  INSN_PIC12_RLF,       INSN_CLASS_OPWF5     },
  { "rrf",     0xfc0,  INSN_PIC12_RRF,       INSN_CLASS_OPWF5     },
  { "sleep",   0xfff,  INSN_PIC12_SLEEP,     INSN_CLASS_IMPLICIT  },
  { "subwf",   0xfc0,  INSN_PIC12_SUBWF,     INSN_CLASS_OPWF5     },
  { "swapf",   0xfc0,  INSN_PIC12_SWAPF,     INSN_CLASS_OPWF5     },
  { "tris",    0xff8,  INSN_PIC12_TRIS,      INSN_CLASS_OPF3      },
  { "xorlw",   0xf00,  INSN_PIC12_XORLW,     INSN_CLASS_LIT8      },
  { "xorwf",   0xfc0,  INSN_PIC12_XORWF,     INSN_CLASS_OPWF5     }
};

const int num_op_12c5xx = TABLE_SIZE(op_12c5xx);

/* PIC 12-bit Enhanced instruction set */
const struct insn op_16c5xx_enh[] = {
  { "movlb",   0xff8,  INSN_PIC12E_MOVLB,    INSN_CLASS_LIT3      },
  { "retfie",  0xfff,  INSN_PIC12E_RETFIE,   INSN_CLASS_IMPLICIT  },
  { "return",  0xfff,  INSN_PIC12E_RETURN,   INSN_CLASS_IMPLICIT  }
};

const int num_op_16c5xx_enh = TABLE_SIZE(op_16c5xx_enh);

/* Scenix SX has a superset of the PIC 12-bit instruction set. */
/*
 * It would be nice if there was a more elegant way to do this,
 * either by adding a flags field to struct insn, or by allowing a
 * processor to have more than one associated table.
 */
const struct insn op_sx[] = {
  { "addwf",   0xfc0,  INSN_PIC12_ADDWF,     INSN_CLASS_OPWF5     },
  { "andlw",   0xf00,  INSN_PIC12_ANDLW,     INSN_CLASS_LIT8      },
  { "andwf",   0xfc0,  INSN_PIC12_ANDWF,     INSN_CLASS_OPWF5     },
  { "bank",    0xff8,  INSN_SX_BANK,         INSN_CLASS_LIT3_BANK }, /* SX only */
  { "bcf",     0xf00,  INSN_PIC12_BCF,       INSN_CLASS_B5        },
  { "bsf",     0xf00,  INSN_PIC12_BSF,       INSN_CLASS_B5        },
  { "btfsc",   0xf00,  INSN_PIC12_BTFSC,     INSN_CLASS_B5        },
  { "btfss",   0xf00,  INSN_PIC12_BTFSS,     INSN_CLASS_B5        },
  { "call",    0xf00,  INSN_PIC12_CALL,      INSN_CLASS_LIT8C12   },
  { "clrf",    0xfe0,  INSN_PIC12_CLRF,      INSN_CLASS_OPF5      },
  { "clrw",    0xfff,  INSN_PIC12_CLRW,      INSN_CLASS_IMPLICIT  },
  { "clrwdt",  0xfff,  INSN_PIC12_CLRWDT,    INSN_CLASS_IMPLICIT  },
  { "comf",    0xfc0,  INSN_PIC12_COMF,      INSN_CLASS_OPWF5     },
  { "decf",    0xfc0,  INSN_PIC12_DECF,      INSN_CLASS_OPWF5     },
  { "decfsz",  0xfc0,  INSN_PIC12_DECFSZ,    INSN_CLASS_OPWF5     },
  { "goto",    0xe00,  INSN_PIC12_GOTO,      INSN_CLASS_LIT9      },
  { "incf",    0xfc0,  INSN_PIC12_INCF,      INSN_CLASS_OPWF5     },
  { "incfsz",  0xfc0,  INSN_PIC12_INCFSZ,    INSN_CLASS_OPWF5     },
  { "iorlw",   0xf00,  INSN_PIC12_IORLW,     INSN_CLASS_LIT8      },
  { "iorwf",   0xfc0,  INSN_PIC12_IORWF,     INSN_CLASS_OPWF5     },
  { "iread",   0xfff,  INSN_SX_IREAD,        INSN_CLASS_IMPLICIT  }, /* SX only */
  { "mode",    0xff0,  INSN_SX_MODE,         INSN_CLASS_LIT4      }, /* SX only */
  { "movf",    0xfc0,  INSN_PIC12_MOVF,      INSN_CLASS_OPWF5     },
  { "movlw",   0xf00,  INSN_PIC12_MOVLW,     INSN_CLASS_LIT8      },
  { "movmw",   0xfff,  INSN_SX_MOVMW,        INSN_CLASS_IMPLICIT  }, /* SX only */
  { "movwf",   0xfe0,  INSN_PIC12_MOVWF,     INSN_CLASS_OPF5      },
  { "movwm",   0xfff,  INSN_SX_MOVWM,        INSN_CLASS_IMPLICIT  }, /* SX only */
  { "nop",     0xfff,  INSN_PIC12_NOP,       INSN_CLASS_IMPLICIT  },
  { "option",  0xfff,  INSN_PIC12_OPTION,    INSN_CLASS_IMPLICIT  },
  { "page",    0xff8,  INSN_SX_PAGE,         INSN_CLASS_LIT3_PAGE }, /* SX only */
  { "reti",    0xfff,  INSN_SX_RETI,         INSN_CLASS_IMPLICIT  }, /* SX only */
  { "retiw",   0xfff,  INSN_SX_RETIW,        INSN_CLASS_IMPLICIT  }, /* SX only */
  { "retlw",   0xf00,  INSN_PIC12_RETLW,     INSN_CLASS_LIT8      },
  { "retp",    0xfff,  INSN_SX_RETP,         INSN_CLASS_IMPLICIT  }, /* SX only */
  { "return",  0xfff,  INSN_SX_RETURN,       INSN_CLASS_IMPLICIT  }, /* SX only */
  { "rlf",     0xfc0,  INSN_PIC12_RLF,       INSN_CLASS_OPWF5     },
  { "rrf",     0xfc0,  INSN_PIC12_RRF,       INSN_CLASS_OPWF5     },
  { "sleep",   0xfff,  INSN_PIC12_SLEEP,     INSN_CLASS_IMPLICIT  },
  { "subwf",   0xfc0,  INSN_PIC12_SUBWF,     INSN_CLASS_OPWF5     },
  { "swapf",   0xfc0,  INSN_PIC12_SWAPF,     INSN_CLASS_OPWF5     },
  { "tris",    0xff8,  INSN_PIC12_TRIS,      INSN_CLASS_OPF5      }, /* FIXME: This should probably be INSN_CLASS_OPF3 as for 12c5xx? */
  { "xorlw",   0xf00,  INSN_PIC12_XORLW,     INSN_CLASS_LIT8      },
  { "xorwf",   0xfc0,  INSN_PIC12_XORWF,     INSN_CLASS_OPWF5     }
};

const int num_op_sx = TABLE_SIZE(op_sx);

/* PIC 14-bit instruction set */
struct insn op_16cxx[] = {
  { "addlw",   0x3e00, INSN_PIC14_ADDLW,     INSN_CLASS_LIT8      },
  { "addwf",   0x3f00, INSN_PIC14_ADDWF,     INSN_CLASS_OPWF7     },
  { "andlw",   0x3f00, INSN_PIC14_ANDLW,     INSN_CLASS_LIT8      },
  { "andwf",   0x3f00, INSN_PIC14_ANDWF,     INSN_CLASS_OPWF7     },
  { "bcf",     0x3c00, INSN_PIC14_BCF,       INSN_CLASS_B7        },
  { "bsf",     0x3c00, INSN_PIC14_BSF,       INSN_CLASS_B7        },
  { "btfsc",   0x3c00, INSN_PIC14_BTFSC,     INSN_CLASS_B7        },
  { "btfss",   0x3c00, INSN_PIC14_BTFSS,     INSN_CLASS_B7        },
  { "call",    0x3800, INSN_PIC14_CALL,      INSN_CLASS_LIT11     },
  { "clrf",    0x3f80, INSN_PIC14_CLRF,      INSN_CLASS_OPF7      },
  { "clrw",    0x3fff, INSN_PIC14_CLRW,      INSN_CLASS_IMPLICIT  },
  { "clrwdt",  0x3fff, INSN_PIC14_CLRWDT,    INSN_CLASS_IMPLICIT  },
  { "comf",    0x3f00, INSN_PIC14_COMF,      INSN_CLASS_OPWF7     },
  { "decf",    0x3f00, INSN_PIC14_DECF,      INSN_CLASS_OPWF7     },
  { "decfsz",  0x3f00, INSN_PIC14_DECFSZ,    INSN_CLASS_OPWF7     },
  { "goto",    0x3800, INSN_PIC14_GOTO,      INSN_CLASS_LIT11     },
  { "halt",    0x3fff, INSN_PIC14_HALT,      INSN_CLASS_IMPLICIT  },  /* undocumented */
  { "incf",    0x3f00, INSN_PIC14_INCF,      INSN_CLASS_OPWF7     },
  { "incfsz",  0x3f00, INSN_PIC14_INCFSZ,    INSN_CLASS_OPWF7     },
  { "iorlw",   0x3f00, INSN_PIC14_IORLW,     INSN_CLASS_LIT8      },
  { "iorwf",   0x3f00, INSN_PIC14_IORWF,     INSN_CLASS_OPWF7     },
  { "movf",    0x3f00, INSN_PIC14_MOVF,      INSN_CLASS_OPWF7     },
  { "movlw",   0x3c00, INSN_PIC14_MOVLW,     INSN_CLASS_LIT8      },
  { "movwf",   0x3f80, INSN_PIC14_MOVWF,     INSN_CLASS_OPF7      },
  { "nop",     0x3f9f, INSN_PIC14_NOP,       INSN_CLASS_IMPLICIT  },
  { "option",  0x3fff, INSN_PIC14_OPTION,    INSN_CLASS_IMPLICIT  },
  { "retfie",  0x3fff, INSN_PIC14_RETFIE,    INSN_CLASS_IMPLICIT  },
  { "retlw",   0x3c00, INSN_PIC14_RETLW,     INSN_CLASS_LIT8      },
  { "return",  0x3fff, INSN_PIC14_RETURN,    INSN_CLASS_IMPLICIT  },
  { "rlf",     0x3f00, INSN_PIC14_RLF,       INSN_CLASS_OPWF7     },
  { "rrf",     0x3f00, INSN_PIC14_RRF,       INSN_CLASS_OPWF7     },
  { "sleep",   0x3fff, INSN_PIC14_SLEEP,     INSN_CLASS_IMPLICIT  },
  { "sublw",   0x3e00, INSN_PIC14_SUBLW,     INSN_CLASS_LIT8      },
  { "subwf",   0x3f00, INSN_PIC14_SUBWF,     INSN_CLASS_OPWF7     },
  { "swapf",   0x3f00, INSN_PIC14_SWAPF,     INSN_CLASS_OPWF7     },
  { "tris",    0x3ff8, INSN_PIC14_TRIS,      INSN_CLASS_OPF7      },
  { "xorlw",   0x3f00, INSN_PIC14_XORLW,     INSN_CLASS_LIT8      },
  { "xorwf",   0x3f00, INSN_PIC14_XORWF,     INSN_CLASS_OPWF7     }
};

const int num_op_16cxx = TABLE_SIZE(op_16cxx);

const struct strict_insn op_16cxx_strict_mask[] = {
  { "addlw",   0x3f00 },
  { "clrw",    0x3fff },
  { "movlw",   0x3f00 },
  { "nop",     0x3fff },
  { "retlw",   0x3f00 },
  { "sublw",   0x3f00 }
};

const int num_op_16cxx_strict_mask = TABLE_SIZE(op_16cxx_strict_mask);


/* PIC 14-bit Enhanced instruction set */
const struct insn op_16cxx_enh[] = {
  { "addfsr",  0x3f80, INSN_PIC14E_ADDFSR,   INSN_CLASS_LITFSR_14 },
  { "addwfc",  0x3f00, INSN_PIC14E_ADDWFC,   INSN_CLASS_OPWF7     },
  { "asrf",    0x3f00, INSN_PIC14E_ASRF,     INSN_CLASS_OPWF7     },
  { "bra",     0x3e00, INSN_PIC14E_BRA,      INSN_CLASS_RBRA9     },
  { "brw",     0x3fff, INSN_PIC14E_BRW,      INSN_CLASS_IMPLICIT  },
  { "callw",   0x3fff, INSN_PIC14E_CALLW,    INSN_CLASS_IMPLICIT  },
  { "lslf",    0x3f00, INSN_PIC14E_LSLF,     INSN_CLASS_OPWF7     },
  { "lsrf",    0x3f00, INSN_PIC14E_LSRF,     INSN_CLASS_OPWF7     },
  { "moviw",   0x3ff8, INSN_PIC14E_MOVIW,    INSN_CLASS_MOVINDF   },
  { "movwi",   0x3ff8, INSN_PIC14E_MOVWI,    INSN_CLASS_MOVINDF   },
  { "movlb",   0x3fe0, INSN_PIC14E_MOVLB,    INSN_CLASS_LIT5      },
  { "movlp",   0x3f80, INSN_PIC14E_MOVLP,    INSN_CLASS_LIT7      },
  { "reset",   0x3fff, INSN_PIC14E_RESET,    INSN_CLASS_IMPLICIT  },
  { "subwfb",  0x3f00, INSN_PIC14E_SUBWFB,   INSN_CLASS_OPWF7     }
};

const int num_op_16cxx_enh = TABLE_SIZE(op_16cxx_enh);

/* PIC 16-bit instruction set */
const struct insn op_17cxx[] = {
  { "addlw",   0xff00, INSN_PIC16_ADDLW,     INSN_CLASS_LIT8      },
  { "addwf",   0xfe00, INSN_PIC16_ADDWF,     INSN_CLASS_OPWF8     },
  { "addwfc",  0xfe00, INSN_PIC16_ADDWFC,    INSN_CLASS_OPWF8     },
  { "andlw",   0xff00, INSN_PIC16_ANDLW,     INSN_CLASS_LIT8      },
  { "andwf",   0xfe00, INSN_PIC16_ANDWF,     INSN_CLASS_OPWF8     },
  { "bcf",     0xf800, INSN_PIC16_BCF,       INSN_CLASS_B8        },
  { "bsf",     0xf800, INSN_PIC16_BSF,       INSN_CLASS_B8        },
  { "btfsc",   0xf800, INSN_PIC16_BTFSC,     INSN_CLASS_B8        },
  { "btfss",   0xf800, INSN_PIC16_BTFSS,     INSN_CLASS_B8        },
  { "btg",     0xf800, INSN_PIC16_BTG,       INSN_CLASS_B8        },
  { "call",    0xe000, INSN_PIC16_CALL,      INSN_CLASS_LIT13     },
  { "clrf",    0xfe00, INSN_PIC16_CLRF,      INSN_CLASS_OPWF8     },
  { "clrwdt",  0xffff, INSN_PIC16_CLRWDT,    INSN_CLASS_IMPLICIT  },
  { "comf",    0xfe00, INSN_PIC16_COMF,      INSN_CLASS_OPWF8     },
  { "cpfseq",  0xff00, INSN_PIC16_CPFSEQ,    INSN_CLASS_OPF8      },
  { "cpfsgt",  0xff00, INSN_PIC16_CPFSGT,    INSN_CLASS_OPF8      },
  { "cpfslt",  0xff00, INSN_PIC16_CPFSLT,    INSN_CLASS_OPF8      },
  { "daw",     0xfe00, INSN_PIC16_DAW,       INSN_CLASS_OPWF8     },
  { "decf",    0xfe00, INSN_PIC16_DECF,      INSN_CLASS_OPWF8     },
  { "decfsz",  0xfe00, INSN_PIC16_DECFSZ,    INSN_CLASS_OPWF8     },
  { "dcfsnz",  0xfe00, INSN_PIC16_DCFSNZ,    INSN_CLASS_OPWF8     },
  { "goto",    0xe000, INSN_PIC16_GOTO,      INSN_CLASS_LIT13     },
  { "incf",    0xfe00, INSN_PIC16_INCF,      INSN_CLASS_OPWF8     },
  { "incfsz",  0xfe00, INSN_PIC16_INCFSZ,    INSN_CLASS_OPWF8     },
  { "infsnz",  0xfe00, INSN_PIC16_INFSNZ,    INSN_CLASS_OPWF8     },
  { "iorlw",   0xff00, INSN_PIC16_IORLW,     INSN_CLASS_LIT8      },
  { "iorwf",   0xfe00, INSN_PIC16_IORWF,     INSN_CLASS_OPWF8     },
  { "lcall",   0xff00, INSN_PIC16_LCALL,     INSN_CLASS_LIT8C16   },
  { "movfp",   0xe000, INSN_PIC16_MOVFP,     INSN_CLASS_FP        },
  { "movpf",   0xe000, INSN_PIC16_MOVPF,     INSN_CLASS_PF        },
  { "movlb",   0xff00, INSN_PIC16_MOVLB,     INSN_CLASS_LIT8      },
  { "movlr",   0xfe00, INSN_PIC16_MOVLR,     INSN_CLASS_LIT4S     },
  { "movlw",   0xff00, INSN_PIC16_MOVLW,     INSN_CLASS_LIT8      },
  { "movwf",   0xff00, INSN_PIC16_MOVWF,     INSN_CLASS_OPF8      },
  { "mullw",   0xff00, INSN_PIC16_MULLW,     INSN_CLASS_LIT8      },
  { "mulwf",   0xff00, INSN_PIC16_MULWF,     INSN_CLASS_OPF8      },
  { "negw",    0xfe00, INSN_PIC16_NEGW,      INSN_CLASS_OPWF8     },
  { "nop",     0xffff, INSN_PIC16_NOP,       INSN_CLASS_IMPLICIT  },
  { "retfie",  0xffff, INSN_PIC16_RETFIE,    INSN_CLASS_IMPLICIT  },
  { "retlw",   0xff00, INSN_PIC16_RETLW,     INSN_CLASS_LIT8      },
  { "return",  0xffff, INSN_PIC16_RETURN,    INSN_CLASS_IMPLICIT  },
  { "rlcf",    0xfe00, INSN_PIC16_RLCF,      INSN_CLASS_OPWF8     },
  { "rlncf",   0xfe00, INSN_PIC16_RLNCF,     INSN_CLASS_OPWF8     },
  { "rrcf",    0xfe00, INSN_PIC16_RRCF,      INSN_CLASS_OPWF8     },
  { "rrncf",   0xfe00, INSN_PIC16_RRNCF,     INSN_CLASS_OPWF8     },
  { "setf",    0xfe00, INSN_PIC16_SETF,      INSN_CLASS_OPWF8     },
  { "sleep",   0xffff, INSN_PIC16_SLEEP,     INSN_CLASS_IMPLICIT  },
  { "sublw",   0xff00, INSN_PIC16_SUBLW,     INSN_CLASS_LIT8      },
  { "subwf",   0xfe00, INSN_PIC16_SUBWF,     INSN_CLASS_OPWF8     },
  { "subwfb",  0xfe00, INSN_PIC16_SUBWFB,    INSN_CLASS_OPWF8     },
  { "swapf",   0xfe00, INSN_PIC16_SWAPF,     INSN_CLASS_OPWF8     },
  { "tablrd",  0xfc00, INSN_PIC16_TABLRD,    INSN_CLASS_TBL3      },
  { "tablwt",  0xfc00, INSN_PIC16_TABLWT,    INSN_CLASS_TBL3      },
  { "tlrd",    0xfc00, INSN_PIC16_TLRD,      INSN_CLASS_TBL2      },
  { "tlwt",    0xfc00, INSN_PIC16_TLWT,      INSN_CLASS_TBL2      },
  { "tstfsz",  0xff00, INSN_PIC16_TSTFSZ,    INSN_CLASS_OPF8      },
  { "xorlw",   0xff00, INSN_PIC16_XORLW,     INSN_CLASS_LIT8      },
  { "xorwf",   0xfe00, INSN_PIC16_XORWF,     INSN_CLASS_OPWF8     }
};

const int num_op_17cxx = TABLE_SIZE(op_17cxx);

const struct insn op_18cxx[] = {
  { "addlw",   0xff00, INSN_PIC16E_ADDLW,    INSN_CLASS_LIT8      },
  { "addwf",   0xfc00, INSN_PIC16E_ADDWF,    INSN_CLASS_OPWFA8    },
  { "addwfc",  0xfc00, INSN_PIC16E_ADDWFC,   INSN_CLASS_OPWFA8    },
  { "andlw",   0xff00, INSN_PIC16E_ANDLW,    INSN_CLASS_LIT8      },
  { "andwf",   0xfc00, INSN_PIC16E_ANDWF,    INSN_CLASS_OPWFA8    },
  { "bc",      0xff00, INSN_PIC16E_BC,       INSN_CLASS_RBRA8     },
  { "bcf",     0xf000, INSN_PIC16E_BCF,      INSN_CLASS_BA8       },
  { "bn",      0xff00, INSN_PIC16E_BN,       INSN_CLASS_RBRA8     },
  { "bnc",     0xff00, INSN_PIC16E_BNC,      INSN_CLASS_RBRA8     },
  { "bnn",     0xff00, INSN_PIC16E_BNN,      INSN_CLASS_RBRA8     },
  { "bnov",    0xff00, INSN_PIC16E_BNOV,     INSN_CLASS_RBRA8     },
  { "bnz",     0xff00, INSN_PIC16E_BNZ,      INSN_CLASS_RBRA8     },
  { "bov",     0xff00, INSN_PIC16E_BOV,      INSN_CLASS_RBRA8     },
  { "bra",     0xf800, INSN_PIC16E_BRA,      INSN_CLASS_RBRA11    },
  { "bsf",     0xf000, INSN_PIC16E_BSF,      INSN_CLASS_BA8       },
  { "btfsc",   0xf000, INSN_PIC16E_BTFSC,    INSN_CLASS_BA8       },
  { "btfss",   0xf000, INSN_PIC16E_BTFSS,    INSN_CLASS_BA8       },
  { "btg",     0xf000, INSN_PIC16E_BTG,      INSN_CLASS_BA8       },
  { "bz",      0xff00, INSN_PIC16E_BZ,       INSN_CLASS_RBRA8     },
  { "call",    0xfe00, INSN_PIC16E_CALL,     INSN_CLASS_CALL20    },
  { "clrf",    0xfe00, INSN_PIC16E_CLRF,     INSN_CLASS_OPFA8     },
  { "clrwdt",  0xffff, INSN_PIC16E_CLRWDT,   INSN_CLASS_IMPLICIT  },
  { "comf",    0xfc00, INSN_PIC16E_COMF,     INSN_CLASS_OPWFA8    },
  { "cpfseq",  0xfe00, INSN_PIC16E_CPFSEQ,   INSN_CLASS_OPFA8     },
  { "cpfsgt",  0xfe00, INSN_PIC16E_CPFSGT,   INSN_CLASS_OPFA8     },
  { "cpfslt",  0xfe00, INSN_PIC16E_CPFSLT,   INSN_CLASS_OPFA8     },
  { "daw",     0xffff, INSN_PIC16E_DAW,      INSN_CLASS_IMPLICIT  },
  { "decf",    0xfc00, INSN_PIC16E_DECF,     INSN_CLASS_OPWFA8    },
  { "decfsz",  0xfc00, INSN_PIC16E_DECFSZ,   INSN_CLASS_OPWFA8    },
  { "dcfsnz",  0xfc00, INSN_PIC16E_DCFSNZ,   INSN_CLASS_OPWFA8    },
  { "goto",    0xff00, INSN_PIC16E_GOTO,     INSN_CLASS_LIT20     },
  { "halt",    0xffff, INSN_PIC16E_HALT,     INSN_CLASS_IMPLICIT  }, /* undocumented */
  { "incf",    0xfc00, INSN_PIC16E_INCF,     INSN_CLASS_OPWFA8    },
  { "incfsz",  0xfc00, INSN_PIC16E_INCFSZ,   INSN_CLASS_OPWFA8    },
  { "infsnz",  0xfc00, INSN_PIC16E_INFSNZ,   INSN_CLASS_OPWFA8    },
  { "iorlw",   0xff00, INSN_PIC16E_IORLW,    INSN_CLASS_LIT8      },
  { "iorwf",   0xfc00, INSN_PIC16E_IORWF,    INSN_CLASS_OPWFA8    },
  { "lfsr",    0xffc0, INSN_PIC16E_LFSR,     INSN_CLASS_FLIT12    },
  { "movf",    0xfc00, INSN_PIC16E_MOVF,     INSN_CLASS_OPWFA8    },
  { "movff",   0xf000, INSN_PIC16E_MOVFF,    INSN_CLASS_FF        },
  { "movlb",   0xff00, INSN_PIC16E_MOVLB,    INSN_CLASS_LIT8      },
  { "movlw",   0xff00, INSN_PIC16E_MOVLW,    INSN_CLASS_LIT8      },
  { "movwf",   0xfe00, INSN_PIC16E_MOVWF,    INSN_CLASS_OPFA8     },
  { "mullw",   0xff00, INSN_PIC16E_MULLW,    INSN_CLASS_LIT8      },
  { "mulwf",   0xfe00, INSN_PIC16E_MULWF,    INSN_CLASS_OPFA8     },
  { "negf",    0xfe00, INSN_PIC16E_NEGF,     INSN_CLASS_OPFA8     },
  { "nop",     0xffff, INSN_PIC16E_NOP,      INSN_CLASS_IMPLICIT  },
  { "pop",     0xffff, INSN_PIC16E_POP,      INSN_CLASS_IMPLICIT  },
  { "push",    0xffff, INSN_PIC16E_PUSH,     INSN_CLASS_IMPLICIT  },
  { "rcall",   0xf800, INSN_PIC16E_RCALL,    INSN_CLASS_RBRA11    },
  { "reset",   0xffff, INSN_PIC16E_RESET,    INSN_CLASS_IMPLICIT  },
  { "retfie",  0xfffe, INSN_PIC16E_RETFIE,   INSN_CLASS_LIT1      },
  { "retlw",   0xff00, INSN_PIC16E_RETLW,    INSN_CLASS_LIT8      },
  { "return",  0xfffe, INSN_PIC16E_RETURN,   INSN_CLASS_LIT1      },
  { "rlcf",    0xfc00, INSN_PIC16E_RLCF,     INSN_CLASS_OPWFA8    },
  { "rlncf",   0xfc00, INSN_PIC16E_RLNCF,    INSN_CLASS_OPWFA8    },
  { "rrcf",    0xfc00, INSN_PIC16E_RRCF,     INSN_CLASS_OPWFA8    },
  { "rrncf",   0xfc00, INSN_PIC16E_RRNCF,    INSN_CLASS_OPWFA8    },
  { "setf",    0xfe00, INSN_PIC16E_SETF,     INSN_CLASS_OPFA8     },
  { "sleep",   0xffff, INSN_PIC16E_SLEEP,    INSN_CLASS_IMPLICIT  },
  { "subfwb",  0xfc00, INSN_PIC16E_SUBFWB,   INSN_CLASS_OPWFA8    },
  { "sublw",   0xff00, INSN_PIC16E_SUBLW,    INSN_CLASS_LIT8      },
  { "subwf",   0xfc00, INSN_PIC16E_SUBWF,    INSN_CLASS_OPWFA8    },
  { "subwfb",  0xfc00, INSN_PIC16E_SUBWFB,   INSN_CLASS_OPWFA8    },
  { "swapf",   0xfc00, INSN_PIC16E_SWAPF,    INSN_CLASS_OPWFA8    },
  { "tblrd",   0xfffc, INSN_PIC16E_TBLRD,    INSN_CLASS_TBL       },
  { "tblwt",   0xfffc, INSN_PIC16E_TBLWT,    INSN_CLASS_TBL       },
  { "trap",    0xffff, INSN_PIC16E_TRAP,     INSN_CLASS_IMPLICIT  }, /* undocumented */
  { "tret",    0xffff, INSN_PIC16E_TRET,     INSN_CLASS_IMPLICIT  }, /* undocumented */
  { "tstfsz",  0xfe00, INSN_PIC16E_TSTFSZ,   INSN_CLASS_OPFA8     },
  { "xorlw",   0xff00, INSN_PIC16E_XORLW,    INSN_CLASS_LIT8      },
  { "xorwf",   0xfc00, INSN_PIC16E_XORWF,    INSN_CLASS_OPWFA8    }
};

const int num_op_18cxx = TABLE_SIZE(op_18cxx);

/* PIC 16-bit "Special" instruction set */
const struct insn op_18cxx_sp[] = {
  { "clrc",    0xffff, INSN_PIC16ES_CLRC,    INSN_CLASS_IMPLICIT  },
  { "clrdc",   0xffff, INSN_PIC16ES_CLRDC,   INSN_CLASS_IMPLICIT  },
  { "clrn",    0xffff, INSN_PIC16ES_CLRN,    INSN_CLASS_IMPLICIT  },
  { "clrov",   0xffff, INSN_PIC16ES_CLROV,   INSN_CLASS_IMPLICIT  },
  { "clrw",    0xffff, INSN_PIC16ES_CLRW,    INSN_CLASS_IMPLICIT  },
  { "clrz",    0xffff, INSN_PIC16ES_CLRZ,    INSN_CLASS_IMPLICIT  },
  { "setc",    0xffff, INSN_PIC16ES_SETC,    INSN_CLASS_IMPLICIT  },
  { "setdc",   0xffff, INSN_PIC16ES_SETDC,   INSN_CLASS_IMPLICIT  },
  { "setn",    0xffff, INSN_PIC16ES_SETN,    INSN_CLASS_IMPLICIT  },
  { "setov",   0xffff, INSN_PIC16ES_SETOV,   INSN_CLASS_IMPLICIT  },
  { "setz",    0xffff, INSN_PIC16ES_SETZ,    INSN_CLASS_IMPLICIT  },
  { "skpc",    0xffff, INSN_PIC16ES_SKPC,    INSN_CLASS_IMPLICIT  },
  { "skpdc",   0xffff, INSN_PIC16ES_SKPDC,   INSN_CLASS_IMPLICIT  },
  { "skpn",    0xffff, INSN_PIC16ES_SKPN,    INSN_CLASS_IMPLICIT  },
  { "skpov",   0xffff, INSN_PIC16ES_SKPOV,   INSN_CLASS_IMPLICIT  },
  { "skpz",    0xffff, INSN_PIC16ES_SKPZ,    INSN_CLASS_IMPLICIT  },
  { "skpnc",   0xffff, INSN_PIC16ES_SKPNC,   INSN_CLASS_IMPLICIT  },
  { "skpndc",  0xffff, INSN_PIC16ES_SKPNDC,  INSN_CLASS_IMPLICIT  },
  { "skpnn",   0xffff, INSN_PIC16ES_SKPNN,   INSN_CLASS_IMPLICIT  },
  { "skpnov",  0xffff, INSN_PIC16ES_SKPNOV,  INSN_CLASS_IMPLICIT  },
  { "skpnz",   0xffff, INSN_PIC16ES_SKPNZ,   INSN_CLASS_IMPLICIT  },
  { "tgc",     0xffff, INSN_PIC16ES_TGC,     INSN_CLASS_IMPLICIT  },
  { "tgdc",    0xffff, INSN_PIC16ES_TGDC,    INSN_CLASS_IMPLICIT  },
  { "tgn",     0xffff, INSN_PIC16ES_TGN,     INSN_CLASS_IMPLICIT  },
  { "tgov",    0xffff, INSN_PIC16ES_TGOV,    INSN_CLASS_IMPLICIT  },
  { "tgz",     0xffff, INSN_PIC16ES_TGZ,     INSN_CLASS_IMPLICIT  }
};

const int num_op_18cxx_sp = TABLE_SIZE(op_18cxx_sp);

/* PIC 16-bit Extended instruction set */
const struct insn op_18cxx_ext[] = {
  { "addfsr",  0xff00, INSN_PIC16EX_ADDFSR,  INSN_CLASS_LITFSR_16 },
  { "addulnk", 0xffc0, INSN_PIC16EX_ADDULNK, INSN_CLASS_LIT6      },
  { "callw",   0xffff, INSN_PIC16EX_CALLW,   INSN_CLASS_IMPLICIT  },
  { "movsf",   0xff80, INSN_PIC16EX_MOVSF,   INSN_CLASS_SF        },
  { "movss",   0xff80, INSN_PIC16EX_MOVSS,   INSN_CLASS_SS        },
  { "pushl",   0xff00, INSN_PIC16EX_PUSHL,   INSN_CLASS_LIT8      },
  { "subfsr",  0xff00, INSN_PIC16EX_SUBFSR,  INSN_CLASS_LITFSR_16 },
  { "subulnk", 0xffc0, INSN_PIC16EX_SUBULNK, INSN_CLASS_LIT6      }
};

const int num_op_18cxx_ext = TABLE_SIZE(op_18cxx_ext);

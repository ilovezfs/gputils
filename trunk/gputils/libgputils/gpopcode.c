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
  { "addwf",   IMSK_PIC12_ADDWF,   INSN_PIC12_ADDWF,     ICODE_ADDWF,  INSN_CLASS_OPWF5     },
  { "andlw",   IMSK_PIC12_ANDLW,   INSN_PIC12_ANDLW,     ICODE_ANDLW,  INSN_CLASS_LIT8      },
  { "andwf",   IMSK_PIC12_ANDWF,   INSN_PIC12_ANDWF,     ICODE_ANDWF,  INSN_CLASS_OPWF5     },
  { "bcf",     IMSK_PIC12_BCF,     INSN_PIC12_BCF,       ICODE_BCF,    INSN_CLASS_B5        },
  { "bsf",     IMSK_PIC12_BSF,     INSN_PIC12_BSF,       ICODE_BSF,    INSN_CLASS_B5        },
  { "btfsc",   IMSK_PIC12_BTFSC,   INSN_PIC12_BTFSC,     ICODE_BTFSC,  INSN_CLASS_B5        },
  { "btfss",   IMSK_PIC12_BTFSS,   INSN_PIC12_BTFSS,     ICODE_BTFSS,  INSN_CLASS_B5        },
  { "call",    IMSK_PIC12_CALL,    INSN_PIC12_CALL,      ICODE_CALL,   INSN_CLASS_LIT8C12   },
  { "clrf",    IMSK_PIC12_CLRF,    INSN_PIC12_CLRF,      ICODE_CLRF,   INSN_CLASS_OPF5      },
  { "clrw",    IMSK_PIC12_CLRW,    INSN_PIC12_CLRW,      ICODE_CLRW,   INSN_CLASS_IMPLICIT  },
  { "clrwdt",  IMSK_PIC12_CLRWDT,  INSN_PIC12_CLRWDT,    ICODE_CLRWDT, INSN_CLASS_IMPLICIT  },
  { "comf",    IMSK_PIC12_COMF,    INSN_PIC12_COMF,      ICODE_COMF,   INSN_CLASS_OPWF5     },
  { "decf",    IMSK_PIC12_DECF,    INSN_PIC12_DECF,      ICODE_DECF,   INSN_CLASS_OPWF5     },
  { "decfsz",  IMSK_PIC12_DECFSZ,  INSN_PIC12_DECFSZ,    ICODE_DECFSZ, INSN_CLASS_OPWF5     },
  { "goto",    IMSK_PIC12_GOTO,    INSN_PIC12_GOTO,      ICODE_GOTO,   INSN_CLASS_LIT9      },
  { "incf",    IMSK_PIC12_INCF,    INSN_PIC12_INCF,      ICODE_INCF,   INSN_CLASS_OPWF5     },
  { "incfsz",  IMSK_PIC12_INCFSZ,  INSN_PIC12_INCFSZ,    ICODE_INCFSZ, INSN_CLASS_OPWF5     },
  { "iorlw",   IMSK_PIC12_IORLW,   INSN_PIC12_IORLW,     ICODE_IORLW,  INSN_CLASS_LIT8      },
  { "iorwf",   IMSK_PIC12_IORWF,   INSN_PIC12_IORWF,     ICODE_IORWF,  INSN_CLASS_OPWF5     },
  { "movf",    IMSK_PIC12_MOVF,    INSN_PIC12_MOVF,      ICODE_MOVF,   INSN_CLASS_OPWF5     },
  { "movlw",   IMSK_PIC12_MOVLW,   INSN_PIC12_MOVLW,     ICODE_MOVLW,  INSN_CLASS_LIT8      },
  { "movwf",   IMSK_PIC12_MOVWF,   INSN_PIC12_MOVWF,     ICODE_MOVWF,  INSN_CLASS_OPF5      },
  { "nop",     IMSK_PIC12_NOP,     INSN_PIC12_NOP,       ICODE_NOP,    INSN_CLASS_IMPLICIT  },
  { "option",  IMSK_PIC12_OPTION,  INSN_PIC12_OPTION,    ICODE_OPTION, INSN_CLASS_IMPLICIT  },
  { "retlw",   IMSK_PIC12_RETLW,   INSN_PIC12_RETLW,     ICODE_RETLW,  INSN_CLASS_LIT8      },
  { "return",  IMSK_PIC12_RETURN,  INSN_PIC12_RETURN,    ICODE_RETURN, INSN_CLASS_IMPLICIT  }, /* FIXME: special mnemonic */
  { "rlf",     IMSK_PIC12_RLF,     INSN_PIC12_RLF,       ICODE_RLF,    INSN_CLASS_OPWF5     },
  { "rrf",     IMSK_PIC12_RRF,     INSN_PIC12_RRF,       ICODE_RRF,    INSN_CLASS_OPWF5     },
  { "sleep",   IMSK_PIC12_SLEEP,   INSN_PIC12_SLEEP,     ICODE_SLEEP,  INSN_CLASS_IMPLICIT  },
  { "subwf",   IMSK_PIC12_SUBWF,   INSN_PIC12_SUBWF,     ICODE_SUBWF,  INSN_CLASS_OPWF5     },
  { "swapf",   IMSK_PIC12_SWAPF,   INSN_PIC12_SWAPF,     ICODE_SWAPF,  INSN_CLASS_OPWF5     },
  { "tris",    IMSK_PIC12_TRIS,    INSN_PIC12_TRIS,      ICODE_TRIS,   INSN_CLASS_OPF3      },
  { "xorlw",   IMSK_PIC12_XORLW,   INSN_PIC12_XORLW,     ICODE_XORLW,  INSN_CLASS_LIT8      },
  { "xorwf",   IMSK_PIC12_XORWF,   INSN_PIC12_XORWF,     ICODE_XORWF,  INSN_CLASS_OPWF5     }
};

const int num_op_12c5xx = TABLE_SIZE(op_12c5xx);

/* PIC 12-bit Enhanced instruction set */
const struct insn op_16c5xx_enh[] = {
  { "movlb",   IMSK_PIC12E_MOVLB,  INSN_PIC12E_MOVLB,    ICODE_MOVLB,  INSN_CLASS_LIT3      },
  { "retfie",  IMSK_PIC12E_RETFIE, INSN_PIC12E_RETFIE,   ICODE_RETFIE, INSN_CLASS_IMPLICIT  },
  { "return",  IMSK_PIC12E_RETURN, INSN_PIC12E_RETURN,   ICODE_RETURN, INSN_CLASS_IMPLICIT  }
};

const int num_op_16c5xx_enh = TABLE_SIZE(op_16c5xx_enh);

/* Scenix SX has a superset of the PIC 12-bit instruction set. */
/*
 * It would be nice if there was a more elegant way to do this,
 * either by adding a flags field to struct insn, or by allowing a
 * processor to have more than one associated table.
 */
const struct insn op_sx[] = {
  { "addwf",   IMSK_PIC12_ADDWF,   INSN_PIC12_ADDWF,     ICODE_ADDWF,  INSN_CLASS_OPWF5     },
  { "andlw",   IMSK_PIC12_ANDLW,   INSN_PIC12_ANDLW,     ICODE_ANDLW,  INSN_CLASS_LIT8      },
  { "andwf",   IMSK_PIC12_ANDWF,   INSN_PIC12_ANDWF,     ICODE_ANDWF,  INSN_CLASS_OPWF5     },
  { "bank",    IMSK_SX_BANK,       INSN_SX_BANK,         ICODE_BANK,   INSN_CLASS_LIT3_BANK }, /* SX only */
  { "bcf",     IMSK_PIC12_BCF,     INSN_PIC12_BCF,       ICODE_BCF,    INSN_CLASS_B5        },
  { "bsf",     IMSK_PIC12_BSF,     INSN_PIC12_BSF,       ICODE_BSF,    INSN_CLASS_B5        },
  { "btfsc",   IMSK_PIC12_BTFSC,   INSN_PIC12_BTFSC,     ICODE_BTFSC,  INSN_CLASS_B5        },
  { "btfss",   IMSK_PIC12_BTFSS,   INSN_PIC12_BTFSS,     ICODE_BTFSS,  INSN_CLASS_B5        },
  { "call",    IMSK_PIC12_CALL,    INSN_PIC12_CALL,      ICODE_CALL,   INSN_CLASS_LIT8C12   },
  { "clrf",    IMSK_PIC12_CLRF,    INSN_PIC12_CLRF,      ICODE_CLRF,   INSN_CLASS_OPF5      },
  { "clrw",    IMSK_PIC12_CLRW,    INSN_PIC12_CLRW,      ICODE_CLRW,   INSN_CLASS_IMPLICIT  },
  { "clrwdt",  IMSK_PIC12_CLRWDT,  INSN_PIC12_CLRWDT,    ICODE_CLRWDT, INSN_CLASS_IMPLICIT  },
  { "comf",    IMSK_PIC12_COMF,    INSN_PIC12_COMF,      ICODE_COMF,   INSN_CLASS_OPWF5     },
  { "decf",    IMSK_PIC12_DECF,    INSN_PIC12_DECF,      ICODE_DECF,   INSN_CLASS_OPWF5     },
  { "decfsz",  IMSK_PIC12_DECFSZ,  INSN_PIC12_DECFSZ,    ICODE_DECFSZ, INSN_CLASS_OPWF5     },
  { "goto",    IMSK_PIC12_GOTO,    INSN_PIC12_GOTO,      ICODE_GOTO,   INSN_CLASS_LIT9      },
  { "incf",    IMSK_PIC12_INCF,    INSN_PIC12_INCF,      ICODE_INCF,   INSN_CLASS_OPWF5     },
  { "incfsz",  IMSK_PIC12_INCFSZ,  INSN_PIC12_INCFSZ,    ICODE_INCFSZ, INSN_CLASS_OPWF5     },
  { "iorlw",   IMSK_PIC12_IORLW,   INSN_PIC12_IORLW,     ICODE_IORLW,  INSN_CLASS_LIT8      },
  { "iorwf",   IMSK_PIC12_IORWF,   INSN_PIC12_IORWF,     ICODE_IORWF,  INSN_CLASS_OPWF5     },
  { "iread",   IMSK_SX_IREAD,      INSN_SX_IREAD,        ICODE_IREAD,  INSN_CLASS_IMPLICIT  }, /* SX only */
  { "mode",    IMSK_SX_MODE,       INSN_SX_MODE,         ICODE_MODE,   INSN_CLASS_LIT4      }, /* SX only */
  { "movf",    IMSK_PIC12_MOVF,    INSN_PIC12_MOVF,      ICODE_MOVF,   INSN_CLASS_OPWF5     },
  { "movlw",   IMSK_PIC12_MOVLW,   INSN_PIC12_MOVLW,     ICODE_MOVLW,  INSN_CLASS_LIT8      },
  { "movmw",   IMSK_SX_MOVMW,      INSN_SX_MOVMW,        ICODE_MOVMW,  INSN_CLASS_IMPLICIT  }, /* SX only */
  { "movwf",   IMSK_PIC12_MOVWF,   INSN_PIC12_MOVWF,     ICODE_MOVWF,  INSN_CLASS_OPF5      },
  { "movwm",   IMSK_SX_MOVWM,      INSN_SX_MOVWM,        ICODE_MOVWM,  INSN_CLASS_IMPLICIT  }, /* SX only */
  { "nop",     IMSK_PIC12_NOP,     INSN_PIC12_NOP,       ICODE_NOP,    INSN_CLASS_IMPLICIT  },
  { "option",  IMSK_PIC12_OPTION,  INSN_PIC12_OPTION,    ICODE_OPTION, INSN_CLASS_IMPLICIT  },
  { "page",    IMSK_SX_PAGE,       INSN_SX_PAGE,         ICODE_PAGE,   INSN_CLASS_LIT3_PAGE }, /* SX only */
  { "reti",    IMSK_SX_RETI,       INSN_SX_RETI,         ICODE_RETI,   INSN_CLASS_IMPLICIT  }, /* SX only */
  { "retiw",   IMSK_SX_RETIW,      INSN_SX_RETIW,        ICODE_RETIW,  INSN_CLASS_IMPLICIT  }, /* SX only */
  { "retlw",   IMSK_PIC12_RETLW,   INSN_PIC12_RETLW,     ICODE_RETLW,  INSN_CLASS_LIT8      },
  { "retp",    IMSK_SX_RETP,       INSN_SX_RETP,         ICODE_RETP,   INSN_CLASS_IMPLICIT  }, /* SX only */
  { "return",  IMSK_SX_RETURN,     INSN_SX_RETURN,       ICODE_RETURN, INSN_CLASS_IMPLICIT  }, /* SX only */
  { "rlf",     IMSK_PIC12_RLF,     INSN_PIC12_RLF,       ICODE_RLF,    INSN_CLASS_OPWF5     },
  { "rrf",     IMSK_PIC12_RRF,     INSN_PIC12_RRF,       ICODE_RRF,    INSN_CLASS_OPWF5     },
  { "sleep",   IMSK_PIC12_SLEEP,   INSN_PIC12_SLEEP,     ICODE_SLEEP,  INSN_CLASS_IMPLICIT  },
  { "subwf",   IMSK_PIC12_SUBWF,   INSN_PIC12_SUBWF,     ICODE_SUBWF,  INSN_CLASS_OPWF5     },
  { "swapf",   IMSK_PIC12_SWAPF,   INSN_PIC12_SWAPF,     ICODE_SWAPF,  INSN_CLASS_OPWF5     },
  { "tris",    IMSK_PIC12_TRIS,    INSN_PIC12_TRIS,      ICODE_TRIS,   INSN_CLASS_OPF5      }, /* FIXME: This should probably be INSN_CLASS_OPF3 as for 12c5xx? */
  { "xorlw",   IMSK_PIC12_XORLW,   INSN_PIC12_XORLW,     ICODE_XORLW,  INSN_CLASS_LIT8      },
  { "xorwf",   IMSK_PIC12_XORWF,   INSN_PIC12_XORWF,     ICODE_XORWF,  INSN_CLASS_OPWF5     }
};

const int num_op_sx = TABLE_SIZE(op_sx);

/* PIC 14-bit instruction set */
struct insn op_16cxx[] = {
  { "addlw",   IMSK_PIC14_ADDLW,   INSN_PIC14_ADDLW,     ICODE_ADDLW,  INSN_CLASS_LIT8      },
  { "addwf",   IMSK_PIC14_ADDWF,   INSN_PIC14_ADDWF,     ICODE_ADDWF,  INSN_CLASS_OPWF7     },
  { "andlw",   IMSK_PIC14_ANDLW,   INSN_PIC14_ANDLW,     ICODE_ANDLW,  INSN_CLASS_LIT8      },
  { "andwf",   IMSK_PIC14_ANDWF,   INSN_PIC14_ANDWF,     ICODE_ANDWF,  INSN_CLASS_OPWF7     },
  { "bcf",     IMSK_PIC14_BCF,     INSN_PIC14_BCF,       ICODE_BCF,    INSN_CLASS_B7        },
  { "bsf",     IMSK_PIC14_BSF,     INSN_PIC14_BSF,       ICODE_BSF,    INSN_CLASS_B7        },
  { "btfsc",   IMSK_PIC14_BTFSC,   INSN_PIC14_BTFSC,     ICODE_BTFSC,  INSN_CLASS_B7        },
  { "btfss",   IMSK_PIC14_BTFSS,   INSN_PIC14_BTFSS,     ICODE_BTFSS,  INSN_CLASS_B7        },
  { "call",    IMSK_PIC14_CALL,    INSN_PIC14_CALL,      ICODE_CALL,   INSN_CLASS_LIT11     },
  { "clrf",    IMSK_PIC14_CLRF,    INSN_PIC14_CLRF,      ICODE_CLRF,   INSN_CLASS_OPF7      },
  { "clrw",    IMSK_PIC14_CLRW,    INSN_PIC14_CLRW,      ICODE_CLRW,   INSN_CLASS_IMPLICIT  },
  { "clrwdt",  IMSK_PIC14_CLRWDT,  INSN_PIC14_CLRWDT,    ICODE_CLRWDT, INSN_CLASS_IMPLICIT  },
  { "comf",    IMSK_PIC14_COMF,    INSN_PIC14_COMF,      ICODE_COMF,   INSN_CLASS_OPWF7     },
  { "decf",    IMSK_PIC14_DECF,    INSN_PIC14_DECF,      ICODE_DECF,   INSN_CLASS_OPWF7     },
  { "decfsz",  IMSK_PIC14_DECFSZ,  INSN_PIC14_DECFSZ,    ICODE_DECFSZ, INSN_CLASS_OPWF7     },
  { "goto",    IMSK_PIC14_GOTO,    INSN_PIC14_GOTO,      ICODE_GOTO,   INSN_CLASS_LIT11     },
  { "halt",    IMSK_PIC14_HALT,    INSN_PIC14_HALT,      ICODE_HALT,   INSN_CLASS_IMPLICIT  },  /* undocumented */
  { "incf",    IMSK_PIC14_INCF,    INSN_PIC14_INCF,      ICODE_INCF,   INSN_CLASS_OPWF7     },
  { "incfsz",  IMSK_PIC14_INCFSZ,  INSN_PIC14_INCFSZ,    ICODE_INCFSZ, INSN_CLASS_OPWF7     },
  { "iorlw",   IMSK_PIC14_IORLW,   INSN_PIC14_IORLW,     ICODE_IORLW,  INSN_CLASS_LIT8      },
  { "iorwf",   IMSK_PIC14_IORWF,   INSN_PIC14_IORWF,     ICODE_IORWF,  INSN_CLASS_OPWF7     },
  { "movf",    IMSK_PIC14_MOVF,    INSN_PIC14_MOVF,      ICODE_MOVF,   INSN_CLASS_OPWF7     },
  { "movlw",   IMSK_PIC14_MOVLW,   INSN_PIC14_MOVLW,     ICODE_MOVLW,  INSN_CLASS_LIT8      },
  { "movwf",   IMSK_PIC14_MOVWF,   INSN_PIC14_MOVWF,     ICODE_MOVWF,  INSN_CLASS_OPF7      },
  { "nop",     IMSK_PIC14_NOP,     INSN_PIC14_NOP,       ICODE_NOP,    INSN_CLASS_IMPLICIT  },
  { "option",  IMSK_PIC14_OPTION,  INSN_PIC14_OPTION,    ICODE_OPTION, INSN_CLASS_IMPLICIT  },
  { "retfie",  IMSK_PIC14_RETFIE,  INSN_PIC14_RETFIE,    ICODE_RETFIE, INSN_CLASS_IMPLICIT  },
  { "retlw",   IMSK_PIC14_RETLW,   INSN_PIC14_RETLW,     ICODE_RETLW,  INSN_CLASS_LIT8      },
  { "return",  IMSK_PIC14_RETURN,  INSN_PIC14_RETURN,    ICODE_RETURN, INSN_CLASS_IMPLICIT  },
  { "rlf",     IMSK_PIC14_RLF,     INSN_PIC14_RLF,       ICODE_RLF,    INSN_CLASS_OPWF7     },
  { "rrf",     IMSK_PIC14_RRF,     INSN_PIC14_RRF,       ICODE_RRF,    INSN_CLASS_OPWF7     },
  { "sleep",   IMSK_PIC14_SLEEP,   INSN_PIC14_SLEEP,     ICODE_SLEEP,  INSN_CLASS_IMPLICIT  },
  { "sublw",   IMSK_PIC14_SUBLW,   INSN_PIC14_SUBLW,     ICODE_SUBLW,  INSN_CLASS_LIT8      },
  { "subwf",   IMSK_PIC14_SUBWF,   INSN_PIC14_SUBWF,     ICODE_SUBWF,  INSN_CLASS_OPWF7     },
  { "swapf",   IMSK_PIC14_SWAPF,   INSN_PIC14_SWAPF,     ICODE_SWAPF,  INSN_CLASS_OPWF7     },
  { "tris",    IMSK_PIC14_TRIS,    INSN_PIC14_TRIS,      ICODE_TRIS,   INSN_CLASS_OPF7      },
  { "xorlw",   IMSK_PIC14_XORLW,   INSN_PIC14_XORLW,     ICODE_XORLW,  INSN_CLASS_LIT8      },
  { "xorwf",   IMSK_PIC14_XORWF,   INSN_PIC14_XORWF,     ICODE_XORWF,  INSN_CLASS_OPWF7     }
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
  { "addfsr",  IMSK_PIC14E_ADDFSR, INSN_PIC14E_ADDFSR,   ICODE_ADDFSR, INSN_CLASS_LITFSR_14 },
  { "addlw",   IMSK_PIC14E_ADDLW,  INSN_PIC14E_ADDLW,    ICODE_ADDLW,  INSN_CLASS_LIT8      },
  { "addwfc",  IMSK_PIC14E_ADDWFC, INSN_PIC14E_ADDWFC,   ICODE_ADDWFC, INSN_CLASS_OPWF7     },
  { "asrf",    IMSK_PIC14E_ASRF,   INSN_PIC14E_ASRF,     ICODE_ASRF,   INSN_CLASS_OPWF7     },
  { "bra",     IMSK_PIC14E_BRA,    INSN_PIC14E_BRA,      ICODE_BRA,    INSN_CLASS_RBRA9     },
  { "brw",     IMSK_PIC14E_BRW,    INSN_PIC14E_BRW,      ICODE_BRW,    INSN_CLASS_IMPLICIT  },
  { "callw",   IMSK_PIC14E_CALLW,  INSN_PIC14E_CALLW,    ICODE_CALLW,  INSN_CLASS_IMPLICIT  },
  { "lslf",    IMSK_PIC14E_LSLF,   INSN_PIC14E_LSLF,     ICODE_LSLF,   INSN_CLASS_OPWF7     },
  { "lsrf",    IMSK_PIC14E_LSRF,   INSN_PIC14E_LSRF,     ICODE_LSRF,   INSN_CLASS_OPWF7     },
  { "moviw",   IMSK_PIC14E_MOVIW,  INSN_PIC14E_MOVIW,    ICODE_MOVIW,  INSN_CLASS_MOVINDF   },
  { "movlb",   IMSK_PIC14E_MOVLB,  INSN_PIC14E_MOVLB,    ICODE_MOVLB,  INSN_CLASS_LIT5      },
  { "movlp",   IMSK_PIC14E_MOVLP,  INSN_PIC14E_MOVLP,    ICODE_MOVLP,  INSN_CLASS_LIT7      },
  { "movlw",   IMSK_PIC14E_MOVLW,  INSN_PIC14E_MOVLW,    ICODE_MOVLW,  INSN_CLASS_LIT8      },
  { "movwi",   IMSK_PIC14E_MOVWI,  INSN_PIC14E_MOVWI,    ICODE_MOVWI,  INSN_CLASS_MOVINDF   },
  { "nop",     IMSK_PIC14E_NOP,    INSN_PIC14E_NOP,      ICODE_NOP,    INSN_CLASS_IMPLICIT  },
  { "reset",   IMSK_PIC14E_RESET,  INSN_PIC14E_RESET,    ICODE_RESET,  INSN_CLASS_IMPLICIT  },
  { "retlw",   IMSK_PIC14E_RETLW,  INSN_PIC14E_RETLW,    ICODE_RETLW,  INSN_CLASS_LIT8      },
  { "sublw",   IMSK_PIC14E_SUBLW,  INSN_PIC14E_SUBLW,    ICODE_SUBLW,  INSN_CLASS_LIT8      },
  { "subwfb",  IMSK_PIC14E_SUBWFB, INSN_PIC14E_SUBWFB,   ICODE_SUBWFB, INSN_CLASS_OPWF7     }
};

const int num_op_16cxx_enh = TABLE_SIZE(op_16cxx_enh);

/* PIC 16-bit instruction set */
const struct insn op_17cxx[] = {
  { "addlw",   IMSK_PIC16_ADDLW,   INSN_PIC16_ADDLW,     ICODE_ADDLW,  INSN_CLASS_LIT8      },
  { "addwf",   IMSK_PIC16_ADDWF,   INSN_PIC16_ADDWF,     ICODE_ADDWF,  INSN_CLASS_OPWF8     },
  { "addwfc",  IMSK_PIC16_ADDWFC,  INSN_PIC16_ADDWFC,    ICODE_ADDWFC, INSN_CLASS_OPWF8     },
  { "andlw",   IMSK_PIC16_ANDLW,   INSN_PIC16_ANDLW,     ICODE_ANDLW,  INSN_CLASS_LIT8      },
  { "andwf",   IMSK_PIC16_ANDWF,   INSN_PIC16_ANDWF,     ICODE_ANDWF,  INSN_CLASS_OPWF8     },
  { "bcf",     IMSK_PIC16_BCF,     INSN_PIC16_BCF,       ICODE_BCF,    INSN_CLASS_B8        },
  { "bsf",     IMSK_PIC16_BSF,     INSN_PIC16_BSF,       ICODE_BSF,    INSN_CLASS_B8        },
  { "btfsc",   IMSK_PIC16_BTFSC,   INSN_PIC16_BTFSC,     ICODE_BTFSC,  INSN_CLASS_B8        },
  { "btfss",   IMSK_PIC16_BTFSS,   INSN_PIC16_BTFSS,     ICODE_BTFSS,  INSN_CLASS_B8        },
  { "btg",     IMSK_PIC16_BTG,     INSN_PIC16_BTG,       ICODE_BTG,    INSN_CLASS_B8        },
  { "call",    IMSK_PIC16_CALL,    INSN_PIC16_CALL,      ICODE_CALL,   INSN_CLASS_LIT13     },
  { "clrf",    IMSK_PIC16_CLRF,    INSN_PIC16_CLRF,      ICODE_CLRF,   INSN_CLASS_OPWF8     },
  { "clrwdt",  IMSK_PIC16_CLRWDT,  INSN_PIC16_CLRWDT,    ICODE_CLRWDT, INSN_CLASS_IMPLICIT  },
  { "comf",    IMSK_PIC16_COMF,    INSN_PIC16_COMF,      ICODE_COMF,   INSN_CLASS_OPWF8     },
  { "cpfseq",  IMSK_PIC16_CPFSEQ,  INSN_PIC16_CPFSEQ,    ICODE_CPFSEQ, INSN_CLASS_OPF8      },
  { "cpfsgt",  IMSK_PIC16_CPFSGT,  INSN_PIC16_CPFSGT,    ICODE_CPFSGT, INSN_CLASS_OPF8      },
  { "cpfslt",  IMSK_PIC16_CPFSLT,  INSN_PIC16_CPFSLT,    ICODE_CPFSLT, INSN_CLASS_OPF8      },
  { "daw",     IMSK_PIC16_DAW,     INSN_PIC16_DAW,       ICODE_DAW,    INSN_CLASS_OPWF8     },
  { "dcfsnz",  IMSK_PIC16_DCFSNZ,  INSN_PIC16_DCFSNZ,    ICODE_DCFSNZ, INSN_CLASS_OPWF8     },
  { "decf",    IMSK_PIC16_DECF,    INSN_PIC16_DECF,      ICODE_DECF,   INSN_CLASS_OPWF8     },
  { "decfsz",  IMSK_PIC16_DECFSZ,  INSN_PIC16_DECFSZ,    ICODE_DECFSZ, INSN_CLASS_OPWF8     },
  { "goto",    IMSK_PIC16_GOTO,    INSN_PIC16_GOTO,      ICODE_GOTO,   INSN_CLASS_LIT13     },
  { "incf",    IMSK_PIC16_INCF,    INSN_PIC16_INCF,      ICODE_INCF,   INSN_CLASS_OPWF8     },
  { "incfsz",  IMSK_PIC16_INCFSZ,  INSN_PIC16_INCFSZ,    ICODE_INCFSZ, INSN_CLASS_OPWF8     },
  { "infsnz",  IMSK_PIC16_INFSNZ,  INSN_PIC16_INFSNZ,    ICODE_INFSNZ, INSN_CLASS_OPWF8     },
  { "iorlw",   IMSK_PIC16_IORLW,   INSN_PIC16_IORLW,     ICODE_IORLW,  INSN_CLASS_LIT8      },
  { "iorwf",   IMSK_PIC16_IORWF,   INSN_PIC16_IORWF,     ICODE_IORWF,  INSN_CLASS_OPWF8     },
  { "lcall",   IMSK_PIC16_LCALL,   INSN_PIC16_LCALL,     ICODE_LCALL,  INSN_CLASS_LIT8C16   },
  { "movfp",   IMSK_PIC16_MOVFP,   INSN_PIC16_MOVFP,     ICODE_MOVFP,  INSN_CLASS_FP        },
  { "movlb",   IMSK_PIC16_MOVLB,   INSN_PIC16_MOVLB,     ICODE_MOVLB,  INSN_CLASS_LIT8      },
  { "movlr",   IMSK_PIC16_MOVLR,   INSN_PIC16_MOVLR,     ICODE_MOVLR,  INSN_CLASS_LIT4S     },
  { "movlw",   IMSK_PIC16_MOVLW,   INSN_PIC16_MOVLW,     ICODE_MOVLW,  INSN_CLASS_LIT8      },
  { "movpf",   IMSK_PIC16_MOVPF,   INSN_PIC16_MOVPF,     ICODE_MOVPF,  INSN_CLASS_PF        },
  { "movwf",   IMSK_PIC16_MOVWF,   INSN_PIC16_MOVWF,     ICODE_MOVWF,  INSN_CLASS_OPF8      },
  { "mullw",   IMSK_PIC16_MULLW,   INSN_PIC16_MULLW,     ICODE_MULLW,  INSN_CLASS_LIT8      },
  { "mulwf",   IMSK_PIC16_MULWF,   INSN_PIC16_MULWF,     ICODE_MULWF,  INSN_CLASS_OPF8      },
  { "negw",    IMSK_PIC16_NEGW,    INSN_PIC16_NEGW,      ICODE_NEGW,   INSN_CLASS_OPWF8     },
  { "nop",     IMSK_PIC16_NOP,     INSN_PIC16_NOP,       ICODE_NOP,    INSN_CLASS_IMPLICIT  },
  { "retfie",  IMSK_PIC16_RETFIE,  INSN_PIC16_RETFIE,    ICODE_RETFIE, INSN_CLASS_IMPLICIT  },
  { "retlw",   IMSK_PIC16_RETLW,   INSN_PIC16_RETLW,     ICODE_RETLW,  INSN_CLASS_LIT8      },
  { "return",  IMSK_PIC16_RETURN,  INSN_PIC16_RETURN,    ICODE_RETURN, INSN_CLASS_IMPLICIT  },
  { "rlcf",    IMSK_PIC16_RLCF,    INSN_PIC16_RLCF,      ICODE_RLCF,   INSN_CLASS_OPWF8     },
  { "rlncf",   IMSK_PIC16_RLNCF,   INSN_PIC16_RLNCF,     ICODE_RLNCF,  INSN_CLASS_OPWF8     },
  { "rrcf",    IMSK_PIC16_RRCF,    INSN_PIC16_RRCF,      ICODE_RRCF,   INSN_CLASS_OPWF8     },
  { "rrncf",   IMSK_PIC16_RRNCF,   INSN_PIC16_RRNCF,     ICODE_RRNCF,  INSN_CLASS_OPWF8     },
  { "setf",    IMSK_PIC16_SETF,    INSN_PIC16_SETF,      ICODE_SETF,   INSN_CLASS_OPWF8     },
  { "sleep",   IMSK_PIC16_SLEEP,   INSN_PIC16_SLEEP,     ICODE_SLEEP,  INSN_CLASS_IMPLICIT  },
  { "sublw",   IMSK_PIC16_SUBLW,   INSN_PIC16_SUBLW,     ICODE_SUBLW,  INSN_CLASS_LIT8      },
  { "subwf",   IMSK_PIC16_SUBWF,   INSN_PIC16_SUBWF,     ICODE_SUBWF,  INSN_CLASS_OPWF8     },
  { "subwfb",  IMSK_PIC16_SUBWFB,  INSN_PIC16_SUBWFB,    ICODE_SUBWFB, INSN_CLASS_OPWF8     },
  { "swapf",   IMSK_PIC16_SWAPF,   INSN_PIC16_SWAPF,     ICODE_SWAPF,  INSN_CLASS_OPWF8     },
  { "tablrd",  IMSK_PIC16_TABLRD,  INSN_PIC16_TABLRD,    ICODE_TABLRD, INSN_CLASS_TBL3      },
  { "tablwt",  IMSK_PIC16_TABLWT,  INSN_PIC16_TABLWT,    ICODE_TABLWT, INSN_CLASS_TBL3      },
  { "tlrd",    IMSK_PIC16_TLRD,    INSN_PIC16_TLRD,      ICODE_TLRD,   INSN_CLASS_TBL2      },
  { "tlwt",    IMSK_PIC16_TLWT,    INSN_PIC16_TLWT,      ICODE_TLWT,   INSN_CLASS_TBL2      },
  { "tstfsz",  IMSK_PIC16_TSTFSZ,  INSN_PIC16_TSTFSZ,    ICODE_TSTFSZ, INSN_CLASS_OPF8      },
  { "xorlw",   IMSK_PIC16_XORLW,   INSN_PIC16_XORLW,     ICODE_XORLW,  INSN_CLASS_LIT8      },
  { "xorwf",   IMSK_PIC16_XORWF,   INSN_PIC16_XORWF,     ICODE_XORWF,  INSN_CLASS_OPWF8     }
};

const int num_op_17cxx = TABLE_SIZE(op_17cxx);

const struct insn op_18cxx[] = {
  { "addlw",   IMSK_PIC16E_ADDLW,  INSN_PIC16E_ADDLW,    ICODE_ADDLW,  INSN_CLASS_LIT8      },
  { "addwf",   IMSK_PIC16E_ADDWF,  INSN_PIC16E_ADDWF,    ICODE_ADDWF,  INSN_CLASS_OPWFA8    },
  { "addwfc",  IMSK_PIC16E_ADDWFC, INSN_PIC16E_ADDWFC,   ICODE_ADDWFC, INSN_CLASS_OPWFA8    },
  { "andlw",   IMSK_PIC16E_ANDLW,  INSN_PIC16E_ANDLW,    ICODE_ANDLW,  INSN_CLASS_LIT8      },
  { "andwf",   IMSK_PIC16E_ANDWF,  INSN_PIC16E_ANDWF,    ICODE_ANDWF,  INSN_CLASS_OPWFA8    },
  { "bc",      IMSK_PIC16E_BC,     INSN_PIC16E_BC,       ICODE_BC,     INSN_CLASS_RBRA8     },
  { "bcf",     IMSK_PIC16E_BCF,    INSN_PIC16E_BCF,      ICODE_BCF,    INSN_CLASS_BA8       },
  { "bn",      IMSK_PIC16E_BN,     INSN_PIC16E_BN,       ICODE_BN,     INSN_CLASS_RBRA8     },
  { "bnc",     IMSK_PIC16E_BNC,    INSN_PIC16E_BNC,      ICODE_BNC,    INSN_CLASS_RBRA8     },
  { "bnn",     IMSK_PIC16E_BNN,    INSN_PIC16E_BNN,      ICODE_BNN,    INSN_CLASS_RBRA8     },
  { "bnov",    IMSK_PIC16E_BNOV,   INSN_PIC16E_BNOV,     ICODE_BNOV,   INSN_CLASS_RBRA8     },
  { "bnz",     IMSK_PIC16E_BNZ,    INSN_PIC16E_BNZ,      ICODE_BNZ,    INSN_CLASS_RBRA8     },
  { "bov",     IMSK_PIC16E_BOV,    INSN_PIC16E_BOV,      ICODE_BOV,    INSN_CLASS_RBRA8     },
  { "bra",     IMSK_PIC16E_BRA,    INSN_PIC16E_BRA,      ICODE_BRA,    INSN_CLASS_RBRA11    },
  { "bsf",     IMSK_PIC16E_BSF,    INSN_PIC16E_BSF,      ICODE_BSF,    INSN_CLASS_BA8       },
  { "btfsc",   IMSK_PIC16E_BTFSC,  INSN_PIC16E_BTFSC,    ICODE_BTFSC,  INSN_CLASS_BA8       },
  { "btfss",   IMSK_PIC16E_BTFSS,  INSN_PIC16E_BTFSS,    ICODE_BTFSS,  INSN_CLASS_BA8       },
  { "btg",     IMSK_PIC16E_BTG,    INSN_PIC16E_BTG,      ICODE_BTG,    INSN_CLASS_BA8       },
  { "bz",      IMSK_PIC16E_BZ,     INSN_PIC16E_BZ,       ICODE_BZ,     INSN_CLASS_RBRA8     },
  { "call",    IMSK_PIC16E_CALL,   INSN_PIC16E_CALL,     ICODE_CALL,   INSN_CLASS_CALL20    },
  { "clrf",    IMSK_PIC16E_CLRF,   INSN_PIC16E_CLRF,     ICODE_CLRF,   INSN_CLASS_OPFA8     },
  { "clrwdt",  IMSK_PIC16E_CLRWDT, INSN_PIC16E_CLRWDT,   ICODE_CLRWDT, INSN_CLASS_IMPLICIT  },
  { "comf",    IMSK_PIC16E_COMF,   INSN_PIC16E_COMF,     ICODE_COMF,   INSN_CLASS_OPWFA8    },
  { "cpfseq",  IMSK_PIC16E_CPFSEQ, INSN_PIC16E_CPFSEQ,   ICODE_CPFSEQ, INSN_CLASS_OPFA8     },
  { "cpfsgt",  IMSK_PIC16E_CPFSGT, INSN_PIC16E_CPFSGT,   ICODE_CPFSGT, INSN_CLASS_OPFA8     },
  { "cpfslt",  IMSK_PIC16E_CPFSLT, INSN_PIC16E_CPFSLT,   ICODE_CPFSLT, INSN_CLASS_OPFA8     },
  { "daw",     IMSK_PIC16E_DAW,    INSN_PIC16E_DAW,      ICODE_DAW,    INSN_CLASS_IMPLICIT  },
  { "dcfsnz",  IMSK_PIC16E_DCFSNZ, INSN_PIC16E_DCFSNZ,   ICODE_DCFSNZ, INSN_CLASS_OPWFA8    },
  { "decf",    IMSK_PIC16E_DECF,   INSN_PIC16E_DECF,     ICODE_DECF,   INSN_CLASS_OPWFA8    },
  { "decfsz",  IMSK_PIC16E_DECFSZ, INSN_PIC16E_DECFSZ,   ICODE_DECFSZ, INSN_CLASS_OPWFA8    },
  { "goto",    IMSK_PIC16E_GOTO,   INSN_PIC16E_GOTO,     ICODE_GOTO,   INSN_CLASS_LIT20     },
  { "halt",    IMSK_PIC16E_HALT,   INSN_PIC16E_HALT,     ICODE_HALT,   INSN_CLASS_IMPLICIT  }, /* undocumented */
  { "incf",    IMSK_PIC16E_INCF,   INSN_PIC16E_INCF,     ICODE_INCF,   INSN_CLASS_OPWFA8    },
  { "incfsz",  IMSK_PIC16E_INCFSZ, INSN_PIC16E_INCFSZ,   ICODE_INCFSZ, INSN_CLASS_OPWFA8    },
  { "infsnz",  IMSK_PIC16E_INFSNZ, INSN_PIC16E_INFSNZ,   ICODE_INFSNZ, INSN_CLASS_OPWFA8    },
  { "iorlw",   IMSK_PIC16E_IORLW,  INSN_PIC16E_IORLW,    ICODE_IORLW,  INSN_CLASS_LIT8      },
  { "iorwf",   IMSK_PIC16E_IORWF,  INSN_PIC16E_IORWF,    ICODE_IORWF,  INSN_CLASS_OPWFA8    },
  { "lfsr",    IMSK_PIC16E_LFSR,   INSN_PIC16E_LFSR,     ICODE_LFSR,   INSN_CLASS_FLIT12    },
  { "movf",    IMSK_PIC16E_MOVF,   INSN_PIC16E_MOVF,     ICODE_MOVF,   INSN_CLASS_OPWFA8    },
  { "movff",   IMSK_PIC16E_MOVFF,  INSN_PIC16E_MOVFF,    ICODE_MOVFF,  INSN_CLASS_FF        },
  { "movlb",   IMSK_PIC16E_MOVLB,  INSN_PIC16E_MOVLB,    ICODE_MOVLB,  INSN_CLASS_LIT4L     },
  { "movlw",   IMSK_PIC16E_MOVLW,  INSN_PIC16E_MOVLW,    ICODE_MOVLW,  INSN_CLASS_LIT8      },
  { "movwf",   IMSK_PIC16E_MOVWF,  INSN_PIC16E_MOVWF,    ICODE_MOVWF,  INSN_CLASS_OPFA8     },
  { "mullw",   IMSK_PIC16E_MULLW,  INSN_PIC16E_MULLW,    ICODE_MULLW,  INSN_CLASS_LIT8      },
  { "mulwf",   IMSK_PIC16E_MULWF,  INSN_PIC16E_MULWF,    ICODE_MULWF,  INSN_CLASS_OPFA8     },
  { "negf",    IMSK_PIC16E_NEGF,   INSN_PIC16E_NEGF,     ICODE_NEGF,   INSN_CLASS_OPFA8     },
  { "nop",     IMSK_PIC16E_NOP,    INSN_PIC16E_NOP,      ICODE_NOP,    INSN_CLASS_IMPLICIT  },
  { "pop",     IMSK_PIC16E_POP,    INSN_PIC16E_POP,      ICODE_POP,    INSN_CLASS_IMPLICIT  },
  { "push",    IMSK_PIC16E_PUSH,   INSN_PIC16E_PUSH,     ICODE_PUSH,   INSN_CLASS_IMPLICIT  },
  { "rcall",   IMSK_PIC16E_RCALL,  INSN_PIC16E_RCALL,    ICODE_RCALL,  INSN_CLASS_RBRA11    },
  { "reset",   IMSK_PIC16E_RESET,  INSN_PIC16E_RESET,    ICODE_RESET,  INSN_CLASS_IMPLICIT  },
  { "retfie",  IMSK_PIC16E_RETFIE, INSN_PIC16E_RETFIE,   ICODE_RETFIE, INSN_CLASS_LIT1      },
  { "retlw",   IMSK_PIC16E_RETLW,  INSN_PIC16E_RETLW,    ICODE_RETLW,  INSN_CLASS_LIT8      },
  { "return",  IMSK_PIC16E_RETURN, INSN_PIC16E_RETURN,   ICODE_RETURN, INSN_CLASS_LIT1      },
  { "rlcf",    IMSK_PIC16E_RLCF,   INSN_PIC16E_RLCF,     ICODE_RLCF,   INSN_CLASS_OPWFA8    },
  { "rlncf",   IMSK_PIC16E_RLNCF,  INSN_PIC16E_RLNCF,    ICODE_RLNCF,  INSN_CLASS_OPWFA8    },
  { "rrcf",    IMSK_PIC16E_RRCF,   INSN_PIC16E_RRCF,     ICODE_RRCF,   INSN_CLASS_OPWFA8    },
  { "rrncf",   IMSK_PIC16E_RRNCF,  INSN_PIC16E_RRNCF,    ICODE_RRNCF,  INSN_CLASS_OPWFA8    },
  { "setf",    IMSK_PIC16E_SETF,   INSN_PIC16E_SETF,     ICODE_SETF,   INSN_CLASS_OPFA8     },
  { "sleep",   IMSK_PIC16E_SLEEP,  INSN_PIC16E_SLEEP,    ICODE_SLEEP,  INSN_CLASS_IMPLICIT  },
  { "subfwb",  IMSK_PIC16E_SUBFWB, INSN_PIC16E_SUBFWB,   ICODE_SUBFWB, INSN_CLASS_OPWFA8    },
  { "sublw",   IMSK_PIC16E_SUBLW,  INSN_PIC16E_SUBLW,    ICODE_SUBLW,  INSN_CLASS_LIT8      },
  { "subwf",   IMSK_PIC16E_SUBWF,  INSN_PIC16E_SUBWF,    ICODE_SUBWF,  INSN_CLASS_OPWFA8    },
  { "subwfb",  IMSK_PIC16E_SUBWFB, INSN_PIC16E_SUBWFB,   ICODE_SUBWFB, INSN_CLASS_OPWFA8    },
  { "swapf",   IMSK_PIC16E_SWAPF,  INSN_PIC16E_SWAPF,    ICODE_SWAPF,  INSN_CLASS_OPWFA8    },
  { "tblrd",   IMSK_PIC16E_TBLRD,  INSN_PIC16E_TBLRD,    ICODE_TBLRD,  INSN_CLASS_TBL       },
  { "tblwt",   IMSK_PIC16E_TBLWT,  INSN_PIC16E_TBLWT,    ICODE_TBLWT,  INSN_CLASS_TBL       },
  { "trap",    IMSK_PIC16E_TRAP,   INSN_PIC16E_TRAP,     ICODE_TRAP,   INSN_CLASS_IMPLICIT  }, /* undocumented */
  { "tret",    IMSK_PIC16E_TRET,   INSN_PIC16E_TRET,     ICODE_TRET,   INSN_CLASS_IMPLICIT  }, /* undocumented */
  { "tstfsz",  IMSK_PIC16E_TSTFSZ, INSN_PIC16E_TSTFSZ,   ICODE_TSTFSZ, INSN_CLASS_OPFA8     },
  { "xorlw",   IMSK_PIC16E_XORLW,  INSN_PIC16E_XORLW,    ICODE_XORLW,  INSN_CLASS_LIT8      },
  { "xorwf",   IMSK_PIC16E_XORWF,  INSN_PIC16E_XORWF,    ICODE_XORWF,  INSN_CLASS_OPWFA8    }
};

const int num_op_18cxx = TABLE_SIZE(op_18cxx);

/* PIC 16-bit "Special" instruction set */
const struct insn op_18cxx_sp[] = {
  { "clrc",    0xffff, INSN_PIC16ES_CLRC,    0, INSN_CLASS_IMPLICIT  },
  { "clrdc",   0xffff, INSN_PIC16ES_CLRDC,   0, INSN_CLASS_IMPLICIT  },
  { "clrn",    0xffff, INSN_PIC16ES_CLRN,    0, INSN_CLASS_IMPLICIT  },
  { "clrov",   0xffff, INSN_PIC16ES_CLROV,   0, INSN_CLASS_IMPLICIT  },
  { "clrw",    0xffff, INSN_PIC16ES_CLRW,    0, INSN_CLASS_IMPLICIT  },
  { "clrz",    0xffff, INSN_PIC16ES_CLRZ,    0, INSN_CLASS_IMPLICIT  },
  { "setc",    0xffff, INSN_PIC16ES_SETC,    0, INSN_CLASS_IMPLICIT  },
  { "setdc",   0xffff, INSN_PIC16ES_SETDC,   0, INSN_CLASS_IMPLICIT  },
  { "setn",    0xffff, INSN_PIC16ES_SETN,    0, INSN_CLASS_IMPLICIT  },
  { "setov",   0xffff, INSN_PIC16ES_SETOV,   0, INSN_CLASS_IMPLICIT  },
  { "setz",    0xffff, INSN_PIC16ES_SETZ,    0, INSN_CLASS_IMPLICIT  },
  { "skpc",    0xffff, INSN_PIC16ES_SKPC,    0, INSN_CLASS_IMPLICIT  },
  { "skpdc",   0xffff, INSN_PIC16ES_SKPDC,   0, INSN_CLASS_IMPLICIT  },
  { "skpn",    0xffff, INSN_PIC16ES_SKPN,    0, INSN_CLASS_IMPLICIT  },
  { "skpov",   0xffff, INSN_PIC16ES_SKPOV,   0, INSN_CLASS_IMPLICIT  },
  { "skpz",    0xffff, INSN_PIC16ES_SKPZ,    0, INSN_CLASS_IMPLICIT  },
  { "skpnc",   0xffff, INSN_PIC16ES_SKPNC,   0, INSN_CLASS_IMPLICIT  },
  { "skpndc",  0xffff, INSN_PIC16ES_SKPNDC,  0, INSN_CLASS_IMPLICIT  },
  { "skpnn",   0xffff, INSN_PIC16ES_SKPNN,   0, INSN_CLASS_IMPLICIT  },
  { "skpnov",  0xffff, INSN_PIC16ES_SKPNOV,  0, INSN_CLASS_IMPLICIT  },
  { "skpnz",   0xffff, INSN_PIC16ES_SKPNZ,   0, INSN_CLASS_IMPLICIT  },
  { "tgc",     0xffff, INSN_PIC16ES_TGC,     0, INSN_CLASS_IMPLICIT  },
  { "tgdc",    0xffff, INSN_PIC16ES_TGDC,    0, INSN_CLASS_IMPLICIT  },
  { "tgn",     0xffff, INSN_PIC16ES_TGN,     0, INSN_CLASS_IMPLICIT  },
  { "tgov",    0xffff, INSN_PIC16ES_TGOV,    0, INSN_CLASS_IMPLICIT  },
  { "tgz",     0xffff, INSN_PIC16ES_TGZ,     0, INSN_CLASS_IMPLICIT  }
};

const int num_op_18cxx_sp = TABLE_SIZE(op_18cxx_sp);

/* PIC 16-bit Extended instruction set */
const struct insn op_18cxx_ext[] = {
  { "addfsr",  IMSK_PIC16EX_ADDFSR,  INSN_PIC16EX_ADDFSR,  ICODE_ADDFSR,  INSN_CLASS_LITFSR_16 },
  { "addulnk", IMSK_PIC16EX_ADDULNK, INSN_PIC16EX_ADDULNK, ICODE_ADDULNK, INSN_CLASS_LIT6      },
  { "callw",   IMSK_PIC16EX_CALLW,   INSN_PIC16EX_CALLW,   ICODE_CALLW,   INSN_CLASS_IMPLICIT  },
  { "movsf",   IMSK_PIC16EX_MOVSF,   INSN_PIC16EX_MOVSF,   ICODE_MOVSF,   INSN_CLASS_SF        },
  { "movss",   IMSK_PIC16EX_MOVSS,   INSN_PIC16EX_MOVSS,   ICODE_MOVSS,   INSN_CLASS_SS        },
  { "pushl",   IMSK_PIC16EX_PUSHL,   INSN_PIC16EX_PUSHL,   ICODE_PUSHL,   INSN_CLASS_LIT8      },
  { "subfsr",  IMSK_PIC16EX_SUBFSR,  INSN_PIC16EX_SUBFSR,  ICODE_SUBFSR,  INSN_CLASS_LITFSR_16 },
  { "subulnk", IMSK_PIC16EX_SUBULNK, INSN_PIC16EX_SUBULNK, ICODE_SUBULNK, INSN_CLASS_LIT6      }
};

const int num_op_18cxx_ext = TABLE_SIZE(op_18cxx_ext);

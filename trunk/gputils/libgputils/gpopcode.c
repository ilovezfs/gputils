/* GNU PIC opcode definitions
   Copyright (C) 2001 Craig Franklin

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
#include "gpopcode.h"

/* FIXME: use const struct */

/* XXXPRO: Need to add a struct here for any extra processor classes. */

/* PIC 12-bit instruction set */
struct insn  op_12c5xx[] = {
  { "ADDWF",  0x1c0,	INSN_CLASS_OPWF5 	},
  { "ANDLW",  0xe00,	INSN_CLASS_LIT8 	},
  { "ANDWF",  0x140,	INSN_CLASS_OPWF5 	},
  { "BCF",    0x400,	INSN_CLASS_B5 		},
  { "BSF",    0x500,	INSN_CLASS_B5 		},
  { "BTFSC",  0x600,	INSN_CLASS_B5 		},
  { "BTFSS",  0x700,	INSN_CLASS_B5		},
  { "CALL",   0x900,	INSN_CLASS_LIT8C 	},
  { "CLRF",   0x060,	INSN_CLASS_OPF5 	},
  { "CLRW",   0x040,	INSN_CLASS_IMPLICIT 	},
  { "CLRWDT", 0x004, 	INSN_CLASS_IMPLICIT 	},
  { "COMF",   0x240,	INSN_CLASS_OPWF5 	},
  { "DECF",   0x0c0,	INSN_CLASS_OPWF5 	},
  { "DECFSZ", 0x2c0,	INSN_CLASS_OPWF5 	},
  { "GOTO",   0xa00,	INSN_CLASS_LIT9 	},
  { "INCF",   0x280,	INSN_CLASS_OPWF5 	},
  { "INCFSZ", 0x3c0,	INSN_CLASS_OPWF5 	},
  { "IORLW",  0xd00,	INSN_CLASS_LIT8 	},
  { "IORWF",  0x100,	INSN_CLASS_OPWF5 	},
  { "MOVF",   0x200,	INSN_CLASS_OPWF5 	},
  { "MOVLW",  0xc00,	INSN_CLASS_LIT8 	},
  { "MOVWF",  0x020,	INSN_CLASS_OPF5 	},
  { "NOP",    0x000,	INSN_CLASS_IMPLICIT 	},
  { "OPTION", 0x002,	INSN_CLASS_IMPLICIT 	},
  { "RETLW",  0x800,	INSN_CLASS_LIT8 	},
  { "RETURN", 0x800,	INSN_CLASS_IMPLICIT 	}, /* special mnemonic */
  { "RLF",    0x340,	INSN_CLASS_OPWF5 	},
  { "RRF",    0x300,	INSN_CLASS_OPWF5 	},
  { "SLEEP",  0x003,	INSN_CLASS_IMPLICIT 	},
  { "SUBWF",  0x080,	INSN_CLASS_OPWF5 	},
  { "SWAPF",  0x380,	INSN_CLASS_OPWF5 	},
  { "TRIS",   0x000,	INSN_CLASS_OPF5 	}, /* not optimal */
  { "XORLW",  0xf00,	INSN_CLASS_LIT8 	},
  { "XORWF",  0x180,	INSN_CLASS_OPWF5 	}
};

const int num_op_12c5xx = TABLE_SIZE(op_12c5xx);

/* Scenix SX has a superset of the PIC 12-bit instruction set */
/*
 * It would be nice if there was a more elegant way to do this,
 * either by adding a flags field to struct insn, or by allowing a
 * processor to have more than one associated table.
 */
struct insn  op_sx[] = {
  { "ADDWF",  0x1c0,	INSN_CLASS_OPWF5 	},
  { "ANDLW",  0xe00,	INSN_CLASS_LIT8 	},
  { "ANDWF",  0x140,	INSN_CLASS_OPWF5 	},
  { "BANK",   0x018,    INSN_CLASS_LIT3_BANK    }, /* SX only */
  { "BCF",    0x400,	INSN_CLASS_B5 		},
  { "BSF",    0x500,	INSN_CLASS_B5 		},
  { "BTFSC",  0x600,	INSN_CLASS_B5 		},
  { "BTFSS",  0x700,	INSN_CLASS_B5		},
  { "CALL",   0x900,	INSN_CLASS_LIT8C 	},
  { "CLRF",   0x060,	INSN_CLASS_OPF5 	},
  { "CLRW",   0x040,	INSN_CLASS_IMPLICIT 	},
  { "CLRWDT", 0x004, 	INSN_CLASS_IMPLICIT 	},
  { "COMF",   0x240,	INSN_CLASS_OPWF5 	},
  { "DECF",   0x0c0,	INSN_CLASS_OPWF5 	},
  { "DECFSZ", 0x2c0,	INSN_CLASS_OPWF5 	},
  { "GOTO",   0xa00,	INSN_CLASS_LIT9 	},
  { "INCF",   0x280,	INSN_CLASS_OPWF5 	},
  { "INCFSZ", 0x3c0,	INSN_CLASS_OPWF5 	},
  { "IORLW",  0xd00,	INSN_CLASS_LIT8 	},
  { "IORWF",  0x100,	INSN_CLASS_OPWF5 	},
  { "IREAD",  0x041,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "MODE",   0x050,    INSN_CLASS_LIT4         }, /* SX only */
  { "MOVF",   0x200,	INSN_CLASS_OPWF5 	},
  { "MOVLW",  0xc00,	INSN_CLASS_LIT8 	},
  { "MOVMW",  0x042,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "MOVWF",  0x020,	INSN_CLASS_OPF5 	},
  { "MOVWM",  0x043,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "NOP",    0x000,	INSN_CLASS_IMPLICIT 	},
  { "OPTION", 0x002,	INSN_CLASS_IMPLICIT 	},
  { "PAGE",   0x010,    INSN_CLASS_LIT3_PAGE    }, /* SX only */
  { "RETI",   0x00e,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "RETIW",  0x00f,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "RETLW",  0x800,	INSN_CLASS_LIT8 	},
  { "RETP",   0x00d,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "RETURN", 0x00c,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "RLF",    0x340,	INSN_CLASS_OPWF5 	},
  { "RRF",    0x300,	INSN_CLASS_OPWF5 	},
  { "SLEEP",  0x003,	INSN_CLASS_IMPLICIT 	},
  { "SUBWF",  0x080,	INSN_CLASS_OPWF5 	},
  { "SWAPF",  0x380,	INSN_CLASS_OPWF5 	},
  { "TRIS",   0x000,	INSN_CLASS_OPF5 	}, /* not optimal */
  { "XORLW",  0xf00,	INSN_CLASS_LIT8 	},
  { "XORWF",  0x180,	INSN_CLASS_OPWF5 	}
};

const int num_op_sx = TABLE_SIZE(op_sx);

/* PIC 14-bit instruction set */
struct insn  op_16cxx[] = {
  { "ADDLW",  0x3e00, 	INSN_CLASS_LIT8 	},
  { "ADDWF",  0x0700,	INSN_CLASS_OPWF7 	},
  { "ANDLW",  0x3900,	INSN_CLASS_LIT8 	},
  { "ANDWF",  0x0500,	INSN_CLASS_OPWF7 	},
  { "BCF",    0x1000,	INSN_CLASS_B7 		},
  { "BSF",    0x1400,	INSN_CLASS_B7 		},
  { "BTFSC",  0x1800,	INSN_CLASS_B7 		},
  { "BTFSS",  0x1c00,	INSN_CLASS_B7		},
  { "CALL",   0x2000,	INSN_CLASS_LIT11 	},
  { "CLRF",   0x0180,	INSN_CLASS_OPF7		},
  { "CLRW",   0x0103,	INSN_CLASS_IMPLICIT 	},
  { "CLRWDT", 0x0064, 	INSN_CLASS_IMPLICIT 	},
  { "COMF",   0x0900,	INSN_CLASS_OPWF7 	},
  { "DECF",   0x0300,	INSN_CLASS_OPWF7 	},
  { "DECFSZ", 0x0b00,	INSN_CLASS_OPWF7 	},
  { "GOTO",   0x2800,	INSN_CLASS_LIT11 	},
  { "INCF",   0x0a00,	INSN_CLASS_OPWF7 	},
  { "INCFSZ", 0x0f00,	INSN_CLASS_OPWF7 	},
  { "IORLW",  0x3800,	INSN_CLASS_LIT8 	},
  { "IORWF",  0x0400,	INSN_CLASS_OPWF7 	},
  { "MOVF",   0x0800,	INSN_CLASS_OPWF7 	},
  { "MOVLW",  0x3000,	INSN_CLASS_LIT8 	},
  { "MOVWF",  0x0080,	INSN_CLASS_OPF7 	},
  { "NOP",    0x0000,	INSN_CLASS_IMPLICIT 	},
  { "OPTION", 0x0062,	INSN_CLASS_IMPLICIT 	},
  { "RETFIE", 0x0009,	INSN_CLASS_IMPLICIT 	},
  { "RETLW",  0x3400,	INSN_CLASS_LIT8 	},
  { "RETURN", 0x0008,	INSN_CLASS_IMPLICIT 	},
  { "RLF",    0x0d00,	INSN_CLASS_OPWF7 	},
  { "RRF",    0x0c00,	INSN_CLASS_OPWF7 	},
  { "SLEEP",  0x0063,	INSN_CLASS_IMPLICIT 	},
  { "SUBLW",  0x3c00,	INSN_CLASS_LIT8 	},
  { "SUBWF",  0x0200,	INSN_CLASS_OPWF7 	},
  { "SWAPF",  0x0e00,	INSN_CLASS_OPWF7 	},
  { "TRIS",   0x0060,	INSN_CLASS_OPF7		},
  { "XORLW",  0x3a00,	INSN_CLASS_LIT8 	},
  { "XORWF",  0x0600,	INSN_CLASS_OPWF7 	}
};

const int num_op_16cxx = TABLE_SIZE(op_16cxx);

/* PIC 16-bit instruction set */
struct insn  op_17cxx[] = {
  { "ADDLW",  0xb100,   INSN_CLASS_LIT8         },
  { "ADDWF",  0x0e00,   INSN_CLASS_OPWF8        },
  { "ADDWFC", 0x1000,   INSN_CLASS_OPWF8        },
  { "ANDLW",  0xb500,   INSN_CLASS_LIT8         },
  { "ANDWF",  0x0a00,   INSN_CLASS_OPWF8        },
  { "BCF",    0x8800,   INSN_CLASS_B8           },
  { "BSF",    0x8000,   INSN_CLASS_B8           },
  { "BTFSC",  0x9800,   INSN_CLASS_B8           },
  { "BTFSS",  0x9000,   INSN_CLASS_B8           },
  { "BTG",    0x3800,   INSN_CLASS_B8           },
  { "CALL",   0xe000,   INSN_CLASS_LIT13        },
  { "CLRF",   0x2800,   INSN_CLASS_OPWF8        },
  { "CLRWDT", 0x0004,   INSN_CLASS_IMPLICIT     },
  { "COMF",   0x1200,   INSN_CLASS_OPWF8        },
  { "CPFSEQ", 0x3100,   INSN_CLASS_OPF8         },
  { "CPFSGT", 0x3200,   INSN_CLASS_OPF8         },
  { "CPFSLT", 0x3000,   INSN_CLASS_OPF8         },
  { "DAW",    0x2e00,   INSN_CLASS_OPWF8        },
  { "DECF",   0x0600,   INSN_CLASS_OPWF8        },
  { "DECFSZ", 0x1600,   INSN_CLASS_OPWF8        },
  { "DCFSNZ", 0x2600,   INSN_CLASS_OPWF8        },
  { "GOTO",   0xc000,   INSN_CLASS_LIT13        },
  { "INCF",   0x1400,   INSN_CLASS_OPWF8        },
  { "INCFSZ", 0x1e00,   INSN_CLASS_OPWF8        },
  { "INFSNZ", 0x2400,   INSN_CLASS_OPWF8        },
  { "IORLW",  0xb300,   INSN_CLASS_LIT8         },
  { "IORWF",  0x0800,   INSN_CLASS_OPWF8        },
  { "LCALL",  0xb700,   INSN_CLASS_LIT8         },
  { "MOVFP",  0x6000,   INSN_CLASS_FP           },
  { "MOVPF",  0x4000,   INSN_CLASS_PF           },
  { "MOVLB",  0xb800,   INSN_CLASS_LIT8         },
  { "MOVLR",  0xba00,   INSN_CLASS_LIT4S        },
  { "MOVLW",  0xb000,   INSN_CLASS_LIT8         },
  { "MOVWF",  0x0100,   INSN_CLASS_OPF8         },
  { "MULLW",  0xbc00,   INSN_CLASS_LIT8         },
  { "MULWF",  0x3400,   INSN_CLASS_OPF8         },
  { "NEGW",   0x2c00,   INSN_CLASS_OPWF8        },
  { "NOP",    0x0000,   INSN_CLASS_IMPLICIT     },
  { "RETFIE", 0x0005,   INSN_CLASS_IMPLICIT     },
  { "RETLW",  0xb600,   INSN_CLASS_LIT8         },
  { "RETURN", 0x0002,   INSN_CLASS_IMPLICIT     },
  { "RLCF",   0x1a00,   INSN_CLASS_OPWF8        },
  { "RLNCF",  0x2200,   INSN_CLASS_OPWF8        },
  { "RRCF",   0x1800,   INSN_CLASS_OPWF8        },
  { "RRNCF",  0x2000,   INSN_CLASS_OPWF8        },
  { "SETF",   0x2a00,   INSN_CLASS_OPWF8        },
  { "SLEEP",  0x0003,   INSN_CLASS_IMPLICIT     },
  { "SUBLW",  0xb200,   INSN_CLASS_LIT8         },
  { "SUBWF",  0x0400,   INSN_CLASS_OPWF8        },
  { "SUBWFB", 0x0200,   INSN_CLASS_OPWF8        },
  { "SWAPF",  0x1c00,   INSN_CLASS_OPWF8        },
  { "TABLRD", 0xa800,   INSN_CLASS_TBL3         },
  { "TABLWT", 0xac00,   INSN_CLASS_TBL3         },
  { "TLRD",   0xa000,   INSN_CLASS_TBL2         },
  { "TLWT",   0xa400,   INSN_CLASS_TBL2         },
  { "TSTFSZ", 0x3300,   INSN_CLASS_OPF8         },
  { "XORLW",  0xb400,   INSN_CLASS_LIT8         },
  { "XORWF",  0x0c00,   INSN_CLASS_OPWF8        }
};

const int num_op_17cxx = TABLE_SIZE(op_17cxx);

struct insn  op_18cxx[] = {
  { "ADDLW",  0x0f00, 	INSN_CLASS_LIT8 	},
  { "ADDWF",  0x2400,	INSN_CLASS_OPWFA8 	},
  { "ADDWFC", 0x2000,	INSN_CLASS_OPWFA8 	},
  { "ANDLW",  0x0b00,	INSN_CLASS_LIT8 	},
  { "ANDWF",  0x1400,	INSN_CLASS_OPWFA8 	},
  { "BC",     0xe200,	INSN_CLASS_RBRA8       	},
  { "BCF",    0x9000,	INSN_CLASS_BA8 		},
  { "BN",     0xe600,	INSN_CLASS_RBRA8       	},
  { "BNC",    0xe300,	INSN_CLASS_RBRA8       	},
  { "BNN",    0xe700,	INSN_CLASS_RBRA8       	},
  { "BNOV",   0xe500,	INSN_CLASS_RBRA8       	},
  { "BNZ",    0xe100,	INSN_CLASS_RBRA8       	},
  { "BRA",    0xd000,	INSN_CLASS_RBRA11	},
  { "BSF",    0x8000,	INSN_CLASS_BA8 		},
  { "BTFSC",  0xb000,	INSN_CLASS_BA8 		},
  { "BTFSS",  0xa000,	INSN_CLASS_BA8		},
  { "BTG",    0x7000,	INSN_CLASS_BA8		},
  { "BOV",    0xe400,	INSN_CLASS_RBRA8       	},
  { "BZ",     0xe000,	INSN_CLASS_RBRA8       	},
  { "CALL",   0xec00,	INSN_CLASS_CALL20 	},
  { "CLRF",   0x6a00,	INSN_CLASS_OPFA8	},
  { "CLRWDT", 0x0004, 	INSN_CLASS_IMPLICIT 	},
  { "COMF",   0x1c00,	INSN_CLASS_OPWFA8 	},
  { "CPFSEQ", 0x6200,	INSN_CLASS_OPFA8 	},
  { "CPFSGT", 0x6400,	INSN_CLASS_OPFA8 	},
  { "CPFSLT", 0x6000,	INSN_CLASS_OPFA8 	},
  { "DAW",    0x0007,	INSN_CLASS_IMPLICIT 	},
  { "DECF",   0x0400,	INSN_CLASS_OPWFA8 	},
  { "DECFSZ", 0x2c00,	INSN_CLASS_OPWFA8 	},
  { "DCFSNZ", 0x4c00,	INSN_CLASS_OPWFA8 	},
  { "GOTO",   0xef00,	INSN_CLASS_LIT20 	},
  { "INCF",   0x2800,	INSN_CLASS_OPWFA8 	},
  { "INCFSZ", 0x3c00,	INSN_CLASS_OPWFA8 	},
  { "INFSNZ", 0x4800,	INSN_CLASS_OPWFA8 	},
  { "IORLW",  0x0900,	INSN_CLASS_LIT8 	},
  { "IORWF",  0x1000,	INSN_CLASS_OPWFA8 	},
  { "LFSR",   0xee00,	INSN_CLASS_FLIT12 	},
  { "MOVF",   0x5000,	INSN_CLASS_OPWFA8 	},
  { "MOVFF",  0xc000,	INSN_CLASS_FF 		},
  { "MOVLB",  0x0100,	INSN_CLASS_LIT8 	},
  { "MOVLW",  0x0e00,	INSN_CLASS_LIT8 	},
  { "MOVWF",  0x6e00,	INSN_CLASS_OPFA8 	},
  { "MULLW",  0x0d00,	INSN_CLASS_LIT8 	},
  { "MULWF",  0x0200,	INSN_CLASS_OPFA8 	},
  { "NEGF",   0x6c00,	INSN_CLASS_OPFA8 	},
  { "NOP",    0x0000,	INSN_CLASS_IMPLICIT 	},
  { "POP",    0x0006,	INSN_CLASS_IMPLICIT 	},
  { "PUSH",   0x0005,	INSN_CLASS_IMPLICIT 	},
  { "RCALL",  0xd800,	INSN_CLASS_RBRA11 	},
  { "RESET",  0x00ff,	INSN_CLASS_IMPLICIT 	}, 
  { "RETFIE", 0x0010,	INSN_CLASS_LIT1 	},
  { "RETLW",  0x0c00,	INSN_CLASS_LIT8 	},
  { "RETURN", 0x0012,	INSN_CLASS_LIT1 	},
  { "RLCF",   0x3400,	INSN_CLASS_OPWFA8 	},
  { "RLNCF",  0x4400,	INSN_CLASS_OPWFA8 	},
  { "RRCF",   0x3000,	INSN_CLASS_OPWFA8 	},
  { "RRNCF",  0x4000,	INSN_CLASS_OPWFA8 	},
  { "SETF",   0x6800,	INSN_CLASS_OPFA8 	},
  { "SLEEP",  0x0003,	INSN_CLASS_IMPLICIT 	},
  { "SUBFWB", 0x5400,	INSN_CLASS_OPWFA8 	},
  { "SUBLW",  0x0800,	INSN_CLASS_LIT8 	},
  { "SUBWF",  0x5c00,	INSN_CLASS_OPWFA8 	},
  { "SUBWFB", 0x5800,	INSN_CLASS_OPWFA8 	},
  { "SWAPF",  0x3800,	INSN_CLASS_OPWFA8 	},
  { "TBLRD",  0x0008,	INSN_CLASS_TBL 		},
  { "TBLWT",  0x000c,	INSN_CLASS_TBL 		},
  { "TSTFSZ", 0x6600,	INSN_CLASS_OPFA8	},
  { "XORLW",  0x0a00,	INSN_CLASS_LIT8 	},
  { "XORWF",  0x1800,	INSN_CLASS_OPWFA8	}
};

const int num_op_18cxx = TABLE_SIZE(op_18cxx);

/* PIC 16-bit "Special" instruction set */
struct insn  op_18cxx_sp[] = {
  { "CLRC",  0x90d8,	INSN_CLASS_IMPLICIT 	},
  { "CLRDC", 0x92d8,	INSN_CLASS_IMPLICIT 	},
  { "CLRN",  0x98d8,	INSN_CLASS_IMPLICIT 	},
  { "CLROV", 0x96d8,	INSN_CLASS_IMPLICIT 	},
  { "CLRW",  0x6ae8,	INSN_CLASS_IMPLICIT 	},
  { "CLRZ",  0x94d8,	INSN_CLASS_IMPLICIT 	},
  { "SETC",  0x80d8,	INSN_CLASS_IMPLICIT 	},
  { "SETDC", 0x82d8,	INSN_CLASS_IMPLICIT 	},
  { "SETN",  0x88d8,	INSN_CLASS_IMPLICIT 	},
  { "SETOV", 0x86d8,	INSN_CLASS_IMPLICIT 	},
  { "SETZ",  0x84d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPC",  0xa0d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPDC", 0xa2d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPN",  0xa8d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPOV", 0xa6d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPZ",  0xa4d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPNC", 0xb0d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPNDC",0xb2d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPNN", 0xb8d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPNOV",0xb6d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPNZ", 0xb4d8,	INSN_CLASS_IMPLICIT 	},
  { "TGC",   0x70d8,	INSN_CLASS_IMPLICIT 	},
  { "TGDC",  0x72d8,	INSN_CLASS_IMPLICIT 	},
  { "TGN",   0x78d8,	INSN_CLASS_IMPLICIT 	},
  { "TGOV",  0x76d8,	INSN_CLASS_IMPLICIT 	},
  { "TGZ",   0x74d8,	INSN_CLASS_IMPLICIT 	}

};

const int num_op_18cxx_sp = TABLE_SIZE(op_18cxx_sp);

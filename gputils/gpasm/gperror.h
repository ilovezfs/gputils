/* Error handling
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin
   Copyright (C) 2012 Borut Razem

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

#ifndef __GPERROR_H__
#define __GPERROR_H__

void gperror_init(void);

void gperror(unsigned int code, char *message);
void gpwarning(unsigned int code, char *message);
void gpmessage(unsigned int code, char *message);

void gpverror(unsigned int code, const char *message, ...);
void gpvwarning(unsigned int code, const char *message, ...);
void gpvmessage(unsigned int code, const char *message, ...);

/* Alternate message functions. Only the prototypes are provided, the user
   must link their own function into gpasm. */
void user_error(unsigned int code, char *message);
void user_warning(unsigned int code, char *message);
void user_message(unsigned int code, char *message);

void add_code(int code);

void gperror_close(void);

/* Error codes
 *
 * The error codes gpasm defines here are identical to MPASM's
 * definitions. Additional gpasm definitions follow the last
 * MPASM definition.
 *
 */
enum GPE_codes {
  GPE_USER             = 101,
  GPE_NOMEM            = 102,
  GPE_SYMFULL          = 103,
  GPE_TEMPFILE         = 104,
  GPE_NOENT            = 105,  /* File or directory not found */
  GPE_STRCPLX          = 106,
  GPE_BADDIGIT         = 107,
  GPE_BADCHAR          = 108,
  GPE_OPENPAR          = 109,  /* Mis-matched open parenthesis */
  GPE_CLOSEPAR         = 110,  /* Mis-matched closed parenthesis */
  GPE_MISSYM           = 111,  /* No symbol in an EQU or SET */
  GPE_NOOP             = 112,  /* Missing an arithmetic operator */
  GPE_NOSYM            = 113,  /* Symbol has not been defined */
  GPE_DIVBY0           = 114,
  GPE_DUPLAB           = 115,
  GPE_DIFFLAB          = 116,
  GPE_ADDROVF          = 117,  /* address overflow */
  GPE_ADDROVR          = 118,  /* overwriting a previously written address */
  GPE_BAD_CALL_ADDR    = 120,
  GPE_ILLEGAL_LABEL    = 121,
  GPE_ILLEGAL_DIR      = 123,
  GPE_ILLEGAL_ARGU     = 124,
  GPE_ILLEGAL_COND     = 125,
  GPE_RANGE            = 126,  /* Argument out of range */
  GPE_TOO_MANY_ARGU    = 127,
  GPE_MISSING_ARGU     = 128,
  GPE_EXPECTED         = 129,
  GPE_EXTRA_PROC       = 130,
  GPE_UNDEF_PROC       = 131,
  GPE_UNKNOWN_PROC     = 132,
  GPE_IHEX             = 133,  /* Hex file format is too small for code */
  GPE_NO_MACRO_NAME    = 135,
  GPE_DUPLICATE_MACRO  = 136,
  GPE_BAD_WHILE_LOOP   = 140,
  GPE_ILLEGAL_NESTING  = 143,
  GPE_UNMATCHED_ENDM   = 145,
  GPE_OBJECT_ONLY      = 149,
  GPE_LABEL_IN_SECTION = 150,
  GPE_UNRESOLVABLE     = 151,
  GPE_WRONG_SECTION    = 152,
  GPE_CONTIG_SECTION   = 154,
  GPE_MUST_BE_LABEL    = 156,
  GPE_ORG_ODD          = 157,
  GPE_FILL_ODD         = 159,
  GPE_CONTIG_CONFIG    = 163,
  GPE_CONTIG_IDLOC     = 164,
  GPE_NO_EXTENDED_MODE = 165,  /*  Extended mode not available for this device */
  GPE_MISSING_BRACKET  = 168,
  GPE_CONSTANT         = 170,  /* Expression within brackets must be constant */
  GPE_IDLOCS_ORDER     = 175,
  GPE_CONFIG_UNKNOWN   = 176,
  GPE_CONFIG_usCONFIG  = 177,
  GPE_RES_ODD_PIC16EA  = 180,
  GPE_UNKNOWN          = 181,

  /* gputils special errors */
  GPE_INTERNAL         = 1101,  /* Internal error */
  GPE_PARSER           = 1102,  /* Parser error */
  GPE_SCANNER          = 1103,  /* Scanner error */

  GPE_IDLOCS_P16E      = 1501,  /* IDLOCS directive use solely to the PIC16E family. */
  GPE_NOF              = 1502,  /* The destination of the storage is not selected. */
  GPE_NOA              = 1503   /* The access of RAM is not selected. */
};

/* Warning codes
 *
 * The warning codes gpasm defines here are identical to MPASM's
 * definitions. Additional gpasm definitions follow the last
 * MPASM definition.
 *
 */
enum GPW_codes {
  GPW_NOT_DEFINED      = 201,
  GPW_RANGE            = 202,  /* Argument out of range */
  GPW_OP_COLUMN_ONE    = 203,
  GPW_DIR_COLUMN_ONE   = 205,
  GPW_MACRO_COLUMN_ONE = 206,
  GPW_LABEL_COLUMN     = 207,
  GPW_MISSING_QUOTE    = 209,
  GPW_EXTRANEOUS       = 211,
  GPW_EXPECTED         = 212,
  GPW_CMDLINE_PROC     = 215,
  GPW_CMDLINE_RADIX    = 216,
  GPW_CMDLINE_HEXFMT   = 217,
  GPW_RADIX            = 218,
  GPW_INVALID_RAM      = 219,
  GPW_EXCEED_ROM       = 220,
  GPW_DISABLE_ERROR    = 222,
  GPW_REDEFINING_PROC  = 223,
  GPW_NOT_RECOMMENDED  = 224,
  GPW_WORD_ALIGNED     = 226,
  GPW_INVALID_ROM      = 228,
  GPW_UNKNOWN          = 231,

  /* gputils special warnings */
  GPW_BANK_PAGE_SEL_AFTER_SKIP = 1201 /* %s after skip instruction. I this really what you intended? */
};

/* Message codes
 *
 * The message codes gpasm defines here are identical to MPASM's
 * definitions. Additional gpasm definitions follow the last
 * MPASM definition.
 *
 */
enum GPM_codes {
  GPM_USER             = 301,  /* User supplied message */
  GPM_BANK             = 302,
  GPM_RANGE            = 303,
  GPM_IDLOC            = 304,
  GPM_NOF              = 305,
  GPM_PAGE             = 306,
  GPM_PAGEBITS         = 307,
  GPM_SUPVAL           = 308,
  GPM_SUPLIN           = 309,
  GPM_SUPRAM           = 310,
  GPM_EXTPAGE          = 312,
  GPM_CBLOCK           = 313,
  GPM_W_MODIFIED       = 316,
  GPM_SPECIAL_MNEMONIC = 318,
  GPM_UNKNOWN          = 319,

  /* gputils special errors */
  GPM_NOA              = 1301
};

#endif

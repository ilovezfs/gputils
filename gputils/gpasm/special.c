/* Implements special instruction mnemonics
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

#include "libgputils.h"
#include "gpasm.h"
#include "gperror.h"
#include "special.h"
#include "directive.h"
#include "evaluate.h"

extern struct pnode *mk_constant(int value);
extern struct pnode *mk_list(struct pnode *head, struct pnode *tail);

struct pnode *make_constant_list(int value1, int value2)
{
  return mk_list(mk_constant(value1), mk_list(mk_constant(value2), NULL));
}

struct pnode *add_symbol_constant(struct pnode *parms, int value)
{
  return mk_list(HEAD(parms), mk_list(mk_constant(value), NULL));
}

gpasmVal set_page_bits(int page)
{
  gpasmVal r;
  int location = 0, page0 = 0, page1 = 0;

  if ((state.device.class == PROC_CLASS_EEPROM8) ||
      (state.device.class == PROC_CLASS_PIC16E)) {
    assert(0);
  } else if (state.device.class == PROC_CLASS_PIC16) {
    location = 0x3;
    gpmessage(GPM_W_MODIFIED, NULL);
    do_insn("movlw", mk_list(mk_constant(page), NULL));
    r = do_insn("movwf", mk_list(mk_constant(location), NULL));
  } else {
    if (state.device.class == PROC_CLASS_PIC14) {
      location = 0xA;
      page0 = 3;
      page1 = 4;    
    } else {
      location = 0x3;
      page0 = 5;
      page1 = 6;    
    }

    switch(page) {
    case 0:
      do_insn("bcf", make_constant_list(location, page0));
      r = do_insn("bcf", make_constant_list(location, page1));
      break;
    case 1:
      do_insn("bsf", make_constant_list(location, page0));
      r = do_insn("bcf", make_constant_list(location, page1));
      break;
    case 2:
      do_insn("bcf", make_constant_list(location, page0));
      r = do_insn("bsf", make_constant_list(location, page1));
      break;
    case 3:
      do_insn("bsf", make_constant_list(location, page0));
      r = do_insn("bsf", make_constant_list(location, page1));
      break;
    default:
      assert(0);
      break;  
    }
  }

  return r;

}

gpasmVal set_bank_bits(int bank)
{
  gpasmVal r;
  int location = 0, bank0 = 0, bank1 = 0;

  if (state.device.class == PROC_CLASS_EEPROM8) {
    assert(0);
  } else if ((state.device.class == PROC_CLASS_PIC16E) ||
             (state.device.class == PROC_CLASS_PIC16)){
    r = do_insn("movlb", mk_list(mk_constant(bank), NULL));
  } else {
    if (state.device.class == PROC_CLASS_PIC14) {
      location = 0x3;
      bank0 = 5;
      bank1 = 6;    
    } else {
      location = 0x4;
      bank0 = 5;
      bank1 = 6;    
    }

    switch(bank) {
    case 0:
      do_insn("bcf", make_constant_list(location, bank0));
      r = do_insn("bcf", make_constant_list(location, bank1));
      break;
    case 1:
      do_insn("bsf", make_constant_list(location, bank0));
      r = do_insn("bcf", make_constant_list(location, bank1));
      break;
    case 2:
      do_insn("bcf", make_constant_list(location, bank0));
      r = do_insn("bsf", make_constant_list(location, bank1));
      break;
    case 3:
      do_insn("bsf", make_constant_list(location, bank0));
      r = do_insn("bsf", make_constant_list(location, bank1));
      break;
    default:
      assert(0);
      break;  
    }
  }

  return r;

}

/**************************************************************************/

static gpasmVal do_addcf(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  gpmessage(GPM_SPECIAL_MNEMONIC, NULL);

  r = do_insn("btfsc", make_constant_list(3, 0));
  r = do_insn("incf", parms);

  return r;
}

static gpasmVal do_adddcf(gpasmVal r,
		          char *name,
		          int arity,
		          struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 1));
  r = do_insn("incf", parms);

  return r;
}

static gpasmVal do_b(gpasmVal r,
		     char *name,
		     int arity,
		     struct pnode *parms)
{

  r = do_insn("goto", parms);

  return r;
}

static gpasmVal do_bc(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 0));
  r = do_insn("goto", parms);

  return r;
}

static gpasmVal do_bdc(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 1));
  r = do_insn("goto", parms);

  return r;
}

static gpasmVal do_bz(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 2));
  r = do_insn("goto", parms);

  return r;
}

static gpasmVal do_bnc(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{

  r = do_insn("btfss", make_constant_list(3, 0));
  r = do_insn("goto", parms);

  return r;
}

static gpasmVal do_bndc(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  r = do_insn("btfss", make_constant_list(3, 1));
  r = do_insn("goto", parms);

  return r;
}

static gpasmVal do_bnz(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{

  r = do_insn("btfss", make_constant_list(3, 2));
  r = do_insn("goto", parms);

  return r;
}

static gpasmVal do_clrc(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("bcf", make_constant_list(3, 0));

  return r;
}

static gpasmVal do_clrdc(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("bcf", make_constant_list(3, 1));

  return r;
}

static gpasmVal do_clrz(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("bcf", make_constant_list(3, 2));

  return r;
}

static gpasmVal do_lcall(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{
  int address = maybe_evaluate(HEAD(parms));
 
  set_page_bits(check_page(address));
  r = do_insn("call", parms);

  return r;
}

static gpasmVal do_lgoto(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{
  int address = maybe_evaluate(HEAD(parms));
 
  set_page_bits(check_page(address));
  r = do_insn("goto", parms);

  return r;
}

static gpasmVal do_movfw(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  r = do_insn("movf", add_symbol_constant(parms, 0));

  return r;
}

static gpasmVal do_negf(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  r = do_insn("comf", add_symbol_constant(parms, 1));
  r = do_insn("incf", parms);

  return r;
}

static gpasmVal do_setc(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }
  
  r = do_insn("bsf", make_constant_list(3, 0));

  return r;
}

static gpasmVal do_setdc(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("bsf", make_constant_list(3, 1));

  return r;
}

static gpasmVal do_setz(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("bsf", make_constant_list(3, 2));

  return r;
}

static gpasmVal do_skpc(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("btfss", make_constant_list(3, 0));

  return r;
}

static gpasmVal do_skpdc(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("btfss", make_constant_list(3, 1));

  return r;
}

static gpasmVal do_skpz(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("btfss", make_constant_list(3, 2));

  return r;
}

static gpasmVal do_skpnc(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("btfsc", make_constant_list(3, 0));

  return r;
}

static gpasmVal do_skpndc(gpasmVal r,
		          char *name,
		          int arity,
		          struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("btfsc", make_constant_list(3, 1));

  return r;
}

static gpasmVal do_skpnz(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  if (arity) {
    gperror(GPE_TOO_MANY_ARGU, NULL);
  }

  r = do_insn("btfsc", make_constant_list(3, 2));

  return r;
}

static gpasmVal do_subcf(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 0));
  r = do_insn("decf", parms);

  return r;
}

static gpasmVal do_subdcf(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 1));
  r = do_insn("decf", parms);

  return r;
}

static gpasmVal do_tstf(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  r = do_insn("movf", add_symbol_constant(parms, 1));

  return r;
}

/* PIC 12-bit and 14-bit "Special" instruction set */
struct insn special[] = {
  { "addcf",    0, (long int)do_addcf,  INSN_CLASS_FUNC,        0 },
  { "adddcf",   0, (long int)do_adddcf, INSN_CLASS_FUNC,        0 },
  { "b",        0, (long int)do_b,      INSN_CLASS_FUNC,        0 },
  { "bc",       0, (long int)do_bc,     INSN_CLASS_FUNC,        0 },
  { "bdc",      0, (long int)do_bdc,    INSN_CLASS_FUNC,        0 },
  { "bz",       0, (long int)do_bz,     INSN_CLASS_FUNC,        0 },
  { "bnc",      0, (long int)do_bnc,    INSN_CLASS_FUNC,        0 },
  { "bndc",     0, (long int)do_bndc,   INSN_CLASS_FUNC,        0 },
  { "bnz",      0, (long int)do_bnz,    INSN_CLASS_FUNC,        0 },
  { "clrc",     0, (long int)do_clrc,   INSN_CLASS_FUNC,        0 },
  { "clrdc",    0, (long int)do_clrdc,  INSN_CLASS_FUNC,        0 },
  { "clrz",     0, (long int)do_clrz,   INSN_CLASS_FUNC,        0 },
  { "lcall",    0, (long int)do_lcall,  INSN_CLASS_FUNC,        0 },
  { "lgoto",    0, (long int)do_lgoto,  INSN_CLASS_FUNC,        0 },
  { "movfw",    0, (long int)do_movfw,  INSN_CLASS_FUNC,        0 },
  { "negf",     0, (long int)do_negf,   INSN_CLASS_FUNC,        0 },
  { "setc",     0, (long int)do_setc,   INSN_CLASS_FUNC,        0 },
  { "setdc",    0, (long int)do_setdc,  INSN_CLASS_FUNC,        0 },
  { "setz",     0, (long int)do_setz,   INSN_CLASS_FUNC,        0 }, 
  { "skpc",     0, (long int)do_skpc,   INSN_CLASS_FUNC,        0 },
  { "skpdc",    0, (long int)do_skpdc,  INSN_CLASS_FUNC,        0 },
  { "skpz",     0, (long int)do_skpz,   INSN_CLASS_FUNC,        0 },
  { "skpnc",    0, (long int)do_skpnc,  INSN_CLASS_FUNC,        0 },
  { "skpndc",   0, (long int)do_skpndc, INSN_CLASS_FUNC,        0 },
  { "skpnz",    0, (long int)do_skpnz,  INSN_CLASS_FUNC,        0 },
  { "subcf",    0, (long int)do_subcf,  INSN_CLASS_FUNC,        0 },
  { "subdcf",   0, (long int)do_subdcf, INSN_CLASS_FUNC,        0 },
  { "tstf",     0, (long int)do_tstf,   INSN_CLASS_FUNC,        0 }

};

const int num_op_special = TABLE_SIZE(special);

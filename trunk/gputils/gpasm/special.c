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

#include "gpasm.h"
#include "gperror.h"
#include "gpopcode.h"
#include "special.h"
#include "directive.h"

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
  int location, page0, page1;

  if (state.device.core_size == CORE_12BIT_MASK) {
    location = 0x3;
    page0    = 5;
    page1    = 6;
  } else {
    location = 0xA;
    page0    = 3;
    page1    = 4;
  }

  switch(page) {
  case 0:
    r = do_insn("bcf", make_constant_list(location, page0));
    r = do_insn("bcf", make_constant_list(location, page1));
    break;
  case 1:
    r = do_insn("bsf", make_constant_list(location, page0));
    r = do_insn("bcf", make_constant_list(location, page1));
    break;
  case 2:
    r = do_insn("bcf", make_constant_list(location, page0));
    r = do_insn("bsf", make_constant_list(location, page1));
    break;
  case 3:
    r = do_insn("bsf", make_constant_list(location, page0));
    r = do_insn("bsf", make_constant_list(location, page1));
    break;
  default:
    r = 0;
    gperror(GPW_UNKNOWN, "Unknown page.");
    break;  
  }

  return r;

}

/**************************************************************************/

extern gpasmVal do_addcf(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  gpmessage(GPM_SPECIAL_MNEMONIC, NULL);

  r = do_insn("btfsc", make_constant_list(3, 0));
  r = do_insn("incf", parms);

  return r;
}

extern gpasmVal do_adddcf(gpasmVal r,
		          char *name,
		          int arity,
		          struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 1));
  r = do_insn("incf", parms);

  return r;
}

extern gpasmVal do_b(gpasmVal r,
		     char *name,
		     int arity,
		     struct pnode *parms)
{

  r = do_insn("goto", parms);

  return r;
}

extern gpasmVal do_bc(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 0));
  r = do_insn("goto", parms);

  return r;
}

extern gpasmVal do_bdc(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 1));
  r = do_insn("goto", parms);

  return r;
}

extern gpasmVal do_bz(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 2));
  r = do_insn("goto", parms);

  return r;
}

extern gpasmVal do_bnc(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{

  r = do_insn("btfss", make_constant_list(3, 0));
  r = do_insn("goto", parms);

  return r;
}

extern gpasmVal do_bndc(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  r = do_insn("btfss", make_constant_list(3, 1));
  r = do_insn("goto", parms);

  return r;
}

extern gpasmVal do_bnz(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{

  r = do_insn("btfss", make_constant_list(3, 2));
  r = do_insn("goto", parms);

  return r;
}

extern gpasmVal do_clrc(gpasmVal r,
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

extern gpasmVal do_clrdc(gpasmVal r,
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

extern gpasmVal do_clrz(gpasmVal r,
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

extern gpasmVal do_lcall(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  r = set_page_bits(check_page(parms));
  r = do_insn("call", parms);

  return r;
}

extern gpasmVal do_lgoto(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  r = set_page_bits(check_page(parms));
  r = do_insn("goto", parms);

  return r;
}

extern gpasmVal do_movfw(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  r = do_insn("movf", add_symbol_constant(parms, 0));

  return r;
}

extern gpasmVal do_negf(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  r = do_insn("comf", add_symbol_constant(parms, 1));
  r = do_insn("incf", parms);

  return r;
}

extern gpasmVal do_setc(gpasmVal r,
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

extern gpasmVal do_setdc(gpasmVal r,
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

extern gpasmVal do_setz(gpasmVal r,
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

extern gpasmVal do_skpc(gpasmVal r,
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

extern gpasmVal do_skpdc(gpasmVal r,
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

extern gpasmVal do_skpz(gpasmVal r,
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

extern gpasmVal do_skpnc(gpasmVal r,
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

extern gpasmVal do_skpndc(gpasmVal r,
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

extern gpasmVal do_skpnz(gpasmVal r,
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

extern gpasmVal do_subcf(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 0));
  r = do_insn("decf", parms);

  return r;
}

extern gpasmVal do_subdcf(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{

  r = do_insn("btfsc", make_constant_list(3, 1));
  r = do_insn("decf", parms);

  return r;
}

extern gpasmVal do_tstf(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{

  r = do_insn("movf", add_symbol_constant(parms, 1));

  return r;
}

/* PIC 12-bit and 14-bit "Special" instruction set */
struct insn special[] = {
  { "addcf",    (long int)do_addcf,  INSN_CLASS_FUNC,        0 },
  { "adddcf",   (long int)do_adddcf, INSN_CLASS_FUNC,        0 },
  { "b",        (long int)do_b,      INSN_CLASS_FUNC,        0 },
  { "bc",       (long int)do_bc,     INSN_CLASS_FUNC,        0 },
  { "bdc",      (long int)do_bdc,    INSN_CLASS_FUNC,        0 },
  { "bz",       (long int)do_bz,     INSN_CLASS_FUNC,        0 },
  { "bnc",      (long int)do_bnc,    INSN_CLASS_FUNC,        0 },
  { "bndc",     (long int)do_bndc,   INSN_CLASS_FUNC,        0 },
  { "bnz",      (long int)do_bnz,    INSN_CLASS_FUNC,        0 },
  { "clrc",     (long int)do_clrc,   INSN_CLASS_FUNC,        0 },
  { "clrdc",    (long int)do_clrdc,  INSN_CLASS_FUNC,        0 },
  { "clrz",     (long int)do_clrz,   INSN_CLASS_FUNC,        0 },
  { "lcall",    (long int)do_lcall,  INSN_CLASS_FUNC,        0 },
  { "lgoto",    (long int)do_lgoto,  INSN_CLASS_FUNC,        0 },
  { "movfw",    (long int)do_movfw,  INSN_CLASS_FUNC,        0 },
  { "negf",     (long int)do_negf,   INSN_CLASS_FUNC,        0 },
  { "setc",     (long int)do_setc,   INSN_CLASS_FUNC,        0 },
  { "setdc",    (long int)do_setdc,  INSN_CLASS_FUNC,        0 },
  { "setz",     (long int)do_setz,   INSN_CLASS_FUNC,        0 }, 
  { "skpc",     (long int)do_skpc,   INSN_CLASS_FUNC,        0 },
  { "skpdc",    (long int)do_skpdc,  INSN_CLASS_FUNC,        0 },
  { "skpz",     (long int)do_skpz,   INSN_CLASS_FUNC,        0 },
  { "skpnc",    (long int)do_skpnc,  INSN_CLASS_FUNC,        0 },
  { "skpndc",   (long int)do_skpndc, INSN_CLASS_FUNC,        0 },
  { "skpnz",    (long int)do_skpnz,  INSN_CLASS_FUNC,        0 },
  { "subcf",    (long int)do_subcf,  INSN_CLASS_FUNC,        0 },
  { "subdcf",   (long int)do_subdcf, INSN_CLASS_FUNC,        0 },
  { "tstf",     (long int)do_tstf,   INSN_CLASS_FUNC,        0 }

};

const int num_op_special = TABLE_SIZE(special);

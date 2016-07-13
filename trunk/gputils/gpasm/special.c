/* Implements special instruction mnemonics
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2016 Molnar Karoly <molnarkaroly@users.sf.net>

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
#include "gpmsg.h"
#include "special.h"
#include "directive.h"
#include "evaluate.h"

/*------------------------------------------------------------------------------------------------*/

static pnode_t *
_make_constant_list(int Value1, int Value2)
{
  return mk_list(mk_constant(Value1), mk_list(mk_constant(Value2), NULL));
}

/*------------------------------------------------------------------------------------------------*/

static pnode_t *
_add_symbol_constant(pnode_t *Parms, int Value)
{
  return mk_list(PnListHead(Parms), mk_list(mk_constant(Value), NULL));
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_addcf(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  gpmsg_vmessage(GPM_SPECIAL_MNEMONIC, NULL);

  do_insn("btfsc", _make_constant_list(3, 0));
  do_insn("incf", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_adddcf(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("btfsc", _make_constant_list(3, 1));
  do_insn("incf", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_b(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("goto", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("btfsc", _make_constant_list(3, 0));
  do_insn("goto", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bdc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("btfsc", _make_constant_list(3, 1));
  do_insn("goto", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bz(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("btfsc", _make_constant_list(3, 2));
  do_insn("goto", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bnc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("btfss", _make_constant_list(3, 0));
  do_insn("goto", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bndc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("btfss", _make_constant_list(3, 1));
  do_insn("goto", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bnz(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("btfss", _make_constant_list(3, 2));
  do_insn("goto", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_clrc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bcf", _make_constant_list(3, 0));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_clrdc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bcf", _make_constant_list(3, 1));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_clrz(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bcf", _make_constant_list(3, 2));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_lcall(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("pagesel", Parms);
  do_insn("call", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_lgoto(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("pagesel", Parms);
  do_insn("goto", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_movfw(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (eval_enforce_arity(Arity, 1)) {
    do_insn("movf", _add_symbol_constant(Parms, 0));
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_negf(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if ((Arity == 1) || (Arity == 2)) {
    do_insn("comf", _add_symbol_constant(Parms, 1));
    do_insn("incf", Parms);
  }
  else {
    eval_enforce_arity(Arity, 2);
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_setc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bsf", _make_constant_list(3, 0));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_setdc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bsf", _make_constant_list(3, 1));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_setz(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("bsf", _make_constant_list(3, 2));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfss", _make_constant_list(3, 0));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpdc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfss", _make_constant_list(3, 1));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpz(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfss", _make_constant_list(3, 2));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpnc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfsc", _make_constant_list(3, 0));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpndc(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfsc", _make_constant_list(3, 1));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_skpnz(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (Arity != 0) {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  do_insn("btfsc", _make_constant_list(3, 2));

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_subcf(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("btfsc", _make_constant_list(3, 0));
  do_insn("decf", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_subdcf(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;
  (void)Arity;

  do_insn("btfsc", _make_constant_list(3, 1));
  do_insn("decf", Parms);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_tstf(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  (void)Name;

  if (eval_enforce_arity(Arity, 1)) {
    do_insn("movf", _add_symbol_constant(Parms, 1));
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_mode(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  pnode_t *val;

  (void)Name;

  if (eval_enforce_arity(Arity, 1)) {
    val = PnListHead(Parms);

    if (PnIsConstant(val) && (PnConstant(val) > 0x1f)) {
      gpmsg_vwarning(GPW_OUT_OF_RANGE, NULL);
      PnConstant(val) &= 0x1f;
    }

    do_insn("movlw", Parms);
    do_insn("movwm", NULL);
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/* PIC 12-bit and 14-bit "Special" instruction set. */
const insn_t special[] = {
  { "addcf",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_addcf  },
  { "adddcf", 0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_adddcf },
  { "b",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_b      },
  { "bc",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bc     },
  { "bdc",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bdc    },
  { "bz",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bz     },
  { "bnc",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bnc    },
  { "bndc",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bndc   },
  { "bnz",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_bnz    },
  { "clrc",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_clrc   },
  { "clrdc",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_clrdc  },
  { "clrz",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_clrz   },
  { "lcall",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_lcall  },
  { "lgoto",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_lgoto  },
  { "movfw",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_movfw  },
  { "negf",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_negf   },
  { "setc",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_setc   },
  { "setdc",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_setdc  },
  { "setz",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_setz   },
  { "skpc",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpc   },
  { "skpdc",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpdc  },
  { "skpz",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpz   },
  { "skpnc",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpnc  },
  { "skpndc", 0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpndc },
  { "skpnz",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_skpnz  },
  { "subcf",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_subcf  },
  { "subdcf", 0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_subdcf },
  { "tstf",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_tstf   }
};

const int num_op_special = TABLE_SIZE(special);

const insn_t op_sx_mode = { 
  "mode", 0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0, _do_mode
};

/* Implements directives, pseudo-ops and processor opcodes
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin

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
#include "directive.h"
#include "evaluate.h"
#include "parse.h"
#include "preprocess.h"
#include "processor.h"
#include "lst.h"
#include "macro.h"
#include "coff.h"
#include "gperror.h"
#include "special.h"
#include "gpcfg.h"

/* Forward declarations */

#define ATTRIB_COND                     1

#define IFLAG_NONE                      0

#define IFLAG_DEST_NONE                 0

#define IFLAG_DEST_DEF                  0x01                        /* 0b00000001 */
#define IFLAG_DEST_DEF_WREG             (IFLAG_DEST_DEF + (1 << 2)) /* 0b00000101 */
#define IFLAG_DEST_DEF_FILE             (IFLAG_DEST_DEF + (2 << 2)) /* 0b00001001 */

#define IFLAG_DEST_SEL                  0x03                        /* 0b00000011 */
#define IFLAG_DEST_SEL_WREG             (IFLAG_DEST_SEL + (1 << 2)) /* 0b00000111 */
#define IFLAG_DEST_SEL_FILE             (IFLAG_DEST_SEL + (2 << 2)) /* 0b00001011 */

#define IFLAG_BANK_NONE                 0

#define IFLAG_BANK_DEF                  0x01                        /* 0b00000001 */
#define IFLAG_BANK_DEF_ACCESS           (IFLAG_BANK_DEF + (1 << 2)) /* 0b00000101 */
#define IFLAG_BANK_DEF_BANK             (IFLAG_BANK_DEF + (2 << 2)) /* 0b00001001 */

#define IFLAG_BANK_SEL                  0x03                        /* 0b00000011 */
#define IFLAG_BANK_SEL_ACCESS           (IFLAG_BANK_SEL + (1 << 2)) /* 0b00000111 */
#define IFLAG_BANK_SEL_BANK             (IFLAG_BANK_SEL + (2 << 2)) /* 0b00001011 */

typedef union {
  unsigned int u;

  struct __attribute__ ((packed)) {
    unsigned dest_mode  : 2;    /* IFLAG_DEST_DEF, IFLAG_DEST_SEL */
    unsigned dest_target: 2;    /* WREG, FILE */
    unsigned            : 4;

    unsigned bank_mode  : 2;    /* IFLAG_BANK_DEF, IFLAG_BANK_SEL */
    unsigned bank_target: 2;    /* ACCESS, BANK */
    unsigned isAccessSFR: 1;
    unsigned isAccessGPR: 1;
    unsigned isAccess   : 1;
    unsigned isReloc    : 1;
  };

  struct __attribute__ ((packed)) {
    unsigned dest  : 4;         /* IFLAG_DEST_DEF_..., IFLAG_DEST_SEL_... */
    unsigned       : 4;

    unsigned bank  : 4;         /* IFLAG_BANK_DEF_..., IFLAG_BANK_SEL_... */
    unsigned access: 3;
    unsigned reloc : 1;
  };
} insn_flags_t;

extern pnode_t *mk_constant(int Value);

static gp_boolean prev_btfsx = false;

/*------------------------------------------------------------------------------------------------*/

static uint16_t
_check_write(uint16_t Value)
{
  int      addr_digits;
  uint16_t insn;
  int      org;

  if (state.mode == MODE_RELOCATABLE) {
    if (state.obj.section == NULL) {
      gperror_verror(GPE_WRONG_SECTION, NULL);
      return Value;
    }
    /* Don't do program memory checks for data memory. */
    if (IS_RAM_ORG) {
      return Value;
    }
  }

  addr_digits = state.device.class->addr_digits;

  if (IS_PIC16_CORE && (state.byte_addr > 0x1ffff)) {
    gperror_verror(GPE_ADDROVF, "Address{0x%0*X}", addr_digits, state.byte_addr);
  }
  else if ((!IS_PIC16E_CORE) && ((state.byte_addr & 0x1ffff) == 0) && ((int)state.byte_addr > 0)) {
    /* We cast state.byte_addr to signed int on purpose to repeat a bug from
       MPASM 5.34 and pass tb.asm testcase. */
    gperror_error(GPE_ADDROVF, "Address wrapped around 0.");
  }

  org = gp_processor_byte_to_real(state.processor, state.byte_addr);

  if (state.mpasm_compatible) {
    /* MPASM(X) compatible mode. */
    if (Value > state.device.class->core_mask) {
      gperror_vmessage(GPM_RANGE, NULL, Value);
      Value &= state.device.class->core_mask;
    }

    if ((state.num.errors == 0) &&
        state.device.class->i_memory_get(state.i_memory, state.byte_addr, &insn, NULL, NULL)) {
      gperror_verror(GPE_ADDROVR, NULL, org);
    }
  }
  else {
    /* GPASM compatible mode. */
    gp_boolean is_config = (gp_processor_is_config_org(state.processor, org) >= 0) ? true : false;

    if (Value > state.device.class->core_mask) {
    /* The size of the config words may be differ the size of the program words. */
      if (!is_config) {
        gperror_vmessage(GPM_RANGE, NULL, Value);
        Value &= state.device.class->core_mask;
      }
      else if (Value > state.device.class->config_mask) {
        gperror_vmessage(GPM_RANGE, NULL, Value);
        Value &= state.device.class->config_mask;
      }
    }

    if (state.num.errors == 0) {
      if (is_config && (state.device.class->config_mask <= 0xFF)) {
        uint8_t byte;

        if (b_memory_get(state.i_memory, state.byte_addr, &byte, NULL, NULL)) {
          gperror_verror(GPE_ADDROVR, NULL, org);
        }
      }
      else {
        if (state.device.class->i_memory_get(state.i_memory, state.byte_addr, &insn, NULL, NULL)) {
          gperror_verror(GPE_ADDROVR, NULL, org);
        }
      }
    }
  }

  if (state.maxrom >= 0) {
    if (org > state.maxrom) {
      gperror_vwarning(GPW_EXCEED_ROM, "Address{0x%0*X} > MAXROM{0x%0*X}",
                       addr_digits, org,
                       addr_digits, state.maxrom);
    }
    else {
      /* check if current org is within a bad address range */
      range_pair_t *cur_badrom;

      for (cur_badrom = state.badrom; cur_badrom != NULL; cur_badrom = cur_badrom->next) {
        if ((org >= cur_badrom->start) && (org <= cur_badrom->end)) {
          gperror_vwarning(GPW_EXCEED_ROM, "BADROM_START{0x%0*X} <= Address{0x%0*X} <= BADROM_END{0x%0*X}",
                           addr_digits, cur_badrom->start,
                           addr_digits, org,
                           addr_digits, cur_badrom->end);
          break;
        }
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_check_processor_select(const char *Name)
{
  if (state.processor == NULL) {
    if ((!state.mpasm_compatible) && (state.strict_level > 0)) {
      if (state.strict_level == 2) {
        gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
        return true;
      }
      else {
        gperror_vwarning(GPW_UNDEF_PROC, "\"%s\"", Name);
      }
    }
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

/* Write a word into the memory image at the current location. */

static void
_emit(uint16_t Value, const char *Name)
{
  /* only write the program data to memory on the second pass */
  if (state.pass == 2) {
    Value = _check_write(Value);
    state.device.class->i_memory_put(state.i_memory, state.byte_addr, Value, Name, NULL);
  }

  state.byte_addr += 2;
}

/*------------------------------------------------------------------------------------------------*/

static void
_emit_byte(uint16_t Value, const char *Name)
{
  int           addr_digits;
  uint8_t       byte;
  uint16_t      word;
  unsigned int  org;
  range_pair_t *cur_badrom;

  if (state.pass == 2) {
    addr_digits = state.device.class->addr_digits;

    if ((state.mode == MODE_RELOCATABLE) && (state.obj.section == NULL)) {
      gperror_verror(GPE_WRONG_SECTION, NULL);
    }

    if (!IS_RAM_ORG) {
      if (IS_PIC16_CORE && (state.byte_addr > 0x1ffff)) {
        gperror_verror(GPE_ADDROVF, "Address{0x%0*X} > 0x1ffff",
                       state.device.class->addr_digits, state.byte_addr);
      }
      else if ((!IS_PIC16E_CORE) && ((state.byte_addr & 0x1ffff) == 0) && ((int)state.byte_addr > 0)) {
        gperror_error(GPE_ADDROVF, "Address wrapped around 0.");
      }

      if (Value > state.device.class->core_mask) {
        word = Value & state.device.class->core_mask;

        gperror_vwarning(GPW_RANGE, "%i (%#x) => %i (%#x)", Value, Value, word, word);
        Value = word;
      }

      org = gp_processor_byte_to_real(state.processor, state.byte_addr);

      if ((state.num.errors == 0) &&
          b_memory_get(state.i_memory, state.byte_addr, &byte, NULL, NULL)) {
        gperror_verror(GPE_ADDROVR, NULL, org);
      }

      if (state.maxrom >= 0) {
        if (org > state.maxrom) {
          gperror_vwarning(GPW_EXCEED_ROM, "Address{0x%0*X} > MAXROM{0x%0*X}",
                           addr_digits, org,
                           addr_digits, state.maxrom);
        }
        else {
          /* check if current org is within a bad address range */
          for (cur_badrom = state.badrom; cur_badrom != NULL; cur_badrom = cur_badrom->next) {
            if ((org >= cur_badrom->start) && (org <= cur_badrom->end)) {
              gperror_vwarning(GPW_EXCEED_ROM, "BADROM_START{0x%0*X} <= Address{0x%0*X} <= BADROM_END{0x%0*X}",
                               addr_digits, cur_badrom->start,
                               addr_digits, org,
                               addr_digits, cur_badrom->end);
              break;
            }
          }
        }
      }
    }

    b_memory_put(state.i_memory, state.byte_addr, (uint8_t)Value, Name, NULL);
  }

  ++state.byte_addr;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_off_or_on(pnode_t *Pnode)
{
  int had_error = false;
  int ret = false;

  if (!PnIsSymbol(Pnode)) {
    had_error = true;
  }
  else if (strcasecmp(PnSymbol(Pnode), "off") == 0) {
    ret = false;
  }
  else if (strcasecmp(PnSymbol(Pnode), "on") == 0) {
    ret = true;
  }
  else {
    had_error = true;
  }

  if (had_error) {
    gperror_error(GPE_EXPECTED, "Expected \"ON\" or \"OFF\".");
  }

  return ret;
}

/*------------------------------------------------------------------------------------------------*/

/* Convert an expression list which may consist of strings, constants, labels,
 * etc. into instruction memory.
 * This is called by the da,data, and dw directives.
 *
 * pnode *L                -  A pointer to a doubly-linked list containing all of the
 *                            expressions.
 * unsigned int char_shift -  Character width in bits (8 but 7 for da on 14-bit PIC).
 */

static unsigned int
_emit_data(pnode_t *List, unsigned int Char_shift, const char *Name)
{
  const pnode_t *p;
  const char    *pc;
  unsigned int   begin_org;
  int            value;
  uint16_t       word;
  unsigned int   n;

  for (begin_org = state.byte_addr; List != NULL; List = PnListTail(List)) {
    p = PnListHead(List);

    if (PnIsString(p)) {
      pc = PnString(p);

      if (IS_PIC16E_CORE && !(SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
        /* Special case of PIC16E strings in code. */
        for (n = 0; *pc != '\0'; ++n) {
          pc = convert_escape_chars(pc, &value);
          _emit_byte(value, Name);
        }
        /* Pad using zero if 16-bit numbers. */
        if (n & 1) {
          _emit_byte(0, Name);
        }
      }
      else {
        while (*pc != '\0') {
          pc = convert_escape_chars(pc, &value);
          value &= 0xff;
          /* If idata or packed and not db or de, emit one character per word. */
          if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
            word = value;
          }
          else {
            word = value << Char_shift;

            if (*pc != '\0') {
              pc = convert_escape_chars(pc, &value);
              word |= value & 0xff;
            }
          }
          _emit(word, Name);
        }
        /* For data and packed emit a terminating nul for strings. */
        if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
          _emit('\0', Name);
        }
      }
    }
    else if (state.device.class->core_mask > 0xff) {
      word = eval_reloc_evaluate(p, RELOCT_ALL, NULL, NULL);
      _emit(word, Name);
    }
    else {
      /* FIXME: This case is for EEPROM8. Do we need the RELOCT_LOW? */
      word = eval_reloc_evaluate(p, RELOCT_LOW, NULL, NULL);
      _emit_byte(word, Name);
    }
  }

  return (state.byte_addr - begin_org);
}

/*------------------------------------------------------------------------------------------------*/

/* Do the work for beginning a conditional assembly block. Leave it disabled by default.
   This is used by do_if, do_ifdef and do_ifndef. */

static void
_enter_if(void)
{
  amode_t *new;

  new = GP_Malloc(sizeof(*new));
  new->mode  = IN_THEN;
  new->upper = state.astack;

  if (state.astack == NULL) {
    new->upper_enabled = true;
  }
  else {
    new->upper_enabled = (state.astack->enabled && state.astack->upper_enabled);
  }

  new->enabled             = false;     /* Only the default. */
  new->before_else_enabled = false;
  state.astack = new;
}

/*------------------------------------------------------------------------------------------------*/

static void
_enter_elif(void)
{
  if (state.astack == NULL) {
    gperror_verror(GPE_ILLEGAL_COND, NULL);
  }
  else if ((state.astack->mode != IN_THEN) && (state.astack->mode != IN_ELIF)) {
    gperror_verror(GPE_ILLEGAL_COND, NULL);
  }

  state.astack->mode = IN_ELIF;
}

/*------------------------------------------------------------------------------------------------*/

/* Checking that a macro definition's parameters are correct. */

static gp_boolean
_macro_parms_simple(pnode_t *Parms)
{
  while (Parms != NULL) {
    if (!PnIsSymbol(PnListHead(Parms))) {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
      return false;
    }
    Parms = PnListTail(Parms);
  }
  return true;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_macro_parm_unique(pnode_t *Macro, pnode_t *List)
{
  char buf[BUFSIZ];

  while (List != NULL) {
    if (STRCMP(PnSymbol(Macro), PnSymbol(PnListHead(List))) == 0) {
      snprintf(buf, sizeof(buf), "Duplicate macro parameter (%s).", PnSymbol(PnListHead(List)));
      gperror_error(GPE_UNKNOWN, buf);
      return false;
    }
    List = PnListTail(List);
  }
  return true;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_macro_parms_ok(pnode_t *Parms)
{
  /* Check if all params are symbols. */
  if (!_macro_parms_simple(Parms)) {
    return false;
  }

  /* Check if params are unique. */
  while (Parms != NULL) {
    if (!_macro_parm_unique(PnListHead(Parms), PnListTail(Parms))) {
      return false;
    }
    Parms = PnListTail(Parms);
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_access_ovr(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else {
    switch (Arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".access_ovr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_ACCESS | STYP_BSS | STYP_OVERLAY;
      break;

    case 1:
      /* new absolute section */
      p = PnListHead(Parms);
      gp_strncpy(state.obj.new_sect_name, ".access_ovr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = eval_maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_ACCESS | STYP_BSS | STYP_OVERLAY | STYP_ABS;
      break;

    default:
      eval_enforce_arity(Arity, 1);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_badram(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  int            maxram;
  int            start;
  int            end;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;

  if (Parms == NULL) {
    gperror_verror(GPE_MISSING_ARGU, NULL);
  }
  else {
    maxram = state.maxram;
    if (maxram >= MAX_RAM) {
      maxram = MAX_RAM - 1;
    }

    for (; Parms != NULL; Parms = PnListTail(Parms)) {
      p = PnListHead(Parms);

      if (PnIsBinOp(p) && (PnBinOpOp(p) == '-')) {
        if (eval_can_evaluate(PnBinOpP0(p)) && eval_can_evaluate(PnBinOpP1(p))) {
          start = eval_evaluate(PnBinOpP0(p));
          end = eval_evaluate(PnBinOpP1(p));

          if (end < start) {
            gperror_vwarning(GPW_INVALID_RAM, "Start{0x%0*X} > End{0x%0*X}",
                             state.device.class->addr_digits, start,
                             state.device.class->addr_digits, end);
          }
          else if (start < 0) {
            gperror_vwarning(GPW_INVALID_RAM, "Start{%i} < 0", start);
          }
          else if (end > maxram) {
            gperror_vwarning(GPW_INVALID_RAM, "End{0x%0*X} > MAXRAM{0x%0*X}",
                             state.device.class->addr_digits, end,
                             state.device.class->addr_digits, maxram);
          }
          else {
            for (; start <= end; start++) {
              state.badram[start] = 1;
            }
          }
        }
      }
      else {
        if (eval_can_evaluate(p)) {
          int loc;

          loc = eval_evaluate(p);

          if ((loc < 0) || (loc > maxram)) {
            gperror_vwarning(GPW_INVALID_RAM, "Address{%#x} > MAXRAM{%#x}", loc, maxram);
          }
          else {
            state.badram[loc] = 1;
          }
        }
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_badrom(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;

  /* FIXME: implement this directive */
  gperror_warning(GPW_UNKNOWN, "gpasm doesn't support the badrom directive yet.");

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bankisel(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  int            num_reloc;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (((!IS_PIC14_CORE) && (!IS_PIC14E_CORE) && (!IS_PIC14EX_CORE) && (!IS_PIC16_CORE)) ||
      (state.processor->num_banks == 1)) {
    state.lst.line.linetype = LTY_NONE;
    gperror_vmessage(GPM_EXTPAGE, NULL);
    return Value;
  }

  if (prev_btfsx) {
    gperror_vwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Bankisel");
  }

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (state.mode == MODE_ABSOLUTE) {
      state.byte_addr += gp_processor_set_ibank(state.device.class,
                                                state.processor->num_banks,
                                                gp_processor_check_ibank(state.device.class, eval_maybe_evaluate(p)),
                                                state.i_memory, state.byte_addr);
    }
    else {
      num_reloc = eval_count_reloc(p);

      if (num_reloc == 0) {
        /* it is an absolute address, generate the bankisel but no relocation */
        state.byte_addr += gp_processor_set_ibank(state.device.class,
                                                  state.processor->num_banks,
                                                  gp_processor_check_ibank(state.device.class, eval_maybe_evaluate(p)),
                                                  state.i_memory, state.byte_addr);
      }
      else if (num_reloc != 1) {
        gperror_verror(GPE_UNRESOLVABLE, "\"%s\"", Name);
      }
      else {
        eval_reloc_evaluate(p, RELOCT_IBANKSEL, NULL, NULL);

        if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
          unsigned int mask;

          for (mask = 0x100; mask < (state.processor->num_banks << PIC14_BANK_SHIFT); mask <<= 1) {
            _emit(0, Name);
          }
        }
        else {
          _emit(0, Name);
        }
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_eval_update_reloc_value(const pnode_t *Pnode, uint16_t Type)
{
  gpasmVal   value;
  gp_boolean is_reloc;
  gpasmVal   reloc_val;

  value = eval_reloc_evaluate(Pnode, Type, &is_reloc, &reloc_val);

  if ((value == 0) && is_reloc) {
    value = reloc_val;
  }

  return value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_banksel(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  int            address;
  int            bank;
  int            num_reloc;
  gp_boolean     bank_var;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (state.processor->num_banks == 1) {
    state.lst.line.linetype = LTY_NONE;
    gperror_vmessage(GPM_EXTPAGE, NULL);
    /* do nothing */
    if (!state.mpasm_compatible) {
      set_global(GLOBAL_ACT_BANK_ADDR, 0, LFT_TEMPORARY, VAL_CONSTANT, true);
    }

    return Value;
  }

  if (prev_btfsx) {
    gperror_vwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Banksel");
  }

  bank_var = false;
  bank     = -1;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (state.mode == MODE_ABSOLUTE) {
      address = eval_maybe_evaluate(p);
      bank    = gp_processor_check_bank(state.device.class, address);
      state.byte_addr += gp_processor_set_bank(state.device.class,
                                               state.processor->num_banks,
                                               bank,
                                               state.i_memory,
                                               state.byte_addr);
      bank_var = true;
    }
    else {
      /* state.mode == MODE_RELOCATABLE */
      num_reloc = eval_count_reloc(p);

      if (num_reloc == 0) {
        /* It is an absolute address, generate the banksel but no relocation. */
        address = eval_maybe_evaluate(p);
        bank    = gp_processor_check_bank(state.device.class, address);
        state.byte_addr += gp_processor_set_bank(state.device.class,
                                                 state.processor->num_banks,
                                                 bank,
                                                 state.i_memory,
                                                 state.byte_addr);
        bank_var = true;
      }
      else if (num_reloc != 1) {
        gperror_verror(GPE_UNRESOLVABLE, "\"%s\"", Name);
      }
      else if (IS_PIC12E_CORE || IS_PIC12I_CORE) {
        address  = _eval_update_reloc_value(p, RELOCT_MOVLB);
        bank     = (address >= 0) ? gp_processor_check_bank(state.device.class, address) : -1;
        bank_var = true;
        _emit(PIC12E_INSN_MOVLB, Name);
      }
      else if (IS_PIC14E_CORE) {
        address  = _eval_update_reloc_value(p, RELOCT_MOVLB);
        bank     = (address >= 0) ? gp_processor_check_bank(state.device.class, address) : -1;
        bank_var = true;
        _emit(PIC14E_INSN_MOVLB, Name);
      }
      else if (IS_PIC14EX_CORE) {
        address  = _eval_update_reloc_value(p, RELOCT_MOVLB);
        bank     = (address >= 0) ? gp_processor_check_bank(state.device.class, address) : -1;
        bank_var = true;
        _emit(PIC14EX_INSN_MOVLB, Name);
      }
      else if (IS_PIC16_CORE) {
        address  = _eval_update_reloc_value(p, RELOCT_BANKSEL);
        bank     = (address >= 0) ? gp_processor_check_bank(state.device.class, address) : -1;
        bank_var = true;
        _emit(0x0000, Name);
      }
      else if (IS_PIC16E_CORE) {
        address  = _eval_update_reloc_value(p, RELOCT_BANKSEL);
        bank     = (address >= 0) ? gp_processor_check_bank(state.device.class, address) : -1;
        bank_var = true;
        _emit(PIC16E_INSN_MOVLB, Name);
      }
      else {
        switch (state.processor->num_banks) {
        case 2:
          address  = _eval_update_reloc_value(p, RELOCT_BANKSEL);
          bank     = (address >= 0) ? gp_processor_check_bank(state.device.class, address) : -1;
          bank_var = true;
          _emit(0, Name);
          break;

        case 4:
          address  = _eval_update_reloc_value(p, RELOCT_BANKSEL);
          bank     = (address >= 0) ? gp_processor_check_bank(state.device.class, address) : -1;
          bank_var = true;
          _emit(0, Name);
          _emit(0, Name);
          break;
        }
      }
    }
  }

  if ((!state.mpasm_compatible) && bank_var) {
    if (bank >= 0) {
      address = gp_processor_bank_num_to_addr(state.processor, bank);
      set_global(GLOBAL_ACT_BANK_ADDR, address, LFT_TEMPORARY, VAL_CONSTANT, true);
    }
    else {
      set_global(GLOBAL_ACT_PAGE_ADDR, GLOBAL_ACT_PAGE_INV, LFT_TEMPORARY, VAL_CONSTANT, true);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_code(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else {
    switch (Arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".code", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_TEXT;
      break;

    case 1:
      /* new absolute section */
      p = PnListHead(Parms);
      gp_strncpy(state.obj.new_sect_name, ".code", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = gp_processor_org_to_byte(state.device.class, eval_maybe_evaluate(p));
      state.obj.new_sect_flags = STYP_TEXT | STYP_ABS;
      break;

    default:
      eval_enforce_arity(Arity, 1);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_code_pack(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (!IS_PIC16E_CORE) {
    gperror_error(GPE_UNKNOWN, "code_pack is only supported on 16bit cores.");
  }
  else {
    state.lst.line.linetype = LTY_SEC;
    state.next_state = STATE_SECTION;

    if (state.mode == MODE_ABSOLUTE) {
      gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
    }
    else {
      switch (Arity) {
      case 0:
        /* new relocatable section */
        gp_strncpy(state.obj.new_sect_name, ".code", sizeof(state.obj.new_sect_name));
        state.obj.new_sect_addr  = 0;
        state.obj.new_sect_flags = STYP_TEXT | STYP_BPACK;
        break;

      case 1:
        /* new absolute section */
        p = PnListHead(Parms);
        gp_strncpy(state.obj.new_sect_name, ".code", sizeof(state.obj.new_sect_name));
        state.obj.new_sect_addr  = gp_processor_org_to_byte(state.device.class, eval_maybe_evaluate(p));
        state.obj.new_sect_flags = STYP_TEXT | STYP_BPACK | STYP_ABS;
        break;

      default:
        eval_enforce_arity(Arity, 1);
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_constant(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  gp_boolean     first;
  const char    *lhs;
  gpasmVal       val;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_SET4;

  first = true;
  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (PnIsBinOp(p) && (PnBinOpOp(p) == '=')) {
      if (eval_enforce_simple(PnBinOpP0(p))) {
        /* fetch the symbol */
        lhs = PnSymbol(PnBinOpP0(p));
        /* constants must be assigned a value at declaration */

        val = eval_maybe_evaluate(PnBinOpP1(p));
        /* put the symbol and value in the table*/
        set_global(lhs, val, LFT_PERMANENT, VAL_CONSTANT, false);

        if (first) {
          Value = val;
          first = false;
        }
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 *
 * configuration memory
 *
 * In addition to memory for storing instructions, each pic has memory for
 * storing configuration data (e.g. code protection, wdt enable, etc.). Each
 * family of the pic microcontrollers treats this memory slightly different.
 *
 * _do_config() - Called by the parser when a __CONFIG assembler directive
 *                is encountered.
 *
 * _do_gpasm_config() - Called by the parser to process MPASM(X) style CONFIG xx=yy
 *                      directives for all devices.
 */

static gp_boolean config_us_used    = false;
static gp_boolean config_mpasm_used = false;

/*
 * Creates the configuration or device id COFF section for do_config and
 * do_gpasm_config. Returns true when a section is created.
 */

static MemBlock_t *
_find_conf_sec_mem(unsigned int Ca)
{
  const conf_mem_block_t *p;
  int                     ba = (IS_PIC16_CORE || IS_PIC16E_CORE) ? Ca : (Ca - (Ca & 1));

  for (p = state.conf_sec_mem; p != NULL; p = p->next) {
    if (p->addr == ba) {
      return p->m;
    }
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

static MemBlock_t *
_add_conf_sec_mem(unsigned int Ca, gp_boolean New_config)
{
  conf_mem_block_t *new = GP_Calloc(1, sizeof(conf_mem_block_t));

  new->addr       = ((IS_PIC16_CORE || IS_PIC16E_CORE) && New_config) ? Ca : (Ca & ~1);
  new->m          = i_memory_create();
  new->new_config = New_config;

  if (state.debug_info) {
    new->file_symbol = state.obj.debug_file;
    new->line_number = state.obj.debug_line;
  }
  else {
    new->file_symbol = state.src->file_symbol;
    new->line_number = state.src->line_number;
  }

  if (state.conf_sec_mem == NULL) {
    /* The list is empty. */
    state.conf_sec_mem = new;
  }
  else {
    /* Append the new node to the end of the list. */
    state.conf_sec_mem_last->next = new;
  }

  state.conf_sec_mem_last = new;

  return new->m;
}

/*------------------------------------------------------------------------------------------------*/

static MemBlock_t *
_get_config_mem(unsigned int Ca, gp_boolean New_config)
{
  MemBlock_t *mem;

  if (state.mode != MODE_RELOCATABLE) {
    return state.c_memory;
  }

  mem = _find_conf_sec_mem(Ca);

  if (mem == NULL) {
    mem = _add_conf_sec_mem(Ca, New_config);
  }

  return mem;
}

/*------------------------------------------------------------------------------------------------*/

/* helper to write configuration data, grabbing defaults when necessary */

static void
_config_16_set_byte_mem(MemBlock_t *Config_mem, const gp_cfg_device_t *P_dev, unsigned int Ca,
                        uint8_t Byte, uint8_t Mask)
{
  uint8_t old_byte;
  char    buf[BUFSIZ];

  snprintf(buf, sizeof(buf), "CONFIG_%0*X", state.device.class->addr_digits, Ca);

  if (!b_memory_get(Config_mem, Ca, &old_byte, NULL, NULL)) {
    old_byte = (uint8_t)gp_cfg_get_default(P_dev, Ca);
  }

  b_memory_put(Config_mem, Ca, (old_byte & ~Mask) | Byte, buf, NULL);
}

/*------------------------------------------------------------------------------------------------*/

static void
_config_16_set_word_mem(MemBlock_t *Config_mem, const gp_cfg_device_t *P_dev,
                        unsigned int Ca, uint8_t Byte, uint8_t Mask)
{
  uint8_t other_byte;
  char    buf[BUFSIZ];

  if (!b_memory_get(Config_mem, Ca ^ 1, &other_byte, NULL, NULL)) {
    snprintf(buf, sizeof(buf), "CONFIG_%0*X", state.device.class->addr_digits, Ca);
    other_byte = gp_cfg_get_default(P_dev, Ca ^ 1);
    b_memory_put(Config_mem, Ca ^ 1, other_byte, buf, NULL);
  }
  _config_16_set_byte_mem(Config_mem, P_dev, Ca, Byte, Mask);
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_config(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t         *p;
  int                    ca;
  int                    value;
  MemBlock_t            *config_mem;
  const gp_cfg_device_t *p_dev;
  uint16_t               word;
  char                   buf[BUFSIZ];

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  config_us_used = true;

  if (config_mpasm_used) {
    gperror_verror(GPE_CONFIG_usCONFIG, NULL);
    return Value;
  }

  if (IS_PIC16E_CORE) {
    gperror_warning(GPW_EXPECTED, "__CONFIG has been deprecated for PIC18 devices. Use directive CONFIG.");
  }

  state.lst.line.linetype = LTY_CONFIG;

  switch(Arity) {
  case 1:
    /* FIXME: Whenever there are more than one configuration address, the
       address must be specified in MPASM. MPASM fails with error:
       "Argument out of range (not a valid config register address)" */
    if (IS_PIC16E_CORE) {
      gperror_warning(GPW_EXPECTED, "18cxxx devices should specify __CONFIG address.");
    }

    if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
      gperror_warning(GPW_EXPECTED, "Enhanced 16cxxx devices should specify __CONFIG address.");
    }

    ca = state.processor->config_addrs[0];
    p = PnListHead(Parms);
    break;

  case 2:
    ca = eval_maybe_evaluate(PnListHead(Parms));
    p = PnListHead(PnListTail(Parms));
    break;

  default:
    eval_enforce_arity(Arity, 2);
    return Value;
  }

  ca = gp_processor_org_to_byte(state.device.class, ca);
  state.lst.config_address = ca;

  if ((eval_can_evaluate(p)) && (state.pass == 2)) {
    config_mem = _get_config_mem(ca, false);
    value = eval_evaluate(p);

    if (IS_PIC16E_CORE) {
      if (value > 0xff) {
        gperror_vwarning(GPW_RANGE, "%i (%#x) > 0xff", value, value);
      }

      p_dev = gp_cfg_find_pic_multi_name(state.processor->names, ARRAY_SIZE(state.processor->names));
      if (p_dev != NULL) {
        /* We do this to also set the other byte in a word. */
        _config_16_set_word_mem(config_mem, p_dev, ca, value, 0xff);
      }
      else {
        assert(0); /* this shouldn't happen */
        b_memory_put(config_mem, ca, value, NULL, NULL);
        /* Hack in case the config defaults are not available. */
      }
    }
    else {
      /* Don't complain for 14 bit enhanced devices.
         Why are the config words 16 bits long in headers?? */
      if ((!IS_PIC14E_CORE) && (!IS_PIC14EX_CORE)) {
        if (value > state.device.class->core_mask) {
          gperror_vmessage(GPM_RANGE, NULL, value);
          value &= state.device.class->core_mask;
        }
      }

      if (state.device.class->i_memory_get(state.c_memory, ca, &word, NULL, NULL)) {
        gperror_verror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, ca));
      }

      snprintf(buf, sizeof(buf), "CONFIG_%0*X",
               state.device.class->addr_digits, gp_processor_byte_to_org(state.device.class, ca));
      state.device.class->i_memory_put(config_mem, ca, value, buf, NULL);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/* Sets defaults over unused portions of configuration memory. */

static void
_config_16_check_defaults(MemBlock_t *Config_mem, const gp_cfg_device_t *P_dev)
{
  const gp_cfg_addr_t *addrs = P_dev->addresses;
  unsigned int         t;
  uint8_t              byte;

  /* FIXME: We do not need to set defaults here, but during
     linking. And to keep it consistent for absolute mode we should
     perform linking in absolute mode. MPASM 5.34 executes MPLINK in
     absolute mode. Below comments are old and not entirely right. */
  /*
   * If we don't set defaults here, then MPLINK (I'm assuming) will pad the
   * entire section with 0xff. That puts the 0xff's in the hex file. MPASM puts
   * nothing in the hex file for unspecified bytes. I'm not sure the best
   * approach here - defaults or nothing. Going to go with defaults.
   */
  for (t = 0; t < P_dev->address_count; ++addrs, ++t) {
    if (!b_memory_get(Config_mem, addrs->address, &byte, NULL, NULL)) {
      _config_16_set_byte_mem(Config_mem, P_dev, addrs->address, addrs->def_value, 0xff);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Support MPASM(X) style CONFIG xxx = yyy syntax for PIC16(E) devices. */

static gpasmVal
_do_16_config(gpasmVal Value, const char *Name, int Arity, const pnode_t *Parms)
{
  static uint8_t            double_mask[64] = { 0, };

  const gp_cfg_device_t    *p_dev;
  const gp_cfg_directive_t *p_dir;
  const gp_cfg_option_t    *p_opt;
  const char               *k_str;
  const char               *v_str;
  pnode_t                  *k;
  pnode_t                  *v;
  unsigned int              ca;
  char                      v_buff[64];
  char                      buf[BUFSIZ];
  unsigned int              dm_addr;
  MemBlock_t               *config_mem;

  state.lst.line.linetype = LTY_DIR;
  config_mpasm_used = true;

  if (config_us_used) {
    gperror_verror(GPE_CONFIG_usCONFIG, NULL);
    return Value;
  }

  /* make sure we an find our device in the config DB */
  p_dev = gp_cfg_find_pic_multi_name(state.processor->names, ARRAY_SIZE(state.processor->names));
  if (p_dev == NULL) {
    gperror_verror(GPE_UNKNOWN, "The %s processor has no entries in the config db. CONFIG cannot be used.",
             state.processor->names[2]);
    return Value;
  }

  /* validate argument format */
  if ((Parms == NULL) || !PnIsBinOp(Parms) || (PnBinOpOp(Parms) != '=')) {
    gperror_error(GPE_CONFIG_UNKNOWN, "Incorrect syntax. Use `CONFIG KEY = VALUE'");
    return Value;
  }

  /* validate parameter types */
  k = PnBinOpP0(Parms);
  v = PnBinOpP1(Parms);

  if (!PnIsSymbol(k) || (!PnIsSymbol(v) && !PnIsConstant(v))) {
    gperror_error(GPE_CONFIG_UNKNOWN, "Incorrect syntax. Use 'CONFIG KEY = VALUE'");
    return Value;
  }

  /* grab string representations */
  k_str = PnSymbol(k);

  if (!PnIsConstant(v)) {
    v_str = PnSymbol(v);
  }
  else {
    int value = PnConstant(v);

    if (state.radix != 10) {
      if (state.radix == 16) {
        snprintf(v_buff, sizeof(v_buff), "%x", value);
      }
      else {
        gperror_error(GPE_CONFIG_UNKNOWN, "CONFIG can't be used in source files with a radix other than 10 or 16.");
      }
    }
    else {
      snprintf(v_buff, sizeof(v_buff), "%d", value);
    }

    v_str = v_buff;
  }

  /* find the directive */
  p_dir = gp_cfg_find_directive(p_dev, k_str, (unsigned int *)&ca, NULL);
  if (p_dir == NULL) {
    snprintf(buf, sizeof(buf), "CONFIG Directive Error: Setting not found for %s processor: \"%s\"",
             state.processor->names[2], k_str);
    gperror_error(GPE_UNKNOWN, buf);
    return Value;
  }

  /* Note address to lister, though it doesn't seem to use it. */
  state.lst.config_address = ca;

  /* find the option */
  p_opt = gp_cfg_find_option(p_dir, v_str);
  if (p_opt == NULL) {
    snprintf(buf, sizeof(buf), "CONFIG Directive Error: Specified value not valid for directive: \"%s\"", v_str);
    gperror_error(GPE_UNKNOWN, buf);
    return Value;
  }

  /* emit the bytes if appropriate */
  if (state.pass == 2) {
    dm_addr    = (ca - p_dev->addresses->address) & 0xFF;
    config_mem = _get_config_mem(ca, true);

    if (state.mode != MODE_RELOCATABLE) {
      _config_16_check_defaults(config_mem, p_dev);
    }

    /* make sure we've not written here yet */
    if (dm_addr < sizeof(double_mask)) {
      if (double_mask[dm_addr] & p_dir->mask) {
        snprintf(buf, sizeof(buf), "CONFIG Directive Error: Multiple definitions found for %s setting.", k_str);
        gperror_error(GPE_UNKNOWN, buf);
        return Value;
      }
      double_mask[dm_addr] |= p_dir->mask;
    }
    else {
      gperror_warning(GPW_UNKNOWN, "Double_mask in do_16_config() needs to be adjusted to account for larger config ranges.");
    }

    /* Let the helper set the data. */
    _config_16_set_byte_mem(config_mem, p_dev, ca, (uint8_t)p_opt->value, (uint8_t)p_dir->mask);
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static void
_config_12_14_set_word_mem(MemBlock_t *Config_mem, const gp_cfg_device_t *P_dev,
                           unsigned int Ca, uint16_t Word, uint16_t Mask)
{
  unsigned int org;
  uint16_t     old_word;
  char         buf[BUFSIZ];

  org = gp_processor_byte_to_org(state.device.class, Ca);
  snprintf(buf, sizeof(buf), "CONFIG_%0*X", state.device.class->addr_digits, org);

  if (!state.device.class->i_memory_get(Config_mem, Ca, &old_word, NULL, NULL)) {
    old_word = gp_cfg_get_default(P_dev, org);
  }

  Word |= (old_word & ~Mask);
  Word &= state.device.class->config_mask;
  state.device.class->i_memory_put(Config_mem, Ca, Word, buf, NULL);
}

/*------------------------------------------------------------------------------------------------*/

/* Sets defaults over unused portions of configuration memory. */

static void
_config_12_14_check_defaults(MemBlock_t *Config_mem, const gp_cfg_device_t *P_dev)
{
  const gp_cfg_addr_t *addrs = P_dev->addresses;
  unsigned int         addr;
  uint16_t             word;
  unsigned int         t;

  for (t = 0; t < P_dev->address_count; ++addrs, ++t) {
    addr = gp_processor_org_to_byte(state.device.class, addrs->address);

    if (!state.device.class->i_memory_get(Config_mem, addr, &word, NULL, NULL)) {
      word = addrs->def_value & state.device.class->config_mask;
      state.device.class->i_memory_put(Config_mem, addr, word, NULL, NULL);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Support MPASM(X) style CONFIG xxx = yyy syntax for PIC14(E) and PIC12(E) devices. */

static gpasmVal
_do_12_14_config(gpasmVal Value, const char *Name, int Arity, const pnode_t *Parms)
{
  static uint16_t           double_mask[256] = { 0, };

  const gp_cfg_device_t    *p_dev;
  const gp_cfg_directive_t *p_dir;
  const gp_cfg_option_t    *p_opt;
  const char               *k_str;
  const char               *v_str;
  const pnode_t            *k;
  const pnode_t            *v;
  unsigned int              conf_org;
  unsigned int              ca;
  char                      v_buff[64];
  char                      buf[BUFSIZ];
  unsigned int              dm_addr;
  MemBlock_t               *config_mem;

  state.lst.line.linetype = LTY_DIR;
  config_mpasm_used = true;

  if (config_us_used) {
    gperror_verror(GPE_CONFIG_usCONFIG, NULL);
    return Value;
  }

  /* make sure we an find our device in the config DB */
  p_dev = gp_cfg_find_pic_multi_name(state.processor->names, ARRAY_SIZE(state.processor->names));
  if (p_dev == NULL) {
    gperror_verror(GPE_UNKNOWN,
                   "The %s processor has no entries in the config db. CONFIG cannot be used.",
                   state.processor->names[2]);
    return Value;
  }

  /* validate argument format */
  if ((Parms == NULL) || !PnIsBinOp(Parms) || (PnBinOpOp(Parms) != '=')) {
    gperror_error(GPE_CONFIG_UNKNOWN, "Incorrect syntax. Use 'CONFIG KEY = VALUE'");
    return Value;
  }

  /* validate parameter types */
  k = PnBinOpP0(Parms);
  v = PnBinOpP1(Parms);

  if (!PnIsSymbol(k) || (!PnIsSymbol(v) && !PnIsConstant(v))) {
    gperror_error(GPE_CONFIG_UNKNOWN, "Incorrect syntax. Use 'CONFIG KEY = VALUE'");
    return Value;
  }

  /* grab string representations */
  k_str = PnSymbol(k);

  if (!PnIsConstant(v)) {
    v_str = PnSymbol(v);
  }
  else {
    int value = PnConstant(v);

    if (state.radix != 10) {
      if (state.radix == 16) {
        snprintf(v_buff, sizeof(v_buff), "%x", value);
      }
      else {
        gperror_error(GPE_CONFIG_UNKNOWN,
                      "CONFIG can't be used in source files with a radix other than 10 or 16.");
      }
    }
    else {
      snprintf(v_buff, sizeof(v_buff), "%d", value);
    }

    v_str = v_buff;
  }

  /* find the directive */
  p_dir = gp_cfg_find_directive(p_dev, k_str, &conf_org, NULL);
  if (p_dir == NULL) {
    snprintf(buf, sizeof(buf), "CONFIG Directive Error: Setting not found for %s processor: \"%s\"",
             state.processor->names[2], k_str);
    gperror_error(GPE_UNKNOWN, buf);
    return Value;
  }

  /* Note address to lister, though it doesn't seem to use it. */
  ca = gp_processor_org_to_byte(state.device.class, conf_org);
  state.lst.config_address = ca;

  /* find the option */
  p_opt = gp_cfg_find_option(p_dir, v_str);
  if (p_opt == NULL) {
    snprintf(buf, sizeof(buf), "CONFIG Directive Error: Specified value not valid for directive: \"%s\"", v_str);
    gperror_error(GPE_UNKNOWN, buf);
    return Value;
  }

  /* emit the bytes if appropriate */
  if (state.pass == 2) {
    dm_addr    = (conf_org - p_dev->addresses->address) & 0xFF;
    config_mem = _get_config_mem(ca, true);

    if (state.mode != MODE_RELOCATABLE) {
      _config_12_14_check_defaults(config_mem, p_dev);
    }

    /* make sure we've not written here yet */
    if (dm_addr < sizeof(double_mask)) {
      if (double_mask[dm_addr] & p_dir->mask) {
        snprintf(buf, sizeof(buf), "CONFIG Directive Error: Multiple definitions found for %s setting.", k_str);
        gperror_error(GPE_UNKNOWN, buf);
        return Value;
      }
      double_mask[dm_addr] |= p_dir->mask;
    }
    else {
      gperror_warning(GPW_UNKNOWN,
                      "Double_mask in do_12_14_config() needs to be adjusted to account for larger config ranges.");
    }

    /* Let the helper set the data. */
    _config_12_14_set_word_mem(config_mem, p_dev, ca, p_opt->value, p_dir->mask);
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_gpasm_config(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  char           buf[BUFSIZ];

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (state.mpasm_compatible) {
    /* The MPASM(X) compatible mode valid only for 16 bit devices. */
    if ((!IS_PIC16_CORE) && (!IS_PIC16E_CORE)) {
      snprintf(buf, sizeof(buf),
               "CONFIG Directive Error: Processor \"%s\" is invalid for CONFIG directive in MPASM(X) mode.",
               state.processor->names[2]);
      gperror_error(GPE_UNKNOWN, buf);
      return Value;
    }

    /* store data from the last section */
    coff_close_section();
  }
  else {
    /* The gpasm compatible mode valid all PIC devices. */
    if ((!IS_PIC16_CORE) && (!IS_PIC16E_CORE) &&
        (!IS_PIC14_CORE) && (!IS_PIC14E_CORE) && (!IS_PIC14EX_CORE) &&
        (!IS_PIC12_CORE) && (!IS_PIC12E_CORE) && (!IS_PIC12I_CORE)) {
      snprintf(buf, sizeof(buf),
               "CONFIG Directive Error: Processor \"%s\" is invalid for CONFIG directive.",
               state.processor->names[2]);
      gperror_error(GPE_UNKNOWN, buf);
      return Value;
    }
  }

  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (IS_PIC16_CORE || IS_PIC16E_CORE) {
      _do_16_config(Value, Name, Arity, p);
    }
    else {
      _do_12_14_config(Value, Name, Arity, p);
    }
  }

  if (state.mpasm_compatible) {
    /* A new section must be started after this directive. */
    state.obj.section = NULL;
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/*
 *  _do_da - The 'da' directive. Identical to 'data' directive, except
 *           for 14-bit cores it packs two 7-bit characters into one word.
 */

static gpasmVal
_do_da(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  unsigned int char_shift;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  char_shift = (IS_PIC14_CORE || IS_PIC14E_CORE || IS_PIC14EX_CORE) ? 7 : 8;

  if ((state.mode == MODE_RELOCATABLE) && (SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
    /* This is a data memory not program. */
    state.lst.line.linetype = LTY_DATA;
    char_shift = 8;
  }

  _emit_data(Parms, char_shift, Name);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/*
 * _do_data - The 'data' directive. Fill words of program memory with data.
 *            Pack strings two characters into one word, first character in
 *            most significant byte of the word except for the pic18cxxx.
 *            Pic18cxxx has byte addressed program memory and strings are
 *            stored directly. Strings are padded to even number of bytes
 *            by adding zero byte to end of string.
 *            Eeprom16 strings have same byte order than pic18cxxx, but
 *            only because all eeprom16 words are stored most significant
 *            byte first (big-endian).
 *            When in idata section, generate initialization data.
 */

static gpasmVal
_do_data(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if ((state.mode == MODE_RELOCATABLE) && (SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
    /* This is a data memory not program. */
    state.lst.line.linetype = LTY_DATA;
  }

  _emit_data(Parms, 8, Name);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/*
 * _do_db  - Reserve program memory words with packed 8-bit values. On the
 *           18cxxx families, dw and db are the same. For the 12 and 14 bit
 *           cores, the upper bits are masked (e.g. the 14-bit core can only
 *           store 14bits at a given program memory address, so the upper 2
 *           in a db directive are meaningless.
 */

static gpasmVal
_do_db(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *L;
  const pnode_t *p;
  unsigned int   org;
  unsigned int   begin_byte_addr;
  const char    *pc;
  int            value;
  uint16_t       v;
  unsigned int   n;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (state.mode == MODE_RELOCATABLE) {
    if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
      /* This is a data memory not program. */
      state.lst.line.linetype = LTY_DATA;
    }
    else if (!(SECTION_FLAGS & STYP_TEXT)) {
      /* only valid in initialized data and text sections */
      gperror_verror(GPE_WRONG_SECTION, NULL);
      return Value;
    }
  }

  L = Parms;

  if (IS_PIC16E_CORE || (SECTION_FLAGS & STYP_DATA)) {
    begin_byte_addr = state.byte_addr;

    for (; L != NULL; L = PnListTail(L)) {
      p = PnListHead(L);

      if (PnIsString(p)) {
        pc = PnString(p);
        while (*pc != '\0') {
          pc = convert_escape_chars(pc, &value);
          _emit_byte(value, Name);
        }
      }
      else {
        value = eval_reloc_evaluate(p, RELOCT_LOW, NULL, NULL);

        if (value < 0) {
          gperror_vwarning(GPW_RANGE, "%i (%#x) < 0", value, value);
        }
        else if (value > 0xFF) {
          gperror_vwarning(GPW_RANGE, "%i (%#x) > 0xff", value, value);
        }

        _emit_byte(value, Name);
      }
    }

    if (state.mpasm_compatible) {
      if ((state.mode == MODE_ABSOLUTE) || !(SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
        if ((state.byte_addr - begin_byte_addr) & 1) {
          _emit_byte(0, Name);
        }
      }
    }
    else {
      org = gp_processor_byte_to_real(state.processor, begin_byte_addr);

      if ((gp_processor_is_config_org(state.processor, org) < 0) &&
          (gp_processor_is_idlocs_org(state.processor, org) < 0) && 
          (gp_processor_is_eeprom_org(state.processor, org) < 0)) {
        if ((state.mode == MODE_ABSOLUTE) || !(SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
          if ((state.byte_addr - begin_byte_addr) & 1) {
            _emit_byte(0, Name);
          }
        }
      }
    }
  } /* if (IS_PIC16E_CORE || (SECTION_FLAGS & STYP_DATA)) */
  else {
    v = 0;
    n = 0;

    while (L != NULL) {
      pc = NULL;
      p = PnListHead(L);

      if (PnIsString(p)) {
        pc = PnString(p);
      }

      for (; ; ) {
        if (PnIsString(p)) {
          pc = convert_escape_chars(pc, &value);
        }
        else {
          value = eval_reloc_evaluate(p, RELOCT_LOW, NULL, NULL);
        }

        if (value < -128) {
          gperror_vwarning(GPW_RANGE, "%i (%#x) < -128", value, value);
        }
        else if (value > 0xFF) {
          gperror_vwarning(GPW_RANGE, "%i (%#x) > 0xff", value, value);
        }

        value &= 0xFF;

        if (state.mpasm_compatible) {
          if (!(n & 1)) {
            v = value << 8;
          }
          else {
            _emit(v | value, Name);
          }
        }
        else {
          org = gp_processor_byte_to_real(state.processor, state.byte_addr);

          if (gp_processor_is_eeprom_org(state.processor, org) >= 0) {
            _emit_byte(value, Name);
          }
          else {
            if (!(n & 1)) {
              v = value << 8;
            }
            else {
              _emit(v | value, Name);
            }
          }
        }

        ++n;

        if (!PnIsString(p) || (*pc == '\0')) {
          L = PnListTail(L);
          break;
        }
      } /* for (; ; ) */
    } /* while (L != NULL) */

    if (state.mpasm_compatible) {
      if (n & 1) {
        _emit(v, Name);
      }
    }
    else {
      org = gp_processor_byte_to_real(state.processor, state.byte_addr);

      if (gp_processor_is_eeprom_org(state.processor, org) < 0) {
        if (n & 1) {
          _emit(v, Name);
        }
      }
    }
  } /* else */
  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_de(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  const char    *pc;
  int            value;
  uint16_t       v;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (IS_PIC16E_CORE) {
    return _do_db(Value, Name, Arity, Parms);
  }

  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (PnIsString(p)) {
      pc = PnString(p);

      while (*pc != '\0') {
        pc = convert_escape_chars(pc, &value);
        _emit(value & 0xff, Name);
      }
    }
    else {
      v = eval_reloc_evaluate(p, RELOCT_ALL, NULL, NULL);
      _emit(v & 0xff, Name);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/* Extension to MPASM(X), used at least by LLVM to emit debugging information. */

static gpasmVal
_do_def(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t      *p;
  const char         *symbol_name;
  gp_symbol_t        *coff_symbol;
  int                 eval;
  int                 value;
  gp_boolean          new_class;
  gp_boolean          new_type;
  int                 coff_class;
  int                 coff_type;
  enum gpasmValTypes  type;
  const char         *lhs;
  const char         *str;

  symbol_name = NULL;
  coff_symbol = NULL;
  value       = 0;
  new_class   = false;
  coff_class  = C_NULL;
  new_type    = false;
  coff_type   = T_NULL;
  type        = VAL_DEBUG;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else {
    if (Arity < 2) {
      eval_enforce_arity(Arity, 2);
      return Value;
    }

    /* the first argument is the symbol Name */
    p = PnListHead(Parms);

    if (eval_enforce_simple(p)) {
      symbol_name = PnSymbol(p);
    }
    else {
      return Value;
    }

    Parms = PnListTail(Parms);

    /* update the properties */
    for (; Parms != NULL; Parms = PnListTail(Parms)) {
      p = PnListHead(Parms);

      if (PnIsBinOp(p) && (PnBinOpOp(p) == '=')) {
        if (eval_enforce_simple(PnBinOpP0(p))) {
          lhs = PnSymbol(PnBinOpP0(p));

          if (strcasecmp(lhs, "value") == 0) {
            value = eval_maybe_evaluate(PnBinOpP1(p));
          }
          else if (strcasecmp(lhs, "size") == 0) {
            eval = eval_maybe_evaluate(PnBinOpP1(p));
            state.byte_addr += IS_RAM_ORG ? eval : gp_processor_org_to_byte(state.device.class, eval);
          }
          else if (strcasecmp(lhs, "type") == 0) {
            eval = eval_maybe_evaluate(PnBinOpP1(p));

            if (eval < 0) {
              gperror_verror(GPE_RANGE, "type{%i (%#x)} < 0", eval, eval);
            }
            else if (eval > 0xffff) {
              gperror_verror(GPE_RANGE, "type{%i (%#x)} > 0xffff", eval, eval);
            }
            else {
              new_type  = true;
              coff_type = eval;
            }
          }
          else if (strcasecmp(lhs, "class") == 0) {
            eval = eval_maybe_evaluate(PnBinOpP1(p));

            if (eval < -128) {
              gperror_verror(GPE_RANGE, "class{%i} < -128", eval, eval);
            }
            else if (eval > 127) {
              gperror_verror(GPE_RANGE, "class{%i} > 127", eval, eval);
            }
            else {
              new_class  = true;
              coff_class = eval;
            }
          }
          else {
            gperror_verror(GPE_ILLEGAL_ARGU, NULL, lhs);
          }
        }
      }
      else {
        if (eval_enforce_simple(p)) {
          str = PnSymbol(p);

          if (strcasecmp(str, "absolute") == 0) {
            type  = VAL_ABSOLUTE;
            value = 0;
          }
          else if (strcasecmp(str, "debug") == 0) {
            type  = VAL_DEBUG;
            value = 0;
          }
          else if (strcasecmp(str, "extern") == 0) {
            type  = VAL_EXTERN;
            value = 0;
          }
          else if (strcasecmp(str, "global") == 0) {
            type  = VAL_GLOBAL;
            value = IS_RAM_ORG ? state.byte_addr : gp_processor_byte_to_org(state.device.class, state.byte_addr);
          }
          else if (strcasecmp(str, "static") == 0) {
            type  = VAL_STATIC;
            value = IS_RAM_ORG ? state.byte_addr : gp_processor_byte_to_org(state.device.class, state.byte_addr);
          }
          else {
            gperror_verror(GPE_ILLEGAL_ARGU, NULL, PnSymbol(p));
          }
        }
      }
    }
  }

  set_global(symbol_name, value, LFT_PERMANENT, type, false);

  /* update the symbol with the values */
  if ((state.pass == 2) && (new_class || new_type)) {
    coff_symbol = gp_coffgen_find_symbol(state.obj.object, symbol_name);
    assert(coff_symbol != NULL);

    if (new_class) {
      coff_symbol->class = coff_class;
    }

    if (new_type) {
      coff_symbol->type = coff_type;
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_define(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  pnode_t  *p;
  symbol_t *curr_def;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit   = false;

  if (Arity < 1) {
    gperror_verror(GPE_MISSING_ARGU, NULL);
  }
  else {
    assert(PnIsList(Parms));
    p = PnListHead(Parms);
    assert(PnIsString(p));

    if (asm_enabled() && !IN_MACRO_WHILE_DEFINITION) {
      if (sym_get_symbol(state.stDefines, PnString(p)) != NULL) {
        gperror_verror(GPE_DUPLAB, NULL, PnString(p));
      }
      else {
        curr_def = sym_add_symbol(state.stDefines, PnString(p));
        p        = PnListTail(Parms);

        if (p != NULL) {
          assert(PnIsList(p));
          sym_annotate_symbol(curr_def, p);
        }
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/* Extension to MPASM(X), used at least by LLVM to emit debugging information. */

static gpasmVal
_do_dim(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  const char    *symbol_name;
  gp_symbol_t   *coff_symbol = NULL;
  int            number_symbols;
  gp_aux_t      *aux_list;
  int            i;
  int            value;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else {
    if (Arity < 3) {
      eval_enforce_arity(Arity, 1);
      return Value;
    }

    /* the first argument is the symbol name */
    p = PnListHead(Parms);

    if (eval_enforce_simple(p)) {
      /* lookup the symbol */
      symbol_name = PnSymbol(p);
      coff_symbol = gp_coffgen_find_symbol(state.obj.object, symbol_name);

      if (coff_symbol == NULL) {
        gperror_verror(GPE_NOSYM, NULL, symbol_name);
        return Value;
      }
    }
    else {
      return Value;
    }
    Parms = PnListTail(Parms);

    /* the second argument must be the number of aux symbols */
    p = PnListHead(Parms);
    number_symbols = eval_maybe_evaluate(p);

    if ((number_symbols < 0) || (number_symbols > 127)) {
      gperror_error(GPE_UNKNOWN, "Number of auxiliary symbols must be less then 128 and positive.");
      return Value;
    }

    state.obj.symbol_num += number_symbols;
    Parms = PnListTail(Parms);

    /* create the symbols */
    aux_list = gp_coffgen_add_aux(state.obj.object, coff_symbol);

    /* write the data to the auxiliary symbols */
    i = 0;
    while (Parms != NULL) {
      p = PnListHead(Parms);
      value = eval_maybe_evaluate(p);

      if (value & (~0xff)) {
        gperror_verror(GPE_RANGE, "%i (%#x) > 0xff", value, value);
        return Value;
      }

      if (aux_list == NULL) {
        gperror_error(GPE_UNKNOWN, "Insufficent number of auxiliary symbols.");
        return Value;
      }

      if (i == (state.obj.newcoff ? SYMBOL_SIZE_v2 : SYMBOL_SIZE_v1)) {
        i = 0;
        aux_list = gp_coffgen_add_aux(state.obj.object, coff_symbol);
      }

      aux_list->_aux_symbol.data[i++] = value;

      Parms = PnListTail(Parms);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_direct(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  pnode_t    *p;
  int         value;
  uint8_t     direct_command = 0;
  const char *direct_string  = NULL;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else if (eval_enforce_arity(Arity, 2)) {
    p = PnListHead(Parms);
    coerce_str1(p);
    value = eval_maybe_evaluate(p);

    if (value < 0) {
      gperror_verror(GPE_RANGE, "%i (%#x) < 0", value, value);
    }
    else if (value > WHILE_LOOP_COUNT_MAX) {
      gperror_verror(GPE_RANGE, "%i (%#x) > %u", value, value, WHILE_LOOP_COUNT_MAX);
    }
    else {
      direct_command = value;
    }

    p = PnListHead(PnListTail(Parms));
    if (PnIsString(p)) {
      if (strlen(PnString(p)) < 255) {
        direct_string = convert_escaped_char(PnString(p), '"');
      }
      else {
        gperror_error(GPE_UNKNOWN, "String must be less than 255 bytes long.");
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }

    if (direct_string == NULL) {
      return Value;
    }

    if (SECTION_FLAGS & STYP_TEXT) {
      coff_add_direct_sym(direct_command, direct_string);
    }
    else {
      gperror_verror(GPE_WRONG_SECTION, NULL);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_dt(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  const char    *pc;
  int            retlw;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  retlw = gp_processor_retlw(state.device.class);

  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (PnIsString(p)) {
      pc = PnString(p);

      while (*pc != '\0') {
        int value;

        pc = convert_escape_chars(pc, &value);
        _emit((value & 0xff) | retlw, Name);
      }
    }
    else {
      uint16_t v;

      v = eval_reloc_evaluate(p, RELOCT_ALL, NULL, NULL);
      _emit((v & 0xff) | retlw, Name);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_dtm(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t  *p;
  const symbol_t *s;
  const insn_t   *i;
  const char     *pc;
  uint16_t        v;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  s = sym_get_symbol(state.stBuiltin, "movlw");
  i = sym_get_symbol_annotation(s);

  if ((!IS_PIC14E_CORE) && (!IS_PIC14EX_CORE)) {
    gperror_verror(GPE_ILLEGAL_DIR, NULL, Name);
  }

  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (PnIsString(p)) {
      pc = PnString(p);

      while (*pc != '\0') {
        int value;

        pc = convert_escape_chars(pc, &value);
        _emit(i->opcode | (value & 0xff), Name);
      }
    }
    else {
      v = eval_reloc_evaluate(p, RELOCT_ALL, NULL, NULL);
      _emit(i->opcode | (v & 0xff), Name);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/*
 *  _do_dw - The 'dw' directive. On all families except for the p18cxxx, the
 *           dw directive is the same as the 'data' directive. For the p18cxxx
 *           it's the same as the 'db' directive. (That's strange, but it's
 *           also the way mpasm does it.)
 */

static gpasmVal
_do_dw(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (state.mode == MODE_RELOCATABLE) {
    if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
      /* This is a data memory not program. */
      state.lst.line.linetype = LTY_DATA;
    }
    else if (!(SECTION_FLAGS & STYP_TEXT)) {
      /* only valid in initialized data and text sections */
      gperror_verror(GPE_WRONG_SECTION, NULL);
      return Value;
    }
  }
  /* MPASM 5.34 seems to do this same for p18cxxx as for others. */
  _emit_data(Parms, 8, Name);

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_else(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (state.astack == NULL) {
    gperror_verror(GPE_ILLEGAL_COND, NULL);
  }
  else if ((state.astack->mode != IN_THEN) && (state.astack->mode != IN_ELIF)) {
    gperror_verror(GPE_ILLEGAL_COND, NULL);
  }
  else {
    state.astack->enabled = !state.astack->before_else_enabled;
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_end(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  state.found_end = true;
  state.lst.line.linetype = LTY_DIR;

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_endif(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  amode_t *old;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (state.astack == NULL) {
    gperror_error(GPE_ILLEGAL_COND, "Illegal condition: \"ENDIF\"");
  }
  else if ((state.astack->mode != IN_THEN) &&
           (state.astack->mode != IN_ELIF) &&
           (state.astack->mode != IN_ELSE)) {
    gperror_error(GPE_ILLEGAL_COND, "Illegal condition: \"ENDIF\"");
  }
  else {
    old = state.astack;
    state.astack = state.astack->upper;
    free(old);
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_endm(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  if (_check_processor_select(Name)) {
    return Value;
  }

  assert(state.mac_head == NULL);
  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (!IN_MACRO_WHILE_DEFINITION) {
    gperror_verror(GPE_UNMATCHED_ENDM, NULL);
  }
  else {
    state.mac_prev = NULL;
  }

  state.mac_body = NULL;

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_endw(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_NOLIST_DIR;
  assert(state.mac_head == NULL);

  if (!IN_MACRO_WHILE_DEFINITION) {
    gperror_error(GPE_ILLEGAL_COND, "Illegal condition: \"ENDW\"");
  }
  else if (eval_maybe_evaluate(state.while_head->parms)) {
    state.next_state = STATE_WHILE;
    state.next_buffer.macro = state.while_head;
  }
  else if (state.pass == 2) {
    macro_list(state.while_head->body);
    state.preproc.do_emit = false;
  }

  state.mac_body = NULL;
  state.mac_prev = NULL;
  state.while_head = NULL;

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_eof(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else if (eval_enforce_arity(Arity, 0)) {
    if (state.debug_info) {
      coff_add_eof_sym();
    }
    else {
      gperror_warning(GPW_UNKNOWN, "Directive ignored when debug info is disabled.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_equ(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_EQU;

  if (eval_enforce_arity(Arity, 1)) {
    Value = eval_maybe_evaluate(PnListHead(Parms));
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/* Recognize it and decodes the characters which are protects a special (meta) character. */

static int
_resolve_meta_chars(char *Dst, int Max_size, const char *Src, int Size)
{
  char       *d;
  char        ch;
  gp_boolean  meta;

  if (Max_size <= 0) {
    return 0;
  }

  /* Leaves room for the termination '\0' character. */
  --Max_size;
  meta = false;
  d = Dst;
  while ((Max_size > 0) && (Size > 0)) {
    ch = *Src++;
    --Size;
    if (ch == '\0') {
      break;
    }

    if (meta) {
      switch (ch) {
        case '"':
        case '\'':
        case '\\':
          *d++ = ch;
          --Max_size;
          break;

        default: {
          *d++ = ch;
          --Max_size;
        }
      }

      meta = false;
    }
    else if (ch == '\\') {
      meta = true;
    }
    else {
      *d++ = ch;
      --Max_size;
    }
  }

  *d = '\0';
  return (d - Dst);
}

/*------------------------------------------------------------------------------------------------*/

static const char *
_hv_macro_resolver(const char *String)
{
  static char out[BUFSIZ];

  char        buf[BUFSIZ];
  const char *st_start;
  const char *st_end;
  const char *hv_start;
  const char *hv_end;
  int         out_idx;
  int         raw_size;
  int         mt_size;
  gp_boolean  is_meta;

  st_start = String;
  st_end   = st_start + strlen(String);
  out_idx  = 0;
  while (st_start < st_end) {
    if (find_hv_macro(st_start, &hv_start, &hv_end)) {
      /* Found a #v() macro. */
      is_meta = false;

      if (hv_start > st_start) {
        /* Before the macro there is a general text. */
        raw_size = hv_start - st_start;
        if (raw_size > (int)(sizeof(buf) - 1)) {
          gperror_verror(GPE_TOO_LONG, NULL, st_start, (size_t)raw_size, sizeof(buf) - 1);
          return NULL;
        }

        /* Decodes the protected characters. */
        mt_size = _resolve_meta_chars(buf, sizeof(buf), st_start, raw_size);
        st_start += raw_size;

        if (mt_size > 0) {
          if (mt_size > (int)(sizeof(out) - out_idx - 1)) {
            gperror_verror(GPE_TOO_LONG, NULL, buf, (size_t)mt_size, sizeof(out) - out_idx - 1);
            return NULL;
          }

          /* It adds to existing text. */
          memcpy(&out[out_idx], buf, mt_size);
          out_idx += mt_size;
          out[out_idx] = '\0';
        }

        if (*(hv_start - 1) == '\\') {
          /* Before the macro there is a meta '\' character: \#v(...) */
          is_meta = true;
        }
      }

      raw_size = hv_end - hv_start;
      st_start = hv_end;

      if (is_meta) {
        /* This is not macro, only simple text. */
        if (raw_size > (int)(sizeof(out) - out_idx - 1)) {
          gperror_verror(GPE_TOO_LONG, NULL, hv_start, (size_t)raw_size, sizeof(out) - out_idx - 1);
          return NULL;
        }

        /* It adds to existing text. */
        memcpy(&out[out_idx], hv_start, raw_size);
        out_idx += raw_size;
        out[out_idx] = '\0';
      }
      else {
        /* This is really macro. */
        if (raw_size > (int)(sizeof(buf) - 1)) {
          gperror_verror(GPE_TOO_LONG, NULL, hv_start, (size_t)raw_size, sizeof(buf) - 1);
          return NULL;
        }

        /* Copies it a temporary buffer. */
        memcpy(buf, hv_start, raw_size);
        buf[raw_size] = '\0';
        /* It executes the macro and get the result. */
        preprocess_line(buf, &raw_size, sizeof(buf));

        if (raw_size > (int)(sizeof(out) - out_idx - 1)) {
          gperror_verror(GPE_TOO_LONG, NULL, buf, (size_t)raw_size, sizeof(out) - out_idx - 1);
          return NULL;
        }

        /* It adds to existing text. */
        memcpy(&out[out_idx], buf, raw_size);
        out_idx += raw_size;
        out[out_idx] = '\0';
      }
    } /* if (find_hv_macro(st_start, &hv_start, &hv_end)) */
    else {
      raw_size = st_end - st_start;
      if (raw_size > (int)(sizeof(out) - out_idx - 1)) {
        gperror_verror(GPE_TOO_LONG, NULL, st_start, (size_t)raw_size, sizeof(out) - out_idx - 1);
        return NULL;
      }

      /* It adds to existing text. */
      memcpy(&out[out_idx], st_start, raw_size);
      out_idx += raw_size;
      out[out_idx] = '\0';
      st_start += raw_size;
    }
  }

  return out;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_error(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  const char    *str;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (PnIsString(p)) {
      if ((str = _hv_macro_resolver(PnString(p))) != NULL) {
        gperror_verror(GPE_USER, NULL, str);
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/************************************************************************
 * do_errlvl - parse the ERRORLEVEL directive
 *
 * If the file gpasm is assembling contains a ERRORLEVEL directive, then scan
 * the comma delimited list of options in *Parms
 *
 * Inputs:
 *   gpasmVal Value - not used, but is returned
 *   char *Name     - not used, but contains the directive name 'list'
 *   int Arity      - not used, but should contain '1'
 *   pnode_t *Parms - a linked list of the parsed parameters
 *
 *
 *
 ************************************************************************/

static gpasmVal
_do_errlvl(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  gpasmVal       value;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;

  if (state.pass == 2) {
    for (; Parms != NULL; Parms = PnListTail(Parms)) {
      p = PnListHead(Parms);

      if (PnIsUnOp(p))  {
        value = eval_evaluate(PnUnOpP0(p));

        if (PnUnOpOp(p) == '-') {
          gperror_add_code(-value);
        }
        else if (PnUnOpOp(p) == '+') {
          gperror_add_code(value);
        }
        else {
          gperror_error(GPE_ILLEGAL_ARGU, "Expected 0, 1, 2, +|-<message number>.");
        }
      }
      else if (PnIsConstant(p)) {
        select_errorlevel(PnConstant(p));
      }
      else {
        gperror_error(GPE_ILLEGAL_ARGU, "Expected 0, 1, 2, +|-<message number>.");
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_exitm(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;

  if (eval_enforce_arity(Arity, 0)) {
    if (state.stGlobal == state.stTop) {
      gperror_error(GPE_UNKNOWN, "Attempt to use \"exitm\" outside of macro.");
    }
    else {
      state.next_state = STATE_EXITMACRO;
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_expand(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (state.cmd_line.macro_expand) {
    gperror_vmessage(GPM_SUPLIN, NULL);
  }
  else {
    if (eval_enforce_arity(Arity, 0)) {
      state.lst.expand = true;
    }
  }
  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_extern(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const char *p;

  state.lst.line.linetype = LTY_SET4;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else {
    for (; Parms != NULL; Parms = PnListTail(Parms)) {
      p = PnSymbol(PnListHead(Parms));

      if (p != NULL) {
        set_global(p, 0, LFT_PERMANENT, VAL_EXTERN, false);
      }
    }
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_file(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else if (eval_enforce_arity(Arity, 1)) {
    if (state.debug_info) {
      p = PnListHead(Parms);

      if (PnIsString(p)) {
        state.obj.debug_file = coff_add_file_sym(PnString(p), 0);
      }
      else {
        gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
      }
    }
    else {
      gperror_warning(GPW_UNKNOWN, "Directive ignored when debug info is disabled.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/* Filling constants is handled here. Filling instructions is handled in the parser. */

static gpasmVal
_do_fill(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *h;
  int            number;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (eval_enforce_arity(Arity, 2)) {
    h = PnListHead(Parms);
    number = eval_fill_number(PnListHead(PnListTail(Parms)));

    for (; number > 0; --number) {
      /* we must evaluate each loop, because some symbols change (i.e. $) */
      _emit(eval_maybe_evaluate(h), Name);
    }
  }
  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_global(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const char     *p;
  const symbol_t *s;
  variable_t     *var;
  char            buf[BUFSIZ];

  state.lst.line.linetype = LTY_SET4;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else {
    for (; Parms != NULL; Parms = PnListTail(Parms)) {
      p = PnSymbol(PnListHead(Parms));

      if (p != NULL) {
        s = sym_get_symbol(state.stTop, p);

        if (s == NULL) {
          gperror_verror(GPE_NOSYM, NULL, p);
        }
        else {
          var = sym_get_symbol_annotation(s);

          if (var == NULL) {
            snprintf(buf, sizeof(buf), "Symbol not assigned a value: \"%s\"", p);
            gperror_warning(GPW_UNKNOWN, buf);
          }
          else {
            if ((var->previous_type == VAL_ADDRESS) ||
                (var->previous_type == VAL_GLOBAL)  ||
                (var->previous_type == VAL_STATIC)) {
              /* make the symbol global */
              var->type = VAL_GLOBAL;
            }
            else if (var->previous_type == VAL_EXTERN) {
              gperror_verror(GPE_DUPLAB, NULL, s);
            }
            else {
              snprintf(buf, sizeof(buf), "Operand must be an address label: \"%s\"", p);
              gperror_error(GPE_MUST_BE_LABEL, buf);
            }
          }
        }
      }
      else {
        gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
      }
    }
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_idata(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else if (IS_PIC12_CORE) {
    gperror_verror(GPE_ILLEGAL_DIR, NULL, Name);
  }
  else {
    switch (Arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".idata", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_DATA;
      break;

    case 1:
      /* new absolute section */
      p = PnListHead(Parms);
      gp_strncpy(state.obj.new_sect_name, ".idata", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = eval_maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_DATA | STYP_ABS;
      break;

    default:
     eval_enforce_arity(Arity, 1);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_idata_acs(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else if (IS_PIC12_CORE) {
    gperror_verror(GPE_ILLEGAL_DIR, NULL, Name);
  }
  else {
    switch (Arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".idata_acs", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_DATA | STYP_ACCESS;
      break;

    case 1:
      /* new absolute section */
      p = PnListHead(Parms);
      gp_strncpy(state.obj.new_sect_name, ".idata_acs", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = eval_maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_DATA | STYP_ACCESS | STYP_ABS;
      break;

    default:
      eval_enforce_arity(Arity, 1);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_ident(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (PnIsString(p)) {
      coff_add_ident_sym(PnString(p));
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_idlocs(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  int           value;
  int           v;
  unsigned int  id_location;
  unsigned int  idreg;
  char          buf[BUFSIZ];
  MemBlock_t   *m;
  uint8_t       byte;
  uint16_t      word;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  id_location = gp_processor_id_location(state.processor);

  if (id_location == 0) {
    gperror_verror(GPE_ILLEGAL_DIR, NULL, Name);
    return Value;
  }

  if (IS_PIC16E_CORE) {
    if (eval_enforce_arity(Arity, 2)) {
      idreg = gp_processor_org_to_byte(state.device.class, eval_maybe_evaluate(PnListHead(Parms)));
      value = eval_maybe_evaluate(PnListHead(PnListTail(Parms)));
    }
    else {
      gperror_error(GPW_EXPECTED, "18cxxx devices should specify __IDLOC address.");
      return Value;
    }
  }
  else {
    if (eval_enforce_arity(Arity, 1)) {
      idreg = id_location;
      value = eval_maybe_evaluate(PnListHead(Parms));
    }
    else {
      return Value;
    }
  }

  if ((state.mode == MODE_RELOCATABLE) && (!state.found_idlocs)) {
    coff_new_section(".idlocs", idreg, STYP_ABS | STYP_TEXT);
    state.found_idlocs = true;
  }

  if (state.pass == 2) {
    m = (state.mode == MODE_RELOCATABLE) ? state.obj.section->data : state.c_memory;

    if (IS_PIC16E_CORE) {
      if (idreg < state.processor->idlocs_addrs[0]) {
        gperror_verror(GPE_RANGE, "Not a valid ID location. Address{0x%0*X} < IDLOCS_MIN{0x%0*X}",
                       state.device.class->addr_digits, idreg,
                       state.device.class->addr_digits, state.processor->idlocs_addrs[0]);
      }
      else if (idreg > state.processor->idlocs_addrs[1]) {
        gperror_verror(GPE_RANGE, "Not a valid ID location. Address{0x%0*X} > IDLOCS_MAX{0x%0*X}",
                       state.device.class->addr_digits, idreg,
                       state.device.class->addr_digits, state.processor->idlocs_addrs[1]);
      }
      else {
        state.lst.line.linetype = LTY_CONFIG;
        state.lst.config_address = idreg;

        if (value & ~0xff) {
          v      = value;
          value &= 0xff;
          snprintf(buf, sizeof(buf), "An ID Locations value too large. Last two hex digits used: 0x%X ==> 0x%02X", v, value);
          gperror_message(GPM_IDLOC, buf);
        }

        if (idreg <= state.device.id_location) {
          gperror_verror(GPE_IDLOCS_ORDER, NULL);
        }

        if (b_memory_get(m, idreg, &byte, NULL, NULL)) {
          gperror_verror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
        }

        snprintf(buf, sizeof(buf), "IDLOCS_%0*X", state.device.class->addr_digits, idreg);
        b_memory_put(m, idreg, value, buf, NULL);
        state.lst.line.was_byte_addr = idreg;
        coff_linenum(1);

        if (state.mode == MODE_RELOCATABLE) {
          state.byte_addr += 1;
        }
      }
    }
    else {
      state.lst.line.linetype = LTY_IDLOCS;

      if (value & ~0xffff) {
        v      = value;
        value &= 0xffff;
        gperror_vmessage(GPM_IDLOC, NULL, v, value);
      }

      if (state.device.class->i_memory_get(m, idreg, &word, NULL, NULL)) {
        gperror_verror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
      }

      snprintf(buf, sizeof(buf), "IDLOCS_%0*X", state.device.class->addr_digits, idreg);
      state.device.class->i_memory_put(m, idreg,     (value & 0xf000) >> 12, buf, NULL);
      snprintf(buf, sizeof(buf), "IDLOCS_%0*X", state.device.class->addr_digits, idreg + 2);
      state.device.class->i_memory_put(m, idreg + 2, (value & 0x0f00) >> 8,  buf, NULL);
      snprintf(buf, sizeof(buf), "IDLOCS_%0*X", state.device.class->addr_digits, idreg + 4);
      state.device.class->i_memory_put(m, idreg + 4, (value & 0x00f0) >> 4,  buf, NULL);
      snprintf(buf, sizeof(buf), "IDLOCS_%0*X", state.device.class->addr_digits, idreg + 6);
      state.device.class->i_memory_put(m, idreg + 6, (value & 0x000f),       buf, NULL);
      state.lst.line.was_byte_addr = idreg;
      coff_linenum(8);

      if (state.mode == MODE_RELOCATABLE) {
        state.byte_addr += 8;
      }
    }
  }
  state.device.id_location = idreg;

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/* Support IDLOCS "abcdef" or IDLOCS 'a', 'b', 'c' syntax for PIC16E devices. */

static gpasmVal
_do_16_idlocs(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  static unsigned int last_idreg = 0;

  const pnode_t *p;
  unsigned int   idreg;
  uint8_t        curvalue;
  const char    *pc;
  int            value;
  int            v;
  int            max_bytes;
  int            n;
  char           buf[BUFSIZ];
  MemBlock_t    *m;

  if (state.mpasm_compatible) {
    snprintf(buf, sizeof(buf), "Directive Error: The %s directive is invalid in MPASM(X) mode.", Name);
    gperror_error(GPE_UNKNOWN, buf);
    return Value;
  }

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (!IS_PIC16E_CORE) {
    gperror_error(GPE_IDLOCS_P16E, NULL);
    return Value;
  }

  idreg = (last_idreg == 0) ? gp_processor_id_location(state.processor) : last_idreg;

  if (idreg == 0) {
    snprintf(buf, sizeof(buf), "The IDLOCS registers not exist in the %s MCU.",
             state.processor->names[2]);
    gperror_error(GPE_UNKNOWN, buf);
    return Value;
  }

  if ((state.mode == MODE_RELOCATABLE) && (!state.found_idlocs)) {
    coff_new_section(".idlocs", idreg, STYP_ABS | STYP_TEXT);
    state.found_idlocs = true;
  }

  if (state.pass == 2) {
    m = (state.mode == MODE_RELOCATABLE) ? state.obj.section->data : state.c_memory;

    if (idreg < state.processor->idlocs_addrs[0]) {
      gperror_verror(GPE_RANGE, "Not a valid ID location. Address{0x%0*X} < IDLOCS_MIN{0x%0*X}",
                     state.device.class->addr_digits, idreg,
                     state.device.class->addr_digits, state.processor->idlocs_addrs[0]);
      return Value;
    }
    else if (idreg > state.processor->idlocs_addrs[1]) {
      gperror_verror(GPE_RANGE, "Not a valid ID location. Address{0x%0*X} > IDLOCS_MAX{0x%0*X}",
                     state.device.class->addr_digits, idreg,
                     state.device.class->addr_digits, state.processor->idlocs_addrs[1]);
      return Value;
    }

    assert(PnIsList(Parms));
    p = PnListHead(Parms);

    state.lst.line.linetype  = LTY_CONFIG;
    state.lst.config_address = idreg;
    max_bytes = state.processor->idlocs_addrs[1] - idreg + 1;
    n = 0;

    for (; Parms != NULL; Parms = PnListTail(Parms)) {
      p = PnListHead(Parms);

      switch (p->tag) {
      case PTAG_SYMBOL:
        value = eval_maybe_evaluate(p);
        goto constant;
        break;

      case PTAG_CONSTANT:
        value = PnConstant(p);
constant:

        if (n >= max_bytes) {
          gperror_warning(GPW_UNKNOWN, "The number of IDLOCS registers not more than 8!");
          goto warning;
        }

        if (value & ~0xff) {
          v = value;
          value &= 0xff;
          snprintf(buf, sizeof(buf), "An ID Locations value too large. Last two hex digits used: 0x%X ==> 0x%02X", v, value);
          gperror_message(GPM_IDLOC, buf);
        }

        if (b_memory_get(m, idreg, &curvalue, NULL, NULL)) {
          gperror_verror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
          return Value;
        }

        snprintf(buf, sizeof(buf), "IDLOCS_%0*X", state.device.class->addr_digits, idreg);
        b_memory_put(m, idreg, value, buf, NULL);
        state.lst.line.was_byte_addr = idreg;

        if (state.mode == MODE_RELOCATABLE) {
          state.byte_addr += 1;
        }

        ++n;
        ++idreg;
        break;

      case PTAG_STRING:
        pc = PnString(p);

        while (*pc != '\0') {
          if (n >= max_bytes) {
            gperror_warning(GPW_UNKNOWN, "The number of IDLOCS registers not more than 8!");
            goto warning;
          }

          pc = convert_escape_chars(pc, &value);

          if (b_memory_get(m, idreg, &curvalue, NULL, NULL)) {
            gperror_verror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
            return Value;
          }

          snprintf(buf, sizeof(buf), "IDLOCS_%0*X", state.device.class->addr_digits, idreg);
          b_memory_put(m, idreg, value, buf, NULL);
          state.lst.line.was_byte_addr = idreg;

          if (state.mode == MODE_RELOCATABLE) {
            state.byte_addr += 1;
          }

          ++n;
          ++idreg;
        }
        break;

      default:
        gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
        return Value;
      } /* switch (p->tag) */
    } /* for (; Parms != NULL; Parms = PnListTail(Parms)) */
warning:

    if (n > 0) {
      coff_linenum(n);
    }
  } /* if (state.pass == 2) */

  last_idreg = idreg;

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_if(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit   = false;

  _enter_if();

  /* Only evaluate the conditional if it matters... */
  if (state.astack->upper_enabled) {
    if (eval_enforce_arity(Arity, 1)) {
      p = PnListHead(Parms);
      state.astack->enabled             = eval_maybe_evaluate(p);
      state.astack->before_else_enabled = state.astack->enabled;
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_elif(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  char           buf[BUFSIZ];

  if (state.mpasm_compatible) {
    snprintf(buf, sizeof(buf), "Directive Error: The %s directive is invalid in MPASM(X) mode.", Name);
    gperror_error(GPE_UNKNOWN, buf);
    return Value;
  }

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit   = false;

  _enter_elif();

  /* Only evaluate the conditional elif it matters... */
  if (state.astack->upper_enabled) {
    if (eval_enforce_arity(Arity, 1)) {
      p = PnListHead(Parms);
      state.astack->enabled              = eval_maybe_evaluate(p);
      state.astack->before_else_enabled |= state.astack->enabled;
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_ifdef(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit   = false;

  _enter_if();

  /* Only evaluate the conditional ifdef it matters... */
  if (state.astack->upper_enabled) {
    if (eval_enforce_arity(Arity, 1)) {
      p = PnListHead(Parms);

      if (!PnIsSymbol(p)) {
        gperror_error(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if ((sym_get_symbol(state.stDefines, PnSymbol(p)) != NULL) ||
               (sym_get_symbol(state.stTop, PnSymbol(p)) != NULL)) {
        state.astack->enabled             = true;
        state.astack->before_else_enabled = state.astack->enabled;
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_elifdef(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  char           buf[BUFSIZ];

  if (state.mpasm_compatible) {
    snprintf(buf, sizeof(buf), "Directive Error: The %s directive is invalid in MPASM(X) mode.", Name);
    gperror_error(GPE_UNKNOWN, buf);
    return Value;
  }

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit   = false;

  _enter_elif();

  /* Only evaluate the conditional elifdef it matters... */
  if (state.astack->upper_enabled) {
    if (eval_enforce_arity(Arity, 1)) {
      p = PnListHead(Parms);

      if (!PnIsSymbol(p)) {
        gperror_error(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if ((sym_get_symbol(state.stDefines, PnSymbol(p)) != NULL) ||
               (sym_get_symbol(state.stTop, PnSymbol(p)) != NULL)) {
        state.astack->enabled              = true;
        state.astack->before_else_enabled |= state.astack->enabled;
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_ifndef(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit   = false;

  _enter_if();

  /* Only evaluate the conditional ifndef it matters... */
  if (state.astack->upper_enabled) {
    if (eval_enforce_arity(Arity, 1)) {
      p = PnListHead(Parms);

      if (!PnIsSymbol(p)) {
        gperror_error(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if ((sym_get_symbol(state.stDefines, PnSymbol(p)) == NULL) &&
               (sym_get_symbol(state.stTop, PnSymbol(p)) == NULL)) {
        state.astack->enabled             = true;
        state.astack->before_else_enabled = state.astack->enabled;
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_elifndef(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  char           buf[BUFSIZ];

  if (state.mpasm_compatible) {
    snprintf(buf, sizeof(buf), "Directive Error: The %s directive is invalid in MPASM(X) mode.", Name);
    gperror_error(GPE_UNKNOWN, buf);
    return Value;
  }

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit   = false;

  _enter_elif();

  /* Only evaluate the conditional elifndef it matters... */
  if (state.astack->upper_enabled) {
    if (eval_enforce_arity(Arity, 1)) {
      p = PnListHead(Parms);

      if (!PnIsSymbol(p)) {
        gperror_error(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if ((sym_get_symbol(state.stDefines, PnSymbol(p)) == NULL) &&
               (sym_get_symbol(state.stTop, PnSymbol(p)) == NULL)) {
        state.astack->enabled              = true;
        state.astack->before_else_enabled |= state.astack->enabled;
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_include(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (PnIsString(p)) {
      state.next_state = STATE_INCLUDE;
      state.next_buffer.file = GP_Strdup(PnString(p));
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_line(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else if (eval_enforce_arity(Arity, 1)) {
    if (state.debug_info) {
      p = PnListHead(Parms);
      state.obj.debug_line = eval_maybe_evaluate(p);
    }
    else {
      gperror_warning(GPW_UNKNOWN, "Directive ignored when debug info is disabled.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/************************************************************************
 * do_list - parse the LIST directive
 *
 * If the file gpasm is assembling contains a LIST directive, then scan
 * and parse will call do_list and pass the comma delimited list of LIST
 * options in *Parms
 *
 * Inputs:
 *   gpasmVal Value - not used, but is returned
 *   char *Name     - not used, but contains the directive name 'list'
 *   int Arity      - not used, but should contain '1'
 *   pnode_t *Parms - a linked list of the parsed parameters
 *
 ************************************************************************/

static gpasmVal
_do_list(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  const char    *lhs;
  int            value;
  char           message[BUFSIZ];

  state.lst.enabled       = true;
  state.lst.line.linetype = LTY_DIR;

  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (PnIsBinOp(p) && (PnBinOpOp(p) == '=')) {
      if (eval_enforce_simple(PnBinOpP0(p))) {
        lhs = PnSymbol(PnBinOpP0(p));

        if (strcasecmp(lhs, "b") == 0) {
          value = eval_maybe_evaluate(PnBinOpP1(p));

          if (value != 0) {
            state.lst.tabstop = value;
          }
        }
        else if (strcasecmp(lhs, "c") == 0) {
          value = eval_maybe_evaluate(PnBinOpP1(p));

          if (value > LST_SRC_POS) {
            state.lst.line_width = value;
          }
          else {
            snprintf(message, sizeof(message),
                     "Argument out of range: Column_width{%i} <= LST_SRC_POS{%i}", value, LST_SRC_POS);
            gperror_error(GPE_RANGE, message);
          }
        }
        else if (strcasecmp(lhs, "f") == 0) {
          if (eval_enforce_simple(PnBinOpP1(p))) {
            select_hexformat(PnSymbol(PnBinOpP1(p)));
          }
        }
        else if (strcasecmp(lhs, "l") == 0) {
          ; /* Ignore this for now: Page length. */
        }
        else if (strcasecmp(lhs, "m") == 0) {
          /* Undocumented, only found in MEMORY.INC and MCP250XX.INC. */
          if (eval_can_evaluate(PnBinOpP1(p))) {
            value = eval_evaluate(PnBinOpP1(p));

            if (value < state.maxrom) {
              snprintf(message, sizeof(message),
                       "Argument out of range, must be greater than or equal to %ld.", state.maxrom);
              gperror_error(GPE_RANGE, message);
            }
            else {
              state.maxrom = value;
            }
          }
        }
        else if (strcasecmp(lhs, "mm") == 0) {
          state.lst.memory_map = _off_or_on(PnBinOpP1(p));
        }
        else if (strcasecmp(lhs, "n") == 0) {
          if (eval_can_evaluate(PnBinOpP1(p))) {
            int number = eval_evaluate(PnBinOpP1(p));

            if ((number > 6) || (number == 0)) {
              state.lst.lines_per_page = number;
            }
            else {
              gperror_verror(GPE_RANGE, "0 < n{%i} <= 6", number);
            }
          }
        }
        else if (strcasecmp(lhs, "p") == 0) {
          if (eval_enforce_simple(PnBinOpP1(p))) {
            select_processor(PnSymbol(PnBinOpP1(p)));
          }
        }
        else if (strcasecmp(lhs, "pe") == 0) {
          state.extended_pic16e = true;

          if (eval_enforce_simple(PnBinOpP1(p))) {
            select_processor(PnSymbol(PnBinOpP1(p)));
          }
        }
        else if (strcasecmp(lhs, "r") == 0) {
          if (eval_enforce_simple(PnBinOpP1(p))) {
            select_radix(PnSymbol(PnBinOpP1(p)));
          }
        }
        else if (strcasecmp(lhs, "st") == 0) {
          state.lst.symbol_table = _off_or_on(PnBinOpP1(p));
        }
        else if (strcasecmp(lhs, "t") == 0) {
          ; /* Ignore this for now: Always wrap long list lines. */
        }
        else if (strcasecmp(lhs, "w") == 0) {
          select_errorlevel(eval_maybe_evaluate(PnBinOpP1(p)));
        }
        else if (strcasecmp(lhs, "x") == 0) {
          if (eval_enforce_simple(PnBinOpP1(p))) {
            select_expand(PnSymbol(PnBinOpP1(p)));
          }
        }
        else {
          gperror_verror(GPE_ILLEGAL_ARGU, NULL, lhs);
        }
      }
    }
    else {
      if (eval_enforce_simple(p)) {
        if (strcasecmp(PnSymbol(p), "free") == 0) {
          ; /* Ignore this directive */
        }
        else if (strcasecmp(PnSymbol(p), "fixed") == 0) {
          ; /* Ignore this directive */
        }
        else if (strcasecmp(PnSymbol(p), "nowrap") == 0) {
          ; /* Ignore this directive */
        }
        else if (strcasecmp(PnSymbol(p), "wrap") == 0) {
          ; /* Ignore this directive */
        }
        else {
          gperror_verror(GPE_ILLEGAL_ARGU, NULL, PnSymbol(p));
        }
      }
    }
  }

  /* The .list symbol is only added to the COFF file if its only action is to turn on the listing. */
  if (Arity == 0) {
    coff_add_list_sym();
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_local(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  gp_boolean     first;
  const char    *lhs;
  gpasmVal       val;

  if (_check_processor_select(Name)) {
    return Value;
  }

  first = true;
  state.lst.line.linetype = LTY_SET4;

  /* like variable except it is put in TOP instead of GLOBAL */

  if (state.stGlobal == state.stTop) {
    gperror_error(GPE_UNKNOWN, "Attempt to use \"local\" outside of macro.");
  }
  else {
    for (; Parms != NULL; Parms = PnListTail(Parms)) {
      p = PnListHead(Parms);

      if (PnIsBinOp(p) && (PnBinOpOp(p) == '=')) {
        if (eval_enforce_simple(PnBinOpP0(p))) {
          /* fetch the symbol */
          lhs = PnSymbol(PnBinOpP0(p));
          val = eval_maybe_evaluate(PnBinOpP1(p));
          /* put the symbol and value in the TOP table*/
          sym_add_symbol(state.stTop, lhs);
          set_global(lhs, val, LFT_TEMPORARY, VAL_CONSTANT, false);

          if (first) {
            Value = val;
            first = false;
          }
        }
      }
      else if (PnIsSymbol(p)) {
        /* put the symbol in the Top table */
        sym_add_symbol(state.stTop, PnSymbol(p));

        if (first) {
          Value = 0;
          first = false;
        }
      }
      else {
        gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_noexpand(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (state.cmd_line.macro_expand) {
    gperror_vmessage(GPM_SUPLIN, NULL);
  }
  else {
    if (eval_enforce_arity(Arity, 0)) {
      state.lst.expand = false;
    }
  }
  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_nolist(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (!state.lst.force) {
    state.lst.enabled = false;
  }

  coff_add_nolist_sym();

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_maxram(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_SET;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (eval_can_evaluate(p)) {
      Value = state.maxram = eval_evaluate(p);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_maxrom(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_SET;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (eval_can_evaluate(p)) {
      Value = state.maxrom = eval_evaluate(p);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_macro(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  macro_head_t *head;

  if (_check_processor_select(Name)) {
    return Value;
  }

  head = GP_Malloc(sizeof(*head));

  head->parms   = Parms;
  head->body    = NULL;
  head->defined = false;
  /* Record data for the list, cod, and coff files. */
  head->line_number = state.src->line_number;
  head->file_symbol = state.src->file_symbol;

  head->src_name = GP_Strdup(state.src->name);

  state.lst.line.linetype = LTY_DIR;

  if (_macro_parms_ok(Parms)) {
    state.mac_head = head;
  }

  state.mac_prev = &(head->body);
  state.mac_body = NULL;

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_messg(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  const char    *str;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (PnIsString(p)) {
      if ((str = _hv_macro_resolver(PnString(p))) != NULL) {
        gperror_vmessage(GPM_USER, NULL, str);
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_org(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  gpasmVal       new_byte_addr;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (eval_can_evaluate(p)) {
      Value = eval_evaluate(p);
      new_byte_addr = gp_processor_real_to_byte(state.processor, Value);
      if (state.mpasm_compatible ||
          ((gp_processor_is_config_org(state.processor, Value) < 0) &&
           (gp_processor_is_eeprom_org(state.processor, Value) < 0))) {
        if (IS_PIC16E_CORE && (Value & 1)) {
          gperror_verror(GPE_ORG_ODD, "Address{0x%0*X}", state.device.class->addr_digits, Value);
        }
      }

      if (state.mode == MODE_ABSOLUTE) {
        state.lst.line.linetype = LTY_ORG;
        state.byte_addr = new_byte_addr;
      }
      else {
        /* Default section name, this will be overwritten if a label is present. */
        snprintf(state.obj.new_sect_name, sizeof(state.obj.new_sect_name), ".org_%0*X",
                 state.device.class->addr_digits, Value);
        state.obj.new_sect_addr  = new_byte_addr;
        state.obj.new_sect_flags = STYP_TEXT | STYP_ABS;
        state.lst.line.linetype  = LTY_SEC;
        state.next_state = STATE_SECTION;
      }
    }
    else {
      Value = 0;
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_page(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (eval_enforce_arity(Arity, 0) && state.lst.enabled) {
    lst_page_start();
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

/* Called by both do_pagesel and do_pageselw, which have a very slight
 * difference between them. */

static gpasmVal
_do_pagesel(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms, uint16_t reloc_type)
{
  const pnode_t *p;
  int            page;
  int            num_reloc;
  gp_boolean     use_wreg = false;

  if ((reloc_type == RELOCT_PAGESEL_WREG) || IS_PIC16_CORE) {
    use_wreg = true;
  }

  if (IS_EEPROM8 || IS_EEPROM16 || IS_PIC16E_CORE || (state.processor->num_pages == 1)) {
    state.lst.line.linetype = LTY_NONE;
    gperror_vmessage(GPM_EXTPAGE, NULL);
    return Value;
  }

  if (IS_PIC16_CORE) {
    gperror_vmessage(GPM_W_MODIFIED, NULL);
  }

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (state.mode == MODE_ABSOLUTE) {
      page = gp_processor_check_page(state.device.class, eval_maybe_evaluate(p));
      state.byte_addr += gp_processor_set_page(state.device.class,
                                               state.processor->num_pages,
                                               page,
                                               state.i_memory,
                                               state.byte_addr,
                                               use_wreg);
      if (!state.mpasm_compatible) {
        set_global(GLOBAL_ACT_PAGE_ADDR,
                   gp_processor_page_bits_to_addr(state.device.class, page),
                   LFT_TEMPORARY, VAL_CONSTANT, true);
      }
    }
    else {
      num_reloc = eval_count_reloc(p);

      if (num_reloc == 0) {
        /* it is an absolute address, generate the pagesel but no relocation */
        page = gp_processor_check_page(state.device.class, eval_maybe_evaluate(p));
        state.byte_addr += gp_processor_set_page(state.device.class, state.processor->num_pages,
                                                 page, state.i_memory, state.byte_addr, use_wreg);
      }
      else if (num_reloc != 1) {
        gperror_error(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if (IS_PIC16_CORE) {
        eval_reloc_evaluate(p, RELOCT_PAGESEL_WREG, NULL, NULL);
        _emit(0x0000, Name);
        _emit(0x0000, Name);
      }
      else if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
        if (!use_wreg) {
          eval_reloc_evaluate(p, RELOCT_PAGESEL_MOVLP, NULL, NULL);
          _emit(PIC14E_INSN_MOVLP, Name);
        }
        else {
          eval_reloc_evaluate(p, RELOCT_PAGESEL_WREG, NULL, NULL);
          _emit(0x0000, Name);
          _emit(0x0000, Name);
        }
      }
      else {
        if (!use_wreg && (state.processor->num_pages == 2)) {
          eval_reloc_evaluate(p, RELOCT_PAGESEL_BITS, NULL, NULL);
          _emit(0, Name);
        }
        else if (state.processor->num_pages >= 2) {
          eval_reloc_evaluate(p, reloc_type, NULL, NULL);
          _emit(0x0000, Name);
          _emit(0x0000, Name);
        }
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_pagesel_wrapper(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (prev_btfsx) {
    gperror_vwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Pagesel");
  }

  return _do_pagesel(Value, Name, Arity, Parms, RELOCT_PAGESEL_BITS);
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_pageselw_wrapper(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (prev_btfsx) {
    gperror_vwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Pageselw");
  }

  return _do_pagesel(Value, Name, Arity, Parms, RELOCT_PAGESEL_WREG);
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_processor(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (eval_enforce_simple(p)) {
      select_processor(PnSymbol(p));
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_radix(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (eval_enforce_simple(p)) {
      select_radix(PnSymbol(p));
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_res(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  int            count;
  int            i;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (eval_can_evaluate(p)) {
      count = eval_evaluate(p);

      state.lst.line.linetype = LTY_RES;

      if (state.mode == MODE_ABSOLUTE) {
        if (IS_PIC16E_CORE && ((count % 2) != 0)) {
          gperror_verror(GPE_RES_ODD_PIC16EA, "res = %i", count);
        }

        count = gp_processor_org_to_byte(state.device.class, count);

        for (i = 0; (i + 1) < count; i += 2) {
          _emit(state.device.class->core_mask, Name);
        }
      }
      else {
        if (SECTION_FLAGS & STYP_TEXT) {
          count = gp_processor_org_to_byte(state.device.class, count);

          if (state.device.class->rom_width < 16) {
            /* FIXME: Most likely this check belongs to our caller. */
            if (count < 2) {
              gperror_warning(GPW_UNKNOWN, "No memory has been reserved by this instruction.");
            }
          }
          for (i = 0; (i + 1) < count; i += 2) {
            /* For some reason program memory is filled with a different value. */
            _emit(state.device.class->core_mask, Name);
          }
        }
        else {
          for (i = 0; i < count; i++) {
            _emit_byte(0, Name);
          }
        }
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_set(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_SET;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (eval_can_evaluate(p)) {
      Value = eval_evaluate(p);
    }
    else {
      Value = 0;
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_space(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  int            i;

  state.lst.line.linetype = LTY_DIR;

  if (state.pass == 2) {
    switch (Arity) {
      case 0:
        /* do nothing */
        break;

      case 1:
        p = PnListHead(Parms);

        if (eval_can_evaluate(p)) {
          for (i = eval_evaluate(p); i > 0; i--) {
            lst_line("");
          }
        }
        break;

      default:
        eval_enforce_arity(Arity, 1);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_subtitle(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (PnIsString(p)) {
      gp_strncpy(state.lst.subtitle_name, PnString(p), sizeof(state.lst.subtitle_name));
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_title(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_NONE;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (PnIsString(p)) {
      gp_strncpy(state.lst.title_name, PnString(p), sizeof(state.lst.title_name));
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_type(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  const char    *symbol_name;
  gp_symbol_t   *coff_symbol;
  int            value;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else if (eval_enforce_arity(Arity, 2)) {
    /* the first argument is the symbol name */
    p = PnListHead(Parms);

    if (eval_enforce_simple(p)) {
      symbol_name = PnSymbol(p);
      coff_symbol = gp_coffgen_find_symbol(state.obj.object, symbol_name);

      if (coff_symbol == NULL) {
        gperror_verror(GPE_NOSYM, NULL, symbol_name);
      }
      else {
        p = PnListHead(PnListTail(Parms));
        value = eval_maybe_evaluate(p);

        if (value < 0) {
          gperror_verror(GPE_RANGE, "type{%i (%#x)} < 0", value, value);
        }
        else if (value > 0xffff) {
          gperror_verror(GPE_RANGE, "type{%i (%#x)} > 0xffff", value, value);
        }
        else {
          coff_symbol->type = value;
        }
      }
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_udata(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else {
    switch (Arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".udata", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_BSS;
      break;

    case 1:
      /* new absolute section */
      p = PnListHead(Parms);
      gp_strncpy(state.obj.new_sect_name, ".udata", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = eval_maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_BSS | STYP_ABS;
      break;

    default:
      eval_enforce_arity(Arity, 1);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_udata_acs(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else {
    switch (Arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".udata_acs", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_BSS | STYP_ACCESS;
      break;

    case 1:
      /* new absolute section */
      p = PnListHead(Parms);
      gp_strncpy(state.obj.new_sect_name, ".udata_acs", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = eval_maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_BSS | STYP_ACCESS | STYP_ABS;
      break;

    default:
      eval_enforce_arity(Arity, 1);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_udata_ovr(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else {
    switch (Arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".udata_ovr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_BSS | STYP_OVERLAY;
      break;

    case 1:
      /* new absolute section */
      p = PnListHead(Parms);
      gp_strncpy(state.obj.new_sect_name, ".udata_ovr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = eval_maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_BSS | STYP_OVERLAY | STYP_ABS;
      break;

    default:
      eval_enforce_arity(Arity, 1);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_udata_shr(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Name);
    return Value;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, Name);
  }
  else {
    switch (Arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".udata_shr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_BSS | STYP_SHARED;
      break;

    case 1:
      /* new absolute section */
      p = PnListHead(Parms);
      gp_strncpy(state.obj.new_sect_name, ".udata_shr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = eval_maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_BSS | STYP_SHARED | STYP_ABS;
      break;

    default:
      eval_enforce_arity(Arity, 1);
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_undefine(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;

  if (_check_processor_select(Name)) {
    return Value;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (eval_enforce_arity(Arity, 1)) {
    p = PnListHead(Parms);

    if (PnIsSymbol(p)) {
      if (!sym_remove_symbol(state.stDefines, PnSymbol(p))) {
        gperror_vwarning(GPW_NOT_DEFINED, NULL, PnSymbol(p));
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_variable(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  const pnode_t *p;
  gp_boolean     first;
  const char    *lhs;
  gpasmVal       val;

  if (_check_processor_select(Name)) {
    return Value;
  }

  first = true;
  state.lst.line.linetype = LTY_SET4;

  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (PnIsBinOp(p) && (PnBinOpOp(p) == '=')) {
      if (eval_enforce_simple(PnBinOpP0(p))) {
        /* fetch the symbol */
        lhs = PnSymbol(PnBinOpP0(p));
        val = eval_maybe_evaluate(PnBinOpP1(p));
        /* put the symbol and value in the table */
        set_global(lhs, val, LFT_TEMPORARY, VAL_CONSTANT, false);

        if (first) {
          Value = val;
          first = false;
        }
      }
    }
    else if (PnIsSymbol(p)) {
      /* put the symbol with a 0 value in the table */
      set_global(PnSymbol(p), 0, LFT_TEMPORARY, VAL_CONSTANT, false);

      if (first) {
        Value = 0;
        first = false;
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_while(gpasmVal Value, const char *Name, int Arity, pnode_t *Parms)
{
  macro_head_t *head;

  if (_check_processor_select(Name)) {
    return Value;
  }

  assert(state.while_depth == 0);

  head = GP_Malloc(sizeof(*head));
  state.lst.line.linetype = LTY_DOLIST_DIR;
  head->parms = (eval_enforce_arity(Arity, 1)) ? PnListHead(Parms) : NULL;
  head->body  = NULL;

  /* Record data for the list, cod, and coff files. */
  head->line_number = state.src->line_number;
  head->file_symbol = state.src->file_symbol;

  head->src_name = GP_Strdup(state.src->name);

  /* DON'T set up state.mac_head; this isn't a macro head. */
  state.while_head = head;

  state.mac_prev = &(head->body);
  state.mac_body = NULL;

  state.while_depth = 1;

  return Value;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
asm_enabled(void)
{
  return ((state.astack == NULL) || (state.astack->enabled && state.astack->upper_enabled));
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_core_sfr_or_common_ram(int Reg_address)
{
  if (gp_processor_find_sfr(state.device.class, Reg_address) != NULL) {
    /* This is a core SFR. */
    return true;
  }

  if (gp_processor_is_common_ram_addr(state.processor, Reg_address) >= 0) {
    /* This is a common GPR. */
    return true;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

static void
_emit_check(unsigned int Insn, int Argument, int Mask, const char *Name)
{
  int test = Argument;
  int v    = Argument & Mask;

  if (test < 0) {
    test = -test;
  }

  /* If there are bits that shouldn't be set then issue a warning. */
  if (test & (~Mask)) {
    gperror_vwarning(GPW_RANGE, "%i (%#x) => %i (%#x)", Argument, Argument, v, v);
  }

  _emit(Insn | v, Name);
}

/*------------------------------------------------------------------------------------------------*/

/*
   For relative branches, issue a warning if the absolute value of
   argument is greater than range.
*/

static void
_emit_check_relative(unsigned int Insn, int Argument, int Mask, int Range, const char *Name)
{
  char full_message[BUFSIZ];

  /* If the branch is too far then issue an error */
  if ((Argument > Range) || (Argument < -(Range + 1))) {
    snprintf(full_message, sizeof(full_message),
             "Argument out of range (%d not between %d and %d).", Argument, -(Range + 1), Range);
    gperror_error(GPE_RANGE, full_message);
  }

  _emit(Insn | (Argument & Mask), Name);
}

/*------------------------------------------------------------------------------------------------*/

static int
_check_flag(int Flag)
{
  if ((Flag != 0) && (Flag != 1)) {
    gperror_vwarning(GPW_RANGE, "%i (%#x)", Flag, Flag);
  }

  return (Flag & 0x1);
}

/*------------------------------------------------------------------------------------------------*/

/* values and masks for types_mask */
#define AR_BITS             1
#define AR_MASK             ((1 << AR_BITS) - 1)
#define AR_GET(types, i)    (((types) >> (AR_BITS * (i))) & AR_MASK)
/* direct addressing */
#define AR_DIRECT           0
/* indexed addressing with literal offset */
#define AR_INDEX            1
/* Byte-Oriented and Bit-Oriented instructions in Indexed Literal Offset Mode. */
#define AR_BIT_BYTE         0x8000

static gp_boolean
_check_16e_arg_types(const pnode_t *Parms, int Arity, unsigned int Types)
{
  const pnode_t *p;
  const pnode_t *p1;
  int            i;
  int            val;
  gp_boolean     ret = true;
  char           buf[80];

  if (state.extended_pic16e) {
    p = Parms;
    for (i = 0; i < Arity; ++i) {
      assert(p != NULL);

      if ((AR_GET(Types, i) != AR_INDEX) && PnIsOffset(PnListHead(p))) {
        gperror_error(GPE_BADCHAR, "Illegal character ([).");
        ret = false;
      }
      else if (AR_BIT_BYTE & Types) {
        if ((AR_GET(Types, i) == AR_INDEX) && PnIsOffset(PnListHead(p))) {
          p1 = PnOffset(PnListHead(p));

          if (eval_can_evaluate_value(p1)) {
            val = eval_evaluate(p1);

            if ((val < 0) || (val > 0x5f)) {
              snprintf(buf, sizeof(buf), "Argument out of range (%i (%#x) not between 0 and 95).", val, val);
              gperror_error(GPE_RANGE, buf);
              ret = false;
            }
          }
          else {
            gperror_verror(GPE_CONSTANT, NULL);
            ret = false;
          }
        }
      }
      else if ((AR_GET(Types, i) == AR_INDEX) && !PnIsOffset(PnListHead(p))) {
        gperror_verror(GPE_MISSING_BRACKET, NULL);
        ret = false;
      }
      p = PnListTail(p);
    }
  }

  return ret;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_check_and_set_bank_bit(enum common_insn Icode, int Bit, int BankSel0, int BankSel1, int BankSel2)
{
  variable_t *var;
  int         addr;
  int         num_of_banks;

  num_of_banks = gp_processor_num_banks(state.processor);

  if (num_of_banks < 2) {
    /* There is no option to choose. */
    return true;
  }

  if ((var = get_global_constant(GLOBAL_ACT_BANK_ADDR)) == NULL) {
    gperror_verror(GPE_INTERNAL, NULL, "The \"" GLOBAL_ACT_BANK_ADDR "\" variable not exists.");
    return false;
  }

  addr = gp_processor_bank_num_to_addr(state.processor, 1);

  if (var->value < 0) {
    /* This value is not valid Bank address. */
    if ((num_of_banks == 2) && (Bit == BankSel0)) {
      /* If exist only two RAM Bank, then choice be evident. */
      if (Icode == ICODE_BCF) {
        /* bcf FSR, 5 or bcf STATUS, RP0 */
        var->value = 0;
      }
      else if (Icode == ICODE_BSF) {
        /* bsf FSR, 5 or bsf STATUS, RP0 */
        var->value = addr;
      }
    }
    return true;
  }

  if (Bit == BankSel0) {
    if (Icode == ICODE_BCF) {
      /* bcf FSR, 5 or bcf STATUS, RP0 */
      var->value &= ~addr;
    }
    else if (Icode == ICODE_BSF) {
      /* bsf FSR, 5 or bsf STATUS, RP0 */
      var->value |= addr;
    }
  }
  else if (Bit == BankSel1) {
    addr = gp_processor_bank_num_to_addr(state.processor, 2);

    if (Icode == ICODE_BCF) {
      /* bcf FSR, 6 or bcf STATUS, RP1 */
      var->value &= ~addr;
    }
    else if (Icode == ICODE_BSF) {
      /* bsf FSR, 6 or bsf STATUS, RP1 */
      var->value |= addr;
    }
  }
  else if (Bit == BankSel2) {
    /* 16F59 */
    addr = gp_processor_bank_num_to_addr(state.processor, 4);

    if (Icode == ICODE_BCF) {
      /* bcf FSR, 7 */
      var->value &= ~addr;
    }
    else if (Icode == ICODE_BSF) {
      /* bsf FSR, 7 */
      var->value |= addr;
    }
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_check_and_set_page_bit(enum common_insn Icode, int Bit, int PageSel0, int PageSel1, int PageSel2)
{
  variable_t *var;
  int         addr;
  int         num_of_pages;

  num_of_pages = gp_processor_num_pages(state.processor);

  if (num_of_pages < 2) {
    /* There is no option to choose. */
    return true;
  }

  if ((var = get_global_constant(GLOBAL_ACT_PAGE_ADDR)) == NULL) {
    gperror_verror(GPE_INTERNAL, NULL, "The \"" GLOBAL_ACT_PAGE_ADDR "\" variable not exists.");
    return false;
  }

  addr = gp_processor_page_bits_to_addr(state.device.class, 1);

  if (var->value < 0) {
    /* This value is not valid Page address. */
    if ((num_of_pages == 2) && (Bit == PageSel0)) {
      /* If exist only two ROM Page, then choice be evident. */
      if (Icode == ICODE_BCF) {
        /* bcf STATUS, PA0 or bcf PCLATH, 3 */
        var->value = 0;
      }
      else if (Icode == ICODE_BSF) {
        /* bsf STATUS, PA0 or bsf PCLATH, 3 */
        var->value = addr;
      }
    }
    return true;
  }

  if (Bit == PageSel0) {
    if (Icode == ICODE_BCF) {
      /* bcf STATUS, PA0 or bcf PCLATH, 3 */
      var->value &= ~addr;
    }
    else if (Icode == ICODE_BSF) {
      /* bsf STATUS, PA0 or bsf PCLATH, 3 */
      var->value |= addr;
    }
  }
  else if (Bit == PageSel1) {
    addr = gp_processor_page_bits_to_addr(state.device.class, 2);

    if (Icode == ICODE_BCF) {
      /* bcf STATUS, PA1 or bcf PCLATH, 4 */
      var->value &= ~addr;
    }
    else if (Icode == ICODE_BSF) {
      /* bsf STATUS, PA1 or bsf PCLATH, 4 */
      var->value |= addr;
    }
  }
  else if (Bit == PageSel2) {
    addr = gp_processor_page_bits_to_addr(state.device.class, 4);

    if (Icode == ICODE_BCF) {
      /* bcf STATUS, PA2 */
      var->value &= ~addr;
    }
    else if (Icode == ICODE_BSF) {
      /* bsf STATUS, PA2 */
      var->value |= addr;
    }
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* Create the message: The register is (not) located on the Access RAM. */

static void
_msg_this_access_ram(unsigned int Reg_address, const char *Reg_name, gp_boolean Is_access)
{
  unsigned int   bank_addr;
  unsigned int   bank_num;
  unsigned int   reg_offs;
  void         (*msg)(int, const char *, ...);
  int            code;
  int            word_digits;

  if (state.strict_level <= 0) {
    return;
  }

  bank_addr   = gp_processor_bank_addr(state.processor, Reg_address);
  bank_num    = gp_processor_bank_num(state.processor, Reg_address);
  reg_offs    = gp_processor_reg_offs(state.processor, Reg_address);
  word_digits = (state.device.class != NULL) ? state.device.class->word_digits : 4;

  if (state.strict_level == 2) {
    msg  = gperror_verror;
    code = (Is_access) ? GPE_IS_ACCRAM : GPE_NO_ACCRAM;
  }
  else {
    msg  = gperror_vwarning;
    code = (Is_access) ? GPW_IS_ACCRAM : GPW_NO_ACCRAM;
  }

  if (Reg_name != NULL) {
    (*msg)(code,
           "'%s' (Addr: 0x%0*X; Bank_%u: 0x%0*X; Offs: 0x%0*X)",
           Reg_name,
           word_digits, Reg_address,
           bank_num,
           word_digits, bank_addr,
           word_digits, reg_offs);
  }
  else {
    (*msg)(code,
           "Addr: 0x%0*X (Bank_%u: 0x%0*X; Offs: 0x%0*X)",
           word_digits, Reg_address,
           bank_num,
           word_digits, bank_addr,
           word_digits, reg_offs);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_msg_access_nosel_def(int Reg_address, const char *Reg_name)
{
  unsigned int   bank_addr;
  unsigned int   bank_num;
  unsigned int   reg_offs;
  void         (*msg)(int, const char *, ...);
  int            code;
  int            word_digits;

  if (state.strict_level <= 0) {
    return;
  }

  if (state.strict_level == 2) {
    msg  = gperror_verror;
    code = GPE_ACC_NOSEL;
  }
  else {
    msg  = gperror_vmessage;
    code = GPM_ACC_DEF;
  }

  if (Reg_address >= 0) {
    bank_addr   = gp_processor_bank_addr(state.processor, Reg_address);
    bank_num    = gp_processor_bank_num(state.processor, Reg_address);
    reg_offs    = gp_processor_reg_offs(state.processor, Reg_address);
    word_digits = (state.device.class != NULL) ? state.device.class->word_digits : 4;

    if (Reg_name != NULL) {
      (*msg)(code,
             "'%s' (Addr: 0x%0*X; Bank_%u: 0x%0*X; Offs: 0x%0*X)",
             Reg_name,
             word_digits, Reg_address,
             bank_num,
             word_digits, bank_addr,
             word_digits, reg_offs);
    }
    else {
      (*msg)(code,
             "Addr: 0x%0*X (Bank_%u: 0x%0*X; Offs: 0x%0*X)",
             word_digits, Reg_address,
             bank_num,
             word_digits, bank_addr,
             word_digits, reg_offs);
    }
  }
  else {
    if (Reg_name != NULL) {
      (*msg)(code,
             "'%s' (Addr: 0x?\?\?\?; Bank_?: 0x?\?\?\?; Offs: 0x?\?\?\?)",
             Reg_name);
    }
    else {
      (*msg)(code,
             "Addr: 0x?\?\?\? (Bank_?: 0x?\?\?\?; Offs: 0x?\?\?\?)");
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_msg_ram_bank(int Reg_address, const char *Reg_name, int Bank_number)
{
  unsigned int bank_addr;
  unsigned int bank_num;
  unsigned int reg_offs;
  int          word_digits;

  if (state.strict_level <= 0) {
    return;
  }

  if (Bank_number < 0) {
    Bank_number = 0;
  }

  if (Reg_address >= 0) {
    bank_addr   = gp_processor_bank_addr(state.processor, Reg_address);
    bank_num    = gp_processor_bank_num(state.processor, Reg_address);
    reg_offs    = gp_processor_reg_offs(state.processor, Reg_address);
    word_digits = (state.device.class != NULL) ? state.device.class->word_digits : 4;

    if (Reg_name != NULL) {
      gperror_vmessage(GPM_BANK,
                       "'%s' (Addr: 0x%0*X; Bank_%u: 0x%0*X; Offs: 0x%0*X)", Bank_number,
                       Reg_name,
                       word_digits, Reg_address,
                       bank_num,
                       word_digits, bank_addr,
                       word_digits, reg_offs);
    }
    else {
      gperror_vmessage(GPM_BANK,
                       "Addr: 0x%0*X (Bank_%u: 0x%0*X; Offs: 0x%0*X)", Bank_number,
                       word_digits, Reg_address,
                       bank_num,
                       word_digits, bank_addr,
                       word_digits, reg_offs);
    }
  }
  else {
    if (Reg_name != NULL) {
      gperror_vmessage(GPM_BANK, 
                       "'%s' (Addr: 0x?\?\?\?; Bank_?: 0x?\?\?\?; Offs: 0x?\?\?\?)", -1,
                       Reg_name);
    }
    else {
      gperror_vmessage(GPM_BANK,
                       "Addr: 0x?\?\?\? (Bank_?: 0x?\?\?\?; Offs: 0x?\?\?\?)", -1);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_reg_addr_check_reloc(int Reg_address, const char *Reg_name, unsigned int Insn_flags,
                      gpasmVal Reloc_value, gp_boolean Need_bank_check)
{
  int               bank_addr;
  insn_flags_t      i_flags;
  const variable_t *var;
  int               bank_assume;
  int               bank_assume_num;

  i_flags.u = Insn_flags;

  if ((Reg_address == 0) && i_flags.isReloc) {
    Reg_address = Reloc_value;
  }

  switch (i_flags.dest_mode) {
    case IFLAG_DEST_NONE:
      break;

    case IFLAG_DEST_DEF: {
      if (i_flags.dest == IFLAG_DEST_DEF_FILE) {
        /* Default selected a file register. */
        if (state.strict_level == 2) {
          gperror_verror(GPE_NOF, NULL);
        }
        else {
          gperror_vmessage(GPM_NOF, NULL);
        }
      }
      break;
    }

    case IFLAG_DEST_SEL:
      break;
  }

  switch (i_flags.bank_mode) {
    case IFLAG_BANK_NONE: {
      if ((i_flags.isReloc) || (Need_bank_check)) {
        if (!state.mpasm_compatible) {
          /* The __ACTIVE_BANK_ADDR variable shows the register which used for BANKSEL directive last time. */
          if ((var = get_global_constant(GLOBAL_ACT_BANK_ADDR)) == NULL) {
            gperror_verror(GPE_INTERNAL, NULL, "The \"" GLOBAL_ACT_BANK_ADDR "\" variable not exists.");
            return;
          }

          bank_assume = var->value;
        }
        else {
          bank_assume = -1;
        }

        if (bank_assume >= 0) {
          bank_assume     = gp_processor_bank_addr(state.processor, bank_assume);
          bank_assume_num = gp_processor_bank_num(state.processor, bank_assume);
          bank_addr       = (Reg_address >= 0) ? gp_processor_bank_addr(state.processor, Reg_address) : -1;

          if (bank_assume != bank_addr) {
            _msg_ram_bank(Reg_address, Reg_name, bank_assume_num);
          }
        }
        else {
          _msg_ram_bank(Reg_address, Reg_name, -1);
        }
      }
      break;
    }

    case IFLAG_BANK_DEF: {
      if (i_flags.bank == IFLAG_BANK_DEF_ACCESS) {
        /* Default selected the Access Area. */
        if (i_flags.isAccess) {
          _msg_access_nosel_def(Reg_address, Reg_name);
        }
        else {
          _msg_ram_bank(Reg_address, Reg_name, -1);
        }
      }
      else if (i_flags.bank == IFLAG_BANK_DEF_BANK) {
        /* Default selected a RAM Bank. */
        if (i_flags.isAccessSFR) {
          _msg_this_access_ram(Reg_address, Reg_name, true);
        }
      }
      break;
    }

    case IFLAG_BANK_SEL: {
      if (i_flags.bank == IFLAG_BANK_SEL_ACCESS) {
        /* Selected the Access Area. */
        if (!i_flags.isAccess) {
          _msg_this_access_ram(Reg_address, Reg_name, false);
        }
      }
      else if (i_flags.bank == IFLAG_BANK_SEL_BANK) {
        /* Selected a RAM Bank. */
        if (i_flags.isAccessSFR) {
          _msg_this_access_ram(Reg_address, Reg_name, true);
        }
      }
      break;
    }
  } /* switch (i_flags.bank_mode) */
}

/*------------------------------------------------------------------------------------------------*/

/* Check that a register address is ok. */

static void
_reg_addr_check(int Reg_address, const char *Reg_name, unsigned int Insn_flags, gpasmVal Reloc_value)
{
  insn_flags_t      i_flags;
  int               bank_addr;
  int               bank_num;
  int               reg_offs;
  int               bank_assume;
  int               bank_assume_num;
  int               word_digits;
  const variable_t *var;
  gp_boolean        need_bank_check;

  i_flags.u = Insn_flags;

  /* Don't check address, the linker takes care of it. */
  if (state.mode == MODE_RELOCATABLE) {
    if ((!i_flags.isReloc) && _core_sfr_or_common_ram(Reg_address)) {
      return;
    }

    _reg_addr_check_reloc(Reg_address, Reg_name, Insn_flags, Reloc_value, true);
    return;
  }

  word_digits = (state.device.class != NULL) ? state.device.class->word_digits : 4;

  if (Reg_address > state.maxram) {
    if ((!state.mpasm_compatible) && (Reg_name != NULL)) {
      gperror_vwarning(GPW_INVALID_RAM, "'%s' -- {0x%0*X} > MAXRAM{0x%0*X}", Reg_name,
                       word_digits, Reg_address,
                       word_digits, state.maxram);
    }
    else {
      gperror_vwarning(GPW_INVALID_RAM, "Address{0x%0*X} > MAXRAM{0x%0*X}",
                       word_digits, Reg_address,
                       word_digits, state.maxram);
    }
  }
  else if (state.badram[Reg_address]) {
    if ((!state.mpasm_compatible) && (Reg_name != NULL)) {
      gperror_vwarning(GPW_INVALID_RAM, "'%s' -- {0x%0*X} in BADRAM", Reg_name,
                       word_digits, Reg_address);
    }
    else {
      gperror_vwarning(GPW_INVALID_RAM, "Address{0x%0*X} in BADRAM",
                       word_digits, Reg_address);
    }
  }

  bank_addr = gp_processor_bank_addr(state.processor, Reg_address);
  bank_num  = gp_processor_bank_num(state.processor, Reg_address);
  reg_offs  = gp_processor_reg_offs(state.processor, Reg_address);

  if (state.mpasm_compatible || IS_PIC16_CORE) {
    /* Only so much can be done compatibility reasons. */
    if (bank_addr > 0) {
      if (Reg_name != NULL) {
        gperror_vmessage(GPM_BANK,
                         "'%s' -- (Bank_%u: 0x%0*X; Offs: 0x%0*X)", 0,
                         Reg_name,
                         bank_num,
                         word_digits, bank_addr,
                         word_digits, reg_offs);
      }
      else {
        gperror_vmessage(GPM_BANK,
                         "Bank_%u: 0x%0*X; Offs: 0x%0*X", 0,
                         bank_num,
                         word_digits, bank_addr,
                         word_digits, reg_offs);
      }
    }

    return;
  }

  /* Don't check bank if common ram or core SFR is addressed. */
  if ((!i_flags.isReloc) && _core_sfr_or_common_ram(Reg_address)) {
    return;
  }

  /* The __ACTIVE_BANK_ADDR variable shows the register which used for BANKSEL directive last time. */
  if ((var = get_global_constant(GLOBAL_ACT_BANK_ADDR)) == NULL) {
    gperror_verror(GPE_INTERNAL, NULL, "The \"" GLOBAL_ACT_BANK_ADDR "\" variable not exists.");
    return;
  }

  bank_assume     = var->value;
  need_bank_check = true;

  /* Check if we are in correct bank. Negative __ACTIVE_BANK_ADDR value means bank is not set yet. */
  if (bank_assume >= 0) {
    /* Necessary only the selected bank address. */
    bank_assume     = gp_processor_bank_addr(state.processor, bank_assume);
    bank_assume_num = gp_processor_bank_num(state.processor, bank_assume);

    if ((!i_flags.isAccess) && (bank_assume != bank_addr)) {
      _msg_ram_bank(Reg_address, Reg_name, bank_assume_num);
      need_bank_check = false;
    }
  }
  else {
    if (gp_processor_num_banks(state.processor) > 1) {
      gperror_vmessage(GPM_NOB, NULL, bank_num);
    }

    /* If no bank is explicitly selected, set bank to this register now. */
    set_global(GLOBAL_ACT_BANK_ADDR, Reg_address, LFT_TEMPORARY, VAL_CONSTANT, true);
  }

  _reg_addr_check_reloc(Reg_address, Reg_name, Insn_flags, Reloc_value, need_bank_check);
}

/*------------------------------------------------------------------------------------------------*/

gpasmVal
do_insn(const char *Op_name, pnode_t *Parameters)
{
  pnode_t          *p;
  const symbol_t   *sym;
  int               addr_digits;
  int               arity;
  int               file;            /* register file address, if applicable */
  unsigned int      bank_num;
  gpasmVal          r;               /* Return value. */
  gp_boolean        is_btfsx = false;
  const insn_t     *ins;
  enum common_insn  icode;
  const char       *sym_name;
  const char       *str;

  /* We want to have r as the value to assign to label. */
  r = IS_RAM_ORG ? state.byte_addr : gp_processor_byte_to_org(state.device.class, state.byte_addr);

  addr_digits = (state.device.class != NULL) ? state.device.class->addr_digits : 4;
  arity       = eval_list_length(Parameters);
  sym         = sym_get_symbol(state.stBuiltin, Op_name);

  if (sym != NULL) {
    sym_name = sym_get_symbol_name(sym);
    ins      = sym_get_symbol_annotation(sym);

    /* Instructions in data sections are not allowed. */
    if (asm_enabled() && (ins->class != INSN_CLASS_FUNC) && IS_RAM_ORG) {
      gperror_verror(GPE_WRONG_SECTION, NULL);
      goto leave;
    }

    /* Interpret the instruction if assembly is enabled, or if it's a conditional. */
    if (asm_enabled() || (ins->attribs & ATTRIB_COND)) {
      state.lst.line.linetype = LTY_INSN;
      icode = ins->icode;

      switch (ins->class) {
      case INSN_CLASS_LIT3_BANK:
        /* SX bank */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);
          _emit_check(ins->opcode, (eval_reloc_evaluate(p, RELOCT_F, NULL, NULL) >> 5),
                      SX_BMSK_BANK, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT3_PAGE:
        /* SX page */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);
          _emit_check(ins->opcode, (eval_reloc_evaluate(p, RELOCT_F, NULL, NULL) >> PIC12_SHIFT_PAGE_ADDR),
                      SX_BMSK_PAGE, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT1:
        /* PIC16E (retfie, return) */
        {
          int flag;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          flag = 0;
          _check_16e_arg_types(Parameters, arity, 0);

          switch (arity) {
          case 1:
            flag = _check_flag(eval_reloc_evaluate(PnListHead(Parameters), RELOCT_F, NULL, NULL));
          case 0:
            _emit(ins->opcode | flag, sym_name);
            break;

          default:
            eval_enforce_arity(arity, 1);
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT3:
        /* PIC12E, PIC12I movlb */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);
          r = eval_reloc_evaluate(p, RELOCT_MOVLB, NULL, NULL);
          _emit_check(ins->opcode, r, PIC12E_BMSK_BANK, sym_name);

          if (!state.mpasm_compatible) {
            /* Set the selection of RAM Banks. */
            bank_num = r & (PIC12E_MASK_MOVLB ^ PIC12_CORE_MASK);
            set_global(GLOBAL_ACT_BANK_ADDR,
                       gp_processor_bank_num_to_addr(state.processor, bank_num),
                       LFT_TEMPORARY, VAL_CONSTANT, true);
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT4:
        /* SX mode */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);
          _emit_check(ins->opcode, eval_reloc_evaluate(p, RELOCT_F, NULL, NULL),
                      SX_BMSK_MODE, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT4L:
	/* PIC16E movlb */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          _check_16e_arg_types(Parameters, arity, 0);
          p = PnListHead(Parameters);
          coerce_str1(p); /* literal instructions can coerce string literals */
          r = eval_reloc_evaluate(p, RELOCT_MOVLB, NULL, NULL);
          _emit_check(ins->opcode, r, PIC16E_BMSK_MOVLB, sym_name);

          if (!state.mpasm_compatible) {
            /* Set the selection of RAM Banks. */
            bank_num = r & (PIC16E_MASK_MOVLB ^ PIC16_CORE_MASK);
            set_global(GLOBAL_ACT_BANK_ADDR,
                       gp_processor_bank_num_to_addr(state.processor, bank_num),
                       LFT_TEMPORARY, VAL_CONSTANT, true);
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT4H:
        /* PIC16 movlr */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);
          _emit_check(ins->opcode, (eval_reloc_evaluate(p, RELOCT_MOVLR, NULL, NULL) << 4),
                      PIC16_BMSK_MOVLR, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT5:
        /* PIC14E movlb */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);
          r = eval_reloc_evaluate(p, RELOCT_MOVLB, NULL, NULL);
          _emit_check(ins->opcode, r, PIC14E_BMSK_MOVLB, sym_name);

          if (!state.mpasm_compatible) {
            /* Set the selection of RAM Banks. */
            bank_num = r & PIC14E_BMSK_MOVLB;
            set_global(GLOBAL_ACT_BANK_ADDR,
                       gp_processor_bank_num_to_addr(state.processor, bank_num),
                       LFT_TEMPORARY, VAL_CONSTANT, true);
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LITBSR_6:
        /* PIC14EX movlb */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);
          r = eval_reloc_evaluate(p, RELOCT_MOVLB, NULL, NULL);
          _emit_check(ins->opcode, r, PIC14EX_BMSK_MOVLB, sym_name);

          if (!state.mpasm_compatible) {
            /* Set the selection of RAM Banks. */
            bank_num = r & PIC14EX_BMSK_MOVLB;
            set_global(GLOBAL_ACT_BANK_ADDR,
                       gp_processor_bank_num_to_addr(state.processor, bank_num),
                       LFT_TEMPORARY, VAL_CONSTANT, true);
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT6:
        /* PIC16E (addulnk, subulnk) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          _check_16e_arg_types(Parameters, arity, 0);
          p = PnListHead(Parameters);
          /* The literal cannot be a relocatable address. */
          _emit_check(ins->opcode, eval_maybe_evaluate(p), PIC16EX_BMSK_xxxULNK, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT7:
        /* PIC14E movlp */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          int page;

          p    = PnListHead(Parameters);
          page = eval_reloc_evaluate(p, RELOCT_PAGESEL_MOVLP, NULL, NULL);
          _emit_check(ins->opcode, page, PIC14E_BMSK_PAGE512, sym_name);

          if (!state.mpasm_compatible) {
            set_global(GLOBAL_ACT_PAGE_ADDR,
                       gp_processor_page_bits_to_addr(state.device.class, page),
                       LFT_TEMPORARY, VAL_CONSTANT, true);
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT8:
        /* PIC1xx (addlw, andlw, iorlw, movlw, retlw, sublw, xorlw),
	   PIC16  movlb,
	   PIC16x mullw,
	   PIC16E pushl */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          _check_16e_arg_types(Parameters, arity, 0);

          p = PnListHead(Parameters);
          coerce_str1(p); /* literal instructions can coerce string literals */

          if (icode == ICODE_MOVLB) {
            _emit_check(ins->opcode, eval_reloc_evaluate(p, RELOCT_MOVLB, NULL, NULL),
                        PIC16_BMSK_MOVLB, sym_name);
          }
          else {
            _emit_check(ins->opcode, eval_reloc_evaluate(p, RELOCT_LOW, NULL, NULL), 0xff, sym_name);
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT8C12:
        /* PIC12x call, SX call */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);

          if ((state.obj.new_sect_flags & STYP_ABS) && eval_can_evaluate_value(p)) {
            int value = eval_evaluate(p);

            /* PC is 11 bits. Mpasm checks the maximum device address. */
            if (value & (~PIC12_PC_MASK)) {
              gperror_verror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC12_PC_MASK);
            }

            if ((value & PIC12_PAGE_BITS) != (r & PIC12_PAGE_BITS)) {
              gperror_vmessage(GPM_PAGE, "0x%0*X != 0x%0*X",
                               addr_digits, (value & PIC12_PAGE_BITS),
                               addr_digits, (r & PIC12_PAGE_BITS));
            }

            if (value & 0x100) {
              gperror_verror(GPE_BAD_CALL_ADDR, "%ins (0x%0*X)", value,
                             addr_digits, value);
            }
          }

          _emit(ins->opcode | (eval_reloc_evaluate(p, RELOCT_CALL, NULL, NULL) & PIC12_BMSK_CALL), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT8C16:
        /* PIC16 lcall */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          int value;

          p     = PnListHead(Parameters);
          value = eval_reloc_evaluate(p, RELOCT_LOW, NULL, NULL);

          /* PC is 16 bits. Mpasm checks the maximum device address. */
          if (value & (~0xffff)) {
            gperror_verror(GPE_RANGE, "Address{0x%0*X} > 0xffff.", addr_digits, value);
          }

          _emit(ins->opcode | (value & 0xff), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT9:
        /* PIC12 goto, SX goto */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);

          if ((state.obj.new_sect_flags & STYP_ABS) && eval_can_evaluate_value(p)) {
            int value = eval_evaluate(p);

            /* PC is 11 bits. Mpasm checks the maximum device address. */
            if (value & (~PIC12_PC_MASK)) {
              gperror_verror(GPE_RANGE, "Address{0x%0*X} > 0x%0*X.",
                             addr_digits, value, addr_digits, PIC12_PC_MASK);
            }

            if ((value & PIC12_PAGE_BITS) != (r & PIC12_PAGE_BITS)) {
              gperror_vmessage(GPM_PAGE, "0x%0*X != 0x%0*X",
                               addr_digits, (value & PIC12_PAGE_BITS),
                               addr_digits, (r & PIC12_PAGE_BITS));
            }
          }

          _emit(ins->opcode | (eval_reloc_evaluate(p, RELOCT_GOTO, NULL, NULL) & PIC12_BMSK_GOTO), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT11:
        /* PIC14x (call, goto) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);

          if ((state.obj.new_sect_flags & STYP_ABS) && eval_can_evaluate_value(p)) {
            int value = eval_evaluate(p);

            if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
              /* PC is 15 bits. Mpasm checks the maximum device address. */
              if (value & (~PIC14E_PC_MASK)) {
                gperror_verror(GPE_RANGE, "Address{0x%0*X} > 0x%0*X.",
                               addr_digits, value, addr_digits, PIC14E_PC_MASK);
              }

              if ((value & PIC14E_PAGE_BITS) != (r & PIC14E_PAGE_BITS)) {
                gperror_vmessage(GPM_PAGE, "0x%0*X != 0x%0*X",
                                 addr_digits, (value & PIC14E_PAGE_BITS),
                                 addr_digits, (r & PIC14E_PAGE_BITS));
              }
            }
            else {
              /* PC is 13 bits.  mpasm checks the maximum device address. */
              if (value & (~PIC14_PC_MASK)) {
                gperror_verror(GPE_RANGE, "Address{0x%0*X} > 0x%0*X.",
                               addr_digits, value, addr_digits, PIC14_PC_MASK);
              }

              if ((value & PIC14_PAGE_BITS) != (r & PIC14_PAGE_BITS)) {
                gperror_vmessage(GPM_PAGE, "0x%0*X != 0x%0*X",
                                 addr_digits, (value & PIC14_PAGE_BITS),
                                 addr_digits, (r & PIC14_PAGE_BITS));
              }
            }
          }

          _emit(ins->opcode |
	                      (eval_reloc_evaluate(p, (icode == ICODE_CALL) ? RELOCT_CALL : RELOCT_GOTO, NULL, NULL) &
	                      PIC14_BMSK_BRANCH),
	        sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT13:
        /* PIC16 (call, goto) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p = PnListHead(Parameters);

          if ((state.obj.new_sect_flags & STYP_ABS) && eval_can_evaluate_value(p)) {
            int value = eval_evaluate(p);

            /* PC is 16 bits.  mpasm checks the maximum device address. */
            if (value & (~PIC16_PC_MASK)) {
              gperror_verror(GPE_RANGE, "Address{0x%0*X} > 0x%0*X.",
                             addr_digits, value, addr_digits, PIC16_PC_MASK);
            }

            if ((value & PIC16_PAGE_BITS) != (r & PIC16_PAGE_BITS)) {
              gperror_vmessage(GPM_PAGE, "0x%0*X != 0x%0*X",
                               addr_digits, (value & PIC16_PAGE_BITS),
                               addr_digits, (r & PIC16_PAGE_BITS));
            }
          }

          _emit(ins->opcode |
	                      (eval_reloc_evaluate(p, (icode == ICODE_CALL) ? RELOCT_CALL : RELOCT_GOTO, NULL, NULL) &
		              PIC16_BMSK_BRANCH),
	        sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LITFSR_14:
        /* PIC14E addfsr */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          int value;
          int fsr;

          p   = PnListHead(Parameters);
          fsr = eval_maybe_evaluate(p);

          if ((fsr == PIC14E_REG_FSR0) || (fsr == PIC14E_REG_FSR1)) {
            fsr = (fsr == PIC14E_REG_FSR1) ? 0x40 : 0x00;
            p   = PnListHead(PnListTail(Parameters));
            /* the offset cannot be a relocatable address */
            value = eval_maybe_evaluate(p);

            if (value < -32) {
              gperror_verror(GPE_RANGE, "%ins < -32", value);
            }
            else if (value > 31) {
              gperror_verror(GPE_RANGE, "%ins > 31", value);
            }

            _emit(ins->opcode | fsr | (value & 0x3f), sym_name);
          }
          else {
            gperror_verror(GPE_RANGE, "FSR = %ins.", fsr);
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LITFSR_16:
        /* PIC16E (addfsr, subfsr) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          int value;
          int fsr;

          _check_16e_arg_types(Parameters, arity, 0);

          p   = PnListHead(Parameters);
          fsr = eval_maybe_evaluate(p);

          if (fsr < 0) {
            gperror_verror(GPE_RANGE, "FSR{%ins} < 0.", fsr);
          }
          else if (fsr > 2) {
            gperror_verror(GPE_RANGE, "FSR{%ins} > 2.", fsr);
          }

          p     = PnListHead(PnListTail(Parameters));
          /* the offset cannot be a relocatable address */
          value = eval_maybe_evaluate(p);

          if (value & (~0x3f)) {
            gperror_verror(GPE_RANGE, "%ins (%#x) > 0x3f", value, value);
          }

          _emit(ins->opcode | ((fsr & 0x3) << 6) | (value & 0x3f), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_RBRA8:
        /* PIC16E (bc, bn, bnc, bnn, bnov, bnz, bov, bz) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          int offset;

          _check_16e_arg_types(Parameters, arity, 0);

          p = PnListHead(Parameters);

          if (eval_count_reloc(p) == 0) {
            offset = eval_maybe_evaluate(p) - (r + 2);

            if (offset & 1) {
              gperror_vwarning(GPW_WORD_ALIGNED, "Offset{%ins (%#x)}", offset, offset);
            }
          }
          else {
            offset = eval_reloc_evaluate(p, RELOCT_CONDBRA, NULL, NULL);
          }

          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -127 <= offset <= 127. */
          _emit_check_relative(ins->opcode, offset, PIC16E_BMSK_RBRA8, 127, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_RBRA9:
        /* PIC14E bra */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          int offset;

          p = PnListHead(Parameters);

          if (eval_count_reloc(p) == 0) {
            offset = eval_maybe_evaluate(p) - (r + 1);
          }
          else {
            offset = eval_reloc_evaluate(p, RELOCT_BRA, NULL, NULL);
          }

          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -256 <= offset <= 255. */
          _emit_check_relative(ins->opcode, offset, PIC14E_BMSK_RBRA9, 255, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_RBRA11:
        /* PIC16E (bra, rcall) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          int offset;

          _check_16e_arg_types(Parameters, arity, 0);

          p = PnListHead(Parameters);

          if (eval_count_reloc(p) == 0) {
            offset = eval_maybe_evaluate(p) - (r + 2);

            if (offset & 1) {
              gperror_vwarning(GPW_WORD_ALIGNED, "%ins (%#x)", offset, offset);
            }
          }
          else {
            offset = eval_reloc_evaluate(p, RELOCT_BRA, NULL, NULL);
          }

          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -1024 <= offset <= 1023. */
          _emit_check_relative(ins->opcode, offset, PIC16E_BMSK_RBRA11, 1023, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_LIT20:
        /* PIC16E goto */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          int dest;

          _check_16e_arg_types(Parameters, arity, 0);

          p    = PnListHead(Parameters);
          dest = eval_reloc_evaluate(p, RELOCT_GOTO, NULL, NULL);
          dest = gp_processor_org_to_byte(state.device.class, dest) >> 1;
          _emit(ins->opcode | (dest & PIC16E_BMSK_BRANCH_LOWER), sym_name);
          eval_reloc_evaluate(p, RELOCT_GOTO2, NULL, NULL);     /* add the second relocation */
          _emit_check(0xf000, dest >> 8, PIC16E_BMSK_BRANCH_HIGHER, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_CALL20:
        /* PIC16E call */
        {
          int            dest;
          int            flag;
          const pnode_t *p2; /* second parameter */

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          flag = 0; /* By default, fast push is not used. */

          if (arity < 1) {
            eval_enforce_arity(arity, 2);
          }
          else {
            _check_16e_arg_types(Parameters, arity, 0);

            p = PnListHead(Parameters);

            switch (arity) {
            case 2:
              p2 = PnListHead(PnListTail(Parameters));
              /* Allow "s" for fast push */
              if (PnIsSymbol(p2) && (strcasecmp(PnSymbol(p2), "s") == 0)) {
                flag = 1;
              }
              else {
                flag = _check_flag(eval_maybe_evaluate(p2));
              }
              break;

            case 1:
              flag = 0;
              break;

            default:
              eval_enforce_arity(arity, 2);
            }

            dest = eval_reloc_evaluate(p, RELOCT_CALL, NULL, NULL);
            dest = gp_processor_org_to_byte(state.device.class, dest) >> 1;
            _emit(ins->opcode | (flag << 8) | (dest & PIC16E_BMSK_BRANCH_LOWER), sym_name);
            eval_reloc_evaluate(p, RELOCT_CALL2, NULL, NULL);     /* add the second relocation */
            _emit_check(0xf000, (dest >> 8), PIC16E_BMSK_BRANCH_HIGHER, sym_name);
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_FLIT12:
        /* PIC16E lfsr */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          int k;

          _check_16e_arg_types(Parameters, arity, 0);

          p    = PnListHead(Parameters);
          file = eval_maybe_evaluate(p);

          if (file > 3) {
            gperror_error(GPE_UNKNOWN, "FSR is out of range.");
          }

          p = PnListHead(PnListTail(Parameters));
          k = eval_reloc_evaluate(p, RELOCT_LFSR1, NULL, NULL);
          _emit_check(ins->opcode | ((file & 3) << 4), (k >> 8), 0xf, sym_name);
          eval_reloc_evaluate(p, RELOCT_LFSR2, NULL, NULL); /* add the second relocation */
          _emit(0xf000 | (k & 0xff), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_FF:
        /* PIC16E movff */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          int dest;

          _check_16e_arg_types(Parameters, arity, 0);

          dest = eval_maybe_evaluate(PnListHead(PnListTail(Parameters)));

          /* The destination can't be the PCL, TOSU, TOSH or TOSL. */
          if (dest == PIC16E_REG_PCL) {
            gperror_error(GPE_UNKNOWN, "The destination cannot be the PCL.");
          }
          else if (dest == PIC16E_REG_TOSU) {
            gperror_error(GPE_UNKNOWN, "The destination cannot be the TOSU.");
          }
          else if (dest == PIC16E_REG_TOSH) {
            gperror_error(GPE_UNKNOWN, "The destination cannot be the TOSH.");
          }
          else if (dest == PIC16E_REG_TOSL) {
            gperror_error(GPE_UNKNOWN, "The destination cannot be the TOSL.");
          }

          _emit_check(ins->opcode, eval_reloc_evaluate(PnListHead(Parameters), RELOCT_FF1, NULL, NULL), 0xfff, sym_name);
          _emit_check(0xf000, eval_reloc_evaluate(PnListHead(PnListTail(Parameters)), RELOCT_FF2, NULL, NULL), 0xfff, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_FP:
        /* PIC16 movfp */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          int reg;

          p    = PnListHead(Parameters);
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, NULL, NULL);
          reg  = eval_reloc_evaluate(PnListHead(PnListTail(Parameters)), RELOCT_P, NULL, NULL);

          if (reg & ~0xf1f) {
            gperror_vwarning(GPW_RANGE, "(%#x & ~0xf1f) != 0", reg, reg);
          }

          _reg_addr_check(file, str, IFLAG_NONE, -1);
          _emit(ins->opcode | ((reg & 0x1f) << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_PF:
        /* PIC16 movpf */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          int reg;

          p    = PnListHead(PnListTail(Parameters));
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, NULL, NULL);
          reg  = eval_reloc_evaluate(PnListHead(Parameters), RELOCT_P, NULL, NULL);

          if (reg & ~0xf1f) {
            gperror_vwarning(GPW_RANGE, "(%#x & ~0xf1f) != 0", reg, reg);
          }

          _reg_addr_check(file, str, IFLAG_NONE, -1);
          _emit(ins->opcode | ((reg & 0x1f) << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_SF:
        /* PIC16E movsf */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          int source;
          int dest;

          _check_16e_arg_types(Parameters, arity, AR_INDEX);

          p      = PnListHead(Parameters);
          source = eval_maybe_evaluate(p);

          p    = PnListHead(PnListTail(Parameters));
          dest = eval_maybe_evaluate(p);

          /* The destination can't be the PCL, TOSU, TOSH or TOSL. */
          if (dest == PIC16E_REG_PCL) {
            gperror_error(GPE_UNKNOWN, "The destination cannot be the PCL.");
          }
          else if (dest == PIC16E_REG_TOSU) {
            gperror_error(GPE_UNKNOWN, "The destination cannot be the TOSU.");
          }
          else if (dest == PIC16E_REG_TOSH) {
            gperror_error(GPE_UNKNOWN, "The destination cannot be the TOSH.");
          }
          else if (dest == PIC16E_REG_TOSL) {
            gperror_error(GPE_UNKNOWN, "The destination cannot be the TOSL.");
          }

          _emit_check(ins->opcode, source, 0x7f, sym_name);
          _emit_check(0xf000, eval_reloc_evaluate(PnListHead(PnListTail(Parameters)), RELOCT_FF2, NULL, NULL), 0xfff, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_SS:
        /* PIC16E movss */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          int source;
          int dest;

          _check_16e_arg_types(Parameters, arity, (AR_INDEX << AR_BITS) | AR_INDEX);

          p      = PnListHead(Parameters);
          source = eval_maybe_evaluate(p);

          p    = PnListHead(PnListTail(Parameters));
          dest = eval_maybe_evaluate(p);

          _emit_check(ins->opcode, source, 0x7f, sym_name);
          _emit_check(0xf000, dest, 0x7f, sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_OPF3:
        /* PIC12 tris */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          p    = PnListHead(Parameters);
          file = eval_reloc_evaluate(p, RELOCT_TRIS_3BIT, NULL, NULL);
          _reg_addr_check(file, NULL, IFLAG_NONE, -1);
          _emit(ins->opcode | (file & PIC12_BMSK_TRIS), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_OPF5:
        /* {PIC12x, SX} (clrf, movwf), SX tris */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          insn_flags_t insn_flags;
          gp_boolean   is_reloc;
          gpasmVal     reloc_val;

          insn_flags.u = IFLAG_NONE;
          p   = PnListHead(Parameters);
          str = pnode_symbol_name(p);

          if (IS_SX_CORE && (icode == ICODE_TRIS)) {
            file = eval_reloc_evaluate(p, RELOCT_TRIS, &is_reloc, &reloc_val);
          }
          else {
            file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          }

          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (file & PIC12_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_OPWF5:
        /* {PIC12x, SX} (addwf, andwf, comf, decf, decfsz, incf, incfsz,
                         iorwf, movf, rlf, rrf, subwf, swapf, xorwf) */
        {
          int          d;
          insn_flags_t insn_flags;
          gp_boolean   is_reloc;
          gpasmVal     reloc_val;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (arity == 0) {
            eval_enforce_arity(arity, 2);
            break;
          }

          insn_flags.u    = IFLAG_NONE;
          d               = F;      /* Default destination of 1 (file). */
          insn_flags.dest = IFLAG_DEST_DEF_FILE;
          p    = PnListHead(Parameters);
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          switch (arity) {
          case 2:
            p = PnListHead(PnListTail(Parameters));
            /* Allow "w" and "f" as destinations. */
            if (PnIsSymbol(p) && (strcasecmp(PnSymbol(p), "f") == 0)) {
              d = F;
              insn_flags.dest = IFLAG_DEST_SEL_FILE;
            }
            else if (PnIsSymbol(p) && (strcasecmp(PnSymbol(p), "w") == 0)) {
              d = W;
              insn_flags.dest = IFLAG_DEST_SEL_WREG;
            }
            else {
              d = _check_flag(eval_maybe_evaluate(p));
              insn_flags.dest = (d == W) ? IFLAG_DEST_SEL_WREG : IFLAG_DEST_SEL_FILE;
            }
            break;

          case 1:
            break;

          default:
            eval_enforce_arity(arity, 2);
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (d << 5) | (file & PIC12_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_B5:
        /* {PIC12x, SX} (bcf, bsf, btfsc, btfss) */
        {
          int          bit;
          insn_flags_t insn_flags;
          gp_boolean   is_reloc;
          gpasmVal     reloc_val;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (eval_enforce_arity(arity, 2)) {
            insn_flags.u = IFLAG_NONE;
            p    = PnListHead(Parameters);
            str  = pnode_symbol_name(p);
            file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
            if (is_reloc) {
              insn_flags.isReloc = true;
            }

            p    = PnListHead(PnListTail(Parameters));
            bit  = eval_maybe_evaluate(p);

            if (bit < 0) {
              gperror_vwarning(GPW_RANGE, "Bit{%ins} < 0.", bit);
            }
            else if (bit > 7) {
              gperror_vwarning(GPW_RANGE, "Bit{%ins} > 7.", bit);
            }

            bit &= 7;

            if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
              is_btfsx = true;
            }

            _reg_addr_check(file, str, insn_flags.u, reloc_val);
            _emit(ins->opcode | (bit << 5) | (file & PIC12_BMSK_FILE), sym_name);

            if ((!state.mpasm_compatible) && (state.mode == MODE_ABSOLUTE)) {
              if (IS_PIC12_CORE) {
                int reg = file & PIC12_BMSK_FILE;

                if (reg == PIC12_REG_STATUS) {
                  /* This code monitors the change of ROM Pages. */
                  if (!_check_and_set_page_bit(icode, bit, PIC12_BIT_STATUS_PA0, PIC12_BIT_STATUS_PA1, PIC12_BIT_STATUS_PA2)) {
                    return 0;
                  }
                }
                else if (reg == PIC12_REG_FSR) {
                  /* This code monitors the change of RAM Banks. */
                  if (!_check_and_set_bank_bit(icode, bit, PIC12_BIT_FSR_RP0, PIC12_BIT_FSR_RP1, PIC12_BIT_FSR_RP2)) {
                    return 0;
                  }
                }
              }
            }
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_B8:
        /* PIC16 (bcf, bsf, btfsc, btfss, btg) */
      {
        int          bit;
        insn_flags_t insn_flags;
        gp_boolean   is_reloc;
        gpasmVal     reloc_val;

        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          insn_flags.u = IFLAG_NONE;
          p    = PnListHead(Parameters);
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          p    = PnListHead(PnListTail(Parameters));
          bit  = eval_maybe_evaluate(p);

          if (bit < 0) {
            gperror_vwarning(GPW_RANGE, "Bit{%ins} < 0.", bit);
          }
          else if (bit > 7) {
            gperror_vwarning(GPW_RANGE, "Bit{%ins} > 7.", bit);
          }

          if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
            is_btfsx = true;
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | ((bit & 7) << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;
      }

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_OPF7:
        /* PIC14x (clrf, movwf, tris) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          insn_flags_t insn_flags;
          gp_boolean   is_reloc;
          gpasmVal     reloc_val;

          insn_flags.u = IFLAG_NONE;
          p   = PnListHead(Parameters);
          str = pnode_symbol_name(p);

          if (icode == ICODE_TRIS) {
            gperror_vwarning(GPW_NOT_RECOMMENDED, "\"tris\"");
            file = eval_reloc_evaluate(p, RELOCT_TRIS, &is_reloc, &reloc_val);
          }
          else {
            file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          }

          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (file & PIC14_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_OPF8:
        /* PIC16 (cpfseq, cpfsgt, cpfslt, movwf, mulwf, tstfsz) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 1)) {
          insn_flags_t insn_flags;
          gp_boolean   is_reloc;
          gpasmVal     reloc_val;

          insn_flags.u = IFLAG_NONE;
          p    = PnListHead(Parameters);
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_OPWF7:
        /* PIC14x (addwf, andwf, comf, decf, decfsz, incf, incfsz, iorwf, movf,
                   rlf, rrf, subwf, swapf, xorwf)
           PIC14E (addwfc, asrf, lslf, lsrf, subwfb) */
        {
          int          d;
          insn_flags_t insn_flags;
          gp_boolean   is_reloc;
          gpasmVal     reloc_val;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (arity == 0) {
            eval_enforce_arity(arity, 2);
            break;
          }

          insn_flags.u    = IFLAG_NONE;
          d               = F;      /* Default destination of 1 (file). */
          insn_flags.dest = IFLAG_DEST_DEF_FILE;
          p    = PnListHead(Parameters);
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          switch (arity) {
          case 2:
            p = PnListHead(PnListTail(Parameters));
            /* Allow "w" and "f" as destinations. */
            if (PnIsSymbol(p) && (strcasecmp(PnSymbol(p), "f") == 0)) {
              d = F;
              insn_flags.dest = IFLAG_DEST_SEL_FILE;
            }
            else if (PnIsSymbol(p) && (strcasecmp(PnSymbol(p), "w") == 0)) {
              d = W;
              insn_flags.dest = IFLAG_DEST_SEL_WREG;
            }
            else {
              d = _check_flag(eval_maybe_evaluate(p));
              insn_flags.dest = (d == W) ? IFLAG_DEST_SEL_WREG : IFLAG_DEST_SEL_FILE;
            }
            break;

          case 1:
            break;

          default:
            eval_enforce_arity(arity, 2);
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (d << 7) | (file & PIC14_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_OPWF8:
        /* PIC16 (addwf, addwfc, andwf, clrf, comf, daw, decf, decfsz, dcfsnz, incf,
                  incfsz, infsnz, iorwf, rlcf, rlncf, rrcf, rrncf, setf, subwf, subwfb,
                  swapf, xorwf) */
        {
          int          d;
          insn_flags_t insn_flags;
          gp_boolean   is_reloc;
          gpasmVal     reloc_val;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (arity == 0) {
            eval_enforce_arity(arity, 2);
            break;
          }

          insn_flags.u    = IFLAG_NONE;
          d               = F;      /* Default destination of 1 (file). */
          insn_flags.dest = IFLAG_DEST_DEF_FILE;
          p    = PnListHead(Parameters);
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          switch (arity) {
          case 2:
            p = PnListHead(PnListTail(Parameters));
            /* Allow "w" and "f" as destinations. */
            if (PnIsSymbol(p) && (strcasecmp(PnSymbol(p), "f") == 0)) {
              d = F;
              insn_flags.dest = IFLAG_DEST_SEL_FILE;
            }
            else if (PnIsSymbol(p) && (strcasecmp(PnSymbol(p), "w") == 0)) {
              d = W;
              insn_flags.dest = IFLAG_DEST_SEL_WREG;
            }
            else {
              d = _check_flag(eval_maybe_evaluate(p));
              insn_flags.dest = (d == W) ? IFLAG_DEST_SEL_WREG : IFLAG_DEST_SEL_FILE;
            }
            break;

          case 1:
            break;

          default:
            eval_enforce_arity(arity, 2);
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (d << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_B7:
        /* PIC14x (bcf, bsf, btfsc, btfss) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          int          bit;
          insn_flags_t insn_flags;
          gp_boolean   is_reloc;
          gpasmVal     reloc_val;

          insn_flags.u = IFLAG_NONE;
          p    = PnListHead(Parameters);
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          p    = PnListHead(PnListTail(Parameters));
          bit  = eval_maybe_evaluate(p);

          if (bit < 0) {
            gperror_vwarning(GPW_RANGE, "Bit{%ins} < 0.", bit);
          }
          else if (bit > 7) {
            gperror_vwarning(GPW_RANGE, "Bit{%ins} > 7.", bit);
          }

          bit &= 7;

          if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
            is_btfsx = true;
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (bit << 7) | (file & PIC14_BMSK_FILE), sym_name);

          if ((!state.mpasm_compatible) && (state.mode == MODE_ABSOLUTE)) {
            if (IS_PIC14_CORE) {
              int reg = file & PIC14_BMSK_FILE;

              if (reg == PIC14_REG_PCLATH) {
                /* This code monitors the change of ROM Pages. */
                if (!_check_and_set_page_bit(icode, bit, 3, 4, -1)) {
                  return 0;
                }
              }
              else if (reg == PIC14_REG_STATUS) {
                /* This code monitors the change of RAM Banks. */
                if (!_check_and_set_bank_bit(icode, bit, PIC14_BIT_STATUS_RP0, PIC14_BIT_STATUS_RP1, -1)) {
                  return 0;
                }
              }
            }
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_OPFA8:
        /* PIC16E (clrf, cpfseq, cpfsgt, cpfslt, movwf, mulwf, negf, setf, tstfsz) */
        {
          const pnode_t *par; /* second parameter */
          int            a;
          insn_flags_t   insn_flags;
          gp_boolean     is_reloc;
          gpasmVal       reloc_val;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (arity == 0) {
            eval_enforce_arity(arity, 2);
            break;
          }

          a = ACCESS;       /* Default destination of 0 (access). */
          insn_flags.u    = IFLAG_NONE;
          insn_flags.bank = IFLAG_BANK_DEF_ACCESS;
          _check_16e_arg_types(Parameters, arity, AR_BIT_BYTE | AR_INDEX);

          p    = PnListHead(Parameters);
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          /* Add relocation for the access bit, if necessary. */
          if (arity < 2) {
            eval_reloc_evaluate(p, RELOCT_ACCESS, NULL, NULL);
          }

          /* Default access (use the BSR unless access is to special registers). */
          /* borutr: I don't know where is the following coming from, but is seems not to be true for MPASM 5.49,
           * so I commented it out:
           *
           * If extended instructions are enabled, access bit should default to 1 for low-end *
           * of Access Memory unless the file is explicitly an offset (e.g. [foo]) *
          if ((state.extended_pic16e == true) && (file <= 0x5f)) {
            if (PnIsOffset(p)) {
              a = ACCESS;
            }
            else {
              a = BANKED;
            }
          }
          else if ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) {
            a = ACCESS;
          }
          else {
            a = BANKED;
          }
           * and replaced it with the following, according to the Data Sheet:
           *
           * "If the index argument is properly bracketed for Indexed
           * Literal Offset Addressing mode, the Access RAM
           * argument is never specified; it will automatically be
           * assumed to be ‘0’. This is in contrast to standard
           * operation (extended instruction set disabled) when ‘a’
           * is set on the basis of the target address. Declaring the
           * Access RAM bit in this mode will also generate an error
           * in the MPASM Assembler.": */
          insn_flags.isAccessGPR = (file < state.device.bsr_boundary) ? true : false;
          insn_flags.isAccessSFR = (file >= (0xf00 + state.device.bsr_boundary)) ? true : false;
          insn_flags.isAccess    = insn_flags.isAccessGPR | insn_flags.isAccessSFR;

          a = ((!state.extended_pic16e) && (!insn_flags.isAccess)) ? BANKED : ACCESS;
          insn_flags.bank = (a == ACCESS) ? IFLAG_BANK_DEF_ACCESS : IFLAG_BANK_DEF_BANK;

          switch (arity) {
          case 2:
            par = PnListHead(PnListTail(Parameters));
            if (PnIsSymbol(par) && (strcasecmp(PnSymbol(par), "a") == 0)) {
              /* Prohibit "a" for BSR to select RAM bank. */
              a = ACCESS;
              insn_flags.bank = IFLAG_BANK_SEL_ACCESS;
            }
            else if (PnIsSymbol(par) && (strcasecmp(PnSymbol(par), "b") == 0)) {
              /* Allow "b" for BSR to select RAM bank. */
              a = BANKED;
              insn_flags.bank = IFLAG_BANK_SEL_BANK;
            }
            else {
              a = _check_flag(eval_maybe_evaluate(par));
              insn_flags.bank = (a == ACCESS) ? IFLAG_BANK_SEL_ACCESS : IFLAG_BANK_SEL_BANK;
            }
            break;

          case 1:
            /* use default a */
            break;

          default:
            eval_enforce_arity(arity, 2);
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (a << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_BA8:
        /* PIC16E (bcf, bsf, btfsc, btfss, btg) */
        {
          const pnode_t *par;
          int            bit;
          int            a;
          insn_flags_t   insn_flags;
          gp_boolean     is_reloc;
          gpasmVal       reloc_val;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if ((arity != 2) && (arity != 3)) {
            eval_enforce_arity(arity, 3);
            break;
          }

          a = ACCESS;
          insn_flags.u    = IFLAG_NONE;
          insn_flags.bank = IFLAG_BANK_DEF_ACCESS;
          _check_16e_arg_types(Parameters, arity, AR_BIT_BYTE | AR_INDEX);

          p    = PnListHead(Parameters);
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          insn_flags.isAccessGPR = (file < state.device.bsr_boundary) ? true : false;
          insn_flags.isAccessSFR = (file >= (0xf00 + state.device.bsr_boundary)) ? true : false;
          insn_flags.isAccess    = insn_flags.isAccessGPR | insn_flags.isAccessSFR;

          if (arity == 3) {
            par = PnListHead(PnListTail(PnListTail(Parameters)));

            if (PnIsSymbol(par) && (strcasecmp(PnSymbol(par), "a") == 0)) {
              /* Prohibit "a" for BSR to select RAM bank. */
              a = ACCESS;
              insn_flags.bank = IFLAG_BANK_SEL_ACCESS;
            }
            else if (PnIsSymbol(par) && (strcasecmp(PnSymbol(par), "b") == 0)) {
              /* Allow "b" for BSR to select RAM bank. */
              a = BANKED;
              insn_flags.bank = IFLAG_BANK_SEL_BANK;
            }
            else {
              a = _check_flag(eval_maybe_evaluate(par));
              insn_flags.bank = (a == ACCESS) ? IFLAG_BANK_SEL_ACCESS : IFLAG_BANK_SEL_BANK;
            }
          }
          else {
            /* Default access (use the BSR unless access is to special registers). */
            /* borutr: I don't know where is the following coming from, but is seems not to be true for MPASM 5.49,
             * so I commented it out:
             *
             * If extended instructions are enabled, access bit should default to 1 for low-end *
             * of Access Memory unless the file is explicitly an offset (e.g. [foo]) *
            if ((state.extended_pic16e == true) && (file <= 0x5f)) {
              if (PnIsOffset(p)) {
                a = ACCESS;
              }
              else {
                a = BANKED;
              }
            }
            else if ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) {
              a = ACCESS;
            }
            else {
              a = BANKED;
            }
             * and replaced it with the following, according to the Data Sheet:
             *
             * "If the index argument is properly bracketed for Indexed
             * Literal Offset Addressing mode, the Access RAM
             * argument is never specified; it will automatically be
             * assumed to be ‘0’. This is in contrast to standard
             * operation (extended instruction set disabled) when ‘a’
             * is set on the basis of the target address. Declaring the
             * Access RAM bit in this mode will also generate an error
             * in the MPASM Assembler.": */
            a = ((!state.extended_pic16e) && (!insn_flags.isAccess)) ? BANKED : ACCESS;
            insn_flags.bank = (a == ACCESS) ? IFLAG_BANK_DEF_ACCESS : IFLAG_BANK_DEF_BANK;
          }

          /* add relocation for the access bit, if necessary */
          if (arity < 3) {
            eval_reloc_evaluate(p, RELOCT_ACCESS, NULL, NULL);
          }

          p   = PnListHead(PnListTail(Parameters));
          bit = eval_maybe_evaluate(p);

          if (bit < 0) {
            gperror_vwarning(GPW_RANGE, "Bit{%ins} < 0.", bit);
          }
          else if (bit > 7) {
            gperror_vwarning(GPW_RANGE, "Bit{%ins} > 7.", bit);
          }

          if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
            is_btfsx = true;
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (a << 8) | ((bit & 7) << 9) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_OPWFA8:
        /* PIC16E (addwf, addwfc, andwf, comf, decf, decfsz, dcfsnz, incf, incfsz,
                   infsnz, iorwf, movf, rlcf, rlncf, rrcf, rrncf, subfwb, subwf,
                   subwfb, swapf, xorwf) */
        {
          const pnode_t *par; /* second parameter */
          int            d;
          int            a;
          insn_flags_t   insn_flags;
          gp_boolean     is_reloc;
          gpasmVal       reloc_val;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (arity == 0) {
            eval_enforce_arity(arity, 2);
            break;
          }

          insn_flags.u    = IFLAG_NONE;
          d               = F;      /* Default destination of 1 (file). */
          insn_flags.dest = IFLAG_DEST_DEF_FILE;
          a               = ACCESS;
          insn_flags.bank = IFLAG_BANK_DEF_ACCESS;

          _check_16e_arg_types(Parameters, arity, AR_BIT_BYTE | AR_INDEX);

          p    = PnListHead(Parameters);
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          /* Add relocation for the access bit, if necessary. */
          if (arity < 3) {
            eval_reloc_evaluate(p, RELOCT_ACCESS, NULL, NULL);
          }

          /* Default access (use the BSR unless access is to special registers) */
          /* borutr: I don't know where is the following coming from, but is seems not to be true for MPASM 5.49,
           * so I commented it out:
           *
           * If extended instructions are enabled, access bit should default to 1 for low-end *
           * of Access Memory unless the file is explicitly an offset (e.g. [foo]) *
          if ((state.extended_pic16e == true) && (file <= 0x5f)) {
            if (PnIsOffset(p)) {
              a = ACCESS;
            }
            else {
              a = BANKED;
            }
          }
          else if ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) {
            a = ACCESS;
          }
          else {
            a = BANKED;
          }
           * and replaced it with the following, according to the Data Sheet:
           *
           * "If the index argument is properly bracketed for Indexed
           * Literal Offset Addressing mode, the Access RAM
           * argument is never specified; it will automatically be
           * assumed to be ‘0’. This is in contrast to standard
           * operation (extended instruction set disabled) when ‘a’
           * is set on the basis of the target address. Declaring the
           * Access RAM bit in this mode will also generate an error
           * in the MPASM Assembler.": */
          insn_flags.isAccessGPR = (file < state.device.bsr_boundary) ? true : false;
          insn_flags.isAccessSFR = (file >= (0xf00 + state.device.bsr_boundary)) ? true : false;
          insn_flags.isAccess    = insn_flags.isAccessGPR | insn_flags.isAccessSFR;

          a = ((!state.extended_pic16e) && (!insn_flags.isAccess)) ? BANKED : ACCESS;
          insn_flags.bank = (a == ACCESS) ? IFLAG_BANK_DEF_ACCESS : IFLAG_BANK_DEF_BANK;

          switch (arity) {
          case 3:
            par = PnListHead(PnListTail(PnListTail(Parameters)));

            if (PnIsSymbol(par) && (strcasecmp(PnSymbol(par), "a") == 0)) {
              /* Prohibit "a" for BSR to select RAM bank. */
              a = ACCESS;
              insn_flags.bank = IFLAG_BANK_SEL_ACCESS;
            }
            else if (PnIsSymbol(par) && (strcasecmp(PnSymbol(par), "b") == 0)) {
              /* Allow "b" for BSR to select RAM bank. */
              a = BANKED;
              insn_flags.bank = IFLAG_BANK_SEL_BANK;
            }
            else {
              a = _check_flag(eval_maybe_evaluate(par));
              insn_flags.bank = (a == ACCESS) ? IFLAG_BANK_SEL_ACCESS : IFLAG_BANK_SEL_BANK;
            }

            /* fall through */
          case 2:
            par = PnListHead(PnListTail(Parameters));
            /* Allow "w" and "f" as destinations. */
            if (PnIsSymbol(par) && (strcasecmp(PnSymbol(par), "f") == 0)) {
              d = F;
              insn_flags.dest = IFLAG_DEST_SEL_FILE;
            }
            else if (PnIsSymbol(par) && (strcasecmp(PnSymbol(par), "w") == 0)) {
              d = W;
              insn_flags.dest = IFLAG_DEST_SEL_WREG;
            }
            else {
              d = _check_flag(eval_maybe_evaluate(par));
              insn_flags.bank = (d == W) ? IFLAG_DEST_SEL_WREG : IFLAG_DEST_SEL_FILE;
            }
            break;

          case 1:
            /* use default a and d */
            break;

          default:
            eval_enforce_arity(arity, 3);
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (d << 9) | (a << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_IMPLICIT:
        /* PIC12x  (clrw, clrwdt, nop, option, return, sleep)
           PIC12E  return
           PIC12I  (retfie, return)
           SX      (iread, movmw, movwm, reti, retiw, retp, return)
           PIC14x  (clrw, clrwdt, halt, nop, option, retfie, return, sleep)
           PIC14E  (brw, callw, reset)
           PIC16   (clrwdt, nop, retfie, return, sleep)
           PIC16E  (clrwdt, daw, halt, nop, pop, push, reset, sleep, trap, tret)
           PIX16EX callw */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (arity != 0) {
          gperror_vwarning(GPW_EXTRANEOUS, NULL);
        }

        if ((icode == ICODE_OPTION) && (state.device.class->core_mask != PIC12_CORE_MASK)){
          gperror_vwarning(GPW_NOT_RECOMMENDED, "\"option\"");
        }

        _emit(ins->opcode, sym_name);
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_TBL:
        /* PIC16E (tblrd, tblwt) */
        if (eval_enforce_arity(arity, 1)) {
          _check_16e_arg_types(Parameters, arity, 0);

          p = PnListHead(Parameters);
          switch (eval_maybe_evaluate(p)) {
          case TBL_NO_CHANGE:
            _emit(ins->opcode, sym_name);
            break;

          case TBL_POST_INC:
            _emit(ins->opcode | 1, sym_name);
            break;

          case TBL_POST_DEC:
            _emit(ins->opcode | 2, sym_name);
            break;

          case TBL_PRE_INC:
            _emit(ins->opcode | 3, sym_name);
            break;

          default:
            gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
          }
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_TBL2:
        /* PIC16 (tlrd, tlwt) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 2)) {
          int          t;  /* read low byte by default */
          insn_flags_t insn_flags;
          gp_boolean   is_reloc;
          gpasmVal     reloc_val;

          /* "0" (lower byte) and "1" (upper byte) */
          insn_flags.u = IFLAG_NONE;
          p = PnListHead(Parameters);
          t = _check_flag(eval_maybe_evaluate(p));

          p    = PnListHead(PnListTail(Parameters));
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (t << 9) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_TBL3:
        /* PIC16 (tablrd, tablwt) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (eval_enforce_arity(arity, 3)) {
          int          t;
          int          inc;
          insn_flags_t insn_flags;
          gp_boolean   is_reloc;
          gpasmVal     reloc_val;

          insn_flags.u = IFLAG_NONE;
          /* "0" (lower byte) and "1" (upper byte) */
          p = PnListHead(Parameters);
          t = _check_flag(eval_maybe_evaluate(p));

          /* "0" (no change) and "1" (postincrement) */
          p   = PnListHead(PnListTail(Parameters));
          inc = _check_flag(eval_maybe_evaluate(p));

          p    = PnListHead(PnListTail(PnListTail(Parameters)));
          str  = pnode_symbol_name(p);
          file = eval_reloc_evaluate(p, RELOCT_F, &is_reloc, &reloc_val);
          if (is_reloc) {
            insn_flags.isReloc = true;
          }

          _reg_addr_check(file, str, insn_flags.u, reloc_val);
          _emit(ins->opcode | (t << 9) | (inc << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_MOVINDF:
        /* PIC14E (moviw, movwi) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (arity == 0) {
          eval_enforce_arity(arity, 1);
          break;
        }

        p = PnListHead(Parameters);
        switch (arity) {
          int      fsr;
          int      opcode;
          int      k;
          pnode_t *p2;
          pnode_t *p3;

        case 1:
          fsr = eval_maybe_evaluate(p);
          /* New opcode for indexed indirect,
             moviw/movwi INDFn handled as moviw/movwi 0[FSRn]. */
          file = -1;

          if ((fsr == PIC14E_REG_INDF0) || (fsr == PIC14E_REG_INDF1)) {
            file = (fsr == PIC14E_REG_INDF1) ? 0x40 : 0x00;
          }

          if (file < 0) {
            if ((fsr == PIC14E_REG_FSR0) || (fsr == PIC14E_REG_FSR1)) {
              file = (fsr == PIC14E_REG_FSR1) ? 0x40 : 0x00;
            }
          }

          if (file >= 0) {
            opcode = ((icode == ICODE_MOVIW) ? PIC14E_INSN_MOVIW_IDX : PIC14E_INSN_MOVWI_IDX) | file;
            _emit(opcode, sym_name);
          }
          else {
            gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
          }
          break;

        case 2:
          p2  = PnListHead(PnListTail(Parameters));
          fsr = eval_maybe_evaluate(p2);

          file = -1;

          if ((fsr == PIC14E_REG_INDF0) || (fsr == PIC14E_REG_INDF1)) {
            file = (fsr == PIC14E_REG_INDF1) ? 0x04 : 0x00;
          }

          if (file < 0) {
            if ((fsr == PIC14E_REG_FSR0) || (fsr == PIC14E_REG_FSR1)) {
              file = (fsr == PIC14E_REG_FSR1) ? 0x04 : 0x00;
            }
          }

          if (file >= 0) {
            k = -1;

            switch (eval_maybe_evaluate(p)) {
              /* ++INDFn, ++FSRn */
            case INCREMENT:     k = 0; break;
              /* --INDFn, --FSRn */
            case DECREMENT:     k = 1; break;
              /* INDFn++, FSRn++ */
            case POSTINCREMENT: k = 2; break;
              /* INDFn--, FSRn-- */
            case POSTDECREMENT: k = 3; break;
            }

            if (k >= 0) {
              _emit(ins->opcode | file | k, sym_name);
            }
            else {
              gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
            }
          }
          else {
            gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
          }
          break;

        case 3:
        case 4:
          p2  = PnListHead(PnListTail(Parameters));
          fsr = eval_maybe_evaluate(p2);

          file = -1;

          if ((fsr == PIC14E_REG_INDF0) || (fsr == PIC14E_REG_INDF1)) {
            file = (fsr == PIC14E_REG_INDF1) ? 0x40 : 0x00;
          }

          if (file < 0) {
            if ((fsr == PIC14E_REG_FSR0) || (fsr == PIC14E_REG_FSR1)) {
              file = (fsr == PIC14E_REG_FSR1) ? 0x40 : 0x00;
            }
          }

          if (file >= 0) {
            switch (eval_maybe_evaluate(p)) {
            case INDFOFFSET:
              p3 = PnListTail(PnListTail(Parameters));
              k  = eval_maybe_evaluate(p3); /* Index */

              if (k < -32) {
                gperror_verror(GPE_RANGE, "Index{%ins} < -32.", k);
              }
              else if (k > 31) {
                gperror_verror(GPE_RANGE, "Index{%ins} > 31.", k);
              }
              else {
                /* New opcode for indexed indirect. */
                opcode = ((icode == ICODE_MOVIW) ? PIC14E_INSN_MOVIW_IDX : PIC14E_INSN_MOVWI_IDX) | file;
                _emit(opcode | (k & 0x3f), sym_name);
              }
              break;

            default:
              gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
            }
          }
          else {
            gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
          }
        } /* switch (arity) */
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case INSN_CLASS_FUNC:
        r = (*ins->doer)(r, Op_name, arity, Parameters);
        break;

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      default:
        assert(0);
        break;
      } /* switch (ins->class) */

      if ((!state.mpasm_compatible) && (!state.skipped_inst)) {
        if (ins->inv_mask & INV_MASK_BANK) {
          /* Invalidates the selection of RAM Banks. */
          set_global(GLOBAL_ACT_BANK_ADDR, GLOBAL_ACT_BANK_INV, LFT_TEMPORARY, VAL_CONSTANT, true);
        }

        if (ins->inv_mask & INV_MASK_PAGE) {
          /* Invalidates the selection of ROM Pages. */
          set_global(GLOBAL_ACT_PAGE_ADDR, GLOBAL_ACT_PAGE_INV, LFT_TEMPORARY, VAL_CONSTANT, true);
        }
      }

      /* Remember if this instruction may skip the next one. */
      if (ins->class != INSN_CLASS_FUNC) {
        state.skipped_inst = ((ins->inv_mask & INV_MASK_SKIP) != 0);
      }
    } /* if (asm_enabled() || (ins->attribs & ATTRIB_COND)) */
  } /* if (sym != NULL) */
  else {
    sym = sym_get_symbol(state.stMacros, Op_name);

    if (sym != NULL) {
      macro_head_t *h = sym_get_symbol_annotation(sym);

      /* Found the macro: execute it */
      if (asm_enabled()) {
        if ((h->defined == false) && (state.pass == 2)) {
          gperror_error(GPE_UNKNOWN, "Forward references to macros are not allowed.");
        }
        else {
          macro_setup(h, arity, Parameters);
          state.preproc.do_emit = false;
        }
      }
    }
    else {
      if (asm_enabled()) {
        if (!state.processor_chosen) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
        }
        else {
          char mesg[80];

          snprintf(mesg, sizeof(mesg), "Unknown opcode: \"%s\"", Op_name);
          gperror_error(GPE_UNKNOWN, mesg);
        }
      }
    }
  }

leave:
  prev_btfsx = is_btfsx;

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* There are several groups of operations that we handle here.  First
   is op_0: the instructions that can happen before the processor type
   is known.  Second is op_1, the set of instructions that are common
   to all processors, third is processor-family specific: op_XXX */

/* Note that instructions within each group are sorted alphabetically. */

const insn_t op_0[] = {
  { "access_ovr", 0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_access_ovr },
  { "bcdirect",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_direct     },
  { "code",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_code       },
  { "code_pack",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_code_pack  },
  { "constant",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_constant   },
  { "end",        0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_end        },
  { "endm",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_endm       },
  { "endw",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_endw       },
  { "equ",        0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_equ        },
  { "error",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_error      },
  { "exitm",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_exitm      },
  { "expand",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_expand     },
  { "extern",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_extern     },
  { "errorlevel", 0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_errlvl     },
  { "global",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_global     },
  { "idata",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_idata      },
  { "idata_acs",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_idata_acs  },
  { "if",         0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_if         },
  { "ifdef",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_ifdef      },
  { "ifndef",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_ifndef     },
  { "elif",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_elif       },
  { "elifdef",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_elifdef    },
  { "elifndef",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_elifndef   },
  { "else",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_else       },
  { "endif",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_endif      },
  { "include",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_include    },
  { "list",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_list       },
  { "local",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_local      },
  { "macro",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_macro      },
  { "messg",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_messg      },
  { "noexpand",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_noexpand   },
  { "nolist",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_nolist     },
  { "page",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_page       },
  { "processor",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_processor  },
  { "radix",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_radix      },
  { "set",        0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_set        },
  { "space",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_space      },
  { "subtitle",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_subtitle   },
  { "title",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_title      },
  { "udata",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_udata      },
  { "udata_acs",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_udata_acs  },
  { "udata_ovr",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_udata_ovr  },
  { "udata_shr",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_udata_shr  },
  { "variable",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_variable   },
  { "while",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_while      },
  { ".def",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_def        },
  { ".dim",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_dim        },
  { ".direct",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_direct     },
  { ".eof",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_eof        },
  { ".file",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_file       },
  { ".ident",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_ident      },
  { ".line",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_line       },
  { ".set",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_set        },
  { ".type",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_type       },
  { "#if",        0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_if         },
  { "#ifdef",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_ifdef      },
  { "#ifndef",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_ifndef     },
  { "#elif",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_elif       },
  { "#elifdef",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_elifdef    },
  { "#elifndef",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_elifndef   },
  { "#else",      0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_else       },
  { "#endif",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, ATTRIB_COND, _do_endif      },
  { "#define",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_define     },
  { "#undefine",  0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_undefine   }
};

const int num_op_0 = TABLE_SIZE(op_0);

const insn_t op_1[] = {
  { "__badram",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_badram           },
  { "__badrom",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_badrom           },
  { "__config",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_config           },
  { "__fuses",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_config           },
  { "__idlocs",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_idlocs           },
  { "__maxram",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_maxram           },
  { "__maxrom",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_maxrom           },
  { "bankisel",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_bankisel         },
  { "banksel",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_banksel          },
  { "config",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_gpasm_config     },
  { "data",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_data             },
  { "da",         0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_da               },
  { "db",         0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_db               },
  { "de",         0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_de               },
  { "dt",         0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_dt               },
  { "dtm",        0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_dtm              },
  { "dw",         0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_dw               },
  { "fill",       0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_fill             },
  { "idlocs",     0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_16_idlocs        },
  { "org",        0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_org              },
  { "pagesel",    0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_pagesel_wrapper  },
  { "pageselw",   0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_pageselw_wrapper },
  { "res",        0, 0, 0, INSN_CLASS_FUNC, INV_MASK_NULL, 0,           _do_res              }
};

const int num_op_1 = TABLE_SIZE(op_1);

/*------------------------------------------------------------------------------------------------*/

void
opcode_init(int Stage)
{
  const insn_t *base = NULL;
  int           i;
  int           count = 0;
  const char   *name;
  symbol_t     *sym;

  switch (Stage) {
    case 0:
      base  = op_0;
      count = num_op_0;
      break;

    case 1:
      base  = op_1;
      count = num_op_1;
      break;

    case 2: {
      state.device.class = gp_processor_class(state.processor);
      base  = state.device.class->instructions;
      count = (base == NULL) ? 0 : *state.device.class->num_instructions;

      if (IS_SX_CORE) {
        /* The page instruction conflicts with the page directive. */
        sym_remove_symbol(state.stBuiltin, "page");
      }
      else if (IS_PIC16E_CORE) {
        state.device.bsr_boundary = gp_processor_bsr_boundary(state.processor);

        /* The 16_bit core special macros are encoded directly into the
         * symbol table like regular instructions. */
        for (i = 0; i < num_op_18cxx_sp; i++) {
          sym_annotate_symbol(sym_add_symbol(state.stBuiltin, op_18cxx_sp[i].name),
                              (void *)&op_18cxx_sp[i]);
        }

        if (state.extended_pic16e) {
          /* Some 18xx devices have an extended instruction set. */
          for (i = 0; i < num_op_18cxx_ext; i++) {
            sym_annotate_symbol(sym_add_symbol(state.stBuiltin, op_18cxx_ext[i].name),
                                (void *)&op_18cxx_ext[i]);
          }
        }
      }
      else if (IS_PIC14E_CORE) {
        for (i = 0; i < num_op_16cxx_enh; i++) {
          sym_annotate_symbol(sym_add_symbol(state.stBuiltin, op_16cxx_enh[i].name),
                              (void *)&op_16cxx_enh[i]);
        }
      }
      else if (IS_PIC14EX_CORE) {
        for (i = 0; i < num_op_16cxx_enhx; i++) {
          sym_annotate_symbol(sym_add_symbol(state.stBuiltin, op_16cxx_enhx[i].name),
                              (void *)&op_16cxx_enhx[i]);
        }
      }

      break;
    }

    case 3:
      /* add 12 and 14 bit special macros */
      base  = special;
      count = num_op_special;
      break;

    default:
      assert(0);
  }

  for (i = 0; i < count; i++) {
    sym_annotate_symbol(sym_add_symbol(state.stBuiltin, base[i].name), (void *)&base[i]);
  }

  switch (Stage) {
    case 1: {
      if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
        /* The pageselw directive not supported on pic14 enhanced devices. */
        sym_remove_symbol(state.stBuiltin, "pageselw");
      }

      break;
    }

    case 2: {
      if (state.processor != NULL) {
        name = gp_processor_name(state.processor, 0);

        /* Special case, some instructions not available on 17c42 devices. */
        if (strcmp(name, "pic17c42") == 0) {
          sym_remove_symbol(state.stBuiltin, "mulwf");
          sym_remove_symbol(state.stBuiltin, "movlr");
          sym_remove_symbol(state.stBuiltin, "mullw");
        }
        /* Special case, some instructions not available on 16f5x devices. */
        else if ((strcmp(name, "pic16f54") == 0) || (strcmp(name, "pic16f57") == 0) ||
                 (strcmp(name, "pic16f59") == 0)) {
          sym_remove_symbol(state.stBuiltin, "addlw");
          sym_remove_symbol(state.stBuiltin, "sublw");
          sym_remove_symbol(state.stBuiltin, "return");
          sym_remove_symbol(state.stBuiltin, "retfie");
        }
        else if ((strcmp(name, "sx48bd") == 0) || (strcmp(name, "sx52bd") == 0)) {
          sym = sym_get_symbol(state.stBuiltin, "mode");

          if (sym != NULL) {
            sym_annotate_symbol(sym, (void *)&op_sx_mode);
          }
        }
        else if (IS_PIC12E_CORE || IS_PIC12I_CORE) {
          sym_remove_symbol(state.stBuiltin, "return");
          for (i = 0; i < num_op_16c5xx_enh; i++) {
            sym_annotate_symbol(sym_add_symbol(state.stBuiltin, op_16c5xx_enh[i].name),
                                (void *)&op_16c5xx_enh[i]);
          }

          if ((strcmp(name, "pic12f529t39a") == 0) || (strcmp(name, "pic12f529t48a") == 0)) {
            sym_remove_symbol(state.stBuiltin, "retfie");
            sym_remove_symbol(state.stBuiltin, "return");
          }
        }
      }

      break;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
begin_cblock(const pnode_t *Cblock)
{
  if (asm_enabled()) {
    state.cblock_defined = true;
    state.cblock = eval_maybe_evaluate(Cblock);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
continue_cblock(void)
{
  if (!state.cblock_defined) {
    gperror_vmessage(GPM_CBLOCK, NULL);
  }

  state.cblock_defined = true;
}

/*------------------------------------------------------------------------------------------------*/

void
cblock_expr(const pnode_t *Expr)
{
  if (asm_enabled()) {
    set_global(PnSymbol(Expr), state.cblock, LFT_PERMANENT, VAL_CBLOCK, false);
    state.cblock++;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
cblock_expr_incr(const pnode_t *Expr, const pnode_t *Incr)
{
  if (asm_enabled()) {
    set_global(PnSymbol(Expr), state.cblock, LFT_PERMANENT, VAL_CBLOCK, false);
    state.cblock += eval_maybe_evaluate(Incr);
  }
}

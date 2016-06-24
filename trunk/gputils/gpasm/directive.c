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

#define ATTRIB_COND 1

extern pnode_t *mk_constant(int value);

static gp_boolean prev_btfsx = false;

/*------------------------------------------------------------------------------------------------*/

static uint16_t
_check_write(uint16_t value)
{
  uint16_t insn;
  int      org;

  if (state.mode == MODE_RELOCATABLE) {
    if (state.obj.section == NULL) {
      gperror_verror(GPE_WRONG_SECTION, NULL);
      return value;
    }
    /* Don't do program memory checks for data memory. */
    if (IS_RAM_ORG) {
      return value;
    }
  }

  if (IS_PIC16_CORE && (state.byte_addr > 0x1ffff)) {
    gperror_verror(GPE_ADDROVF, "Address{%#x}", state.byte_addr);
  }
  else if ((!IS_PIC16E_CORE) && ((state.byte_addr & 0x1ffff) == 0) && ((int)state.byte_addr > 0)) {
    /* We cast state.byte_addr to signed int on purpose to repeat a bug from
       MPASM 5.34 and pass tb.asm testcase. */
    gperror_error(GPE_ADDROVF, "Address wrapped around 0.");
  }

  org = gp_processor_byte_to_real(state.processor, state.byte_addr);

  if (state.mpasm_compatible) {
    /* MPASM(X) compatible mode. */
    if (value > state.device.class->core_mask) {
      gperror_vmessage(GPM_RANGE, NULL, value);
      value &= state.device.class->core_mask;
    }

    if ((state.num.errors == 0) &&
        state.device.class->i_memory_get(state.i_memory, state.byte_addr, &insn, NULL, NULL)) {
      gperror_verror(GPE_ADDROVR, NULL, org);
    }
  }
  else {
    /* GPASM compatible mode. */
    gp_boolean is_config = (gp_processor_is_config_org(state.processor, org) >= 0) ? true : false;

    if (value > state.device.class->core_mask) {
    /* The size of the config words may be differ the size of the program words. */
      if (!is_config) {
        gperror_vmessage(GPM_RANGE, NULL, value);
        value &= state.device.class->core_mask;
      }
      else if (value > state.device.class->config_mask) {
        gperror_vmessage(GPM_RANGE, NULL, value);
        value &= state.device.class->config_mask;
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
      gperror_vwarning(GPW_EXCEED_ROM, "Address{%#x} > MAXROM{%#x}", org, state.maxrom);
    }
    else {
      /* check if current org is within a bad address range */
      range_pair_t *cur_badrom;

      for (cur_badrom = state.badrom; cur_badrom != NULL; cur_badrom = cur_badrom->next) {
        if ((org >= cur_badrom->start) && (org <= cur_badrom->end)) {
          gperror_vwarning(GPW_EXCEED_ROM, "BADROM_START{%#x} <= Address{%#x} <= BADROM_END{%#x}",
                     cur_badrom->start, org, cur_badrom->end);
          break;
        }
      }
    }
  }
  return value;
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
_emit(uint16_t value, const char *name)
{
  /* only write the program data to memory on the second pass */
  if (state.pass == 2) {
    value = _check_write(value);
    state.device.class->i_memory_put(state.i_memory, state.byte_addr, value, name, NULL);
  }

  state.byte_addr += 2;
}

/*------------------------------------------------------------------------------------------------*/

static void
_emit_byte(uint16_t value, const char *name)
{
  uint8_t       byte;
  uint16_t      word;
  unsigned int  org;
  range_pair_t *cur_badrom;

  if (state.pass == 2) {
    if ((state.mode == MODE_RELOCATABLE) && (state.obj.section == NULL)) {
      gperror_verror(GPE_WRONG_SECTION, NULL);
    }

    if (!IS_RAM_ORG) {
      if (IS_PIC16_CORE && (state.byte_addr > 0x1ffff)) {
        gperror_verror(GPE_ADDROVF, "Address{%#x} > 0x1ffff", state.byte_addr);
      }
      else if ((!IS_PIC16E_CORE) && ((state.byte_addr & 0x1ffff) == 0) && ((int)state.byte_addr > 0)) {
        gperror_error(GPE_ADDROVF, "Address wrapped around 0.");
      }

      if (value > state.device.class->core_mask) {
        word = value & state.device.class->core_mask;

        gperror_vwarning(GPW_RANGE, "%i (%#x) => %i (%#x)", value, value, word, word);
        value = word;
      }

      org = gp_processor_byte_to_real(state.processor, state.byte_addr);

      if ((state.num.errors == 0) &&
          b_memory_get(state.i_memory, state.byte_addr, &byte, NULL, NULL)) {
        gperror_verror(GPE_ADDROVR, NULL, org);
      }

      if (state.maxrom >= 0) {
        if (org > state.maxrom) {
          gperror_vwarning(GPW_EXCEED_ROM, "Address{%#x} > MAXROM{%#x}", org, state.maxrom);
        }
        else {
          /* check if current org is within a bad address range */
          for (cur_badrom = state.badrom; cur_badrom != NULL; cur_badrom = cur_badrom->next) {
            if ((org >= cur_badrom->start) && (org <= cur_badrom->end)) {
              gperror_vwarning(GPW_EXCEED_ROM, "BADROM_START{%#x} <= Address{%#x} <= BADROM_END{%#x}",
                         cur_badrom->start, org, cur_badrom->end);
              break;
            }
          }
        }
      }
    }

    b_memory_put(state.i_memory, state.byte_addr, (uint8_t)value, name, NULL);
  }
  ++state.byte_addr;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_off_or_on(pnode_t *p)
{
  int had_error = false;
  int ret = false;

  if (p->tag != PTAG_SYMBOL) {
    had_error = true;
  }
  else if (strcasecmp(p->value.symbol, "off") == 0) {
    ret = false;
  }
  else if (strcasecmp(p->value.symbol, "on") == 0) {
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
_emit_data(pnode_t *L, unsigned int char_shift, const char *name)
{
  const pnode_t  *p;
  const char     *pc;
  unsigned int    begin_org;
  int             value;
  uint16_t        word;
  unsigned int    n;

  for (begin_org = state.byte_addr; L != NULL; L = TAIL(L)) {
    p = HEAD(L);

    if (p->tag == PTAG_STRING) {
      pc = p->value.string;

      if (IS_PIC16E_CORE && !(SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
        /* Special case of PIC16E strings in code. */
        for (n = 0; *pc != '\0'; ++n) {
          pc = convert_escape_chars(pc, &value);
          _emit_byte(value, name);
        }
        /* Pad using zero if 16-bit numbers. */
        if (n & 1) {
          _emit_byte(0, name);
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
            word = value << char_shift;

            if (*pc != '\0') {
              pc = convert_escape_chars(pc, &value);
              word |= value & 0xff;
            }
          }
          _emit(word, name);
        }
        /* For data and packed emit a terminating nul for strings. */
        if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
          _emit(0, name);
        }
      }
    }
    else if (state.device.class->core_mask > 0xff) {
      word = reloc_evaluate(p, RELOCT_ALL);
      _emit(word, name);
    }
    else {
      /* FIXME: This case is for EEPROM8. Do we need the RELOCT_LOW? */
      word = reloc_evaluate(p, RELOCT_LOW);
      _emit_byte(word, name);
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
_macro_parms_simple(pnode_t *parms)
{
  while (parms != NULL) {
    if (HEAD(parms)->tag != PTAG_SYMBOL) {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
      return false;
    }
    parms = TAIL(parms);
  }
  return true;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_macro_parm_unique(pnode_t *M, pnode_t *L)
{
  while (L != NULL) {
    if (STRCMP(M->value.symbol, HEAD(L)->value.symbol) == 0) {
      char buf[BUFSIZ];

      snprintf(buf, sizeof(buf), "Duplicate macro parameter (%s).", HEAD(L)->value.symbol);
      gperror_error(GPE_UNKNOWN, buf);
      return false;
    }
    L = TAIL(L);
  }
  return true;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_macro_parms_ok(pnode_t *parms)
{
  /* Check if all params are symbols. */
  if (!_macro_parms_simple(parms)) {
    return false;
  }

  /* Check if params are unique. */
  while (parms != NULL) {
    if (!_macro_parm_unique(HEAD(parms), TAIL(parms))) {
      return false;
    }
    parms = TAIL(parms);
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_access_ovr(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".access_ovr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_ACCESS | STYP_BSS | STYP_OVERLAY;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sect_name, ".access_ovr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_ACCESS | STYP_BSS | STYP_OVERLAY | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_badram(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  int            maxram;
  int            start;
  int            end;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_DIR;

  if (parms == NULL) {
    gperror_verror(GPE_MISSING_ARGU, NULL);
  }
  else {
    maxram = state.maxram;
    if (maxram >= MAX_RAM) {
      maxram = MAX_RAM - 1;
    }

    for (; parms != NULL; parms = TAIL(parms)) {
      p = HEAD(parms);

      if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '-')) {
        if (can_evaluate(p->value.binop.p0) && can_evaluate(p->value.binop.p1)) {
          start = evaluate(p->value.binop.p0);
          end = evaluate(p->value.binop.p1);

          if (end < start) {
            gperror_vwarning(GPW_INVALID_RAM, "Start{%#x} > End{%#x}", start, end);
          }
          else if (start < 0) {
            gperror_vwarning(GPW_INVALID_RAM, "Start{%i} < 0", start);
          }
          else if (end > maxram) {
            gperror_vwarning(GPW_INVALID_RAM, "End{%#x} > MAXRAM{%#x}", end, maxram);
          }
          else {
            for (; start <= end; start++) {
              state.badram[start] = 1;
            }
          }
        }
      }
      else {
        if (can_evaluate(p)) {
          int loc;

          loc = evaluate(p);

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

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_badrom(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_DIR;

  /* FIXME: implement this directive */
  gperror_warning(GPW_UNKNOWN, "gpasm doesn't support the badrom directive yet.");

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_bankisel(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  int            num_reloc;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (((!IS_PIC14_CORE) && (!IS_PIC14E_CORE) && (!IS_PIC14EX_CORE) && (!IS_PIC16_CORE)) ||
      (state.processor->num_banks == 1)) {
    state.lst.line.linetype = LTY_NONE;
    gperror_vmessage(GPM_EXTPAGE, NULL);
    return r;
  }

  if (prev_btfsx) {
    gperror_vwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Bankisel");
  }

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (state.mode == MODE_ABSOLUTE) {
      state.byte_addr += gp_processor_set_ibank(state.device.class,
                                                state.processor->num_banks,
                                                gp_processor_check_ibank(state.device.class, maybe_evaluate(p)),
                                                state.i_memory, state.byte_addr);
    }
    else {
      num_reloc = count_reloc(p);

      if (num_reloc == 0) {
        /* it is an absolute address, generate the bankisel but no relocation */
        state.byte_addr += gp_processor_set_ibank(state.device.class,
                                                  state.processor->num_banks,
                                                  gp_processor_check_ibank(state.device.class, maybe_evaluate(p)),
                                                  state.i_memory, state.byte_addr);
      }
      else if (num_reloc != 1) {
        gperror_verror(GPE_UNRESOLVABLE, "\"%s\"", name);
      }
      else {
        reloc_evaluate(p, RELOCT_IBANKSEL);

        if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
          unsigned int mask;

          for (mask = 0x100; mask < (state.processor->num_banks << PIC14_BANK_SHIFT); mask <<= 1) {
            _emit(0, name);
          }
        }
        else {
          _emit(0, name);
        }
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_banksel(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  int            address;
  int            bank;
  int            num_reloc;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (state.processor->num_banks == 1) {
    state.lst.line.linetype = LTY_NONE;
    gperror_vmessage(GPM_EXTPAGE, NULL);
    /* do nothing */
    if (!state.mpasm_compatible) {
      set_global(GLOBAL_ACT_BANK_ADDR, 0, LFT_TEMPORARY, GVT_CONSTANT, true);
    }
    return r;
  }

  if (prev_btfsx) {
    gperror_vwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Banksel");
  }

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (state.mode == MODE_ABSOLUTE) {
      address = maybe_evaluate(p);
      bank = gp_processor_check_bank(state.device.class, address);
      state.byte_addr += gp_processor_set_bank(state.device.class,
                                               state.processor->num_banks,
                                               bank,
                                               state.i_memory,
                                               state.byte_addr);
      if (!state.mpasm_compatible) {
        set_global(GLOBAL_ACT_BANK_ADDR,
                   gp_processor_bank_num_to_addr(state.processor, bank),
                   LFT_TEMPORARY, GVT_CONSTANT, true);
      }
    }
    else {
      /* state.mode == MODE_RELOCATABLE */
      num_reloc = count_reloc(p);

      if (num_reloc == 0) {
        /* It is an absolute address, generate the banksel but no relocation. */
        address = maybe_evaluate(p);
        bank = gp_processor_check_bank(state.device.class, address);
        state.byte_addr += gp_processor_set_bank(state.device.class,
                                                 state.processor->num_banks,
                                                 bank,
                                                 state.i_memory,
                                                 state.byte_addr);
      }
      else if (num_reloc != 1) {
        gperror_verror(GPE_UNRESOLVABLE, "\"%s\"", name);
      }
      else if (IS_PIC12E_CORE || IS_PIC12I_CORE) {
        reloc_evaluate(p, RELOCT_MOVLB);
        _emit(PIC12E_INSN_MOVLB, name);
      }
      else if (IS_PIC14E_CORE) {
        reloc_evaluate(p, RELOCT_MOVLB);
        _emit(PIC14E_INSN_MOVLB, name);
      }
      else if (IS_PIC14EX_CORE) {
        reloc_evaluate(p, RELOCT_MOVLB);
        _emit(PIC14EX_INSN_MOVLB, name);
      }
      else if (IS_PIC16_CORE) {
        reloc_evaluate(p, RELOCT_BANKSEL);
        _emit(0x0000, name);
      }
      else if (IS_PIC16E_CORE) {
        reloc_evaluate(p, RELOCT_BANKSEL);
        _emit(PIC16E_INSN_MOVLB, name);
      }
      else {
        switch (state.processor->num_banks) {
        case 2:
          reloc_evaluate(p, RELOCT_BANKSEL);
          _emit(0, name);
          break;

        case 4:
          reloc_evaluate(p, RELOCT_BANKSEL);
          _emit(0, name);
          _emit(0, name);
          break;
        }
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_code(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".code", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_TEXT;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sect_name, ".code", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = gp_processor_org_to_byte(state.device.class, maybe_evaluate(p));
      state.obj.new_sect_flags = STYP_TEXT | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_code_pack(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (!IS_PIC16E_CORE) {
    gperror_error(GPE_UNKNOWN, "code_pack is only supported on 16bit cores.");
  }
  else {
    state.lst.line.linetype = LTY_SEC;
    state.next_state = STATE_SECTION;

    if (state.mode == MODE_ABSOLUTE) {
      gperror_verror(GPE_OBJECT_ONLY, NULL, name);
    }
    else {
      switch (arity) {
      case 0:
        /* new relocatable section */
        gp_strncpy(state.obj.new_sect_name, ".code", sizeof(state.obj.new_sect_name));
        state.obj.new_sect_addr  = 0;
        state.obj.new_sect_flags = STYP_TEXT | STYP_BPACK;
        break;

      case 1:
        /* new absolute section */
        p = HEAD(parms);
        gp_strncpy(state.obj.new_sect_name, ".code", sizeof(state.obj.new_sect_name));
        state.obj.new_sect_addr  = gp_processor_org_to_byte(state.device.class, maybe_evaluate(p));
        state.obj.new_sect_flags = STYP_TEXT | STYP_BPACK | STYP_ABS;
        break;

      default:
        enforce_arity(arity, 1);
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_constant(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  gp_boolean     first;
  const char    *lhs;
  gpasmVal       val;

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_SET4;

  first = true;
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        /* fetch the symbol */
        lhs = p->value.binop.p0->value.symbol;
        /* constants must be assigned a value at declaration */

        val = maybe_evaluate(p->value.binop.p1);
        /* put the symbol and value in the table*/
        set_global(lhs, val, LFT_PERMANENT, GVT_CONSTANT, false);

        if (first) {
          r = val;
          first = false;
        }
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
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
 * do_config() - Called by the parser when a __CONFIG assembler directive
 *               is encountered.
 *
 * do_gpasm_config() - Called by the parser to process MPASM(X) style CONFIG xx=yy
 *                     directives for all devices.
 */

static gp_boolean config_us_used    = false;
static gp_boolean config_mpasm_used = false;

/*
 * Creates the configuration or device id COFF section for do_config and
 * do_gpasm_config. Returns true when a section is created.
 */

static MemBlock_t *
_find_conf_sec_mem(unsigned int ca)
{
  conf_mem_block_t *p;
  int               ba = (IS_PIC16_CORE || IS_PIC16E_CORE) ? ca : (ca - (ca & 1));

  for (p = state.conf_sec_mem; p != NULL; p = p->next) {
    if (p->addr == ba) {
      return p->m;
    }
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

static MemBlock_t *
_add_conf_sec_mem(unsigned int ca, gp_boolean new_config)
{
  conf_mem_block_t *new = GP_Malloc(sizeof(conf_mem_block_t));
  conf_mem_block_t *p;

  new->addr = ((IS_PIC16_CORE || IS_PIC16E_CORE) && new_config) ? ca : (ca & ~1);
  new->m = i_memory_create();
  new->new_config = new_config;

  if (state.debug_info) {
    new->file_symbol = state.obj.debug_file;
    new->line_number = state.obj.debug_line;
  }
  else {
    new->file_symbol = state.src->file_symbol;
    new->line_number = state.src->line_number;
  }
  new->next = NULL;

  if (state.conf_sec_mem != NULL) {
    for (p = state.conf_sec_mem; p->next != NULL; p = p->next)
      ;
    p->next = new;
  }
  else {
    state.conf_sec_mem = new;
  }

  return new->m;
}

/*------------------------------------------------------------------------------------------------*/

static MemBlock_t *
_get_config_mem(unsigned int ca, gp_boolean new_config)
{
  MemBlock_t *mem;

  if (state.mode != MODE_RELOCATABLE) {
    return state.c_memory;
  }
  else {
    mem = _find_conf_sec_mem(ca);

    if (mem == NULL) {
      mem = _add_conf_sec_mem(ca, new_config);
    }

    return mem;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* helper to write configuration data, grabbing defaults when necessary */

static void
_config_16_set_byte_mem(MemBlock_t *config_mem, const gp_cfg_device_t *p_dev, unsigned int ca,
                        uint8_t byte, uint8_t mask)
{
  uint8_t old_byte;
  char    buf[BUFSIZ];

  snprintf(buf, sizeof(buf), "CONFIG_%#x", ca);

  if (!b_memory_get(config_mem, ca, &old_byte, NULL, NULL)) {
    old_byte = (uint8_t)gp_cfg_get_default(p_dev, ca);
  }

  b_memory_put(config_mem, ca, (old_byte & ~mask) | byte, buf, NULL);
}

/*------------------------------------------------------------------------------------------------*/

static void
_config_16_set_word_mem(MemBlock_t *config_mem, const gp_cfg_device_t *p_dev,
                        unsigned int ca, uint8_t byte, uint8_t mask)
{
  uint8_t other_byte;
  char    buf[BUFSIZ];

  if (!b_memory_get(config_mem, ca ^ 1, &other_byte, NULL, NULL)) {
    snprintf(buf, sizeof(buf), "CONFIG_%#x", ca);
    other_byte = gp_cfg_get_default(p_dev, ca ^ 1);
    b_memory_put(config_mem, ca ^ 1, other_byte, buf, NULL);
  }
  _config_16_set_byte_mem(config_mem, p_dev, ca, byte, mask);
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_config(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t         *p;
  int                    ca;
  int                    value;
  MemBlock_t            *config_mem;
  const gp_cfg_device_t *p_dev;
  uint16_t               word;
  char                   buf[BUFSIZ];

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  config_us_used = true;

  if (config_mpasm_used) {
    gperror_verror(GPE_CONFIG_usCONFIG, NULL);
    return r;
  }

  if (IS_PIC16E_CORE) {
    gperror_warning(GPW_EXPECTED, "__CONFIG has been deprecated for PIC18 devices. Use directive CONFIG.");
  }

  state.lst.line.linetype = LTY_CONFIG;

  switch(arity) {
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
    p = HEAD(parms);
    break;

  case 2:
    ca = maybe_evaluate(HEAD(parms));
    p = HEAD(TAIL(parms));
    break;

  default:
    enforce_arity(arity, 2);
    return r;
  }

  ca = gp_processor_org_to_byte(state.device.class, ca);
  state.lst.config_address = ca;

  if ((can_evaluate(p)) && (state.pass == 2)) {
    config_mem = _get_config_mem(ca, false);
    value = evaluate(p);

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

      snprintf(buf, sizeof(buf), "CONFIG_%#x", gp_processor_byte_to_org(state.device.class, ca));
      state.device.class->i_memory_put(config_mem, ca, value, buf, NULL);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* Sets defaults over unused portions of configuration memory. */

static void
_config_16_check_defaults(MemBlock_t *config_mem, const gp_cfg_device_t *p_dev)
{
  const gp_cfg_addr_t *addrs = p_dev->addresses;
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
  for (t = 0; t < p_dev->address_count; ++addrs, ++t) {
    if (!b_memory_get(config_mem, addrs->address, &byte, NULL, NULL)) {
      _config_16_set_byte_mem(config_mem, p_dev, addrs->address, addrs->def_value, 0xff);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Support MPASM(X) style CONFIG xxx = yyy syntax for PIC16(E) devices. */

static gpasmVal
_do_16_config(gpasmVal r, const char *name, int arity, const pnode_t *parms)
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
    return r;
  }

  /* make sure we an find our device in the config DB */
  p_dev = gp_cfg_find_pic_multi_name(state.processor->names, ARRAY_SIZE(state.processor->names));
  if (p_dev == NULL) {
    gperror_verror(GPE_UNKNOWN, "The %s processor has no entries in the config db. CONFIG cannot be used.",
             state.processor->names[2]);
    return r;
  }

  /* validate argument format */
  if ((parms == NULL) || (parms->tag != PTAG_BINOP) || (parms->value.binop.op != '=')) {
    gperror_error(GPE_CONFIG_UNKNOWN, "Incorrect syntax. Use `CONFIG KEY = VALUE'");
    return r;
  }

  /* validate parameter types */
  k = parms->value.binop.p0;
  v = parms->value.binop.p1;

  if ((k->tag != PTAG_SYMBOL) ||
      ((v->tag != PTAG_SYMBOL) && (v->tag != PTAG_CONSTANT))) {
    gperror_error(GPE_CONFIG_UNKNOWN, "Incorrect syntax. Use 'CONFIG KEY = VALUE'");
    return r;
  }

  /* grab string representations */
  k_str = k->value.symbol;

  if (v->tag != PTAG_CONSTANT) {
    v_str = v->value.symbol;
  }
  else {
    int value = v->value.constant;

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
    return r;
  }

  /* Note address to lister, though it doesn't seem to use it. */
  state.lst.config_address = ca;

  /* find the option */
  p_opt = gp_cfg_find_option(p_dir, v_str);
  if (p_opt == NULL) {
    snprintf(buf, sizeof(buf), "CONFIG Directive Error: Specified value not valid for directive: \"%s\"", v_str);
    gperror_error(GPE_UNKNOWN, buf);
    return r;
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
        return r;
      }
      double_mask[dm_addr] |= p_dir->mask;
    }
    else {
      gperror_warning(GPW_UNKNOWN, "Double_mask in do_16_config() needs to be adjusted to account for larger config ranges.");
    }

    /* Let the helper set the data. */
    _config_16_set_byte_mem(config_mem, p_dev, ca, (uint8_t)p_opt->value, (uint8_t)p_dir->mask);
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static void
_config_12_14_set_word_mem(MemBlock_t *config_mem, const gp_cfg_device_t *p_dev,
                           unsigned int ca, uint16_t word, uint16_t mask)
{
  unsigned int org;
  uint16_t     old_word;
  char         buf[BUFSIZ];

  org = gp_processor_byte_to_org(state.device.class, ca);
  snprintf(buf, sizeof(buf), "CONFIG_%#x", org);

  if (!state.device.class->i_memory_get(config_mem, ca, &old_word, NULL, NULL)) {
    old_word = gp_cfg_get_default(p_dev, org);
  }

  word |= (old_word & ~mask);
  word &= state.device.class->config_mask;
  state.device.class->i_memory_put(config_mem, ca, word, buf, NULL);
}

/*------------------------------------------------------------------------------------------------*/

/* Sets defaults over unused portions of configuration memory. */

static void
_config_12_14_check_defaults(MemBlock_t *config_mem, const gp_cfg_device_t *p_dev)
{
  const gp_cfg_addr_t *addrs = p_dev->addresses;
  unsigned int         addr;
  uint16_t             word;
  unsigned int         t;

  for (t = 0; t < p_dev->address_count; ++addrs, ++t) {
    addr = gp_processor_org_to_byte(state.device.class, addrs->address);

    if (!state.device.class->i_memory_get(config_mem, addr, &word, NULL, NULL)) {
      word = addrs->def_value & state.device.class->config_mask;
      state.device.class->i_memory_put(config_mem, addr, word, NULL, NULL);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Support MPASM(X) style CONFIG xxx = yyy syntax for PIC14(E) and PIC12(E) devices. */

static gpasmVal
_do_12_14_config(gpasmVal r, const char *name, int arity, const pnode_t *parms)
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
    return r;
  }

  /* make sure we an find our device in the config DB */
  p_dev = gp_cfg_find_pic_multi_name(state.processor->names, ARRAY_SIZE(state.processor->names));
  if (p_dev == NULL) {
    gperror_verror(GPE_UNKNOWN,
             "The %s processor has no entries in the config db. CONFIG cannot be used.",
             state.processor->names[2]);
    return r;
  }

  /* validate argument format */
  if ((parms == NULL) || (parms->tag != PTAG_BINOP) || (parms->value.binop.op != '=')) {
    gperror_error(GPE_CONFIG_UNKNOWN, "Incorrect syntax. Use 'CONFIG KEY = VALUE'");
    return r;
  }

  /* validate parameter types */
  k = parms->value.binop.p0;
  v = parms->value.binop.p1;

  if ((k->tag != PTAG_SYMBOL) || ((v->tag != PTAG_SYMBOL) && (v->tag != PTAG_CONSTANT))) {
    gperror_error(GPE_CONFIG_UNKNOWN, "Incorrect syntax. Use 'CONFIG KEY = VALUE'");
    return r;
  }

  /* grab string representations */
  k_str = k->value.symbol;

  if (v->tag != PTAG_CONSTANT) {
    v_str = v->value.symbol;
  }
  else {
    int value = v->value.constant;

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
    return r;
  }

  /* Note address to lister, though it doesn't seem to use it. */
  ca = gp_processor_org_to_byte(state.device.class, conf_org);
  state.lst.config_address = ca;

  /* find the option */
  p_opt = gp_cfg_find_option(p_dir, v_str);
  if (p_opt == NULL) {
    snprintf(buf, sizeof(buf), "CONFIG Directive Error: Specified value not valid for directive: \"%s\"", v_str);
    gperror_error(GPE_UNKNOWN, buf);
    return r;
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
        return r;
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

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_gpasm_config(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  char           buf[BUFSIZ];

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (state.mpasm_compatible) {
    /* The MPASM(X) compatible mode valid only for 16 bit devices. */
    if ((!IS_PIC16_CORE) && (!IS_PIC16E_CORE)) {
      snprintf(buf, sizeof(buf),
               "CONFIG Directive Error: Processor \"%s\" is invalid for CONFIG directive in MPASM(X) mode.",
               state.processor->names[2]);
      gperror_error(GPE_UNKNOWN, buf);
      return r;
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
      return r;
    }
  }

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if (IS_PIC16_CORE || IS_PIC16E_CORE) {
      _do_16_config(r, name, arity, p);
    }
    else {
      _do_12_14_config(r, name, arity, p);
    }
  }

  if (state.mpasm_compatible) {
    /* A new section must be started after this directive. */
    state.obj.section = NULL;
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/*
 *  _do_da - The 'da' directive. Identical to 'data' directive, except
 *           for 14-bit cores it packs two 7-bit characters into one word.
 */

static gpasmVal
_do_da(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  unsigned int char_shift;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  char_shift = (IS_PIC14_CORE || IS_PIC14E_CORE || IS_PIC14EX_CORE) ? 7 : 8;

  if ((state.mode == MODE_RELOCATABLE) && (SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
    /* This is a data memory not program. */
    state.lst.line.linetype = LTY_DATA;
    char_shift = 8;
  }

  _emit_data(parms, char_shift, name);

  return r;
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
_do_data(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if ((state.mode == MODE_RELOCATABLE) && (SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
    /* This is a data memory not program. */
    state.lst.line.linetype = LTY_DATA;
  }

  _emit_data(parms, 8, name);

  return r;
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
_do_db(gpasmVal r, const char *name, int arity, pnode_t *parms)
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
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (state.mode == MODE_RELOCATABLE) {
    if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
      /* This is a data memory not program. */
      state.lst.line.linetype = LTY_DATA;
    }
    else if (!(SECTION_FLAGS & STYP_TEXT)) {
      /* only valid in initialized data and text sections */
      gperror_verror(GPE_WRONG_SECTION, NULL);
      return r;
    }
  }

  L = parms;

  if (IS_PIC16E_CORE || (SECTION_FLAGS & STYP_DATA)) {
    begin_byte_addr = state.byte_addr;

    for (; L != NULL; L = TAIL(L)) {
      p = HEAD(L);

      if (p->tag == PTAG_STRING) {
        pc = p->value.string;
        while (*pc != '\0') {
          pc = convert_escape_chars(pc, &value);
          _emit_byte(value, name);
        }
      }
      else {
        value = reloc_evaluate(p, RELOCT_LOW);

        if (value < 0) {
          gperror_vwarning(GPW_RANGE, "%i (%#x) < 0", value, value);
        }
        else if (value > 0xFF) {
          gperror_vwarning(GPW_RANGE, "%i (%#x) > 0xff", value, value);
        }

        _emit_byte(value, name);
      }
    }

    if (state.mpasm_compatible) {
      if ((state.mode == MODE_ABSOLUTE) || !(SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
        if ((state.byte_addr - begin_byte_addr) & 1) {
          _emit_byte(0, name);
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
            _emit_byte(0, name);
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
      p = HEAD(L);

      if (p->tag == PTAG_STRING) {
        pc = p->value.string;
      }

      for (; ; ) {
        if (p->tag == PTAG_STRING) {
          pc = convert_escape_chars(pc, &value);
        }
        else {
          value = reloc_evaluate(p, RELOCT_LOW);
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
            _emit(v | value, name);
          }
        }
        else {
          org = gp_processor_byte_to_real(state.processor, state.byte_addr);

          if (gp_processor_is_eeprom_org(state.processor, org) >= 0) {
            _emit_byte(value, name);
          }
          else {
            if (!(n & 1)) {
              v = value << 8;
            }
            else {
              _emit(v | value, name);
            }
          }
        }

        ++n;

        if ((p->tag != PTAG_STRING) || (*pc == '\0')) {
          L = TAIL(L);
          break;
        }
      } /* for (; ; ) */
    } /* while (L != NULL) */

    if (state.mpasm_compatible) {
      if (n & 1) {
        _emit(v, name);
      }
    }
    else {
      org = gp_processor_byte_to_real(state.processor, state.byte_addr);

      if (gp_processor_is_eeprom_org(state.processor, org) < 0) {
        if (n & 1) {
          _emit(v, name);
        }
      }
    }
  } /* else */
  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_de(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  const char    *pc;
  int            value;
  uint16_t       v;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (IS_PIC16E_CORE) {
    return _do_db(r, name, arity, parms);
  }

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      pc = p->value.string;

      while (*pc != '\0') {
        pc = convert_escape_chars(pc, &value);
        _emit(value & 0xff, name);
      }
    }
    else {
      v = reloc_evaluate(p, RELOCT_ALL);
      _emit(v & 0xff, name);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* Extension to MPASM(X), used at least by LLVM to emit debugging information. */

static gpasmVal
_do_def(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t      *p;
  char               *symbol_name = NULL;
  gp_symbol_t        *coff_symbol = NULL;
  int                 eval;
  int                 value = 0;
  gp_boolean          new_class = false;
  int                 coff_class = C_NULL;
  gp_boolean          new_type = false;
  int                 coff_type = T_NULL;
  enum gpasmValTypes  type = GVT_DEBUG;
  char               *lhs;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    if (arity < 2) {
      enforce_arity(arity, 2);
      return r;
    }

    /* the first argument is the symbol name */
    p = HEAD(parms);

    if (enforce_simple(p)) {
      symbol_name = p->value.symbol;
    }
    else {
      return r;
    }

    parms = TAIL(parms);

    /* update the properties */
    for (; parms != NULL; parms = TAIL(parms)) {
      p = HEAD(parms);

      if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '=')) {
        if (enforce_simple(p->value.binop.p0)) {
          lhs = p->value.binop.p0->value.symbol;

          if (strcasecmp(lhs, "value") == 0) {
            value = maybe_evaluate(p->value.binop.p1);
          }
          else if (strcasecmp(lhs, "size") == 0) {
            eval = maybe_evaluate(p->value.binop.p1);
            state.byte_addr += IS_RAM_ORG ? eval : gp_processor_org_to_byte(state.device.class, eval);
          }
          else if (strcasecmp(lhs, "type") == 0) {
            eval = maybe_evaluate(p->value.binop.p1);

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
            eval = maybe_evaluate(p->value.binop.p1);

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
        if (enforce_simple(p)) {
          if (strcasecmp(p->value.symbol, "absolute") == 0) {
            type  = GVT_ABSOLUTE;
            value = 0;
          }
          else if (strcasecmp(p->value.symbol, "debug") == 0) {
            type  = GVT_DEBUG;
            value = 0;
          }
          else if (strcasecmp(p->value.symbol, "extern") == 0) {
            type  = GVT_EXTERN;
            value = 0;
          }
          else if (strcasecmp(p->value.symbol, "global") == 0) {
            type  = GVT_GLOBAL;
            value = IS_RAM_ORG ? state.byte_addr : gp_processor_byte_to_org(state.device.class, state.byte_addr);
          }
          else if (strcasecmp(p->value.symbol, "static") == 0) {
            type  = GVT_STATIC;
            value = IS_RAM_ORG ? state.byte_addr : gp_processor_byte_to_org(state.device.class, state.byte_addr);
          }
          else {
            gperror_verror(GPE_ILLEGAL_ARGU, NULL, p->value.symbol);
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

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_define(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  pnode_t *p;

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (arity < 1) {
    gperror_verror(GPE_MISSING_ARGU, NULL);
  }
  else {
    assert(parms->tag == PTAG_LIST);
    p = HEAD(parms);
    assert(p->tag == PTAG_STRING);

    if (asm_enabled() && !IN_MACRO_WHILE_DEFINITION) {
      if (sym_get_symbol(state.stDefines, p->value.string) != NULL) {
        gperror_verror(GPE_DUPLAB, NULL, p->value.string);
      }
      else {
        symbol_t *curr_def = sym_add_symbol(state.stDefines, p->value.string);

        p = TAIL(parms);

        if (p != NULL) {
          assert(p->tag == PTAG_LIST);
          sym_annotate_symbol(curr_def, p);
        }
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* Extension to MPASM(X), used at least by LLVM to emit debugging information. */

static gpasmVal
_do_dim(gpasmVal r, const char *name, int arity, pnode_t *parms)
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
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    if (arity < 3) {
      enforce_arity(arity, 1);
      return r;
    }

    /* the first argument is the symbol name */
    p = HEAD(parms);

    if (enforce_simple(p)) {
      /* lookup the symbol */
      symbol_name = p->value.symbol;
      coff_symbol = gp_coffgen_find_symbol(state.obj.object, symbol_name);

      if (coff_symbol == NULL) {
        gperror_verror(GPE_NOSYM, NULL, symbol_name);
        return r;
      }
    }
    else {
      return r;
    }
    parms = TAIL(parms);

    /* the second argument must be the number of aux symbols */
    p = HEAD(parms);
    number_symbols = maybe_evaluate(p);

    if ((number_symbols < 0) || (number_symbols > 127)) {
      gperror_error(GPE_UNKNOWN, "Number of auxiliary symbols must be less then 128 and positive.");
      return r;
    }

    state.obj.symbol_num += number_symbols;
    parms = TAIL(parms);

    /* create the symbols */
    aux_list = gp_coffgen_add_aux(state.obj.object, coff_symbol);

    /* write the data to the auxiliary symbols */
    i = 0;
    while (parms != NULL) {
      p = HEAD(parms);
      value = maybe_evaluate(p);

      if (value & (~0xff)) {
        gperror_verror(GPE_RANGE, "%i (%#x) > 0xff", value, value);
        return r;
      }

      if (aux_list == NULL) {
        gperror_error(GPE_UNKNOWN, "Insufficent number of auxiliary symbols.");
        return r;
      }

      if (i == (state.obj.newcoff ? SYMBOL_SIZE_v2 : SYMBOL_SIZE_v1)) {
        i = 0;
        aux_list = gp_coffgen_add_aux(state.obj.object, coff_symbol);
      }

      aux_list->_aux_symbol.data[i++] = value;

      parms = TAIL(parms);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_direct(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  pnode_t       *p;
  int            value;
  uint8_t        direct_command = 0;
  const char    *direct_string  = NULL;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 2)) {
    p = HEAD(parms);
    coerce_str1(p);
    value = maybe_evaluate(p);

    if (value < 0) {
      gperror_verror(GPE_RANGE, "%i (%#x) < 0", value, value);
    }
    else if (value > WHILE_LOOP_COUNT_MAX) {
      gperror_verror(GPE_RANGE, "%i (%#x) > %u", value, value, WHILE_LOOP_COUNT_MAX);
    }
    else {
      direct_command = value;
    }

    p = HEAD(TAIL(parms));
    if (p->tag == PTAG_STRING) {
      if (strlen(p->value.string) < 255) {
        direct_string = convert_escaped_char(p->value.string, '"');
      }
      else {
        gperror_error(GPE_UNKNOWN, "String must be less than 255 bytes long.");
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }

    if (direct_string == NULL) {
      return r;
    }

    if (SECTION_FLAGS & STYP_TEXT) {
      coff_add_directsym(direct_command, direct_string);
    }
    else {
      gperror_verror(GPE_WRONG_SECTION, NULL);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_dt(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  int            retlw;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  retlw = gp_processor_retlw(state.device.class);

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      const char *pc = p->value.string;

      while (*pc != '\0') {
        int value;

        pc = convert_escape_chars(pc, &value);
        _emit((value & 0xff) | retlw, name);
      }
    }
    else {
      uint16_t v;

      v = reloc_evaluate(p, RELOCT_ALL);
      _emit((v & 0xff) | retlw, name);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_dtm(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t  *p;
  const symbol_t *s;
  const insn_t   *i;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  s = sym_get_symbol(state.stBuiltin, "movlw");
  i = sym_get_symbol_annotation(s);

  if ((!IS_PIC14E_CORE) && (!IS_PIC14EX_CORE)) {
    gperror_verror(GPE_ILLEGAL_DIR, NULL, name);
  }

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      const char *pc = p->value.string;

      while (*pc != '\0') {
        int value;

        pc = convert_escape_chars(pc, &value);
        _emit(i->opcode | (value & 0xff), name);
      }
    }
    else {
      uint16_t v;

      v = reloc_evaluate(p, RELOCT_ALL);
      _emit(i->opcode | (v & 0xff), name);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/*
 *  _do_dw - The 'dw' directive. On all families except for the p18cxxx, the
 *           dw directive is the same as the 'data' directive. For the p18cxxx
 *           it's the same as the 'db' directive. (That's strange, but it's
 *           also the way mpasm does it.)
 */

static gpasmVal
_do_dw(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (state.mode == MODE_RELOCATABLE) {
    if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
      /* This is a data memory not program. */
      state.lst.line.linetype = LTY_DATA;
    }
    else if (!(SECTION_FLAGS & STYP_TEXT)) {
      /* only valid in initialized data and text sections */
      gperror_verror(GPE_WRONG_SECTION, NULL);
      return r;
    }
  }
  /* MPASM 5.34 seems to do this same for p18cxxx as for others. */
  _emit_data(parms, 8, name);

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_else(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (_check_processor_select(name)) {
    return r;
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

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_end(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  state.found_end = true;
  state.lst.line.linetype = LTY_DIR;

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_endif(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (_check_processor_select(name)) {
    return r;
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
    amode_t *old;

    old = state.astack;
    state.astack = state.astack->upper;
    free(old);
  }
  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_endm(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (_check_processor_select(name)) {
    return r;
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

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_endw(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_NOLIST_DIR;
  assert(state.mac_head == NULL);

  if (!IN_MACRO_WHILE_DEFINITION) {
    gperror_error(GPE_ILLEGAL_COND, "Illegal condition: \"ENDW\"");
  }
  else if (maybe_evaluate(state.while_head->parms)) {
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

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_eof(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 0)) {
    if (state.debug_info) {
      coff_add_eofsym();
    }
    else {
      gperror_warning(GPW_UNKNOWN, "Directive ignored when debug info is disabled.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_equ(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_EQU;

  if (enforce_arity(arity, 1)) {
    r = maybe_evaluate(HEAD(parms));
  }

  return r;
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
_do_error(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  const char    *str;

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      if ((str = _hv_macro_resolver(p->value.string)) != NULL) {
        gperror_verror(GPE_USER, NULL, str);
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/************************************************************************
 * do_errlvl - parse the ERRORLEVEL directive
 *
 * If the file gpasm is assembling contains a ERRORLEVEL directive, then scan
 * the comma delimited list of options in *parms
 *
 * Inputs:
 *   gpasmVal r - not used, but is returned
 *   char *name - not used, but contains the directive name 'list'.
 *   int arity - not used, but should contain '1'
 *   pnode_t *parms - a linked list of the parsed parameters
 *
 *
 *
 ************************************************************************/

static gpasmVal
_do_errlvl(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;

  if (state.pass == 2) {
    for (; parms != NULL; parms = TAIL(parms)) {
      p = HEAD(parms);

      if (p->tag == PTAG_UNOP)  {
        gpasmVal value = evaluate(p->value.unop.p0);

        if (p->value.unop.op == '-') {
          gperror_add_code(-value);
        }
        else if (p->value.unop.op == '+') {
          gperror_add_code(value);
        }
        else {
          gperror_error(GPE_ILLEGAL_ARGU, "Expected 0, 1, 2, +|-<message number>.");
        }
      }
      else if (p->tag == PTAG_CONSTANT) {
        select_errorlevel(p->value.constant);
      }
      else {
        gperror_error(GPE_ILLEGAL_ARGU, "Expected 0, 1, 2, +|-<message number>.");
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_exitm(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 0)) {
    if (state.stGlobal == state.stTop) {
      gperror_error(GPE_UNKNOWN, "Attempt to use \"exitm\" outside of macro.");
    }
    else {
      state.next_state = STATE_EXITMACRO;
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_expand(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (state.cmd_line.macro_expand) {
    gperror_vmessage(GPM_SUPLIN, NULL);
  }
  else {
    if (enforce_arity(arity, 0)) {
      state.lst.expand = true;
    }
  }
  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_extern(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const char *p;

  state.lst.line.linetype = LTY_SET4;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    for (; parms != NULL; parms = TAIL(parms)) {
      p = HEAD(parms)->value.symbol;

      if (p != NULL) {
        set_global(p, 0, LFT_PERMANENT, GVT_EXTERN, false);
      }
    }
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_file(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 1)) {
    if (state.debug_info) {
      p = HEAD(parms);

      if (p->tag == PTAG_STRING) {
        state.obj.debug_file = coff_add_filesym(p->value.string, 0);
      }
      else {
        gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
      }
    }
    else {
      gperror_warning(GPW_UNKNOWN, "Directive ignored when debug info is disabled.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* Filling constants is handled here. Filling instructions is handled in the parser. */

static gpasmVal
_do_fill(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *h;
  int            number;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (enforce_arity(arity, 2)) {
    h = HEAD(parms);
    number = eval_fill_number(HEAD(TAIL(parms)));

    for (; number > 0; --number) {
      /* we must evaluate each loop, because some symbols change (i.e. $) */
      _emit(maybe_evaluate(h), name);
    }
  }
  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_global(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const char     *p;
  const symbol_t *s;
  variable_t     *var;
  char            buf[BUFSIZ];

  state.lst.line.linetype = LTY_SET4;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    for (; parms != NULL; parms = TAIL(parms)) {
      p = HEAD(parms)->value.symbol;

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
            if ((var->previous_type == GVT_ADDRESS) ||
                (var->previous_type == GVT_GLOBAL)  ||
                (var->previous_type == GVT_STATIC)) {
              /* make the symbol global */
              var->type = GVT_GLOBAL;
            }
            else if (var->previous_type == GVT_EXTERN) {
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
_do_idata(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (IS_PIC12_CORE) {
    gperror_verror(GPE_ILLEGAL_DIR, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".idata", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_DATA;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sect_name, ".idata", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_DATA | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_idata_acs(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (IS_PIC12_CORE) {
    gperror_verror(GPE_ILLEGAL_DIR, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".idata_acs", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_DATA | STYP_ACCESS;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sect_name, ".idata_acs", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_DATA | STYP_ACCESS | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_ident(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      coff_add_identsym(p->value.string);
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_idlocs(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  int           value;
  int           v;
  unsigned int  id_location;
  unsigned int  idreg;
  char          buf[BUFSIZ];
  MemBlock_t   *m;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  id_location = gp_processor_id_location(state.processor);

  if (id_location == 0) {
    gperror_verror(GPE_ILLEGAL_DIR, NULL, name);
    return r;
  }

  if (IS_PIC16E_CORE) {
    if (enforce_arity(arity, 2)) {
      idreg = gp_processor_org_to_byte(state.device.class, maybe_evaluate(HEAD(parms)));
      value = maybe_evaluate(HEAD(TAIL(parms)));
    }
    else {
      gperror_error(GPW_EXPECTED, "18cxxx devices should specify __IDLOC address.");
      return r;
    }
  }
  else {
    if (enforce_arity(arity, 1)) {
      idreg = id_location;
      value = maybe_evaluate(HEAD(parms));
    }
    else {
      return r;
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
        gperror_verror(GPE_RANGE, "Not a valid ID location. Address{%#x} < IDLOCS_MIN{%#x}",
                 idreg, state.processor->idlocs_addrs[0]);
      }
      else if (idreg > state.processor->idlocs_addrs[1]) {
        gperror_verror(GPE_RANGE, "Not a valid ID location. Address{%#x} > IDLOCS_MAX{%#x}",
                 idreg, state.processor->idlocs_addrs[1]);
      }
      else {
        uint8_t curvalue;

        state.lst.line.linetype = LTY_CONFIG;
        state.lst.config_address = idreg;

        if (value & ~0xff) {
          v = value;
          value &= 0xff;
          snprintf(buf, sizeof(buf), "An ID Locations value too large. Last two hex digits used: 0x%X ==> 0x%02X", v, value);
          gperror_message(GPM_IDLOC, buf);
        }

        if (idreg <= state.device.id_location) {
          gperror_verror(GPE_IDLOCS_ORDER, NULL);
        }

        if (b_memory_get(m, idreg, &curvalue, NULL, NULL)) {
          gperror_verror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
        }

        snprintf(buf, sizeof(buf), "IDLOCS_%#x", idreg);
        b_memory_put(m, idreg, value, buf, NULL);
        state.lst.line.was_byte_addr = idreg;
        coff_linenum(1);

        if (state.mode == MODE_RELOCATABLE) {
          state.byte_addr += 1;
        }
      }
    }
    else {
      uint16_t word;

      state.lst.line.linetype = LTY_IDLOCS;

      if (value & ~0xffff) {
        v = value;
        value &= 0xffff;
        gperror_vmessage(GPM_IDLOC, NULL, v, value);
      }

      if (state.device.class->i_memory_get(m, idreg, &word, NULL, NULL)) {
        gperror_verror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
      }

      snprintf(buf, sizeof(buf), "IDLOCS_%#x", idreg);
      state.device.class->i_memory_put(m, idreg,     (value & 0xf000) >> 12, buf, NULL);
      snprintf(buf, sizeof(buf), "IDLOCS_%#x", idreg + 2);
      state.device.class->i_memory_put(m, idreg + 2, (value & 0x0f00) >> 8,  buf, NULL);
      snprintf(buf, sizeof(buf), "IDLOCS_%#x", idreg + 4);
      state.device.class->i_memory_put(m, idreg + 4, (value & 0x00f0) >> 4,  buf, NULL);
      snprintf(buf, sizeof(buf), "IDLOCS_%#x", idreg + 6);
      state.device.class->i_memory_put(m, idreg + 6, (value & 0x000f),       buf, NULL);
      state.lst.line.was_byte_addr = idreg;
      coff_linenum(8);

      if (state.mode == MODE_RELOCATABLE) {
        state.byte_addr += 8;
      }
    }
  }
  state.device.id_location = idreg;

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* Support IDLOCS "abcdef" or IDLOCS 'a', 'b', 'c' syntax for PIC16E devices. */

static gpasmVal
_do_16_idlocs(gpasmVal r, const char *name, int arity, pnode_t *parms)
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
    snprintf(buf, sizeof(buf), "Directive Error: The %s directive is invalid in MPASM(X) mode.", name);
    gperror_error(GPE_UNKNOWN, buf);
    return r;
  }

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (!IS_PIC16E_CORE) {
    gperror_error(GPE_IDLOCS_P16E, NULL);
    return r;
  }

  idreg = (last_idreg == 0) ? gp_processor_id_location(state.processor) : last_idreg;

  if (idreg == 0) {
    snprintf(buf, sizeof(buf), "The IDLOCS registers not exist in the %s MCU.",
             state.processor->names[2]);
    gperror_error(GPE_UNKNOWN, buf);
    return r;
  }

  if ((state.mode == MODE_RELOCATABLE) && (!state.found_idlocs)) {
    coff_new_section(".idlocs", idreg, STYP_ABS | STYP_TEXT);
    state.found_idlocs = true;
  }

  if (state.pass == 2) {
    m = (state.mode == MODE_RELOCATABLE) ? state.obj.section->data : state.c_memory;

    if (idreg < state.processor->idlocs_addrs[0]) {
      gperror_verror(GPE_RANGE, "Not a valid ID location. Address{%#x} < IDLOCS_MIN{%#x}",
               idreg, state.processor->idlocs_addrs[0]);
      return r;
    }
    else if (idreg > state.processor->idlocs_addrs[1]) {
      gperror_verror(GPE_RANGE, "Not a valid ID location. Address{%#x} > IDLOCS_MAX{%#x}",
               idreg, state.processor->idlocs_addrs[1]);
      return r;
    }

    assert(parms->tag == PTAG_LIST);
    p = HEAD(parms);

    state.lst.line.linetype = LTY_CONFIG;
    state.lst.config_address = idreg;
    max_bytes = state.processor->idlocs_addrs[1] - idreg + 1;
    n = 0;

    for (; parms != NULL; parms = TAIL(parms)) {
      p = HEAD(parms);

      switch (p->tag) {
      case PTAG_SYMBOL:
        value = maybe_evaluate(p);
        goto constant;
        break;

      case PTAG_CONSTANT:
        value = p->value.constant;
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
          return r;
        }

        snprintf(buf, sizeof(buf), "IDLOCS_%#x", idreg);
        b_memory_put(m, idreg, value, buf, NULL);
        state.lst.line.was_byte_addr = idreg;

        if (state.mode == MODE_RELOCATABLE) {
          state.byte_addr += 1;
        }

        ++n;
        ++idreg;
        break;

      case PTAG_STRING:
        pc = p->value.string;

        while (*pc != '\0') {
          if (n >= max_bytes) {
            gperror_warning(GPW_UNKNOWN, "The number of IDLOCS registers not more than 8!");
            goto warning;
          }

          pc = convert_escape_chars(pc, &value);

          if (b_memory_get(m, idreg, &curvalue, NULL, NULL)) {
            gperror_verror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
            return r;
          }

          snprintf(buf, sizeof(buf), "IDLOCS_%#x", idreg);
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
        return r;
      } /* switch (p->tag) */
    } /* for (; parms != NULL; parms = TAIL(parms)) */
warning:

    if (n > 0) {
      coff_linenum(n);
    }
  } /* if (state.pass == 2) */

  last_idreg = idreg;

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_if(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  _enter_if();

  /* Only evaluate the conditional if it matters... */
  if (state.astack->upper_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);
      state.astack->enabled             = maybe_evaluate(p);
      state.astack->before_else_enabled = state.astack->enabled;
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_elif(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  char           buf[BUFSIZ];

  if (state.mpasm_compatible) {
    snprintf(buf, sizeof(buf), "Directive Error: The %s directive is invalid in MPASM(X) mode.", name);
    gperror_error(GPE_UNKNOWN, buf);
    return r;
  }

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  _enter_elif();

  /* Only evaluate the conditional elif it matters... */
  if (state.astack->upper_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);
      state.astack->enabled              = maybe_evaluate(p);
      state.astack->before_else_enabled |= state.astack->enabled;
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_ifdef(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  _enter_if();

  /* Only evaluate the conditional ifdef it matters... */
  if (state.astack->upper_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);

      if (p->tag != PTAG_SYMBOL) {
        gperror_error(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if ((sym_get_symbol(state.stDefines, p->value.symbol) != NULL) ||
               (sym_get_symbol(state.stTop, p->value.symbol) != NULL)) {
        state.astack->enabled             = true;
        state.astack->before_else_enabled = state.astack->enabled;
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_elifdef(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  char           buf[BUFSIZ];

  if (state.mpasm_compatible) {
    snprintf(buf, sizeof(buf), "Directive Error: The %s directive is invalid in MPASM(X) mode.", name);
    gperror_error(GPE_UNKNOWN, buf);
    return r;
  }

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  _enter_elif();

  /* Only evaluate the conditional elifdef it matters... */
  if (state.astack->upper_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);

      if (p->tag != PTAG_SYMBOL) {
        gperror_error(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if ((sym_get_symbol(state.stDefines, p->value.symbol) != NULL) ||
               (sym_get_symbol(state.stTop, p->value.symbol) != NULL)) {
        state.astack->enabled              = true;
        state.astack->before_else_enabled |= state.astack->enabled;
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_ifndef(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  _enter_if();

  /* Only evaluate the conditional ifndef it matters... */
  if (state.astack->upper_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);

      if (p->tag != PTAG_SYMBOL) {
        gperror_error(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if ((sym_get_symbol(state.stDefines, p->value.symbol) == NULL) &&
               (sym_get_symbol(state.stTop, p->value.symbol) == NULL)) {
        state.astack->enabled             = true;
        state.astack->before_else_enabled = state.astack->enabled;
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_elifndef(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  char           buf[BUFSIZ];

  if (state.mpasm_compatible) {
    snprintf(buf, sizeof(buf), "Directive Error: The %s directive is invalid in MPASM(X) mode.", name);
    gperror_error(GPE_UNKNOWN, buf);
    return r;
  }

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  _enter_elif();

  /* Only evaluate the conditional elifndef it matters... */
  if (state.astack->upper_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);

      if (p->tag != PTAG_SYMBOL) {
        gperror_error(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if ((sym_get_symbol(state.stDefines, p->value.symbol) == NULL) &&
               (sym_get_symbol(state.stTop, p->value.symbol) == NULL)) {
        state.astack->enabled              = true;
        state.astack->before_else_enabled |= state.astack->enabled;
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_include(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      state.next_state = STATE_INCLUDE;
      state.next_buffer.file = GP_Strdup(p->value.string);
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_line(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 1)) {
    if (state.debug_info) {
      p = HEAD(parms);
      state.obj.debug_line = maybe_evaluate(p);
    }
    else {
      gperror_warning(GPW_UNKNOWN, "Directive ignored when debug info is disabled.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/************************************************************************
 * do_list - parse the LIST directive
 *
 * If the file gpasm is assembling contains a LIST directive, then scan
 * and parse will call do_list and pass the comma delimited list of LIST
 * options in *parms
 *
 * Inputs:
 *   gpasmVal r - not used, but is returned
 *   char *name - not used, but contains the directive name 'list'.
 *   int arity - not used, but should contain '1'
 *   pnode_t *parms - a linked list of the parsed parameters
 *
 *
 *
 ************************************************************************/

static gpasmVal
_do_list(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  state.lst.enabled = true;
  state.lst.line.linetype = LTY_DIR;

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        char *lhs;

        lhs = p->value.binop.p0->value.symbol;

        if (strcasecmp(lhs, "b") == 0) {
          int b;

          b = maybe_evaluate(p->value.binop.p1);

          if (b != 0) {
            state.lst.tabstop = b;
          }
        }
        else if (strcasecmp(lhs, "c") == 0) {
          int c;

          c = maybe_evaluate(p->value.binop.p1);

          if (c > LST_SRC_POS) {
            state.lst.line_width = c;
          }
          else {
            char message[BUFSIZ];

            snprintf(message, sizeof(message),
                     "Argument out of range: Column_width{%i} <= LST_SRC_POS{%i}", c, LST_SRC_POS);
            gperror_error(GPE_RANGE, message);
          }
        }
        else if (strcasecmp(lhs, "f") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            select_hexformat(p->value.binop.p1->value.symbol);
          }
        }
        else if (strcasecmp(lhs, "l") == 0) {
          ; /* Ignore this for now: page length */
        }
        else if (strcasecmp(lhs, "m") == 0) {
          /* Undocumented, only found in MEMORY.INC and MCP250XX.INC. */
          if (can_evaluate(p->value.binop.p1)) {
            int value = evaluate(p->value.binop.p1);

            if (value < state.maxrom) {
              char message[BUFSIZ];

              snprintf(message, sizeof(message),
                       "Argument out of range, must be greater than or equal to %ld.",
                       state.maxrom);
              gperror_error(GPE_RANGE, message);
            }
            else {
              state.maxrom = value;
            }
          }
        }
        else if (strcasecmp(lhs, "mm") == 0) {
          state.lst.memorymap = _off_or_on(p->value.binop.p1);
        }
        else if (strcasecmp(lhs, "n") == 0) {
          if (can_evaluate(p->value.binop.p1)) {
            int number = evaluate(p->value.binop.p1);

            if ((number > 6) || (number == 0)) {
              state.lst.linesperpage = number;
            }
            else {
              gperror_verror(GPE_RANGE, "0 < n{%i} <= 6", number);
            }
          }
        }
        else if (strcasecmp(lhs, "p") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            select_processor(p->value.binop.p1->value.symbol);
          }
        }
        else if (strcasecmp(lhs, "pe") == 0) {
          state.extended_pic16e = true;

          if (enforce_simple(p->value.binop.p1)) {
            select_processor(p->value.binop.p1->value.symbol);
          }
        }
        else if (strcasecmp(lhs, "r") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            select_radix(p->value.binop.p1->value.symbol);
          }
        }
        else if (strcasecmp(lhs, "st") == 0) {
          state.lst.symboltable = _off_or_on(p->value.binop.p1);
        }
        else if (strcasecmp(lhs, "t") == 0) {
          ; /* Ignore this for now: always wrap long list lines */
        }
        else if (strcasecmp(lhs, "w") == 0) {
          select_errorlevel(maybe_evaluate(p->value.binop.p1));
        }
        else if (strcasecmp(lhs, "x") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            select_expand(p->value.binop.p1->value.symbol);
          }
        }
        else {
          gperror_verror(GPE_ILLEGAL_ARGU, NULL, lhs);
        }
      }
    }
    else {
      if (enforce_simple(p)) {
        if (strcasecmp(p->value.symbol, "free") == 0) {
          ; /* Ignore this directive */
        }
        else if (strcasecmp(p->value.symbol, "fixed") == 0) {
          ; /* Ignore this directive */
        }
        else if (strcasecmp(p->value.symbol, "nowrap") == 0) {
          ; /* Ignore this directive */
        }
        else if (strcasecmp(p->value.symbol, "wrap") == 0) {
          ; /* Ignore this directive */
        }
        else {
          gperror_verror(GPE_ILLEGAL_ARGU, NULL, p->value.symbol);
        }
      }
    }
  }

  /* The .list symbol is only added to the COFF file if its only action is to turn on
     the listing */
  if (arity == 0) {
    coff_add_listsym();
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_local(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  gp_boolean     first;

  if (_check_processor_select(name)) {
    return r;
  }

  first = true;
  state.lst.line.linetype = LTY_SET4;

  /* like variable except it is put in TOP instead of GLOBAL */

  if (state.stGlobal == state.stTop) {
    gperror_error(GPE_UNKNOWN, "Attempt to use \"local\" outside of macro.");
  }
  else {
    for (; parms != NULL; parms = TAIL(parms)) {
      p = HEAD(parms);

      if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '=')) {
        if (enforce_simple(p->value.binop.p0)) {
          char *lhs;
          gpasmVal val;

          /* fetch the symbol */
          lhs = p->value.binop.p0->value.symbol;
          val = maybe_evaluate(p->value.binop.p1);
          /* put the symbol and value in the TOP table*/
          sym_add_symbol(state.stTop, lhs);
          set_global(lhs, val, LFT_TEMPORARY, GVT_CONSTANT, false);

          if (first) {
            r = val;
            first = false;
          }
        }
      }
      else if (p->tag == PTAG_SYMBOL) {
        /* put the symbol in the Top table */
        sym_add_symbol(state.stTop, p->value.symbol);

        if (first) {
          r = 0;
          first = false;
        }
      }
      else {
        gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_noexpand(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (state.cmd_line.macro_expand) {
    gperror_vmessage(GPM_SUPLIN, NULL);
  }
  else {
    if (enforce_arity(arity, 0)) {
      state.lst.expand = false;
    }
  }
  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_nolist(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (!state.lst.force) {
    state.lst.enabled = false;
  }

  coff_add_nolistsym();

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_maxram(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_SET;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (can_evaluate(p)) {
      r = state.maxram = evaluate(p);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_maxrom(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_SET;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (can_evaluate(p)) {
      r = state.maxrom = evaluate(p);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_macro(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  macro_head_t *head;

  if (_check_processor_select(name)) {
    return r;
  }

  head = GP_Malloc(sizeof(*head));

  head->parms = parms;
  head->body = NULL;
  head->defined = false;
  /* Record data for the list, cod, and coff files. */
  head->line_number = state.src->line_number;
  head->file_symbol = state.src->file_symbol;

  head->src_name = GP_Strdup(state.src->name);

  state.lst.line.linetype = LTY_DIR;

  if (_macro_parms_ok(parms)) {
    state.mac_head = head;
  }

  state.mac_prev = &(head->body);
  state.mac_body = NULL;

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_messg(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  const char    *str;

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      if ((str = _hv_macro_resolver(p->value.string)) != NULL) {
        gperror_vmessage(GPM_USER, NULL, str);
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_org(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (can_evaluate(p)) {
      gpasmVal new_byte_addr;

      r = evaluate(p);
      new_byte_addr = gp_processor_real_to_byte(state.processor, r);
      if (state.mpasm_compatible ||
          ((gp_processor_is_config_org(state.processor, r) < 0) &&
           (gp_processor_is_eeprom_org(state.processor, r) < 0))) {
        if (IS_PIC16E_CORE && (r & 1)) {
          gperror_verror(GPE_ORG_ODD, "Address{%#x}", r);
        }
      }

      if (state.mode == MODE_ABSOLUTE) {
        state.lst.line.linetype = LTY_ORG;
        state.byte_addr = new_byte_addr;
      }
      else {
        /* Default section name, this will be overwritten if a label is present. */
        snprintf(state.obj.new_sect_name, sizeof(state.obj.new_sect_name), ".org_%x", r);
        state.obj.new_sect_addr  = new_byte_addr;
        state.obj.new_sect_flags = STYP_TEXT | STYP_ABS;
        state.lst.line.linetype  = LTY_SEC;
        state.next_state = STATE_SECTION;
      }
    }
    else {
      r = 0;
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_page(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 0) && state.lst.enabled) {
    lst_page_start();
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* Called by both do_pagesel and do_pageselw, which have a very slight
 * difference between them. */

static gpasmVal
_do_pagesel(gpasmVal r, const char *name, int arity, pnode_t *parms, uint16_t reloc_type)
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
    return r;
  }

  if (IS_PIC16_CORE) {
    gperror_vmessage(GPM_W_MODIFIED, NULL);
  }

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (state.mode == MODE_ABSOLUTE) {
      page = gp_processor_check_page(state.device.class, maybe_evaluate(p));
      state.byte_addr += gp_processor_set_page(state.device.class,
                                               state.processor->num_pages,
                                               page,
                                               state.i_memory,
                                               state.byte_addr,
                                               use_wreg);
      if (!state.mpasm_compatible) {
        set_global(GLOBAL_ACT_PAGE_ADDR,
                   gp_processor_page_bits_to_addr(state.device.class, page),
                   LFT_TEMPORARY, GVT_CONSTANT, true);
      }
    }
    else {
      num_reloc = count_reloc(p);

      if (num_reloc == 0) {
        /* it is an absolute address, generate the pagesel but no relocation */
        page = gp_processor_check_page(state.device.class, maybe_evaluate(p));
        state.byte_addr += gp_processor_set_page(state.device.class, state.processor->num_pages,
                                                 page, state.i_memory, state.byte_addr, use_wreg);
      }
      else if (num_reloc != 1) {
        gperror_error(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if (IS_PIC16_CORE) {
        reloc_evaluate(p, RELOCT_PAGESEL_WREG);
        _emit(0x0000, name);
        _emit(0x0000, name);
      }
      else if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
        if (!use_wreg) {
          reloc_evaluate(p, RELOCT_PAGESEL_MOVLP);
          _emit(PIC14E_INSN_MOVLP, name);
        }
        else {
          reloc_evaluate(p, RELOCT_PAGESEL_WREG);
          _emit(0x0000, name);
          _emit(0x0000, name);
        }
      }
      else {
        if (!use_wreg && (state.processor->num_pages == 2)) {
          reloc_evaluate(p, RELOCT_PAGESEL_BITS);
          _emit(0, name);
        }
        else if (state.processor->num_pages >= 2) {
          reloc_evaluate(p, reloc_type);
          _emit(0x0000, name);
          _emit(0x0000, name);
        }
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_pagesel_wrapper(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (prev_btfsx) {
    gperror_vwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Pagesel");
  }

  return _do_pagesel(r, name, arity, parms, RELOCT_PAGESEL_BITS);
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_pageselw_wrapper(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (prev_btfsx) {
    gperror_vwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Pageselw");
  }

  return _do_pagesel(r, name, arity, parms, RELOCT_PAGESEL_WREG);
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_processor(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 1)) {
    const pnode_t *p = HEAD(parms);

    if (enforce_simple(p)) {
      select_processor(p->value.symbol);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_radix(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (enforce_simple(p)) {
      select_radix(p->value.symbol);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_res(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  int            count;
  int            i;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (can_evaluate(p)) {
      count = evaluate(p);

      state.lst.line.linetype = LTY_RES;

      if (state.mode == MODE_ABSOLUTE) {
        if (IS_PIC16E_CORE && ((count % 2) != 0)) {
          gperror_verror(GPE_RES_ODD_PIC16EA, "res = %i", count);
        }

        count = gp_processor_org_to_byte(state.device.class, count);

        for (i = 0; (i + 1) < count; i += 2) {
          _emit(state.device.class->core_mask, name);
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
            _emit(state.device.class->core_mask, name);
          }
        }
        else {
          for (i = 0; i < count; i++) {
            _emit_byte(0, name);
          }
        }
      }
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_set(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_SET;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (can_evaluate(p)) {
      r = evaluate(p);
    }
    else {
      r = 0;
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_space(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_DIR;

  if (state.pass == 2) {
    switch (arity) {
      case 0:
        /* do nothing */
        break;

      case 1:
        p = HEAD(parms);

        if (can_evaluate(p)) {
          int i;

          for (i = evaluate(p); i > 0; i--) {
            lst_line("");
          }
        }
        break;

      default:
        enforce_arity(arity, 1);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_subtitle(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      gp_strncpy(state.lst.subtitle_name, p->value.string, sizeof(state.lst.subtitle_name));
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_title(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  state.lst.line.linetype = LTY_NONE;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      gp_strncpy(state.lst.title_name, p->value.string, sizeof(state.lst.title_name));
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_type(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  const char    *symbol_name;
  gp_symbol_t   *coff_symbol;
  int            value;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 2)) {
    /* the first argument is the symbol name */
    p = HEAD(parms);

    if (enforce_simple(p)) {
      symbol_name = p->value.symbol;
      coff_symbol = gp_coffgen_find_symbol(state.obj.object, symbol_name);

      if (coff_symbol == NULL) {
        gperror_verror(GPE_NOSYM, NULL, symbol_name);
      }
      else {
        p = HEAD(TAIL(parms));
        value = maybe_evaluate(p);

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

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_udata(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".udata", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_BSS;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sect_name, ".udata", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_BSS | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_udata_acs(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".udata_acs", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_BSS | STYP_ACCESS;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sect_name, ".udata_acs", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_BSS | STYP_ACCESS | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_udata_ovr(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".udata_ovr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_BSS | STYP_OVERLAY;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sect_name, ".udata_ovr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_BSS | STYP_OVERLAY | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_udata_shr(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (state.processor == NULL) {
    gperror_verror(GPE_UNDEF_PROC, "\"%s\"", name);
    return r;
  }

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gperror_verror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sect_name, ".udata_shr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = 0;
      state.obj.new_sect_flags = STYP_BSS | STYP_SHARED;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sect_name, ".udata_shr", sizeof(state.obj.new_sect_name));
      state.obj.new_sect_addr  = maybe_evaluate(p);
      state.obj.new_sect_flags = STYP_BSS | STYP_SHARED | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_undefine(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;

  if (_check_processor_select(name)) {
    return r;
  }

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_SYMBOL) {
      if (!sym_remove_symbol(state.stDefines, p->value.symbol)) {
        gperror_vwarning(GPW_NOT_DEFINED, NULL, p->value.symbol);
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_variable(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  const pnode_t *p;
  gp_boolean     first;
  char          *lhs;
  gpasmVal       val;

  if (_check_processor_select(name)) {
    return r;
  }

  first = true;
  state.lst.line.linetype = LTY_SET4;

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        /* fetch the symbol */
        lhs = p->value.binop.p0->value.symbol;
        val = maybe_evaluate(p->value.binop.p1);
        /* put the symbol and value in the table */
        set_global(lhs, val, LFT_TEMPORARY, GVT_CONSTANT, false);

        if (first) {
          r = val;
          first = false;
        }
      }
    }
    else if (p->tag == PTAG_SYMBOL) {
      /* put the symbol with a 0 value in the table */
      set_global(p->value.symbol, 0, LFT_TEMPORARY, GVT_CONSTANT, false);

      if (first) {
        r = 0;
        first = false;
      }
    }
    else {
      gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

static gpasmVal
_do_while(gpasmVal r, const char *name, int arity, pnode_t *parms)
{
  macro_head_t *head;

  if (_check_processor_select(name)) {
    return r;
  }

  assert(state.while_depth == 0);

  head = GP_Malloc(sizeof(*head));
  state.lst.line.linetype = LTY_DOLIST_DIR;
  head->parms = (enforce_arity(arity, 1)) ? HEAD(parms) : NULL;
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

  return r;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
asm_enabled(void)
{
  return ((state.astack == NULL) || (state.astack->enabled && state.astack->upper_enabled));
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_core_sfr_or_common_ram(int file)
{
  if (gp_processor_find_sfr(state.device.class, file) != NULL) {
    /* This is a core SFR. */
    return true;
  }

  if (gp_processor_is_common_ram_addr(state.processor, file) >= 0) {
    /* This is a common GPR. */
    return true;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

/* Check that a register file address is ok. */

void
file_ok(unsigned int file)
{
  int               bank_addr;
  int               bank_num;
  int               reg_offs;
  int               bank_assume;
  const variable_t *var;

  /* Don't check address, the linker takes care of it. */
  if (state.mode == MODE_RELOCATABLE) {
    return;
  }

  if (file > state.maxram) {
    gperror_vwarning(GPW_INVALID_RAM, "Register{%#x} > MAXRAM{%#x}", file, state.maxram);
  }
  else if (state.badram[file]) {
    gperror_vwarning(GPW_INVALID_RAM, "Address{%#x} in BADRAM.", file);
  }

  bank_addr = gp_processor_bank_addr(state.processor, file);
  bank_num  = gp_processor_bank_num(state.processor, file);
  reg_offs  = gp_processor_reg_offs(state.processor, file);

  if (state.mpasm_compatible || IS_PIC16_CORE) {
    /* Only so much can be done compatibility reasons. */
    if (bank_addr > 0) {
      gperror_vmessage(GPM_BANK, "Bank_bits = %#x, register{%#x}.", 0, bank_addr, reg_offs);
    }

    return;
  }

  /* Don't check bank if common ram or core SFR is addressed. */
  if (_core_sfr_or_common_ram(file)) {
    return;
  }

  /* The __ACTIVE_BANK_ADDR variable shows the register which used for BANKSEL directive last time. */
  if ((var = get_global_constant(GLOBAL_ACT_BANK_ADDR)) == NULL) {
    gperror_verror(GPE_INTERNAL, NULL, "The \"" GLOBAL_ACT_BANK_ADDR "\" variable not exists.");
    return;
  }

  bank_assume = var->value;

  /* Check if we are in correct bank. Negative __ACTIVE_BANK_ADDR value means bank is not set yet. */
  if (bank_assume >= 0) {
    /* Necessary only the selected bank address. */
    bank_assume = gp_processor_bank_addr(state.processor, bank_assume);
    bank_num    = gp_processor_bank_num(state.processor, bank_assume);
    if (bank_assume != bank_addr) {
      gperror_vmessage(GPM_BANK, "Bank_bits = %#x, register{%#x}.", bank_num, bank_addr, reg_offs);
    }
  }
  else {
    if (gp_processor_num_banks(state.processor) > 1) {
      gperror_vmessage(GPM_NOB, NULL, bank_num);
    }

    /* If no bank is explicitly selected, set bank to this register now. */
    set_global(GLOBAL_ACT_BANK_ADDR, file, LFT_TEMPORARY, GVT_CONSTANT, true);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_emit_check(unsigned int insn, int argument, int mask, const char *name)
{
  int test = argument;
  int v = argument & mask;

  if (test < 0) {
    test = -test;
  }

  /* If there are bits that shouldn't be set then issue a warning. */
  if (test & (~mask)) {
    gperror_vwarning(GPW_RANGE, "%i (%#x) => %i (%#x)", argument, argument, v, v);
  }

  _emit(insn | v, name);
}

/*------------------------------------------------------------------------------------------------*/

/*
   For relative branches, issue a warning if the absolute value of
   argument is greater than range.
*/

static void
_emit_check_relative(unsigned int insn, int argument, int mask, int range, const char *name)
{
  char full_message[BUFSIZ];

  /* If the branch is too far then issue an error */
  if ((argument > range) || (argument < -(range+1))) {
    snprintf(full_message, sizeof(full_message),
             "Argument out of range (%d not between %d and %d).",
             argument, -(range+1), range);
    gperror_error(GPE_RANGE, full_message);
  }

  _emit(insn | (argument & mask), name);
}

/*------------------------------------------------------------------------------------------------*/

static int
_check_flag(int flag)
{
  if ((flag != 0) && (flag != 1)) {
    gperror_vwarning(GPW_RANGE, "%i (%#x)", flag, flag);
  }

  return (flag & 0x1);
}

/*------------------------------------------------------------------------------------------------*/

/* values and masks for types_mask */
#define AR_BITS          1
#define AR_MASK          ((1 << AR_BITS) - 1)
#define AR_GET(types, i) ((types >> (AR_BITS * (i))) && AR_MASK)
#define AR_DIRECT        0 /* direct addressing */
#define AR_INDEX         1 /* indexed addressing with literal offset */
#define AR_BIT_BYTE      0x8000 /* Byte-Oriented and Bit-Oriented
                                 * Instructions in Indexed Literal
                                 * Offset Mode */

static gp_boolean
_check_16e_arg_types(const pnode_t *parms, int arity, unsigned int types)
{
  const pnode_t *p;
  int            i;
  int            val;
  gp_boolean     ret = true;
  char           buf[80];

  if (state.extended_pic16e) {
    p = parms;
    for (i = 0; i < arity; ++i) {
      assert(p != NULL);

      if ((AR_GET(types, i) != AR_INDEX) && (HEAD(p)->tag == PTAG_OFFSET)) {
        gperror_error(GPE_BADCHAR, "Illegal character ([).");
        ret = false;
      }
      else if (AR_BIT_BYTE & types) {
        if ((AR_GET(types, i) == AR_INDEX) && (HEAD(p)->tag == PTAG_OFFSET)) {
          const pnode_t *p1 = HEAD(p)->value.offset;

          if (can_evaluate_value(p1)) {
            val = evaluate(p1);

            if (val < 0 || val > 0x5f) {
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
      else if ((AR_GET(types, i) == AR_INDEX) && (HEAD(p)->tag != PTAG_OFFSET)) {
        gperror_verror(GPE_MISSING_BRACKET, NULL);
        ret = false;
      }
      p = TAIL(p);
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

gpasmVal
do_insn(const char *Op_name, pnode_t *Parameters)
{
  pnode_t          *p;
  const symbol_t   *sym;
  int               arity;
  int               file;            /* register file address, if applicable */
  unsigned int      bank_num;
  gpasmVal          r;               /* Return value. */
  gp_boolean        is_btfsx = false;
  const insn_t     *ins;
  enum common_insn  icode;
  const char       *sym_name;

  /* We want to have r as the value to assign to label. */
  r = IS_RAM_ORG ? state.byte_addr : gp_processor_byte_to_org(state.device.class, state.byte_addr);

  arity = list_length(Parameters);

  sym = sym_get_symbol(state.stBuiltin, Op_name);

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

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);
          _emit_check(ins->opcode, (reloc_evaluate(p, RELOCT_F) >> 5), SX_BMSK_BANK, sym_name);
        }
        break;

      case INSN_CLASS_LIT3_PAGE:
        /* SX page */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);
          _emit_check(ins->opcode, (reloc_evaluate(p, RELOCT_F) >> PIC12_SHIFT_PAGE_ADDR), SX_BMSK_PAGE, sym_name);
        }
        break;

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
            flag = _check_flag(reloc_evaluate(HEAD(Parameters), RELOCT_F));
          case 0:
            _emit(ins->opcode | flag, sym_name);
            break;

          default:
            enforce_arity(arity, 1);
          }
        }
        break;

      case INSN_CLASS_LIT3:
        /* PIC12E, PIC12I movlb */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);
          r = reloc_evaluate(p, RELOCT_MOVLB);
          _emit_check(ins->opcode, r, PIC12E_BMSK_BANK, sym_name);

          if ((!state.mpasm_compatible) && (state.mode == MODE_ABSOLUTE)) {
            /* Set the selection of RAM Banks. */
            bank_num = r & (PIC12E_MASK_MOVLB ^ PIC12_CORE_MASK);
            set_global(GLOBAL_ACT_BANK_ADDR,
                       gp_processor_bank_num_to_addr(state.processor, bank_num),
                       LFT_TEMPORARY, GVT_CONSTANT, true);
          }
        }
        break;

      case INSN_CLASS_LIT4:
        /* SX mode */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);
          _emit_check(ins->opcode, reloc_evaluate(p, RELOCT_F), SX_BMSK_MODE, sym_name);
        }
        break;

      case INSN_CLASS_LIT4L:
	/* PIC16E movlb */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          _check_16e_arg_types(Parameters, arity, 0);
          p = HEAD(Parameters);
          coerce_str1(p); /* literal instructions can coerce string literals */
          r = reloc_evaluate(p, RELOCT_MOVLB);
          _emit_check(ins->opcode, r, PIC16E_BMSK_MOVLB, sym_name);

          if ((!state.mpasm_compatible) && (state.mode == MODE_ABSOLUTE)) {
            /* Set the selection of RAM Banks. */
            bank_num = r & (PIC16E_MASK_MOVLB ^ PIC16_CORE_MASK);
            set_global(GLOBAL_ACT_BANK_ADDR,
                       gp_processor_bank_num_to_addr(state.processor, bank_num),
                       LFT_TEMPORARY, GVT_CONSTANT, true);
          }
        }
        break;

      case INSN_CLASS_LIT4H:
        /* PIC16 movlr */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);
          _emit_check(ins->opcode, (reloc_evaluate(p, RELOCT_MOVLR) << 4), PIC16_BMSK_MOVLR, sym_name);
        }
        break;

      case INSN_CLASS_LIT5:
        /* PIC14E movlb */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);
          r = reloc_evaluate(p, RELOCT_MOVLB);
          _emit_check(ins->opcode, r, PIC14E_BMSK_MOVLB, sym_name);

          if ((!state.mpasm_compatible) && (state.mode == MODE_ABSOLUTE)) {
            /* Set the selection of RAM Banks. */
            bank_num = r & PIC14E_BMSK_MOVLB;
            set_global(GLOBAL_ACT_BANK_ADDR,
                       gp_processor_bank_num_to_addr(state.processor, bank_num),
                       LFT_TEMPORARY, GVT_CONSTANT, true);
          }
        }
        break;

      case INSN_CLASS_LITBSR_6:
        /* PIC14EX movlb */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);
          r = reloc_evaluate(p, RELOCT_MOVLB);
          _emit_check(ins->opcode, r, PIC14EX_BMSK_MOVLB, sym_name);

          if ((!state.mpasm_compatible) && (state.mode == MODE_ABSOLUTE)) {
            /* Set the selection of RAM Banks. */
            bank_num = r & PIC14EX_BMSK_MOVLB;
            set_global(GLOBAL_ACT_BANK_ADDR,
                       gp_processor_bank_num_to_addr(state.processor, bank_num),
                       LFT_TEMPORARY, GVT_CONSTANT, true);
          }
        }
        break;

      case INSN_CLASS_LIT6:
        /* PIC16E (addulnk, subulnk) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          _check_16e_arg_types(Parameters, arity, 0);
          p = HEAD(Parameters);
          /* The literal cannot be a relocatable address. */
          _emit_check(ins->opcode, maybe_evaluate(p), PIC16EX_BMSK_xxxULNK, sym_name);
        }
        break;

      case INSN_CLASS_LIT7:
        /* PIC14E movlp */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          int page;

          p = HEAD(Parameters);
          page = reloc_evaluate(p, RELOCT_PAGESEL_MOVLP);
          _emit_check(ins->opcode, page, PIC14E_BMSK_PAGE512, sym_name);

          if ((!state.mpasm_compatible) && (state.mode == MODE_ABSOLUTE)) {
            set_global(GLOBAL_ACT_PAGE_ADDR,
                       gp_processor_page_bits_to_addr(state.device.class, page),
                       LFT_TEMPORARY, GVT_CONSTANT, true);
          }
        }
        break;

      case INSN_CLASS_LIT8:
        /* PIC1xx (addlw, andlw, iorlw, movlw, retlw, sublw, xorlw),
	   PIC16  movlb,
	   PIC16x mullw,
	   PIC16E pushl */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          _check_16e_arg_types(Parameters, arity, 0);

          p = HEAD(Parameters);
          coerce_str1(p); /* literal instructions can coerce string literals */

          if (icode == ICODE_MOVLB) {
            _emit_check(ins->opcode, reloc_evaluate(p, RELOCT_MOVLB), PIC16_BMSK_MOVLB, sym_name);
          }
          else {
            _emit_check(ins->opcode, reloc_evaluate(p, RELOCT_LOW), 0xff, sym_name);
          }
        }
        break;

      case INSN_CLASS_LIT8C12:
        /* PIC12x call, SX call */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);

          if ((state.obj.new_sect_flags & STYP_ABS) && can_evaluate_value(p)) {
            int value = evaluate(p);

            /* PC is 11 bits.  mpasm checks the maximum device address. */
            if (value & (~PIC12_PC_MASK)) {
              gperror_verror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC12_PC_MASK);
            }

            if ((value & PIC12_PAGE_BITS) != (r & PIC12_PAGE_BITS)) {
              gperror_vmessage(GPM_PAGE, "%#x != %#x", (value & PIC12_PAGE_BITS), (r & PIC12_PAGE_BITS));
            }

            if (value & 0x100) {
              gperror_verror(GPE_BAD_CALL_ADDR, "%ins (%#x)", value, value);
            }
          }

          _emit(ins->opcode | (reloc_evaluate(p, RELOCT_CALL) & PIC12_BMSK_CALL), sym_name);
        }
        break;

      case INSN_CLASS_LIT8C16:
        /* PIC16 lcall */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          int value;

          p = HEAD(Parameters);
          value = reloc_evaluate(p, RELOCT_LOW);

          /* PC is 16 bits.  mpasm checks the maximum device address. */
          if (value & (~0xffff)) {
            gperror_verror(GPE_RANGE, "Address{%#x} > 0xffff.", value);
          }

          _emit(ins->opcode | (value & 0xff), sym_name);
        }
        break;

      case INSN_CLASS_LIT9:
        /* PIC12 goto, SX goto */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);

          if ((state.obj.new_sect_flags & STYP_ABS) && can_evaluate_value(p)) {
            int value = evaluate(p);

            /* PC is 11 bits.  mpasm checks the maximum device address. */
            if (value & (~PIC12_PC_MASK)) {
              gperror_verror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC12_PC_MASK);
            }

            if ((value & PIC12_PAGE_BITS) != (r & PIC12_PAGE_BITS)) {
              gperror_vmessage(GPM_PAGE, "%#x != %#x", (value & PIC12_PAGE_BITS), (r & PIC12_PAGE_BITS));
            }
          }

          _emit(ins->opcode | (reloc_evaluate(p, RELOCT_GOTO) & PIC12_BMSK_GOTO), sym_name);
        }
        break;

      case INSN_CLASS_LIT11:
        /* PIC14x (call, goto) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);

          if ((state.obj.new_sect_flags & STYP_ABS) && can_evaluate_value(p)) {
            int value = evaluate(p);

            if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
              /* PC is 15 bits.  mpasm checks the maximum device address. */
              if (value & (~PIC14E_PC_MASK)) {
                gperror_verror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC14E_PC_MASK);
              }

              if ((value & PIC14E_PAGE_BITS) != (r & PIC14E_PAGE_BITS)) {
                gperror_vmessage(GPM_PAGE, "%#x != %#x", (value & PIC14E_PAGE_BITS), (r & PIC14E_PAGE_BITS));
              }
            }
            else {
              /* PC is 13 bits.  mpasm checks the maximum device address. */
              if (value & (~PIC14_PC_MASK)) {
                gperror_verror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC14_PC_MASK);
              }

              if ((value & PIC14_PAGE_BITS) != (r & PIC14_PAGE_BITS)) {
                gperror_vmessage(GPM_PAGE, "%#x != %#x", (value & PIC14_PAGE_BITS), (r & PIC14_PAGE_BITS));
              }
            }
          }

          _emit(ins->opcode |
	                   (reloc_evaluate(p, (icode == ICODE_CALL) ? RELOCT_CALL : RELOCT_GOTO) &
	                       PIC14_BMSK_BRANCH),
	       sym_name);
        }
        break;

      case INSN_CLASS_LIT13:
        /* PIC16 (call, goto) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);

          if ((state.obj.new_sect_flags & STYP_ABS) && can_evaluate_value(p)) {
            int value = evaluate(p);

            /* PC is 16 bits.  mpasm checks the maximum device address. */
            if (value & (~PIC16_PC_MASK)) {
              gperror_verror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC16_PC_MASK);
            }

            if ((value & PIC16_PAGE_BITS) != (r & PIC16_PAGE_BITS)) {
              gperror_vmessage(GPM_PAGE, "%#x != %#x", (value & PIC16_PAGE_BITS), (r & PIC16_PAGE_BITS));
            }
          }

          _emit(ins->opcode |
	                   (reloc_evaluate(p, (icode == ICODE_CALL) ? RELOCT_CALL : RELOCT_GOTO) &
		               PIC16_BMSK_BRANCH),
	       sym_name);
        }
        break;

      case INSN_CLASS_LITFSR_14:
        /* PIC14E addfsr */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 2)) {
          int value;
          int fsr;

          p = HEAD(Parameters);
          fsr = maybe_evaluate(p);

          if ((fsr == PIC14E_REG_FSR0) || (fsr == PIC14E_REG_FSR1)) {
            fsr = (fsr == PIC14E_REG_FSR1) ? 0x40 : 0x00;
            p = HEAD(TAIL(Parameters));
            /* the offset cannot be a relocatable address */
            value = maybe_evaluate(p);

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

      case INSN_CLASS_LITFSR_16:
        /* PIC16E (addfsr, subfsr) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 2)) {
          int value;
          int fsr;

          _check_16e_arg_types(Parameters, arity, 0);

          p = HEAD(Parameters);
          fsr = maybe_evaluate(p);

          if (fsr < 0) {
            gperror_verror(GPE_RANGE, "FSR{%ins} < 0.", fsr);
          }
          else if (fsr > 2) {
            gperror_verror(GPE_RANGE, "FSR{%ins} > 2.", fsr);
          }

          p = HEAD(TAIL(Parameters));
          /* the offset cannot be a relocatable address */
          value = maybe_evaluate(p);

          if (value & (~0x3f)) {
            gperror_verror(GPE_RANGE, "%ins (%#x) > 0x3f", value, value);
          }

          _emit(ins->opcode | ((fsr & 0x3) << 6) | (value & 0x3f), sym_name);
        }
        break;

      case INSN_CLASS_RBRA8:
        /* PIC16E (bc, bn, bnc, bnn, bnov, bnz, bov, bz) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          int offset;

          _check_16e_arg_types(Parameters, arity, 0);

          p = HEAD(Parameters);

          if (count_reloc(p) == 0) {
            offset = maybe_evaluate(p) - (r + 2);

            if (offset & 1) {
              gperror_vwarning(GPW_WORD_ALIGNED, "Offset{%ins (%#x)}", offset, offset);
            }
          }
          else {
            offset = reloc_evaluate(p, RELOCT_CONDBRA);
          }

          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -127 <= offset <= 127. */
          _emit_check_relative(ins->opcode, offset, PIC16E_BMSK_RBRA8, 127, sym_name);
        }
        break;

      case INSN_CLASS_RBRA9:
        /* PIC14E bra */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          int offset;

          p = HEAD(Parameters);

          if (count_reloc(p) == 0) {
            offset = maybe_evaluate(p) - (r + 1);
          }
          else {
            offset = reloc_evaluate(p, RELOCT_BRA);
          }

          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -256 <= offset <= 255. */
          _emit_check_relative(ins->opcode, offset, PIC14E_BMSK_RBRA9, 255, sym_name);
        }
        break;

      case INSN_CLASS_RBRA11:
        /* PIC16E (bra, rcall) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          int offset;

          _check_16e_arg_types(Parameters, arity, 0);

          p = HEAD(Parameters);

          if (count_reloc(p) == 0) {
            offset = maybe_evaluate(p) - (r + 2);

            if (offset & 1) {
              gperror_vwarning(GPW_WORD_ALIGNED, "%ins (%#x)", offset, offset);
            }
          }
          else {
            offset = reloc_evaluate(p, RELOCT_BRA);
          }

          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -1024 <= offset <= 1023. */
          _emit_check_relative(ins->opcode, offset, PIC16E_BMSK_RBRA11, 1023, sym_name);
        }
        break;

      case INSN_CLASS_LIT20:
        /* PIC16E goto */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          int dest;

          _check_16e_arg_types(Parameters, arity, 0);

          p = HEAD(Parameters);
          dest = reloc_evaluate(p, RELOCT_GOTO);
          dest = gp_processor_org_to_byte(state.device.class, dest) >> 1;
          _emit(ins->opcode | (dest & PIC16E_BMSK_BRANCH_LOWER), sym_name);
          reloc_evaluate(p, RELOCT_GOTO2);     /* add the second relocation */
          _emit_check(0xf000, dest >> 8, PIC16E_BMSK_BRANCH_HIGHER, sym_name);
        }
        break;

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
            enforce_arity(arity, 2);
          }
          else {
            _check_16e_arg_types(Parameters, arity, 0);

            p = HEAD(Parameters);

            switch (arity) {
            case 2:
              p2 = HEAD(TAIL(Parameters));
              /* Allow "s" for fast push */
              if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "s") == 0)) {
                flag = 1;
              }
              else {
                flag = _check_flag(maybe_evaluate(p2));
              }
              break;

            case 1:
              flag = 0;
              break;

            default:
              enforce_arity(arity, 2);
            }

            dest = reloc_evaluate(p, RELOCT_CALL);
            dest = gp_processor_org_to_byte(state.device.class, dest) >> 1;
            _emit(ins->opcode | (flag << 8) | (dest & PIC16E_BMSK_BRANCH_LOWER), sym_name);
            reloc_evaluate(p, RELOCT_CALL2);     /* add the second relocation */
            _emit_check(0xf000, (dest >> 8), PIC16E_BMSK_BRANCH_HIGHER, sym_name);
          }
        }
        break;

      case INSN_CLASS_FLIT12:
        /* PIC16E lfsr */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 2)) {
          int k;

          _check_16e_arg_types(Parameters, arity, 0);

          p = HEAD(Parameters);
          file = maybe_evaluate(p);

          if (file > 3) {
            gperror_error(GPE_UNKNOWN, "FSR is out of range.");
          }

          p = HEAD(TAIL(Parameters));
          k = reloc_evaluate(p, RELOCT_LFSR1);
          _emit_check(ins->opcode | ((file & 3) << 4), (k >> 8), 0xf, sym_name);
          reloc_evaluate(p, RELOCT_LFSR2); /* add the second relocation */
          _emit(0xf000 | (k & 0xff), sym_name);
        }
        break;

      case INSN_CLASS_FF:
        /* PIC16E movff */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 2)) {
          int dest;

          _check_16e_arg_types(Parameters, arity, 0);

          dest = maybe_evaluate(HEAD(TAIL(Parameters)));

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

          _emit_check(ins->opcode, reloc_evaluate(HEAD(Parameters), RELOCT_FF1), 0xfff, sym_name);
          _emit_check(0xf000, reloc_evaluate(HEAD(TAIL(Parameters)), RELOCT_FF2), 0xfff, sym_name);
        }
        break;

      case INSN_CLASS_FP:
        /* PIC16 movfp */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 2)) {
          int reg = 0;

          file = reloc_evaluate(HEAD(Parameters), RELOCT_F);
          reg  = reloc_evaluate(HEAD(TAIL(Parameters)), RELOCT_P);
          file_ok(file);

          if (reg & ~0xf1f) {
            gperror_vwarning(GPW_RANGE, "(%#x & ~0xf1f) != 0", reg, reg);
          }

          _emit(ins->opcode | ((reg & 0x1f) << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_PF:
        /* PIC16 movpf */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 2)) {
          int reg = 0;

          file = reloc_evaluate(HEAD(TAIL(Parameters)), RELOCT_F);
          reg  = reloc_evaluate(HEAD(Parameters), RELOCT_P);
          file_ok(file);

          if (reg & ~0xf1f) {
            gperror_vwarning(GPW_RANGE, "(%#x & ~0xf1f) != 0", reg, reg);
          }

          _emit(ins->opcode | ((reg & 0x1f) << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_SF:
        /* PIC16E movsf */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 2)) {
          int source;
          int dest;

          _check_16e_arg_types(Parameters, arity, AR_INDEX);

          p = HEAD(Parameters);
          source = maybe_evaluate(p);

          p = HEAD(TAIL(Parameters));
          dest = maybe_evaluate(p);

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
          _emit_check(0xf000, reloc_evaluate(HEAD(TAIL(Parameters)), RELOCT_FF2), 0xfff, sym_name);
        }
        break;

      case INSN_CLASS_SS:
        /* PIC16E movss */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 2)) {
          int source;
          int dest;

          _check_16e_arg_types(Parameters, arity, (AR_INDEX << AR_BITS) | AR_INDEX);

          p = HEAD(Parameters);
          source = maybe_evaluate(p);

          p = HEAD(TAIL(Parameters));
          dest = maybe_evaluate(p);

          _emit_check(ins->opcode, source, 0x7f, sym_name);
          _emit_check(0xf000, dest, 0x7f, sym_name);
        }
        break;

      case INSN_CLASS_OPF3:
        /* PIC12 tris */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);
          file = reloc_evaluate(p, RELOCT_TRIS_3BIT);
          file_ok(file);
          _emit(ins->opcode | (file & PIC12_BMSK_TRIS), sym_name);
        }
        break;

      case INSN_CLASS_OPF5:
        /* {PIC12x, SX} (clrf, movwf), SX tris */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);

          if (IS_SX_CORE && (icode == ICODE_TRIS)) {
            file = reloc_evaluate(p, RELOCT_TRIS);
          }
          else {
            file = reloc_evaluate(p, RELOCT_F);
          }

          file_ok(file);
          _emit(ins->opcode | (file & PIC12_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_OPWF5:
        /* {PIC12x, SX} (addwf, andwf, comf, decf, decfsz, incf, incfsz,
                         iorwf, movf, rlf, rrf, subwf, swapf, xorwf) */
        {
          const pnode_t *p2; /* second parameter */
          int d; /* Default destination of 1 (file). */

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          d = 1; /* Default destination of 1 (file). */
          p = HEAD(Parameters);
          switch (arity) {
          case 2:
            p2 = HEAD(TAIL(Parameters));
            /* Allow "w" and "f" as destinations. */
            if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "f") == 0)) {
              d = 1;
            }
            else if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "w") == 0)) {
              d = 0;
            }
            else {
              d = _check_flag(maybe_evaluate(p2));
            }
            break;

          case 1:
            d = 1;
            gperror_vmessage(GPM_NOF, NULL);
            break;

          default:
            enforce_arity(arity, 2);
          }

          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          _emit(ins->opcode | (d << 5) | (file & PIC12_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_B5:
        /* {PIC12x, SX} (bcf, bsf, btfsc, btfss) */
        {
          const pnode_t *b;
          const pnode_t *f;
          int            bit;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (enforce_arity(arity, 2)) {
            f    = HEAD(Parameters);
            b    = HEAD(TAIL(Parameters));
            file = reloc_evaluate(f, RELOCT_F);
            bit  = maybe_evaluate(b);

            if (bit < 0) {
              gperror_vwarning(GPW_RANGE, "Bit{%ins} < 0.", bit);
            }
            else if (bit > 7) {
              gperror_vwarning(GPW_RANGE, "Bit{%ins} > 7.", bit);
            }

            file_ok(file);
            bit &= 7;

            if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
              is_btfsx = true;
            }

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

      case INSN_CLASS_B8:
        /* PIC16 (bcf, bsf, btfsc, btfss, btg) */
        {
          const pnode_t *b;
          const pnode_t *f;
          int            bit;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (enforce_arity(arity, 2)) {
            f    = HEAD(Parameters);
            b    = HEAD(TAIL(Parameters));
            file = reloc_evaluate(f, RELOCT_F);
            bit  = maybe_evaluate(b);

            if (bit < 0) {
              gperror_vwarning(GPW_RANGE, "Bit{%ins} < 0.", bit);
            }
            else if (bit > 7) {
              gperror_vwarning(GPW_RANGE, "Bit{%ins} > 7.", bit);
            }

            file_ok(file);

            if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
              is_btfsx = true;
            }

            _emit(ins->opcode | ((bit & 7) << 8) | (file & PIC16_BMSK_FILE), sym_name);
          }
        }
        break;

      case INSN_CLASS_OPF7:
        /* PIC14x (clrf, movwf, tris) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p = HEAD(Parameters);

          if (icode == ICODE_TRIS) {
            gperror_vwarning(GPW_NOT_RECOMMENDED, "\"tris\"");
            file = reloc_evaluate(p, RELOCT_TRIS);
          }
          else {
            file = reloc_evaluate(p, RELOCT_F);
          }

          file_ok(file);
          _emit(ins->opcode | (file & PIC14_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_OPF8:
        /* PIC16 (cpfseq, cpfsgt, cpfslt, movwf, mulwf, tstfsz) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 1)) {
          p    = HEAD(Parameters);
          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          _emit(ins->opcode | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_OPWF7:
        /* PIC14x (addwf, andwf, comf, decf, decfsz, incf, incfsz, iorwf, movf,
                   rlf, rrf, subwf, swapf, xorwf)
           PIC14E (addwfc, asrf, lslf, lsrf, subwfb) */
        {
          const pnode_t *p2; /* second parameter */
          int d;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          d = 1; /* Default destination of 1 (file). */
          p = HEAD(Parameters);
          switch (arity) {
          case 2:
            p2 = HEAD(TAIL(Parameters));
            /* Allow "w" and "f" as destinations. */
            if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "f") == 0)) {
              d = 1;
            }
            else if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "w") == 0)) {
              d = 0;
            }
            else {
              d = _check_flag(maybe_evaluate(p2));
            }
            break;

          case 1:
            d = 1;
            gperror_vmessage(GPM_NOF, NULL);
            break;

          default:
            enforce_arity(arity, 2);
          }

          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          _emit(ins->opcode | (d << 7) | (file & PIC14_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_OPWF8:
        /* PIC16 (addwf, addwfc, andwf, clrf, comf, daw, decf, decfsz, dcfsnz, incf,
                  incfsz, infsnz, iorwf, rlcf, rlncf, rrcf, rrncf, setf, subwf, subwfb,
                  swapf, xorwf) */
        {
          const pnode_t *p2; /* second parameter */
          int d;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          d = 1; /* Default destination of 1 (file). */
          p = HEAD(Parameters);
          switch (arity) {
          case 2:
            p2 = HEAD(TAIL(Parameters));
            /* Allow "w" and "f" as destinations. */
            if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "f") == 0)) {
              d = 1;
            }
            else if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "w") == 0)) {
              d = 0;
            }
            else {
              d = _check_flag(maybe_evaluate(p2));
            }
            break;

          case 1:
            d = 1;
            gperror_vmessage(GPM_NOF, NULL);
            break;

          default:
            enforce_arity(arity, 2);
          }

          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          _emit(ins->opcode | (d << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_B7:
        /* PIC14x (bcf, bsf, btfsc, btfss) */
        {
          const pnode_t *b;
          const pnode_t *f;
          int            bit;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (enforce_arity(arity, 2)) {
            f    = HEAD(Parameters);
            b    = HEAD(TAIL(Parameters));
            file = reloc_evaluate(f, RELOCT_F);
            bit  = maybe_evaluate(b);

            if (bit < 0) {
              gperror_vwarning(GPW_RANGE, "Bit{%ins} < 0.", bit);
            }
            else if (bit > 7) {
              gperror_vwarning(GPW_RANGE, "Bit{%ins} > 7.", bit);
            }

            file_ok(file);
            bit &= 7;

            if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
              is_btfsx = true;
            }

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
        }
        break;

      case INSN_CLASS_OPFA8:
        /* PIC16E (clrf, cpfseq, cpfsgt, cpfslt, movwf, mulwf, negf, setf, tstfsz) */
        {
          const pnode_t *par; /* second parameter */
          int            a;   /* Default destination of 0 (access). */
          gp_boolean     isAccess;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          a        = 0;
          isAccess = false;
          _check_16e_arg_types(Parameters, arity, AR_BIT_BYTE | AR_INDEX);

          p     = HEAD(Parameters);
          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);

          /* Add relocation for the access bit, if necessary. */
          if (arity < 2) {
            reloc_evaluate(p, RELOCT_ACCESS);
          }

          /* Default access (use the BSR unless access is to special registers) */
          /* borutr: I don't know where is the following coming from, but is seems not to be true for MPASM 5.49,
           * so I commented it out:
           *
           * If extended instructions are enabled, access bit should default to 1 for low-end *
           * of Access Memory unless the file is explicitly an offset (e.g. [foo]) *
          if ((state.extended_pic16e == true) && (file <= 0x5f)) {
            if (p->tag == PTAG_OFFSET) {
              a = 0;
            }
            else {
              a = 1;
            }
          }
          else if ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) {
            a = 0;
          }
          else {
            a = 1;
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
          isAccess = ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) ? true : false;
          a = ((state.extended_pic16e != true) && (!isAccess)) ? 1 : 0;

          switch (arity) {
          case 2:
            par = HEAD(TAIL(Parameters));
            /* Prohibit "a" for BSR to select RAM bank. */
            if ((!state.mpasm_compatible) && (par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "a") == 0)) {
              a = 0;
            }
            /* Allow "b" for BSR to select RAM bank. */
            else if ((par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "b") == 0)) {
              a = 1;
            }
            else {
              a = _check_flag(maybe_evaluate(par));
            }
            break;

          case 1:
            /* use default a */
            if ((!state.mpasm_compatible) && (state.strict_level > 0)) {
              if (isAccess) {
                if (state.strict_level == 2) {
                  gperror_verror(GPE_NOA, NULL);
                }
                else {
                  gperror_vmessage(GPM_NOA, NULL);
                }
              }
              else {
                gperror_vmessage(GPM_BANK, NULL, 0);
              }
            }
            break;

          default:
            enforce_arity(arity, 2);
          }

          _emit(ins->opcode | (a << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_BA8:
        /* PIC16E (bcf, bsf, btfsc, btfss, btg) */
        {
          const pnode_t *b;
          const pnode_t *f;
          const pnode_t *par;
          int            bit;
          int            a;
          gp_boolean     isAccess;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if ((arity != 2) && (arity != 3)) {
            enforce_arity(arity, 3);
            break;
          }

          a        = 0;
          isAccess = false;
          _check_16e_arg_types(Parameters, arity, AR_BIT_BYTE | AR_INDEX);

          f    = HEAD(Parameters);
          file = reloc_evaluate(f, RELOCT_F);

          if (arity == 3) {
            par = HEAD(TAIL(TAIL(Parameters)));

            /* Prohibit "a" for BSR to select RAM bank. */
            if ((!state.mpasm_compatible) && (par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "a") == 0)) {
              a = 0;
            }
            /* Allow "b" for BSR to select RAM bank. */
            else if ((par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "b") == 0)) {
              a = 1;
            }
            else {
              a = _check_flag(maybe_evaluate(par));
            }
          }
          else {
            /* Default access (use the BSR unless access is to special
               registers) */
            /* borutr: I don't know where is the following coming from, but is seems not to be true for MPASM 5.49,
             * so I commented it out:
             *
             * If extended instructions are enabled, access bit should default to 1 for low-end *
             * of Access Memory unless the file is explicitly an offset (e.g. [foo]) *
            if ((state.extended_pic16e == true) && (file <= 0x5f)) {
              if (f->tag == PTAG_OFFSET) {
                a = 0;
              }
              else {
                a = 1;
              }
            }
            else if ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) {
              a = 0;
            }
            else {
              a = 1;
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
            isAccess = ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) ? true : false;
            a = ((state.extended_pic16e != true) && (!isAccess)) ? 1 : 0;

            if ((!state.mpasm_compatible) && (state.strict_level > 0)) {
              if (isAccess) {
                if (state.strict_level == 2) {
                  gperror_verror(GPE_NOA, NULL);
                }
                else {
                  gperror_vmessage(GPM_NOA, NULL);
                }
              }
              else {
                gperror_vmessage(GPM_BANK, NULL, 0);
              }
            }
          }

          /* add relocation for the access bit, if necessary */
          if (arity < 3) {
            reloc_evaluate(f, RELOCT_ACCESS);
          }

          file_ok(file);

          b = HEAD(TAIL(Parameters));
          bit = maybe_evaluate(b);

          if (bit < 0) {
            gperror_vwarning(GPW_RANGE, "Bit{%ins} < 0.", bit);
          }
          else if (bit > 7) {
            gperror_vwarning(GPW_RANGE, "Bit{%ins} > 7.", bit);
          }

          if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
            is_btfsx = true;
          }

          _emit(ins->opcode | (a << 8) | ((bit & 7) << 9) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_OPWFA8:
        /* PIC16E (addwf, addwfc, andwf, comf, decf, decfsz, dcfsnz, incf, incfsz,
                   infsnz, iorwf, movf, rlcf, rlncf, rrcf, rrncf, subfwb, subwf,
                   subwfb, swapf, xorwf) */
        {
          const pnode_t *par; /* second parameter */
          int            d;   /* Default destination of 1 (file). */
          int            a;
          gp_boolean     isAccess;
          gp_boolean     thereIsA;
          gp_boolean     thereIsD;

          if (state.processor == NULL) {
            gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
            return 0;
          }

          if (arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          d        = 1; /* Default destination of 1 (file). */
          a        = 0;
          isAccess = false;
          thereIsA = false;
          thereIsD = false;

          _check_16e_arg_types(Parameters, arity, AR_BIT_BYTE | AR_INDEX);

          p = HEAD(Parameters);
          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);

          /* Add relocation for the access bit, if necessary. */
          if (arity < 3) {
            reloc_evaluate(p, RELOCT_ACCESS);
          }

          /* Default access (use the BSR unless access is to special registers) */
          /* borutr: I don't know where is the following coming from, but is seems not to be true for MPASM 5.49,
           * so I commented it out:
           *
           * If extended instructions are enabled, access bit should default to 1 for low-end *
           * of Access Memory unless the file is explicitly an offset (e.g. [foo]) *
          if ((state.extended_pic16e == true) && (file <= 0x5f)) {
            if (p->tag == PTAG_OFFSET) {
              a = 0;
            }
            else {
              a = 1;
            }
          }
          else if ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) {
            a = 0;
          }
          else {
            a = 1;
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
          isAccess = ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) ? true : false;
          a = ((state.extended_pic16e != true) && (!isAccess)) ? 1 : 0;

          switch (arity) {
          case 3:
            par = HEAD(TAIL(TAIL(Parameters)));

            /* Prohibit "a" for BSR to select RAM bank. */
            if ((!state.mpasm_compatible) && (par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "a") == 0)) {
              a = 0;
            }
            /* Allow "b" for BSR to select RAM bank. */
            else if ((par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "b") == 0)) {
              a = 1;
            }
            else {
              a = _check_flag(maybe_evaluate(par));
            }

            thereIsA = true;

            /* fall through */
          case 2:
            par = HEAD(TAIL(Parameters));
            /* Allow "w" and "f" as destinations. */
            if ((par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "f") == 0)) {
              d = 1;
            }
            else if ((par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "w") == 0)) {
              d = 0;
            }
            else {
              d = _check_flag(maybe_evaluate(par));
            }

            thereIsD = true;
            break;

          case 1:
            /* use default a and d */
            break;

          default:
            enforce_arity(arity, 3);
          }

          if ((!state.mpasm_compatible) && (state.strict_level > 0)) {
            if (!thereIsD) {
              if (state.strict_level == 2) {
                gperror_verror(GPE_NOF, NULL);
              }
              else {
                gperror_vmessage(GPM_NOF, NULL);
              }
            }

            if (!thereIsA) {
              if (isAccess) {
                if (state.strict_level == 2) {
                  gperror_verror(GPE_NOA, NULL);
                }
                else {
                  gperror_vmessage(GPM_NOA, NULL);
                }
              }
              else {
                gperror_vmessage(GPM_BANK, NULL, 0);
              }
            }
          }

          _emit(ins->opcode | (d << 9) | (a << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

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

        if ((icode == ICODE_OPTION) && (state.device.class->core_mask != CORE_12BIT_MASK)){
          gperror_vwarning(GPW_NOT_RECOMMENDED, "\"option\"");
        }

        _emit(ins->opcode, sym_name);
        break;

      case INSN_CLASS_TBL:
        /* PIC16E (tblrd, tblwt) */
        if (enforce_arity(arity, 1)) {
          _check_16e_arg_types(Parameters, arity, 0);

          p = HEAD(Parameters);
          switch(maybe_evaluate(p)) {
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

      case INSN_CLASS_TBL2:
        /* PIC16 (tlrd, tlwt) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 2)) {
          const pnode_t *p2; /* second parameter */
          int            t;  /* read low byte by default */

          /* "0" (lower byte) and "1" (upper byte) */
          p = HEAD(Parameters);
          t = _check_flag(maybe_evaluate(p));

          p2   = HEAD(TAIL(Parameters));
          file = reloc_evaluate(p2, RELOCT_F);

          file_ok(file);
          _emit(ins->opcode | (t << 9) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_TBL3:
        /* PIC16 (tablrd, tablwt) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (enforce_arity(arity, 3)) {
          const pnode_t *p2; /* second parameter */
          const pnode_t *p3; /* third parameter */
          int            t;
          int            inc;

          /* "0" (lower byte) and "1" (upper byte) */
          p = HEAD(Parameters);
          t = _check_flag(maybe_evaluate(p));

          /* "0" (no change) and "1" (postincrement) */
          p2  = HEAD(TAIL(Parameters));
          inc = _check_flag(maybe_evaluate(p2));

          p3   = HEAD(TAIL(TAIL(Parameters)));
          file = reloc_evaluate(p3, RELOCT_F);

          file_ok(file);
          _emit(ins->opcode | (t << 9) | (inc << 8) | (file & PIC16_BMSK_FILE), sym_name);
        }
        break;

      case INSN_CLASS_MOVINDF:
        /* PIC14E (moviw, movwi) */
        if (state.processor == NULL) {
          gperror_verror(GPE_UNDEF_PROC, "\"%s\"", Op_name);
          return 0;
        }

        if (arity == 0) {
          enforce_arity(arity, 1);
          break;
        }

        p = HEAD(Parameters);
        switch (arity) {
          int      fsr;
          int      opcode;
          int      k;
          pnode_t *p2;
          pnode_t *p3;

        case 1:
          fsr = maybe_evaluate(p);
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
          p2  = HEAD(TAIL(Parameters));
          fsr = maybe_evaluate(p2);

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

            switch (maybe_evaluate(p)) {
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
          p2  = HEAD(TAIL(Parameters));
          fsr = maybe_evaluate(p2);

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
            switch (maybe_evaluate(p)) {
            case INDFOFFSET:
              p3 = TAIL(TAIL(Parameters));
              k  = maybe_evaluate(p3); /* Index */

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

      case INSN_CLASS_FUNC:
        r = (*ins->doer)(r, Op_name, arity, Parameters);
        break;

      default:
        assert(0);
        break;
      } /* switch (ins->class) */

      if ((!state.mpasm_compatible) && (state.mode == MODE_ABSOLUTE) && (!state.skipped_inst)) {
        if (ins->inv_mask & INV_MASK_BANK) {
          /* Invalidates the selection of RAM Banks. */
          set_global(GLOBAL_ACT_BANK_ADDR, GLOBAL_ACT_BANK_INV, LFT_TEMPORARY, GVT_CONSTANT, true);
        }

        if (ins->inv_mask & INV_MASK_PAGE) {
          /* Invalidates the selection of ROM Pages. */
          set_global(GLOBAL_ACT_PAGE_ADDR, GLOBAL_ACT_PAGE_INV, LFT_TEMPORARY, GVT_CONSTANT, true);
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
opcode_init(int stage)
{
  const insn_t *base = NULL;
  int           i;
  int           count = 0;
  const char   *name;
  symbol_t     *sym;

  switch (stage) {
  case 0:
    base  = op_0;
    count = num_op_0;
    break;

  case 1:
    base  = op_1;
    count = num_op_1;
    break;

  case 2:
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

  switch (stage) {
  case 1:
    if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
      /* The pageselw directive not supported on pic14 enhanced devices. */
      sym_remove_symbol(state.stBuiltin, "pageselw");
    }
    break;

  case 2:
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

/*------------------------------------------------------------------------------------------------*/

void
begin_cblock(const pnode_t *c)
{
  if (asm_enabled()) {
    state.cblock_defined = true;
    state.cblock = maybe_evaluate(c);
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
cblock_expr(const pnode_t *s)
{
  if (asm_enabled()) {
    set_global(s->value.symbol, state.cblock, LFT_PERMANENT, GVT_CBLOCK, false);
    state.cblock++;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
cblock_expr_incr(const pnode_t *s, const pnode_t *incr)
{
  if (asm_enabled()) {
    set_global(s->value.symbol, state.cblock, LFT_PERMANENT, GVT_CBLOCK, false);
    state.cblock += maybe_evaluate(incr);
  }
}

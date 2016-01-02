/* Implements directives, pseudo-ops and processor opcodes
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin

    Copyright (C) 2014 Molnar Karoly <molnarkaroly@users.sf.net>

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
#include "processor.h"
#include "lst.h"
#include "macro.h"
#include "coff.h"
#include "gperror.h"
#include "special.h"
#include "gpcfg.h"

/* Forward declarations */

#define ATTRIB_COND 1

extern struct pnode *mk_constant(int value);

static gp_boolean prev_btfsx = false;

static unsigned short
checkwrite(unsigned short value)
{
  unsigned short insn;
  int org;

  if (state.mode == MODE_RELOCATABLE) {
    if (state.obj.section == NULL) {
      gpverror(GPE_WRONG_SECTION, NULL);
      return value;
    }
    /* Don't do program memory checks for data memory. */
    if (IS_RAM_ORG) {
      return value;
    }
  }

  if (IS_PIC16_CORE && (state.byte_addr > 0x1ffff)) {
    gpverror(GPE_ADDROVF, "Address{%#x}", state.byte_addr);
  }
  else if ((!IS_PIC16E_CORE) && ((state.byte_addr & 0x1ffff) == 0) && ((int)state.byte_addr > 0)) {
    /* We cast state.byte_addr to signed int on purpose to repeat a bug from
       MPASM 5.34 and pass tb.asm testcase. */
    gperror(GPE_ADDROVF, "Address wrapped around 0.");
  }

  org = gp_processor_byte_to_real(state.processor, state.byte_addr);

  if (state.mpasm_compatible) {
    /* MPASM(X) compatible mode. */
    if (value > state.device.class->core_mask) {
      gpvmessage(GPM_RANGE, NULL, value);
      value &= state.device.class->core_mask;
    }

    if ((state.num.errors == 0) &&
        state.device.class->i_memory_get(state.i_memory, state.byte_addr, &insn, NULL, NULL)) {
      gpverror(GPE_ADDROVR, NULL, org);
    }
  }
  else {
    /* GPASM compatible mode. */
    gp_boolean is_config = (gp_processor_is_config_org(state.processor, org) >= 0) ? true : false;

    if (value > state.device.class->core_mask) {
    /* The size of the config words may be differ the size of the program words. */
      if (!is_config) {
        gpvmessage(GPM_RANGE, NULL, value);
        value &= state.device.class->core_mask;
      }
      else if (value > state.device.class->config_mask) {
        gpvmessage(GPM_RANGE, NULL, value);
        value &= state.device.class->config_mask;
      }
    }

    if (state.num.errors == 0) {
      if (is_config && (state.device.class->config_mask <= 0xFF)) {
        unsigned char byte;

        if (b_memory_get(state.i_memory, state.byte_addr, &byte, NULL, NULL)) {
          gpverror(GPE_ADDROVR, NULL, org);
        }
      }
      else {
        if (state.device.class->i_memory_get(state.i_memory, state.byte_addr, &insn, NULL, NULL)) {
          gpverror(GPE_ADDROVR, NULL, org);
        }
      }
    }
  }

  if (state.maxrom >= 0) {
    if (org > state.maxrom) {
      gpvwarning(GPW_EXCEED_ROM, "Address{%#x} > MAXROM{%#x}", org, state.maxrom);
    }
    else {
      /* check if current org is within a bad address range */
      struct range_pair *cur_badrom;

      for (cur_badrom = state.badrom; cur_badrom != NULL; cur_badrom = cur_badrom->next) {
        if ((org >= cur_badrom->start) && (org <= cur_badrom->end)) {
          gpvwarning(GPW_EXCEED_ROM, "BADROM_START{%#x} <= Address{%#x} <= BADROM_END{%#x}",
                     cur_badrom->start, org, cur_badrom->end);
          break;
        }
      }
    }
  }
  return value;
}

/* Write a word into the memory image at the current location. */

static void
emit(unsigned short value, const char *name)
{
  /* only write the program data to memory on the second pass */
  if (state.pass == 2) {
    value = checkwrite(value);
    state.device.class->i_memory_put(state.i_memory, state.byte_addr, value, name, NULL);
  }

  state.byte_addr += 2;
}

static void
emit_byte(unsigned short value, const char *name)
{
  if (state.pass == 2) {
    if ((state.mode == MODE_RELOCATABLE) && (state.obj.section == NULL)) {
      gpverror(GPE_WRONG_SECTION, NULL);
    }

    if (!IS_RAM_ORG) {
      unsigned char byte;
      int org;

      if (IS_PIC16_CORE && (state.byte_addr > 0x1ffff)) {
        gpverror(GPE_ADDROVF, "Address{%#x} > 0x1ffff", state.byte_addr);
      }
      else if ((!IS_PIC16E_CORE) && ((state.byte_addr & 0x1ffff) == 0) && ((int)state.byte_addr > 0)) {
        gperror(GPE_ADDROVF, "Address wrapped around 0.");
      }

      if (value > state.device.class->core_mask) {
        unsigned short v = value & state.device.class->core_mask;

        gpvwarning(GPW_RANGE, "%i (%#x) => %i (%#x)", value, value, v, v);
        value = v;
      }

      org = gp_processor_byte_to_real(state.processor, state.byte_addr);

      if ((state.num.errors == 0) &&
          b_memory_get(state.i_memory, state.byte_addr, &byte, NULL, NULL)) {
        gpverror(GPE_ADDROVR, NULL, org);
      }

      if (state.maxrom >= 0) {
        if (org > state.maxrom) {
          gpvwarning(GPW_EXCEED_ROM, "Address{%#x} > MAXROM{%#x}", org, state.maxrom);
        }
        else {
          /* check if current org is within a bad address range */
          struct range_pair *cur_badrom;

          for (cur_badrom = state.badrom; cur_badrom != NULL; cur_badrom = cur_badrom->next) {
            if ((org >= cur_badrom->start) && (org <= cur_badrom->end)) {
              gpvwarning(GPW_EXCEED_ROM, "BADROM_START{%#x} <= Address{%#x} <= BADROM_END{%#x}",
                         cur_badrom->start, org, cur_badrom->end);
              break;
            }
          }
        }
      }
    }

    b_memory_put(state.i_memory, state.byte_addr, (unsigned char)value, name, NULL);
  }
  ++state.byte_addr;
}

static gp_boolean
off_or_on(struct pnode *p)
{
  int had_error = false, ret = false;

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
    gperror(GPE_EXPECTED, "Expected \"ON\" or \"OFF\".");
  }

  return ret;
}

/* convert an expression list which may consist of strings, constants, labels,
 * etc. into instruction memory.
 * This is called by the da,data, and dw directives.
 *
 * pnode *L -        A pointer to a doubly-linked list containing all of the
 *                   expressions.
 * int char_shift -  Character width in bits (8 but 7 for da on 14-bit PIC).
 */

static int
emit_data(struct pnode *L, int char_shift, const char *name)
{
  unsigned begin_org = state.byte_addr;
  struct pnode *p;

  for (; L != NULL; L = TAIL(L)) {
    const char *pc;
    p = HEAD(L);

    if (p->tag == PTAG_STRING) {
      pc = p->value.string;

      if (IS_PIC16E_CORE && !(SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
        /* Special case of PIC16E strings in code */
        int n = 0;
        while (*pc != '\0') {
          int value;

          pc = convert_escape_chars(pc, &value);
          emit_byte(value, name);
          ++n;
        }
        /* Pad using zero if 16-bit numbers */
        if (n & 1) {
          emit_byte(0, name);
          ++n;
        }
      }
      else {
        while (*pc != '\0') {
          int value;
          unsigned short v;

          pc = convert_escape_chars(pc, &value);
          value &= 0xff;
          /* If idata or packed and not db or de, emit one character per word */
          if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
            v = value;
          }
          else {
            v = value << char_shift;

            if (*pc != '\0') {
              pc = convert_escape_chars(pc, &value);
              v |= value & 0xff;
            }
          }
          emit(v, name);
        }
        /* For data and packed emit a terminating nul for strings */
        if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
          emit(0, name);
        }
      }
    }
    else if (state.device.class->core_mask > 0xff) {
      unsigned short v;

      v = reloc_evaluate(p, RELOCT_ALL);
      emit(v, name);
    }
    else {
      unsigned short v;
      /* FIXME: This case is for EEPROM8. Do we need the RELOCT_LOW? */
      v = reloc_evaluate(p, RELOCT_LOW);
      emit_byte(v, name);
    }
  }

  return (state.byte_addr - begin_org);
}

/* Do the work for beginning a conditional assembly block.  Leave it
   disabled by default.  This is used by do_if, do_ifdef and
   do_ifndef. */

static void
enter_if(void)
{
  struct amode *new = GP_Malloc(sizeof(*new));

  new->mode = IN_THEN;
  new->prev = state.astack;

  if (state.astack == NULL) {
    new->prev_enabled = true;
  }
  else {
    new->prev_enabled = state.astack->enabled && state.astack->prev_enabled;
  }

  new->enabled = false;         /* Only the default */
  state.astack = new;
}

/* Checking that a macro definition's parameters are correct */

static int
macro_parms_simple(struct pnode *parms)
{
  while (parms != NULL) {
    if (HEAD(parms)->tag != PTAG_SYMBOL) {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
      return 0;
    }
    parms = TAIL(parms);
  }
  return 1;
}

static gp_boolean
macro_parm_unique(struct pnode *M, struct pnode *L)
{
  while (L != NULL) {
    if (STRCMP(M->value.symbol, HEAD(L)->value.symbol) == 0) {
      char buf[BUFSIZ];

      snprintf(buf, sizeof(buf), "Duplicate macro parameter (%s).",
               HEAD(L)->value.symbol);
      gperror(GPE_UNKNOWN, buf);
      return false;
    }
    L = TAIL(L);
  }
  return true;
}

static gp_boolean
macro_parms_ok(struct pnode *parms)
{
  /* Check if all params are symbols. */
  if (!macro_parms_simple(parms)) {
    return false;
  }

  /* Check if params are unique. */
  while (parms != NULL) {
    if (!macro_parm_unique(HEAD(parms), TAIL(parms))) {
      return false;
    }
    parms = TAIL(parms);
  }

  return true;
}

/************************************************************************/

static gpasmVal
do_access_ovr(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sec_name, ".access_ovr", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_ACCESS | STYP_BSS | STYP_OVERLAY;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sec_name, ".access_ovr", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_ACCESS | STYP_ABS | STYP_BSS | STYP_OVERLAY;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal
do_badram(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;
  int maxram;
  int start, end;

  state.lst.line.linetype = LTY_DIR;

  if (parms == NULL) {
    gpverror(GPE_MISSING_ARGU, NULL);
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
            gpvwarning(GPW_INVALID_RAM, "Start{%#x} > End{%#x}", start, end);
          }
          else if (start < 0) {
            gpvwarning(GPW_INVALID_RAM, "Start{%i} < 0", start);
          }
          else if (end > maxram) {
            gpvwarning(GPW_INVALID_RAM, "End{%#x} > MAXRAM{%#x}", end, maxram);
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
            gpvwarning(GPW_INVALID_RAM, "Address{%#x} > MAXRAM{%#x}", loc, maxram);
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

static gpasmVal
do_badrom(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_DIR;

  /* FIXME: implement this directive */
  gpwarning(GPW_UNKNOWN, "gpasm doesn't support the badrom directive yet.");

  return r;
}

static gpasmVal
do_bankisel(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;
  int num_reloc;

  if (((!IS_PIC14_CORE) && (!IS_PIC14E_CORE) && (!IS_PIC14EX_CORE) && (!IS_PIC16_CORE)) ||
      (state.processor->num_banks == 1)) {
    state.lst.line.linetype = LTY_NONE;
    gpvmessage(GPM_EXTPAGE, NULL);
    return r;
  }

  if (prev_btfsx) {
    gpvwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Bankisel");
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
        gpverror(GPE_UNRESOLVABLE, "\"%s\"", name);
      }
      else {
        reloc_evaluate(p, RELOCT_IBANKSEL);

        if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
          unsigned int mask;

          for (mask = 0x100; mask < (state.processor->num_banks << PIC14_BANK_SHIFT); mask <<= 1) {
            emit(0, name);
          }
        }
        else {
          emit(0, name);
        }
      }
    }
  }

  return r;
}

static gpasmVal
do_banksel(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;
  int address;
  int bank;
  int num_reloc;

  if (state.processor == NULL) {
    gpverror(GPE_UNDEF_PROC, NULL);
    return r;
  }

  if (state.processor->num_banks == 1) {
    state.lst.line.linetype = LTY_NONE;
    gpvmessage(GPM_EXTPAGE, NULL);
    /* do nothing */
    return r;
  }

  if (prev_btfsx) {
    gpvwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Banksel");
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
    }
    else {
      num_reloc = count_reloc(p);

      if (num_reloc == 0) {
        /* it is an absolute address, generate the banksel but no relocation */
        address = maybe_evaluate(p);
        bank = gp_processor_check_bank(state.device.class, address);
        state.byte_addr += gp_processor_set_bank(state.device.class,
                                                 state.processor->num_banks,
                                                 bank,
                                                 state.i_memory,
                                                 state.byte_addr);
      }
      else if (num_reloc != 1) {
        gpverror(GPE_UNRESOLVABLE, "\"%s\"", name);
      }
      else if (IS_PIC12E_CORE) {
        reloc_evaluate(p, RELOCT_MOVLB);
        emit(PIC12E_INSN_MOVLB, name);
      }
      else if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
        reloc_evaluate(p, RELOCT_MOVLB);
        emit(PIC14E_INSN_MOVLB, name);
      }
      else if (IS_PIC16_CORE) {
        reloc_evaluate(p, RELOCT_BANKSEL);
        emit(0x0000, name);
      }
      else if (IS_PIC16E_CORE) {
        reloc_evaluate(p, RELOCT_BANKSEL);
        emit(PIC16E_INSN_MOVLB, name);
      }
      else {
        switch (state.processor->num_banks) {
        case 2:
          reloc_evaluate(p, RELOCT_BANKSEL);
          emit(0, name);
          break;

        case 4:
          reloc_evaluate(p, RELOCT_BANKSEL);
          emit(0, name);
          emit(0, name);
          break;
        }
      }
    }
  }

  return r;
}

static gpasmVal
do_code(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sec_name, ".code", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_TEXT;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sec_name, ".code", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = gp_processor_org_to_byte(state.device.class, maybe_evaluate(p));
      state.obj.new_sec_flags = STYP_TEXT | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal
do_code_pack(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;

  if (!IS_PIC16E_CORE) {
    gperror(GPE_UNKNOWN, "code_pack is only supported on 16bit cores.");
  }
  else {
    state.lst.line.linetype = LTY_SEC;
    state.next_state = STATE_SECTION;

    if (state.mode == MODE_ABSOLUTE) {
      gpverror(GPE_OBJECT_ONLY, NULL, name);
    }
    else {
      switch (arity) {
      case 0:
        /* new relocatable section */
        gp_strncpy(state.obj.new_sec_name, ".code", sizeof(state.obj.new_sec_name));
        state.obj.new_sec_addr = 0;
        state.obj.new_sec_flags = STYP_TEXT | STYP_BPACK;
        break;

      case 1:
        /* new absolute section */
        p = HEAD(parms);
        gp_strncpy(state.obj.new_sec_name, ".code", sizeof(state.obj.new_sec_name));
        state.obj.new_sec_addr = gp_processor_org_to_byte(state.device.class, maybe_evaluate(p));
        state.obj.new_sec_flags = STYP_TEXT | STYP_ABS | STYP_BPACK;
        break;

      default:
        enforce_arity(arity, 1);
      }
    }
  }

  return r;
}

static gpasmVal
do_constant(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;
  int first = 1;

  state.lst.line.linetype = LTY_SET4;

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        char *lhs;
        gpasmVal val;

        /* fetch the symbol */
        lhs = p->value.binop.p0->value.symbol;
        /* constants must be assigned a value at declaration */

        val = maybe_evaluate(p->value.binop.p1);
        /* put the symbol and value in the table*/
        set_global(lhs, val, PERMANENT, GVT_CONSTANT);

        if (first) {
          r = val;
          first = 0;
        }
      }
    }
    else {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

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
 * do_gpasm_config() - Called by the parser to process MPASM style CONFIG xx=yy
 *                     directives for all devices.
 */

static gp_boolean config_us_used = false;
static gp_boolean config_mpasm_used = false;

/*
 * Creates the configuration or device id COFF section for do_config and
 * do_gpasm_config. Returns true when a section is created.
 */

static MemBlock *
find_conf_sec_mem(int ca)
{
  struct conf_mem_block_s *p;
  int ba = (IS_PIC16_CORE || IS_PIC16E_CORE) ? ca : (ca - (ca & 1));

  for (p = state.conf_sec_mem; p != NULL; p = p->next) {
    if (p->addr == ba) {
      return p->m;
    }
  }

  return NULL;
}

static MemBlock *
add_conf_sec_mem(int ca, gp_boolean new_config)
{
  struct conf_mem_block_s *new = GP_Malloc(sizeof(struct conf_mem_block_s));

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
    struct conf_mem_block_s *p;

    for (p = state.conf_sec_mem; p->next != NULL; p = p->next)
      ;
    p->next = new;
  }
  else {
    state.conf_sec_mem = new;
  }

  return new->m;
}

static MemBlock *
get_config_mem(int ca, gp_boolean new_config)
{
  if (state.mode != MODE_RELOCATABLE) {
    return state.c_memory;
  }
  else {
    MemBlock *mem = find_conf_sec_mem(ca);

    if (mem == NULL) {
      mem = add_conf_sec_mem(ca, new_config);
    }

    return mem;
  }
}

/* helper to write configuration data, grabbing defaults when necessary */
static void
config_16_set_byte_mem(MemBlock *config_mem, const gp_cfg_device_t *p_dev,
                       int ca, unsigned char byte, unsigned char mask)
{
  unsigned char old_byte;
  char buf[BUFSIZ];

  snprintf(buf, sizeof(buf), "CONFIG_%#x", ca);

  if (!b_memory_get(config_mem, ca, &old_byte, NULL, NULL)) {
    old_byte = (unsigned char)gp_cfg_get_default(p_dev, ca);
  }

  b_memory_put(config_mem, ca, (old_byte & ~mask) | byte, buf, NULL);
}

static void
config_16_set_word_mem(MemBlock *config_mem, const gp_cfg_device_t *p_dev,
                       int ca, unsigned char byte, unsigned char mask)
{
  unsigned char other_byte;
  char buf[BUFSIZ];

  if (!b_memory_get(config_mem, ca ^ 1, &other_byte, NULL, NULL)) {
    snprintf(buf, sizeof(buf), "CONFIG_%#x", ca);
    other_byte = gp_cfg_get_default(p_dev, ca ^ 1);
    b_memory_put(config_mem, ca ^ 1, other_byte, buf, NULL);
  }
  config_16_set_byte_mem(config_mem, p_dev, ca, byte, mask);
}

static gpasmVal
do_config(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;
  int ca;
  int value;

  if (state.processor == NULL) {
    gpverror(GPE_UNDEF_PROC, NULL);
    return r;
  }

  config_us_used = true;

  if (config_mpasm_used) {
    gpverror(GPE_CONFIG_usCONFIG, NULL);
    return r;
  }

  if (IS_PIC16E_CORE) {
    gpwarning(GPW_EXPECTED, "__CONFIG has been deprecated for PIC18 devices. Use directive CONFIG.");
  }

  state.lst.line.linetype = LTY_CONFIG;

  switch(arity) {
  case 1:
    /* FIXME: Whenever there are more than one configuration address, the
       address must be specified in MPASM. MPASM fails with error:
       "Argument out of range (not a valid config register address)" */
    if (IS_PIC16E_CORE) {
      gpwarning(GPW_EXPECTED, "18cxxx devices should specify __CONFIG address.");
    }

    if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
      gpwarning(GPW_EXPECTED, "Enhanced 16cxxx devices should specify __CONFIG address.");
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
    MemBlock *config_mem = get_config_mem(ca, false);
    value = evaluate(p);

    if (IS_PIC16E_CORE) {
      const gp_cfg_device_t *p_dev;

      if (value > 0xff) {
        gpvwarning(GPW_RANGE, "%i (%#x) > 0xff", value, value);
      }

      p_dev = gp_cfg_find_pic_multi_name(state.processor->names, ARRAY_SIZE(state.processor->names));
      if (p_dev != NULL) {
        /* We do this to also set the other byte in a word. */
        config_16_set_word_mem(config_mem, p_dev, ca, value, 0xff);
      }
      else {
        assert(0); /* this shouldn't happen */
        b_memory_put(config_mem, ca, value, NULL, NULL);
        /* Hack in case the config defaults are not available. */
      }
    }
    else {
      unsigned short word;
      char buf[BUFSIZ];

      /* Don't complain for 14 bit enhanced devices.
         Why are the config words 16 bits long in headers?? */
      if ((!IS_PIC14E_CORE) && (!IS_PIC14EX_CORE)) {
        if (value > state.device.class->core_mask) {
          gpvmessage(GPM_RANGE, NULL, value);
          value &= state.device.class->core_mask;
        }
      }

      if (state.device.class->i_memory_get(state.c_memory, ca, &word, NULL, NULL)) {
        gpverror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, ca));
      }

      snprintf(buf, sizeof(buf), "CONFIG_%#x", gp_processor_byte_to_org(state.device.class, ca));
      state.device.class->i_memory_put(config_mem, ca, value, buf, NULL);
    }
  }

  return r;
}

/* Sets defaults over unused portions of configuration memory. */
static void
config_16_check_defaults(MemBlock *config_mem, const gp_cfg_device_t *p_dev)
{
  const gp_cfg_addr_t *addrs = p_dev->addresses;
  int t;

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
    unsigned char byte;

    if (!b_memory_get(config_mem, addrs->address, &byte, NULL, NULL)) {
      config_16_set_byte_mem(config_mem, p_dev, addrs->address, addrs->def_value, 0xff);
    }
  }
}

/* Support MPASM style CONFIG xxx = yyy syntax for PIC16(E) devices. */
static gpasmVal
do_16_config(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  static unsigned char double_mask[64] = { 0, };

  const gp_cfg_device_t *p_dev;
  const gp_cfg_directive_t *p_dir;
  const gp_cfg_option_t *p_opt;
  const char *k_str, *v_str;
  struct pnode *k, *v;
  int ca;
  char v_buff[64];
  char buf[BUFSIZ];

  state.lst.line.linetype = LTY_DIR;
  config_mpasm_used = true;

  if (config_us_used) {
    gpverror(GPE_CONFIG_usCONFIG, NULL);
    return r;
  }

  /* make sure we an find our device in the config DB */
  p_dev = gp_cfg_find_pic_multi_name(state.processor->names, ARRAY_SIZE(state.processor->names));
  if (p_dev == NULL) {
    gpverror(GPE_UNKNOWN,
            "The %s processor has no entries in the config db. CONFIG cannot be used.",
            state.processor->names[2]);
    return r;
  }

  /* validate argument format */
  if ((parms == NULL) || (parms->tag != PTAG_BINOP) || (parms->value.binop.op != '=')) {
    gperror(GPE_CONFIG_UNKNOWN, "Incorrect syntax. use `CONFIG KEY = VALUE'");
    return r;
  }

  /* validate parameter types */
  k = parms->value.binop.p0;
  v = parms->value.binop.p1;

  if ((k->tag != PTAG_SYMBOL) ||
      ((v->tag != PTAG_SYMBOL) && (v->tag != PTAG_CONSTANT))) {
    gperror(GPE_CONFIG_UNKNOWN, "Incorrect syntax. use `CONFIG KEY = VALUE'");
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
        gperror(GPE_CONFIG_UNKNOWN,
                "CONFIG can't be used in source files with a radix other than 10 or 16.");
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
    gperror(GPE_UNKNOWN, buf);
    return r;
  }

  /* Note address to lister, though it doesn't seem to use it. */
  state.lst.config_address = ca;

  /* find the option */
  p_opt = gp_cfg_find_option(p_dir, v_str);
  if (p_opt == NULL) {
    snprintf(buf, sizeof(buf), "CONFIG Directive Error: Specified value not valid for directive: \"%s\"", v_str);
    gperror(GPE_UNKNOWN, buf);
    return r;
  }

  /* emit the bytes if appropriate */
  if (state.pass == 2) {
    unsigned int dm_addr = (ca - p_dev->addresses->address) & 0xFF;
    MemBlock *config_mem = get_config_mem(ca, true);

    if (state.mode != MODE_RELOCATABLE) {
      config_16_check_defaults(config_mem, p_dev);
    }

    /* make sure we've not written here yet */
    if (dm_addr < sizeof(double_mask)) {
      if (double_mask[dm_addr] & p_dir->mask) {
        snprintf(buf, sizeof(buf), "CONFIG Directive Error: Multiple definitions found for %s setting.", k_str);
        gperror(GPE_UNKNOWN, buf);
        return r;
      }
      double_mask[dm_addr] |= p_dir->mask;
    }
    else {
      gpwarning(GPW_UNKNOWN,
                "Double_mask in do_16_config() needs to be adjusted to account for larger config ranges.");
    }

    /* Let the helper set the data. */
    config_16_set_byte_mem(config_mem, p_dev, ca, (unsigned char)p_opt->value,
                           (unsigned char)p_dir->mask);
  }

  return r;
}

/*-------------------------------------------------------------------------*/

static void
config_12_14_set_word_mem(MemBlock *config_mem, const gp_cfg_device_t *p_dev,
                          int ca, unsigned short word, unsigned short mask)
{
  int org;
  unsigned short old_word;
  char buf[BUFSIZ];

  org = gp_processor_byte_to_org(state.device.class, ca);
  snprintf(buf, sizeof(buf), "CONFIG_%#x", org);

  if (!state.device.class->i_memory_get(config_mem, ca, &old_word, NULL, NULL)) {
    old_word = gp_cfg_get_default(p_dev, org);
  }

  word |= (old_word & ~mask);
  word &= state.device.class->config_mask;
  state.device.class->i_memory_put(config_mem, ca, word, buf, NULL);
}

/* Sets defaults over unused portions of configuration memory. */
static void
config_12_14_check_defaults(MemBlock *config_mem, const gp_cfg_device_t *p_dev)
{
  const gp_cfg_addr_t *addrs = p_dev->addresses;
  unsigned int addr;
  unsigned short word;
  int t;

  for (t = 0; t < p_dev->address_count; ++addrs, ++t) {
    addr = gp_processor_org_to_byte(state.device.class, addrs->address);

    if (!state.device.class->i_memory_get(config_mem, addr, &word, NULL, NULL)) {
      word = addrs->def_value & state.device.class->config_mask;
      state.device.class->i_memory_put(config_mem, addr, word, NULL, NULL);
    }
  }
}

/* Support MPASM style CONFIG xxx = yyy syntax for PIC14(E) and PIC12(E) devices. */
static gpasmVal
do_12_14_config(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  static unsigned short double_mask[256] = { 0, };

  const gp_cfg_device_t *p_dev;
  const gp_cfg_directive_t *p_dir;
  const gp_cfg_option_t *p_opt;
  const char *k_str, *v_str;
  const struct pnode *k, *v;
  int conf_org;
  int ca;
  char v_buff[64];
  char buf[BUFSIZ];

  state.lst.line.linetype = LTY_DIR;
  config_mpasm_used = true;

  if (config_us_used) {
    gpverror(GPE_CONFIG_usCONFIG, NULL);
    return r;
  }

  /* make sure we an find our device in the config DB */
  p_dev = gp_cfg_find_pic_multi_name(state.processor->names, ARRAY_SIZE(state.processor->names));
  if (p_dev == NULL) {
    gpverror(GPE_UNKNOWN,
            "The %s processor has no entries in the config db. CONFIG cannot be used.",
            state.processor->names[2]);
    return r;
  }

  /* validate argument format */
  if ((parms == NULL) || (parms->tag != PTAG_BINOP) || (parms->value.binop.op != '=')) {
    gperror(GPE_CONFIG_UNKNOWN, "Incorrect syntax. Use `CONFIG KEY = VALUE'");
    return r;
  }

  /* validate parameter types */
  k = parms->value.binop.p0;
  v = parms->value.binop.p1;

  if ((k->tag != PTAG_SYMBOL) || ((v->tag != PTAG_SYMBOL) && (v->tag != PTAG_CONSTANT))) {
    gperror(GPE_CONFIG_UNKNOWN, "Incorrect syntax. Use `CONFIG KEY = VALUE'");
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
        gperror(GPE_CONFIG_UNKNOWN,
                "CONFIG can't be used in source files with a radix other than 10 or 16.");
      }
    }
    else {
      snprintf(v_buff, sizeof(v_buff), "%d", value);
    }

    v_str = v_buff;
  }

  /* find the directive */
  p_dir = gp_cfg_find_directive(p_dev, k_str, (unsigned int *)&conf_org, NULL);
  if (p_dir == NULL) {
    snprintf(buf, sizeof(buf), "CONFIG Directive Error: Setting not found for %s processor: \"%s\"",
             state.processor->names[2], k_str);
    gperror(GPE_UNKNOWN, buf);
    return r;
  }

  /* Note address to lister, though it doesn't seem to use it. */
  ca = gp_processor_org_to_byte(state.device.class, conf_org);
  state.lst.config_address = ca;

  /* find the option */
  p_opt = gp_cfg_find_option(p_dir, v_str);
  if (p_opt == NULL) {
    snprintf(buf, sizeof(buf), "CONFIG Directive Error: Specified value not valid for directive: \"%s\"", v_str);
    gperror(GPE_UNKNOWN, buf);
    return r;
  }

  /* emit the bytes if appropriate */
  if (state.pass == 2) {
    unsigned int dm_addr = (conf_org - p_dev->addresses->address) & 0xFF;
    MemBlock *config_mem = get_config_mem(ca, true);

    if (state.mode != MODE_RELOCATABLE) {
      config_12_14_check_defaults(config_mem, p_dev);
    }

    /* make sure we've not written here yet */
    if (dm_addr < sizeof(double_mask)) {
      if (double_mask[dm_addr] & p_dir->mask) {
        snprintf(buf, sizeof(buf), "CONFIG Directive Error: Multiple definitions found for %s setting.", k_str);
        gperror(GPE_UNKNOWN, buf);
        return r;
      }
      double_mask[dm_addr] |= p_dir->mask;
    }
    else {
      gpwarning(GPW_UNKNOWN,
                "Double_mask in do_12_14_config() needs to be adjusted to account for larger config ranges.");
    }

    /* Let the helper set the data. */
    config_12_14_set_word_mem(config_mem, p_dev, ca, p_opt->value, p_dir->mask);
  }

  return r;
}

static gpasmVal
do_gpasm_config(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;
  char buf[BUFSIZ];

  if (state.mpasm_compatible) {
    /* The MPASM(X) compatible mode valid only for 16 bit devices. */
    if ((!IS_PIC16_CORE) && (!IS_PIC16E_CORE)) {
      snprintf(buf, sizeof(buf),
               "CONFIG Directive Error: Processor \"%s\" is invalid for CONFIG directive in MPASM mode.",
               state.processor->names[2]);
      gperror(GPE_UNKNOWN, buf);
      return r;
    }

    /* store data from the last section */
    coff_close_section();
  }
  else {
    /* The gpasm compatible mode valid all PIC devices. */
    if ((!IS_PIC16_CORE) && (!IS_PIC16E_CORE) &&
        (!IS_PIC14_CORE) && (!IS_PIC14E_CORE) && (!IS_PIC14EX_CORE) &&
        (!IS_PIC12_CORE) && (!IS_PIC12E_CORE)) {
      snprintf(buf, sizeof(buf),
               "CONFIG Directive Error: Processor \"%s\" is invalid for CONFIG directive.",
               state.processor->names[2]);
      gperror(GPE_UNKNOWN, buf);
      return r;
    }
  }

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if (IS_PIC16_CORE || IS_PIC16E_CORE) {
      do_16_config(r, name, arity, p);
    }
    else {
      do_12_14_config(r, name, arity, p);
    }
  }

  if (state.mpasm_compatible) {
    /* A new section must be started after this directive. */
    state.obj.section = NULL;
  }

  return r;
}

/*-------------------------------------------------------------------------
 *  do_da - The 'da' directive. Identical to 'data' directive, except
 *          for 14-bit cores it packs two 7-bit characters into one word.
 */
static gpasmVal
do_da(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  int char_shift = (IS_PIC14_CORE || IS_PIC14E_CORE || IS_PIC14EX_CORE) ? 7 : 8;

  if ((state.mode == MODE_RELOCATABLE) && (SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
    /* This is a data memory not program. */
    state.lst.line.linetype = LTY_DATA;
    char_shift = 8;
  }

  emit_data(parms, char_shift, name);

  return r;
}

/*-------------------------------------------------------------------------
 * do_data - The 'data' directive. Fill words of program memory with data.
 *           Pack strings two characters into one word, first character in
 *           most significant byte of the word except for the pic18cxxx.
 *           Pic18cxxx has byte addressed program memory and strings are
 *           stored directly. Strings are padded to even number of bytes
 *           by adding zero byte to end of string.
 *           Eeprom16 strings have same byte order than pic18cxxx, but
 *           only because all eeprom16 words are stored most significant
 *           byte first (big-endian).
 *           When in idata section, generate initialization data.
 */

static gpasmVal
do_data(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  if ((state.mode == MODE_RELOCATABLE) && (SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
    /* This is a data memory not program. */
    state.lst.line.linetype = LTY_DATA;
  }

  emit_data(parms, 8, name);

  return r;
}

/*-------------------------------------------------------------------------
 * do_db  - Reserve program memory words with packed 8-bit values. On the
 *          18cxxx families, dw and db are the same. For the 12 and 14 bit
 *          cores, the upper bits are masked (e.g. the 14-bit core can only
 *          store 14bits at a given program memory address, so the upper 2
 *          in a db directive are meaningless.
 */

static gpasmVal
do_db(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *L = parms;
  const struct pnode *p;
  unsigned int org;

  if (state.mode == MODE_RELOCATABLE) {
    if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
      /* This is a data memory not program. */
      state.lst.line.linetype = LTY_DATA;
    }
    else if (!(SECTION_FLAGS & STYP_TEXT)) {
      /* only valid in initialized data and text sections */
      gpverror(GPE_WRONG_SECTION, NULL);
      return r;
    }
  }

  if (IS_PIC16E_CORE || (SECTION_FLAGS & STYP_DATA)) {
    unsigned int begin_byte_addr = state.byte_addr;

    for (; L != NULL; L = TAIL(L)) {
      const char *pc;

      p = HEAD(L);

      if (p->tag == PTAG_STRING) {
        pc = p->value.string;
        while (*pc != '\0') {
          int value;

          pc = convert_escape_chars(pc, &value);
          emit_byte(value, name);
        }
      }
      else {
        int value = reloc_evaluate(p, RELOCT_LOW);

        if (value < 0) {
          gpvwarning(GPW_RANGE, "%i (%#x) < 0", value, value);
        }
        else if (value > 0xFF) {
          gpvwarning(GPW_RANGE, "%i (%#x) > 0xff", value, value);
        }

        emit_byte(value, name);
      }
    }

    if (state.mpasm_compatible) {
      if ((state.mode == MODE_ABSOLUTE) || !(SECTION_FLAGS & (STYP_DATA | STYP_BPACK))) {
        if ((state.byte_addr - begin_byte_addr) & 1) {
          emit_byte(0, name);
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
            emit_byte(0, name);
          }
        }
      }
    }
  } /* if (IS_PIC16E_CORE || (SECTION_FLAGS & STYP_DATA)) */
  else {
    unsigned short v = 0;
    unsigned int n = 0;
    while (L != NULL) {
      const char *pc = NULL;
      int value;

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
          gpvwarning(GPW_RANGE, "%i (%#x) < -128", value, value);
        }
        else if (value > 0xFF) {
          gpvwarning(GPW_RANGE, "%i (%#x) > 0xff", value, value);
        }

        value &= 0xFF;

        if (state.mpasm_compatible) {
          if (!(n & 1)) {
            v = value << 8;
          }
          else {
            emit(v | value, name);
          }
        }
        else {
          org = gp_processor_byte_to_real(state.processor, state.byte_addr);

          if (gp_processor_is_eeprom_org(state.processor, org) >= 0) {
            emit_byte(value, name);
          }
          else {
            if (!(n & 1)) {
              v = value << 8;
            }
            else {
              emit(v | value, name);
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
        emit(v, name);
      }
    }
    else {
      org = gp_processor_byte_to_real(state.processor, state.byte_addr);

      if (gp_processor_is_eeprom_org(state.processor, org) < 0) {
        if (n & 1) {
          emit(v, name);
        }
      }
    }
  } /* else */
  return r;
}

static gpasmVal
do_de(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  if (IS_PIC16E_CORE) {
    return do_db(r, name, arity, parms);
  }

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      const char *pc = p->value.string;

      while (*pc != '\0') {
        int value;

        pc = convert_escape_chars(pc, &value);
        emit(value & 0xff, name);
      }
    }
    else {
      unsigned short v;

      v = reloc_evaluate(p, RELOCT_ALL);
      emit(v & 0xff, name);
    }
  }

  return r;
}

/* Extension to MPASM, used at least by LLVM to emit debugging information. */
static gpasmVal
do_def(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  char *symbol_name = NULL;
  gp_symbol_type *coff_symbol = NULL;
  int eval;
  int value = 0;
  gp_boolean new_class = false;
  int coff_class = C_NULL;
  gp_boolean new_type = false;
  int coff_type = T_NULL;
  enum gpasmValTypes type = GVT_DEBUG;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
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
          char *lhs;

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
              gpverror(GPE_RANGE, "type{%i (%#x)} < 0", eval, eval);
            }
            else if (eval > 0xffff) {
              gpverror(GPE_RANGE, "type{%i (%#x)} > 0xffff", eval, eval);
            }
            else {
              new_type = true;
              coff_type = eval;
            }
          }
          else if (strcasecmp(lhs, "class") == 0) {
            eval = maybe_evaluate(p->value.binop.p1);

            if (eval < -128) {
              gpverror(GPE_RANGE, "class{%i} < -128", eval, eval);
            }
            else if (eval > 127) {
              gpverror(GPE_RANGE, "class{%i} > 127", eval, eval);
            }
            else {
              new_class = true;
              coff_class = eval;
            }
          }
          else {
            gpverror(GPE_ILLEGAL_ARGU, NULL, lhs);
          }
        }
      }
      else {
        if (enforce_simple(p)) {
          if (strcasecmp(p->value.symbol, "absolute") == 0) {
            type = GVT_ABSOLUTE;
            value = 0;
          }
          else if (strcasecmp(p->value.symbol, "debug") == 0) {
            type = GVT_DEBUG;
            value = 0;
          }
          else if (strcasecmp(p->value.symbol, "extern") == 0) {
            type = GVT_EXTERN;
            value = 0;
          }
          else if (strcasecmp(p->value.symbol, "global") == 0) {
            type = GVT_GLOBAL;
            value = IS_RAM_ORG ? state.byte_addr : gp_processor_byte_to_org(state.device.class, state.byte_addr);
          }
          else if (strcasecmp(p->value.symbol, "static") == 0) {
            type = GVT_STATIC;
            value = IS_RAM_ORG ? state.byte_addr : gp_processor_byte_to_org(state.device.class, state.byte_addr);
          }
          else {
            gpverror(GPE_ILLEGAL_ARGU, NULL, p->value.symbol);
          }
        }
      }
    }
  }

  set_global(symbol_name, value, PERMANENT, type);

  /* update the symbol with the values */
  if ((state.pass == 2) && (new_class || new_type)) {
    coff_symbol = gp_coffgen_findsymbol(state.obj.object, symbol_name);
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

static gpasmVal
do_define(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (arity < 1) {
    gpverror(GPE_MISSING_ARGU, NULL);
  }
  else {
    assert(parms->tag == PTAG_LIST);
    p = HEAD(parms);
    assert(p->tag == PTAG_STRING);

    if (asm_enabled() && !IN_MACRO_WHILE_DEFINITION) {
      if (get_symbol(state.stDefines, p->value.string) != NULL) {
        gpverror(GPE_DUPLAB, NULL, p->value.string);
      }
      else {
        struct symbol *curr_def = add_symbol(state.stDefines, p->value.string);

        p = TAIL(parms);

        if (p != NULL) {
          assert(p->tag == PTAG_LIST);
          annotate_symbol(curr_def, p);
        }
      }
    }
  }

  return r;
}

/* Extension to MPASM, used at least by LLVM to emit debugging information. */
static gpasmVal
do_dim(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  const char *symbol_name;
  gp_symbol_type *coff_symbol = NULL;
  int number_symbols;
  gp_aux_type *aux_list;
  int i;
  int value;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
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
      coff_symbol = gp_coffgen_findsymbol(state.obj.object, symbol_name);

      if (coff_symbol == NULL) {
        gpverror(GPE_NOSYM, NULL, symbol_name);
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
      gperror(GPE_UNKNOWN, "Number of auxiliary symbols must be less then 128 and positive.");
      return r;
    }

    state.obj.symbol_num += number_symbols;
    parms = TAIL(parms);

    /* create the symbols */
    aux_list = gp_coffgen_addaux(state.obj.object, coff_symbol);

    /* write the data to the auxiliary symbols */
    i = 0;
    while (parms != NULL) {
      p = HEAD(parms);
      value = maybe_evaluate(p);

      if (value & (~0xff)) {
        gpverror(GPE_RANGE, "%i (%#x) > 0xff", value, value);
        return r;
      }

      if (aux_list == NULL) {
        gperror(GPE_UNKNOWN, "Insufficent number of auxiliary symbols.");
        return r;
      }

      if (i == (state.obj.newcoff ? SYMBOL_SIZE_v2 : SYMBOL_SIZE_v1)) {
        i = 0;
        aux_list = gp_coffgen_addaux(state.obj.object, coff_symbol);
      }

      aux_list->_aux_symbol.data[i++] = value;

      parms = TAIL(parms);
    }
  }

  return r;
}

static gpasmVal
do_direct(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct pnode *p;
  int value;
  unsigned char direct_command = 0;
  const char *direct_string = NULL;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 2)) {
    p = HEAD(parms);
    coerce_str1(p);
    value = maybe_evaluate(p);

    if (value < 0) {
      gpverror(GPE_RANGE, "%i (%#x) < 0", value, value);
    }
    else if (value > 255) {
      gpverror(GPE_RANGE, "%i (%#x) > 255", value, value);
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
        gperror(GPE_UNKNOWN, "String must be less than 255 bytes long.");
      }
    }
    else {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }

    if (direct_string == NULL) {
      return r;
    }

    if (SECTION_FLAGS & STYP_TEXT) {
      coff_add_directsym(direct_command, direct_string);
    }
    else {
      gpverror(GPE_WRONG_SECTION, NULL);
    }
  }

  return r;
}

static gpasmVal
do_dt(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  int retlw = gp_processor_retlw(state.device.class);

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      const char *pc = p->value.string;

      while (*pc != '\0') {
        int value;

        pc = convert_escape_chars(pc, &value);
        emit((value & 0xff) | retlw, name);
      }
    }
    else {
      unsigned short v;

      v = reloc_evaluate(p, RELOCT_ALL);
      emit((v & 0xff) | retlw, name);
    }
  }

  return r;
}

static gpasmVal
do_dtm(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  struct symbol *s = get_symbol(state.stBuiltin, "movlw");
  const struct insn *i = get_symbol_annotation(s);

  if ((!IS_PIC14E_CORE) && (!IS_PIC14EX_CORE)) {
    gpverror(GPE_ILLEGAL_DIR, NULL, name);
  }

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      const char *pc = p->value.string;

      while (*pc != '\0') {
        int value;

        pc = convert_escape_chars(pc, &value);
        emit(i->opcode | (value & 0xff), name);
      }
    }
    else {
      unsigned short v;

      v = reloc_evaluate(p, RELOCT_ALL);
      emit(i->opcode | (v & 0xff), name);
    }
  }

  return r;
}

/*-------------------------------------------------------------------------
 *  do_dw - The 'dw' directive. On all families except for the p18cxxx, the
 *          dw directive is the same as the 'data' directive. For the p18cxxx
 *          it's the same as the 'db' directive. (That's strange, but it's
 *          also the way mpasm does it.)
 */
static gpasmVal
do_dw(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  if (state.mode == MODE_RELOCATABLE) {
    if (SECTION_FLAGS & (STYP_DATA | STYP_BPACK)) {
      /* This is a data memory not program. */
      state.lst.line.linetype = LTY_DATA;
    }
    else if (!(SECTION_FLAGS & STYP_TEXT)) {
      /* only valid in initialized data and text sections */
      gpverror(GPE_WRONG_SECTION, NULL);
      return r;
    }
  }
  /* MPASM 5.34 seems to do this same for p18cxxx as for others. */
  emit_data(parms, 8, name);

  return r;
}


static gpasmVal
do_else(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (state.astack == NULL) {
    gpverror(GPE_ILLEGAL_COND, NULL);
  }
  else if ((state.astack->mode != IN_THEN)) {
    gpverror(GPE_ILLEGAL_COND, NULL);
  }
  else {
    state.astack->enabled = !state.astack->enabled;
  }

  return r;
}

static gpasmVal
do_end(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.found_end = true;
  state.lst.line.linetype = LTY_DIR;

  return r;
}

static gpasmVal
do_endif(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (state.astack == NULL) {
    gperror(GPE_ILLEGAL_COND, "Illegal condition: \"ENDIF\"");
  }
  else if ((state.astack->mode != IN_THEN) && (state.astack->mode != IN_ELSE)) {
    gperror(GPE_ILLEGAL_COND, "Illegal condition: \"ENDIF\"");
  }
  else {
    struct amode *old;

    old = state.astack;
    state.astack = state.astack->prev;
    free(old);
  }
  return r;
}

static gpasmVal
do_endm(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  assert(state.mac_head == NULL);
  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (!IN_MACRO_WHILE_DEFINITION) {
    gpverror(GPE_UNMATCHED_ENDM, NULL);
  }
  else {
    state.mac_prev = NULL;
  }

  state.mac_body = NULL;

  return r;
}

static gpasmVal
do_endw(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_NOLIST_DIR;
  assert(state.mac_head == NULL);

  if (!IN_MACRO_WHILE_DEFINITION) {
    gperror(GPE_ILLEGAL_COND, "Illegal condition: \"ENDW\"");
  }
  else if (maybe_evaluate(state.while_head->parms)) {
    state.next_state = STATE_WHILE;
    state.next_buffer.macro = state.while_head;
  }
  else if (state.pass == 2) {
    list_macro(state.while_head->body);
    state.preproc.do_emit = false;
  }

  state.mac_body = NULL;
  state.mac_prev = NULL;
  state.while_head = NULL;

  return r;
}

static gpasmVal
do_eof(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 0)) {
    if (state.debug_info) {
      coff_add_eofsym();
    }
    else {
      gpwarning(GPW_UNKNOWN, "Directive ignored when debug info is disabled.");
    }
  }

  return r;
}

static gpasmVal
do_equ(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_EQU;

  if (enforce_arity(arity, 1)) {
    r = maybe_evaluate(HEAD(parms));
  }

  return r;
}

static gpasmVal
do_error(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      gpverror(GPE_USER, NULL, p->value.string);
    }
    else {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

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
 *   struct pnode *parms - a linked list of the parsed parameters
 *
 *
 *
 ************************************************************************/

static gpasmVal
do_errlvl(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  state.lst.line.linetype = LTY_DIR;

  if (state.pass == 2) {
    for (; parms != NULL; parms = TAIL(parms)) {
      p = HEAD(parms);

      if (p->tag == PTAG_UNOP)  {
        gpasmVal value = evaluate(p->value.unop.p0);

        if (p->value.unop.op == '-') {
          add_code(-value);
        }
        else if (p->value.unop.op == '+') {
          add_code(value);
        }
        else {
          gperror(GPE_ILLEGAL_ARGU, "Expected 0, 1, 2, +|-<message number>.");
        }
      }
      else if (p->tag == PTAG_CONSTANT) {
        select_errorlevel(p->value.constant);
      }
      else {
        gperror(GPE_ILLEGAL_ARGU, "Expected 0, 1, 2, +|-<message number>.");
      }
    }
  }

  return r;
}

static gpasmVal
do_exitm(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 0)) {
    if (state.stGlobal == state.stTop) {
      gperror(GPE_UNKNOWN, "Attempt to use \"exitm\" outside of macro.");
    }
    else {
      state.next_state = STATE_EXITMACRO;
    }
  }

  return r;
}

static gpasmVal
do_expand(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (state.cmd_line.macro_expand) {
    gpvmessage(GPM_SUPLIN, NULL);
  }
  else {
    if (enforce_arity(arity, 0)) {
      state.lst.expand = true;
    }
  }
  return r;
}

static gpasmVal
do_extern(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const char *p;

  state.lst.line.linetype = LTY_SET4;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    for (; parms != NULL; parms = TAIL(parms)) {
      p = HEAD(parms)->value.symbol;

      if (p != NULL) {
        set_global(p, 0, PERMANENT, GVT_EXTERN);
      }
    }
  }

  return 0;
}

static gpasmVal
do_file(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 1)) {
    if (state.debug_info) {
      p = HEAD(parms);

      if (p->tag == PTAG_STRING) {
        state.obj.debug_file = coff_add_filesym(p->value.string, 0);
      }
      else {
        gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
      }
    }
    else {
      gpwarning(GPW_UNKNOWN, "Directive ignored when debug info is disabled.");
    }
  }

  return r;
}

/* Filling constants is handled here. Filling instructions is handled
   in the parser. */

static gpasmVal
do_fill(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *h;
  int number;
  int i;

  if (enforce_arity(arity, 2)) {
    h = HEAD(parms);
    number = eval_fill_number(HEAD(TAIL(parms)));

    for (i = 1; i <= number ; i += 1) {
      /* we must evaluate each loop, because some symbols change (i.e. $) */
      emit(maybe_evaluate(h), name);
    }
  }
  return r;
}

static gpasmVal
do_global(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const char *p;
  const struct symbol *s;
  struct variable *var;
  char buf[BUFSIZ];

  state.lst.line.linetype = LTY_SET4;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    for (; parms != NULL; parms = TAIL(parms)) {
      p = HEAD(parms)->value.symbol;

      if (p != NULL) {
        s = get_symbol(state.stTop, p);

        if (s == NULL) {
          gpverror(GPE_NOSYM, NULL, p);
        }
        else {
          var = get_symbol_annotation(s);

          if (var == NULL) {
            snprintf(buf, sizeof(buf), "Symbol not assigned a value: \"%s\"", p);
            gpwarning(GPW_UNKNOWN, buf);
          }
          else {
            if ((var->previous_type == GVT_ADDRESS) ||
                (var->previous_type == GVT_GLOBAL)  ||
                (var->previous_type == GVT_STATIC)) {
              /* make the symbol global */
              var->type = GVT_GLOBAL;
            }
            else if (var->previous_type == GVT_EXTERN) {
              gpverror(GPE_DUPLAB, NULL, s);
            }
            else {
              snprintf(buf, sizeof(buf), "Operand must be an address label: \"%s\"", p);
              gperror(GPE_MUST_BE_LABEL, buf);
            }
          }
        }
      }
      else {
        gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
      }
    }
  }

  return 0;
}

static gpasmVal
do_idata(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (IS_PIC12_CORE) {
    gpverror(GPE_ILLEGAL_DIR, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sec_name, ".idata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_DATA;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sec_name, ".idata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_DATA | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal
do_idata_acs(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (IS_PIC12_CORE) {
    gpverror(GPE_ILLEGAL_DIR, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sec_name, ".idata_acs", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_DATA | STYP_ACCESS;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sec_name, ".idata_acs", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_DATA | STYP_ABS | STYP_ACCESS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal
do_ident(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      coff_add_identsym(p->value.string);
    }
    else {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

static gpasmVal
do_idlocs(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  int value, v;
  unsigned int id_location;
  unsigned int idreg;
  char buf[BUFSIZ];

  if (state.processor == NULL) {
    gpverror(GPE_UNDEF_PROC, NULL);
    return r;
  }

  id_location = gp_processor_id_location(state.processor);

  if (id_location == 0) {
    gpverror(GPE_ILLEGAL_DIR, NULL, name);
    return r;
  }

  if (IS_PIC16E_CORE) {
    if (enforce_arity(arity, 2)) {
      idreg = gp_processor_org_to_byte(state.device.class, maybe_evaluate(HEAD(parms)));
      value = maybe_evaluate(HEAD(TAIL(parms)));
    }
    else {
      gperror(GPW_EXPECTED,"18cxxx devices should specify __IDLOC address.");
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
    MemBlock *m = (state.mode == MODE_RELOCATABLE) ? state.obj.section->data : state.c_memory;

    if (IS_PIC16E_CORE) {
      if (idreg < state.processor->idlocs_addrs[0]) {
        gpverror(GPE_RANGE, "Not a valid ID location. Address{%#x} < IDLOCS_MIN{%#x}",
                 idreg, state.processor->idlocs_addrs[0]);
      }
      else if (idreg > state.processor->idlocs_addrs[1]) {
        gpverror(GPE_RANGE, "Not a valid ID location. Address{%#x} > IDLOCS_MAX{%#x}",
                 idreg, state.processor->idlocs_addrs[1]);
      }
      else {
        unsigned char curvalue;

        state.lst.line.linetype = LTY_CONFIG;
        state.lst.config_address = idreg;

        if (value & ~0xff) {
          v = value;
          value &= 0xff;
          snprintf(buf, sizeof(buf), "An ID Locations value too large. Last two hex digits used: 0x%X ==> 0x%02X", v, value);
          gpmessage(GPM_IDLOC, buf);
        }

        if (idreg <= state.device.id_location) {
          gpverror(GPE_IDLOCS_ORDER, NULL);
        }

        if (b_memory_get(m, idreg, &curvalue, NULL, NULL)) {
          gpverror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
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
      unsigned short word;

      state.lst.line.linetype = LTY_IDLOCS;

      if (value & ~0xffff) {
        v = value;
        value &= 0xffff;
        gpvmessage(GPM_IDLOC, NULL, v, value);
      }

      if (state.device.class->i_memory_get(m, idreg, &word, NULL, NULL)) {
        gpverror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
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

/* Support IDLOCS "abcdef" or IDLOCS 'a', 'b', 'c' syntax for PIC16E devices. */
static gpasmVal
do_16_idlocs(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  static unsigned int last_idreg = 0;

  const struct pnode *p;
  unsigned int idreg;
  unsigned char curvalue;
  const char *pc;
  int value, v;
  int max_bytes;
  int n;
  char buf[BUFSIZ];

  if (state.mpasm_compatible) {
    snprintf(buf, sizeof(buf), "Directive Error: The %s directive is invalid in MPASM mode.", name);
    gperror(GPE_UNKNOWN, buf);
    return r;
  }

  if (state.processor == NULL) {
    gpverror(GPE_UNDEF_PROC, NULL);
    return r;
  }

  if (!IS_PIC16E_CORE) {
    gperror(GPE_IDLOCS_P16E, NULL);
    return r;
  }

  idreg = (last_idreg == 0) ? gp_processor_id_location(state.processor) : last_idreg;

  if (idreg == 0) {
    snprintf(buf, sizeof(buf), "The IDLOCS registers not exist in the %s MCU.",
             state.processor->names[2]);
    gperror(GPE_UNKNOWN, buf);
    return r;
  }

  if ((state.mode == MODE_RELOCATABLE) && (!state.found_idlocs)) {
    coff_new_section(".idlocs", idreg, STYP_ABS | STYP_TEXT);
    state.found_idlocs = true;
  }

  if (state.pass == 2) {
    MemBlock *m = (state.mode == MODE_RELOCATABLE) ? state.obj.section->data : state.c_memory;

    if (idreg < state.processor->idlocs_addrs[0]) {
      gpverror(GPE_RANGE, "Not a valid ID location. Address{%#x} < IDLOCS_MIN{%#x}",
               idreg, state.processor->idlocs_addrs[0]);
      return r;
    }
    else if (idreg > state.processor->idlocs_addrs[1]) {
      gpverror(GPE_RANGE, "Not a valid ID location. Address{%#x} > IDLOCS_MAX{%#x}",
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
          gpwarning(GPW_UNKNOWN, "The number of IDLOCS registers not more than 8!");
          goto warning;
        }

        if (value & ~0xff) {
          v = value;
          value &= 0xff;
          snprintf(buf, sizeof(buf), "An ID Locations value too large. Last two hex digits used: 0x%X ==> 0x%02X", v, value);
          gpmessage(GPM_IDLOC, buf);
        }

        if (b_memory_get(m, idreg, &curvalue, NULL, NULL)) {
          gpverror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
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
            gpwarning(GPW_UNKNOWN, "The number of IDLOCS registers not more than 8!");
            goto warning;
          }

          pc = convert_escape_chars(pc, &value);

          if (b_memory_get(m, idreg, &curvalue, NULL, NULL)) {
            gpverror(GPE_ADDROVR, NULL, gp_processor_byte_to_org(state.device.class, idreg));
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
        gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
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

static gpasmVal
do_if(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  enter_if();

  /* Only evaluate the conditional if it matters... */
  if (state.astack->prev_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);
      state.astack->enabled = maybe_evaluate(p);
    }
  }

  return r;
}

static gpasmVal
do_ifdef(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  enter_if();

  /* Only evaluate the conditional if it matters... */
  if (state.astack->prev_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);

      if (p->tag != PTAG_SYMBOL) {
        gperror(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else {
        if ((get_symbol(state.stDefines, p->value.symbol)) ||
            (get_symbol(state.stTop, p->value.symbol))) {
          state.astack->enabled = true;
        }
      }
    }
  }

  return r;
}

static gpasmVal
do_ifndef(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  enter_if();

  /* Only evaluate the conditional if it matters... */
  if (state.astack->prev_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);

      if (p->tag != PTAG_SYMBOL) {
        gperror(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else {
        if ((!get_symbol(state.stDefines, p->value.symbol)) &&
            (!get_symbol(state.stTop, p->value.symbol))) {
          state.astack->enabled = true;
        }
      }
    }
  }

  return r;
}

static gpasmVal
do_include(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      state.next_state = STATE_INCLUDE;
      state.next_buffer.file = GP_Strdup(p->value.string);
    }
    else {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

static gpasmVal
do_line(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 1)) {
    if (state.debug_info) {
      p = HEAD(parms);
      state.obj.debug_line = maybe_evaluate(p);
    }
    else {
      gpwarning(GPW_UNKNOWN, "Directive ignored when debug info is disabled.");
    }
  }

  return r;
}

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
 *   struct pnode *parms - a linked list of the parsed parameters
 *
 *
 *
 ************************************************************************/

static gpasmVal
do_list(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

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
            gperror(GPE_RANGE, message);
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
          /* Undocumented, only found in MEMORY.INC and
             MCP250XX.INC. */
          if (can_evaluate(p->value.binop.p1)) {
            int value = evaluate(p->value.binop.p1);

            if (value < state.maxrom) {
              char message[BUFSIZ];

              snprintf(message, sizeof(message),
                       "Argument out of range, must be greater than or equal to %ld.",
                       state.maxrom);
              gperror(GPE_RANGE, message);
            }
            else {
              state.maxrom = value;
            }
          }
        }
        else if (strcasecmp(lhs, "mm") == 0) {
          state.lst.memorymap = off_or_on(p->value.binop.p1);
        }
        else if (strcasecmp(lhs, "n") == 0) {
          if (can_evaluate(p->value.binop.p1)) {
            int number = evaluate(p->value.binop.p1);

            if ((number > 6) || (number == 0)) {
              state.lst.linesperpage = number;
            }
            else {
              gpverror(GPE_RANGE, "0 < n{%i} <= 6", number);
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
          state.lst.symboltable = off_or_on(p->value.binop.p1);
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
          gpverror(GPE_ILLEGAL_ARGU, NULL, lhs);
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
          gpverror(GPE_ILLEGAL_ARGU, NULL, p->value.symbol);
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

static gpasmVal
do_local(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  int first = 1;

  state.lst.line.linetype = LTY_SET4;

  /* like variable except it is put in TOP instead of GLOBAL */

  if (state.stGlobal == state.stTop) {
    gperror(GPE_UNKNOWN, "Attempt to use \"local\" outside of macro.");
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
          add_symbol(state.stTop, lhs);
          set_global(lhs, val, TEMPORARY, GVT_CONSTANT);

          if (first) {
            r = val;
            first = 0;
          }
        }
      }
      else if (p->tag == PTAG_SYMBOL) {
        /* put the symbol in the Top table */
        add_symbol(state.stTop, p->value.symbol);

        if (first) {
          r = 0;
          first = 0;
        }
      }
      else {
        gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
      }
    }
  }

  return r;
}

static gpasmVal
do_noexpand(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (state.cmd_line.macro_expand) {
    gpvmessage(GPM_SUPLIN, NULL);
  }
  else {
    if (enforce_arity(arity, 0)) {
      state.lst.expand = false;
    }
  }
  return r;
}

static gpasmVal
do_nolist(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (!state.lst.force) {
    state.lst.enabled = false;
  }

  coff_add_nolistsym();

  return r;
}

static gpasmVal
do_maxram(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_SET;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (can_evaluate(p)) {
      r = state.maxram = evaluate(p);
    }
  }

  return r;
}

static gpasmVal
do_maxrom(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_SET;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (can_evaluate(p)) {
      r = state.maxrom = evaluate(p);
    }
  }

  return r;
}

static gpasmVal
do_macro(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct macro_head *head = GP_Malloc(sizeof(*head));

  head->parms = parms;
  head->body = NULL;
  head->defined = false;
  /* Record data for the list, cod, and coff files. */
  head->line_number = state.src->line_number;
  head->file_symbol = state.src->file_symbol;

  head->src_name = GP_Strdup(state.src->name);

  state.lst.line.linetype = LTY_DIR;

  if (macro_parms_ok(parms)) {
    state.mac_head = head;
  }

  state.mac_prev = &(head->body);
  state.mac_body = NULL;

  return r;
}

static gpasmVal
do_messg(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      gpvmessage(GPM_USER, NULL, p->value.string);
    }
    else {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

static gpasmVal
do_org(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

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
          gpverror(GPE_ORG_ODD, "Address{%#x}", r);
        }
      }

      if (state.mode == MODE_ABSOLUTE) {
        state.lst.line.linetype = LTY_ORG;
        state.byte_addr = new_byte_addr;
      }
      else {
        /* Default section name, this will be overwritten if a label is present. */
        snprintf(state.obj.new_sec_name, sizeof(state.obj.new_sec_name), ".org_%x", r);
        state.obj.new_sec_addr = new_byte_addr;
        state.obj.new_sec_flags = STYP_TEXT | STYP_ABS;
        state.lst.line.linetype = LTY_SEC;
        state.next_state = STATE_SECTION;
      }
    }
    else {
      r = 0;
    }
  }

  return r;
}

static gpasmVal
do_page(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 0) && state.lst.enabled) {
    lst_page_start();
  }

  return r;
}

/* Called by both do_pagesel and do_pageselw, which have a very slight
 * difference between them */
static gpasmVal
_do_pagesel(gpasmVal r, const char *name, int arity, struct pnode *parms, unsigned short reloc_type)
{
  const struct pnode *p;
  int page;
  int num_reloc;
  gp_boolean use_wreg = false;

  if (state.processor == NULL) {
    gpverror(GPE_UNDEF_PROC, NULL);
    return r;
  }

  if ((reloc_type == RELOCT_PAGESEL_WREG) || IS_PIC16_CORE) {
    use_wreg = true;
  }

  if (IS_EEPROM8 || IS_EEPROM16 || IS_PIC16E_CORE ||
      (state.processor->num_pages == 1)) {
    state.lst.line.linetype = LTY_NONE;
    gpvmessage(GPM_EXTPAGE, NULL);
    return r;
  }

  if (IS_PIC16_CORE) {
    gpvmessage(GPM_W_MODIFIED, NULL);
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
    }
    else {
      num_reloc = count_reloc(p);

      if (num_reloc == 0) {
        /* it is an absolute address, generate the pagesel but no relocation */
        page = gp_processor_check_page(state.device.class, maybe_evaluate(p));
        state.byte_addr += gp_processor_set_page(state.device.class,
                                                 state.processor->num_pages,
                                                 page,
                                                 state.i_memory,
                                                 state.byte_addr,
                                                 use_wreg);
      }
      else if (num_reloc != 1) {
        gperror(GPE_ILLEGAL_LABEL, "Illegal label.");
      }
      else if (IS_PIC16_CORE) {
        reloc_evaluate(p, RELOCT_PAGESEL_WREG);
        emit(0x0000, name);
        emit(0x0000, name);
      }
      else if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
        if (!use_wreg) {
          reloc_evaluate(p, RELOCT_PAGESEL_MOVLP);
          emit(PIC14E_INSN_MOVLP, name);
        }
        else {
          reloc_evaluate(p, RELOCT_PAGESEL_WREG);
          emit(0x0000, name);
          emit(0x0000, name);
        }
      }
      else {
        if (!use_wreg && (state.processor->num_pages == 2)) {
          reloc_evaluate(p, RELOCT_PAGESEL_BITS);
          emit(0, name);
        }
        else if (state.processor->num_pages >= 2) {
          reloc_evaluate(p, reloc_type);
          emit(0x0000, name);
          emit(0x0000, name);
        }
      }
    }
  }

  return r;
}

static gpasmVal
do_pagesel(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  if (prev_btfsx) {
    gpvwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Pagesel");
  }

  return _do_pagesel(r, name, arity, parms, RELOCT_PAGESEL_BITS);
}

static gpasmVal
do_pageselw(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  if (prev_btfsx) {
    gpvwarning(GPW_BANK_PAGE_SEL_AFTER_SKIP, NULL, "Pageselw");
  }

  return _do_pagesel(r, name, arity, parms, RELOCT_PAGESEL_WREG);
}

static gpasmVal
do_processor(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 1)) {
    const struct pnode *p = HEAD(parms);

    if (enforce_simple(p)) {
      select_processor(p->value.symbol);
    }
  }

  return r;
}

static gpasmVal
do_radix(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_DIR;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (enforce_simple(p)) {
      select_radix(p->value.symbol);
    }
  }

  return r;
}

static gpasmVal
do_res(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  int count;
  int i;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (can_evaluate(p)) {
      count = evaluate(p);

      state.lst.line.linetype = LTY_RES;

      if (state.mode == MODE_ABSOLUTE) {
        if (IS_PIC16E_CORE && ((count % 2) != 0)) {
          gpverror(GPE_RES_ODD_PIC16EA, "res = %i", count);
        }

        count = gp_processor_org_to_byte(state.device.class, count);

        for (i = 0; (i + 1) < count; i += 2) {
          emit(state.device.class->core_mask, name);
        }
      }
      else {
        if (SECTION_FLAGS & STYP_TEXT) {
          count = gp_processor_org_to_byte(state.device.class, count);

          if (state.device.class->rom_width < 16) {
            /* FIXME: Most likely this check belongs to our caller. */
            if (count < 2) {
              gpwarning(GPW_UNKNOWN, "No memory has been reserved by this instruction.");
            }
          }
          for (i = 0; (i + 1) < count; i += 2) {
            /* For some reason program memory is filled with a different value. */
            emit(state.device.class->core_mask, name);
          }
        }
        else {
          for (i = 0; i < count; i++) {
            emit_byte(0, name);
          }
        }
      }
    }
  }

  return r;
}

static gpasmVal
do_set(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

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

static gpasmVal
do_space(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

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

static gpasmVal
do_subtitle(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      gp_strncpy(state.lst.subtitle_name, p->value.string, sizeof(state.lst.subtitle_name));
    }
    else {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

static gpasmVal
do_title(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  state.lst.line.linetype = LTY_NONE;

  if (enforce_arity(arity, 1)) {
    const struct pnode *p = HEAD(parms);

    if (p->tag == PTAG_STRING) {
      gp_strncpy(state.lst.title_name, p->value.string, sizeof(state.lst.title_name));
    }
    else {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

static gpasmVal
do_type(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  const char *symbol_name = NULL;
  gp_symbol_type *coff_symbol = NULL;
  int value;

  state.lst.line.linetype = LTY_DIR;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else if (enforce_arity(arity, 2)) {
    /* the first argument is the symbol name */
    p = HEAD(parms);

    if (enforce_simple(p)) {
      symbol_name = p->value.symbol;
      coff_symbol = gp_coffgen_findsymbol(state.obj.object, symbol_name);

      if (coff_symbol == NULL) {
        gpverror(GPE_NOSYM, NULL, symbol_name);
      }
      else {
        p = HEAD(TAIL(parms));
        value = maybe_evaluate(p);

        if (value < 0) {
          gpverror(GPE_RANGE, "type{%i (%#x)} < 0", value, value);
        }
        else if (value > 0xffff) {
          gpverror(GPE_RANGE, "type{%i (%#x)} > 0xffff", value, value);
        }
        else {
          coff_symbol->type = value;
        }
      }
    }
  }

  return r;
}

static gpasmVal
do_udata(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sec_name, ".udata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sec_name, ".udata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal
do_udata_acs(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sec_name, ".udata_acs", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS | STYP_ACCESS;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sec_name, ".udata_acs", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS | STYP_ACCESS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal
do_udata_ovr(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sec_name, ".udata_ovr", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS | STYP_OVERLAY;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sec_name, ".udata_ovr", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS | STYP_OVERLAY;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal
do_udata_shr(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_SEC;
  state.next_state = STATE_SECTION;

  if (state.mode == MODE_ABSOLUTE) {
    gpverror(GPE_OBJECT_ONLY, NULL, name);
  }
  else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      gp_strncpy(state.obj.new_sec_name, ".udata_shr", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS | STYP_SHARED;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      gp_strncpy(state.obj.new_sec_name, ".udata_shr", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS | STYP_SHARED;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal
do_undefine(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;

  state.lst.line.linetype = LTY_DIR;
  state.preproc.do_emit = false;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);

    if (p->tag == PTAG_SYMBOL) {
      if (remove_symbol(state.stDefines, p->value.symbol) == 0) {
        gpvwarning(GPW_NOT_DEFINED, NULL, p->value.symbol);
      }
    }
    else {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

static gpasmVal
do_variable(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  const struct pnode *p;
  gp_boolean first = true;

  state.lst.line.linetype = LTY_SET4;

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        char *lhs;
        gpasmVal val;

        /* fetch the symbol */
        lhs = p->value.binop.p0->value.symbol;
        val = maybe_evaluate(p->value.binop.p1);
        /* put the symbol and value in the table */
        set_global(lhs, val, TEMPORARY, GVT_CONSTANT);

        if (first) {
          r = val;
          first = false;
        }
      }
    }
    else if (p->tag == PTAG_SYMBOL) {
      /* put the symbol with a 0 value in the table */
      set_global(p->value.symbol, 0, TEMPORARY, GVT_CONSTANT);

      if (first) {
        r = 0;
        first = false;
      }
    }
    else {
      gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
    }
  }

  return r;
}

static gpasmVal
do_while(gpasmVal r, const char *name, int arity, struct pnode *parms)
{
  struct macro_head *head = GP_Malloc(sizeof(*head));

  assert(state.while_depth == 0);

  state.lst.line.linetype = LTY_DOLIST_DIR;
  head->parms = (enforce_arity(arity, 1)) ? HEAD(parms) : NULL;
  head->body = NULL;

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

int
asm_enabled(void)
{
  return ((state.astack == NULL) ||
          (state.astack->enabled && state.astack->prev_enabled));
}

/* Check that a register file address is ok. */
void
file_ok(unsigned int file)
{
  unsigned int bank_bits;

  /* don't check address, the linker takes care of it */
  if (state.mode == MODE_RELOCATABLE) {
    return;
  }

  if (file > state.maxram) {
    gpvwarning(GPW_INVALID_RAM, "Register{%#x} > MAXRAM{%#x}", file, state.maxram);
  }
  else if (state.badram[file]) {
    gpvwarning(GPW_INVALID_RAM, "Address{%#x} in BADRAM.", file);
  }

  bank_bits = file & state.processor->bank_bits;

  /* Issue bank message if necessary. */
  if (bank_bits != 0) {
    gpvmessage(GPM_BANK, "Bank_bits = %#x, register{%#x}.", bank_bits,
               file & (~state.device.class->bank_mask));
  }
}

static void
emit_check(int insn, int argument, int mask, const char *name)
{
  int test = argument;
  int v = argument & mask;

  if (test < 0) {
    test = -test;
  }

  /* If there are bits that shouldn't be set then issue a warning. */
  if (test & (~mask)) {
    gpvwarning(GPW_RANGE, "%i (%#x) => %i (%#x)", argument, argument, v, v);
  }

  emit(insn | v, name);
}

/*
   For relative branches, issue a warning if the absolute value of
   argument is greater than range.
*/
static void
emit_check_relative(int insn, int argument, int mask, int range, const char *name)
{
  char full_message[BUFSIZ];

  /* If the branch is too far then issue an error */
  if ((argument > range) || (argument < -(range+1))) {
    snprintf(full_message,
             sizeof(full_message),
             "Argument out of range (%d not between %d and %d).",
             argument,
             -(range+1),
             range);
    gperror(GPE_RANGE, full_message);
  }

  emit(insn | (argument & mask), name);
}

static int
check_flag(int flag)
{
  if ((flag != 0) && (flag != 1)) {
    gpvwarning(GPW_RANGE, "%i (%#x)", flag, flag);
  }

  return (flag & 0x1);
}

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
check_16e_arg_types(struct pnode *parms, int arity, unsigned int types)
{
  gp_boolean ret = true;

  if (state.extended_pic16e) {
    const struct pnode *p = parms;
    int i;

    for (i = 0; i < arity; ++i) {
      assert(p != NULL);

      if ((AR_GET(types, i) != AR_INDEX) && (HEAD(p)->tag == PTAG_OFFSET)) {
        gperror(GPE_BADCHAR, "Illegal character ([).");
        ret = false;
      }
      else if (AR_BIT_BYTE & types) {
        if ((AR_GET(types, i) == AR_INDEX) && (HEAD(p)->tag == PTAG_OFFSET)) {
          const struct pnode *p1 = HEAD(p)->value.offset;

          if (can_evaluate_value(p1)) {
            int val = evaluate(p1);

            if (val < 0 || val > 0x5f) {
              char buf[64];

              snprintf(buf, sizeof(buf), "Argument out of range (%i (%#x) not between 0 and 95).", val, val);
              gperror(GPE_RANGE, buf);
              ret = false;
            }
          }
          else {
            gpverror(GPE_CONSTANT, NULL);
            ret = false;
          }
        }
      }
      else if ((AR_GET(types, i) == AR_INDEX) && (HEAD(p)->tag != PTAG_OFFSET)) {
        gpverror(GPE_MISSING_BRACKET, NULL);
        ret = false;
      }
      p = TAIL(p);
    }
  }

  return ret;
}

gpasmVal
do_insn(const char *name, struct pnode *parms)
{
  const struct symbol *s;
  int arity;
  struct pnode *p;
  int file;             /* register file address, if applicable */
  gpasmVal r;           /* Return value. */
  gp_boolean is_btfsx = false;

  /* We want to have r as the value to assign to label. */
  r = IS_RAM_ORG ? state.byte_addr : gp_processor_byte_to_org(state.device.class, state.byte_addr);

  arity = list_length(parms);

  s = get_symbol(state.stBuiltin, name);

  if (s != NULL) {
    const struct insn *i = get_symbol_annotation(s);
    enum common_insn icode;

    /* Instructions in data sections are not allowed. */
    if (asm_enabled() && (i->class != INSN_CLASS_FUNC) && IS_RAM_ORG) {
      gpverror(GPE_WRONG_SECTION, NULL);
      goto leave;
    }

    /* Interpret the instruction if assembly is enabled, or if it's a conditional. */
    if (asm_enabled() || (i->attribs & ATTRIB_COND)) {
      state.lst.line.linetype = LTY_INSN;
      icode = i->icode;

      switch (i->class) {
      case INSN_CLASS_LIT3_BANK:
        /* SX bank */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, (reloc_evaluate(p, RELOCT_F) >> 5), SX_BMSK_BANK, s->name);
        }
        break;

      case INSN_CLASS_LIT3_PAGE:
        /* SX page */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, (reloc_evaluate(p, RELOCT_F) >> 9), SX_BMSK_PAGE, s->name);
        }
        break;

      case INSN_CLASS_LIT1:
        /* PIC16E (retfie, return) */
        {
          int flag = 0;

          check_16e_arg_types(parms, arity, 0);

          switch (arity) {
          case 1:
            flag = check_flag(reloc_evaluate(HEAD(parms), RELOCT_F));
          case 0:
            emit(i->opcode | flag, s->name);
            break;

          default:
            enforce_arity(arity, 1);
          }
        }
        break;

      case INSN_CLASS_LIT3:
        /* PIC12E movlb */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, reloc_evaluate(p, RELOCT_MOVLB), PIC12E_BMSK_BANK, s->name);
        }
        break;

      case INSN_CLASS_LIT4:
        /* SX mode */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, reloc_evaluate(p, RELOCT_F), SX_BMSK_MODE, s->name);
        }
        break;

      case INSN_CLASS_LIT4L:
	/* PIC16E movlb */
        if (enforce_arity(arity, 1)) {
          check_16e_arg_types(parms, arity, 0);
          p = HEAD(parms);
          coerce_str1(p); /* literal instructions can coerce string literals */
          emit_check(i->opcode, reloc_evaluate(p, RELOCT_MOVLB), PIC16E_BMSK_MOVLB, s->name);
        }
        break;

      case INSN_CLASS_LIT4H:
        /* PIC16 movlr */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, (reloc_evaluate(p, RELOCT_MOVLR) << 4), PIC16_BMSK_MOVLR, s->name);
        }
        break;

      case INSN_CLASS_LIT5:
        /* PIC14E movlb */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, reloc_evaluate(p, RELOCT_MOVLB), PIC14E_BMSK_MOVLB, s->name);
        }
        break;

      case INSN_CLASS_LIT6:
        /* PIC16E (addulnk, subulnk) */
        if (enforce_arity(arity, 1)) {
          check_16e_arg_types(parms, arity, 0);
          p = HEAD(parms);
          /* The literal cannot be a relocatable address. */
          emit_check(i->opcode, maybe_evaluate(p), PIC16EX_BMSK_xxxULNK, s->name);
        }
        break;

      case INSN_CLASS_LIT7:
        /* PIC14E movlp */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, reloc_evaluate(p, RELOCT_PAGESEL_MOVLP), PIC14E_BMSK_PAGE512, s->name);
        }
        break;

      case INSN_CLASS_LIT8:
        /* PIC1xx (addlw, andlw, iorlw, movlw, retlw, sublw, xorlw),
	   PIC16  movlb,
	   PIC16x mullw,
	   PIC16E pushl */
        if (enforce_arity(arity, 1)) {
          check_16e_arg_types(parms, arity, 0);

          p = HEAD(parms);
          coerce_str1(p); /* literal instructions can coerce string literals */

          if (icode == ICODE_MOVLB) {
            emit_check(i->opcode, reloc_evaluate(p, RELOCT_MOVLB), PIC16_BMSK_MOVLB, s->name);
          }
          else {
            emit_check(i->opcode, reloc_evaluate(p, RELOCT_LOW), 0xff, s->name);
          }
        }
        break;

      case INSN_CLASS_LIT8C12:
        /* PIC12x call, SX call */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);

          if ((state.obj.new_sec_flags & STYP_ABS) && can_evaluate_value(p)) {
            int value = evaluate(p);

            /* PC is 11 bits.  mpasm checks the maximum device address. */
            if (value & (~PIC12_PC_MASK)) {
              gpverror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC12_PC_MASK);
            }

            if ((value & PIC12_PAGE_BITS) != (r & PIC12_PAGE_BITS)) {
              gpvmessage(GPM_PAGE, "%#x != %#x", (value & PIC12_PAGE_BITS), (r & PIC12_PAGE_BITS));
            }

            if (value & 0x100) {
              gpverror(GPE_BAD_CALL_ADDR, "%i (%#x)", value, value);
            }
          }

          emit(i->opcode | (reloc_evaluate(p, RELOCT_CALL) & PIC12_BMSK_CALL), s->name);
        }
        break;

      case INSN_CLASS_LIT8C16:
        /* PIC16 lcall */
        if (enforce_arity(arity, 1)) {
          int value;

          p = HEAD(parms);
          value = reloc_evaluate(p, RELOCT_LOW);

          /* PC is 16 bits.  mpasm checks the maximum device address. */
          if (value & (~0xffff)) {
            gpverror(GPE_RANGE, "Address{%#x} > 0xffff.", value);
          }

          emit(i->opcode | (value & 0xff), s->name);
        }
        break;

      case INSN_CLASS_LIT9:
        /* PIC12 goto, SX goto */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);

          if ((state.obj.new_sec_flags & STYP_ABS) && can_evaluate_value(p)) {
            int value = evaluate(p);

            /* PC is 11 bits.  mpasm checks the maximum device address. */
            if (value & (~PIC12_PC_MASK)) {
              gpverror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC12_PC_MASK);
            }

            if ((value & PIC12_PAGE_BITS) != (r & PIC12_PAGE_BITS)) {
              gpvmessage(GPM_PAGE, "%#x != %#x", (value & PIC12_PAGE_BITS), (r & PIC12_PAGE_BITS));
            }
          }

          emit(i->opcode | (reloc_evaluate(p, RELOCT_GOTO) & PIC12_BMSK_GOTO), s->name);
        }
        break;

      case INSN_CLASS_LIT11:
        /* PIC14x (call, goto) */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);

          if ((state.obj.new_sec_flags & STYP_ABS) && can_evaluate_value(p)) {
            int value = evaluate(p);

            if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
              /* PC is 15 bits.  mpasm checks the maximum device address. */
              if (value & (~PIC14E_PC_MASK)) {
                gpverror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC14E_PC_MASK);
              }

              if ((value & PIC14E_PAGE_BITS) != (r & PIC14E_PAGE_BITS)) {
                gpvmessage(GPM_PAGE, "%#x != %#x", (value & PIC14E_PAGE_BITS), (r & PIC14E_PAGE_BITS));
              }
            }
            else {
              /* PC is 13 bits.  mpasm checks the maximum device address. */
              if (value & (~PIC14_PC_MASK)) {
                gpverror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC14_PC_MASK);
              }

              if ((value & PIC14_PAGE_BITS) != (r & PIC14_PAGE_BITS)) {
                gpvmessage(GPM_PAGE, "%#x != %#x", (value & PIC14_PAGE_BITS), (r & PIC14_PAGE_BITS));
              }
            }
          }

          emit(i->opcode |
	           (reloc_evaluate(p, (icode == ICODE_CALL) ? RELOCT_CALL : RELOCT_GOTO) &
	               PIC14_BMSK_BRANCH),
	       s->name);
        }
        break;

      case INSN_CLASS_LIT13:
        /* PIC16 (call, goto) */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);

          if ((state.obj.new_sec_flags & STYP_ABS) && can_evaluate_value(p)) {
            int value = evaluate(p);

            /* PC is 16 bits.  mpasm checks the maximum device address. */
            if (value & (~PIC16_PC_MASK)) {
              gpverror(GPE_RANGE, "Address{%#x} > %#x.", value, PIC16_PC_MASK);
            }

            if ((value & PIC16_PAGE_BITS) != (r & PIC16_PAGE_BITS)) {
              gpvmessage(GPM_PAGE, "%#x != %#x", (value & PIC16_PAGE_BITS), (r & PIC16_PAGE_BITS));
            }
          }

          emit(i->opcode |
	           (reloc_evaluate(p, (icode == ICODE_CALL) ? RELOCT_CALL : RELOCT_GOTO) &
		       PIC16_BMSK_BRANCH),
	       s->name);
        }
        break;

      case INSN_CLASS_LITFSR_14:
        /* PIC14E addfsr */
        if (enforce_arity(arity, 2)) {
          int value;
          int fsr;

          p = HEAD(parms);
          fsr = maybe_evaluate(p);

          if ((fsr == PIC14E_REG_FSR0) || (fsr == PIC14E_REG_FSR1)) {
            fsr = (fsr == PIC14E_REG_FSR1) ? 0x40 : 0x00;
            p = HEAD(TAIL(parms));
            /* the offset cannot be a relocatable address */
            value = maybe_evaluate(p);

            if (value < -32) {
              gpverror(GPE_RANGE, "%i < -32", value);
            }
            else if (value > 31) {
              gpverror(GPE_RANGE, "%i > 31", value);
            }

            emit(i->opcode | fsr | (value & 0x3f), s->name);
          }
          else {
            gpverror(GPE_RANGE, "FSR = %i.", fsr);
          }
        }
        break;

      case INSN_CLASS_LITFSR_16:
        /* PIC16E (addfsr, subfsr) */
        if (enforce_arity(arity, 2)) {
          int value;
          int fsr;

          check_16e_arg_types(parms, arity, 0);

          p = HEAD(parms);
          fsr = maybe_evaluate(p);

          if (fsr < 0) {
            gpverror(GPE_RANGE, "FSR{%i} < 0.", fsr);
          }
          else if (fsr > 2) {
            gpverror(GPE_RANGE, "FSR{%i} > 2.", fsr);
          }

          p = HEAD(TAIL(parms));
          /* the offset cannot be a relocatable address */
          value = maybe_evaluate(p);

          if (value & (~0x3f)) {
            gpverror(GPE_RANGE, "%i (%#x) > 0x3f", value, value);
          }

          emit(i->opcode | ((fsr & 0x3) << 6) | (value & 0x3f), s->name);
        }
        break;

      case INSN_CLASS_RBRA8:
        /* PIC16E (bc, bn, bnc, bnn, bnov, bnz, bov, bz) */
        if (enforce_arity(arity, 1)) {
          int offset;

          check_16e_arg_types(parms, arity, 0);

          p = HEAD(parms);

          if (count_reloc(p) == 0) {
            offset = maybe_evaluate(p) - (r + 2);

            if (offset & 1) {
              gpvwarning(GPW_WORD_ALIGNED, "Offset{%i (%#x)}", offset, offset);
            }
          }
          else {
            offset = reloc_evaluate(p, RELOCT_CONDBRA);
          }

          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -127 <= offset <= 127. */
          emit_check_relative(i->opcode, offset, PIC16E_BMSK_RBRA8, 127, s->name);
        }
        break;

      case INSN_CLASS_RBRA9:
        /* PIC14E bra */
        if (enforce_arity(arity, 1)) {
          int offset;

          p = HEAD(parms);

          if (count_reloc(p) == 0) {
            offset = maybe_evaluate(p) - (r + 1);
          }
          else {
            offset = reloc_evaluate(p, RELOCT_BRA);
          }

          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -256 <= offset <= 255. */
          emit_check_relative(i->opcode, offset, PIC14E_BMSK_RBRA9, 255, s->name);
        }
        break;

      case INSN_CLASS_RBRA11:
        /* PIC16E (bra, rcall) */
        if (enforce_arity(arity, 1)) {
          int offset;

          check_16e_arg_types(parms, arity, 0);

          p = HEAD(parms);

          if (count_reloc(p) == 0) {
            offset = maybe_evaluate(p) - (r + 2);

            if (offset & 1) {
              gpvwarning(GPW_WORD_ALIGNED, "%i (%#x)", offset, offset);
            }
          }
          else {
            offset = reloc_evaluate(p, RELOCT_BRA);
          }

          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -1024 <= offset <= 1023. */
          emit_check_relative(i->opcode, offset, PIC16E_BMSK_RBRA11, 1023, s->name);
        }
        break;

      case INSN_CLASS_LIT20:
        /* PIC16E goto */
        if (enforce_arity(arity, 1)) {
          int dest;

          check_16e_arg_types(parms, arity, 0);

          p = HEAD(parms);
          dest = reloc_evaluate(p, RELOCT_GOTO);
          dest = gp_processor_org_to_byte(state.device.class, dest) >> 1;
          emit(i->opcode | (dest & PIC16E_BMSK_BRANCH_LOWER), s->name);
          reloc_evaluate(p, RELOCT_GOTO2);     /* add the second relocation */
          emit_check(0xf000, dest >> 8, PIC16E_BMSK_BRANCH_HIGHER, s->name);
        }
        break;

      case INSN_CLASS_CALL20:
        /* PIC16E call */
        {
          int dest;
          int flag = 0; /* By default, fast push is not used. */
          struct pnode *p2; /* second parameter */

          if (arity < 1) {
            enforce_arity(arity, 2);
          }
          else {
            check_16e_arg_types(parms, arity, 0);

            p = HEAD(parms);

            switch (arity) {
            case 2:
              p2 = HEAD(TAIL(parms));
              /* Allow "s" for fast push */
              if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "s") == 0)) {
                flag = 1;
              }
              else {
                flag = check_flag(maybe_evaluate(p2));
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
            emit(i->opcode | (flag << 8) | (dest & PIC16E_BMSK_BRANCH_LOWER), s->name);
            reloc_evaluate(p, RELOCT_CALL2);     /* add the second relocation */
            emit_check(0xf000, (dest >> 8), PIC16E_BMSK_BRANCH_HIGHER, s->name);
          }
        }
        break;

      case INSN_CLASS_FLIT12:
        /* PIC16E lfsr */
        {
          if (enforce_arity(arity, 2)) {
            int k;

            check_16e_arg_types(parms, arity, 0);

            p = HEAD(parms);
            file = maybe_evaluate(p);

            if (file > 3) {
              gperror(GPE_UNKNOWN, "FSR is out of range.");
            }

            p = HEAD(TAIL(parms));
            k = reloc_evaluate(p, RELOCT_LFSR1);
            emit_check(i->opcode | ((file & 3) << 4), (k >> 8), 0xf, s->name);
            reloc_evaluate(p, RELOCT_LFSR2); /* add the second relocation */
            emit(0xf000 | (k & 0xff), s->name);
          }
        }
        break;

      case INSN_CLASS_FF:
        /* PIC16E movff */
        if (enforce_arity(arity, 2)) {
          int dest;

          check_16e_arg_types(parms, arity, 0);

          dest = maybe_evaluate(HEAD(TAIL(parms)));

          /* The destination can't be the PCL, TOSU, TOSH or TOSL. */
          if (dest == PIC16E_REG_PCL) {
            gperror(GPE_UNKNOWN, "The destination cannot be the PCL.");
          }
          else if (dest == PIC16E_REG_TOSU) {
            gperror(GPE_UNKNOWN, "The destination cannot be the TOSU.");
          }
          else if (dest == PIC16E_REG_TOSH) {
            gperror(GPE_UNKNOWN, "The destination cannot be the TOSH.");
          }
          else if (dest == PIC16E_REG_TOSL) {
            gperror(GPE_UNKNOWN, "The destination cannot be the TOSL.");
          }

          emit_check(i->opcode, reloc_evaluate(HEAD(parms), RELOCT_FF1), 0xfff, s->name);
          emit_check(0xf000, reloc_evaluate(HEAD(TAIL(parms)), RELOCT_FF2), 0xfff, s->name);
        }
        break;

      case INSN_CLASS_FP:
        /* PIC16 movfp */
        if (enforce_arity(arity, 2)) {
          int reg = 0;

          file = reloc_evaluate(HEAD(parms), RELOCT_F);
          reg = reloc_evaluate(HEAD(TAIL(parms)), RELOCT_P);
          file_ok(file);

          if (reg & ~0xf1f) {
            gpvwarning(GPW_RANGE, "(%#x & ~0xf1f) != 0", reg, reg);
          }

          emit(i->opcode | ((reg & 0x1f) << 8) | (file & PIC16_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_PF:
        /* PIC16 movpf */
        if (enforce_arity(arity, 2)) {
          int reg = 0;

          file = reloc_evaluate(HEAD(TAIL(parms)), RELOCT_F);
          reg = reloc_evaluate(HEAD(parms), RELOCT_P);
          file_ok(file);

          if (reg & ~0xf1f) {
            gpvwarning(GPW_RANGE, "(%#x & ~0xf1f) != 0", reg, reg);
          }

          emit(i->opcode | ((reg & 0x1f) << 8) | (file & PIC16_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_SF:
        /* PIC16E movsf */
        if (enforce_arity(arity, 2)) {
          int source;
          int dest;

          check_16e_arg_types(parms, arity, AR_INDEX);

          p = HEAD(parms);
          source = maybe_evaluate(p);

          p = HEAD(TAIL(parms));
          dest = maybe_evaluate(p);

          /* The destination can't be the PCL, TOSU, TOSH or TOSL. */
          if (dest == PIC16E_REG_PCL) {
            gperror(GPE_UNKNOWN, "The destination cannot be the PCL.");
          }
          else if (dest == PIC16E_REG_TOSU) {
            gperror(GPE_UNKNOWN, "The destination cannot be the TOSU.");
          }
          else if (dest == PIC16E_REG_TOSH) {
            gperror(GPE_UNKNOWN, "The destination cannot be the TOSH.");
          }
          else if (dest == PIC16E_REG_TOSL) {
            gperror(GPE_UNKNOWN, "The destination cannot be the TOSL.");
          }

          emit_check(i->opcode, source, 0x7f, s->name);
          emit_check(0xf000, reloc_evaluate(HEAD(TAIL(parms)), RELOCT_FF2), 0xfff, s->name);
        }
        break;

      case INSN_CLASS_SS:
        /* PIC16E movss */
        if (enforce_arity(arity, 2)) {
          int source;
          int dest;

          check_16e_arg_types(parms, arity, (AR_INDEX << AR_BITS) | AR_INDEX);

          p = HEAD(parms);
          source = maybe_evaluate(p);

          p = HEAD(TAIL(parms));
          dest = maybe_evaluate(p);

          emit_check(i->opcode, source, 0x7f, s->name);
          emit_check(0xf000, dest, 0x7f, s->name);
        }
        break;

      case INSN_CLASS_OPF3:
        /* PIC12 tris */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          file = reloc_evaluate(p, RELOCT_TRIS_3BIT);
          file_ok(file);
          emit(i->opcode | (file & PIC12_BMSK_TRIS), s->name);
        }
        break;

      case INSN_CLASS_OPF5:
        /* {PIC12x, SX} (clrf, movwf), SX tris */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);

          if (IS_SX_CORE && (icode == ICODE_TRIS)) {
            file = reloc_evaluate(p, RELOCT_TRIS);
          }
          else {
            file = reloc_evaluate(p, RELOCT_F);
          }

          file_ok(file);
          emit(i->opcode | (file & PIC12_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_OPWF5:
        /* {PIC12x, SX} (addwf, andwf, comf, decf, decfsz, incf, incfsz,
                         iorwf, movf, rlf, rrf, subwf, swapf, xorwf) */
        {
          int d = 1; /* Default destination of 1 (file). */
          struct pnode *p2; /* second parameter */

          if (arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          p = HEAD(parms);
          switch (arity) {
          case 2:
            p2 = HEAD(TAIL(parms));
            /* Allow "w" and "f" as destinations. */
            if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "f") == 0)) {
              d = 1;
            }
            else if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "w") == 0)) {
              d = 0;
            }
            else {
              d = check_flag(maybe_evaluate(p2));
            }
            break;

          case 1:
            d = 1;
            gpvmessage(GPM_NOF, NULL);
            break;

          default:
            enforce_arity(arity, 2);
          }

          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          emit(i->opcode | (d << 5) | (file & PIC12_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_B5:
        /* {PIC12x, SX} (bcf, bsf, btfsc, btfss) */
        {
          struct pnode *f, *b;
          int bit;

          if (enforce_arity(arity, 2)) {
            f = HEAD(parms);
            b = HEAD(TAIL(parms));
            file = reloc_evaluate(f, RELOCT_F);
            bit = maybe_evaluate(b);

            if (bit < 0) {
              gpvwarning(GPW_RANGE, "Bit{%i} < 0.", bit);
            }
            else if (bit > 7) {
              gpvwarning(GPW_RANGE, "Bit{%i} > 7.", bit);
            }

            file_ok(file);

            if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
              is_btfsx = true;
            }

            emit(i->opcode | ((bit & 7) << 5) | (file & PIC12_BMSK_FILE), s->name);
          }
        }
        break;

      case INSN_CLASS_B8:
        /* PIC16 (bcf, bsf, btfsc, btfss, btg) */
        {
          struct pnode *f, *b;
          int bit;

          if (enforce_arity(arity, 2)) {
            f = HEAD(parms);
            b = HEAD(TAIL(parms));
            file = reloc_evaluate(f, RELOCT_F);
            bit = maybe_evaluate(b);

            if (bit < 0) {
              gpvwarning(GPW_RANGE, "Bit{%i} < 0.", bit);
            }
            else if (bit > 7) {
              gpvwarning(GPW_RANGE, "Bit{%i} > 7.", bit);
            }

            file_ok(file);

            if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
              is_btfsx = true;
            }

            emit(i->opcode | ((bit & 7) << 8) | (file & PIC16_BMSK_FILE), s->name);
          }
        }
        break;

      case INSN_CLASS_OPF7:
        /* PIC14x (clrf, movwf, tris) */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);

          if (icode == ICODE_TRIS) {
            gpvwarning(GPW_NOT_RECOMMENDED, "\"tris\"");
            file = reloc_evaluate(p, RELOCT_TRIS);
          }
          else {
            file = reloc_evaluate(p, RELOCT_F);
          }

          file_ok(file);
          emit(i->opcode | (file & PIC14_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_OPF8:
        /* PIC16 (cpfseq, cpfsgt, cpfslt, movwf, mulwf, tstfsz) */
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          emit(i->opcode | (file & PIC16_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_OPWF7:
        /* PIC14x (addwf, andwf, comf, decf, decfsz, incf, incfsz, iorwf, movf,
                   rlf, rrf, subwf, swapf, xorwf)
           PIC14E (addwfc, asrf, lslf, lsrf, subwfb) */
        {
          int d = 1; /* Default destination of 1 (file). */
          struct pnode *p2; /* second parameter */

          if (arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          p = HEAD(parms);
          switch (arity) {
          case 2:
            p2 = HEAD(TAIL(parms));
            /* Allow "w" and "f" as destinations. */
            if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "f") == 0)) {
              d = 1;
            }
            else if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "w") == 0)) {
              d = 0;
            }
            else {
              d = check_flag(maybe_evaluate(p2));
            }
            break;

          case 1:
            d = 1;
            gpvmessage(GPM_NOF, NULL);
            break;

          default:
            enforce_arity(arity, 2);
          }

          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          emit(i->opcode | (d << 7) | (file & PIC14_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_OPWF8:
        /* PIC16 (addwf, addwfc, andwf, clrf, comf, daw, decf, decfsz, dcfsnz, incf,
                  incfsz, infsnz, iorwf, rlcf, rlncf, rrcf, rrncf, setf, subwf, subwfb,
                  swapf, xorwf) */
        {
          int d = 1; /* Default destination of 1 (file). */
          struct pnode *p2; /* second parameter */

          if (arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          p = HEAD(parms);
          switch (arity) {
          case 2:
            p2 = HEAD(TAIL(parms));
            /* Allow "w" and "f" as destinations. */
            if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "f") == 0)) {
              d = 1;
            }
            else if ((p2->tag == PTAG_SYMBOL) && (strcasecmp(p2->value.symbol, "w") == 0)) {
              d = 0;
            }
            else {
              d = check_flag(maybe_evaluate(p2));
            }
            break;

          case 1:
            d = 1;
            gpvmessage(GPM_NOF, NULL);
            break;

          default:
            enforce_arity(arity, 2);
          }

          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          emit(i->opcode | (d << 8) | (file & PIC16_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_B7:
        /* PIC14x (bcf, bsf, btfsc, btfss) */
        {
          struct pnode *f, *b;
          int bit;

          if (enforce_arity(arity, 2)) {
            f = HEAD(parms);
            b = HEAD(TAIL(parms));
            file = reloc_evaluate(f, RELOCT_F);
            bit = maybe_evaluate(b);

            if (bit < 0) {
              gpvwarning(GPW_RANGE, "Bit{%i} < 0.", bit);
            }
            else if (bit > 7) {
              gpvwarning(GPW_RANGE, "Bit{%i} > 7.", bit);
            }

            file_ok(file);

            if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
              is_btfsx = true;
            }

            emit(i->opcode | ((bit & 7) << 7) | (file & PIC14_BMSK_FILE), s->name);
          }
        }
        break;

      case INSN_CLASS_OPFA8:
        /* PIC16E (clrf, cpfseq, cpfsgt, cpfslt, movwf, mulwf, negf, setf, tstfsz) */
        {
          int a = 0; /* Default destination of 0 (access). */
          gp_boolean isAccess = false;
          struct pnode *par; /* second parameter */

          if (arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          check_16e_arg_types(parms, arity, AR_BIT_BYTE | AR_INDEX);

          p = HEAD(parms);
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
            par = HEAD(TAIL(parms));
            /* Prohibit "a" for BSR to select RAM bank. */
            if ((!state.mpasm_compatible) && (par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "a") == 0)) {
              a = 0;
            }
            /* Allow "b" for BSR to select RAM bank. */
            else if ((par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "b") == 0)) {
              a = 1;
            }
            else {
              a = check_flag(maybe_evaluate(par));
            }
            break;

          case 1:
            /* use default a */
            if ((!state.mpasm_compatible) && (state.strict_level > 0)) {
              if (isAccess) {
                if (state.strict_level == 2) {
                  gpverror(GPE_NOA, NULL);
                }
                else {
                  gpvmessage(GPM_NOA, NULL);
                }
              }
              else {
                gpvmessage(GPM_BANK, NULL);
              }
            }
            break;

          default:
            enforce_arity(arity, 2);
          }

          emit(i->opcode | (a << 8) | (file & PIC16_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_BA8:
        /* PIC16E (bcf, bsf, btfsc, btfss, btg) */
        {
          struct pnode *f, *b, *par;
          int bit, a = 0;
          gp_boolean isAccess = false;

          if ((arity != 2) && (arity != 3)) {
            enforce_arity(arity, 3);
            break;
          }

          check_16e_arg_types(parms, arity, AR_BIT_BYTE | AR_INDEX);

          f = HEAD(parms);
          file = reloc_evaluate(f, RELOCT_F);

          if (arity == 3) {
            par = HEAD(TAIL(TAIL(parms)));

            /* Prohibit "a" for BSR to select RAM bank. */
            if ((!state.mpasm_compatible) && (par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "a") == 0)) {
              a = 0;
            }
            /* Allow "b" for BSR to select RAM bank. */
            else if ((par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "b") == 0)) {
              a = 1;
            }
            else {
              a = check_flag(maybe_evaluate(par));
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
                  gpverror(GPE_NOA, NULL);
                }
                else {
                  gpvmessage(GPM_NOA, NULL);
                }
              }
              else {
                gpvmessage(GPM_BANK, NULL);
              }
            }
          }

          /* add relocation for the access bit, if necessary */
          if (arity < 3) {
            reloc_evaluate(f, RELOCT_ACCESS);
          }

          file_ok(file);

          b = HEAD(TAIL(parms));
          bit = maybe_evaluate(b);

          if (bit < 0) {
            gpvwarning(GPW_RANGE, "Bit{%i} < 0.", bit);
          }
          else if (bit > 7) {
            gpvwarning(GPW_RANGE, "Bit{%i} > 7.", bit);
          }

          if ((icode == ICODE_BTFSC) || (icode == ICODE_BTFSS)) {
            is_btfsx = true;
          }

          emit(i->opcode | (a << 8) | ((bit & 7) << 9) | (file & PIC16_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_OPWFA8:
        /* PIC16E (addwf, addwfc, andwf, comf, decf, decfsz, dcfsnz, incf, incfsz,
                   infsnz, iorwf, movf, rlcf, rlncf, rrcf, rrncf, subfwb, subwf,
                   subwfb, swapf, xorwf) */
        {
          int d = 1; /* Default destination of 1 (file). */
          int a = 0;
          gp_boolean isAccess = false;
          gp_boolean thereIsA = false;
          gp_boolean thereIsD = false;
          struct pnode *par; /* second parameter */

          if (arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          check_16e_arg_types(parms, arity, AR_BIT_BYTE | AR_INDEX);

          p = HEAD(parms);
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
            par = HEAD(TAIL(TAIL(parms)));

            /* Prohibit "a" for BSR to select RAM bank. */
            if ((!state.mpasm_compatible) && (par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "a") == 0)) {
              a = 0;
            }
            /* Allow "b" for BSR to select RAM bank. */
            else if ((par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "b") == 0)) {
              a = 1;
            }
            else {
              a = check_flag(maybe_evaluate(par));
            }

            thereIsA = true;

            /* fall through */
          case 2:
            par = HEAD(TAIL(parms));
            /* Allow "w" and "f" as destinations. */
            if ((par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "f") == 0)) {
              d = 1;
            }
            else if ((par->tag == PTAG_SYMBOL) && (strcasecmp(par->value.symbol, "w") == 0)) {
              d = 0;
            }
            else {
              d = check_flag(maybe_evaluate(par));
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
                gpverror(GPE_NOF, NULL);
              }
              else {
                gpvmessage(GPM_NOF, NULL);
              }
            }

            if (!thereIsA) {
              if (isAccess) {
                if (state.strict_level == 2) {
                  gpverror(GPE_NOA, NULL);
                }
                else {
                  gpvmessage(GPM_NOA, NULL);
                }
              }
              else {
                gpvmessage(GPM_BANK, NULL);
              }
            }
          }

          emit(i->opcode | (d << 9) | (a << 8) | (file & PIC16_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_IMPLICIT:
        /* PIC12x  (clrw, clrwdt, nop, option, return, sleep)
           PIC12E  (retfie, return)
           SX      (iread, movmw, movwm, reti, retiw, retp, return)
           PIC14x  (clrw, clrwdt, halt, nop, option, retfie, return, sleep)
           PIC14E  (brw, callw, reset)
           PIC16   (clrwdt, nop, retfie, return, sleep)
           PIC16E  (clrwdt, daw, halt, nop, pop, push, reset, sleep, trap, tret)
           PIX16EX callw */
        if (arity != 0) {
          gpvwarning(GPW_EXTRANEOUS, NULL);
        }

        if ((icode == ICODE_OPTION) && (state.device.class->core_mask != CORE_12BIT_MASK)){
          gpvwarning(GPW_NOT_RECOMMENDED, "\"option\"");
        }

        emit(i->opcode, s->name);
        break;

      case INSN_CLASS_TBL:
        /* PIC16E (tblrd, tblwt) */
        if (enforce_arity(arity, 1)) {
          check_16e_arg_types(parms, arity, 0);

          p = HEAD(parms);
          switch(maybe_evaluate(p)) {
          case TBL_NO_CHANGE:
            emit(i->opcode, s->name);
            break;

          case TBL_POST_INC:
            emit(i->opcode | 1, s->name);
            break;

          case TBL_POST_DEC:
            emit(i->opcode | 2, s->name);
            break;

          case TBL_PRE_INC:
            emit(i->opcode | 3, s->name);
            break;

          default:
            gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
          }
        }
        break;

      case INSN_CLASS_TBL2:
        /* PIC16 (tlrd, tlwt) */
        if (enforce_arity(arity, 2)) {
          int t = 0; /* read low byte by default */
          struct pnode *p2; /* second parameter */

          /* "0" (lower byte) and "1" (upper byte) */
          p = HEAD(parms);
          t = check_flag(maybe_evaluate(p));

          p2 = HEAD(TAIL(parms));
          file = reloc_evaluate(p2, RELOCT_F);

          file_ok(file);
          emit(i->opcode | (t << 9) | (file & PIC16_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_TBL3:
      /* PIC16 (tablrd, tablwt) */
        if (enforce_arity(arity, 3)) {
          int inc = 0, t = 0;
          struct pnode *p2; /* second parameter */
          struct pnode *p3; /* third parameter */

          /* "0" (lower byte) and "1" (upper byte) */
          p = HEAD(parms);
          t = check_flag(maybe_evaluate(p));

          /* "0" (no change) and "1" (postincrement) */
          p2 = HEAD(TAIL(parms));
          inc = check_flag(maybe_evaluate(p2));

          p3 = HEAD(TAIL(TAIL(parms)));
          file = reloc_evaluate(p3, RELOCT_F);

          file_ok(file);
          emit(i->opcode | (t << 9) | (inc << 8) | (file & PIC16_BMSK_FILE), s->name);
        }
        break;

      case INSN_CLASS_MOVINDF:
        /* PIC14E (moviw, movwi) */
        if (arity == 0) {
          enforce_arity(arity, 1);
          break;
        }

        p = HEAD(parms);
        switch (arity) {
          int fsr;
          int opcode;
          int k;
          struct pnode *p2, *p3;

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
            emit(opcode, s->name);
          }
          else {
            gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
          }
          break;

        case 2:
          p2 = HEAD(TAIL(parms));
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
              emit(i->opcode | file | k, s->name);
            }
            else {
              gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
            }
          }
          else {
            gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
          }
          break;

        case 3:
        case 4:
          p2 = HEAD(TAIL(parms));
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
              p3 = TAIL(TAIL(parms));
              k = maybe_evaluate(p3); /* Index */

              if (k < -32) {
                gpverror(GPE_RANGE, "Index{%i} < -32.", k);
              }
              else if (k > 31) {
                gpverror(GPE_RANGE, "Index{%i} > 31.", k);
              }
              else {
                /* New opcode for indexed indirect. */
                opcode = ((icode == ICODE_MOVIW) ? PIC14E_INSN_MOVIW_IDX : PIC14E_INSN_MOVWI_IDX) | file;
                emit(opcode | (k & 0x3f), s->name);
              }
              break;

            default:
              gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
            }
          }
          else {
            gperror(GPE_ILLEGAL_ARGU, "Illegal argument.");
          }
        } /* switch (arity) */
        break;

      case INSN_CLASS_FUNC:
        r = (*i->doer)(r, name, arity, parms);
        break;

      default:
        assert(0);
        break;
      } /* switch (i->class) */
    } /* if (asm_enabled() || (i->attribs & ATTRIB_COND)) */
  } /* if (s != NULL) */
  else {
    s = get_symbol(state.stMacros, name);

    if (s != NULL) {
      struct macro_head *h = get_symbol_annotation(s);

      /* Found the macro: execute it */
      if (asm_enabled()) {
        if ((h->defined == false) && (state.pass == 2)) {
          gperror(GPE_UNKNOWN, "Forward references to macros are not allowed.");
        }
        else {
          setup_macro(h, arity, parms);
          state.preproc.do_emit = false;
        }
      }
    }
    else {
      if (asm_enabled()) {
        if (!state.processor_chosen) {
          gpverror(GPE_UNDEF_PROC, NULL);
        }
        else {
          char mesg[80];

          snprintf(mesg, sizeof(mesg), "Unknown opcode: \"%s\"", name);
          gperror(GPE_UNKNOWN, mesg);
        }
      }
    }
  }

leave:
  prev_btfsx = is_btfsx;

  return r;
}

/************************************************************************/

/* There are several groups of operations that we handle here.  First
   is op_0: the instructions that can happen before the processor type
   is known.  Second is op_1, the set of instructions that are common
   to all processors, third is processor-family specific: op_XXX */

/* Note that instructions within each group are sorted alphabetically. */

const struct insn op_0[] = {
  { "access_ovr", 0, 0, 0, INSN_CLASS_FUNC, 0,           do_access_ovr },
  { "bcdirect",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_direct     },
  { "code",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_code       },
  { "code_pack",  0, 0, 0, INSN_CLASS_FUNC, 0,           do_code_pack  },
  { "constant",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_constant   },
  { "else",       0, 0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_else       },
  { "end",        0, 0, 0, INSN_CLASS_FUNC, 0,           do_end        },
  { "endif",      0, 0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_endif      },
  { "endm",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_endm       },
  { "endw",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_endw       },
  { "equ",        0, 0, 0, INSN_CLASS_FUNC, 0,           do_equ        },
  { "error",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_error      },
  { "exitm",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_exitm      },
  { "expand",     0, 0, 0, INSN_CLASS_FUNC, 0,           do_expand     },
  { "extern",     0, 0, 0, INSN_CLASS_FUNC, 0,           do_extern     },
  { "errorlevel", 0, 0, 0, INSN_CLASS_FUNC, 0,           do_errlvl     },
  { "global",     0, 0, 0, INSN_CLASS_FUNC, 0,           do_global     },
  { "idata",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_idata      },
  { "idata_acs",  0, 0, 0, INSN_CLASS_FUNC, 0,           do_idata_acs  },
  { "if",         0, 0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_if         },
  { "ifdef",      0, 0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_ifdef      },
  { "ifndef",     0, 0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_ifndef     },
  { "include",    0, 0, 0, INSN_CLASS_FUNC, 0,           do_include    },
  { "list",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_list       },
  { "local",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_local      },
  { "macro",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_macro      },
  { "messg",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_messg      },
  { "noexpand",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_noexpand   },
  { "nolist",     0, 0, 0, INSN_CLASS_FUNC, 0,           do_nolist     },
  { "page",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_page       },
  { "processor",  0, 0, 0, INSN_CLASS_FUNC, 0,           do_processor  },
  { "radix",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_radix      },
  { "set",        0, 0, 0, INSN_CLASS_FUNC, 0,           do_set        },
  { "space",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_space      },
  { "subtitle",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_subtitle   },
  { "title",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_title      },
  { "udata",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_udata      },
  { "udata_acs",  0, 0, 0, INSN_CLASS_FUNC, 0,           do_udata_acs  },
  { "udata_ovr",  0, 0, 0, INSN_CLASS_FUNC, 0,           do_udata_ovr  },
  { "udata_shr",  0, 0, 0, INSN_CLASS_FUNC, 0,           do_udata_shr  },
  { "variable",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_variable   },
  { "while",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_while      },
  { ".def",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_def        },
  { ".dim",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_dim        },
  { ".direct",    0, 0, 0, INSN_CLASS_FUNC, 0,           do_direct     },
  { ".eof",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_eof        },
  { ".file",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_file       },
  { ".ident",     0, 0, 0, INSN_CLASS_FUNC, 0,           do_ident      },
  { ".line",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_line       },
  { ".set",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_set        },
  { ".type",      0, 0, 0, INSN_CLASS_FUNC, 0,           do_type       },
  { "#if",        0, 0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_if         },
  { "#else",      0, 0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_else       },
  { "#endif",     0, 0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_endif      },
  { "#ifdef",     0, 0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_ifdef      },
  { "#ifndef",    0, 0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_ifndef     },
  { "#define",    0, 0, 0, INSN_CLASS_FUNC, 0,           do_define     },
  { "#undefine",  0, 0, 0, INSN_CLASS_FUNC, 0,           do_undefine   }
};

const int num_op_0 = TABLE_SIZE(op_0);

const struct insn op_1[] = {
  { "__badram",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_badram       },
  { "__badrom",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_badrom       },
  { "__config",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_config       },
  { "__fuses",    0, 0, 0, INSN_CLASS_FUNC, 0,           do_config       },
  { "__idlocs",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_idlocs       },
  { "__maxram",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_maxram       },
  { "__maxrom",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_maxrom       },
  { "bankisel",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_bankisel     },
  { "banksel",    0, 0, 0, INSN_CLASS_FUNC, 0,           do_banksel      },
  { "config",     0, 0, 0, INSN_CLASS_FUNC, 0,           do_gpasm_config },
  { "data",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_data         },
  { "da",         0, 0, 0, INSN_CLASS_FUNC, 0,           do_da           },
  { "db",         0, 0, 0, INSN_CLASS_FUNC, 0,           do_db           },
  { "de",         0, 0, 0, INSN_CLASS_FUNC, 0,           do_de           },
  { "dt",         0, 0, 0, INSN_CLASS_FUNC, 0,           do_dt           },
  { "dtm",        0, 0, 0, INSN_CLASS_FUNC, 0,           do_dtm          },
  { "dw",         0, 0, 0, INSN_CLASS_FUNC, 0,           do_dw           },
  { "fill",       0, 0, 0, INSN_CLASS_FUNC, 0,           do_fill         },
  { "idlocs",     0, 0, 0, INSN_CLASS_FUNC, 0,           do_16_idlocs    },
  { "org",        0, 0, 0, INSN_CLASS_FUNC, 0,           do_org          },
  { "pagesel",    0, 0, 0, INSN_CLASS_FUNC, 0,           do_pagesel      },
  { "pageselw",   0, 0, 0, INSN_CLASS_FUNC, 0,           do_pageselw     },
  { "res",        0, 0, 0, INSN_CLASS_FUNC, 0,           do_res          }
};

const int num_op_1 = TABLE_SIZE(op_1);

void
opcode_init(int stage)
{
  int i;
  int count = 0;
  const struct insn *base = NULL;

  switch (stage) {
  case 0:
    base = op_0;
    count = num_op_0;
    break;

  case 1:
    base = op_1;
    count = num_op_1;
    break;

  case 2:
    state.device.class = gp_processor_class(state.processor);
    base = state.device.class->instructions;
    count = (base == NULL) ? 0 : *state.device.class->num_instructions;

    if (IS_SX_CORE) {
      /* The page instruction conflicts with the page directive. */
      remove_symbol(state.stBuiltin, "page");
    }
    else if (IS_PIC16E_CORE) {
      state.device.bsr_boundary = gp_processor_bsr_boundary(state.processor);

      /* The 16_bit core special macros are encoded directly into the
       * symbol table like regular instructions. */
      for (i = 0; i < num_op_18cxx_sp; i++) {
        annotate_symbol(add_symbol(state.stBuiltin, op_18cxx_sp[i].name),
                        (void *)&op_18cxx_sp[i]);
      }

      if (state.extended_pic16e) {
        /* Some 18xx devices have an extended instruction set. */
        for (i = 0; i < num_op_18cxx_ext; i++) {
          annotate_symbol(add_symbol(state.stBuiltin, op_18cxx_ext[i].name),
                          (void *)&op_18cxx_ext[i]);
        }
      }
    }
    else if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
      for (i = 0; i < num_op_16cxx_enh; i++) {
        annotate_symbol(add_symbol(state.stBuiltin, op_16cxx_enh[i].name),
                        (void *)&op_16cxx_enh[i]);
      }
    }
    break;

  case 3:
    /* add 12 and 14 bit special macros */
    base = special;
    count = num_op_special;
    break;

  default:
    assert(0);
  }

  for (i = 0; i < count; i++) {
    annotate_symbol(add_symbol(state.stBuiltin, base[i].name), (void *)&base[i]);
  }

  switch (stage) {
  case 1:
    if (IS_PIC14E_CORE || IS_PIC14EX_CORE) {
      /* The pageselw directive not supported on pic14 enhanced devices. */
      remove_symbol(state.stBuiltin, "pageselw");
    }
    break;

  case 2:
    if (state.processor != NULL) {
      const char *name = gp_processor_name(state.processor, 0);

      /* Special case, some instructions not available on 17c42 devices. */
      if (strcmp(name, "pic17c42") == 0) {
        remove_symbol(state.stBuiltin, "mulwf");
        remove_symbol(state.stBuiltin, "movlr");
        remove_symbol(state.stBuiltin, "mullw");
      }
      /* Special case, some instructions not available on 16f5x devices. */
      else if (strcmp(name, "pic16f54") == 0 ||
               strcmp(name, "pic16f57") == 0 ||
               strcmp(name, "pic16f59") == 0) {
        remove_symbol(state.stBuiltin, "addlw");
        remove_symbol(state.stBuiltin, "sublw");
        remove_symbol(state.stBuiltin, "return");
        remove_symbol(state.stBuiltin, "retfie");
      }
      else if (strcmp(name, "sx48bd") == 0 ||
               strcmp(name, "sx52bd") == 0) {
        struct symbol *mode_sym = get_symbol(state.stBuiltin, "mode");

        if (mode_sym != NULL) {
          annotate_symbol(mode_sym, (void *)&op_sx_mode);
        }
      }
      else if (IS_PIC12E_CORE) {
        remove_symbol(state.stBuiltin, "return");
        for (i = 0; i < num_op_16c5xx_enh; i++) {
          annotate_symbol(add_symbol(state.stBuiltin, op_16c5xx_enh[i].name),
                          (void *)&op_16c5xx_enh[i]);
        }

        if (strcmp(name, "pic12f529t39a") == 0 ||
            strcmp(name, "pic12f529t48a") == 0) {
          remove_symbol(state.stBuiltin, "retfie");
          remove_symbol(state.stBuiltin, "return");
        }
      }
    }
    break;
  }
}

/************************************************************************/

void
begin_cblock(const struct pnode *c)
{
  if (asm_enabled()) {
    state.cblock_defined = true;
    state.cblock = maybe_evaluate(c);
  }
}

void
continue_cblock(void)
{
  if (!state.cblock_defined) {
    gpvmessage(GPM_CBLOCK, NULL);
  }

  state.cblock_defined = true;
}

void
cblock_expr(const struct pnode *s)
{
  if (asm_enabled()) {
    set_global(s->value.symbol, state.cblock, PERMANENT, GVT_CBLOCK);
    state.cblock++;
  }
}

void
cblock_expr_incr(const struct pnode *s, const struct pnode *incr)
{
  if (asm_enabled()) {
    set_global(s->value.symbol, state.cblock, PERMANENT, GVT_CBLOCK);
    state.cblock += maybe_evaluate(incr);
  }
}

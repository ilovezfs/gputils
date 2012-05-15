/* Implements directives, pseudo-ops and processor opcodes
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin

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

static unsigned short checkwrite(unsigned short value)
{
  unsigned short insn;

  if (state.mode == relocatable) {
    if (state.obj.section == NULL) {
      gperror(GPE_WRONG_SECTION, NULL);
      return value;
    }
    /* Don't do program memory checks for data memory */
    if (IS_RAM_ORG)
      return value;
  }

  if (state.device.class == PROC_CLASS_PIC16 && (state.org > 0x1ffff)) {
    gperror(GPE_ADDROVF, NULL);
  }
  else if (state.device.class != PROC_CLASS_PIC16E &&
           (state.org & 0x1FFFF) == 0 && (int)state.org > 0) {
    /* We cast state.org to signed int on purpose to repeat a bug from
       MPASM 5.34 and pass tb.asm testcase. */
    gperror(GPE_ADDROVF, "Address wrapped around 0");
  }

  if(value > state.device.class->core_size) {
    gpmessage(GPM_RANGE,NULL);
    value &= state.device.class->core_size;
  }

  if (0 == state.num.errors && state.device.class->i_memory_get(state.i_memory, state.org, &insn)) {
    gperror(GPE_ADDROVR, NULL);
  }

  if (state.maxrom >= 0) {
    int org = gp_processor_byte_to_org(state.device.class, state.org);
    if (org > state.maxrom) {
      gpwarning(GPW_EXCEED_ROM, NULL);
    } else {
      /* check if current org is within a bad address range */
      struct range_pair *cur_badrom;
      for (cur_badrom = state.badrom; cur_badrom != NULL;
            cur_badrom = cur_badrom->next) {
        if (org >= cur_badrom->start && org <= cur_badrom->end) {
          gpwarning(GPW_EXCEED_ROM, NULL);
          break;
        }
      }
    }
  }
  return value;
}

/* Write a word into the memory image at the current location */

static void emit(unsigned short value)
{
  /* only write the program data to memory on the second pass */
  if (state.pass == 2) {
    value = checkwrite(value);
    state.device.class->i_memory_put(state.i_memory, state.org, value);
  }

  state.org += 2;
}

static void emit_byte(unsigned char value)
{
  if (state.pass == 2) {
    unsigned char byte;

    if ((state.mode == relocatable) && (state.obj.section == NULL)) {
      gperror(GPE_WRONG_SECTION, NULL);
    }

    if (!IS_RAM_ORG) {
      if (state.device.class == PROC_CLASS_PIC16 && (state.org > 0x1ffff)) {
        gperror(GPE_ADDROVF, NULL);
      }
      else if (state.device.class != PROC_CLASS_PIC16E &&
               (state.org & 0x1FFFF) == 0 && (int)state.org > 0) {
        gperror(GPE_ADDROVF, "Address wrapped around 0");
      }

      if (0 == state.num.errors && b_memory_get(state.i_memory, state.org, &byte)) {
        gperror(GPE_ADDROVR, NULL);
      }

      if (state.maxrom >= 0) {
        int org = gp_processor_byte_to_org(state.device.class, state.org);
        if (org > state.maxrom) {
          gpwarning(GPW_EXCEED_ROM, NULL);
        } else {
          /* check if current org is within a bad address range */
          struct range_pair *cur_badrom;
          for (cur_badrom = state.badrom; cur_badrom != NULL;
               cur_badrom = cur_badrom->next) {
            if (org >= cur_badrom->start && org <= cur_badrom->end) {
              gpwarning(GPW_EXCEED_ROM, NULL);
              break;
            }
          }
        }
      }
    }

    b_memory_put(state.i_memory, state.org, value);
  }
  ++state.org;
}

static int off_or_on(struct pnode *p)
{
  int had_error = 0, ret = 0;

  if (p->tag != symbol)
    had_error = 1;
  else if (strcasecmp(p->value.symbol, "OFF") == 0)
    ret = 0;
  else if (strcasecmp(p->value.symbol, "ON") == 0)
    ret = 1;
  else
    had_error = 1;

  if (had_error)
     gperror(GPE_EXPECTED, "Expected \"ON\" or \"OFF\".");

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

static int emit_data(struct pnode *L, int char_shift)
{
  unsigned begin_org = state.org;
  struct pnode *p;

  for(; L; L = TAIL(L)) {
    const char *pc;
    p = HEAD(L);
    if (p->tag == string) {
      pc = p->value.string;
      if (state.device.class == PROC_CLASS_PIC16E &&
          !(SECTION_FLAGS & (STYP_DATA|STYP_BPACK))) {
        /* Special case of PIC16E strings in code */
        int n = 0;
        while(*pc) {
          int value;
          pc = convert_escape_chars(pc, &value);
          emit_byte(value);
          ++n;
        }
        /* Pad using zero if 16-bit numbers */
        if (n & 1) {
          emit_byte(0);
          ++n;
        }
      }
      else {
        while (*pc) {
          int value;
          unsigned short v;
          pc = convert_escape_chars(pc, &value);
          value &= 0xFF;
          /* If idata or packed and not db or de, emit one character per word */
          if (SECTION_FLAGS & (STYP_DATA|STYP_BPACK)) {
            v = value;
          }
          else {
            v = value << char_shift;
            if (*pc) {
              pc = convert_escape_chars(pc, &value);
              v |= value & 0xFF;
            }
          }
          emit(v);
        }
        /* For data and packed emit a terminating nul for strings */
        if (SECTION_FLAGS & (STYP_DATA|STYP_BPACK))
          emit(0);
      }
    }
    else if (state.device.class->core_size > 0xFF) {
      unsigned short v;
      v = reloc_evaluate(p, RELOCT_ALL);
      emit(v);
    }
    else {
      unsigned short v;
      /* FIXME: This case is for EEPROM8. Do we need the RELOCT_LOW? */
      v = reloc_evaluate(p, RELOCT_LOW);
      emit_byte(v);
    }
  }
  return state.org - begin_org;
}

/* Do the work for beginning a conditional assembly block.  Leave it
   disabled by default.  This is used by do_if, do_ifdef and
   do_ifndef. */

static void enter_if(void)
{
  struct amode *new = malloc(sizeof(*new));

  new->mode = in_then;
  new->prev = state.astack;
  if (state.astack == NULL)
    new->prev_enabled = 1;
  else
    new->prev_enabled = state.astack->enabled && state.astack->prev_enabled;
  new->enabled = 0;     /* Only the default */
  state.astack = new;
}

/* Checking that a macro definition's parameters are correct */

static int macro_parms_simple(struct pnode *parms)
{
  if (parms == NULL) {
    return 1;
  } else if (HEAD(parms)->tag != symbol) {
    gperror(GPE_ILLEGAL_ARGU, NULL);
    return 0;
  } else {
    return (macro_parms_simple(TAIL(parms)));
  }
}

static int list_symbol_member(struct pnode *M, struct pnode *L)
{
  if (L == NULL) {
    return 0;
  } else if (STRCMP(M->value.symbol, HEAD(L)->value.symbol) == 0) {
    char buf[BUFSIZ];
    snprintf(buf,
             sizeof(buf),
             "Duplicate macro parameter (%s).",
             HEAD(L)->value.symbol);
    gperror(GPE_UNKNOWN, buf);
    return 1;
  } else {
    return list_symbol_member(M, TAIL(L));
  }
}

static int macro_parms_unique(struct pnode *parms)
{
  if (parms == NULL)
    return 1;
  else
    return (!list_symbol_member(HEAD(parms), TAIL(parms)) &&
            macro_parms_unique(TAIL(parms)));
}

static int macro_parms_ok(struct pnode *parms)
{
  return (macro_parms_simple(parms) && macro_parms_unique(parms));
}

/************************************************************************/

static gpasmVal do_access_ovr(gpasmVal r,
                              char *name,
                              int arity,
                              struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name,
              ".access_ovr",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_ACCESS | STYP_BSS | STYP_OVERLAY;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name,
              ".access_ovr",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_ACCESS | STYP_ABS | STYP_BSS | STYP_OVERLAY;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_badram(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
        (p->value.binop.op == '-')) {
      int start, end;

      if (can_evaluate(p->value.binop.p0) &&
          can_evaluate(p->value.binop.p1)) {
        start = evaluate(p->value.binop.p0);
        end = evaluate(p->value.binop.p1);

        if ((end < start) ||
            (start < 0) ||
            (MAX_RAM <= end)) {
          gpwarning(GPW_INVALID_RAM, NULL);
        } else {
          for (; start <= end; start++)
            state.badram[start] = 1;
        }
      }
    } else {
      if (can_evaluate(p)) {
        int loc;

        loc = evaluate(p);
        if ((loc < 0) ||
            (MAX_RAM <= loc)) {
          gpwarning(GPW_INVALID_RAM, NULL);
         } else
          state.badram[loc] = 1;
      }
    }
  }

  return r;
}

static gpasmVal do_badrom(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{

  state.lst.line.linetype = dir;

  /* FIXME: implement this directive */
  gpwarning(GPW_UNKNOWN, "gpasm doesn't support the badrom directive yet");

  return r;
}

static void
set_bankisel(int address)
{
  if (state.device.class == PROC_CLASS_PIC14) {
    if (address < 0x100) {
      /* bcf 0x3, 0x7 */
      emit(0x1383);
    } else {
      /* bsf 0x3, 0x7 */
      emit(0x1783);
    }
  } else {
    /* movlb bank */
    emit(0xb800 | gp_processor_check_bank(state.device.class, address));
  }

}

static gpasmVal do_bankisel(gpasmVal r,
                            char *name,
                            int arity,
                            struct pnode *parms)
{
  struct pnode *p;
  int num_reloc;

  if ((state.device.class != PROC_CLASS_PIC14) &&
      (state.device.class != PROC_CLASS_PIC16)) {
    gpmessage(GPM_EXTPAGE, NULL);
    return r;
  }

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (state.mode == absolute) {
      set_bankisel(maybe_evaluate(p));
    } else {
      num_reloc = count_reloc(p);

      if (num_reloc == 0) {
        /* it is an absolute address, generate the bankisel but no relocation */
        set_bankisel(maybe_evaluate(p));
      } else if (num_reloc != 1) {
        gperror(GPE_UNRESOLVABLE, NULL);
      } else {
        reloc_evaluate(p, RELOCT_IBANKSEL);
        emit(0);
      }
    }
  }

  return r;
}

static gpasmVal do_banksel(gpasmVal r,
                           char *name,
                           int arity,
                           struct pnode *parms)
{
  struct pnode *p;
  int address;
  int bank;
  int num_reloc;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (state.mode == absolute) {
      address = maybe_evaluate(p);
      bank = gp_processor_check_bank(state.device.class, address);
      state.org += gp_processor_set_bank(state.device.class,
                                         state.processor->num_banks,
                                         bank,
                                         state.i_memory,
                                         state.org);
    } else {
      num_reloc = count_reloc(p);

      if (num_reloc == 0) {
        /* it is an absolute address, generate the banksel but no relocation */
        address = maybe_evaluate(p);
        bank = gp_processor_check_bank(state.device.class, address);
        state.org += gp_processor_set_bank(state.device.class,
                                           state.processor->num_banks,
                                           bank,
                                           state.i_memory,
                                           state.org);
      } else if (num_reloc != 1) {
        gperror(GPE_UNRESOLVABLE, NULL);
      } else if (state.device.class == PROC_CLASS_PIC14E) {
        reloc_evaluate(p, RELOCT_MOVLB);
        emit(0x20);
      } else if (state.device.class == PROC_CLASS_PIC16) {
        reloc_evaluate(p, RELOCT_BANKSEL);
        emit(0);
      } else if (state.device.class == PROC_CLASS_PIC16E) {
        reloc_evaluate(p, RELOCT_BANKSEL);
        emit(0x100);
      } else {
        switch (state.processor->num_banks) {
        case 2:
          reloc_evaluate(p, RELOCT_BANKSEL);
          emit(0);
          break;

        case 4:
          reloc_evaluate(p, RELOCT_BANKSEL);
          emit(0);
          emit(0);
          break;
        }
      }
    }
  }

  return r;
}

static gpasmVal do_code(gpasmVal r,
                        char *name,
                        int arity,
                        struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name, ".code", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_TEXT;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name, ".code", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = gp_processor_org_to_byte(state.device.class, maybe_evaluate(p));
      state.obj.new_sec_flags = STYP_TEXT | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_code_pack(gpasmVal r,
                             char *name,
                             int arity,
                             struct pnode *parms)
{
  struct pnode *p;

  if (!IS_16BIT_CORE)
    gperror(GPE_UNKNOWN, "code_pack is only supported on 16bit cores");
  else {
    state.lst.line.linetype = sec;
    state.next_state = state_section;

    if (state.mode == absolute) {
      gperror(GPE_OBJECT_ONLY, NULL);
    } else {
      switch (arity) {
      case 0:
        /* new relocatable section */
        strncpy(state.obj.new_sec_name, ".code", sizeof(state.obj.new_sec_name));
        state.obj.new_sec_addr = 0;
        state.obj.new_sec_flags = STYP_TEXT | STYP_BPACK;
        break;

      case 1:
        /* new absolute section */
        p = HEAD(parms);
        strncpy(state.obj.new_sec_name, ".code", sizeof(state.obj.new_sec_name));
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

static gpasmVal do_constant(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  int value;

  state.lst.line.linetype = dir;

  for (; parms; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
        (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        char *lhs;
        /* fetch the symbol */
        lhs = p->value.binop.p0->value.symbol;
        /* constants must be assigned a value at declaration */
        value = maybe_evaluate(p->value.binop.p1);
        /* put the symbol and value in the table*/
        set_global(lhs, value, PERMANENT, gvt_constant);
      }
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
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
 * do_config( ) - Called by the parser when a __CONFIG assembler directive
 *       is encountered.
 *
 * do_16_config( ) - Called by the parser to process MPASM style CONFIG xx=yy
 *       directives for 16 bit devices.
 */

static gp_boolean config_us_used;
static gp_boolean config_16_used;

/*
 * creates the configuration or device id COFF section for do_config and
 * do_16_config. returns true when a section is created.
 */
static gp_boolean config_add_section(int ca)
{
  if (state.mode == relocatable) {
    if ((!state.found_devid) && ((ca == DEVID1) || (ca == DEVID2))) {
      coff_new_section(".devid", ca, STYP_ABS | STYP_TEXT);
      state.found_devid = true;
          return true;
    } else if (!state.found_config) {
      int addr = gp_processor_org_to_byte(state.device.class,
                                          state.processor->config_addrs[0]);
      coff_new_section(".config", addr, STYP_ABS | STYP_TEXT);
      state.found_config = true;
          return true;
    }
  }

  return false;
}

/* helper to write configuration data, grabbing defaults when necessary */
static void config_16_set_mem(const struct gp_cfg_device *p_dev, int ca, unsigned char byte, unsigned char mask)
{
  unsigned char other_byte;
  unsigned char old_byte;

  if (!b_memory_get(state.c_memory, ca ^ 1, &other_byte)) {
    unsigned char fill_val = gp_cfg_get_default(p_dev, ca ^ 1);
    b_memory_put(state.c_memory, ca ^ 1, fill_val);
  }
  if (!b_memory_get(state.c_memory, ca, &old_byte))
    old_byte = gp_cfg_get_default(p_dev, ca);
  b_memory_put(state.c_memory, ca, (old_byte & ~mask) | byte);
}

static gpasmVal do_config(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{
  struct pnode *p;
  int ca;
  int value;

  config_us_used = true;
  if(config_16_used) {
    gperror(GPE_CONFIG_usCONFIG, NULL);
    return r;
  }

  if (IS_16BIT_CORE) {
    gpwarning(GPW_EXPECTED,"__CONFIG has been deprecated for PIC18 devices. Use directive CONFIG.");
  }

  state.lst.line.linetype = config;

  switch(arity) {
  case 1:
    /* FIXME: Whenever there are more than one configuration address, the
       address must be specified in MPASM. MPASM fails with error
       "Argument out of range (not a valid config register address)" */
    if (IS_16BIT_CORE) {
      gpwarning(GPW_EXPECTED,"18cxxx devices should specify __CONFIG address");
    }
    if (state.device.class == PROC_CLASS_PIC14E) {
      gpwarning(GPW_EXPECTED,"Enhanced 16cxxx devices should specify __CONFIG address");
    }
    ca = state.processor->config_addrs[0];
    p = HEAD(parms);
    break;

  case 2:
    ca = maybe_evaluate(HEAD(parms));
    p = HEAD(TAIL(parms));
    break;

  default:
    enforce_arity(arity,2);
    return r;
  }

  ca = gp_processor_org_to_byte(state.device.class, ca);
  state.lst.config_address = ca;
  /* FIXME: The .config section(s) should be placed in another list to
     be created at the END. That way we do not mess up the sections we
     are currently in and the section order will match with MPASM
     5.34. */
  config_add_section(ca);

  /* FIXME: For PIC18 (like for other PIC) MPASM always fills in two
     bytes. The byte not specified for PIC18 will contain the default
     values. For the two bytes it generates a section named
     .config_<org>_<filename>.O. */

  if ((can_evaluate(p)) && (state.pass == 2)) {
    value = evaluate(p);

    if (IS_16BIT_CORE) {
      const struct gp_cfg_device *p_dev;
      if(value > 0xff) {
        gpwarning(GPW_RANGE,0);
      }
      p_dev = gp_cfg_find_pic_multi(sizeof(state.processor->names) /
                                    sizeof(*state.processor->names),
                                    state.processor->names);
      if (p_dev) {
        /* We do this to also set the other byte in a word. */
        config_16_set_mem(p_dev, ca, value, 0xff);
      }
      else {
        /* Hack in case the config defaults are not available. */
        b_memory_put(state.c_memory, ca, value);
      }
    } else {
      unsigned short word;

      /* Don't complain for 14 bit enhanced devices */
      /* Why are the config words 16 bits long in headers?? */
      if (state.device.class != PROC_CLASS_PIC14E) {
        if(value > state.device.class->core_size) {
          gpmessage(GPM_RANGE,NULL);
          value &= state.device.class->core_size;
        }
      }

      if (state.device.class->i_memory_get(state.c_memory, ca, &word)) {
        gperror(GPE_ADDROVR, NULL);
      }

      state.device.class->i_memory_put(state.c_memory, ca, value);
      /* FIXME: need line_number? this one will be wrong coff_linenum(1) */
    }

    /* FIXME: MPASM continues the section that was interrupted. */
    /* force the section to end */
    state.obj.section = NULL;
  }

  return r;
}

/* Sets defaults over unused portions of configuration memory. */
static void config_16_check_defaults(const struct gp_cfg_device *p_dev)
{
  const struct gp_cfg_addr *addrs = p_dev->config_addrs;
  int t;

  /* FIXME: We do not need to set defaults here, but during
     linking. And to keep it consistent for absolute mode we should
     perform linking in absolute mode. MPASM 5.34 executes MPLINK in
     absolute mode. Below comments are old and not entirely right. */
  /*
   * if we don't set defaults here, then MPLINK (I'm assuming) will pad the
   * entire section with 0xff. That puts the 0xff's in the hex file. MPASM puts
   * nothing in the hex file for unspecified bytes. I'm not sure the best
   * approach here - defaults or nothing. going to go with defaults.
   *
   * if this is commented out, the calls to config_16_set_mem below _will_
   * use defaults for adjacent values.
   */
  for(t=0; t<p_dev->addr_count; addrs++, t++) {
    unsigned char byte;
    if(!b_memory_get(state.c_memory, addrs->addr, &byte)) {
      config_16_set_mem(p_dev, addrs->addr, addrs->defval, 0xff);
    }
  }
}

/* Support MPASM style CONFIG xxx = yyy syntax. */
static gpasmVal _do_16_config(gpasmVal r,
                             char *name,
                             int arity,
                             struct pnode *parms)
{
  static unsigned char double_mask[64];
  const struct gp_cfg_device *p_dev;
  const struct gp_cfg_directive *p_dir;
  const struct gp_cfg_option *p_opt;
  const char *k_str, *v_str;
  struct pnode *k, *v;
  char v_buff[64];
  int ca;

  state.lst.line.linetype = config;
  config_16_used = true;
  if (config_us_used) {
    gperror(GPE_CONFIG_usCONFIG, NULL);
    return r;
  }

  /* make sure we an find our device in the config DB */
  p_dev = gp_cfg_find_pic_multi(sizeof(state.processor->names) /
                sizeof(*state.processor->names),
                state.processor->names);
  if (!p_dev) {
    gperror(GPE_UNKNOWN, "the selected processor has no entries in the config db. CONFIG cannot be used.");
    return r;
  }

  /* validate argument format */
  if (!parms || parms->tag != binop || parms->value.binop.op != '=') {
    gperror(GPE_CONFIG_UNKNOWN, "incorrect syntax. use `CONFIG KEY = VALUE'");
    return r;
  }

  /* validate parameter types */
  k = parms->value.binop.p0;
  v = parms->value.binop.p1;
  if (k->tag != symbol || (v->tag != symbol && v->tag != constant)) {
    gperror(GPE_CONFIG_UNKNOWN, "incorrect syntax. use `CONFIG KEY = VALUE'");
    return r;
  }

  /* grab string representations */
  k_str = k->value.symbol;
  if (v->tag != constant)
      v_str = v->value.symbol;
  else {
    int value = v->value.constant;

    if (state.radix != 10) {
      if (state.radix == 16) {
        snprintf(v_buff, sizeof(v_buff), "%x", value);
      } else {
        gperror(GPE_CONFIG_UNKNOWN, "CONFIG can't be used in source files with a radix other than 10 or 16");
      }
    } else {
      snprintf(v_buff, sizeof(v_buff), "%d", value);
    }

    v_str = v_buff;
  }

  /* find the directive */
  p_dir = gp_cfg_find_directive(p_dev, k_str, &ca, NULL);
  if (!p_dir) {
    gperror(GPE_CONFIG_UNKNOWN, "CONFIG Directive Error:  (setting not found for selected processor)");
    return r;
  }

  /* note address to lister, though it doesn't seem to use it */
  state.lst.config_address = ca;

  /* find the option */
  p_opt = gp_cfg_find_option(p_dir, v_str);
  if (!p_opt) {
    gperror(GPE_CONFIG_UNKNOWN, "CONFIG Directive Error:  (specified value not valid for directive)");
    return r;
  }

  /* make sure the section exists ... */
  /* FIXME: For PIC18 we'd need to create a section for each config byte (like MPASM) */
  config_add_section(ca);
  config_16_check_defaults(p_dev);

  /* emit the bytes if appropriate */
  if (state.pass == 2) {
    unsigned char dm_addr = (unsigned char)(ca - p_dev->config_addrs->addr);

    /* make sure we've not written here yet */
    if (dm_addr < sizeof(double_mask)) {
      if (double_mask[dm_addr] & p_dir->mask) {
        gperror(GPE_CONFIG_UNKNOWN, "CONFIG Directive Error:  (multiple definitions found for setting)");
        return r;
      }
      double_mask[dm_addr] |= p_dir->mask;
    } else {
      gpwarning(GPW_UNKNOWN, "double_mask in do_16_config() needs to be adjusted to account for larger config ranges");
    }

    /* let the helper set the data. */
    config_16_set_mem(p_dev, ca, p_opt->byte, p_dir->mask);
  }

  return r;
}

static gpasmVal do_16_config(gpasmVal r,
               char *name,
               int arity,
               struct pnode *parms)
{
  /* FIXME: MPASM creates .config_<address>_<filename>.O sections for
     each configuration byte for relative files. For absolute it
     performs what MPLINK does i.e. fills all config bytes with
     default values. There can be undefined bytes in middle of
     configuration if there is no default.

     No instructions can follow after a CONFIG. Must leave no current
     section.
 */
  for (; parms != NULL; parms = TAIL(parms)) {
    struct pnode *p = HEAD(parms);
    _do_16_config(r, name, arity, p);
  }

  /* A new section must be started after this directive. */
  state.obj.section = NULL;

  return r;
}

/*-------------------------------------------------------------------------
 *  do_da - The 'da' directive. Identical to 'data' directive, except
 *          for 14-bit cores it packs two 7-bit characters into one word.
 */
static gpasmVal do_da(gpasmVal r,
                      char *name,
                      int arity,
                      struct pnode *parms)
{
  int char_shift = state.device.class == PROC_CLASS_PIC14 ? 7 : 8;
  if ((state.mode == relocatable) &&
      (SECTION_FLAGS & (STYP_DATA|STYP_BPACK))) {
    /* This is a data memory not program */
    state.lst.line.linetype = data;
    char_shift = 8;
  }

  emit_data(parms, char_shift);

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

static gpasmVal do_data(gpasmVal r,
                        char *name,
                        int arity,
                        struct pnode *parms)
{
  if ((state.mode == relocatable) &&
      (SECTION_FLAGS & (STYP_DATA|STYP_BPACK))) {
    /* This is a data memory not program */
    state.lst.line.linetype = data;
  }

  emit_data(parms, 8);

  return r;
}
/*-------------------------------------------------------------------------
 * do_db  - Reserve program memory words with packed 8-bit values. On the
 *          18cxxx families, dw and db are the same. For the 12 and 14 bit
 *          cores, the upper bits are masked (e.g. the 14-bit core can only
 *          store 14bits at a given program memory address, so the upper 2
 *          in a db directive are meaningless.
 */


static gpasmVal do_db(gpasmVal r,
                      char *name,
                      int arity,
                      struct pnode *parms)
{
  struct pnode *L = parms;
  struct pnode *p;

  if ((state.mode == relocatable) &&
      (SECTION_FLAGS & (STYP_DATA|STYP_BPACK))) {
    /* This is a data memory not program */
    state.lst.line.linetype = data;

    /* only valid in initialized data sections */
    if (SECTION_FLAGS & STYP_BSS)
      gperror(GPE_WRONG_SECTION, NULL);
  }

  if (state.device.class == PROC_CLASS_PIC16E ||
      (SECTION_FLAGS & STYP_DATA)) {
    unsigned begin_org = state.org;
    for(; L; L = TAIL(L)) {
      const char *pc;
      p = HEAD(L);
      if (p->tag == string) {
        int n = 0;
        pc = p->value.string;
        while(*pc) {
          int value;
          pc = convert_escape_chars(pc, &value);
          emit_byte(value);
          ++n;
        }
      }
      else {
        int value;
        value = reloc_evaluate(p, RELOCT_LOW);
        if (value < 0 || value > 0xFF) {
          gpwarning(GPW_RANGE, NULL);
        }
        emit_byte(value);
      }
    }
    if (state.mode == absolute || !(SECTION_FLAGS & (STYP_DATA|STYP_BPACK))) {
      if ((state.org - begin_org) & 1) {
        emit_byte(0);
      }
    }
  }
  else {
    unsigned short v = 0;
    unsigned n = 0;
    while(L) {
      const char *pc = NULL;
      int value;
      p = HEAD(L);
      if (p->tag == string)
        pc = p->value.string;
      for(;;) {
        if (p->tag == string) {
          pc = convert_escape_chars(pc, &value);
        }
        else {
          value = reloc_evaluate(p, RELOCT_LOW);
        }
        if (value < -128 || value > 0xFF) {
          gpwarning(GPW_RANGE, 0);
        }
        value &= 0xFF;
        if (!(n & 1))
          v = value << 8;
        else
          emit(v | value);
        ++n;
        if (p->tag != string || !*pc) {
          L = TAIL(L);
          break;
        }
      }
    }
    if (n & 1)
      emit(v);
  }
  return r;
}

static gpasmVal do_de(gpasmVal r,
                      char *name,
                      int arity,
                      struct pnode *parms)
{
  struct pnode *p;

  if (state.device.class == PROC_CLASS_PIC16E)
    return do_db(r, name, arity, parms);

  for(; parms; parms = TAIL(parms)) {
    p = HEAD(parms);
    if (p->tag == string) {
      const char *pc = p->value.string;
      while (*pc) {
        int value;
        pc = convert_escape_chars(pc, &value);
        emit(value & 0xFF);
      }
    }
    else {
      unsigned short v;
      v = reloc_evaluate(p, RELOCT_ALL);
      emit(v & 0xFF);
    }
  }

  return r;
}

/* Extension to MPASM, used at least by LLVM to emit debugging information. */
static gpasmVal do_def(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  char *symbol_name = NULL;
  gp_symbol_type *coff_symbol = NULL;
  int eval;
  int value = 0;
  gp_boolean new_class = false;
  int coff_class = C_NULL;
  gp_boolean new_type = false;
  int coff_type = T_NULL;
  enum gpasmValTypes type = gvt_debug;

  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    if (arity < 2) {
      enforce_arity(arity, 2);
      return r;
    }

    /* the first argument is the symbol name */
    p = HEAD(parms);
    if (enforce_simple(p)) {
      symbol_name = p->value.symbol;
    } else {
      return r;
    }
    parms = TAIL(parms);

    /* update the properties */
    for (; parms; parms = TAIL(parms)) {
      p = HEAD(parms);
      if ((p->tag == binop) &&
          (p->value.binop.op == '=')) {
        if (enforce_simple(p->value.binop.p0)) {
          char *lhs;

          lhs = p->value.binop.p0->value.symbol;
          if (strcasecmp(lhs, "value") == 0) {
            value = maybe_evaluate(p->value.binop.p1);
          } else if (strcasecmp(lhs, "size") == 0) {
            eval = maybe_evaluate(p->value.binop.p1);
            state.org += IS_RAM_ORG ? eval : gp_processor_org_to_byte(state.device.class, eval);
          } else if (strcasecmp(lhs, "type") == 0) {
            eval = maybe_evaluate(p->value.binop.p1);
            if ((eval < 0) || (eval > 0xffff)) {
              gperror(GPE_RANGE, NULL);
            } else {
              new_type = true;
              coff_type = eval;
            }
          } else if (strcasecmp(lhs, "class") == 0) {
            eval = maybe_evaluate(p->value.binop.p1);
            if ((eval < -128) || (eval > 127)) {
              gperror(GPE_RANGE, NULL);
            } else {
              new_class = true;
              coff_class = eval;
            }
          } else {
            gperror(GPE_ILLEGAL_ARGU, NULL);
          }
        }
      } else {
        if (enforce_simple(p)) {
          if (strcasecmp(p->value.symbol, "absolute") == 0) {
            type = gvt_absolute;
            value = 0;
          } else if (strcasecmp(p->value.symbol, "debug") == 0) {
            type = gvt_debug;
            value = 0;
          } else if (strcasecmp(p->value.symbol, "extern") == 0) {
            type = gvt_extern;
            value = 0;
          } else if (strcasecmp(p->value.symbol, "global") == 0) {
            type = gvt_global;
            value = IS_RAM_ORG ? state.org : gp_processor_byte_to_org(state.device.class, state.org);
          } else if (strcasecmp(p->value.symbol, "static") == 0) {
            type = gvt_static;
            value = IS_RAM_ORG ? state.org : gp_processor_byte_to_org(state.device.class, state.org);
          } else {
            gperror(GPE_ILLEGAL_ARGU, NULL);
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
    if (new_class)
      coff_symbol->class = coff_class;

    if (new_type)
      coff_symbol->type = coff_type;
  }

  return r;
}

static gpasmVal do_define(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  struct symbol *current_definition;

  state.lst.line.linetype = dir;

  if (arity < 1) {
    /* FIXME: find a more elegant way to do this */
    state.pass = 2;
    gperror(GPE_MISSING_ARGU, NULL);
    exit(1);
  }
  assert(arity <= 2);

  p = HEAD(parms);
  if (p->tag == string) {
    if((asm_enabled()) && (!state.mac_prev)) {
      if ((get_symbol(state.stDefines, p->value.string) != NULL)
          && (state.pass == 1)) {
        /* FIXME: find a more elegant way to do this */
        state.pass = 2;
        gperror(GPE_DUPLAB, NULL);
        exit(1);
      }
      current_definition = add_symbol(state.stDefines, p->value.string);
      if (TAIL(parms)) {
        struct pnode *p2 = HEAD(TAIL(parms));
        assert(p2->tag == string);
        annotate_symbol(current_definition, strdup(p2->value.string));
      }
    }
  }

  return r;
}

/* Extension to MPASM, used at least by LLVM to emit debugging information. */
static gpasmVal do_dim(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  char *symbol_name = NULL;
  gp_symbol_type *coff_symbol = NULL;
  int number_symbols;
  gp_aux_type *aux_list;
  int i;
  int value;

  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
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
        gperror(GPE_NOSYM, NULL);
        return r;
      }
    } else {
      return r;
    }
    parms = TAIL(parms);

    /* the second argument must be the number of aux symbols */
    p = HEAD(parms);
    number_symbols = maybe_evaluate(p);
    if ((number_symbols < 0) || (number_symbols > 127)) {
      gperror(GPE_UNKNOWN, "number of auxiliary symbols must be less then 128 and positive");
      return r;
    }
    state.obj.symbol_num += number_symbols;
    parms = TAIL(parms);

    /* create the symbols */
    aux_list = gp_coffgen_addaux(state.obj.object, coff_symbol);

    /* write the data to the auxiliary symbols */
    i = 0;
    while(parms) {
      p = HEAD(parms);
      value = maybe_evaluate(p);
      if (value & (~0xff)) {
        gperror(GPE_RANGE, NULL);
        return r;
      }

      if (aux_list == NULL) {
        gperror(GPE_UNKNOWN, "insufficent number of auxiliary symbols");
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

static gpasmVal do_direct(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{

  struct pnode *p;
  int value;
  unsigned char direct_command = 0;
  char *direct_string = NULL;

  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 2)) {
    p = HEAD(parms);
    coerce_str1(p);
    value = maybe_evaluate(p);
    if ((value < 0) || (value > 255)) {
      gperror(GPE_RANGE, NULL);
    } else {
      direct_command = value;
    }

    p= HEAD(TAIL(parms));
    if (p->tag == string) {
      if (strlen(p->value.string) < 255) {
        direct_string = convert_escaped_char(p->value.string,'"');
      } else {
        gperror(GPE_UNKNOWN, "string must be less than 255 bytes long");
      }
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }

    if (direct_string == NULL) {
      return r;
    }

    if (SECTION_FLAGS & STYP_TEXT) {
      coff_add_directsym(direct_command, direct_string);
    } else {
      gperror(GPE_WRONG_SECTION, NULL);
    }

  }

  return r;
}

static gpasmVal do_dt(gpasmVal r,
                      char *name,
                      int arity,
                      struct pnode *parms)
{
  int retlw = gp_processor_retlw(state.device.class);
  struct pnode *p;

  for(; parms; parms = TAIL(parms)) {
    p = HEAD(parms);
    if (p->tag == string) {
      const char *pc = p->value.string;
      while (*pc) {
        int value;
        pc = convert_escape_chars(pc, &value);
        emit((value & 0xFF) | retlw);
      }
    }
    else {
      unsigned short v;
      v = reloc_evaluate(p, RELOCT_ALL);
      emit((v & 0xFF) | retlw);
    }
  }

  return r;
}

/*-------------------------------------------------------------------------
 *  do_dw - The 'dw' directive. On all families except for the p18cxxx, the
 *          dw directive is the same as the 'data' directive. For the p18cxxx
 *          it's the same as the 'db' directive. (That's strange, but it's
 *          also the way mpasm does it).
 */
static gpasmVal do_dw(gpasmVal r,
                      char *name,
                      int arity,
                      struct pnode *parms)
{
  if ((state.mode == relocatable) &&
      (SECTION_FLAGS & (STYP_DATA|STYP_BPACK))) {
    /* This is a data memory not program */
    state.lst.line.linetype = data;

    /* only valid in initialized data sections */
    if (SECTION_FLAGS & STYP_BSS)
      gperror(GPE_WRONG_SECTION, NULL);
  }
  /* MPASM 5.34 seems to do this same for p18cxxx as for others. */
  emit_data(parms, 8);

  return r;
}


static gpasmVal do_else(gpasmVal r,
                        char *name,
                        int arity,
                        struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (state.astack == NULL)
    gperror(GPE_ILLEGAL_COND, NULL);
  else if ((state.astack->mode != in_then))
    gperror(GPE_ILLEGAL_COND, NULL);
  else
    state.astack->enabled = !state.astack->enabled;

  return r;
}

static gpasmVal do_end(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  state.found_end = 1;
  state.lst.line.linetype = dir;

  return r;
}

static gpasmVal do_endif(gpasmVal r,
                         char *name,
                         int arity,
                         struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (state.astack == NULL) {
    gperror(GPE_ILLEGAL_COND, "Illegal condition (ENDIF).");
  } else if ((state.astack->mode != in_then) &&
             (state.astack->mode != in_else)) {
    gperror(GPE_ILLEGAL_COND, "Illegal condition (ENDIF).");
  } else {
    struct amode *old;

    old = state.astack;
    state.astack = state.astack->prev;
    free(old);
  }
  return r;
}

static gpasmVal do_endm(gpasmVal r,
                        char *name,
                        int arity,
                        struct pnode *parms)
{
  assert(!state.mac_head);
  state.lst.line.linetype = dir;
  if (state.mac_prev == NULL)
    gperror(GPE_UNMATCHED_ENDM, NULL);
  else
    state.mac_prev = NULL;

  state.mac_body = NULL;

  return r;
}

static gpasmVal do_endw(gpasmVal r,
                        char *name,
                        int arity,
                        struct pnode *parms)
{
  state.lst.line.linetype = dir;

  assert(!state.mac_head);
  if (state.mac_prev == NULL) {
    gperror(GPE_ILLEGAL_COND, "Illegal condition (ENDW).");
  } else if (maybe_evaluate(state.while_head->parms)) {
    state.next_state = state_while;
    state.next_buffer.macro = state.while_head;
  }

  state.mac_body = NULL;
  state.mac_prev = NULL;
  state.while_head = NULL;

  return r;
}

static gpasmVal do_eof(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 0)) {
    if (state.debug_info) {
      coff_add_eofsym();
    } else {
      gpwarning(GPW_UNKNOWN, "directive ignored when debug info is disabled");
    }
  }

  return r;
}

static gpasmVal do_equ(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  state.lst.line.linetype = equ;
  if (enforce_arity(arity, 1)) {
    r = maybe_evaluate(HEAD(parms));
  }

  return r;
}

static gpasmVal do_error(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == string) {
      gperror(GPE_USER, p->value.string);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
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

static gpasmVal do_errlvl(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;

  if (state.pass == 2) {
    for (; parms; parms = TAIL(parms)) {
      p = HEAD(parms);
      if (p->tag == unop)  {
        gpasmVal value = evaluate(p->value.unop.p0);

        if (p->value.unop.op == '-') {
          add_code(-value);
        } else if (p->value.unop.op == '+') {
          add_code(value);
        } else {
          gperror(GPE_ILLEGAL_ARGU, "Expected 0, 1, 2, +|-<message number>");
        }
      } else if (p->tag == constant) {
        select_errorlevel(p->value.constant);
      } else {
        gperror(GPE_ILLEGAL_ARGU, "Expected 0, 1, 2, +|-<message number>");
      }
    }
  }

  return r;
}

static gpasmVal do_exitm(gpasmVal r,
                         char *name,
                         int arity,
                         struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 0)) {
    if (state.stGlobal == state.stTop) {
      gperror(GPE_UNKNOWN, "Attempt to use \"exitm\" outside of macro");
    } else {
      state.next_state = state_exitmacro;
    }
  }

  return r;
}

static gpasmVal do_expand(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (state.cmd_line.macro_expand) {
    gpmessage(GPM_SUPLIN, NULL);
  } else {
    if (enforce_arity(arity, 0)) {
      state.lst.expand = true;
    }
  }
  return r;
}

static gpasmVal do_extern(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{
  char *p;
  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    for (; parms; parms = TAIL(parms)) {
      p = maybe_evaluate_concat(HEAD(parms));
      if (p) {
        set_global(p, 0, PERMANENT, gvt_extern);
      }
    }
  }

  return r;
}

static gpasmVal do_file(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 1)) {
    if (state.debug_info) {
      p = HEAD(parms);
      if (p->tag == string) {
        state.obj.debug_file = coff_add_filesym(p->value.string, 0);
      } else {
        gperror(GPE_ILLEGAL_ARGU, NULL);
      }
    } else {
      gpwarning(GPW_UNKNOWN, "directive ignored when debug info is disabled");
    }
  }

  return r;
}

/* Filling constants is handled here. Filling instructions is handled
   in the parser. */

static gpasmVal do_fill(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *h;
  int number;
  int i;

  if (enforce_arity(arity, 2)) {
    h = HEAD(parms);
    number = eval_fill_number(HEAD(TAIL(parms)));

    for (i = 1; i <= number ; i += 1) {
      /* we must evaluate each loop, because some symbols change (i.e. $) */
      emit(maybe_evaluate(h));
    }
  }
  return r;
}

static gpasmVal do_global(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{
  char *p;
  char buf[BUFSIZ];
  struct symbol *s;
  struct variable *var;

  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    for (; parms; parms = TAIL(parms)) {
      p = maybe_evaluate_concat(HEAD(parms));
      if (p) {
        s = get_symbol(state.stTop, p);
        if (s == NULL) {
          snprintf(buf,
                   sizeof(buf),
                   "Symbol not previously defined (%s).",
                   p);
          gperror(GPE_NOSYM, buf);
        } else {
          var = get_symbol_annotation(s);
          if (var == NULL) {
            snprintf(buf,
                     sizeof(buf),
                     "Symbol not assigned a value (%s).",
                     p);
            gpwarning(GPW_UNKNOWN, buf);
          } else {
            if ((var->previous_type == gvt_address) ||
                (var->previous_type == gvt_global)  ||
                (var->previous_type == gvt_static)) {
              /* make the symbol global */
              var->type = gvt_global;
            } else if (var->previous_type == gvt_extern) {
              gperror(GPE_DUPLAB, NULL);
            } else {
              snprintf(buf,
                       sizeof(buf),
                       "Operand must be an address label (%s).",
                       p);
              gperror(GPE_MUST_BE_LABEL, buf);
            }
          }
        }
      } else {
        gperror(GPE_ILLEGAL_ARGU, NULL);
      }
    }
  }

  return r;
}

static gpasmVal do_idata(gpasmVal r,
                         char *name,
                         int arity,
                         struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name, ".idata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_DATA;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name, ".idata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_DATA | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_idata_acs(gpasmVal r,
                             char *name,
                             int arity,
                             struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name, ".idata_acs", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_DATA | STYP_ACCESS;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name, ".idata_acs", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_DATA | STYP_ABS | STYP_ACCESS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_ident(gpasmVal r,
                         char *name,
                         int arity,
                         struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == string) {
      coff_add_identsym(p->value.string);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_idlocs(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{
  int value;
  unsigned int id_location;
  unsigned int idreg;

  id_location = gp_processor_id_location(state.processor);
  if (id_location == 0) {
    gperror(GPE_ILLEGAL_DIR, NULL);
    return r;
  }

  if (state.device.class == PROC_CLASS_PIC16E) {
    if (enforce_arity(arity,2)) {
      idreg = gp_processor_org_to_byte(state.device.class, maybe_evaluate(HEAD(parms)));
      value = maybe_evaluate(HEAD(TAIL(parms)));
    } else {
      gperror(GPW_EXPECTED,"18cxxx devices should specify __IDLOC address");
      return r;
    }
  } else {
    if (enforce_arity(arity,1)) {
      idreg = id_location;
      value = maybe_evaluate(HEAD(parms));
    } else {
      return r;
    }
  }

  if ((state.mode == relocatable) && (!state.found_idlocs)) {
    coff_new_section(".idlocs", idreg, STYP_ABS | STYP_TEXT);
    state.found_idlocs = true;
  }

  if (state.pass == 2) {
    if (IS_16BIT_CORE) {
      if (idreg > IDLOC7 || idreg < IDLOC0) {
        gperror(GPE_RANGE, "Argument out of range (not a valid ID location)");
      } else {
        unsigned char curvalue;
        state.lst.line.linetype = config;
        state.lst.config_address = idreg;
        if(value > 0xff) {
          gpmessage(GPM_IDLOC, NULL);
        }
        if (idreg <= state.device.id_location) {
          gperror(GPE_IDLOCS_ORDER, NULL);
        }
        if (b_memory_get(state.c_memory, idreg, &curvalue))
          gperror(GPE_ADDROVR, NULL);
        b_memory_put(state.c_memory, idreg, value);
      }

    } else {
      unsigned short word;
      state.lst.line.linetype = idlocs;

      if (value > 0xffff) {
        gpmessage(GPM_IDLOC, NULL);
        value &= 0xffff;
      }

      if (state.device.class->i_memory_get(state.c_memory, idreg, &word)) {
        gperror(GPE_ADDROVR, NULL);
      }

      state.device.class->i_memory_put(state.c_memory, idreg,
                                       (value & 0xf000) >> 12);
      state.device.class->i_memory_put(state.c_memory, idreg + 2,
                                       (value & 0x0f00) >> 8);
      state.device.class->i_memory_put(state.c_memory, idreg + 4,
                                       (value & 0x00f0) >> 4);
      state.device.class->i_memory_put(state.c_memory, idreg + 6,
                                       value & 0x000f);

    }
  }
  state.device.id_location = idreg;

  return r;
}

static gpasmVal do_if(gpasmVal r,
                      char *name,
                      int arity,
                      struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
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

static gpasmVal do_ifdef(gpasmVal r,
                         char *name,
                         int arity,
                         struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  enter_if();

  /* Only evaluate the conditional if it matters... */
  if (state.astack->prev_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);
      if (p->tag != symbol) {
        gperror(GPE_ILLEGAL_LABEL, NULL);
      } else {
        if ((get_symbol(state.stDefines, p->value.symbol)) ||
            (get_symbol(state.stTop, p->value.symbol)))
          state.astack->enabled = 1;
      }
    }
  }

  return r;
}

static gpasmVal do_ifndef(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  enter_if();

  /* Only evaluate the conditional if it matters... */
  if (state.astack->prev_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);
      if (p->tag != symbol) {
        gperror(GPE_ILLEGAL_LABEL, NULL);
      } else {
        if ((!get_symbol(state.stDefines, p->value.symbol)) &&
            (!get_symbol(state.stTop, p->value.symbol)))
          state.astack->enabled = 1;
      }
    }
  }

  return r;
}

static gpasmVal do_include(gpasmVal r,
                           char *name,
                           int arity,
                           struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == string) {
      state.next_state = state_include;
      state.next_buffer.file = strdup(p->value.string);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_line(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 1)) {
    if (state.debug_info) {
      p = HEAD(parms);
      state.obj.debug_line = maybe_evaluate(p);
    } else {
      gpwarning(GPW_UNKNOWN, "directive ignored when debug info is disabled");
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

static gpasmVal do_list(gpasmVal r,
                        char *name,
                        int arity,
                        struct pnode *parms)
{
  struct pnode *p;

  state.lst.enabled = true;
  state.lst.line.linetype = dir;

  for (; parms; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
        (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        char *lhs;

        lhs = p->value.binop.p0->value.symbol;
        if (strcasecmp(lhs, "b") == 0) {
          int b;

          b = maybe_evaluate(p->value.binop.p1);
          if (b != 0)
            state.lst.tabstop = b;
        } else if (strcasecmp(lhs, "c") == 0) {
          ; /* Ignore this for now: column width not used */
        } else if (strcasecmp(lhs, "f") == 0) {
          if (enforce_simple(p->value.binop.p1))
            select_hexformat(p->value.binop.p1->value.symbol);
        } else if (strcasecmp(lhs, "l") == 0) {
          ; /* Ignore this for now: page length */
        } else if (strcasecmp(lhs, "m") == 0) {
          /* Undocumented, only found in MEMORY.INC and
             MCP250XX.INC. */
          if (can_evaluate(p->value.binop.p1)) {
            int value = evaluate(p->value.binop.p1);
            if (value < state.maxrom) {
              char message[BUFSIZ];
              snprintf(message, sizeof(message),
                       "Argument out of range (must be greater than or equal to %ld)",
                       state.maxrom);
              gperror(GPE_RANGE, message);
            }
            else
              state.maxrom = value;
          }
        } else if (strcasecmp(lhs, "mm") == 0) {
          state.lst.memorymap = off_or_on(p->value.binop.p1);
        } else if (strcasecmp(lhs, "n") == 0) {
          if (can_evaluate(p->value.binop.p1)) {
            int number = evaluate(p->value.binop.p1);

            if ((number > 6) || (number == 0)) {
              state.lst.linesperpage = number;
            } else {
              gperror(GPE_RANGE, NULL);
            }
          }
        } else if (strcasecmp(lhs, "p") == 0) {
          if (enforce_simple(p->value.binop.p1))
            select_processor(p->value.binop.p1->value.symbol);
        } else if (strcasecmp(lhs, "pe") == 0) {
          state.extended_pic16e = true;
          if (enforce_simple(p->value.binop.p1))
            select_processor(p->value.binop.p1->value.symbol);
        } else if (strcasecmp(lhs, "r") == 0) {
          if (enforce_simple(p->value.binop.p1))
            select_radix(p->value.binop.p1->value.symbol);
        } else if (strcasecmp(lhs, "st") == 0) {
          state.lst.symboltable = off_or_on(p->value.binop.p1);
        } else if (strcasecmp(lhs, "t") == 0) {
          ; /* Ignore this for now: always wrap long list lines */
        } else if (strcasecmp(lhs, "w") == 0) {
          select_errorlevel(maybe_evaluate(p->value.binop.p1));
        } else if (strcasecmp(lhs, "x") == 0) {
          if (enforce_simple(p->value.binop.p1))
            select_expand(p->value.binop.p1->value.symbol);
        } else {
          gperror(GPE_ILLEGAL_ARGU, NULL);
        }
      }
    } else {
      if (enforce_simple(p)) {
        if (strcasecmp(p->value.symbol, "free") == 0) {
          ; /* Ignore this directive */
        } else if (strcasecmp(p->value.symbol, "fixed") == 0) {
          ; /* Ignore this directive */
        } else if (strcasecmp(p->value.symbol, "nowrap") == 0) {
          ; /* Ignore this directive */
        } else if (strcasecmp(p->value.symbol, "wrap") == 0) {
          ; /* Ignore this directive */
        } else {
          gperror(GPE_ILLEGAL_ARGU, NULL);
        }
      }
    }
  }

  /* The .list symbol is only added to the COFF file if its only action is to turn on
     the listing */
  if (arity == 0)
    coff_add_listsym();

  return r;
}

static gpasmVal do_local(gpasmVal r,
                         char *name,
                         int arity,
                         struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;

  /* like variable except it is put in TOP instead of GLOBAL */

  if (state.stGlobal == state.stTop) {
    gperror(GPE_UNKNOWN, "Attempt to use \"local\" outside of macro");
  } else {
    for (; parms; parms = TAIL(parms)) {
      p = HEAD(parms);
      if ((p->tag == binop) &&
          (p->value.binop.op == '=')) {
        if (enforce_simple(p->value.binop.p0)) {
          char *lhs;
          gpasmVal value;
          /* fetch the symbol */
          lhs = p->value.binop.p0->value.symbol;
          value = maybe_evaluate(p->value.binop.p1);
          /* put the symbol and value in the TOP table*/
          add_symbol(state.stTop, lhs);
          set_global(lhs, value, TEMPORARY, gvt_constant);
        }
      } else if (p->tag == symbol) {
        /* put the symbol in the Top table */
        add_symbol(state.stTop, p->value.symbol);
      } else {
        gperror(GPE_ILLEGAL_ARGU, NULL);
      }
    }
  }

  return r;
}

static gpasmVal do_noexpand(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (state.cmd_line.macro_expand) {
    gpmessage(GPM_SUPLIN, NULL);
  } else {
    if (enforce_arity(arity, 0)) {
      state.lst.expand = false;
    }
  }
  return r;
}

static gpasmVal do_nolist(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  state.lst.line.linetype = dir;

  if (!state.lst.force)
    state.lst.enabled = false;

  coff_add_nolistsym();

  return r;
}

static gpasmVal do_maxram(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = set;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p))
      r = state.maxram = evaluate(p);
  }

  return r;
}

static gpasmVal do_maxrom(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = set;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p))
      r = state.maxrom = evaluate(p);
  }

  return r;
}

static gpasmVal do_macro(gpasmVal r,
                         char *name,
                         int arity,
                         struct pnode *parms)
{
  struct macro_head *head = malloc(sizeof(*head));

  head->parms = parms;
  head->body = NULL;
  head->defined = 0;
  /* Record data for the list, cod, and coff files */
  head->line_number = state.src->line_number;
  head->file_symbol = state.src->file_symbol;

  head->src_name = strdup(state.src->name);

  state.lst.line.linetype = dir;
  if (macro_parms_ok(parms))
    state.mac_head = head;

  state.mac_prev = &(head->body);
  state.mac_body = NULL;

  return r;
}

static gpasmVal do_messg(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == string) {
      gpmessage(GPM_USER, p->value.string);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_org(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p)) {
      gpasmVal new_org;
      r = evaluate(p);
      if ((r & 0x1) && state.device.class == PROC_CLASS_PIC16E)
        gperror(GPE_ORG_ODD, NULL);
      new_org = gp_processor_org_to_byte(state.device.class, r);
      if (state.mode == absolute) {
        state.lst.line.linetype = org;
        state.org = new_org;
      } else {
        /* Default section name, this will be overwritten if a label is
           present. */
        snprintf(state.obj.new_sec_name,
                 sizeof(state.obj.new_sec_name),
                 ".org_%x",
                 r);
        state.obj.new_sec_addr = new_org;
        state.obj.new_sec_flags = STYP_TEXT | STYP_ABS;
        state.lst.line.linetype = sec;
        state.next_state = state_section;
      }
    }
    else
      r = 0;
  }

  return r;
}

static gpasmVal do_page(gpasmVal r,
                        char *name,
                        int arity,
                        struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 0) && state.lst.enabled)
    lst_throw();

  return r;
}

/* Called by both do_pagesel and do_pageselw, which have a very slight
 * difference between them */
static gpasmVal _do_pagesel(gpasmVal r,
                           char *name,
                           int arity,
                           struct pnode *parms,
                           unsigned short reloc_type)
{
  struct pnode *p;
  int page;
  int num_reloc;
  int use_wreg = 0;

  if ((reloc_type == RELOCT_PAGESEL_WREG) ||
      (state.device.class == PROC_CLASS_PIC16)) {
    use_wreg = 1;
  }

  if ((state.device.class == PROC_CLASS_EEPROM8) ||
      (state.device.class == PROC_CLASS_EEPROM16) ||
      (state.device.class == PROC_CLASS_PIC16E)) {
    /* do nothing */
    return r;
  } else if (state.device.class == PROC_CLASS_PIC16) {
    gpmessage(GPM_W_MODIFIED, NULL);
  }

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (state.mode == absolute) {
      page = gp_processor_check_page(state.device.class, maybe_evaluate(p));
      state.org += gp_processor_set_page(state.device.class,
                                         state.processor->num_pages,
                                         page,
                                         state.i_memory,
                                         state.org,
                                         use_wreg);
    } else {
      num_reloc = count_reloc(p);

      if (num_reloc == 0) {
        /* it is an absolute address, generate the pagesel but no relocation */
        page = gp_processor_check_page(state.device.class, maybe_evaluate(p));

        state.org += gp_processor_set_page(state.device.class,
                                           state.processor->num_pages,
                                           page,
                                           state.i_memory,
                                           state.org,
                                           use_wreg);
      } else if (num_reloc != 1) {
        gperror(GPE_ILLEGAL_LABEL, NULL);
      } else if (state.device.class == PROC_CLASS_PIC16) {
        reloc_evaluate(p, RELOCT_PAGESEL_WREG);
        emit(0);
        emit(0);
      } else if (state.device.class == PROC_CLASS_PIC14E) {
        if (use_wreg == 0) {
          reloc_evaluate(p, RELOCT_PAGESEL_MOVLP);
          emit(0x3180);
        } else {
          reloc_evaluate(p, RELOCT_PAGESEL_WREG);
          emit(0);
          emit(0);
        }
      } else {
        if ((use_wreg == 0) &&
            (state.processor->num_pages == 2)) {
          reloc_evaluate(p, RELOCT_PAGESEL_BITS);
          emit(0);
        }
        else if ((state.processor->num_pages == 2) ||
            (state.processor->num_pages == 4)) {
          reloc_evaluate(p, reloc_type);
          emit(0);
          emit(0);
        }
      }
    }

  }

  return r;
}

static gpasmVal do_pagesel(gpasmVal r,
                           char *name,
                           int arity,
                           struct pnode *parms)
{
  return _do_pagesel(r, name, arity, parms, RELOCT_PAGESEL_BITS);
}

static gpasmVal do_pageselw(gpasmVal r,
                           char *name,
                           int arity,
                           struct pnode *parms)
{
  return _do_pagesel(r, name, arity, parms, RELOCT_PAGESEL_WREG);
}

static gpasmVal do_processor(gpasmVal r,
                             char *name,
                             int arity,
                             struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    struct pnode *p = HEAD(parms);

    if (enforce_simple(p))
      select_processor(p->value.symbol);
  }

  return r;
}

static gpasmVal do_radix(gpasmVal r,
                         char *name,
                         int arity,
                         struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (enforce_simple(p)) {
      select_radix(p->value.symbol);
    }
  }

  return r;
}

static gpasmVal do_res(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  int count;
  int i;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p)) {
      count = evaluate(p);

      state.lst.line.linetype = res;

      if (state.mode == absolute) {
        if (state.device.class == PROC_CLASS_PIC16E && count % 2 != 0)
          gperror(GPE_RES_ODD_PIC16EA, NULL);
        count = gp_processor_org_to_byte(state.device.class, count);
        for (i = 0; i + 1 < count; i += 2) {
          emit(state.device.class->core_size);
        }
      } else {
        if (SECTION_FLAGS & STYP_TEXT) {
          count = gp_processor_org_to_byte(state.device.class, count);
          if (state.device.class->rom_width < 16) {
            /* FIXME: Most likely this check belongs to our caller. */
            if (count < 2)
              gpwarning(GPW_UNKNOWN, "No memory has been reserved by this instruction.");
          }
          for (i = 0; i + 1 < count; i += 2) {
            /* For some reason program memory is filled with a different
               value. */
            emit(state.device.class->core_size);
          }
        } else {
          for (i = 0; i < count; i++) {
            emit_byte(0);
          }
        }
      }
    }
  }

  return r;
}

static gpasmVal do_set(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = set;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p))
      r = evaluate(p);
    else
      r = 0;
  }

  return r;
}

static gpasmVal do_space(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;

  if (state.pass == 2) {
    switch (arity) {
      case 0:
        /* do nothing */
        break;

      case 1:
        p = HEAD(parms);
        if (can_evaluate(p)) {
          int i;

          for (i = evaluate(p); i > 0; i--)
            lst_line("");
        }
        break;

      default:
        enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_subtitle(gpasmVal r,
                        char *name,
                        int arity,
                        struct pnode *parms)
{
  struct pnode *p;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == string) {
#define LEN sizeof(state.lst.subtitle_name)
      strncpy(state.lst.subtitle_name, p->value.string, LEN);
#undef LEN
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_title(gpasmVal r,
                        char *name,
                        int arity,
                        struct pnode *parms)
{
  struct pnode *p;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == string) {
#define LEN sizeof(state.lst.title_name)
      strncpy(state.lst.title_name, p->value.string, LEN);
#undef LEN
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_type(gpasmVal r,
                        char *name,
                        int arity,
                        struct pnode *parms)
{
  struct pnode *p;
  char *symbol_name = NULL;
  gp_symbol_type *coff_symbol = NULL;
  int value;

  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 2)) {
    /* the first argument is the symbol name */
    p = HEAD(parms);
    if (enforce_simple(p)) {
      symbol_name = p->value.symbol;
      coff_symbol = gp_coffgen_findsymbol(state.obj.object, symbol_name);
      if (coff_symbol == NULL) {
        gperror(GPE_NOSYM, NULL);
      } else {
        p = HEAD(TAIL(parms));
        value = maybe_evaluate(p);
        if ((value < 0) || (value > 0xffff)) {
          gperror(GPE_RANGE, NULL);
        } else {
          coff_symbol->type = value;
        }
      }
    }
  }

  return r;
}

static gpasmVal do_udata(gpasmVal r,
                         char *name,
                         int arity,
                         struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name, ".udata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name, ".udata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_udata_acs(gpasmVal r,
                             char *name,
                             int arity,
                             struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name,
              ".udata_acs",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS | STYP_ACCESS;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name,
              ".udata_acs",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS | STYP_ACCESS;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_udata_ovr(gpasmVal r,
                             char *name,
                             int arity,
                             struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name,
              ".udata_ovr",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS | STYP_OVERLAY;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name,
              ".udata_ovr",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS | STYP_OVERLAY;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_udata_shr(gpasmVal r,
                             char *name,
                             int arity,
                             struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name,
              ".udata_shr",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS | STYP_SHARED;
      break;

    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name,
              ".udata_shr",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS | STYP_SHARED;
      break;

    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_undefine(gpasmVal r,
                            char *name,
                            int arity,
                            struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == symbol) {
      if (remove_symbol(state.stDefines, p->value.symbol) == 0)
        gpwarning(GPW_NOT_DEFINED, NULL);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_variable(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  int value;

  state.lst.line.linetype = dir;

  for (; parms; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
        (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        char *lhs;
        /* fetch the symbol */
        lhs = p->value.binop.p0->value.symbol;
        value = maybe_evaluate(p->value.binop.p1);
        /* put the symbol and value in the table*/
        set_global(lhs, value, TEMPORARY, gvt_constant);
      }
    } else if (p->tag == symbol) {
      /* put the symbol with a 0 value in the table*/
      set_global(p->value.symbol, 0, TEMPORARY, gvt_constant);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_while(gpasmVal r,
                         char *name,
                         int arity,
                         struct pnode *parms)
{
  struct macro_head *head = malloc(sizeof(*head));
  struct pnode *p;

  if (state.src->type == src_while) {
    state.pass = 2; /* Ensure error actually gets displayed */
    gperror(GPE_UNKNOWN, "gpasm does not yet support nested while loops");
    exit (1);
  }

  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    head->parms = p;
  } else {
    head->parms = NULL;
  }

  head->body = NULL;
  /* Record data for the list, cod, and coff files */
  head->line_number = state.src->line_number;
  head->file_symbol = state.src->file_symbol;

  head->src_name = strdup(state.src->name);

  /* DON'T set up state.mac_head; this isn't a macro head. */
  state.while_head = head;

  state.mac_prev = &(head->body);
  state.mac_body = NULL;

  return r;
}

int asm_enabled(void)
{
  return ((state.astack == NULL) ||
          (state.astack->enabled &&
           state.astack->prev_enabled));
}

/* Check that a register file address is ok */
void file_ok(unsigned int file)
{
  /* don't check address, the linker takes care of it */
  if (state.mode == relocatable)
    return;

  if ((file > state.maxram) || (state.badram[file])) {
    gpwarning(GPW_INVALID_RAM, NULL);
  }

  /* Issue bank message if necessary */
  if (file & state.device.class->bank_mask)
    gpmessage(GPM_BANK, NULL);
}

static void emit_check(int insn, int argument, int mask)
{
  int test = argument;

  if (test < 0)
    test = -test;

  /* If there are bits that shouldn't be set then issue a warning */
  if (test & (~mask)) {
    gpwarning(GPW_RANGE, NULL);
  }
  emit(insn | (argument & mask));
}

/*
   For relative branches, issue a warning if the absolute value of
   argument is greater than range
*/
static void emit_check_relative(int insn, int argument, int mask, int range)
{
  char full_message[BUFSIZ];

  /* If the branch is too far then issue an error */
  if ((argument > range) || (argument < -(range+1))) {
    snprintf(full_message,
             sizeof(full_message),
             "Argument out of range (%d not between %d and %d)\n",
             argument,
             -(range+1),
             range);
    gperror(GPE_RANGE, full_message);
  }

  emit(insn | (argument & mask));
}

static int check_flag(int flag)
{
  if ((flag != 0) && (flag != 1))
    gpwarning(GPW_RANGE, NULL);

  return flag & 0x1;
}

gpasmVal do_insn(char *name, struct pnode *parms)
{
  struct symbol *s;
  int arity;
  struct pnode *p;
  int file;             /* register file address, if applicable */
  gpasmVal r;           /* Return value */

  if (!IS_RAM_ORG)
    /* We want to have r as the value to assign to label */
    r = gp_processor_byte_to_org(state.device.class, state.org);
  else
    r = state.org;

  arity = list_length(parms);

  s = get_symbol(state.stBuiltin, name);

  if (s) {
    struct insn *i;

    i = get_symbol_annotation(s);
    /* Interpret the instruction if assembly is enabled, or if it's a
       conditional. */
    if (asm_enabled() || (i->attribs & ATTRIB_COND)) {
      state.lst.line.linetype = insn;
      switch (i->class) {
      case INSN_CLASS_LIT3_BANK:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, (reloc_evaluate(p, RELOCT_F) >> 5), 0x07);
        }
        break;

      case INSN_CLASS_LIT3_PAGE:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, (reloc_evaluate(p, RELOCT_F) >> 9), 0x07);
        }
        break;

      case INSN_CLASS_LIT1:
        {
          int s = 0;
          switch (arity) {
          case 1:
            s = check_flag(reloc_evaluate(HEAD(parms), RELOCT_F));
          case 0:
            emit(i->opcode | s);
            break;

          default:
            enforce_arity(arity, 1);
          }
        }
        break;

      case INSN_CLASS_LIT4:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, reloc_evaluate(p, RELOCT_F), 0x0f);
        }
        break;

      case INSN_CLASS_LIT5:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
         if (strcasecmp(i->name, "movlb") == 0) {
            emit_check(i->opcode, reloc_evaluate(p, RELOCT_MOVLB), 0x1f);
          } else {
            emit_check(i->opcode, maybe_evaluate(p), 0x1f);
          }
        }
        break;

      case INSN_CLASS_LIT4S:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          emit_check(i->opcode, (reloc_evaluate(p, RELOCT_MOVLR) << 4), 0xf0);
        }
        break;

      case INSN_CLASS_LIT6:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          /* The literal cannot be a relocatable address */
          emit_check(i->opcode, maybe_evaluate(p), 0x3f);
        }
        break;

      case INSN_CLASS_LIT7:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          /* The literal cannot be a relocatable address */
          emit_check(i->opcode, maybe_evaluate(p), 0x7f);
        }
        break;

      case INSN_CLASS_LIT8:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          coerce_str1(p); /* literal instructions can coerce string literals */
          if (strcasecmp(i->name, "movlb") == 0) {
            emit_check(i->opcode, reloc_evaluate(p, RELOCT_MOVLB), 0xf);
          } else {
            emit_check(i->opcode, reloc_evaluate(p, RELOCT_LOW), 0xff);
          }
        }
        break;

      case INSN_CLASS_LIT8C12:
        if (enforce_arity(arity, 1)) {
          int value;

          p = HEAD(parms);
          value = reloc_evaluate(p, RELOCT_CALL);

          /* PC is 11 bits.  mpasm checks the maximum device address. */
          if (value & (~0x7ff))
            gperror(GPE_RANGE, NULL);

          if ((value & 0x600) != (r & 0x600))
            gpmessage(GPM_PAGE, NULL);

          if (value & 0x100)
            gperror(GPE_BAD_CALL_ADDR, NULL);

          emit(i->opcode | (value & 0xff));
        }
        break;

      case INSN_CLASS_LIT8C16:
        if (enforce_arity(arity, 1)) {
          int value;

          p = HEAD(parms);
          value = reloc_evaluate(p, RELOCT_LOW);

          /* PC is 16 bits.  mpasm checks the maximum device address. */
          if (value & (~0xffff))
            gperror(GPE_RANGE, NULL);

          emit(i->opcode | (value & 0xff));
        }
        break;

      case INSN_CLASS_LIT9:
        if (enforce_arity(arity, 1)) {
          int value;

          p = HEAD(parms);
          value = reloc_evaluate(p, RELOCT_GOTO);

          /* PC is 11 bits.  mpasm checks the maximum device address. */
          if (value & (~0x7ff))
            gperror(GPE_RANGE, NULL);

          if ((value & 0x600) != (r & 0x600))
            gpmessage(GPM_PAGE, NULL);

          emit(i->opcode | (value & 0x1ff));
        }
        break;

      case INSN_CLASS_LIT11:
        if (enforce_arity(arity, 1)) {
          int value;

          p = HEAD(parms);
          if (strcasecmp(i->name, "goto") == 0) {
            value = reloc_evaluate(p, RELOCT_GOTO);
          } else {
            value = reloc_evaluate(p, RELOCT_CALL);
          }

          if (state.device.class == PROC_CLASS_PIC14E) {
            /* PC is 15 bits.  mpasm checks the maximum device address. */
            if (value & (~0x7fff))
              gperror(GPE_RANGE, NULL);
            if ((value & 0x7800) != (r & 0x7800))
              gpmessage(GPM_PAGE, NULL);
          } else {
            /* PC is 13 bits.  mpasm checks the maximum device address. */
            if (value & (~0x1fff))
              gperror(GPE_RANGE, NULL);
            if ((value & 0x1800) != (r & 0x1800))
              gpmessage(GPM_PAGE, NULL);
          }

          emit(i->opcode | (value & 0x7ff));
        }
        break;

      case INSN_CLASS_LIT13:
        if (enforce_arity(arity, 1)) {
          int value;

          p = HEAD(parms);
          if (strcasecmp(i->name, "goto") == 0) {
            value = reloc_evaluate(p, RELOCT_GOTO);
          } else {
            value = reloc_evaluate(p, RELOCT_CALL);
          }

          /* PC is 16 bits.  mpasm checks the maximum device address. */
          if (value & (~0xffff))
            gperror(GPE_RANGE, NULL);

          if ((value & 0xe000) != (r & 0xe000))
            gpmessage(GPM_PAGE, NULL);

          emit(i->opcode | (value & 0x1fff));
        }
        break;

      case INSN_CLASS_LITFSR_14:
        if (enforce_arity(arity, 2)) {
          int value;
          int fsr;

          p = HEAD(parms);
          fsr = maybe_evaluate(p);
          if (fsr == 4 || fsr == 6) {
            fsr = (fsr == 6) ? 0x40 : 0x00;
            p = HEAD(TAIL(parms));
            /* the offset cannot be a relocatable address */
            value = maybe_evaluate(p);
            if (value < -32 || value > 31)
              gperror(GPE_RANGE, NULL);

            emit(i->opcode | fsr | (value & 0x3f));
          } else
            gperror(GPE_RANGE, NULL);
        }
        break;

      case INSN_CLASS_LITFSR_16:
        if (enforce_arity(arity, 2)) {
          int value;
          int fsr;

          p = HEAD(parms);
          fsr = maybe_evaluate(p);
          if ((fsr < 0) || (fsr > 2))
            gperror(GPE_RANGE, NULL);

          p = HEAD(TAIL(parms));
          /* the offset cannot be a relocatable address */
          value = maybe_evaluate(p);
          if (value & (~0x3f))
            gperror(GPE_RANGE, NULL);

          emit(i->opcode | ((fsr & 0x3) << 6) | (value & 0x3f));
        }
        break;

      case INSN_CLASS_RBRA8:
        /* used only in PROC_CLASS_PIC16E */
        if (enforce_arity(arity, 1)) {
          int offset;

          p = HEAD(parms);
          if (count_reloc(p) == 0) {
            offset = maybe_evaluate(p) - (r + 2);
            if (offset & 1) {
              gpwarning(GPW_WORD_ALIGNED, NULL);
            }
          } else {
            offset = reloc_evaluate(p, RELOCT_CONDBRA);
          }
          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -127 <= offset <= 127. */
          emit_check_relative(i->opcode, offset, 0xff, 127);
        }
        break;

      case INSN_CLASS_RBRA9:
        if (enforce_arity(arity, 1)) {
          int offset;

          p = HEAD(parms);
          if (count_reloc(p) == 0) {
            offset = maybe_evaluate(p) - (r + 1);
          } else {
            offset = reloc_evaluate(p, RELOCT_BRA);
          }
          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          /* The offset for the relative branch must be
             between -256 <= offset <= 255. */
          emit_check_relative(i->opcode, offset, 0x1ff, 255);
        }
        break;

      case INSN_CLASS_RBRA11:
        /* used only in PROC_CLASS_PIC16E */
        if (enforce_arity(arity, 1)) {
          int offset;

          p = HEAD(parms);
          if (count_reloc(p) == 0) {
            offset = maybe_evaluate(p) - (r + 2);
            if (offset & 1) {
              gpwarning(GPW_WORD_ALIGNED, NULL);
            }
          } else {
            offset = reloc_evaluate(p, RELOCT_BRA);
          }
          offset = gp_processor_org_to_byte(state.device.class, offset) >> 1;

          emit_check_relative(i->opcode, offset, 0x7ff, 0x3ff);
        }
        break;

      case INSN_CLASS_LIT20:
        if (enforce_arity(arity, 1)) {
          int dest;
          p = HEAD(parms);
          dest = reloc_evaluate(p, RELOCT_GOTO);
          dest = gp_processor_org_to_byte(state.device.class, dest) >> 1;
          emit(i->opcode | (dest & 0xff));
          reloc_evaluate(p, RELOCT_GOTO2);     /* add the second relocation */
          emit_check(0xf000, dest>>8, 0xfff);
        }
        break;

      case INSN_CLASS_CALL20:
        {
          int dest;
          int s = 0; /* By default, fast push is not used */
          struct pnode *p2; /* second parameter */

          if (arity < 1) {
            enforce_arity(arity, 2);
          } else {
            p = HEAD(parms);
            switch (arity) {
            case 2:
              p2 = HEAD(TAIL(parms));
              /* Allow "s" for fast push */
              if ((p2->tag == symbol) &&
                  (strcasecmp(p2->value.symbol, "s") == 0))
                s = 1;
              else
                s = check_flag(maybe_evaluate(p2));
              break;

            case 1:
              s = 0;
              break;

            default:
              enforce_arity(arity, 2);
            }
            dest = reloc_evaluate(p, RELOCT_CALL);
            dest = gp_processor_org_to_byte(state.device.class, dest) >> 1;
            emit(i->opcode | (s<<8) | (dest & 0xff));
            reloc_evaluate(p, RELOCT_CALL2);     /* add the second relocation */
            emit_check(0xf000, (dest>>8), 0xfff);
          }
        }
        break;

      case INSN_CLASS_FLIT12:
        {
          int k;

          if (enforce_arity(arity, 2)) {
            p = HEAD(parms);
            file = maybe_evaluate(p);
            if(file > 3)
              gperror(GPE_UNKNOWN, "FSR is out of range");

            p = HEAD(TAIL(parms));
            k = reloc_evaluate(p, RELOCT_LFSR1);
            emit_check(i->opcode | ((file & 3) << 4), (k>>8), 0xf);
            reloc_evaluate(p, RELOCT_LFSR2); /* add the second relocation */
            emit(0xf000 | (k & 0xff));
          }
        }
        break;

      case INSN_CLASS_FF:
        if (enforce_arity(arity, 2)) {
          int dest = maybe_evaluate(HEAD(TAIL(parms)));

          /* destination can't be PCL, TOSU, TOSH, TOSL */
          if ((dest == 0xff9) ||
              (dest == 0xfff) ||
              (dest == 0xffe) ||
              (dest == 0xffd)) {
            gperror(GPE_UNKNOWN, "Invalid destination");
          }

          emit_check(i->opcode, reloc_evaluate(HEAD(parms), RELOCT_FF1), 0xfff);
          emit_check(0xf000, reloc_evaluate(HEAD(TAIL(parms)), RELOCT_FF2), 0xfff);

        }
        break;

      case INSN_CLASS_FP:
        if (enforce_arity(arity, 2)) {
          int reg=0;
          file=reloc_evaluate(HEAD(parms), RELOCT_F);
          reg=reloc_evaluate(HEAD(TAIL(parms)), RELOCT_P);
          file_ok(file);
          if (reg & ~0xf1f) {
            gpwarning(GPW_RANGE, NULL);
          }
          emit(i->opcode | ( (reg  & 0x1f) << 8) |
              (file & 0xff) );
        }
        break;

      case INSN_CLASS_PF:
        if (enforce_arity(arity, 2)) {
          int reg=0;
          file=reloc_evaluate(HEAD(TAIL(parms)), RELOCT_F);
          reg=reloc_evaluate(HEAD(parms), RELOCT_P);
          file_ok(file);
          if (reg & ~0xf1f) {
            gpwarning(GPW_RANGE, NULL);
          }
          emit(i->opcode | ( (reg & 0x1f) << 8) |
              (file & 0xff) );
        }
        break;

      case INSN_CLASS_SF:
        if (enforce_arity(arity, 2)) {
          int source;
          int dest;

          p = HEAD(parms);
          if (p->tag != offset)
            gperror(GPE_MISSING_BRACKET, NULL);
          source = maybe_evaluate(p);

          p = HEAD(TAIL(parms));
          dest = maybe_evaluate(p);

          /* destination can't be PCL, TOSU, TOSH, TOSL */
          if ((dest == 0xff9) ||
              (dest == 0xfff) ||
              (dest == 0xffe) ||
              (dest == 0xffd)) {
            gperror(GPE_UNKNOWN, "Invalid destination");
          }

          emit_check(i->opcode, source, 0x7f);
          emit_check(0xf000, reloc_evaluate(HEAD(TAIL(parms)), RELOCT_FF2), 0xfff);
        }
        break;

      case INSN_CLASS_SS:
        if (enforce_arity(arity, 2)) {
          int source;
          int dest;

          p = HEAD(parms);
          if (p->tag != offset)
            gperror(GPE_MISSING_BRACKET, NULL);
          source = maybe_evaluate(p);

          p = HEAD(TAIL(parms));
          if (p->tag != offset)
            gperror(GPE_MISSING_BRACKET, NULL);
          dest = maybe_evaluate(p);

          emit_check(i->opcode, source, 0x7f);
          emit_check(0xf000, dest, 0x7f);
        }
        break;

      case INSN_CLASS_OPF5:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          if (strcasecmp(i->name, "tris") == 0) {
            file = reloc_evaluate(p, RELOCT_TRIS);
          } else {
            file = reloc_evaluate(p, RELOCT_F);
          }
          file_ok(file);
          emit(i->opcode | (file & 0x1f));
        }
        break;

      case INSN_CLASS_OPWF5:
        {
          int d = 1; /* Default destination of 1 (file) */
          struct pnode *p2; /* second parameter */

          if(arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          p = HEAD(parms);
          switch (arity) {
          case 2:
            p2 = HEAD(TAIL(parms));
            /* Allow "w" and "f" as destinations */
            if ((p2->tag == symbol) &&
                (strcasecmp(p2->value.symbol, "f") == 0))
              d = 1;
            else if ((p2->tag == symbol) &&
                     (strcasecmp(p2->value.symbol, "w") == 0))
              d = 0;
            else
              d = check_flag(maybe_evaluate(p2));
            break;

          case 1:
            d = 1;
            gpmessage(GPM_NOF,NULL);
            break;

          default:
            enforce_arity(arity, 2);
          }
          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          emit(i->opcode | (d << 5) | (file & 0x1f));
        }
        break;

      case INSN_CLASS_B5:
        {
          struct pnode *f, *b;
          int bit;

          if (enforce_arity(arity, 2)) {
            f = HEAD(parms);
            b = HEAD(TAIL(parms));
            file = reloc_evaluate(f, RELOCT_F);
            bit = maybe_evaluate(b);
            if (!((0 <= bit) && (bit <= 7)))
              gpwarning(GPW_RANGE, NULL);
            file_ok(file);
            emit(i->opcode | ((bit & 7) << 5) |(file & 0x1f));
          }
        }
        break;

      case INSN_CLASS_B8:
        {
          struct pnode *f, *b;
          int bit;

          if (enforce_arity(arity, 2)) {
            f = HEAD(parms);
            b = HEAD(TAIL(parms));
            file = reloc_evaluate(f, RELOCT_F);
            bit = maybe_evaluate(b);
            if (!((0 <= bit) && (bit <= 7)))
              gpwarning(GPW_RANGE, NULL);
            file_ok(file);
            emit(i->opcode | ((bit & 7) << 8) | (file & 0xff));
          }
        }
        break;

      case INSN_CLASS_OPF7:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          if (strcasecmp(i->name, "tris") == 0) {
            gpwarning(GPW_NOT_RECOMMENDED, NULL);
            file = reloc_evaluate(p, RELOCT_TRIS);
          } else {
            file = reloc_evaluate(p, RELOCT_F);
          }
          file_ok(file);
          emit(i->opcode | (file & 0x7f));
        }
        break;

      case INSN_CLASS_OPF8:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          emit(i->opcode | (file & 0xff));
        }
        break;

      case INSN_CLASS_OPWF7:
        {
          int d = 1; /* Default destination of 1 (file) */
          struct pnode *p2; /* second parameter */

          if(arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          p = HEAD(parms);
          switch (arity) {
          case 2:
            p2 = HEAD(TAIL(parms));
            /* Allow "w" and "f" as destinations */
            if ((p2->tag == symbol) &&
                (strcasecmp(p2->value.symbol, "f") == 0))
              d = 1;
            else if ((p2->tag == symbol) &&
                     (strcasecmp(p2->value.symbol, "w") == 0))
              d = 0;
            else
              d = check_flag(maybe_evaluate(p2));
            break;

          case 1:
            d = 1;
            gpmessage(GPM_NOF,NULL);
            break;

          default:
            enforce_arity(arity, 2);
          }
          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          emit(i->opcode | (d << 7) | (file & 0x7f));
        }
        break;

      case INSN_CLASS_OPWF8:
        {
          int d = 1; /* Default destination of 1 (file) */
          struct pnode *p2; /* second parameter */

          if(arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          p = HEAD(parms);
          switch (arity) {
          case 2:
            p2 = HEAD(TAIL(parms));
            /* Allow "w" and "f" as destinations */
            if ((p2->tag == symbol) &&
                (strcasecmp(p2->value.symbol, "f") == 0))
              d = 1;
            else if ((p2->tag == symbol) &&
                     (strcasecmp(p2->value.symbol, "w") == 0))
              d = 0;
            else
              d = check_flag(maybe_evaluate(p2));
            break;

          case 1:
            d = 1;
            gpmessage(GPM_NOF,NULL);
            break;

          default:
            enforce_arity(arity, 2);
          }
          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);
          emit(i->opcode | (d << 8) | (file & 0xff));
        }
        break;

      case INSN_CLASS_B7:
        {
          struct pnode *f, *b;
          int bit;

          if (enforce_arity(arity, 2)) {
            f = HEAD(parms);
            b = HEAD(TAIL(parms));
            file = reloc_evaluate(f, RELOCT_F);
            bit = maybe_evaluate(b);
            if (!((0 <= bit) && (bit <= 7)))
              gpwarning(GPW_RANGE, NULL);
            file_ok(file);
            emit(i->opcode | ((bit & 7) << 7) | (file & 0x7f));
          }
        }
        break;

      case INSN_CLASS_OPFA8:
        {
          int a = 0;
          struct pnode *p2; /* second parameter */

          if(arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          p = HEAD(parms);
          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);

          /* add relocation for the access bit, if necessary */
          if (arity < 2)
            reloc_evaluate(p, RELOCT_ACCESS);

          /* Default access (use the BSR unless access is to special registers) */
          /* If extended instructions are enabled, access bit should default to 1 for low-end */
          /* of Access Memory unless the file is explicitly an offset (e.g. [foo]) */
          if ((state.extended_pic16e == true) && (file <= 0x5f)) {
            if (p->tag == offset) {
              a = 0;
            } else {
              a = 1;
            }
          } else if ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) {
            a = 0;
          } else {
            a = 1;
          }

          switch (arity) {
          case 2:
            p2 = HEAD(TAIL(parms));
            /* Allow "b" for BSR to select RAM bank */
            if ((p2->tag == symbol) &&
                (strcasecmp(p2->value.symbol, "b") == 0))
              a = 1;
            else
              a = check_flag(maybe_evaluate(p2));
            break;

          case 1:
            /* use default a */
            break;

          default:
            enforce_arity(arity, 2);
          }
          emit(i->opcode | (a << 8) | (file & 0xff));
        }
        break;

      case INSN_CLASS_BA8:
        {
          struct pnode *f, *b,*par;
          int bit,a=0;

          if ((arity != 2) && (arity != 3)) {
            enforce_arity(arity, 3);
            break;
          }

          f = HEAD(parms);
          file = reloc_evaluate(f, RELOCT_F);

          if (arity == 3) {
            par = HEAD(TAIL(TAIL(parms)));

            if ((par->tag == symbol) &&
                (strcasecmp(par->value.symbol, "b") == 0))
              a = 1;
            else
              a = check_flag(maybe_evaluate(par));
          } else {
            /* Default access (use the BSR unless access is to special
               registers) */
            /* If extended instructions are enabled, access bit should default to 1 for low-end */
            /* of Access Memory unless the file is explicitly an offset (e.g. [foo]) */
            if ((state.extended_pic16e == true) && (file <= 0x5f)) {
              if (f->tag == offset) {
                a = 0;
              } else {
                a = 1;
              }
            } else if ((file < state.device.bsr_boundary) ||
                (file >= (0xf00 + state.device.bsr_boundary))) {
              a = 0;
            } else {
              a = 1;
            }
          }

          /* add relocation for the access bit, if necessary */
          if (arity < 3)
            reloc_evaluate(f, RELOCT_ACCESS);

          b = HEAD(TAIL(parms));
          bit = maybe_evaluate(b);
          if (!((0 <= bit) && (bit <= 7)))
            gpwarning(GPW_RANGE, NULL);
          file_ok(file);
          emit(i->opcode | ( a << 8) | ((bit & 7) << 9) | (file & 0xff));
        }
        break;

      case INSN_CLASS_OPWFA8:
        {
          int d = 1; /* Default destination of 1 (file) */
          int a = 0;
          struct pnode *par; /* second parameter */

          if(arity == 0) {
            enforce_arity(arity, 2);
            break;
          }

          p = HEAD(parms);
          file = reloc_evaluate(p, RELOCT_F);
          file_ok(file);

          /* add relocation for the access bit, if necessary */
          if (arity < 3)
            reloc_evaluate(p, RELOCT_ACCESS);

          /* Default access (use the BSR unless access is to special registers) */
          /* If extended instructions are enabled, access bit should default to 1 for low-end */
          /* of Access Memory */
          if ((state.extended_pic16e == true) && (file <= 0x5f)) {
            if (p->tag == offset) {
              a = 0;
            } else {
              a = 1;
            }
          } else if ((file < state.device.bsr_boundary) || (file >= (0xf00 + state.device.bsr_boundary))) {
            a = 0;
          } else {
            a = 1;
          }

          switch (arity) {
          case 3:
            par = HEAD(TAIL(TAIL(parms)));

            if ((par->tag == symbol) &&
                (strcasecmp(par->value.symbol, "b") == 0))
              a = 1;
            else
              a = check_flag(maybe_evaluate(par));
            /* fall through */
          case 2:
            par = HEAD(TAIL(parms));
            /* Allow "w" and "f" as destinations */
            if ((par->tag == symbol) &&
                (strcasecmp(par->value.symbol, "f") == 0))
              d = 1;
            else if ((par->tag == symbol) &&
                     (strcasecmp(par->value.symbol, "w") == 0))
              d = 0;
            else
              d = check_flag(maybe_evaluate(par));
            break;

          case 1:
            /* use default a and d */
            gpmessage(GPM_NOF,NULL);
            break;

          default:
            enforce_arity(arity, 3);
          }
          emit(i->opcode | (d << 9) | (a << 8) | (file & 0xff));
        }
        break;

      case INSN_CLASS_IMPLICIT:
        if (arity != 0) {
          gpwarning(GPW_EXTRANEOUS, NULL);
        }
        if ((strcasecmp(i->name, "option") == 0) &&
            (state.device.class->core_size != CORE_12BIT_MASK)){
          gpwarning(GPW_NOT_RECOMMENDED, NULL);
        }
        emit(i->opcode);
        break;

      case INSN_CLASS_TBL:
        if (enforce_arity(arity, 1)) {
          p = HEAD(parms);
          switch(maybe_evaluate(p))
            {
            case TBL_NO_CHANGE:
              emit(i->opcode);
              break;

            case TBL_POST_INC:
              emit(i->opcode | 1);
              break;

            case TBL_POST_DEC:
              emit(i->opcode | 2);
              break;

            case TBL_PRE_INC:
              emit(i->opcode | 3);
              break;

            default:
              gperror(GPE_ILLEGAL_ARGU, NULL);
            }
        }
        break;

      case INSN_CLASS_MOVINDF:
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
          /* New opcode for indexed indirect */
          /* moviw/movwi INDFn handled as moviw/movwi 0[INDFn] */
          if (fsr == 4 || fsr == 6) {
            fsr = (fsr == 6) ? 0x40 : 0x00;
            if (strcasecmp(i->name, "moviw") == 0)
              opcode = 0x3f00 | fsr;
            else
              opcode = 0x3f80 | fsr;
            emit(opcode);
          } else
            gperror(GPE_ILLEGAL_ARGU, NULL);
          break;

        case 2:
          p2 = HEAD(TAIL(parms));
          fsr = maybe_evaluate(p2);
          if (fsr == 4 || fsr == 6) {
            fsr = (fsr == 6) ? 0x04 : 0x00;
            switch (maybe_evaluate(p)) {
            case INCREMENT:
              emit(i->opcode | fsr | 0);
              break;

            case DECREMENT:
              emit(i->opcode | fsr | 1);
              break;

            case POSTINCREMENT:
              emit(i->opcode | fsr | 2);
              break;

            case POSTDECREMENT:
              emit(i->opcode | fsr | 3);
              break;
            }
          } else
            gperror(GPE_ILLEGAL_ARGU, NULL);
          break;

        case 3:
        case 4:
          p2 = HEAD(TAIL(parms));
          fsr = maybe_evaluate(p2);
          if (fsr == 4 || fsr == 6) {
            fsr = (fsr == 6) ? 0x40 : 0x00;
            switch (maybe_evaluate(p)) {
            case INDFOFFSET:
              p3 = TAIL(TAIL(parms));
              k = maybe_evaluate(p3); /* Index */
              if ((k < -32) || (k > 31))
                gperror(GPE_RANGE, NULL);
              else {
                /* New opcode for indexed indirect */
                if (strcasecmp(i->name, "moviw") == 0)
                  opcode = 0x3f00 | fsr;
                else
                  opcode = 0x3f80 | fsr;
                emit(opcode | (k & 0x3f));
              }
              break;

            default:
              gperror(GPE_ILLEGAL_ARGU, NULL);
            }
          } else
            gperror(GPE_ILLEGAL_ARGU, NULL);
        }
        break;

      case INSN_CLASS_TBL2:
        if (enforce_arity(arity, 2)) {
          int t=0; /* read low byte by default */
          struct pnode *p2; /* second parameter */

          /* "0" (lower byte) and "1" (upper byte) */
          p = HEAD(parms);
          t = check_flag(maybe_evaluate(p));

          p2 = HEAD(TAIL(parms));
          file = reloc_evaluate(p2, RELOCT_F);

          file_ok(file);
          emit(i->opcode | (t << 9) | (file & 0xff));
        }
        break;

      case INSN_CLASS_TBL3:
        if (enforce_arity(arity, 3)) {
          int inc=0,t=0;
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
          emit(i->opcode | (t << 9) | (inc << 8) | (file & 0xff));
        }
        break;

      case INSN_CLASS_FUNC:
        r = (*i->doer)(r, name, arity, parms);
        break;

      default:
        assert(0);
        break;
      }
    }
  } else {
    s = get_symbol(state.stMacros, name);
    if (s) {
      struct macro_head *h = get_symbol_annotation(s);

      /* Found the macro: execute it */
      if (asm_enabled()) {
        if ((h->defined != 1) && (state.pass == 2)) {
          gperror(GPE_UNKNOWN, "Forward references to macros are not allowed.");
        } else {
          setup_macro(h, arity, parms);
        }
      }
    } else {

      if (asm_enabled()) {
        if (state.processor_chosen == 0){
          gperror(GPE_UNDEF_PROC, NULL);
        } else {
          char mesg[80];
          snprintf(mesg, sizeof(mesg), "Unknown opcode \"%.40s\"", name);
          gperror(GPE_UNKNOWN, mesg);
        }
      }

    }
  }

  return r;
}

/************************************************************************/

/* There are several groups of operations that we handle here.  First
   is op_0: the instructions that can happen before the processor type
   is known.  Second is op_1, the set of instructions that are common
   to all processors, third is processor-family specific: op_XXX */

/* Note that instructions within each group are sorted alphabetically */

struct insn op_0[] = {
  { "access_ovr", 0, 0, INSN_CLASS_FUNC, 0, do_access_ovr },
  { "code",       0, 0, INSN_CLASS_FUNC, 0, do_code },
  { "code_pack",  0, 0, INSN_CLASS_FUNC, 0, do_code_pack },
  { "constant",   0, 0, INSN_CLASS_FUNC, 0, do_constant },
  { "else",       0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_else },
  { "end",        0, 0, INSN_CLASS_FUNC, 0, do_end },
  { "endif",      0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_endif },
  { "endm",       0, 0, INSN_CLASS_FUNC, 0, do_endm },
  { "endw",       0, 0, INSN_CLASS_FUNC, 0, do_endw },
  { "equ",        0, 0, INSN_CLASS_FUNC, 0, do_equ },
  { "error",      0, 0, INSN_CLASS_FUNC, 0, do_error },
  { "exitm",      0, 0, INSN_CLASS_FUNC, 0, do_exitm },
  { "expand",     0, 0, INSN_CLASS_FUNC, 0, do_expand },
  { "extern",     0, 0, INSN_CLASS_FUNC, 0, do_extern },
  { "errorlevel", 0, 0, INSN_CLASS_FUNC, 0, do_errlvl },
  { "global",     0, 0, INSN_CLASS_FUNC, 0, do_global },
  { "idata",      0, 0, INSN_CLASS_FUNC, 0, do_idata },
  { "idata_acs",  0, 0, INSN_CLASS_FUNC, 0, do_idata_acs },
  { "if",         0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_if },
  { "ifdef",      0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_ifdef },
  { "ifndef",     0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_ifndef },
  { "include",    0, 0, INSN_CLASS_FUNC, 0, do_include },
  { "list",       0, 0, INSN_CLASS_FUNC, 0, do_list },
  { "local",      0, 0, INSN_CLASS_FUNC, 0, do_local },
  { "macro",      0, 0, INSN_CLASS_FUNC, 0, do_macro },
  { "messg",      0, 0, INSN_CLASS_FUNC, 0, do_messg },
  { "noexpand",   0, 0, INSN_CLASS_FUNC, 0, do_noexpand },
  { "nolist",     0, 0, INSN_CLASS_FUNC, 0, do_nolist },
  { "page",       0, 0, INSN_CLASS_FUNC, 0, do_page },
  { "processor",  0, 0, INSN_CLASS_FUNC, 0, do_processor },
  { "radix",      0, 0, INSN_CLASS_FUNC, 0, do_radix },
  { "set",        0, 0, INSN_CLASS_FUNC, 0, do_set },
  { "space",      0, 0, INSN_CLASS_FUNC, 0, do_space },
  { "subtitle",   0, 0, INSN_CLASS_FUNC, 0, do_subtitle },
  { "title",      0, 0, INSN_CLASS_FUNC, 0, do_title },
  { "udata",      0, 0, INSN_CLASS_FUNC, 0, do_udata },
  { "udata_acs",  0, 0, INSN_CLASS_FUNC, 0, do_udata_acs },
  { "udata_ovr",  0, 0, INSN_CLASS_FUNC, 0, do_udata_ovr },
  { "udata_shr",  0, 0, INSN_CLASS_FUNC, 0, do_udata_shr },
  { "variable",   0, 0, INSN_CLASS_FUNC, 0, do_variable },
  { "while",      0, 0, INSN_CLASS_FUNC, 0, do_while },
  { ".def",       0, 0, INSN_CLASS_FUNC, 0, do_def },
  { ".dim",       0, 0, INSN_CLASS_FUNC, 0, do_dim },
  { ".direct",    0, 0, INSN_CLASS_FUNC, 0, do_direct },
  { ".eof",       0, 0, INSN_CLASS_FUNC, 0, do_eof },
  { ".file",      0, 0, INSN_CLASS_FUNC, 0, do_file },
  { ".ident",     0, 0, INSN_CLASS_FUNC, 0, do_ident },
  { ".line",      0, 0, INSN_CLASS_FUNC, 0, do_line },
  { ".set",       0, 0, INSN_CLASS_FUNC, 0, do_set },
  { ".type",      0, 0, INSN_CLASS_FUNC, 0, do_type },
  { "#if",        0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_if },
  { "#else",      0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_else },
  { "#endif",     0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_endif },
  { "#ifdef",     0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_ifdef },
  { "#ifndef",    0, 0, INSN_CLASS_FUNC, ATTRIB_COND, do_ifndef },
  { "#define",    0, 0, INSN_CLASS_FUNC, 0, do_define },
  { "#undefine",  0, 0, INSN_CLASS_FUNC, 0, do_undefine }
};

const int num_op_0 = TABLE_SIZE(op_0);

struct insn op_1[] = {
  { "__badram",   0, 0, INSN_CLASS_FUNC, 0, do_badram },
  { "__badrom",   0, 0, INSN_CLASS_FUNC, 0, do_badrom },
  { "__config",   0, 0, INSN_CLASS_FUNC, 0, do_config },
  { "__fuses",    0, 0, INSN_CLASS_FUNC, 0, do_config },
  { "__idlocs",   0, 0, INSN_CLASS_FUNC, 0, do_idlocs },
  { "__maxram",   0, 0, INSN_CLASS_FUNC, 0, do_maxram },
  { "__maxrom",   0, 0, INSN_CLASS_FUNC, 0, do_maxrom },
  { "bankisel",   0, 0, INSN_CLASS_FUNC, 0, do_bankisel },
  { "banksel",    0, 0, INSN_CLASS_FUNC, 0, do_banksel },
  { "CONFIG",     0, 0, INSN_CLASS_FUNC, 0, do_16_config },
  { "data",       0, 0, INSN_CLASS_FUNC, 0, do_data },
  { "da",         0, 0, INSN_CLASS_FUNC, 0, do_da },
  { "db",         0, 0, INSN_CLASS_FUNC, 0, do_db },
  { "de",         0, 0, INSN_CLASS_FUNC, 0, do_de },
  { "dt",         0, 0, INSN_CLASS_FUNC, 0, do_dt },
  { "dw",         0, 0, INSN_CLASS_FUNC, 0, do_dw },
  { "fill",       0, 0, INSN_CLASS_FUNC, 0, do_fill },
  { "org",        0, 0, INSN_CLASS_FUNC, 0, do_org },
  { "pagesel",    0, 0, INSN_CLASS_FUNC, 0, do_pagesel },
  { "pageselw",   0, 0, INSN_CLASS_FUNC, 0, do_pageselw },
  { "res",        0, 0, INSN_CLASS_FUNC, 0, do_res }
};

const int num_op_1 = TABLE_SIZE(op_1);

void opcode_init(int stage)
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
    count = base == NULL ? 0 : *state.device.class->num_instructions;
    if (state.device.class == PROC_CLASS_SX) {
      /* page instruction conflicts with the page directive */
      remove_symbol(state.stBuiltin, "page");
    }
    else if (state.device.class == PROC_CLASS_PIC16E) {
      state.c_memory_base = CONFIG1L;
      state.device.bsr_boundary = gp_processor_bsr_boundary(state.processor);

      /* The 16_bit core special macros are encoded directly into the
       * symbol table like regular instructions. */
      for (i = 0; i < num_op_18cxx_sp; i++)
        annotate_symbol( add_symbol(state.stBuiltin, op_18cxx_sp[i].name),
                         (void*)&op_18cxx_sp[i]);

      if (state.extended_pic16e) {
        /* Some 18xx devices have an extended instruction set. */
        for (i = 0; i < num_op_18cxx_ext; i++)
          annotate_symbol( add_symbol(state.stBuiltin, op_18cxx_ext[i].name),
                           (void*)&op_18cxx_ext[i]);
      }
    }
    else if (state.device.class == PROC_CLASS_PIC14E) {
      for (i = 0; i < num_op_16cxx_enh; i++)
        annotate_symbol( add_symbol(state.stBuiltin, op_16cxx_enh[i].name),
                         (void*)&op_16cxx_enh[i]);
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

  for (i = 0; i < count; i++)
    annotate_symbol(add_symbol(state.stBuiltin, base[i].name), (void*)&base[i]);

  if (stage == 1 && state.device.class == PROC_CLASS_PIC14E) {
    /* pageselw directive not supported on pic14 enhanced devices*/
    remove_symbol(state.stBuiltin, "pageselw");
  }

  if (state.processor) {
    const char *name = gp_processor_name(state.processor, 0);

    /* Special Case, Some instructions not available on 17c42 devices */
    if (strcmp(name, "pic17c42") == 0) {
      remove_symbol(state.stBuiltin, "MULWF");
      remove_symbol(state.stBuiltin, "MOVLR");
      remove_symbol(state.stBuiltin, "MULLW");
    }
    /* Special Case, Some instructions not available on 16f5x devices */
    else if (strcmp(name, "pic16f54") == 0 ||
        strcmp(name, "pic16f57") == 0 ||
        strcmp(name, "pic16f59") == 0) {
      remove_symbol(state.stBuiltin, "ADDLW");
      remove_symbol(state.stBuiltin, "SUBLW");
      remove_symbol(state.stBuiltin, "RETURN");
      remove_symbol(state.stBuiltin, "RETFIE");
    }
    else if (strcmp(name, "sx48bd") == 0 ||
        strcmp(name, "sx52bd") == 0) {
      struct symbol *mode_sym = get_symbol(state.stBuiltin, "MODE");
      if (mode_sym != NULL)
        annotate_symbol(mode_sym, &op_sx_mode);
    }
  }
}

/************************************************************************/

void begin_cblock(struct pnode *c)
{
  if (asm_enabled()) {
    state.cblock_defined = 1;
    state.cblock = maybe_evaluate(c);
  }
}

void continue_cblock(void)
{
  if (state.cblock_defined == 0)
    gpmessage(GPM_CBLOCK, NULL);
  state.cblock_defined = 1;
}

void cblock_expr(struct pnode *s)
{
  if ((asm_enabled()) && (can_evaluate_concatenation(s))) {
    set_global(evaluate_concatenation(s), state.cblock, PERMANENT, gvt_cblock);
    state.cblock++;
  }
}

void cblock_expr_incr(struct pnode *s, struct pnode *incr)
{
  if ((asm_enabled()) && (can_evaluate_concatenation(s))) {
    set_global(evaluate_concatenation(s), state.cblock, PERMANENT, gvt_cblock);
    state.cblock += maybe_evaluate(incr);
  }
}

/* ".LST" file output for gpasm
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

#include "stdhdr.h"

#include "libgputils.h"
#include "directive.h"
#include "gpasm.h"
#include "cod.h"
#include "coff.h"
#include "lst.h"

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

#define STRINGIFY(s) _str(s)
#define _str(s) #s

#define IS_EEPROM (IS_EEPROM8 || IS_EEPROM16)
#define IS_BYTE   (IS_PIC16E_CORE || IS_EEPROM)

void
lst_throw(void)
{
  if (state.lst.f != NULL) {
    state.lst.page++;
    fprintf(state.lst.f,
            "%s%s %*.*s   %-28sPAGE %2d\n%s\n%s\n",
            (state.lst.page == 1) ? "" : "\f",
            GPASM_VERSION_STRING,
            (int)(45 - sizeof(GPASM_VERSION_STRING)),
            (int)(45 - sizeof(GPASM_VERSION_STRING)),
            state.srcfilename,
            state.lst.startdate,
            state.lst.page,
            state.lst.title_name,
            state.lst.subtitle_name);
    state.lst.lineofpage = 4;
    cod_lst_line(COD_NORMAL_LST_LINE);
    cod_lst_line(COD_NORMAL_LST_LINE);
    cod_lst_line(COD_NORMAL_LST_LINE);
    state.lst.line_number += 3;
  }
}

void
lst_page_start(void)
{
  lst_throw();
  switch (state.lst.lst_state) {
  case LST_IN_MEM:
    lst_line("LOC    OBJECT CODE    LINE  SOURCE TEXT");
    lst_line("  VALUE");
    break;

  case LST_IN_SYMTAB:
    lst_line("SYMBOL TABLE");
    lst_line("%-32s  %-8s", "  LABEL", "  VALUE");
    break;

  default:
    lst_line(NULL);
    break;
  }

  lst_line(NULL);
}

static void
lst_check_page_start(void)
{
  if ((state.lst.linesperpage != 0) &&
      ((state.lst.lineofpage == 0) ||
       (state.lst.lineofpage > state.lst.linesperpage))) {
    lst_page_start();
  }
}

static int
lst_spaces(int n)
{
  int i = n;

  lst_check_page_start();
  while (i--) {
    putc(' ', state.lst.f);
  }

  return n;
}

static void
lst_eol(void)
{
  if (state.lst.f != NULL) {
    putc('\n', state.lst.f);
    state.lst.line_number++;
    state.lst.lineofpage++;
    cod_lst_line(COD_NORMAL_LST_LINE);
  }
}

void
lst_line(const char *format, ...)
{
  if (state.lst.f != NULL) {
    if (format != NULL) {
      va_list args;

      lst_check_page_start();
      va_start(args, format);
      vfprintf(state.lst.f, format, args);
      va_end(args);
    }
    lst_eol();
  }
}

void
lst_err_line(const char *type, unsigned int code, const char *format, va_list args)
{
  if (state.lst.f != NULL) {
    lst_check_page_start();
    fprintf(state.lst.f, "%s[%03d]%s: ", type, code, (strcmp(type, "Error") == 0) ? "  " : "");
    vfprintf(state.lst.f, format, args);
    lst_eol();
  }
}

/* Print part of a line. Output must not contain newline. Needs call
   to lst_eol at end of line. */
static int
lst_printf(const char *format, ...)
{
  int r = 0;

  if (state.lst.f != NULL) {
    va_list args;

    lst_check_page_start();
    va_start(args, format);
    r = vfprintf(state.lst.f, format, args);
    va_end(args);
  }
  return r;
}

void
lst_init(void)
{
  state.lst.lineofpage = 0;
  state.lst.page = 0;
  state.lst.linesperpage = 59;
  state.lst.line_number = 1;
  state.lst.memorymap = true;
  state.lst.symboltable = true;
  state.lst.lst_state = LST_IN_MEM;

  /* Determine state.startdate */
  gp_date_string(state.lst.startdate, sizeof(state.lst.startdate));

  if (!state.cmd_line.macro_expand){
    state.lst.expand = true;
  }

  state.lst.force = (state.cmd_line.lst_force) ? true : false;
  state.lst.config_address = 0;
  state.lst.title_name[0] = '\0';
  state.lst.subtitle_name[0] = '\0';
  state.lst.tabstop = 8;        /* Default tabstop every 8 */
  state.lst.line_width = 132;   /* Default line width is 132 */

  if (state.lstfile != OUT_NAMED) {
    snprintf(state.lstfilename, sizeof(state.lstfilename),
             "%s.lst", state.basefilename);
  }

  if (state.lstfile == OUT_SUPPRESS) {
    state.lst.f = NULL;
    state.lst.enabled = false;
    unlink(state.lstfilename);
  } else {
    state.lst.f = fopen(state.lstfilename, "wt");

    if (state.lst.f == NULL) {
      perror(state.lstfilename);
      exit(1);
    }
    state.lst.enabled = true;
  }

  cod_lst_line(COD_FIRST_LST_LINE);
}

void
lst_memory_map(MemBlock *m)
{
#define MEM_IS_USED(m, i)  (((m)->memory != NULL) ? (IS_BYTE ? ((m)->memory[i].data & BYTE_USED_MASK) : (((m)->memory[2 * (i)].data & BYTE_USED_MASK) || ((m)->memory[2 * (i) + 1].data & BYTE_USED_MASK))) : 0)

  int i, j, base, row_used, num_per_line, num_per_block;

  lst_line(NULL);
  lst_line(NULL);
  lst_line("MEMORY USAGE MAP ('X' = Used,  '-' = Unused)");
  lst_line(NULL);

  num_per_line = 64;
  num_per_block = 16;

  while (m) {
    unsigned int max_mem = MAX_I_MEM  >> !IS_BYTE;

    base = (m->base << I_MEM_BITS) >> !IS_BYTE;

    for (i = 0; i < max_mem; i += num_per_line) {
      row_used = 0;

      for (j = 0; j < num_per_line; j++) {
        if (MEM_IS_USED(m, i + j)) {
          row_used = 1;
          break;
        }
      }

      if (row_used) {
        if ((state.device.class != NULL) && state.show_full_addr && IS_PIC16E_CORE) {
          /* Gpasm mode: Print all address digits. */
          lst_printf("%0*X :", state.device.class->addr_digits, (i + base));
        }
        else {
          /* MPASM(X) compatible: Print only lower 4 address digits. */
          lst_printf("%04X :", (i + base) & 0xffff);
        }

        for (j = 0; j < num_per_line; j++) {
          if ((j % num_per_block) == 0) {
            lst_printf(" ");
          }
          lst_printf(MEM_IS_USED(m, i + j) ? "X" : "-");
        }

        lst_eol();
        lst_check_page_start();
      }
    }

    m = m->next;
  }

  lst_line(NULL);
  lst_line("All other memory blocks unused.");
  lst_line(NULL);

  /* it seems that MPASM includes config bytes into program memory usage
   * count for 16 bit cores. See gpasm testsuite:
   * gpasm/testsuite/gpasm.mchip/listfiles/configX.lst */
#define IS_PIC16  (IS_PIC16_CORE || IS_PIC16E_CORE)

  if (IS_EEPROM) {
    lst_line("Memory Bytes Used: %5i", b_memory_used(state.i_memory));
  }
  else {
    unsigned int used = gp_processor_byte_to_real(state.processor, ((!IS_PIC16) && (state.processor != NULL)) ?
      b_range_memory_used(state.i_memory, 0,
                          gp_processor_org_to_byte(state.device.class, state.processor->prog_mem_size)) :
      b_memory_used(state.i_memory));

    lst_line("Program Memory %s Used: %5i", IS_BYTE ? "Bytes" : "Words", used);

    if ((state.processor != NULL) && (state.processor->prog_mem_size >= 0)) {
      lst_line("Program Memory %s Free: %5u",
               IS_BYTE ? "Bytes" : "Words",
               (used <= state.processor->prog_mem_size) ? (state.processor->prog_mem_size - used) : 0);
    }
  }
  lst_line(NULL);
}

void
lst_close(void)
{
  cod_lst_line(COD_LAST_LST_LINE);

  state.lst.lst_state = LST_IN_NONE;

  if (state.lst.f != NULL) {
    lst_line(NULL);
    lst_line("Errors   : %5d", state.num.errors);
    lst_line("Warnings : %5d reported, %5d suppressed",
             state.num.warnings,
             state.num.warnings_suppressed);
    lst_line("Messages : %5d reported, %5d suppressed",
             state.num.messages,
             state.num.messages_suppressed);
    lst_line(NULL);
    putc('\f', state.lst.f);

    fclose(state.lst.f);
  }
}

/* find relocation by address */
static gp_reloc_type *
find_reloc_by_address(unsigned short address)
{
  gp_reloc_type *p;

  for (p = state.obj.section->relocations; p != NULL; p = p->next) {
    if (p->address == address) {
      break;
    }
  }

  return p;
}

/* get previous relocation type */
static unsigned short
prev_reloc_type(void)
{
  gp_reloc_type *p;

  if (state.obj.section->relocations == state.obj.section->relocations_tail) {
    return 0;
  }

  for (p = state.obj.section->relocations;
       (p->next != NULL) && (p->next != state.obj.section->relocations_tail); p = p->next)
    ;
  assert(p->next);
  return ((p->address == p->next->address) ? p->type : 0);
}

/* print word value with undefined nibbles repleced by "?" */
/* enable assertions
 * #define DO_ASSERT */
/* disable assertions */
#undef DO_ASSERT

#ifdef DO_ASSERT
#define ASSERT(expr)    assert(expr)
#else
#define ASSERT(expr)    ((void) 0)
#endif

static int
print_reloc(unsigned short type, unsigned short current_value)
{
  proc_class_t class = state.device.class;

  switch (type) {
  case RELOCT_CALL:
    if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) ||
        (class == PROC_CLASS_SX)    || (class == PROC_CLASS_PIC16E)) {
      ASSERT(0 == (current_value & 0xff));
      return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    } else if ((class == PROC_CLASS_PIC14) || (class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT(0 == (current_value & MASK_PIC14_BRANCH));
      return lst_printf("%01X??? ", (current_value >> 12) & 0x000f);
    } else if (class == PROC_CLASS_PIC16) {
      ASSERT(0 == (current_value & MASK_PIC16_BRANCH));
      return lst_printf("???? ");
    } else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_GOTO:
    if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) ||
        (class == PROC_CLASS_SX)) {
      ASSERT(0 == (current_value & MASK_PIC12_GOTO));
      return lst_printf("%01X??? ", (current_value >> 12) & 0x000f);
    } else if ((class == PROC_CLASS_PIC14) || (class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT(0 == (current_value & MASK_PIC14_BRANCH));
      return lst_printf("%01X??? ", (current_value >> 12) & 0x000f);
    } else if (class == PROC_CLASS_PIC16) {
      ASSERT(0 == (current_value & MASK_PIC16_BRANCH));
      return lst_printf("???? ");
    } else if (class == PROC_CLASS_PIC16E) {
      ASSERT(0 == (current_value & 0xff));
      return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    } else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_LOW:
    /* On non-16bits devices DB directive generates values with
     * low byte != 0 if more then one byte is defined, for example
     * DB 0x12, 0x34 generates 0x1234, so the following assertion fails
     * in such cases.
     * TODO: This should be solved in DB directive handling function
     * do_db(), file directive.c.
     * ASSERT(0 == (current_value & 0xff)); */
    return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    break;

  case RELOCT_HIGH:
  case RELOCT_LFSR2:
  case RELOCT_UPPER:
  case RELOCT_CONDBRA:
    ASSERT(0 == (current_value & 0xff));
    return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    break;

  case RELOCT_P:
    {
      char buf[5] = "????";

      if (RELOCT_F != prev_reloc_type()) {
        sprintf(&buf[2], "%02X", current_value & 0x00ff);
      }
      ASSERT(0 == (current_value & 0x1f00));
      return lst_printf("%s ", buf);
    }
    break;

  case RELOCT_BANKSEL:
    /* TODO: to be implemented */
    return lst_printf("???? ");
    break;

  case RELOCT_ALL:
    return lst_printf("???? ");
    break;

  case RELOCT_IBANKSEL:
    if (class == PROC_CLASS_PIC14) {
      ASSERT(0 == (current_value & 0x0f00));  /* 1383 or 1783 */
      return lst_printf("1?83 ");
    } else if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      return lst_printf("???? ");
    } else if (class == PROC_CLASS_PIC16) {
      ASSERT(0 == (current_value & 0x00ff));
      return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    } else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_F:
    if (class == PROC_CLASS_SX) {
      ASSERT((current_value & 0x0007) == 0);
      return lst_printf("%03X?? ", (current_value >> 4) & 0x000f);
    } else if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E)) {
      ASSERT(0 == (current_value & MASK_PIC12_FILE));
    } else if ((class == PROC_CLASS_PIC14) || (class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT(0 == (current_value & MASK_PIC14_FILE));
    } else if ((class == PROC_CLASS_PIC16) || (class == PROC_CLASS_PIC16E)) {
      ASSERT(0 == (current_value & MASK_PIC16_FILE));
    } else {
      ASSERT(0);
      return 0;
    }
    return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    break;

  case RELOCT_TRIS_3BIT:
    ASSERT(0 == (current_value & 0x0007));
    return lst_printf("%03X? ", (current_value >> 4) & 0x000f);
    break;

  case RELOCT_TRIS:
    ASSERT(0 == (current_value & 0x001f));
    return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    break;

  case RELOCT_MOVLR:
    ASSERT(0 == (current_value & 0x00f0));
    return lst_printf("%02X?%01X ", (current_value >> 8) & 0xff, current_value & 0x000f);
    break;

  case RELOCT_GOTO2:
    /* This is only used for PIC16E (pic18). */
  case RELOCT_FF1:
  case RELOCT_FF2:
    /* removed assertion since it fails during sdcc pic16 library
     * compilation: do_insn, case INSN_CLASS_FF
     * ASSERT(0 == (current_value & 0x0fff)); */
    return lst_printf("%01X??? ", (current_value >> 12) & 0x000f);
    break;

  case RELOCT_LFSR1:
    return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    break;

  case RELOCT_BRA:
    if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT(0 == (current_value & MASK_PIC14E_RBRA9));
    } else if (class == PROC_CLASS_PIC16E) {
      ASSERT(0 == (current_value & MASK_PIC16E_RBRA11));
    } else {
      ASSERT(0);
      return 0;
    }
    return lst_printf("%01X??? ", (current_value >> 12) & 0x000f);
    break;

  case RELOCT_MOVLB:
    if (class == PROC_CLASS_PIC12E) {
      ASSERT(0 == (current_value & MASK_PIC12E_BANK));
      return lst_printf("%03X? ", (current_value >> 4) & 0x0fff);
    } else if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT(0 == (current_value & MASK_PIC14E_BANK));
      return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    } else if ((class == PROC_CLASS_PIC16) || (class == PROC_CLASS_PIC16E)) {
      ASSERT(0 == (current_value & MASK_PIC16E_BANK));
      return lst_printf("%03X? ", (current_value >> 4) & 0x0fff);
    } else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_ACCESS:
    if (class == PROC_CLASS_PIC16E) {
      ASSERT(0 == (current_value & 0x00ff));
    } else {
      ASSERT(0);
      return 0;
    }
    return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    break;

  case RELOCT_PAGESEL_WREG:
  case RELOCT_PAGESEL_BITS:
    /* TODO: to be implemented */
    return lst_printf("???? ");
    break;

  case RELOCT_PAGESEL_MOVLP:
    if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT(0 == (current_value & MASK_PIC14E_PAGE));
    } else {
      ASSERT(0);
      return 0;
    }
    return lst_printf("31?? ", (current_value >> 16) & 0xff);
    break;

  /* unimplemented relocations */
  case RELOCT_PAGESEL:
  case RELOCT_SCNSZ_LOW:
  case RELOCT_SCNSZ_HIGH:
  case RELOCT_SCNSZ_UPPER:
  case RELOCT_SCNEND_LOW:
  case RELOCT_SCNEND_HIGH:
  case RELOCT_SCNEND_UPPER:
  case RELOCT_SCNEND_LFSR1:
  case RELOCT_SCNEND_LFSR2:
  default:
    return 0;
  }
}

static unsigned int
lst_data(unsigned int pos, MemBlock *m, unsigned int byte_addr,
         unsigned int bytes_emitted, unsigned short reloc_type)
{
  unsigned char emit_byte;
  unsigned short emit_word;
  int lst_bytes = 0;
  gp_boolean is_eeprom_area = (gp_processor_is_eeprom_byte_addr(state.processor, byte_addr) >= 0) ? true : false;

  /* When in a idata or byte packed section or eeprom area, print byte by byte. */
  if (IS_EEPROM8 || is_eeprom_area || (state.obj.new_sec_flags & (STYP_DATA | STYP_BPACK))) {
    while ((bytes_emitted > lst_bytes) && ((pos + 3) <= LST_LINENUM_POS)) {
      b_memory_get(m, byte_addr, &emit_byte, NULL, NULL);
      pos += lst_printf("%02X ", emit_byte);
      ++byte_addr;
      ++lst_bytes;
    }
  }
  else {    /* non-code pack section */
    /* list first byte on odd address */
    if (bytes_emitted && (byte_addr & 1)) {
      b_memory_get(m, byte_addr, &emit_byte, NULL, NULL);
      pos += lst_printf("%02X ", emit_byte);
      ++byte_addr;
      ++lst_bytes;
    }
    /* list full words */
    while (((bytes_emitted - lst_bytes) > 1) && ((pos + 5) <= LST_LINENUM_POS)) {
      state.device.class->i_memory_get(m, byte_addr, &emit_word, NULL, NULL);

      /* Display '?' for undefined bytes if it is a relocatable code. */
      if (reloc_type != 0) {
        int n = print_reloc(reloc_type, emit_word);

        pos += (n == 0) ? lst_printf("%04X ", emit_word) : n;
      }
      else {
        pos += lst_printf("%04X ", emit_word);
      }

      byte_addr += 2;
      lst_bytes += 2;
    }

    if (((bytes_emitted - lst_bytes) == 1) && ((pos + 3) <= LST_LINENUM_POS)) {
      b_memory_get(m, byte_addr, &emit_byte, NULL, NULL);
      pos += lst_printf("%02X ", emit_byte);
      ++byte_addr;
      ++lst_bytes;
    }
  }

  /* append appropriate spacing */
  lst_spaces(LST_LINENUM_POS - pos);

  return lst_bytes;
}

void
lst_format_line(const char *src_line, int value)
{
  unsigned int emitted = 0;
  unsigned int emitted_lines = 0;
  unsigned int byte_addr = 0;
  unsigned int bytes_emitted = 0;
  unsigned int lst_bytes;
  const char *addr_fmt = IS_PIC16E_CORE ? "%06X " : (IS_EEPROM ? "%04X " : "%04X   ");
#define ADDR_LEN 7
  unsigned int pos = 0;
  unsigned short reloc_type;
  MemBlock *m = state.i_memory;

  assert(src_line != NULL);

  if ((state.mode == MODE_RELOCATABLE) && (state.obj.section != NULL) &&
      (state.obj.new_sec_flags & STYP_TEXT) && (state.obj.section->relocations_tail != NULL)) {
      if ((state.obj.section->address + state.obj.section->relocations_tail->address) > state.lst.line.was_byte_addr) {
        /* already passed it, go back to the history */
        gp_reloc_type *p = find_reloc_by_address(state.lst.line.was_byte_addr);
        reloc_type = (p != NULL) ? p->type : 0;
      }
      else if ((state.obj.section->address + state.obj.section->relocations_tail->address) == state.lst.line.was_byte_addr) {
        reloc_type = state.obj.section->relocations_tail->type;
      }
      else {
        reloc_type = 0;
      }
    }
  else {
    reloc_type = 0;
  }

  switch (state.lst.line.linetype) {
  case LTY_INSN:
    emitted_lines = emitted = state.byte_addr - state.lst.line.was_byte_addr;
    break;

  case LTY_DATA:
    emitted = state.byte_addr - state.lst.line.was_byte_addr;
    if ((SECTION_FLAGS & (STYP_TEXT | (state.mpasm_compatible ? STYP_BPACK : 0))) == STYP_TEXT) {
      /* generate line numbers for data directives in program memory;
       * in mpasm compatibility mode code_pack doesn't generate line numbers */
      emitted_lines = emitted;
    }
    break;

  case LTY_RES:
    if (SECTION_FLAGS & STYP_DATA) {
      /* generate data listing for idata */
      emitted = state.byte_addr - state.lst.line.was_byte_addr;
    }
    else if (!IS_RAM_ORG) {
      /* generate line numbers for res directives in program memory */
      emitted_lines = emitted = state.byte_addr - state.lst.line.was_byte_addr;
    }
    break;

  case LTY_IDLOCS:
    /* always 8 bytes (4 words) */
    emitted = 8;
    break;

  default:
    break;
  }

  coff_linenum(emitted_lines);

  /* Don't write to file if list is disabled with NOLIST directive */
  if (!state.lst.enabled) {
    return;
  }

  switch (state.lst.line.linetype) {
  case LTY_EQU:
  case LTY_SET:
    pos += lst_printf("  %08X", value);
    lst_spaces(LST_LINENUM_POS - pos);
    break;

  case LTY_SET4:
    pos += lst_printf("  %04X", value & 0xffff);
    lst_spaces(LST_LINENUM_POS - pos);
    break;

  case LTY_ORG:
    pos += lst_printf(addr_fmt, gp_processor_byte_to_real(state.processor, state.byte_addr));
    lst_spaces(LST_LINENUM_POS - pos);
    break;

  case LTY_IDLOCS:
    /* not used for 16 bit devices, config is used */
    m = state.c_memory;
    pos += lst_printf(addr_fmt, gp_processor_byte_to_real(state.processor, state.device.id_location));
    lst_bytes = lst_data(pos, m, state.device.id_location, emitted, reloc_type);
    byte_addr = state.device.id_location + lst_bytes;
    bytes_emitted = emitted - lst_bytes;
    break;

  case LTY_DATA:
  case LTY_RES:
    pos += lst_printf(addr_fmt, state.lst.line.was_byte_addr);
    goto lst_data;

  case LTY_INSN:
    pos += lst_printf(addr_fmt, gp_processor_byte_to_real(state.processor, state.lst.line.was_byte_addr));

lst_data:

    lst_bytes = lst_data(pos, m, state.lst.line.was_byte_addr, emitted, reloc_type);
    byte_addr = state.lst.line.was_byte_addr + lst_bytes;
    bytes_emitted = emitted - lst_bytes;
    break;

  case LTY_CONFIG:
    if (IS_PIC16E_CORE) {
      /* config data is byte addressable, but we only want to print
         words in the list file. */
      if (state.lst.config_address == CONFIG4L) {
        /* Special case */
        unsigned short word;

        state.device.class->i_memory_get(state.c_memory,
                                         state.lst.config_address, &word, NULL, NULL);
        pos += lst_printf(addr_fmt, state.lst.config_address);
        pos += lst_printf("%04X", word);
        lst_spaces(LST_LINENUM_POS - pos);
      } else if ((state.lst.config_address & 1) == 0) {
        /* if it is an even address don't print anything */
        lst_spaces(LST_LINENUM_POS);
      } else {
        unsigned short word;

        state.device.class->i_memory_get(state.c_memory,
                                         state.lst.config_address - 1, &word, NULL, NULL);
        pos += lst_printf(addr_fmt, state.lst.config_address - 1);
        pos += lst_printf("%04X", word);
        lst_spaces(LST_LINENUM_POS - pos);
      }
    } else {
      unsigned short word;

      state.device.class->i_memory_get(state.c_memory, state.lst.config_address, &word, NULL, NULL);
      pos += lst_printf(addr_fmt, gp_processor_byte_to_real(state.processor,
                        state.lst.config_address));
      pos += lst_printf("%04X", word);
      lst_spaces(LST_LINENUM_POS - pos);
    }
    break;

  case LTY_SEC:
  case LTY_DIR:
  case LTY_NONE:
  default:
    lst_spaces(LST_LINENUM_POS);
    break;
  }

  if (state.stGlobal == state.stTop) {
    lst_printf("%05d ", state.src->line_number);
  } else {
    lst_printf("    M ");
  }

  /* Now copy source line to listing, expanding tabs as required */
  {
    int src_column = 0;           /* current source line column */
    int lst_column = LST_SRC_POS; /* current listing column after the SRC_POS */
    const char *p = src_line;

    while (*p != '\0') {
      if (*p == '\t') {
        int len = state.lst.tabstop - (src_column % state.lst.tabstop);

        while (len--) {
          if (lst_column >= state.lst.line_width) {
            lst_eol();
            lst_spaces(LST_SRC_POS);
            lst_column = LST_SRC_POS;
          }
          ++lst_column;
          ++src_column;
          putc(' ', state.lst.f);
        }
      } else {
        if (lst_column >= state.lst.line_width) {
          lst_eol();
          lst_spaces(LST_SRC_POS);
          lst_column = LST_SRC_POS;
        }
        ++lst_column;
        ++src_column;
        putc(*p, state.lst.f);
      }
      ++p;
    }
  }

  /* Tell the .cod file that the next line(s) has an opcode(s) */
  state.cod.emitting = emitted;

  lst_eol();

  if (bytes_emitted > 0) {
    while (bytes_emitted > 0) {
      /* data left to print on separate lines */

      pos = lst_spaces(ADDR_LEN);
      lst_bytes = lst_data(pos, m, byte_addr, bytes_emitted, reloc_type);
      byte_addr += lst_bytes;
      bytes_emitted -= lst_bytes;
      lst_eol();
    }
    state.cod.emitting = 0;
  }
}

static void
cod_symbol_table(void)
{
  int i;
  struct symbol **lst, **ps, *s;

  ps = lst = GP_Malloc(state.stGlobal->count * sizeof(struct symbol *));

  for (i = 0; i < HASH_SIZE; i++) {
    for (s = state.stGlobal->hash_table[i]; s != NULL; s = s->next) {
      if (s != NULL) {
        *ps++ = s;
      }
    }
  }

  assert(ps == &lst[state.stGlobal->count]);

  qsort(lst, state.stGlobal->count, sizeof(struct symbol *), symbol_compare);

  cod_write_symbols(lst, state.stGlobal->count);

  free(lst);
}

/* append the symbol table to the .lst file */
struct lst_symbol_s {
  const struct symbol *sym;
  enum lst_sym_type_e {
    LST_SYMBOL,
    LST_DEFINE,
    LST_MACRO
  } type;
};

static int
lst_symbol_compare(const void *p0, const void *p1)
{
  return strcmp(((const struct lst_symbol_s *)p0)->sym->name,
                ((const struct lst_symbol_s *)p1)->sym->name);
}

void
lst_symbol_table(void)
{
  int i;
  struct lst_symbol_s *lst, *ps;
  struct symbol *s;
  int count = state.stGlobal->count + state.stDefines->count + state.stMacros->count;
  state.lst.lst_state = LST_IN_SYMTAB;

  lst_line("SYMBOL TABLE");
  lst_line("  LABEL                             VALUE");
  lst_line(NULL);

  cod_symbol_table();

  ps = lst = GP_Malloc(count * sizeof(struct lst_symbol_s));

  for (i = 0; i < HASH_SIZE; i++) {
    for (s = state.stGlobal->hash_table[i]; s != NULL; s = s->next) {
      ps->sym = s;
      ps->type = LST_SYMBOL;
      ++ps;
    }
  }

  for (i = 0; i < HASH_SIZE; i++) {
    for (s = state.stDefines->hash_table[i]; s != NULL; s = s->next) {
      ps->sym = s;
      ps->type = LST_DEFINE;
      ++ps;
    }
  }

  for (i = 0; i < HASH_SIZE; i++) {
    for (s = state.stMacros->hash_table[i]; s != NULL; s = s->next) {
      ps->sym = s;
      ps->type = LST_MACRO;
      ++ps;
    }
  }

  assert(ps == &lst[count]);

  qsort(lst, count, sizeof(struct lst_symbol_s), lst_symbol_compare);

  for (i = 0; i < count; i++) {
    const void *p = get_symbol_annotation(lst[i].sym);
    const char *name = get_symbol_name(lst[i].sym);

    switch (lst[i].type) {
    case LST_SYMBOL:
      /* symbol */
      lst_line("%-32s  %08X", name, (p != NULL) ? ((const struct variable *)p)->value : 0);
      break;

    case LST_DEFINE:
      /* define */
      if (p != NULL) {
        assert(((const struct pnode *)p)->tag == PTAG_LIST);
        assert(HEAD((const struct pnode *)p)->tag == PTAG_STRING);
      }
      lst_line("%-32s  %s", name, (p != NULL) ? HEAD((const struct pnode *)p)->value.string : "");
      break;

    case LST_MACRO:
      /* define */
      lst_line("%-32s  ", name);
      break;
    }
  }
  free(lst);
}


/*
 * Preprocessed file generator
 */

void
preproc_init(void)
{
  const char *name = state.preproc.preprocfilename;

  if (name != NULL) {
    if ((name[0] == '-') && (name[1] == '\0')) {
      state.preproc.f = stdout;
    }
    else if ((state.preproc.f = fopen(name, "wt")) == NULL) {
      perror(name);
      exit(1);
    }
  }
}

void
preproc_emit(void)
{
  if (state.preproc.f != NULL) {
    if (state.preproc.do_emit && asm_enabled()) {
      fprintf(state.preproc.f, "%s\n", state.preproc.curr_src_line.line);
    }
    else {
      state.preproc.do_emit = true;
    }
  }
}

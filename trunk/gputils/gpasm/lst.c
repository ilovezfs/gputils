/* ".LST" file output for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin
   Copyright (C) 2012 Borut Razem

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
#include "directive.h"
#include "gpasm.h"
#include "cod.h"
#include "coff.h"
#include "lst.h"

#if !HAVE_DECL_STRVERSCMP
#include "libiberty.h"
#endif

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

#define STRINGIFY(s) _str(s)
#define _str(s) #s

#define IS_EEPROM (IS_EEPROM8 || IS_EEPROM16)
#define IS_BYTE   (IS_PIC16E_CORE || IS_EEPROM)

typedef struct {
  const symbol_t *sym;
  enum lst_sym_type_e {
    LST_SYMBOL,
    LST_DEFINE,
    LST_MACRO
  } type;
} lst_symbol_t;

/*------------------------------------------------------------------------------------------------*/

static void
_lst_check_page_start(void)
{
  if ((state.lst.linesperpage != 0) &&
      ((state.lst.lineofpage == 0) || (state.lst.lineofpage > state.lst.linesperpage))) {
    lst_page_start();
  }
}

/*------------------------------------------------------------------------------------------------*/

static unsigned int
_lst_spaces(unsigned int n)
{
  unsigned int i = n;

  _lst_check_page_start();
  while (i--) {
    putc(' ', state.lst.f);
  }

  return n;
}

/*------------------------------------------------------------------------------------------------*/

static void
_lst_eol(void)
{
  if (state.lst.f != NULL) {
    putc('\n', state.lst.f);
    state.lst.line_number++;
    state.lst.lineofpage++;
    cod_lst_line(COD_NORMAL_LST_LINE);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Print part of a line. Output must not contain newline. Needs call to lst_eol at end of line. */

static unsigned int
_lst_printf(const char *format, ...)
{
  int r = 0;

  if (state.lst.f != NULL) {
    va_list args;

    _lst_check_page_start();
    va_start(args, format);
    r = vfprintf(state.lst.f, format, args);
    va_end(args);
  }
  assert(r >= 0);
  return (unsigned int)r;
}

/*------------------------------------------------------------------------------------------------*/

/* find relocation by address */

static gp_reloc_t *
_find_reloc_by_address(uint16_t address)
{
  gp_reloc_t *p;

  for (p = state.obj.section->relocation_list; p != NULL; p = p->next) {
    if (p->address == address) {
      break;
    }
  }

  return p;
}

/*------------------------------------------------------------------------------------------------*/

/* get previous relocation type */

static uint16_t
_prev_reloc_type(void)
{
  gp_reloc_t *p;

  if (state.obj.section->relocation_list == state.obj.section->relocation_list_tail) {
    return 0;
  }

  p = state.obj.section->relocation_list_tail->prev;
  assert(p != NULL);
  return ((p->address == p->next->address) ? p->type : 0);
}

/*------------------------------------------------------------------------------------------------*/

/* print word value with undefined nibbles replaced by "?" */
/* enable assertions
 * #define DO_ASSERT */
/* disable assertions */
#undef DO_ASSERT

#ifdef DO_ASSERT
#define ASSERT(expr)    assert(expr)
#else
#define ASSERT(expr)    ((void)0)
#endif

static unsigned int
_print_reloc(uint16_t type, uint16_t current_value, unsigned int index)
{
  proc_class_t class = state.device.class;

  switch (type) {
  case RELOCT_CALL:
    if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I) ||
        (class == PROC_CLASS_SX)    || (class == PROC_CLASS_PIC16E)) {
      ASSERT((current_value & PIC12_BMSK_CALL) == 0);
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else if ((class == PROC_CLASS_PIC14) || (class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT((current_value & PIC14_BMSK_CALL) == 0);
      return _lst_printf("%X??? ", (current_value & 0xf000) >> 12);
    }
    else if (class == PROC_CLASS_PIC16) {
      ASSERT((current_value & PIC16_BMSK_CALL) == 0);
      return _lst_printf("???? ");
    }
    else if (class == PROC_CLASS_PIC16E) {
      ASSERT((current_value & PIC16E_BMSK_CALL0) == 0);
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_GOTO:
    if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I) ||
        (class == PROC_CLASS_SX)) {
      ASSERT((current_value & PIC12_BMSK_GOTO) == 0);
      return _lst_printf("%X??? ", (current_value & 0xf000) >> 12);
    }
    else if ((class == PROC_CLASS_PIC14) || (class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT((current_value & PIC14_BMSK_GOTO) == 0);
      return _lst_printf("%X??? ", (current_value & 0xf000) >> 12);
    }
    else if (class == PROC_CLASS_PIC16) {
      ASSERT((current_value & PIC16_BMSK_GOTO) == 0);
      return _lst_printf("???? ");
    }
    else if (class == PROC_CLASS_PIC16E) {
      ASSERT((current_value & PIC16E_BMSK_GOTO0) == 0);
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else {
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
     * ASSERT((current_value & 0xff) == 0); */
    return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    break;

  case RELOCT_HIGH:
  case RELOCT_LFSR2:
  case RELOCT_UPPER:
  case RELOCT_CONDBRA:
    ASSERT((current_value & 0xff) == 0);
    return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    break;

  case RELOCT_P:
    {
      char buf[5] = "????";

      if (RELOCT_F != _prev_reloc_type()) {
        sprintf(&buf[2], "%02X", current_value & 0x00ff);
      }
      ASSERT((current_value & 0x1f00) == 0);
      return _lst_printf("%s ", buf);
    }
    break;

  case RELOCT_BANKSEL:
    if (class == PROC_CLASS_PIC12) {
      ASSERT((current_value & PIC12_BMSK_BxF) == 0);  /* 04A4-04E4 or 05A4-05E4 */
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else if ((class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
      ASSERT((current_value & PIC12E_BMSK_MOVLB) == 0);
      return _lst_printf("%03X? ", (current_value & 0x0fff0) >> 4);
    }
    else if (class == PROC_CLASS_PIC14) {
      ASSERT((current_value & PIC14_BMSK_BxF) == 0);  /* 1283-1303 or 1683-1703 */
      return _lst_printf("%X??? ", (current_value & 0xf000) >> 12);
    }
    else if (class == PROC_CLASS_PIC14E) {
      ASSERT((current_value & PIC14E_BMSK_MOVLB) == 0);  /* 0020-003F */
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else if (class == PROC_CLASS_PIC14EX) {
      ASSERT((current_value & PIC14EX_BMSK_MOVLB) == 0);  /* 0140-017F */
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else if (class == PROC_CLASS_PIC16) {
      ASSERT((current_value & PIC16_BMSK_MOVLB) == 0);  /* 0100-010F */
      return _lst_printf("%03X? ", (current_value & 0xfff0) >> 4);
    }
    else if (class == PROC_CLASS_PIC16E) {
      ASSERT((current_value & PIC16E_BMSK_MOVLB) == 0);  /* 0100-010F */
      return _lst_printf("%03X? ", (current_value & 0xfff0) >> 4);
    }
    return _lst_printf("???? ");
    break;

  case RELOCT_ALL:
    return _lst_printf("???? ");
    break;

  case RELOCT_IBANKSEL:
    if (class == PROC_CLASS_PIC14) {
      ASSERT((current_value & PIC12_BMSK_BxF) == 0);
      return _lst_printf("1?83 ");  /* 1383 or 1783 */
    }
    else if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT((current_value & PIC12_BMSK_BxF) == 0);
      return _lst_printf("%X??? ", (current_value & 0xf000) >> 12);  /* 120A-118A or 160A-158A */
    }
    else if (class == PROC_CLASS_PIC16) {
      ASSERT((current_value & 0x00ff) == 0);
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_F:
    if (class == PROC_CLASS_SX) {
      ASSERT((current_value & 0x0007) == 0);
      return _lst_printf("%03X? ", (current_value & 0xfff0) >> 4);
    }
    else if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
      ASSERT((current_value & MASK_PIC12_FILE) == 0);
    }
    else if ((class == PROC_CLASS_PIC14) || (class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT((current_value & MASK_PIC14_FILE) == 0);
    }
    else if ((class == PROC_CLASS_PIC16) || (class == PROC_CLASS_PIC16E)) {
      ASSERT((current_value & MASK_PIC16_FILE) == 0);
    }
    else {
      ASSERT(0);
      return 0;
    }
    return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    break;

  case RELOCT_TRIS_3BIT:
    ASSERT((current_value & PIC12_BMSK_TRIS) == 0);
    return _lst_printf("%03X? ", (current_value & 0x00f0) >> 4);
    break;

  case RELOCT_TRIS:
    ASSERT((current_value & 0x001f) == 0);
    return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    break;

  case RELOCT_MOVLR:
    if (class == PROC_CLASS_PIC16) {
      ASSERT((current_value & PIC16_BMSK_MOVLR) == 0);
      return _lst_printf("%02X?%X ", (current_value & 0xff00) >> 8, current_value & 0x000f);  /* 010.-01F. */
    }
    else {
      ASSERT(0);
      return 0;
    }

  case RELOCT_MOVLB:
    if ((class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
      ASSERT((current_value & PIC12E_BMSK_MOVLB) == 0);
      return _lst_printf("%03X? ", (current_value & 0xfff0) >> 4);
    }
    else if (class == PROC_CLASS_PIC14E) {
      ASSERT((current_value & PIC14E_BMSK_MOVLB) == 0);
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else if (class == PROC_CLASS_PIC14EX) {
      ASSERT((current_value & PIC14EX_BMSK_MOVLB) == 0);
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else if (class == PROC_CLASS_PIC16) {
      ASSERT((current_value & PIC16_BMSK_MOVLB) == 0);
      return _lst_printf("%03X? ", (current_value & 0xfff0) >> 4);  /* 01.0-01.F */
    }
    else if (class == PROC_CLASS_PIC16E) {
      ASSERT((current_value & PIC16E_BMSK_MOVLB) == 0);
      return _lst_printf("%03X? ", (current_value & 0xfff0) >> 4);
    }
    else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_GOTO2:
    /* This is only used for PIC16E (pic18). */
  case RELOCT_FF1:
  case RELOCT_FF2:
    /* removed assertion since it fails during sdcc pic16 library
     * compilation: do_insn, case INSN_CLASS_FF
     * ASSERT((current_value & 0x0fff) == 0); */
    return _lst_printf("%X??? ", (current_value & 0xf000) >> 12);
    break;

  case RELOCT_LFSR1:
    return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    break;

  case RELOCT_BRA:
    if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT((current_value & PIC14E_BMSK_RBRA9) == 0);
    }
    else if (class == PROC_CLASS_PIC16E) {
      ASSERT((current_value & PIC16E_BMSK_RBRA11) == 0);
    }
    else {
      ASSERT(0);
      return 0;
    }
    return _lst_printf("%X??? ", (current_value & 0xf000) >> 12);
    break;

  case RELOCT_ACCESS:
    if (class == PROC_CLASS_PIC16E) {
      ASSERT((current_value & 0x00ff) == 0);
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_PAGESEL_WREG:
    if (index == 0) {
      /* movlw PAGE_VALUE */
      if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
        ASSERT((current_value & PIC12_BMSK_MOVLW) == 0);
        return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8); /* 0C00 -- movlw ... */
      }
      else if (class == PROC_CLASS_PIC14) {
        ASSERT((current_value & PIC14_BMSK_MOVLW) == 0);
        return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8); /* 3000 -- movlw ... */
      }
      else {
        ASSERT(0);
        return 0;
      }
    }
    else {
      /* movwf register */
      if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
        ASSERT((current_value & PIC12_BMSK_FILE) == 0);
        return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8); /* 0023 -- movwf STATUS */
      }
      else if (class == PROC_CLASS_PIC14) {
        ASSERT((current_value & PIC14_BMSK_FILE) == 0);
        return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8); /* 008A -- movwf PCLATH */
      }
      else {
        ASSERT(0);
        return 0;
      }
    }
    break;

  case RELOCT_PAGESEL_BITS:
    if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
      ASSERT((current_value & PIC12_BMSK_BxF) == 0);
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);  /* 04A3-04E3 or 05A3-05E3 */
    }
    else if (class == PROC_CLASS_PIC14) {
      ASSERT((current_value & PIC14_BMSK_BxF) == 0);
      return _lst_printf("%X??? ", (current_value & 0xf000) >> 12);  /* 120A-118A or 160A-158A */
    }
    else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_PAGESEL_MOVLP:
    if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
      ASSERT((current_value & PIC14E_BMSK_MOVLP) == 0);
      return _lst_printf("%02X?? ", (current_value & 0xff00) >> 8);
    }
    else {
      ASSERT(0);
      return 0;
    }
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

/*------------------------------------------------------------------------------------------------*/

static unsigned int
_lst_data(unsigned int pos, MemBlock_t *m, unsigned int byte_addr, unsigned int bytes_emitted, uint16_t reloc_type)
{
  uint8_t      emit_byte;
  uint16_t     emit_word;
  unsigned int start_addr;
  unsigned int lst_bytes;
  gp_boolean   is_eeprom_area;
  unsigned int n;

  lst_bytes      = 0;
  is_eeprom_area = (gp_processor_is_eeprom_byte_addr(state.processor, byte_addr) >= 0) ? true : false;
  /* When in a idata or byte packed section or eeprom area, print byte by byte. */
  if (IS_EEPROM8 || is_eeprom_area || (state.obj.new_sect_flags & (STYP_DATA | STYP_BPACK))) {
    while ((bytes_emitted > lst_bytes) && ((pos + 3) <= LST_LINENUM_POS)) {
      b_memory_get(m, byte_addr, &emit_byte, NULL, NULL);
      pos += _lst_printf("%02X ", emit_byte);
      ++byte_addr;
      ++lst_bytes;
    }
  }
  else {    /* non-code pack section */
    /* list first byte on odd address */
    if ((bytes_emitted != 0) && (byte_addr & 1)) {
      b_memory_get(m, byte_addr, &emit_byte, NULL, NULL);
      pos += _lst_printf("%02X ", emit_byte);
      ++byte_addr;
      ++lst_bytes;
    }
    /* list full words */
    start_addr = byte_addr;
    while (((bytes_emitted - lst_bytes) > 1) && ((pos + 5) <= LST_LINENUM_POS)) {
      state.device.class->i_memory_get(m, byte_addr, &emit_word, NULL, NULL);

      /* Display '?' for undefined bytes if it is a relocatable code. */
      if (reloc_type != 0) {
        n = _print_reloc(reloc_type, emit_word, (byte_addr - start_addr) / 2);

        pos += (n == 0) ? _lst_printf("%04X ", emit_word) : n;
      }
      else {
        pos += _lst_printf("%04X ", emit_word);
      }

      byte_addr += 2;
      lst_bytes += 2;
    }

    if (((bytes_emitted - lst_bytes) == 1) && ((pos + 3) <= LST_LINENUM_POS)) {
      b_memory_get(m, byte_addr, &emit_byte, NULL, NULL);
      pos += _lst_printf("%02X ", emit_byte);
      ++byte_addr;
      ++lst_bytes;
    }
  }

  /* append appropriate spacing */
  _lst_spaces(LST_LINENUM_POS - pos);

  return lst_bytes;
}

/*------------------------------------------------------------------------------------------------*/

static void
_cod_symbol_table(void)
{
  const symbol_t **lst;
  size_t           sym_count;

  sym_count = sym_get_symbol_count(state.stGlobal);

  if (sym_count == 0) {
    return;
  }

  lst = sym_clone_symbol_array(state.stGlobal, sym_compare_fn);
  assert(lst != NULL);

  cod_write_symbols(lst, sym_count);
  free(lst);
}

/*------------------------------------------------------------------------------------------------*/

void
lst_init(void)
{
  state.lst.lineofpage   = 0;
  state.lst.page         = 0;
  state.lst.linesperpage = 59;
  state.lst.line_number  = 1;
  state.lst.memorymap    = true;
  state.lst.symboltable  = true;
  state.lst.lst_state    = LST_IN_MEM;

  /* Determine state.startdate. */
  gp_date_string(state.lst.startdate, sizeof(state.lst.startdate));

  if (!state.cmd_line.macro_expand){
    state.lst.expand = true;
  }

  state.lst.force = (state.cmd_line.lst_force) ? true : false;
  state.lst.config_address   = 0;
  state.lst.title_name[0]    = '\0';
  state.lst.subtitle_name[0] = '\0';
  state.lst.tabstop          = 8;   /* Default tabstop every 8. */
  state.lst.line_width       = 132; /* Default line width is 132. */

  if (state.lstfile != OUT_NAMED) {
    snprintf(state.lstfilename, sizeof(state.lstfilename), "%s.lst", state.basefilename);
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

/*------------------------------------------------------------------------------------------------*/

void
lst_close(void)
{
  cod_lst_line(COD_LAST_LST_LINE);

  state.lst.lst_state = LST_IN_NONE;

  if (state.lst.f != NULL) {
    lst_line(NULL);
    lst_line("Errors   : %5d", state.num.errors);
    lst_line("Warnings : %5d reported, %5d suppressed", state.num.warnings, state.num.warnings_suppressed);
    lst_line("Messages : %5d reported, %5d suppressed", state.num.messages, state.num.messages_suppressed);
    lst_line(NULL);
    putc('\f', state.lst.f);

    fclose(state.lst.f);
  }
}

/*------------------------------------------------------------------------------------------------*/

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

/*------------------------------------------------------------------------------------------------*/

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

/*------------------------------------------------------------------------------------------------*/

void
lst_line(const char *format, ...)
{
  if (state.lst.f != NULL) {
    if (format != NULL) {
      va_list args;

      _lst_check_page_start();
      va_start(args, format);
      vfprintf(state.lst.f, format, args);
      va_end(args);
    }
    _lst_eol();
  }
}

/*------------------------------------------------------------------------------------------------*/

void
lst_err_line(const char *type, unsigned int code, const char *format, va_list args)
{
  if (state.lst.f != NULL) {
    _lst_check_page_start();
    fprintf(state.lst.f, "%s[%03d]%s: ", type, code, (strcmp(type, "Error") == 0) ? "  " : "");
    vfprintf(state.lst.f, format, args);
    _lst_eol();
  }
}

/*------------------------------------------------------------------------------------------------*/

void
lst_memory_map(MemBlock_t *m)
{
#define MEM_IS_USED(m, i)  (((m)->memory != NULL) ? (IS_BYTE ? ((m)->memory[i].data & BYTE_USED_MASK) : (((m)->memory[2 * (i)].data & BYTE_USED_MASK) || ((m)->memory[2 * (i) + 1].data & BYTE_USED_MASK))) : 0)

  int          i;
  int          j;
  int          base;
  int          row_used;
  int          num_per_line;
  int          num_per_block;
  unsigned int max_mem;

  lst_line(NULL);
  lst_line(NULL);
  lst_line("MEMORY USAGE MAP ('X' = Used,  '-' = Unused)");
  lst_line(NULL);

  num_per_line  = 64;
  num_per_block = 16;

  while (m != NULL) {
    max_mem = I_MEM_MAX >> !IS_BYTE;
    base    = IMemAddrFromBase(m->base) >> !IS_BYTE;

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
          _lst_printf("%0*X :", state.device.class->addr_digits, (i + base));
        }
        else {
          /* MPASM(X) compatible: Print only lower 4 address digits. */
          _lst_printf("%04X :", (i + base) & 0xffff);
        }

        for (j = 0; j < num_per_line; j++) {
          if ((j % num_per_block) == 0) {
            _lst_printf(" ");
          }
          _lst_printf(MEM_IS_USED(m, i + j) ? "X" : "-");
        }

        _lst_eol();
        _lst_check_page_start();
      }
    }

    m = m->next;
  }

  lst_line(NULL);
  lst_line("All other memory blocks unused.");
  lst_line("");

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
  lst_line("");
}

/*------------------------------------------------------------------------------------------------*/

void
lst_format_line(const char *src_line, unsigned int value)
{
  unsigned int  emitted = 0;
  unsigned int  emitted_lines = 0;
  unsigned int  byte_addr = 0;
  unsigned int  bytes_emitted = 0;
  unsigned int  lst_bytes;
  const char   *addr_fmt = IS_PIC16E_CORE ? "%06X " : (IS_EEPROM ? "%04X " : "%04X   ");
#define ADDR_LEN 7
  unsigned int  pos = 0;
  uint16_t      reloc_type;
  MemBlock_t   *m = state.i_memory;
  uint16_t      word;
  gp_reloc_t   *p;

  assert(src_line != NULL);

  if ((state.mode == MODE_RELOCATABLE) && (state.obj.section != NULL) &&
      (state.obj.new_sect_flags & STYP_TEXT) && (state.obj.section->relocation_list_tail != NULL)) {
      if ((state.obj.section->address + state.obj.section->relocation_list_tail->address) > state.lst.line.was_byte_addr) {
        /* already passed it, go back to the history */
        p = _find_reloc_by_address(state.lst.line.was_byte_addr);
        reloc_type = (p != NULL) ? p->type : 0;
      }
      else if ((state.obj.section->address + state.obj.section->relocation_list_tail->address) == state.lst.line.was_byte_addr) {
        reloc_type = state.obj.section->relocation_list_tail->type;
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
    pos += _lst_printf("  %08X", value);
    _lst_spaces(LST_LINENUM_POS - pos);
    break;

  case LTY_SET4:
    pos += _lst_printf("  %04X", value & 0xffff);
    _lst_spaces(LST_LINENUM_POS - pos);
    break;

  case LTY_ORG:
    pos += _lst_printf(addr_fmt, gp_processor_byte_to_real(state.processor, state.byte_addr));
    _lst_spaces(LST_LINENUM_POS - pos);
    break;

  case LTY_IDLOCS:
    /* not used for 16 bit devices, config is used */
    m = state.c_memory;
    pos += _lst_printf(addr_fmt, gp_processor_byte_to_real(state.processor, state.device.id_location));
    lst_bytes = _lst_data(pos, m, state.device.id_location, emitted, reloc_type);
    byte_addr = state.device.id_location + lst_bytes;
    bytes_emitted = emitted - lst_bytes;
    break;

  case LTY_DATA:
  case LTY_RES:
    pos += _lst_printf(addr_fmt, state.lst.line.was_byte_addr);
    goto lst_data;

  case LTY_INSN:
    pos += _lst_printf(addr_fmt, gp_processor_byte_to_real(state.processor, state.lst.line.was_byte_addr));

lst_data:

    lst_bytes = _lst_data(pos, m, state.lst.line.was_byte_addr, emitted, reloc_type);
    byte_addr = state.lst.line.was_byte_addr + lst_bytes;
    bytes_emitted = emitted - lst_bytes;
    break;

  case LTY_CONFIG:
    if (IS_PIC16E_CORE) {
      /* config data is byte addressable, but we only want to print
         words in the list file. */
      if (state.lst.config_address == CONFIG4L) {
        /* Special case */
        state.device.class->i_memory_get(state.c_memory, state.lst.config_address, &word, NULL, NULL);
        pos += _lst_printf(addr_fmt, state.lst.config_address);
        pos += _lst_printf("%04X", word);
        _lst_spaces(LST_LINENUM_POS - pos);
      } else if ((state.lst.config_address & 1) == 0) {
        /* if it is an even address don't print anything */
        _lst_spaces(LST_LINENUM_POS);
      } else {
        state.device.class->i_memory_get(state.c_memory, state.lst.config_address - 1, &word, NULL, NULL);
        pos += _lst_printf(addr_fmt, state.lst.config_address - 1);
        pos += _lst_printf("%04X", word);
        _lst_spaces(LST_LINENUM_POS - pos);
      }
    } else {
      state.device.class->i_memory_get(state.c_memory, state.lst.config_address, &word, NULL, NULL);
      pos += _lst_printf(addr_fmt, gp_processor_byte_to_real(state.processor, state.lst.config_address));
      pos += _lst_printf("%04X", word);
      _lst_spaces(LST_LINENUM_POS - pos);
    }
    break;

  case LTY_SEC:
  case LTY_DIR:
  case LTY_NONE:
  default:
    _lst_spaces(LST_LINENUM_POS);
    break;
  }

  if (state.stGlobal == state.stTop) {
    _lst_printf("%05d ", state.src->line_number);
  } else {
    _lst_printf("    M ");
  }

  /* Now copy source line to listing, expanding tabs as required */
  {
    int         src_column = 0;           /* current source line column */
    int         lst_column = LST_SRC_POS; /* current listing column after the SRC_POS */
    const char *p = src_line;

    while (*p != '\0') {
      if (*p == '\t') {
        int len = state.lst.tabstop - (src_column % state.lst.tabstop);

        while (len--) {
          if (lst_column >= state.lst.line_width) {
            _lst_eol();
            _lst_spaces(LST_SRC_POS);
            lst_column = LST_SRC_POS;
          }
          ++lst_column;
          ++src_column;
          putc(' ', state.lst.f);
        }
      } else {
        if (lst_column >= state.lst.line_width) {
          _lst_eol();
          _lst_spaces(LST_SRC_POS);
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

  _lst_eol();

  if (bytes_emitted > 0) {
    while (bytes_emitted > 0) {
      /* data left to print on separate lines */

      pos = _lst_spaces(ADDR_LEN);
      lst_bytes = _lst_data(pos, m, byte_addr, bytes_emitted, reloc_type);
      byte_addr += lst_bytes;
      bytes_emitted -= lst_bytes;
      _lst_eol();
    }
    state.cod.emitting = 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

static int
_lst_symbol_verscmp(const void *p0, const void *p1)
{
  return strverscmp(sym_get_symbol_name(((const lst_symbol_t *)p0)->sym),
                    sym_get_symbol_name(((const lst_symbol_t *)p1)->sym));
}

/*------------------------------------------------------------------------------------------------*/

static int
_lst_symbol_cmp(const void *p0, const void *p1)
{
  return strcmp(sym_get_symbol_name(((const lst_symbol_t *)p0)->sym),
                sym_get_symbol_name(((const lst_symbol_t *)p1)->sym));
}


/*------------------------------------------------------------------------------------------------*/

/* append the symbol table to the .lst file */

void
lst_symbol_table(void)
{
  const symbol_t **clone;
  size_t           sym_count;
  lst_symbol_t    *lst;
  lst_symbol_t    *ps;
  size_t           count;
  size_t           i;

  state.lst.lst_state = LST_IN_SYMTAB;

  lst_line("SYMBOL TABLE");
  lst_line("  LABEL                             VALUE");
  lst_line(NULL);

  _cod_symbol_table();

  count = sym_get_symbol_count(state.stGlobal) + sym_get_symbol_count(state.stDefines) +
	  sym_get_symbol_count(state.stMacros);
  if (count == 0) {
    return;
  }

  ps = lst = GP_Malloc(count * sizeof(lst_symbol_t));

  clone = sym_clone_symbol_array(state.stGlobal, NULL);
  if (clone != NULL) {
    sym_count = sym_get_symbol_count(state.stGlobal);
    for (i = 0; i < sym_count; i++) {
      ps->sym  = clone[i];
      ps->type = LST_SYMBOL;
      ++ps;
    }

    free(clone);
  }

  clone = sym_clone_symbol_array(state.stDefines, NULL);
  if (clone != NULL) {
    sym_count = sym_get_symbol_count(state.stDefines);
    for (i = 0; i < sym_count; i++) {
      ps->sym  = clone[i];
      ps->type = LST_DEFINE;
      ++ps;
    }

    free(clone);
  }

  clone = sym_clone_symbol_array(state.stMacros, NULL);
  if (clone != NULL) {
    sym_count = sym_get_symbol_count(state.stMacros);
    for (i = 0; i < sym_count; i++) {
      ps->sym  = clone[i];
      ps->type = LST_MACRO;
      ++ps;
    }

    free(clone);
  }

  assert(ps == &lst[count]);

  if (!state.mpasm_compatible) {
    qsort(lst, count, sizeof(lst_symbol_t), _lst_symbol_verscmp);
  }
  else {
    qsort(lst, count, sizeof(lst_symbol_t), _lst_symbol_cmp);
  }

  for (i = 0; i < count; i++) {
    const char *name = sym_get_symbol_name(lst[i].sym);
    const void *p    = sym_get_symbol_annotation(lst[i].sym);

    switch (lst[i].type) {
    case LST_SYMBOL:
      /* symbol */
      lst_line("%-32s  %08X", name, (p != NULL) ? ((const variable_t *)p)->value : 0);
      break;

    case LST_DEFINE:
      /* define */
      if (p != NULL) {
        assert(((const pnode_t *)p)->tag == PTAG_LIST);
        assert(HEAD((const pnode_t *)p)->tag == PTAG_STRING);
      }
      lst_line("%-32s  %s", name, (p != NULL) ? HEAD((const pnode_t *)p)->value.string : "");
      break;

    case LST_MACRO:
      /* define */
      lst_line("%-32s  ", name);
      break;
    }
  }

  free(lst);
}


/*------------------------------------------------------------------------------------------------*/

/*
 * Preprocessed file generator.
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

/*------------------------------------------------------------------------------------------------*/

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

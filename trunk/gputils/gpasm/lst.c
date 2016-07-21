/* ".LST" file output for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin
   Copyright (C) 2012 Borut Razem

    Copyright (C) 2016 Molnar Karoly

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

#define STRINGIFY(s)            _str(s)
#define _str(s)                 #s

#define IS_EEPROM               (IS_EEPROM8 || IS_EEPROM16)
#define IS_BYTE                 (IS_PIC16E_CORE || IS_EEPROM)

#define MEM_USED_CHAR           'X'
#define MEM_UNUSED_CHAR         '-'

typedef struct {
  const symbol_t *sym;
  enum lst_sym_type_e {
    LST_SYMBOL,
    LST_DEFINITION,
    LST_MACRO
  } type;
} lst_symbol_t;

/*------------------------------------------------------------------------------------------------*/

static void
_lst_check_page_start(void)
{
  if ((state.lst.lines_per_page != 0) &&
      ((state.lst.line_of_page == 0) || (state.lst.line_of_page > state.lst.lines_per_page))) {
    lst_page_start();
  }
}

/*------------------------------------------------------------------------------------------------*/

static unsigned int
_lst_spaces(unsigned int Num)
{
  unsigned int i = Num;

  _lst_check_page_start();
  while (i-- != 0) {
    putc(' ', state.lst.f);
  }

  return Num;
}

/*------------------------------------------------------------------------------------------------*/

static void
_lst_eol(void)
{
  if (state.lst.f != NULL) {
    putc('\n', state.lst.f);
    state.lst.line_number++;
    state.lst.line_of_page++;
    cod_lst_line(COD_NORMAL_LST_LINE);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Print part of a line. Output must not contain newline. Needs call to lst_eol at end of line. */

static unsigned int
_lst_printf(const char *Format, ...)
{
  int     r = 0;
  va_list args;

  if (state.lst.f != NULL) {
    _lst_check_page_start();
    va_start(args, Format);
    r = vfprintf(state.lst.f, Format, args);
    va_end(args);
  }

  assert(r >= 0);
  return (unsigned int)r;
}

/*------------------------------------------------------------------------------------------------*/

static void
_lst_putc(char Ch)
{
  if (state.lst.f != NULL) {
    putc(Ch, state.lst.f);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* find relocation by address */

static gp_reloc_t *
_find_reloc_by_address(uint16_t Address)
{
  gp_reloc_t *p;

  for (p = state.obj.section->relocation_list.first; p != NULL; p = p->next) {
    if (p->address == Address) {
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

  if (state.obj.section->relocation_list.first == state.obj.section->relocation_list.last) {
    return 0;
  }

  p = state.obj.section->relocation_list.last->prev;
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
_print_reloc(uint16_t Type, uint16_t Current_value, unsigned int Index)
{
  proc_class_t class = state.device.class;

  switch (Type) {
    case RELOCT_CALL: {
      if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I) ||
          (class == PROC_CLASS_SX)    || (class == PROC_CLASS_PIC16E)) {
        ASSERT((Current_value & PIC12_BMSK_CALL) == 0);
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else if ((class == PROC_CLASS_PIC14) || (class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
        ASSERT((Current_value & PIC14_BMSK_CALL) == 0);
        return _lst_printf("%X??? ", (Current_value & 0xf000) >> 12);
      }
      else if (class == PROC_CLASS_PIC16) {
        ASSERT((Current_value & PIC16_BMSK_CALL) == 0);
        return _lst_printf("???? ");
      }
      else if (class == PROC_CLASS_PIC16E) {
        ASSERT((Current_value & PIC16E_BMSK_CALL0) == 0);
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else {
        ASSERT(0);
        return 0;
      }

      break;
    }

    /*..................................*/

    case RELOCT_GOTO: {
      if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I) ||
          (class == PROC_CLASS_SX)) {
        ASSERT((Current_value & PIC12_BMSK_GOTO) == 0);
        return _lst_printf("%X??? ", (Current_value & 0xf000) >> 12);
      }
      else if ((class == PROC_CLASS_PIC14) || (class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
        ASSERT((Current_value & PIC14_BMSK_GOTO) == 0);
        return _lst_printf("%X??? ", (Current_value & 0xf000) >> 12);
      }
      else if (class == PROC_CLASS_PIC16) {
        ASSERT((Current_value & PIC16_BMSK_GOTO) == 0);
        return _lst_printf("???? ");
      }
      else if (class == PROC_CLASS_PIC16E) {
        ASSERT((Current_value & PIC16E_BMSK_GOTO0) == 0);
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else {
        ASSERT(0);
        return 0;
      }

      break;
    }

    /*..................................*/

    case RELOCT_LOW:
      /* On non-16bits devices DB directive generates values with
       * low byte != 0 if more then one byte is defined, for example
       * DB 0x12, 0x34 generates 0x1234, so the following assertion fails
       * in such cases.
       * TODO: This should be solved in DB directive handling function
       * do_db(), file directive.c.
       * ASSERT((Current_value & 0xff) == 0); */
      return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      break;

    /*..................................*/

    case RELOCT_HIGH:
    case RELOCT_LFSR2:
    case RELOCT_UPPER:
    case RELOCT_CONDBRA:
      ASSERT((Current_value & 0xff) == 0);
      return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      break;

    /*..................................*/

    case RELOCT_P: {
      char buf[5] = "????";

      if (RELOCT_F != _prev_reloc_type()) {
        sprintf(&buf[2], "%02X", Current_value & 0x00ff);
      }

      ASSERT((Current_value & 0x1f00) == 0);
      return _lst_printf("%s ", buf);
      break;
    }

    /*..................................*/

    case RELOCT_BANKSEL: {
      if (class == PROC_CLASS_PIC12) {
        ASSERT((Current_value & PIC12_BMSK_BxF) == 0);  /* 04A4-04E4 or 05A4-05E4 */
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else if ((class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
        ASSERT((Current_value & PIC12E_BMSK_MOVLB) == 0);
        return _lst_printf("%03X? ", (Current_value & 0x0fff0) >> 4);
      }
      else if (class == PROC_CLASS_PIC14) {
        ASSERT((Current_value & PIC14_BMSK_BxF) == 0);  /* 1283-1303 or 1683-1703 */
        return _lst_printf("%X??? ", (Current_value & 0xf000) >> 12);
      }
      else if (class == PROC_CLASS_PIC14E) {
        ASSERT((Current_value & PIC14E_BMSK_MOVLB) == 0);  /* 0020-003F */
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else if (class == PROC_CLASS_PIC14EX) {
        ASSERT((Current_value & PIC14EX_BMSK_MOVLB) == 0);  /* 0140-017F */
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else if (class == PROC_CLASS_PIC16) {
        ASSERT((Current_value & PIC16_BMSK_MOVLB) == 0);  /* 0100-010F */
        return _lst_printf("%03X? ", (Current_value & 0xfff0) >> 4);
      }
      else if (class == PROC_CLASS_PIC16E) {
        ASSERT((Current_value & PIC16E_BMSK_MOVLB) == 0);  /* 0100-010F */
        return _lst_printf("%03X? ", (Current_value & 0xfff0) >> 4);
      }
      return _lst_printf("???? ");
      break;
    }

    /*..................................*/

    case RELOCT_ALL:
      return _lst_printf("???? ");
      break;

    /*..................................*/

    case RELOCT_IBANKSEL: {
      if (class == PROC_CLASS_PIC14) {
        ASSERT((Current_value & PIC12_BMSK_BxF) == 0);
        return _lst_printf("1?83 ");  /* 1383 or 1783 */
      }
      else if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
        ASSERT((Current_value & PIC12_BMSK_BxF) == 0);
        return _lst_printf("%X??? ", (Current_value & 0xf000) >> 12);  /* 120A-118A or 160A-158A */
      }
      else if (class == PROC_CLASS_PIC16) {
        ASSERT((Current_value & 0x00ff) == 0);
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else {
        ASSERT(0);
        return 0;
      }

      break;
    }

    /*..................................*/

    case RELOCT_F: {
      if (class == PROC_CLASS_SX) {
        ASSERT((Current_value & 0x0007) == 0);
        return _lst_printf("%03X? ", (Current_value & 0xfff0) >> 4);
      }
      else if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
        ASSERT((Current_value & MASK_PIC12_FILE) == 0);
      }
      else if ((class == PROC_CLASS_PIC14) || (class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
        ASSERT((Current_value & MASK_PIC14_FILE) == 0);
      }
      else if ((class == PROC_CLASS_PIC16) || (class == PROC_CLASS_PIC16E)) {
        ASSERT((Current_value & MASK_PIC16_FILE) == 0);
      }
      else {
        ASSERT(0);
        return 0;
      }
      return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      break;
    }

    /*..................................*/

    case RELOCT_TRIS_3BIT:
      ASSERT((Current_value & PIC12_BMSK_TRIS) == 0);
      return _lst_printf("%03X? ", (Current_value & 0x00f0) >> 4);
      break;

    /*..................................*/

    case RELOCT_TRIS:
      ASSERT((Current_value & 0x001f) == 0);
      return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      break;

    /*..................................*/

    case RELOCT_MOVLR: {
      if (class == PROC_CLASS_PIC16) {
        ASSERT((Current_value & PIC16_BMSK_MOVLR) == 0);
        return _lst_printf("%02X?%X ", (Current_value & 0xff00) >> 8, Current_value & 0x000f);  /* 010.-01F. */
      }
      else {
        ASSERT(0);
        return 0;
      }

      break;
    }

    /*..................................*/

    case RELOCT_MOVLB: {
      if ((class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
        ASSERT((Current_value & PIC12E_BMSK_MOVLB) == 0);
        return _lst_printf("%03X? ", (Current_value & 0xfff0) >> 4);
      }
      else if (class == PROC_CLASS_PIC14E) {
        ASSERT((Current_value & PIC14E_BMSK_MOVLB) == 0);
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else if (class == PROC_CLASS_PIC14EX) {
        ASSERT((Current_value & PIC14EX_BMSK_MOVLB) == 0);
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else if (class == PROC_CLASS_PIC16) {
        ASSERT((Current_value & PIC16_BMSK_MOVLB) == 0);
        return _lst_printf("%03X? ", (Current_value & 0xfff0) >> 4);  /* 01.0-01.F */
      }
      else if (class == PROC_CLASS_PIC16E) {
        ASSERT((Current_value & PIC16E_BMSK_MOVLB) == 0);
        return _lst_printf("%03X? ", (Current_value & 0xfff0) >> 4);
      }
      else {
        ASSERT(0);
        return 0;
      }

      break;
    }

    /*..................................*/

    case RELOCT_GOTO2:
      /* This is only used for PIC16E (pic18). */
    case RELOCT_FF1:
    case RELOCT_FF2:
      /* removed assertion since it fails during sdcc pic16 library
       * compilation: do_insn, case INSN_CLASS_FF
       * ASSERT((Current_value & 0x0fff) == 0); */
      return _lst_printf("%X??? ", (Current_value & 0xf000) >> 12);
      break;

    /*..................................*/

    case RELOCT_LFSR1:
      return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      break;

    /*..................................*/

    case RELOCT_BRA: {
      if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
        ASSERT((Current_value & PIC14E_BMSK_RBRA9) == 0);
      }
      else if (class == PROC_CLASS_PIC16E) {
        ASSERT((Current_value & PIC16E_BMSK_RBRA11) == 0);
      }
      else {
        ASSERT(0);
        return 0;
      }
      return _lst_printf("%X??? ", (Current_value & 0xf000) >> 12);
      break;
    }

    /*..................................*/

    case RELOCT_ACCESS: {
      if (class == PROC_CLASS_PIC16E) {
        ASSERT((Current_value & 0x00ff) == 0);
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else {
        ASSERT(0);
        return 0;
      }
      break;
    }

    /*..................................*/

    case RELOCT_PAGESEL_WREG: {
      if (Index == 0) {
        /* movlw PAGE_VALUE */
        if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
          ASSERT((Current_value & PIC12_BMSK_MOVLW) == 0);
          return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8); /* 0C00 -- movlw ... */
        }
        else if (class == PROC_CLASS_PIC14) {
          ASSERT((Current_value & PIC14_BMSK_MOVLW) == 0);
          return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8); /* 3000 -- movlw ... */
        }
        else {
          ASSERT(0);
          return 0;
        }
      }
      else {
        /* movwf register */
        if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
          ASSERT((Current_value & PIC12_BMSK_FILE) == 0);
          return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8); /* 0023 -- movwf STATUS */
        }
        else if (class == PROC_CLASS_PIC14) {
          ASSERT((Current_value & PIC14_BMSK_FILE) == 0);
          return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8); /* 008A -- movwf PCLATH */
        }
        else {
          ASSERT(0);
          return 0;
        }
      }

      break;
    }

    /*..................................*/

    case RELOCT_PAGESEL_BITS: {
      if ((class == PROC_CLASS_PIC12) || (class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
        ASSERT((Current_value & PIC12_BMSK_BxF) == 0);
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);  /* 04A3-04E3 or 05A3-05E3 */
      }
      else if (class == PROC_CLASS_PIC14) {
        ASSERT((Current_value & PIC14_BMSK_BxF) == 0);
        return _lst_printf("%X??? ", (Current_value & 0xf000) >> 12);  /* 120A-118A or 160A-158A */
      }
      else {
        ASSERT(0);
        return 0;
      }

      break;
    }

    /*..................................*/

    case RELOCT_PAGESEL_MOVLP: {
      if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
        ASSERT((Current_value & PIC14E_BMSK_MOVLP) == 0);
        return _lst_printf("%02X?? ", (Current_value & 0xff00) >> 8);
      }
      else {
        ASSERT(0);
        return 0;
      }

      break;
    }

    /*..................................*/

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
_lst_data(unsigned int Position, MemBlock_t *M, unsigned int Byte_addr, unsigned int Bytes_emitted,
          uint16_t Reloc_type)
{
  uint8_t      emit_byte;
  uint16_t     emit_word;
  unsigned int start_addr;
  unsigned int lst_bytes;
  gp_boolean   is_eeprom_area;
  unsigned int n;

  lst_bytes      = 0;
  is_eeprom_area = (gp_processor_is_eeprom_byte_addr(state.processor, Byte_addr) >= 0) ? true : false;
  /* When in a idata or byte packed section or eeprom area, print byte by byte. */
  if (IS_EEPROM8 || is_eeprom_area || (state.obj.new_sect_flags & (STYP_DATA | STYP_BPACK))) {
    while ((Bytes_emitted > lst_bytes) && ((Position + 3) <= LST_LINENUM_POS)) {
      gp_mem_b_get(M, Byte_addr, &emit_byte, NULL, NULL);
      Position += _lst_printf("%02X ", emit_byte);
      ++Byte_addr;
      ++lst_bytes;
    }
  }
  else {    /* non-code pack section */
    /* list first byte on odd address */
    if ((Bytes_emitted != 0) && (Byte_addr & 1)) {
      gp_mem_b_get(M, Byte_addr, &emit_byte, NULL, NULL);
      Position += _lst_printf("%02X ", emit_byte);
      ++Byte_addr;
      ++lst_bytes;
    }
    /* list full words */
    start_addr = Byte_addr;
    while (((Bytes_emitted - lst_bytes) > 1) && ((Position + 5) <= LST_LINENUM_POS)) {
      state.device.class->i_memory_get(M, Byte_addr, &emit_word, NULL, NULL);

      /* Display '?' for undefined bytes if it is a relocatable code. */
      if (Reloc_type != 0) {
        n = _print_reloc(Reloc_type, emit_word, (Byte_addr - start_addr) / 2);

        Position += (n == 0) ? _lst_printf("%04X ", emit_word) : n;
      }
      else {
        Position += _lst_printf("%04X ", emit_word);
      }

      Byte_addr += 2;
      lst_bytes += 2;
    }

    if (((Bytes_emitted - lst_bytes) == 1) && ((Position + 3) <= LST_LINENUM_POS)) {
      gp_mem_b_get(M, Byte_addr, &emit_byte, NULL, NULL);
      Position += _lst_printf("%02X ", emit_byte);
      ++Byte_addr;
      ++lst_bytes;
    }
  }

  /* append appropriate spacing */
  _lst_spaces(LST_LINENUM_POS - Position);

  return lst_bytes;
}

/*------------------------------------------------------------------------------------------------*/

static void
_cod_symbol_table(void)
{
  const symbol_t **lst;
  size_t           sym_count;

  sym_count = gp_sym_get_symbol_count(state.stGlobal);

  if (sym_count == 0) {
    return;
  }

  if (!state.mpasm_compatible) {
    lst = gp_sym_clone_symbol_array(state.stGlobal, gp_sym_version_compare_fn);
  }
  else {
    lst = gp_sym_clone_symbol_array(state.stGlobal, gp_sym_compare_fn);
  }

  assert(lst != NULL);

  cod_write_symbols(lst, sym_count);
  free(lst);
}

/*------------------------------------------------------------------------------------------------*/

void
lst_init(void)
{
  state.lst.line_of_page   = 0;
  state.lst.page           = 0;
  state.lst.lines_per_page = 59;
  state.lst.line_number    = 1;
  state.lst.memory_map     = true;
  state.lst.symbol_table   = true;
  state.lst.lst_state      = LST_IN_MEM;

  /* Determine state.start_date. */
  gp_date_string(state.lst.start_date, sizeof(state.lst.start_date));

  if (!state.cmd_line.macro_expand){
    state.lst.expand = true;
  }

  state.lst.force            = state.cmd_line.lst_force;
  state.lst.config_address   = 0;
  state.lst.title_name[0]    = '\0';
  state.lst.subtitle_name[0] = '\0';
  state.lst.tabstop          = 8;   /* Default tabstop every 8. */
  state.lst.line_width       = 132; /* Default line width is 132. */

  if (state.lst_file != OUT_NAMED) {
    snprintf(state.lst_file_name, sizeof(state.lst_file_name), "%s.lst", state.base_file_name);
  }

  if (state.lst_file == OUT_SUPPRESS) {
    state.lst.f       = NULL;
    state.lst.enabled = false;
    unlink(state.lst_file_name);
  }
  else {
    state.lst.f = fopen(state.lst_file_name, "wt");

    if (state.lst.f == NULL) {
      perror(state.lst_file_name);
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
    state.lst.f = NULL;
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
            state.src_file_name,
            state.lst.start_date,
            state.lst.page,
            state.lst.title_name,
            state.lst.subtitle_name);

    state.lst.line_of_page = 4;
    cod_lst_line(COD_NORMAL_LST_LINE);
    cod_lst_line(COD_NORMAL_LST_LINE);
    cod_lst_line(COD_NORMAL_LST_LINE);
    state.lst.line_number += 3;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_mem_header(void)
{
  lst_line("LOC    OBJECT CODE    LINE  SOURCE TEXT");
  lst_line("  VALUE");
}

/*------------------------------------------------------------------------------------------------*/

static void
_symbol_table_header(void)
{
  lst_line("SYMBOL TABLE");

  if (!state.mpasm_compatible) {
    lst_line("%-32s  %-10s  %-8s    %-11s", "  LABEL", "   TYPE", "   VALUE", "     VALUE");
    lst_line("%-32s  %-10s  %-8s    %-11s", "",        "",        "   (hex)", "     (dec)");
  }
  else {
    lst_line("%-32s  %-8s", "  LABEL", " VALUE");
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_memory_map_header(void)
{
  lst_line("MEMORY USAGE MAP ('%c' = Used,  '%c' = Unused)", MEM_USED_CHAR, MEM_UNUSED_CHAR);
}

/*------------------------------------------------------------------------------------------------*/

void
lst_page_start(void)
{
  lst_throw();
  switch (state.lst.lst_state) {
    case LST_IN_MEM:
      _mem_header();
      break;

    case LST_IN_SYMTAB:
      _symbol_table_header();
      break;

    case LST_IN_MAP:
      _memory_map_header();
      break;

    default:
      lst_line(NULL);
      break;
  }

  lst_line(NULL);
}

/*------------------------------------------------------------------------------------------------*/

void
lst_line(const char *Format, ...)
{
  va_list args;

  if (state.lst.f != NULL) {
    if (Format != NULL) {
      _lst_check_page_start();
      va_start(args, Format);
      vfprintf(state.lst.f, Format, args);
      va_end(args);
    }
    _lst_eol();
  }
}

/*------------------------------------------------------------------------------------------------*/

void
lst_err_line(const char *Type, unsigned int Code, const char *Format, va_list Args)
{
  if (state.lst.f != NULL) {
    _lst_check_page_start();
    fprintf(state.lst.f, "%s[%03d]%s: ", Type, Code, (strcmp(Type, "Error") == 0) ? "  " : "");
    vfprintf(state.lst.f, Format, Args);
    _lst_eol();
  }
}

/*------------------------------------------------------------------------------------------------*/

#define MEM_IS_USED(M, I)  \
        (((M)->memory != NULL) ? (IS_BYTE ? (gp_mem_b_offset_is_used(M, I)) : \
                                            (gp_mem_i_offset_is_used_le(M, (I) * 2) == W_USED_ALL)) : false)

#define NUM_PER_LINE            64
#define NUM_PER_BLOCK           16

void
lst_memory_map(MemBlock_t *M)
{
  unsigned int max_mem;
  unsigned int base;
  unsigned int i;
  unsigned int j;
  int          addr_digits;
  gp_boolean   row_is_used;
  char         row_map[NUM_PER_LINE];
  char         ch;
  unsigned int used;

  state.lst.lst_state = LST_IN_MAP;
  lst_line(NULL);
  lst_line(NULL);
  _memory_map_header();
  lst_line(NULL);

  addr_digits = (state.device.class != NULL) ? state.device.class->addr_digits : 4;

  while (M != NULL) {
    max_mem = I_MEM_MAX >> !IS_BYTE;
    base    = IMemAddrFromBase(M->base) >> !IS_BYTE;

    for (i = 0; i < max_mem; i += NUM_PER_LINE) {
      row_is_used = false;

      for (j = 0; j < NUM_PER_LINE; j++) {
        if (MEM_IS_USED(M, i + j)) {
          ch = MEM_USED_CHAR;
          row_is_used = true;
        }
        else {
          ch = MEM_UNUSED_CHAR;
        }

        row_map[j] = ch;
      }

      if (row_is_used) {
        if (state.show_full_addr && IS_PIC16E_CORE) {
          /* Gpasm mode: Print all address digits. */
          _lst_printf("%0*X :", addr_digits, (i + base));
        }
        else {
          /* MPASM(X) compatible: Print only lower 4 address digits. */
          _lst_printf("%04X :", (i + base) & 0xffff);
        }

        for (j = 0; j < NUM_PER_LINE; j++) {
          if ((j % NUM_PER_BLOCK) == 0) {
            _lst_putc(' ');
          }

          _lst_putc(row_map[j]);
        }

        _lst_eol();
        _lst_check_page_start();
      }
    }

    M = M->next;
  }

  lst_line(NULL);
  lst_line("All other memory blocks unused.");
  lst_line(NULL);

  /* it seems that MPASM includes config bytes into program memory usage
   * count for 16 bit cores. See gpasm testsuite:
   * gpasm/testsuite/gpasm.mchip/listfiles/configX.lst */
#define IS_PIC16  (IS_PIC16_CORE || IS_PIC16E_CORE)

  if (IS_EEPROM) {
    lst_line("Memory Bytes Used: %5i", gp_mem_b_used(state.i_memory));
  }
  else {
    used = gp_processor_insn_from_byte_p(state.processor, ((!IS_PIC16) && (state.processor != NULL)) ?
                        b_range_memory_used(state.i_memory, 0,
                                            gp_processor_byte_from_insn_c(state.device.class,
                                                                          state.processor->prog_mem_size)) :
                        gp_mem_b_used(state.i_memory));

    lst_line("Program Memory %s Used: %5i", IS_BYTE ? "Bytes" : "Words", used);

    if ((state.processor != NULL) && (state.processor->prog_mem_size >= 0)) {
      lst_line("Program Memory %s Free: %5u",
               IS_BYTE ? "Bytes" : "Words",
               (used <= state.processor->prog_mem_size) ? (state.processor->prog_mem_size - used) : 0);
    }
  }

  lst_line(NULL);
}

/*------------------------------------------------------------------------------------------------*/

#define ADDR_LEN                7

void
lst_format_line(const char *Src_line, unsigned int Value)
{
  unsigned int  emitted;
  unsigned int  emitted_lines;
  unsigned int  byte_addr;
  unsigned int  bytes_emitted;
  unsigned int  lst_bytes;
  const char   *addr_fmt;
  unsigned int  pos;
  uint16_t      reloc_type;
  MemBlock_t   *m;
  uint32_t      addr;
  uint16_t      word;
  gp_reloc_t   *reloc;

  assert(Src_line != NULL);

  emitted       = 0;
  emitted_lines = 0;
  bytes_emitted = 0;
  addr_fmt      = IS_PIC16E_CORE ? "%06X " : (IS_EEPROM ? "%04X " : "%04X   ");
  pos           = 0;
  m             = state.i_memory;

  byte_addr = state.lst.line.was_byte_addr;

  if ((state.mode == MODE_RELOCATABLE) && (state.obj.section != NULL) &&
      (state.obj.new_sect_flags & STYP_TEXT) && (state.obj.section->relocation_list.last != NULL)) {
      addr = state.obj.section->address + state.obj.section->relocation_list.last->address;

      if (addr > byte_addr) {
        /* already passed it, go back to the history */
        reloc      = _find_reloc_by_address(byte_addr);
        reloc_type = (reloc != NULL) ? reloc->type : 0;
      }
      else if (addr == byte_addr) {
        reloc_type = state.obj.section->relocation_list.last->type;
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
      emitted_lines = emitted = state.byte_addr - byte_addr;
      break;

    case LTY_DATA: {
      emitted = state.byte_addr - byte_addr;

      if ((SECTION_FLAGS & (STYP_TEXT | (state.mpasm_compatible ? STYP_BPACK : 0))) == STYP_TEXT) {
        /* generate line numbers for data directives in program memory;
         * in mpasm compatibility mode code_pack doesn't generate line numbers */
        emitted_lines = emitted;
      }
      break;
    }

    case LTY_RES: {
      if (SECTION_FLAGS & STYP_DATA) {
        /* generate data listing for idata */
        emitted = state.byte_addr - byte_addr;
      }
      else if (!IS_RAM_ORG) {
        /* generate line numbers for res directives in program memory */
        emitted_lines = emitted = state.byte_addr - byte_addr;
      }
      break;
    }

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

  byte_addr = 0;
  switch (state.lst.line.linetype) {
    case LTY_EQU:
    case LTY_SET:
      pos += _lst_printf("  %08X", Value);
      _lst_spaces(LST_LINENUM_POS - pos);
      break;

    case LTY_SET4:
      pos += _lst_printf("  %04X", Value & 0xffff);
      _lst_spaces(LST_LINENUM_POS - pos);
      break;

    case LTY_ORG:
      pos += _lst_printf(addr_fmt, gp_processor_insn_from_byte_p(state.processor, state.byte_addr));
      _lst_spaces(LST_LINENUM_POS - pos);
      break;

    case LTY_IDLOCS:
      /* not used for 16 bit devices, config is used */
      m              = state.c_memory;
      pos           += _lst_printf(addr_fmt, gp_processor_insn_from_byte_p(state.processor, state.device.id_location));
      lst_bytes      = _lst_data(pos, m, state.device.id_location, emitted, reloc_type);
      byte_addr      = state.device.id_location + lst_bytes;
      bytes_emitted  = emitted - lst_bytes;
      break;

    case LTY_DATA:
    case LTY_RES:
      pos += _lst_printf(addr_fmt, state.lst.line.was_byte_addr);
      goto lst_data;

    case LTY_INSN:
      pos += _lst_printf(addr_fmt, gp_processor_insn_from_byte_p(state.processor, state.lst.line.was_byte_addr));

lst_data:

      lst_bytes     = _lst_data(pos, m, state.lst.line.was_byte_addr, emitted, reloc_type);
      byte_addr     = state.lst.line.was_byte_addr + lst_bytes;
      bytes_emitted = emitted - lst_bytes;
      break;

    case LTY_CONFIG: {
      if (IS_PIC16E_CORE) {
        /* The config data is byte addressable, but we only want to print words in the list file. */
        if (state.lst.config_address == CONFIG4L) {
          /* Special case */
          state.device.class->i_memory_get(state.c_memory, state.lst.config_address, &word, NULL, NULL);
          pos += _lst_printf(addr_fmt, state.lst.config_address);
          pos += _lst_printf("%04X", word);
          _lst_spaces(LST_LINENUM_POS - pos);
        }
        else if ((state.lst.config_address & 1) == 0) {
          /* if it is an even address don't print anything */
          _lst_spaces(LST_LINENUM_POS);
        }
        else {
          state.device.class->i_memory_get(state.c_memory, state.lst.config_address - 1, &word, NULL, NULL);
          pos += _lst_printf(addr_fmt, state.lst.config_address - 1);
          pos += _lst_printf("%04X", word);
          _lst_spaces(LST_LINENUM_POS - pos);
        }
      }
      else {
        state.device.class->i_memory_get(state.c_memory, state.lst.config_address, &word, NULL, NULL);
        pos += _lst_printf(addr_fmt, gp_processor_insn_from_byte_p(state.processor, state.lst.config_address));
        pos += _lst_printf("%04X", word);
        _lst_spaces(LST_LINENUM_POS - pos);
      }
      break;
    }

    case LTY_SEC:
    case LTY_DIR:
    case LTY_NONE:
    default:
      _lst_spaces(LST_LINENUM_POS);
      break;
  }

  if (state.stGlobal == state.stTop) {
    _lst_printf("%05d ", state.src_list.last->line_number);
  }
  else {
    _lst_printf("    M ");
  }

  /* Now copy source line to listing, expanding tabs as required. */
  {
    int         src_column = 0;           /* current source line column */
    int         lst_column = LST_SRC_POS; /* current listing column after the SRC_POS */
    const char *p = Src_line;

    while (*p != '\0') {
      if (*p == '\t') {
        int len = state.lst.tabstop - (src_column % state.lst.tabstop);

        while (len-- > 0) {
          if (lst_column >= state.lst.line_width) {
            _lst_eol();
            _lst_spaces(LST_SRC_POS);
            lst_column = LST_SRC_POS;
          }
          ++lst_column;
          ++src_column;
          putc(' ', state.lst.f);
        }
      }
      else {
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

      pos            = _lst_spaces(ADDR_LEN);
      lst_bytes      = _lst_data(pos, m, byte_addr, bytes_emitted, reloc_type);
      byte_addr     += lst_bytes;
      bytes_emitted -= lst_bytes;
      _lst_eol();
    }
    state.cod.emitting = 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

static int
_lst_symbol_verscmp(const void *P0, const void *P1)
{
  return strverscmp(gp_sym_get_symbol_name(((const lst_symbol_t *)P0)->sym),
                    gp_sym_get_symbol_name(((const lst_symbol_t *)P1)->sym));
}

/*------------------------------------------------------------------------------------------------*/

static int
_lst_symbol_cmp(const void *P0, const void *P1)
{
  return strcmp(gp_sym_get_symbol_name(((const lst_symbol_t *)P0)->sym),
                gp_sym_get_symbol_name(((const lst_symbol_t *)P1)->sym));
}

/*------------------------------------------------------------------------------------------------*/

/* append the symbol table to the .lst file */

void
lst_symbol_table(void)
{
  const pnode_t    *list;
  const pnode_t    *head;
  const char       *string;
  const symbol_t  **clone;
  size_t            sym_count;
  lst_symbol_t     *lst;
  lst_symbol_t     *ps;
  size_t            count;
  size_t            i;
  const char       *name;
  const void       *ptr;
  const variable_t *var;
  gp_boolean        dec_ok;
  gp_boolean        hex_ok;
  long              val;

  state.lst.lst_state = LST_IN_SYMTAB;
  _symbol_table_header();
  lst_line(NULL);

  _cod_symbol_table();

  count = gp_sym_get_symbol_count(state.stGlobal) + gp_sym_get_symbol_count(state.stDefines) +
	  gp_sym_get_symbol_count(state.stMacros);
  if (count == 0) {
    return;
  }

  ps = lst = GP_Malloc(count * sizeof(lst_symbol_t));

  clone = gp_sym_clone_symbol_array(state.stGlobal, NULL);
  if (clone != NULL) {
    sym_count = gp_sym_get_symbol_count(state.stGlobal);
    for (i = 0; i < sym_count; i++) {
      ps->sym  = clone[i];
      ps->type = LST_SYMBOL;
      ++ps;
    }

    free(clone);
  }

  clone = gp_sym_clone_symbol_array(state.stDefines, NULL);
  if (clone != NULL) {
    sym_count = gp_sym_get_symbol_count(state.stDefines);
    for (i = 0; i < sym_count; i++) {
      ps->sym  = clone[i];
      ps->type = LST_DEFINITION;
      ++ps;
    }

    free(clone);
  }

  clone = gp_sym_clone_symbol_array(state.stMacros, NULL);
  if (clone != NULL) {
    sym_count = gp_sym_get_symbol_count(state.stMacros);
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
    name = gp_sym_get_symbol_name(lst[i].sym);
    ptr  = gp_sym_get_symbol_annotation(lst[i].sym);

    switch (lst[i].type) {
      case LST_SYMBOL: {
        /* symbol */
        var = (const variable_t *)ptr;

        if (!state.mpasm_compatible) {
          if (var != NULL) {
            if (FlagIsClr(var->flags, VATRR_HAS_NO_VALUE)) {
              lst_line("%-32s  %-10s    %08X    %11d", name, value_type_to_str(var, false), var->value, var->value);
            }
            else {
              lst_line("%-32s  %-10s    %8s    %-11s", name, value_type_to_str(var, false), "HAS NO", "HAS NO");
            }
          }
          else {
            lst_line("%-32s", name);
          }
        }
        else {
          lst_line("%-32s  %08X", name, (var != NULL) ? var->value : 0);
        }
        break;
      }

      case LST_DEFINITION: {
        /* define */
        list = (const pnode_t *)ptr;

        if (list != NULL) {
          assert(PnIsList(list));
          head = PnListHead(list);

          assert(PnIsString(head));
          string = PnString(head);
        }
        else {
          string = NULL;
        }

        if (!state.mpasm_compatible) {
          if (string != NULL) {
            val = dec_strtol(string, &dec_ok);

            if (!dec_ok) {
              val = hex_strtol(string, &hex_ok);
            }

            if (dec_ok) {
              /* This definition contains is just a decimal number. */
              lst_line("%-32s  %-10s    %08lX    '%s'", name, "DEFINITION", val, string);
            }
            else if (hex_ok) {
              /* This definition contains is just a hexadecimal number. */
              lst_line("%-32s  %-10s    '%s'    %11ld", name, "DEFINITION", string, val);
            }
            else {
              /* This definition contains a general text (not just number). */
              lst_line("%-32s  %-10s    '%s'", name, "DEFINITION", string);
            }
          }
          else {
            lst_line("%-32s  %-10s", name, "DEFINITION");
          }
        } /* if (!state.mpasm_compatible) */
        else {
          lst_line("%-32s  %s", name, (string != NULL) ? string : "");
        }
        break;
      }

      case LST_MACRO: {
        /* define */
        if (!state.mpasm_compatible) {
          lst_line("%-32s  %-10s", name, "MACRO");
        }
        else {
          lst_line("%-32s", name);
        }
        break;
      }
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
  const char *name = state.preproc.preproc_file_name;

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

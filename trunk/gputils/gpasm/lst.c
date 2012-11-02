/* ".LST" file output for gpasm
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
#include "directive.h"
#include "gpasm.h"
#include "cod.h"
#include "coff.h"
#include "lst.h"

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

#define MPASM_LIST

#ifdef MPASM_LIST
#define LINENUM_POS 22
#else
#define LINENUM_POS 15
#endif

#define STRINGIFY(s) _str(s)
#define _str(s) #s

void
lst_throw(void)
{
  if(state.lst.f) {
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

static void
lst_check_page_start(void)
{
  if (state.lst.linesperpage != 0 &&
      (state.lst.lineofpage == 0 ||
       state.lst.lineofpage > state.lst.linesperpage)) {
    lst_throw();
    switch (state.lst.lst_state) {
    case in_mem:
      lst_line("LOC  OBJECT CODE     LINE SOURCE TEXT");
      lst_line("  VALUE");
      break;

    case in_symtab:
      lst_line("SYMBOL TABLE");
      lst_line("%-32s  %-8s", "  LABEL", "  VALUE");
      break;

    default:
      lst_line("");
      break;
    }
    lst_line("");
  }
}

static int
lst_spaces(int n)
{
  int i = n;

  lst_check_page_start();
  while (i--)
    fputc(' ', state.lst.f);

  return n;
}

static void
lst_eol(void)
{
  if (state.lst.f) {
    fputc('\n', state.lst.f);
    state.lst.line_number++;
    state.lst.lineofpage++;
    cod_lst_line(COD_NORMAL_LST_LINE);
  }
}

void
lst_line(const char *format, ...)
{
  if (state.lst.f) {
    va_list args;
    lst_check_page_start();
    va_start(args, format);
    vfprintf(state.lst.f, format, args);
    va_end(args);
    lst_eol();
  }
}

void
lst_err_line(const char *type, unsigned int code, const char *format, va_list args)
{
  if (state.lst.f) {
    lst_check_page_start();
    fprintf(state.lst.f, "%s[%03d]  : ", type, code);
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
  if (state.lst.f) {
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
  state.lst.memorymap = 1;
  state.lst.symboltable = 1;
  state.lst.lst_state = in_mem;

  /* Determine state.startdate */
  gp_date_string(state.lst.startdate, sizeof(state.lst.startdate));

  if (!state.cmd_line.macro_expand){
    state.lst.expand = true;
  }

  if (state.cmd_line.lst_force)
    state.lst.force = true;
  else
    state.lst.force = false;

  state.lst.config_address = 0;
  state.lst.title_name[0] = '\0';
  state.lst.subtitle_name[0] = '\0';
  state.lst.tabstop = 8;        /* Default tabstop every 8 */

  if (state.lstfile != named) {
    snprintf(state.lstfilename, sizeof(state.lstfilename),
             "%s.lst", state.basefilename);
  }

  if (state.lstfile == suppress) {
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
#define MEM_IS_USED(m, i)   (IS_16BIT_CORE ? ((m)->memory[i] & BYTE_USED_MASK) : (((m)->memory[2 * (i)] & BYTE_USED_MASK) || ((m)->memory[2 * (i) + 1] & BYTE_USED_MASK)))

  int i, j, base, row_used, num_per_line, num_per_block;
  unsigned int used;

  lst_line("");
  lst_line("");
  lst_line("MEMORY USAGE MAP ('X' = Used,  '-' = Unused)");
  lst_line("");

  num_per_line = 64;
  num_per_block = 16;

  while (m) {
    unsigned int max_mem = MAX_I_MEM  >> (!IS_16BIT_CORE);
    assert(m->memory != NULL);

    base = (m->base << I_MEM_BITS) >> (!IS_16BIT_CORE);

    for (i = 0; i < max_mem; i += num_per_line) {
      row_used = 0;

      for (j = 0; j < num_per_line; j++) {
        if (MEM_IS_USED(m, i + j)) {
          row_used = 1;
          break;
        }
      }

      if(row_used) {
        lst_printf("%04X :", i + base);
        for (j = 0; j < num_per_line; j++) {
          if ((j % num_per_block) == 0) {
            lst_printf(" ");
          }
          lst_printf(MEM_IS_USED(m, i + j) ? "X" : "-");
        }

        lst_eol();
      }
    }

    m = m->next;
  }

  lst_line("");
  lst_line("All other memory blocks unused.");
  lst_line("");

  /* it seems that MPASM includes config bytes into program memory usage
   * count for 16 bit cores. See gpasm testsuite:
   * gpasm/testsuite/gpasm.mchip/listfiles/configX.lst */
#define IS_PIC16  (state.device.class == PROC_CLASS_PIC16 || state.device.class == PROC_CLASS_PIC16E)

  used = gp_processor_byte_to_org(state.device.class, (!IS_PIC16 && state.processor) ?
    b_range_memory_used(state.i_memory, 0,
      gp_processor_org_to_byte(state.device.class, state.processor->config_addrs[0])) :
    b_memory_used(state.i_memory));
  lst_line("Program Memory %s Used: %5i", IS_16BIT_CORE ? "Bytes" : "Words", used);
  /* maxrom is not the program memory size.
  lst_line("Program Memory %s Free: %5d", IS_16BIT_CORE ? "Bytes" : "Words", state.maxrom - used);
  */
}

void
lst_close(void)
{
  cod_lst_line(COD_LAST_LST_LINE);

  state.lst.lst_state = in_none;

  if (state.lst.f) {
    lst_line("");
    lst_line("Errors   : %5d", state.num.errors);
    lst_line("Warnings : %5d reported, %5d suppressed",
             state.num.warnings,
             state.num.warnings_suppressed);
    lst_line("Messages : %5d reported, %5d suppressed",
             state.num.messages,
             state.num.messages_suppressed);
    lst_line("");
    fputc('\f', state.lst.f);

    fclose(state.lst.f);
  }
}

/* print word value with undefined nibbles repleced by "?" */
#define DO_ASSERT

#ifdef DO_ASSERT
#define ASSERT(expr)    assert(expr)
#else
#define ASSERT(expr)    ((void) 0)
#endif

/* find relocation by address */
static gp_reloc_type *
find_reloc_by_address(unsigned short address)
{
  gp_reloc_type *p;

  for (p = state.obj.section->relocations; p; p = p->next) {
    if (p->address == address)
      break;
  }

  return p;
}

/* get previous relocation type */
static unsigned short
prev_reloc_type(void)
{
  gp_reloc_type *p;

  if (state.obj.section->relocations == state.obj.section->relocations_tail)
    return 0;

  for (p = state.obj.section->relocations; p->next && p->next != state.obj.section->relocations_tail; p = p->next)
    ;
  assert(p->next);
  return (p->address == p->next->address) ? p->type : 0;
}

static int
print_reloc(unsigned short type, unsigned short current_value)
{
  switch (type) {
  case RELOCT_CALL:
    if (PROC_CLASS_PIC12 == state.device.class || PROC_CLASS_SX == state.device.class || PROC_CLASS_PIC16E == state.device.class) {
      ASSERT(0 == (current_value & 0xff));
      return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    } else if (PROC_CLASS_PIC14 == state.device.class || PROC_CLASS_PIC14E == state.device.class) {
      ASSERT(0 == (current_value & 0x7ff));
      return lst_printf("%01X??? ", (current_value >> 12) & 0x000f);
    } else if (PROC_CLASS_PIC16 == state.device.class) {
      ASSERT(0 == (current_value & 0x1fff));
      return lst_printf("???? ");
    } else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_GOTO:
    if (PROC_CLASS_PIC12 == state.device.class || PROC_CLASS_SX == state.device.class) {
      ASSERT(0 == (current_value & 0x1ff));
      return lst_printf("%01X??? ", (current_value >> 12) & 0x000f);
    } else if (PROC_CLASS_PIC14 == state.device.class || PROC_CLASS_PIC14E == state.device.class) {
      ASSERT(0 == (current_value & 0x7ff));
      return lst_printf("%01X??? ", (current_value >> 12) & 0x000f);
    } else if (PROC_CLASS_PIC16 == state.device.class) {
      ASSERT(0 == (current_value & 0x1fff));
      return lst_printf("???? ");
    } else if (PROC_CLASS_PIC16E == state.device.class) {
      ASSERT(0 == (current_value & 0xff));
      return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    } else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_LOW:
    /* on non-16bits devices DB directive generates velues with
     * low byte != 0 if more then one byte is defined, for example
     * DB 0x12, 0x34 generates 0x1234, so the following assertion fails
     * in such cases.
     * TODO: this should be solved in DB directive handling function
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
    if (PROC_CLASS_PIC14 == state.device.class || PROC_CLASS_PIC14E == state.device.class) {
      ASSERT(0 == (current_value & 0x0f00));  /* 1383 or 1783 */
      return lst_printf("1?83 ");
    } else if (PROC_CLASS_PIC16 == state.device.class) {
      ASSERT(0 == (current_value & 0x00ff));
      return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    } else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_F:
    if (PROC_CLASS_SX == state.device.class) {
      ASSERT(0 == (current_value & 0x0007));
      return lst_printf("%03X?? ", (current_value >> 4) & 0x000f);
    } else if (PROC_CLASS_PIC12 == state.device.class) {
      ASSERT(0 == (current_value & 0x001f));
    } else if (PROC_CLASS_PIC14 == state.device.class || PROC_CLASS_PIC14E == state.device.class) {
      ASSERT(0 == (current_value & 0x007f));
    } else if (PROC_CLASS_PIC16 == state.device.class || PROC_CLASS_PIC16E == state.device.class) {
      ASSERT(0 == (current_value & 0x00ff));
    } else {
      ASSERT(0);
      return 0;
    }
    return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    break;

  case RELOCT_TRIS:
    if (PROC_CLASS_PIC12 == state.device.class || PROC_CLASS_SX == state.device.class) {
      ASSERT(0 == (current_value & 0x0007));
      return lst_printf("%03X? ", (current_value >> 4) & 0x000f);
    } else if (PROC_CLASS_PIC14 == state.device.class || PROC_CLASS_PIC14E == state.device.class) {
      ASSERT(0 == (current_value & 0x001f));
      return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    } else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_MOVLR:
    ASSERT(0 == (current_value & 0x00f0));
    return lst_printf("%02X?%01X ", (current_value >> 8) & 0xff, current_value & 0x000f);
    break;

  case RELOCT_GOTO2:
    /* This is only used for PIC16E (pic18) */
  case RELOCT_FF1:
  case RELOCT_FF2:
    ASSERT(0 == (current_value & 0x0fff));
    return lst_printf("%01X??? ", (current_value >> 12) & 0x000f);
    break;

  case RELOCT_LFSR1:
    return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    break;

  case RELOCT_BRA:
    if (PROC_CLASS_PIC14E == state.device.class) {
      ASSERT(0 == (current_value & 0x01ff));
    } else if (PROC_CLASS_PIC16E == state.device.class) {
      ASSERT(0 == (current_value & 0x07ff));
    } else {
      ASSERT(0);
      return 0;
    }
    return lst_printf("%01X??? ", (current_value >> 12) & 0x000f);
    break;

  case RELOCT_MOVLB:
    if (PROC_CLASS_PIC14E == state.device.class) {
      ASSERT(0 == (current_value & 0x001f));
      return lst_printf("%02X?? ", (current_value >> 8) & 0x00ff);
    } else if (PROC_CLASS_PIC16 == state.device.class || PROC_CLASS_PIC16E == state.device.class) {
      ASSERT(0 == (current_value & 0x000f));
      return lst_printf("%03X? ", (current_value >> 4) & 0x0fff);
    } else {
      ASSERT(0);
      return 0;
    }
    break;

  case RELOCT_ACCESS:
    if (PROC_CLASS_PIC16E == state.device.class) {
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
    if (PROC_CLASS_PIC14E == state.device.class) {
      ASSERT(0 == (current_value & 0x007f));
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

unsigned int
lst_data(unsigned int pos, unsigned int byte_org,
                      unsigned int bytes_emitted)
{
  int lst_bytes = 0;

  /* when in a idata or byte packed section, print byte by byte */
  if (state.obj.new_sec_flags & (STYP_DATA | STYP_BPACK)) {
    while (bytes_emitted > lst_bytes && pos + 3 <= LINENUM_POS) {
      unsigned char emit_byte;

      b_memory_get(state.i_memory, byte_org, &emit_byte);
      pos += lst_printf("%02X ", emit_byte);
      ++byte_org;
      ++lst_bytes;
    }
  }
  else {    /* non-code pack section */
    /* list first byte on odd address */
    if (bytes_emitted && (byte_org & 1) != 0) {
      unsigned char emit_byte;

      b_memory_get(state.i_memory, byte_org, &emit_byte);
      pos += lst_printf("%02X ", emit_byte);
      ++byte_org;
      ++lst_bytes;
    }
    /* list full words */
    while (bytes_emitted - lst_bytes > 1 && pos + 5 <= LINENUM_POS) {
      unsigned short emit_word;

      state.device.class->i_memory_get(state.i_memory, byte_org, &emit_word);

      /* display '?' for undefined bytes if it is a relocatable code */
      if (state.mode == relocatable &&
          state.obj.section &&
          state.obj.new_sec_flags & STYP_TEXT &&
          state.obj.section->relocations_tail) {
        if (state.obj.section->address + state.obj.section->relocations_tail->address > byte_org) {
          /* already passed it, go back to the history */
          gp_reloc_type *p = find_reloc_by_address(byte_org);
          if (p) {
            int n = print_reloc(p->type, emit_word);
            if (0 == n) {
              pos += lst_printf("%04X ", emit_word);
            } else {
              pos += n;
            }
          } else {
            pos += lst_printf("%04X ", emit_word);
          }
        } else if (state.obj.section->address + state.obj.section->relocations_tail->address == byte_org) {
          int n = print_reloc(state.obj.section->relocations_tail->type, emit_word);
          if (0 == n) {
            pos += lst_printf("%04X ", emit_word);
          } else {
            pos += n;
          }
        } else {
          pos += lst_printf("%04X ", emit_word);
        }
      } else {
        pos += lst_printf("%04X ", emit_word);
      }

      byte_org += 2;
      lst_bytes += 2;
    }
    if (bytes_emitted - lst_bytes == 1 && pos + 3 <= LINENUM_POS) {
      unsigned char emit_byte;

      b_memory_get(state.i_memory, byte_org, &emit_byte);
      pos += lst_printf("%02X ", emit_byte);
      ++byte_org;
      ++lst_bytes;
    }
  }

  /* append appropriate spacing */
  lst_spaces(LINENUM_POS - pos);

  return lst_bytes;
}

void
lst_format_line(const char *src_line, int value)
{
  unsigned int emitted = 0;
  unsigned int emitted_lines = 0;
  unsigned int byte_org = 0;
  unsigned int bytes_emitted = 0;
  unsigned int lst_bytes;
  const char *addr_fmt = IS_16BIT_CORE ? "%06X " : "%04X   ";
#define ADDR_LEN 7
  unsigned int pos = 0;

  assert(src_line != NULL);

  switch (state.lst.line.linetype) {
  case insn:
    emitted_lines = emitted = state.org - state.lst.line.was_org;
    break;

  case data:
    emitted = state.org - state.lst.line.was_org;
    if ((SECTION_FLAGS & (STYP_TEXT | (state.mpasm_compatible ? STYP_BPACK : 0))) == STYP_TEXT) {
      /* generate line numbers for data directives in program memory;
       * in mpasm compatibility mode code_pack doesn't generate line numbers */
      emitted_lines = emitted;
    }
    break;

  case res:
    if (!IS_RAM_ORG) {
      /* generate line numbers for res directives in program memory */
      emitted_lines = emitted = state.org - state.lst.line.was_org;
    }
    break;

  default:
    break;
  }

  coff_linenum(emitted_lines);

  /* Don't write to file if list is disabled with NOLIST directive */
  if (!state.lst.enabled)
    return;

  switch (state.lst.line.linetype) {
  case equ:
  case set:
    pos += lst_printf("  %08X", value);
    lst_spaces(LINENUM_POS - pos);
    break;

  case set4:
    pos += lst_printf("  %04X", value & 0xffff);
    lst_spaces(LINENUM_POS - pos);
    break;

  case org:
    pos += lst_printf(addr_fmt, gp_processor_byte_to_org(state.device.class, state.org));
    lst_spaces(LINENUM_POS - pos);
    break;

  case idlocs:
    /* not used for 16 bit devices, config is used */
    {
      unsigned short id[2];
      state.device.class->i_memory_get(state.i_memory, state.device.id_location, id);
      state.device.class->i_memory_get(state.i_memory, state.device.id_location+2, id+1);
      pos += lst_printf(addr_fmt, gp_processor_byte_to_org(state.device.class, gp_processor_id_location(state.processor)));
      pos += lst_printf("%04X %04X", id[0], id[1]);
      lst_spaces(LINENUM_POS - pos);
    }
    break;

  case insn:
  case data:
  case res:
    {
      byte_org = state.lst.line.was_org;
      bytes_emitted = emitted;
      pos += lst_printf(addr_fmt, gp_processor_byte_to_org(state.device.class, state.lst.line.was_org));
      lst_bytes = lst_data(pos, byte_org, bytes_emitted);
      byte_org += lst_bytes;
      bytes_emitted -= lst_bytes;
    }
    break;

  case config:
    if (IS_16BIT_CORE) {
      /* config data is byte addressable, but we only want to print
         words in the list file. */
      if (state.lst.config_address == CONFIG4L) {
        /* Special case */
        unsigned short word;
        state.device.class->i_memory_get(state.i_memory,
                                         state.lst.config_address, &word);
        pos += lst_printf(addr_fmt, state.lst.config_address);
        pos += lst_printf("%04X", word);
        lst_spaces(LINENUM_POS - pos);
      } else if((state.lst.config_address & 0x1) == 0) {
        /* if it is an even address don't print anything */
        lst_spaces(LINENUM_POS);
      } else {
        unsigned short word;
        state.device.class->i_memory_get(state.i_memory,
                                         state.lst.config_address - 1, &word);
        pos += lst_printf(addr_fmt, state.lst.config_address - 1);
        pos += lst_printf("%04X", word);
        lst_spaces(LINENUM_POS - pos);
      }
    } else {
      unsigned short word;
      state.device.class->i_memory_get(state.i_memory,
                                       state.lst.config_address, &word);
      pos += lst_printf(addr_fmt,
                        gp_processor_byte_to_org(state.device.class,
                                                 state.lst.config_address));
      pos += lst_printf("%04X", word);
      lst_spaces(LINENUM_POS - pos);
    }
    break;

  case sec:
  case dir:
  case none:
  default:
    lst_spaces(LINENUM_POS);
    break;
  }

  if (state.stGlobal == state.stTop) {
    lst_printf("%05d ", state.src->line_number);
  } else {
    lst_printf("    M ");
  }

  /* Now copy source line to listing, expanding tabs as required */
  {
    int column = 0;
    const char *old = src_line;

    while (*old) {
      if (*old == '\t') {
        int len = state.lst.tabstop - column % state.lst.tabstop;

        column += len;

        while (len--)
          lst_printf(" ");
      } else {
        ++column;

        lst_printf("%c", *old);
      }
      old++;
    }
  }

  /* Tell the .cod file that the next line(s) has an opcode(s) */
  state.cod.emitting = emitted;

  lst_eol();

  if (state.lst.line.linetype == idlocs) {
    unsigned short id[2];
    state.device.class->i_memory_get(state.i_memory,
                                     state.device.id_location + 4, id);
    state.device.class->i_memory_get(state.i_memory,
                                     state.device.id_location + 6, id+1);
    lst_spaces(ADDR_LEN);
    lst_line("%04X %04X ", id[0], id[1]);
  }

  if (bytes_emitted > 0) {
    while (bytes_emitted > 0) {
      /* data left to print on separate lines */

      pos = lst_spaces(ADDR_LEN);
      lst_bytes = lst_data(pos, byte_org, bytes_emitted);
      byte_org += lst_bytes;
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

  ps = lst = malloc(state.stGlobal->count * sizeof (struct symbol *));

  for (i = 0; i < HASH_SIZE; i++)
    for (s = state.stGlobal->hash_table[i]; s; s = s->next)
      if (NULL != s)
        *ps++ = s;

  assert(ps == &lst[state.stGlobal->count]);

  qsort(lst, state.stGlobal->count, sizeof (struct symbol *), symbol_compare);

  cod_write_symbols(lst, state.stGlobal->count);

  free(lst);
}

/* append the symbol table to the .lst file */
struct lst_symbol_s {
  struct symbol *sym;
  enum lst_sym_type_e { LST_SYMBOL, LST_DEFINE, LST_MACRO } type;
};

static int
lst_symbol_compare(const void *p0, const void *p1)
{
  return strcmp(((struct lst_symbol_s *)p0)->sym->name, ((struct lst_symbol_s *)p1)->sym->name);
}

void
lst_symbol_table(void)
{
  int i;
  struct lst_symbol_s *lst, *ps;
  struct symbol *s;
  int count = state.stGlobal->count + state.stDefines->count + state.stMacros->count;
  state.lst.lst_state = in_symtab;

  lst_line("SYMBOL TABLE");
  lst_line("  LABEL                             VALUE");
  lst_line("");

  cod_symbol_table();

  ps = lst = malloc(count * sizeof (struct lst_symbol_s));

  for (i = 0; i < HASH_SIZE; i++)
    for (s = state.stGlobal->hash_table[i]; s; s = s->next) {
      ps->sym = s;
      ps->type = LST_SYMBOL;
      ++ps;
    }

  for (i = 0; i < HASH_SIZE; i++)
    for (s = state.stDefines->hash_table[i]; s; s = s->next) {
      ps->sym = s;
      ps->type = LST_DEFINE;
      ++ps;
    }

  for (i = 0; i < HASH_SIZE; i++)
    for (s = state.stMacros->hash_table[i]; s; s = s->next) {
      ps->sym = s;
      ps->type = LST_MACRO;
      ++ps;
    }

  assert(ps == &lst[count]);

  qsort(lst, count, sizeof (struct lst_symbol_s), lst_symbol_compare);

  for (i = 0; i < count; i++) {
    void *p = get_symbol_annotation(lst[i].sym);
    const char *name = get_symbol_name(lst[i].sym);

    switch (lst[i].type) {
    case LST_SYMBOL:
      /* symbol */
      lst_line("%-32s  %08X", name, p ? ((struct variable *)p)->value : 0);
      break;

    case LST_DEFINE:
      /* define */
      if (p) {
        assert(list == ((struct pnode *)p)->tag);
        assert(string == HEAD((struct pnode *)p)->tag);
      }
      lst_line("%-32s  %s", name, p ? HEAD((struct pnode *)p)->value.string : "");
      break;

    case LST_MACRO:
      /* define */
      lst_line("%-32s  ", name);
      break;
    }
  }
  free(lst);
}

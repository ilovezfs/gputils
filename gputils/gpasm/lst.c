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
#include "gpasm.h"
#include "cod.h"
#include "coff.h"
#include "lst.h"

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

extern int _16bit_core;

void lst_throw()
{
  if(state.lst.f) {
    state.lst.page++;
    fprintf(state.lst.f,
            "%s%-32s%-12s%-29sPAGE %2d\n%s\n%s\n",
            (state.lst.page == 1) ? "" : "\f",
            GPASM_VERSION_STRING,
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

static void lst_check_page_start(void)
{
  if (state.lst.linesperpage != 0 &&
      (state.lst.lineofpage == 0 ||
       state.lst.lineofpage > state.lst.linesperpage)) {
    lst_throw();
    lst_line("LOC  OBJECT CODE     LINE SOURCE TEXT");
    lst_line("  VALUE");
    lst_line(" ");
  }
}

static void lst_eol(void)
{
  if (state.lst.f) {
    fputc('\n', state.lst.f);
    state.lst.line_number++;
    state.lst.lineofpage++;
    cod_lst_line(COD_NORMAL_LST_LINE);
  }
}

void lst_line(const char *format, ...)
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

/* Print part of a line. Output must not contain newline. Needs call
   to lst_eol at end of line. */
static int lst_printf(const char *format, ...)
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

void lst_init()
{
  state.lst.lineofpage = 0;
  state.lst.page = 0;
  state.lst.linesperpage = 60;
  state.lst.line_number = 1;
  state.lst.memorymap = 1;
  state.lst.symboltable = 1;

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

void lst_memory_map(MemBlock *m)
{
  int i, j, base, row_used, num_per_line, num_per_block;

  lst_line("");
  lst_line("");
  lst_line("MEMORY USAGE MAP ('X' = Used,  '-' = Unused)");
  lst_line("");

  if (_16bit_core) {
    /* uses byte addressing so read half as many words */
    num_per_line = 32;
    num_per_block = 8;
  } else {
    num_per_line = 64;
    num_per_block = 16;
  }

  while(m) {
    assert(m->memory != NULL);

    base = (m->base << I_MEM_BITS);

    for (i = 0; i < MAX_I_MEM; i += num_per_line) {
      row_used = 0;

      for (j = 0; j < num_per_line; j++) {
        if (m->memory[i+j] & BYTE_USED_MASK) {
          row_used = 1;
          break;
        }
      }

      if(row_used) {
        lst_printf("%08x :", (i + base) << _16bit_core);
        for (j = 0; j < num_per_line; j++) {
          if ((j % num_per_block) == 0) {
            lst_printf(" ");
          }
          if (m->memory[i + j] & BYTE_USED_MASK) {
            lst_printf("X");
            if (_16bit_core) {
              /* each word has two bytes */
              lst_printf("X");
            }
          } else {
            lst_printf("-");
            if (_16bit_core) {
              /* each word has two bytes */
              lst_printf("-");
            }
          }
        }

        lst_eol();
      }
    }

    m = m->next;
  }

  lst_line("");
  lst_line("All other memory blocks unused.");
  lst_line("");

  lst_line("Program Memory Words Used: %i",
           gp_processor_byte_to_org(state.device.class, b_memory_used(state.i_memory)));
}

void lst_close()
{
  cod_lst_line(COD_LAST_LST_LINE);

  if (state.lst.f) {
    fprintf(state.lst.f,
            "\n\n");
    fprintf(state.lst.f,
            "Errors   : %7d\n",
            state.num.errors);
    fprintf(state.lst.f,
            "Warnings : %7d reported, %7d suppressed\n",
            state.num.warnings,
            state.num.warnings_suppressed);
    fprintf(state.lst.f,
            "Messages : %7d reported, %7d suppressed\n",
            state.num.messages,
            state.num.messages_suppressed);
    fprintf(state.lst.f,
            "\f\n");

    fclose(state.lst.f);
  }
}

unsigned int lst_data(unsigned int byte_org,
                      unsigned int bytes_emitted)
{
#define DATA_BYTES_PER_LINE 3

  unsigned int lst_bytes = 0;
  int len = 0;

  if (bytes_emitted == 0)
    return 0;

  /* when in a idata or byte packed section, print byte by byte */
  if (state.obj.new_sec_flags & (STYP_DATA|STYP_BPACK)) {
    /* list first byte */
    if ((byte_org & 1) != 0) {
      unsigned char emit_byte;
      b_memory_get(state.i_memory, byte_org, &emit_byte);
      len += lst_printf("%02X ", emit_byte);
      ++lst_bytes;
    }
    /* list whole words */
    if (1 < bytes_emitted - lst_bytes) {
      unsigned char emit_bytes[2];
      b_memory_get(state.i_memory, byte_org+lst_bytes, emit_bytes);
      b_memory_get(state.i_memory, byte_org+lst_bytes+1, emit_bytes+1);
      len += lst_printf("%02X %02X ", emit_bytes[0], emit_bytes[1]);
      lst_bytes += 2;
    }
    if (lst_bytes < DATA_BYTES_PER_LINE && bytes_emitted > lst_bytes) {
      /* we have space for an extra byte */
      unsigned char emit_byte;
      b_memory_get(state.i_memory, byte_org+lst_bytes, &emit_byte);
      len += lst_printf("%02X", emit_byte);
      ++lst_bytes;
    }
  }
  else {    /* non-code pack section */
    /* list full words as bytes */
    for (; lst_bytes+1 < bytes_emitted && lst_bytes < 4; lst_bytes+=2) {
      unsigned short emit_word;
      state.device.class->i_memory_get(state.i_memory, byte_org+lst_bytes,
                                       &emit_word);
      len += lst_printf("%04X ", emit_word);
    }
    if (bytes_emitted < 4) {
      /* list extra byte if odd */
      if (lst_bytes < bytes_emitted) {
        unsigned char emit_byte;
        b_memory_get(state.i_memory, byte_org+lst_bytes, &emit_byte);
        len += lst_printf("%02X   ", emit_byte);
        ++lst_bytes;
      }
      else {
        len += lst_printf("     ");
      }
    }
  }

  /* append appropriate spacing */
  for(; len < 10; len++) {
    lst_printf(" ");
  }
  return lst_bytes;
}

void lst_format_line(const char *src_line, int value)
{
  unsigned int emitted = 0;
  unsigned int byte_org = 0;
  unsigned int bytes_emitted = 0;
  unsigned int lst_bytes;

  assert(src_line != NULL);

  switch (state.lst.line.linetype) {
  case insn:
    emitted = state.org - state.lst.line.was_org;
    break;
  case data:
    if (SECTION_FLAGS & STYP_TEXT) {
      /* generate line numbers for data directives in program memory */
      emitted = state.org - state.lst.line.was_org;
    }
    break;
  case res:
    if (!IS_RAM_ORG) {
      /* generate line numbers for res directives in program memory */
      emitted = state.org - state.lst.line.was_org;
    }
    break;
  default:
    break;
  }

  coff_linenum(emitted);

  /* Don't write to file if list is disabled with NOLIST directive */
  if (!state.lst.enabled)
    return;

  switch (state.lst.line.linetype) {
  case equ:
  case set:
    lst_printf("  %08X     ", value);
    break;
  case org:
    lst_printf("%04X           ", gp_processor_byte_to_org(state.device.class, state.org));
    break;
  case idlocs:
    /* not used for 16 bit devices, config is used */
    {
      unsigned short id[2];
      state.device.class->i_memory_get(state.i_memory, state.device.id_location, id);
      state.device.class->i_memory_get(state.i_memory, state.device.id_location+2, id+1);
      lst_printf("%04X %04X %04X ",
               gp_processor_byte_to_org(state.device.class, gp_processor_id_location(state.processor)),
               id[0], id[1]);
    }
    break;
  case insn:
  case data:
    byte_org = state.lst.line.was_org;
    bytes_emitted = emitted;
    lst_printf("%04X ",
               gp_processor_byte_to_org(state.device.class, state.lst.line.was_org));

    lst_bytes = lst_data(byte_org, bytes_emitted);
    byte_org += lst_bytes;
    bytes_emitted -= lst_bytes;

    break;
  case config:
    if(_16bit_core) {
      /* config data is byte addressable, but we only want to print
         words in the list file. */
      if (state.lst.config_address == CONFIG4L) {
        /* Special case */
        unsigned short word;
        state.device.class->i_memory_get(state.i_memory,
                                         state.lst.config_address, &word);
        lst_printf("%06X %04X    ", state.lst.config_address, word);
      } else if((state.lst.config_address & 0x1) == 0) {
        /* if it is an even address don't print anything */
        lst_printf("               ");
      } else {
        unsigned short word;
        state.device.class->i_memory_get(state.i_memory,
                                         state.lst.config_address - 1, &word);
        lst_printf("%06X %04X    ", state.lst.config_address - 1, word);
      }
    } else {
      unsigned short word;
      state.device.class->i_memory_get(state.i_memory,
                                       state.lst.config_address, &word);
      lst_printf("%06X %04X    ",
                 gp_processor_byte_to_org(state.device.class,
                                          state.lst.config_address),
                 word);
    }
    break;
  case res:
  case sec:
  case dir:
  case none:
  default:
    lst_printf("               ");
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
    lst_line("     %04X %04X ", id[0], id[1]);
  }

  if (bytes_emitted > 0) {
    while (bytes_emitted > 0) {
      /* data left to print on separate lines */

      lst_printf("     ");
      lst_bytes = lst_data(byte_org, bytes_emitted);
      byte_org += lst_bytes;
      bytes_emitted -= lst_bytes;
      lst_eol();
    }
    state.cod.emitting = 0;
  }
}

/* append the symbol table to the .lst file */

void lst_symbol_table(struct symbol_table *table)
{
  int i;
  const char *symbol_format = "%-32s  %08X";
  struct symbol **lst, **ps, *s;

  lst_line("SYMBOL TABLE");
  lst_line("%-32s  %-8s", "  LABEL", "  VALUE");
  lst_line("");

  ps = lst = malloc(table->count * sizeof(lst[0]));

  for (i = 0; i < HASH_SIZE; i++)
    for (s = table->hash_table[i]; s; s = s->next)
      *ps++ = s;

  assert(ps == &lst[table->count]);

  qsort(lst, table->count, sizeof(lst[0]), symbol_compare);

  for (i = 0; i < table->count; i++) {
    struct variable *var;

    var = get_symbol_annotation(lst[i]);
    lst_line(symbol_format,
             get_symbol_name(lst[i]),
             var ? var->value : 0);
  }
  cod_write_symbols(lst,table->count);
}

void lst_defines_table(struct symbol_table *table)
{
  int i;
  const char *symbol_format = "%-32s  %s";
  struct symbol **lst, **ps, *s;

  ps = lst = malloc(table->count * sizeof(lst[0]));

  for (i = 0; i < HASH_SIZE; i++)
    for (s = table->hash_table[i]; s; s = s->next)
      *ps++ = s;

  assert(ps == &lst[table->count]);

  qsort(lst, table->count, sizeof(lst[0]), symbol_compare);

  for (i = 0; i < table->count; i++) {
    char *defined_as;

    defined_as = get_symbol_annotation(lst[i]);
    lst_line(symbol_format,
             get_symbol_name(lst[i]),
             defined_as);
  }
}

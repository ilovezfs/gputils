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

void lst_line(char *line)
{
  if (state.lst.f) {
    if (state.lst.linesperpage != 0) {
      if ((state.lst.lineofpage++ % state.lst.linesperpage) == 0) {
        lst_throw();
        lst_line("LOC  OBJECT CODE     LINE SOURCE TEXT");
        lst_line("  VALUE");
        lst_line(" ");
      }
    }
    fprintf(state.lst.f, "%s\n", line);
    state.lst.line_number++;
    cod_lst_line(COD_NORMAL_LST_LINE);
  }
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
  state.lst.tabstop = 8;	/* Default tabstop every 8 */

  if (state.lstfile != named) {
    strncpy(state.lstfilename, state.basefilename, sizeof(state.lstfilename));
    strncat(state.lstfilename, ".lst", sizeof(state.lstfilename));  
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
  char buf[BUFSIZ];
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
	if (m->memory[i+j] & MEM_USED_MASK) {
	  row_used = 1;
	  break;
	}
      }

      if(row_used) {
        snprintf(buf, sizeof(buf), "%08x :", (i + base) << _16bit_core);
	for (j = 0; j < num_per_line; j++) {
          if ((j % num_per_block) == 0) {
	    strncat(buf, " ", sizeof(buf));
          }
          if (m->memory[i + j] & MEM_USED_MASK) {
	    strncat(buf, "X", sizeof(buf));
	    if (_16bit_core) {
	      /* each word has two bytes */
	      strncat(buf, "X", sizeof(buf));
	    }
          } else {
	    strncat(buf, "-", sizeof(buf));
	    if (_16bit_core) {
	      /* each word has two bytes */
	      strncat(buf, "-", sizeof(buf));
	    }
          }
        }

        lst_line(buf);
      }
    }

    m = m->next;
  }

  lst_line("");
  lst_line("All other memory blocks unused.");
  lst_line("");

  snprintf(buf, sizeof(buf), 
           "Program Memory Words Used: %i",
           i_memory_used(state.i_memory));
  lst_line(buf);

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

unsigned int lst_data(char *m, unsigned int byte_org,
    unsigned int bytes_emitted, size_t sizeof_m)
{
  char buf[BUFSIZ];
  unsigned int i;
  unsigned int lst_bytes = 0;
  size_t m_spacing, m_prev_len = strlen(m), m_after_len;

  if (bytes_emitted == 0)
    return 0;

  /* when in a byte packed section, print byte by byte */
  if (state.obj.new_sec_flags & STYP_BPACK) {
    gp_boolean started_odd = (byte_org & 1) != 0;

    if (started_odd) {
      /* not word-aligned */
      /* list first byte */
      unsigned char emit_byte = (unsigned char)(i_memory_get(state.i_memory,
          (byte_org >> 1)) >> 8);
      snprintf(buf, sizeof(buf), "%02X ", emit_byte);
      strncat(m, buf, sizeof_m);
      ++lst_bytes;
    } else if (bytes_emitted == 1) {
      /* word-aligned */
      /* but only one byte */
      unsigned char emit_byte = (unsigned char)(i_memory_get(state.i_memory,
          (byte_org >> 1)) & 0xff);
      snprintf(buf, sizeof(buf), "%02X ", emit_byte);
      strncat(m, buf, sizeof_m);
      ++lst_bytes;
    }
    /* list whole words */
    for (i = 0; (i < ((bytes_emitted-started_odd) >> 1)) && (i < 1); ++i) {
      unsigned int emit_word = i_memory_get(state.i_memory,
          ((byte_org+started_odd) >> 1) + i) & 0xffff;
      snprintf(buf, sizeof(buf), "%02X %02X ", emit_word & 0x00ff,
          emit_word >> 8);
      strncat(m, buf, sizeof_m);
      lst_bytes += 2;
    }
    if (i != 1 && started_odd && bytes_emitted > 1) {
      /* we have space for an extra byte if we had no whole word and we started odd */
      snprintf(buf, sizeof(buf), "%02X", i_memory_get(state.i_memory,
          ((byte_org + 1) >> 1)) & 0xff00 >> 8);
      strncat(m, buf, sizeof_m);
      ++lst_bytes;
    } else if (!started_odd && i == 1 && bytes_emitted > 2) {
      /* we have space for another byte, word aligned */
      snprintf(buf, sizeof(buf), "%02X", i_memory_get(state.i_memory,
          (byte_org + 2) >> 1) & 0xff);
      strncat(m, buf, sizeof_m);
      ++lst_bytes;
    }
  }
  else {    /* non-code pack section */
    /* list full words as bytes */
    for (i = 0; (i < (bytes_emitted >> 1)) && (i < 2); ++i) {
      unsigned int emit_word = i_memory_get(state.i_memory,
            (byte_org>>1) + i) & 0xffff;
      snprintf(buf, sizeof(buf), "%04X ", emit_word);
      strncat(m, buf, sizeof_m);
      lst_bytes += 2;
    }
    if (bytes_emitted < 4) {
      /* list extra byte if odd */
      if (((byte_org+bytes_emitted) & 1) != 0) {
        snprintf(buf, sizeof(buf), "%02X   ", i_memory_get(state.i_memory,
                (byte_org+bytes_emitted)>>1) & 0x00ff);
        strncat(m, buf, sizeof_m);
        ++lst_bytes;
      }
      else {
        strncat(m, "     ", sizeof_m);
      }
    }
  }

  /* append appropriate spacing */
  m_after_len = strlen(m);
  m_spacing = m_after_len - m_prev_len;
  for(; m_spacing < 10; m_spacing++) {
    m[m_after_len++] = ' ';
  }
  m[m_after_len] = '\0';
  return lst_bytes;
}

void lst_format_line(char *src_line, int value)
{
  char m[BUFSIZ];
  char buf[BUFSIZ];
  unsigned int emitted = 0;
  unsigned int byte_org = 0;
  unsigned int bytes_emitted = 0;
  unsigned int lst_bytes;

  assert(src_line != NULL);
  
  switch (state.lst.line.linetype) {
  case equ: 
  case set:
    snprintf(m, sizeof(m), "  %08X", value);
    strncat(m, "     ", sizeof(m));
    break;
  case org:
    snprintf(m, sizeof(m), "%04X      ", state.org << _16bit_core);
    strncat(m, "     ", sizeof(m));
    break;
  case idlocs:
    /* not used for 16 bit devices, config is used */
    snprintf(m, sizeof(m), "%04X %04X %04X ",
            state.device.id_location,
            i_memory_get(state.i_memory, state.device.id_location) & 0xffff,
            i_memory_get(state.i_memory, 
		         state.device.id_location + 1) & 0xffff);
    break;
  case insn:
    byte_org = (state.lst.line.was_org << 1);
    if (state.obj.section)
      byte_org -= (state.obj.section->emitted_pack_byte ? 1 : 0);
    bytes_emitted = (state.org << 1) - byte_org;
    
    /* deal with offset changes for non-word aligned data */
    if (state.obj.section) {
      /* do we have a pending byte for a full word? */
      if (state.obj.section->have_pack_byte) {
        /* did we emit some bytes? if so we have to subtract a half word */
        if (bytes_emitted > 0 && state.obj.section->have_pack_byte)
          bytes_emitted--;
        /* is this a label or something with no instructions? then our
         * org must be modified. */
        else if (bytes_emitted == 0)
          byte_org--;
      }
    }
    emitted = (bytes_emitted >> 1);
    if (bytes_emitted > 0 && ((byte_org & 1) == 0) && ((bytes_emitted & 1) != 0))
      emitted += 1;
    snprintf(m, sizeof(m), "%04X ", byte_org >> (1 - _16bit_core));

    lst_bytes = lst_data(m, byte_org, bytes_emitted, sizeof(m));
    byte_org += lst_bytes;
    bytes_emitted -= lst_bytes;

    break;
  case config:
    if(_16bit_core) {
      /* config data is byte addressable, but we only want to print
	 words in the list file. */
      if (state.lst.config_address == CONFIG4L) {
        /* Special case */
        snprintf(m, sizeof(m), "%06X %04X    ",   
                 state.lst.config_address,
                 i_memory_get(state.i_memory, 
		 	      state.lst.config_address >> 1) & 0xffff);
      } else if((state.lst.config_address & 0x1) == 0) {
        /* if it is an even address don't print anything */
	strncpy(m, "               ", sizeof(m));
      } else {
        snprintf(m, sizeof(m), "%06X %04X    ",   
                 state.lst.config_address - 1,
                 i_memory_get(state.i_memory, 
			     (state.lst.config_address - 1) >> 1) & 0xffff);
      }
    } else {
      snprintf(m, sizeof(m), "%06X %04X    ",   
               state.lst.config_address,
               i_memory_get(state.i_memory, 
		            state.lst.config_address) & 0xffff);
    }
    break;
  case res:
    strncpy(m, "               ", sizeof(m));
    if (SECTION_FLAGS & STYP_TEXT) {
      /* generate line numbers for res directives in program memory */
      emitted = state.org - state.lst.line.was_org;
    }
    break;
  case sec:
  case dir:
  case none:
  default:
    strncpy(m, "               ", sizeof(m));
    break;
  }

  if (state.stGlobal == state.stTop) {
    snprintf(buf, sizeof(buf), "%05d ", state.src->line_number);
  } else {		  
    snprintf(buf, sizeof(buf), "    M ");
  }
  strncat(m, buf, sizeof(m));

  /* Now copy 'l' to 'e', expanding tabs as required */
  {
    int offset = strlen(m);
    int column = 0;
    char *old;
    char *e = m + offset;

    old = src_line;

    while (*old) {
      if (*old == '\t') {
        int len = state.lst.tabstop - column % state.lst.tabstop;

        if ((offset + (column += len)) >= sizeof(m))
          break;

        while (len--)
          *e++ = ' ';
      } else {
        if ((offset + (++column)) >= sizeof(m))
          break;

        *e++ = *old;
      }
      old++;
    }
    *e = '\0';		/* terminate the new string */
  }

  coff_linenum(emitted);

  /* Don't write to file is list is disabled */
  if (!state.lst.enabled)
    return;

  /* Tell the .cod file that the next line(s) has an opcode(s) */
  state.cod.emitting = emitted;

  lst_line(m);

#ifdef GPUTILS_DEBUG
  fprintf(stderr, "%s\n\n", m);
#endif

  if (state.lst.line.linetype == idlocs) {
    snprintf(m, sizeof(m), "     %04X %04X ",
             i_memory_get(state.i_memory, 
		          state.device.id_location + 2) & 0xffff,
             i_memory_get(state.i_memory, 
		          state.device.id_location + 3) & 0xffff);      
    lst_line(m);   
  }

  if (bytes_emitted > 0) {
    while (bytes_emitted > 0) {
      /* data left to print on separate lines */

      strncpy(m, "     ", sizeof(m));
      lst_bytes = lst_data(m, byte_org, bytes_emitted, sizeof(m));
      byte_org += lst_bytes;
      bytes_emitted -= lst_bytes;
      lst_line(m);
    }
    state.cod.emitting = 0;
  }
 
  /* we reset this here, otherwise labels will display bytes again */
  if(state.obj.section)
    state.obj.section->emitted_pack_byte = false;
}

/* append the symbol table to the .lst file */

void lst_symbol_table(struct symbol_table *table)
{
  int i;
  const char *symbol_format = "%-32s  %08X";
  struct symbol **lst, **ps, *s;
  char buf[BUFSIZ];

  lst_line("SYMBOL TABLE");
  snprintf(buf, sizeof(buf), "%-32s  %-8s", "  LABEL", "  VALUE");
  lst_line(buf);
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
    snprintf(buf, sizeof(buf),
            symbol_format, 
            get_symbol_name(lst[i]),
            var ? var->value : 0);
    lst_line(buf);
  }
  cod_write_symbols(lst,table->count);
}

void lst_defines_table(struct symbol_table *table)
{
  int i;
  const char *symbol_format = "%-32s  %s";
  struct symbol **lst, **ps, *s;
  char buf[BUFSIZ];

  ps = lst = malloc(table->count * sizeof(lst[0]));

  for (i = 0; i < HASH_SIZE; i++)
    for (s = table->hash_table[i]; s; s = s->next) 
      *ps++ = s;

  assert(ps == &lst[table->count]);

  qsort(lst, table->count, sizeof(lst[0]), symbol_compare);

  for (i = 0; i < table->count; i++) {
    char *defined_as;

    defined_as = get_symbol_annotation(lst[i]);
    snprintf(buf, sizeof(buf),
            symbol_format, 
            get_symbol_name(lst[i]),
            defined_as);
    lst_line(buf);
  }
}

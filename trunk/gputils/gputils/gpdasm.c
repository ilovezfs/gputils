/* Disassembles ".HEX" files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2014-2015 Molnar Karoly <molnarkaroly@users.sf.net>

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
#include "gpcfg.h"
#include "gpdasm.h"
#include "labelset.h"
#include "parse.h"
#include "scan.h"

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

#define USER_LABEL_ALIGN        15

extern int yyparse(void);

struct gpdasm_state state = {
  .processor = NULL,                /* processor type */
  .class     = PROC_CLASS_GENERIC,  /* 12 bit device */
  .format    = 1,                   /* output format */
};

static const char *processor_name = NULL;

static gp_cfg_addr_pack_t addr_pack = { 0, };

static struct {
  gp_boolean is_print;
  unsigned int number;
  int words[PIC16E_IDLOCS_SIZE];
} idlocs_pack = { 0, 0, { -1, } };

static gp_boolean prev_empty_line = false;

static char border[] = "===============================================================================";

static char out_buffer[BUFSIZ];

enum {
  OPT_STRICT = 0x100,
  OPT_STRICT_OPTIONS
};

//#define GET_OPTIONS "?chijk:lmnop:stvy"
#define GET_OPTIONS "chijk:lmnop:stvy"

static struct option longopts[] =
{
  { "mnemonics",      no_argument,       NULL, 'c' },
  { "help",           no_argument,       NULL, 'h' },
  { "hex-info",       no_argument,       NULL, 'i' },
  { "mov-fsrn",       no_argument,       NULL, 'j' },
  { "label-list",     required_argument, NULL, 'k' },
  { "list-chips",     no_argument,       NULL, 'l' },
  { "dump",           no_argument,       NULL, 'm' },
  { "show-names",     no_argument,       NULL, 'n' },
  { "show-config",    no_argument,       NULL, 'o' },
  { "processor",      required_argument, NULL, 'p' },
  { "short",          no_argument,       NULL, 's' },
  { "strict-options", no_argument,       NULL, OPT_STRICT_OPTIONS },
  { "use-tab",        no_argument,       NULL, 't' },
  { "version",        no_argument,       NULL, 'v' },
  { "extended",       no_argument,       NULL, 'y' },
  { "strict",         no_argument,       NULL, OPT_STRICT },
  { NULL,             no_argument,       NULL, 0   }
};

/*------------------------------------------------------------------------------------------------*/

static size_t
unexpand(char *Dst, size_t Size, const char *Src)
{
  size_t in_read_idx;
  size_t in_read_virt_idx;
  size_t out_write_idx;
  size_t first_tab;
  size_t last_char;
  size_t space_num;

  if (Size <= 1) {
    return 0;
  }

  --Size;

  in_read_idx      = 0;
  in_read_virt_idx = 0;
  out_write_idx    = 0;
  first_tab        = 0;
  last_char        = 0;
  space_num        = 0;

  while ((Src[in_read_idx] != '\0') && (out_write_idx < Size)) {
    if (Src[in_read_idx] == ' ') {
      ++space_num;
    }
    else if (Src[in_read_idx] == '\n') {
      /* This a newline character. */
      Dst[out_write_idx++] = Src[in_read_idx];
      in_read_virt_idx = 0;
      last_char        = 0;
      space_num        = 0;
      ++in_read_idx;
      continue;
    }
    else {
      /* This a not-space character. */
      if (space_num > 1) {
        first_tab = ((last_char + TABULATOR_SIZE) / TABULATOR_SIZE) * TABULATOR_SIZE;

        if (first_tab > in_read_virt_idx) {
          first_tab = 0;
        }

        if ((last_char + 1) < first_tab) {
          if (first_tab <= in_read_virt_idx) {
            Dst[out_write_idx++] = '\t';

            if (out_write_idx >= Size) {
              goto _exit;
            }

            space_num = in_read_virt_idx - first_tab;
          }

          while (space_num >= TABULATOR_SIZE) {
            Dst[out_write_idx++] = '\t';

            if (out_write_idx >= Size) {
              goto _exit;
            }

            space_num -= TABULATOR_SIZE;
          }
        }
      }

      while (space_num > 0) {
        Dst[out_write_idx++] = ' ';

        if (out_write_idx >= Size) {
          goto _exit;
        }

        --space_num;
      }

      Dst[out_write_idx++] = Src[in_read_idx];
      last_char = in_read_idx;
    }

    ++in_read_idx;
    ++in_read_virt_idx;
  } /* while ((Src[in_read_idx] != '\0') && */

  while (space_num >= TABULATOR_SIZE) {
    Dst[out_write_idx++] = '\t';
    if (out_write_idx >= Size) {
      goto _exit;
    }

    space_num -= TABULATOR_SIZE;
  }

_exit:

  Dst[out_write_idx] = '\0';
  return out_write_idx;
}

/*------------------------------------------------------------------------------------------------*/

static void
ux_print(gp_boolean newline, const char *format, ...)
{
  va_list(ap);
  char buffer[BUFSIZ];
  const char *bptr;
  size_t len;

  va_start(ap, format);
  len = (size_t)vsnprintf(buffer, sizeof(buffer), format, ap);
  va_end(ap);

  if ((int)len < 0) {
    return;
  }

  bptr = buffer;

  if (newline) {
    if (state.use_tab) {
      unexpand(out_buffer, sizeof(out_buffer), buffer);
      bptr = out_buffer;
    }

    puts(bptr);
  }
  else {
    if (state.use_tab) {
      len = unexpand(out_buffer, sizeof(out_buffer), buffer);
      bptr = out_buffer;
    }

    printf("%s", bptr);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
select_processor(void)
{
  pic_processor_t found = NULL;

  if (processor_name == NULL) {
    fprintf(stderr, "Error: Must select the processor.\n");
    exit(1);
  }

  found = gp_find_processor(processor_name);

  if (found != NULL) {
    state.processor = found;
  } else {
    fprintf(stderr, "Error: Didn't find any processor named: %s\n", processor_name);
    printf("Here are the supported processors:\n");
    gp_dump_processor_list(true, PROC_CLASS_UNKNOWN, PROC_CLASS_UNKNOWN, PROC_CLASS_UNKNOWN);
    exit(1);
  }

  state.class = gp_processor_class(state.processor);

  if (state.class->instructions == NULL) {
    fprintf(stderr, "Error: Unsupported processor class.\n");
    exit(1);
  }

  state.proc_regs = gp_register_find_mcu(gp_register_db, gp_register_db_size,
                                         state.processor->names[1]);
}

/*------------------------------------------------------------------------------------------------*/

static void
write_header(void)
{
  if (!state.format) {
    ux_print(true, "        processor %s\n"
                   "        radix dec", state.processor->names[1]);

    if (state.processor->header != NULL) {
      ux_print(true, "\n"
                     "        include %s", state.processor->header);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
write_core_sfr_list(void)
{
  int i;
  const core_sfr_t *table;

  if (state.format || (state.class->core_sfr_table == NULL) ||
      (state.class->core_sfr_number == 0)) {
    return;
  }

  ux_print(true, "\n"
                 "F       equ     1\n"
                 "W       equ     0");

  if (state.class == PROC_CLASS_PIC16E) {
    ux_print(true, "A       equ     0\n"
                   "B       equ     1");
  }

  ux_print(true, "");

  table = state.class->core_sfr_table;
  for (i = state.class->core_sfr_number; i > 0; ++table, --i) {
    if ((state.class == PROC_CLASS_PIC14E) || (state.class == PROC_CLASS_PIC14EX)) {
      if (strcmp(table->name, "FSR0L") == 0) {
        ux_print(true, "FSR0    equ     0x%03x", table->address);
      }
      else if (strcmp(table->name, "FSR1L") == 0) {
        ux_print(true, "FSR1    equ     0x%03x", table->address);
      }
    }

    ux_print(true, "%-7s equ     0x%03x", table->name, table->address);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
end_asm(void)
{
  if (!state.format) {
    ux_print(true, "\n"
                   "        end");
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
write_org(int org, int addr_digits, const char *title, const char *Address_name, int offset)
{
  size_t length;
  char buffer[BUFSIZ];

  if (!state.format) {
    ux_print(true, "");

    if (title != NULL) {
      ux_print(true, "        ; %s", title);
    }

    if (Address_name != NULL) {
      if (offset > 0) {
        length = snprintf(buffer, sizeof(buffer), "        org     (%s + 0x%0*x)", Address_name,
                          addr_digits, offset);
      }
      else {
        length = snprintf(buffer, sizeof(buffer), "        org     %s", Address_name);
      }

      gp_exclamation(buffer, sizeof(buffer), length, "; address: 0x%0*x", addr_digits, org);
    }
    else {
      snprintf(buffer, sizeof(buffer), "        org     0x%0*x", addr_digits, org);
    }

    ux_print(true, "\n%s\n", buffer);
    prev_empty_line = true;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
mark_false_addresses(MemBlock *memory)
{
  MemBlock *m;
  int i, maximum;
  int org;
  int insn_size;
  int num_words;
  unsigned short data;

  m = memory;
  while (m != NULL) {
    i = m->base << I_MEM_BITS;
    maximum = i + MAX_I_MEM;

    insn_size = 2;
    while (i < maximum) {
      org = gp_processor_byte_to_real(state.processor, i);

      if (gp_processor_is_idlocs_org(state.processor, org) >= 0) {
        insn_size = (state.class == PROC_CLASS_PIC16E) ? 1 : 2;
      }
      else if (gp_processor_is_config_org(state.processor, org) >= 0) {
        insn_size = (state.class == PROC_CLASS_PIC16E) ? 1 : 2;
      }
      else if (gp_processor_is_eeprom_org(state.processor, org) >= 0) {
        insn_size = 1;
      }
      else {
        if (state.class->i_memory_get(m, i, &data, NULL, NULL) == W_USED_ALL) {
          num_words = gp_disassemble_mark_false_addresses(m, i, state.processor);
          insn_size = (num_words != 1) ? 4 : 2;
        }
      }

      i += insn_size;
    }

    m = m->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
recognize_labels_and_spec_words(MemBlock *memory)
{
  MemBlock *m;
  int i, maximum, index;
  int org;
  int offset;
  int insn_size;
  int num_words;
  unsigned char byte;
  unsigned short data;
  const vector_t *vector;
  gpdasm_fstate_t fstate;
  const gp_cfg_device_t *dev;
  gp_cfg_addr_hit_t *hit;
  unsigned int max_width;
  unsigned int type;
  lset_symbol_t *sym;

  if (state.show_config) {
    dev = gp_cfg_find_pic_multi_name(state.processor->names, ARRAY_SIZE(state.processor->names));
    if (dev == NULL) {
      fprintf(stderr, "Warning: The %s processor has no entries in the config db.",
              state.processor->names[2]);
    }
  }
  else {
    dev = NULL;
  }

  m = memory;
  fstate.wreg = 0;
  fstate.pclath = 0;
  fstate.pclath_valid = 0xff;
  addr_pack.hit_count = 0;
  max_width = 0;
  idlocs_pack.number = 0;
  idlocs_pack.is_print = true;
  while (m != NULL) {
    i = m->base << I_MEM_BITS;
    maximum = i + MAX_I_MEM;

    insn_size = 2;
    while (i < maximum) {
      org = gp_processor_byte_to_real(state.processor, i);

      if ((index = gp_processor_is_idlocs_org(state.processor, org)) >= 0) {
        insn_size = (state.class == PROC_CLASS_PIC16E) ? 1 : 2;

        if (state.class == PROC_CLASS_PIC16E) {
          if (b_memory_get(m, i, &byte, NULL, NULL)) {
            idlocs_pack.words[index] = byte;

            if (!isprint(byte)) {
              idlocs_pack.is_print = false;
            }

            ++idlocs_pack.number;
          }
          else {
            idlocs_pack.words[index] = -1;
          }
        }
        else {
          if (state.class->i_memory_get(m, i, &data, NULL, NULL) == W_USED_ALL) {
            sym = lset_symbol_find_addr(state.lset_root.sections[SECT_SPEC_CODE], org, -1, true);

            if ((sym != NULL) && !(sym->attr & CSYM_ORG)) {
              if (sym->start == (long)org) {
                b_memory_set_addr_name(m, i, sym->name);
                sym->attr |= CSYM_USED;
              }
            }

            idlocs_pack.words[index] = data;
            ++idlocs_pack.number;
          }
          else {
            idlocs_pack.words[index] = -1;
          }
        }
      }
      else if (gp_processor_is_config_org(state.processor, org) >= 0) {
        insn_size = (state.class == PROC_CLASS_PIC16E) ? 1 : 2;

        if (dev != NULL) {
          if (addr_pack.hit_count < GP_CFG_ADDR_PACK_MAX) {
            if (state.class == PROC_CLASS_PIC16E) {
              if (b_memory_get(m, i, &byte, NULL, NULL)) {
                hit = &addr_pack.hits[addr_pack.hit_count];

                if (gp_cfg_decode_directive(dev, org, byte, hit) > 0) {
                  if (max_width < hit->max_dir_width) {
                    max_width = hit->max_dir_width;
                  }

                  ++addr_pack.hit_count;
                }
              }
            }
            else {
              if (state.class->i_memory_get(m, i, &data, NULL, NULL) == W_USED_ALL) {
                hit = &addr_pack.hits[addr_pack.hit_count];

                if (gp_cfg_decode_directive(dev, org, data, hit) > 0) {
                  if (max_width < hit->max_dir_width) {
                    max_width = hit->max_dir_width;
                  }

                  ++addr_pack.hit_count;
                }
              }
            }
          }
          else {
            fprintf(stderr, "Warning: The value of GP_CFG_ADDR_PACK_MAX too little: %u",
                    GP_CFG_ADDR_PACK_MAX);
          }
        } /* if (dev != NULL) */
      } /* else if (gp_processor_is_config_org(state.processor, org) >= 0) */
      else if ((offset = gp_processor_is_eeprom_org(state.processor, org)) >= 0) {
        if (b_memory_get(m, i, &byte, NULL, NULL)) {
          sym = lset_symbol_find_addr(state.lset_root.sections[SECT_SPEC_EEDATA], offset, -1, true);

          if ((sym != NULL) && !(sym->attr & CSYM_ORG)) {
            if (sym->start == (long)offset) {
              b_memory_set_addr_name(m, i, sym->name);
              sym->attr |= CSYM_USED;
            }
          }
        }

        insn_size = 1;
      }
      else {
        if (state.class->i_memory_get(m, i, &data, NULL, NULL) == W_USED_ALL) {
          sym = lset_symbol_find_addr(state.lset_root.sections[SECT_SPEC_CODE], org, -1, true);

          if ((sym != NULL) && !(sym->attr & CSYM_ORG)) {
            type = (sym->attr & CSYM_DATA) ? W_CONST_DATA : 0;

            if (sym->start == (long)org) {
              type |= W_ADDR_T_LABEL;
              b_memory_set_addr_name(m, i, sym->name);
            }

            b_memory_set_type(m, i, type);
            insn_size = 2;
          }
          else {
            if (state.class == PROC_CLASS_SX) {
            /* Unlike the others, the address of reset vector located the top of program memory. */
              org = (org == state.processor->maxrom) ? -1 : org;
            }

            vector = gp_processor_find_vector(state.class, org);

            if (vector != NULL) {
              b_memory_set_addr_type(m, i, W_ADDR_T_LABEL, 0);
              b_memory_set_addr_name(m, i, vector->name);
            }

            num_words = gp_disassemble_find_labels(m, i, state.processor, &fstate);
            insn_size = (num_words != 1) ? 4 : 2;
          }
        }
      }

      i += insn_size;
    }

    m = m->next;
  }

  addr_pack.max_dir_width = max_width;
}

/*------------------------------------------------------------------------------------------------*/

static void
user_data_finder(MemArg *Argument)
{
  lset_symbol_t *sym;

  if (Argument->arg != NULL) {
    return;
  }

  sym = lset_symbol_find_addr(state.lset_root.sections[SECT_SPEC_DATA], (long)Argument->val, -1, true);

  if (sym != NULL) {
    Argument->arg  = sym->name;
    Argument->offs = (int)((long)Argument->val - sym->start);
    /* This will be necessary for the later listing. */
    sym->attr |= CSYM_USED;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
recognize_registers(MemBlock *memory)
{
  MemBlock *m;
  int i, maximum;
  int org;
  int insn_size;
  int num_words;
  unsigned short data;
  gpdasm_fstate_t fstate;

  if (state.class == PROC_CLASS_SX) {
    return;
  }

  m = memory;
  fstate.wreg = 0;
  fstate.bank = 0;
  fstate.bank_valid = 0xff;
  fstate.proc_regs = state.proc_regs;
  fstate.bsr_boundary = gp_processor_bsr_boundary(state.processor);
  fstate.need_sfr_equ = false;
  while (m != NULL) {
    i = m->base << I_MEM_BITS;
    maximum = i + MAX_I_MEM;

    insn_size = 2;
    while (i < maximum) {
      org = gp_processor_byte_to_real(state.processor, i);

      if (gp_processor_is_idlocs_org(state.processor, org) >= 0) {
        insn_size = (state.class == PROC_CLASS_PIC16E) ? 1 : 2;
      }
      else if (gp_processor_is_config_org(state.processor, org) >= 0) {
        insn_size = (state.class == PROC_CLASS_PIC16E) ? 1 : 2;
      }
      else if (gp_processor_is_eeprom_org(state.processor, org) >= 0) {
        insn_size = 1;
      }
      else {
        if (state.class->i_memory_get(m, i, &data, NULL, NULL) == W_USED_ALL) {
          num_words = gp_disassemble_find_registers(m, i, state.processor,
                                                    &fstate, user_data_finder);
          insn_size = (num_words != 1) ? 4 : 2;
        }
      }

      i += insn_size;
    }

    m = m->next;
  }

  state.need_sfr_equ = fstate.need_sfr_equ;
}

/*------------------------------------------------------------------------------------------------*/

static void
denominate_labels(MemBlock *memory)
{
  MemBlock *m;
  int i, maximum;
  unsigned int type;
  unsigned int func_idx;
  unsigned int label_idx;
  char buffer[BUFSIZ];

  m = memory;
  func_idx  = 0;
  label_idx = 0;
  while (m != NULL) {
    i = m->base << I_MEM_BITS;
    maximum = i + MAX_I_MEM;

    while (i < maximum) {
      type = b_memory_get_addr_type(m, i, NULL, NULL);

      if (type & W_ADDR_T_FUNC) {
        snprintf(buffer, sizeof(buffer), "function_%03u", func_idx);
        b_memory_set_addr_name(m, i, buffer);
        ++func_idx;
      }
      else if (type & W_ADDR_T_LABEL) {
        snprintf(buffer, sizeof(buffer), "label_%03u", label_idx);
        b_memory_set_addr_name(m, i, buffer);
        ++label_idx;
      }

      i += 2;
    }

    m = m->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static size_t
byte_exclamation(char *buffer, size_t buffer_length, size_t current_length, unsigned char byte)
{
  int l;
  size_t length;

  l = snprintf(&buffer[current_length], buffer_length - current_length, "%-*s0x%02x",
               TABULATOR_SIZE, "db", (unsigned int)byte);

  if (l <= 0) {
    return current_length;
  }

  length = current_length + l;

  if (isprint(byte)) {
    gp_exclamation(buffer, buffer_length, length, "; '%c'", byte);
  }

  return length;
}

/*------------------------------------------------------------------------------------------------*/

static void
show_config(void)
{
  gp_cfg_addr_hit_t *hit;
  unsigned int m, n;

  if (addr_pack.hit_count == 0) {
    return;
  }

  ux_print(true, "");
  for (m = 0; m < addr_pack.hit_count; ++m) {
    hit = &addr_pack.hits[m];

    for (n = 0; n < hit->pair_count; ++n) {
      ux_print(true, "        CONFIG  %-*s = %s", addr_pack.max_dir_width,
               hit->pairs[n].directive->name, hit->pairs[n].option->name);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
show_idlocs(void)
{
  unsigned int i, j;
  int word;
  gp_boolean prev_exist, act_exist, aligned;
  char buffer[PIC16E_IDLOCS_SIZE * 2];

  if (idlocs_pack.number == 0) {
    return;
  }

  if (state.class == PROC_CLASS_PIC16E) {
    prev_exist = false;
    act_exist  = false;
    aligned    = false;
    for (i = 0, word = 0; i < PIC16E_IDLOCS_SIZE; ++i) {
      act_exist = (idlocs_pack.words[i] >= 0) ? true : false;

      if (i == 0) {
        /* Start at the beginning. */
        aligned = act_exist;
      }

      if ((prev_exist == false) && (act_exist == true)) {
        ++word;

        if (word > 1) {
          /* There are several separate words or characters. */
          break;
        }
      }

      prev_exist = act_exist;
    }

    if (aligned && (word == 1)) {
      /* One word and start at the beginning. */
      if (idlocs_pack.is_print) {
        /* Only printable characters are there in it. */
        for (i = 0, j = 0; i < PIC16E_IDLOCS_SIZE; ++i) {
          if ((word = idlocs_pack.words[i]) > 0) {
            buffer[j++] = (char)word;
          }
        }

        buffer[j] = '\0';
        ux_print(true, "\n"
                       "        IDLOCS  \"%s\"", buffer);
      }
      else {
        /* Not only printable characters are there in it. */
        ux_print(true, "");
        for (i = 0; i < PIC16E_IDLOCS_SIZE; ++i) {
          if ((word = idlocs_pack.words[i]) >= 0) {
            if (isalnum(word)) {
              ux_print(true, "        __idlocs _IDLOC%u, '%c'", i, word);
            }
            else {
              ux_print(true, "        __idlocs _IDLOC%u, 0x%02X", i, word);
            }
          }
        }
      }
    } /* if (aligned && (word == 1)) */
    else {
      /* There are several separate words or characters, or do not start at the beginning. */
      prev_exist = false;
      act_exist  = false;
      ux_print(true, "");
      for (i = 0; i < PIC16E_IDLOCS_SIZE; ++i) {
        if ((word = idlocs_pack.words[i]) >= 0) {
          act_exist = true;

          if (isalnum(word)) {
            ux_print(true, "        __idlocs _IDLOC%u, '%c'", i, word);
          }
          else {
            ux_print(true, "        __idlocs _IDLOC%u, 0x%02X", i, word);
          }
        }
        else {
          act_exist = false;
        }

      if ((prev_exist == true) && (act_exist == false)) {
        /* Indicates the break. */
        ux_print(true, "");
      }

      prev_exist = act_exist;
      }
    }
  } /* if (state.class == PROC_CLASS_PIC16E) */
  else {
    if (idlocs_pack.number != PIC12_IDLOCS_SIZE) {
      fprintf(stderr, "The IDLOCS size is not %u words!\n", PIC12_IDLOCS_SIZE);
    }

    word  = (idlocs_pack.words[0] & 0x000F) << 12;
    word |= (idlocs_pack.words[1] & 0x000F) << 8;
    word |= (idlocs_pack.words[2] & 0x000F) << 4;
    word |= (idlocs_pack.words[3] & 0x000F);
    ux_print(true, "\n"
                   "        __idlocs 0x%04X", word);
  }
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean need_section_print(const lset_section_t *Section)
{
  unsigned int i;

  for (i = 0; i < Section->symbol_number; ++ i) {
    if (Section->symbol_table[i]->attr & CSYM_USED) {
      return true;
    }
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

static void
list_user_labels(int Addr_digits)
{
  unsigned int i;
  const lset_section_t *sect;
  const lset_symbol_t *sym;
  size_t length;
  char buffer[BUFSIZ];

  if (((sect = state.lset_root.sections[SECT_SPEC_DATA]) != NULL) &&
      (sect->symbol_table != NULL)) {
    if (need_section_print(sect)) {
      printf("\n;%s\n; DATA address definitions\n\n", border);
      for (i = 0; i < sect->symbol_number; ++i) {
        sym = sect->symbol_table[i];

        if (sym->attr & CSYM_USED) {
          length = snprintf(buffer, sizeof(buffer), "%-*s equ     0x%0*lX",
                            USER_LABEL_ALIGN, sym->name, Addr_digits, sym->start);

          if (sym->attr & CSYM_END) {
            gp_exclamation(buffer, sizeof(buffer), length, "; size: %li bytes",
                           sym->end - sym->start + 1);
          }

          ux_print(true, "%s", buffer);
        }
      }
    }
  }

  if (((sect = state.lset_root.sections[SECT_SPEC_EEDATA]) != NULL) &&
      (sect->symbol_table != NULL)) {
    if (need_section_print(sect)) {
      ux_print(true, "\n"
                     ";%s\n"
                     "; EEDATA address definitions\n", border);
      for (i = 0; i < sect->symbol_number; ++i) {
        sym = sect->symbol_table[i];

        if (sym->attr & CSYM_USED) {
          length = snprintf(buffer, sizeof(buffer), "%-*s equ     (__EEPROM_START + 0x%0*lX)",
                            USER_LABEL_ALIGN, sym->name, Addr_digits, sym->start);

          if (sym->attr & CSYM_END) {
            gp_exclamation(buffer, sizeof(buffer), length, "; size: %li bytes",
                           sym->end - sym->start + 1);
          }

          ux_print(true, "%s", buffer);
        }
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
add_constant(lset_section_t *Section, const char *Name, long Start, unsigned int Attr)
{
  if (lset_symbol_find_addr(Section, Start, -1, false) != NULL) {
    return;
  }

  lset_symbol_new(Section, Name, Start, -1, CSYM_START | Attr, 0);
}

/*------------------------------------------------------------------------------------------------*/

static void
load_processor_constants(void)
{
  pic_processor_t processor;
  lset_section_t *sect;
  const int *pair;
  int addr;
  const vector_t *vec;
  unsigned int num;
  char buf[BUFSIZ];

  processor = state.processor;

  /* The code section. */

  if ((sect = state.lset_root.sections[SECT_SPEC_CODE]) == NULL) {
    sect = lset_section_new(&state.lset_root, "CODE", 0);
  }

  if (sect != NULL) {
    addr = processor->prog_mem_size;

    if (addr > 0) {
      add_constant(sect, "__CODE_START",        0, CSYM_ORG);
      add_constant(sect, "__CODE_END",   addr - 1, CSYM_ORG);
    }

    if ((pair = gp_processor_idlocs_exist(processor)) != NULL) {
      lset_symbol_new(sect, "__IDLOCS_START", pair[0], -1, CSYM_START, 0);
      lset_symbol_new(sect, "__IDLOCS_END",   pair[1], -1, CSYM_END,   0);
    }

    if ((pair = gp_processor_config_exist(processor)) != NULL) {
      lset_symbol_new(sect, "__CONFIG_START", pair[0], -1, CSYM_START, 0);
      lset_symbol_new(sect, "__CONFIG_END",   pair[1], -1, CSYM_END,   0);
    }

    if ((processor->class->vector_table != NULL) || (processor->class->vector_number > 0)) {
      vec = processor->class->vector_table;
      num = processor->class->vector_number;

      for (; num; ++vec, --num) {
        buf[0] = '_';
        buf[1] = '_';
        gp_stptoupper(&buf[2], vec->name, sizeof(buf) - 2);

        if (vec->address < 0) {
          /* This a SX type processor. */
          addr = processor->prog_mem_size;

          if (addr > 0) {
            --addr;
          }
        }
        else {
          addr = vec->address;
        }

        add_constant(sect, buf, addr, CSYM_ORG);
      }
    }
  } /* if (sect != NULL) */

  /* The data section. */

  if ((sect = state.lset_root.sections[SECT_SPEC_DATA]) == NULL) {
    sect = lset_section_new(&state.lset_root, "DATA", 0);
  }

  if (sect != NULL) {
    if ((pair = gp_processor_common_ram_exist(processor)) != NULL) {
      if (lset_symbol_find_addr(sect, pair[0], pair[1], false) == NULL) {
        lset_symbol_new(sect, "Common_RAM", pair[0], pair[1], CSYM_START | CSYM_END, 0);
      }
    }

    if ((pair = gp_processor_linear_ram_exist(processor)) != NULL) {
      if (lset_symbol_find_addr(sect, pair[0], pair[1], false) == NULL) {
        lset_symbol_new(sect, "Linear_RAM", pair[0], pair[1], CSYM_START | CSYM_END, 0);
      }
    }
  }

  /* The eedata section. */

  if ((sect = state.lset_root.sections[SECT_SPEC_EEDATA]) == NULL) {
    sect = lset_section_new(&state.lset_root, "EEDATA", 0);
  }

  if (sect != NULL) {
    if ((pair = gp_processor_eeprom_exist(processor)) != NULL) {
      add_constant(sect, "__EEPROM_START", pair[0], CSYM_ORG);
      add_constant(sect, "__EEPROM_END",   pair[1], CSYM_ORG);
    }
  }

  lset_sections_choose(&state.lset_root);
  lset_section_make_symbol_tables(&state.lset_root);
  lset_section_check_bounds(&state.lset_root);
}

/*------------------------------------------------------------------------------------------------*/

static void
dasm(MemBlock *memory)
{
  MemBlock *m;
  int i, maximum;
  int org;
  int offset;
  int insn_size;
  int last_loc;
  int num_words;
  int behavior;
  int bsr_boundary;
  unsigned short data;
  unsigned char byte;
  unsigned int type;
  const char *label_name;
  int addr_digits;
  int word_digits;
  size_t length;
  const lset_symbol_t *sym;
  char buffer[BUFSIZ];

  if (state.show_names && ((state.class == PROC_CLASS_PIC12)   ||
                           (state.class == PROC_CLASS_PIC12E)  ||
                           (state.class == PROC_CLASS_SX)      ||
                           (state.class == PROC_CLASS_PIC14)   ||
                           (state.class == PROC_CLASS_PIC14E)  ||
                           (state.class == PROC_CLASS_PIC14EX) ||
                           (state.class == PROC_CLASS_PIC16)   ||
                           (state.class == PROC_CLASS_PIC16E))) {
    if (state.class == PROC_CLASS_PIC16E) {
      mark_false_addresses(memory);
    }

    recognize_labels_and_spec_words(memory);
    recognize_registers(memory);
    denominate_labels(memory);
  }

  bsr_boundary = gp_processor_bsr_boundary(state.processor);
  addr_digits = state.class->addr_digits;
  word_digits = state.class->word_digits;

  write_header();

  if (state.show_names) {
    ux_print(true, "\n"
                   "; The recognition of labels and registers is not always good, therefore\n"
                   "; be treated cautiously the results.");
    if (state.need_sfr_equ) {
      write_core_sfr_list();
    }
  }

  if (!state.format && state.show_config) {
    show_config();
    show_idlocs();
  }

  if (state.show_names) {
    behavior = GPDIS_SHOW_NAMES | GPDIS_SHOW_BYTES | GPDIS_SHOW_EXCLAMATION;

    if (state.show_fsrn) {
      behavior |= GPDIS_SHOW_FSRN;
    }

    list_user_labels(addr_digits);
  }
  else {
    behavior = GPDIS_SHOW_NOTHING;
  }

  m = memory;
  last_loc = 0;
  while (m != NULL) {
    i = m->base << I_MEM_BITS;
    maximum = i + MAX_I_MEM;

    insn_size = 2;
    while (i < maximum) {
      org = gp_processor_byte_to_real(state.processor, i);

      if ((offset = gp_processor_is_idlocs_org(state.processor, org)) >= 0) {
        /* This is idlocs word/bytes. Not need disassemble. */
        if (state.class == PROC_CLASS_PIC16E) {
          if (!state.show_config) {
            if (b_memory_get(m, i, &byte, NULL, NULL)) {
              if (last_loc != (i - insn_size)) {
                if (state.show_names && (offset == 0)) {
                  ux_print(true, "\n"
                                 ";%s\n"
                                 "; IDLOCS area", border);
                }

                write_org(org, addr_digits, "idlocs", NULL, 0);
              }

              last_loc = i;

              if (state.format) {
                length = snprintf(buffer, sizeof(buffer), "%0*x:  %02x  ",
                                  addr_digits, org, (unsigned int)byte);
              } else {
                length = snprintf(buffer, sizeof(buffer), "        ");
              }

              byte_exclamation(buffer, sizeof(buffer), length, byte);
              ux_print(true, "%s", buffer);
            }
            else {
              last_loc = 0;
            }
          }

          insn_size = 1;
        } /* if (state.class == PROC_CLASS_PIC16E) */
        else {
          if (!state.show_config) {
            if (state.class->i_memory_get(m, i, &data, NULL, NULL)) {
              if (last_loc != (i - insn_size)) {
                sym = lset_symbol_find_addr(state.lset_root.sections[SECT_SPEC_CODE], org, -1, true);

                if ((sym != NULL) && (sym->attr & CSYM_ORG)) {
                  write_org(org, addr_digits, "idlocs", sym->name, 0);
                } else {
                  write_org(org, addr_digits, "idlocs", NULL, 0);
                }
              }

              last_loc = i;

              if (state.format) {
                length = snprintf(buffer, sizeof(buffer), "%0*x:  %0*x  ",
                                  addr_digits, org, word_digits, (unsigned int)data);
              } else {
                length = snprintf(buffer, sizeof(buffer), "        ");
              }

              length += snprintf(buffer + length, sizeof(buffer) - length, "%-*s0x%0*x",
                                 TABULATOR_SIZE, "dw", word_digits, (unsigned int)data);

              if (state.processor->idlocs_mask != 0) {
                unsigned int tmp = (~state.processor->idlocs_mask) & data;

                gp_exclamation(buffer, sizeof(buffer), length, "; in fact: 0x%0*x", word_digits, tmp);
              }

              ux_print(true, "%s", buffer);
            }
            else {
              last_loc = 0;
            }
          }

          insn_size = 2;
        }
      } /* if (gp_processor_is_idlocs_org(state.processor, org) >= 0) */
      else if ((offset = gp_processor_is_config_org(state.processor, org)) >= 0) {
        /* This is config word/bytes. Not need disassemble. */
        if (state.class == PROC_CLASS_PIC16E) {
          if (!state.show_config) {
            if (b_memory_get(m, i, &byte, NULL, NULL)) {
              if (last_loc != (i - insn_size)) {
                if (state.show_names && (offset == 0)) {
                  ux_print(true, "\n"
                                 ";%s\n"
                                 "; CONFIG Bits area", border);
                }

                write_org(org, addr_digits, "config", NULL, 0);
              }

              last_loc = i;

              if (state.format) {
                ux_print(false, "%0*x:  %02x  ", addr_digits, org, (unsigned int)byte);
              } else {
                ux_print(false, "        ");
              }

              ux_print(true, "%-*s0x%02x", TABULATOR_SIZE, "db", (unsigned int)byte);
            }
            else {
              last_loc = 0;
            }
          }

          insn_size = 1;
        } /* if (state.class == PROC_CLASS_PIC16E) */
        else {
          if (!state.show_config) {
            if (state.class->i_memory_get(m, i, &data, NULL, NULL)) {
              if (last_loc != (i - insn_size)) {
                if (state.show_names && (offset == 0)) {
                  ux_print(true, "\n"
                                 ";%s\n"
                                 "; CONFIG Bits area", border);
                }

                write_org(org, addr_digits, "config", NULL, 0);
              }

              last_loc = i;

              if (state.format) {
                ux_print(false, "%0*x:  %0*x  ", addr_digits, org, word_digits, (unsigned int)data);
              } else {
                ux_print(false, "        ");
              }

              ux_print(true, "%-*s0x%0*x", TABULATOR_SIZE, "dw", word_digits, (unsigned int)data);
            }
            else {
              last_loc = 0;
            }
          }

          insn_size = 2;
        }
      } /* else if (gp_processor_is_config_org(state.processor, org) >= 0) */
      else if ((offset = gp_processor_is_eeprom_org(state.processor, org)) >= 0) {
        if (b_memory_get(m, i, &byte, NULL, &label_name)) {
          if (last_loc != (i - insn_size)) {
            sym = lset_symbol_find_addr(state.lset_root.sections[SECT_SPEC_EEDATA], org, -1, true);

            if (state.show_names && (offset == 0)) {
              ux_print(true, "\n"
                             ";%s\n"
                             "; EEDATA area", border);
            }

            if ((sym != NULL) && (sym->attr & CSYM_ORG)) {
              write_org(org, addr_digits, "eeprom", sym->name, 0);
            }
            else {
              sym = lset_symbol_find_addr(state.lset_root.sections[SECT_SPEC_EEDATA],
                                          state.processor->eeprom_addrs[0],
                                          state.processor->eeprom_addrs[1], true);

              if ((sym != NULL) && (sym->attr & CSYM_ORG)) {
                write_org(org, addr_digits, "eeprom", sym->name, offset);
              }
              else {
                write_org(org, addr_digits, "eeprom", NULL, offset);
              }
            }
          }

          last_loc = i;

          if (state.show_names && (label_name != NULL)) {
            length = snprintf(buffer, sizeof(buffer), "%s", label_name);
            gp_exclamation(buffer, sizeof(buffer), length, "; address: 0x%0*x", addr_digits, org);

            if (! prev_empty_line) {
              ux_print(true, "");
            }

            ux_print(true, "%s\n", buffer);
            prev_empty_line = true;
          }

          if (state.format) {
            length = snprintf(buffer, sizeof(buffer), "%0*x:  %02x    ",
                              addr_digits, org, (unsigned int)byte);
          } else {
            length = snprintf(buffer, sizeof(buffer), "        ");
          }

          byte_exclamation(buffer, sizeof(buffer), length, byte);
          ux_print(true, "%s", buffer);
          prev_empty_line = false;
        }
        else {
          last_loc = 0;
        }

        insn_size = 1;
      } /* else if (gp_processor_is_eeprom_org(state.processor, org) >= 0) */
      else {
        /* This is program word. */
        if (state.class->i_memory_get(m, i, &data, NULL, &label_name) == W_USED_ALL) {
          if (last_loc != (i - insn_size)) {
            sym = lset_symbol_find_addr(state.lset_root.sections[SECT_SPEC_CODE], org, -1, true);

            if (state.show_names && (org == 0)) {
              ux_print(true, "\n"
                             ";%s\n"
                             "; CODE area", border);
            }

            if ((sym != NULL) && (sym->attr & CSYM_ORG)) {
              write_org(org, addr_digits, "code", sym->name, 0);
            }
            else {
              write_org(org, addr_digits, "code", NULL, 0);
            }
          }

          last_loc = i;

          if (state.show_names && (label_name != NULL)) {
            length = snprintf(buffer, sizeof(buffer), "%s", label_name);
            gp_exclamation(buffer, sizeof(buffer), length, "; address: 0x%0*x", addr_digits, org);

            if (! prev_empty_line) {
              ux_print(true, "");
            }

            ux_print(true, "%s\n", buffer);
            prev_empty_line = true;
          }

          if (state.format) {
            length = snprintf(buffer, sizeof(buffer), "%0*x:  %0*x  ",
                              addr_digits, org, word_digits, (unsigned int)data);
          } else {
            length = snprintf(buffer, sizeof(buffer), "        ");
          }

          type = b_memory_get_type(m, i);

          if (type & W_CONST_DATA) {
            gp_disassemble_show_data(m, i, state.class, behavior, buffer, sizeof(buffer), length);
            ux_print(true, "%s", buffer);
            prev_empty_line = false;
            num_words = 1;
          }
          else {
            num_words = gp_disassemble(m, i, state.class, bsr_boundary, state.processor->prog_mem_size,
                                       behavior, buffer, sizeof(buffer), length);
            ux_print(true, "%s", buffer);
            prev_empty_line = false;

            if (num_words != 1) {
              /* Some 18xx instructions use two words. */
              if (state.format) {
                state.class->i_memory_get(m, i + 2, &data, NULL, NULL);
                ux_print(true, "%0*x:  %0*x", addr_digits, gp_processor_byte_to_real(state.processor, i + 2),
                         word_digits, (unsigned int)data);
                prev_empty_line = false;
              }

              insn_size = 4;
            }
            else {
              insn_size = 2;
            }
          }
        }
      }

      i += insn_size;
    }

    m = m->next;
  }

  end_asm();
}

/*------------------------------------------------------------------------------------------------*/

static void
show_usage(void)
{
  printf("Usage: gpdasm [options] file\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -c, --mnemonics                Decode the special mnemonics.\n");
  printf("  -h, --help                     Show this usage message.\n");
  printf("  -i, --hex-info                 Show the informations of the input hex file.\n");
  printf("  -j, --mov-fsrn                 In the MOVIW or MOVWI instructions show as base\n");
  printf("                                 the FSRn register instead of the INDFn. [INDFn]\n");
  printf("  -k FILE, --label-list FILE     A file which lists the names and addresses of\n"
         "                                 the labels in the disassembled program code.\n"
         "                                 (With the -n, -o and -s options.)\n");
  printf("  -l, --list-chips               List the supported processors.\n");
  printf("  -m, --dump                     Memory dump of the input hex file.\n");
  printf("  -n, --show-names               For some case of SFR, shows the name of\n"
         "                                 instead of the address. In addition shows\n"
         "                                 the labels also.\n");
  printf("  -o, --show-config              Show the CONFIG and IDLOCS - or __idlocs -\n"
         "                                 directives.\n");
  printf("  -p PROC, --processor PROC      Select the processor.\n");
  printf("  -s, --short                    Print short format output. (Creates a compilable\n"
         "                                 source. See also the -k, -n and -o options.)\n");
  printf("  -t, --use-tab                  Uses tabulator character in the written disassembled\n"
         "                                 text.\n");
  printf("  -v, --version                  Print the gpdasm version information and exit.\n");
  printf("  -y, --extended                 Enable 18xx extended mode.\n");
  printf("      --strict                   Disassemble only opcodes generated by gpasm\n"
         "                                 in case of instructions with several opcodes.\n\n");
  printf("      --strict-options           If this is set, then an option may not be parameter\n"
         "                                 of an another option. For example: -p --dump\n");
  printf("For example:\n"
         "  gpdasm -nos -k program.ulist -p12f1822 program.hex > program.dis\n\n");
  printf("Report bugs to:\n");
  printf("%s\n", PACKAGE_BUGREPORT);
  exit(0);
}

/*------------------------------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
  int option_index;
  const char *command;
  int c;
  gp_boolean strict_options = false;
  gp_boolean print_hex_info = false;
  gp_boolean memory_dump    = false;
  gp_boolean strict         = false;
  gp_boolean usage          = false;
  const char *filename        = NULL;
  const char *label_list_name = NULL;
  const int *pair;

  gp_init();

  state.i_memory = i_memory_create();
  state.show_names  = false;
  state.show_fsrn   = false;
  state.show_config = false;
  state.use_tab     = false;

  /* Scan through the options for the --strict-options flag. */
  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, NULL)) != EOF) {
    if (c == OPT_STRICT_OPTIONS) {
      strict_options = true;
      break;
    }
  }

  /* Restores the getopt_long index. */
  optind = 1;
  while (true) {
    /* This is necessary for the gp_exit_is_excluded_arg() function. */
    option_index = -1;
    command = argv[optind];
    if ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, &option_index)) == EOF) {
      break;
    }

    if (strict_options) {
      gp_exit_if_arg_an_option(longopts, ARRAY_SIZE(longopts), option_index, optarg, c, command);
    }

    switch (c) {
    case '?':
    case 'h':
      usage = true;
      break;

    case 'c':
      gp_decode_mnemonics = true;
      break;

    case 'i':
      print_hex_info = true;
      break;

    case 'j':
      state.show_fsrn = true;
      break;

    case 'k':
      label_list_name = optarg;
      break;

    case 'l':
      gp_dump_processor_list(true, PROC_CLASS_UNKNOWN, PROC_CLASS_UNKNOWN, PROC_CLASS_UNKNOWN);
      exit(0);
      break;

    case 'm':
      memory_dump = true;
      break;

    case 'n':
      state.show_names = true;
      break;

    case 'o':
      state.show_config = true;
      break;

    case 'p':
      processor_name = optarg;
      break;

    case 's':
      state.format = 0;
      break;

    case 't':
      state.use_tab = true;
      break;

    case 'v':
      fprintf(stderr, "%s\n", GPDASM_VERSION_STRING);
      exit(0);
      break;

    case 'y':
      gp_decode_extended = true;
      break;

    case OPT_STRICT:
      strict = true;
      break;

    case OPT_STRICT_OPTIONS:
      /* do nothing */
      break;
    }

    if (usage) {
      break;
    }
  }

  if ((optind + 1) == argc) {
    filename = argv[optind];
  } else {
    usage = true;
  }

  if (usage) {
    show_usage();
  }

  select_processor();

  state.hex_info = readhex(filename, state.i_memory);

  if (state.hex_info->error) {
    state.num.errors++;
  }

  if (strict && (state.class != NULL) && (state.class->patch_strict != NULL)) {
    state.class->patch_strict();
  }

  if (print_hex_info) {
    printf("hex file name:   %s\n", filename);
    printf("hex file format: ");

    if (state.hex_info->hex_format == INHX8M) {
      printf("inhx8m\n");
    } else if (state.hex_info->hex_format == INHX16) {
      printf("inhx16\n");
    } else if (state.hex_info->hex_format == INHX32) {
      printf("inhx32\n");
    } else {
      printf("UNKNOWN\n");
    }

    printf("number of bytes: %i\n\n", state.hex_info->size);
  }

  if (label_list_name != NULL) {
    open_label_source(label_list_name);
    lset_init(&state.lset_root, label_list_name);
    yyparse();
    close_label_source();
    lset_sections_choose(&state.lset_root);
    lset_section_make_symbol_tables(&state.lset_root);
    lset_section_check_bounds(&state.lset_root);

    lset_symbol_check_absolute_limits(state.lset_root.sections[SECT_SPEC_CODE],
                                      0, state.processor->prog_mem_size - 1);

    if (state.class == PROC_CLASS_PIC16E) {
      lset_symbol_check_align(state.lset_root.sections[SECT_SPEC_CODE], 2);
    }

    if (state.lset_root.sections[SECT_SPEC_EEDATA] != NULL) {
      if ((pair = gp_processor_eeprom_exist(state.processor)) != NULL) {
        lset_symbol_check_absolute_limits(state.lset_root.sections[SECT_SPEC_EEDATA],
                                          0, pair[1] - pair[0]);
      }
      else {
        fprintf(stderr, "Error: The processor does not have EEPROM.\n");
        exit(1);
      }
    }
  }
  else {
    lset_init(&state.lset_root, NULL);
  }

  if (state.num.errors == 0) {
    if (memory_dump) {
      print_i_memory(state.i_memory, state.processor);
    } else {
      load_processor_constants();
      dasm(state.i_memory);
    }
  }

  lset_delete(&state.lset_root);
  i_memory_free(state.i_memory);

  return ((state.num.errors > 0) ? EXIT_FAILURE : EXIT_SUCCESS);
}

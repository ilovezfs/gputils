/* PIC Processor selection
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
#include "gpmsg.h"

/*------------------------------------------------------------------------------------------------*/

static void
_make_bank_constants(pic_processor_t Proc, int Num_of_banks)
{
  int  bank_size;
  int  bank_addr;
  int  i;
  char name[64];

  i = (Num_of_banks == 1) ? 0 : __ACTIVE_BANK_INV;

  set_global(__ACTIVE_BANK_ADDR, i, VAL_VARIABLE, true);

  if (Num_of_banks <= 0) {
    return;
  }

  set_global("__NUM_BANKS", Num_of_banks, VAL_CONSTANT, true);

  bank_size = Proc->class->bank_size;
  bank_addr = 0;
  i         = 0;
  while (true) {
    if (i == 0) {
      set_global("__BANK_FIRST", bank_addr, VAL_CONSTANT, true);
    }

    snprintf(name, sizeof(name), "__BANK_%i", i);
    set_global(name, bank_addr, VAL_CONSTANT, true);
    ++i;

    if (i == Num_of_banks) {
      set_global("__BANK_LAST", bank_addr, VAL_CONSTANT, true);
      break;
    }

    bank_addr += bank_size;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_make_page_constants(pic_processor_t Proc, int Num_of_pages)
{
  int  page_size;
  int  page_addr;
  int  i;
  char name[64];

  i = (Num_of_pages == 1) ? 0 : __ACTIVE_PAGE_INV;

  set_global(__ACTIVE_PAGE_ADDR, i, VAL_VARIABLE, true);

  if (Num_of_pages <= 0) {
    return;
  }

  set_global("__NUM_PAGES", Num_of_pages, VAL_CONSTANT, true);

  page_size = Proc->class->page_size;
  page_addr = 0;
  i         = 0;
  while (true) {
    if (i == 0) {
      set_global("__PAGE_FIRST", page_addr, VAL_CONSTANT, true);
    }

    snprintf(name, sizeof(name), "__PAGE_%i", i);
    set_global(name, page_addr, VAL_CONSTANT, true);
    ++i;

    if (i == Num_of_pages) {
      set_global("__PAGE_LAST", page_addr, VAL_CONSTANT, true);
      break;
    }

    page_addr += page_size;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
select_processor(const char *name)
{
  pic_processor_t  found;
  proc_class_t     class;
  const int       *pair;
  const vector_t  *vec;
  unsigned int     num;
  int              addr;
  int              num_of_banks;
  int              num_of_pages;
  char             buf[BUFSIZ];
  int              badrom_idx;
  long             start;
  long             end;

  if (state.cmd_line.processor) {
    gpmsg_vwarning(GPW_CMDLINE_PROC, NULL);
  }
  else {
    found = gp_find_processor(name);

    if (found != NULL) {
      if (state.processor == NULL) {
        /* If in extended mode: Check if processor supports extended instruction set. */
        if (state.extended_pic16e && !(found->pic16e_flags & PIC16E_FLAG_HAVE_EXTINST)) {
          gpmsg_verror(GPE_NO_EXTENDED_MODE, NULL);
        }

        state.processor = found;
        state.maxram    = found->maxram;
        state.maxrom    = found->maxrom;

        if (state.maxrom > 0) {
          /* Initialize badrom from internal processor info. */
          gp_bitarray_create(&state.badrom, state.maxrom);

          for (badrom_idx = 0; badrom_idx < MAX_BADROM; badrom_idx += 2) {
            start = found->badrom[badrom_idx];
            end   = found->badrom[badrom_idx + 1];

            if ((start >= 0) && (end >= start)) {
              gp_bitarray_write_range(&state.badrom, start, end, true);
            }
          }
        }

        set_global(found->defined_as, 1, VAL_CONSTANT, false);

        if (!state.mpasm_compatible) {
          class = found->class;

          num_of_banks = gp_processor_num_banks(found);
          num_of_pages = gp_processor_num_pages(found);

          if ((class == PROC_CLASS_GENERIC) || (class == PROC_CLASS_PIC12) ||
              (class == PROC_CLASS_PIC12E)  || (class == PROC_CLASS_PIC12I)) {
            if ((class == PROC_CLASS_PIC12E) || (class == PROC_CLASS_PIC12I)) {
              set_global("__ENHANCED", 1,                 VAL_CONSTANT, true);
            }

            set_global("__12_BIT",     1,                 VAL_CONSTANT, true);
            set_global("__PAGE_BITS",  PIC12_PAGE_BITS,   VAL_CONSTANT, true);
            set_global("__PAGE_MASK",  class->page_mask,  VAL_CONSTANT, true);
            set_global("__PAGE_SHIFT", PIC12_PAGE_SHIFT,  VAL_CONSTANT, true);
            set_global("__PAGE_SIZE",  class->page_size,  VAL_CONSTANT, true);
            set_global("__PAGE_INV",   __ACTIVE_PAGE_INV, VAL_CONSTANT, true);

            _make_bank_constants(found, num_of_banks);
            _make_page_constants(found, num_of_pages);
          }
          else if (class == PROC_CLASS_SX) {
            set_global("__12_BIT",     1,                 VAL_CONSTANT, true);
            set_global("__PAGE_BITS",  SX_PAGE_BITS,      VAL_CONSTANT, true);
            set_global("__PAGE_MASK",  class->page_mask,  VAL_CONSTANT, true);
            set_global("__PAGE_SHIFT", PIC12_PAGE_SHIFT,  VAL_CONSTANT, true);
            set_global("__PAGE_SIZE",  class->page_size,  VAL_CONSTANT, true);
            set_global("__PAGE_INV",   __ACTIVE_PAGE_INV, VAL_CONSTANT, true);

            _make_bank_constants(found, num_of_banks);
            _make_page_constants(found, num_of_pages);
          }
          else if (class == PROC_CLASS_PIC14) {
            set_global("__14_BIT",     1,                 VAL_CONSTANT, true);
            set_global("__PAGE_BITS",  PIC14_PAGE_BITS,   VAL_CONSTANT, true);
            set_global("__PAGE_MASK",  class->page_mask,  VAL_CONSTANT, true);
            set_global("__PAGE_SHIFT", PIC14_PAGE_SHIFT,  VAL_CONSTANT, true);
            set_global("__PAGE_SIZE",  class->page_size,  VAL_CONSTANT, true);
            set_global("__PAGE_INV",   __ACTIVE_PAGE_INV, VAL_CONSTANT, true);

            _make_bank_constants(found, num_of_banks);
            _make_page_constants(found, num_of_pages);
          }
          else if ((class == PROC_CLASS_PIC14E) || (class == PROC_CLASS_PIC14EX)) {
            set_global("__14_BIT",        1,                    VAL_CONSTANT, true);
            set_global("__ENHANCED",      1,                    VAL_CONSTANT, true);
            set_global("__PAGE_BITS",     PIC14E_PAGE_BITS,     VAL_CONSTANT, true);
            set_global("__PAGE_MASK",     class->page_mask,     VAL_CONSTANT, true);
            set_global("__PAGE_SHIFT",    PIC14_PAGE_SHIFT,     VAL_CONSTANT, true);
            set_global("__PAGE_SIZE",     class->page_size,     VAL_CONSTANT, true);
            set_global("__PAGE512_SHIFT", PIC14E_PAGE512_SHIFT, VAL_CONSTANT, true);
            set_global("__PAGE_INV",      __ACTIVE_PAGE_INV,    VAL_CONSTANT, true);

            _make_bank_constants(found, num_of_banks);
            _make_page_constants(found, num_of_pages);
          }
          else if (class == PROC_CLASS_PIC16) {
            set_global("__16_BIT",     1,                 VAL_CONSTANT, true);
            set_global("__PAGE_BITS",  PIC16_PAGE_BITS,   VAL_CONSTANT, true);
            set_global("__PAGE_MASK",  class->page_mask,  VAL_CONSTANT, true);
            set_global("__PAGE_SHIFT", PIC16_PAGE_SHIFT,  VAL_CONSTANT, true);
            set_global("__PAGE_SIZE",  class->page_size,  VAL_CONSTANT, true);
            set_global("__PAGE_INV",   __ACTIVE_PAGE_INV, VAL_CONSTANT, true);

            _make_bank_constants(found, num_of_banks);
            _make_page_constants(found, num_of_pages);
          }
          else if (class == PROC_CLASS_PIC16E) {
            set_global("__16_BIT",   1, VAL_CONSTANT, true);
            set_global("__EXTENDED", 1, VAL_CONSTANT, true);

            _make_bank_constants(found, num_of_banks);
          }

          set_global("__BANK_BITS",  found->bank_bits,       VAL_CONSTANT, true);
          set_global("__BANK_MASK",  class->bank_mask,       VAL_CONSTANT, true);
          set_global("__BANK_SHIFT", class->bank_bits_shift, VAL_CONSTANT, true);
          set_global("__BANK_SIZE",  class->bank_size,       VAL_CONSTANT, true);
          set_global("__BANK_INV",   __ACTIVE_BANK_INV,      VAL_CONSTANT, true);

          if ((pair = gp_processor_common_ram_exist(found)) != NULL) {
            set_global("__COMMON_RAM_START", pair[0], VAL_CONSTANT, true);
            set_global("__COMMON_RAM_END",   pair[1], VAL_CONSTANT, true);
          }

          if (found->common_ram_max > 0) {
            set_global("__COMMON_RAM_MAX", found->common_ram_max, VAL_CONSTANT, true);
          }

          if ((pair = gp_processor_linear_ram_exist(found)) != NULL) {
            set_global("__LINEAR_RAM_START", pair[0], VAL_CONSTANT, true);
            set_global("__LINEAR_RAM_END",   pair[1], VAL_CONSTANT, true);
          }

          if ((class->vector_table != NULL) || (class->vector_number > 0)) {
            vec = class->vector_table;
            num = class->vector_number;

            for (; num; ++vec, --num) {
              buf[0] = '_';
              buf[1] = '_';
              gp_stptoupper(&buf[2], vec->name, sizeof(buf) - 2);

              if (vec->address < 0) {
                /* This a SX type processor. */
                addr = found->prog_mem_size;

                if (addr > 0) {
                  --addr;
                }
              }
              else {
                addr = vec->address;
              }

              set_global(buf, addr, VAL_CONSTANT, true);
            }
          }

          addr = found->prog_mem_size;

          if (addr > 0) {
            set_global("__CODE_START",        0, VAL_CONSTANT, true);
            set_global("__CODE_END",   addr - 1, VAL_CONSTANT, true);
          }

          if ((pair = gp_processor_idlocs_exist(found)) != NULL) {
            set_global("__IDLOCS_START", pair[0], VAL_CONSTANT, true);
            set_global("__IDLOCS_END",   pair[1], VAL_CONSTANT, true);
          }

          if ((pair = gp_processor_config_exist(found)) != NULL) {
            set_global("__CONFIG_START", pair[0], VAL_CONSTANT, true);
            set_global("__CONFIG_END",   pair[1], VAL_CONSTANT, true);
          }

          if ((pair = gp_processor_eeprom_exist(found)) != NULL) {
            set_global("__EEPROM_START", pair[0], VAL_CONSTANT, true);
            set_global("__EEPROM_END",   pair[1], VAL_CONSTANT, true);
          }

          addr = gp_processor_bsr_boundary(found);

          if (addr > 0) {
            set_global("__ACC_RAM_LOW_START",        0, VAL_CONSTANT, true);
            set_global("__ACC_RAM_LOW_END",   addr - 1, VAL_CONSTANT, true);
          }
        } /* if (!state.mpasm_compatible) */
      } /* if (state.processor == NULL) */
      else if (state.processor != found) {
        gpmsg_vwarning(GPW_REDEFINING_PROC, NULL);
        gpmsg_verror(GPE_EXTRA_PROC, NULL);
      }
    }
    else {
      if (state.pass > 0) {
        gpmsg_verror(GPE_UNKNOWN_PROC, NULL, name);
      }
      else {
        printf("Didn't find any processor named: %s\nHere are the supported processors:\n", name);
        gp_dump_processor_list(true, PROC_CLASS_UNKNOWN, PROC_CLASS_UNKNOWN, PROC_CLASS_UNKNOWN);
        exit(1);
      }
    }

    /* Load the instruction sets if necessary. */
    if ((!state.processor_chosen) && (state.processor != NULL)) {
      opcode_init(1);   /* General directives. */
      /* Separate the directives from the opcodes. */
      state.stBuiltin = gp_sym_push_table(state.stBuiltin, true);
      opcode_init(2);   /* Processor-specific. */

      if ((!IS_PIC16_CORE) && (!IS_PIC16E_CORE)) {
        opcode_init(3);   /* Special pseudo ops for 12 and 14 bit devices. */
      }

      state.processor_chosen = true;
    }
  }
}

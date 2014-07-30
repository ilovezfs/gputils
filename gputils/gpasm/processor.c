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
#include "gperror.h"

void select_processor(const char *name)
{
  pic_processor_t found = NULL;
  int addr0, addr1;

  if (state.cmd_line.processor) {
    gpvwarning(GPW_CMDLINE_PROC, NULL);
  } else {
    found = gp_find_processor(name);

    if (found != NULL) {
      int badrom_idx;

      if (state.processor == NULL) {
        /* If in extended mode: check if processor supports extended instruction set. */
        if (state.extended_pic16e && !(found->pic16e_flags & PIC16E_FLAG_HAVE_EXTINST)) {
          gpverror(GPE_NO_EXTENDED_MODE, NULL);
        }

        state.processor = found;
        state.maxrom = found->maxrom;
        /* Initialize badrom from internal processor info. */
        state.badrom = NULL;

        for (badrom_idx = 0; badrom_idx < MAX_BADROM; badrom_idx += 2) {
          long start, end;

          start = found->badrom[badrom_idx];
          end   = found->badrom[badrom_idx + 1];

          if ((start == -1) || (end == -1)) {
            break;
          }
          else {
            struct range_pair *new_pair = malloc(sizeof(struct range_pair));

            new_pair->start = start;
            new_pair->end = end;
            new_pair->next = state.badrom;
            state.badrom = new_pair;
          }
        }

        set_global(found->defined_as, 1, PERMANENT, GVT_CONSTANT);

        if (!state.mpasm_compatible) {
          addr0 = found->common_ram_addrs[0];
          addr1 = found->common_ram_addrs[1];

          if ((addr0 > 0) && (addr1 > 0)) {
            set_global("__COMMON_RAM_START", addr0, PERMANENT, GVT_CONSTANT);
            set_global("__COMMON_RAM_END",   addr1, PERMANENT, GVT_CONSTANT);
          }

          if ((found->class->vector_table != NULL) || (found->class->vector_number > 0)) {
            const vector_t *vec = found->class->vector_table;
            unsigned int num    = found->class->vector_number;
            char buf[BUFSIZ];

            for (; num; ++vec, --num) {
              buf[0] = '_';
              buf[1] = '_';
              stptoupper(&buf[2], vec->name, sizeof(buf) - 2);

              if (vec->address < 0) {
                /* This a SX type processor. */
                addr0 = found->prog_mem_size;

                if (addr0 > 0) {
                  --addr0;
                }
              }
              else {
                addr0 = vec->address;
              }

              set_global(buf, addr0, PERMANENT, GVT_CONSTANT);
            }
          }

          addr0 = found->prog_mem_size;

          if (addr0 > 0) {
            set_global("__PROGRAM_END", addr0 - 1, PERMANENT, GVT_CONSTANT);
          }

          addr0 = found->eeprom_addrs[0];
          addr1 = found->eeprom_addrs[1];

          if ((addr0 > 0) && (addr1 > 0)) {
            set_global("__EEPROM_START", addr0, PERMANENT, GVT_CONSTANT);
            set_global("__EEPROM_END",   addr1, PERMANENT, GVT_CONSTANT);
          }

          addr0 = gp_processor_bsr_boundary(found);

          if (addr0 > 0) {
            set_global("__ACC_RAM_LOW_END", addr0 - 1, PERMANENT, GVT_CONSTANT);
          }
        } /* if (!state.mpasm_compatible) */
      } /* if (state.processor == NULL) */
      else if (state.processor != found) {
        gpvwarning(GPW_REDEFINING_PROC, NULL);
        gpverror(GPE_EXTRA_PROC, NULL);
      }
    } else {
      if (state.pass) {
        gpverror(GPE_UNKNOWN_PROC, NULL, name);
      } else {
        printf("Didn't find any processor named: %s\nHere are the supported processors:\n", name);
        gp_dump_processor_list(true, NULL, NULL);
        exit(1);
      }
    }

    /* Load the instruction sets if necessary. */
    if ((state.processor_chosen == 0) && (state.processor != NULL)) {
      opcode_init(1);   /* General directives. */
      /* Separate the directives from the opcodes. */
      state.stBuiltin = push_symbol_table(state.stBuiltin, true);
      opcode_init(2);   /* Processor-specific. */

      if ((!IS_PIC16_CORE) && (!IS_PIC16E_CORE)) {
        opcode_init(3);   /* Special pseudo ops for 12 and 14 bit devices. */
      }
      state.processor_chosen = 1;
    }
  }
}

/* Disassembles ".HEX" files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

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
#include "gpdasm.h"

char *processor_name = NULL;

struct gpdasm_state state = {
  NULL,                 /* processor type */
  PROC_CLASS_GENERIC,   /* 12 bit device */
  1                     /* output format */
};

static void
select_processor(void)
{
  const struct px *found = NULL;

  if (processor_name == NULL) {
    printf("error: must select processor\n");
    exit(1);
  }

  found = gp_find_processor(processor_name);

  if (found != NULL) {
    state.processor = found;
  } else {
    printf("Didn't find any processor named: %s\nHere are the supported processors:\n",
            processor_name);
    gp_dump_processor_list(true, 0);
    exit(1);
  }

  state.class = gp_processor_class(state.processor);

  if (state.class->instructions == NULL) {
    fprintf(stderr, "error: unsupported processor class\n");
    exit(1);
  }

  return;
}

static void
writeheader(void)
{
  if (!state.format) {
    printf("\n");
    printf("        processor %s\n", processor_name);
  }
}

static void
write_core_sfr_list(void)
{
  int i;
  const core_sfr_t *table;

  if (!state.show_names || state.format || (state.class->core_sfr_table == NULL) ||
      (state.class->core_sfr_number == 0)) {
    return;
  }

  if (state.class == PROC_CLASS_PIC16E) {
    printf("\nf\tequ\t1\nw\tequ\t0\na\tequ\t0\n\n");
  }
  else {
    printf("\nf\tequ\t1\nw\tequ\t0\n\n");
  }

  table = state.class->core_sfr_table;
  for (i = state.class->core_sfr_number; i > 0; ++table, --i) {
    if (strcmp(table->name, "FSR0L") == 0) {
      printf("FSR0\tequ\t0x%03x\n", table->address);
    }
    else if (strcmp(table->name, "FSR1L") == 0) {
      printf("FSR1\tequ\t0x%03x\n", table->address);
    }
    else if (strcmp(table->name, "FSR2L") == 0) {
      printf("FSR2\tequ\t0x%03x\n", table->address);
    }

    printf("%s\tequ\t0x%03x\n", table->name, table->address);
  }
}

static void
closeasm(void)
{
  if (!state.format) {
    printf("        end\n");
  }
}

static void
writeorg(int org)
{
  if (!state.format) {
    printf("\n");
    printf("        org\t%#x\n", org);
  }
}

static void
dasm(MemBlock *memory)
{
  MemBlock *m = memory;
  int i, maximum;
  int insn_size;
  int last_loc;
  int num_words;
  int bsr_boundary;
  char buffer[80];

  writeheader();
  write_core_sfr_list();

  bsr_boundary = gp_processor_bsr_boundary(state.processor);
  last_loc = 0;
  while (m != NULL) {
    i = m->base << I_MEM_BITS;

    maximum = i + MAX_I_MEM;

    insn_size = 2;
    while (i < maximum) {
      int org = gp_processor_byte_to_org(state.class, i);
      unsigned short data;

      if (gp_processor_is_config_addr(state.processor, org)) {
        /* This is config word/bytes. Not need disassemble. */
        if (state.class->config_size <= 0xFF) {
          unsigned char byte;

          if (b_memory_get(m, i, &byte, NULL, NULL)) {
            if (last_loc != (i - insn_size)) {
              writeorg(org);
            }

            last_loc = i;

            if (state.format) {
              printf("%06x:  %02x  ", org, (unsigned int)byte);
            } else {
              printf("        ");
            }

            printf("db\t0x%02x\n", (unsigned int)byte);
          }
          else {
            last_loc = 0;
          }

          insn_size = 1;
        }
        else {
          if (state.class->i_memory_get(m, i, &data, NULL, NULL)) {
            if (last_loc != (i - insn_size)) {
              writeorg(org);
            }

            last_loc = i;

            if (state.format) {
              printf("%06x:  %04x  ", org, (unsigned int)data);
            } else {
              printf("        ");
            }

            printf("dw\t0x%04x\n", (unsigned int)data);
          }
          else {
            last_loc = 0;
          }

          insn_size = 2;
        }
      }
      else {
        /* This is program word. */
        if (state.class->i_memory_get(memory, i, &data, NULL, NULL)) {
          if (last_loc != (i - insn_size)) {
            writeorg(org);
          }

          last_loc = i;

          if (state.format) {
            printf("%06x:  %04x  ", org, data);
          } else {
            printf("        ");
          }

          num_words = gp_disassemble(memory, i, state.class, bsr_boundary, state.show_names,
                                     buffer, sizeof(buffer));
          printf("%s\n", buffer);

          if (num_words != 1) {
            /* some 18xx instructions use two words */
            if (state.format) {
              state.class->i_memory_get(memory, i + 2, &data, NULL, NULL);
              printf("%06x:  %04x\n", gp_processor_byte_to_org(state.class, i + 2), data);
            }
            insn_size = 4;
          }
          else {
            insn_size = 2;
          }
        }
      }

      i += insn_size;
    }

    m = m->next;
  }

  closeasm();
}

static void
show_usage(void)
{
  printf("Usage: gpdasm [options] file\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -c, --mnemonics                Decode special mnemonics.\n");
  printf("  -h, --help                     Show this usage message.\n");
  printf("  -i, --hex-info                 Information on input hex file.\n");
  printf("  -l, --list-chips               List supported processors.\n");
  printf("  -m, --dump                     Memory dump hex file.\n");
  printf("  -n, --show-names               For some case of SFR, shows the name of\n"
         "                                 instead of the address.\n");
  printf("  -p PROC, --processor PROC      Select processor.\n");
  printf("  -s, --short                    Print short format.\n");
  printf("  -v, --version                  Show version.\n");
  printf("  -y, --extended                 Enable 18xx extended mode.\n");
  printf("      --strict                   Disassemble only opcodes generated by gpasm\n"
         "                                 in case of instructions with several opcodes.\n");
  printf("\n");
  printf("Report bugs to:\n");
  printf("%s\n", PACKAGE_BUGREPORT);
  exit(0);
}

#define GET_OPTIONS "?chilmnp:svy"

  /* Used: himpsv */
  static struct option longopts[] =
  {
    { "mnemonics",   0, 0, 'c' },
    { "help",        0, 0, 'h' },
    { "hex-info",    0, 0, 'i' },
    { "list-chips",  0, 0, 'l' },
    { "dump",        0, 0, 'm' },
    { "show-names",  0, 0, 'n' },
    { "processor",   1, 0, 'p' },
    { "short",       0, 0, 's' },
    { "version",     0, 0, 'v' },
    { "extended",    0, 0, 'y' },
    { "strict",      0, 0, 't' },
    { 0, 0, 0, 0 }
  };

#define GETOPT_FUNC getopt_long(argc, argv, GET_OPTIONS, longopts, 0)

int main(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;

  int c;
  gp_boolean print_hex_info = false;
  gp_boolean memory_dump = false;
  gp_boolean strict = false;
  gp_boolean usage = false;
  char *filename = NULL;

  gp_init();

  state.i_memory = i_memory_create();
  state.show_names = false;

  while ((c = GETOPT_FUNC) != EOF) {
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

    case 'l':
      gp_dump_processor_list(true, 0);
      exit(0);
      break;

    case 'm':
      memory_dump = true;
      break;

    case 'n':
      state.show_names = true;
      break;

    case 'p':
      processor_name = optarg;
      break;

    case 's':
      state.format = 0;
      break;

    case 'y':
      gp_decode_extended = true;
      break;

    case 'v':
      fprintf(stderr, "%s\n", GPDASM_VERSION_STRING);
      exit(0);
      break;

    case 't':
      strict = true;
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

    if (state.hex_info->hex_format == inhx8m) {
      printf("inhx8m\n");
    } else if (state.hex_info->hex_format == inhx16) {
      printf("inhx16\n");
    } else if (state.hex_info->hex_format == inhx32) {
      printf("inhx32\n");
    } else {
      printf("UNKNOWN\n");
    }

    printf("number of bytes: %i\n", state.hex_info->size);
    printf("\n");
  }

  if (state.num.errors == 0) {
    if (memory_dump) {
      print_i_memory(state.i_memory, state.class);
    } else {
      dasm(state.i_memory);
    }
  }

  i_memory_free(state.i_memory);

  return ((state.num.errors > 0) ? EXIT_FAILURE : EXIT_SUCCESS);
}

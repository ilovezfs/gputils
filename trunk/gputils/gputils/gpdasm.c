/* Disassembles ".HEX" files
   Copyright (C) 2001, 2002, 2003, 2004
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
     generic,		   /* processor type */  
     PROC_CLASS_GENERIC,   /* 12 bit device */
     1			   /* output format */
     };

void
select_processor(void)
{
  struct px *found = NULL;

  if (processor_name == NULL) {
    printf("error: must select processor\n");
    exit(1);
  }

  found = gp_find_processor(processor_name);

  if (found) {
    state.processor = found->tag;
  } else {
    printf("Didn't find any processor named: %s\nHere are the supported processors:\n",
            processor_name);
    gp_dump_processor_list(true, 0);
    exit(1);
  }

  state.class = gp_processor_class(state.processor);
   
  switch (state.class) {
  case PROC_CLASS_EEPROM8:
  case PROC_CLASS_GENERIC:
    fprintf(stderr, "error: unsupported processor class\n");
    exit(1);
    break;
  case PROC_CLASS_PIC12:
  case PROC_CLASS_SX:
  case PROC_CLASS_PIC14:
  case PROC_CLASS_PIC16:
  case PROC_CLASS_PIC16E:
    break;
  default:
    assert(0);
  }

  return;
}

void writeheader()
{
  if (!state.format) {
    printf("\n");
    printf("        processor %s\n", processor_name);
  }
}

void closeasm()
{
  if (!state.format) {
    printf("        end\n");
  }
}

void writeorg(int address)
{
  if (!state.format) {
    printf("\n");
    printf("        org\t%#x\n", address);
  }
}

void dasm(MemBlock *memory)
{
  MemBlock *m = memory;
  int i, maximum;
  int lastloc = 0;
  char buffer[80];
  int byte_addr = 0;

  if (state.class == PROC_CLASS_PIC16E) {
    byte_addr = 1;
  }

  writeheader();

  while(m) {
    i = m->base << I_MEM_BITS;

    maximum = i + MAX_I_MEM;
    
    while (i < maximum) {
      if (((i_memory_get(memory, i)) & MEM_USED_MASK) == 0) {
        i++;
      } else {
        if (lastloc != i - 1){
          writeorg(i << byte_addr);
        }
        lastloc = i;
        if (state.format) {
          printf("%06x:  %04x  ",
                 i << byte_addr,
                 (i_memory_get(memory, i) & 0xffff));
        } else {
          printf("        ");
        }
        gp_disassemble(memory, &i, state.class, buffer);
        printf("%s\n", buffer);
        if ((state.format) && (i != lastloc)) {
          /* some 18xx instructions use two words */
          printf("%06x:  %04x\n",
                 i << byte_addr,
                 (i_memory_get(memory, i) & 0xffff));
        }        
        i++;
      } 
    }
    
    m = m->next;
  }

  closeasm();

}

void show_usage(void)
{
  printf("Usage: gpdasm [options] file\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -h, --help                     Show this usage message.\n");
  printf("  -i, --hex-info                 Information on input hex file.\n");
  printf("  -l, --list-chips               List supported processors.\n");
  printf("  -m, --dump                     Memory dump hex file.\n");
  printf("  -p PROC, --processor PROC      Select processor.\n");
  printf("  -s, --short                    Print short format.\n");
  printf("  -v, --version                  Show version.\n");
  printf("\n");
  printf("Report bugs to:\n");
  printf("%s\n", BUG_REPORT_URL);
  exit(0);
}

#define GET_OPTIONS "?hilmp:sv"

  /* Used: himpsv */
  static struct option longopts[] =
  {
    { "help",        0, 0, 'h' },
    { "hex-info",    0, 0, 'i' },
    { "list-chips",  0, 0, 'l' },
    { "dump",        0, 0, 'm' },
    { "processor",   1, 0, 'p' },
    { "short",       0, 0, 's' },
    { "version",     0, 0, 'v' },
    { 0, 0, 0, 0 }
  };

#define GETOPT_FUNC getopt_long(argc, argv, GET_OPTIONS, longopts, 0)

int main(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  int print_hex_info = 0;
  int usage = 0;
  int memory_dump = 0;
  char *filename = 0;

  gp_init();

  state.i_memory = i_memory_create();

  while ((c = GETOPT_FUNC) != EOF) {
    switch (c) {
    case '?':
    case 'h':
      usage = 1;
      break;
    case 'i':
      print_hex_info = 1;
      break;
    case 'l':
      gp_dump_processor_list(true, 0);
      exit(0);
      break;
    case 'm':
      memory_dump = 1;
      break;
    case 'p':
      processor_name = optarg;
      break;
    case 's':
      state.format = 0;
      break;
    case 'v':
      fprintf(stderr, "%s\n", GPDASM_VERSION_STRING);
      exit(0);
    }
    if (usage)
      break;
  }

  if ((optind + 1) == argc) {
    filename = argv[optind];
  } else {
    usage = 1;
  }

  if (usage) {
    show_usage();
  }

  select_processor();

  state.hex_info = readhex(filename, state.i_memory);

  if (state.hex_info->error) {
    state.num.errors++;
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
    if(memory_dump) {
      print_i_memory(state.i_memory, state.class == PROC_CLASS_PIC16E ? 1 : 0);
    } else {
      dasm(state.i_memory);
    }
  }

  i_memory_free(state.i_memory);

  if (state.num.errors > 0)
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;

}

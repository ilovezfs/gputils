/* Disassembles ".HEX" files
   Copyright (C) 2001 Craig Franklin

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
#include "gpdasm.h"
#include "gpdis.c"

struct gpdasm_state state = {
     pic14,		/* processor type */  
     1			/* output format */
     };

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
    printf("        org     0x%04x\n", address);
  }
}

void dasm(MemBlock *memory)
{
  MemBlock *m = memory;
  int i, maximum;
  int lastloc = 0;
  char buffer[80];

  while(m) {
    i = m->base << I_MEM_BITS;

    maximum = i + MAX_I_MEM;
    
    while (i < maximum) {
      if (((i_memory_get(memory, i)) & MEM_USED_MASK) == 0) {
        ++i;
      } else {
	 if (lastloc != i - 1){
	   writeorg(i);
	 }
         if (state.processor == pic12) {
           mem2asm12(i_memory_get(memory, i), buffer);
	 } else if (state.processor == pic14) {
           mem2asm14(i_memory_get(memory, i), buffer);
         }
         if (state.format) {
	   printf("%06x:  %04x  %s\n", i, (i_memory_get(memory, i) & 0xffff), buffer);
	 } else {
	   printf("        %s\n", buffer);	 
         }
	 lastloc = i;
         ++i;
      } 
    }
    
    m = m->next;
  }

  closeasm();

}

void show_usage(void)
{
  printf("Usage: gpdasm <options> <filename>\n");
  printf("Where <options> are:\n");
  #ifdef HAVE_GETOPT_LONG
  printf("  -h, --help                     Show this usage message.\n");
  printf("  -i, --hex-info                 Information on input hex file.\n");
  printf("  -m, --dump                     Memory dump hex file.\n");
  printf("  -p PROC, --processor PROC      Select processor.\n");
  printf("  -s, --short                    Print short format.\n");
  printf("  -v, --version                  Show version.\n");
  #else
  printf("  -h       Show this usage message.\n");
  printf("  -i       Information on input hex file.\n");
  printf("  -m       Memory dump hex file.\n");
  printf("  -p PROC  Select processor.\n");
  printf("  -s       Print short format.\n");
  printf("  -v       Show version.\n");
  #endif
  printf("\n");
  printf("Report bugs to:\n");
  printf("%s\n", BUG_REPORT_URL);
  exit(0);
}

#define GET_OPTIONS "?himp:sv"

#ifdef HAVE_GETOPT_LONG

  /* Used: himpsv */
  static struct option longopts[] =
  {
    { "help",        0, 0, 'h' },
    { "hex-info",    0, 0, 'i' },
    { "dump",        0, 0, 'm' },
    { "processor",   1, 0, 'p' },
    { "short",       0, 0, 's' },
    { "version",     0, 0, 'v' },
    { 0, 0, 0, 0 }
  };

  #define GETOPT_FUNC getopt_long(argc, argv, GET_OPTIONS, longopts, 0)

#else

  #define GETOPT_FUNC getopt(argc, argv, GET_OPTIONS)

#endif

int main(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  int print_hex_info = 0;
  int usage = 0;
  int memory_dump = 0;
  char *filename = 0;

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
    case 'm':
      memory_dump = 1;
      break;
    case 'p':
      if (strcasecmp(optarg, "pic12") == 0)
	state.processor = pic12;
      else if (strcasecmp(optarg, "pic14") == 0)
	state.processor = pic14;
      else {
	fprintf(stderr,
		"Error: unrecognised processor family \"%s\"\n",
		optarg);
        exit(1);
      }
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

  if (optind < argc) {
    filename = argv[optind];
  } else {
    usage = 1;
  }

  if (usage) {
    show_usage();
  }

  state.hex_info = readhex(filename, state.i_memory);

  if (state.hex_info.error) {
    state.num.errors++;
  }

  if (print_hex_info) {
    printf("hex file name:   %s\n", filename);
    printf("hex file format: ");
    if (state.hex_info.hex_format == inhx8m) {
      printf("inhx8m\n");
    } else if (state.hex_info.hex_format == inhx16) {
      printf("inhx16\n");
    } else if (state.hex_info.hex_format == inhx32) {
      printf("inhx32\n");
    } else {
      printf("UNKNOWN\n");
    }
    printf("number of bytes: %i\n", state.hex_info.size);
    printf("\n");
  }
  
  if (state.num.errors == 0) {
    if(memory_dump) {
      print_i_memory(state.i_memory);
    } else {
      dasm(state.i_memory);
    }
  }

  i_memory_free(state.i_memory);

  if (state.num.errors > 0)
    return 1;
  else
    return 0;

}

/* Displays contents of ".COD" files
   Copyright (C) 2001, 2002, 2003, 2004
   Scott Dattalo

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
#include "gpvc.h"
#include "dump.h"
#include "block.h"

FILE *codefile;  
char filename[BUFFER_LENGTH];
char temp[COD_BLOCK_SIZE];
char *source_file_names[MAX_SOURCE_FILES];
FILE *source_files[MAX_SOURCE_FILES];
DirBlockInfo main_dir;

int addrsize;

char directory_block_data[COD_BLOCK_SIZE];
char * SymbolType4[154] = {
  "a_reg          ", "x_reg          ", "c_short        ", "c_long         ",
  "c_ushort       ", "c_ulong        ", "c_pointer      ", "c_upointer     ",
  "table          ", "m_byte         ", "m_boolean      ", "m_index        ",
  "byte_array     ", "u_byte_array   ", "word_array     ", "u_word_array   ",
  "func_void_none ", "func_void_byte ", "funcVoidTwobyte", "func_void_long ",
  "func_void_undef", "func_int_none  ", "func_int_byte  ", "func_int_twobyt",
  "func_int_long  ", "func_int_undef ", "func_long_none ", "func_long_byte ",
  "funcLongTwobyte", "func_long_long ", "func_long_undef", "pfun_void_none ",
  "pfun_void_byte ", "pfunVoidTwobyte", "pfun_void_long ", "pfun_void_undef",
  "pfun_int_none  ", "pfun_int_byte  ", "pfunIntTwobyte ", "pfun_int_long  ",
  "pfun_int_undef ", "pfun_long_none ", "pfun_long_byte ", "pfun_long_twoby",
  "pfun_long_long ", "pfun_long_undef", "address        ", "constant       ",
  "bad_und        ", "br_und         ", "upper_und      ", "byte_und       ",
  "add_und        ", "m_keyword      ", "add_mi_und     ", "vector         ",
  "port_w         ", "port_r         ", "port_rw        ", "port_rmw       ",
  "endif          ", "exundef        ", "macro_t        ", "macro_s        ",
  "macro_a        ", "macro_c        ", "c_keyword      ", "void           ",
  "c_enum         ", "c_typedef1     ", "c_utypedef1    ", "c_typedef2     ",
  "c_utypedef2    ", "cp_typedef1    ", "cp_utypedef1   ", "cp_typedef2    ",
  "cp_utypedef2   ", "c_struct       ", "i_struct       ", "l_const        ",
  "s_short        ", "s_ushort       ", "s_long         ", "s_ulong        ",
  "sa_short       ", "sa_ushort      ", "sa_long        ", "sa_ulong       ",
  "sp_short       ", "sp_ushort      ", "sp_long        ", "sp_ulong       ",
  "FixedPointer   ", "PointerFunction", "cc_reg         ", "PtrF_void_none ",
  "PtrF_void_byte ", "PtrF_void_twobyt", "PtrF_void_long ", "PtrF_void_undef",
  "PtrF_int_none  ", "PtrF_int_byte  ", "PtrF_int_twobyte", "PtrF_int_long  ",
  "PtrF_int_undef ", "PtrF_long_none ", "PtrF_long_byte ",
  "PtrF_long_twobyte", "PtrF_long_long ", "PtrF_long_undef",
  "PfAR_void_none ", "PfAR_void_byte ", "PfAR_void_twobyte",
  "PfAR_void_long ", "PfAR_void_undef", "PfAR_int_none  ", "PfAR_int_byte  ",
  "PfAR_int_twobyte", "PfAR_int_long  ", "PfAR_int_undef ", "PfAR_long_none ",
  "PfAR_long_byte ", "PfAR_long_twobyte", "PfAR_long_long ",
  "PfAR_long_undef", "FWDlit         ", "Pfunc          ", "mgoto          ",
  "mcgoto         ", "mcgoto2        ", "mcgoto3        ", "mcgoto4        ",
  "mcgoto74       ", "mcgoto17       ", "mccall17       ", "mcall          ",
  "mc_call        ", "res_word       ", "local          ", "unknown        ",
  "varlabel       ", "external       ", "global         ", "segment        ",
  "Bankaddr       ", "bit_0          ", "bit_1          ", "bit_2          ",
  "bit_3          ", "bit_4          ", "bit_5          ", "bit_6          ",
  "bit_7          ", "debug          "
};


void show_usage(void)
{
  printf("Usage: gpdasm [options] file\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -a, --all          Display all information in .cod file.\n");
  printf("  -d, --directory    Display directory header.\n");
  printf("  -l, --listing      Display source listing cross.\n");
  printf("  -m, --message      Display debug message area.\n");
  printf("  -h, --help         Show this usage message.\n");
  printf("  -r, --rom          Display rom.\n");
  printf("  -s, --symbols      Display symbols.\n");
  printf("  -v, --version      Show version.\n");
  printf("\n");
  printf("Report bugs to:\n");
  printf("%s\n", BUG_REPORT_URL);
  exit(0);
}

#define GET_OPTIONS "?adhlmrsv"

  /* Used: adhlmrsv */
  static struct option longopts[] =
  {
    { "all",         0, 0, 'a' },
    { "directory",   0, 0, 'd' },
    { "help",        0, 0, 'h' },
    { "listing",     0, 0, 'l' },
    { "message",     0, 0, 'm' },
    { "rom",         0, 0, 'r' },
    { "symbols",     0, 0, 's' },
    { "version",     0, 0, 'v' },
    { 0, 0, 0, 0 }
  };

#define GETOPT_FUNC getopt_long(argc, argv, GET_OPTIONS, longopts, 0)

int main(int argc, char *argv[])
{
  extern int optind;
  unsigned int buffer_size;
  int c,usage=0;
  int display_flags;
  Directory *dir;

  gp_init();

  addrsize = 1;

#define DISPLAY_NOTHING 0
#define DISPLAY_DIR     1
#define DISPLAY_SYM     2
#define DISPLAY_ROM     4
#define DISPLAY_SRC     8
#define DISPLAY_MESS    16
#define DISPLAY_ALL     0xff

  display_flags = DISPLAY_NOTHING;
  while ((c = GETOPT_FUNC) != EOF) {
    switch (c) {
    case '?':
    case 'h':
      usage = 1;
      break;
    case 'a':
      display_flags = DISPLAY_ALL;
      break;
    case 'd':
      display_flags |= DISPLAY_DIR;
      break;
    case 's':
      display_flags |= DISPLAY_SYM;
      break;
    case 'r':
      display_flags |= DISPLAY_ROM;
      break;
    case 'l':
      display_flags |= DISPLAY_SRC;
      break;
    case 'm':
      display_flags |= DISPLAY_MESS;
      break;
    case 'v':
      fprintf(stderr, "%s\n", GPVC_VERSION_STRING);
      exit(0);
    }
    if (usage)
      break;
  }
  
  if ((optind + 1) == argc)
    strcpy(filename, argv[optind]);
  else
    usage = 1;

  if(display_flags == DISPLAY_NOTHING)
    display_flags = DISPLAY_ALL;

  if (usage) {
    show_usage();
  }

  codefile = fopen(filename,"rb");
  if(codefile == NULL) {
    perror(filename);
    exit(1);
  }

  /* Start off by reading the directory block */
  read_directory();

  fseek(codefile, 0,SEEK_SET);
  buffer_size = fread(directory_block_data, 1, COD_BLOCK_SIZE, codefile);

  if(display_flags & DISPLAY_DIR)
    directory_block();

  dir = (Directory *)directory_block_data;

  if(display_flags & DISPLAY_ROM)
    dump_code();

  if(display_flags & DISPLAY_SYM) {
    dump_symbols();
    dump_lsymbols();
    dump_local_vars();
  }

  dump_source_files();

  if(display_flags & DISPLAY_SRC)
    dump_line_symbols();

  if(display_flags & DISPLAY_MESS)
    dump_message_area();

  return EXIT_SUCCESS;
}

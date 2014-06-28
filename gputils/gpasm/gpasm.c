/* top level functions for gpasm
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
#include "gperror.h"
#include "scan.h"
#include "deps.h"
#include "directive.h"
#include "lst.h"
#include "cod.h"
#include "processor.h"
#include "coff.h"

struct gpasm_state state;

static gp_boolean cmd_processor = false;
static char *processor_name = NULL;

int yyparse(void);
extern int yydebug;

#define GET_OPTIONS "?D:I:a:cCde:ghikl::LmMno:p:qr:uvw:yP:"

enum {
  OPT_MPASM_COMPATIBLE = 0x100
};

static struct option longopts[] =
{
  { "define",           required_argument, NULL, 'D' },
  { "include",          required_argument, NULL, 'I' },
  { "hex-format",       required_argument, NULL, 'a' },
  { "object",           no_argument,       NULL, 'c' },
  { "new-coff",         no_argument,       NULL, 'C' },
  { "debug",            no_argument,       NULL, 'd' },
  { "expand",           required_argument, NULL, 'e' },
  { "debug-info",       no_argument,       NULL, 'g' },
  { "help",             no_argument,       NULL, 'h' },
  { "ignore-case",      no_argument,       NULL, 'i' },
  { "error",            no_argument,       NULL, 'k' },
  { "list-chips",       optional_argument, NULL, 'l' },
  { "force-list",       no_argument,       NULL, 'L' },
  { "dump",             no_argument,       NULL, 'm' },
  { "deps",             no_argument,       NULL, 'M' },
  { "dos",              no_argument,       NULL, 'n' },
  { "output",           required_argument, NULL, 'o' },
  { "processor",        required_argument, NULL, 'p' },
  { "quiet",            no_argument,       NULL, 'q' },
  { "radix",            required_argument, NULL, 'r' },
  { "absolute",         no_argument,       NULL, 'u' },
  { "version",          no_argument,       NULL, 'v' },
  { "warning",          required_argument, NULL, 'w' },
  { "extended",         no_argument,       NULL, 'y' },
  { "mpasm-compatible", no_argument,       NULL, OPT_MPASM_COMPATIBLE },
  { "preprocess",       required_argument, NULL, 'P' },
  { NULL,               no_argument,       NULL, '\0' }
};

void
init(void)
{
  gp_init();

  /* restore gpasm to its initialized state */
  state.mode = MODE_ABSOLUTE;
  state.extended_pic16e = false;

  state.radix = 16;
  state.hex_format = INHX32;
  state.case_insensitive = false;
  state.quiet = false;
  state.use_absolute_path = false;
  state.error_level = 0;
  state.debug_info = false;
  state.path_num = 0;
  state.preproc.do_emit = true;

  state.cmd_line.radix = false;
  state.cmd_line.hex_format = false;
  state.cmd_line.error_level = false;
  state.cmd_line.macro_expand = false;
  state.cmd_line.processor = false;
  state.cmd_line.lst_force = false;

  state.pass = 0;
  state.org = 0;
  state.device.id_location = 0;
  state.dos_newlines = false;
  state.memory_dump = false;
  state.found_config = false;
  state.found_devid = false;
  state.found_idlocs = false;
  state.found_end = false;
  state.maxram = (MAX_RAM - 1);

  state.codfile = OUT_NORMAL;
  state.depfile = OUT_SUPPRESS;
  state.errfile = OUT_SUPPRESS;
  state.hexfile = OUT_NORMAL;
  state.lstfile = OUT_NORMAL;
  state.objfile = OUT_SUPPRESS;

  state.num.errors    = 0;
  state.num.warnings  = 0;
  state.num.messages  = 0;
  state.num.warnings_suppressed = 0;
  state.num.messages_suppressed = 0;

  state.processor = NULL;
  state.processor_chosen = 0;

  state.cod.enabled = false;
  state.dep.enabled = false;
  state.err.enabled = false;
  state.lst.enabled = false;
  state.obj.enabled = false;
  state.obj.newcoff = 1;  /* use new Microchip COFF format by default */

  state.obj.object = NULL;
  state.obj.section = NULL;
  state.obj.symbol_num = 0;
  state.obj.section_num = 0;

  state.astack = NULL;

  state.next_state = STATE_NOCHANGE;

  state.while_depth = 0;
}

void
add_path(const char *path)
{
  if (state.path_num < MAX_PATHS) {
    state.paths[state.path_num++] = strdup(path);
  }
  else {
    fprintf(stderr, "too many -I paths\n");
    exit(1);
  }
}

static void
show_usage(void)
{
  printf("Usage: gpasm [options] file\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -a FMT, --hex-format FMT       Select hex file format. [inhx32]\n");
  printf("  -c, --object                   Output relocatable object.\n");
  printf("  -C, --old-coff                 Use old Microchip COFF format.\n");
  printf("  -d, --debug                    Output debug messages.\n");
  printf("  -D SYM=VAL, --define SYM=VAL   Define SYM with value VAL.\n");
  printf("  -e [ON|OFF], --expand [ON|OFF] Macro expansion.\n");
  printf("  -g, --debug-info               Use debug directives for COFF.\n");
  printf("  -h, --help                     Show this usage message.\n");
  printf("  -i, --ignore-case              Case insensitive.\n");
  printf("  -I DIR, --include DIR          Specify include directory.\n");
  printf("  -k, --error                    Enables creation of the error file.\n");
  printf("  -l[12[ce]|14[ce]|16[ce]], --list-chips[=([12[ce]|14[ce]|16[ce]])]\n");
  printf("                                 List supported processors based on various aspects.\n");
  printf("  -L, --force-list               Ignore nolist directives.\n");
  printf("  -m, --dump                     Memory dump.\n");
  printf("      --mpasm-compatible         MPASM compatibility mode.\n");
  printf("  -M, --deps                     Output dependency file.\n");
#ifndef HAVE_DOS_BASED_FILE_SYSTEM
  printf("  -n, --dos                      Use DOS newlines in hex file.\n");
#endif
  printf("  -o FILE, --output FILE         Alternate name of output files. Option effect of:\n");
  printf("                                 -- If the \"-c\" option included in the command line:\n");
  printf("                                      FILE.o, FILE.lst, FILE.err\n");
  printf("                                        (The \"FILE.o\" should specified.)\n");
  printf("                                 -- If the \"-c\" option not included in the command line:\n");
  printf("                                      FILE.hex, FILE.cod, FILE.lst, FILE.err\n");
  printf("                                        (The \"FILE.hex\" should specified.)\n");
  printf("  -p PROC, --processor PROC      Select processor.\n");
  printf("  -P FILE, --preprocess FILE     Write preprocessed asm file to FILE.\n");
  printf("  -q, --quiet                    Quiet.\n");
  printf("  -r RADIX, --radix RADIX        Select radix. [hex]\n");
  printf("  -u, --absolute                 Use absolute pathes. \n");
  printf("  -v, --version                  Show version.\n");
  printf("  -w [0|1|2], --warning [0|1|2]  Set message level. [0]\n");
  printf("  -y, --extended                 Enable 18xx extended mode.\n");
  printf("\n");
#ifdef USE_DEFAULT_PATHS
  if (gp_header_path) {
    printf("Default header file path %s\n", gp_header_path);
  }
  else {
    printf("Default header file path NOT SET.\n");
  }
  printf("\n");
#endif
  printf("Report bugs to:\n");
  printf("%s\n", PACKAGE_BUGREPORT);
}

void
process_args( int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  gp_boolean usage = false;
  int usage_code = 0;
  char *pc;

  /* Scan through the options for the -i flag. It must be set before the
     defines are read. */
  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, 0)) != EOF) {
    switch (c) {
    case 'i':
      state.case_insensitive = true;
      break;
    }
  }

  /* reset the getopt_long index for the next call */
  optind = 1;

  /* initalize the defines table for command line arguments */
  state.stDefines = push_symbol_table(NULL, state.case_insensitive);

  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, 0)) != EOF) {
    switch (c) {
    case '?':
      usage_code = 1;

    case 'h':
      usage = true;
      break;

    case 'a':
      select_hexformat(optarg);
      state.cmd_line.hex_format = true;
      break;

    case 'c':
      state.mode    = MODE_RELOCATABLE;
      state.codfile = OUT_SUPPRESS;
      state.hexfile = OUT_SUPPRESS;
      state.lstfile = OUT_NORMAL;
      state.objfile = OUT_NORMAL;
      break;

    case 'C':
      state.obj.newcoff = 0;
      break;

    case 'd':
      gp_debug_disable = false;
      break;

    case 'D':
      if ((optarg != NULL) && (strlen(optarg) > 0)) {
        struct symbol *sym;
        char *lhs, *rhs;

        lhs = strdup(optarg);
        rhs = strchr(lhs, '=');

        if (rhs != NULL) {
          *rhs = '\0';  /* Terminate the left-hand side */
          rhs++;        /* right-hand side begins after the '=' */
        }

        sym = get_symbol(state.stDefines, lhs);

        if (sym == NULL) {
          sym = add_symbol(state.stDefines, lhs);
        }

        if (rhs != NULL) {
          annotate_symbol(sym, mk_list(mk_string(rhs), NULL));
        }
      }
      break;

    case 'e':
      select_expand(optarg);
      state.cmd_line.macro_expand = true;
      break;

    case 'g':
      state.debug_info = true;
      break;

    case 'I':
      add_path(optarg);
      break;

    case 'i':
      state.case_insensitive = true;
      break;

    case 'k':
      state.errfile = OUT_NORMAL;
      break;

    case 'L':
      state.cmd_line.lst_force = true;
      break;

    case 'l': {
      long pic_family = -1;

      if (optarg != NULL) {
        pic_family = strtol(optarg, NULL, 16);
      }

      switch (pic_family) {
        case 0x12C:
          gp_dump_processor_list(false, PROC_CLASS_PIC12);
          break;

        case 0x12:
          gp_dump_processor_list(false, PROC_CLASS_PIC12);
        case 0x12E:
          gp_dump_processor_list(false, PROC_CLASS_PIC12E);
          break;

        case 0x14C:
          gp_dump_processor_list(false, PROC_CLASS_PIC14);
          break;

        case 0x14:
          gp_dump_processor_list(false, PROC_CLASS_PIC14);
        case 0x14E:
          gp_dump_processor_list(false, PROC_CLASS_PIC14E);
          break;

        case 0x16C:
          gp_dump_processor_list(false, PROC_CLASS_PIC16);
          break;

        case 0x16:
          gp_dump_processor_list(false, PROC_CLASS_PIC16);
        case 0x16E:
          gp_dump_processor_list(false, PROC_CLASS_PIC16E);
          break;

        default: {
          gp_dump_processor_list(true, NULL);
          }
        }

      exit(0);
      break;
      }

    case 'M':
      state.depfile = OUT_NORMAL;
      break;

    case 'm':
      state.memory_dump = true;
      break;

    case 'n':
      #ifndef HAVE_DOS_BASED_FILE_SYSTEM
        state.dos_newlines = true;
      #endif
      break;

    case 'o':
      strncpy(state.objfilename, optarg, sizeof(state.objfilename));
      strncpy(state.basefilename, optarg, sizeof(state.basefilename));
      pc = strrchr(state.basefilename, '.');

      if (pc != NULL) {
        *pc = '\0';
      }
      break;

    case 'p':
      cmd_processor = true;
      processor_name = optarg;
      break;

    case 'q':
      state.quiet = true;
      break;

    case 'r':
      select_radix(optarg);
      state.cmd_line.radix = true;
      break;

    case 'u':
      state.use_absolute_path = true;
      break;

    case 'w':
      select_errorlevel(atoi(optarg));
      state.cmd_line.error_level = true;
      break;

    case 'y':
      state.extended_pic16e = true;
      break;

    case 'v':
      fprintf(stderr, "%s\n", GPASM_VERSION_STRING);
      exit(0);

    case OPT_MPASM_COMPATIBLE:
      state.mpasm_compatible = true;
      break;

    case 'P':
      state.preproc.preprocfilename = optarg;
      break;
    }

    if (usage) {
      break;
    }
  }

  if ((optind + 1) == argc) {
    state.srcfilename = argv[optind];
  }
  else {
    usage = true;
  }

  if (usage) {
    show_usage();
    exit(usage_code);
  }

  /* Add the header path to the include paths list last, so that the user
     specified directories are searched first. */
  if (gp_header_path) {
    add_path(gp_header_path);
  }

  if (state.use_absolute_path) {
    state.srcfilename = gp_absolute_path(state.srcfilename);
  }
}

int
assemble(void)
{
  char *pc;
  struct symbol_table *cmd_defines;

  /* store the command line defines to restore on second pass */
  cmd_defines = state.stDefines;
  state.c_memory = state.i_memory = i_memory_create();

  if (state.basefilename[0] == '\0') {
    strncpy(state.basefilename, state.srcfilename, sizeof(state.basefilename));
    pc = strrchr(state.basefilename, '.');

    if (pc != NULL) {
      *pc = '\0';
    }
  }

  /* Builtins are always case insensitive. */
  state.stBuiltin = push_symbol_table(NULL, true);
  state.stDirective = state.stBuiltin;
  state.stMacros = push_symbol_table(NULL, state.case_insensitive);
  state.stTop =
    state.stGlobal = push_symbol_table(NULL, state.case_insensitive);
  state.stDefines = push_symbol_table(cmd_defines, state.case_insensitive);
  state.stMacroParams = push_symbol_table(NULL, state.case_insensitive);
  opcode_init(0);

  /* the tables are built, select the processor if -p was used */
  if (cmd_processor) {
    select_processor(processor_name);
    state.cmd_line.processor = true;
  }

  state.pass = 1;
  open_src(state.srcfilename, 0);
  yyparse();
  yylex_destroy();

  state.pass++;
  state.org = 0;
  state.device.id_location = 0;
  state.cblock = 0;
  state.cblock_defined = 0;
  state.preproc.do_emit = true;
  /* clean out defines for second pass */
  state.stMacros = push_symbol_table(NULL, state.case_insensitive);
  state.stDefines = push_symbol_table(cmd_defines, state.case_insensitive);
  state.stMacroParams = push_symbol_table(NULL, state.case_insensitive);
  purge_temp_symbols(state.stTop);

  if (!state.cmd_line.radix) {
    state.radix = 16;
  }

  state.obj.symbol_num = 0;
  state.obj.section_num = 0;
  /* finish the MACRO and WHILE definition */
  state.mac_prev = NULL;
  state.mac_head = state.while_head = NULL;
  state.while_depth = 0;

  state.astack = NULL;

  /* Initial section. */
  state.obj.new_sec_flags = (state.mode == MODE_ABSOLUTE) ? STYP_TEXT : 0;

  state.found_config = false;
  state.found_devid = false;
  state.found_idlocs = false;
  state.found_end = false;
  coff_init();
  cod_init();
  gperror_init();
  deps_init();
  lst_init();
  preproc_init();

  /* reset the processor for 2nd pass */
  state.processor = NULL;
  state.processor_chosen = 0;
  state.cmd_line.processor = false;

  if (cmd_processor) {
    select_processor(processor_name);
    state.cmd_line.processor = true;
  }

  open_src(state.srcfilename, 0);
  if (!gp_debug_disable) {
    yydebug = 1;
  }
  else {
    yydebug = 0;
  }
  yyparse();

  assert(state.pass == 2);

  if (state.obj.object) {
    /* Set F_ABSOLUTE COFF flag if absolute mode. */
    if (state.mode == MODE_ABSOLUTE) {
      state.obj.object->flags |= F_ABSOLUTE;
    }

    /* Set F_EXTENDED18 COFF flag if 18xx extended mode enabled. */
    if (state.extended_pic16e) {
      state.obj.object->flags |= F_EXTENDED18;
    }
  }

  pop_symbol_table(state.stBuiltin);

  hex_init();

  if (state.memory_dump) {
    print_i_memory(state.i_memory, state.device.class);
  }

  /* Maybe produce a symbol table. */
  if (state.lst.symboltable) {
    lst_throw(); /* Start symbol table on a fresh page. */
    lst_symbol_table();
  }

  /* Maybe produce a memory map. */
  if ((state.mode == MODE_ABSOLUTE) && (state.lst.memorymap)) {
    lst_memory_map(state.i_memory);
  }

  /* Finish off the object, dependency, listing, and symbol files. */
  coff_close_file();
  gperror_close();
  deps_close();
  lst_close();

  if (state.processor) {
    cod_close_file();
  }

  free_files();

  if ((state.num.errors > 0) || (gp_num_errors > 0)) {
    return EXIT_FAILURE;
  }
  else {
    return EXIT_SUCCESS;
  }
}

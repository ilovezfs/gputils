/* GNU PIC object symbol strip
   Copyright (C) 2005
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
#include "gpstrip.h"

struct gpstrip_state state;
gp_boolean verbose;

static void
_conditional_remove(gp_symbol_type *symbol)
{
  const symbol_t *sym;

  sym = sym_get_symbol(state.symbol_keep, symbol->name);
  if (sym == NULL) {
    if (verbose) {
      gp_message("removing symbol \"%s\"", symbol->name);
    }
    gp_coffgen_del_symbol(state.object, symbol);
  }
}

static void
_remove_sections(void)
{
  size_t           i;
  const symbol_t  *sym;
  gp_section_type *section;

  /* FIXME: Check for relocations from other sections. Error out if they exist. */

  for (i = 0; i < sym_get_symbol_count(state.section_remove); ++i) {
    sym     = sym_get_symbol_with_index(state.section_remove, i);
    section = gp_coffgen_find_section(state.object, state.object->sections, sym_get_symbol_name(sym));
    if (section != NULL) {
      if (verbose) {
        gp_message("removing section \"%s\"", sym_get_symbol_name(sym));
      }

      /* remove the sections symbols */
      gp_coffgen_del_section_symbols(state.object, section);

      /* remove the section */
      gp_coffgen_del_section(state.object, section);
    }
  }
}

static void
_remove_symbols(void)
{
  size_t          i;
  const symbol_t *sym;
  gp_symbol_type *symbol;

  for (i = 0; i < sym_get_symbol_count(state.symbol_remove); ++i) {
    sym    = sym_get_symbol_with_index(state.symbol_remove, i);
    symbol = gp_coffgen_find_symbol(state.object, sym_get_symbol_name(sym));
    if (symbol != NULL) {
      if (!gp_coffgen_has_reloc(state.object, symbol)) {
        _conditional_remove(symbol);
      }
    }
  }
}

static void
_strip_all(void)
{
  gp_section_type *section;

  if (state.object->flags & F_EXEC) {
    section = state.object->sections;
    while (section != NULL) {
      /* remove the line numbers, have too because the symbols will be removed */
      section->num_lineno        = 0;
      section->line_numbers      = NULL;
      section->line_numbers_tail = NULL;

      /* remove the relocations, they should already be removed */
      section->num_reloc        = 0;
      section->relocations      = NULL;
      section->relocations_tail = NULL;
    
      section = section->next;
    }  

    /* remove all symbols */
    state.object->num_symbols  = 0;
    state.object->symbols      = NULL;
    state.object->symbols_tail = NULL;
  } else {
    gp_error("can not strip all symbols because the object file is not executable");
  }
}

static void
_strip_debug(void)
{
  gp_section_type *section;
  gp_symbol_type  *list;
  gp_symbol_type  *symbol;

  section = state.object->sections;
  while (section != NULL) {
    /* remove the line numbers */
    section->num_lineno        = 0;
    section->line_numbers      = NULL;
    section->line_numbers_tail = NULL;
    
    section = section->next;
  }

  list = state.object->symbols;
  while (list != NULL) {
    /* remove any debug symbols */
    symbol = list;
    list   = list->next;
    if (symbol->section_number == N_DEBUG) {
      _conditional_remove(symbol);
    }
  }
}

static void
_strip_unneeded(void)
{
  gp_symbol_type *list;
  gp_symbol_type *symbol;

  list = state.object->symbols;
  while (list != NULL) {
    symbol = list;
    list   = list->next;
    
    /* if the symbol has a relocation or is global it can't be removed */
    if (!gp_coffgen_has_reloc(state.object, symbol) && !gp_coffgen_is_global(symbol)) {
      _conditional_remove(symbol);
    }
  }
}

static void
_discard_all(void)
{
  gp_symbol_type *list;
  gp_symbol_type *symbol;

  list = state.object->symbols;
  while (list != NULL) {
    symbol = list;
    list   = list->next;
    
    if (!gp_coffgen_is_global(symbol)) {
      _conditional_remove(symbol);
    }
  }
}

static void 
_add_name(symbol_table_t *table, char *name)
{
  const symbol_t *sym;

  sym = sym_get_symbol(table, name);
  if (sym == NULL) {
    sym_add_symbol(table, name);
  }
}

static void
_show_usage(void)
{
  printf("Usage: gpstrip [options] file(s)\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -g, --strip-debug                     Strip debug symbols.\n");
  printf("  -h, --help                            Show this usage message.\n");
  printf("  -k SYMBOL, --keep-symbol SYMBOL       Keep symbol.\n");
  printf("  -n SYMBOL, --strip-symbol SYMBOL      Remove symbol.\n");
  printf("  -o FILE, --output FILE                Alternate name of output file.\n");
  printf("  -p, --preserve-dates                  Preserve dates.\n");
  printf("  -r SECTION, --remove-section SECTION  Remove section.\n");
  printf("      --strict-options                  If this is set, then an option may not be parameter\n"
         "                                        of an another option. For example: -o --version\n");
  printf("  -s, --strip-all                       Remove all symbols.\n");
  printf("  -u, --strip-unneeded                  Strip symbols not need for relocations.\n");
  printf("  -v, --version                         Show version.\n");
  printf("  -V, --verbose                         Verbose mode.\n");
  printf("  -x, --discard-all                     Remove non-global symbols.\n\n");
  printf("Report bugs to:\n");
  printf("%s\n", PACKAGE_BUGREPORT);
  exit(0);
}

enum {
  OPT_STRICT_OPTIONS = 0x100
};

#define GET_OPTIONS "ghk:n:o:pr:suvVx"

static struct option longopts[] =
{
  { "strip-debug",    no_argument,       NULL, 'g' },
  { "help",           no_argument,       NULL, 'h' },
  { "keep-symbol",    required_argument, NULL, 'k' },
  { "strip-symbol",   required_argument, NULL, 'n' },
  { "output",         required_argument, NULL, 'o' },
  { "preserve-dates", no_argument,       NULL, 'p' },
  { "remove-section", required_argument, NULL, 'r' },
  { "strict-options", no_argument,       NULL, OPT_STRICT_OPTIONS },
  { "strip-all",      no_argument,       NULL, 's' },
  { "strip-unneeded", no_argument,       NULL, 'u' },
  { "version",        no_argument,       NULL, 'v' },
  { "verbose",        no_argument,       NULL, 'V' },
  { "discard-all",    no_argument,       NULL, 'x' },
  { NULL,             no_argument,       NULL, '\0'}
};

int main(int argc, char *argv[])
{
  int         option_index;
  const char *command;
  int         c;
  gp_boolean  strict_options = false;
  gp_boolean  usage          = false;

  gp_init();

  /* initalize */
  verbose = false;
  state.strip_debug    = false;
  state.preserve_dates = false;
  state.strip_all      = false;
  state.strip_unneeded = false;
  state.discard_all    = false;
  state.output_file    = NULL;
  state.symbol_keep    = sym_push_table(NULL, false);
  state.symbol_remove  = sym_push_table(NULL, false);
  state.section_remove = sym_push_table(NULL, false);

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

    case 'g':
      state.strip_debug = true;
      break;

    case 'k':
      _add_name(state.symbol_keep, optarg);
      break;

    case 'n':
      _add_name(state.symbol_remove, optarg);
      break;

    case 'o':
      state.output_file = optarg;
      break;

    case 'p':
      state.preserve_dates = true;
      break;

    case 'r':
      _add_name(state.section_remove, optarg);
      break;

    case 's':
      state.strip_all = true;
      break;

    case 'u':
      state.strip_unneeded = true;
      break;

    case 'x':
      state.discard_all = true;
      break;

    case 'V':
      verbose = true;
      break;

    case 'v':
      fprintf(stderr, "%s\n", GPSTRIP_VERSION_STRING);
      exit(0);

    case OPT_STRICT_OPTIONS:
      /* do nothing */
      break;
    }

    if (usage)
      break;
  }

  if ((optind == argc) || (usage)) {
    _show_usage();
  }

  for ( ; optind < argc; optind++) {
    state.input_file = argv[optind];

    if ((gp_identify_coff_file(state.input_file) != GP_COFF_OBJECT_V2) && 
        (gp_identify_coff_file(state.input_file) != GP_COFF_OBJECT)) {
      gp_error("\"%s\" is not a valid object file", state.input_file);
      exit(1);
    }

    state.object = gp_read_coff(state.input_file);

    if (state.object != NULL) {
      _remove_sections();
      _remove_symbols();
    
      if (state.strip_all) {
        _strip_all();
      }

      if (state.strip_debug) {
        if (state.strip_all) {
          gp_message("strip debug ignored");
        } else {
          _strip_debug();
        }
      }

      if (state.strip_unneeded) {
        if (state.strip_all) {
          gp_message("strip unneeded ignored");
        } else {
          _strip_unneeded();
        }
      }

      if (state.discard_all) {
        if (state.strip_all) {
          gp_message("discard all ignored");
        } else {
          _discard_all();
        }
      }

      if (state.output_file != NULL) {
        state.object->filename = state.output_file;
      }

      if (!state.preserve_dates) {
        /* FIXME: need to update the output file dates */
        state.object->time = (long)time(NULL);
      }
    
      if (gp_num_errors == 0) {
        /* no errors have occured so write the file */
        if (!gp_write_coff(state.object, 0)) {
          gp_error("system error while writing object file");
        }
      } else if (state.output_file) {
        /* a new file is being written, but errors have occurred, delete
           the file if it exists */
        unlink(state.output_file);
      }

      /* FIXME: free state.output_file */
    }
  }

  return ((gp_num_errors != 0) ? EXIT_FAILURE : EXIT_SUCCESS);
}

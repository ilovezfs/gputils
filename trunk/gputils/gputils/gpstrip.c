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

void
conditional_remove(gp_symbol_type *symbol)
{
  struct symbol *sym;

  sym = get_symbol(state.symbol_keep, symbol->name);
  if (sym == NULL) {
    if (verbose) {
      gp_message("removing symbol \"%s\"", symbol->name);
    }
    gp_coffgen_delsymbol(state.object, symbol);
  }

}

void
remove_sections(void)
{
  int i;
  struct symbol *sym;
  gp_section_type *section;
  gp_symbol_type *symbol = NULL;
  gp_symbol_type *list = NULL;

  /* FIXME:  Check for relocations from other sections.  Error out if
     they exist */

  for (i = 0; i < HASH_SIZE; i++) {
    for (sym = state.section_remove->hash_table[i]; sym; sym = sym->next) {
      section = gp_coffgen_findsection(state.object, 
                                       state.object->sections,
                                       sym->name);
      if (section) {
        if (verbose) {
          gp_message("removing section \"%s\"", sym->name);
        }

        /* remove all symbols for the section */
        list = state.object->symbols;
        while (list != NULL) {
          /* advance the pointer so the symbol can be freed */
          symbol = list;
          list = list->next;
          if (symbol->section == section) {
            gp_coffgen_delsymbol(state.object, symbol);
          }
        }

        /* remove the section */
        gp_coffgen_delsection(state.object, section);
      }
    }
  }

}

void
remove_symbols(void)
{
  int i;
  struct symbol *sym;
  gp_symbol_type *symbol = NULL;

  for (i = 0; i < HASH_SIZE; i++) {
    for (sym = state.symbol_remove->hash_table[i]; sym; sym = sym->next) {
      symbol = gp_coffgen_findsymbol(state.object, sym->name);
      if (symbol) {
        if (!gp_coffgen_has_reloc(state.object, symbol)) {
          conditional_remove(symbol);
        }
      }
    }
  }

}

void
strip_all(void)
{
  gp_section_type *section = state.object->sections;

  if (state.object->flags & F_EXEC) {
    while (section != NULL) {
      /* remove the line numbers, have too because the symbols will be
         removed */
      section->num_lineno = 0;
      section->line_numbers = NULL;
      section->line_numbers_tail = NULL;

      /* remove the relocations, they should already be removed */
      section->num_reloc = 0;
      section->relocations = NULL;
      section->relocations_tail = NULL;
    
      section = section->next;
    }  

    /* remove all symbols */
    state.object->num_symbols = 0;
    state.object->symbols = NULL;
    state.object->symbols_tail = NULL;

  } else {
    gp_error("can not strip all symbols because the object file is not executable");
  }

}

void
strip_debug(void)
{
  gp_section_type *section = state.object->sections;
  gp_symbol_type *list = NULL;
  gp_symbol_type *symbol = NULL;

  while (section != NULL) {
    /* remove the line numbers */
    section->num_lineno = 0;
    section->line_numbers = NULL;
    section->line_numbers_tail = NULL;
    
    section = section->next;
  }

  list = state.object->symbols;
  while (list != NULL) {
    /* remove any debug symbols */
    symbol = list;
    list = list->next;
    if (symbol->section_number == N_DEBUG) {
      conditional_remove(symbol);
    }
  }

}

void
strip_unneeded(void)
{
  gp_symbol_type *list = NULL;
  gp_symbol_type *symbol = NULL;

  list = state.object->symbols;
  while (list != NULL) {
    symbol = list;
    list = list->next;
    
    /* if the symbol has a relocation or is global it can't be removed */
    if (!gp_coffgen_has_reloc(state.object, symbol) && 
        !gp_coffgen_is_global(symbol)) {
      conditional_remove(symbol);
    }
  }

}

void
discard_all(void)
{
  gp_symbol_type *list = NULL;
  gp_symbol_type *symbol = NULL;

  list = state.object->symbols;
  while (list != NULL) {
    symbol = list;
    list = list->next;
    
    if (!gp_coffgen_is_global(symbol)) {
      conditional_remove(symbol);
    }
  }
}

void 
add_name(struct symbol_table *table, char *name)
{
  struct symbol *sym;

  sym = get_symbol(table, name);
  if (sym == NULL)
    sym = add_symbol(table, name);

}

void show_usage(void)
{
  printf("Usage: gpstrip [options] file\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -g, --strip-debug                     Strip debug symbols.\n");
  printf("  -h, --help                            Show this usage message.\n");
  printf("  -k SYMBOL, --keep-symbol SYMBOL       Keep symbol.\n");
  printf("  -n SYMBOL, --strip-symbol SYMBOL      Remove symbol.\n");
  printf("  -o FILE, --output FILE                Alternate name of output file.\n");
  printf("  -p, --preserve-dates                  Preserve dates.\n");
  printf("  -r SECTION, --remove-section SECTION  Remove section.\n");
  printf("  -s, --strip-all                       Remove all symbols.\n");
  printf("  -u, --strip-unneeded                  Strip symbols not need for relocations.\n");
  printf("  -v, --version                         Show version.\n");
  printf("  -V, --verbose                         Verbose mode.\n");
  printf("  -x, --discard-all                     Remove non-global symbols.\n");
  printf("\n");
  printf("Report bugs to:\n");
  printf("%s\n", PACKAGE_BUGREPORT);
  exit(0);
}

#define GET_OPTIONS "?ghk:n:o:pr:suvVx"

  static struct option longopts[] =
  {
    { "strip-debug",    0, 0, 'g' },
    { "help",           0, 0, 'h' },
    { "keep-symbol",    1, 0, 'k' },
    { "strip-symbol",   1, 0, 'n' },
    { "output",         1, 0, 'o' },
    { "preserve-dates", 0, 0, 'p' },
    { "remove-section", 1, 0, 'r' },
    { "strip-all",      0, 0, 's' },
    { "strip-unneeded", 0, 0, 'u' },
    { "version",        0, 0, 'v' },
    { "verbose",        0, 0, 'V' },
    { "discard-all",    0, 0, 'x' },
    { 0, 0, 0, 0 }
  };

#define GETOPT_FUNC getopt_long(argc, argv, GET_OPTIONS, longopts, 0)

int main(int argc, char *argv[])
{
  extern int optind;
  int c;
  int usage = 0;

  gp_init();

  /* initalize */
  verbose = false;
  state.strip_debug = false;
  state.preserve_dates = false;
  state.strip_all = false;
  state.strip_unneeded = false;
  state.discard_all = false;
  state.output_file = NULL;
  state.symbol_keep = push_symbol_table(NULL, false);
  state.symbol_remove = push_symbol_table(NULL, false);
  state.section_remove = push_symbol_table(NULL, false);

  while ((c = GETOPT_FUNC) != EOF) {
    switch (c) {
    case '?':
    case 'h':
      usage = 1;
      break;
    case 'g':
      state.strip_debug = true;
      break;
    case 'k':
      add_name(state.symbol_keep, optarg);
      break;
    case 'n':
      add_name(state.symbol_remove, optarg);
      break;
    case 'o':
      state.output_file = optarg;
      break;
    case 'p':
      state.preserve_dates = true;
      break;
    case 'r':
      add_name(state.section_remove, optarg);
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
    }
    if (usage)
      break;
  }

  if ((optind + 1) == argc) {
    state.input_file = argv[optind];
  } else {
    usage = 1;
  }

  if (usage) {
    show_usage();
  }

  if (gp_identify_coff_file(state.input_file) != object_file) {
    gp_error("\"%s\" is not a valid object file", state.input_file);
    exit(1);
  }

  state.object = gp_read_coff(state.input_file);

  if (state.object) {
    remove_sections();
    remove_symbols();
    
    if (state.strip_all) {
      strip_all();
    }

    if (state.strip_debug) {
      if (state.strip_all) {
        gp_message("strip debug ignored");
      } else {
        strip_debug();
      }
    }

    if (state.strip_unneeded) {
      if (state.strip_all) {
        gp_message("strip unneeded ignored");
      } else {
        strip_unneeded();
      }
    }

    if (state.discard_all) {
      if (state.strip_all) {
        gp_message("discard all ignored");
      } else {
        discard_all();
      }
    }

    if (state.output_file) {
      state.object->filename = state.output_file;
    }

    if (!state.preserve_dates) {
      /* FIXME: need to update the output file dates */
      state.object->time = (long)time(NULL);
    }
    
    if (gp_num_errors == 0) {
      /* no errors have occured so write the file */
      if (gp_write_coff(state.object, 0))
        gp_error("system error while writing object file");
    } else if (state.output_file) {
      /* a new file is being written, but errors have occurred, delete
         the file if it exists */
      unlink(state.output_file);
    }

  }

  if (gp_num_errors)
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;
}

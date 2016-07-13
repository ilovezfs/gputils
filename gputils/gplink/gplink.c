/* GNU PIC Linker
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2015-2016 Molnar Karoly <molnarkaroly@users.sf.net>

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
#include "gplink.h"
#include "cod.h"
#include "scan.h"
#include "lst.h"
#include "map.h"
#include "script.h"

extern int yyparse(void);
extern int yydebug;

#define OPTIMIZE_LEVEL_DEFAULT          1

struct gplink_state state;
static gp_boolean   processor_mismatch_warning;
static gp_boolean   enable_cinit_wanings;

#define GET_OPTIONS "a:b:cCdf:hI:lmo:O:p:qrs:t:u:vw"

enum {
  OPT_MPLINK_COMPATIBLE = 0x100,
  OPT_STRICT_OPTIONS
};

static struct option longopts[] =
{
  { "hex-format",         required_argument, NULL, 'a' },
  { "optimize-banksel",   required_argument, NULL, 'b' },
  { "object",             no_argument,       NULL, 'c' },
  { "no-cinit-warnings",  no_argument,       NULL, 'C' },
  { "debug",              no_argument,       NULL, 'd' },
  { "fill",               required_argument, NULL, 'f' },
  { "help",               no_argument,       NULL, 'h' },
  { "include",            required_argument, NULL, 'I' },
  { "no-list",            no_argument,       NULL, 'l' },
  { "map",                no_argument,       NULL, 'm' },
  { "output",             required_argument, NULL, 'o' },
  { "optimize",           required_argument, NULL, 'O' },
  { "optimize-pagesel",   required_argument, NULL, 'p' },
  { "quiet",              no_argument,       NULL, 'q' },
  { "use-shared",         no_argument,       NULL, 'r' },
  { "script",             required_argument, NULL, 's' },
  { "stack",              required_argument, NULL, 't' },
  { "strict-options",     no_argument,       NULL, OPT_STRICT_OPTIONS },
  { "macro",              required_argument, NULL, 'u' },
  { "version",            no_argument,       NULL, 'v' },
  { "processor-mismatch", no_argument,       NULL, 'w' },
  { "mplink-compatible",  no_argument,       NULL, OPT_MPLINK_COMPATIBLE },
  { NULL,                 no_argument,       NULL, '\0'}
};

/*------------------------------------------------------------------------------------------------*/

static void
_show_usage(void)
{
  printf("Usage: gplink [options] [objects] [libraries]\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -a FMT, --hex-format FMT       Select hex file format.\n");
  printf("  -b OPT, --optimize-banksel OPT Remove unnecessary Banksel directives. [0]\n");
  printf("  -c, --object                   Output executable object file.\n");
  printf("  -C, --no-cinit-warnings        Disable this warnings of _cinit section with -O2 option:\n"
         "                                   \"Relocation symbol _cinit has no section.\"\n");
  printf("  -d, --debug                    Output debug messages.\n");
  printf("  -f VALUE, --fill VALUE         Fill unused program memory with value.\n");
  printf("  -h, --help                     Show this usage message.\n");
  printf("  -I DIR, --include DIR          Specify include directory.\n");
  printf("  -l, --no-list                  Disable list file output.\n");
  printf("  -m, --map                      Output a map file.\n");
  printf("      --mplink-compatible        MPLINK compatibility mode.\n");
  printf("  -o FILE, --output FILE         Alternate name of output file.\n");
  printf("  -O OPT, --optimize OPT         Optimization level. [1]\n");
  printf("  -p OPT, --optimize-pagesel OPT Remove unnecessary Pagesel directives. [0]\n");
  printf("  -q, --quiet                    Quiet.\n");
  printf("  -r, --use-shared               Use shared memory if necessary.\n");
  printf("  -s FILE, --script FILE         Linker script.\n");
  printf("  -t SIZE, --stack SIZE          Create a stack section.\n");
  printf("      --strict-options           If this is set, then an option may not be parameter\n"
         "                                   of an another option. For example: -s --quiet\n");
  printf("  -u, --macro symbol[=value]     Add macro value for script.\n");
  printf("  -v, --version                  Show version.\n");
  printf("  -w, --processor-mismatch       Disable \"processor mismatch\" warning.\n");
  printf("\n");
#ifdef USE_DEFAULT_PATHS
  if (gp_lkr_path != NULL) {
    printf("Default linker script path %s\n", gp_lkr_path);
  }
  else {
    printf("Default linker script path NOT SET\n");
  }

  if (gp_lib_path != NULL) {
    printf("Default library path %s\n", gp_lib_path);
  }
  else {
    printf("Default library path NOT SET\n");
  }
  printf("\n");
#endif
  printf("Report bugs to:\n");
  printf("%s\n", PACKAGE_BUGREPORT);
  exit(0);
}

/*------------------------------------------------------------------------------------------------*/

/* return the number of missing symbols */

static size_t
_count_missing(void)
{
  return gp_sym_get_symbol_count(state.symbol.missing);
}

/*------------------------------------------------------------------------------------------------*/

static void
_object_append(gp_object_t *object)
{
  gp_object_t *list;

  /* append the entry to the list */
  if (state.object == NULL) {
    state.object    = object;
    /* store the processor type from the first object file */
    state.processor = object->processor;
    state.class     = object->class;
  }
  else {
    list = state.object;

    while (list->next != NULL) {
      list = list->next;
    }
    list->next = object;

    if (object->class != state.class) {
      gp_error("Processor family mismatch in \"%s\".", object->filename);
    }
    else if ((processor_mismatch_warning) && (object->processor != state.processor)) {
      gp_warning("Processor mismatch in \"%s\".", object->filename);
    }
  }

  if (state.optimize.weak_symbols) {
    gp_coffgen_check_relocations(object, RELOC_DISABLE_WARN);
    gp_coffopt_remove_weak(object);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_archive_append(gp_archive_t *file, const char *name)
{
  struct archivelist *new;

  /* make the new entry */
  new = (struct archivelist *)GP_Malloc(sizeof(*new));
  new->name    = GP_Strdup(name);
  new->archive = file;
  new->next    = NULL;

  /* append the entry to the list */
  if (state.archives == NULL) {
    state.archives = new;
  }
  else {
    struct archivelist *list = state.archives;

    while (list->next != NULL){
      list = list->next;
    }
    list->next = new;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Scan the archive for missing symbol definitions.  This has to be done
   recursively.  The order of the archive members is unknown and there
   might be inter member dependancies.  Scan the archive muliple times.
   Stop whenever a complete pass through the archive happens and no
   objects are added. */

static gp_boolean
_scan_index(symbol_table_t *table, gp_archive_t *archive)
{
  const symbol_t *sym_miss;
  const symbol_t *sym_arch;
  gp_archive_t   *member;
  gp_object_t    *object;
  size_t          i;
  int             num_added;
  const char     *name;
  char           *object_name;
  gp_boolean      modified;

  num_added = 1; /* initalize to 1 so while loop can be entered */
  modified  = false;
  while (num_added != 0) {
    num_added = 0;
    for (i = 0; i < gp_sym_get_symbol_count(state.symbol.missing); ++i) {
      sym_miss = gp_sym_get_symbol_with_index(state.symbol.missing, i);
      name     = gp_sym_get_symbol_name(sym_miss);
      assert(name != NULL);
      /* Search for missing symbol name in archive symbol table. */
      sym_arch = gp_sym_get_symbol(table, name);

      if (sym_arch != NULL) {
        /* Fetch the archive member, convert its binary data to an object
           file, and add the object to the object list. */
        member      = gp_sym_get_symbol_annotation(sym_arch);
        object_name = gp_archive_member_name(member);
        object      = gp_convert_file(object_name, &member->data);
        _object_append(object);
        gp_cofflink_add_symbols(state.symbol.definition, state.symbol.missing, object);
        /* The symbol tables have been modified. Need to take another
           pass to make sure we get everything. */
        num_added++;
        modified = true;
        free(object_name);
        /* This branch of the table has been modified. Go to the next one. */
        break;
      }
    }
  }

  return modified;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_scan_archive(gp_archive_t *archive, const char *name)
{
  gp_boolean      modified;
  symbol_table_t *archive_tbl;

  state.symbol.archive = gp_sym_push_table(NULL, false);

  /* If necessary, build a symbol index for the archive. */
  if (gp_archive_have_index(archive) == 0) {
    archive_tbl = gp_sym_push_table(NULL, true);
    gp_archive_make_index(archive, archive_tbl);
    archive = gp_archive_add_index(archive_tbl, archive);
    gp_warning("\"%s\" is missing symbol index.", name);
    archive_tbl = gp_sym_pop_table(archive_tbl);
  }

  /* Read the symbol index. */
  gp_archive_read_index(state.symbol.archive, archive);

  /* Scan the symbol index for symbols in the missing symbol table.
     If found, add the object to state.objects. */
  modified = _scan_index(state.symbol.archive, archive);

  state.symbol.archive = gp_sym_pop_table(state.symbol.archive);

  return modified;
}

/*------------------------------------------------------------------------------------------------*/

/* Remove a symbol the linker created from the missing table. */

static void
_remove_linker_symbol(char *name)
{
  const symbol_t *sym;

  sym = gp_sym_get_symbol(state.symbol.missing, name);
  if (sym != NULL) {
    gp_cofflink_remove_symbol(state.symbol.missing, name);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Add a symbol the linker created to the symbol table. */

static void
_add_linker_symbol(const char *name)
{
  gp_symbol_t *current;
  gp_symbol_t *found;

  found   = NULL;
  current = state.object->symbol_list.first;
  while (current != NULL) {
    if ((current->name != NULL) && (strcmp(current->name, name) == 0) && (current->section_number > N_UNDEF)) {
      found = current;
      break;
    }
    current = current->next;
  }

  assert(found != NULL);
  gp_cofflink_add_symbol(state.symbol.definition, found, NULL);
}

/*------------------------------------------------------------------------------------------------*/

/* Search the object list for an idata section. */

static void
_search_idata(void)
{
  gp_object_t  *object;
  gp_section_t *section;

  object = state.object;
  while (object != NULL) {
    section = object->section_list.first;
    while (section != NULL) {
      if (section->flags & STYP_DATA) {
        state.has_idata = true;
        return;
      }
      section = section->next;
    }
    object = object->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Build the symbol tables. Determine which objects from the archives are required for linking. */

static void
_build_tables(void)
{
  gp_object_t        *object;
  struct archivelist *arlist;
  gp_boolean          modified;
  size_t              i;
  const symbol_t     *sym;
  const char         *name;
  gp_coffsymbol_t    *var;

  /* Create the object file symbol tables. */
  object = state.object;
  while (object != NULL) {
    gp_cofflink_add_symbols(state.symbol.definition, state.symbol.missing, object);
    object = object->next;
  }

  /* All of the objects have been scanned. If there are remaining references
     to symbols, then the archives must contain the missing references. */
  if ((_count_missing() > 0) && (state.archives != NULL)) {
    modified = false;
    arlist   = state.archives;
    while (true) {
      if (_scan_archive(arlist->archive, arlist->name)) {
        modified = true;
      }
      if (_count_missing() == 0) {
        /* No more missing references, no need to continue. */
        break;
      }
      else if (arlist->next == NULL) {
        if (modified) {
          /* At least one object was loaded from an archive and there are
             still missing symbols.  Scan all the archives again. */
          modified = false;
          arlist = state.archives;
        }
        else {
          /* Quit */
          break;
        }
      }
      else {
        arlist = arlist->next;
      }
    }
  }

  _search_idata();

  if (state.has_idata) {
    _remove_linker_symbol("_cinit");
  }

  if (state.has_stack) {
    _remove_linker_symbol("_stack");
    _remove_linker_symbol("_stack_end");
  }

  /* All of the archives have been scanned. If there are still missing
     references, it is an error. */
  if (_count_missing() > 0) {
    for (i = 0; i < gp_sym_get_symbol_count(state.symbol.missing); ++i) {
      sym  = gp_sym_get_symbol_with_index(state.symbol.missing, i);
      name = gp_sym_get_symbol_name(sym);
      assert(name != NULL);
      var = gp_sym_get_symbol_annotation(sym);
      assert(var != NULL);
      gp_error("Missing definition for symbol \"%s\", required by \"%s\".", name, var->file->filename);
    }
    exit(1);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Read a coff object or archive. gplink doesn't care about file extensions.
   This allows alternate extensions such as .a archives and .obj coff objects. */

void
gplink_open_coff(const char *name)
{
  gp_object_t  *object;
  gp_archive_t *archive;
  FILE         *coff;
  char          file_name[PATH_MAX + 1];

  strncpy(file_name, name, sizeof(file_name));

  coff = fopen(file_name, "rb");
  if ((coff == NULL) && (strchr(file_name, PATH_SEPARATOR_CHAR) == 0)) {
    /* If no PATH_SEPARATOR_CHAR in name, try searching include pathes. */
    int i;

    for (i = 0; i < state.num_paths; i++) {
      snprintf(file_name, sizeof(file_name), "%s" PATH_SEPARATOR_STR "%s", state.paths[i], name);
      coff = fopen(file_name, "rb");

      if (coff != NULL) {
        break;
      }
    }
  }

  if (coff == NULL) {
    perror(name);
    exit(1);
  }

  /* FIXME: Three files are opened, surely one is sufficent. */

  switch (gp_identify_coff_file(file_name)) {
    case GP_COFF_OBJECT_V2:
    case GP_COFF_OBJECT:
      /* read the object */
      object = gp_read_coff(file_name);
      /*object_append(object, file_name);*/
      _object_append(object);
      break;

    case GP_COFF_ARCHIVE:
      /* read the archive */
      archive = gp_archive_read(file_name);
      _archive_append(archive, file_name);
      break;

    case GP_COFF_SYS_ERR:
      gp_error("Can't open file \"%s\".", file_name);
      break;

    case GP_COFF_UNKNOWN:
      gp_error("\"%s\" is not a valid coff object or archive.", file_name);
      break;

    default:
      assert(0);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_set_optimize_level(void)
{
  /* default */
  state.optimize.pagesel       = false;
  state.optimize.dead_sections = false;
  state.optimize.weak_symbols  = false;

  switch(state.optimize.level) {
  case 3:
    /* fall through */
  case 2:
    state.optimize.dead_sections = true;
    /* fall through */
  case 1:
    state.optimize.weak_symbols  = true;
    /* fall through */
  case 0:
    break;

  default:
    gp_error("Invalid optimization level: %i", state.optimize.level);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_init(void)
{
  gp_init();

  /* initialize */
  gp_date_string(state.start_date, sizeof(state.start_date));
  state.hex_format          = INHX32;
  state.num_paths           = 0;
  state.processor           = NULL;
  state.optimize.level      = OPTIMIZE_LEVEL_DEFAULT;
  state.optimize.pagesel    = 0;
  state.optimize.banksel    = 0;
  state.cod_file            = OUT_NORMAL;
  state.hex_file            = OUT_NORMAL;
  state.lst_file            = OUT_NORMAL;
  state.map_file            = OUT_SUPPRESS;
  state.obj_file            = OUT_SUPPRESS;
  state.fill_enable         = false;
  state.fill_value          = 0;
  state.has_stack           = false;
  state.stack_size          = 0;
  state.has_idata           = false;
  state.src_file_names      = NULL;
  state.src_file_names_tail = NULL;
  state.object              = NULL;
  state.archives            = NULL;

  /* set default output filename to be a.o, a.hex, a.cod, a.map */
  strncpy(state.base_file_name, "a", sizeof(state.base_file_name));

  state.ifdef = NULL;
  state.script_symbols     = gp_sym_push_table(NULL, false);

  /* The symbols are case sensitive. */
  state.symbol.definition  = gp_sym_push_table(NULL, false);
  state.symbol.missing     = gp_sym_push_table(NULL, false);
  state.section.definition = gp_sym_push_table(NULL, false);
  state.section.logical    = gp_sym_push_table(NULL, false);
}

/*------------------------------------------------------------------------------------------------*/

void
gplink_add_path(const char *path)
{
  if (state.num_paths < MAX_PATHS) {
    state.paths[state.num_paths++] = GP_Strdup(path);
  }
  else {
    gp_error("Too many -I paths.");
  }
}

/*------------------------------------------------------------------------------------------------*/

/* I have this take the func in anticipation of the option
  printf("  -z, --defsym symbol=value      Add symbol value to symbol table.\n");
*/

static void
_parse_define(const char *optarg, void (*func)(const char *name, long value))
{
  long  value = 0;
  char *pc = strchr(optarg, '=');

  if (pc != NULL) {
    *pc++ = '\0';
    value = strtol(pc, &pc, 10);
  }
  func(optarg, value);
}

/*------------------------------------------------------------------------------------------------*/

static void
_process_args(int argc, char *argv[])
{
  int         option_index;
  int         c;
  const char *command;
  gp_boolean  strict_options;
  gp_boolean  usage;
  char       *pc;

  strict_options             = false;
  usage                      = false;
  enable_cinit_wanings       = true;
  processor_mismatch_warning = true;

  /* Scan through the options for the --strict-options flag. */
  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, NULL)) != EOF) {
    if (c == OPT_STRICT_OPTIONS) {
      strict_options = true;
      break;
    }
  }

  /* Restores the getopt_long index. */
  optind = 1;

  /* second pass through options */
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

    if (c == 'O') {
      state.optimize.level = atoi(optarg);
      break;
    }
  }

  /* reset the getopt_long index for the next call */
  optind = 1;

  _set_optimize_level();

  /* third pass through options */
  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, NULL)) != EOF) {
    switch (c) {
    case 'a':
      if (strcasecmp(optarg, "inhx8m") == 0) {
        state.hex_format = INHX8M;
      }
      else if (strcasecmp(optarg, "inhx16") == 0) {
        state.hex_format = INHX16;
      }
      else if (strcasecmp(optarg, "inhx32") == 0) {
        state.hex_format = INHX32;
      }
      else {
        gp_error("Invalid hex format \"%s\", expected inhx8m, inhx16, or inhx32.", optarg);
      }
      break;

    case 'b':
      state.optimize.banksel = (unsigned int)strtol(optarg, &pc, 10);

      if ((pc == NULL) || (*pc != '\0')) {
        gp_error("Invalid character %#x in number constant.", *pc);
      }
      break;

    case 'c':
      state.obj_file = OUT_NORMAL;
      break;

    case 'C':
      enable_cinit_wanings = false;
      break;

    case 'd':
      gp_debug_disable = false;
      yydebug = 1;
      break;

    case 'f':
      state.fill_value = strtol(optarg, &pc, 16);
      if ((pc == NULL) || (*pc != '\0')) {
        gp_error("Invalid character %#x in number constant.", *pc);
      }
      else if (state.fill_value > 0xffff) {
        gp_error("Fill value exceeds 0xffff: %#x", *pc);
      }
      else {
        state.fill_enable = true;
      }
      break;

    case '?':
    case 'h':
      usage = true;
      break;

    case 'I':
      gplink_add_path(optarg);
      break;

    case 'l':
      state.lst_file = OUT_SUPPRESS;
      break;

    case 'm':
      state.map_file = OUT_NORMAL;
      break;

    case 'o':
      strncpy(state.base_file_name, optarg, sizeof(state.base_file_name));
      pc = strrchr(state.base_file_name, '.');

      if (pc != NULL) {
        *pc = '\0';
      }
      break;

    case 'O':
      /* do nothing */
      break;

    case 'p':
      state.optimize.pagesel = (unsigned int)strtol(optarg, &pc, 10);

      if ((pc == NULL) || (*pc != '\0')) {
        gp_error("Invalid character %#x in number constant.", *pc);
      }
      break;

    case 'q':
      gp_quiet = true;
      break;

    case 'r':
      gp_relocate_to_shared = true;
      break;

    case 's':
      {
        srcfns_t *fn;

        fn = GP_Malloc(sizeof(srcfns_t));
        fn->file_name = GP_Strdup(optarg);
        fn->next      = NULL;

        if (state.src_file_names == NULL) {
          state.src_file_names = fn;
        }
        else {
          state.src_file_names_tail->next = fn;
        }

        state.src_file_names_tail = fn;
      }
      break;

    case 't': {
      state.stack_size = strtol(optarg, &pc, 10);

      if ((pc == NULL) || (*pc != '\0')) {
        gp_error("Invalid character %#x in number constant.", *pc);
      }
      else {
        state.has_stack = true;
      }
      break;
    }

    case 'u':
      _parse_define(optarg, script_add_macro);
      break;

    case 'v':
      fprintf(stderr, "%s\n", GPLINK_VERSION_STRING);
      exit(0);
      break;

    case 'w':
      processor_mismatch_warning = false;
      break;

    case OPT_MPLINK_COMPATIBLE:
      state.mplink_compatible = true;
      break;

    case OPT_STRICT_OPTIONS:
      /* do nothing */
      break;
    }

    if (usage) {
      break;
    }
  }

  /* check if the first file is the linker script */
  if (argv[optind] != NULL) {
    pc = strrchr(argv[optind], '.');

    if ((pc != NULL) && (strcasecmp(pc, ".lkr") == 0)) {
      srcfns_t *fn;

      fn = GP_Malloc(sizeof(srcfns_t));
      fn->file_name = GP_Strdup(argv[optind++]);
      fn->next      = NULL;

      if (state.src_file_names == NULL) {
        state.src_file_names = fn;
      }
      else {
        state.src_file_names_tail->next = fn;
      }

      state.src_file_names_tail = fn;
    }
  }

  if ((state.src_file_names == NULL) && (optind >= argc)) {
    /* No linker script was specified and no object filenames were provided,
       so print the usage. */
    usage = true;
  }

  if (usage) {
    _show_usage();
  }

  /* Add the library path to the include paths list last, so that the user
     specified directories are searched first. */
  if (gp_lib_path != NULL) {
    gplink_add_path(gp_lib_path);
  }

  if (gp_lkr_path != NULL) {
    gplink_add_path(gp_lkr_path);
  }

  /* Open all objects and archives in the file list. */
  for ( ; optind < argc; optind++) {
    gplink_open_coff(argv[optind]);
  }
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_linker(void)
{
  MemBlock_t *data;
  MemBlock_t *program;
  srcfns_t   *p;

  /* setup output filenames */
  snprintf(state.hex_file_name, sizeof(state.hex_file_name), "%s.hex", state.base_file_name);
  snprintf(state.map_file_name, sizeof(state.map_file_name), "%s.map", state.base_file_name);
  snprintf(state.obj_file_name, sizeof(state.obj_file_name), "%s.cof", state.base_file_name);

  /* Read the script. */
  if (state.src_file_names != NULL) {
    p = state.src_file_names;

    do {
      open_src(p->file_name, false);
      yyparse();
      p = p->next;
    } while (p != NULL);
#ifdef USE_DEFAULT_PATHS
  }
  else if ((state.object != NULL) && (gp_lkr_path != NULL)) {
    /* The processor is known because an object was on the command line. So
       use one of the default scripts that are distributed with gputils. */
    const char *script_name;
    char        file_name[BUFSIZ];

    assert(state.processor);
    script_name = gp_processor_script(state.processor);

    if (script_name == NULL) {
      gp_error("Linker script not specified and can't determine default script.");
      return EXIT_FAILURE;
    }

    snprintf(file_name, sizeof(file_name), "%s" PATH_SEPARATOR_STR "%s", gp_lkr_path, script_name);
    gp_message("Using default linker script \"%s\".", file_name);
    open_src(file_name, false);
    yyparse();
#endif
  }
  else {
    /* The user must supply the linker script name.  The processor isn't
       commanded so the linker has no way to pick. */
    gp_error("Linker script not specified.");
    return EXIT_FAILURE;
  }

  if (state.object == NULL) {
    gp_error("Missing input object file.");
    return EXIT_FAILURE;
  }

  /* An error occured while reading the input files, no need to continue. */
  if (gp_num_errors > 0) {
    return EXIT_FAILURE;
  }

  /* Construct the symbol tables. Determine which archive members are
     required to resolve external references. */
  _build_tables();

  /* combine all object files into one object */
  gp_cofflink_combine_objects(state.object);

  /* add the stack section */
  if (state.has_stack) {
    gp_cofflink_make_stack(state.object, state.stack_size);
    _add_linker_symbol("_stack");
    _add_linker_symbol("_stack_end");
  }

  if (state.has_idata) {
    gp_cofflink_make_cinit(state.object);
    _add_linker_symbol("_cinit");
  }

  /* clean up symbol table */
  gp_cofflink_clean_table(state.object, state.symbol.definition);
  gp_coffgen_check_relocations(state.object, (enable_cinit_wanings) ? RELOC_ENABLE_CINIT_WARN : 0);

  if (state.optimize.dead_sections) {
    gp_coffopt_remove_dead_sections(state.object, 0, enable_cinit_wanings);
  }

  /* combine overlay sections */
  gp_cofflink_combine_overlay(state.object, 0);

  /* combine all sections with the same name */
  gp_cofflink_merge_sections(state.object);
  gp_symbol_make_hash_table(state.object);

  /* create ROM data for initialized data sections */
  gp_cofflink_make_idata(state.object, state.mplink_compatible);

  /* create memory representing target memory */
  data    = gp_mem_i_create();
  program = gp_mem_i_create();

  /* allocate memory for absolute sections */
  gp_debug("Verifying absolute sections.");
  gp_cofflink_reloc_abs(state.object, program, state.class->org_to_byte_shift,
                        STYP_ROM_AREA);

  gp_cofflink_reloc_abs(state.object, data, 0,
                        STYP_RAM_AREA | STYP_SHARED | STYP_OVERLAY | STYP_ACCESS);

  if (state.mplink_compatible) {
    /* allocate cinit section to the lowest possible address */
    gp_section_t *cinit_section;

    cinit_section = gp_coffgen_find_section(state.object, state.object->section_list.first, ".cinit");

    if (cinit_section != NULL) {
      gp_cofflink_reloc_cinit(state.object, program, state.class->org_to_byte_shift,
                              cinit_section, state.section.definition);
    }
  }

  /* FIXME: allocate assigned stacks */

  /* allocate memory for relocatable assigned sections */
  gp_debug("Relocating assigned sections.");
  gp_cofflink_reloc_assigned(state.object, program, state.class->org_to_byte_shift,
                             STYP_ROM_AREA,
                             state.section.definition, state.section.logical);

  gp_cofflink_reloc_assigned(state.object, data, 0,
                             STYP_RAM_AREA | STYP_SHARED | STYP_OVERLAY | STYP_ACCESS,
                             state.section.definition, state.section.logical);

  /* FIXME: allocate unassigned stacks */

  /* allocate memory for relocatable unassigned sections */
  gp_debug("Relocating unassigned sections.");
  gp_cofflink_reloc_unassigned(state.object, program, state.class->org_to_byte_shift,
                               STYP_ROM_AREA, state.section.definition);

  gp_cofflink_reloc_unassigned(state.object, data, 0,
                               STYP_RAM_AREA | STYP_SHARED | STYP_OVERLAY | STYP_ACCESS,
                               state.section.definition);

  /* load the table with the relocated addresses */
  gp_cofflink_add_cinit_section(state.object);

  gp_cofflink_update_table(state.object, state.class->org_to_byte_shift);

  if (state.optimize.pagesel > 0) {
    gp_coffopt_remove_unnecessary_pagesel(state.object);
  }

  if (state.optimize.banksel > 0) {
    gp_coffopt_remove_unnecessary_banksel(state.object);
  }

  gp_coffgen_make_linenum_array(state.object);

  gp_cofflink_fill_pages(state.object, program, state.section.definition);

  gp_mem_i_free(data);
  gp_mem_i_free(program);

  /* patch raw data with the relocated symbol values */
  gp_cofflink_patch(state.object);

  /* Modify the executable object name. */
  if (state.object->filename != NULL) {
    free(state.object->filename);
  }

  state.object->filename  = GP_Strdup(state.obj_file_name);
  state.object->flags    |= F_EXEC;

  if (state.obj_file == OUT_NORMAL) {
    /* write the executable object in memory */
    if (!gp_writeobj_write_coff(state.object, gp_num_errors)) {
      gp_error("Error while writing object file.");
      exit(1);
    }
  }
  else {
    unlink(state.object->filename);
  }

  /* convert the executable object into a hex file */
  state.i_memory = gp_cofflink_make_memory(state.object);

  /* write hex file */
  if (!gp_writehex(state.base_file_name, state.i_memory, state.hex_format, gp_num_errors,
                   0, state.class->core_mask)) {
    gp_error("Error while writing hex file.");
    exit(1);
  }

  /* convert the executable object into a cod file and list file */
  cod_init();
  lst_write();
  cod_close_file();

  /* write map file */
  make_map();

  gp_mem_i_free(state.i_memory);
  gp_coffgen_free_object(state.object);

  return (gp_num_errors <= 0);
}

/*------------------------------------------------------------------------------------------------*/

int
main(int argc, char *argv[])
{
  _init();
  _process_args(argc, argv);

  return (_linker() ? EXIT_SUCCESS : EXIT_FAILURE);
}

/* GNU PIC Linker
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
#include "gplink.h"
#include "cod.h"
#include "scan.h"
#include "lst.h"
#include "map.h"

struct gplink_state state;

int yyparse(void);
extern int yydebug;

/* return the number of missing symbols */
int count_missing(void)
{
  return state.symbol.missing->count;
}

void object_append(gp_object_type *file, char *name)
{
  /* append the entry to the list */
  if (state.object == NULL) {
    state.object = file;
    /* store the processor type from the first object file */
    state.processor = file->processor;
    state.class = file->class;
  } else {
    gp_object_type *list = state.object;

    while(list->next != NULL){
      list = list->next;
    }
    list->next = file;
    
    if (file->class != state.class) {
      gp_error("processor family mismatch in \"%s\"", file->filename);
    } else if (file->processor != state.processor) {
      gp_warning("processor mismatch in \"%s\"", file->filename);
    }

  }

  if (state.optimize.weak_symbols) {
    gp_cofflink_remove_weak(file);
  }

  return;
}

void archive_append(gp_archive_type *file, char *name)
{
  struct archivelist *new;

  /* make the new entry */  
  new = (struct archivelist *)malloc(sizeof(*new));
  new->name = strdup(name); 
  new->archive = file;
  new->next = NULL;
  
  /* append the entry to the list */
  if (state.archives == NULL) {
    state.archives = new;
  } else {
    struct archivelist *list = state.archives;

    while(list->next != NULL){
      list = list->next;
    }
    list->next = new;
  }

  return;
}

/* Scan the archive for missing symbol definitions.  This has to be done
   recursively.  The order of the archive members is unknown and there
   might be inter member dependancies.  Scan the archive muliple times.  
   Stop whenever a complete pass through the archive happens and no
   objects are added. */

gp_boolean
scan_index(struct symbol_table *table, gp_archive_type *archive)
{
  struct symbol *s;
  struct symbol *m;
  gp_archive_type *member;
  gp_object_type *object;
  int i;
  int num_added = 1; /* initalize to 1 so while loop can be entered */
  char *name;
  char *object_name;
  gp_boolean modified = false;

  while (num_added != 0) {
    num_added = 0;
    for (i = 0; i < HASH_SIZE; i++) {
      for (s = state.symbol.missing->hash_table[i]; s; s = s->next) {
	name = get_symbol_name(s);
        assert(name != NULL);
        /* Search for missing symbol name in archive symbol table */
        m = get_symbol(table, name);
        if (m != NULL) {
          /* Fetch the archive member, convert its binary data to an object 
	     file, and add the object to the object list */ 
          member = get_symbol_annotation(m);
          object_name = gp_archive_member_name(member);
          object = gp_convert_file(object_name, member->file);
          object_append(object, object_name);	
          gp_link_add_symbols(state.symbol.definition, 
                              state.symbol.missing, 
			      object);		
          /* The symbol tables have been modified.  Need to take another
	     pass to make sure we get everything. */
          num_added++;
          modified = true;
          free(object_name);
          /* This branch of the table has been modified. Go to the next one */
	  break;
	}      
      }
    }
  }

  return modified;
}
    
gp_boolean
scan_archive(gp_archive_type *archive, char *name)
{
  gp_boolean modified;

  state.symbol.archive = push_symbol_table(NULL, false);

  /* If necessary, build a symbol index for the archive. */
  if (gp_archive_have_index(archive) == 0) {
    struct symbol_table *archive_tbl = NULL;
  
    archive_tbl = push_symbol_table(NULL, true);
    gp_archive_make_index(archive, archive_tbl);
    archive = gp_archive_add_index(archive_tbl, archive);
    gp_warning("\"%s\" is missing symbol index", name);
    archive_tbl = pop_symbol_table(archive_tbl);
  }

  /* Read the symbol index */
  gp_archive_read_index(state.symbol.archive, archive);

  /* Scan the symbol index for symbols in the missing symbol table.  If
     found, add the object to state.objects. */
  modified = scan_index(state.symbol.archive, archive);

  state.symbol.archive = pop_symbol_table(state.symbol.archive);

  return modified;
}

/* Remove a symbol the linker created from the missing table */

static void
remove_linker_symbol(char *name)
{
  struct symbol *sym;

  sym = get_symbol(state.symbol.missing, name);
  if (sym != NULL) {
    gp_link_remove_symbol(state.symbol.missing, name);
  }

  return;
}

/* Add a symbol the linker created to the symbol table. */

static void
add_linker_symbol(char *name)
{
  gp_symbol_type *current = state.object->symbols;
  gp_symbol_type *found = NULL;

  while (current != NULL) {
    if ((current->name != NULL) &&
        (strcmp(current->name, name) == 0) &&
        (current->section_number > 0)) {
      found = current;
      break;
    }
    current = current->next;
  }

  assert(found != NULL);
  gp_link_add_symbol(state.symbol.definition, found, NULL);

  return;
}

/* Search the object list for an idata section. */

static void
search_idata(void)
{
  gp_object_type *list = state.object;
  gp_section_type *section;

  while (list != NULL) {
    section = list->sections;
    while (section != NULL) {
      if (section->flags & STYP_DATA) {
        state.has_idata = true;
        return;    
      }
      section = section->next;
    }
    list = list->next;
  }

  return;
}

/* Build the symbol tables.  Determine which objects from the archives are 
   required for linking */
    
void build_tables(void)
{
  gp_object_type *list = state.object;
  struct archivelist *arlist;
  gp_boolean modified;
  int i;
  struct symbol *s;
  char *name;
  gp_coffsymbol_type *var;

  /* Create the object file symbol tables */  
  while (list != NULL) {
    gp_link_add_symbols(state.symbol.definition, 
                        state.symbol.missing, 
			list);
    list = list->next;
  }

  /* All of the objects have been scanned.  If there are remaining references
     to symbols, then the archives must contain the missing references. */
  if (count_missing() && (state.archives != NULL)) {
    modified = false;
    arlist = state.archives;
    while (1) {
      if (scan_archive(arlist->archive, arlist->name)) {
        modified = true;
      }
      if (count_missing() == 0) {
        /* No more missing references, no need to continue. */
        break;
      } else if (arlist->next == NULL) {
        if (modified) {
          /* At least one object was loaded from an archive and there are
             still missing symbols.  Scan all the archives again. */
          modified = false;
          arlist = state.archives;
        } else {
          /* Quit */
          break;
        }
      } else {
        arlist = arlist->next;
      }
    }
  }

  search_idata();

  if (state.has_idata) {
    remove_linker_symbol("_cinit");
  }

  if (state.has_stack) {
    remove_linker_symbol("_stack");
    remove_linker_symbol("_stack_end");
  }

  /* All of the archives have been scanned.  If there are still missing
     references, it is an error */
  if (count_missing()) {
    for (i = 0; i < HASH_SIZE; i++) {
      for (s = state.symbol.missing->hash_table[i]; s; s = s->next) {
        name = get_symbol_name(s);
        assert(name != NULL);
        var = get_symbol_annotation(s);
        assert(var != NULL);
        gp_error("missing definition for symbol \"%s\", required by \"%s\"",
                 name,
                 var->file->filename);
      }
    }
    exit(1);
  }

  return;
}

/* Read a coff object or archive.  gplink doesn't care about file extensions.
   This allows alternate extensions such as .a archives and .obj coff 
   objects.  */

void gplink_open_coff(char *name)
{
  gp_object_type *object;
  gp_archive_type *archive;
  FILE *coff;
  char file_name[BUFSIZ];
  
  strncpy(file_name, name, sizeof(file_name));
  
  coff = fopen(file_name, "rb");
  if ((coff == NULL) && (strchr(file_name, PATH_CHAR) == 0)) { 
    /* If no "/" in name, try searching include pathes */
    int i;

    for(i = 0; i < state.numpaths; i++) {
      strncpy(file_name, state.paths[i], sizeof(file_name));
      strncat(file_name, COPY_CHAR, sizeof(file_name));
      strncat(file_name, name, sizeof(file_name));
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

  /* FIXME: Three files are opened, surely one is sufficent */

  switch(gp_identify_coff_file(file_name)) {
  case object_file:
    /* read the object */  
    object = gp_read_coff(file_name);
    object_append(object, file_name);
    break;
  case archive_file:
    /* read the archive */  
    archive = gp_archive_read(file_name);
    archive_append(archive, file_name);
    break;
  case sys_err_file:
    gp_error("can't open file \"%s\"", file_name);     
    break;
  case unknown_file:
    gp_error("\"%s\" is not a valid coff object or archive", file_name);     
    break; 
  default:
    assert(0);
  }

}   

static void
set_optimize_level(void)
{

  /* default */
  state.optimize.weak_symbols = false;

  switch(state.optimize.level) {
  case 3:
    /* fall through */
  case 2:
    /* fall through */
  case 1:
    state.optimize.weak_symbols = true;
    /* fall through */
  case 0:
    break;
  default:
    gp_error("invalid optimization level");
  }

  return;
}

#define GET_OPTIONS "?a:cdf:hI:lmo:O:qrs:t:v"

  static struct option longopts[] =
  {
    { "hex-format",  1, 0, 'a' },
    { "object",      0, 0, 'c' },
    { "debug",       0, 0, 'd' },
    { "fill",        1, 0, 'f' },
    { "help",        0, 0, 'h' },
    { "include",     1, 0, 'I' },
    { "no-list",     0, 0, 'l' },
    { "map",         0, 0, 'm' },
    { "output",      1, 0, 'o' },
    { "optimize",    1, 0, 'O' },
    { "quiet",       0, 0, 'q' },
    { "use-shared",  0, 0, 'r' },
    { "script",      1, 0, 's' },
    { "stack",       1, 0, 't' },
    { "version",     0, 0, 'v' },
    { 0, 0, 0, 0 }
  };

void
init(void)
{

  gp_init();

  /* initialize */
  gp_date_string(state.startdate, sizeof(state.startdate));
  state.hex_format = inhx32;
  state.numpaths = 0;
  state.byte_addr = 0;
  state.processor = no_processor;
  state.optimize.level = 1;
  state.codfile = normal;
  state.hexfile = normal;
  state.lstfile = normal;
  state.mapfile = suppress;
  state.objfile = suppress;
  state.fill_enable = false;
  state.fill_value = 0;
  state.has_stack = false;
  state.stack_size = 0;
  state.has_idata = false;
  state.srcfilename = NULL;
  state.object  = NULL;
  state.archives = NULL;

  /* set default output filename to be a.o, a.hex, a.cod, a.map */
  strncpy(state.basefilename, "a", sizeof(state.basefilename));

  /* The symbols are case sensitive */
  state.symbol.definition = push_symbol_table(NULL, false);
  state.symbol.missing = push_symbol_table(NULL, false);
  state.section.definition = push_symbol_table(NULL, false);
  state.section.logical = push_symbol_table(NULL, false);

  return;
}

void gplink_add_path(char *path)
{
  if(state.numpaths < MAX_PATHS) {
    state.paths[state.numpaths++] = strdup(path);
  } else {
    gp_error("too many -I paths");
  }
}

void show_usage(void)
{
  printf("Usage: gplink [options] [objects] [libraries] \n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -a FMT, --hex-format FMT       Select hex file format.\n");
  printf("  -c, --object                   Output executable object file.\n");
  printf("  -d, --debug                    Output debug messages.\n");
  printf("  -f VALUE, --fill VALUE         Fill unused program memory with value.\n");
  printf("  -h, --help                     Show this usage message.\n");
  printf("  -I DIR, --include DIR          Specify include directory.\n");
  printf("  -l, --no-list                  Disable list file output.\n");
  printf("  -m, --map                      Output a map file.\n");
  printf("  -o FILE, --output FILE         Alternate name of output file.\n");
  printf("  -O OPT, --optimize OPT         Optimization level [1].\n");
  printf("  -q, --quiet                    Quiet.\n");
  printf("  -r, --use-shared               Use shared memory if necessary.\n");
  printf("  -s FILE, --script FILE         Linker script.\n");
  printf("  -t SIZE, --stack SIZE          Create a stack section.\n");
  printf("  -v, --version                  Show version.\n");
  printf("\n");
#ifdef USE_DEFAULT_PATHS
  if (gp_lkr_path) {
    printf("Default linker script path %s\n", gp_lkr_path);
  } else {
    printf("Default linker script path NOT SET\n");
  }
  if (gp_lkr_path) {
    printf("Default library path %s\n", gp_lib_path);
  } else {
    printf("Default library path NOT SET\n");
  }
  printf("\n");    
#endif
  printf("Report bugs to:\n");
  printf("%s\n", PACKAGE_BUGREPORT);
  exit(0);
}

void
process_args( int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  gp_boolean usage = false;
  char *pc;

  /* first pass through options */
  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, 0)) != EOF) {
    switch (c) {
    case 'O':
      state.optimize.level = atoi(optarg);
      break;
    }
  }

  /* reset the getopt_long index for the next call */
  optind = 1;

  set_optimize_level();

  /* second pass through options */
  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, 0)) != EOF) {
    switch (c) {
    case 'a':
      if (strcasecmp(optarg, "inhx8m") == 0) {
        state.hex_format = inhx8m;
      } else if (strcasecmp(optarg, "inhx16") == 0) {
        state.hex_format = inhx16;
      } else if (strcasecmp(optarg, "inhx32") == 0) {
        state.hex_format = inhx32;
      } else {
        gp_error("invalid hex format \"%s\", expected inhx8m, inhx16, or inhx32",
                 optarg);
      }
      break;
    case 'c':
      state.objfile = normal;
      break;
    case 'd':
      gp_debug_disable = false;
      yydebug = 1;
      break;
    case 'f':
      state.fill_value = strtol(optarg, &pc, 16);
      if ((pc == NULL) || (*pc != '\0')) {
        gp_error("invalid character %#x in number constant", *pc);
      } else if (state.fill_value > 0xffff) {
        gp_error("fill value exceeds 0xffff", *pc);
      } else {
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
      state.lstfile = suppress;
      break;
    case 'm':
      state.mapfile = normal;
      break;
    case 'o':
      strncpy(state.basefilename, optarg, sizeof(state.basefilename));
      pc = strrchr(state.basefilename, '.');
      if (pc)
        *pc = 0;
      break;
    case 'O':
      /* do nothing */
      break;
    case 'q':
      gp_quiet = true;
      break;
    case 'r':
      gp_relocate_to_shared = true;
      break;
    case 's':
      state.srcfilename = optarg;
      break;
    case 't':
      state.stack_size = strtol(optarg, &pc, 10);
      if ((pc == NULL) || (*pc != '\0')) {
        gp_error("invalid character %#x in number constant", *pc);
      } else {
        state.has_stack = true;
      }
      break;
    case 'v':
      fprintf(stderr, "%s\n", GPLINK_VERSION_STRING);
      exit(0);
    }
    if (usage)
      break;
  }

  /* check if the first file is the linker script */
  if (argv[optind]) {
    pc = strrchr(argv[optind], '.');
    if (strcasecmp(pc, ".lkr") == 0) {
      if (state.srcfilename == NULL) {
        state.srcfilename = argv[optind++];
      } else {
        gp_error("linker script specified twice (%s and %s)",
                 state.srcfilename,
                 argv[optind]);
      }
    }
  }

  if ((state.srcfilename == NULL) &&
      (optind >= argc)) {
    /* No linker script was specified and no object filenames were provided,
       so print the usage */
    usage = true;
  }

  if (usage) {
    show_usage();
  }

  /* Add the library path to the include paths list last, so that the user
     specified directories are searched first */
  if (gp_lib_path) {
    gplink_add_path(gp_lib_path);
  }

  /* Open all objects and archives in the file list. */ 
  for ( ; optind < argc; optind++) {
    gplink_open_coff(argv[optind]);
  }

}

int
linker(void)
{
  MemBlock *data, *program;

  /* setup output filenames */
  strncpy(state.hexfilename, state.basefilename, sizeof(state.hexfilename));
  strncat(state.hexfilename, ".hex", sizeof(state.hexfilename));
  strncpy(state.mapfilename, state.basefilename, sizeof(state.mapfilename));
  strncat(state.mapfilename, ".map", sizeof(state.mapfilename));
  strncpy(state.objfilename, state.basefilename, sizeof(state.objfilename));
  strncat(state.objfilename, ".cof", sizeof(state.objfilename));

  /* Read the script */
  if (state.srcfilename) {
    open_src(state.srcfilename, 0);
    yyparse();
#ifdef USE_DEFAULT_PATHS
  } else if ((state.object) && (gp_lkr_path)) {
    /* The processor is known because an object was on the command line. So
       use one of the default scripts that are distributed with gputils. */
    char file_name[BUFSIZ];
    char *script_name;
    
    assert(state.processor != no_processor); 
    script_name = gp_processor_script(state.processor);
    if (script_name == NULL) {
      gp_error("linker script not specified and can't determine default script");
      return EXIT_FAILURE; 
    }
    strncpy(file_name, gp_lkr_path, sizeof(file_name));
    strncat(file_name, COPY_CHAR, sizeof(file_name));
    strncat(file_name, script_name, sizeof(file_name));
    gp_message("using default linker script \"%s\"", file_name);
    open_src(file_name, 0);
    yyparse();
#endif
  } else {
    /* The user must supply the linker script name.  The processor isn't
       commanded so the linker has no way to pick. */ 
    gp_error("linker script not specified");
    return EXIT_FAILURE; 
  }

  if (state.object == NULL) {
    gp_error("missing input object file");
    return EXIT_FAILURE; 
  }

  if (state.class == PROC_CLASS_PIC16E) {
    state.byte_addr = 1;
  } else {
    state.byte_addr = 0;
  }

  /* Construct the symbol tables. Determine which archive members are 
     required to resolve external references. */
  build_tables();  

  /* add the stack section */
  if (state.has_stack) {
    gp_cofflink_make_stack(state.object, state.stack_size);
  }

  /* combine all object files into one object */
  gp_cofflink_combine_objects(state.object);

  /* combine overlay sections */
  gp_cofflink_combine_overlay(state.object, 0);

  /* combine all sections with the same name */
  gp_cofflink_merge_sections(state.object, state.byte_addr);

  /* create ROM data for initialized data sections */
  gp_cofflink_make_idata(state.object);

  /* create memory representing target memory */ 
  data = i_memory_create();
  program = i_memory_create();

  /* allocate memory for absolute sections */
  gp_debug("verifying absolute sections.");
  gp_cofflink_reloc_abs(program, 
                        state.byte_addr,
                        state.object->sections,
                        STYP_TEXT | STYP_DATA_ROM);
  gp_cofflink_reloc_abs(data, 
                        0,
                        state.object->sections, 
                        STYP_DATA | STYP_BSS | STYP_SHARED | 
                        STYP_OVERLAY | STYP_ACCESS);

  /* FIXME: allocate assigned stacks */ 

  /* allocate memory for relocatable assigned sections */
  gp_debug("relocating assigned sections.");
  gp_cofflink_reloc_assigned(program, 
                             state.byte_addr,
                             state.object->sections,
                             STYP_TEXT | STYP_DATA_ROM,
                             state.section.definition,
                             state.section.logical);
  gp_cofflink_reloc_assigned(data, 
                             0,
                             state.object->sections, 
                             STYP_DATA | STYP_BSS | STYP_SHARED | 
                             STYP_OVERLAY | STYP_ACCESS,
                             state.section.definition,
                             state.section.logical);
  
  /* FIXME: allocate unassigned stacks */ 

  /* allocate memory for relocatable unassigned sections */
  gp_debug("relocating unassigned sections.");
  gp_cofflink_reloc_unassigned(program, 
                               state.byte_addr,
                               state.object->sections,
                               STYP_TEXT | STYP_DATA_ROM,
                               state.section.definition);
  gp_cofflink_reloc_unassigned(data, 
                               0,
                               state.object->sections, 
                               STYP_DATA | STYP_BSS | STYP_SHARED | 
                               STYP_OVERLAY | STYP_ACCESS,
                               state.section.definition);

  /* load the table with the relocated addresses */
  gp_add_cinit_section(state.object, state.byte_addr);

  gp_cofflink_update_table(state.object);

  gp_cofflink_fill_pages(state.object,
                         program,
                         state.byte_addr,
                         state.section.definition);

  i_memory_free(data);
  i_memory_free(program);

  if (state.has_idata) {
    add_linker_symbol("_cinit");
  }

  if (state.has_stack) {
    add_linker_symbol("_stack");
    add_linker_symbol("_stack_end");
  }

  /* patch raw data with the relocated symbol values */
  gp_cofflink_patch(state.object, state.symbol.definition);

  /* clean up symbol table */
  gp_cofflink_clean_table(state.object);

  /* modify the executable object data */
  state.object->filename = strdup(state.objfilename);
  state.object->flags |= F_EXEC;

  if (state.objfile == normal) {
    /* write the executable object in memory */
    gp_write_coff(state.object, gp_num_errors);
  } else {
    unlink(state.object->filename);
  }

  /* convert the executable object into a hex file */
  state.i_memory = gp_cofflink_make_memory(state.object);
  
  /* write hex file */
  writehex(state.basefilename,
           state.i_memory,
           state.hex_format,
           gp_num_errors,
           0,
           0);

  /* convert the executable object into a cod file and list file */
  cod_init();
  write_lst();
  cod_close_file();

  /* write map file */
  make_map();

  i_memory_free(state.i_memory);
  
  if (gp_num_errors > 0)
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;

}

int 
main(int argc, char *argv[])
{
  init();
  process_args(argc, argv);

  return linker();
}

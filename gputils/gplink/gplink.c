/* GNU PIC Linker
   Copyright (C) 2001, 2002, 2003
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
#include "map.h"

struct gplink_state state;

int yyparse(void);

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
    
    if (file->class != state.class)
      gp_error("processor family mismatch in \"%s\"", file->filename);
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

int scan_index(struct symbol_table *table, gp_archive_type *archive)
{
  struct symbol *s;
  struct symbol *m;
  gp_archive_type *member;
  gp_object_type *object;
  int i;
  int num_added = 1; /* initalize to 1 so while loop can be entered */
  char *name;
  char *object_name;

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
          free(object_name);
          /* This branch of the table has been modified. Go to the next one */
	  break;
	}      
      }
    }
  }

  return 0;
}
    
int scan_archive(gp_archive_type *archive, char *name)
{
  state.symbol.archive = push_symbol_table(NULL, 0);

  /* If necessary, build a symbol index for the archive. */
  if (gp_archive_have_index(archive) == 0) {
    struct symbol_table *archive_tbl = NULL;
  
    archive_tbl = push_symbol_table(NULL, 1);
    gp_archive_make_index(archive, archive_tbl);
    archive = gp_archive_add_index(archive_tbl, archive);
    gp_warning("\"%s\" is missing symbol index", name);
    archive_tbl = pop_symbol_table(archive_tbl);
  }

  /* Read the symbol index */
  gp_archive_read_index(state.symbol.archive, archive);

  /* Scan the symbol index for symbols in the missing symbol table.  If
     found, add the object to state.objects. */
  scan_index(state.symbol.archive, archive);

  state.symbol.archive = pop_symbol_table(state.symbol.archive);

  return 0;
}

/* Remove special symbols from the missing table. These symbols are created
   by the linker. */

static void
remove_special(void)
{
  struct symbol *sym;

  sym = get_symbol(state.symbol.missing, "_cinit");
  if (sym != NULL) {
    gp_link_remove_symbol(state.symbol.missing, "_cinit");
  }

  return;
}

/* Locate the _cinit symbol that the linker created and add it to the 
   symbol table. */

static void
add_cinit(void)
{
  gp_symbol_type *current = state.object->symbols;
  gp_symbol_type *found = NULL;

  while (current != NULL) {
    if ((current->name != NULL) &&
        (strcmp(current->name, "_cinit") == 0) &&
        (current->section_number > 0)) {
      found = current;
      break;
    }
    current = current->next;
  }

  /* If a _cinit symbol definition existings, add it to the symbol table */
  if (found != NULL)
    gp_link_add_symbol(state.symbol.definition, found, NULL);

  return;
}

/* Build the symbol tables.  Determine which objects from the archives are 
   required for linking */
    
void build_tables(void)
{
  gp_object_type *list = state.object;
  struct archivelist *arlist = state.archives;
  int i;
  struct symbol *s;
  char *name;

  /* Create the object file symbol tables */  
  while (list != NULL) {
    gp_link_add_symbols(state.symbol.definition, 
                        state.symbol.missing, 
			list);
    list = list->next;
  }

  /* All of the objects have been scanned.  If there are remaining references
     to symbols, then the archives must contain the missing references. */
  if (count_missing()) {
    while (arlist != NULL) {
      scan_archive(arlist->archive, arlist->name);
      if (count_missing() == 0) {
        /* No more missing references, no need to continue. */
        break;
      }
      arlist = arlist->next;
    }
  }

  remove_special();

  /* All of the archives have been scanned.  If there are still missing
     references, it is an error */
  if (count_missing()) {
    for (i = 0; i < HASH_SIZE; i++) {
      for (s = state.symbol.missing->hash_table[i]; s; s = s->next) {
        name = get_symbol_name(s);
        assert(name != NULL);
        gp_error("missing definition for symbol \"%s\"", name);
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
  
  strcpy(file_name, name);
  
  coff = fopen(file_name, "rb");
  if ((coff == NULL) && (strchr(file_name, PATH_CHAR) == 0)) { 
    /* If no "/" in name, try searching include pathes */
    int i;

    for(i = 0; i < state.numpaths; i++) {
      strcpy(file_name, state.paths[i]);
      strcat(file_name, COPY_CHAR);
      strcat(file_name, name);
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
  printf("  -m, --map                      Output a map file.\n");
  printf("  -o FILE, --output FILE         Alternate name of output file.\n");
  printf("  -q, --quiet                    Quiet.\n");
  printf("  -s FILE, --script FILE         Linker script.\n");
  printf("  -v, --version                  Show version.\n");
  printf("\n");
  #ifdef USE_DEFAULT_PATHS
    #ifdef HAVE_DOS_BASED_FILE_SYSTEM
      printf("Default linker script path %s\n", DOS_LKR_PATH);
    #else
      printf("Default linker script path %s\n", GPLINK_LKR_PATH);
    #endif
  #endif
  printf("\n");    
  printf("Report bugs to:\n");
  printf("%s\n", BUG_REPORT_URL);
  exit(0);
}

#define GET_OPTIONS "?a:cdf:hI:mo:qs:v"

  static struct option longopts[] =
  {
    { "hex-format",  1, 0, 'a' },
    { "object",      0, 0, 'c' },
    { "debug",       0, 0, 'd' },
    { "fill",        1, 0, 'f' },
    { "help",        0, 0, 'h' },
    { "include",     1, 0, 'I' },
    { "map",         0, 0, 'm' },
    { "output",      1, 0, 'o' },
    { "quiet",       0, 0, 'q' },
    { "script",      1, 0, 's' },
    { "version",     0, 0, 'v' },
    { 0, 0, 0, 0 }
  };

#define GETOPT_FUNC getopt_long(argc, argv, GET_OPTIONS, longopts, 0)

int main(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  int usage = 0;
  char *pc;

  /* initialize */
  state.hex_format = inhx32;
  state.numpaths = 0;
  state.processor = no_processor;
  state.codfile = normal;
  state.hexfile = normal;
  state.mapfile = suppress;
  state.objfile = suppress;
  state.fill_enable = 0;
  state.fill_value = 0;
  state.srcfilename = NULL;
  state.object  = NULL;
  state.archives = NULL;

  /* The symbols are case sensitive */
  state.symbol.definition = push_symbol_table(NULL, 0);
  state.symbol.missing = push_symbol_table(NULL, 0);
  state.section.definition = push_symbol_table(NULL, 0);
  state.section.logical = push_symbol_table(NULL, 0);

  #ifdef PARSE_DEBUG
  {
    extern int yydebug;
    yydebug = 1; /* enable parse debug */
  }
  #endif
 
  while ((c = GETOPT_FUNC) != EOF) {
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
      gp_debug_disable = 0;
      break;
    case 'f':
      state.fill_value = strtol(optarg, &pc, 16);
      if ((pc == NULL) || (*pc != '\0')) {
        gp_error("invalid character %#x in number constant", *pc);
      } else if (state.fill_value > 0xffff) {
        gp_error("fill value exceeds 0xffff", *pc);
      } else {
        state.fill_enable = 1;
      }
      break;
    case '?':
    case 'h':
      usage = 1;
      break;
    case 'I':
      gplink_add_path(optarg);
      break;
    case 'm':
      state.mapfile = normal;
      break;
    case 'o':
      strcpy(state.basefilename, optarg);
      pc = strrchr(state.basefilename, '.');
      if (pc)
        *pc = 0;
      break;
    case 'q':
      gp_quiet = 1;
      break;
    case 's':
      state.srcfilename = optarg;
      break;
    case 'v':
      fprintf(stderr, "%s\n", GPLINK_VERSION_STRING);
      exit(0);
    }
    if (usage)
      break;
  }

  if ((state.srcfilename == NULL) &&
      (optind >= argc)) {
    /* No linker script was specified and no object filenames were provided,
       so print the usage */
    usage = 1;
  }

  if (usage) {
    show_usage();
  }

  if(state.basefilename[0] == '\0') {
    /* set default output filename to be a.o, a.hex, a.cod, a.map */
    strcpy(state.basefilename, "a");
  }

  /* setup output filenames */
  strcpy(state.codfilename, state.basefilename);
  strcat(state.codfilename, ".cod");  
  strcpy(state.hexfilename, state.basefilename);
  strcat(state.hexfilename, ".hex");  
  strcpy(state.mapfilename, state.basefilename);
  strcat(state.mapfilename, ".map");  
  strcpy(state.objfilename, state.basefilename);
  strcat(state.objfilename, ".cof");  

  /* Open all objects and archives in the file list. */ 
  for ( ; optind < argc; optind++) {
    gplink_open_coff(argv[optind]);
  }

  /* Read the script */
  if (state.srcfilename != NULL) {
    open_src(state.srcfilename, 0);
    yyparse();
#ifdef USE_DEFAULT_PATHS
  } else if (state.object != NULL) {
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
    #ifdef HAVE_DOS_BASED_FILE_SYSTEM
      strcpy(file_name, DOS_LKR_PATH);    
    #else
      strcpy(file_name, GPLINK_LKR_PATH);
    #endif
    strcat(file_name, COPY_CHAR);
    strcat(file_name, script_name);
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

  /* Construct the symbol tables. Determine which archive members are 
     required to resolve external references.  */
  build_tables();  

  /* relocate the sections */
  gp_cofflink_reloc(state.object, 
                    state.section.definition, 
                    state.section.logical);

  add_cinit();

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

  /* convert the executable object into a cod file */
  write_cod();

  /* write map file */
  make_map();

  i_memory_free(state.i_memory);
  
  if (gp_num_errors > 0)
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;

}

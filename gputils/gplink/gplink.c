/* GNU PIC Linker
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
#include "gpcoff.h"
#include "gplink.h"
#include "scan.h"
#include "gpreadobj.h"
#include "gpsymbol.h"
#include "gparchive.h"
#include "gpcofflink.h"

/* FIXME:  Strip out all of the linker functions and place them in the 
           library.  */

struct gplink_state state = {
    _hex,               /* produce hex file by default */
    inhx8m,		/* hex_format */
    0,			/* quiet */
    0                   /* debug */
};

int yyparse(void);

/* return the number of missing symbols */
int count_missing(void)
{
  return state.symbol.missing->count;
}


/* FIXME: replace these functions with gpmessage.c functions */
void gplink_error(char *messg)
{
  state.num.errors++;
  if (state.quiet != 1)
    printf("error: %s\n", messg); 

  return;
}

void gplink_warning(char *messg)
{
  state.num.warnings++;
  if (state.quiet != 1)
    printf("warning: %s\n", messg); 

  return;
}

void gplink_debug(char *messg)
{
  if ((state.quiet != 1) && (state.debug == 1))
    printf("debug: %s\n", messg);

  return;
}

void object_append(struct objectfile *file, char *name)
{
  struct objectlist *new;

  /* make the new entry */  
  new = (struct objectlist *)malloc(sizeof(*new));
  new->name = strdup(name); 
  new->object = file;
  new->next = NULL;
  
  /* append the entry to the list */
  if (state.objects == NULL) {
    state.objects = new;
  } else {
    struct objectlist *list = state.objects;

    /* Locate the end of the list.  It is critical that the list appears in 
       the same order as the file arguments.  It can have an impact on the
       executable if some information is not specified and has to be 
       inferred. */
    while(list->next != NULL){
      list = list->next;
    }
    list->next = new;
  }

  return;
}

void archive_append(struct coff_archive *file, char *name)
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

int scan_index(struct symbol_table *table, struct coff_archive *archive)
{
  struct symbol *s;
  struct symbol *m;
  struct coff_archive *member;
  struct objectfile *object;
  int i;
  int num_added = 1; /* initalize to 1 so while loop can be entered */
  char *name;
  char buffer[BUFSIZ];
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
	  object = convert_object((unsigned char *)member->file, &buffer[0]);
          if (object == NULL) {
	    gplink_error(&buffer[0]);
	  } else {
            object_append(object, object_name);	
            gp_link_add_symbols(state.symbol.definition, 
	                        state.symbol.missing, 
				object);		
            /* The symbol tables have been modified.  Need to take another
	       pass to make sure we get everything. */
	    num_added++;
	  }      
          free(object_name);
          /* This branch of the table has been modified. Go to the next one */
	  break;
	}      
      }
    }
  }

  return 0;
}
    
int scan_archive(struct coff_archive *archive, char *name)
{
  char buffer[BUFSIZ];

  /* FIXME: are the symbols case insensitive? */
  state.symbol.archive = push_symbol_table(NULL, 1);

  /* If necessary, build a symbol index for the archive. */
  if (gp_archive_have_index(archive) == 0) {
    struct symbol_table *archive_tbl = NULL;
  
    archive_tbl = push_symbol_table(NULL, 1);
    if (gp_archive_make_index(archive, archive_tbl, &buffer[0])) {
      gplink_error(&buffer[0]);
    }
    archive = gp_archive_add_index(archive_tbl, archive, &buffer[0]);
    sprintf(buffer, "\"%s\" is missing symbol index", name);
    gplink_warning(&buffer[0]);
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

/* Build the symbol tables.  Determine which objects from the archives are 
   required for linking */
    
void build_tables(void)
{
  struct objectlist *list = state.objects;
  struct archivelist *arlist = state.archives;

  /* Create the object file symbol tables */  
  while (list != NULL) {
    gp_link_add_symbols(state.symbol.definition, 
                        state.symbol.missing, 
			list->object);
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

  if (count_missing()) {
    /* FIXME: generate an error. Missing definitions for external references */
    exit(1);
  }

  return;
}

/* Read a coff object or archive.  gplink doesn't care about file extensions.
   This allows alternate extensions such as .a archives and .obj coff 
   objects.  */

void gplink_open_coff(char *name)
{
  struct objectfile *object_file;
  struct coff_archive *archive_file;
  char buffer[BUFSIZ];
  
  /* FIXME:  need to add include pathes to this search */

  switch(identify_coff_file(name)) {
  case object:
    /* read the object */  
    object_file = readobj(name, &buffer[0]);
    if (object_file == NULL) {
      gplink_error(&buffer[0]);
    } else {
      object_append(object_file, name);
    }
    break;
  case archive:
    /* read the archive */  
    archive_file = read_coff_archive(name, &buffer[0]);
    if (archive_file == NULL) {
      gplink_error(&buffer[0]);
    } else {
      archive_append(archive_file, name);
    }
    break;
  case sys_err:
    sprintf(&buffer[0], "can't open file \"%s\"",
            name);
    gplink_error(&buffer[0]);     
    break;
  case unknown:
    sprintf(&buffer[0], "\"%s\" is not a valid coff object or archive",
            name);
    gplink_error(&buffer[0]);     
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
    gplink_error("too many -I paths");
  }
}


void show_usage(void)
{
  printf("Usage: gplink <options> [ <object> | <library> ] \n");
  printf("Where <options> are:\n");
  #ifdef HAVE_GETOPT_LONG
  printf("  -a FMT, --hex-format FMT       Select hex file format.\n");
  printf("  -c, --object                   Output executable object file.\n");
  printf("  -h, --help                     Show this usage message.\n");
  printf("  -I DIR, --include DIR          Specify include directory.\n");
  printf("  -o FILE, --output FILE         Alternate name of hex file.\n");
  printf("  -q, --quiet                    Quiet.\n");
  printf("  -s FILE, --script FILE         Linker script.\n");
  printf("  -v, --version                  Show version.\n");
  #else
  printf("  -a FMT      Select hex file format.\n");
  printf("  -c          Output executable object file.\n");
  printf("  -h          Show this usage message.\n");
  printf("  -I DIR      Specify include directory.\n");
  printf("  -o FILE     Alternate name of output file.\n");
  printf("  -q          Quiet.\n");
  printf("  -s FILE     Linker script.\n");
  printf("  -v          Show version.\n");
  #endif
  printf("\n");
  printf("Report bugs to:\n");
  printf("%s\n", BUG_REPORT_URL);
  exit(0);
}

#define GET_OPTIONS "?a:cdhI:o:qs:v"

#ifdef HAVE_GETOPT_LONG

  static struct option longopts[] =
  {
    { "hex-format",  1, 0, 'a' },
    { "object",      0, 0, 'c' },
    { "debug",       0, 0, 'd' },
    { "help",        0, 0, 'h' },
    { "include",     1, 0, 'I' },
    { "output",      1, 0, 'o' },
    { "quiet",       0, 0, 'q' },
    { "script",      1, 0, 'r' },
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
  int usage = 0;

  /* initialize */
  state.srcfilename = NULL;
  state.objects  = NULL;
  state.archives = NULL;

  /* FIXME: are the symbols case insensitive? */
  state.symbol.definition = push_symbol_table(NULL, 1);
  state.symbol.missing = push_symbol_table(NULL, 1);
  state.section.definition = push_symbol_table(NULL, 1);
  state.section.logical = push_symbol_table(NULL, 1);

  #ifdef PARSE_DEBUG
  {
    extern int yydebug;
    yydebug = 1; /* enable parse debug */
  }
  #endif

 
  while ((c = GETOPT_FUNC) != EOF) {
    switch (c) {
    case 'a':

      break;
    case 'c':
      state.mode = _object;
      break;
    case 'd':
      state.debug = 1;
      break;
    case '?':
    case 'h':
      usage = 1;
      break;
    case 'I':
      gplink_add_path(optarg);
      break;
    case 'o':
      state.basefilename = optarg;
      break;
    case 'q':
      state.quiet = 1;
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

  /* FIXME: Remove this test, scripts can also specify the files. Later in
    check that there is at least one object name passed to linker */
  if (optind >= argc) {
    usage = 1;
  }

  if (usage) {
    show_usage();
  }

  /* Open all objects and archives in the file list. */ 
  for ( ; optind < argc; optind++) {
    gplink_open_coff(argv[optind]);
  }

  /* Read the script */
  if (state.srcfilename != NULL) {
    open_src(state.srcfilename, 0);
    yyparse();
  } else {
    /* FIXME: Maybe add a default script so a user supplied one isn't always 
       necessary. */ 
    gplink_error("linker command file not specified");
  }

  /* Construct the symbol tables. Determine which archive members are 
     required to resolve external references.  */
  build_tables();  

  /* relocate sections */
  gp_link_reloc(state.objects, 
                state.section.definition,
	        state.section.logical);
	
  /* patch raw data with the relocated symbol values */
  gp_link_patch(state.objects,
                state.symbol.definition); 

  /* create the output object file */
  state.output = gp_link_combine(state.objects);

  /* write output file */
  if (state.mode == _object) {
    /* write the executable object in memory */

  } else if (state.mode == _hex) {
    /* convert the executable object into a hex file */


    /* convert the executable object into a cod file */


  } else {
    assert(0);
  }

  if (state.num.errors > 0)
    return 1;
  else
    return 0;

}

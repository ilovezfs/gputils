/* GNU PIC Librarian
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

#include "gplib.h"
#include "gpsymbol.h"
#include "gparchive.h"
#include "gpcofflink.h"

/* FIXME:  It seems that Mchip lib files do not use a symbol table as the
first entry in the library.  Mplib does check to make sure the same symbol
name doesn't appear more than once.  For now, gplib will do the same.  In
the future, a symbol table should help reduce the link time */ 

/* FIXME:  check for duplicate symbols */

struct gplib_state state = {
  ar_null,            /* default mode, do nothing */              
  { 0 ,0 },           /* set the error counters to zero */
  0,                  /* number of objects */
  0                   /* quiet */
};

struct symbol_table *definition_tbl = NULL;
struct symbol_table *symbol_index = NULL;


/* FIXME: replace with functions from gpmessage.c */
void gplib_error(char *messg)
{
  state.num.errors++;
  if (state.quiet != 1)
    printf("error: %s\n", messg); 

  return;
}

void gplib_warning(char *messg)
{
  state.num.warnings++;
  if (state.quiet != 1)
    printf("warning: %s\n", messg); 

  return;
}

void select_mode(enum lib_modes mode)
{
  if (state.mode == ar_null) { 
    state.mode = mode;
  } else {
    gplib_error("multiple library operations selected");
  }
}

void show_usage(void)
{
  printf("Usage: gplib <options> library <member>\n");
  printf("Where <options> are:\n");
  #ifdef HAVE_GETOPT_LONG
  printf("  -c, --create               Create a new library \n");
  printf("  -d, --delete               Delete member from library \n");
  printf("  -h, --help                 Show this usage message \n");
  printf("  -n, --no-index             Don't add symbol index \n");
  printf("  -q, --quiet                Quiet mode \n");
  printf("  -r, --replace              Add or replace member from library \n");
  printf("  -s, --symbols              List global symbols in library \n");
  printf("  -t, --list                 List members in library \n");
  printf("  -v, --version              Show version \n");
  printf("  -x, --extract              Extract member from library \n");
  #else
  printf("  -c             Create a new library \n");
  printf("  -d             Delete member from library \n");
  printf("  -h             Show this usage message \n");
  printf("  -n             Don't add symbol index \n");
  printf("  -q             Quiet mode \n");
  printf("  -r             Add or replace member from library \n");
  printf("  -s             List global symbols in library \n");
  printf("  -t             List members in library \n");
  printf("  -v             Show version \n");
  printf("  -x             Extract member from library \n");
  #endif
  printf("\n");
  printf("Report bugs to:\n");
  printf("%s\n", BUG_REPORT_URL);
  exit(0);
}

#define GET_OPTIONS "?cdhnqrstvx"

#ifdef HAVE_GETOPT_LONG

  static struct option longopts[] =
  {
    { "create",      0, 0, 'c' },
    { "delete",      0, 0, 'd' },
    { "extract",     0, 0, 'x' },
    { "help",        0, 0, 'h' },
    { "no-index",    0, 0, 'n' },
    { "quiet",       0, 0, 'q' },
    { "replace",     0, 0, 'r' },
    { "symbols",     0, 0, 's' },
    { "list",        0, 0, 't' },
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
  int i = 0;
  int c;
  int usage = 0;
  int update_archive = 0;
  char buffer[BUFSIZ];
  int  no_index = 0;

  /* FIXME: are the symbols case insensitive? */
  definition_tbl = push_symbol_table(NULL, 1);
  symbol_index = push_symbol_table(NULL, 1);

  while ((c = GETOPT_FUNC) != EOF) {
    switch (c) {
    case '?':
    case 'h':
      usage = 1;
      break;
    case 'c':
      select_mode(ar_create);
      break;
    case 'd':
      select_mode(ar_delete);
      break;
    case 'n':
      no_index = 1;
      break;
    case 'q':
      state.quiet = 1;
      break;
    case 'r':
      select_mode(ar_replace);
      break;
    case 's':
      select_mode(ar_symbols);
      break;
    case 't':
      select_mode(ar_list);
      break;
    case 'v':
      fprintf(stderr, "%s\n", GPLIB_VERSION_STRING);
      exit(0);
      break;
    case 'x':
      select_mode(ar_extract);
      break;
    }
    if (usage)
      break;
  }

  if (optind < argc) {
    /* fetch the library name */
    state.filename = argv[optind];
    optind++;
    /* some operations require object filenames or membernames */
    for ( ; optind < argc; optind++) {
      state.objectname[state.numobjects] = argv[optind];
      if (state.numobjects >= MAX_NAMES) {
        gplib_error("exceeded maximum number of object files");
        break;
      }
      state.numobjects++;
    }
  } else {
    usage = 1;
  }

  /* User did not select an operation */
  if (state.mode == ar_null)
    usage = 1;

  /* User did not provide object names */
  if ((state.mode != ar_list) && (state.mode != ar_symbols) && 
      (state.numobjects == 0))
    usage = 1;

  if (usage) {
    show_usage();
  }

  /* if we are not creating a new archive, we have to read an existing one */
  if (state.mode != ar_create) {
    state.archive = read_coff_archive(state.filename, &buffer[0]);
    if (state.archive == NULL) {
      gplib_error(&buffer[0]);
      return 1;
    }
  }

  /* FIXME: Process the options when argv's read, like gplink does.  */
    
  /* process the option */
  i = 0;
  switch (state.mode) {
  case ar_create: 
    while (i < state.numobjects) {
      state.archive = add_archive_member(state.archive, 
                                         state.objectname[i],
                                         &buffer[0]);
      if (state.archive == NULL) {
        gplib_error(&buffer[0]);
        break;      
      }
      i++;
    }
    update_archive = 1;
    break;

  case ar_delete: 
    while (i < state.numobjects) {
      state.archive = delete_archive_member(state.archive, 
                                            state.objectname[i],
                                            &buffer[0]);
      if (state.archive == NULL) {
        gplib_error(&buffer[0]);
        break;      
      }
      i++;
    }
    update_archive = 1;
    break;

  case ar_extract:
    while (i < state.numobjects) {
      if (extract_archive_member(state.archive, 
                                 state.objectname[i],
                                 &buffer[0])) {
        gplib_error(&buffer[0]);
        break;
      }
      i++;
    } 
    break;

  case ar_list:
    list_archive_members(state.archive);
    break;

  case ar_replace:
    while (i < state.numobjects) {
      state.archive = add_archive_member(state.archive, 
                                         state.objectname[i],
                                         &buffer[0]);
      if (state.archive == NULL) {
        gplib_error(&buffer[0]);
        break;      
      }
      i++;
    }
    update_archive = 1;
    break; 

  case ar_symbols:
    gp_archive_read_index(symbol_index, state.archive);
    gp_archive_print_table(symbol_index);
    break;

  case ar_null:
  default:
    assert(0);
  }

  /* If the archive is being modified remove the old symbol index */
  if (update_archive == 1) {
    state.archive = gp_archive_remove_index(state.archive);
  }

  /* check for duplicate symbols */
  if (gp_archive_make_index(state.archive, definition_tbl, &buffer[0])) {
    gplib_error(&buffer[0]);
  }

  /* add the symbol index to the archive */      
  if ((update_archive == 1) && (no_index == 0)) {
      state.archive = gp_archive_add_index(definition_tbl, 
                                           state.archive, 
					   &buffer[0]);
  }

  /* write the new or modified archive */
  if ((update_archive == 1) && (state.num.errors == 0)) {
    if (write_archive(state.archive, state.filename, &buffer[0]))
      gplib_error(&buffer[0]);
  }

  if (state.num.errors > 0)
    return 1;
  else
    return 0;

}

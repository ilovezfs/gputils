/* GNU PIC Librarian
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
#include "gplib.h"

struct gplib_state state = {
  .mode       = AR_NULL,            /* default mode, do nothing */
  .numobjects = 0,                  /* number of objects */
};

struct symbol_table *definition_tbl = NULL;
struct symbol_table *symbol_index = NULL;

void select_mode(enum lib_modes mode)
{
  if (state.mode == AR_NULL) {
    state.mode = mode;
  } else {
    gp_error("multiple library operations selected");
  }
}

/* return the object name without the path */

static char *
object_name(char *file_name)
{
  char *name;

#ifdef HAVE_DOS_BASED_FILE_SYSTEM
  for (name = file_name + strlen(file_name) - 1; name >= file_name; --name) {
    if ((*name == UNIX_PATH_CHAR) || (*name == PATH_CHAR)) {
      return ++name;
    }
  }

  return file_name;
#else
  name = strrchr(file_name, PATH_CHAR);
  if (name != NULL) {
    return ++name;
  } else {
    return file_name;
  }
#endif
}

static gp_boolean
has_path(const char *file_name)
{
  char *name;

  name = strrchr(file_name, PATH_CHAR);
#ifdef HAVE_DOS_BASED_FILE_SYSTEM
  if (name != NULL) {
    name = strrchr(file_name, UNIX_PATH_CHAR);
  }
#endif

  return ((name == NULL) ? false : true);
}

void show_usage(void)
{
  printf("Usage: gplib [options] library [member]\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -c, --create       Create a new library.\n");
  printf("  -d, --delete       Delete member from library.\n");
  printf("  -h, --help         Show this usage message.\n");
  printf("  -n, --no-index     Don't add symbol index.\n");
  printf("  -q, --quiet        Quiet mode.\n");
  printf("  -r, --replace      Add or replace member from library.\n");
  printf("  -s, --symbols      List global symbols in library.\n");
  printf("  -t, --list         List members in library.\n");
  printf("  -v, --version      Show version.\n");
  printf("  -x, --extract      Extract member from library.\n\n");
  printf("Report bugs to:\n");
  printf("%s\n", PACKAGE_BUGREPORT);
  exit(0);
}

#define GET_OPTIONS "?cdhnqrstvx"

static struct option longopts[] =
{
  { "create",   no_argument, NULL, 'c' },
  { "delete",   no_argument, NULL, 'd' },
  { "extract",  no_argument, NULL, 'x' },
  { "help",     no_argument, NULL, 'h' },
  { "no-index", no_argument, NULL, 'n' },
  { "quiet",    no_argument, NULL, 'q' },
  { "replace",  no_argument, NULL, 'r' },
  { "symbols",  no_argument, NULL, 's' },
  { "list",     no_argument, NULL, 't' },
  { "version",  no_argument, NULL, 'v' },
  { NULL,       no_argument, NULL, '\0'}
};

int main(int argc, char *argv[])
{
  int c;
  int i;
  gp_boolean usage          = false;
  gp_boolean update_archive = false;
  gp_boolean no_index       = false;
  gp_archive_type *object = NULL;

  gp_init();

  /* symbols are case sensitive */
  definition_tbl = push_symbol_table(NULL, false);
  symbol_index = push_symbol_table(NULL, false);

  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, NULL)) != EOF) {
    switch (c) {
    case '?':
    case 'h':
      usage = true;
      break;

    case 'c':
      select_mode(AR_CREATE);
      break;

    case 'd':
      select_mode(AR_DELETE);
      break;

    case 'n':
      no_index = true;
      break;

    case 'q':
      gp_quiet = true;
      break;

    case 'r':
      select_mode(AR_REPLACE);
      break;

    case 's':
      select_mode(AR_SYMBOLS);
      break;

    case 't':
      select_mode(AR_LIST);
      break;

    case 'v':
      fprintf(stderr, "%s\n", GPLIB_VERSION_STRING);
      exit(0);
      break;

    case 'x':
      select_mode(AR_EXTRACT);
      break;
    }

    if (usage)
      break;
  }

  if (optind < argc) {
    /* fetch the library name */
    state.filename = argv[optind++];
    /* some operations require object filenames or membernames */
    for ( ; optind < argc; optind++) {
      state.objectname[state.numobjects] = argv[optind];
      if (state.numobjects >= MAX_OBJ_NAMES) {
        gp_error("exceeded maximum number of object files");
        break;
      }
      state.numobjects++;
    }
  } else {
    usage = true;
  }

  /* User did not select an operation */
  if (state.mode == AR_NULL) {
    usage = true;
  }

  /* User did not provide object names */
  if ((state.mode != AR_LIST) &&
      (state.mode != AR_SYMBOLS) &&
      (state.numobjects == 0)) {
    usage = true;
  }

  if (usage) {
    show_usage();
  }

  /* if we are not creating a new archive, we have to read an existing one */
  if (state.mode != AR_CREATE) {
    if (gp_identify_coff_file(state.filename) != GP_COFF_ARCHIVE) {
      gp_error("\"%s\" is not a valid archive file", state.filename);
      exit(1);
    } else {
      state.archive = gp_archive_read(state.filename);
    }
  }

  /* process the option */
  i = 0;
  switch (state.mode) {
  case AR_CREATE:
  case AR_REPLACE:
    while (i < state.numobjects) {
      if ((gp_identify_coff_file(state.objectname[i]) != GP_COFF_OBJECT_V2) &&
          (gp_identify_coff_file(state.objectname[i]) != GP_COFF_OBJECT)) {
        gp_error("\"%s\" is not a valid object file", state.objectname[i]);
        break;
      } else {
        state.archive = gp_archive_add_member(state.archive,
                                              state.objectname[i],
                                              object_name(state.objectname[i]));
      }
      i++;
    }
    update_archive = true;
    break;

  case AR_DELETE:
    while (i < state.numobjects) {
      if (has_path(state.objectname[i])) {
        gp_error("invalid object name \"%s\"", state.objectname[i]);
        break;
      }
      object = gp_archive_find_member(state.archive, state.objectname[i]);
      if (object == NULL) {
        gp_error("object \"%s\" not found", state.objectname[i]);
        break;
      } else {
        state.archive = gp_archive_delete_member(state.archive,
                                                 state.objectname[i]);
      }
      i++;
    }
    update_archive = true;
    break;

  case AR_EXTRACT:
    while (i < state.numobjects) {
      if (has_path(state.objectname[i])) {
        gp_error("invalid object name \"%s\"", state.objectname[i]);
        break;
      }
      object = gp_archive_find_member(state.archive, state.objectname[i]);
      if (object == NULL) {
        gp_error("object \"%s\" not found", state.objectname[i]);
        break;
      } else {
        if (gp_archive_extract_member(state.archive, state.objectname[i])) {
          gp_error("can't write file \"%s\"", state.objectname[i]);
          break;
        }
      }
      i++;
    }
    break;

  case AR_LIST:
    gp_archive_list_members(state.archive);
    break;

  case AR_SYMBOLS:
    if (gp_archive_have_index(state.archive) == 0) {
      gp_error("this archive has no symbol index");
    } else {
      gp_archive_read_index(symbol_index, state.archive);
      gp_archive_print_table(symbol_index);
    }
    break;

  case AR_NULL:
  default:
    assert(0);
  }

  /* If the archive is being modified remove the old symbol index */
  if (update_archive) {
    state.archive = gp_archive_remove_index(state.archive);
  }

  /* check for duplicate symbols */
  gp_archive_make_index(state.archive, definition_tbl);

  /* add the symbol index to the archive */
  if (update_archive && (!no_index)) {
    state.archive = gp_archive_add_index(definition_tbl, state.archive);
  }

  /* write the new or modified archive */
  if (update_archive && (gp_num_errors == 0)) {
    if (gp_archive_write(state.archive, state.filename)) {
      gp_error("can't write the new archive file");
    }
  }

  return ((gp_num_errors > 0) ? EXIT_FAILURE : EXIT_SUCCESS);
}

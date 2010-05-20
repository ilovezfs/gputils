/* Map file generation
   Copyright (C) 2003, 2004, 2005
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
#include "map.h"

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

#define SECTION_UNKNOWN 0 
#define SECTION_ROMDATA 1 
#define SECTION_CODE    2 
#define SECTION_IDATA   3 
#define SECTION_UDATA   4 

/* FIXME: Sort symbols by name and size. */ 

void 
map_line(const char *format, ...)
{
  va_list args;
  char buffer[BUFSIZ]; 

  if (state.map.f == NULL)
    return;

  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  fprintf(state.map.f, "%s\n", buffer);

  return;
}

static int
_section_value(gp_section_type *section)
{
  int value = 0;

  if ((section->flags & STYP_TEXT) || 
      (section->flags & STYP_DATA_ROM))  {
    value = SECTION_CODE;
  } else if (section->flags & STYP_DATA) {
    value = SECTION_IDATA;
  } else if ((section->flags & STYP_BSS) ||
             (section->flags & STYP_OVERLAY)) {
    value = SECTION_UDATA;
  } else if (section->flags & STYP_DATA_ROM) {
    value = SECTION_ROMDATA;
  } else {
    value = SECTION_UNKNOWN;
  }

  return value;
}

static int
compare_sections(const void *a, const void *b)
{
  gp_section_type *section_a = *((gp_section_type **)a);
  gp_section_type *section_b = *((gp_section_type **)b);
  int value_a = _section_value(section_a);
  int value_b = _section_value(section_b);

  if (value_a < value_b)
    return -1;

  if (value_a > value_b)
    return +1;

  if (section_a->address < section_b->address)
    return -1;

  if (section_a->address > section_b->address)
    return +1;

  return 0;
}

static void
_write_sections(void)
{
  gp_section_type *section = NULL;
  gp_section_type **section_list = NULL;
  char *type;
  char *location;
  long i;

  /* Some malloc implementations return NULL for malloc(0) */
  section_list = malloc(sizeof(gp_section_type *) * state.object->num_sections);
  if (!section_list && state.object->num_sections > 0) {
    fprintf(stderr, "error: out of memory\n");
    exit(1);
  }

  section = state.object->sections;
  for (i = 0; i < state.object->num_sections; i++) {
    assert(section != NULL);
    section_list[i] = section;
    section = section->next;
  }

  qsort((void *)section_list,
        state.object->num_sections,
        sizeof(gp_section_type *),
        compare_sections);

  map_line("                                 Section Info");
  map_line("                  Section       Type    Address   Location Size(Bytes)");
  map_line("                ---------  ---------  ---------  ---------  ---------");

  for (i = 0; i < state.object->num_sections; i++) {
    int org_to_byte_shift = state.class->org_to_byte_shift;
    section = section_list[i];
    switch (_section_value(section)) {
    case SECTION_ROMDATA:
      type = "romdata";
      break;
    case SECTION_CODE:
      type = "code";
      break;
    case SECTION_IDATA:
      type = "idata";
      break;
    case SECTION_UDATA:
      type = "udata";
      break;
    case SECTION_UNKNOWN:
    default:
      type = "UNKNOWN";
    }

    if ((section->flags & STYP_TEXT) || 
        (section->flags & STYP_DATA_ROM))  {
      location = "program";
    } else {
      location = "data";
      org_to_byte_shift = 0;
    }

    assert(section->name != NULL);
    
    if (section->size != 0) {
      map_line("%25s %10s   %#08x %10s   %#08x",
               section->name,
               type,
               gp_byte_to_org(org_to_byte_shift, section->address),
               location,
               section->size);
    }
  }
  map_line(" ");

  free(section_list);
}

static void
_write_program_memory(void)
{
  gp_section_type *section = NULL;
  int prog_size = 0;

  map_line("                              Program Memory Usage");
  map_line("                               Start         End");
  map_line("                           ---------   ---------");
  section = state.object->sections;

  while (section != NULL) {
    if (((section->flags & STYP_TEXT) || 
         (section->flags & STYP_DATA_ROM)) && (section->size != 0)) {
      map_line("                            %#08x    %#08x",
               gp_processor_byte_to_org(state.class, section->address),
               gp_processor_byte_to_org(state.class, section->address + section->size - 1));
      prog_size += section->size;
    }
    section = section->next;  
  }
  map_line(" ");
  map_line("                            %i program addresses used",
           prog_size >> 1);
  map_line(" ");
}

struct file_stack {
  gp_symbol_type *symbol;
  struct file_stack *previous;
};

static struct file_stack *
push_file(struct file_stack *stack, gp_symbol_type *symbol)
{
  struct file_stack *new;
  
  /* allocate memory for the new stack */
  new = (struct file_stack *)malloc(sizeof(*new));

  new->previous = stack;
  new->symbol = symbol;

  return new;
}

static struct file_stack *
pop_file(struct file_stack *stack)
{
  struct file_stack *old;

  if (stack) {
    old = stack;
    stack = stack->previous;
    free(old);
  }

  return stack;
}

static void
_write_symbols(void)
{
  struct file_stack *stack = NULL;
  gp_symbol_type *file = NULL;
  gp_symbol_type *symbol = NULL;
  char *location;
  char *storage;
  char *file_name;

  map_line("                              Symbols");
  map_line("                     Name    Address   Location    Storage File");
  map_line("                ---------  ---------  ---------  --------- ---------");

  symbol = state.object->symbols;

  while (symbol != NULL) {
    if (symbol->class == C_FILE) {
      stack = push_file(stack, symbol);
    } else if (symbol->class == C_EOF) {
      stack = pop_file(stack);
    } else if ((symbol->section_number > 0) && 
               (symbol->class != C_SECTION)) {
      int org_to_byte_shift = state.class->org_to_byte_shift;
      if (stack == NULL) {
        /* the symbol is not between a .file/.eof pair */
        file_name = " ";
      } else {
        file = stack->symbol;
        assert(file != NULL);
        assert(file->aux_list != NULL);
        file_name = file->aux_list->_aux_symbol._aux_file.filename;
      }
      
      assert(symbol->section != NULL);
      assert(symbol->name != NULL);
      
      if ((symbol->section->flags & STYP_TEXT) ||
          (symbol->section->flags & STYP_DATA_ROM)) {
        location = "program";
      } else {
        location = "data";
	org_to_byte_shift = 0;
      }

      if (symbol->class == C_EXT) {
        storage = "extern";
      } else {
        storage = "static";
      }

      map_line("%25s   %#08x %10s %10s %s",
               symbol->name,
               gp_byte_to_org(org_to_byte_shift, symbol->value),
               location,
               storage,
               file_name);
  
    }
    symbol = symbol->next;  
  }
  map_line(" ");
}


void 
make_map(void)
{
  if ((gp_num_errors) || (state.mapfile == suppress)) {
    unlink(state.mapfilename);
    return;
  }

  state.map.f = fopen(state.mapfilename, "wt");
  if (state.map.f == NULL) {
    perror(state.mapfilename);
    exit(1);
  }

  map_line("%s", GPLINK_VERSION_STRING);
  map_line("Map File - Created %s", state.startdate);
  map_line(" ");
  
  /* write sections */
  _write_sections();

  /* program memory usage */
  _write_program_memory();

  /* write symbols */
  _write_symbols();

  fclose(state.map.f);

}

/* Map file generation
   Copyright (C) 2003
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

/* FIXME: Sort sections so program sections are grouped and data sections are 
          grouped. */ 
/* FIXME: Sort program memory so it is semi-contiguous. */ 
/* FIXME: Sort symbols by name and size. */ 

void 
map_line(const char *format, ...)
{
  va_list args;
  char buffer[BUFSIZ]; 

  if (state.map.f == NULL)
    return;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  fprintf(state.map.f, "%s\n", buffer);

  return;
}

static void
_write_sections(void)
{
  gp_section_type *section = NULL;
  char *type;
  char *location;

  map_line("                                 Section Info");
  map_line("                  Section       Type    Address   Location Size(Bytes)");
  map_line("                ---------  ---------  ---------  ---------  ---------");
  section = state.object->sections;

  while (section != NULL) {
    if (section->flags & STYP_TEXT) {
      type = "code";
    } else if (section->flags & STYP_DATA) {
      type = "idata";
    } else if ((section->flags & STYP_BSS) ||
               (section->flags & STYP_OVERLAY)) {
      type = "udata";
    } else if (section->flags & STYP_DATA_ROM) {
      type = "romdata";
    } else {
      type = "UNKNOWN";
    }

    if (section->flags & STYP_TEXT) {
      location = "program";
    } else {
      location = "data";
    }

    assert(section->name != NULL);
    
    map_line("%25s %10s   %#08x %10s   %#08x",
             section->name,
             type,
             section->address,
             location,
             section->size);

    section = section->next;  
  }
  map_line(" ");

  return;
}

static void
_write_program_memory(void)
{
  gp_section_type *section = NULL;
  int size = 0;

  map_line("                              Program Memory Usage");
  map_line("                               Start         End");
  map_line("                           ---------   ---------");
  section = state.object->sections;

  while (section != NULL) {
    if (section->flags & STYP_TEXT) {
      map_line("                            %#08x    %#08x",
               section->address,
               section->address + (section->size >> 1));
      size += (section->size >> 1);
    }
    section = section->next;  
  }
  map_line(" ");
  map_line("                            %i program addresses used",
           size);
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

  old = stack;
  stack = stack->previous;
  free(old);

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
      file = stack->symbol;
      
      assert(file != NULL);
      assert(file->aux_list != NULL);
      assert(symbol->section != NULL);
      
      if (symbol->section->flags & STYP_TEXT) {
        location = "program";
      } else {
        location = "data";
      }

      if (symbol->class == C_EXT) {
        storage = "extern";
      } else {
        storage = "static";
      }

      assert(symbol->name != NULL);
      assert(file->aux_list->_aux_symbol._aux_file.filename != NULL);

      map_line("%25s   %#08x %10s %10s %s",
               symbol->name,
               symbol->value,
               location,
               storage,
               file->aux_list->_aux_symbol._aux_file.filename);
  
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

  /* Determine start date */
  gp_data_string(state.map.startdate);

  map_line("%s", GPLINK_VERSION_STRING);
  map_line("Map File - Created %s", state.map.startdate);
  map_line(" ");
  
  /* write sections */
  _write_sections();

  /* program memory usage */
  _write_program_memory();

  /* write symbols */
  _write_symbols();

  fclose(state.map.f);

}

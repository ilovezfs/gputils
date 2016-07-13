/* Map file generation
   Copyright (C) 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2016 Molnar Karoly <molnarkaroly@users.sf.net>

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

struct syms_s {
  gp_symbol_t *symbol;
  gp_symbol_t *file;
};

struct file_stack {
  gp_symbol_t       *symbol;
  struct file_stack *previous;
};

/*------------------------------------------------------------------------------------------------*/

static void
_map_line(const char *format, ...)
{
  va_list args;

  if (state.map.f != NULL) {
    if (format != NULL) {
      va_start(args, format);
      vfprintf(state.map.f, format, args);
      va_end(args);
    }
    putc('\n', state.map.f);
  }
}

/*------------------------------------------------------------------------------------------------*/

static unsigned int
_section_value(gp_section_t *section)
{
  unsigned int value = 0;

  if (section->flags & STYP_TEXT) {
    value = SECTION_CODE;
  }
  else if (section->flags & STYP_DATA) {
    value = SECTION_IDATA;
  }
  else if (section->flags & (STYP_BSS | STYP_OVERLAY)) {
    value = SECTION_UDATA;
  }
  else if (section->flags & STYP_DATA_ROM) {
    value = SECTION_ROMDATA;
  }
  else {
    value = SECTION_UNKNOWN;
  }

  return value;
}

/*------------------------------------------------------------------------------------------------*/

static int
_cmp_sections(const void *a, const void *b)
{
  gp_section_t *section_a = *((gp_section_t **)a);
  gp_section_t *section_b = *((gp_section_t **)b);
  unsigned int  value_a   = _section_value(section_a);
  unsigned int  value_b   = _section_value(section_b);

  if (value_a < value_b) {
    return -1;
  }

  if (value_a > value_b) {
    return  1;
  }

  if (section_a->address < section_b->address) {
    return -1;
  }

  if (section_a->address > section_b->address) {
    return  1;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

static void
_write_sections(void)
{
  gp_section_t  *section = NULL;
  gp_section_t **section_list = NULL;
  char          *type;
  char          *location;
  unsigned int   i;
  unsigned int   num_sections;
  int            org_to_byte_shift;

  num_sections = state.object->section_list.num_nodes;
  section_list = GP_Malloc(sizeof(gp_section_t *) * num_sections);

  section = state.object->section_list.first;
  for (i = 0; i < num_sections; i++) {
    assert(section != NULL);
    section_list[i] = section;
    section         = section->next;
  }

  qsort((void *)section_list, num_sections, sizeof(gp_section_t *), _cmp_sections);

  _map_line("                                 Section Info");
  _map_line("                  Section       Type    Address   Location Size(Bytes)");
  _map_line("                ---------  ---------  ---------  ---------  ---------");

  for (i = 0; i < num_sections; i++) {
    org_to_byte_shift = state.class->org_to_byte_shift;
    section           = section_list[i];
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

    if (section->flags & STYP_ROM_AREA) {
      location = "program";
    }
    else {
      location = "data";
      org_to_byte_shift = 0;
    }

    assert(section->name != NULL);

    if (section->size != 0) {
      _map_line("%25s %10s   0x%06x %10s   0x%06x", section->name, type,
                gp_byte_to_org(org_to_byte_shift, section->address), location, section->size);
    }
  }

  free(section_list);

  _map_line(NULL);
  _map_line(NULL);
  _map_line(NULL);
}

/*------------------------------------------------------------------------------------------------*/

static void
_write_program_memory(void)
{
  gp_section_t *section = NULL;
  int           prog_size = 0;

  _map_line("                              Program Memory Usage");
  _map_line("                               Start         End");
  _map_line("                           ---------   ---------");
  section = state.object->section_list.first;

  while (section != NULL) {
    if ((section->flags & STYP_ROM_AREA) && (section->size > 0)) {
      _map_line("                            0x%06x    0x%06x",
                gp_processor_byte_to_org(state.class, section->address),
                gp_processor_byte_to_org(state.class, section->address + section->size - 1));
      prog_size += section->size;
    }
    section = section->next;
  }
  _map_line("                            %d program addresses used",
            gp_processor_byte_to_org(state.class, prog_size));
/*
  _map_line("                            %d out of %d program addresses used, program memory utilization is %d%%",
            gp_processor_byte_to_org(state.class, prog_size),
            state.processor->prog_mem_size, state.processor->prog_mem_size * 100 / prog_size);
*/
  _map_line(NULL);
  _map_line(NULL);
  _map_line(NULL);
}

/*------------------------------------------------------------------------------------------------*/

static struct file_stack *
_push_file(struct file_stack *stack, gp_symbol_t *symbol)
{
  struct file_stack *new;

  /* allocate memory for the new stack */
  new = (struct file_stack *)GP_Malloc(sizeof(*new));

  new->previous = stack;
  new->symbol   = symbol;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

static struct file_stack *
_pop_file(struct file_stack *stack)
{
  struct file_stack *old;

  if (stack != NULL) {
    old   = stack;
    stack = stack->previous;
    free(old);
  }

  return stack;
}

/*------------------------------------------------------------------------------------------------*/

static int
_cmp_name(const void *p1, const void *p2)
{
  return strcmp(((struct syms_s *)p1)->symbol->name, ((struct syms_s *)p2)->symbol->name);
}

/*------------------------------------------------------------------------------------------------*/

static int
_cmp_address(const void *p1, const void *p2)
{
  return ((struct syms_s *)p1)->symbol->value - ((struct syms_s *)p2)->symbol->value;
}

/*------------------------------------------------------------------------------------------------*/

static void
_write_symbols(void)
{
  struct syms_s     *syms;
  const gp_symbol_t *sm;
  int                num_syms;
  int                i;
  struct file_stack *stack = NULL;
  gp_symbol_t       *symbol = NULL;

  syms = GP_Malloc(sizeof(struct syms_s) * state.object->num_symbols);

  num_syms = 0;
  symbol   = state.object->symbol_list.first;
  while (symbol != NULL) {
    if (symbol->class == C_FILE) {
      stack = _push_file(stack, symbol);
    }
    else if (symbol->class == C_EOF) {
      stack = _pop_file(stack);
    }
    else if ((symbol->section_number > N_UNDEF) && (symbol->class != C_SECTION)) {
      if (stack == NULL) {
        /* the symbol is not between a .file/.eof pair */
        syms[num_syms].file = NULL;
      }
      else {
        syms[num_syms].file = stack->symbol;
        assert(syms[num_syms].file != NULL);
        assert(syms[num_syms].file->aux_list.first != NULL);
      }

      assert(symbol->section != NULL);
      assert(symbol->name != NULL);

      syms[num_syms].symbol = symbol;
      ++num_syms;
      assert(num_syms <= state.object->num_symbols);
    }
    symbol = symbol->next;
  }

  qsort(syms, num_syms, sizeof(struct syms_s), _cmp_name);

  _map_line("                              Symbols - Sorted by Name");
  _map_line("                     Name    Address   Location    Storage File");
  _map_line("                ---------  ---------  ---------  --------- ---------");

  for (i = 0; i < num_syms; ++i) {
    sm = syms[i].symbol;
    _map_line("%25s   0x%06x %10s %10s %s", sm->name, sm->value,
              (sm->section->flags & STYP_ROM_AREA) ? "program" : "data",
              (sm->class == C_EXT) ? "extern" : "static",
              (syms[i].file != NULL) ? syms[i].file->aux_list.first->_aux_symbol._aux_file.filename : "");
  }
  _map_line(NULL);
  _map_line(NULL);
  _map_line(NULL);

  qsort(syms, num_syms, sizeof(struct syms_s), _cmp_address);

  _map_line("                              Symbols - Sorted by Address");
  _map_line("                     Name    Address   Location    Storage File");
  _map_line("                ---------  ---------  ---------  --------- ---------");

  for (i = 0; i < num_syms; ++i) {
    sm = syms[i].symbol;
    _map_line("%25s   0x%06x %10s %10s %s", sm->name, sm->value,
              (sm->section->flags & STYP_ROM_AREA) ? "program" : "data",
              (sm->class == C_EXT) ? "extern" : "static",
              (syms[i].file != NULL) ? syms[i].file->aux_list.first->_aux_symbol._aux_file.filename : "");
  }

  free(syms);
  _map_line(NULL);
  _map_line(NULL);
}

/*------------------------------------------------------------------------------------------------*/

void
make_map(void)
{
  if ((gp_num_errors > 0) || (state.map_file == OUT_SUPPRESS)) {
    unlink(state.map_file_name);
    return;
  }

  state.map.f = fopen(state.map_file_name, "wt");
  if (state.map.f == NULL) {
    perror(state.map_file_name);
    exit(1);
  }

  _map_line("%s", GPLINK_VERSION_STRING);
  _map_line("Map File - Created %s", state.start_date);
  _map_line(NULL);

  /* write sections */
  _write_sections();

  /* program memory usage */
  _write_program_memory();

  /* write symbols */
  _write_symbols();

  fclose(state.map.f);
}

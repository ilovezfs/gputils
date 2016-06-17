/* GNU PIC general coff functions
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

/*------------------------------------------------------------------------------------------------*/

gp_object_type *
gp_coffgen_init(void)
{
  gp_object_type *object;

  /* allocate memory for the object file */
  object = (gp_object_type *)GP_Calloc(1, sizeof(gp_object_type));

  /* initialize the object */
  object->processor = gp_find_processor("generic");
  object->class     = PROC_CLASS_GENERIC;
  object->time      = (long)time(NULL);

  return object;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_find_section(gp_object_type *object, gp_section_type *start, const char *name)
{
  gp_section_type *current;
  gp_section_type *found;

  if (object == NULL) {
    return NULL;
  }

  current = start;
  found   = NULL;

  while (current != NULL) {
    if ((current->name != NULL) && (strcmp(current->name, name) == 0)) {
      found = current;
      break;
    }
    current = current->next;
  }

  return found;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_new_section(const char *name, MemBlock *data)
{
  gp_section_type *new;

  /* allocate memory for the section */
  new = (gp_section_type *)GP_Calloc(1, sizeof(gp_section_type));

  /* initialize section */
  new->name = GP_Strdup(name);
  new->data = (data != NULL) ? data : i_memory_create();

  return new;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_add_section(gp_object_type *object, const char *name, MemBlock *data)
{
  gp_section_type *new;

  if (object == NULL) {
    return NULL;
  }

  new = gp_coffgen_new_section(name, data);

  if (object->sections == NULL) {
    /* the list is empty */
    object->sections = new;
  }
  else {
    /* append the new object to the end of the list */
    object->sections_tail->next = new;
  }

  object->sections_tail = new;
  (object->num_sections)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_coffgen_del_section_symbols(gp_object_type *object, gp_section_type *section)
{
  gp_symbol_type *list;
  gp_symbol_type *symbol;

  /* remove all symbols for the section */
  list = object->symbols;
  while (list != NULL) {
    /* advance the pointer so the symbol can be freed */
    symbol = list;
    list = list->next;

    if (symbol->section == section) {
      gp_coffgen_del_symbol(object, symbol);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_del_section(gp_object_type *object, gp_section_type *section)
{
  gp_section_type *list;
  gp_section_type *previous;
  gp_section_type *removed;

  if (object == NULL) {
    return NULL;
  }

  list     = object->sections;
  previous = NULL;
  removed  = NULL;

  while (list != NULL) {
    if (list == section) {
      removed = section;

      if (previous == NULL) {
        /* removing the first section in the list */
        object->sections = list->next;

        if (object->sections == NULL) {
          /* there are no sections in the list */
          object->sections_tail = NULL;
        }
        else if (object->sections->next == NULL) {
          /* there is one section in the list */
          object->sections_tail = object->sections;
        }
      }
      else {
        previous->next = list->next;

        if (list->next == NULL) {
          /* The last section in the list is being removed, so update the tail. */
          object->sections_tail = previous;
        }
      }
      break;
    }
    previous = list;
    list = list->next;
  }

  (object->num_sections)--;

  /* FIXME: gp_coffgen_free_section(second); */

  return removed;
}

/*------------------------------------------------------------------------------------------------*/

gp_reloc_type *
gp_coffgen_add_reloc(gp_section_type *section)
{
  gp_reloc_type *new;

  /* allocate memory for the relocation */
  new = (gp_reloc_type *)GP_Calloc(1, sizeof(gp_reloc_type));

  if (section->relocations == NULL) {
    /* the list is empty */
    section->relocations = new;
  }
  else {
    section->relocations_tail->next = new;
    new->prev = section->relocations_tail;
  }

  section->relocations_tail = new;
  (section->num_reloc)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

gp_reloc_type *
gp_coffgen_del_reloc(gp_section_type *section, gp_reloc_type *relocation)
{
  gp_symbol_type *symbol;

  if ((section->relocations == NULL) || (section->num_reloc == 0)) {
    return NULL;
  }

  if (section->relocations == relocation) {
    /* This is first element of the list. */
    section->relocations = relocation->next;
  }
  else {
    relocation->prev->next = relocation->next;
  }

  if (section->relocations_tail == relocation) {
    /* This is last element of the list. */
    section->relocations_tail = relocation->prev;
  }
  else {
    relocation->next->prev = relocation->prev;
  }

  (section->num_reloc)--;

  symbol = relocation->symbol;

  if (symbol->num_reloc_link > 0) {
    (symbol->num_reloc_link)--;
  }
  else {
    gp_warning("Number of relocation links of symbol is zero: '%s'", symbol->name);
  }

  return relocation;
}

/*------------------------------------------------------------------------------------------------*/

const char *
gp_coffgen_reloc_type_to_str(uint16_t type)
{
  static const char * const type_str[] = {
    "",
    "RELOCT_CALL",
    "RELOCT_GOTO",
    "RELOCT_HIGH",
    "RELOCT_LOW",
    "RELOCT_P",
    "RELOCT_BANKSEL",
    "RELOCT_PAGESEL",
    "RELOCT_ALL",
    "RELOCT_IBANKSEL",
    "RELOCT_F",
    "RELOCT_TRIS",
    "RELOCT_MOVLR",
    "RELOCT_MOVLB",
    "RELOCT_GOTO2/CALL2",
    "RELOCT_FF1",
    "RELOCT_FF2",
    "RELOCT_LFSR1",
    "RELOCT_LFSR2",
    "RELOCT_BRA/RCALL",
    "RELOCT_CONDBRA",
    "RELOCT_UPPER",
    "RELOCT_ACCESS",
    "RELOCT_PAGESEL_WREG",
    "RELOCT_PAGESEL_BITS",
    "RELOCT_SCNSZ_LOW",
    "RELOCT_SCNSZ_HIGH",
    "RELOCT_SCNSZ_UPPER",
    "RELOCT_SCNEND_LOW",
    "RELOCT_SCNEND_HIGH",
    "RELOCT_SCNEND_UPPER",
    "RELOCT_SCNEND_LFSR1",
    "RELOCT_SCNEND_LFSR2",
    "RELOCT_TRIS_3BIT",
    "RELOCT_PAGESEL_MOVLP"
  };

  if (type >= ARRAY_SIZE(type_str)) {
    type = 0;
  }

  return type_str[type];
}

/*------------------------------------------------------------------------------------------------*/

gp_linenum_type *
gp_coffgen_add_linenum(gp_section_type *section)
{
  gp_linenum_type *new;

  /* allocate memory for the relocation */
  new = (gp_linenum_type *)GP_Calloc(1, sizeof(gp_linenum_type));

  if (section->line_numbers == NULL) {
    /* the list is empty */
    section->line_numbers = new;
  }
  else {
    section->line_numbers_tail->next = new;
    new->prev = section->line_numbers_tail;
  }

  section->line_numbers_tail = new;
  (section->num_lineno)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

gp_linenum_type *
gp_coffgen_del_linenum(gp_section_type *section, gp_linenum_type *linenum)
{
  if ((section->line_numbers == NULL) || (section->num_lineno == 0)) {
    return NULL;
  }

  if (section->line_numbers == linenum) {
    /* This is first element of the list. */
    section->line_numbers = linenum->next;
  }
  else {
    linenum->prev->next = linenum->next;
  }

  if (section->line_numbers_tail == linenum) {
    /* This is last element of the list. */
    section->line_numbers_tail = linenum->prev;
  }
  else {
    linenum->next->prev = linenum->prev;
  }

  (section->num_lineno)--;

  return linenum;
}

/*------------------------------------------------------------------------------------------------*/

gp_linenum_type *
gp_coffgen_find_linenum_by_address(gp_section_type *section, unsigned int address)
{
  gp_linenum_type *linenum;

  if ((section->line_numbers == NULL) || (section->num_lineno == 0)) {
    return NULL;
  }

  linenum = section->line_numbers;
  while (linenum != NULL) {
    if (linenum->address == address) {
      return linenum;
    }
    linenum = linenum->next;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

gp_linenum_type *
gp_coffgen_del_linenum_by_address(gp_section_type *section, unsigned int address)
{
  gp_linenum_type *linenum;

  linenum = gp_coffgen_find_linenum_by_address(section, address);

  if (linenum == NULL) {
    return NULL;
  }

  return gp_coffgen_del_linenum(section, linenum);
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_coffgen_del_linenum_by_address_area(gp_section_type *section, unsigned int address_start,
                                       unsigned int address_end)
{
  gp_linenum_type *linenum;

  if ((section->line_numbers == NULL) || (section->num_lineno == 0)) {
    return false;
  }

  linenum = section->line_numbers;
  while ((linenum != NULL) && (linenum->address <= address_end)) {
    if (linenum->address >= address_start) {
      gp_coffgen_del_linenum(section, linenum);
    }
    linenum = linenum->next;
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

gp_symbol_type *
gp_coffgen_find_symbol(gp_object_type *object, const char *name)
{
  gp_symbol_type *current;
  gp_symbol_type *found;

  if (object == NULL) {
    return NULL;
  }

  current = object->symbols;
  found   = NULL;

  while (current != NULL) {
    if ((current->class != C_SECTION) && (current->name != NULL) &&
        (strcmp(current->name, name) == 0)) {
      found = current;
      break;
    }
    current = current->next;
  }

  return found;
}

/*------------------------------------------------------------------------------------------------*/

gp_symbol_type *
gp_coffgen_find_section_symbol(gp_object_type *object, const char *name)
{
  gp_symbol_type *current;
  gp_symbol_type *found;

  if (object == NULL) {
    return NULL;
  }

  current = object->symbols;
  found   = NULL;

  while (current != NULL) {
    if ((current->class == C_SECTION) && (current->name != NULL) &&
        (strcmp(current->name, name) == 0)) {
      found = current;
      break;
    }
    current = current->next;
  }

  return found;
}

/*------------------------------------------------------------------------------------------------*/

/* Search for a symbol, based on the value and name of host section.
   The function is slow, but only need the error messages. */

gp_symbol_type *
gp_coffgen_find_symbol_section_value(gp_object_type *object, const char *section_name, long value)
{
  gp_symbol_type *current;
  gp_symbol_type *found;

  if ((object == NULL) || (section_name == NULL)) {
    return NULL;
  }

  current = object->symbols;
  found   = NULL;

  while (current != NULL) {
    if ((current->class != C_SECTION) && (current->class != C_FILE) &&
        (current->section_name != NULL) &&
        (strcmp(current->section_name, section_name) == 0) &&
        (current->value == value)) {
      found = current;
      break;
    }
    current = current->next;
  }

  return found;
}

/*------------------------------------------------------------------------------------------------*/

gp_aux_type *
gp_coffgen_add_aux(gp_object_type *Object, gp_symbol_type *Symbol)
{
  gp_aux_type *new;

  /* allocate memory for the auxiliary symbol */
  new = (gp_aux_type *)GP_Calloc(1, sizeof(gp_aux_type));

  if (Symbol->aux_list == NULL) {
    /* the list is empty */
    Symbol->aux_list = new;
  }
  else {
    /* append the new object to the end of the list */
    Symbol->aux_list_tail->next = new;
  }

  Symbol->aux_list_tail = new;

  (Symbol->num_auxsym)++;
  (Object->num_symbols)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

gp_symbol_type *
gp_coffgen_add_symbol(gp_object_type *Object)
{
  gp_symbol_type *new;

  /* allocate memory for the symbol */
  new = (gp_symbol_type *)GP_Calloc(1, sizeof(gp_symbol_type));
  new->number = Object->num_symbols;

  if (Object->symbols == NULL) {
    /* the list is empty */
    Object->symbols = new;
  }
  else {
    Object->symbols_tail->next = new;
  }

  Object->symbols_tail = new;
  (Object->num_symbols)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

gp_symbol_type *
gp_coffgen_del_symbol(gp_object_type *Object, gp_symbol_type *Symbol)
{
  gp_symbol_type *list;
  gp_symbol_type *previous;
  gp_symbol_type *removed;

  if (Object == NULL) {
    return NULL;
  }

  list     = Object->symbols;
  previous = NULL;
  removed  = NULL;

  while (list != NULL) {
    if (list == Symbol) {
      removed = Symbol;

      if (previous == NULL) {
        /* removing the first symbol in the list */
        Object->symbols = list->next;
        if (Object->symbols == NULL) {
          /* there are no symbols in the list */
          Object->symbols_tail = NULL;
        }
        else if (Object->symbols->next == NULL) {
          /* there is one symbol in the list */
          Object->symbols_tail = Object->symbols;
        }
      }
      else {
        previous->next = list->next;

        if (list->next == NULL) {
          /* The last symbol in the list is being removed, so update the tail. */
          Object->symbols_tail = previous;
        }
      }
      break;
    }
    previous = list;
    list = list->next;
  }

  Object->num_symbols -= Symbol->num_auxsym + 1;

  /* FIXME: gp_coffgen_free_symbol(Symbol); */

  return removed;
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if any relocation uses the symbol. */

gp_boolean
gp_coffgen_symbol_has_reloc(const gp_symbol_type *Symbol)
{
  return ((Symbol->num_reloc_link > 0) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if the symbol is global. */

gp_boolean
gp_coffgen_is_global_symbol(const gp_symbol_type *Symbol)
{
  return (((Symbol->class == C_EXT) && (Symbol->section_number == N_SCNUM)) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if the symbol is external. */

gp_boolean
gp_coffgen_is_external_symbol(const gp_symbol_type *Symbol)
{
  return (((Symbol->class == C_EXT) && (Symbol->section_number == N_UNDEF)) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if the symbol is debug. */

gp_boolean
gp_coffgen_is_debug_symbol(const gp_symbol_type *Symbol)
{
  return (((Symbol->class == C_NULL) && (Symbol->section_number == N_DEBUG)) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if the symbol is absolute. */

gp_boolean
gp_coffgen_is_absolute_symbol(const gp_symbol_type *Symbol)
{
  return (((Symbol->class == C_NULL) && (Symbol->section_number == N_ABS)) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of section */

gp_section_type *
gp_coffgen_make_block_section(unsigned int number)
{
  gp_section_type *new;
  unsigned int     i;

  if (number == 0) {
    return NULL;
  }

  /* allocate memory for the sections */
  new = (gp_section_type *)GP_Calloc(number, sizeof(gp_section_type));

  /* don't process the last entry */
  number--;

  /* initialize the pointers to create the linked list */
  for (i = 0; i < number; i++) {
    new[i].next = &new[i + 1];
  }

  /* assign the tail of the list */
  new[number].next = NULL;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of relocations */

gp_reloc_type *
gp_coffgen_make_block_reloc(unsigned int number)
{
  gp_reloc_type *new;
  unsigned int   i;

  if (number == 0) {
    return NULL;
  }

  /* allocate memory for the relocations */
  new = (gp_reloc_type *)GP_Calloc(number, sizeof(gp_reloc_type));

  /* don't process the last entry */
  number--;

  /* initialize the pointers to create the linked list */
  for (i = 0; i < number; i++) {
    new[i+1].prev = &new[i];
    new[i].next   = &new[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of line numbers */

gp_linenum_type *
gp_coffgen_make_block_linenum(unsigned int number)
{
  gp_linenum_type *new;
  unsigned int     i;

  if (number == 0) {
    return NULL;
  }

  /* allocate memory for the symbol */
  new = (gp_linenum_type *)GP_Calloc(number, sizeof(gp_linenum_type));

  /* don't process the last entry */
  number--;

  /* initialize the pointers to create the linked list */
  for (i = 0; i < number; i++) {
    new[i+1].prev = &new[i];
    new[i].next   = &new[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of symbols */

gp_symbol_type *
gp_coffgen_make_block_symbol(unsigned int number)
{
  gp_symbol_type *new;
  unsigned int    i;

  if (number == 0) {
    return NULL;
  }

  /* allocate memory for the symbols */
  new = (gp_symbol_type *)GP_Calloc(sizeof(*new) * number, sizeof(gp_symbol_type));

  /* don't process the last entry */
  number--;

  /* initialize the pointers to create the linked list */
  for (i = 0; i < number; i++) {
    new[i].name = NULL;
    new[i].next = &new[i + 1];
  }

  /* assign the tail of the list */
  new[number].name = NULL;
  new[number].next = NULL;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of auxiliary symbols */

gp_aux_type *
gp_coffgen_make_block_aux(unsigned int number)
{
  gp_aux_type  *new;
  unsigned int  i;

  if (number == 0) {
    return NULL;
  }

  /* allocate memory for the symbols */
  new = (gp_aux_type *)GP_Calloc(sizeof(*new) * number, sizeof(gp_aux_type));

  /* don't process the last entry */
  number--;

  /* initialize the pointers to create the linked list */
  for (i = 0; i < number; i++) {
    new[i].type = AUX_NONE;
    new[i].next = &new[i + 1];
  }

  /* assign the tail of the list */
  new[number].next = NULL;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_coffgen_free_section(gp_section_type *section)
{
  gp_reloc_type   *relocation;
  gp_linenum_type *line_number;
  gp_reloc_type   *old_relocation;
  gp_linenum_type *old_line_number;

  i_memory_free(section->data);

  relocation = section->relocations;
  while (relocation != NULL) {
    old_relocation = relocation;
    relocation = relocation->next;
    free(old_relocation);
  }

  line_number = section->line_numbers;
  while (line_number != NULL) {
    old_line_number = line_number;
    line_number = line_number->next;
    free(old_line_number);
  }

  if (section->line_numbers_array != NULL) {
    free(section->line_numbers_array);
  }

  free(section->name);
  free(section);
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_coffgen_free_symbol(gp_symbol_type *symbol)
{
  gp_aux_type *aux;
  gp_aux_type *old_aux;
  unsigned int num_auxsym;

  if (symbol == NULL) {
    return 0;
  }

  /* free the auxilary symbols */
  aux        = symbol->aux_list;
  num_auxsym = symbol->num_auxsym;

  while (aux != NULL) {
    old_aux = aux;
    aux = aux->next;
    free(old_aux);
  }

  free(symbol->name);
  free(symbol);

  return num_auxsym;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_coffgen_free_object(gp_object_type *object)
{
  gp_section_type *section;
  gp_symbol_type  *symbol;
  gp_section_type *old_section;
  gp_symbol_type  *old_symbol;

  if (object == NULL) {
    return false;
  }

  free(object->filename);

  section = object->sections;
  while (section != NULL) {
    old_section = section;
    section     = section->next;
    gp_coffgen_free_section(old_section);
  }

  symbol = object->symbols;
  while (symbol != NULL) {
    old_symbol = symbol;
    symbol     = symbol->next;
    gp_coffgen_free_symbol(old_symbol);
  }

  if (object->symbol_hashtable != NULL) {
    free(object->symbol_hashtable);
  }

  free(object);

  return true;
}

/*------------------------------------------------------------------------------------------------*/

int
gp_determine_aux_symbol(gp_symbol_type *symbol)
{
  int aux_type = AUX_NONE;

  if (strcasecmp(".direct", symbol->name) == 0) {
    return AUX_DIRECT;
  }

  if (strcasecmp(".ident", symbol->name) == 0) {
    return AUX_IDENT;
  }

  if ((symbol->derived_type & 7) == DT_FCN) {
    return AUX_FCN;
  }

  switch (symbol->class) {
    case C_FILE:
      aux_type = AUX_FILE;
      break;

    case C_SECTION:
      aux_type = AUX_SCN;
      break;

    case C_BLOCK:
    case C_FCN:
      aux_type = (symbol->name[1] == 'b') ? AUX_BOBF : AUX_EOBF;
      break;

    default:
      aux_type = AUX_NONE;
  }

  return aux_type;
}

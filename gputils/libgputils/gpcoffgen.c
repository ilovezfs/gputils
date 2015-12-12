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

gp_object_type *
gp_coffgen_init(void)
{
  gp_object_type *object = NULL;

  /* allocate memory for the object file */
  object = (gp_object_type *)GP_Calloc(1, sizeof(gp_object_type));

  /* initialize the object */
  object->processor = gp_find_processor("generic");
  object->class = PROC_CLASS_GENERIC;
  object->time = (long)time(NULL);

  return object;
}

gp_section_type *
gp_coffgen_findsection(gp_object_type *object,
                       gp_section_type *start,
                       const char *name)
{
  gp_section_type *current = NULL;
  gp_section_type *found = NULL;

  if (object == NULL) {
    return NULL;
  }

  current = start;

  while (current != NULL) {
    if ((current->name != NULL) && (strcmp(current->name, name) == 0)) {
      found = current;
      break;
    }
    current = current->next;
  }

  return found;
}

gp_section_type *
gp_coffgen_newsection(const char *name, MemBlock *data)
{
  gp_section_type *new = NULL;

  /* allocate memory for the section */
  new = (gp_section_type *)GP_Calloc(1, sizeof(gp_section_type));

  /* initialize section */
  new->name = GP_Strdup(name);
  new->data = (data != NULL) ? data : i_memory_create();

  return new;
}

gp_section_type *
gp_coffgen_addsection(gp_object_type *object, const char *name, MemBlock *data)
{
  gp_section_type *new = NULL;

  if (object == NULL) {
    return NULL;
  }

  new = gp_coffgen_newsection(name, data);

  if (object->sections == NULL) {
    /* the list is empty */
    object->sections = new;
  }
  else {
    /* append the new object to the end of the list */
    object->sections_tail->next = new;
  }

  object->sections_tail = new;
  object->num_sections++;

  return new;
}

void
gp_coffgen_delsectionsyms(gp_object_type *object, gp_section_type *section)
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
      gp_coffgen_delsymbol(object, symbol);
    }
  }
}

gp_section_type *
gp_coffgen_delsection(gp_object_type *object, gp_section_type *section)
{
  gp_section_type *list = NULL;
  gp_section_type *previous = NULL;
  gp_section_type *removed = NULL;

  if (object == NULL) {
    return NULL;
  }

  list = object->sections;
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

  object->num_sections--;

  /* FIXME: gp_coffgen_free_section(second); */

  return removed;
}

gp_reloc_type *
gp_coffgen_addreloc(gp_section_type *section)
{
  gp_reloc_type *new = NULL;

  /* allocate memory for the relocation */
  new = (gp_reloc_type *)GP_Calloc(1, sizeof(gp_reloc_type));

  if (section->relocations == NULL) {
    /* the list is empty */
    section->relocations = new;
  }
  else {
    section->relocations_tail->next = new;
  }

  section->relocations_tail = new;
  section->num_reloc++;

  return new;
}

gp_linenum_type *
gp_coffgen_addlinenum(gp_section_type *section)
{
  gp_linenum_type *new = NULL;

  /* allocate memory for the relocation */
  new = (gp_linenum_type *)GP_Calloc(1, sizeof(gp_linenum_type));

  if (section->line_numbers == NULL) {
    /* the list is empty */
    section->line_numbers = new;
  }
  else {
    section->line_numbers_tail->next = new;
  }

  section->line_numbers_tail = new;
  section->num_lineno++;

  return new;
}

gp_symbol_type *
gp_coffgen_findsymbol(gp_object_type *object, const char *name)
{
  gp_symbol_type *current = NULL;
  gp_symbol_type *found = NULL;

  if (object == NULL) {
    return NULL;
  }

  current = object->symbols;

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

gp_symbol_type *
gp_coffgen_findsectionsymbol(gp_object_type *object, const char *name)
{
  gp_symbol_type *current = NULL;
  gp_symbol_type *found = NULL;

  if (object == NULL) {
    return NULL;
  }

  current = object->symbols;

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

/* Search for a symbol, based on the value and name of host section.
   The function is slow, but only need the error messages. */

gp_symbol_type *
gp_coffgen_findsymbol_sect_val(gp_object_type *object, const char *section_name, long value)
{
  gp_symbol_type *current = NULL;
  gp_symbol_type *found = NULL;

  if ((object == NULL) || (section_name == NULL)) {
    return NULL;
  }

  current = object->symbols;

  while (current != NULL) {
    if ((current->class != C_SECTION) &&
        (current->class != C_FILE) &&
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

gp_aux_type *
gp_coffgen_addaux(gp_object_type *object, gp_symbol_type *symbol)
{
  gp_aux_type *new = NULL;
  gp_aux_type *list = NULL;

  /* allocate memory for the auxiliary symbol */
  new = (gp_aux_type *)GP_Calloc(1, sizeof(gp_aux_type));

  if (symbol->aux_list == NULL) {
    /* the list is empty */
    symbol->aux_list = new;
  }
  else {
    /* append the new object to the end of the list */
    list = symbol->aux_list;
    while (list->next != NULL) {
      list = list->next;
    }
    list->next = new;
  }

  symbol->num_auxsym++;
  object->num_symbols++;

  return new;
}

gp_symbol_type *
gp_coffgen_addsymbol(gp_object_type *object)
{
  gp_symbol_type *new = NULL;

  /* allocate memory for the symbol */
  new = (gp_symbol_type *)GP_Calloc(1, sizeof(gp_symbol_type));
  new->number = object->num_symbols;

  if (object->symbols == NULL) {
    /* the list is empty */
    object->symbols = new;
  }
  else {
    object->symbols_tail->next = new;
  }

  object->symbols_tail = new;
  object->num_symbols += 1;

  return new;
}

gp_symbol_type *
gp_coffgen_delsymbol(gp_object_type *object, gp_symbol_type *symbol)
{
  gp_symbol_type *list = NULL;
  gp_symbol_type *previous = NULL;
  gp_symbol_type *removed = NULL;

  if (object == NULL) {
    return NULL;
  }

  list = object->symbols;
  while (list != NULL) {
    if (list == symbol) {
      removed = symbol;

      if (previous == NULL) {
        /* removing the first symbol in the list */
        object->symbols = list->next;
        if (object->symbols == NULL) {
          /* there are no symbols in the list */
          object->symbols_tail = NULL;
        }
        else if (object->symbols->next == NULL) {
          /* there is one symbol in the list */
          object->symbols_tail = object->symbols;
        }
      }
      else {
        previous->next = list->next;

        if (list->next == NULL) {
          /* The last symbol in the list is being removed, so update
             the tail. */
          object->symbols_tail = previous;
        }
      }
      break;
    }
    previous = list;
    list = list->next;
  }

  object->num_symbols -= (symbol->num_auxsym + 1);

  /* FIXME: gp_coffgen_free_symbol(symbol); */

  return removed;
}

/* Determine if any relocation uses the symbol. */

gp_boolean
gp_coffgen_has_reloc(gp_object_type *object, gp_symbol_type *symbol)
{
  gp_section_type *section;
  gp_reloc_type *relocation;

  section = object->sections;
  while (section != NULL) {
    relocation = section->relocations;
    while (relocation != NULL) {
      if (relocation->symbol == symbol) {
        return true;
      }
      relocation = relocation->next;
    }
    section = section->next;
  }

  return false;
}

/* Determine if the symbol is global */

gp_boolean
gp_coffgen_is_global(gp_symbol_type *symbol)
{
  return ((symbol->class == C_EXT) && (symbol->section_number == N_SCNUM)) ? true : false;
}

/* Determine if the symbol is external */

gp_boolean
gp_coffgen_is_external(gp_symbol_type *symbol)
{
  return ((symbol->class == C_EXT) && (symbol->section_number == N_UNDEF)) ? true : false;
}

/* Determine if the symbol is debug */

gp_boolean
gp_coffgen_is_debug(gp_symbol_type *symbol)
{
  return (symbol->class == N_DEBUG) ? true : false;
}

/* Determine if the symbol is absolute */

gp_boolean
gp_coffgen_is_absolute(gp_symbol_type *symbol)
{
  return (symbol->class == N_ABS) ? true : false;
}

/* allocate a block of section */

gp_section_type *
gp_coffgen_blocksec(unsigned int number)
{
  gp_section_type *new = NULL;
  unsigned int i;

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


/* allocate a block of relocations */

gp_reloc_type *
gp_coffgen_blockrel(unsigned int number)
{
  gp_reloc_type *new = NULL;
  unsigned int i;

  if (number == 0) {
    return NULL;
  }

  /* allocate memory for the relocations */
  new = (gp_reloc_type *)GP_Calloc(number, sizeof(gp_reloc_type));

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

/* allocate a block of line numbers */

gp_linenum_type *
gp_coffgen_blockline(unsigned int number)
{
  gp_linenum_type *new = NULL;
  unsigned int i;

  if (number == 0) {
    return NULL;
  }

  /* allocate memory for the symbol */
  new = (gp_linenum_type *)GP_Calloc(number, sizeof(gp_linenum_type));

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


/* allocate a block of symbols */

gp_symbol_type *
gp_coffgen_blocksym(unsigned int number)
{
  gp_symbol_type *new = NULL;
  unsigned int i;

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

/* allocate a block of auxiliary symbols */

gp_aux_type *
gp_coffgen_blockaux(unsigned int number)
{
  gp_aux_type *new = NULL;
  unsigned int i;

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

int
gp_coffgen_free_section(gp_section_type *section)
{
  gp_reloc_type *relocation;
  gp_linenum_type *line_number;
  gp_reloc_type *old_relocation;
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

  free(section->name);
  free(section);

  return 0;
}

int
gp_coffgen_free_symbol(gp_symbol_type *symbol)
{
  gp_aux_type *aux;
  gp_aux_type *old_aux;
  int num_auxsym = symbol->num_auxsym;

  /* free the auxilary symbols */
  aux = symbol->aux_list;
  while (aux != NULL) {
    old_aux = aux;
    aux = aux->next;
    free(old_aux);
  }

  free(symbol->name);
  free(symbol);

  return num_auxsym;
}

int
gp_coffgen_free(gp_object_type *object)
{
  gp_section_type *section;
  gp_symbol_type *symbol;
  gp_section_type *old_section;
  gp_symbol_type *old_symbol;

  if (object == NULL) {
    return 1;
  }

  free(object->filename);

  section = object->sections;
  while (section != NULL) {
    old_section = section;
    section = section->next;
    gp_coffgen_free_section(old_section);
  }

  symbol = object->symbols;
  while (symbol != NULL) {
    old_symbol = symbol;
    symbol = symbol->next;
    gp_coffgen_free_symbol(old_symbol);
  }

  free(object);

  return 0;
}

int
gp_determine_aux(gp_symbol_type *symbol)
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

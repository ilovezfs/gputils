/* GNU PIC general coff functions
   Copyright (C) 2001, 2002, 2003, 2004
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
  object = (gp_object_type *)malloc(sizeof(*object));

  /* initialize the object */
  object->filename = NULL;
  object->processor = generic;
  object->class = PROC_CLASS_GENERIC;
  object->time = (long)time(NULL);
  object->flags = 0;
  object->num_sections = 0;
  object->sections = NULL;
  object->sections_tail = NULL;
  object->num_symbols = 0;
  object->symbols = NULL;
  object->symbols_tail = NULL;
  object->next = NULL;

  return object;
}

gp_section_type *
gp_coffgen_findsection(gp_object_type *object, 
                       gp_section_type *start,
                       char *name)
{
  gp_section_type *current = NULL;
  gp_section_type *found = NULL;

  if (object == NULL)
    return NULL;

  current = start;

  while (current != NULL) {
    if ((current->name != NULL) &&
        (strcmp(current->name, name) == 0)) {
      found = current;
      break;
    }
    current = current->next;
  }

  return found;
}

gp_section_type *
gp_coffgen_newsection(char *name)
{
  gp_section_type *new = NULL;

  /* allocate memory for the section */
  new = (gp_section_type *)malloc(sizeof(*new));

  /* initialize section */
  new->name = strdup(name);
  new->symbol = NULL;
  new->flags = 0;
  new->address = 0;
  new->size = 0;
  new->data = i_memory_create();
  new->num_reloc  = 0;
  new->relocations = NULL;
  new->relocations_tail = NULL;
  new->num_lineno = 0;
  new->line_numbers = NULL;
  new->line_numbers_tail = NULL;
  new->next = NULL;

  return new;
}

gp_section_type *
gp_coffgen_addsection(gp_object_type *object, char *name)
{
  gp_section_type *new = NULL;

  if (object == NULL)
    return NULL;

  new = gp_coffgen_newsection(name);

  if (object->sections == NULL) {
    /* the list is empty */
    object->sections = new;
  } else {
    /* append the new object to the end of the list */  
    object->sections_tail->next = new;
  }
  object->sections_tail = new;

  object->num_sections++;

  return new;
}

gp_reloc_type *
gp_coffgen_addreloc(gp_section_type *section)
{
  gp_reloc_type *new = NULL;

  /* allocate memory for the relocation */
  new = (gp_reloc_type *)malloc(sizeof(*new));

  new->address = 0;
  new->symbol = NULL;
  new->symbol_number = 0;
  new->offset = 0;
  new->type = 0;    
  new->next = NULL;

  if (section->relocations == NULL) {
    /* the list is empty */
    section->relocations = new;
  } else {
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
  new = (gp_linenum_type *)malloc(sizeof(*new));

  new->symbol = NULL; 
  new->line_number = 0;
  new->address = 0; 
  new->next = NULL;

  if (section->line_numbers == NULL) {
    /* the list is empty */
    section->line_numbers = new;
  } else {
    section->line_numbers_tail->next = new;
  }
  section->line_numbers_tail = new;

  section->num_lineno++;

  return new;
}

gp_symbol_type *
gp_coffgen_findsymbol(gp_object_type *object, char *name)
{
  gp_symbol_type *current = NULL;
  gp_symbol_type *found = NULL;

  if (object == NULL)
    return NULL;
  
  current = object->symbols;

  while (current != NULL) {
    if ((current->name != NULL) &&
        (strcmp(current->name, name) == 0)) {
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

  /* allocate memory for the auxilary symbol */
  new = (gp_aux_type *)malloc(sizeof(*new));
  new->next = NULL;

  if (symbol->aux_list == NULL) {
    /* the list is empty */
    symbol->aux_list = new;
  } else {
    /* append the new object to the end of the list */  
    list = symbol->aux_list;
    while (list->next != NULL) {
      list = list->next;
    }
    list->next = new;
  }

  symbol->num_auxsym += 1;
  object->num_symbols += 1;

  return new;
}

gp_symbol_type *
gp_coffgen_addsymbol(gp_object_type *object)
{
  gp_symbol_type *new = NULL;

  /* allocate memory for the symbol */
  new = (gp_symbol_type *)malloc(sizeof(*new));
  
  new->name = NULL;
  new->value = 0;
  new->section_number = 0;
  new->section = NULL;
  new->type = 0;
  new->class = 0;
  new->num_auxsym = 0;
  new->aux_list = NULL;
  new->number = object->num_symbols;
  new->next = NULL;

  if (object->symbols == NULL) {
    /* the list is empty */
    object->symbols = new;
  } else {
    object->symbols_tail->next = new;
  }
  object->symbols_tail = new;

  object->num_symbols += 1;

  return new;
}

/* allocate a block of section */

gp_section_type *
gp_coffgen_blocksec(unsigned int number)
{
  gp_section_type *new = NULL;
  unsigned int i;
  
  if (number == 0)
    return NULL;
    
  /* allocate memory for the sections */
  new = (gp_section_type *)malloc(sizeof(*new) * number);

  /* don't process the last entry */
  number--;

  /* initialize the pointers to create the linked list */  
  for(i = 0; i < number; i++)
    new[i].next = &new[i+1];
  
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
  
  if (number == 0)
    return NULL;
    
  /* allocate memory for the relocations */
  new = (gp_reloc_type *)malloc(sizeof(*new) * number);

  /* don't process the last entry */
  number--;

  /* initialize the pointers to create the linked list */  
  for(i = 0; i < number; i++)
    new[i].next = &new[i+1];
  
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
  
  if (number == 0)
    return NULL;
    
  /* allocate memory for the symbol */
  new = (gp_linenum_type *)malloc(sizeof(*new) * number);

  /* don't process the last entry */
  number--;

  /* initialize the pointers to create the linked list */  
  for(i = 0; i < number; i++)
    new[i].next = &new[i+1];
  
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
  
  if (number == 0)
    return NULL;
    
  /* allocate memory for the symbols */
  new = (gp_symbol_type *)malloc(sizeof(*new) * number);

  /* don't process the last entry */
  number--;

  /* initialize the pointers to create the linked list */  
  for(i = 0; i < number; i++) {
    new[i].name = NULL;
    new[i].next = &new[i+1];
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
  
  if (number == 0)
    return NULL;
    
  /* allocate memory for the symbols */
  new = (gp_aux_type *)malloc(sizeof(*new) * number);

  /* don't process the last entry */
  number--;

  /* initialize the pointers to create the linked list */  
  for(i = 0; i < number; i++)
    new[i].next = &new[i+1];
  
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
  
  if (object == NULL)
    return 1;

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

  switch (symbol->class) {
  case C_FILE:
    aux_type = AUX_FILE;
    break;
  case C_SECTION:
    aux_type = AUX_SCN;
    break;
  default:
    aux_type = AUX_NONE;
  }

  return aux_type;
}

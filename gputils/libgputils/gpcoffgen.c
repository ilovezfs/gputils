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
gp_coffgen_find_section(gp_object_type *Object, gp_section_type *Start, const char *Name)
{
  gp_section_type *current;
  gp_section_type *found;

  if (Object == NULL) {
    return NULL;
  }

  current = Start;
  found   = NULL;

  while (current != NULL) {
    if ((current->name != NULL) && (strcmp(current->name, Name) == 0)) {
      found = current;
      break;
    }
    current = current->next;
  }

  return found;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_new_section(const char *Name, MemBlock *Data)
{
  gp_section_type *new;

  /* allocate memory for the section */
  new = (gp_section_type *)GP_Calloc(1, sizeof(gp_section_type));

  /* initialize section */
  new->name = GP_Strdup(Name);
  new->data = (Data != NULL) ? Data : i_memory_create();

  return new;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_add_section(gp_object_type *Object, const char *Name, MemBlock *Data)
{
  gp_section_type *new;

  if (Object == NULL) {
    return NULL;
  }

  new = gp_coffgen_new_section(Name, Data);

  if (Object->section_list == NULL) {
    /* the list is empty */
    Object->section_list = new;
  }
  else {
    /* append the new object to the end of the list */
    Object->section_list_tail->next = new;
  }

  Object->section_list_tail = new;
  (Object->num_sections)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_coffgen_del_section_symbols(gp_object_type *Object, gp_section_type *Section)
{
  gp_symbol_type *list;
  gp_symbol_type *symbol;

  if (Object == NULL) {
    return NULL;
  }

  /* remove all symbols for the section */
  list = Object->symbol_list;
  while (list != NULL) {
    /* advance the pointer so the symbol can be freed */
    symbol = list;
    list   = list->next;

    if (symbol->section == Section) {
      gp_coffgen_del_symbol(Object, symbol);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_del_section(gp_object_type *Object, gp_section_type *Section)
{
  gp_section_type *list;
  gp_section_type *previous;
  gp_section_type *removed;

  if (Object == NULL) {
    return NULL;
  }

  list     = Object->section_list;
  previous = NULL;
  removed  = NULL;

  while (list != NULL) {
    if (list == Section) {
      removed = Section;

      if (previous == NULL) {
        /* removing the first section in the list */
        Object->section_list = list->next;

        if (Object->section_list == NULL) {
          /* there are no sections in the list */
          Object->section_list_tail = NULL;
        }
        else if (Object->section_list->next == NULL) {
          /* there is one section in the list */
          Object->section_list_tail = Object->section_list;
        }
      }
      else {
        previous->next = list->next;

        if (list->next == NULL) {
          /* The last section in the list is being removed, so update the tail. */
          Object->section_list_tail = previous;
        }
      }
      break;
    }
    previous = list;
    list = list->next;
  }

  (Object->num_sections)--;

  /* FIXME: gp_coffgen_free_section(removed); */

  return removed;
}

/*------------------------------------------------------------------------------------------------*/

gp_reloc_type *
gp_coffgen_add_reloc(gp_section_type *Section)
{
  gp_reloc_type *new;

  /* allocate memory for the relocation */
  new = (gp_reloc_type *)GP_Calloc(1, sizeof(gp_reloc_type));

  if (Section->relocation_list == NULL) {
    /* the list is empty */
    Section->relocation_list = new;
  }
  else {
    Section->relocation_list_tail->next = new;
    new->prev = Section->relocation_list_tail;
  }

  Section->relocation_list_tail = new;
  (Section->num_reloc)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

gp_reloc_type *
gp_coffgen_del_reloc(gp_section_type *Section, gp_reloc_type *Relocation)
{
  gp_symbol_type *symbol;

  if ((Section->relocation_list == NULL) || (Section->num_reloc == 0)) {
    return NULL;
  }

  if (Section->relocation_list == Relocation) {
    /* This is first element of the list. */
    Section->relocation_list = Relocation->next;
  }
  else {
    Relocation->prev->next = Relocation->next;
  }

  if (Section->relocation_list_tail == Relocation) {
    /* This is last element of the list. */
    Section->relocation_list_tail = Relocation->prev;
  }
  else {
    Relocation->next->prev = Relocation->prev;
  }

  (Section->num_reloc)--;

  symbol = Relocation->symbol;

  if (symbol->num_reloc_link > 0) {
    (symbol->num_reloc_link)--;
  }
  else {
    gp_warning("Number of relocation links of symbol is zero: '%s'", symbol->name);
  }

  return Relocation;
}

/*------------------------------------------------------------------------------------------------*/

const char *
gp_coffgen_reloc_type_to_str(uint16_t Type)
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

  if (Type >= ARRAY_SIZE(type_str)) {
    Type = 0;
  }

  return type_str[Type];
}

/*------------------------------------------------------------------------------------------------*/

gp_linenum_type *
gp_coffgen_add_linenum(gp_section_type *Section)
{
  gp_linenum_type *new;

  /* allocate memory for the relocation */
  new = (gp_linenum_type *)GP_Calloc(1, sizeof(gp_linenum_type));

  if (Section->line_number_list == NULL) {
    /* the list is empty */
    Section->line_number_list = new;
  }
  else {
    Section->line_number_list_tail->next = new;
    new->prev = Section->line_number_list_tail;
  }

  Section->line_number_list_tail = new;
  (Section->num_lineno)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

gp_linenum_type *
gp_coffgen_del_linenum(gp_section_type *Section, gp_linenum_type *Linenum)
{
  if ((Section->line_number_list == NULL) || (Section->num_lineno == 0)) {
    return NULL;
  }

  if (Section->line_number_list == Linenum) {
    /* This is first element of the list. */
    Section->line_number_list = Linenum->next;
  }
  else {
    Linenum->prev->next = Linenum->next;
  }

  if (Section->line_number_list_tail == Linenum) {
    /* This is last element of the list. */
    Section->line_number_list_tail = Linenum->prev;
  }
  else {
    Linenum->next->prev = Linenum->prev;
  }

  (Section->num_lineno)--;

  return Linenum;
}

/*------------------------------------------------------------------------------------------------*/

gp_linenum_type *
gp_coffgen_find_linenum_by_address(gp_section_type *Section, unsigned int Address)
{
  gp_linenum_type *linenum;

  if ((Section->line_number_list == NULL) || (Section->num_lineno == 0)) {
    return NULL;
  }

  linenum = Section->line_number_list;
  while (linenum != NULL) {
    if (linenum->address == Address) {
      return linenum;
    }
    linenum = linenum->next;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

gp_linenum_type *
gp_coffgen_del_linenum_by_address(gp_section_type *Section, unsigned int Address)
{
  gp_linenum_type *linenum;

  linenum = gp_coffgen_find_linenum_by_address(Section, Address);

  if (linenum == NULL) {
    return NULL;
  }

  return gp_coffgen_del_linenum(Section, linenum);
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_coffgen_del_linenum_by_address_area(gp_section_type *Section, unsigned int Address_start,
                                       unsigned int Address_end)
{
  gp_linenum_type *linenum;

  if ((Section->line_number_list == NULL) || (Section->num_lineno == 0)) {
    return false;
  }

  linenum = Section->line_number_list;
  while ((linenum != NULL) && (linenum->address <= Address_end)) {
    if (linenum->address >= Address_start) {
      gp_coffgen_del_linenum(Section, linenum);
    }
    linenum = linenum->next;
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

gp_symbol_type *
gp_coffgen_find_symbol(gp_object_type *Object, const char *Name)
{
  gp_symbol_type *current;
  gp_symbol_type *found;

  if (Object == NULL) {
    return NULL;
  }

  current = Object->symbol_list;
  found   = NULL;

  while (current != NULL) {
    if ((current->class != C_SECTION) && (current->name != NULL) &&
        (strcmp(current->name, Name) == 0)) {
      found = current;
      break;
    }
    current = current->next;
  }

  return found;
}

/*------------------------------------------------------------------------------------------------*/

gp_symbol_type *
gp_coffgen_find_section_symbol(gp_object_type *Object, const char *Name)
{
  gp_symbol_type *current;
  gp_symbol_type *found;

  if (Object == NULL) {
    return NULL;
  }

  current = Object->symbol_list;
  found   = NULL;

  while (current != NULL) {
    if ((current->class == C_SECTION) && (current->name != NULL) &&
        (strcmp(current->name, Name) == 0)) {
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
gp_coffgen_find_symbol_section_value(gp_object_type *Object, const char *Section_name, long Value)
{
  gp_symbol_type *current;
  gp_symbol_type *found;

  if ((Object == NULL) || (Section_name == NULL)) {
    return NULL;
  }

  current = Object->symbol_list;
  found   = NULL;

  while (current != NULL) {
    if ((current->class != C_SECTION) && (current->class != C_FILE) &&
        (current->section_name != NULL) &&
        (strcmp(current->section_name, Section_name) == 0) &&
        (current->value == Value)) {
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

  if (Object->symbol_list == NULL) {
    /* the list is empty */
    Object->symbol_list = new;
  }
  else {
    Object->symbol_list_tail->next = new;
  }

  Object->symbol_list_tail = new;
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

  list     = Object->symbol_list;
  previous = NULL;
  removed  = NULL;

  while (list != NULL) {
    if (list == Symbol) {
      removed = Symbol;

      if (previous == NULL) {
        /* removing the first symbol in the list */
        Object->symbol_list = list->next;
        if (Object->symbol_list == NULL) {
          /* there are no symbols in the list */
          Object->symbol_list_tail = NULL;
        }
        else if (Object->symbol_list->next == NULL) {
          /* there is one symbol in the list */
          Object->symbol_list_tail = Object->symbol_list;
        }
      }
      else {
        previous->next = list->next;

        if (list->next == NULL) {
          /* The last symbol in the list is being removed, so update the tail. */
          Object->symbol_list_tail = previous;
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
gp_coffgen_make_block_section(unsigned int Number)
{
  gp_section_type *new;
  unsigned int     i;

  if (Number == 0) {
    return NULL;
  }

  /* allocate memory for the sections */
  new = (gp_section_type *)GP_Calloc(Number, sizeof(gp_section_type));

  /* don't process the last entry */
  Number--;

  /* initialize the pointers to create the linked list */
  for (i = 0; i < Number; i++) {
    new[i].next = &new[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of relocations */

gp_reloc_type *
gp_coffgen_make_block_reloc(unsigned int Number)
{
  gp_reloc_type *new;
  unsigned int   i;

  if (Number == 0) {
    return NULL;
  }

  /* allocate memory for the relocations */
  new = (gp_reloc_type *)GP_Calloc(Number, sizeof(gp_reloc_type));

  /* don't process the last entry */
  Number--;

  /* initialize the pointers to create the linked list */
  for (i = 0; i < Number; i++) {
    new[i+1].prev = &new[i];
    new[i].next   = &new[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of line numbers */

gp_linenum_type *
gp_coffgen_make_block_linenum(unsigned int Number)
{
  gp_linenum_type *new;
  unsigned int     i;

  if (Number == 0) {
    return NULL;
  }

  /* allocate memory for the symbol */
  new = (gp_linenum_type *)GP_Calloc(Number, sizeof(gp_linenum_type));

  /* don't process the last entry */
  Number--;

  /* initialize the pointers to create the linked list */
  for (i = 0; i < Number; i++) {
    new[i+1].prev = &new[i];
    new[i].next   = &new[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of symbols */

gp_symbol_type *
gp_coffgen_make_block_symbol(unsigned int Number)
{
  gp_symbol_type *new;
  unsigned int    i;

  if (Number == 0) {
    return NULL;
  }

  /* allocate memory for the symbols */
  new = (gp_symbol_type *)GP_Calloc(sizeof(*new) * Number, sizeof(gp_symbol_type));

  /* don't process the last entry */
  Number--;

  /* initialize the pointers to create the linked list */
  for (i = 0; i < Number; i++) {
    new[i].next = &new[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of auxiliary symbols */

gp_aux_type *
gp_coffgen_make_block_aux(unsigned int Number)
{
  gp_aux_type  *new;
  unsigned int  i;

  if (Number == 0) {
    return NULL;
  }

  /* allocate memory for the symbols */
  new = (gp_aux_type *)GP_Calloc(sizeof(*new) * Number, sizeof(gp_aux_type));

  /* don't process the last entry */
  Number--;

  /* initialize the pointers to create the linked list */
  for (i = 0; i < Number; i++) {
    new[i].type = AUX_NONE;
    new[i].next = &new[i + 1];
  }

  /* assign the tail of the list */
  new[Number].next = NULL;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_coffgen_free_section(gp_section_type *Section)
{
  gp_reloc_type   *relocation;
  gp_linenum_type *line_number;
  gp_reloc_type   *old_relocation;
  gp_linenum_type *old_line_number;

  i_memory_free(Section->data);

  relocation = Section->relocation_list;
  while (relocation != NULL) {
    old_relocation = relocation;
    relocation = relocation->next;
    free(old_relocation);
  }

  line_number = Section->line_number_list;
  while (line_number != NULL) {
    old_line_number = line_number;
    line_number = line_number->next;
    free(old_line_number);
  }

  if (Section->line_numbers_array != NULL) {
    free(Section->line_numbers_array);
  }

  free(Section->name);
  free(Section);
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_coffgen_free_symbol(gp_symbol_type *Symbol)
{
  gp_aux_type  *aux;
  gp_aux_type  *old_aux;
  unsigned int  num_auxsym;

  if (Symbol == NULL) {
    return 0;
  }

  /* free the auxilary symbols */
  aux        = Symbol->aux_list;
  num_auxsym = Symbol->num_auxsym;

  while (aux != NULL) {
    old_aux = aux;
    aux = aux->next;
    free(old_aux);
  }

  free(Symbol->name);
  free(Symbol);

  return num_auxsym;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_coffgen_free_object(gp_object_type *Object)
{
  gp_section_type *section;
  gp_symbol_type  *symbol;
  gp_section_type *old_section;
  gp_symbol_type  *old_symbol;

  if (Object == NULL) {
    return false;
  }

  free(Object->filename);

  section = Object->section_list;
  while (section != NULL) {
    old_section = section;
    section     = section->next;
    gp_coffgen_free_section(old_section);
  }

  symbol = Object->symbol_list;
  while (symbol != NULL) {
    old_symbol = symbol;
    symbol     = symbol->next;
    gp_coffgen_free_symbol(old_symbol);
  }

  if (Object->symbol_hashtable != NULL) {
    free(Object->symbol_hashtable);
  }

  free(Object);

  return true;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_coffgen_determine_aux_symbol(const gp_symbol_type *Symbol)
{
  unsigned int aux_type = AUX_NONE;

  if (strcasecmp(".direct", Symbol->name) == 0) {
    return AUX_DIRECT;
  }

  if (strcasecmp(".ident", Symbol->name) == 0) {
    return AUX_IDENT;
  }

  if ((Symbol->derived_type & 7) == DT_FCN) {
    return AUX_FCN;
  }

  switch (Symbol->class) {
    case C_FILE:
      aux_type = AUX_FILE;
      break;

    case C_SECTION:
      aux_type = AUX_SCN;
      break;

    case C_BLOCK:
    case C_FCN:
      aux_type = (Symbol->name[1] == 'b') ? AUX_BOBF : AUX_EOBF;
      break;

    default:
      aux_type = AUX_NONE;
  }

  return aux_type;
}

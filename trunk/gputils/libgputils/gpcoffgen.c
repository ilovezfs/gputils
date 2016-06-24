/* GNU PIC general coff functions
   Copyright (C) 2001, 2002, 2003, 2004, 2005
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

static unsigned int object_serial_id  = 0;
static unsigned int section_serial_id = 0;

/*------------------------------------------------------------------------------------------------*/

gp_object_type *
gp_coffgen_new_object(const char *File_name)
{
  gp_object_type *object;

  /* allocate memory for the object file */
  object = (gp_object_type *)GP_Calloc(1, sizeof(gp_object_type));

  /* initialize the object */
  object->filename  = GP_Strdup(File_name);
  object->processor = gp_find_processor("generic");
  object->class     = PROC_CLASS_GENERIC;
  object->time      = (uint32_t)time(NULL);
  object->serial_id = object_serial_id++;

  return object;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_coffgen_transfer_object_data(gp_object_type *Receiver, gp_object_type *Sender)
{
  if ((Receiver == NULL) || (Sender == NULL)) {
    return;
  }

  if (Sender->num_sections != 0) {
    if (Receiver->num_sections == 0) {
      /* The object has no sections. */
      Receiver->section_list = Sender->section_list;
    }
    else {
      /* Append the sections from the second object to the first. */
      Receiver->section_list_tail->next = Sender->section_list;
      Sender->section_list->prev        = Receiver->section_list_tail;
    }

    Receiver->section_list_tail  = Sender->section_list_tail;
    Receiver->num_sections      += Sender->num_sections;

    /* In the "Sender" object will not stay reference onto any section.
       If exists block of sections, this will remain there. */
    Sender->section_list      = NULL;
    Sender->section_list_tail = NULL;
    Sender->num_sections      = 0;
  }

  if (Sender->num_symbols != 0) {
    if (Receiver->num_symbols == 0) {
      /* The object has no symbols. */
      Receiver->symbol_list = Sender->symbol_list;
    }
    else {
      /* Append the symbols from the second object to the first. */
      Receiver->symbol_list_tail->next = Sender->symbol_list;
      Sender->symbol_list->prev        = Receiver->symbol_list_tail;
    }

    Receiver->symbol_list_tail  = Sender->symbol_list_tail;
    Receiver->num_symbols      += Sender->num_symbols;

    /* In the "Sender" object will not stay reference onto any symbol.
       If exists block of symbols, this will remain there. */
    Sender->symbol_list      = NULL;
    Sender->symbol_list_tail = NULL;
    Sender->num_symbols      = 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_coffgen_update_all_object_id(gp_object_type *Object)
{
  gp_section_type *section;
  gp_symbol_type  *symbol;
  unsigned int     id;

  if (Object == NULL) {
    return false;
  }

  id = Object->serial_id;
  if ((Object->section_list != NULL) && (Object->num_sections > 0)) {
    section = Object->section_list;
    while (section != NULL) {
      section->object_id = id;
      section            = section->next;
    }
  }

  if ((Object->symbol_list != NULL) && (Object->num_symbols > 0)) {
    symbol = Object->symbol_list;
    while (symbol != NULL) {
      symbol->object_id = id;
      symbol            = symbol->next;
    }
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_find_section(gp_object_type *Object, gp_section_type *Start, const char *Name)
{
  gp_section_type *current;

  if ((Object == NULL) || (Start == NULL)) {
    return NULL;
  }

  current = Start;

  while (current != NULL) {
    if ((current->name != NULL) && (strcmp(current->name, Name) == 0)) {
      return current;
    }
    current = current->next;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_new_section(const char *Name, MemBlock_t *Data)
{
  gp_section_type *new;

  /* allocate memory for the section */
  new = (gp_section_type *)GP_Calloc(1, sizeof(gp_section_type));

  /* initialize section */
  new->name      = GP_Strdup(Name);
  new->data      = (Data != NULL) ? Data : i_memory_create();
  new->serial_id = section_serial_id++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of section -- gpreadobj.c */

gp_section_type *
gp_coffgen_make_block_section(gp_object_type *Object)
{
  gp_section_type **ptr_array;
  unsigned int      n_sections;
  unsigned int      id;
  unsigned int      i;

  if (Object->section_list != NULL) {
    gp_error("'%s': The list of sections already exists, can not be created again.", Object->filename);
    assert(0);
  }

  if ((n_sections = Object->num_sections) == 0) {
    return NULL;
  }

  ptr_array = (gp_section_type **)GP_Malloc(n_sections * sizeof(gp_section_type *));

  id = Object->serial_id;
  for (i = 0; i < n_sections; i++) {
    ptr_array[i] = (gp_section_type *)GP_Calloc(1, sizeof(gp_section_type));
    ptr_array[i]->object_id = id;
    ptr_array[i]->serial_id = section_serial_id++;
  }

  /* Do not process the last item. */
  n_sections--;

  /* Initialize the pointers to create the linked list. */
  for (i = 0; i < n_sections; i++) {
    ptr_array[i + 1]->prev = ptr_array[i];
    ptr_array[i]->next     = ptr_array[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  Object->section_ptr_array = ptr_array;
  Object->section_list      = ptr_array[0];
  Object->section_list_tail = ptr_array[n_sections];

  return Object->section_list;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_add_exists_section(gp_object_type *Object, gp_section_type *Section)
{
  if (Object == NULL) {
    return NULL;
  }

  if (Object->section_list == NULL) {
    /* the list is empty */
    Object->section_list = Section;
  }
  else {
    /* append the new section to the end of the list */
    Section->prev = Object->section_list_tail;
    Object->section_list_tail->next = Section;
  }

  Object->section_list_tail = Section;
  Section->object_id        = Object->serial_id;
  (Object->num_sections)++;

  return Section;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_insert_section(gp_object_type *Object, gp_section_type *Ancestor, gp_section_type *Following)
{
  gp_section_type *next_sect;

  if (Object == NULL) {
    return NULL;
  }

  if (Ancestor->object_id != Object->serial_id) {
    gp_error("The '%s'{%u} section does not belong to this object: '%s'{%u}",
             Ancestor->name, Ancestor->object_id, Object->filename, Object->serial_id);
    assert(0);
  }

  if (Object->section_list_tail == Ancestor) {
    /* This is last element of the list. */
    Ancestor->next  = Following;
    Following->prev = Ancestor;
    Following->next = NULL;
    Object->section_list_tail = Following;
  }
  else {
    /*
      The "Following" can not be the first item in the "section_list".

         +----------+   +----------+
      <--|prev      |<--|prev      |<--
         | Ancestor |   |   Next   |
      -->|      next|-->|      next|-->
         +----------+   +----------+

         +----------+   +----------+   +----------+
      <--|prev      |<--|prev      |<--|prev      |<--
         | Ancestor |   |Following |   |   Next   |
      -->|      next|-->|      next|-->|      next|-->
         +----------+   +----------+   +----------+
    */
    next_sect       = Ancestor->next;
    Ancestor->next  = Following;
    Following->prev = Ancestor;
    Following->next = next_sect;

    if (next_sect != NULL) {
      /* The "Next" section exists. */
      next_sect->prev = Following;
    }
  }

  Following->object_id = Object->serial_id;
  (Object->num_sections)++;

  return Following;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type *
gp_coffgen_add_section(gp_object_type *Object, const char *Name, MemBlock_t *Data)
{
  if (Object == NULL) {
    return NULL;
  }

  return gp_coffgen_add_exists_section(Object, gp_coffgen_new_section(Name, Data));
}

/*------------------------------------------------------------------------------------------------*/

void
gp_coffgen_transfer_section_data(gp_section_type *Receiver, gp_section_type *Sender)
{
  if ((Receiver == NULL) || (Sender == NULL)) {
    return;
  }

  if (Sender->num_reloc != 0) {
    /* Append the relocations from the "Sender" section to the "Receiver". */
    if (Receiver->num_reloc == 0) {
      Receiver->relocation_list = Sender->relocation_list;
    }
    else {
      Receiver->relocation_list_tail->next = Sender->relocation_list;
      Sender->relocation_list->prev        = Receiver->relocation_list_tail;
    }

    Receiver->relocation_list_tail = Sender->relocation_list_tail;
    Receiver->num_reloc += Sender->num_reloc;

    /* In the "Sender" section will not stay reference onto any relocation.
       If exists block of relocations, this will remain there. */
    Sender->relocation_list      = NULL;
    Sender->relocation_list_tail = NULL;
    Sender->num_reloc            = 0;
  }

  if (Sender->num_lineno != 0) {
    /* Append the line numbers from the "Sender" section to the "Receiver". */
    if (Receiver->num_lineno == 0) {
      Receiver->line_number_list = Sender->line_number_list;
    }
    else {
      Receiver->line_number_list_tail->next = Sender->line_number_list;
      Sender->line_number_list->prev        = Receiver->line_number_list_tail;
    }

    Receiver->line_number_list_tail = Sender->line_number_list_tail;
    Receiver->num_lineno += Sender->num_lineno;

    /* In the "Sender" section will not stay reference onto any line number.
       If exists block of line numbers, this will remain there. */
    Sender->line_number_list      = NULL;
    Sender->line_number_list_tail = NULL;
    Sender->num_lineno            = 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_coffgen_update_all_section_id(gp_section_type *Section)
{
  gp_reloc_type   *relocation;
  gp_linenum_type *line_number;
  unsigned int     id;

  if (Section == NULL) {
    return false;
  }

  id = Section->serial_id;

  if ((Section->relocation_list != NULL) && (Section->num_reloc > 0)) {
    relocation = Section->relocation_list;
    while (relocation != NULL) {
      relocation->section_id = id;
      relocation             = relocation->next;
    }
  }

  if ((Section->line_number_list != NULL) && (Section->num_lineno > 0)) {
    line_number = Section->line_number_list;
    while (line_number != NULL) {
      line_number->section_id = id;
      line_number             = line_number->next;
    }
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_coffgen_put_reserve_section_symbols(gp_object_type *Object, gp_section_type *Section)
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
      gp_coffgen_put_reserve_symbol(Object, symbol);
    }
  }
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
gp_coffgen_put_reserve_section(gp_object_type *Object, gp_section_type *Section)
{
  if ((Object->section_list == NULL) || (Object->num_sections == 0)) {
    return NULL;
  }

  if (Section->object_id != Object->serial_id) {
    gp_error("The '%s'{%u} section does not belong to this object: '%s'{%u}",
             Section->name, Section->object_id, Object->filename, Object->serial_id);
    assert(0);
  }

  if (Object->section_list == Section) {
    /* This is first element of the list. */
    Object->section_list = Section->next;
  }
  else {
    Section->prev->next = Section->next;
  }

  if (Object->section_list_tail == Section) {
    /* This is last element of the list. */
    Object->section_list_tail = Section->prev;
  }
  else {
    Section->next->prev = Section->prev;
  }

  (Object->num_sections)--;

  /* Put in the reserved linked list. */
  if (Object->reserved_section_list == NULL) {
    /* the list is empty */
    Object->reserved_section_list = Section;
    Section->prev = NULL;
  }
  else {
    Section->prev = Object->reserved_section_list_tail;
    Object->reserved_section_list_tail->next = Section;
    Section->next = NULL;
  }

  Object->reserved_section_list_tail = Section;
  return Section;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_coffgen_del_section(gp_object_type *Object, gp_section_type *Section)
{
  if ((Object->section_list == NULL) || (Object->num_sections == 0)) {
    return false;
  }

  if (Section->object_id != Object->serial_id) {
    gp_error("The '%s'{%u} section does not belong to this object: '%s'{%u}",
             Section->name, Section->object_id, Object->filename, Object->serial_id);
    assert(0);
  }

  if (Object->section_list == Section) {
    /* This is first element of the list. */
    Object->section_list = Section->next;
  }
  else {
    Section->prev->next = Section->next;
  }

  if (Object->section_list_tail == Section) {
    /* This is last element of the list. */
    Object->section_list_tail = Section->prev;
  }
  else {
    Section->next->prev = Section->prev;
  }

  (Object->num_sections)--;
  gp_coffgen_free_section(Section);
  return true;
}

/*------------------------------------------------------------------------------------------------*/

static int
_section_cmp(const void *P0, const void *P1)
{
  const gp_section_type *s0 = *(const gp_section_type **)P0;
  const gp_section_type *s1 = *(const gp_section_type **)P1;
  uint32_t               a0 = s0->address;
  uint32_t               a1 = s1->address;

  if (a0 < a1) {
    return -1;
  }

  if (a0 > a1) {
    return 1;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

gp_section_type **
gp_coffgen_make_section_array(gp_object_type *Object, unsigned int *Num_sections,
                              unsigned int Page_addr, uint32_t Flags)
{
  proc_class_t      class;
  gp_section_type  *section;
  gp_section_type **array;
  unsigned int      i;
  unsigned int      page;
  unsigned int      n_sections;

  if ((Object == NULL) || (Num_sections == NULL)) {
    return NULL;
  }

  class = Object->class;

  n_sections = 0;
  section    = Object->section_list;
  while (section != NULL) {
    FlagClr(section->opt_flags, OPT_FLAGS_GPCOFFGEN_MODULE);
    page = gp_processor_page_addr(class, gp_processor_byte_to_org(class, section->address));

    if ((page == Page_addr) && (FlagsIsNotAllClr(section->flags, Flags))) {
      ++n_sections;
      FlagSet(section->opt_flags, OPT_FLAGS_GPCOFFGEN_MODULE);
    }
    section = section->next;
  }

  if (n_sections == 0) {
    return NULL;
  }

  array = (gp_section_type **)GP_Malloc(n_sections * sizeof(gp_section_type *));

  i       = 0;
  section = Object->section_list;
  while (section != NULL) {
    if (FlagIsSet(section->opt_flags, OPT_FLAGS_GPCOFFGEN_MODULE)) {
      array[i] = section;
      ++i;
    }
    section = section->next;
  }

  if (n_sections > 1) {
    qsort(array, n_sections, sizeof(gp_section_type *), _section_cmp);
  }

  *Num_sections = n_sections;
  return array;
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

/* allocate a block of symbols -- gpreadobj.c */

gp_symbol_type *
gp_coffgen_make_block_symbol(gp_object_type *Object)
{
  gp_symbol_type **ptr_array;
  unsigned int     n_symbols;
  unsigned int     id;
  unsigned int     i;

  if (Object->symbol_list != NULL) {
    gp_error("'%s': The list of symbols already exists, can not be created again.", Object->filename);
    assert(0);
  }

  if ((n_symbols = Object->num_symbols) == 0) {
    return NULL;
  }

  ptr_array = (gp_symbol_type **)GP_Malloc(n_symbols * sizeof(gp_symbol_type *));

  id = Object->serial_id;
  for (i = 0; i < n_symbols; i++) {
    ptr_array[i] = (gp_symbol_type *)GP_Calloc(1, sizeof(gp_symbol_type));
    ptr_array[i]->object_id = id;
  }

  /* Do not process the last item. */
  n_symbols--;

  /* Initialize the pointers to create the linked list. */
  for (i = 0; i < n_symbols; i++) {
    ptr_array[i + 1]->prev = ptr_array[i];
    ptr_array[i]->next     = ptr_array[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  Object->symbol_ptr_array = ptr_array;
  Object->symbol_list      = ptr_array[0];
  Object->symbol_list_tail = ptr_array[n_symbols];

  return Object->symbol_list;
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
    new->prev = Object->symbol_list_tail;
    Object->symbol_list_tail->next = new;
  }

  Object->symbol_list_tail = new;
  new->object_id           = Object->serial_id;
  (Object->num_symbols)++;

  return new;
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
    /* append the new aux symbol to the end of the list */
    new->prev = Symbol->aux_list_tail;
    Symbol->aux_list_tail->next = new;
  }

  Symbol->aux_list_tail = new;

  (Symbol->num_auxsym)++;
  (Object->num_symbols)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of auxiliary symbols -- gpreadobj.c */

gp_aux_type *
gp_coffgen_make_block_aux(gp_symbol_type *Symbol)
{
  gp_aux_type **ptr_array;
  unsigned int  n_auxsyms;
  unsigned int  i;

  if (Symbol->aux_list != NULL) {
    gp_error("'%s': The list of aux symbols already exists, can not be created again.", Symbol->name);
    assert(0);
  }

  if ((n_auxsyms = Symbol->num_auxsym) == 0) {
    return NULL;
  }

  ptr_array = (gp_aux_type **)GP_Malloc(n_auxsyms * sizeof(gp_aux_type *));

  for (i = 0; i < n_auxsyms; i++) {
    ptr_array[i] = (gp_aux_type *)GP_Calloc(1, sizeof(gp_aux_type));
#if (AUX_NONE != 0)
    ptr_array[i]->type = AUX_NONE;
#endif
  }

  /* Do not process the last item. */
  n_auxsyms--;

  /* Initialize the pointers to create the linked list. */
  for (i = 0; i < n_auxsyms; i++) {
    ptr_array[i + 1]->prev = ptr_array[i];
    ptr_array[i]->next     = ptr_array[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  Symbol->aux_list      = ptr_array[0];
  Symbol->aux_list_tail = ptr_array[n_auxsyms];

  free(ptr_array);

  return Symbol->aux_list;
}

/*------------------------------------------------------------------------------------------------*/

gp_symbol_type *
gp_coffgen_put_reserve_symbol(gp_object_type *Object, gp_symbol_type *Symbol)
{
  if ((Object->symbol_list == NULL) || (Object->num_symbols == 0)) {
    return NULL;
  }

  if (Symbol->object_id != Object->serial_id) {
    gp_error("The '%s'{%u} symbol does not belong to this object: '%s'{%u}",
             Symbol->name, Symbol->object_id, Object->filename, Object->serial_id);
    assert(0);
  }

  if (Object->symbol_list == Symbol) {
    /* This is first element of the list. */
    Object->symbol_list = Symbol->next;
  }
  else {
    Symbol->prev->next = Symbol->next;
  }

  if (Object->symbol_list_tail == Symbol) {
    /* This is last element of the list. */
    Object->symbol_list_tail = Symbol->prev;
  }
  else {
    Symbol->next->prev = Symbol->prev;
  }

  Object->num_symbols -= Symbol->num_auxsym + 1;

  /* Put in the reserved linked list. */
  if (Object->reserved_symbol_list == NULL) {
    /* the list is empty */
    Object->reserved_symbol_list = Symbol;
    Symbol->prev = NULL;
  }
  else {
    Symbol->prev = Object->reserved_symbol_list_tail;
    Object->reserved_symbol_list_tail->next = Symbol;
    Symbol->next = NULL;
  }

  Object->reserved_symbol_list_tail = Symbol;
  return Symbol;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_coffgen_del_symbol(gp_object_type *Object, gp_symbol_type *Symbol)
{
  if ((Object->symbol_list == NULL) || (Object->num_symbols == 0)) {
    return false;
  }

  if (Symbol->object_id != Object->serial_id) {
    gp_error("The '%s'{%u} symbol does not belong to this object: '%s'{%u}",
             Symbol->name, Symbol->object_id, Object->filename, Object->serial_id);
    assert(0);
  }

  if (Object->symbol_list == Symbol) {
    /* This is first element of the list. */
    Object->symbol_list = Symbol->next;
  }
  else {
    Symbol->prev->next = Symbol->next;
  }

  if (Object->symbol_list_tail == Symbol) {
    /* This is last element of the list. */
    Object->symbol_list_tail = Symbol->prev;
  }
  else {
    Symbol->next->prev = Symbol->prev;
  }

  Object->num_symbols -= gp_coffgen_free_symbol(Symbol) + 1;
  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate a block of relocations -- gpreadobj.c */

gp_reloc_type *
gp_coffgen_make_block_reloc(gp_section_type *Section)
{
  gp_reloc_type **ptr_array;
  unsigned int    n_relocations;
  unsigned int    id;
  unsigned int    i;

  if (Section->relocation_list != NULL) {
    gp_error("'%s': The list of relocations already exists, can not be created again.", Section->name);
    assert(0);
  }

  if ((n_relocations = Section->num_reloc) == 0) {
    return NULL;
  }

  ptr_array = (gp_reloc_type **)GP_Malloc(n_relocations * sizeof(gp_reloc_type *));

  id = Section->serial_id;
  for (i = 0; i < n_relocations; i++) {
    ptr_array[i] = (gp_reloc_type *)GP_Calloc(1, sizeof(gp_reloc_type));
    ptr_array[i]->section_id = id;
  }

  /* Do not process the last item. */
  n_relocations--;

  /* Initialize the pointers to create the linked list. */
  for (i = 0; i < n_relocations; i++) {
    ptr_array[i + 1]->prev = ptr_array[i];
    ptr_array[i]->next     = ptr_array[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  Section->relocation_list      = ptr_array[0];
  Section->relocation_list_tail = ptr_array[n_relocations];

  free(ptr_array);

  return Section->relocation_list;
}

/*------------------------------------------------------------------------------------------------*/

gp_reloc_type *
gp_coffgen_add_reloc(gp_section_type *Section)
{
  gp_reloc_type *new;

  /* allocate memory for the relocation */
  new = (gp_reloc_type *)GP_Calloc(1, sizeof(gp_reloc_type));
  new->section_id = Section->serial_id;

  if (Section->relocation_list == NULL) {
    /* the list is empty */
    Section->relocation_list = new;
  }
  else {
    new->prev = Section->relocation_list_tail;
    Section->relocation_list_tail->next = new;
  }

  Section->relocation_list_tail = new;
  (Section->num_reloc)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_coffgen_del_reloc(gp_section_type *Section, gp_reloc_type *Relocation)
{
  gp_symbol_type *symbol;

  if ((Section->relocation_list == NULL) || (Section->num_reloc == 0)) {
    return false;
  }

  if (Relocation->section_id != Section->serial_id) {
    gp_error("The relocation{%u} does not belong to this section: '%s'{%u}",
             Relocation->section_id, Section->name, Section->serial_id);
    assert(0);
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

  free(Relocation);
  return true;
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

/* allocate a block of line numbers -- gpreadobj.c */

gp_linenum_type *
gp_coffgen_make_block_linenum(gp_section_type *Section)
{
  gp_linenum_type **ptr_array;
  unsigned int      n_line_numbers;
  unsigned int      id;
  unsigned int      i;

  if (Section->line_number_list != NULL) {
    gp_error("'%s': The list of line numbers already exists, can not be created again.", Section->name);
    assert(0);
  }

  if ((n_line_numbers = Section->num_lineno) == 0) {
    return NULL;
  }

  ptr_array = (gp_linenum_type **)GP_Malloc(n_line_numbers * sizeof(gp_linenum_type *));

  id = Section->serial_id;
  for (i = 0; i < n_line_numbers; i++) {
    ptr_array[i] = (gp_linenum_type *)GP_Calloc(1, sizeof(gp_linenum_type));
    ptr_array[i]->section_id = id;
  }

  /* Do not process the last item. */
  n_line_numbers--;

  /* Initialize the pointers to create the linked list. */
  for (i = 0; i < n_line_numbers; i++) {
    ptr_array[i + 1]->prev = ptr_array[i];
    ptr_array[i]->next     = ptr_array[i + 1];
  }

  /* The head and tail value already NULL. (GP_Calloc) */

  Section->line_number_list      = ptr_array[0];
  Section->line_number_list_tail = ptr_array[n_line_numbers];

  free(ptr_array);

  return Section->line_number_list;
}

/*------------------------------------------------------------------------------------------------*/

gp_linenum_type *
gp_coffgen_add_linenum(gp_section_type *Section)
{
  gp_linenum_type *new;

  /* allocate memory for the line number */
  new = (gp_linenum_type *)GP_Calloc(1, sizeof(gp_linenum_type));
  new->section_id = Section->serial_id;

  if (Section->line_number_list == NULL) {
    /* the list is empty */
    Section->line_number_list = new;
  }
  else {
    new->prev = Section->line_number_list_tail;
    Section->line_number_list_tail->next = new;
  }

  Section->line_number_list_tail = new;
  (Section->num_lineno)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_coffgen_del_linenum(gp_section_type *Section, gp_linenum_type *Linenum)
{
  if ((Section->line_number_list == NULL) || (Section->num_lineno == 0)) {
    return false;
  }

  if (Linenum->section_id != Section->serial_id) {
    gp_error("The line number{%u} does not belong to this section: '%s'{%u}",
             Linenum->section_id, Section->name, Section->serial_id);
    assert(0);
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

  free(Linenum);
  return true;
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

gp_boolean
gp_coffgen_del_linenum_by_address(gp_section_type *Section, unsigned int Address)
{
  gp_linenum_type *linenum;

  linenum = gp_coffgen_find_linenum_by_address(Section, Address);

  if (linenum == NULL) {
    return false;
  }

  return gp_coffgen_del_linenum(Section, linenum);
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_coffgen_del_linenum_by_address_area(gp_section_type *Section, unsigned int Address_start,
                                       unsigned int Address_end)
{
  gp_linenum_type *linenum;
  gp_linenum_type *next;
  unsigned int     num;

  if ((Section->line_number_list == NULL) || (Section->num_lineno == 0)) {
    return false;
  }

  num     = 0;
  linenum = Section->line_number_list;
  while ((linenum != NULL) && (linenum->address <= Address_end)) {
    next = linenum->next;

    if (linenum->address >= Address_start) {
      num += (gp_coffgen_del_linenum(Section, linenum) != false) ? 1 : 0;
    }

    linenum = next;
  }

  return num;
}

/*------------------------------------------------------------------------------------------------*/

static int
_linenum_cmp(const void *P0, const void *P1)
{
  const gp_linenum_type *l0   = *(const gp_linenum_type **)P0;
  const gp_linenum_type *l1   = *(const gp_linenum_type **)P1;
  const gp_symbol_type  *sym0 = l0->symbol;
  const gp_symbol_type  *sym1 = l1->symbol;
  unsigned int           num0 = l0->line_number;
  unsigned int           num1 = l1->line_number;

  if (sym0 < sym1) {
    return -1;
  }

  if (sym0 > sym1) {
    return 1;
  }

  if (num0 < num1) {
    return -1;
  }

  if (num0 > num1) {
    return 1;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

/* Use gplink/glink.c */

void
gp_coffgen_make_linenum_arrays(gp_object_type *Object)
{
  gp_section_type  *section;
  gp_linenum_type  *linenum;
  gp_linenum_type **array;
  unsigned int      n_linenums;
  unsigned int      prev_num;
  unsigned int      i;

  section = Object->section_list;
  while (section != NULL) {
    n_linenums = section->num_lineno;
    array      = (gp_linenum_type **)GP_Malloc(n_linenums * sizeof(gp_linenum_type *));
    i          = 0;
    prev_num   = (unsigned int)(-1);
    linenum    = section->line_number_list;
    while (linenum != NULL) {
      /* From among identical line numbers only places the first in the array. */
      if (prev_num != linenum->line_number) {
        array[i] = linenum;
        ++i;
        prev_num = linenum->line_number;
      }

      linenum = linenum->next;
    }

    /* Reduces the required memory size. */
    array = (gp_linenum_type **)GP_Realloc(array, i * sizeof(gp_linenum_type *));
    section->line_numbers_array        = array;
    section->line_numbers_array_length = i;

    qsort(array, i, sizeof(gp_linenum_type *), _linenum_cmp);
    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Use gplink/lst.c */

gp_linenum_type *
gp_coffgen_find_linenum(const gp_section_type *Section, const gp_symbol_type *Symbol,
                        unsigned int Line_number)
{
  gp_linenum_type   linenum;
  gp_linenum_type  *ptr;
  gp_linenum_type **ret;

  if ((Section == NULL) || (Section->line_numbers_array == NULL)) {
    return NULL;
  }

  linenum.symbol      = Symbol;
  linenum.line_number = Line_number;
  ptr = &linenum;
  ret = (gp_linenum_type **)bsearch(&ptr, Section->line_numbers_array, Section->line_numbers_array_length,
                                    sizeof(gp_linenum_type *), _linenum_cmp);
  return ((ret != NULL) ? *ret : NULL);
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

void
gp_coffgen_free_section(gp_section_type *Section)
{
  gp_reloc_type   *relocation;
  gp_linenum_type *line_number;
  gp_reloc_type   *old_relocation;
  gp_linenum_type *old_line_number;

  if (Section->data != NULL) {
    i_memory_free(Section->data);
  }

  relocation = Section->relocation_list;
  while (relocation != NULL) {
    old_relocation = relocation;
    relocation     = relocation->next;
    free(old_relocation);
  }

  line_number = Section->line_number_list;
  while (line_number != NULL) {
    old_line_number = line_number;
    line_number     = line_number->next;
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
  num_auxsym = Symbol->num_auxsym;

  aux = Symbol->aux_list;
  while (aux != NULL) {
    old_aux = aux;
    aux     = aux->next;
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

  symbol = Object->symbol_list;
  while (symbol != NULL) {
    old_symbol = symbol;
    symbol     = symbol->next;
    gp_coffgen_free_symbol(old_symbol);
  }

  symbol = Object->reserved_symbol_list;
  while (symbol != NULL) {
    old_symbol = symbol;
    symbol     = symbol->next;
    gp_coffgen_free_symbol(old_symbol);
  }

  if (Object->symbol_ptr_array != NULL) {
    free(Object->symbol_ptr_array);
  }

  if (Object->symbol_hashtable != NULL) {
    free(Object->symbol_hashtable);
  }

  section = Object->section_list;
  while (section != NULL) {
    old_section = section;
    section     = section->next;
    gp_coffgen_free_section(old_section);
  }

  section = Object->reserved_section_list;
  while (section != NULL) {
    old_section = section;
    section     = section->next;
    gp_coffgen_free_section(old_section);
  }

  if (Object->section_ptr_array != NULL) {
    free(Object->section_ptr_array);
  }

  free(Object->filename);
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

/* GNU PIC general coff functions
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2016 Molnar Karoly

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

#define CHECK_RELOCATIONS_PASS_MAX      20
#define CHECK_RELOCATIONS_DEPTH_MAX     100

static unsigned int object_serial_id  = 0;
static unsigned int section_serial_id = 0;

/*------------------------------------------------------------------------------------------------*/

gp_object_t *
gp_coffgen_new_object(const char *File_name)
{
  gp_object_t *object;

  /* allocate memory for the object file */
  object = (gp_object_t *)GP_Calloc(1, sizeof(gp_object_t));

  /* initialize the object */
  object->filename  = GP_Strdup(File_name);
  object->processor = gp_find_processor("generic");
  object->class     = PROC_CLASS_GENERIC;
  object->time      = (uint32_t)time(NULL);
  object->serial_id = object_serial_id++;

  gp_list_set_delete_node_func(&object->section_list, (gp_node_del_t)gp_coffgen_free_section);
  gp_list_set_delete_node_func(&object->dead_section_list, (gp_node_del_t)gp_coffgen_free_section);

  gp_list_set_delete_node_func(&object->symbol_list, (gp_node_del_t)gp_coffgen_free_symbol);
  gp_list_set_delete_node_func(&object->dead_symbol_list, (gp_node_del_t)gp_coffgen_free_symbol);

  return object;
}

/*------------------------------------------------------------------------------------------------*/

/* Move linked lists between two object. */

void
gp_coffgen_transfer_object_data(gp_object_t *Receiver, gp_object_t *Sender)
{
  if ((Receiver == NULL) || (Sender == NULL)) {
    return;
  }

  gp_list_move(&Receiver->section_list, &Sender->section_list);
  gp_list_move(&Receiver->dead_section_list, &Sender->dead_section_list);
  gp_list_move(&Receiver->symbol_list, &Sender->symbol_list);
  gp_list_move(&Receiver->dead_symbol_list, &Sender->dead_symbol_list);

  Receiver->num_symbols += Sender->num_symbols;
  Sender->num_symbols    = 0;
}

/*------------------------------------------------------------------------------------------------*/

/* Update the object identifier in the all internal linked lists. */

gp_boolean
gp_coffgen_update_all_object_id(gp_object_t *Object)
{
  gp_section_t *section;
  gp_symbol_t  *symbol;
  unsigned int  id;

  if (Object == NULL) {
    return false;
  }

  id      = Object->serial_id;
  section = Object->section_list.first;
  while (section != NULL) {
    section->object_id = id;
    section            = section->next;
  }

  section = Object->dead_section_list.first;
  while (section != NULL) {
    section->object_id = id;
    section            = section->next;
  }

  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    symbol->object_id = id;
    symbol            = symbol->next;
  }

  symbol = Object->dead_symbol_list.first;
  while (symbol != NULL) {
    symbol->object_id = id;
    symbol            = symbol->next;
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* Find a "Name" section from the given starting section. */

gp_section_t *
gp_coffgen_find_section(gp_object_t *Object, gp_section_t *Start, const char *Name)
{
  gp_section_t *section;

  if ((Object == NULL) || (Start == NULL)) {
    return NULL;
  }

  section = Start;
  while (section != NULL) {
    if ((section->name != NULL) && (strcmp(section->name, Name) == 0)) {
      return section;
    }
    section = section->next;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Create a new section. */

gp_section_t *
gp_coffgen_new_section(const char *Name, MemBlock_t *Data)
{
  gp_section_t *new;

  /* allocate memory for the section */
  new = (gp_section_t *)gp_list_node_new(sizeof(gp_section_t));

  /* initialize section */
  new->name      = GP_Strdup(Name);
  new->data      = (Data != NULL) ? Data : gp_mem_i_create();
  new->serial_id = section_serial_id++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* Allocate a block of section. -- gpreadobj.c */

gp_section_t *
gp_coffgen_make_block_section(gp_object_t *Object, unsigned int Num_sections)
{
  gp_section_t **ptr_array;
  unsigned int   id;
  unsigned int   i;

  if (Object->section_list.first != NULL) {
    gp_error("'%s': The list of sections already exists, can not be created again.", Object->filename);
    assert(0);
  }

  if (Num_sections == 0) {
    return NULL;
  }

  ptr_array = (gp_section_t **)gp_list_make_block(&Object->section_list, Num_sections, sizeof(gp_section_t));

  id = Object->serial_id;
  for (i = 0; i < Num_sections; i++) {
    ptr_array[i]->object_id = id;
    ptr_array[i]->serial_id = section_serial_id++;
  }

  /* The first->prev and last->next values is already NULL. */

  Object->section_ptr_array = ptr_array;

  return Object->section_list.first;
}

/*------------------------------------------------------------------------------------------------*/

/* Add to object an exists section. */

gp_section_t *
gp_coffgen_add_exists_section(gp_object_t *Object, gp_section_t *Section)
{
  if (Object == NULL) {
    return NULL;
  }

  gp_list_node_append(&Object->section_list, Section);
  Section->object_id = Object->serial_id;

  return Section;
}

/*------------------------------------------------------------------------------------------------*/

/* Behind a section inserts another section. */

gp_section_t *
gp_coffgen_insert_after_section(gp_object_t *Object, gp_section_t *Ancestor, gp_section_t *Following)
{
  if (Object == NULL) {
    return NULL;
  }

  if (Ancestor->object_id != Object->serial_id) {
    gp_error("The '%s'{%u} section does not belong to this object: '%s'{%u}",
             Ancestor->name, Ancestor->object_id, Object->filename, Object->serial_id);
    assert(0);
  }

  gp_list_node_insert_after(&Object->section_list, Ancestor, Following);
  Following->object_id = Object->serial_id;

  return Following;
}

/*------------------------------------------------------------------------------------------------*/

/* Add a new section to the object. */

gp_section_t *
gp_coffgen_add_section(gp_object_t *Object, const char *Name, MemBlock_t *Data)
{
  if (Object == NULL) {
    return NULL;
  }

  return gp_coffgen_add_exists_section(Object, gp_coffgen_new_section(Name, Data));
}

/*------------------------------------------------------------------------------------------------*/

/* Move linked lists between two section. */

void
gp_coffgen_transfer_section_data(gp_section_t *Receiver, gp_section_t *Sender)
{
  if ((Receiver == NULL) || (Sender == NULL)) {
    return;
  }

  gp_list_move(&Receiver->relocation_list, &Sender->relocation_list);
  gp_list_move(&Receiver->line_number_list, &Sender->line_number_list);
}

/*------------------------------------------------------------------------------------------------*/

/* Update the section identifier in the all internal linked lists. */

gp_boolean
gp_coffgen_update_all_section_id(gp_section_t *Section)
{
  gp_reloc_t   *relocation;
  gp_linenum_t *line_number;
  unsigned int  id;

  if (Section == NULL) {
    return false;
  }

  id = Section->serial_id;

  relocation = Section->relocation_list.first;
  while (relocation != NULL) {
    relocation->section_id = id;
    relocation             = relocation->next;
  }

  line_number = Section->line_number_list.first;
  while (line_number != NULL) {
    line_number->section_id = id;
    line_number             = line_number->next;
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* Move all symbols of section to dead list. */

void
gp_coffgen_move_reserve_section_symbols(gp_object_t *Object, gp_section_t *Section)
{
  gp_symbol_t *list;
  gp_symbol_t *symbol;

  if (Object == NULL) {
    return NULL;
  }

  /* Move all symbols for the section into dead list. */
  list = Object->symbol_list.first;
  while (list != NULL) {
    symbol = list;
    list   = list->next;

    if (symbol->section == Section) {
      gp_coffgen_move_reserve_symbol(Object, symbol);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Delete all symbols of section. */

void
gp_coffgen_del_section_symbols(gp_object_t *Object, gp_section_t *Section)
{
  gp_symbol_t *list;
  gp_symbol_t *symbol;

  if (Object == NULL) {
    return NULL;
  }

  /* Remove all symbols for the section. */
  list = Object->symbol_list.first;
  while (list != NULL) {
    symbol = list;
    list   = list->next;

    if (symbol->section == Section) {
      gp_coffgen_del_symbol(Object, symbol);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_decrease_relocation_counts(gp_section_t *Section)
{
  gp_reloc_t   *relocation;
  gp_symbol_t  *symbol;
  gp_section_t *sym_sect;

  relocation = Section->relocation_list.first;
  while (relocation != NULL) {
    symbol   = relocation->symbol;
    sym_sect = symbol->section;

    if (symbol->reloc_count_all_section > 0) {
      (symbol->reloc_count_all_section)--;

      if (sym_sect == Section) {
        /*  Relocation reference from own section. */
        if (symbol->reloc_count_own_section > 0) {
          (symbol->reloc_count_own_section)--;
        }
        else {
          gp_warning("Number of relocation references of symbol from own section is zero: '%s'", symbol->name);
        }
      }
      else {
        /*  Relocation reference from another section. */
        if (symbol->reloc_count_other_section > 0) {
          (symbol->reloc_count_other_section)--;

          if (sym_sect->reloc_count > 0) {
            (sym_sect->reloc_count)--;
          }
          else {
            gp_warning("Number of relocation references of section from another section is zero: '%s'", sym_sect->name);
          }
        }
        else {
          gp_warning("Number of relocation references of symbol from another section is zero: '%s'", symbol->name);
        }
      }
    }

    relocation = relocation->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Move a section to dead list. */

gp_section_t *
gp_coffgen_move_reserve_section(gp_object_t *Object, gp_section_t *Section)
{
  if (Object->section_list.first == NULL) {
    return NULL;
  }

  if (Section->object_id != Object->serial_id) {
    gp_error("The '%s'{%u} section does not belong to this object: '%s'{%u}",
             Section->name, Section->object_id, Object->filename, Object->serial_id);
    assert(0);
  }

  gp_list_node_move(&Object->dead_section_list, &Object->section_list, Section);
  _decrease_relocation_counts(Section);
  return Section;
}

/*------------------------------------------------------------------------------------------------*/

/* Delete a section from object. */

gp_boolean
gp_coffgen_del_section(gp_object_t *Object, gp_section_t *Section)
{
  if (Object->section_list.first == NULL) {
    return false;
  }

  if (Section->object_id != Object->serial_id) {
    gp_error("The '%s'{%u} section does not belong to this object: '%s'{%u}",
             Section->name, Section->object_id, Object->filename, Object->serial_id);
    assert(0);
  }

  gp_list_node_remove(&Object->section_list, Section);
  gp_coffgen_free_section(Section);
  return true;
}

/*------------------------------------------------------------------------------------------------*/

static int
_section_cmp(const void *P0, const void *P1)
{
  const gp_section_t *s0 = *(const gp_section_t **)P0;
  const gp_section_t *s1 = *(const gp_section_t **)P1;
  uint32_t            a0 = s0->address;
  uint32_t            a1 = s1->address;

  if (a0 < a1) {
    return -1;
  }

  if (a0 > a1) {
    return 1;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

/* Put in an array that sections which are located the same page. */

gp_section_t **
gp_coffgen_make_section_array(gp_object_t *Object, unsigned int *Num_sections,
                              unsigned int Page_addr, uint32_t Flags)
{
  proc_class_t   class;
  gp_section_t  *section;
  gp_section_t **array;
  unsigned int   i;
  unsigned int   page;
  unsigned int   n_sections;

  if ((Object == NULL) || (Num_sections == NULL)) {
    return NULL;
  }

  class = Object->class;

  /* Examines the sections. */
  n_sections = 0;
  section    = Object->section_list.first;
  while (section != NULL) {
    FlagClr(section->opt_flags, OPT_FLAGS_GPCOFFGEN_MODULE);
    page = gp_processor_page_addr(class, gp_processor_insn_from_byte_c(class, section->address));

    if ((page == Page_addr) && (FlagsIsNotAllClr(section->flags, Flags))) {
      /* Located on same page and there is at least one valid flag. */
      ++n_sections;
      FlagSet(section->opt_flags, OPT_FLAGS_GPCOFFGEN_MODULE);
    }
    section = section->next;
  }

  if (n_sections == 0) {
    return NULL;
  }

  array = (gp_section_t **)GP_Malloc(n_sections * sizeof(gp_section_t *));

  /* Populate the array. */
  i       = 0;
  section = Object->section_list.first;
  while (section != NULL) {
    if (FlagIsSet(section->opt_flags, OPT_FLAGS_GPCOFFGEN_MODULE)) {
      array[i] = section;
      ++i;
    }
    section = section->next;
  }

  if (n_sections > 1) {
    qsort(array, n_sections, sizeof(gp_section_t *), _section_cmp);
  }

  *Num_sections = n_sections;
  return array;
}

/*------------------------------------------------------------------------------------------------*/

/* Find by name a symbol that not section symbol. */

gp_symbol_t *
gp_coffgen_find_symbol(gp_object_t *Object, const char *Name)
{
  gp_symbol_t *symbol;

  if (Object == NULL) {
    return NULL;
  }

  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    if ((symbol->class != C_SECTION) && (symbol->name != NULL) && (strcmp(symbol->name, Name) == 0)) {
      return symbol;
    }
    symbol = symbol->next;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Find by name a symbol that a section symbol. */

gp_symbol_t *
gp_coffgen_find_section_symbol(gp_object_t *Object, const char *Name)
{
  gp_symbol_t *symbol;

  if (Object == NULL) {
    return NULL;
  }

  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    if ((symbol->class == C_SECTION) && (symbol->name != NULL) && (strcmp(symbol->name, Name) == 0)) {
      return symbol;
    }
    symbol = symbol->next;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Search for a symbol, based on the value and name of host section.
   The function is slow, but only need the error messages. */

gp_symbol_t *
gp_coffgen_find_symbol_section_value(gp_object_t *Object, const char *Section_name, long Value)
{
  gp_symbol_t *symbol;

  if ((Object == NULL) || (Section_name == NULL)) {
    return NULL;
  }

  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    if ((symbol->class != C_SECTION) && (symbol->class != C_FILE) &&
        (symbol->section_name != NULL) &&
        (strcmp(symbol->section_name, Section_name) == 0) &&
        (symbol->value == Value)) {
      return symbol;
    }
    symbol = symbol->next;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Allocate a block of symbols. -- gpreadobj.c */

gp_symbol_t *
gp_coffgen_make_block_symbol(gp_object_t *Object)
{
  gp_symbol_t  **ptr_array;
  unsigned int   n_symbols;
  unsigned int   id;
  unsigned int   i;

  if ((n_symbols = Object->num_symbols) == 0) {
    return NULL;
  }

  if (Object->symbol_list.first != NULL) {
    gp_error("'%s': The list of symbols already exists, can not be created again.", Object->filename);
    assert(0);
  }

  ptr_array = (gp_symbol_t **)gp_list_make_block(&Object->symbol_list, n_symbols, sizeof(gp_symbol_t));

  id = Object->serial_id;
  for (i = 0; i < n_symbols; i++) {
    ptr_array[i]->object_id = id;
  }

  /* The first->prev and last->next values is already NULL. */

  Object->symbol_ptr_array = ptr_array;

  return Object->symbol_list.first;
}

/*------------------------------------------------------------------------------------------------*/

/* Add a new symbol to the object. */

gp_symbol_t *
gp_coffgen_add_symbol(gp_object_t *Object, const char *Name, int16_t Section_number)
{
  gp_symbol_t *new;

  /* allocate memory for the symbol */
  new = (gp_symbol_t *)gp_list_node_append(&Object->symbol_list, gp_list_node_new(sizeof(gp_symbol_t)));
  new->name           = GP_Strdup(Name);
  new->number         = Object->num_symbols;
  new->section_number = Section_number;
  new->object_id      = Object->serial_id;
  (Object->num_symbols)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* Add a new auxiliary symbol to the symbol from the object. */

gp_aux_t *
gp_coffgen_add_aux(gp_object_t *Object, gp_symbol_t *Symbol)
{
  gp_aux_t *new;

  new = (gp_aux_t *)gp_list_node_append(&Symbol->aux_list, gp_list_node_new(sizeof(gp_aux_t)));

  (Object->num_symbols)++;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* Allocate a block of auxiliary symbols. -- gpreadobj.c */

gp_aux_t *
gp_coffgen_make_block_aux(gp_symbol_t *Symbol, unsigned int Num_auxsyms)
{
  gp_aux_t     **ptr_array;
#if (AUX_NONE != 0)
  unsigned int   i;
#endif

  if (Symbol->aux_list.first != NULL) {
    gp_error("'%s': The list of aux symbols already exists, can not be created again.", Symbol->name);
    assert(0);
  }

  if (Num_auxsyms == 0) {
    return NULL;
  }

  ptr_array = (gp_aux_t **)gp_list_make_block(&Symbol->aux_list, Num_auxsyms, sizeof(gp_aux_t));
#if (AUX_NONE != 0)
  for (i = 0; i < Num_auxsyms; i++) {
    ptr_array[i]->type = AUX_NONE;
  }
#endif
  free(ptr_array);

  return Symbol->aux_list.first;
}

/*------------------------------------------------------------------------------------------------*/

/* Move a symbol to dead list. */

gp_symbol_t *
gp_coffgen_move_reserve_symbol(gp_object_t *Object, gp_symbol_t *Symbol)
{
  if (Object->symbol_list.first == NULL) {
    return NULL;
  }

  if (Symbol->object_id != Object->serial_id) {
    gp_error("The '%s'{%u} symbol does not belong to this object: '%s'{%u}",
             Symbol->name, Symbol->object_id, Object->filename, Object->serial_id);
    assert(0);
  }

  gp_list_node_move(&Object->dead_symbol_list, &Object->symbol_list, Symbol);
  Object->num_symbols -= 1 + Symbol->aux_list.num_nodes;

  return Symbol;
}

/*------------------------------------------------------------------------------------------------*/

/* Delete the symbol from the object. */

gp_boolean
gp_coffgen_del_symbol(gp_object_t *Object, gp_symbol_t *Symbol)
{
  if (Object->symbol_list.first == NULL) {
    return false;
  }

  if (Symbol->object_id != Object->serial_id) {
    gp_error("The '%s'{%u} symbol does not belong to this object: '%s'{%u}",
             Symbol->name, Symbol->object_id, Object->filename, Object->serial_id);
    assert(0);
  }

  gp_list_node_remove(&Object->symbol_list, Symbol);
  Object->num_symbols -= 1 + gp_coffgen_free_symbol(Symbol);
  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* Make an array from symbol list of object. */

gp_symbol_t **
gp_coffgen_make_symbol_array(const gp_object_t *Object, int (*Cmp)(const void *, const void *))
{
  gp_symbol_t  **array;
  gp_symbol_t   *symbol;
  unsigned int   i;

  if ((Object == NULL) || (Object->symbol_list.first == NULL)) {
    return NULL;
  }

  array = (gp_symbol_t **)GP_Malloc(Object->symbol_list.num_nodes * sizeof(gp_symbol_t *));

  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    array[i] = symbol;
    ++i;
    symbol = symbol->next;
  }

  assert(Object->symbol_list.num_nodes == i);

  if ((Cmp != NULL) && (i > 1)) {
    qsort(array, i, sizeof(gp_symbol_t *), Cmp);
  }

  return array;
}

/*------------------------------------------------------------------------------------------------*/

/* Convert to string the symbol type. */

const char *
gp_coffgen_symbol_type_to_str(uint8_t Type)
{
  static const char * const type_str[] = {
    "T_NULL",
    "T_VOID",
    "T_CHAR",
    "T_SHORT",
    "T_INT",
    "T_LONG",
    "T_FLOAT",
    "T_DOUBLE",
    "T_STRUCT",
    "T_UNION",
    "T_ENUM",
    "T_MOE",
    "T_UCHAR",
    "T_USHORT",
    "T_UINT",
    "T_ULONG",
    "T_LNGDBL",
    "T_SLONG",
    "T_USLONG"
  };

  if (Type >= ARRAY_SIZE(type_str)) {
    return NULL;
  }

  return type_str[Type];
}

/*------------------------------------------------------------------------------------------------*/

/* Convert to string the symbol derived type. */

const char *
gp_coffgen_symbol_derived_type_to_str(uint32_t Type)
{
  static const char * const type_str[] = {
    "DT_NON",
    "DT_PTR",
    "DT_FCN",
    "DT_ARY",
    "DT_ROMPTR",
    "DT_FARROMPTR"
  };

  if (Type >= ARRAY_SIZE(type_str)) {
    return NULL;
  }

  return type_str[Type];
}

/*------------------------------------------------------------------------------------------------*/

/* Convert to string the symbol class. */

const char *
gp_coffgen_symbol_class_to_str(uint8_t Class)
{
  switch (Class) {
    case C_NULL:    return "C_NULL";
    case C_AUTO:    return "C_AUTO";      /* automatic variable */
    case C_EXT:     return "C_EXT";       /* external symbol */
    case C_STAT:    return "C_STAT";      /* static */
    case C_REG:     return "C_REG";       /* register variable */
    case C_EXTDEF:  return "C_EXTDEF";    /* external definition */
    case C_LABEL:   return "C_LABEL";     /* label */
    case C_ULABEL:  return "C_ULABEL";    /* undefined label */
    case C_MOS:     return "C_MOS";       /* member of structure */
    case C_ARG:     return "C_ARG";       /* function argument */
    case C_STRTAG:  return "C_STRTAG";    /* structure tag */
    case C_MOU:     return "C_MOU";       /* member of union */
    case C_UNTAG:   return "C_UNTAG";     /* union tag */
    case C_TPDEF:   return "C_TPDEF";     /* type definition */
    case C_USTATIC: return "C_USTATIC";   /* undefined static */
    case C_ENTAG:   return "C_ENTAG";     /* enumeration tag */
    case C_MOE:     return "C_MOE";       /* member of enumeration */
    case C_REGPARM: return "C_REGPARM";   /* register parameter */
    case C_FIELD:   return "C_FIELD";     /* bit field */
    case C_AUTOARG: return "C_AUTOARG";   /* auto argument */
    case C_LASTENT: return "C_LASTENT";   /* dummy entry (end of block) */
    case C_BLOCK:   return "C_BLOCK";     /* ".bb" or ".eb" */
    case C_FCN:     return "C_FCN";       /* ".bf" or ".ef" */
    case C_EOS:     return "C_EOS";       /* end of structure */
    case C_FILE:    return "C_FILE";      /* file name */
    case C_LINE:    return "C_LINE";      /* line number reformatted as symbol table entry */
    case C_ALIAS:   return "C_ALIAS";     /* duplicate tag */
    case C_HIDDEN:  return "C_HIDDEN";    /* ext symbol in dmert public lib */
    case C_EOF:     return "C_EOF";       /* end of file */
    case C_LIST:    return "C_LIST";      /* absoulte listing on or off */
    case C_SECTION: return "C_SECTION";   /* section */
    case C_EFCN:    return "C_EFCN";      /* physical end of function */
    default:        return NULL;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Allocate a block of relocations. -- gpreadobj.c */

gp_reloc_t *
gp_coffgen_make_block_reloc(gp_section_t *Section, unsigned int Num_relocations)
{
  gp_reloc_t   **ptr_array;
  unsigned int   id;
  unsigned int   i;

  if (Section->relocation_list.first != NULL) {
    gp_error("'%s': The list of relocations already exists, can not be created again.", Section->name);
    assert(0);
  }

  if (Num_relocations == 0) {
    return NULL;
  }

  ptr_array = (gp_reloc_t **)gp_list_make_block(&Section->relocation_list, Num_relocations, sizeof(gp_reloc_t));

  id = Section->serial_id;
  for (i = 0; i < Num_relocations; i++) {
    ptr_array[i]->section_id = id;
  }

  free(ptr_array);

  return Section->relocation_list.first;
}

/*------------------------------------------------------------------------------------------------*/

/* Add a new relocation to the section. */

gp_reloc_t *
gp_coffgen_add_reloc(gp_section_t *Section)
{
  gp_reloc_t *new;

  /* allocate memory for the relocation */
  new = (gp_reloc_t *)gp_list_node_append(&Section->relocation_list, gp_list_node_new(sizeof(gp_reloc_t)));
  new->section_id = Section->serial_id;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* Clears all relocation counters. */

static void
_clear_relocation_counts(const gp_object_t *Object)
{
  gp_section_t *section;
  gp_symbol_t  *symbol;

  section = Object->section_list.first;
  while (section != NULL) {
    section->reloc_count = 0;

    if (FlagsIsNotAllClr(section->flags, STYP_ABS | STYP_DATA)) {
      /* This section get protection immediately. The additional sections' protection
         is based on this. */
      FlagSet(section->opt_flags, OPT_FLAGS_PROTECTED_SECTION);
    }
    else {
      FlagClr(section->opt_flags, OPT_FLAGS_PROTECTED_SECTION);
    }

    section = section->next;
  }

  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    symbol->reloc_count_all_section   = 0;
    symbol->reloc_count_own_section   = 0;
    symbol->reloc_count_other_section = 0;
    symbol = symbol->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_check_section_relocations(proc_class_t Class, gp_section_t *Section, unsigned int Pass,
                           unsigned int Behavior, unsigned int Level)
{
  gp_reloc_t   *relocation;
  gp_symbol_t  *symbol;
  gp_section_t *sym_sect;
  gp_boolean    use_counters;
  gp_boolean    change;

  if (Level >= CHECK_RELOCATIONS_DEPTH_MAX) {
    gp_warning("Depth of relocations check reached the limit: %u", CHECK_RELOCATIONS_DEPTH_MAX);
    return false;
  }

  use_counters = (Pass == 0) ? true : false;
  change       = false;
  relocation   = Section->relocation_list.first;
  while (relocation != NULL) {
    symbol = relocation->symbol;
    if (use_counters) {
      (symbol->reloc_count_all_section)++;
    }

    sym_sect = symbol->section;
    if (sym_sect == NULL) {
      /* This is an orphan symbol. */
      if (FlagIsClr(Behavior, RELOC_DISABLE_WARN)) {
        if (FlagIsSet(Behavior, RELOC_ENABLE_CINIT_WARN) || (strcmp(symbol->name, "_cinit") != 0)) {
          gp_warning("Relocation symbol \"%s\" [0x%0*X] has no section. (pass %u)",
                     symbol->name, Class->addr_digits, relocation->address, Pass);
        }
      }
    }
    else {
      if (sym_sect == Section) {
        /* In this case not change the level of protection. */
        if (use_counters) {
          (symbol->reloc_count_own_section)++;
        }
      }
      else {
        /* The symbol is located another section. */
        if (use_counters) {
          (symbol->reloc_count_other_section)++;
          (sym_sect->reloc_count)++;
        }

        if (FlagIsSet(Section->opt_flags, OPT_FLAGS_PROTECTED_SECTION)) {
          /* If the section of reference is protected, then the section of symbol also will
             be protected. */
          if (FlagIsClr(sym_sect->opt_flags, OPT_FLAGS_PROTECTED_SECTION)) {
            /* Only then modify and check if have not been under protection. */
            FlagSet(sym_sect->opt_flags, OPT_FLAGS_PROTECTED_SECTION);
            change = _check_section_relocations(Class, sym_sect, Pass, Behavior, Level + 1);
          }
        }
      }
    }
    relocation = relocation->next;
  }

  return change;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_check_object_relocations(const gp_object_t *Object, unsigned int Pass, unsigned int Behavior)
{
  proc_class_t  class;
  gp_section_t *section;
  gp_boolean    change;
  unsigned int  level;

  level   = 0;
  change  = false;
  class   = Object->class;
  section = Object->section_list.first;
  while (section != NULL) {
    change |= _check_section_relocations(class, section, Pass, Behavior, level);
    section = section->next;
  }

  return change;
}

/*------------------------------------------------------------------------------------------------*/

/* Handle the relocation counters of sections and symbols. */

void
gp_coffgen_check_relocations(const gp_object_t *Object, gp_boolean Enable_cinit_warns)
{
  unsigned int pass;

  _clear_relocation_counts(Object);
  pass = 0;
  while (true) {
    /* Need for further rounds because may occur back and forth references. */
    if (!_check_object_relocations(Object, pass, Enable_cinit_warns)) {
      break;
    }

    ++pass;

    if (pass > CHECK_RELOCATIONS_PASS_MAX) {
      gp_warning("Number of relocations check reached the limit: %u", CHECK_RELOCATIONS_PASS_MAX);
      break;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Delete the relocation from the section. */

gp_boolean
gp_coffgen_del_reloc(gp_section_t *Section, gp_reloc_t *Relocation)
{
  gp_section_t *section;
  gp_symbol_t  *symbol;

  if (Section->relocation_list.first == NULL) {
    return false;
  }

  if (Relocation->section_id != Section->serial_id) {
    gp_error("The relocation{%u} does not belong to this section: '%s'{%u}",
             Relocation->section_id, Section->name, Section->serial_id);
    assert(0);
  }

  gp_list_node_remove(&Section->relocation_list, Relocation);

  symbol  = Relocation->symbol;
  assert(symbol != NULL);
  section = symbol->section;
  assert(section != NULL);

  if (symbol->reloc_count_all_section > 0) {
    (symbol->reloc_count_all_section)--;

    if (symbol->section == Section) {
      /*  Relocation reference from own section. */
      if (symbol->reloc_count_own_section > 0) {
        (symbol->reloc_count_own_section)--;
      }
      else {
        gp_warning("Number of relocation references of symbol from own section is zero: '%s'", symbol->name);
      }
    }
    else {
      /*  Relocation reference from another section. */
      if (symbol->reloc_count_other_section > 0) {
        (symbol->reloc_count_other_section)--;

        if (section->reloc_count > 0) {
          (section->reloc_count)--;
        }
        else {
          gp_warning("Number of relocation references of section from another section is zero: '%s'", section->name);
        }
      }
      else {
        gp_warning("Number of relocation references of symbol from another section is zero: '%s'", symbol->name);
      }
    }
  }
  else {
    gp_warning("Number of relocation references of symbol from all section is zero: '%s'", symbol->name);
  }

  gp_list_node_free(&Section->relocation_list, Relocation);
  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* Convert to string the relocation type. */

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

/* Allocate a block of line numbers. -- gpreadobj.c */

gp_linenum_t *
gp_coffgen_make_block_linenum(gp_section_t *Section, unsigned int Num_linenums)
{
  gp_linenum_t **ptr_array;
  unsigned int   id;
  unsigned int   i;

  if (Section->line_number_list.first != NULL) {
    gp_error("'%s': The list of line numbers already exists, can not be created again.", Section->name);
    assert(0);
  }

  if (Num_linenums == 0) {
    return NULL;
  }

  ptr_array = (gp_linenum_t **)gp_list_make_block(&Section->line_number_list, Num_linenums, sizeof(gp_linenum_t));

  id = Section->serial_id;
  for (i = 0; i < Num_linenums; i++) {
    ptr_array[i]->section_id = id;
  }

  free(ptr_array);

  return Section->line_number_list.first;
}

/*------------------------------------------------------------------------------------------------*/

/* Add new line number to the section. */

gp_linenum_t *
gp_coffgen_add_linenum(gp_section_t *Section)
{
  gp_linenum_t *new;

  /* allocate memory for the line number */
  new = (gp_linenum_t *)gp_list_node_append(&Section->line_number_list, gp_list_node_new(sizeof(gp_linenum_t)));
  new->section_id = Section->serial_id;

  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* Delete the line number from the section. */

gp_boolean
gp_coffgen_del_linenum(gp_section_t *Section, gp_linenum_t *Linenum)
{
  if (Section->line_number_list.first == NULL) {
    return false;
  }

  if (Linenum->section_id != Section->serial_id) {
    gp_error("The line number{%u} does not belong to this section: '%s'{%u}",
             Linenum->section_id, Section->name, Section->serial_id);
    assert(0);
  }

  gp_list_node_delete(&Section->line_number_list, Linenum);
  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* Find line number by address in the section. */

gp_linenum_t *
gp_coffgen_find_linenum_by_address(gp_section_t *Section, unsigned int Address)
{
  gp_linenum_t *linenum;

  if (Section->line_number_list.first == NULL) {
    return NULL;
  }

  linenum = Section->line_number_list.first;
  while (linenum != NULL) {
    if (linenum->address == Address) {
      return linenum;
    }
    linenum = linenum->next;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Delete a line number by address from the section. */

gp_boolean
gp_coffgen_del_linenum_by_address(gp_section_t *Section, unsigned int Address)
{
  gp_linenum_t *linenum;

  linenum = gp_coffgen_find_linenum_by_address(Section, Address);

  if (linenum == NULL) {
    return false;
  }

  return gp_coffgen_del_linenum(Section, linenum);
}

/*------------------------------------------------------------------------------------------------*/

/* Delete line numbers by address range from the section. */

unsigned int
gp_coffgen_del_linenum_by_address_area(gp_section_t *Section, unsigned int Address_start,
                                       unsigned int Address_end)
{
  gp_linenum_t *linenum;
  gp_linenum_t *next;
  unsigned int  num;

  if (Section->line_number_list.first == NULL) {
    return false;
  }

  num     = 0;
  linenum = Section->line_number_list.first;
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
  const gp_linenum_t *l0   = *(const gp_linenum_t **)P0;
  const gp_linenum_t *l1   = *(const gp_linenum_t **)P1;
  const gp_symbol_t  *sym0 = l0->symbol;
  const gp_symbol_t  *sym1 = l1->symbol;
  unsigned int        num0 = l0->line_number;
  unsigned int        num1 = l1->line_number;

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

/* Create line number array. Use gplink/gplink.c */

void
gp_coffgen_make_linenum_array(gp_object_t *Object)
{
  gp_section_t  *section;
  gp_linenum_t  *linenum;
  gp_linenum_t **array;
  unsigned int   n_linenums;
  unsigned int   prev_num;
  unsigned int   i;

  section = Object->section_list.first;
  while (section != NULL) {
    n_linenums = section->line_number_list.num_nodes;
    array      = (gp_linenum_t **)GP_Malloc(n_linenums * sizeof(gp_linenum_t *));
    i          = 0;
    prev_num   = (unsigned int)(-1);
    linenum    = section->line_number_list.first;
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
    array = (gp_linenum_t **)GP_Realloc(array, i * sizeof(gp_linenum_t *));
    section->line_numbers_array        = array;
    section->line_numbers_array_length = i;

    qsort(array, i, sizeof(gp_linenum_t *), _linenum_cmp);
    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Find a line number in array by symbol and number. Use gplink/lst.c */

gp_linenum_t *
gp_coffgen_find_linenum(const gp_section_t *Section, const gp_symbol_t *Symbol,
                        unsigned int Line_number)
{
  gp_linenum_t   linenum;
  gp_linenum_t  *ptr;
  gp_linenum_t **ret;

  if ((Section == NULL) || (Section->line_numbers_array == NULL)) {
    return NULL;
  }

  linenum.symbol      = Symbol;
  linenum.line_number = Line_number;
  ptr = &linenum;
  ret = (gp_linenum_t **)bsearch(&ptr, Section->line_numbers_array, Section->line_numbers_array_length,
                                 sizeof(gp_linenum_t *), _linenum_cmp);
  return ((ret != NULL) ? *ret : NULL);
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if any relocation uses the symbol. */

gp_boolean
gp_coffgen_symbol_has_reloc(const gp_symbol_t *Symbol, int Type)
{
  switch (Type) {
    case COFF_SYM_RELOC_ALL:
    default:
      return ((Symbol->reloc_count_all_section > 0) ? true : false);

    case COFF_SYM_RELOC_OWN:
      return ((Symbol->reloc_count_own_section > 0) ? true : false);

    case COFF_SYM_RELOC_OTHER:
      return ((Symbol->reloc_count_other_section > 0) ? true : false);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if the symbol is global. */

gp_boolean
gp_coffgen_is_global_symbol(const gp_symbol_t *Symbol)
{
  return (((Symbol->class == C_EXT) && (Symbol->section_number == N_SCNUM)) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if the symbol is external. */

gp_boolean
gp_coffgen_is_external_symbol(const gp_symbol_t *Symbol)
{
  return (((Symbol->class == C_EXT) && (Symbol->section_number == N_UNDEF)) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if the symbol is debug. */

gp_boolean
gp_coffgen_is_debug_symbol(const gp_symbol_t *Symbol)
{
  return (((Symbol->class == C_NULL) && (Symbol->section_number == N_DEBUG)) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if the symbol is absolute. */

gp_boolean
gp_coffgen_is_absolute_symbol(const gp_symbol_t *Symbol)
{
  return (((Symbol->class == C_NULL) && (Symbol->section_number == N_ABS)) ? true : false);
}

/*------------------------------------------------------------------------------------------------*/

/* Free the section. */

void
gp_coffgen_free_section(gp_section_t *Section)
{
  if (Section->data != NULL) {
    gp_mem_i_free(Section->data);
  }

  gp_list_delete(&Section->relocation_list);
  gp_list_delete(&Section->line_number_list);

  if (Section->line_numbers_array != NULL) {
    free(Section->line_numbers_array);
  }

  free(Section->name);
  free(Section);
}

/*------------------------------------------------------------------------------------------------*/

/* Free the symbol. */

unsigned int
gp_coffgen_free_symbol(gp_symbol_t *Symbol)
{
  unsigned int num_auxsym;

  if (Symbol == NULL) {
    return 0;
  }

  /* free the auxilary symbols */
  num_auxsym = Symbol->aux_list.num_nodes;
  if (num_auxsym > 0) {
    gp_list_delete(&Symbol->aux_list);
  }

  if (Symbol->name != NULL) {
    free(Symbol->name);
  }

  free(Symbol);

  return num_auxsym;
}

/*------------------------------------------------------------------------------------------------*/

/* Free the object. */

gp_boolean
gp_coffgen_free_object(gp_object_t *Object)
{
  if (Object == NULL) {
    return false;
  }

  gp_list_delete(&Object->section_list);
  gp_list_delete(&Object->dead_section_list);

  if (Object->section_ptr_array != NULL) {
    free(Object->section_ptr_array);
  }

  gp_list_delete(&Object->symbol_list);
  gp_list_delete(&Object->dead_symbol_list);

  if (Object->symbol_ptr_array != NULL) {
    free(Object->symbol_ptr_array);
  }

  if (Object->symbol_hashtable != NULL) {
    free(Object->symbol_hashtable);
  }

  if (Object->filename != NULL) {
    free(Object->filename);
  }

  free(Object);

  return true;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_coffgen_determine_aux_symbol(const gp_symbol_t *Symbol)
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

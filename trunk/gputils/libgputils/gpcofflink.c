/* GNU PIC coff linker functions
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

gp_boolean gp_relocate_to_shared = false;

/*------------------------------------------------------------------------------------------------*/

/* Two symbol tables are constructed. The first contains the definitions of all
   external symbols in all the object files.  This symbol table is used for
   relocation and linking.  The second table contains all external symbols
   that do not yet have a definition.  This table is used to determine which
   objects in a library are to be linked against.  This table should be empty
   at the begining of the relocation process. */

void
gp_cofflink_add_symbol(symbol_table_t *table, gp_symbol_type *symbol, gp_object_type *file)
{
  symbol_t           *sym;
  gp_coffsymbol_type *var;

  /* Search the for the symbol. If not found, then add it to the global symbol table. */
  sym = sym_get_symbol(table, symbol->name);

  if (sym != NULL) {
    return;
  }

  sym = sym_add_symbol(table, symbol->name);
  var = GP_Malloc(sizeof(*var));
  var->symbol = symbol;
  var->file   = file;
  sym_annotate_symbol(sym, var);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cofflink_remove_symbol(symbol_table_t *table, char *name)
{
  symbol_t           *sym;
  gp_coffsymbol_type *var;

  sym = sym_get_symbol(table, name);

  if (sym == NULL) {
    return;
  }

  var = sym_get_symbol_annotation(sym);
  free(var);
  sym_remove_symbol(table, name);
}

/*------------------------------------------------------------------------------------------------*/

/* Add the external symbols from an object file to the appropriate symbol
   tables. NOTE: The missing symbol table is optional. This feature is
   not used for generating symbol indexes for archives. */

gp_boolean
gp_cofflink_add_symbols(symbol_table_t *definition, symbol_table_t *missing, gp_object_type *object)
{
  gp_symbol_type     *symbol;
  symbol_t           *sym;
  gp_coffsymbol_type *var;

  if ((definition == NULL) || (object == NULL)) {
    return false;
  }

  /* The gp_convert_file() function has read it the elements of the "object". */

  symbol = object->symbols;

  while (symbol != NULL) {
    /* process all external symbols that are not directives */
    if ((symbol->class == C_EXT) && (symbol->name[0] != '.')) {
      if (symbol->section_number == 0) {
        /* This symbol is defined elsewhere. Check for it in the symbol
           definitions. If it doesn't exist there, add it to the missing
           symbol table, if not already entered. */
        sym = sym_get_symbol(definition, symbol->name);

        if ((sym == NULL) && (missing != NULL)) {
          gp_cofflink_add_symbol(missing, symbol, object);
        }
      }
      else {
        /* External symbol definition. See if it is already defined, it so
           it is an error. Add the symbol to the symbol definitions and remove
           it from the missing symbol table if it exists there. */
        sym = sym_get_symbol(definition, symbol->name);

        if (sym != NULL) {
          /* duplicate symbol */
          var = sym_get_symbol_annotation(sym);
          gp_error("Duplicate symbol \"%s\" defined in \"%s\" and \"%s\".",
                   symbol->name, var->file->filename, object->filename);
        }
        else {
          gp_cofflink_add_symbol(definition, symbol, object);
        }

        if (missing != NULL) {
          gp_cofflink_remove_symbol(missing, symbol->name);
        }
      }
    }

    symbol = symbol->next;
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* Combine all sections and symbols from all objects into one object file. */

void
gp_cofflink_combine_objects(gp_object_type *object)
{
  gp_object_type *list;

  /* assign the time the operation occured */
  object->time = (long)time(NULL);

  /* combine the symbol tables */
  list = object->next;
  while (list != NULL) {
    if (list->num_symbols != 0) {
      if (object->num_symbols == 0) {
        /* The object has no symbols. */
        object->symbols = list->symbols;
      }
      else {
        /* Append the symbols from the second object to the first. */
        object->symbols_tail->next = list->symbols;
      }
      object->symbols_tail = list->symbols_tail;
      object->num_symbols += list->num_symbols;
    }
    list = list->next;
  }

  /* append the sections onto the list */
  list = object->next;
  while (list != NULL) {
    if (list->num_sections != 0) {
      if (object->num_sections == 0) {
        /* The object has no sections. */
        object->sections = list->sections;
      }
      else {
        /* Append the sections from the second object to the first. */
        object->sections_tail->next = list->sections;
      }
      object->sections_tail = list->sections_tail;
      object->num_sections += list->num_sections;
    }
    list = list->next;
  }

  /* FIXME: breaking the chain isn't good */
  object->next = NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Cleanup the symbol table after combining objects. */

void
gp_cofflink_clean_table(gp_object_type *object, symbol_table_t *symbols)
{
  gp_section_type    *section;
  gp_reloc_type      *relocation;
  gp_symbol_type     *symbol;
  gp_coffsymbol_type *var;
  const symbol_t     *sym;
  gp_symbol_type     *previous;

  gp_debug("Cleaning symbol table.");

  /* point all relocations to the symbol definitions */
  section = object->sections;
  while (section != NULL) {
    relocation = section->relocations;
    while (relocation != NULL) {
      symbol = relocation->symbol;

      if ((symbol->class == C_EXT) && (symbol->section_number == N_UNDEF)) {
        /* This is an external symbol defined elsewhere. */
        sym = sym_get_symbol(symbols, symbol->name);
        assert(sym != NULL);
        var = sym_get_symbol_annotation(sym);
        assert(var != NULL);
        relocation->symbol = var->symbol;
        /* Increase the number of relocation links. */
        (relocation->symbol->num_reloc_link)++;
      }

      relocation = relocation->next;
    }

    section = section->next;
  }

  symbol   = object->symbols;
  previous = NULL;
  while (symbol != NULL) {
    if ((symbol->class == C_EXT) && (symbol->section_number == N_UNDEF)) {
      /* This is an external symbol defined elsewhere. */
      if (previous == NULL) {
        /* removing first symbol in the list */
        object->symbols = object->symbols->next;
      }
      else {
        previous->next = symbol->next;
      }
      gp_debug("  removed symbol \"%s\"", symbol->name);
      (object->num_symbols)--;
    }
    else {
      previous = symbol;
    }

    symbol = symbol->next;
  }

  return;
}

/*------------------------------------------------------------------------------------------------*/

/* Update the line number offsets. */

static void
_update_line_numbers(gp_linenum_type *line_number, unsigned int offset)
{
  while (line_number != NULL) {
    line_number->address += offset;
    line_number = line_number->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Combine overlay sections in an object file. */

void
gp_cofflink_combine_overlay(gp_object_type *object, gp_boolean remove_symbol)
{
  gp_section_type *first;
  gp_section_type *second;
  gp_symbol_type  *symbol;

  first = object->sections;

  while (first != NULL) {
    if (first->flags & STYP_OVERLAY) {
      second = gp_coffgen_find_section(object, first->next, first->name);

      if (second != NULL) {
        /* The sections must have the same properties or they can't be combined. */
        if (first->flags != second->flags) {
          gp_error("Section types for \"%s\" do not match.", first->name);
          continue;
        }
        else if ((first->flags & STYP_ABS) && (first->address != second->address)) {
          gp_error("Different addresses for absolute overlay sections \"%s\" (%#x != %#x).",
                   first->name, first->address, second->address);
          continue;
        }

        /* Set the size of the first section to the larger of the two. */
        if (second->size > first->size) {
          first->size = second->size;
          first->symbol->aux_list->_aux_symbol._aux_scn.length = second->size;
        }

        /* Remove the section symbol. */
        if (remove_symbol) {
          gp_coffgen_del_symbol(object, second->symbol);
        }

        /* Update the symbol table */
        symbol = object->symbols;
        while (symbol != NULL) {
          if (symbol->section == second) {
            symbol->section = first;
          }
          symbol = symbol->next;
        }

        /* Remove the second section. */
        gp_coffgen_del_section(object, second);

        /* Take another pass. */
        gp_cofflink_combine_overlay(object, remove_symbol);
        return;
      }
    }
    first = first->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Allocate memory for a stack. */

void
gp_cofflink_make_stack(gp_object_type *object, int num_bytes)
{
  gp_section_type *new;
  int              i;
  gp_symbol_type  *symbol;

  new = gp_coffgen_add_section(object, ".stack", NULL);
  new->flags = STYP_BSS;
  new->size = num_bytes;

  gp_debug("Allocating stack memory of size %#x (%i).", num_bytes, num_bytes);

  /* mark the memory locations as used */
  for (i = 0; i < num_bytes; i++) {
    b_memory_put(new->data, i, 0, ".stack", NULL);
  }

  /* create the symbol for the start address of the stack */
  symbol = gp_coffgen_find_symbol(object, "_stack");
  if ((symbol != NULL) && (symbol->section_number > 0)) {
    gp_error("_stack symbol already exists");
  }
  else {
    symbol = gp_coffgen_add_symbol(object);
    symbol->name           = GP_Strdup("_stack");
    symbol->value          = 0;
    symbol->section_number = 1;
    symbol->section        = new;
    symbol->type           = T_NULL;
    symbol->class          = C_EXT;
  }

  /* create the symbol for the end of the stack */
  symbol = gp_coffgen_find_symbol(object, "_stack_end");
  if ((symbol != NULL) && (symbol->section_number > 0)) {
    gp_error("_stack_end symbol already exists.");
  }
  else {
    symbol = gp_coffgen_add_symbol(object);
    symbol->name           = GP_Strdup("_stack_end");
    symbol->value          = num_bytes - 1;
    symbol->section_number = 1;
    symbol->section        = new;
    symbol->type           = T_NULL;
    symbol->class          = C_EXT;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Merge all sections in one object file with the same name. The overlayed
   sections must have been combined first.  */

void
gp_cofflink_merge_sections(gp_object_type *object)
{
  gp_section_type *first;
  gp_section_type *second;
  gp_symbol_type  *symbol;
  gp_reloc_type   *relocation;
  unsigned int     section_org;
  uint8_t          data;
  const char      *section_name;
  const char      *symbol_name;

  first = object->sections;
  while (first != NULL) {
    second = gp_coffgen_find_section(object, first->next, first->name);

    if (second != NULL) {
      /* The sections must have the same properties or they can't be combined. */
      if ((first->flags & STYP_ABS) ||
          (second->flags & STYP_ABS) ||
          (strcmp(first->name, ".config") == 0) ||
          (strcmp(first->name, ".idlocs") == 0)) {
        gp_error("File \"%s\", section \"%s\" (%#x) is absolute but occurs in more than one file.",
                 object->filename, first->name, first->address);
        exit(1);
      }

      gp_debug("  merging section \"%s\" with section \"%s\"", first->name, second->name);

      /* Update the addresses in the relocation table. */
      relocation = second->relocations;
      while (relocation != NULL) {
        relocation->address += first->size;
        relocation = relocation->next;
      }

      /* Copy the section data. */
      if (gp_has_data(second)) {
        unsigned int last   = second->size;
        unsigned int offset = first->size;
        unsigned int byte_addr;

        if (!gp_has_data(first)) {
          /* TODO optimization: adopt data from second by moving second->size bytes from org to org + offset */
          first->data = i_memory_create();
        }

        for (byte_addr = 0; byte_addr < last; byte_addr++) {
          if (b_memory_get(second->data, byte_addr, &data, &section_name, &symbol_name)) {
            b_memory_put(first->data, byte_addr + offset, data, section_name, symbol_name);
          }
          else {
            assert(0);
          }
        }
      }

      /* Update the line number offsets. */
      _update_line_numbers(second->line_numbers, first->size);

      if (first->flags & (STYP_TEXT | STYP_DATA_ROM)) {
        section_org = gp_processor_byte_to_org(object->class, first->size);
      }
      else {
        section_org = first->size;
      }

      /* Update the symbol table. */
      symbol = object->symbols;
      while (symbol != NULL) {
        if ((symbol->section_number > 0) && (symbol->section == second)) {
          symbol->section = first;
          symbol->value += section_org;
        }
        symbol = symbol->next;
      }

      /* Add section sizes. */
      first->size += second->size;

      /* Append the relocations from the second section to the first. */
      if (second->num_reloc != 0) {
        if (first->num_reloc == 0) {
          first->relocations = second->relocations;
        }
        else {
          first->relocations_tail->next = second->relocations;
          second->relocations->prev = first->relocations_tail;
        }
        first->num_reloc += second->num_reloc;
        first->relocations_tail = second->relocations_tail;
      }

      /* Append the line numbers from the second section to the first. */
      if (second->num_lineno != 0) {
        if (first->num_lineno == 0) {
          first->line_numbers = second->line_numbers;
        }
        else {
          first->line_numbers_tail->next = second->line_numbers;
          second->line_numbers->prev = first->line_numbers_tail;
        }
        first->num_lineno += second->num_lineno;
        first->line_numbers_tail = second->line_numbers_tail;
      }

      /* Remove the second section. */
      gp_coffgen_del_section(object, second);

      /* Take another pass. */
      gp_cofflink_merge_sections(object);
      return;
    }
    first = first->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* copy data from idata section to the ROM section */

static void
_copy_rom_section(const gp_object_type *object, const gp_section_type *idata, gp_section_type *rom)
{
  proc_class_t    class;
  int             from;
  int             stop;
  int             to;
  const char     *section_name;
  const char     *symbol_name;
  uint8_t         data;
  uint16_t        insn_retlw;

  class = object->class;
  from  = idata->address;
  stop  = from + idata->size;
  to    = rom->address;

  if (class->rom_width == 8) {
    /* PIC16E */
    for ( ; from < stop; ++from, ++to) {
      if (b_memory_get(idata->data, from, &data, &section_name, &symbol_name)) {
        b_memory_put(rom->data, to, data, section_name, symbol_name);
      }
    }
  }
  else {
    /* PIC12, PIC12E, PIC12I, SX, PIC14, PIC14E, PIC14EX, PIC16 */
    /* select "retlw" instruction */
    insn_retlw = gp_processor_retlw(class);

    for ( ; from < stop; ++from, to += 2) {
      if (b_memory_get(idata->data, from, &data, &section_name, &symbol_name)) {
        class->i_memory_put(rom->data, to, insn_retlw | data, section_name, symbol_name);
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

static char *
_create_i_section_name(const char *name)
{
  /* create the new section */
  size_t  len = strlen(name);
  char   *name_i = GP_Malloc(len + 3);

  memcpy(name_i, name, len);
  memcpy(name_i + len, "_i", 3);
  return name_i;
}

/*------------------------------------------------------------------------------------------------*/

/* create a program memory section to hold the data */

static void
_create_rom_section(gp_object_type *object, gp_section_type *section)
{
  gp_section_type *new;
  char            *name;

  /* create the new section */
  name = _create_i_section_name(section->name);
  new  = gp_coffgen_new_section(name, NULL);
  free(name);

  if (object->class->rom_width == 8) {
    /* PIC16E */
    new->size = section->size;
    /* force the section size to be an even number of bytes */
    if (section->size & 1) {
      b_memory_put(new->data, section->size, 0, object->filename, "adjust");
      new->size++;
    }
  }
  else {
    /* PIC12, PIC12E, PIC12I, SX, PIC14, PIC14E, PIC14EX, PIC16 */
    new->size = section->size << 1;
  }
  new->flags = STYP_DATA_ROM;

  /* Copy the data to get the MEM_USED_MASK correct. It is
     copied again later to ensure that any patched data is
     updated in the ROM section. */
  _copy_rom_section(object, section, new);

  /* insert the new ROM section after the idata section */
  if (section == object->sections_tail) {
    object->sections_tail = new;
  }
  new->next = section->next;
  section->next = new;

  object->num_sections++;
}

/*------------------------------------------------------------------------------------------------*/

/* write a word (16 bit) into four bytes of memory (non PIC16E) */

static void
_write_table_u16(proc_class_t class, const gp_section_type *section, int address, int insn,
                 int data, const char *symbol_name)
{
  class->i_memory_put(section->data, address,     insn | (data & 0xff), section->name, symbol_name);
  class->i_memory_put(section->data, address + 2, insn | (data >> 8),   section->name, symbol_name);
}

/*------------------------------------------------------------------------------------------------*/

/* write a long (32 bit) into four bytes of memory (PIC16E) */

static void
_write_table_u32(const proc_class_t class, const gp_section_type *section, int address, int data,
                 const char *symbol_name)
{
  class->i_memory_put(section->data, address,     data & 0xffff, section->name, symbol_name);
  class->i_memory_put(section->data, address + 2, data >> 16,    section->name, symbol_name);
}

/*------------------------------------------------------------------------------------------------*/

/* read a word from four bytes of memory (non PIC16E) */

static uint16_t
_read_table_u16(proc_class_t class, const gp_section_type *section, int address)
{
  uint16_t data[2];

  class->i_memory_get(section->data, address,     data,     NULL, NULL);
  class->i_memory_get(section->data, address + 2, data + 1, NULL, NULL);

  return ((data[0] & 0xff) | ((data[1] & 0xff) << 8));
}

/*------------------------------------------------------------------------------------------------*/

/* create the symbol for the start address of the table */

void
gp_cofflink_make_cinit(gp_object_type *object)
{
  gp_symbol_type *symbol;

  /* create the symbol for the start address of the table */
  /* TODO MPLINK 4.34 does not create this. We must implement the
     section address relocations RELOCT_SCN*. */
  symbol = gp_coffgen_find_symbol(object, "_cinit");

  if ((symbol != NULL) && (symbol->section_number > 0)) {
    gp_error("_cinit symbol already exists.");
  }
  else {
    symbol = gp_coffgen_add_symbol(object);
    symbol->name           = GP_Strdup("_cinit");
    symbol->value          = 0;
    symbol->section_number = 1;
    symbol->section        = NULL;
    symbol->type           = T_NULL;
    symbol->class          = C_EXT;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* create ROM data for initialized data sections */

void
gp_cofflink_make_idata(gp_object_type *object, gp_boolean force_cinit)
{
  gp_section_type *section;
  proc_class_t     class;
  gp_section_type *new;
  int              count_sections;
  int              byte_count;
  int              i;
  int              insn_retlw;
  gp_symbol_type  *symbol;

  count_sections = 0;
  section        = object->sections;
  while (section != NULL) {
    if (section->flags & STYP_DATA) {
      _create_rom_section(object, section);
      count_sections++;
    }
    section = section->next;
  }

  if ((count_sections > 0) || force_cinit) {
    class = object->class;
    new   = gp_coffgen_add_section(object, ".cinit", NULL);
    new->flags = STYP_DATA_ROM;

    byte_count = 2 + count_sections * 12;
    if (class->rom_width != 8) {
      /* PIC12, PIC12E, PIC12I, SX, PIC14, PIC14E, PIC14EX, PIC16 */
      /* retlw is used so 16-bit count is stored in 4 bytes not 2 */
      byte_count += 2;
    }

    new->size = byte_count;

    /* load the table with data */
    for (i = 0; i < byte_count; i++) {
      b_memory_put(new->data, i, 0, ".cinit", "table");
    }

    if (class->rom_width == 8) {
      /* PIC16E */
      class->i_memory_put(new->data, 0, count_sections, ".cinit", "table_size");
    }
    else {
      /* PIC12, PIC12E, PIC12I, SX, PIC14, PIC14E, PIC14EX, PIC16 */
      insn_retlw = gp_processor_retlw(class);
      _write_table_u16(class, new, 0, insn_retlw, count_sections, "table_size");
    }

    /* update the section pointer in _cinit */
    symbol = gp_coffgen_find_symbol(object, "_cinit");

    if (!force_cinit) {
      assert(symbol != NULL);
    }

    if (symbol != NULL) {
      symbol->section = new;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* load the relocated sections addresses in the table */

void
gp_cofflink_add_cinit_section(gp_object_type *object)
{
  gp_section_type       *section;
  proc_class_t           class;
  const gp_section_type *new;
  const gp_section_type *prog_section;
  int                    insn_retlw;
  int                    count_sections;
  int                    base_address;
  char                  *prog_name;
  uint16_t               number;

  new = gp_coffgen_find_section(object, object->sections, ".cinit");

  if (new != NULL) {
    /* scan through the sections to determine the addresses */
    count_sections = 0;
    base_address   = new->address;
    class          = object->class;

    if (class->rom_width == 8) {
      /* PIC16E */
      base_address += 2;
      insn_retlw = 0;
    }
    else {
      /* PIC12, PIC12E, PIC12I, SX, PIC14, PIC14E, PIC14EX, PIC16 */
      base_address += 4;
      insn_retlw = gp_processor_retlw(class);
    }

    section = object->sections;
    while (section != NULL) {
      if (section->flags & STYP_DATA) {
        /* locate the rom table */
        prog_name    = _create_i_section_name(section->name);
        prog_section = gp_coffgen_find_section(object, object->sections, prog_name);
        free(prog_name);

        if (class->rom_width == 8) {
          /* PIC16E */
          /* Write program memory address (from: source address of data in CODE space). */
          _write_table_u32(class, new, base_address,
                           gp_processor_byte_to_org(class, prog_section->address), "prog_mem_addr");

          /* Write data memory address (to: destination address of values in DATA space). */
          _write_table_u32(class, new, base_address + 4, section->address, "data_mem_addr");

          /* Write the table size (size: number of bytes to copy from CODE to DATA). */
          _write_table_u32(class, new, base_address + 8, section->size, "table_size");
        }
        else {
          /* PIC12, PIC12E, PIC12I, SX, PIC14, PIC14E, PIC14EX, PIC16 */
          /* Write program memory address (from: source address of data in CODE space). */
          _write_table_u16(class, new, base_address, insn_retlw,
                           gp_processor_byte_to_org(class, prog_section->address), "prog_mem_addr");

          /* Write data memory address (to: destination address of values in DATA space). */
          _write_table_u16(class, new, base_address + 4, insn_retlw, section->address, "data_mem_addr");

          /* Write the table size (size: number of bytes to copy from CODE to DATA). */
          _write_table_u16(class, new, base_address + 8, insn_retlw, section->size, "table_size");
        }

        count_sections++;
        base_address += 12;
      } /* if (section->flags & STYP_DATA) */

      section = section->next;
    } /* while (section != NULL) */

    /* make sure the section count matches */
    if (class->rom_width == 8) {
      /* PIC16E */
      class->i_memory_get(new->data, new->address, &number, NULL, NULL);
    }
    else {
      /* PIC12, PIC12E, PIC12I, SX, PIC14, PIC14E, PIC14EX, PIC16 */
      number = _read_table_u16(class, new, new->address);
    }
    assert(number == count_sections);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Set the memory used flags in a block of words. */

static void
_set_used(const gp_object_type *object, MemBlock *m, int org_to_byte_shift, unsigned int address,
          unsigned int size, const char *type, const char *section_name,
          gp_boolean p16e_align_needed)
{
  uint8_t               data;
  const gp_symbol_type *symbol;
  const char           *symbol_name;
  const char           *old_section_name;
  const char           *old_symbol_name;

  if (p16e_align_needed && (size & 1)) {
    /* code_pack --> STYP_BPACK */
    gp_debug("    align to even size: %u ==> %u", size, size + 1);
    ++size;
  }

  gp_debug("      marking %#x (%u) words from %#x to %#x as used",
           size, size,
           gp_byte_to_org(org_to_byte_shift, address),
           gp_byte_to_org(org_to_byte_shift, address + size - 1));

  for ( ; size; address++, size--) {
    if (b_memory_get(m, address, &data, &old_section_name, &old_symbol_name)) {
      if ((old_section_name != NULL) && (section_name != NULL)) {
        symbol      = gp_symbol_find(object, section_name, address);
        symbol_name = (symbol != NULL) ? symbol->name : NULL;
        gp_error("More %s sections use same address: %#x -- \"%s/%s\", \"%s/%s\"",
                 type, gp_byte_to_org(org_to_byte_shift, address),
                 old_section_name, old_symbol_name, section_name, symbol_name);
      }
      else {
        gp_error("More %s sections use same address: %#x", type, gp_byte_to_org(org_to_byte_shift, address));
      }
      return;
    }
    else {
      symbol      = gp_symbol_find(object, section_name, address);
      symbol_name = (symbol != NULL) ? symbol->name : NULL;
      b_memory_put(m, address, 0, section_name, symbol_name);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* allocate space for the absolute sections */

void
gp_cofflink_reloc_abs(gp_object_type *object, MemBlock *m,
                      int org_to_byte_shift, unsigned long flags)
{
  gp_section_type *section;
  int              org;
  gp_boolean       p16e_align_needed;

  section = object->sections;
  while (section != NULL) {
    if ((section->flags & STYP_ABS) && (section->flags & flags)) {
      /* Workaround for the "odd size memory problem" in the PIC16E class.
         code_pack --> STYP_BPACK */
      p16e_align_needed = false;

      if ((object->class == PROC_CLASS_PIC16E) &&
          (section->flags & (STYP_TEXT | STYP_DATA_ROM)) &&
          (section->size & 1)) {
        org = gp_processor_byte_to_real(object->processor, section->address);

        if ((gp_processor_is_idlocs_org(object->processor, org) < 0) &&
            (gp_processor_is_config_org(object->processor, org) < 0) &&
            (gp_processor_is_eeprom_org(object->processor, org) < 0)) {
          p16e_align_needed = true;
        }
      }

      _set_used(object, m, org_to_byte_shift, section->address, section->size, "absolute",
                section->name, p16e_align_needed);

      /* Set the relocated flag. */
      section->flags |= STYP_RELOC;
    }
    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Search through all the sections in the object list. Locate the biggest
   assigned section that has not been relocated. */

static gp_section_type *
_find_big_assigned(gp_section_type *section, unsigned long flags, symbol_table_t *logical_sections)
{
  gp_section_type *biggest;
  const symbol_t  *sym;

  biggest = NULL;
  while (section != NULL) {
    sym = sym_get_symbol(logical_sections, section->name);

    if ((sym != NULL) && (section->flags & flags) && !(section->flags & STYP_RELOC)) {
      /* This section has not been relocated. */
      if ((biggest == NULL) || (section->size > biggest->size)) {
        biggest = section;
      }
    }
    section = section->next;
  }

  return biggest;
}

/*------------------------------------------------------------------------------------------------*/

/* Search through all the sections in the object list.  Locate the biggest
   section that has not been relocated. */

static gp_section_type *
_find_big_section(gp_section_type *section, unsigned long flags)
{
  gp_section_type *biggest;

  biggest = NULL;
  while (section != NULL) {
    if ((section->flags & flags) && !(section->flags & STYP_RELOC)) {
      /* This section has not been relocated. */
      if ((biggest == NULL) || (section->size > biggest->size)) {
        biggest = section;
      }
    }
    section = section->next;
  }

  if (biggest != NULL) {
    gp_debug("  biggest section = %s, section flags = %#x, flags = %#x",
             biggest->name, biggest->flags, flags);
  }

  return biggest;
}

/*------------------------------------------------------------------------------------------------*/

/* Search through the target memory. Locate the smallest block of memory
   that is larger than the requested size. Return the address of that
   block. */

static gp_boolean
_search_memory(const MemBlock *m, int org_to_byte_shift,
               unsigned int start, unsigned int stop, unsigned int size,
               unsigned int *block_address, unsigned int *block_size,
               gp_boolean stop_at_first)
{
  unsigned int address;
  unsigned int current_address = 0;
  unsigned int current_size = 0;
  gp_boolean   mem_used;
  gp_boolean   in_block = false;
  gp_boolean   end_block = false;
  gp_boolean   success = false;
  uint8_t      byte;

  /* set the size to max value */
  *block_size = (unsigned int)(-1);

  for (address = start; address <= stop; address++) {
    mem_used = b_memory_get(m, address, &byte, NULL, NULL);

    if (address == stop) {
      if (in_block) {
        /* end of the section definition */
        end_block = true;
        /* increment for last address */
        current_size++;
      }
      else if (start == stop) {
        /* special case, one word section */
        if (! mem_used) {
          end_block = true;
          current_address = start;
          current_size    = 1;
        }
      }
      in_block = false;
    }
    else if (mem_used) {
      if (in_block) {
        /* end of an unused block of memory */
        end_block = true;
      }
      in_block = false;
    }
    else {
      if (! in_block) {
        /* start of an unused block of memory */
        gp_debug("    start unused block at %#x", gp_byte_to_org(org_to_byte_shift, address));
        current_address = address;
        current_size    = 1;
      }
      else {
        /* continuation of an unused block of memory */
        current_size++;
      }
      in_block = true;
    }

    if (end_block) {
      gp_debug("    end unused block at %#x with size %#x",
               gp_byte_to_org(org_to_byte_shift, address),
               gp_byte_to_org(org_to_byte_shift, current_size));
      if (current_size >= size) {
        if (stop_at_first) {
          *block_size    = current_size;
          *block_address = current_address;
          success = true;
          break;
        }
        else if (current_size < *block_size) {
          *block_size    = current_size;
          *block_address = current_address;
          success = true;
        }
      }
      end_block = false;
    }
  }

  return success;
}

/*------------------------------------------------------------------------------------------------*/

/* Move data in i_memory. This function assumes the move will be towards
   a higher address. */

static void
_move_data(MemBlock *m, unsigned int address, unsigned int size, unsigned int new_address)
{
  int         org;
  uint8_t     data;
  const char *section_name;
  const char *symbol_name;

  if (address == new_address) {
    return;
  }

  gp_debug("    moving %#x (%u) bytes from %#x to %#x", size, size, address, new_address);

  for (org = address + size - 1; org >= 0; org--) {
    b_memory_assert_get(m, org, &data, &section_name, &symbol_name);
    gp_debug("      moving byte %#x from %#x to %#x", data, org, new_address + org);
    b_memory_put(m, new_address + org, data, section_name, symbol_name);
    b_memory_clear(m, org);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* map real addres to shadow address */

static unsigned int
_map_to_shadow_address(linker_section_t *section_def, unsigned int address)
{
  if (section_def->shadow_sym != NULL) {
    unsigned int new_address = address - section_def->start + section_def->shadow_val;

    gp_debug("      mapping shadow address %#x => %#x", address, new_address);
    return new_address;
  }
  else {
    return address;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* unmap real addres from shadow address */

static unsigned int
_unmap_from_shadow_address(linker_section_t *section_def, unsigned int address)
{
  if (section_def->shadow_sym != NULL) {
    unsigned int new_address = address - section_def->shadow_val + section_def->start;

    gp_debug("      unmapping shadow address %#x => %#x", address, new_address);
    return new_address;
  }
  else {
    return address;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Compare function for sym_clone_symbol_array(). */

static int
_sect_addr_cmp(const void *P0, const void *P1)
{
  const symbol_t         *sym0 = *(const symbol_t **)P0;
  const symbol_t         *sym1 = *(const symbol_t **)P1;
  const linker_section_t *sect0;
  const linker_section_t *sect1;

  sect0 = sym_get_symbol_annotation(sym0);
  sect1 = sym_get_symbol_annotation(sym1);

  if (sect0->start < sect1->start) {
    return -1;
  }

  if (sect0->start > sect1->start) {
    return 1;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

/* allocate memory for relocatable assigned sections */

void
gp_cofflink_reloc_assigned(gp_object_type *object, MemBlock *m, int org_to_byte_shift,
                           unsigned long flags, symbol_table_t *sections,
                           symbol_table_t *logical_sections)
{
  gp_section_type  *section;
  gp_section_type  *current;
  const symbol_t   *sym;
  char             *section_name;
  linker_section_t *section_def;
  unsigned int      current_shadow_address;
  unsigned int      current_size;
  int               org;
  gp_boolean        p16e_align_needed;

  section = object->sections;
  while (true) {
    current = _find_big_assigned(section, flags, logical_sections);

    if (current == NULL) {
      break;
    }

    /* Fetch the logical section. */
    sym = sym_get_symbol(logical_sections, current->name);
    assert(sym != NULL);

    /* Fetch the section definition. */
    section_name = (char *)sym_get_symbol_annotation(sym);
    sym          = sym_get_symbol(sections, section_name);
    assert(sym != NULL);

    section_def = (linker_section_t *)sym_get_symbol_annotation(sym);
    assert(section_def != NULL);

    p16e_align_needed = false;

    /* Workaround for the "odd size memory problem" in the PIC16E class.
       code_pack --> STYP_BPACK */
    if ((object->class == PROC_CLASS_PIC16E) &&
        (current->flags & flags) &&
        (current->flags & STYP_ABS) &&
        (current->flags & (STYP_TEXT | STYP_DATA_ROM)) &&
        (current->size & 1)) {
      org = gp_processor_byte_to_real(object->processor, current->address);

      if ((gp_processor_is_idlocs_org(object->processor, org) < 0) &&
          (gp_processor_is_config_org(object->processor, org) < 0) &&
          (gp_processor_is_eeprom_org(object->processor, org) < 0)) {
        p16e_align_needed = true;
      }
    }

    /* assign the address to this section */
    if (_search_memory(m, org_to_byte_shift,
                       _map_to_shadow_address(section_def, section_def->start),
                       _map_to_shadow_address(section_def, section_def->end),
                       (p16e_align_needed) ? (current->size + 1) : current->size,
                       &current_shadow_address, &current_size, false) == 1) {
      gp_debug("    logical section: '%s'", current->name);
      gp_debug("    section name   : '%s'", section_name);
      gp_debug("    successful relocation to %#x", gp_byte_to_org(org_to_byte_shift, current_shadow_address));

      if (gp_has_data(current)) {
        _move_data(current->data, current->shadow_address, current->size, current_shadow_address);
      }

      current->shadow_address = current_shadow_address;
      current->address        = _unmap_from_shadow_address(section_def, current_shadow_address);
      _set_used(object, m, 0, current_shadow_address, current->size, "relocatable assigned",
                section_name, p16e_align_needed);

      /* Update the line number offsets. */
      _update_line_numbers(current->line_numbers, current->address);

      /* Set the relocated flag. */
      current->flags |= STYP_RELOC;
    }
    else {
      gp_error("No target memory available for section \"%s\".", current->name);
      return;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* allocate memory for cinit section */

void
gp_cofflink_reloc_cinit(gp_object_type *object, MemBlock *m, int org_to_byte_shift,
                        gp_section_type *cinit_section, const symbol_table_t *sections)
{
  unsigned int       size;
  gp_boolean         success;
  gp_boolean         type_avail;
  unsigned int       current_shadow_address;
  unsigned int       current_size;
  unsigned int       smallest_shadow_address;
  unsigned int       smallest_address;
  size_t             i;
  const symbol_t   **sym_list;
  const symbol_t    *sym;
  size_t             sym_count;
  linker_section_t  *section_def;

  if ((cinit_section == NULL) || (cinit_section->flags & STYP_RELOC)) {
    return;
  }

  size = cinit_section->size;

  gp_debug("  Relocating cinit code.");

  success                 = false;
  type_avail              = false;
  smallest_shadow_address = (unsigned int)(-1);
  smallest_address        = (unsigned int)(-1);
  sym_list                = sym_clone_symbol_array(sections, _sect_addr_cmp);

  /* Search the section definitions for the smallest block of memory that the section will fit in. */

  sym_count = sym_get_symbol_count(sections);
  for (i = 0; i < sym_count; ++i) {
    sym         = sym_list[i];
    section_def = (linker_section_t *)sym_get_symbol_annotation(sym);

    if ((section_def->type == SECT_CODEPAGE) && !section_def->protected) {
      gp_debug("  section     = '%s'",     cinit_section->name);
      gp_debug("    name      = '%s'",     sym_get_symbol_name(sym));
      gp_debug("    size      = %#x (%u)", cinit_section->size, cinit_section->size);
      gp_debug("    def start = %#x",      section_def->start);
      gp_debug("    def end   = %#x",      section_def->end);

      if (section_def->shadow_sym != NULL) {
        gp_debug("    def shadow_sym = %s",  section_def->shadow_sym);
        gp_debug("    def shadow_val = %#x", section_def->shadow_val);
      }

      type_avail = true;

      if (_search_memory(m, org_to_byte_shift,
                         _map_to_shadow_address(section_def, section_def->start),
                         _map_to_shadow_address(section_def, section_def->end),
                         size, &current_shadow_address, &current_size, true) == 1) {
        success = true;

        if (smallest_shadow_address > current_shadow_address) {
          smallest_shadow_address = current_shadow_address;
          smallest_address        = _unmap_from_shadow_address(section_def, smallest_shadow_address);
        }
      }
    }
  }

  free(sym_list);

  /* set the memory used flag for all words in the block */
  if (success) {
    gp_debug("    successful relocation to %#x", gp_byte_to_org(org_to_byte_shift, smallest_shadow_address));

    if (gp_has_data(cinit_section)) {
      _move_data(cinit_section->data, cinit_section->shadow_address, size, smallest_shadow_address);
    }
    cinit_section->shadow_address = smallest_shadow_address;
    cinit_section->address        = smallest_address;
    _set_used(object, m, 0, smallest_shadow_address, size, "cinit", cinit_section->name, false);

    /* Update the line number offsets. */
    _update_line_numbers(cinit_section->line_numbers, cinit_section->address);

    /* Set the relocated flag. */
    cinit_section->flags |= STYP_RELOC;
  }
  else if (!type_avail) {
    gp_error("Linker script has no definition that matches the type of section \"%s\".",
             cinit_section->name);
  }
  else {
    gp_error("No target memory available for section \"%s\".", cinit_section->name);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* allocate memory for relocatable unassigned sections */

void
gp_cofflink_reloc_unassigned(gp_object_type *object, MemBlock *m, int org_to_byte_shift,
                             unsigned long flags, const symbol_table_t *sections)
{
  gp_section_type   *section;
  gp_section_type   *current;
  enum section_type  type;
  unsigned int       size;
  gp_boolean         first_time;
  gp_boolean         success;
  gp_boolean         type_avail;
  size_t             i;
  const symbol_t   **sym_list;
  const symbol_t    *sym;
  size_t             sym_count;
  linker_section_t  *section_def;
  unsigned int       current_shadow_address;
  unsigned int       current_size;
  unsigned int       smallest_shadow_address;
  unsigned int       smallest_address;
  unsigned int       smallest_size;
  const char        *msg;
  int                org;
  gp_boolean         p16e_align_needed;

  section = object->sections;
  while (true) {
    current = _find_big_section(section, flags);

    if (current == NULL) {
      break;
    }

    size = current->size;
    p16e_align_needed = false;

    /* determine what type of sections are being relocated */
    if (current->flags & (STYP_TEXT | STYP_DATA_ROM)) {
      type = SECT_CODEPAGE;
      msg  = "relocating codepage";
      gp_debug("  relocating code");
    }
    else if (current->flags & STYP_ACCESS) {
      type = SECT_ACCESSBANK;
      msg  = "relocating accessbank";
      gp_debug("  relocating accessbank");
    }
    else if (current->flags & STYP_SHARED) {
      type = SECT_SHAREBANK;
      msg  = "relocating sharebank";
      gp_debug("  relocating sharebank");
    }
    else {
      type = SECT_DATABANK;
      msg  = "relocating databank";
      gp_debug("  relocating data");
    }

    first_time = true;

    /* Workaround for the "odd size memory problem" in the PIC16E class. */
    if ((object->class == PROC_CLASS_PIC16E) && (type == SECT_CODEPAGE) && (size & 1)) {
      org = gp_processor_byte_to_real(object->processor, current->address);

      if ((gp_processor_is_idlocs_org(object->processor, org) < 0) &&
          (gp_processor_is_config_org(object->processor, org) < 0) &&
          (gp_processor_is_eeprom_org(object->processor, org) < 0)) {
        p16e_align_needed = true;
      }
    }


next_pass:

    success                 = false;
    type_avail              = false;
    smallest_shadow_address = 0;
    smallest_address        = 0;
    smallest_size           = (unsigned int)(-1);
    sym_list                = sym_clone_symbol_array(sections, _sect_addr_cmp);

    /* search the section definitions for the smallest block of memory that
       the section will fit in */
    sym_count = sym_get_symbol_count(sections);
    for (i = 0; i < sym_count; ++i) {
      sym         = sym_list[i];
      section_def = sym_get_symbol_annotation(sym);

      if ((section_def->type == type) && (!section_def->protected)) {
        gp_debug("  section     = '%s'",     current->name);
        gp_debug("    name      = %s",       sym_get_symbol_name(sym));
        gp_debug("    size      = %#x (%u)", size, size);
        gp_debug("    def start = %#x",      section_def->start);
        gp_debug("    def end   = %#x",      section_def->end);

        if (section_def->shadow_sym != NULL) {
          gp_debug("    def shadow_sym = %s",  section_def->shadow_sym);
          gp_debug("    def shadow_val = %#x", section_def->shadow_val);
        }

        type_avail = true;

        if (_search_memory(m, org_to_byte_shift,
                           _map_to_shadow_address(section_def, section_def->start),
                           _map_to_shadow_address(section_def, section_def->end),
                           (p16e_align_needed) ? (size + 1) : size,
                           &current_shadow_address, &current_size, false) == 1) {
          success = true;

          if (smallest_size > current_size) {
            smallest_size           = current_size;
            smallest_shadow_address = current_shadow_address;
            smallest_address        = _unmap_from_shadow_address(section_def, smallest_shadow_address);
          }
        }
      }
    }

    free(sym_list);

    /* set the memory used flag for all words in the block */
    if (success) {
      gp_debug("    successful relocation to %#x", gp_byte_to_org(org_to_byte_shift, smallest_shadow_address));

      if (gp_has_data(current)) {
        _move_data(current->data, current->shadow_address, size, smallest_shadow_address);
      }

      current->shadow_address = smallest_shadow_address;
      current->address        = smallest_address;
      _set_used(object, m, 0, smallest_shadow_address, size, msg, current->name, p16e_align_needed);

      /* Update the line number offsets */
      _update_line_numbers(current->line_numbers, current->address);

      /* Set the relocated flag */
      current->flags |= STYP_RELOC;
    }
    else if (gp_relocate_to_shared && first_time && (type == SECT_DATABANK)) {
      first_time = false;
      type = SECT_SHAREBANK;
      gp_warning("Relocation of section \"%s\" failed, relocating to a shared memory location.", current->name);
      goto next_pass;
    }
    else if (!type_avail) {
      gp_error("Linker script has no definition that matches the type of section \"%s\".", current->name);
      return;
    }
    else {
      gp_error("No target memory available for section \"%s\".", current->name);
      return;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Update all symbols with their new relocated values. */

void
gp_cofflink_update_table(gp_object_type *object, int org_to_byte_shift)
{
  gp_symbol_type  *symbol;
  gp_section_type *section;
  gp_section_type *sym_sect;
  int              offset;

  gp_debug("Updating symbols with their new relocated values.");

  symbol = object->symbols;
  while (symbol != NULL) {
    if (symbol->section_number > 0) {
      sym_sect = symbol->section;
      assert(sym_sect != NULL);

      if (!(sym_sect->flags & STYP_ABS)) {
        offset = sym_sect->address;

        if (sym_sect->flags & (STYP_TEXT | STYP_DATA_ROM)) {
          offset = gp_byte_to_org(org_to_byte_shift, offset);
        }

        symbol->value += offset;
      }
    }

    symbol = symbol->next;
  }

  gp_debug("Stripping section relocated flag.");

  section = object->sections;
  while (section != NULL) {
    section->flags &= ~(STYP_RELOC);
    section = section->next;
  }
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

void
gp_cofflink_make_linenum_arrays(gp_object_type *object)
{
  gp_section_type  *section;
  gp_linenum_type  *linenum;
  gp_linenum_type **array;
  unsigned int      n_linenums;
  unsigned int      prev_num;
  unsigned int      i;

  section = object->sections;
  while (section != NULL) {
    n_linenums = section->num_lineno;
    array      = (gp_linenum_type **)GP_Calloc(n_linenums, sizeof(gp_linenum_type *));
    i          = 0;
    prev_num   = (unsigned int)(-1);
    linenum    = section->line_numbers;
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

gp_linenum_type *
gp_cofflink_find_linenum(const gp_section_type *section, const gp_symbol_type *symbol,
                         unsigned int line_number)
{
  gp_linenum_type   linenum;
  gp_linenum_type  *ptr;
  gp_linenum_type **ret;

  if ((section == NULL) || (section->line_numbers_array == NULL)) {
    return NULL;
  }

  linenum.symbol      = symbol;
  linenum.line_number = line_number;
  ptr = &linenum;
  ret = (gp_linenum_type **)bsearch(&ptr, section->line_numbers_array, section->line_numbers_array_length,
                                    sizeof(gp_linenum_type *), _linenum_cmp);
  return ((ret != NULL) ? *ret : NULL);
}

/*------------------------------------------------------------------------------------------------*/

/* Create sections to fill unused memory in the pages with constant data. */

void
gp_cofflink_fill_pages(gp_object_type *object, MemBlock *m, const symbol_table_t *sections)
{
  linker_section_t  *section_def;
  size_t             i;
  const symbol_t   **sym_list;
  const symbol_t    *sym;
  size_t             sym_count;
  gp_boolean         found;
  char               fill_name[BUFSIZ];
  int                fill_number;
  gp_section_type   *section;
  unsigned int       current_shadow_address;
  unsigned int       current_size;
  int                org;
  int                end;

  gp_debug("Adding fill sections.");
  sym_list = sym_clone_symbol_array(sections, _sect_addr_cmp);

  /* search for any section definitions that have a fill */
  sym_count   = sym_get_symbol_count(sections);
  fill_number = 1;
  for (i = 0; i < sym_count; ++i) {
    sym         = sym_list[i];
    section_def = (linker_section_t *)sym_get_symbol_annotation(sym);

    if ((section_def->type == SECT_CODEPAGE) && (section_def->use_fill)) {
      while (true) {
        found = _search_memory(m,
                               object->class->org_to_byte_shift,
                               _map_to_shadow_address(section_def, section_def->start),
                               _map_to_shadow_address(section_def, section_def->end),
                               1,
                               &current_shadow_address,
                               &current_size, false);
        if (found) {
          snprintf(fill_name, sizeof(fill_name), ".fill_%i", fill_number++);
          gp_debug("  new section \"%s\" at %#x with size %#x and data %#x",
                   fill_name, current_shadow_address, current_size, section_def->fill);
          section = gp_coffgen_find_section(object, object->sections, fill_name);

          if (section != NULL) {
            gp_error("Fill section \"%s\" already exists.", fill_name);
            free(sym_list);
            return;
          }
          else {
            /* create a new section for the fill data */
            section                 = gp_coffgen_add_section(object, fill_name, NULL);
            section->address        = _unmap_from_shadow_address(section_def, current_shadow_address);
            section->shadow_address = current_shadow_address;
            section->size           = current_size;
            section->flags          = STYP_TEXT;
            /* FIXME: do we really need a section symbol? */

            /* mark the memory as used */
            _set_used(object, m, object->class->org_to_byte_shift,
                      current_shadow_address, current_size, "fill", section->name, false);

            /* fill the section memory */
            org = current_shadow_address;
            end = org + current_size;
            for ( ; org < end; org += 2) {
              object->class->i_memory_put(section->data, org, section_def->fill, section->name, NULL);
            }
          }
        }
        else {
          break;
        }
      }
    }
  }

  free(sym_list);
}

/*------------------------------------------------------------------------------------------------*/

static void
_check_relative(const gp_section_type *section, int org, int argument, int range)
{
  /* If the branch is too far then issue a warning */
  if ((argument > range) || (argument < -(range + 1))) {
    gp_warning("Relative branch out of range in at %#x of section \"%s\".", org, section->name);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* patch one word with the relocated address */

static void
_patch_addr(proc_class_t class, int num_pages, int num_banks, int bsr_boundary,
            gp_section_type *section, const gp_reloc_type *relocation)
{
  const gp_symbol_type *symbol;
  int                   byte_addr;
  int                   value;
  uint16_t              current_value;
  int                   data;
  int                   offset;
  int                   bank;
  int                   page;
  gp_boolean            write_data;

  symbol    = relocation->symbol;
  byte_addr = relocation->address + section->address;
  value     = symbol->value       + relocation->offset;

  gp_debug("  patching at %#x from %s/%s with %#x", byte_addr, section->name, symbol->name, value);

  /* fetch the current contents of the memory */
  class->i_memory_get(section->data, byte_addr, &current_value, NULL, NULL);

  /* FIXME: Not sure if warnings should be generated for out of range
            arguments. The linker should make sure values are within
            ranges in the linker scripts. */

  data       = 0;
  write_data = true;
  switch (relocation->type) {
  case RELOCT_ALL:
    data = value & 0xffff;
    break;

  case RELOCT_CALL:
    data = class->reloc_call(value);
    break;

  case RELOCT_GOTO:
    data = class->reloc_goto(value);
    break;

  case RELOCT_LOW:
    data = value & 0xff;
    break;

  case RELOCT_HIGH:
    data = class->reloc_high((symbol->section->flags & (STYP_TEXT | STYP_DATA_ROM)), value);
    break;

  case RELOCT_UPPER:
    data = (value >> 16) & 0xff;
    break;

  case RELOCT_P:
    data = (value & 0x1f) << 8;
    break;

  case RELOCT_BANKSEL:
    bank = gp_processor_check_bank(class, value);
    gp_processor_set_bank(class, num_banks, bank, section->data, byte_addr);
    write_data = false;
    break;

  case RELOCT_IBANKSEL:
    bank = gp_processor_check_ibank(class, value);
    gp_processor_set_ibank(class, num_banks, bank, section->data, byte_addr);
    write_data = false;
    break;

  case RELOCT_F:
    data = class->reloc_f(value);
    break;

  case RELOCT_TRIS:
  case RELOCT_TRIS_3BIT:
    data = class->reloc_tris(value);
    break;

  case RELOCT_MOVLR:
    data = (value << 4) & 0xf0;
    break;

  case RELOCT_MOVLB:
    data = class->reloc_movlb(value);
    break;

  case RELOCT_GOTO2:
    /* This is only used for PIC16E (pic18). */
    data = (value >> 9) & 0xfff;
    break;

  case RELOCT_FF1:
  case RELOCT_FF2:
    data = value & 0xfff;
    break;

  case RELOCT_LFSR1:
    data = (value >> 8) & 0x00f;
    break;

  case RELOCT_LFSR2:
    data = value & 0x0ff;
    break;

  case RELOCT_BRA:
    data = class->reloc_bra(section, value, byte_addr);
    break;

  case RELOCT_CONDBRA:
    {
      /* This is only used for PIC16E (pic18). */
      offset = value - byte_addr - 2;

      if (offset & 1) {
        if (symbol->name != NULL) {
          gp_warning("Destination address must be word aligned at %#x of section \"%s\" at symbol: \"%s\".",
                     byte_addr, section->name, symbol->name);
        }
        else {
          gp_warning("Destination address must be word aligned at %#x of section \"%s\".",
                     byte_addr, section->name);
        }
      }

      offset >>= 1;
      _check_relative(section, byte_addr, offset, 0x7f);
      data = offset & 0xff;
    }
    break;

  case RELOCT_ACCESS:
    if (((value >= 0) && (value < bsr_boundary)) ||
        ((value >= (0xf00 + bsr_boundary)) && (value < 0x1000))) {
      data = 0;
    }
    else {
      data = 0x100;
    }
    break;

  case RELOCT_PAGESEL_WREG:
    page = gp_processor_check_page(class, value);
    gp_processor_set_page(class, num_pages, page, section->data, byte_addr, true);
    write_data = false;
    break;

  case RELOCT_PAGESEL_BITS:
  case RELOCT_PAGESEL_MOVLP:
    page = gp_processor_check_page(class, value);
    gp_processor_set_page(class, num_pages, page, section->data, byte_addr, false);
    write_data = false;
    break;

  /* unimplemented relocations */
  case RELOCT_PAGESEL:
  case RELOCT_SCNSZ_LOW:
  case RELOCT_SCNSZ_HIGH:
  case RELOCT_SCNSZ_UPPER:
  case RELOCT_SCNEND_LOW:
  case RELOCT_SCNEND_HIGH:
  case RELOCT_SCNEND_UPPER:
  case RELOCT_SCNEND_LFSR1:
  case RELOCT_SCNEND_LFSR2:
  default: {
      if (symbol->name != NULL) {
        gp_error("Unimplemented relocation = %s (%u) in section \"%s\" at symbol \"%s\".",
                 gp_coffgen_reloc_type_to_str(relocation->type),
                 relocation->type, section->name, symbol->name);
      }
      else {
        gp_error("Unimplemented relocation = %s (%u) in section \"%s\".",
                 gp_coffgen_reloc_type_to_str(relocation->type),
                 relocation->type, section->name);
      }
      assert(0);
    }
  }

  /* update memory with the new value */
  if (write_data) {
    class->i_memory_put(section->data, byte_addr, current_value | data, section->name, symbol->name);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Patch all addresses with the relocated symbols. The relocations are
   stripped from the sections. */

void
gp_cofflink_patch(gp_object_type *object)
{
  proc_class_t         class;
  gp_section_type     *section;
  const gp_reloc_type *relocation;
  int                  num_pages;
  int                  num_banks;
  int                  bsr_boundary;

  bsr_boundary = gp_processor_bsr_boundary(object->processor);
  num_pages    = gp_processor_num_pages(object->processor);
  num_banks    = gp_processor_num_banks(object->processor);

  gp_debug("Patching data with relocated symbols.");

  class   = object->class;
  section = object->sections;
  while (section != NULL) {
    if (gp_has_data(section)) {
      /* patch raw data with relocation entries */
      relocation = section->relocations;
      while (relocation != NULL) {
        _patch_addr(class, num_pages, num_banks, bsr_boundary, section, relocation);
        relocation = relocation->next;
      }

      /* update the rom with the patched idata sections */
      if ((section->flags & STYP_DATA) && (section->num_reloc > 0)) {
        assert(section->next->flags & STYP_DATA_ROM);
        _copy_rom_section(object, section, section->next);
      }

      /* strip the relocations from the section */
      section->num_reloc        = 0;
      section->relocations      = NULL;
      section->relocations_tail = NULL;
    }

    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* copy all executable data to new memory */

MemBlock *
gp_cofflink_make_memory(gp_object_type *object)
{
  const gp_section_type *section;
  MemBlock              *m;
  unsigned int           addr;
  unsigned int           org;
  unsigned int           stop;
  const char            *section_name;
  const char            *symbol_name;
  uint8_t                byte;
  proc_class_t           class;
  pic_processor_t        processor;
  gp_boolean             not_8_bit;

  m         = i_memory_create();
  section   = object->sections;
  class     = object->class;
  processor = object->processor;

  if ((class == PROC_CLASS_PIC12)  || (class == PROC_CLASS_PIC12E) ||
      (class == PROC_CLASS_PIC12I) || (class == PROC_CLASS_SX)     ||
      (class == PROC_CLASS_PIC14)  || (class == PROC_CLASS_PIC14E) ||
      (class == PROC_CLASS_PIC14EX)) {
    not_8_bit = true;
  }
  else {
    not_8_bit = false;
  }

  while (section != NULL) {
    if (section->flags & (STYP_TEXT | STYP_DATA_ROM)) {
      addr = section->address;
      stop = addr + section->size;
      gp_debug("   make memory: section \"%s\" (0x%06X - 0x%06X)", section->name, addr, stop - 1);

      for ( ; addr < stop; addr++) {
        /* fetch the current contents of the memory */
        b_memory_assert_get(section->data, addr, &byte, &section_name, &symbol_name);

        if (not_8_bit) {
          org = gp_processor_byte_to_org(class, addr);

          if (gp_processor_is_idlocs_org(processor, org) >= 0) {
            if (addr & 1) {
              /* This is higher byte. */
              byte |= ((object->processor->idlocs_mask) >> 8) & 0xFF;
            } else {
              /* This is lower byte. */
              byte |= object->processor->idlocs_mask & 0xFF;
            }
          }
        }

        /* write data to new memory */
        b_memory_put(m, addr, byte, section_name, symbol_name);
      }
    }
    section = section->next;
  }

 return m;
}

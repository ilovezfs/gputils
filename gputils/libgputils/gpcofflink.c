/* GNU PIC coff linker functions
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

gp_boolean gp_relocate_to_shared = false;

/*------------------------------------------------------------------------------------------------*/

/* Two symbol tables are constructed. The first contains the definitions of all
   external symbols in all the object files.  This symbol table is used for
   relocation and linking.  The second table contains all external symbols
   that do not yet have a definition.  This table is used to determine which
   objects in a library are to be linked against.  This table should be empty
   at the begining of the relocation process. */

void
gp_cofflink_add_symbol(symbol_table_t *Table, gp_symbol_t *Symbol, gp_object_t *File)
{
  symbol_t        *sym;
  gp_coffsymbol_t *var;

  /* Search the for the symbol. If not found, then add it to the global symbol table. */
  sym = gp_sym_get_symbol(Table, Symbol->name);

  if (sym != NULL) {
    return;
  }

  sym = gp_sym_add_symbol(Table, Symbol->name);
  var = GP_Malloc(sizeof(*var));
  var->symbol = Symbol;
  var->file   = File;
  gp_sym_annotate_symbol(sym, var);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_cofflink_remove_symbol(symbol_table_t *Table, char *Name)
{
  symbol_t        *sym;
  gp_coffsymbol_t *var;

  sym = gp_sym_get_symbol(Table, Name);

  if (sym == NULL) {
    return;
  }

  var = gp_sym_get_symbol_annotation(sym);
  free(var);
  gp_sym_remove_symbol(Table, Name);
}

/*------------------------------------------------------------------------------------------------*/

/* Add the external symbols from an object file to the appropriate symbol
   tables. NOTE: The missing symbol table is optional. This feature is
   not used for generating symbol indexes for archives. */

gp_boolean
gp_cofflink_add_symbols(symbol_table_t *Definition, symbol_table_t *Missing, gp_object_t *Object)
{
  gp_symbol_t     *symbol;
  symbol_t        *sym;
  gp_coffsymbol_t *var;

  if ((Definition == NULL) || (Object == NULL)) {
    return false;
  }

  /* The gp_convert_file() function has read it the elements of the "object". */

  symbol = Object->symbol_list.first;

  while (symbol != NULL) {
    /* process all external symbols that are not directives */
    if ((symbol->class == C_EXT) && (symbol->name[0] != '.')) {
      if (symbol->section_number == N_UNDEF) {
        /* This symbol is defined elsewhere. Check for it in the symbol
           definitions. If it doesn't exist there, add it to the missing
           symbol table, if not already entered. */
        sym = gp_sym_get_symbol(Definition, symbol->name);

        if ((sym == NULL) && (Missing != NULL)) {
          gp_cofflink_add_symbol(Missing, symbol, Object);
        }
      }
      else {
        /* External symbol definition. See if it is already defined, it so
           it is an error. Add the symbol to the symbol definitions and remove
           it from the missing symbol table if it exists there. */
        sym = gp_sym_get_symbol(Definition, symbol->name);

        if (sym != NULL) {
          /* duplicate symbol */
          var = gp_sym_get_symbol_annotation(sym);
          gp_error("Duplicate symbol \"%s\" defined in \"%s\" and \"%s\".",
                   symbol->name, var->file->filename, Object->filename);
        }
        else {
          gp_cofflink_add_symbol(Definition, symbol, Object);
        }

        if (Missing != NULL) {
          gp_cofflink_remove_symbol(Missing, symbol->name);
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
gp_cofflink_combine_objects(gp_object_t *Object)
{
  gp_object_t *object_list;

  /* assign the time the operation occured */
  Object->time = (uint32_t)time(NULL);

  /* append the sections onto the list */
  object_list = Object->next;
  while (object_list != NULL) {
    gp_coffgen_transfer_object_data(Object, object_list);
    object_list = object_list->next;
  }

  gp_coffgen_update_all_object_id(Object);

  /* FIXME: breaking the chain isn't good */
  Object->next = NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Cleanup the symbol table after combining objects. */

void
gp_cofflink_clean_table(gp_object_t *Object, symbol_table_t *Symbols)
{
  gp_section_t    *section;
  gp_reloc_t      *relocation;
  gp_symbol_t     *symbol;
  gp_coffsymbol_t *var;
  const symbol_t  *sym;
  gp_symbol_t     *next;

  gp_debug("Cleaning symbol table.");

  /* point all relocations to the symbol definitions */
  section = Object->section_list.first;
  while (section != NULL) {
    relocation = section->relocation_list.first;
    while (relocation != NULL) {
      symbol = relocation->symbol;

      if ((symbol->class == C_EXT) && (symbol->section_number == N_UNDEF)) {
        /* This is an external symbol defined elsewhere. */
        sym = gp_sym_get_symbol(Symbols, symbol->name);
        assert(sym != NULL);
        var = gp_sym_get_symbol_annotation(sym);
        assert(var != NULL);
        symbol = var->symbol;
        assert(symbol != NULL);
        relocation->symbol = symbol;
      }

      relocation = relocation->next;
    }

    section = section->next;
  }

  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    next = symbol->next;
    if ((symbol->class == C_EXT) && (symbol->section_number == N_UNDEF)) {
      gp_debug("  removed symbol \"%s\"", symbol->name);
      gp_coffgen_del_symbol(Object, symbol);
    }
    symbol = next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Update the line number offsets. */

static void
_update_line_numbers(gp_linenum_t *Line_number, unsigned int Offset)
{
  while (Line_number != NULL) {
    Line_number->address += Offset;
    Line_number = Line_number->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Combine overlay sections in an object file. */

void
gp_cofflink_combine_overlay(gp_object_t *Object, gp_boolean Remove_symbol)
{
  int           addr_digits;
  gp_section_t *first;
  gp_section_t *second;
  gp_symbol_t  *symbol;

  addr_digits = Object->class->addr_digits;
  first       = Object->section_list.first;
  while (first != NULL) {
    if (first->flags & STYP_OVERLAY) {
      second = gp_coffgen_find_section(Object, first->next, first->name);

      if (second != NULL) {
        /* The sections must have the same properties or they can't be combined. */
        if (first->flags != second->flags) {
          gp_error("Section types for \"%s\" do not match.", first->name);
          continue;
        }
        else if ((first->flags & STYP_ABS) && (first->address != second->address)) {
          gp_error("Different addresses for absolute overlay sections \"%s\" (0x%0*X != 0x%0*X).",
                   first->name, addr_digits, first->address, addr_digits, second->address);
          continue;
        }

        /* Set the size of the first section to the larger of the two. */
        if (second->size > first->size) {
          first->size = second->size;
          first->symbol->aux_list.first->_aux_symbol._aux_scn.length = second->size;
        }

        /* Remove the section symbol. */
        if (Remove_symbol) {
          gp_coffgen_del_symbol(Object, second->symbol);
        }

        /* Update the symbol table */
        symbol = Object->symbol_list.first;
        while (symbol != NULL) {
          if (symbol->section == second) {
            symbol->section = first;
          }
          symbol = symbol->next;
        }

        /* Remove the second section. */
        gp_coffgen_del_section(Object, second);

        /* Take another pass. */
        gp_cofflink_combine_overlay(Object, Remove_symbol);
        return;
      }
    }
    first = first->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Allocate memory for a stack. */

void
gp_cofflink_make_stack(gp_object_t *Object, unsigned int Num_bytes)
{
  gp_section_t *new;
  gp_symbol_t  *symbol;
  int           i;

  new = gp_coffgen_add_section(Object, ".stack", NULL);
  new->flags = STYP_BSS;
  new->size  = Num_bytes;

  gp_debug("Allocating stack memory of size %#x (%i).", Num_bytes, Num_bytes);

  /* mark the memory locations as used */
  for (i = 0; i < Num_bytes; i++) {
    gp_mem_b_put(new->data, i, 0, ".stack", NULL);
  }

  /* create the symbol for the start address of the stack */
  symbol = gp_coffgen_find_symbol(Object, "_stack");
  if ((symbol != NULL) && (symbol->section_number > N_UNDEF)) {
    gp_error("_stack symbol already exists");
  }
  else {
    symbol = gp_coffgen_add_symbol(Object, "_stack");
//    symbol->value          = 0;
    symbol->section_number = N_SCNUM;
    symbol->section        = new;
//    symbol->type           = T_NULL;
    symbol->class          = C_EXT;
  }

  /* create the symbol for the end of the stack */
  symbol = gp_coffgen_find_symbol(Object, "_stack_end");
  if ((symbol != NULL) && (symbol->section_number > N_UNDEF)) {
    gp_error("_stack_end symbol already exists.");
  }
  else {
    symbol = gp_coffgen_add_symbol(Object, "_stack_end");
    symbol->value          = Num_bytes - 1;
    symbol->section_number = N_SCNUM;
    symbol->section        = new;
//    symbol->type           = T_NULL;
    symbol->class          = C_EXT;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Merge all sections in one object file with the same name. The overlayed
   sections must have been combined first.  */

void
gp_cofflink_merge_sections(gp_object_t *Object)
{
  int           addr_digits;
  gp_section_t *first;
  gp_section_t *second;
  gp_symbol_t  *symbol;
  gp_reloc_t   *relocation;
  unsigned int  section_org;
  uint8_t       data;
  const char   *section_name;
  const char   *symbol_name;
  unsigned int  last;
  unsigned int  offset;
  unsigned int  byte_addr;

  addr_digits = Object->class->addr_digits;
  first       = Object->section_list.first;
  while (first != NULL) {
    second = gp_coffgen_find_section(Object, first->next, first->name);

    if (second != NULL) {
      /* The sections must have the same properties or they can't be combined. */
      if ((first->flags & STYP_ABS) ||
          (second->flags & STYP_ABS) ||
          (strcmp(first->name, ".config") == 0) ||
          (strcmp(first->name, ".idlocs") == 0)) {
        gp_error("File \"%s\", section \"%s\" (0x%0*X) is absolute but occurs in more than one file.",
                 Object->filename, first->name, addr_digits, first->address);
        exit(1);
      }

      gp_debug("  merging section \"%s\" with section \"%s\"", first->name, second->name);

      /* Update the addresses in the relocation table. */
      relocation = second->relocation_list.first;
      while (relocation != NULL) {
        relocation->address += first->size;
        relocation = relocation->next;
      }

      /* Copy the section data. */
      if (gp_writeobj_has_data(second)) {
        last   = second->size;
        offset = first->size;

        if (!gp_writeobj_has_data(first)) {
          /* TODO optimization: adopt data from second by moving second->size bytes from org to org + offset */
          first->data = gp_mem_i_create();
        }

        for (byte_addr = 0; byte_addr < last; byte_addr++) {
          if (gp_mem_b_get(second->data, byte_addr, &data, &section_name, &symbol_name)) {
            gp_mem_b_put(first->data, byte_addr + offset, data, section_name, symbol_name);
          }
          else {
            assert(0);
          }
        }
      }

      /* Update the line number offsets. */
      _update_line_numbers(second->line_number_list.first, first->size);

      if (first->flags & STYP_ROM_AREA) {
        section_org = gp_processor_byte_to_org(Object->class, first->size);
      }
      else {
        section_org = first->size;
      }

      /* Update the symbol table. */
      symbol = Object->symbol_list.first;
      while (symbol != NULL) {
        if ((symbol->section_number > N_UNDEF) && (symbol->section == second)) {
          symbol->section  = first;
          symbol->value   += section_org;
        }
        symbol = symbol->next;
      }

      /* Add section sizes. */
      first->size += second->size;

      /* Append the relocations from the second section to the first. */
      /* Append the line numbers from the second section to the first. */
      gp_coffgen_transfer_section_data(first, second);
      gp_coffgen_update_all_section_id(first);

      /* Remove the second section. */
      gp_coffgen_del_section(Object, second);

      /* Take another pass. */
      gp_cofflink_merge_sections(Object);
      return;
    }
    first = first->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* copy data from idata section to the ROM section */

static void
_copy_rom_section(const gp_object_t *Object, const gp_section_t *Idata, gp_section_t *Rom)
{
  proc_class_t  class;
  int           from;
  int           stop;
  int           to;
  const char   *section_name;
  const char   *symbol_name;
  uint8_t       data;
  uint16_t      insn_retlw;

  class = Object->class;
  from  = Idata->address;
  stop  = from + Idata->size;
  to    = Rom->address;

  if (class->rom_width == 8) {
    /* PIC16E */
    for ( ; from < stop; ++from, ++to) {
      if (gp_mem_b_get(Idata->data, from, &data, &section_name, &symbol_name)) {
        gp_mem_b_put(Rom->data, to, data, section_name, symbol_name);
      }
    }
  }
  else {
    /* PIC12, PIC12E, PIC12I, SX, PIC14, PIC14E, PIC14EX, PIC16 */
    /* select "retlw" instruction */
    insn_retlw = gp_processor_retlw(class);

    for ( ; from < stop; ++from, to += 2) {
      if (gp_mem_b_get(Idata->data, from, &data, &section_name, &symbol_name)) {
        class->i_memory_put(Rom->data, to, insn_retlw | data, section_name, symbol_name);
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* create the new section name */

static char *
_create_i_section_name(const char *Name)
{
  size_t  len = strlen(Name);
  char   *name_i = GP_Malloc(len + 3);

  memcpy(name_i, Name, len);
  memcpy(name_i + len, "_i", 3);
  return name_i;
}

/*------------------------------------------------------------------------------------------------*/

/* create a program memory section to hold the data */

static void
_create_rom_section(gp_object_t *Object, gp_section_t *Section)
{
  gp_section_t *new;
  char         *name;

  /* create the new section */
  name = _create_i_section_name(Section->name);
  new  = gp_coffgen_new_section(name, NULL);
  free(name);

  if (Object->class->rom_width == 8) {
    /* PIC16E */
    new->size = Section->size;
    /* Force the section size to be an even number of bytes. */
    if (Section->size & 1) {
      gp_mem_b_put(new->data, Section->size, 0, Object->filename, "adjust");
      new->size++;
    }
  }
  else {
    /* PIC12, PIC12E, PIC12I, SX, PIC14, PIC14E, PIC14EX, PIC16 */
    new->size = Section->size << 1;
  }
  new->flags = STYP_DATA_ROM;

  /* Copy the data to get the MEM_USED_MASK correct. It is copied again later
     to ensure that any patched data is updated in the ROM section. */
  _copy_rom_section(Object, Section, new);

  /* Insert the new ROM section after the idata section. */
  gp_coffgen_insert_after_section(Object, Section, new);
}

/*------------------------------------------------------------------------------------------------*/

/* write a word (16 bit) into four bytes of memory (non PIC16E) */

static void
_write_table_u16(proc_class_t Class, const gp_section_t *Section, unsigned int Byte_address,
                 uint16_t Insn, uint16_t Data, const char *Symbol_name)
{
  Class->i_memory_put(Section->data, Byte_address,     Insn | (Data & 0xff), Section->name, Symbol_name);
  Class->i_memory_put(Section->data, Byte_address + 2, Insn | (Data >> 8),   Section->name, Symbol_name);
}

/*------------------------------------------------------------------------------------------------*/

/* write a long (32 bit) into four bytes of memory (PIC16E) */

static void
_write_table_u32(const proc_class_t Class, const gp_section_t *Section, unsigned int Byte_address,
                 uint32_t Data, const char *Symbol_name)
{
  Class->i_memory_put(Section->data, Byte_address,     Data & 0xffff, Section->name, Symbol_name);
  Class->i_memory_put(Section->data, Byte_address + 2, Data >> 16,    Section->name, Symbol_name);
}

/*------------------------------------------------------------------------------------------------*/

/* read a word from four bytes of memory (non PIC16E) */

static uint16_t
_read_table_u16(proc_class_t Class, const gp_section_t *Section, unsigned int Byte_address)
{
  uint16_t data[2];

  Class->i_memory_get(Section->data, Byte_address,     data,     NULL, NULL);
  Class->i_memory_get(Section->data, Byte_address + 2, data + 1, NULL, NULL);

  return ((data[0] & 0xff) | ((data[1] & 0xff) << 8));
}

/*------------------------------------------------------------------------------------------------*/

/* create the symbol for the start address of the table */

void
gp_cofflink_make_cinit(gp_object_t *Object)
{
  gp_symbol_t *symbol;

  /* create the symbol for the start address of the table */
  /* TODO MPLINK 4.34 does not create this. We must implement the
     section address relocations RELOCT_SCN*. */
  symbol = gp_coffgen_find_symbol(Object, "_cinit");

  if ((symbol != NULL) && (symbol->section_number > N_UNDEF)) {
    gp_error("_cinit symbol already exists.");
  }
  else {
    symbol = gp_coffgen_add_symbol(Object, "_cinit");
//    symbol->value          = 0;
    symbol->section_number = N_SCNUM;
//    symbol->type           = T_NULL;
    symbol->class          = C_EXT;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* create ROM data for initialized data sections */

void
gp_cofflink_make_idata(gp_object_t *Object, gp_boolean Force_cinit)
{
  gp_section_t *section;
  proc_class_t  class;
  gp_section_t *new;
  int           count_sections;
  int           byte_count;
  int           i;
  int           insn_retlw;
  gp_symbol_t  *symbol;

  count_sections = 0;
  section        = Object->section_list.first;
  while (section != NULL) {
    if (section->flags & STYP_DATA) {
      _create_rom_section(Object, section);
      count_sections++;
    }
    section = section->next;
  }

  if ((count_sections > 0) || Force_cinit) {
    class = Object->class;
    new   = gp_coffgen_add_section(Object, ".cinit", NULL);
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
      gp_mem_b_put(new->data, i, 0, ".cinit", "table");
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
    symbol = gp_coffgen_find_symbol(Object, "_cinit");

    if (!Force_cinit) {
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
gp_cofflink_add_cinit_section(gp_object_t *Object)
{
  gp_section_t       *section;
  proc_class_t        class;
  const gp_section_t *new;
  const gp_section_t *prog_section;
  int                 insn_retlw;
  int                 count_sections;
  int                 base_address;
  char               *prog_name;
  uint16_t            number;

  new = gp_coffgen_find_section(Object, Object->section_list.first, ".cinit");

  if (new != NULL) {
    /* scan through the sections to determine the addresses */
    count_sections = 0;
    base_address   = new->address;
    class          = Object->class;

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

    section = Object->section_list.first;
    while (section != NULL) {
      if (section->flags & STYP_DATA) {
        /* locate the rom table */
        prog_name    = _create_i_section_name(section->name);
        prog_section = gp_coffgen_find_section(Object, Object->section_list.first, prog_name);
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
_set_used(const gp_object_t *Object, MemBlock_t *M, unsigned int Org_to_byte_shift,
          unsigned int Byte_address, unsigned int Size, const char *Type, const char *Section_name,
          gp_boolean P16e_align_needed)
{
  uint8_t            data;
  const gp_symbol_t *symbol;
  const char        *symbol_name;
  const char        *old_section_name;
  const char        *old_symbol_name;
  int                addr_digits;

  if (P16e_align_needed && (Size & 1)) {
    /* code_pack --> STYP_BPACK */
    gp_debug("    align to even size: %u ==> %u", Size, Size + 1);
    ++Size;
  }

  addr_digits = Object->class->addr_digits;
  gp_debug("      marking %#x (%u) words from 0x%0*X to 0x%0*X as used", Size, Size,
           addr_digits, gp_byte_to_org(Org_to_byte_shift, Byte_address),
           addr_digits, gp_byte_to_org(Org_to_byte_shift, Byte_address + Size - 1));

  for ( ; Size > 0; Byte_address++, Size--) {
    if (gp_mem_b_get(M, Byte_address, &data, &old_section_name, &old_symbol_name)) {
      if ((old_section_name != NULL) && (Section_name != NULL)) {
        symbol      = gp_symbol_find(Object, Section_name, Byte_address);
        symbol_name = (symbol != NULL) ? symbol->name : NULL;
        gp_error("More %s sections use same address: 0x%0*X -- \"%s/%s\", \"%s/%s\"", Type,
                 addr_digits, gp_byte_to_org(Org_to_byte_shift, Byte_address),
                 old_section_name, old_symbol_name, Section_name, symbol_name);
      }
      else {
        gp_error("More %s sections use same address: 0x%0*X", Type,
                 addr_digits, gp_byte_to_org(Org_to_byte_shift, Byte_address));
      }
      return;
    }
    else {
      symbol      = gp_symbol_find(Object, Section_name, Byte_address);
      symbol_name = (symbol != NULL) ? symbol->name : NULL;
      gp_mem_b_put(M, Byte_address, 0, Section_name, symbol_name);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* allocate space for the absolute sections */

void
gp_cofflink_reloc_abs(gp_object_t *Object, MemBlock_t *M, unsigned int Org_to_byte_shift,
                      uint32_t Flags)
{
  gp_section_t *section;
  unsigned int  org;
  gp_boolean    p16e_align_needed;

  section = Object->section_list.first;
  while (section != NULL) {
    if ((section->flags & STYP_ABS) && (section->flags & Flags)) {
      /* Workaround for the "odd size memory problem" in the PIC16E class.
         code_pack --> STYP_BPACK */
      p16e_align_needed = false;

      if ((Object->class == PROC_CLASS_PIC16E) &&
          (section->flags & STYP_ROM_AREA) &&
          (section->size & 1)) {
        org = gp_processor_byte_to_real(Object->processor, section->address);

        if ((gp_processor_is_idlocs_org(Object->processor, org) < 0) &&
            (gp_processor_is_config_org(Object->processor, org) < 0) &&
            (gp_processor_is_eeprom_org(Object->processor, org) < 0)) {
          p16e_align_needed = true;
        }
      }

      _set_used(Object, M, Org_to_byte_shift, section->address, section->size, "absolute",
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

static gp_section_t *
_find_big_assigned(gp_section_t *Section, uint32_t Flags, symbol_table_t *Logical_sections)
{
  gp_section_t   *biggest;
  const symbol_t *sym;

  biggest = NULL;
  while (Section != NULL) {
    sym = gp_sym_get_symbol(Logical_sections, Section->name);

    if ((sym != NULL) && (Section->flags & Flags) && !(Section->flags & STYP_RELOC)) {
      /* This section has not been relocated. */
      if ((biggest == NULL) || (Section->size > biggest->size)) {
        biggest = Section;
      }
    }
    Section = Section->next;
  }

  return biggest;
}

/*------------------------------------------------------------------------------------------------*/

/* Search through all the sections in the object list.  Locate the biggest
   section that has not been relocated. */

static gp_section_t *
_find_big_section(gp_section_t *Section, uint32_t Flags)
{
  gp_section_t *biggest;

  biggest = NULL;
  while (Section != NULL) {
    if ((Section->flags & Flags) && !(Section->flags & STYP_RELOC)) {
      /* This section has not been relocated. */
      if ((biggest == NULL) || (Section->size > biggest->size)) {
        biggest = Section;
      }
    }
    Section = Section->next;
  }

  if (biggest != NULL) {
    gp_debug("  biggest section = %s, section flags = %#x, Flags = %#x",
             biggest->name, biggest->flags, Flags);
  }

  return biggest;
}

/*------------------------------------------------------------------------------------------------*/

/* Search through the target memory. Locate the smallest block of memory
   that is larger than the requested size. Return the address of that
   block. */

static gp_boolean
_search_memory(const MemBlock_t *M, unsigned int Org_to_byte_shift, unsigned int Start,
               unsigned int Stop, unsigned int Size, unsigned int *Block_address,
               unsigned int *Block_size, gp_boolean Stop_at_first)
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
  *Block_size = (unsigned int)(-1);

  for (address = Start; address <= Stop; address++) {
    mem_used = gp_mem_b_get(M, address, &byte, NULL, NULL);

    if (address == Stop) {
      if (in_block) {
        /* end of the section definition */
        end_block = true;
        /* increment for last address */
        current_size++;
      }
      else if (Start == Stop) {
        /* special case, one word section */
        if (! mem_used) {
          end_block = true;
          current_address = Start;
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
        gp_debug("    start unused block at %#x", gp_byte_to_org(Org_to_byte_shift, address));
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
               gp_byte_to_org(Org_to_byte_shift, address),
               gp_byte_to_org(Org_to_byte_shift, current_size));
      if (current_size >= Size) {
        if (Stop_at_first) {
          *Block_size    = current_size;
          *Block_address = current_address;
          success = true;
          break;
        }
        else if (current_size < *Block_size) {
          *Block_size    = current_size;
          *Block_address = current_address;
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
_move_data(MemBlock_t *M, unsigned int Byte_address, unsigned int Size, unsigned int New_address)
{
  int         org;
  uint8_t     data;
  const char *section_name;
  const char *symbol_name;

  if (Byte_address == New_address) {
    return;
  }

  gp_debug("    moving %#x (%u) bytes from %#x to %#x", Size, Size, Byte_address, New_address);

  for (org = Byte_address + Size - 1; org >= 0; org--) {
    gp_mem_b_assert_get(M, org, &data, &section_name, &symbol_name);
    gp_debug("      moving byte %#x from %#x to %#x", data, org, New_address + org);
    gp_mem_b_put(M, New_address + org, data, section_name, symbol_name);
    gp_mem_b_clear(M, org);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* map real addres to shadow address */

static unsigned int
_map_to_shadow_address(const linker_section_t *Section_def, unsigned int Address)
{
  unsigned int new_address;

  if (Section_def->shadow_sym != NULL) {
    new_address = Address + Section_def->shadow_val - Section_def->start;
    gp_debug("      mapping shadow address %#x => %#x", Address, new_address);
    return new_address;
  }
  else {
    return Address;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* unmap real addres from shadow address */

static unsigned int
_unmap_from_shadow_address(const linker_section_t *Section_def, unsigned int Address)
{
  unsigned int new_address;

  if (Section_def->shadow_sym != NULL) {
    new_address = Address + Section_def->start - Section_def->shadow_val;
    gp_debug("      unmapping shadow address %#x => %#x", Address, new_address);
    return new_address;
  }
  else {
    return Address;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Compare function for gp_sym_clone_symbol_array(). */

static int
_sect_addr_cmp(const void *P0, const void *P1)
{
  const symbol_t         *sym0 = *(const symbol_t **)P0;
  const symbol_t         *sym1 = *(const symbol_t **)P1;
  const linker_section_t *sect0;
  const linker_section_t *sect1;

  sect0 = gp_sym_get_symbol_annotation(sym0);
  sect1 = gp_sym_get_symbol_annotation(sym1);

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
gp_cofflink_reloc_assigned(gp_object_t *Object, MemBlock_t *M, unsigned int Org_to_byte_shift,
                           uint32_t Flags, symbol_table_t *Sections, symbol_table_t *Logical_sections)
{
  gp_section_t     *section;
  gp_section_t     *current;
  const symbol_t   *sym;
  char             *section_name;
  linker_section_t *section_def;
  unsigned int      current_shadow_address;
  unsigned int      current_size;
  unsigned int      org;
  gp_boolean        p16e_align_needed;

  section = Object->section_list.first;
  while (true) {
    current = _find_big_assigned(section, Flags, Logical_sections);

    if (current == NULL) {
      break;
    }

    /* Fetch the logical section. */
    sym = gp_sym_get_symbol(Logical_sections, current->name);
    assert(sym != NULL);

    /* Fetch the section definition. */
    section_name = (char *)gp_sym_get_symbol_annotation(sym);
    sym          = gp_sym_get_symbol(Sections, section_name);
    assert(sym != NULL);

    section_def = (linker_section_t *)gp_sym_get_symbol_annotation(sym);
    assert(section_def != NULL);

    p16e_align_needed = false;

    /* Workaround for the "odd size memory problem" in the PIC16E class.
       code_pack --> STYP_BPACK */
    if ((Object->class == PROC_CLASS_PIC16E) &&
        (current->flags & Flags) &&
        (current->flags & STYP_ABS) &&
        (current->flags & STYP_ROM_AREA) &&
        (current->size & 1)) {
      org = gp_processor_byte_to_real(Object->processor, current->address);

      if ((gp_processor_is_idlocs_org(Object->processor, org) < 0) &&
          (gp_processor_is_config_org(Object->processor, org) < 0) &&
          (gp_processor_is_eeprom_org(Object->processor, org) < 0)) {
        p16e_align_needed = true;
      }
    }

    /* assign the address to this section */
    if (_search_memory(M, Org_to_byte_shift,
                       _map_to_shadow_address(section_def, section_def->start),
                       _map_to_shadow_address(section_def, section_def->end),
                       (p16e_align_needed) ? (current->size + 1) : current->size,
                       &current_shadow_address, &current_size, false) == 1) {
      gp_debug("    logical section: '%s'", current->name);
      gp_debug("    section name   : '%s'", section_name);
      gp_debug("    successful relocation to %#x", gp_byte_to_org(Org_to_byte_shift, current_shadow_address));

      if (gp_writeobj_has_data(current)) {
        _move_data(current->data, current->shadow_address, current->size, current_shadow_address);
      }

      current->shadow_address = current_shadow_address;
      current->address        = _unmap_from_shadow_address(section_def, current_shadow_address);
      _set_used(Object, M, 0, current_shadow_address, current->size, "relocatable assigned",
                section_name, p16e_align_needed);

      /* Update the line number offsets. */
      _update_line_numbers(current->line_number_list.first, current->address);

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
gp_cofflink_reloc_cinit(gp_object_t *Object, MemBlock_t *M, unsigned int Org_to_byte_shift,
                        gp_section_t *Cinit_section, const symbol_table_t *Sections)
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

  if ((Cinit_section == NULL) || (Cinit_section->flags & STYP_RELOC)) {
    return;
  }

  size = Cinit_section->size;

  gp_debug("  Relocating cinit code.");

  success                 = false;
  type_avail              = false;
  smallest_shadow_address = (unsigned int)(-1);
  smallest_address        = (unsigned int)(-1);
  sym_list                = gp_sym_clone_symbol_array(Sections, _sect_addr_cmp);

  /* Search the section definitions for the smallest block of memory that the section will fit in. */

  sym_count = gp_sym_get_symbol_count(Sections);
  for (i = 0; i < sym_count; ++i) {
    sym         = sym_list[i];
    section_def = (linker_section_t *)gp_sym_get_symbol_annotation(sym);

    if ((section_def->type == SECT_CODEPAGE) && !section_def->protected) {
      gp_debug("  section     = '%s'",     Cinit_section->name);
      gp_debug("    name      = '%s'",     gp_sym_get_symbol_name(sym));
      gp_debug("    size      = %#x (%u)", Cinit_section->size, Cinit_section->size);
      gp_debug("    def start = %#x",      section_def->start);
      gp_debug("    def end   = %#x",      section_def->end);

      if (section_def->shadow_sym != NULL) {
        gp_debug("    def shadow_sym = %s",  section_def->shadow_sym);
        gp_debug("    def shadow_val = %#x", section_def->shadow_val);
      }

      type_avail = true;

      if (_search_memory(M, Org_to_byte_shift,
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
    gp_debug("    successful relocation to %#x", gp_byte_to_org(Org_to_byte_shift, smallest_shadow_address));

    if (gp_writeobj_has_data(Cinit_section)) {
      _move_data(Cinit_section->data, Cinit_section->shadow_address, size, smallest_shadow_address);
    }
    Cinit_section->shadow_address = smallest_shadow_address;
    Cinit_section->address        = smallest_address;
    _set_used(Object, M, 0, smallest_shadow_address, size, "cinit", Cinit_section->name, false);

    /* Update the line number offsets. */
    _update_line_numbers(Cinit_section->line_number_list.first, Cinit_section->address);

    /* Set the relocated flag. */
    Cinit_section->flags |= STYP_RELOC;
  }
  else if (!type_avail) {
    gp_error("Linker script has no definition that matches the type of section \"%s\".",
             Cinit_section->name);
  }
  else {
    gp_error("No target memory available for section \"%s\".", Cinit_section->name);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* allocate memory for relocatable unassigned sections */

void
gp_cofflink_reloc_unassigned(gp_object_t *Object, MemBlock_t *M, unsigned int Org_to_byte_shift,
                             uint32_t Flags, const symbol_table_t *Sections)
{
  gp_section_t      *section;
  gp_section_t      *current;
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
  unsigned int       org;
  gp_boolean         p16e_align_needed;

  section = Object->section_list.first;
  while (true) {
    current = _find_big_section(section, Flags);

    if (current == NULL) {
      break;
    }

    size = current->size;
    p16e_align_needed = false;

    /* determine what type of sections are being relocated */
    if (current->flags & STYP_ROM_AREA) {
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
    if ((Object->class == PROC_CLASS_PIC16E) && (type == SECT_CODEPAGE) && (size & 1)) {
      org = gp_processor_byte_to_real(Object->processor, current->address);

      if ((gp_processor_is_idlocs_org(Object->processor, org) < 0) &&
          (gp_processor_is_config_org(Object->processor, org) < 0) &&
          (gp_processor_is_eeprom_org(Object->processor, org) < 0)) {
        p16e_align_needed = true;
      }
    }


next_pass:

    success                 = false;
    type_avail              = false;
    smallest_shadow_address = 0;
    smallest_address        = 0;
    smallest_size           = (unsigned int)(-1);
    sym_list                = gp_sym_clone_symbol_array(Sections, _sect_addr_cmp);

    /* search the section definitions for the smallest block of memory that
       the section will fit in */
    sym_count = gp_sym_get_symbol_count(Sections);
    for (i = 0; i < sym_count; ++i) {
      sym         = sym_list[i];
      section_def = gp_sym_get_symbol_annotation(sym);

      if ((section_def->type == type) && (!section_def->protected)) {
        gp_debug("  section     = '%s'",     current->name);
        gp_debug("    name      = %s",       gp_sym_get_symbol_name(sym));
        gp_debug("    size      = %#x (%u)", size, size);
        gp_debug("    def start = %#x",      section_def->start);
        gp_debug("    def end   = %#x",      section_def->end);

        if (section_def->shadow_sym != NULL) {
          gp_debug("    def shadow_sym = %s",  section_def->shadow_sym);
          gp_debug("    def shadow_val = %#x", section_def->shadow_val);
        }

        type_avail = true;

        if (_search_memory(M, Org_to_byte_shift,
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
      gp_debug("    successful relocation to %#x", gp_byte_to_org(Org_to_byte_shift, smallest_shadow_address));

      if (gp_writeobj_has_data(current)) {
        _move_data(current->data, current->shadow_address, size, smallest_shadow_address);
      }

      current->shadow_address = smallest_shadow_address;
      current->address        = smallest_address;
      _set_used(Object, M, 0, smallest_shadow_address, size, msg, current->name, p16e_align_needed);

      /* Update the line number offsets */
      _update_line_numbers(current->line_number_list.first, current->address);

      /* Set the relocated flag */
      current->flags |= STYP_RELOC;
    }
    else if (gp_relocate_to_shared && first_time && (type == SECT_DATABANK)) {
      first_time = false;
      type       = SECT_SHAREBANK;
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
gp_cofflink_update_table(gp_object_t *Object, unsigned int Org_to_byte_shift)
{
  gp_symbol_t  *symbol;
  gp_section_t *section;
  gp_section_t *sym_sect;
  unsigned int  offset;

  gp_debug("Updating symbols with their new relocated values.");

  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    if (symbol->section_number > N_UNDEF) {
      sym_sect = symbol->section;
      assert(sym_sect != NULL);

      if (!(sym_sect->flags & STYP_ABS)) {
        offset = sym_sect->address;

        if (sym_sect->flags & STYP_ROM_AREA) {
          offset = gp_byte_to_org(Org_to_byte_shift, offset);
        }

        symbol->value += offset;
      }
    }

    symbol = symbol->next;
  }

  gp_debug("Stripping section relocated flag.");

  section = Object->section_list.first;
  while (section != NULL) {
    section->flags &= ~(STYP_RELOC);
    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Create sections to fill unused memory in the pages with constant data. */

void
gp_cofflink_fill_pages(gp_object_t *Object, MemBlock_t *M, const symbol_table_t *Sections)
{
  linker_section_t  *section_def;
  size_t             i;
  const symbol_t   **sym_list;
  const symbol_t    *sym;
  size_t             sym_count;
  gp_boolean         found;
  char               fill_name[BUFSIZ];
  unsigned int       fill_number;
  gp_section_t      *section;
  unsigned int       current_shadow_address;
  unsigned int       current_size;
  unsigned int       org;
  unsigned int       end;

  gp_debug("Adding fill sections.");
  sym_list = gp_sym_clone_symbol_array(Sections, _sect_addr_cmp);

  /* search for any section definitions that have a fill */
  sym_count   = gp_sym_get_symbol_count(Sections);
  fill_number = 1;
  for (i = 0; i < sym_count; ++i) {
    sym         = sym_list[i];
    section_def = (linker_section_t *)gp_sym_get_symbol_annotation(sym);

    if ((section_def->type == SECT_CODEPAGE) && (section_def->use_fill)) {
      while (true) {
        found = _search_memory(M, Object->class->org_to_byte_shift,
                               _map_to_shadow_address(section_def, section_def->start),
                               _map_to_shadow_address(section_def, section_def->end),
                               1, &current_shadow_address, &current_size, false);
        if (found) {
          snprintf(fill_name, sizeof(fill_name), ".fill_%u", fill_number++);
          gp_debug("  new section \"%s\" at %#x with size %#x and data %#x",
                   fill_name, current_shadow_address, current_size, section_def->fill);
          section = gp_coffgen_find_section(Object, Object->section_list.first, fill_name);

          if (section != NULL) {
            gp_error("Fill section \"%s\" already exists.", fill_name);
            free(sym_list);
            return;
          }
          else {
            /* create a new section for the fill data */
            section                 = gp_coffgen_add_section(Object, fill_name, NULL);
            section->address        = _unmap_from_shadow_address(section_def, current_shadow_address);
            section->shadow_address = current_shadow_address;
            section->size           = current_size;
            section->flags          = STYP_TEXT;
            /* FIXME: do we really need a section symbol? */

            /* mark the memory as used */
            _set_used(Object, M, Object->class->org_to_byte_shift,
                      current_shadow_address, current_size, "fill", section->name, false);

            /* fill the section memory */
            org = current_shadow_address;
            end = org + current_size;
            for ( ; org < end; org += 2) {
              Object->class->i_memory_put(section->data, org, section_def->fill, section->name, NULL);
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
_check_relative(const gp_section_t *Section, unsigned int Org, int Argument, int Range)
{
  /* If the branch is too far then issue a warning */
  if ((Argument > Range) || (Argument < -(Range + 1))) {
    gp_warning("Relative branch out of range in at %#x of section \"%s\".", Org, Section->name);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* patch one word with the relocated address */

static void
_patch_addr(gp_object_t *Object, gp_section_t *Section, const gp_reloc_t *Relocation)
{
  proc_class_t       class;
  int                num_pages;
  int                num_banks;
  const gp_symbol_t *symbol;
  int                byte_addr;
  int                value;
  uint16_t           current_value;
  int                data;
  int                offset;
  int                bank;
  int                page;
  gp_boolean         write_data;

  class     = Object->class;
  num_pages = gp_processor_num_pages(Object->processor);
  num_banks = gp_processor_num_banks(Object->processor);

  symbol    = Relocation->symbol;
  byte_addr = Relocation->address + Section->address;
  value     = symbol->value       + Relocation->offset;

  gp_debug("  patching at %#x from %s/%s with %#x", byte_addr, Section->name, symbol->name, value);

  /* fetch the current contents of the memory */
  class->i_memory_get(Section->data, byte_addr, &current_value, NULL, NULL);

  /* FIXME: Not sure if warnings should be generated for out of range arguments.
            The linker should make sure values are within ranges in the linker scripts. */

  data       = 0;
  write_data = true;
  switch (Relocation->type) {
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
      data = class->reloc_high((symbol->section->flags & STYP_ROM_AREA), value);
      break;

    case RELOCT_UPPER:
      data = (value >> 16) & 0xff;
      break;

    case RELOCT_P:
      data = (value & 0x1f) << 8;
      break;

    case RELOCT_BANKSEL:
      bank = gp_processor_check_bank(class, value);
      gp_processor_set_bank(class, num_banks, bank, Section->data, byte_addr);
      write_data = false;
      break;

    case RELOCT_IBANKSEL:
      bank = gp_processor_check_ibank(class, value);
      gp_processor_set_ibank(class, num_banks, bank, Section->data, byte_addr);
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
      data = class->reloc_bra(Section, value, byte_addr);
      break;

    case RELOCT_CONDBRA: {
      /* This is only used for PIC16E (pic18). */
      offset = value - byte_addr - 2;

      if (offset & 1) {
        if (symbol->name != NULL) {
          gp_warning("Destination address must be word aligned at %#x of section \"%s\" at symbol: \"%s\".",
                     byte_addr, Section->name, symbol->name);
        }
        else {
          gp_warning("Destination address must be word aligned at %#x of section \"%s\".",
                     byte_addr, Section->name);
        }
      }

      offset >>= 1;
      _check_relative(Section, byte_addr, offset, 0x7f);
      data = offset & 0xff;
      break;
    }

    case RELOCT_ACCESS:
      data = (gp_processor_is_p16e_access(Object->processor, value, false)) ? 0 : 0x100;
      break;

    case RELOCT_PAGESEL_WREG:
      page = gp_processor_check_page(class, value);
      gp_processor_set_page(class, num_pages, page, Section->data, byte_addr, true);
      write_data = false;
      break;

    case RELOCT_PAGESEL_BITS:
    case RELOCT_PAGESEL_MOVLP:
      page = gp_processor_check_page(class, value);
      gp_processor_set_page(class, num_pages, page, Section->data, byte_addr, false);
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
                 gp_coffgen_reloc_type_to_str(Relocation->type),
                 Relocation->type, Section->name, symbol->name);
      }
      else {
        gp_error("Unimplemented relocation = %s (%u) in section \"%s\".",
                 gp_coffgen_reloc_type_to_str(Relocation->type),
                 Relocation->type, Section->name);
      }
      assert(0);
    }
  }

  /* update memory with the new value */
  if (write_data) {
    class->i_memory_put(Section->data, byte_addr, current_value | data, Section->name, symbol->name);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Patch all addresses with the relocated symbols. The relocations are
   stripped from the sections. */

void
gp_cofflink_patch(gp_object_t *Object)
{
  gp_section_t     *section;
  const gp_reloc_t *relocation;

  gp_debug("Patching data with relocated symbols.");

  section = Object->section_list.first;
  while (section != NULL) {
    if (gp_writeobj_has_data(section)) {
      /* patch raw data with relocation entries */
      relocation = section->relocation_list.first;
      while (relocation != NULL) {
        _patch_addr(Object, section, relocation);
        relocation = relocation->next;
      }

      /* update the rom with the patched idata sections */
      if ((section->flags & STYP_DATA) && (section->relocation_list.first != NULL)) {
        assert(section->next->flags & STYP_DATA_ROM);
        _copy_rom_section(Object, section, section->next);
      }

      /* strip the relocations from the section */
      gp_list_delete(&section->relocation_list);
    }

    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* copy all executable data to new memory */

MemBlock_t *
gp_cofflink_make_memory(gp_object_t *Object)
{
  MemBlock_t         *m;
  const gp_section_t *section;
  proc_class_t        class;
  pic_processor_t     processor;
  int                 addr_digits;
  unsigned int        addr;
  unsigned int        org;
  unsigned int        stop;
  const char         *section_name;
  const char         *symbol_name;
  uint8_t             byte;
  gp_boolean          not_8_bit;

  m           = gp_mem_i_create();
  section     = Object->section_list.first;
  class       = Object->class;
  processor   = Object->processor;
  addr_digits = class->addr_digits;

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
    if (section->flags & STYP_ROM_AREA) {
      addr = section->address;
      stop = addr + section->size;
      gp_debug("   make memory: section \"%s\" (0x%0*X - 0x%0*X)", section->name,
               addr_digits, addr, addr_digits, stop - 1);

      for ( ; addr < stop; addr++) {
        /* fetch the current contents of the memory */
        gp_mem_b_assert_get(section->data, addr, &byte, &section_name, &symbol_name);

        if (not_8_bit) {
          org = gp_processor_byte_to_org(class, addr);

          if (gp_processor_is_idlocs_org(processor, org) >= 0) {
            if (addr & 1) {
              /* This is higher byte. */
              byte |= ((Object->processor->idlocs_mask) >> 8) & 0xFF;
            }
            else {
              /* This is lower byte. */
              byte |= Object->processor->idlocs_mask & 0xFF;
            }
          }
        }

        /* write data to new memory */
        gp_mem_b_put(m, addr, byte, section_name, symbol_name);
      }
    }
    section = section->next;
  }

 return m;
}

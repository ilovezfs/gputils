/* GNU PIC coff linker functions
   Copyright (C) 2001, 2002, 2003
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

/* Two symbol tables are constructed. The first contains the definitions of all
   external symbols in all the object files.  This symbol table is used for
   relocation and linking.  The second table contains all external symbols 
   that do not yet have a definition.  This table is used to determine which
   objects in a library are to be linked against.  This table should be empty
   at the begining of the relocation process. */

void 
gp_link_add_symbol(struct symbol_table *table,
		   gp_symbol_type *symbol,
		   gp_object_type *file)
{
  struct symbol      *sym;
  gp_coffsymbol_type *var;

  /* Search the for the symbol.  If not found, then add it to 
     the global symbol table.  */
  sym = get_symbol(table, symbol->name);
  if (sym != NULL)
    return;  

  sym = add_symbol(table, symbol->name);
  var = malloc(sizeof(*var));
  var->symbol = symbol;
  var->file = file;
  annotate_symbol(sym, var);
  
  return;
}

void 
gp_link_remove_symbol(struct symbol_table *table, char *name)
{
  struct symbol      *sym;
  gp_coffsymbol_type *var;

  sym = get_symbol(table, name);
  if (sym == NULL)
    return;

  var = get_symbol_annotation(sym);
  free(var);
  remove_symbol(table, name);

  return;
}

/* Add the external symbols from an object file to the appropriate symbol 
   tables.  NOTE: The missing symbol table is optional. This feature is
   not used for generating symbol indexes for archives. */ 

int 
gp_link_add_symbols(struct symbol_table *definition,
                    struct symbol_table *missing,
                    gp_object_type *object)  
{
  gp_symbol_type *symbol = NULL;
  struct symbol *sym; 
  gp_coffsymbol_type *var;

  if ((definition == NULL) || (object == NULL))
    return 1;

  symbol = object->symbols;

  while (symbol != NULL ) {
    /* process all external symbols that are not directives */
    if ((symbol->class == C_EXT) && (symbol->name[0] != '.')) {
      if (symbol->section_number == 0) {
        /* This symbol is defined elsewhere. Check for it in the symbol 
	   definitions.  If it doesn't exist there, add it to the missing
	   symbol table, if not already entered. */
	sym = get_symbol(definition, symbol->name); 
        if ((sym == NULL) && (missing != NULL))
          gp_link_add_symbol(missing, symbol, object);
      } else {
        /* External symbol definition.  See if it is already defined, it so
	   it is an error. Add the symbol to the symbol definitions and remove
	   it from the missing symbol table if it exists there */
	sym = get_symbol(definition, symbol->name); 
        if (sym != NULL) {
          /* duplicate symbol */
          var = get_symbol_annotation(sym);
          gp_error("duplicate symbol \"%s\" defined in \"%s\" and \"%s\"", 
                   symbol->name,
                   var->file->filename,
                   object->filename);
        } else {	
          gp_link_add_symbol(definition, symbol, object);
        }
        if (missing != NULL)
          gp_link_remove_symbol(missing, symbol->name);
      }     
    }
    
    symbol = symbol->next;

  }

  return 0;
}

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
        /* The object has no symbols */
        object->symbols = list->symbols;
      } else {
        /* Append the symbols from the second object to the first */
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
        /* The object has no sections */
        object->sections = list->sections;
      } else {
        /* Append the sections from the second object to the first */
        object->sections_tail->next = list->sections;
      }
      object->sections_tail = list->sections_tail;
      object->num_sections += list->num_sections;
    }
    list = list->next;
  }

  /* FIXME: breaking the chain isn't good */
  object->next = NULL;

  return;
}

/* Update the line number offsets */

static void
_update_line_numbers(gp_linenum_type *line_number, int offset)
{

  while (line_number != NULL) {
    line_number->address += offset;
    line_number = line_number->next;
  }

}

/* Combine overlay sections in an object file. */

void
gp_cofflink_combine_overlay(gp_object_type *object, int remove_symbol)
{
  gp_section_type *first = NULL;
  gp_section_type *second = NULL;
  gp_section_type *list = NULL;
  gp_symbol_type  *symbol = NULL;
  int count;

  first = object->sections;
  
  while (first != NULL) {
    if (first->flags & STYP_OVERLAY) {
      second = gp_coffgen_findsection(object, first->next, first->name);    
      if (second != NULL) {
        /* The sections must have the same properties or they can't be 
           combined. */
        if (first->flags != second->flags) {
          gp_error("section types for \"%s\" do not match",
                   first->name);
          continue;
        } else if ((first->flags & STYP_ABS) &&
                   (first->address != second->address)) {
          gp_error("different addresses for absolute overlay sections \"%s\"",
                   first->name);
          continue;
        }

        /* Set the size of the first section to the larger of the two */
        if (second->size > first->size) {
          first->size = second->size;
          first->symbol->aux_list->_aux_symbol._aux_scn.length = second->size;
        }

        /* Remove the section symbol */
        if (remove_symbol) {
          symbol = object->symbols;
          while (symbol != NULL) {
            if (symbol->next == second->symbol) {
              symbol->next = second->symbol->next;
              /* FIXME : count = gp_coffgen_free_symbol(second->symbol); */
              count = 1;
              object->num_symbols -= (count + 1);
              break;
            }
            symbol = symbol->next;
          }
        }

        /* Update the symbol table */
        symbol = object->symbols;
        while (symbol != NULL) {
          if (symbol->section == second) {
            symbol->section = first;
          }
          symbol = symbol->next;
        }

        /* Remove the second section*/
        list = object->sections;
        while(list != NULL) {
          if (list->next == second) {
            list->next = second->next;
            break;
          }
          list = list->next;
        }
        object->num_sections--;
        /* FIXME: gp_coffgen_free_section(second); */
        
        /* Take another pass */
        gp_cofflink_combine_overlay(object, remove_symbol);
        return;
      }
    }
    first = first->next;
  }

  return;
}

/* Merge all sections in one object file with the same name. The overlayed
   sections must have been combined first.  */

void
gp_cofflink_merge_sections(gp_object_type *object)
{
  gp_section_type *first;
  gp_section_type *second;
  gp_section_type *list = NULL;
  gp_symbol_type  *symbol = NULL;
  gp_reloc_type   *relocation = NULL;
  unsigned int org;
  unsigned int last;
  unsigned int offset;
  unsigned int data;

  first = object->sections;
  while (first != NULL) {
    second = gp_coffgen_findsection(object, first->next, first->name);    
    if (second != NULL) {
      /* The sections must have the same properties or they can't be 
         combined. */
      if ((first->flags & STYP_ABS) || 
          (second->flags & STYP_ABS) ||
          (strcmp(first->name, ".config") == 0) ||
          (strcmp(first->name, ".idlocs") == 0)) {
        gp_error("section \"%s\" is absolute but occurs in more than one file",
                 first->name);
        exit(1);
      }

      gp_debug("  merging section \"%s\" with section \"%s\"", 
               first->name,
               second->name);

      /* Update the addresses in the relocation table */
      relocation = second->relocations;
      while (relocation != NULL) {
        relocation->address += first->size;
        relocation = relocation->next;
      }

      /* Update the section symbol for the section symbol */
      second->symbol->value = first->size;

      /* Copy the section data */
      if (second->flags & STYP_TEXT) {
        /* the section is executable, so each word is two bytes */
        last = second->size / 2;
        offset = first->size / 2;
      } else {
        /* the section is data, so each word is one byte */
        last = second->size;
        offset = first->size;
      }
      if(_has_data(second)) {
        for (org = 0; org < last; org++) {
          data = i_memory_get(second->data, org);
          assert((data & MEM_USED_MASK) != 0);
          i_memory_put(first->data, org + offset, data);
        }      
      }
      
      /* Update the line number offsets */
      _update_line_numbers(second->line_numbers, offset);

      /* Add section sizes */
      first->size += second->size;

      /* Append the relocations from the second section to the first */
      if (first->num_reloc == 0) {
        first->relocations = second->relocations;
      } else {
        first->relocations_tail->next = second->relocations;
      }
      first->num_reloc += second->num_reloc;
      first->relocations_tail = second->relocations_tail;
      
      /* Append the line numbers from the second section to the first. */
      if (first->num_lineno == 0) {
        first->line_numbers = second->line_numbers;
      } else {
        first->line_numbers_tail->next = second->line_numbers;
      }
      first->num_lineno += second->num_lineno;
      first->line_numbers_tail = second->line_numbers_tail;

      /* Update the symbol table */
      symbol = object->symbols;
      while (symbol != NULL) {
        if ((symbol->section_number > 0) &&  
            (symbol->section == second)) {
          symbol->section = first;
          symbol->value += offset;
        }
        symbol = symbol->next;
      }
      
      /* Remove the second section*/
      list = object->sections;
      while(list != NULL) {
        if (list->next == second) {
          list->next = second->next;
          break;
        }
        list = list->next;
      }
      object->num_sections--;
      /* FIXME: gp_coffgen_free_section(second); */
        
      /* Take another pass */
      gp_cofflink_merge_sections(object);
      return;
    }
    first = first->next;
  }

  return;
}

/* Set the memory used flags in a block of words */

static void
_set_used(MemBlock *m, int byte_addr, unsigned int address, unsigned int size)
{
  unsigned int org;
  unsigned int stop;
  unsigned int data;

  org = address >> byte_addr;
  stop = org + size;
  for ( ; org < stop; org++) {
    data = i_memory_get(m, org);
    if (data & MEM_USED_MASK) {
      gp_error("multiple sections using address %#lx", org);
      return;
    } else {
      i_memory_put(m, org, MEM_USED_MASK);
    }
  }

  return;
}

/* allocate space for the absolute sections */

void
gp_cofflink_reloc_abs(MemBlock *m,
                      int byte_addr,
                      gp_section_type *section,
                      unsigned long flags)
{
  unsigned int size;
  unsigned int addr;

  while (section != NULL) {
    if ((section->flags & STYP_ABS) &&
        (section->flags & flags)) {
      if (section->flags & STYP_TEXT) {
        /* size is in bytes, but words are stored in memory */
        size = section->size / 2;
      } else {
        size = section->size;
      }

      addr = section->address >> byte_addr;
      _set_used(m, byte_addr, addr, size);

      /* Set the relocated flag */
      section->flags |= STYP_RELOC;

    }
    section = section->next;
  }

}

/* Search through all the sections in the object list.  Locate the biggest
   assigned section that has not been relocated. */

gp_section_type *
gp_cofflink_find_big_assigned(gp_section_type *section, 
                              unsigned long flags,
                              struct symbol_table *logical_sections)  
{
  gp_section_type *biggest = NULL;
  struct symbol *sym;

  while (section != NULL) {
    sym = get_symbol(logical_sections, section->name);
    if ((sym != NULL) &&
        (section->flags & flags) &&
        !(section->flags & STYP_RELOC)) {
      /* This section has not been relocated */
      if ((biggest == NULL) ||
          (section->size > biggest->size)) {
        biggest = section;
      }
    }
    section = section->next;
  }

  return biggest;
}

/* Search through all the sections in the object list.  Locate the biggest
   section that has not been relocated. */

gp_section_type *
gp_cofflink_find_big_section(gp_section_type *section, 
                             unsigned long flags)
{
  gp_section_type *biggest = NULL;

  while (section != NULL) {
    if ((section->flags & flags) &&
        !(section->flags & STYP_RELOC)) {
      /* This section has not been relocated */
      if ((biggest == NULL) ||
          (section->size > biggest->size)) {
        biggest = section;
      }
    }
    section = section->next;
  }

  if (biggest != NULL) {
    gp_debug("  biggest section = %s, section flags = %#x, flags = %#x", 
             biggest->name,
             biggest->flags,
             flags);
  }
           
  return biggest;
}

/* Search through the target memory.  Locate the smallest block of memory
   that is larger than the requested size.  Return the address of that 
   block */

static int
_search_memory(MemBlock *m,
               int byte_addr, 
               unsigned int start,
               unsigned int stop,
               unsigned int size,
               unsigned int *block_address,
               unsigned int *block_size)
{
  unsigned int org;
  unsigned int current_address = 0;
  unsigned int current_size = 0;
  int mem_used;
  int in_block = 0;
  int end_block = 0;
  int success = 0;

  /* data is stored as words in memory */
  start = start >> byte_addr;
  stop = stop >> byte_addr;

  /* set the size to max value */
  *block_size = 0xffffffff;

  for (org = start; org <= stop; org++) {
    mem_used = i_memory_get(m, org) & MEM_USED_MASK;
    if (org == stop) { 
      if (in_block == 1) {
        /* end of the section definition */
        end_block = 1;
        /* increment for last address */
        current_size++;
      } else if (start == stop) {
        /* special case, one word section */
        if (!mem_used) {
          end_block = 1;
          current_address = start;
          current_size = 1;
        }
      }
      in_block = 0;
    } else if (mem_used) {
      if (in_block == 1) {
        /* end of an unused block of memory */
        end_block = 1;
      }
      in_block = 0;
    } else {
      if (in_block == 0) {
        /* start of an unused block of memory */
        gp_debug("    start unused block at %#x", org);
        current_address = org;
        current_size = 1;
      } else {
        /* continuation of an unused block of memory */
        current_size++;
      }      
      in_block = 1;
    }
  
    if (end_block == 1) {
      gp_debug("    end unused block at %#x with size %#x", 
               org, 
               current_size);
      if ((current_size >= size) &&
          (current_size < *block_size)) {
        *block_size = current_size;
        *block_address = current_address;
        success = 1;
      }    
      end_block = 0;
    }
  }

  return success;
}

/* Move data in i_memory.  This function assumes the move will be towards
   a higher address. */

static void
_move_data(MemBlock *m, 
           int byte_addr,
           unsigned int address, 
           unsigned int size,
           unsigned int new_address) 
{
  int org;
  unsigned int data;

  if (address == new_address)
    return;

  gp_debug("    moving %#x words from %#x to %#x", 
           size,
           address,
           new_address);

  /* data is stored as words in memory */
  address = address >> byte_addr;
  new_address = new_address >> byte_addr;

  for (org = address + size - 1; org >= 0; org--) {
    data = i_memory_get(m, org);
    gp_debug("      moving word %#x from %#x to %#x", 
             data, org, new_address + org);
    assert(data & MEM_USED_MASK);
    i_memory_put(m, org, 0x0);
    i_memory_put(m, new_address + org, data);
  }

  return;
}

/* allocate memory for relocatable assigned sections */

void
gp_cofflink_reloc_assigned(MemBlock *m,
                           int byte_addr,
                           gp_section_type *section,
                           unsigned long flags,
                           struct symbol_table *sections,
                           struct symbol_table *logical_sections)  
{
  gp_section_type *current;
  unsigned int size;
  struct symbol *sym;
  char *section_name;
  struct linker_section *section_def;
  unsigned int current_address;
  unsigned int current_size;

  while (1) {
    current = gp_cofflink_find_big_assigned(section, flags, logical_sections);
    
    if (current == NULL)
      break;
    
    if (current->flags & STYP_TEXT) {
      /* size is in bytes, but words are stored in memory */
      size = current->size / 2;
    } else {
      size = current->size;
    }

    /* Fetch the logical section */
    sym = get_symbol(logical_sections, current->name);
    assert(sym != NULL);

    /* Fetch the section definition */ 
    section_name = get_symbol_annotation(sym);
    sym = get_symbol(sections, section_name);
    assert(sym != NULL);
    section_def = get_symbol_annotation(sym);
    assert(section_def != NULL);
        
    /* assign the address to this section */
    if (_search_memory(m, byte_addr, section_def->start, section_def->end,
                       size, &current_address, &current_size) == 1) {
      gp_debug("    sucessful relocation to %#x", current_address);
      if (_has_data(current)) {
        _move_data(current->data, byte_addr, current->address, size, 
	           current_address);
      }
      current->address = current_address;
      _set_used(m, byte_addr, current_address, size);

      /* Update the line number offsets */
      _update_line_numbers(current->line_numbers, current_address >> byte_addr);

      /* Set the relocated flag */
      current->flags |= STYP_RELOC;

    } else {
      gp_error("no target memory available for section \"%s\"", current->name);    
      return;
    }
  }

  return;
}

/* allocate memory for relocatable unassigned sections */

void
gp_cofflink_reloc_unassigned(MemBlock *m,
                             int byte_addr,
                             gp_section_type *section,
                             unsigned long flags,
                             struct symbol_table *sections)  

{
  gp_section_type *current;
  enum section_type type;
  unsigned int size;
  int success;
  int i;
  struct symbol *sym;
  struct linker_section *section_def;
  unsigned int current_address;
  unsigned int current_size;
  unsigned int smallest_address;
  unsigned int smallest_size;

  while (1) {
    current = gp_cofflink_find_big_section(section, flags);
    
    if (current == NULL)
      break;
   
    if (current->flags & STYP_TEXT) {
      /* size is in bytes, but words are stored in memory */
      size = current->size / 2;
    } else {
      size = current->size;
    }

    /* determine what type of sections are being relocated */
    if (current->flags & STYP_TEXT) {
      type = codepage;
      gp_debug("  relocating code");
    } else if (current->flags & STYP_ACCESS) {
      type = accessbank;
      gp_debug("  relocating accessbank");
    } else if (current->flags & STYP_SHARED) {
      type = sharebank;
      gp_debug("  relocating sharebank");
    } else {
      type = databank;
      gp_debug("  relocating data");
    }

    success = 0;
    smallest_address = 0;
    smallest_size = 0xffffffff;

    /* search the section definitions for the smallest block of memory that 
       the section will fit in */
    for (i = 0; i < HASH_SIZE; i++) {
      for (sym = sections->hash_table[i]; sym; sym = sym->next) {
        section_def = get_symbol_annotation(sym);
        if ((section_def->type == type) &&
            (section_def->protected == 0)) {
          gp_debug("  section = %s", current->name);
          gp_debug("    size = %#x", current->size);
          gp_debug("    def start = %#x", section_def->start);
          gp_debug("    def end = %#x", section_def->end);
          
          if (_search_memory(m, 
                             byte_addr,
                             section_def->start,
                             section_def->end,
                             size,
                             &current_address,
                             &current_size) == 1) {
            success = 1;
            if (smallest_size > current_size) {
              smallest_size = current_size;
              smallest_address = current_address;
            }
          }
        }
      }
    }

    /* set the memory used flag for all words in the block */
    if (success == 1) {
      gp_debug("    sucessful relocation to %#x", smallest_address);
      if (_has_data(current)) {
        _move_data(current->data, byte_addr, current->address, size, 
	           smallest_address);
      }
      current->address = smallest_address;
      _set_used(m, byte_addr, smallest_address, size);

      /* Update the line number offsets */
      _update_line_numbers(current->line_numbers, smallest_address >> byte_addr);

      /* Set the relocated flag */
      current->flags |= STYP_RELOC;

    } else {
      gp_error("no target memory available for section \"%s\"", current->name);    
      return;
    }
  }

  return;
}

/* update all symbols with their new relocated values  */

static void 
_update_table(gp_object_type *object)  
{
  gp_symbol_type *symbol = object->symbols;
  gp_section_type *section = object->sections;

  gp_debug("updating symbols with their new relocated values");

  while (symbol != NULL) {
    if (symbol->section_number > 0) {
      assert(symbol->section != NULL);
      if (!(symbol->section->flags & STYP_ABS))
        symbol->value += symbol->section->address;
    }

    symbol = symbol->next;
  }

  gp_debug("stripping section relocated flag");

  while (section != NULL) {
    section->flags &= ~(STYP_RELOC);
    section = section->next;
  }

}

/* Create sections to fill unused memory in the pages with constant data. */

void 
gp_cofflink_fill_pages(gp_object_type *object,
                       MemBlock *m,
                       int byte_addr,
                       struct symbol_table *sections)
{
  struct linker_section *section_def;
  int i;
  struct symbol *sym;
  int found;
  char fill_name[BUFSIZ];
  int fill_number = 1;
  gp_section_type *section = NULL;
  unsigned int current_address;
  unsigned int current_size;
  int org;
  int end;

  gp_debug("adding fill sections");

  /* search for any section definitions that have a fill */
  for (i = 0; i < HASH_SIZE; i++) {
    for (sym = sections->hash_table[i]; sym; sym = sym->next) {
      section_def = get_symbol_annotation(sym);
      if ((section_def->type == codepage) &&
          (section_def->use_fill == 1)) {
        while (1) {
          found = _search_memory(m, 
                                 byte_addr,
                                 section_def->start,
                                 section_def->end,
                                 1,
                                 &current_address,
                                 &current_size);
          if (found == 1) {
            sprintf(fill_name, ".fill_%i", fill_number++);
            gp_debug("  new section \"%s\" at %#x with size %#x and data %#x", 
                     fill_name,
                     current_address,
                     current_size,
                     section_def->fill);
            section = gp_coffgen_findsection(object, 
                                             object->sections,
                                             fill_name);
            if (section != NULL) {
              gp_error("fill section \"%s\" aready exists", fill_name);    
              return;
            } else {
              /* create a new section for the fill data */
              section = gp_coffgen_addsection(object, fill_name); 
              section->address = current_address; 
              section->size = current_size * 2; /* size in bytes */
              section->flags = STYP_TEXT;
              /* FIXME: do we really need a section symbol? */
              
              /* mark the memory as used */
              _set_used(m, byte_addr, current_address, current_size);
              
              /* fill the section memory */
              org = current_address >> byte_addr;
              end = org + current_size;
              for ( ; org <= end; org++) {
                i_memory_put(section->data,
                             org,
                             MEM_USED_MASK | section_def->fill);
              }
            }
          } else {
            break;
          }
        }
      }
    }
  } 

  return;
}

/* relocate all sections in the object file */

void 
gp_cofflink_reloc(gp_object_type *object,
                  struct symbol_table *sections,
                  struct symbol_table *logical_sections)  
{
  MemBlock *data, *program;
  int byte_addr;
  
  /* Enhanced 16 bit devices (18xx) use byte addressing */
  if (object->class == PROC_CLASS_PIC16E)
    byte_addr = 1;
  else
    byte_addr = 0;

  /* create memory representing target memory */ 
  data = i_memory_create();
  program = i_memory_create();

  /* combine all object files into one object */
  gp_cofflink_combine_objects(object);

  /* combine overlay sections */
  gp_cofflink_combine_overlay(object, 0);

  /* combine all sections with the same name */
  gp_cofflink_merge_sections(object);

  /* allocate memory for absolute sections */
  gp_debug("verifying absolute sections.");
  gp_cofflink_reloc_abs(program, 
                        byte_addr,
                        object->sections,
                        STYP_TEXT);
  gp_cofflink_reloc_abs(data, 
                        0,
                        object->sections, 
                        STYP_DATA | STYP_BSS | STYP_SHARED | 
                        STYP_OVERLAY | STYP_ACCESS);

  /* FIXME: allocate assigned stacks */ 

  /* allocate memory for relocatable assigned sections */
  gp_debug("relocating assigned sections.");
  gp_cofflink_reloc_assigned(program, 
                             byte_addr,
                             object->sections,
                             STYP_TEXT,
                             sections,
                             logical_sections);
  gp_cofflink_reloc_assigned(data, 
                             0,
                             object->sections, 
                             STYP_DATA | STYP_BSS | STYP_SHARED | 
                             STYP_OVERLAY | STYP_ACCESS,
                             sections,
                             logical_sections);
  
  /* FIXME: allocate unassigned stacks */ 

  /* allocate memory for relocatable unassigned sections */
  gp_debug("relocating unassigned sections.");
  gp_cofflink_reloc_unassigned(program, 
                               byte_addr,
                               object->sections,
                               STYP_TEXT,
                               sections);
  gp_cofflink_reloc_unassigned(data, 
                               0,
                               object->sections, 
                               STYP_DATA | STYP_BSS | STYP_SHARED | 
                               STYP_OVERLAY | STYP_ACCESS,
                               sections);

  _update_table(object);

  gp_cofflink_fill_pages(object, program, byte_addr, sections);

  i_memory_free(data);
  i_memory_free(program);

  return;
}

static void
check_relative(gp_section_type *section, int org, int argument, int range)
{
  /* If the branch is too far then issue a warning */
  if ((argument > range) || (argument < -(range+1))) {
    gp_warning("relative branch out of range in at %#x of section \"%s\"",
               org,
	       section->name);
  }

  return;
} 

/* patch one word with the relocated address */ 

void 
gp_cofflink_patch_addr(enum proc_class class,
                       int bsr_boundary,
                       gp_section_type *section, 
                       gp_symbol_type *symbol,
                       gp_reloc_type *relocation)
{
  int org;
  int data;
  int value;
  int offset;
  int write_data = 1;

  /* section address are byte addresses */
  if (class == PROC_CLASS_PIC16E)
    org = section->address >> 1;
  else
    org = section->address;
  
  org += (relocation->address >> 1);
  
  value = symbol->value + relocation->offset;

  gp_debug("  patching %#x from %s with %#x", 
           org, 
           section->name, 
           value);
  
  /* fetch the current contents of the memory */
  data = i_memory_get(section->data, org);
  assert(data & MEM_USED_MASK);

  /* FIXME: Not sure if warnings should be generated for out of range 
            arguments. The linker should make sure values are within
            ranges in the linker scripts. */

  switch (relocation->type) {
  case RELOCT_CALL:
    switch(class) {
    case PROC_CLASS_PIC12:
    case PROC_CLASS_SX:
      data = data | (value & 0xff);
      break;
    case PROC_CLASS_PIC14:
      data = data | (value & 0x7ff);
      break;
    case PROC_CLASS_PIC16:
      data = data | (value & 0x1fff);
      break;
    case PROC_CLASS_PIC16E:
      data = data | ((value >> 1) & 0xff);
      break;
    default:
      assert(0);
    }
    break;
  case RELOCT_GOTO:
    switch(class) {
    case PROC_CLASS_PIC12:
    case PROC_CLASS_SX:
      data = data | (value & 0x1ff);
      break;
    case PROC_CLASS_PIC14:
      data = data | (value & 0x7ff);
      break;
    case PROC_CLASS_PIC16:
      data = data | (value & 0x1fff);
      break;
    case PROC_CLASS_PIC16E:
      data = data | ((value >> 1) & 0xff);
      break;
    default:
      assert(0);
    }
    break;
  case RELOCT_HIGH:
    data = data | ((value >> 8) & 0xff);   
    break; 
  case RELOCT_LOW:
    data = data | (value & 0xff);   
    break; 
  case RELOCT_P:
    data = data | ((value & 0x1f) << 8);   
    break; 
  case RELOCT_BANKSEL:
    {
      int bank = gp_processor_check_bank(class, value);
      gp_processor_set_bank(class, bank, section->data, org);
      write_data = 0;
    }
    break;
  case RELOCT_IBANKSEL:
    switch(class) {
    case PROC_CLASS_PIC14:
      if (value < 0x100) {
        /* bcf 0x3, 0x7 */
        data = MEM_USED_MASK | 0x1383;
      } else {
        /* bsf 0x3, 0x7 */
        data = MEM_USED_MASK | 0x1783;
      }
      break;
    case PROC_CLASS_PIC16:
      /* movlb bank */
      data = MEM_USED_MASK | 0xb800 | gp_processor_check_bank(class, value);
      break;
    default:
      assert(0);
    }
    break; 
  case RELOCT_F:
    switch(class) {
    case PROC_CLASS_PIC12:
    case PROC_CLASS_SX:
      data = data | (value & 0x1f);
      break;
    case PROC_CLASS_PIC14:
      data = data | (value & 0x7f);
      break;
    case PROC_CLASS_PIC16:
    case PROC_CLASS_PIC16E:
      data = data | (value & 0xff);
      break;
    default:
      assert(0);
    }
    break; 
  case RELOCT_TRIS:
    switch(class) {
    case PROC_CLASS_PIC12:
    case PROC_CLASS_SX:
      data = data | (value & 0x1f);
      break;
    case PROC_CLASS_PIC14:
      data = data | (value & 0x7f);
      break;
    default:
      assert(0);
    }
    break; 
  case RELOCT_MOVLR:
    data = data | ((value << 4) & 0xf0);
    break;
  case RELOCT_MOVLB:
    data = data | (value & 0xff);
    break;
  case RELOCT_GOTO2:
    data = data | ((value >> 9) & 0xfff);
    break;
  case RELOCT_FF1:
    data = data | (value & 0xfff);
    break;
  case RELOCT_FF2:
    data = data | (value & 0xfff);
    break;
  case RELOCT_LFSR1:
    data = data | ((value >> 8) & 0xf);
    break;
  case RELOCT_LFSR2:
    data = data | (value & 0xff);
    break;
  case RELOCT_BRA:
    offset = (value - ((org + 1) << 1)) >> 1;
    check_relative(section, org, offset, 0x7ff);
    data = data | (offset & 0x7ff);
    break;
  case RELOCT_CONDBRA:
    offset = (value - ((org + 1) << 1)) >> 1;
    check_relative(section, org, offset, 127);
    data = data | (offset & 0xff);
    break;
  case RELOCT_UPPER:
    data = data | ((value >> 16) & 0x3f);   
    break; 
  case RELOCT_ACCESS:
    {
      int a;
      
      if ((value < bsr_boundary) || 
          (value >= (0xf00 + bsr_boundary))) {
        a = 0;
      } else {
        a = 1;
      }

      data = data | (a << 8);   
    }
    break;
  case RELOCT_PAGESEL_WREG:
  case RELOCT_PAGESEL_BITS:
    {
      int page = gp_processor_check_page(class, value);
      gp_processor_set_page(class, page, section->data, org);
      write_data = 0;
    }
    break;
  /* unimplemented relocations */
  case RELOCT_PAGESEL:
  case RELOCT_ALL:
  case RELOCT_SCNSZ_LOW:
  case RELOCT_SCNSZ_HIGH:
  case RELOCT_SCNSZ_UPPER:
  case RELOCT_SCNEND_LOW:
  case RELOCT_SCNEND_HIGH:
  case RELOCT_SCNEND_UPPER:
  case RELOCT_SCNEND_LFSR1:
  case RELOCT_SCNEND_LFSR2:
  default:
    gp_error("unimplemented relocation = %i in section \"%s\"", 
             relocation->type,
             section->name);
    assert(0);
  }

  /* update memory with the new value */
  if (write_data)
    i_memory_put(section->data, org, data);

  return;
}

/* Patch all addresses with the relocated symbols.  The relocations are
   stripped from the sections. */

void 
gp_cofflink_patch(gp_object_type *object,
                  struct symbol_table *symbols)
{
  gp_section_type    *section = object->sections;
  gp_reloc_type      *relocation;
  gp_symbol_type     *symbol;
  gp_coffsymbol_type *var;
  struct symbol      *sym;
  int bsr_boundary = 0;
  
  if (object->class == PROC_CLASS_PIC16E)  
    bsr_boundary = gp_processor_bsr_boundary(object->processor);

  gp_debug("patching data with relocated symbols");

  while (section != NULL) {
    if (_has_data(section)) {
      /* patch raw data with relocation entries */
      relocation = section->relocations;
      while (relocation != NULL) {
        symbol = relocation->symbol;
        if ((symbol->class == C_EXT) && 
            (symbol->section_number == 0)) {
          /* This is an external symbol defined elsewhere */
          sym = get_symbol(symbols, symbol->name);
          assert(sym != NULL);
          var = get_symbol_annotation(sym);
          assert(var != NULL);	  
          symbol = var->symbol;
        }
        gp_cofflink_patch_addr(object->class,
	                       bsr_boundary,
			       section,
			       symbol,
			       relocation);

        relocation = relocation->next;
      }
    
      /* strip the relocations from the section */
      section->num_reloc = 0;
      section->relocations = NULL;
      section->relocations_tail = NULL;
    }

    section = section->next;
  }

  return;
}

/* Cleanup the symbol table after relocating and linking. */

void 
gp_cofflink_clean_table(gp_object_type *object)
{
  gp_symbol_type *previous = NULL;
  gp_symbol_type *symbol = object->symbols;

  gp_debug("cleaning symbol table");

  while (symbol != NULL) {
    if ((symbol->class == C_EXT) && 
        (symbol->section_number == 0)) {
      /* This is an external symbol defined elsewhere */
      if (previous == NULL) {
        /* removing first symbol in the list */
        object->symbols = object->symbols->next;
      } else {
        previous->next = symbol->next;
      }
      gp_debug("  removed symbol \"%s\"", symbol->name);
      object->num_symbols--;
    } else {
      previous = symbol;
    }

    symbol = symbol->next;
  }

  return;
}

/* copy all executatable data to new memory */

MemBlock * 
gp_cofflink_make_memory(gp_object_type *object)
{
  gp_section_type *section = object->sections;
  MemBlock *m;
  unsigned int org;
  unsigned int stop;
  unsigned int data;

  m = i_memory_create();

  while (section != NULL) {
    if ((section->flags & STYP_TEXT) ||
        (section->flags & STYP_DATA_ROM)) {
      if (object->class == PROC_CLASS_PIC16E)
        org = section->address >> 1;
      else
        org = section->address;
      
      stop = org + (section->size / 2);
            
      for ( ; org < stop; org++) {
        /* fetch the current contents of the memory */
        data = i_memory_get(section->data, org);
        assert(data & MEM_USED_MASK);
        /* write data to new memory */
        i_memory_put(m, org, data);
      }
    }
    section = section->next;
  }
 
 return m;
}

/* GNU PIC coff linker functions
   Copyright (C) 2001 Craig Franklin

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
                   char *name,
		   struct syment *coffsym,
		   gp_object_type *file)
{
  struct symbol      *sym;
  gp_coffsymbol_type *var;

  /* Search the for the symbol.  If not found, then add it to 
     the global symbol table.  */
  sym = get_symbol(table, name);
  if (sym != NULL)
    return;  

  sym = add_symbol(table, name);
  var = malloc(sizeof(*var));
  var->coffsym = coffsym;
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
  gp_symbol_type *list = NULL;
  struct syment *coffsym = NULL;
  char name[BUFSIZ];
  struct symbol *sym; 
  gp_coffsymbol_type *var;
  int i;

  if ((definition == NULL) || (object == NULL))
    return 1;

  list = object->sym_table;

  while (list != NULL ) {
    coffsym = &list->symbol;
    gp_coffgen_fetch_symbol_name(object, coffsym, &name[0]);
    /* process all external symbols that are not directives */
    if ((coffsym->st_class == C_EXT) && (name[0] != '.')) {
      if (coffsym->sec_num == 0) {
        /* This symbol is defined elsewhere. Check for it in the symbol 
	   definitions.  If it doesn't exist there, add it to the missing
	   symbol table, if not already entered. */
	sym = get_symbol(definition, &name[0]); 
        if ((sym == NULL) && (missing != NULL))
          gp_link_add_symbol(missing, &name[0], coffsym, object);
      } else {
        /* External symbol definition.  See if it is already defined, it so
	   it is an error. Add the symbol to the symbol definitions and remove
	   it from the missing symbol table if it exists there */
	sym = get_symbol(definition, &name[0]); 
        if (sym != NULL) {
          /* duplicate symbol */
          var = get_symbol_annotation(sym);
          gp_error("duplicate symbol \"%s\" defined in \"%s\" and \"%s\"", 
                   name,
                   var->file->filename,
                   object->filename);
        } else {	
          gp_link_add_symbol(definition, &name[0], coffsym, object);
        }
        if (missing != NULL)
          gp_link_remove_symbol(missing, &name[0]);
      }     
    }

    /* don't process auxillary symbols */
    for (i = 0; i < coffsym->num_auxsym; i++) {
      list = list->next;
    }
    
    list = list->next;

  }

  return 0;
}

/* FIXME: need a function to check the section definitions for overlapping
          addresses */

void 
gp_link_reloc(struct objectlist *list,
              struct symbol_table *sections,
              struct symbol_table *logical_sections)  
{
  gp_object_type  *object;
  gp_section_type *section;
  gp_symbol_type  *symbol;
  char name[256];
  int i;
  struct symbol *sym;
  char *section_name;
  struct linker_section *section_def;

  assert(list != NULL);
  assert(sections != NULL);
  assert(logical_sections != NULL);
  
  while (list != NULL) {
    object = list->object;
    assert(object != NULL);
    section = object->sections;
    while (section != NULL) {
      gp_coffgen_fetch_section_name(object, &section->header, &name[0]);
      sym = get_symbol(logical_sections, &name[0]);
      if (sym == NULL) {
        /* FIXME: Maybe issue error or place code in a default location*/  
      } else {
        /* find the linker script section definition that this section belongs
	   in */ 
        section_name = get_symbol_annotation(sym);
        sym = get_symbol(sections, section_name);
        assert(sym != NULL);
        section_def = get_symbol_annotation(sym);
        assert(section_def != NULL);
        /* assign the address to this section */
	section->header.s_paddr = section_def->next_address;
        section->header.s_vaddr = section_def->next_address;
        section_def->next_address += (section->header.s_size / 2);
      }
    
      /* update the symbol addresses for this section */
      symbol = object->sym_table;
      while (symbol != NULL) {
        if (symbol->symbol.sec_num == section->number) {
          symbol->symbol.value += section->header.s_paddr; 
        }
      
        /* don't process auxillary symbols */
        for (i = 0; i < symbol->symbol.num_auxsym; i++) {
          symbol = symbol->next;
        }
    
        symbol = symbol->next;
      }

      section = section->next;
    }

    list = list->next;
  }

  return;
}

/* FIXME: finish defining patch operations */

/* patch one word with the relocated address */ 

void 
gp_link_patch_addr(MemBlock *m, 
                   unsigned int org,
                   long value,
                   unsigned short type)
{
  unsigned int data;
  
  /* fetch the current contents of the memory */
  data = i_memory_get(m, org);
  assert((data & MEM_USED_MASK) != 0);

  switch (type) {
  case RELOCT_CALL:
    /* lowest eleven bits contain address */
    if (value > 0x7ff) {
      /* issue a warning */
    }
    data = data | (value & 0x7ff);    
    break;
  case RELOCT_GOTO:
    /* lowest eleven bits contain address */
    if (value > 0x7ff) {
      /* issue a warning */
    }
    data = data | (value & 0x7ff);   
    break;
  case RELOCT_BANKSEL:
  
    break;
  case RELOCT_F:
    /* lowest seven bits contain address */
    if (value > 0x7F) {
      /* issue a warning */
    }
    data = data | (value & 0x7f);   
    break; 
  default:
    printf("unimplemented relocation = %i\n", type);
    assert(0);
  }

  /* update memory with the new value */
  i_memory_put(m, org, data);

  return;
}

/* patch all addresses with the relocated symbols */

void 
gp_link_patch(struct objectlist *list,
              struct symbol_table *symbols)
{
  gp_object_type     *object;
  gp_section_type    *section;
  gp_reloc_type      *relocation;
  gp_symbol_type     *symbolentry;
  gp_coffsymbol_type *var;
  struct syment      *symbol;
  char name[256];
  struct symbol *sym;
  
  assert(list != NULL);
  assert(symbols != NULL);

  while (list != NULL) {
    object = list->object;
    assert(object != NULL);
    section = object->sections;
    while (section != NULL) {
      /* patch raw data with relocation entries */
      relocation = section->relocations;
      while (relocation != NULL) {
        symbolentry = gp_coffgen_findsymbolnum(object, 
                                               relocation->relocation.r_symndx);
        symbol = &symbolentry->symbol;
        if ((symbol->st_class == C_EXT) && 
            (symbol->sec_num == 0)) {
	  /* This is an external symbol defined elsewhere */
	  gp_coffgen_fetch_symbol_name(object, symbol, &name[0]);
          sym = get_symbol(symbols, &name[0]);
	  assert(sym != NULL);
	  var = get_symbol_annotation(sym);
	  assert(var != NULL);	  
	  symbol = var->coffsym;
        }
        gp_link_patch_addr(section->data,
                           relocation->relocation.r_vaddr >> 1,
                           symbol->value, 
                           relocation->relocation.r_type);
  
        relocation = relocation->next;
      }

      section = section->next;
    }

    list = list->next;
  }

  return;
}

/* Combine the relocated and linked files into one object file.  This function
   is destructive.  Objects in the list are modified. */

gp_object_type *
gp_link_combine(struct objectlist *list, 
                char *name, 
                enum pic_processor processor)
{
  gp_object_type  *new = NULL;

  assert(list != NULL);
  assert(name != NULL);

  new = gp_coffgen_init(name, processor);
  new->file_header.f_flags = F_EXEC;

  return new;
}

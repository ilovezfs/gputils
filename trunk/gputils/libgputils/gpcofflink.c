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
#include "gpcoff.h"
#include "gpreadobj.h"
#include "gpsymbol.h"
#include "gpcoffgen.h"
#include "gpcofflink.h"

/* Two symbol tables are constructed. The first contains the definitions of all
   external symbols in all the object files.  This symbol table is used for
   relocation and linking.  The second table contains all external symbols 
   that do not yet have a definition.  This table is used to determine which
   objects in a library are to be linked against.  This table should be empty
   at the begining of the relocation process. */

void gp_link_add_symbol(struct symbol_table *table,
                        char *name,
		        struct syment *coffsym,
		        struct objectfile *file)
{
  struct symbol     *sym;
  struct coffsymbol *var;

  /* Search the for the symbol.  If not found, then add it to 
     the global symbol table.  */
  sym = get_symbol(table, name);
  if (sym != NULL)
    return;  

  sym = add_symbol(table, name);
  var = malloc(sizeof(*var));
  var->coffsym = coffsym;
  var->file = file;
  var->filename = file->filename;
  annotate_symbol(sym, var);
  
  return;
}

void gp_link_remove_symbol(struct symbol_table *table, char *name)
{
  struct symbol     *sym;
  struct coffsymbol *var;

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
   used for generating symbol indexes for archives. */ 

int gp_link_add_symbols(struct symbol_table *definition,
                        struct symbol_table *missing,
                        struct objectfile *object)  
{
  struct syment *coffsym = NULL;
  char name[256];
  struct symbol *sym; 
  int i;

  if ((definition == NULL) || (object == NULL))
    return 1;

  for (i = 0; i < object->file_header.f_nsyms; i++) {
    coffsym = &object->symtbl[i];
    gp_fetch_symbol_name(object, coffsym, &name[0]);
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
          /* FIXME: generate an error, duplicate symbols */

        } else {	
          gp_link_add_symbol(definition, &name[0], coffsym, object);
        }
        if (missing != NULL)
          gp_link_remove_symbol(missing, &name[0]);
      }     
    }

    /* don't process auxillary symbols */
    if (coffsym->num_auxsym == 1)
      i++;

  }

  return 0;
}

int gp_link_check_object(char *name,
                         struct objectfile *object)
{
  int valid = 0;


  return valid;
}

/* FIXME: need a function to check the section definitions for overlapping
          addresses */

void gp_link_reloc(struct objectlist *list,
                   struct symbol_table *sections,
                   struct symbol_table *logical_sections)  
{
  struct objectfile *object;
  struct section    *section;
  int i;
  char name[256];
  struct symbol *sym;
  char *section_name;
  struct linker_section *section_def;
  struct syment *symbol;
  int index;

  assert(list != NULL);
  assert(sections != NULL);
  assert(logical_sections != NULL);
  
  while (list != NULL) {
    object = list->object;
    assert(object != NULL);
    for (i = 0; i < object->file_header.f_nscns; i++) {
      section = &object->sections[i];
      gp_fetch_section_name(object, &section->header, &name[0]);
      sym = get_symbol(logical_sections, &name[0]);
      if (sym == NULL) {
        /* FIXME: issue error */  
      } else {
        /* find the linker script section defintion that this section belongs
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

    }

    /* update the symbol addresses */
    for (i = 0; i < object->file_header.f_nsyms; i++) {    
      symbol = &object->symtbl[i];
      if (symbol->sec_num > 0) {
        index = symbol->sec_num - 1;
        section = &object->sections[index];
        symbol->value += section->header.s_paddr; 
      }
      
      /* don't process auxillary symbols */
      if (symbol->num_auxsym == 1)
        i++;
    }

    list = list->next;
  }

  return;
}

/* FIXME: finish defining patch operations */

void gp_link_patch_addr(unsigned int *data, 
                        long value,
			unsigned short type)
{

  switch (type) {
  case RELOCT_CALL:
    /* lowest eleven bits contain address */
    if (value > 0x7ff) {
      /* issue a warning */
    }
    *data = *data | (value & 0x7ff);    
    break;
  case RELOCT_GOTO:
    /* lowest eleven bits contain address */
    if (value > 0x7ff) {
      /* issue a warning */
    }
    *data = *data | (value & 0x7ff);   
    break;
  case RELOCT_BANKSEL:
  
    break;
  case RELOCT_F:
    /* lowest seven bits contain address */
    if (value > 0x7F) {
      /* issue a warning */
    }
    *data = *data | (value & 0x7f);   
    break; 
  default:
    printf("unimplemented relocation = %i\n", type);
    assert(0);
  }

  return;
}

void gp_link_patch(struct objectlist *list,
                   struct symbol_table *symbols)
{
  struct objectfile *object;
  struct section    *section;
  int i, j;
  char name[256];
  struct symbol *sym;
  struct reloc *relocation;
  struct syment *symbol;
  unsigned int *data;
  
  assert(list != NULL);
  assert(symbols != NULL);

  while (list != NULL) {
    object = list->object;
    assert(object != NULL);
    
    for (i = 0; i < object->file_header.f_nscns; i++) {
      section = &object->sections[i];
      /* patch raw data with relocation entries */
      for (j = 0; j < section->header.s_nreloc; j++) {  
        relocation = &section->relocations[j];
        symbol = &object->symtbl[relocation->r_symndx]; 
        if ((symbol->st_class == C_EXT) && (symbol->sec_num == 0)) {
	  /* This is an external symbol defined elsewhere */
	  gp_fetch_symbol_name(object, symbol, &name[0]);
          sym = get_symbol(symbols, &name[0]);
	  assert(sym != NULL);
	  symbol = get_symbol_annotation(sym);
	  assert(symbol != NULL);	  
	}
	data = &section->data[relocation->r_vaddr >> 1];
	gp_link_patch_addr(data, symbol->value, relocation->r_type);
  
      }
    }

    list = list->next;
  }

  return;
}

void gp_link_copy_section(struct section *output, struct section *input)
{



}

struct objectfile *gp_link_combine(struct objectlist *list)
{
  struct objectlist *store;
  struct objectfile *object;
  struct objectfile *output = NULL;
  struct section    *section;
  char name[256];
  struct symbol_table *section_names;
  int i;
  struct symbol *sym;
  int *number;
  
  int num_sections = 0;
  int num_symbols  = 0;
  unsigned int strtbl_size  = 0;

  if (list != NULL)
     output = gp_coff_init("a.out");

  /* create a symbol table with the section names */
  section_names = push_symbol_table(NULL, 1);

  store = list;

  /* scan through the files to determine number of sections */ 
  while (list != NULL) {
    object = list->object;
    assert(object != NULL);

    num_symbols += object->file_header.f_nsyms;
    strtbl_size += get_32((unsigned char *)&object->strtbl[0]);
    
    for (i = 0; i < object->file_header.f_nscns; i++) {
      section = &object->sections[i];

      gp_fetch_section_name(object, &section->header, &name[0]);
      sym = get_symbol(section_names, &name[0]);
      if (sym == NULL) {
	sym = add_symbol(section_names, &name[0]);
        number = (int *)malloc(sizeof(int));
	*number = 1;
	annotate_symbol(sym, number);
        num_sections++;
      } else {
        number = get_symbol_annotation(sym);
	/**number++; FIXME: This causes a silly warning */
      }
    }

    list = list->next;
  }

  /* allocate memory for the output object pointers */
  output->sections = (struct section *)
                        malloc(num_sections * sizeof(object->sections));
  output->symtbl = (struct syment *)
                      malloc(num_symbols * sizeof(object->symtbl));
  output->strtbl = (char *)malloc(strtbl_size);

  output->file_header.f_flags = F_EXEC;  

  list = store;

  /* combine the objects */
  while (list != NULL) {
    object = list->object;
    assert(object != NULL);

    /* copy the symbol pointers */
    for (i = 0; i < object->file_header.f_nsyms; i++) {
      output->symtbl[output->file_header.f_nsyms] = object->symtbl[i];
      output->file_header.f_nsyms++;
    }
    
    /* copy the string table */
    strtbl_size = get_32((unsigned char *)&output->strtbl[0]);
    memcpy(&output->strtbl[strtbl_size], 
           &object->strtbl[4], 
	   get_32((unsigned char *)&object->strtbl[0]));


    for (i = 0; i < object->file_header.f_nscns; i++) {
      section = &object->sections[i];

      gp_fetch_section_name(object, &section->header, &name[0]);
      sym = get_symbol(section_names, &name[0]);
      assert(sym != NULL);
      number = get_symbol_annotation(sym);      
      if (*number == 1) {
        int index;
	
	/* Only one instance of this section, add it */
        output->file_header.f_nscns++;
	index = output->file_header.f_nscns;
        gp_link_copy_section(&output->sections[index], section);
      } else {
        /* combine section */
      
      }
    }

    list = list->next;
  }

  return output;
}

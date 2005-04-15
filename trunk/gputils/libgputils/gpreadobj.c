/* Read coff objects 
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

gp_coff_type 
gp_identify_coff_file(char *filename)
{
  FILE *file;
  char magic[SARMAG + 1];
  
  if ( (file = fopen(filename,"rb")) == NULL ) {
    return sys_err_file;
  }

  /* Read the magic number. Archive magic numbers are longest, so read
     their size */
  fread(&magic[0], 1, SARMAG, file);
  fclose(file); 

  if (((magic[1]<<8) + magic[0]) == MICROCHIP_MAGIC)
    return object_file;
    
  if (strncmp(magic, ARMAG, SARMAG) == 0)
    return archive_file;

  return unknown_file;
}

/* Read a binary file and store it in memory. */

gp_binary_type *
gp_read_file(char *filename)
{
  FILE *infile;
  gp_binary_type *file;

  infile = fopen(filename,"rb");
  if (infile == NULL) {
    perror(filename);
    exit(1);
  }
  
  file = (gp_binary_type *)malloc(sizeof(*file));

  /* determine the size of the file */
  fseek(infile, 0, SEEK_END);
  file->size = ftell(infile);
  rewind(infile); 

  /* read the object file into memory */
  file->file = (char *)malloc(file->size);
  fread(file->file, 1, file->size, infile); 

  fclose(infile);

  return file;
} 

/* free a binary file. */

void
gp_free_file(gp_binary_type *file)
{
  if (file == NULL)
    return;
    
  free(file->file);
  free(file);
  
  return;
} 

static void 
_read_file_header(gp_object_type *object, char *file)
{

  if (gp_getl16(&file[0]) != MICROCHIP_MAGIC)
    gp_error("invalid magic number in \"%s\"", object->filename);

  object->num_sections = gp_getl16(&file[2]);
  object->time         = gp_getl32(&file[4]);
  object->symbol_ptr   = gp_getl32(&file[8]);
  object->num_symbols  = gp_getl32(&file[12]);
  
  if (gp_getl16(&file[16]) != OPT_HDR_SIZ)
    gp_error("incorrect optional header size in \"%s\"", object->filename);
    
  object->flags = gp_getl16(&file[18]);

}

static void
_read_opt_header(gp_object_type *object, char *file)
{
 
  if (gp_getl16(&file[0]) != OPTMAGIC)
    gp_error("invalid optional magic number in \"%s\"", object->filename);
  
  if (gp_getl16(&file[2]) != 1)
    gp_error("invalid assembler version in \"%s\"", object->filename);
  
  object->processor = gp_processor_coff_proc(gp_getl32(&file[4]));
  if (object->processor == no_processor)
    gp_error("invalid processor type %#04x in \"%s\"",
             gp_getl32(&file[4]),
             object->filename);

  object->class = gp_processor_class(object->processor);
  
  if (gp_processor_rom_width(object->class) != gp_getl32(&file[8]))
    gp_error("invalid rom width for selected processor in \"%s\"", 
             object->filename);
  
  if (gp_getl32(&file[12]) != 8)
    gp_error("invalid ram width in \"%s\"", object->filename);

}

static void 
_read_section_header(gp_object_type *object,
                     gp_section_type *section, 
                     char *file, 
                     char *string_table)
{
  char buffer[9];
  unsigned int offset;

  if (gp_getl32(&file[0]) == 0) {
    /* read name from the string table */
    offset = gp_getl32(&file[4]);
    section->name = strdup(&string_table[offset]);
  } else {
    strncpy(buffer, &file[0], 8);
    /* the name can occupy all 8 chars without a null terminator */
    buffer[8] = '\0';
    section->name = strdup(buffer);
  }

  section->symbol = gp_coffgen_findsymbol(object, section->name);

  section->address = gp_getl32(&file[8]);
  if (section->address != gp_getl32(&file[12]))
    gp_error("virtual address does not equal physical address in \"%s\"", 
             object->filename);
  
  section->size       = gp_getl32(&file[16]);
  section->data_ptr   = gp_getl32(&file[20]);
  section->reloc_ptr  = gp_getl32(&file[24]);
  section->lineno_ptr = gp_getl32(&file[28]);
  section->num_reloc  = gp_getl16(&file[32]);
  section->num_lineno = gp_getl16(&file[34]);
  section->flags      = gp_getl32(&file[36]); 

  section->data = i_memory_create();
  section->relocations = NULL;
  section->relocations_tail = NULL;
  section->line_numbers = NULL;
  section->line_numbers_tail = NULL;
}

static void 
_read_reloc(gp_object_type *object,
            gp_section_type *section,
            gp_reloc_type *relocation,
            char *file)
{
  relocation->address = gp_getl32(&file[0]);
  relocation->symbol  = &object->symbols[gp_getl32(&file[4])];
  relocation->offset  = gp_getl16(&file[8]);
  relocation->type    = gp_getl16(&file[10]);

  if (relocation->address > section->size)
    gp_error("relocation at address %#x in section \"%s\" of \"%s\" exceeds the section size", 
             relocation->address,
             section->name,
             object->filename);

}

static void 
_read_lineno(gp_object_type *object,
             gp_linenum_type *line_number,
             char *file)
{

  line_number->symbol      = &object->symbols[gp_getl32(&file[0])];
  line_number->line_number = gp_getl16(&file[4]);
  line_number->address     = gp_getl32(&file[6]); 

  /* FIXME: function index and flags are unused, so far.
  line_number->l_flags  = gp_getl16(&file[10]);
  line_number->l_fcnndx = gp_getl32(&file[12]); 
  */

}

static void 
_read_sections(gp_object_type *object, char *file)
{
  int i, j;
  char *section_ptr;
  char *string_table;
  char *loc;
  gp_section_type *current = NULL;
  gp_reloc_type *current_reloc = NULL;
  gp_linenum_type *current_linenum = NULL;
  unsigned int number;
  unsigned int value;
  int org;

  /* move to the start of the section headers */
  section_ptr = file + FILE_HDR_SIZ + OPT_HDR_SIZ;

  /* setup pointer to string table */
  string_table = &file[object->symbol_ptr + 
                 (SYMBOL_SIZE * object->num_symbols)];

  object->sections = gp_coffgen_blocksec(object->num_sections);
  current = object->sections;

  for (i = 0; i < object->num_sections; i++) {
    _read_section_header(object, current, section_ptr, string_table);

    current->number = i + 1;
    section_ptr += SEC_HDR_SIZ;

    /* read the data */
    if ((current->size) && (current->data_ptr)) {
      if ((object->class == PROC_CLASS_PIC16E) && 
          ((current->flags & STYP_TEXT) ||
           (current->flags & STYP_DATA_ROM)))
        org = current->address >> 1;
      else
        org = current->address;

      loc = &file[current->data_ptr];
      if ((current->flags & STYP_TEXT) || (current->flags & STYP_DATA_ROM)) {
        /* size is in bytes, but words are stored in memory */
        number = current->size / 2;
      } else {
        number = current->size;
      }
      for (j = 0; j < number; j++) {
        if ((current->flags & STYP_TEXT) || (current->flags & STYP_DATA_ROM)) {
          value = (unsigned int)gp_getl16(&loc[j * 2]);
        } else {
          value = (unsigned int)loc[j];
        }        
        i_memory_put(current->data, org + j, MEM_USED_MASK | value);
      }
    }

    /* read the relocations */
    if ((current->num_reloc) && (current->reloc_ptr)) {
      loc = &file[current->reloc_ptr];
      number = current->num_reloc;
      current->relocations = gp_coffgen_blockrel(number);
      current_reloc = current->relocations;
      for (j = 0; j < number; j++) {
        _read_reloc(object, current, current_reloc, loc);
        loc += RELOC_SIZ;
        current->relocations_tail = current_reloc;
        current_reloc = current_reloc->next;
      }
    }

    /* read the line numbers */
    if ((current->num_lineno) && (current->lineno_ptr)) {
      loc = &file[current->lineno_ptr];
      number = current->num_lineno;
      current->line_numbers = gp_coffgen_blockline(number);
      current_linenum = current->line_numbers;
      for (j = 0; j < number; j++) {
        _read_lineno(object, current_linenum, loc);
        loc += LINENO_SIZ;
        current->line_numbers_tail = current_linenum;
        current_linenum = current_linenum->next;
      }
    }

    object->sections_tail = current;
    current = current->next;
  }

}

static void
_read_aux(gp_aux_type *aux, int aux_type, char *file, char *string_table)
{
  
  aux->type = aux_type;

  switch (aux_type) {
    case AUX_DIRECT:
      aux->_aux_symbol._aux_direct.command = file[0];
      aux->_aux_symbol._aux_direct.string = 
        strdup(&string_table[gp_getl32(&file[4])]);
      break;
    case AUX_FILE:
      aux->_aux_symbol._aux_file.filename = 
        strdup(&string_table[gp_getl32(&file[0])]);
      aux->_aux_symbol._aux_file.line_number = gp_getl32(&file[4]);
      break;
    case AUX_IDENT:
      aux->_aux_symbol._aux_ident.string = 
        strdup(&string_table[gp_getl32(&file[0])]);
      break;
    case AUX_SCN:
      aux->_aux_symbol._aux_scn.length  = gp_getl32(&file[0]);
      aux->_aux_symbol._aux_scn.nreloc  = gp_getl16(&file[4]);
      aux->_aux_symbol._aux_scn.nlineno = gp_getl16(&file[6]);
      break;
    default:
      memcpy(&aux->_aux_symbol.data[0], file, SYMBOL_SIZE);
  }

}

static void 
_read_symbol(gp_symbol_type *symbol, char *file, char *string_table)
{
  char buffer[9];
  unsigned int offset;

  if (gp_getl32(&file[0]) == 0) {
    /* read name from the string table */
    offset = gp_getl32(&file[4]);
    symbol->name = strdup(&string_table[offset]);
  } else {
    strncpy(buffer, &file[0], 8);
    /* the name can occupy all 8 chars without a null terminator */
    buffer[8] = '\0';
    symbol->name = strdup(buffer);
  }

  symbol->value = gp_getl32(&file[8]);
  symbol->section_number = gp_getl16(&file[12]);
  symbol->type = gp_getl16(&file[14]);
  symbol->class = file[16];
  symbol->num_auxsym = file[17];

  symbol->section = NULL;
  symbol->aux_list = NULL;
}

static void 
_read_symtbl(gp_object_type *object, char *file)
{
  int i;
  int j;
  int number = object->num_symbols;
  int num_auxsym;
  int aux_type;
  gp_symbol_type *current = NULL; 
  gp_aux_type *current_aux = NULL; 
  char *string_table;

  if (number != 0) {
    /* create a block of symbols */
    object->symbols = gp_coffgen_blocksym(number);

    /* setup pointer to string table */
    string_table = &file[object->symbol_ptr + (SYMBOL_SIZE * number)];

    /* read the symbols */
    file = &file[object->symbol_ptr];
    current = object->symbols;
    for (i = 0; i < number; i++) {
      /* read the symbol */
      _read_symbol(current, file, string_table);
      current->number = i;
      num_auxsym = current->num_auxsym;
      file += SYMBOL_SIZE;

      if (num_auxsym != 0) {
        current->aux_list = gp_coffgen_blockaux(current->num_auxsym);
        current_aux = current->aux_list;
        aux_type = gp_determine_aux(current);
        
        /* read the aux symbols */
        for (j = 0; j < num_auxsym; j++) {
          _read_aux(current_aux, aux_type, file, string_table);
          current_aux = current_aux->next;
          file += SYMBOL_SIZE;
          i++;
        }

        /* advance the through the list */
        for (j = 0; j < num_auxsym; j++) {
          /* COFF places all symbols inluding auxiliary, in the symbol table.
             However, in memory, gputils attaches auxiliary symbols to their
             associated primary symbol.  When reading COFF, space is reserved
             for the auxiliary symbols but not used.  Later the space is 
             freed.  This simplifies assigning the pointer in the 
             relocations. */
          current = current->next;
        }

      }

      current = current->next;
    }
  }
}

/* remove space reserved for auxiliary entries, add section pointers, and 
   setup tail pointer */

static void 
_clean_symtbl(gp_object_type *object)
{
  gp_symbol_type *current = NULL; 
  gp_symbol_type *next_symbol = NULL; 
  gp_symbol_type *old_symbol = NULL; 
  unsigned int i;

  current = object->symbols;

  while (current != NULL) {
    /* assign section pointer, section numbers start at 1 not 0 */
    if (current->section_number > 0)
      current->section = &object->sections[current->section_number - 1];
    else
      current->section = NULL;
    
    if (current->num_auxsym != 0) {
      next_symbol = current->next;
      for (i = 0; i < current->num_auxsym; i++) {
        old_symbol = next_symbol;
        next_symbol = next_symbol->next;
        /* FIXME: Can't free the single symbols because they were allocated
           in blocks.  This won't be a problem once obstacks are used.
        free(old_symbol);
        */
      }
      current->next = next_symbol;
    }
    
    object->symbols_tail = current;
    current = current->next;
  }

}

gp_object_type *
gp_convert_file(char *filename, char *file)
{
  gp_object_type *object;

  /* initialize object file */
  object = gp_coffgen_init();
  object->filename = strdup(filename);

  /* read the object */
  _read_file_header(object, file);
  _read_opt_header(object, file + FILE_HDR_SIZ);
  _read_symtbl(object, file);
  _read_sections(object, file);
  _clean_symtbl(object);

  return object;
}

gp_object_type *
gp_read_coff(char *filename)
{
  gp_binary_type *file;
  gp_object_type *object;

  file = gp_read_file(filename);
  if (file == NULL)
    return NULL;

  object = gp_convert_file(filename, file->file);
  gp_free_file(file);

  return object;
}

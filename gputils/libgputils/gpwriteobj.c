/* Write coff objects 
   Copyright (C) 2003, 2004
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

/* String table offsets are 16 bits so this coff has a limit on the 
   maximum string table size. */
#define MAX_STRING_TABLE 0xffff

/* write the symbol or section name into the string table */

static int
_gp_coffgen_addstring(char *name, char *table)
{
  int nbytes;
  int offset;
  size_t sizeof_name = strlen(name) + 1;

  assert(name != NULL);

  /* read the number of bytes in the string table */
  offset = nbytes = gp_getl32(&table[0]);

  /* check the length against the max string table size */
  nbytes += sizeof_name;
  assert(!(nbytes > MAX_STRING_TABLE));

  /* copy the string to the table */
  memmove(&table[offset], name, sizeof_name);

  /* write the new byte count */
  gp_putl32(&table[0], nbytes);

  return offset;
}

static void
_gp_coffgen_addname(char *name, char *ptr, size_t sizeof_ptr, char *table)
{
  int length;
  int offset;

  if (name == NULL)
    return;
  
  length = strlen(name);

  if (length < 9) {
    /* The string will fit in the structure. */
    strncpy(ptr, name, sizeof_ptr);
  } else {
    offset = _gp_coffgen_addstring(name, table);
 
    /* write zeros and offset */
    gp_putl32(&ptr[0], 0);
    gp_putl32(&ptr[4], offset);

  }

  return;
}

/* write the file header */
static void 
_gp_coffgen_write_filehdr(gp_object_type *object, FILE *fp) 
{
  gp_fputl16(MICROCHIP_MAGIC, fp);
  gp_fputl16(object->num_sections, fp);
  gp_fputl32(object->time, fp);
  gp_fputl32(object->symbol_ptr, fp);
  gp_fputl32(object->num_symbols, fp);
  gp_fputl16(OPT_HDR_SIZ, fp);
  gp_fputl16(object->flags, fp);

  return;
}

/* write the optional header */
static void 
_gp_coffgen_write_opthdr(gp_object_type *object, FILE *fp) 
{
  unsigned long coff_type = gp_processor_coff_type(object->processor);

  /* make sure we catch unfinished processors */
  assert(coff_type);

  /* write the data to file */
  gp_fputl16(OPTMAGIC, fp);
  gp_fputl16(1, fp);
  gp_fputl32(coff_type, fp);
  gp_fputl32(gp_processor_rom_width(object->class), fp);
  gp_fputl32(8, fp);

  return;
}

/* write the section header */
static void 
_gp_coffgen_write_scnhdr(gp_section_type *section, char *table, FILE *fp) 
{
  char name[8];
  
  _gp_coffgen_addname(section->name, name, sizeof(name), table);
  gp_fputvar(&name[0], 8, fp);
  gp_fputl32(section->address, fp);
  gp_fputl32(section->address, fp);
  gp_fputl32(section->size, fp);
  gp_fputl32(section->data_ptr, fp);
  gp_fputl32(section->reloc_ptr, fp);
  gp_fputl32(section->lineno_ptr, fp);
  gp_fputl16(section->num_reloc, fp);
  gp_fputl16(section->num_lineno, fp);
  gp_fputl32(section->flags, fp);

  return;
}

/* write the section data */
static void 
_gp_coffgen_write_data(enum proc_class class, 
                       gp_section_type *section, 
                       FILE *fp) 
{
  unsigned int org;
  unsigned int last;
  unsigned int data;

  if ((class == PROC_CLASS_PIC16E) && 
      ((section->flags & STYP_TEXT) ||
       (section->flags & STYP_DATA_ROM)))
    org = section->address >> 1;
  else
    org = section->address;

  if ((section->flags & STYP_TEXT) || (section->flags & STYP_DATA_ROM)) {
    /* the section is executable, so each word is two bytes */
    last = org + (section->size / 2);
  } else {
    /* the section is data, so each word is one byte */
    last = org + section->size;
  }

#ifdef GPUTILS_DEBUG
  printf("section \"%s\"\nsize= %i\ndata:\n", section->name, section->size);
  print_i_memory(section->data, class == PROC_CLASS_PIC16E ? 1 : 0);
#endif

  for ( ; org < last; org++) {
    data = i_memory_get(section->data, org);
    assert(data & MEM_USED_MASK);

    if ((section->flags & STYP_TEXT) || (section->flags & STYP_DATA_ROM)) {
      gp_fputl16(data & 0xffff, fp);
    } else {
      fputc((int)(data & 0xff), fp);
    }
  }

  return;
}

/* write the section relocations */
static void 
_gp_coffgen_write_reloc(gp_section_type *section, FILE *fp) 
{
  gp_reloc_type *current = section->relocations;

  while (current != NULL) {

    gp_fputl32(current->address, fp);
    gp_fputl32(current->symbol->number, fp);
    gp_fputl16(current->offset, fp);
    gp_fputl16(current->type, fp);
  
    current = current->next;
  }

  return;
}

/* write the section linenumbers */
static void 
_gp_coffgen_write_linenum(gp_section_type *section, FILE *fp) 
{
  gp_linenum_type *current = section->line_numbers;

  while (current != NULL) {

    gp_fputl32(current->symbol->number, fp);
    gp_fputl16(current->line_number, fp);
    gp_fputl32(current->address, fp); 
    gp_fputl16(0, fp);
    gp_fputl32(0, fp);

    current = current->next;
  }

  return;
}

/* write the auxiliary symbols */
static void 
_gp_coffgen_write_auxsymbols(gp_aux_type *aux, char *table, FILE *fp) 
{
  unsigned int offset;

  while(aux != NULL) {

    switch (aux->type) {
    case AUX_FILE:
      /* add the filename to the string table */
      offset = _gp_coffgen_addstring(aux->_aux_symbol._aux_file.filename, 
                                     table);
      gp_fputl32(offset, fp);
      gp_fputl32(aux->_aux_symbol._aux_file.line_number, fp);
      gp_fputl32(0, fp);
      gp_fputl32(0, fp);
      gp_fputl16(0, fp);
      break;
    case AUX_SCN:
      /* write section auxiliary symbol */
      gp_fputl32(aux->_aux_symbol._aux_scn.length, fp);
      gp_fputl16(aux->_aux_symbol._aux_scn.nreloc, fp);
      gp_fputl16(aux->_aux_symbol._aux_scn.nlineno, fp);
      gp_fputl32(0, fp);
      gp_fputl32(0, fp);
      gp_fputl16(0, fp);
      break;
    default:
      /* copy the data to the file */
      gp_fputvar(&aux->_aux_symbol.data[0], 18, fp);
    }
   
    aux = aux->next;
  }

  return;
}

/* write the symbol table */
static void 
_gp_coffgen_write_symbols(gp_object_type *object, char *table, FILE *fp) 
{
  gp_symbol_type *current;
  char name[8];

  current = object->symbols;

  while(current != NULL) {

    _gp_coffgen_addname(current->name, name, sizeof(name), table);

    gp_fputvar(&name[0], 8, fp);
    gp_fputl32(current->value, fp);
    if (current->section_number < 1) {
      gp_fputl16(current->section_number, fp);
    } else {
      gp_fputl16(current->section->number, fp);
    }    
    gp_fputl16(current->type, fp);
    fputc(current->class, fp);
    fputc(current->num_auxsym, fp);

    if (current->num_auxsym)
      _gp_coffgen_write_auxsymbols(current->aux_list, table, fp);
   
    current = current->next;
  }

  return;
}

int
_has_data(gp_section_type *section)
{

  if (section->size == 0)
    return 0;

  if (section->flags & STYP_TEXT)
    return 1;

  if (section->flags & STYP_DATA)
    return 1;

  if (section->flags & STYP_DATA_ROM)
    return 1;

  return 0;
}

/* update all the coff pointers */
static int
_gp_updateptr(gp_object_type *object)
{
  int loc = 0;
  gp_section_type *section = NULL;
  gp_symbol_type *symbol = NULL;
  int section_number = 1;
  int symbol_number = 0;

  loc = FILE_HDR_SIZ + OPT_HDR_SIZ + (SEC_HDR_SIZ * object->num_sections);

  /* update the data pointers in the section headers */
  section = object->sections;
  while (section != NULL) {
    section->number = section_number;
    section_number++;
    section->data_ptr = 0;
    if (_has_data(section)) {
      section->data_ptr = loc;
      loc += section->size;
    }
    section = section->next;
  }

  /* update the relocation pointers in the section headers */
  section = object->sections;
  while (section != NULL) {
    section->reloc_ptr  = 0;
    if (section->num_reloc != 0) {
      section->reloc_ptr = loc;
      loc += (section->num_reloc * RELOC_SIZ);
    }
    section = section->next;
  }

  /* update the line number pointers in the section headers */
  section = object->sections;
  while (section != NULL) {
    section->lineno_ptr = 0;
    if (section->num_lineno != 0) {
      section->lineno_ptr = loc;
      loc += (section->num_lineno * LINENO_SIZ);
    }    
    section = section->next;
  }

  /* update symbol table pointer */
  object->symbol_ptr = loc;

  /* update the symbol numbers */
  symbol = object->symbols;
  while (symbol != NULL) {
    symbol->number = symbol_number;
    symbol_number += 1 + symbol->num_auxsym; 
    symbol = symbol->next;
  }

  return 0;
}

/* write the coff file */
int
gp_write_coff(gp_object_type *object, int numerrors)
{
  FILE *coff;
  gp_section_type *section = NULL;
  char table[MAX_STRING_TABLE]; /* string table */

  if (numerrors) {
    unlink(object->filename);
    return 0;
  }
  
  coff = fopen(object->filename, "wb");
  if (coff == NULL) {
    perror(object->filename);
    exit(1);
  }

  /* update file pointers in the coff */
  _gp_updateptr(object);

  /* initialize the string table byte count */
  gp_putl32(&table[0], 4);

  /* write the data to the file */
  _gp_coffgen_write_filehdr(object, coff); 
  _gp_coffgen_write_opthdr(object, coff);

  section = object->sections;

  /* write section headers */
  while (section != NULL) {
    _gp_coffgen_write_scnhdr(section, &table[0], coff); 
    section = section->next;
  }

  /* write section data */
  section = object->sections;
  while (section != NULL) {
    if (_has_data(section)) {
      _gp_coffgen_write_data(object->class, section, coff); 
    }
    section = section->next;
  }

  /* write section relocations */
  section = object->sections;
  while (section != NULL) {
    if (section->num_reloc != 0) {
      _gp_coffgen_write_reloc(section, coff); 
    }
    section = section->next;
  }

  /* write section line numbers */
  section = object->sections;
  while (section != NULL) {
    if (section->num_lineno != 0) {
      _gp_coffgen_write_linenum(section, coff); 
    }    
    section = section->next;
  }

  /* write symbols */
  if (object->num_symbols != 0) {
    _gp_coffgen_write_symbols(object, &table[0], coff); 
  } 

  /* write string table */
  fwrite(&table[0], 1, gp_getl32(&table[0]), coff);
  
  fclose(coff);

  return 0;
}

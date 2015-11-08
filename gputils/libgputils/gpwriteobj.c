/* Write coff objects
   Copyright (C) 2003, 2004, 2005
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
_gp_coffgen_addstring(const char *name, unsigned char *table)
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
  memcpy(&table[offset], name, sizeof_name);

  /* write the new byte count */
  gp_putl32(&table[0], nbytes);

  return offset;
}

static void
_gp_coffgen_addname(const char *name, FILE *fp, unsigned char *table)
{
  int length;
  int offset;

  if (name == NULL) {
    return;
  }

  length = strlen(name);

  if (length < 9) {
    static const unsigned char zero[8];
    /* The string will fit in the structure. */
    if (length > 0) {
      gp_fputvar(name, length, fp);
    }

    if (length < 8) {
      gp_fputvar(zero, 8 - length, fp);
    }
  } else {
    offset = _gp_coffgen_addstring(name, table);

    /* write zeros and offset */
    gp_fputl32(0, fp);
    gp_fputl32(offset, fp);
  }
}

/* write the file header */
static void
_gp_coffgen_write_filehdr(const gp_object_type *object, FILE *fp)
{
  gp_fputl16((object->isnew ? MICROCHIP_MAGIC_v2 : MICROCHIP_MAGIC_v1), fp);
  gp_fputl16(object->num_sections, fp);
  gp_fputl32(object->time, fp);
  gp_fputl32(object->symbol_ptr, fp);
  gp_fputl32(object->num_symbols, fp);
  gp_fputl16((object->isnew ? OPT_HDR_SIZ_v2: OPT_HDR_SIZ_v1), fp);
  gp_fputl16(object->flags, fp);
}

/* write the optional header */
static void
_gp_coffgen_write_opthdr(const gp_object_type *object, FILE *fp)
{
  unsigned long coff_type = gp_processor_coff_type(object->processor);

  /* make sure we catch unfinished processors */
  assert(coff_type);

  /* write the data to file */
  gp_fputl16(object->isnew ? OPTMAGIC_v2 : OPTMAGIC_v1, fp);

  if (object->isnew) {
    gp_fputl32(1, fp);
  }
  else {
    gp_fputl16(1, fp);
  }

  gp_fputl32(coff_type, fp);
  gp_fputl32(gp_processor_rom_width(object->class), fp);
  gp_fputl32(8, fp);
}

/* write the section header */
static void
_gp_coffgen_write_scnhdr(const gp_section_type *section, int org_to_byte_shift,
                         unsigned char *table, FILE *fp)
{
  if (!(section->flags & (STYP_TEXT | STYP_DATA_ROM))) {
    org_to_byte_shift = 0;
  }

  _gp_coffgen_addname(section->name, fp, table);
  gp_fputl32(gp_byte_to_org(org_to_byte_shift, section->address), fp);
  gp_fputl32(gp_byte_to_org(org_to_byte_shift, section->address), fp);
  gp_fputl32(section->size, fp);
  gp_fputl32(section->data_ptr, fp);
  gp_fputl32(section->reloc_ptr, fp);
  gp_fputl32(section->lineno_ptr, fp);
  gp_fputl16(section->num_reloc, fp);
  gp_fputl16(section->num_lineno, fp);
  /* Don't write internal section flags. */
  gp_fputl32(section->flags & ~(STYP_RELOC | STYP_BPACK), fp);
}

/* write the section data */
static void
_gp_coffgen_write_data(proc_class_t class,
                       const gp_section_type *section,
                       FILE *fp)
{
  unsigned int org;
  unsigned int last;

  org = section->shadow_address;
  last = org + section->size;

#ifdef GPUTILS_DEBUG
  printf("section \"%s\"\nsize= %i\ndata:\n", section->name, section->size);
  print_i_memory(section->data, class);
#endif

  for ( ; org < last; org++) {
    unsigned char b;

    b_memory_assert_get(section->data, org, &b, NULL, NULL);
    fputc(b, fp);
  }
}

/* write the section relocations */
static void
_gp_coffgen_write_reloc(const gp_section_type *section, FILE *fp)
{
  gp_reloc_type *current = section->relocations;

  while (current != NULL) {
    gp_fputl32(current->address, fp);
    gp_fputl32(current->symbol->number, fp);
    gp_fputl16(current->offset, fp);
    gp_fputl16(current->type, fp);

    current = current->next;
  }
}

/* write the section linenumbers */
static void
_gp_coffgen_write_linenum(const gp_section_type *section, int org_to_byte_shift, FILE *fp)
{
  gp_linenum_type *current = section->line_numbers;

  if (!(section->flags & (STYP_TEXT | STYP_DATA_ROM))) {
    org_to_byte_shift = 0;
  }

  while (current != NULL) {
    gp_fputl32(current->symbol->number, fp);
    gp_fputl16(current->line_number, fp);
    gp_fputl32(gp_byte_to_org(org_to_byte_shift, current->address), fp);
    gp_fputl16(0, fp);
    gp_fputl32(0, fp);

    current = current->next;
  }
}

/* write the auxiliary symbols */
static void
_gp_coffgen_write_auxsymbols(const gp_aux_type *aux, unsigned char *table, FILE *fp,
                             int isnew)
{
  unsigned int offset;

  while (aux != NULL) {
    switch (aux->type) {
    case AUX_DIRECT:
      /* add the direct string to the string table */
      offset = _gp_coffgen_addstring(aux->_aux_symbol._aux_direct.string, table);
      gp_fputl32(aux->_aux_symbol._aux_direct.command, fp);
      gp_fputl32(offset, fp);
      gp_fputl32(0, fp);
      gp_fputl32(0, fp);
      if (isnew) gp_fputl32(0, fp); else gp_fputl16(0, fp);
      break;

    case AUX_FILE:
      /* add the filename to the string table */
      offset = _gp_coffgen_addstring(aux->_aux_symbol._aux_file.filename, table);
      gp_fputl32(offset, fp);
      gp_fputl32(aux->_aux_symbol._aux_file.line_number, fp);
      fputc(aux->_aux_symbol._aux_file.flags, fp);
      gp_fputvar("\0\0\0\0\0\0", 7, fp);
      if (isnew) gp_fputl32(0, fp); else gp_fputl16(0, fp);
      break;

    case AUX_IDENT:
      /* add the ident string to the string table */
      offset = _gp_coffgen_addstring(aux->_aux_symbol._aux_ident.string, table);
      gp_fputl32(offset, fp);
      gp_fputl32(0, fp);
      gp_fputl32(0, fp);
      gp_fputl32(0, fp);
      if (isnew) gp_fputl32(0, fp); else gp_fputl16(0, fp);
      break;

    case AUX_SCN:
      /* write section auxiliary symbol */
      gp_fputl32(aux->_aux_symbol._aux_scn.length, fp);
      gp_fputl16(aux->_aux_symbol._aux_scn.nreloc, fp);
      gp_fputl16(aux->_aux_symbol._aux_scn.nlineno, fp);
      gp_fputl32(0, fp);
      gp_fputl32(0, fp);
      if (isnew) gp_fputl32(0, fp); else gp_fputl16(0, fp);
      break;

    default:
      /* copy the data to the file */
      gp_fputvar(&aux->_aux_symbol.data[0], ((isnew) ? SYMBOL_SIZE_v2 : SYMBOL_SIZE_v1), fp);
    }

    aux = aux->next;
  }
}

/* write the symbol table */
static void
_gp_coffgen_write_symbols(const gp_object_type *object, unsigned char *table, FILE *fp)
{
  gp_symbol_type *current;

  current = object->symbols;

  while (current != NULL) {
    _gp_coffgen_addname(current->name, fp, table);
    gp_fputl32(current->value, fp);

    if (current->section_number < 1) {
      gp_fputl16(current->section_number, fp);
    } else {
      gp_fputl16(current->section->number, fp);
    }

    if (object->isnew) {
      gp_fputl32(current->type | (current->derived_type << 5), fp);
    }
    else {
      gp_fputl16(current->type | (current->derived_type << 4), fp);
    }

    fputc(current->class, fp);
    fputc(current->num_auxsym, fp);

    if (current->num_auxsym > 0) {
      _gp_coffgen_write_auxsymbols(current->aux_list, table, fp, object->isnew);
    }

    current = current->next;
  }
}

gp_boolean
_has_data(const gp_section_type *section)
{

  if (section->size == 0) {
    return false;
  }

  if (section->flags & STYP_TEXT) {
    return true;
  }

  if (section->flags & STYP_DATA) {
    return true;
  }

  if (section->flags & STYP_DATA_ROM) {
    return true;
  }

  return false;
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

  loc = ((object->isnew) ?
         (FILE_HDR_SIZ_v2 + OPT_HDR_SIZ_v2 + (SEC_HDR_SIZ_v2 * object->num_sections)) :
         (FILE_HDR_SIZ_v1 + OPT_HDR_SIZ_v1 + (SEC_HDR_SIZ_v1 * object->num_sections)));

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
  unsigned char table[MAX_STRING_TABLE]; /* string table */

  if (numerrors > 0) {
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
    _gp_coffgen_write_scnhdr(section, object->class->org_to_byte_shift,
                             &table[0], coff);
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
      _gp_coffgen_write_linenum(section, object->class->org_to_byte_shift, coff);
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

/* check if the object is absolute: all sections are absolute and there
   are no relocations (undefined symbols) */
gp_boolean
gp_is_absolute_object(const gp_object_type *object)
{
  gp_section_type *section;

  for (section = object->sections; section; section = section->next) {
    if ((section->num_reloc != 0) || !(section->flags & STYP_ABS)) {
      return false;
    }
  }

  return true;
}

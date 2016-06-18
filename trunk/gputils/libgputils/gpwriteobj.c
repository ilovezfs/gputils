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
#define MAX_STRING_TABLE      0xffff

/*------------------------------------------------------------------------------------------------*/

/* write the symbol or section name into the string table */

static unsigned int
_add_string(const char *name, uint8_t *table)
{
  uint32_t nbytes;
  uint32_t offset;
  uint32_t sizeof_name;

  assert(name != NULL);

  sizeof_name = strlen(name) + 1;

  /* read the number of bytes in the string table */
  offset = nbytes = gp_getl32(&table[0]);

  /* check the length against the max string table size */
  nbytes += sizeof_name;
  assert(nbytes <= MAX_STRING_TABLE);

  /* copy the string to the table */
  memcpy(&table[offset], name, sizeof_name);

  /* write the new byte count */
  gp_putl32(&table[0], nbytes);

  return offset;
}

/*------------------------------------------------------------------------------------------------*/

static void
_add_name(const char *name, FILE *fp, uint8_t *table)
{
  static const uint8_t zero[8] = { 0, };

  uint32_t             length;
  uint32_t             offset;

  if (name == NULL) {
    return;
  }

  length = strlen(name);

  if (length <= sizeof(zero)) {
    /* The string will fit in the structure. */
    if (length > 0) {
      gp_fputvar(name, length, fp);     /* section name if less then 8 characters */
    }

    if (length < sizeof(zero)) {
      gp_fputvar(zero, sizeof(zero) - length, fp);
    }
  } else {
    offset = _add_string(name, table);

    /* write zeros and offset */
    gp_fputl32(0, fp);                  /* first four characters are 0 */
    gp_fputl32(offset, fp);             /* pointer to the string table */
  }
}

/*------------------------------------------------------------------------------------------------*/

/* write the file header */

static void
_write_filehdr(const gp_object_type *object, FILE *fp)
{
  /* 'f_magic'  -- magic number */
  gp_fputl16((object->isnew ? MICROCHIP_MAGIC_v2 : MICROCHIP_MAGIC_v1), fp);
  /* 'f_nscns'  -- number of sections */
  gp_fputl16(object->num_sections, fp);
  /* 'f_timdat' -- time and date stamp */
  gp_fputl32(object->time, fp);
  /* 'f_symptr' -- file ptr to symtab */
  gp_fputl32(object->symbol_ptr, fp);
  /* 'f_nsyms'  -- # symtab entries */
  gp_fputl32(object->num_symbols, fp);
  /* 'f_opthdr' -- sizeof(opt hdr) */
  gp_fputl16((object->isnew ? OPT_HDR_SIZ_v2: OPT_HDR_SIZ_v1), fp);
  /* 'f_flags'  -- flags */
  gp_fputl16(object->flags, fp);
}

/*------------------------------------------------------------------------------------------------*/

/* write the optional header */

static void
_write_opthdr(const gp_object_type *object, FILE *fp)
{
  uint32_t coff_type = gp_processor_coff_type(object->processor);

  /* make sure we catch unfinished processors */
  assert(coff_type != 0);

  /* write the data to file */
  /* 'opt_magic' */
  gp_fputl16(object->isnew ? OPTMAGIC_v2 : OPTMAGIC_v1, fp);

  /* 'vstamp' -- version stamp of compiler */
  if (object->isnew) {
    gp_fputl32(1, fp);
  }
  else {
    gp_fputl16(1, fp);
  }

  /* 'proc_type'      -- processor type */
  gp_fputl32(coff_type, fp);
  /* 'rom_width_bits' -- ROM width bits */
  gp_fputl32(gp_processor_rom_width(object->class), fp);
  /* 'ram_width_bits' -- RAM width bits */
  gp_fputl32(8, fp);
}

/*------------------------------------------------------------------------------------------------*/

/* write the section header */

static void
_write_scnhdr(const gp_section_type *section, int org_to_byte_shift, uint8_t *table, FILE *fp)
{
  uint32_t section_address;

  if ((section->flags & (STYP_TEXT | STYP_DATA_ROM)) == 0) {
    org_to_byte_shift = 0;
  }

  section_address = gp_byte_to_org(org_to_byte_shift, section->address);
  _add_name(section->name, fp, table);
  /* 's_paddr'   -- physical address */
  gp_fputl32(section_address, fp);
  /* 's_vaddr'   -- virtual address */
  gp_fputl32(section_address, fp);
  /* 's_size'    -- section size */
  gp_fputl32(section->size, fp);
  /* 's_scnptr'  -- file ptr to raw data */
  gp_fputl32(section->data_ptr, fp);
  /* 's_relptr'  -- file ptr to relocation */
  gp_fputl32(section->reloc_ptr, fp);
  /* 's_lnnoptr' -- file ptr to line numbers */
  gp_fputl32(section->lineno_ptr, fp);
  /* 's_nreloc'  -- # reloc entries */
  gp_fputl16(section->num_reloc, fp);
  /* 's_nlnno'   -- # line number entries */
  gp_fputl16(section->num_lineno, fp);
  /* 's_flags'   -- Don't write internal section flags. */
  gp_fputl32(section->flags & ~(STYP_RELOC | STYP_BPACK), fp);
}

/*------------------------------------------------------------------------------------------------*/

/* write the section data */

static void
_write_data(pic_processor_t processor, const gp_section_type *section, FILE *fp)
{
  unsigned int org;
  unsigned int end;
  uint8_t      b;

  org = section->shadow_address;
  end = org + section->size;

#ifdef GPUTILS_DEBUG
  printf("section \"%s\"\nsize= %li\ndata:\n", section->name, section->size);
  i_memory_print(section->data, processor);
#endif

  for ( ; org < end; org++) {
    b_memory_assert_get(section->data, org, &b, NULL, NULL);
    fputc(b, fp);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* write the section relocations */

static void
_write_reloc(const gp_section_type *section, FILE *fp)
{
  gp_reloc_type *current = section->relocation_list;

  while (current != NULL) {
    /* 'r_vaddr'  -- entry relative virtual address */
    gp_fputl32(current->address, fp);
    /* 'r_symndx' -- index into symbol table */
    gp_fputl32(current->symbol->number, fp);
    /* 'r_offset' -- offset to be added to address of symbol 'r_symndx' */
    gp_fputl16(current->offset, fp);
    /* 'r_type'   -- relocation type */
    gp_fputl16(current->type, fp);

    current = current->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* write the section linenumbers */

static void
_write_linenum(const gp_section_type *section, int org_to_byte_shift, FILE *fp)
{
  gp_linenum_type *current = section->line_number_list;

  if (!(section->flags & (STYP_TEXT | STYP_DATA_ROM))) {
    org_to_byte_shift = 0;
  }

  while (current != NULL) {
    /* 'l_srcndx' -- symbol table index of associated source file */
    gp_fputl32(current->symbol->number, fp);
    /* 'l_lnno'   -- line number */
    gp_fputl16(current->line_number, fp);
    /* 'l_paddr'  -- address of code for this lineno */
    gp_fputl32(gp_byte_to_org(org_to_byte_shift, current->address), fp);
    /* 'l_flags'  -- bit flags for the line number */
    gp_fputl16(0, fp);
    /* 'l_fcnndx' -- symbol table index of associated function, if there is one */
    gp_fputl32(0, fp);

    current = current->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* write the auxiliary symbols */

static void
_write_auxsymbols(const gp_aux_type *aux, uint8_t *table, FILE *fp, gp_boolean isnew)
{
  unsigned int offset;

  while (aux != NULL) {
    switch (aux->type) {
    case AUX_DIRECT:
      /* add the direct string to the string table */
      offset = _add_string(aux->_aux_symbol._aux_direct.string, table);
      gp_fputl32(aux->_aux_symbol._aux_direct.command, fp);
      gp_fputl32(offset, fp);
      gp_fputl32(0, fp);
      gp_fputl32(0, fp);

      if (isnew) {
        gp_fputl32(0, fp);
      }
      else {
        gp_fputl16(0, fp);
      }
      break;

    case AUX_FILE:
      /* add the filename to the string table */
      offset = _add_string(aux->_aux_symbol._aux_file.filename, table);
      gp_fputl32(offset, fp);
      gp_fputl32(aux->_aux_symbol._aux_file.line_number, fp);
      fputc(aux->_aux_symbol._aux_file.flags, fp);
      gp_fputvar("\0\0\0\0\0\0", 7, fp);

      if (isnew) {
        gp_fputl32(0, fp);
      }
      else {
        gp_fputl16(0, fp);
      }
      break;

    case AUX_IDENT:
      /* add the ident string to the string table */
      offset = _add_string(aux->_aux_symbol._aux_ident.string, table);
      gp_fputl32(offset, fp);
      gp_fputl32(0, fp);
      gp_fputl32(0, fp);
      gp_fputl32(0, fp);

      if (isnew) {
        gp_fputl32(0, fp);
      }
      else {
        gp_fputl16(0, fp);
      }
      break;

    case AUX_SCN:
      /* write section auxiliary symbol */
      gp_fputl32(aux->_aux_symbol._aux_scn.length, fp);
      gp_fputl16(aux->_aux_symbol._aux_scn.nreloc, fp);
      gp_fputl16(aux->_aux_symbol._aux_scn.nlineno, fp);
      gp_fputl32(0, fp);
      gp_fputl32(0, fp);

      if (isnew) {
        gp_fputl32(0, fp);
      }
      else {
        gp_fputl16(0, fp);
      }
      break;

    default:
      /* copy the data to the file */
      gp_fputvar(&aux->_aux_symbol.data[0], ((isnew) ? SYMBOL_SIZE_v2 : SYMBOL_SIZE_v1), fp);
    }

    aux = aux->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* write the symbol table */

static void
_write_symbols(const gp_object_type *object, uint8_t *table, FILE *fp)
{
  gp_symbol_type *current;

  current = object->symbol_list;

  while (current != NULL) {
    _add_name(current->name, fp, table);
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
      _write_auxsymbols(current->aux_list, table, fp, object->isnew);
    }

    current = current->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* update all the coff pointers */

static void
_updateptr(gp_object_type *object)
{
  int              loc;
  gp_section_type *section;
  gp_symbol_type  *symbol;
  int              section_number;
  int              symbol_number;

  loc = (object->isnew) ? (FILE_HDR_SIZ_v2 + OPT_HDR_SIZ_v2 + (SEC_HDR_SIZ_v2 * object->num_sections)) :
                          (FILE_HDR_SIZ_v1 + OPT_HDR_SIZ_v1 + (SEC_HDR_SIZ_v1 * object->num_sections));

  section_number = 1;

  /* update the data pointers in the section headers */
  section = object->section_list;
  while (section != NULL) {
    section->number = section_number;
    section_number++;
    section->data_ptr = 0;

    if (gp_has_data(section)) {
      section->data_ptr = loc;
      loc += section->size;
    }
    section = section->next;
  }

  /* update the relocation pointers in the section headers */
  section = object->section_list;
  while (section != NULL) {
    section->reloc_ptr  = 0;

    if (section->num_reloc != 0) {
      section->reloc_ptr = loc;
      loc += (section->num_reloc * RELOC_SIZ);
    }
    section = section->next;
  }

  /* update the line number pointers in the section headers */
  section = object->section_list;
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
  symbol_number = 0;
  symbol        = object->symbol_list;
  while (symbol != NULL) {
    symbol->number = symbol_number;
    symbol_number += 1 + symbol->num_auxsym;
    symbol = symbol->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_has_data(const gp_section_type *section)
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

/*------------------------------------------------------------------------------------------------*/

/* write the coff file */

gp_boolean
gp_write_coff(gp_object_type *object, int numerrors)
{
  FILE            *coff;
  unsigned int     org_to_byte_shift;
  gp_section_type *section;
  uint8_t          table[MAX_STRING_TABLE]; /* string table */

  if (numerrors > 0) {
    unlink(object->filename);
    return false;
  }

  coff = fopen(object->filename, "wb");
  if (coff == NULL) {
    perror(object->filename);
    return false;
  }

  org_to_byte_shift = object->class->org_to_byte_shift;

  /* update file pointers in the coff */
  _updateptr(object);

  /* initialize the string table byte count */
  gp_putl32(&table[0], 4);

  /* write the data to the file */
  _write_filehdr(object, coff);
  _write_opthdr(object, coff);

  /* write section headers */
  section = object->section_list;
  while (section != NULL) {
    _write_scnhdr(section, org_to_byte_shift, &table[0], coff);
    section = section->next;
  }

  /* write section data */
  section = object->section_list;
  while (section != NULL) {
    if (gp_has_data(section)) {
      _write_data(object->processor, section, coff);
    }
    section = section->next;
  }

  /* write section relocations */
  section = object->section_list;
  while (section != NULL) {
    if (section->num_reloc != 0) {
      _write_reloc(section, coff);
    }
    section = section->next;
  }

  /* write section line numbers */
  section = object->section_list;
  while (section != NULL) {
    if (section->num_lineno != 0) {
      _write_linenum(section, org_to_byte_shift, coff);
    }
    section = section->next;
  }

  /* write symbols */
  if (object->num_symbols != 0) {
    _write_symbols(object, &table[0], coff);
  }

  /* write string table */
  fwrite(&table[0], 1, gp_getl32(&table[0]), coff);

  fclose(coff);
  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* check if the object is absolute: all sections are absolute and there
   are no relocations (undefined symbols) */

gp_boolean
gp_is_absolute_object(const gp_object_type *object)
{
  gp_section_type *section;

  for (section = object->section_list; section != NULL; section = section->next) {
    if ((section->num_reloc != 0) || !(section->flags & STYP_ABS)) {
      return false;
    }
  }

  return true;
}

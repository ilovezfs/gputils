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
gp_identify_coff_file(const char *filename)
{
  FILE *file;
  char magic[SARMAG + 1];

  if ((file = fopen(filename, "rb")) == NULL) {
    return sys_err_file;
  }

  /* Read the magic number. Archive magic numbers are longest, so read
     their size */
  fread(&magic[0], 1, SARMAG, file);
  fclose(file);

  if (((magic[1]<<8) + magic[0]) == MICROCHIP_MAGIC_v1)
    return object_file;
  if (((magic[1]<<8) + magic[0]) == MICROCHIP_MAGIC_v2)
    return object_file_v2;

  if (strncmp(magic, ARMAG, SARMAG) == 0)
    return archive_file;

  return unknown_file;
}

/* Read a binary file and store it in memory. */

gp_binary_type *
gp_read_file(const char *filename)
{
  FILE *infile;
  gp_binary_type *file;
  struct stat statbuf;

  infile = fopen(filename,"rb");
  if (infile == NULL) {
    perror(filename);
    exit(1);
  }

  file = (gp_binary_type *)malloc(sizeof(*file));

  /* determine the size of the file */
  fstat(fileno(infile), &statbuf);
  file->size = statbuf.st_size;

  /* read the object file into memory */
  file->file = (unsigned char *)malloc(file->size);
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

/* Return optional header size (0 for no header) */
static int
_read_file_header(gp_object_type *object, const unsigned char *file)
{
  int isnew = 0;
  int opt_hdr;

  if (gp_getl16(&file[0]) == MICROCHIP_MAGIC_v2)
    isnew = 1;
  else if (gp_getl16(&file[0]) != MICROCHIP_MAGIC_v1)
    gp_error("invalid magic number in \"%s\"", object->filename);

  object->isnew = isnew;
  object->version      = gp_getl16(&file[0]);
  object->num_sections = gp_getl16(&file[2]);
  object->time         = gp_getl32(&file[4]);
  object->symbol_ptr   = gp_getl32(&file[8]);
  object->num_symbols  = gp_getl32(&file[12]);

  opt_hdr = gp_getl16(&file[16]);
  if (opt_hdr != 0 && opt_hdr != (isnew ? OPT_HDR_SIZ_v2 : OPT_HDR_SIZ_v1) )
    gp_error("incorrect optional header size (%d) in \"%s\"", opt_hdr, object->filename);

  object->symbol_size = (object->version == MICROCHIP_MAGIC_v1 ?
                         SYMBOL_SIZE_v1 : SYMBOL_SIZE_v2);
  object->flags = gp_getl16(&file[18]);

  return opt_hdr;
}

static void
_read_opt_header(gp_object_type *object, const unsigned char *file)
{
  unsigned short optmagic;
  unsigned long vstamp, proc_code, rom_width, ram_width;
  size_t offset = 0;

  optmagic = gp_getl16(&file[0]);
  if (optmagic != (object->isnew ? OPTMAGIC_v2 : OPTMAGIC_v1))
    gp_error("invalid optional magic number (%#04x) in \"%s\"", optmagic, object->filename);

  offset = 2;
  if (object->isnew) {
    vstamp = gp_getl32(&file[offset]);
    offset += 4;
  } else {
    vstamp = gp_getl16(&file[offset]);
    offset += 2;
  }

  if (!object->isnew && vstamp != 1)
    gp_error("invalid assembler version (%ld) in \"%s\"", vstamp, object->filename);

  proc_code = gp_getl32(&file[offset]);
  offset += 4;

  rom_width = gp_getl32(&file[offset]);
  offset += 4;

  object->processor = gp_processor_coff_proc(proc_code);
  if (object->processor == no_processor) {
    /* Fallback to a generic processor of matching rom width */
    switch(rom_width) {
    case 8: object->processor = gp_find_processor("pic18cxx"); break;
    case 12: object->processor = gp_find_processor("pic16c5x"); break;
    case 14: object->processor = gp_find_processor("pic16cxx"); break;
    case 16: object->processor = gp_find_processor("pic17cxx"); break;
    }
    if (object->processor == no_processor)
      gp_error("invalid processor type (%#04lx) in \"%s\"",
               proc_code,
               object->filename);
    else
      gp_warning("unknown processor type (%#04lx) in \"%s\" defaulted to %s",
                 proc_code,
                 object->filename,
                 gp_processor_name(object->processor, 0));
  }

  object->class = gp_processor_class(object->processor);
  if (gp_processor_rom_width(object->class) != rom_width) {
    if (object->class == PROC_CLASS_EEPROM8 && rom_width == 16) {
      object->processor = gp_find_processor("eeprom16");
      object->class = gp_processor_class(object->processor);
    }
    else
      gp_error("invalid rom width for selected processor (%ld) in \"%s\"",
               rom_width,
               object->filename);
  }

  ram_width = gp_getl32(&file[offset]);
  if (ram_width != 8)
    gp_error("invalid ram width (%ld) in \"%s\"", ram_width, object->filename);
  offset += 4;
}

static void
_read_section_header(gp_object_type *object,
                     gp_section_type *section,
                     const unsigned char *file,
                     const char *string_table)
{
  char buffer[9];
  unsigned int offset;

  if (gp_getl32(&file[0]) == 0) {
    /* read name from the string table */
    offset = gp_getl32(&file[4]);
    section->name = strdup(&string_table[offset]);
  } else {
    memcpy(buffer, &file[0], 8);
    /* the name can occupy all 8 chars without a null terminator */
    buffer[8] = '\0';
    section->name = strdup(buffer);
  }

  section->symbol = gp_coffgen_findsymbol(object, section->name);

  section->address = gp_getl32(&file[8]);
  section->virtual_address = gp_getl32(&file[12]);
  if (section->address != section->virtual_address)
    gp_error("virtual address does not equal physical address in \"%s\"",
             object->filename);

  section->size       = gp_getl32(&file[16]);
  section->data_ptr   = gp_getl32(&file[20]);
  section->reloc_ptr  = gp_getl32(&file[24]);
  section->lineno_ptr = gp_getl32(&file[28]);
  section->num_reloc  = gp_getl16(&file[32]);
  section->num_lineno = gp_getl16(&file[34]);
  section->flags      = gp_getl32(&file[36]);

  if (section->data_ptr)
    section->data = i_memory_create();
  else
    section->data = NULL;
  section->relocations = NULL;
  section->relocations_tail = NULL;
  section->line_numbers = NULL;
  section->line_numbers_tail = NULL;
  section->is_used = false;

  if (section->flags & (STYP_TEXT|STYP_DATA_ROM))
    section->address = gp_processor_org_to_byte(object->class, section->address);
  section->shadow_address = section->address;
}

static void
_read_reloc(gp_object_type *object,
            gp_section_type *section,
            gp_reloc_type *relocation,
            const unsigned char *file)
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
             int org_to_byte_shift,
             gp_linenum_type *line_number,
             const unsigned char *file)
{

  line_number->symbol      = &object->symbols[gp_getl32(&file[0])];
  line_number->line_number = gp_getl16(&file[4]);
  line_number->address     = gp_org_to_byte(org_to_byte_shift, gp_getl32(&file[6]));

  /* FIXME: function index and flags are unused, so far.
  line_number->l_flags  = gp_getl16(&file[10]);
  line_number->l_fcnndx = gp_getl32(&file[12]);
  */

}

static void
_read_sections(gp_object_type *object, const unsigned char *file)
{
  int i, j;
  const unsigned char *section_ptr;
  const char *string_table;
  const unsigned char *loc;
  gp_section_type *current = NULL;
  gp_reloc_type *current_reloc = NULL;
  gp_linenum_type *current_linenum = NULL;
  unsigned int number;
  int org;

  /* move to the start of the section headers */
  section_ptr = file + (object->isnew ? (FILE_HDR_SIZ_v2 + OPT_HDR_SIZ_v2) :
            (FILE_HDR_SIZ_v1 + OPT_HDR_SIZ_v1));

  /* setup pointer to string table */
  string_table = (const char*)
    &file[object->symbol_ptr +
          (object->symbol_size * object->num_symbols)];

  object->sections = gp_coffgen_blocksec(object->num_sections);
  current = object->sections;

  for (i = 0; i < object->num_sections; i++) {
    _read_section_header(object, current, section_ptr, string_table);

    current->number = i + 1;
    section_ptr += (object->isnew ? SEC_HDR_SIZ_v2  : SEC_HDR_SIZ_v1);

    /* read the data */
    if ((current->size) && (current->data_ptr)) {
      org = current->address;
      loc = &file[current->data_ptr];
      number = current->size;
      for (j = 0; j < number; j++) {
        b_memory_put(current->data, org + j, loc[j]);
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
      int org_to_byte_shift;
      if (current->flags & (STYP_TEXT|STYP_DATA_ROM))
        org_to_byte_shift = object->class->org_to_byte_shift;
      else
        org_to_byte_shift = 0;
      loc = &file[current->lineno_ptr];
      number = current->num_lineno;
      current->line_numbers = gp_coffgen_blockline(number);
      current_linenum = current->line_numbers;
      for (j = 0; j < number; j++) {
        _read_lineno(object, org_to_byte_shift, current_linenum, loc);
        loc += LINENO_SIZ;
        current->line_numbers_tail = current_linenum;
        current_linenum = current_linenum->next;
      }
    }

    object->sections_tail = current;
    current = current->next;
  }

}

struct lazy_linking_s {
  union {
    gp_symbol_type *symbol;
    gp_aux_type *aux;
  } read;
  struct lazy_linking_s *next;
};

static void
_read_aux(gp_object_type *object, int i, gp_aux_type *aux, int aux_type,
          const unsigned char *file, const char *string_table, struct lazy_linking_s *lazy_linking)
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
      aux->_aux_symbol._aux_file.flags = file[8];
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
    case AUX_FCN_CALLS: {
      unsigned long calleendx = gp_getl32(&file[0]);
      /* First symbol index is 0 */
      if (calleendx < i)
        aux->_aux_symbol._aux_fcn_calls.callee = lazy_linking[calleendx].read.symbol;
      else if (calleendx == 0xFFFFFFFF)
        /* "higher order function", call through a pointer */
        aux->_aux_symbol._aux_fcn_calls.callee = NULL;
      else {
        /* Symbol not read yet, link for lazy binding */
        lazy_linking[i].read.aux = aux;
        lazy_linking[i].next = lazy_linking[calleendx].next;
        lazy_linking[calleendx].next = &lazy_linking[i];
      }
      aux->_aux_symbol._aux_fcn_calls.is_interrupt = gp_getl32(&file[4]);
      break;
    }
    default:
      memcpy(&aux->_aux_symbol.data[0], file, object->symbol_size);
  }

}

static void
_read_symbol(gp_object_type *object, int i, gp_symbol_type *symbol,
             const unsigned char *file, const char *string_table, struct lazy_linking_s *lazy_linking)
{
  char buffer[9];
  unsigned int offset;
  size_t file_off = 0;
  struct lazy_linking_s *current_lazy;

  if (gp_getl32(&file[0]) == 0) {
    /* read name from the string table */
    offset = gp_getl32(&file[4]);
    symbol->name = strdup(&string_table[offset]);
  } else {
    memcpy(buffer, &file[0], 8);
    /* the name can occupy all 8 chars without a null terminator */
    buffer[8] = '\0';
    symbol->name = strdup(buffer);
  }

  file_off = 8;
  symbol->value = gp_getl32(&file[file_off]);           file_off += 4;
  symbol->section_number = gp_getl16(&file[file_off]);  file_off += 2;
  if(object->isnew) {
    unsigned type = gp_getl32(&file[file_off]);         file_off += 4;
    symbol->type = type & 0x1F;
    symbol->derived_type = type >> 5;
  } else {
    /* TODO Make sure the old format had this alignment */
    unsigned type = gp_getl16(&file[file_off]);         file_off += 2;
    symbol->type = type & 0x0F;
    symbol->derived_type = type >> 4;
  }
  symbol->class = file[file_off];                                               file_off += 1;
  symbol->num_auxsym = file[file_off];                                  file_off += 1;

  symbol->section = NULL;
  symbol->aux_list = NULL;

  lazy_linking[i].read.symbol = symbol;
  /* update those aux entries that pointed to us */
  current_lazy = lazy_linking[i].next;
  while (current_lazy != NULL) {
    assert(current_lazy < &lazy_linking[i]);
    switch(current_lazy->read.aux->type) {
    case AUX_FCN_CALLS:
      current_lazy->read.aux->_aux_symbol._aux_fcn_calls.callee = symbol;
      break;
    default:
      assert(!"Lazy symbol binding not implemented");
    }
    current_lazy = current_lazy->next;
  }
}

static void
_read_symtbl(gp_object_type *object, const unsigned char *file)
{
  int i;
  int j;
  int number = object->num_symbols;
  int num_auxsym;
  int aux_type;
  gp_symbol_type *current = NULL;
  gp_aux_type *current_aux = NULL;
  const char *string_table;
  struct lazy_linking_s *lazy_linking;

  if (number != 0) {
    /* create a block of symbols */
    object->symbols = gp_coffgen_blocksym(number);

    /* setup pointer to string table */
    string_table = (const char *)
      &file[object->symbol_ptr + (object->symbol_size * number)];

    /* setup lazy linking of symbol table indices */
    lazy_linking = (struct lazy_linking_s*)malloc(sizeof(struct lazy_linking_s)*number);
    memset(lazy_linking, 0, sizeof(struct lazy_linking_s)*number);

    /* read the symbols */
    file = &file[object->symbol_ptr];
    current = object->symbols;
    for (i = 0; i < number; i++) {
      /* read the symbol */
      _read_symbol(object, i, current, file, string_table, lazy_linking);
      current->number = i;
      num_auxsym = current->num_auxsym;
      file += object->symbol_size;

      if (num_auxsym != 0) {
        current->aux_list = gp_coffgen_blockaux(current->num_auxsym);
        current_aux = current->aux_list;
        aux_type = gp_determine_aux(current);

        /* read the aux symbols */
        for (j = 0; j < num_auxsym; j++) {
          _read_aux(object, i+1, current_aux, aux_type, file, string_table, lazy_linking);
          /* AUX_FCN may be followed by AUX_FCN_CALLS */
          if (aux_type == AUX_FCN)
            aux_type = AUX_FCN_CALLS;
          current_aux = current_aux->next;
          file += object->symbol_size;
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
gp_convert_file(const char *filename, const unsigned char *file)
{
  gp_object_type *object;

  /* initialize object file */
  object = gp_coffgen_init();
  object->filename = strdup(filename);

  /* read the object */
  if (_read_file_header(object, file) != 0)
    _read_opt_header(object, file + (object->isnew ? FILE_HDR_SIZ_v2 : FILE_HDR_SIZ_v1));
  _read_symtbl(object, file);
  _read_sections(object, file);
  _clean_symtbl(object);

  return object;
}

gp_object_type *
gp_read_coff(const char *filename)
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

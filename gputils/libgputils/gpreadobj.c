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

static short
check_getl16(const unsigned char *addr, gp_binary_type *data)
{
  assert(NULL != data);

  if (addr < (data->file + data->size)) {
    return gp_getl16(addr);
  }
  else {
    gp_error("Bad object file format.");
    exit(0);
    /* just to make the compiler satisfied */
    return 0;
  }
}

static long
check_getl32(const unsigned char *addr, gp_binary_type *data)
{
  assert(NULL != data);

  if (addr < (data->file + data->size)) {
    return gp_getl32(addr);
  }
  else {
    gp_error("Bad object file format.");
    exit(0);
    /* just to make the compiler satisfied */
    return 0;
  }
}

gp_coff_type
gp_identify_coff_file(const char *filename)
{
  FILE *file;
  char magic[SARMAG + 1];
  int n;

  if ((file = fopen(filename, "rb")) == NULL) {
    return GP_COFF_SYS_ERR;
  }

  /* Read the magic number. Archive magic numbers are longest, so read
     their size */
  n = fread(&magic[0], 1, SARMAG, file);
  fclose(file);

  if (n == SARMAG) {
    if (((magic[1] << 8) + magic[0]) == MICROCHIP_MAGIC_v1) {
      return GP_COFF_OBJECT;
    }

    if (((magic[1] << 8) + magic[0]) == MICROCHIP_MAGIC_v2) {
      return GP_COFF_OBJECT_V2;
    }

    if (strncmp(magic, ARMAG, SARMAG) == 0) {
      return GP_COFF_ARCHIVE;
    }
  }

  return GP_COFF_UNKNOWN;
}

/* Read a binary file and store it in memory. */

gp_binary_type *
gp_read_file(const char *filename)
{
  FILE *infile;
  gp_binary_type *file;
  struct stat statbuf;
  int n;

  infile = fopen(filename, "rb");
  if (infile == NULL) {
    perror(filename);
    exit(1);
  }

  file = (gp_binary_type *)GP_Malloc(sizeof(*file));

  /* determine the size of the file */
  fstat(fileno(infile), &statbuf);
  file->size = statbuf.st_size;

  /* read the object file into memory */
  file->file = (unsigned char *)GP_Malloc(file->size);
  n = fread(file->file, 1, file->size, infile);

  if (n != file->size) {
    gp_error("File \"%s\" size doesn't match the statbuf.st_size.", filename);
  }

  fclose(infile);

  return file;
}

/* free a binary file. */

void
gp_free_file(gp_binary_type *data)
{
  if (data == NULL) {
    return;
  }

  free(data->file);
  free(data);
}

/* Return optional header size (0 for no header) */
static int
_read_file_header(gp_object_type *object, const unsigned char *file, gp_binary_type *data)
{
  gp_boolean isnew = false;
  int opt_hdr;

  if (check_getl16(&file[0], data) == MICROCHIP_MAGIC_v2) {
    isnew = true;
  }
  else if (check_getl16(&file[0], data) != MICROCHIP_MAGIC_v1) {
    gp_error("Invalid magic number in \"%s\".", object->filename);
  }

  object->isnew        = isnew;
  object->version      = check_getl16(&file[0], data);
  object->num_sections = check_getl16(&file[2], data);
  object->time         = check_getl32(&file[4], data);
  object->symbol_ptr   = check_getl32(&file[8], data);
  object->num_symbols  = check_getl32(&file[12], data);

  opt_hdr = check_getl16(&file[16], data);

  if ((opt_hdr != 0) && (opt_hdr != (isnew ? OPT_HDR_SIZ_v2 : OPT_HDR_SIZ_v1))) {
    gp_error("Incorrect optional header size (%d) in \"%s\".", opt_hdr, object->filename);
  }

  object->symbol_size = (object->version == MICROCHIP_MAGIC_v1) ? SYMBOL_SIZE_v1 : SYMBOL_SIZE_v2;
  object->flags = check_getl16(&file[18], data);

  return opt_hdr;
}

static void
_read_opt_header(gp_object_type *object, const unsigned char *file, gp_binary_type *data)
{
  unsigned short optmagic;
  unsigned long vstamp, proc_code, rom_width, ram_width;
  size_t offset = 0;

  optmagic = check_getl16(&file[0], data);

  if (optmagic != ((object->isnew) ? OPTMAGIC_v2 : OPTMAGIC_v1)) {
    gp_error("Invalid optional magic number (%#04x) in \"%s\".", optmagic, object->filename);
  }

  offset = 2;

  if (object->isnew) {
    vstamp = check_getl32(&file[offset], data);
    offset += 4;
  }
  else {
    vstamp = check_getl16(&file[offset], data);
    offset += 2;
  }

  if ((!object->isnew) && (vstamp != 1)) {
    gp_error("Invalid assembler version (%ld) in \"%s\".", vstamp, object->filename);
  }

  proc_code = check_getl32(&file[offset], data);
  offset += 4;

  rom_width = check_getl32(&file[offset], data);
  offset += 4;

  object->processor = gp_processor_coff_proc(proc_code);

  if (object->processor == NULL) {
    /* Fallback to a generic processor of matching rom width */
    switch(rom_width) {
    case 8:  object->processor = gp_find_processor("pic18cxx"); break;
    case 12: object->processor = gp_find_processor("pic16c5x"); break;
    case 14: object->processor = gp_find_processor("pic16cxx"); break;
    case 16: object->processor = gp_find_processor("pic17cxx"); break;
    }

    if (object->processor == NULL) {
      gp_error("Invalid processor type (%#04lx) in \"%s\".",
               proc_code, object->filename);
    }
    else {
      gp_warning("Unknown processor type (%#04lx) in \"%s\" defaulted to %s.",
                 proc_code, object->filename,
                 gp_processor_name(object->processor, 0));
    }
  }

  object->class = gp_processor_class(object->processor);

  if (gp_processor_rom_width(object->class) != rom_width) {
    if ((object->class == PROC_CLASS_EEPROM8) && (rom_width == 16)) {
      object->processor = gp_find_processor("eeprom16");
      object->class = gp_processor_class(object->processor);
    }
    else {
      gp_error("Invalid rom width for selected processor (%ld) in \"%s\".",
               rom_width, object->filename);
    }
  }

  ram_width = check_getl32(&file[offset], data);

  if (ram_width != 8) {
    gp_error("Invalid ram width (%ld) in \"%s\".", ram_width, object->filename);
  }

  offset += 4;
}

static void
_read_section_header(gp_object_type *object, gp_section_type *section,
                     const unsigned char *file, const char *string_table, gp_binary_type *data)
{
  char buffer[9];
  unsigned int offset;

  if (check_getl32(&file[0], data) == 0) {
    /* read name from the string table */
    offset = check_getl32(&file[4], data);
    section->name = GP_Strdup(&string_table[offset]);
  }
  else {
    memcpy(buffer, &file[0], sizeof(buffer) - 1);
    /* the name can occupy all 8 chars without a null terminator */
    buffer[8] = '\0';
    section->name = GP_Strdup(buffer);
  }

  section->symbol = gp_coffgen_findsectionsymbol(object, section->name);

  section->address = check_getl32(&file[8], data);
  section->virtual_address = check_getl32(&file[12], data);

  if (section->address != section->virtual_address) {
    gp_error("Virtual address does not equal physical address in \"%s\".",
             object->filename);
  }

  section->size       = check_getl32(&file[16], data);
  section->data_ptr   = check_getl32(&file[20], data);
  section->reloc_ptr  = check_getl32(&file[24], data);
  section->lineno_ptr = check_getl32(&file[28], data);
  section->num_reloc  = check_getl16(&file[32], data);
  section->num_lineno = check_getl16(&file[34], data);
  section->flags      = check_getl32(&file[36], data);
  section->data       = (section->data_ptr) ? i_memory_create() : NULL;

  section->relocations = NULL;
  section->relocations_tail = NULL;
  section->line_numbers = NULL;
  section->line_numbers_tail = NULL;
  section->is_used = false;

  if (section->flags & (STYP_TEXT | STYP_DATA_ROM)) {
    section->address = gp_processor_org_to_byte(object->class, section->address);
  }

  if (((section->flags & (STYP_TEXT | STYP_ABS)) == (STYP_TEXT | STYP_ABS)) && (section->address & 1)) {
    gp_error("Absolute code section \"%s\" must start at a word-aligned address.\n", section->name);
  }

  section->shadow_address = section->address;
}

static void
_read_reloc(gp_object_type *object, gp_section_type *section, gp_reloc_type *relocation,
            const unsigned char *file, gp_binary_type *data)
{
  relocation->address = check_getl32(&file[0], data);
  relocation->symbol  = &object->symbols[check_getl32(&file[4], data)];
  relocation->offset  = check_getl16(&file[8], data);
  relocation->type    = check_getl16(&file[10], data);

  if (relocation->address >= section->size) {
    gp_error("Relocation at address %#x in section \"%s\" of \"%s\" exceeds the section size.",
             relocation->address, section->name, object->filename);
  }
}

static void
_read_lineno(gp_object_type *object, int org_to_byte_shift, gp_linenum_type *line_number,
             const unsigned char *file, gp_binary_type *data)
{
  line_number->symbol      = &object->symbols[check_getl32(&file[0], data)];
  line_number->line_number = check_getl16(&file[4], data);
  line_number->address     = gp_org_to_byte(org_to_byte_shift, check_getl32(&file[6], data));

  /* FIXME: function index and flags are unused, so far.
  line_number->l_flags  = check_getl16(&file[10], data);
  line_number->l_fcnndx = check_getl32(&file[12], data);
  */
}

static void
_read_sections(gp_object_type *object, const unsigned char *file, gp_binary_type *data)
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
  string_table = (const char *)&file[object->symbol_ptr + (object->symbol_size * object->num_symbols)];

  object->sections = gp_coffgen_blocksec(object->num_sections);
  current = object->sections;

  for (i = 0; i < object->num_sections; i++) {
    _read_section_header(object, current, section_ptr, string_table, data);

    current->number = i + 1;
    section_ptr += (object->isnew) ? SEC_HDR_SIZ_v2 : SEC_HDR_SIZ_v1;

    /* read the data */
    if ((current->size > 0) && (current->data_ptr > 0)) {
      org = current->address;
      loc = &file[current->data_ptr];
      number = current->size;

      for (j = 0; j < number; j++) {
        b_memory_put(current->data, org + j, loc[j], current->name, NULL);
      }
    }

    /* read the relocations */
    if ((current->num_reloc > 0) && (current->reloc_ptr > 0)) {
      loc = &file[current->reloc_ptr];
      number = current->num_reloc;
      current->relocations = gp_coffgen_blockrel(number);
      current_reloc = current->relocations;

      for (j = 0; j < number; j++) {
        _read_reloc(object, current, current_reloc, loc, data);
        loc += RELOC_SIZ;
        current->relocations_tail = current_reloc;
        current_reloc = current_reloc->next;
      }
    }

    /* read the line numbers */
    if ((current->num_lineno > 0) && (current->lineno_ptr > 0)) {
      int org_to_byte_shift;

      if (current->flags & (STYP_TEXT | STYP_DATA_ROM)) {
        org_to_byte_shift = object->class->org_to_byte_shift;
      }
      else {
        org_to_byte_shift = 0;
      }

      loc = &file[current->lineno_ptr];
      number = current->num_lineno;
      current->line_numbers = gp_coffgen_blockline(number);
      current_linenum = current->line_numbers;

      for (j = 0; j < number; j++) {
        _read_lineno(object, org_to_byte_shift, current_linenum, loc, data);
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
_read_aux(gp_object_type *object, int i, gp_aux_type *aux, int aux_type, const unsigned char *file,
          const char *string_table, struct lazy_linking_s *lazy_linking, gp_binary_type *data)
{
  aux->type = aux_type;

  switch (aux_type) {
    case AUX_DIRECT:
      aux->_aux_symbol._aux_direct.command = file[0];
      aux->_aux_symbol._aux_direct.string =
        GP_Strdup(&string_table[check_getl32(&file[4], data)]);
      break;

    case AUX_FILE:
      aux->_aux_symbol._aux_file.filename =
        GP_Strdup(&string_table[check_getl32(&file[0], data)]);
      aux->_aux_symbol._aux_file.line_number = check_getl32(&file[4], data);
      aux->_aux_symbol._aux_file.flags = file[8];
      break;

    case AUX_IDENT:
      aux->_aux_symbol._aux_ident.string =
        GP_Strdup(&string_table[check_getl32(&file[0], data)]);
      break;

    case AUX_SCN:
      aux->_aux_symbol._aux_scn.length  = check_getl32(&file[0], data);
      aux->_aux_symbol._aux_scn.nreloc  = check_getl16(&file[4], data);
      aux->_aux_symbol._aux_scn.nlineno = check_getl16(&file[6], data);
      break;

    case AUX_FCN_CALLS: {
      unsigned long calleendx = check_getl32(&file[0], data);

      /* First symbol index is 0. */
      if (calleendx < i) {
        aux->_aux_symbol._aux_fcn_calls.callee = lazy_linking[calleendx].read.symbol;
      }
      else if (calleendx == 0xffffffff) {
        /* "higher order function", call through a pointer */
        aux->_aux_symbol._aux_fcn_calls.callee = NULL;
      }
      else {
        /* Symbol not read yet, link for lazy binding. */
        lazy_linking[i].read.aux = aux;
        lazy_linking[i].next = lazy_linking[calleendx].next;
        lazy_linking[calleendx].next = &lazy_linking[i];
      }
      aux->_aux_symbol._aux_fcn_calls.is_interrupt = check_getl32(&file[4], data);
      break;
    }

    default:
      memcpy(&aux->_aux_symbol.data[0], file, object->symbol_size);
  }
}

static void
_read_symbol(gp_object_type *object, int i, gp_symbol_type *symbol, const unsigned char *file,
             const char *string_table, struct lazy_linking_s *lazy_linking, gp_binary_type *data)
{
  char buffer[9];
  unsigned int offset;
  size_t file_off = 0;
  struct lazy_linking_s *current_lazy;

  if (check_getl32(&file[0], data) == 0) {
    /* read name from the string table */
    offset = check_getl32(&file[4], data);
    symbol->name = GP_Strdup(&string_table[offset]);
  }
  else {
    memcpy(buffer, &file[0], 8);
    /* the name can occupy all 8 chars without a null terminator */
    buffer[8] = '\0';
    symbol->name = GP_Strdup(buffer);
  }

  file_off = 8;
  symbol->value = check_getl32(&file[file_off], data);
  file_off += 4;
  symbol->section_number = check_getl16(&file[file_off], data);
  file_off += 2;

  if (object->isnew) {
    unsigned int type = check_getl32(&file[file_off], data);

    file_off += 4;
    symbol->type = type & 0x1F;
    symbol->derived_type = type >> 5;
  }
  else {
    /* TODO: Make sure the old format had this alignment. */
    unsigned int type = check_getl16(&file[file_off], data);

    file_off += 2;
    symbol->type = type & 0x0F;
    symbol->derived_type = type >> 4;
  }

  symbol->class = file[file_off];
  file_off += 1;
  symbol->num_auxsym = file[file_off];
  file_off += 1;

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
        assert(!"Lazy symbol binding not implemented.");
    }
    current_lazy = current_lazy->next;
  }
}

static void
_read_symtbl(gp_object_type *object, const unsigned char *file, gp_binary_type *data)
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
  const char *section_name = NULL;

  if (number > 0) {
    /* create a block of symbols */
    object->symbols = gp_coffgen_blocksym(number);

    /* setup pointer to string table */
    string_table = (const char *)&file[object->symbol_ptr + (object->symbol_size * number)];

    /* setup lazy linking of symbol table indices */
    lazy_linking = (struct lazy_linking_s *)GP_Calloc(number, sizeof(struct lazy_linking_s));

    /* read the symbols */
    file = &file[object->symbol_ptr];
    current = object->symbols;

    for (i = 0; i < number; i++) {
      /* read the symbol */
      _read_symbol(object, i, current, file, string_table, lazy_linking, data);

      if ((current->class == C_SECTION) || (current->class == C_FILE)) {
        section_name = current->name;
      }

      current->section_name = section_name;
      current->number = i;
      num_auxsym = current->num_auxsym;
      file += object->symbol_size;

      if (num_auxsym > 0) {
        current->aux_list = gp_coffgen_blockaux(current->num_auxsym);
        current_aux = current->aux_list;
        aux_type = gp_determine_aux(current);

        /* read the aux symbols */
        for (j = 0; j < num_auxsym; j++) {
          _read_aux(object, i + 1, current_aux, aux_type, file, string_table, lazy_linking, data);
          /* AUX_FCN may be followed by AUX_FCN_CALLS */
          if (aux_type == AUX_FCN) {
            aux_type = AUX_FCN_CALLS;
          }

          current_aux = current_aux->next;
          file += object->symbol_size;
          i++;
        }

        /* advance the through the list */
        for (j = 0; j < num_auxsym; j++) {
          /* COFF places all symbols inluding auxiliary, in the symbol table.
             However, in memory, gputils attaches auxiliary symbols to their
             associated primary symbol. When reading COFF, space is reserved
             for the auxiliary symbols but not used. Later the space is
             freed. This simplifies assigning the pointer in the
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
  /* gp_symbol_type *old_symbol = NULL; */
  unsigned int i;

  current = object->symbols;

  while (current != NULL) {
    /* assign section pointer, section numbers start at 1 not 0 */
    if (current->section_number > 0) {
      current->section = &object->sections[current->section_number - 1];
    }
    else {
      current->section = NULL;
    }

    if (current->num_auxsym > 0) {
      next_symbol = current->next;
      for (i = 0; i < current->num_auxsym; i++) {
        /* old_symbol = next_symbol; */
        next_symbol = next_symbol->next;
        /* FIXME: Can't free the single symbols because they were allocated
           in blocks. This won't be a problem once obstacks are used.
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
gp_convert_file(const char *filename, gp_binary_type *data)
{
  gp_object_type *object;

  /* initialize object file */
  object = gp_coffgen_init();
  object->filename = GP_Strdup(filename);

  /* read the object */
  if (_read_file_header(object, data->file, data) != 0) {
    _read_opt_header(object, data->file + (object->isnew ? FILE_HDR_SIZ_v2 : FILE_HDR_SIZ_v1), data);
  }
  _read_symtbl(object, data->file, data);
  _read_sections(object, data->file, data);
  _clean_symtbl(object);

  return object;
}

gp_object_type *
gp_read_coff(const char *filename)
{
  gp_object_type *object;
  gp_binary_type *data = gp_read_file(filename);

  if (data == NULL) {
    return NULL;
  }

  object = gp_convert_file(filename, data);
  gp_free_file(data);

  return object;
}

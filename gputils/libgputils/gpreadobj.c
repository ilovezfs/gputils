/* Read coff objects
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

struct lazy_linking_s {
  union {
    gp_symbol_t *symbol;
    gp_aux_t    *aux;
  } read;
  struct lazy_linking_s *next;
};

/*------------------------------------------------------------------------------------------------*/

static uint16_t
_check_getl16(const uint8_t *Address, const gp_binary_t *Data)
{
  assert(Data != NULL);

  if (Address < (Data->file + Data->size)) {
    return gp_getl16(Address);
  }
  else {
    gp_error("Bad object file format.");
    exit(0);
    /* just to make the compiler satisfied */
    return 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

static uint32_t
_check_getl32(const uint8_t *Address, const gp_binary_t *Data)
{
  assert(Data != NULL);

  if (Address < (Data->file + Data->size)) {
    return gp_getl32(Address);
  }
  else {
    gp_error("Bad object file format.");
    exit(0);
    /* just to make the compiler satisfied */
    return 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Return optional header size (0 for no header). */

static unsigned int
_read_file_header(gp_object_t *Object, const uint8_t *File, const gp_binary_t *Data)
{
  gp_boolean   isnew = false;
  unsigned int opt_hdr;
  uint16_t     version;

  /* 'f_magic'  -- magic number */
  version = _check_getl16(&File[0], Data);

  if (version == MICROCHIP_MAGIC_v2) {
    isnew = true;
  }
  else if (version != MICROCHIP_MAGIC_v1) {
    gp_error("Invalid magic number in \"%s\".", Object->filename);
  }

  Object->isnew        = isnew;
  Object->version      = version;
  /* 'f_nscns'  -- number of sections */
  Object->num_sections = _check_getl16(&File[2], Data);
  /* 'f_timdat' -- time and date stamp */
  Object->time         = _check_getl32(&File[4], Data);
  /* 'f_symptr' -- file ptr to symtab */
  Object->symbol_ptr   = _check_getl32(&File[8], Data);
  /* 'f_nsyms'  -- # symtab entries */
  Object->num_symbols  = _check_getl32(&File[12], Data);

  /* 'f_opthdr' -- sizeof(opt hdr) */
  opt_hdr = _check_getl16(&File[16], Data);

  if ((opt_hdr != 0) && (opt_hdr != (isnew ? OPT_HDR_SIZ_v2 : OPT_HDR_SIZ_v1))) {
    gp_error("Incorrect optional header size (%u) in \"%s\".", opt_hdr, Object->filename);
  }

  Object->symbol_size = (version == MICROCHIP_MAGIC_v1) ? SYMBOL_SIZE_v1 : SYMBOL_SIZE_v2;
  /* 'f_flags'  -- flags */
  Object->flags       = _check_getl16(&File[18], Data);

  return opt_hdr;
}

/*------------------------------------------------------------------------------------------------*/

static void
_read_opt_header(gp_object_t *Object, const uint8_t *File, const gp_binary_t *Data)
{
  uint16_t     opt_magic;
  uint32_t     vstamp;
  uint32_t     proc_code;
  uint32_t     rom_width;
  uint32_t     ram_width;
  unsigned int data_idx;

  /* 'opt_magic' */
  opt_magic = _check_getl16(&File[0], Data);

  if (opt_magic != ((Object->isnew) ? OPTMAGIC_v2 : OPTMAGIC_v1)) {
    gp_error("Invalid optional magic number (%#04x) in \"%s\".", opt_magic, Object->filename);
  }

  data_idx = 2;

  if (Object->isnew) {
    /* 'vstamp' -- version stamp of compiler */
    vstamp = _check_getl32(&File[data_idx], Data);
    data_idx += 4;
  }
  else {
    /* 'vstamp' -- version stamp of compiler */
    vstamp = _check_getl16(&File[data_idx], Data);
    data_idx += 2;
  }

  if ((!Object->isnew) && (vstamp != 1)) {
    gp_error("Invalid assembler version (%ld) in \"%s\".", vstamp, Object->filename);
  }

  /* 'proc_type'      -- processor type */
  proc_code = _check_getl32(&File[data_idx], Data);
  data_idx += 4;

  /* 'rom_width_bits' -- ROM width bits */
  rom_width = _check_getl32(&File[data_idx], Data);
  data_idx += 4;

  Object->processor = gp_processor_coff_proc(proc_code);

  if (Object->processor == NULL) {
    /* Fallback to a generic processor of matching rom width. */
    switch (rom_width) {
      case 8:  Object->processor = gp_find_processor("pic18cxx"); break;
      case 12: Object->processor = gp_find_processor("pic16c5x"); break;
      case 14: Object->processor = gp_find_processor("pic16cxx"); break;
      case 16: Object->processor = gp_find_processor("pic17cxx"); break;
    }

    if (Object->processor == NULL) {
      gp_error("Invalid processor type (%#04x) in \"%s\".", proc_code, Object->filename);
    }
    else {
      gp_warning("Unknown processor type (%#04x) in \"%s\" defaulted to %s.",
                 proc_code, Object->filename, gp_processor_name(Object->processor, 0));
    }
  }

  Object->class = gp_processor_class(Object->processor);

  if (gp_processor_rom_width(Object->class) != rom_width) {
    if ((Object->class == PROC_CLASS_EEPROM8) && (rom_width == 16)) {
      Object->processor = gp_find_processor("eeprom16");
      Object->class     = gp_processor_class(Object->processor);
    }
    else {
      gp_error("Invalid rom width for selected processor (%u) in \"%s\".",
               rom_width, Object->filename);
    }
  }

  /* 'ram_width_bits' -- RAM width bits */
  ram_width = _check_getl32(&File[data_idx], Data);

  if (ram_width != 8) {
    gp_error("Invalid ram width (%u) in \"%s\".", ram_width, Object->filename);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_read_section_header(gp_object_t *Object, gp_section_t *Section, const uint8_t *File,
                     const char *String_table, const gp_binary_t *Data)
{
  char     buffer[9];
  uint32_t string_offset;

  if (_check_getl32(&File[0], Data) == 0) {
    /* Long name, read this from the string table. */
    string_offset = _check_getl32(&File[4], Data);
    Section->name = GP_Strdup(&String_table[string_offset]);
  }
  else {
    memcpy(buffer, &File[0], sizeof(buffer) - 1);
    /* The name can occupy all 8 chars without a null terminator. */
    buffer[8]     = '\0';
    Section->name = GP_Strdup(buffer);
  }

  Section->symbol = gp_coffgen_find_section_symbol(Object, Section->name);

  /* 's_paddr'   -- physical address */
  Section->address         = _check_getl32(&File[8], Data);
  /* 's_vaddr'   -- virtual address */
  Section->virtual_address = _check_getl32(&File[12], Data);

  if (Section->address != Section->virtual_address) {
    gp_error("Virtual address does not equal physical address in \"%s\".", Object->filename);
  }

  /* 's_size'    -- section size */
  Section->size       = _check_getl32(&File[16], Data);
  /* 's_scnptr'  -- file ptr to raw data */
  Section->data_ptr   = _check_getl32(&File[20], Data);
  /* 's_relptr'  -- file ptr to relocation */
  Section->reloc_ptr  = _check_getl32(&File[24], Data);
  /* 's_lnnoptr' -- file ptr to line numbers */
  Section->lineno_ptr = _check_getl32(&File[28], Data);
  /* 's_nreloc'  -- # reloc entries */
  Section->num_reloc  = _check_getl16(&File[32], Data);
  /* 's_nlnno'   -- # line number entries */
  Section->num_lineno = _check_getl16(&File[34], Data);
  /* 's_flags'   -- section flags */
  Section->flags      = _check_getl32(&File[36], Data);
  Section->data       = (Section->data_ptr != 0) ? i_memory_create() : NULL;

  if (Section->flags & STYP_ROM_AREA) {
    Section->address = gp_processor_org_to_byte(Object->class, Section->address);
  }

  if (((Section->flags & (STYP_TEXT | STYP_ABS)) == (STYP_TEXT | STYP_ABS)) && (Section->address & 1)) {
    gp_error("Absolute code section \"%s\" must start at a word-aligned address.\n", Section->name);
  }

  Section->shadow_address = Section->address;
}

/*------------------------------------------------------------------------------------------------*/

static void
_read_reloc(gp_object_t *Object, const gp_section_t *Section, gp_reloc_t *Relocation,
            const uint8_t *File, const gp_binary_t *Data)
{
  gp_symbol_t *symbol;
  uint32_t     symbol_index;

  /* 'r_vaddr'  -- entry relative virtual address */
  Relocation->address = _check_getl32(&File[0], Data);
  /* 'r_symndx' -- index into symbol table */
  symbol_index        = _check_getl32(&File[4], Data);
  symbol              = Object->symbol_ptr_array[symbol_index];
  Relocation->symbol  = symbol;
  /* 'r_offset' -- offset to be added to address of symbol 'r_symndx' */
  Relocation->offset  = _check_getl16(&File[8], Data);
  /* 'r_type'   -- relocation type */
  Relocation->type    = _check_getl16(&File[10], Data);

  /* Increase the number of relocation links. */
  (symbol->num_reloc_link)++;

  if (Relocation->address >= Section->size) {
    gp_error("Relocation at address %#x in section \"%s\" of \"%s\" exceeds the section size.",
             Relocation->address, Section->name, Object->filename);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_read_lineno(gp_object_t *Object, const gp_section_t *Section, unsigned int Org_to_byte_shift,
             gp_linenum_t *Line_number, const uint8_t *File, const gp_binary_t *Data)
{
  uint32_t symbol_index;
  uint32_t insn_address;

  /* 'l_srcndx' -- symbol table index of associated source file */
  symbol_index             = _check_getl32(&File[0], Data);
  Line_number->symbol      = Object->symbol_ptr_array[symbol_index];
  /* 'l_lnno'   -- line number */
  Line_number->line_number = _check_getl16(&File[4], Data);
  /* 'l_paddr'  -- address of code for this lineno */
  insn_address             = _check_getl32(&File[6], Data);
  Line_number->address     = gp_org_to_byte(Org_to_byte_shift, insn_address);

  /* FIXME: function index and flags are unused, so far.
     'l_flags'  -- bit flags for the line number
  Line_number->l_flags  = _check_getl16(&File[10], Data);
     'l_fcnndx' -- symbol table index of associated function, if there is one
  Line_number->l_fcnndx = _check_getl32(&File[12], Data);
  */
}

/*------------------------------------------------------------------------------------------------*/

static void
_read_sections(gp_object_t *Object, const uint8_t *File, const gp_binary_t *Data)
{
  unsigned int   i;
  unsigned int   j;
  const uint8_t *section_ptr;
  const uint8_t *data_ptr;
  const char    *string_table;
  gp_section_t  *section;
  gp_reloc_t    *relocation;
  gp_linenum_t  *linenum;
  unsigned int   number;
  uint32_t       byte_addr;
  uint32_t       header_size;
  unsigned int   org_to_byte_shift;

  /* move to the start of the section headers */
  section_ptr = File + (Object->isnew ? (FILE_HDR_SIZ_v2 + OPT_HDR_SIZ_v2) :
                                        (FILE_HDR_SIZ_v1 + OPT_HDR_SIZ_v1));

  header_size = (Object->isnew) ? SEC_HDR_SIZ_v2 : SEC_HDR_SIZ_v1;

  /* setup pointer to string table */
  string_table = (const char *)&File[Object->symbol_ptr + (Object->symbol_size * Object->num_symbols)];

  section = gp_coffgen_make_block_section(Object);

  for (i = 0; i < Object->num_sections; i++) {
    _read_section_header(Object, section, section_ptr, string_table, Data);

    section->number  = i + 1;
    section_ptr     += header_size;

    /* read the data */
    if ((section->size > 0) && (section->data_ptr > 0)) {
      byte_addr = section->address;
      data_ptr  = &File[section->data_ptr];
      number    = section->size;

      for (j = 0; j < number; j++) {
        b_memory_put(section->data, byte_addr + j, data_ptr[j], section->name, NULL);
      }
    }

    /* read the relocations */
    if ((section->num_reloc > 0) && (section->reloc_ptr > 0)) {
      data_ptr   = &File[section->reloc_ptr];
      number     = section->num_reloc;
      relocation = gp_coffgen_make_block_reloc(section);

      for (j = 0; j < number; j++) {
        _read_reloc(Object, section, relocation, data_ptr, Data);
        data_ptr   += RELOC_SIZ;
        relocation  = relocation->next;
      }
    }

    /* read the line numbers */
    if ((section->num_lineno > 0) && (section->lineno_ptr > 0)) {
      if (section->flags & STYP_ROM_AREA) {
        org_to_byte_shift = Object->class->org_to_byte_shift;
      }
      else {
        org_to_byte_shift = 0;
      }

      data_ptr = &File[section->lineno_ptr];
      number   = section->num_lineno;
      linenum  = gp_coffgen_make_block_linenum(section);

      for (j = 0; j < number; j++) {
        _read_lineno(Object, section, org_to_byte_shift, linenum, data_ptr, Data);
        data_ptr += LINENO_SIZ;
        linenum   = linenum->next;
      }
    }

    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_read_aux(gp_object_t *Object, uint32_t i, gp_aux_t *Aux, unsigned int Aux_type, const uint8_t *File,
          const char *String_table, struct lazy_linking_s *Lazy_linking, const gp_binary_t *Data)
{
  uint32_t string_offset;
  uint32_t calleendx;

  Aux->type = Aux_type;

  switch (Aux_type) {
    case AUX_DIRECT:
      Aux->_aux_symbol._aux_direct.command = File[0];
      string_offset                        = _check_getl32(&File[4], Data);
      Aux->_aux_symbol._aux_direct.string  = GP_Strdup(&String_table[string_offset]);
      break;

    case AUX_FILE:
      /* 'x_offset'  -- String table offset for filename. */
      string_offset                          = _check_getl32(&File[0], Data);
      Aux->_aux_symbol._aux_file.filename    = GP_Strdup(&String_table[string_offset]);
      /* 'x_incline' -- Line number at which this file was included, 0->not included. */
      Aux->_aux_symbol._aux_file.line_number = _check_getl32(&File[4], Data);
      /* 'x_flags' */
      Aux->_aux_symbol._aux_file.flags       = File[8];
      break;

    case AUX_IDENT:
      /* 'x_tagndx' -- Symbol index of struct/union/enum tagname. */
      string_offset                      = _check_getl32(&File[0], Data);
      Aux->_aux_symbol._aux_ident.string = GP_Strdup(&String_table[string_offset]);
      break;

    case AUX_SCN:
      /* 'x_scnlen' -- Section Length. */
      Aux->_aux_symbol._aux_scn.length  = _check_getl32(&File[0], Data);
      /* 'x_nreloc' -- Number of relocation entries. */
      Aux->_aux_symbol._aux_scn.nreloc  = _check_getl16(&File[4], Data);
      /* 'x_nlinno' -- Number of line numbers. */
      Aux->_aux_symbol._aux_scn.nlineno = _check_getl16(&File[6], Data);
      break;

    case AUX_FCN_CALLS: {
      /* 'x_calleendx' -- Symbol table entry of callee - 1. */
      calleendx = _check_getl32(&File[0], Data);

      /* First symbol index is 0. */
      if (calleendx < i) {
        Aux->_aux_symbol._aux_fcn_calls.callee = Lazy_linking[calleendx].read.symbol;
      }
      else if (calleendx == UINT32_MAX) {
        /* "higher order function", call through a pointer */
        Aux->_aux_symbol._aux_fcn_calls.callee = NULL;
      }
      else {
        /* Symbol not read yet, link for lazy binding. */
        Lazy_linking[i].read.aux     = Aux;
        Lazy_linking[i].next         = Lazy_linking[calleendx].next;
        Lazy_linking[calleendx].next = &Lazy_linking[i];
      }

      /* 'x_is_interrupt' -- 0: not, 1: low, 2: high */
      Aux->_aux_symbol._aux_fcn_calls.is_interrupt = _check_getl32(&File[4], Data);
      break;
    }

    default:
      memcpy(&Aux->_aux_symbol.data[0], File, Object->symbol_size);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_read_symbol(gp_object_t *Object, int i, gp_symbol_t *Symbol, const uint8_t *File,
             const char *String_table, struct lazy_linking_s *Lazy_linking, const gp_binary_t *Data)
{
  char                   buffer[9];
  uint32_t               string_offset;
  uint32_t               type;
  uint32_t               data_idx;
  struct lazy_linking_s *current_lazy;

  /*   's_zeros'  -- first four characters are 0 */
  if (_check_getl32(&File[0], Data) == 0) {
    /* read name from the string table */
    string_offset = _check_getl32(&File[4], Data);
    /* 's_offset' -- pointer to the string table */
    Symbol->name  = GP_Strdup(&String_table[string_offset]);
  }
  else {
    /* 'name'     -- symbol name if less than 8 characters */
    memcpy(buffer, &File[0], 8);
    /* the name can occupy all 8 chars without a null terminator */
    buffer[8]    = '\0';
    Symbol->name = GP_Strdup(buffer);
  }

  data_idx  = 8;
  /* 'value'      -- symbol value */
  Symbol->value          = _check_getl32(&File[data_idx], Data);
  data_idx += 4;
  /* 'sec_num'    -- section number */
  Symbol->section_number = _check_getl16(&File[data_idx], Data);
  data_idx += 2;

  if (Object->isnew) {
    /* 'type' */
    type = _check_getl32(&File[data_idx], Data);
    data_idx += 4;

    Symbol->type         = type & 0x1F;
    Symbol->derived_type = type >> 5;
  }
  else {
    /* TODO: Make sure the old format had this alignment. */
    /* 'type' */
    type = _check_getl16(&File[data_idx], Data);
    data_idx += 2;

    Symbol->type         = type & 0x0F;
    Symbol->derived_type = type >> 4;
  }

  /* 'st_class'   -- storage class */
  Symbol->class = File[data_idx];
  data_idx += 1;
  /* 'num_auxsym' -- number of auxiliary symbols */
  Symbol->num_auxsym = File[data_idx];
  data_idx += 1;

  Lazy_linking[i].read.symbol = Symbol;
  /* update those aux entries that pointed to us */
  current_lazy = Lazy_linking[i].next;
  while (current_lazy != NULL) {
    assert(current_lazy < &Lazy_linking[i]);
    switch (current_lazy->read.aux->type) {
      case AUX_FCN_CALLS:
        current_lazy->read.aux->_aux_symbol._aux_fcn_calls.callee = Symbol;
        break;

      default:
        assert(!"Lazy symbol binding not implemented.");
    }
    current_lazy = current_lazy->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_read_symbol_table(gp_object_t *Object, const uint8_t *File, const gp_binary_t *Data)
{
  unsigned int           i;
  unsigned int           j;
  unsigned int           number;
  unsigned int           num_auxsym;
  unsigned int           aux_type;
  gp_symbol_t           *symbol;
  gp_aux_t              *aux;
  const char            *string_table;
  struct lazy_linking_s *lazy_linking;
  const char            *section_name;

  number = Object->num_symbols;
  if (number == 0) {
    return;
  }

    /* create a block of symbols */
  gp_coffgen_make_block_symbol(Object);

  /* setup pointer to string table */
  string_table = (const char *)&File[Object->symbol_ptr + (Object->symbol_size * number)];

  /* setup lazy linking of symbol table indices */
  lazy_linking = (struct lazy_linking_s *)GP_Calloc(number, sizeof(struct lazy_linking_s));

  /* read the symbols */
  File         = &File[Object->symbol_ptr];
  symbol       = Object->symbol_list;
  section_name = NULL;

  for (i = 0; i < number; i++) {
    /* read the symbol */
    _read_symbol(Object, i, symbol, File, string_table, lazy_linking, Data);

    if ((symbol->class == C_SECTION) || (symbol->class == C_FILE)) {
      section_name = symbol->name;
    }

    symbol->section_name = section_name;
    symbol->number  = i;
    num_auxsym      = symbol->num_auxsym;
    File           += Object->symbol_size;

    if (num_auxsym > 0) {
      aux      = gp_coffgen_make_block_aux(symbol);
      aux_type = gp_coffgen_determine_aux_symbol(symbol);

      /* read the aux symbols */
      for (j = 0; j < num_auxsym; j++) {
        i++;
        _read_aux(Object, i, aux, aux_type, File, string_table, lazy_linking, Data);
        /* AUX_FCN may be followed by AUX_FCN_CALLS */
        if (aux_type == AUX_FCN) {
          aux_type = AUX_FCN_CALLS;
        }

        aux   = aux->next;
        File += Object->symbol_size;
      }

      /* advance the through the list */
      for (j = 0; j < num_auxsym; j++) {
        /* COFF places all symbols inluding auxiliary, in the symbol table.
           However, in memory, gputils attaches auxiliary symbols to their
           associated primary symbol. When reading COFF, space is reserved
           for the auxiliary symbols but not used. Later the space is freed.
           This simplifies assigning the pointer in the relocations. */
        symbol = symbol->next;
      }
    } /* if (num_auxsym > 0) */

    symbol = symbol->next;
  } /* for (i = 0; i < number; i++) */
}

/*------------------------------------------------------------------------------------------------*/

/* Add section pointers and omit from chain the auxiliary entries. */

static void
_clean_symbol_table(gp_object_t *Object)
{
  gp_symbol_t  *curr_symbol;
  gp_symbol_t  *next_symbol;
  gp_symbol_t  *aux_symbol;
  unsigned int  i;

  curr_symbol = Object->symbol_list;
  while (curr_symbol != NULL) {
    if (curr_symbol->section_number > 0) {
      /* Assign section pointer, section numbers start at 1 not 0. */
      curr_symbol->section = Object->section_ptr_array[curr_symbol->section_number - 1];
    }
    else {
      curr_symbol->section = NULL;
    }

    if (curr_symbol->num_auxsym > 0) {
      /* Omit from chain this auxiliary entries. */
      next_symbol = curr_symbol->next;
      for (i = 0; i < curr_symbol->num_auxsym; i++) {
        aux_symbol  = next_symbol;
        next_symbol = next_symbol->next;
//        gp_coffgen_put_reserve_symbol(Object, aux_symbol);
        gp_coffgen_del_symbol(Object, aux_symbol);
      }
    }

    curr_symbol = curr_symbol->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

gp_coff_t
gp_identify_coff_file(const char *File_name)
{
  FILE *file;
  char  magic[SARMAG + 1];
  int   n;

  if ((file = fopen(File_name, "rb")) == NULL) {
    return GP_COFF_SYS_ERR;
  }

  /* Read the magic number. Archive magic numbers are longest, so read their size. */
  n = fread(magic, 1, SARMAG, file);
  fclose(file);

  if (n == SARMAG) {
    if (gp_getu16((const uint8_t *)magic) == MICROCHIP_MAGIC_v1) {
      return GP_COFF_OBJECT;
    }

    if (gp_getu16((const uint8_t *)magic) == MICROCHIP_MAGIC_v2) {
      return GP_COFF_OBJECT_V2;
    }

    if (strncmp(magic, ARMAG, SARMAG) == 0) {
      return GP_COFF_ARCHIVE;
    }
  }

  return GP_COFF_UNKNOWN;
}

/*------------------------------------------------------------------------------------------------*/

/* Read a binary file and store it in memory. */

gp_binary_t *
gp_read_file(const char *File_name)
{
  FILE        *infile;
  gp_binary_t *file;
  struct stat  statbuf;
  off_t        n;

  infile = fopen(File_name, "rb");
  if (infile == NULL) {
    perror(File_name);
    exit(1);
  }

  file = (gp_binary_t *)GP_Malloc(sizeof(*file));

  /* determine the size of the file */
  fstat(fileno(infile), &statbuf);
  file->size = statbuf.st_size;

  /* read the object file into memory */
  file->file = (uint8_t *)GP_Malloc(file->size);
  n = (off_t)fread(file->file, 1, (size_t)file->size, infile);

  if (n != file->size) {
    gp_error("File \"%s\" size doesn't match the statbuf.st_size.", File_name);
  }

  fclose(infile);

  return file;
}

/*------------------------------------------------------------------------------------------------*/

/* free a binary file. */

void
gp_free_file(gp_binary_t *Data)
{
  if (Data == NULL) {
    return;
  }

  free(Data->file);
  free(Data);
}

/*------------------------------------------------------------------------------------------------*/

gp_object_t *
gp_convert_file(const char *File_name, const gp_binary_t *Data)
{
  gp_object_t *object;

  /* initialize object file */
  object = gp_coffgen_new_object(File_name);

  /* read the object */
  if (_read_file_header(object, Data->file, Data) != 0) {
    _read_opt_header(object, Data->file + (object->isnew ? FILE_HDR_SIZ_v2 : FILE_HDR_SIZ_v1), Data);
  }
  _read_symbol_table(object, Data->file, Data);
  _read_sections(object, Data->file, Data);
  _clean_symbol_table(object);

  return object;
}

/*------------------------------------------------------------------------------------------------*/

gp_object_t *
gp_read_coff(const char *File_name)
{
  gp_binary_t *data;
  gp_object_t *object;

  data = gp_read_file(File_name);
  if (data == NULL) {
    return NULL;
  }

  object = gp_convert_file(File_name, data);
  gp_free_file(data);

  return object;
}

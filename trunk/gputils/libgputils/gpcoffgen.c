/* GNU PIC general coff functions
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


#if 0

/* FIXME: Use it or make it global to remove the warning */

static short 
_get_16(char *addr)
{
  return (addr[1] << 8) | addr[0];
}

static void 
_put_16(char *addr, short data)
{
  addr[1] = (data >> 8) & 0xff;
  addr[0] = data & 0xff;

  return;
}

#endif

static long 
_get_32(char *addr)
{
  long value;

  value  = (unsigned char)addr[0];
  value |= (unsigned char)addr[1] << 8;
  value |= (unsigned char)addr[2] << 16;
  value |= (unsigned char)addr[3] << 24;

  return value;
}

static void 
_put_32(char *addr, long data)
{
  addr[0] = data & 0xff;
  addr[1] = (data >> 8)  & 0xff;
  addr[2] = (data >> 16) & 0xff;
  addr[3] = (data >> 24) & 0xff;

  return;
}

gp_object_type *
gp_coffgen_init(char *name, enum pic_processor processor)
{
  gp_object_type *object = NULL;
  enum proc_class class = gp_processor_class(processor);
  unsigned long coff_type = gp_processor_coff_type(processor);
  int rom_width;

  /* make sure we catch unfinished processors */
  assert(coff_type);

  switch (class) {
  case PROC_CLASS_GENERIC:
  case PROC_CLASS_PIC12:
  case PROC_CLASS_SX:
    rom_width = 12;
    break;
  case PROC_CLASS_PIC14:
    rom_width = 14;
    break;
  case PROC_CLASS_PIC16:
  case PROC_CLASS_PIC16E:
    rom_width = 16;
    break;
  case PROC_CLASS_EEPROM8:
  default:
    assert(0);
  }
  
  /* allocate memory for the object file */
  object = (gp_object_type *)malloc(sizeof(*object));

  /* initialize the object */
  object->filename        = strdup(name);
  object->sections        = NULL;
  object->sym_table       = NULL;
  _put_32(&object->string_table[0], 4); /* the byte count is 32 bits */

  /* initialize the file header */
  object->file_header.f_magic  = MICROCHIP_MAGIC;
  object->file_header.f_nscns  = 0;
  object->file_header.f_timdat = (long)time(NULL);
  object->file_header.f_symptr = 0;
  object->file_header.f_nsyms  = 0;
  object->file_header.f_opthdr = OPT_HDR_SIZ;
  object->file_header.f_flags  = 0;

  /* initialize the optional header */
  object->opt_header.opt_magic      = OPTMAGIC;
  object->opt_header.vstamp         = 1;
  object->opt_header.proc_type      = coff_type;
  object->opt_header.rom_width_bits = rom_width;
  object->opt_header.ram_width_bits = 8;

  return object;
}

void
gp_coffgen_fetch_symbol_name(gp_object_type *object, 
                             struct syment *symbol,
			     char *string)
{
  unsigned long index;

  if (symbol->sym_name.ptr.s_zeros == 0) {
    index = symbol->sym_name.ptr.s_offset;
    strncpy(string, &object->string_table[index], BUFSIZ);
  } else {
    strncpy(string, &symbol->sym_name.name[0], 8);
    /* the name can occupy all 8 chars without a null terminator */
    string[8] = '\0';
  }

  return;
}

void 
gp_coffgen_fetch_section_name(gp_object_type *object, 
                              struct scnhdr *header,
			      char *string)
{
  unsigned long index;

  if (header->s_name.ptr.s_zeros == 0) {
    index = header->s_name.ptr.s_offset;
    strncpy(string, &object->string_table[index], BUFSIZ);
  } else {
    strncpy(string, &header->s_name.name[0], 8);
    /* the name can occupy all 8 chars without a null terminator */
    string[8] = '\0';
  }

  return;
}

gp_section_type *
gp_coffgen_findsection(gp_object_type *object, char *name)
{
  gp_section_type *current = NULL;
  gp_section_type *found = NULL;
  char current_name[BUFSIZ];

  if (object == NULL)
    return NULL;

  current = object->sections;

  while (current != NULL) {
    gp_coffgen_fetch_section_name(object, &current->header, &current_name[0]);
    if (strcmp(&current_name[0], name) == 0) {
      found = current;
      break;
    }
    current = current->next;
  }

  return found;
}

int
gp_coffgen_addstring(gp_object_type *object, char *name)
{
  int nbytes;
  int offset;

  assert(name != NULL);

  /* read the number of bytes in the string table */
  offset = nbytes = _get_32(&object->string_table[0]);

  /* check the length against the max string table size */
  nbytes += strlen(name) + 1;
  assert(!(nbytes > MAX_STRING_TABLE));

  /* copy the string to the table */
  strcpy(&object->string_table[offset], name);
    
  /* write the new byte count */
  _put_32(&object->string_table[0], nbytes);

  return offset;
}

void
gp_coffgen_addname(gp_object_type *object, char *ptr, char *name)
{
  int length;
  int offset;

  if (name == NULL)
    return;
  
  length = strlen(name);
  
  if (length < 9) {
    /* The string will fit in the structure. */
    strncpy(ptr, name, 8);
  } else {
    offset = gp_coffgen_addstring(object, name);

    /* write zeros and offset */
    _put_32(&ptr[0], 0);
    _put_32(&ptr[4], offset);

  }

  return;
}

gp_section_type *
gp_coffgen_addsection(gp_object_type *object, char *name)
{
  gp_section_type *section = NULL;
  gp_section_type *list = NULL;

  if (object == NULL)
    return NULL;

  /* allocate memory for the section */
  section = (gp_section_type *)malloc(sizeof(*section));

  /* initialize section */
  section->number = 1;
  gp_coffgen_addname(object, &section->header.s_name.name[0], name);
  section->header.s_paddr   = 0;
  section->header.s_vaddr   = 0;
  section->header.s_size    = 0;
  section->header.s_scnptr  = 0;
  section->header.s_relptr  = 0;
  section->header.s_lnnoptr = 0;
  section->header.s_nreloc  = 0;
  section->header.s_nlnno   = 0;
  section->header.s_flags   = 0; 
  section->data             = i_memory_create();
  section->relocations      = NULL;
  section->line_numbers     = NULL;
  section->next             = NULL;

  if (object->sections == NULL) {
    /* the list is empty */
    object->sections = section;
  } else {
    /* append the new object to the end of the list */  
    list = object->sections;
    while (list->next != NULL) {
      list = list->next;
    }
    list->next = section;
    section->number = list->number + 1;
  }

  object->file_header.f_nscns++;

  return section;
}

gp_reloc_type *
gp_coffgen_addreloc(gp_section_type *section)
{
  gp_reloc_type *new = NULL;
  gp_reloc_type *list = NULL;

  /* allocate memory for the relocation */
  new = (gp_reloc_type *)malloc(sizeof(*new));

  new->relocation.r_vaddr  = 0;
  new->relocation.r_symndx = 0;
  new->relocation.r_offset = 0;
  new->relocation.r_type   = 0;    
  new->next                = NULL;

  if (section->relocations == NULL) {
    /* the list is empty */
    section->relocations = new;
  } else {
    /* append the new object to the end of the list */  
    list = section->relocations;
    while (list->next != NULL) {
      list = list->next;
    }
    list->next = new;
  }

  section->header.s_nreloc++;

  return new;
}

gp_linenum_type *
gp_coffgen_addlinenum(gp_section_type *section)
{
  gp_linenum_type *new = NULL;
  gp_linenum_type *list = NULL;

  /* allocate memory for the relocation */
  new = (gp_linenum_type *)malloc(sizeof(*new));

  new->linenumber.l_srcndx = 0; 
  new->linenumber.l_lnno   = 0;
  new->linenumber.l_paddr  = 0; 
  new->linenumber.l_flags  = 0;
  new->linenumber.l_fcnndx = 0; 
  new->next                = NULL;

  if (section->line_numbers == NULL) {
    /* the list is empty */
    section->line_numbers = new;
  } else {
    /* append the new object to the end of the list */  
    list = section->line_numbers;
    while (list->next != NULL) {
      list = list->next;
    }
    list->next = new;
  }

  section->header.s_nlnno++;

  return new;
}

gp_symbol_type *
gp_coffgen_findsymbol(gp_object_type *object, char *name)
{
  gp_symbol_type *current = NULL;
  gp_symbol_type *found = NULL;
  char current_name[BUFSIZ];

  if (object == NULL)
    return NULL;
  
  current = object->sym_table;

  while (current != NULL) {
    gp_coffgen_fetch_symbol_name(object, &current->symbol, &current_name[0]);
    if (strcmp(&current_name[0], name) == 0) {
      found = current;
      break;
    }
    current = current->next;
  }

  return found;
}

gp_symbol_type *
gp_coffgen_addsymbol(gp_object_type *object, char *name, int num_aux)
{
  gp_symbol_type *new = NULL;
  gp_symbol_type *list = NULL;
  int i;

  /* allocate memory for the relocation */
  new = (gp_symbol_type *)malloc(sizeof(*new));
  
  gp_coffgen_addname(object, &new->symbol.sym_name.name[0], name);
  new->symbol.value      = 0;
  new->symbol.sec_num    = 0;
  new->symbol.type       = 0;
  new->symbol.st_class   = 0;
  new->symbol.num_auxsym = num_aux;
  new->next              = NULL;

  if (object->sym_table == NULL) {
    /* the list is empty */
    object->sym_table = new;
  } else {
    /* append the new object to the end of the list */  
    list = object->sym_table;
    while (list->next != NULL) {
      list = list->next;
    }
    list->next = new;
  }

  /* add the auxillary symbols */
  for(i = 0; i < num_aux; i++) {
    gp_coffgen_addsymbol(object, NULL, 0);
  }

  object->file_header.f_nsyms += 1;

  return new;
}

int
gp_coffgen_free(gp_object_type *object)
{
  gp_section_type *section;
  gp_reloc_type *relocation;
  gp_linenum_type *line_number;
  gp_symbol_type *symbol;

  gp_section_type *old_section;
  gp_reloc_type *old_relocation;
  gp_linenum_type *old_line_number;
  gp_symbol_type *old_symbol;
  
  if (object == NULL)
    return 1;

  free(object->filename);
  
  section = object->sections;
  while (section != NULL) {
    i_memory_free(section->data);

    relocation = section->relocations;
    while (relocation != NULL) {
      old_relocation = relocation;
      relocation = relocation->next;
      free(old_relocation);
    }  

    line_number = section->line_numbers;
    while (line_number != NULL) {
      old_line_number = line_number;
      line_number = line_number->next;
      free(old_line_number);
    }  
  
    old_section = section;
    section = section->next;
    free(old_section);
  }
  
  symbol = object->sym_table;
  while (symbol != NULL) {
    old_symbol = symbol;
    symbol = symbol->next;
    free(old_symbol);
  }  

  free(object);

  return 0;
}

static void 
_fput_16 (int data, FILE *fp) 
{
  fputc((int)(data & 255), fp);
  fputc((int)((data >> 8) & 255), fp);

  return;
}

static void 
_fput_32(long data, FILE *fp) 
{
  fputc((int)(data & 255), fp);
  fputc((int)((data >> 8) & 255), fp);
  fputc((int)((data >> 16) & 255), fp);
  fputc((int)((data >> 24) & 255), fp);

  return;
}

/* write the file header */
static void 
_gp_coffgen_write_filehdr(struct filehdr *file_header, FILE *fp) 
{
  _fput_16(file_header->f_magic,  fp);
  _fput_16(file_header->f_nscns,  fp);
  _fput_32(file_header->f_timdat, fp);
  _fput_32(file_header->f_symptr, fp);
  _fput_32(file_header->f_nsyms,  fp);
  _fput_16(file_header->f_opthdr, fp);
  _fput_16(file_header->f_flags,  fp);

  return;
}

/* write the optional header */
static void 
_gp_coffgen_write_opthdr(struct opthdr *opt_header, FILE *fp) 
{
  _fput_16(opt_header->opt_magic,      fp);
  _fput_16(opt_header->vstamp,         fp);
  _fput_32(opt_header->proc_type,      fp);
  _fput_32(opt_header->rom_width_bits, fp);
  _fput_32(opt_header->ram_width_bits, fp);

  return;
}

/* write the section header */
static void 
_gp_coffgen_write_scnhdr(struct scnhdr *header, FILE *fp) 
{

  fputc(header->s_name.name[0], fp);
  fputc(header->s_name.name[1], fp);
  fputc(header->s_name.name[2], fp);
  fputc(header->s_name.name[3], fp);
  fputc(header->s_name.name[4], fp);
  fputc(header->s_name.name[5], fp);
  fputc(header->s_name.name[6], fp);
  fputc(header->s_name.name[7], fp);
  _fput_32(header->s_paddr, fp);
  _fput_32(header->s_vaddr, fp);
  _fput_32(header->s_size, fp);
  _fput_32(header->s_scnptr, fp);
  _fput_32(header->s_relptr, fp);
  _fput_32(header->s_lnnoptr, fp);
  _fput_16(header->s_nreloc, fp);
  _fput_16(header->s_nlnno, fp);
  _fput_32(header->s_flags, fp);

  return;
}

/* write the section data */
static void 
_gp_coffgen_write_data(gp_section_type *section, FILE *fp) 
{
  unsigned int org;
  unsigned int last;
  unsigned int data;
  
  org = section->header.s_paddr;

  if (section->header.s_flags & STYP_TEXT) {
    /* the section is executable, so each word is two bytes */
    last = section->header.s_paddr + (section->header.s_size / 2);
  } else {
    /* the section is data, so each word is one byte */
    last = section->header.s_paddr + section->header.s_size;
  }

  for (org = section->header.s_paddr; org < last; org++) {
    data = i_memory_get(section->data, org);
    assert((data & MEM_USED_MASK) != 0);

    if (section->header.s_flags & STYP_TEXT) {
      _fput_16(data & 0xffff, fp);
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
  struct reloc *relocation;

  while (current != NULL) {
    relocation = &current->relocation;
  
    _fput_32(relocation->r_vaddr,  fp);
    _fput_32(relocation->r_symndx, fp);
    _fput_16(relocation->r_offset, fp);
    _fput_16(relocation->r_type,   fp);
  
    current = current->next;
  }

  return;
}

/* write the section linenumbers */
static void 
_gp_coffgen_write_linenum(gp_section_type *section, FILE *fp) 
{
  gp_linenum_type *current = section->line_numbers;
  struct lineno *linenumber;

  while (current != NULL) {
    linenumber = &current->linenumber;

    _fput_32(linenumber->l_srcndx, fp);
    _fput_16(linenumber->l_lnno,   fp);
    _fput_32(linenumber->l_paddr,  fp); 
    _fput_16(linenumber->l_flags,  fp);
    _fput_32(linenumber->l_fcnndx, fp);

    current = current->next;
  }

  return;
}

/* write the symbol table */
static void 
_gp_coffgen_write_symbols(gp_object_type *object, FILE *fp) 
{
  gp_symbol_type *current;
  struct syment *symbol;

  current = object->sym_table;

  while(current != NULL) {
    symbol = &current->symbol;

    fputc(symbol->sym_name.name[0], fp);
    fputc(symbol->sym_name.name[1], fp);
    fputc(symbol->sym_name.name[2], fp);
    fputc(symbol->sym_name.name[3], fp);
    fputc(symbol->sym_name.name[4], fp);
    fputc(symbol->sym_name.name[5], fp);
    fputc(symbol->sym_name.name[6], fp);
    fputc(symbol->sym_name.name[7], fp);
    _fput_32(symbol->value, fp);
    _fput_16(symbol->sec_num, fp);
    _fput_16(symbol->type, fp);
    fputc(symbol->st_class, fp);
    fputc(symbol->num_auxsym, fp);
   
    current = current->next;
  }

  return;
}

static int
_has_data(gp_section_type *section)
{

  if (section->header.s_size == 0)
    return 0;

  if (section->header.s_flags & STYP_TEXT)
    return 1;

  if (section->header.s_flags & STYP_DATA)
    return 1;

  if (section->header.s_flags & STYP_DATA_ROM)
    return 1;

  return 0;
}

/* update all the coff pointers */
int
gp_coffgen_updateptr(gp_object_type *object)
{
  int loc = 0;
  gp_section_type *section = NULL;

  loc = FILE_HDR_SIZ + OPT_HDR_SIZ + (SEC_HDR_SIZ * object->file_header.f_nscns);

  /* update the data pointers in the section headers */
  section = object->sections;
  while (section != NULL) {
    section->header.s_scnptr  = 0;
    if (_has_data(section)) {
      section->header.s_scnptr = loc;
      loc += section->header.s_size;
    }
    section = section->next;
  }

  /* update the relocation pointers in the section headers */
  section = object->sections;
  while (section != NULL) {
    section->header.s_relptr  = 0;
    if (section->header.s_nreloc != 0) {
      section->header.s_relptr = loc;
      loc += (section->header.s_nreloc * RELOC_SIZ);
    }
    section = section->next;
  }

  /* update the line number pointers in the section headers */
  section = object->sections;
  while (section != NULL) {
    section->header.s_lnnoptr = 0;
    if (section->header.s_nlnno != 0) {
      section->header.s_lnnoptr = loc;
      loc += (section->header.s_nlnno * LINENO_SIZ);
    }    
    section = section->next;
  }

  /* update symbol table pointer */
  object->file_header.f_symptr = loc;

  return 0;
}

/* write the coff file */
int
gp_coffgen_writecoff(gp_object_type *object)
{
  FILE *coff;
  gp_section_type *section = NULL;
  
  coff = fopen(object->filename, "w");
  if (coff == NULL) {
    perror(object->filename);
    exit(1);
  }

  /* write the data to the file */
  _gp_coffgen_write_filehdr(&object->file_header, coff); 
  _gp_coffgen_write_opthdr(&object->opt_header, coff);

  section = object->sections;

  /* write section headers */
  while (section != NULL) {
    _gp_coffgen_write_scnhdr(&section->header, coff); 
    section = section->next;
  }

  /* write section data */
  section = object->sections;
  while (section != NULL) {
    if (_has_data(section)) {
      _gp_coffgen_write_data(section, coff); 
    }
    section = section->next;
  }

  /* write section relocations */
  section = object->sections;
  while (section != NULL) {
    if (section->header.s_nreloc != 0) {
      _gp_coffgen_write_reloc(section, coff); 
    }
    section = section->next;
  }

  /* write section line numbers */
  section = object->sections;
  while (section != NULL) {
    if (section->header.s_nlnno != 0) {
      _gp_coffgen_write_linenum(section, coff); 
    }    
    section = section->next;
  }

  /* write symbols */
  if (object->file_header.f_nsyms != 0) {
    _gp_coffgen_write_symbols(object, coff); 
  } 

  /* write string table */
  fwrite(&object->string_table, 1, _get_32(&object->string_table[0]), coff);
  
  fclose(coff);

  return 0;
}

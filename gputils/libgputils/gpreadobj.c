/* Read coff objects 
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
  assert(infile != NULL);
  
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
_read_file_header(struct filehdr *header, char *file)
{

  header->f_magic  = _get_16(&file[0]);
  header->f_nscns  = _get_16(&file[2]);
  header->f_timdat = _get_32(&file[4]);
  header->f_symptr = _get_32(&file[8]);
  header->f_nsyms  = _get_32(&file[12]);
  header->f_opthdr = _get_16(&file[16]);
  header->f_flags  = _get_16(&file[18]);
}

static void 
_read_opt_header(struct opthdr *header, char *file)
{
  header->opt_magic      = _get_16(&file[0]);
  header->vstamp         = _get_16(&file[2]);
  header->proc_type      = _get_32(&file[4]);
  header->rom_width_bits = _get_32(&file[8]);
  header->ram_width_bits = _get_32(&file[12]);
}

static void 
_read_sec_header(struct scnhdr *section, char *file)
{
  section->s_name.name[0] = file[0];  
  section->s_name.name[1] = file[1];
  section->s_name.name[2] = file[2]; 
  section->s_name.name[3] = file[3];
  section->s_name.name[4] = file[4];  
  section->s_name.name[5] = file[5];
  section->s_name.name[6] = file[6]; 
  section->s_name.name[7] = file[7]; 
  section->s_paddr        = _get_32(&file[8]);
  section->s_vaddr        = _get_32(&file[12]);
  section->s_size         = _get_32(&file[16]);
  section->s_scnptr       = _get_32(&file[20]);
  section->s_relptr       = _get_32(&file[24]);
  section->s_lnnoptr      = _get_32(&file[28]);
  section->s_nreloc       = _get_16(&file[32]);
  section->s_nlnno        = _get_16(&file[34]);
  section->s_flags        = _get_32(&file[36]); 
}

static void 
_read_reloc(struct reloc *relocation, char *file)
{
  relocation->r_vaddr   = _get_32(&file[0]);
  relocation->r_symndx  = _get_32(&file[4]);
  relocation->r_offset  = _get_16(&file[8]);
  relocation->r_type    = _get_16(&file[10]);
}

static void 
_read_lineno(struct lineno *line, char *file)
{

  line->l_srcndx = _get_32(&file[0]); 
  line->l_lnno   = _get_16(&file[4]);
  line->l_paddr  = _get_32(&file[6]); 
  line->l_flags  = _get_16(&file[10]);
  line->l_fcnndx = _get_32(&file[12]); 
}

static void 
_read_sections(gp_object_type *object, char *file)
{
  int i, j;
  char *section_ptr;
  char *loc;
  gp_section_type *current = NULL;
  gp_reloc_type *current_reloc = NULL;
  gp_linenum_type *current_linenum = NULL;
  unsigned int number;
  unsigned int value;

  /* move to the start of the section headers */
  section_ptr = file + FILE_HDR_SIZ + OPT_HDR_SIZ;

  for (i = 0; i < object->file_header.f_nscns; i++) {
    /* allocate memory for the section */
    if (i == 0) {
      /* initialize the linked list the first time */
      current = object->sections = (gp_section_type *)malloc(sizeof(*current));
    } else {
      current->next = (gp_section_type *)malloc(sizeof(*current));
      current = current->next;
    }

    current->number = i + 1;
    
    _read_sec_header(&current->header, section_ptr);

    section_ptr += SEC_HDR_SIZ;

    /* read the data */
    if ((current->header.s_size) && (current->header.s_scnptr)) {
      loc = &file[current->header.s_scnptr];
      number = current->header.s_size >> 1;
      current->data = i_memory_create();
      for (j = 0; j < number; j++) {
        value = MEM_USED_MASK | (unsigned int)_get_16(&loc[j * 2]);
        i_memory_put(current->data, current->header.s_paddr + j, value);
      }
    }

    /* read the relocations */
    if ((current->header.s_nreloc) && (current->header.s_relptr)) {
      loc = &file[current->header.s_relptr];
      number = current->header.s_nreloc;
      for (j = 0; j < number; j++) {
        if (j == 0) {
          current->relocations = (gp_reloc_type *)malloc(sizeof(*current_reloc));
          current_reloc = current->relocations;
        } else {
          current_reloc->next = (gp_reloc_type *)malloc(sizeof(*current_reloc));
          current_reloc = current_reloc->next;
        }
        _read_reloc(&current_reloc->relocation, loc);
        loc += RELOC_SIZ;
      }
    }

    /* read the line numbers */
    if ((current->header.s_nlnno) && (current->header.s_lnnoptr)) {
      loc = &file[current->header.s_lnnoptr];
      number = current->header.s_nlnno;
      for (j = 0; j < number; j++) {
        if (j == 0) {
          current->line_numbers = 
            (gp_linenum_type *)malloc(sizeof(*current_linenum));
          current_linenum = current->line_numbers;
        } else {
          current_linenum->next = 
            (gp_linenum_type *)malloc(sizeof(*current_linenum));
          current_linenum = current_linenum->next;
        }
        _read_lineno(&current_linenum->linenumber, loc);
        loc += LINENO_SIZ;
      }
    }
  }

}

static void 
_read_symbol(struct syment *symbol, char *file)
{
  symbol->sym_name.name[0]    = file[0];
  symbol->sym_name.name[1]    = file[1];
  symbol->sym_name.name[2]    = file[2];
  symbol->sym_name.name[3]    = file[3];
  symbol->sym_name.name[4]    = file[4];
  symbol->sym_name.name[5]    = file[5];
  symbol->sym_name.name[6]    = file[6];
  symbol->sym_name.name[7]    = file[7];
  symbol->value      = _get_32(&file[8]);
  symbol->sec_num    = _get_16(&file[12]);
  symbol->type       = _get_16(&file[14]);
  symbol->st_class   = file[16];
  symbol->num_auxsym = file[17];
}

static void 
_read_symtbl(gp_object_type *object, char *file)
{
  int i;
  int number = object->file_header.f_nsyms;
  gp_symbol_type *current = NULL; 

  if (number != 0) {
    file = &file[object->file_header.f_symptr];
    for (i = 0; i < number; i++) {
      if (i == 0) {
        object->sym_table = (gp_symbol_type *)malloc(sizeof(*current));
        current = object->sym_table;
      } else {
        current->next = (gp_symbol_type *)malloc(sizeof(*current));
        current = current->next;
      }         
      _read_symbol(&current->symbol, file);
      file += SYMBOL_SIZE;
    }
  }
}

static void 
_read_strtbl(gp_object_type *object, char *file)
{
  unsigned int address;
  unsigned int nbytes;
  
  address = object->file_header.f_symptr + 
            (SYMBOL_SIZE * object->file_header.f_nsyms);

  nbytes = _get_32(&file[address]);
  memcpy(&object->string_table[0], &file[address], nbytes);

}

gp_object_type *
gp_convert_file(char *file)
{
  gp_object_type *object;

  /* allocate memory for the object file */
  object = (gp_object_type *)malloc(sizeof(*object));

  _read_file_header(&object->file_header, file);
  _read_opt_header(&object->opt_header, file + FILE_HDR_SIZ);
  _read_sections(object, file);
  _read_symtbl(object, file);
  _read_strtbl(object, file);

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

  object = gp_convert_file(file->file);
  if (object != NULL)
    object->filename = strdup(filename);

  gp_free_file(file);

  return object;
}

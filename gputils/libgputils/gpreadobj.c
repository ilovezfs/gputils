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
#include "gpcoff.h"
#include "gpreadobj.h"
#include "gpsymbol.h"
#include "gparchive.h"

unsigned short get_16(unsigned char *addr)
{
  return (addr[1] << 8) | addr[0];
}

unsigned long get_32(unsigned char *addr)
{
  unsigned long value;

  value  = addr[0];
  value |= addr[1] << 8;
  value |= addr[2] << 16;
  value |= addr[3] << 24;

  return value;
}

void gp_fetch_symbol_name(struct objectfile *object, 
                          struct syment *coffsym,
			  char *string)
{
  unsigned long index;

  if (coffsym->sym_name.ptr.s_zeros == 0) {
    index = coffsym->sym_name.ptr.s_offset;
    strcpy(string, &object->strtbl[index]);
  } else {
    strncpy(string, &coffsym->sym_name.name[0], 8);
    /* the name can occupy all 8 chars without a null terminator */
    string[9] = '\0';
  }

  return;
}

void gp_fetch_section_name(struct objectfile *object, 
                           struct scnhdr *header,
			   char *string)
{
  unsigned long index;

  if (header->s_name.ptr.s_zeros == 0) {
    index = header->s_name.ptr.s_offset;
    strcpy(string, &object->strtbl[index]);
  } else {
    strncpy(string, &header->s_name.name[0], 8);
    /* the name can occupy all 8 chars without a null terminator */
    string[9] = '\0';
  }

  return;
}


void read_file_header(struct filehdr *header, unsigned char *file)
{

  header->f_magic  = get_16(&file[0]);
  header->f_nscns  = get_16(&file[2]);
  header->f_timdat = get_32(&file[4]);
  header->f_symptr = get_32(&file[8]);
  header->f_nsyms  = get_32(&file[12]);
  header->f_opthdr = get_16(&file[16]);
  header->f_flags  = get_16(&file[18]);
}

void read_opt_header(struct opthdr *header, unsigned char *file)
{
  header->opt_magic      = get_16(&file[0]);
  header->vstamp         = get_16(&file[2]);
  header->proc_type      = get_32(&file[4]);
  header->rom_width_bits = get_32(&file[8]);
  header->ram_width_bits = get_32(&file[12]);
}

void read_sec_header(struct scnhdr *section, unsigned char *file)
{
  section->s_name.name[0] = file[0];  
  section->s_name.name[1] = file[1];
  section->s_name.name[2] = file[2]; 
  section->s_name.name[3] = file[3];
  section->s_name.name[4] = file[4];  
  section->s_name.name[5] = file[5];
  section->s_name.name[6] = file[6]; 
  section->s_name.name[7] = file[7]; 
  section->s_paddr        = get_32(&file[8]);
  section->s_vaddr        = get_32(&file[12]);
  section->s_size         = get_32(&file[16]);
  section->s_scnptr       = get_32(&file[20]);
  section->s_relptr       = get_32(&file[24]);
  section->s_lnnoptr      = get_32(&file[28]);
  section->s_nreloc       = get_16(&file[32]);
  section->s_nlnno        = get_16(&file[34]);
  section->s_flags        = get_32(&file[36]); 
}

void read_reloc(struct reloc *relocation, unsigned char *file)
{
  relocation->r_vaddr   = get_32(&file[0]);
  relocation->r_symndx  = get_32(&file[4]);
  relocation->r_offset  = get_16(&file[8]);
  relocation->r_type    = get_16(&file[10]);
}

void read_lineno(struct lineno *line, unsigned char *file)
{

  line->l_srcndx = get_32(&file[0]); 
  line->l_lnno   = get_16(&file[4]);
  line->l_paddr  = get_32(&file[6]); 
  line->l_flags  = get_16(&file[10]);
  line->l_fcnndx = get_32(&file[12]); 
}

void read_sections(struct objectfile *object, unsigned char *file)
{
  int i, j;
  unsigned char *section_ptr;
  struct section *current = NULL;
  unsigned char *loc;
  unsigned int number;

  /* move to the start of the section headers */
  section_ptr = file + FILE_HDR_SIZ + OPT_HDR_SIZ;

  /* allocate memory for the sections */
  object->sections = (struct section *) malloc(object->file_header.f_nscns * 
                                               sizeof(struct section));

  for (i = 0; i < object->file_header.f_nscns; i++) {

    current = &object->sections[i];

    read_sec_header(&current->header, section_ptr);

    section_ptr += SEC_HDR_SIZ;

    if (current->header.s_size) {
      loc = &file[current->header.s_scnptr];
      number = current->header.s_size >> 1;
      current->data = (unsigned int *)malloc(number * sizeof(unsigned int)); 
      for (j = 0; j < number; j++)
        current->data[j] = (unsigned int)get_16(&loc[j * 2]); 
    }

    if (current->header.s_nreloc) {
      loc = &file[current->header.s_relptr];
      number = current->header.s_nreloc;
      current->relocations = (struct reloc *)malloc(number * 
                                                    sizeof(struct reloc)); 
      for (j = 0; j < number; j++) {
        read_reloc(&current->relocations[j], loc);
        loc += RELOC_SIZ;
      }
    }

    if (current->header.s_nlnno) {
      loc = &file[current->header.s_lnnoptr];
      number = current->header.s_nlnno;
      current->linenumbers = (struct lineno *)malloc(number * 
                                                     sizeof(struct lineno)); 
      for (j = 0; j < number; j++) {
        read_lineno(&current->linenumbers[j], loc);
        loc += LINENO_SIZ;
      }
    }

  }

}

void read_symbol(struct syment *symbol, unsigned char *file)
{
  symbol->sym_name.name[0]    = file[0];
  symbol->sym_name.name[1]    = file[1];
  symbol->sym_name.name[2]    = file[2];
  symbol->sym_name.name[3]    = file[3];
  symbol->sym_name.name[4]    = file[4];
  symbol->sym_name.name[5]    = file[5];
  symbol->sym_name.name[6]    = file[6];
  symbol->sym_name.name[7]    = file[7];
  symbol->value      = get_32(&file[8]);
  symbol->sec_num    = get_16(&file[12]);
  symbol->type       = get_16(&file[14]);
  symbol->st_class   = file[16];
  symbol->num_auxsym = file[17];
}


void read_symtbl(struct objectfile *object, unsigned char *file)
{
  int i;
  int number = object->file_header.f_nsyms;

  if (number) {
    file = &file[object->file_header.f_symptr];
  
    object->symtbl = (struct syment *)malloc(number * sizeof(struct syment));
  
    for (i = 0; i < number; i++) {
      read_symbol(&object->symtbl[i], file);
      file += SYMBOL_SIZE;
    }

  }

}

void read_strtbl(struct objectfile *object, unsigned char *file)
{
  unsigned int address;
  unsigned int nbytes;
  int i;
  
  address = object->file_header.f_symptr + 
            (SYMBOL_SIZE * object->file_header.f_nsyms);

  nbytes = get_32(&file[address]);
  object->strtbl = (char *)malloc(nbytes);

  for (i = 0; i < nbytes; i++)
    object->strtbl[i] = file[address + i];

}

int check_object(unsigned char *file)
{
  unsigned short magic;

  /* make sure the file is a coff object */  
  magic = ((unsigned char)file[1] << 8) + 
           (unsigned char)file[0];
  if (magic != MICROCHIP_MAGIC) {
    return 1;  
  }

  return 0;
}

/* convert a file stored in memory to a coff object */

struct objectfile *convert_object(unsigned char *file, char *message)
{
  struct objectfile *object;

  if (file == NULL)
    return NULL;

  if (check_object(file)) {
    sprintf(message, "File is not a coff object");
    return NULL;
  }
  
  /* allocate memory for the object file */
  object = (struct objectfile *)malloc(sizeof(*object));

  read_file_header(&object->file_header, file);
  read_opt_header(&object->opt_header, file + FILE_HDR_SIZ);
  read_sections(object, file);
  read_symtbl(object, file);
  read_strtbl(object, file);

  return object;
}

/* Read a binary file and store it in memory. */

struct binaryfile *readfile(char *filename, char *message)
{
  FILE *infile;
  struct binaryfile *file;

  if ( (infile = fopen(filename,"rb")) == NULL ) {
    sprintf(message, "Can't open file \"%s\"\n", filename);
    return NULL;
  }
  
  file = (struct binaryfile *)malloc(sizeof(*file));

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

struct objectfile *readobj(char *filename, char *message)
{
  struct binaryfile *file;
  struct objectfile *object;
  
  file = readfile(filename, message);
  if (file == NULL)
    return NULL;

  object = convert_object((unsigned char *)file->file, message);
  object->filename = strdup(filename);

  return object;

} 

/* FIXME: use this function for all coff file checks */

enum coff_types identify_coff_file(char *filename)
{
  FILE *file;
  char magic[SARMAG + 1];
  
  if ( (file = fopen(filename,"rb")) == NULL ) {
    return sys_err;
  }

  /* Read the magic number. Archive magic numbers are longest, so read
     their size */
  fread(&magic[0], 1, SARMAG, file);
  fclose(file); 

  if (((magic[1]<<8) + magic[0]) == MICROCHIP_MAGIC)
    return object;
    
  if (strncmp(magic, ARMAG, SARMAG) == 0)
    return archive;

  return unknown;
}

void freeobj(struct objectfile *object)
{
  int i;
  struct section *current;

  if (object == NULL)
    return;

  free(object->filename);
  for (i = 0; i < object->file_header.f_nscns; i++) {
    current = &object->sections[i];
    free(current->data);
    free(current->relocations);
    free(current->linenumbers);
  }
  free(object->sections);
  free(object->symtbl);
  free(object->strtbl);
  free(object);

  return;
}

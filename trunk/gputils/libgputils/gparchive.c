/* GNU PIC archive functions
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

/* FIXME: member headers always start on an even-byte boundary. A newline
   character is often used to fill the gap. */
 
int count_archive_members(struct coff_archive *archive)
{
  int number = 0;

  /* If present, skip the symbol index. */
  if (gp_archive_have_index(archive))
    archive = archive->next;
  
  while (archive != NULL) {
    number++;
    archive = archive->next;
  }

  return number;
}

void list_archive_members(struct coff_archive *archive)
{
  const char *format = "%-24s  %06i bytes  %s";
  char name[256];
  char *end;
  int date;
  time_t time;
  int size;

  /* If present, skip the symbol index. */
  if (gp_archive_have_index(archive))
    archive = archive->next;

  while (archive != NULL) {
    sscanf(archive->header.ar_name, "%s/", name);
    sscanf(archive->header.ar_date, "%il", &date);
    sscanf(archive->header.ar_size, "%il", &size);
    end = strchr(&name[0], '/');
    if (end != NULL) 
      *end = '\0';
    time = date;
    printf(format, name, size, ctime(&time));
    archive = archive->next;
  }

  return;
}

struct coff_archive *find_archive_member(struct coff_archive *archive, 
                                         char *objectname)
{
  char name[256];
  char *end;
  struct coff_archive *found = NULL;

  /* If present, skip the symbol index. */
  if (gp_archive_have_index(archive))
    archive = archive->next;

  while (archive != NULL) {
    sscanf(archive->header.ar_name, "%s/", name);
    end = strrchr(&name[0], '/');
    if (end != NULL) 
      *end = '\0';
    if (strcmp(objectname, name) == 0) {
      found = archive;
      break;
    }
    archive = archive->next;
  }

  return found;
}

int free_archive_member(struct coff_archive *archive)
{

  if (archive->file != NULL)
    free(archive->file);
  if (archive != NULL)
    free(archive);

  return 0;
}

struct coff_archive *delete_archive_member(struct coff_archive *archive, 
                                           char *objectname,
                                           char *message)
{
  struct coff_archive *object = NULL;
  struct coff_archive *list = NULL;
  
  object = find_archive_member(archive, objectname);

  if (object == NULL) {
    sprintf(message, "object \"%s\" not found", objectname);  
    return NULL;
  }

  if (object == archive) {
    /* the first object in the list is being deleted */
    archive = archive->next;
  } else {
    /* locate and remove the member */

    list = archive;
    while (list != NULL) {
      if (list->next == object) {
        list->next = object->next;
        break;
      }
      list = list->next;
    }  
  }

  free_archive_member(object);

  return archive;
}

struct coff_archive *add_archive_member(struct coff_archive *archive, 
                                        char *objectname,
                                        char *message)
{
  struct coff_archive *object = NULL;
  struct coff_archive *newmember = NULL;
  struct coff_archive *list = NULL;
  struct binaryfile *newobject = NULL;
  char name[256];
  char date[12];
  char size[10];
  int timer;
  unsigned short magic;
  
  newobject = readfile(objectname, message);

  if (newobject == NULL) {
    return NULL;
  }
 
  /* make sure the file is a coff object */  
  magic = ((unsigned char)newobject->file[1] << 8) + 
           (unsigned char)newobject->file[0];
  if (magic != MICROCHIP_MAGIC) {
    sprintf(message, "\"%s\" is not an object file", objectname);
    return NULL;  
  }

  newmember = (struct coff_archive *)malloc(sizeof(*newmember));
  newmember->next = NULL;

  /* Point the archive member file to the object file.  The object is never
     freed, so this is ok.  It will be cleaned up later */
  newmember->file = newobject->file;

  /* fill in the archive header */
  memset(&newmember->header, 0x20, AR_HDR_SIZ); /* fill the header with space */

  timer = (int)time(NULL);
  
  sprintf(name, "%s/", objectname);
  sprintf(date, "%il", timer);
  sprintf(size, "%lil", newobject->size);

  /* FIXME:  These functions over right the 0x20 that the header is filled 
     with. */ 
  strncpy(&newmember->header.ar_name[0], &name[0], 256);
  strncpy(&newmember->header.ar_date[0], &date[0], 12);
  strncpy(&newmember->header.ar_size[0], &size[0], 10);
  strncpy(&newmember->header.ar_fmag[0], ARMAG, 2);

  object = find_archive_member(archive, objectname);
  
  if (object != NULL) {
    /* the object already exists, replace it */
    archive = delete_archive_member(archive, objectname, message);
  }

  if (archive == NULL) {
    /* the list is empty */
    archive = newmember;
  } else {
    /* append the new object to the end of the list */  
    list = archive;
    while (list->next != NULL) {
      list = list->next;
    }
    list->next = newmember;
  }
  
  return archive;
}

int extract_archive_member(struct coff_archive *archive, 
                           char *objectname,
                           char *message)
{
  struct coff_archive *object = NULL;
  char filename[256];
  FILE *output_file;
  int size;

  object = find_archive_member(archive, objectname);

  if (object == NULL) {
    sprintf(message, "object \"%s\" not found", objectname);
    return 1;
  }

  /* if the object doesn't have an extension, add one.  This is done for
     some libs generated with other tools.  It should not be necessary
     for libs generated by gplib. */
  strcpy(filename, objectname);
  if (strrchr(filename, '.') == NULL)
    strcat(filename, ".o");

  output_file = fopen(filename, "wt");
  if (output_file == NULL) {
    sprintf(message, "can't write file \"%s\"", objectname);  
    return 1;
  }

  sscanf(object->header.ar_size, "%il", &size);
  fwrite(object->file, 1, size, output_file);

  fclose(output_file);

  /* FIXME: put the proper date on the output file */

  return 0;
}

int write_archive(struct coff_archive *archive, 
                  char *archivename,
                  char *message)
{
  FILE *output_file;
  int size;

  if (archive == NULL)
    assert(0);

  output_file = fopen(archivename, "wt");
  if (output_file == NULL) {
    sprintf(message, "can't write file \"%s\"", archivename);
    return 1;
  }

  fputs(ARMAG, output_file); /* write the archive magic number */

  while (archive != NULL) {
    fwrite(&archive->header, 1, AR_HDR_SIZ, output_file);
    sscanf(archive->header.ar_size, "%il", &size);
    fwrite(archive->file, 1, size, output_file);
    archive = archive->next;
  }

  fclose(output_file);

  return 0;
}

/* update the offset numbers for the archive, this is only required
   if a symbol index is created */

void gp_archive_update_offsets(struct coff_archive *archive)
{
  unsigned int offset = SARMAG;
  int size = 0;

  while (archive != NULL) {
    archive->offset = offset;
    sscanf(archive->header.ar_size, "%il", &size);
    offset += AR_HDR_SIZ + size;
    archive = archive->next;  
  }  

  return;
}

/* place the symbol from the archive symbol index in the archive symbol table */

void gp_archive_add_symbol(struct symbol_table *table,
                           char *name,
		           struct coff_archive *member)
{
  struct symbol *sym;

  /* Search the for the symbol.  If not found, then add it to 
     the global symbol table.  */
  sym = get_symbol(table, name);
  if (sym != NULL) {
    /* FIXME: an error should be generated here.  duplicate symbols */ 
    return;
  }  
  sym = add_symbol(table, name);
  annotate_symbol(sym, member);

  return;
}

/* This function reads the symbol index in the archive.  The symbols are
placed in the archive symbol table.  This table stores the name of the 
symbol and the archive member that the symbol is defined in. */ 

void gp_archive_read_index(struct symbol_table *table,
                           struct coff_archive *archive)
{
  int number = 0;
  int i;
  char *name;
  char *offset;
  int offset_value = 0;
  struct coff_archive *list;
  char *file;
  
  if (gp_archive_have_index(archive) != 1) {
    printf("error: this archive has no symbol index\n");
    return;
  }
  
  file = archive->file;
  
  /* read the number of symbols */
  number = get_32((unsigned char *)file);  
  
  /* set the pointers to the offsets and symbol names */
  offset = &file[AR_INDEX_NUMBER_SIZ];
  name = offset + (AR_INDEX_OFFSET_SIZ * number);
  
  for (i = 0; i < number; i++) {
    /* get the symbol offset from the symbol index */
    offset_value = get_32((unsigned char *)offset); 

    /* Locate the object file the symbol is defined in.  The both should
       have the same offset */
    list = archive;
    while (list != NULL) {
      if (list->offset == offset_value)
        break;    
      list = list->next;
    }
    
    if (list == NULL) {
      assert(0);
    }
    
    /* add the symbol to the archive symbol table */
    gp_archive_add_symbol(table, name, list);

    /* increment the pointers to the next symbol */
    name += strlen(name) + 1;
    offset += AR_INDEX_OFFSET_SIZ;
  }  

  return;
}

/* print the archive symbol table */

void gp_archive_print_table(struct symbol_table *table)
{
  struct symbol **lst, **ps, *s;
  int i;
  const char *format = "%-32s %s\n";
  struct coff_archive *member;
  char name[256];
  char *end;

  /* FIXME: This function is overkill.  It implements the necessary steps
     for what the linker needs, but the archive tool doesn't need anything
     this complicated. */
  
  if (table == NULL)
    return;
  
  /* sort the index */
  ps = lst = malloc(table->count * sizeof(lst[0]));
  for (i = 0; i < HASH_SIZE; i++)
    for (s = table->hash_table[i]; s; s = s->next) 
      *ps++ = s;
  assert(ps == &lst[table->count]);
  qsort(lst, table->count, sizeof(lst[0]), symbol_compare);    

  for (i = 0; i < table->count; i++) {
    /* determine the archive member the symbol is defined in */
    member = get_symbol_annotation(lst[i]);
    assert(member != NULL);
    /* determine the archive member name */
    sscanf(member->header.ar_name, "%s/", name);
    end = strchr(&name[0], '/');
    if (end != NULL) 
      *end = '\0';
    /* print it */
    printf(format, get_symbol_name(lst[i]), name);
  }

  return;
}

/* FIXME: use the readfile function then convert to archive */

struct coff_archive *read_coff_archive(char *filename, char *message)
{
  FILE *infile;
  struct coff_archive *archive = NULL;
  struct coff_archive *list = NULL;
  struct coff_archive *new = NULL;
  struct ar_hdr tmpheader;
  fpos_t position;
  int object_size;
  char armag[SARMAG + 1];
  unsigned short magic;
  int first_time = 1;

  if ( (infile = fopen(filename,"rb")) == NULL ) {
    sprintf(message, "Can't open archive \"%s\"", filename);
    return NULL;
  }

  fread(&armag[0], 1, SARMAG, infile); /* read the magic number */
  
  if (strncmp(armag, ARMAG, SARMAG) != 0) {
    if (armag[7] == 0x0d) {
      sprintf(message, "Malformed archive, strings in dos format");
    } else {
      sprintf(message, "\"%s\" is not a valid archive", filename);  
    }
    fclose(infile);
    return NULL;
  }

  while (feof(infile) == 0) {
    /* allocate space for the next archive member */
    new = (struct coff_archive *)malloc(sizeof(*new));
    new->next = NULL;

    /* read the archive header */
    fread(&new->header, AR_HDR_SIZ, 1, infile);

    /* read the object file or symbol index into memory */
    sscanf(new->header.ar_size, "%il", &object_size);
    new->file = (char *)malloc(sizeof(char)*object_size);
    fread(new->file, sizeof(char), object_size, infile); 

    if (ferror(infile)) {
      sprintf(message, "can't read archive \"%s\"", filename);  
      fclose(infile);
      return NULL;
    }

    if (!((first_time == 1) && (new->header.ar_name[0] == '/'))) {
      /* make sure the member is a coff object */  
      magic = ((unsigned char)new->file[1] << 8) + 
               (unsigned char)new->file[0];
      if (magic != MICROCHIP_MAGIC) {
        char name[256];
      
        sscanf(new->header.ar_name, "%s/", name);
        sprintf(message, "warning: archive member \"%s\" is not an object file",
	        name);
      }
    }

    /* insert the new member in the archive list */
    if (archive == NULL) {
      /* this is the first entry */
      archive = new;
      list = new;
    } else {
      list->next = new;
      list = new;
    }

    first_time = 0;

    /* Some malformed libs have a couple of extra bytes on the end.  The 
       while eof test passes, but there are no other members.  Test for 
       it. */ 
    fgetpos(infile, &position);
    fread(&tmpheader, AR_HDR_SIZ, 1, infile);
    if (feof(infile))
      break;  
    fsetpos(infile, &position);    

  }

  gp_archive_update_offsets(archive);

  fclose(infile);

  return archive;
}

/* FIXME: put this in a better place */
void gp_archive_put_int(char *buff, int value)
{
  buff[0] = value & 0xff;
  buff[1] = (value >> 8) & 0xff;
  buff[2] = (value >> 16) & 0xff;
  buff[3] = (value >> 24) & 0xff;
}

/* Create a symbol index for the archive.  This is always done to make
   sure duplicate symbols don't get into the library.  This data can 
   and should be stored in the file.  The only reason not to is if you
   need complaiblity with other tools.  */

int gp_archive_make_index(struct coff_archive *archive, 
                          struct symbol_table *definition,
			  char *message)
{
  struct objectfile *object = NULL;
  char name[256];
  char *end;

  /* If present, skip the symbol index. */
  if (gp_archive_have_index(archive))
    archive = archive->next;
        
  while (archive != NULL) {
    sscanf(archive->header.ar_name, "%s/", name);
    end = strchr(&name[0], '/');
    if (end != NULL) 
      *end = '\0';
    object = convert_object((unsigned char *)archive->file, message);
    if (object == NULL) {
      printf("DEBUG: This is the name \"%s\"\n", &name[0]);
      return 1;
    }
    object->filename = strdup(name); 
    gp_link_add_symbols(definition, NULL, object);
    /* FIXME:  This function is not working */
    /*freeobj(object); */
    archive = archive->next;  
  }

  return 0;
}

/* add the symbol index to the archive */
struct coff_archive *gp_archive_add_index(struct symbol_table *table,
                                          struct coff_archive *archive, 
                                          char *message)
{
  struct coff_archive *newmember = NULL;
  struct coff_archive *member = NULL;
  int i;
  struct symbol **lst, **ps, *s;
  struct coffsymbol *var;
  char size[10];
  char *ptr;
  long int indexsize = 0;
  char *symname;
  
  if ((archive == NULL) || (table == NULL))
    return NULL;
  
  /* sort the index */
  ps = lst = malloc(table->count * sizeof(lst[0]));
  for (i = 0; i < HASH_SIZE; i++)
    for (s = table->hash_table[i]; s; s = s->next) 
      *ps++ = s;
  assert(ps == &lst[table->count]);
  qsort(lst, table->count, sizeof(lst[0]), symbol_compare);  

  /* determine the symbol index size */
  indexsize = AR_INDEX_NUMBER_SIZ;
  for (i = 0; i < table->count; i++) {
    symname = get_symbol_name(lst[i]);
    indexsize += strlen(symname) + 1 + AR_INDEX_OFFSET_SIZ;
  }

  /* create a new member for the index and place it in the archive */
  newmember = (struct coff_archive *)malloc(sizeof(*newmember));
  newmember->file = (char *)malloc(sizeof(char)*indexsize);
  newmember->next = NULL;  

  /* fill in the archive header */
  memset(&newmember->header, 0x20, AR_HDR_SIZ); /* fill the header with space */

  newmember->header.ar_name[0] = '/';
  sprintf(size, "%lil", indexsize);

  strncpy(&newmember->header.ar_size[0], &size[0], 10);
  strncpy(&newmember->header.ar_fmag[0], ARMAG, 2);

  newmember->next = archive;
  archive = newmember;

  /* recalculate the file offsets for the symbol table */
  gp_archive_update_offsets(archive);  
  
  /* write the number of symbols to the member */
  ptr = archive->file;
  gp_archive_put_int(ptr, table->count);
  ptr += 4;
  
  /* write the offsets to the member */
  for (i = 0; i < table->count; i++) {
    var = get_symbol_annotation(lst[i]);
    member = find_archive_member(archive, var->filename);
    gp_archive_put_int(ptr, member->offset);
    ptr += 4;
  }

  /* write the symbol names to the member */
  for (i = 0; i < table->count; i++) {
    strcpy(ptr, get_symbol_name(lst[i]));
    ptr += strlen(get_symbol_name(lst[i])) + 1;
  }
  
  return archive;
}

/* Determine if the archive has a symbol index */

int gp_archive_have_index(struct coff_archive *archive)
{
  int have_index = 0;

  if ((archive != NULL) && (archive->header.ar_name[0] == '/'))
    have_index = 1;
    
  return have_index;
}

/* Remove the symbol index if one exists */ 

struct coff_archive *gp_archive_remove_index(struct coff_archive *archive)
{
  struct coff_archive *member = NULL;
  
  /* If present, remove the symbol index. */
  if (gp_archive_have_index(archive)) {
    member = archive;
    archive = archive->next;
    free_archive_member(member);
  }
    
  return archive;
}

char *gp_archive_member_name(struct coff_archive *archive)
{
  char name[256];
  char *end;

  sscanf(archive->header.ar_name, "%s/", name);
  /* FIXME: sscanf isn't working like I want it to.  strcpy would work just 
     as well */
  end = strchr(&name[0], '/');
  if (end != NULL) 
    *end = '\0';

  return strdup(name);
}



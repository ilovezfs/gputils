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
 
int 
gp_archive_count_members(gp_archive_type *archive)
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

/* This function is unused */

char *
gp_archive_member_name(gp_archive_type *archive)
{
  char name[256];
  char *end;

  sscanf(archive->header.ar_name, "%s/", name);
  end = strchr(&name[0], '/');
  if (end != NULL) 
    *end = '\0';

  return strdup(name);
}

void 
gp_archive_list_members(gp_archive_type *archive)
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

gp_archive_type *
gp_archive_find_member(gp_archive_type *archive, char *objectname)
{
  char name[256];
  char *end;
  gp_archive_type *found = NULL;

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

int 
gp_archive_free_member(gp_archive_type *archive)
{

  if (archive->file != NULL)
    free(archive->file);
  if (archive != NULL)
    free(archive);

  return 0;
}

gp_archive_type *
gp_archive_delete_member(gp_archive_type *archive, 
                         char *objectname)
{
  gp_archive_type *object = NULL;
  gp_archive_type *list = NULL;
  
  object = gp_archive_find_member(archive, objectname);
  assert(object != NULL);

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

  gp_archive_free_member(object);

  return archive;
}

gp_archive_type *
gp_archive_add_member(gp_archive_type *archive, 
                      char *objectname)
{
  gp_archive_type *oldmember = NULL;
  gp_archive_type *newmember = NULL;
  gp_archive_type *list = NULL;
  gp_binary_type *newobject = NULL;
  char name[256];
  char date[12];
  char size[10];
  int timer;

  newobject = gp_read_file(objectname);

  newmember = (gp_archive_type *)malloc(sizeof(*newmember));
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

  oldmember = gp_archive_find_member(archive, objectname);
  
  if (oldmember != NULL) {
    /* the object already exists, replace it */
    archive = gp_archive_delete_member(archive, objectname);
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

int 
gp_archive_extract_member(gp_archive_type *archive, 
                          char *objectname)
{
  gp_archive_type *object = NULL;
  char filename[256];
  FILE *output_file;
  int size;

  object = gp_archive_find_member(archive, objectname);
  assert(object != NULL);

  /* if the object doesn't have an extension, add one.  This is done for
     some libs generated with other tools.  It should not be necessary
     for libs generated by gplib. */
  strcpy(filename, objectname);
  if (strrchr(filename, '.') == NULL)
    strcat(filename, ".o");

  output_file = fopen(filename, "wt");
  if (output_file == NULL)
    return 1;

  sscanf(object->header.ar_size, "%il", &size);
  fwrite(object->file, 1, size, output_file);

  fclose(output_file);

  /* FIXME: put the proper date on the output file */

  return 0;
}

int 
gp_archive_write(gp_archive_type *archive, 
                 char *archivename)
{
  FILE *output_file;
  int size;

  assert(archive != NULL);

  output_file = fopen(archivename, "wt");
  if (output_file == NULL)
    return 1;

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

void 
gp_archive_update_offsets(gp_archive_type *archive)
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

/* read a coff archive and store it in memory */

gp_archive_type *
gp_archive_read(char *filename)
{
  FILE *infile;
  gp_archive_type *archive = NULL;
  gp_archive_type *list = NULL;
  gp_archive_type *new = NULL;
  struct ar_hdr tmpheader;
  fpos_t position;
  int object_size;
  char buffer[SARMAG + 1];

  infile = fopen(filename,"rb");
  assert(infile != NULL);

  fread(&buffer[0], 1, SARMAG, infile); /* read the magic number */
  if (strncmp(buffer, ARMAG, SARMAG) != 0) {
    fclose(infile);
    return NULL;
  }

  while (feof(infile) == 0) {
    /* allocate space for the next archive member */
    new = (gp_archive_type *)malloc(sizeof(*new));
    new->next = NULL;

    /* read the archive header */
    fread(&new->header, AR_HDR_SIZ, 1, infile);

    /* read the object file or symbol index into memory */
    sscanf(new->header.ar_size, "%il", &object_size);
    new->file = (char *)malloc(sizeof(char)*object_size);
    fread(new->file, sizeof(char), object_size, infile); 

    /* insert the new member in the archive list */
    if (archive == NULL) {
      /* this is the first entry */
      archive = new;
      list = new;
    } else {
      list->next = new;
      list = new;
    }

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

/* Determine if the archive has a symbol index */

int 
gp_archive_have_index(gp_archive_type *archive)
{

  if ((archive != NULL) && (archive->header.ar_name[0] == '/'))
    return 1;
    
  return 0;
}

/* Remove the symbol index if one exists */ 

gp_archive_type *
gp_archive_remove_index(gp_archive_type *archive)
{
  gp_archive_type *member = NULL;
  
  /* If present, remove the symbol index. */
  if (gp_archive_have_index(archive)) {
    member = archive;
    archive = archive->next;
    gp_archive_free_member(member);
  }
    
  return archive;
}

/* Create a symbol index for the archive.  This is always done to make
   sure duplicate symbols don't get into the library.  This data can 
   and should be stored in the file.  The only reason not to is if you
   need compatibility with other tools.  */

int 
gp_archive_make_index(gp_archive_type *archive, 
                      struct symbol_table *definition)
{
  gp_object_type *object = NULL;
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
    object = gp_convert_file(archive->file);
    assert(object != NULL);
    object->filename = strdup(name); 
    gp_link_add_symbols(definition, NULL, object);
    archive = archive->next;  
  }

  return 0;
}

/* add the symbol index to the archive */
gp_archive_type *
gp_archive_add_index(struct symbol_table *table,
                     gp_archive_type *archive)
{
  gp_archive_type *newmember = NULL;
  gp_archive_type *member = NULL;
  int i;
  struct symbol **lst, **ps, *s;
  gp_coffsymbol_type *var;
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
  newmember = (gp_archive_type *)malloc(sizeof(*newmember));
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
  _put_32(ptr, table->count);
  ptr += 4;
  
  /* write the offsets to the member */
  for (i = 0; i < table->count; i++) {
    var = get_symbol_annotation(lst[i]);
    member = gp_archive_find_member(archive, var->file->filename);
    _put_32(ptr, member->offset);
    ptr += 4;
  }

  /* write the symbol names to the member */
  for (i = 0; i < table->count; i++) {
    strcpy(ptr, get_symbol_name(lst[i]));
    ptr += strlen(get_symbol_name(lst[i])) + 1;
  }
  
  return archive;
}

/* place the symbol from the archive symbol index in the archive symbol table */

int 
gp_archive_add_symbol(struct symbol_table *table,
                      char *name,
                      gp_archive_type *member)
{
  struct symbol *sym;

  /* Search the for the symbol.  If not found, then add it to 
     the global symbol table.  */
  sym = get_symbol(table, name);
  if (sym != NULL) {
    return 1;
  }  
  sym = add_symbol(table, name);
  annotate_symbol(sym, member);

  return 0;
}

/* This function reads the symbol index in the archive.  The symbols are
placed in the archive symbol table.  This table stores the name of the 
symbol and the archive member that the symbol is defined in. */ 

void 
gp_archive_read_index(struct symbol_table *table,
                      gp_archive_type *archive)
{
  int number = 0;
  int i;
  char *name;
  char *offset;
  int offset_value = 0;
  gp_archive_type *list;
  char *file;
  
  assert(gp_archive_have_index(archive));
  
  file = archive->file;
  
  /* read the number of symbols */
  number = _get_32(file);  
  
  /* set the pointers to the offsets and symbol names */
  offset = &file[AR_INDEX_NUMBER_SIZ];
  name = offset + (AR_INDEX_OFFSET_SIZ * number);
  
  for (i = 0; i < number; i++) {
    /* get the symbol offset from the symbol index */
    offset_value = _get_32(offset); 

    /* Locate the object file the symbol is defined in.  The both should
       have the same offset */
    list = archive;
    while (list != NULL) {
      if (list->offset == offset_value)
        break;    
      list = list->next;
    }
    
    assert(list != NULL);
    
    /* add the symbol to the archive symbol table */
    gp_archive_add_symbol(table, name, list);

    /* increment the pointers to the next symbol */
    name += strlen(name) + 1;
    offset += AR_INDEX_OFFSET_SIZ;
  }  

  return;
}

/* print the archive symbol table */

void 
gp_archive_print_table(struct symbol_table *table)
{
  struct symbol **lst, **ps, *s;
  int i;
  const char *format = "%-32s %s\n";
  gp_archive_type *member;
  char name[256];
  char *end;
  
  assert(table != NULL);
  
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

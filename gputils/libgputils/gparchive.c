/* GNU PIC archive functions
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

/* FIXME: member headers always start on an even-byte boundary. A newline
   character is often used to fill the gap. */

int
gp_archive_count_members(const gp_archive_type *archive)
{
  int number = 0;

  /* If present, skip the symbol index. */
  if (gp_archive_have_index(archive)) {
    archive = archive->next;
  }

  while (archive != NULL) {
    number++;
    archive = archive->next;
  }

  return number;
}

/* This function is unused. */

char *
gp_archive_member_name(const gp_archive_type *archive)
{
  char name[256];
  char *end;

  sscanf(archive->header.ar_name, "%255s/", name);
  end = strchr(&name[0], '/');
  if (end != NULL) {
    *end = '\0';
  }

  return GP_Strdup(name);
}

void
gp_archive_list_members(const gp_archive_type *archive)
{
  char name[256];
  char *end;
  int date;
  time_t time;
  int size;

  /* If present, skip the symbol index. */
  if (gp_archive_have_index(archive)) {
    archive = archive->next;
  }

  while (archive != NULL) {
    sscanf(archive->header.ar_name, "%255s/", name);
    sscanf(archive->header.ar_date, "%il", &date);
    sscanf(archive->header.ar_size, "%il", &size);
    end = strchr(&name[0], '/');
    if (end != NULL) {
      *end = '\0';
    }
    time = date;
    printf("%-24s  %06i bytes  %s", name, size, ctime(&time));
    archive = archive->next;
  }
}

gp_archive_type *
gp_archive_find_member(gp_archive_type *archive, const char *objectname)
{
  char name[256];
  char *end;
  gp_archive_type *found = NULL;

  /* If present, skip the symbol index. */
  if (gp_archive_have_index(archive)) {
    archive = archive->next;
  }

  while (archive != NULL) {
    sscanf(archive->header.ar_name, "%255s/", name);
    end = strrchr(&name[0], '/');
    if (end != NULL) {
      *end = '\0';
    }

    if (strcmp(objectname, name) == 0) {
      found = archive;
      break;
    }
    archive = archive->next;
  }

  return found;
}

void
gp_archive_free_member(gp_archive_type *archive)
{
  if (archive->data.file != NULL) {
    free(archive->data.file);
  }

  if (archive != NULL) {
    free(archive);
  }
}

gp_archive_type *
gp_archive_delete_member(gp_archive_type *archive, const char *objectname)
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
gp_archive_add_member(gp_archive_type *archive, const char *filename, const char *objectname)
{
  gp_archive_type *oldmember = NULL;
  gp_archive_type *newmember = NULL;
  gp_archive_type *list = NULL;
  gp_binary_type *newobject = NULL;
  char name[256];
  char date[12];
  char size[10];
  int timer;

  newobject = gp_read_file(filename);

  newmember = (gp_archive_type *)GP_Malloc(sizeof(*newmember));
  newmember->next = NULL;

  /* Point the archive member file to the object file.  The object is never
     freed, so this is ok.  It will be cleaned up later */
  newmember->data = *newobject;

  /* fill in the archive header */
  memset(&newmember->header, ' ', AR_HDR_SIZ); /* fill the header with space */

  timer = (int)time(NULL);

  snprintf(name, sizeof(name), "%s/", objectname);
  snprintf(date, sizeof(date), "%il", timer);
  snprintf(size, sizeof(size), "%lil", newobject->size);

  /* FIXME: These functions overwrite the ' ' that the header is filled with. */
  strncpy(newmember->header.ar_name, &name[0], sizeof(newmember->header.ar_name));
  strncpy(newmember->header.ar_date, &date[0], sizeof(newmember->header.ar_date));
  strncpy(newmember->header.ar_size, &size[0], sizeof(newmember->header.ar_size));
  strncpy(newmember->header.ar_fmag, ARMAG,    sizeof(newmember->header.ar_fmag));

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
gp_archive_extract_member(gp_archive_type *archive, const char *objectname)
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
  snprintf(filename, sizeof(filename),
           "%s%s", objectname, (strrchr(filename, '.') == NULL) ? ".o" : "");

  output_file = fopen(filename, "wb");
  if (output_file == NULL) {
    perror(filename);
    exit(1);
  }

  sscanf(object->header.ar_size, "%il", &size);
  fwrite(object->data.file, 1, size, output_file);

  fclose(output_file);

  /* FIXME: put the proper date on the output file */

  return 0;
}

int
gp_archive_write(gp_archive_type *archive, const char *archivename)
{
  FILE *output_file;
  int size;

  assert(archive != NULL);

  output_file = fopen(archivename, "wb");
  if (output_file == NULL) {
    perror(archivename);
    exit(1);
  }

  fputs(ARMAG, output_file); /* write the archive magic number */

  while (archive != NULL) {
    fwrite(&archive->header, 1, AR_HDR_SIZ, output_file);
    sscanf(archive->header.ar_size, "%il", &size);
    fwrite(archive->data.file, 1, size, output_file);
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
}

/* read a coff archive and store it in memory */

gp_archive_type *
gp_archive_read(const char *filename)
{
  FILE *infile;
  gp_archive_type *archive = NULL;
  gp_archive_type *list = NULL;
  gp_archive_type *new = NULL;
  struct ar_hdr tmpheader;
  fpos_t position;
  int object_size;
  char buffer[SARMAG + 1];

  infile = fopen(filename, "rb");
  if (infile == NULL) {
    perror(filename);
    exit(1);
  }

  /* read the magic number */
  if ((fread(&buffer[0], 1, SARMAG, infile) != SARMAG) ||
      (strncmp(buffer, ARMAG, SARMAG) != 0)) {
    fclose(infile);
    return NULL;
  }

  while (feof(infile) == 0) {
    /* allocate space for the next archive member */
    new = (gp_archive_type *)GP_Malloc(sizeof(gp_archive_type));
    new->next = NULL;

    /* read the archive header */
    if (fread(&new->header, 1, AR_HDR_SIZ, infile) != AR_HDR_SIZ) {
      gp_error("bad archive \"%s\"", filename);
    }

    /* read the object file or symbol index into memory */
    sscanf(new->header.ar_size, "%il", &object_size);
    new->data.size = object_size;
    new->data.file = (unsigned char *)GP_Malloc(object_size);
    if (fread(new->data.file, sizeof(char), object_size, infile) != object_size) {
      gp_error("bad archive \"%s\"", filename);
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

    /* Some malformed libs have a couple of extra bytes on the end.  The
       while eof test passes, but there are no other members.  Test for
       it. */
    fgetpos(infile, &position);
    if (fread(&tmpheader, 1, AR_HDR_SIZ, infile) != AR_HDR_SIZ) {
      break;
    }
    fsetpos(infile, &position);
  }

  gp_archive_update_offsets(archive);

  fclose(infile);

  return archive;
}

/* Determine if the archive has a symbol index */

gp_boolean
gp_archive_have_index(const gp_archive_type *archive)
{
  return (((archive != NULL) && (archive->header.ar_name[0] == '/')) ? true : false);
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
  if (gp_archive_have_index(archive)) {
    archive = archive->next;
  }

  while (archive != NULL) {
    sscanf(archive->header.ar_name, "%255s/", name);
    end = strchr(&name[0], '/');
    if (end != NULL) {
      *end = '\0';
    }

    object = gp_convert_file(name, &archive->data);
    assert(object != NULL);
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
  const struct symbol **lst, **ps, *s;
  const gp_coffsymbol_type *var;
  char size[10];
  unsigned char *ptr;
  long int indexsize = 0;
  const char *symname;

  if ((archive == NULL) || (table == NULL)) {
    return NULL;
  }

  /* sort the index */
  ps = lst = GP_Malloc(table->count * sizeof(lst[0]));
  for (i = 0; i < HASH_SIZE; i++) {
    for (s = table->hash_table[i]; s; s = s->next) {
      *ps++ = s;
    }
  }
  assert(ps == &lst[table->count]);
  qsort(lst, table->count, sizeof(lst[0]), symbol_compare);

  /* determine the symbol index size */
  indexsize = AR_INDEX_NUMBER_SIZ;
  for (i = 0; i < table->count; i++) {
    symname = get_symbol_name(lst[i]);
    indexsize += strlen(symname) + 1 + AR_INDEX_OFFSET_SIZ;
  }

  /* create a new member for the index and place it in the archive */
  newmember = (gp_archive_type *)GP_Malloc(sizeof(*newmember));
  newmember->data.file = (unsigned char *)GP_Malloc(indexsize);
  newmember->data.size = indexsize;
  newmember->next = NULL;

  /* fill in the archive header */
  memset(&newmember->header, ' ', AR_HDR_SIZ); /* fill the header with space */

  newmember->header.ar_name[0] = '/';
  snprintf(size, sizeof(size), "%lil", indexsize);

  strncpy(newmember->header.ar_size, size,  sizeof(newmember->header.ar_size));
  strncpy(newmember->header.ar_fmag, ARMAG, sizeof(newmember->header.ar_fmag));

  newmember->next = archive;
  archive = newmember;

  /* recalculate the file offsets for the symbol table */
  gp_archive_update_offsets(archive);

  /* write the number of symbols to the member */
  ptr = archive->data.file;
  gp_putl32(ptr, table->count);
  ptr += 4;

  /* write the offsets to the member */
  for (i = 0; i < table->count; i++) {
    var = get_symbol_annotation(lst[i]);
    member = gp_archive_find_member(archive, var->file->filename);
    gp_putl32(ptr, member->offset);
    ptr += 4;
  }

  /* write the symbol names to the member */
  for (i = 0; i < table->count; i++) {
    const char *symbol_name = get_symbol_name(lst[i]);
    const size_t symbol_len = strlen(symbol_name) + 1;

    memcpy(ptr, symbol_name, symbol_len);
    ptr += symbol_len;
  }

  return archive;
}

/* place the symbol from the archive symbol index in the archive symbol table */

gp_boolean
gp_archive_add_symbol(struct symbol_table *table, const char *name,
                      gp_archive_type *member)
{
  struct symbol *sym;

  /* Search the for the symbol. If not found, then add it to
     the global symbol table. */
  sym = get_symbol(table, name);
  if (sym != NULL) {
    return true;
  }
  sym = add_symbol(table, name);
  annotate_symbol(sym, member);

  return false;
}

/* This function reads the symbol index in the archive.  The symbols are
placed in the archive symbol table.  This table stores the name of the
symbol and the archive member that the symbol is defined in. */

void
gp_archive_read_index(struct symbol_table *table, gp_archive_type *archive)
{
  int number = 0;
  int i;
  const char *name;
  const unsigned char *offset;
  int offset_value = 0;
  gp_archive_type *list;
  const unsigned char *file;

  assert(gp_archive_have_index(archive));

  file = archive->data.file;

  /* read the number of symbols */
  number = gp_getl32(file);

  /* set the pointers to the offsets and symbol names */
  offset = &file[AR_INDEX_NUMBER_SIZ];
  name = (const char *)offset + (AR_INDEX_OFFSET_SIZ * number);

  for (i = 0; i < number; i++) {
    /* get the symbol offset from the symbol index */
    offset_value = gp_getl32(offset);

    /* Locate the object file the symbol is defined in.  The both should
       have the same offset */
    list = archive;
    while (list != NULL) {
      if (list->offset == offset_value) {
        break;
      }
      list = list->next;
    }

    assert(list != NULL);

    /* add the symbol to the archive symbol table */
    gp_archive_add_symbol(table, name, list);

    /* increment the pointers to the next symbol */
    name += strlen(name) + 1;
    offset += AR_INDEX_OFFSET_SIZ;
  }
}

/* print the archive symbol table */

void
gp_archive_print_table(struct symbol_table *table)
{
  const struct symbol **lst, **ps, *s;
  int i;
  const gp_archive_type *member;
  char name[256];
  char *end;

  assert(table != NULL);

  /* sort the index */
  ps = lst = GP_Malloc(table->count * sizeof(lst[0]));
  for (i = 0; i < HASH_SIZE; i++) {
    for (s = table->hash_table[i]; s; s = s->next) {
      *ps++ = s;
    }
  }

  assert(ps == &lst[table->count]);
  qsort(lst, table->count, sizeof(lst[0]), symbol_compare);

  for (i = 0; i < table->count; i++) {
    /* determine the archive member the symbol is defined in */
    member = get_symbol_annotation(lst[i]);
    assert(member != NULL);
    /* determine the archive member name */
    sscanf(member->header.ar_name, "%255s/", name);
    end = strchr(&name[0], '/');
    if (end != NULL) {
      *end = '\0';
    }
    /* print it */
    printf("%-32s %s\n", get_symbol_name(lst[i]), name);
  }
}

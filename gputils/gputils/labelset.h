
/* Header to label list handler.

   Copyright (C) 2014 Molnar Karoly

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

#ifndef __LABELSET_H__
#define __LABELSET_H__

#define DSECTION_MAX            50
#define DSYMBOL_MAX             10000

#define SECT_NAME_GLOBAL        "GLOBAL"

enum {
  SECT_SPEC_CODE = 0,
  SECT_SPEC_DATA,
  SECT_SPEC_EEDATA
};

#define SECT_SPEC_MAX_NUM       (SECT_SPEC_EEDATA + 1)

enum {
  CSYM_START = (1 << 0),                /* The start of the symbol. */
  CSYM_END   = (1 << 1),                /* The symbol an area. */
  CSYM_DATA  = (1 << 2),                /* The symbol a data in the code area. */

  CSYM_ORG   = (1 << 15),               /* Internal use only unto the ORG directive. (Does not belong to the input parser.) */
  CSYM_USED  = (1 << 16)                /* The symbol used in the disassembled source. */
};

typedef struct lset_symbol {
  char *name;
  long start;
  long end;
  unsigned int attr;                    /* CSYM_yyyy */
  int line_number;
  struct lset_symbol *prev;
  struct lset_symbol *next;
} lset_symbol_t;

typedef struct lset_section {
  char *name;
  unsigned int symbol_number;
  int line_number;
  lset_symbol_t *symbol_first;
  lset_symbol_t *symbol_actual;
  lset_symbol_t *symbol_last;
  lset_symbol_t **symbol_table;
  struct lset_section *prev;
  struct lset_section *next;
} lset_section_t;

typedef struct lset_section_root {
  char *file_name;
  unsigned int is_data;
  unsigned int section_number;
  lset_section_t *section_global;
  lset_section_t *section_first;
  lset_section_t *section_actual;
  lset_section_t *section_last;

  lset_section_t *sections[SECT_SPEC_MAX_NUM];
} lset_section_root_t;

extern void lset_init(lset_section_root_t *Root, const char *File_name);

extern void lset_symbol_list(const lset_symbol_t *Symbol);
extern void lset_symbol_list_all(const lset_section_t *Section, int Use_table);
extern lset_symbol_t *lset_symbol_find(const lset_section_t *Section, const char *Name);
extern lset_symbol_t *lset_symbol_find_addr(const lset_section_t *Section, long Start_addr, long End_addr, int Use_table);

extern lset_symbol_t *lset_symbol_new(lset_section_t *Section, const char *Name, long Start, long End,
                                      unsigned int Attr, int Line_number);

extern void lset_symbol_make_table(lset_section_t *Section);
extern void lset_symbol_check_bounds(const lset_section_t *Section);
extern void lset_symbol_check_absolute_limits(const lset_section_t *Section, long Min, long Max);
extern void lset_symbol_check_align(const lset_section_t *Section, long Align);
extern void lset_symbol_free_all(lset_section_t *Section);

extern void lset_section_list(const lset_section_root_t *Root);
extern void lset_section_full_list(const lset_section_root_t *Root, int Use_table);
extern lset_section_t *lset_section_find(const lset_section_root_t *Root, const char *Name);
extern lset_section_t *lset_section_make_global(lset_section_root_t *Root);
extern lset_section_t *lset_section_new(lset_section_root_t *Root, const char *Name, int Line_number);
extern void lset_section_make_symbol_tables(lset_section_root_t *Root);
extern void lset_section_check_bounds(const lset_section_root_t *Root);
extern void lset_sections_choose(lset_section_root_t *Root);

extern void lset_delete(lset_section_root_t *Root);

#endif /* __LABELSET_H__ */

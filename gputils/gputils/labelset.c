
/* Label list handler.

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

#include "stdhdr.h"

#include "libgputils.h"
#include "labelset.h"

extern void yyerror(const char *Text, ...);

static const char *section_names[SECT_SPEC_MAX_NUM] = { "CODE", "DATA", "EEDATA" };

/*------------------------------------------------------------------------------------------------*/

void
lset_init(lset_section_root_t *Root, const char *File_name) {
  if (Root == NULL) {
    return;
  }

  Root->file_name      = (File_name != NULL) ? GP_Strdup(File_name) : NULL;
  Root->is_data        = 0;
  Root->section_number = 0;
  Root->section_global = NULL;
  Root->section_first  = NULL;
  Root->section_actual = NULL;
  Root->section_last   = NULL;
  Root->sections[SECT_SPEC_CODE]   = NULL;
  Root->sections[SECT_SPEC_DATA]   = NULL;
  Root->sections[SECT_SPEC_EEDATA] = NULL;
}

/*------------------------------------------------------------------------------------------------*/

void
lset_symbol_list(const lset_symbol_t *Symbol) {
  const char *type;

  type = (Symbol->attr & CSYM_DATA) ? " (DATA)" : "";

  if (Symbol->attr & CSYM_END) {
    printf("  [line %4i]: \"%s\" = %li:%li%s\n", Symbol->line_number, Symbol->name,
           Symbol->start, Symbol->end, type);
  }
  else {
    printf("  [line %4i]: \"%s\" = %li%s\n", Symbol->line_number, Symbol->name,
           Symbol->start, type);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
lset_symbol_list_all(const lset_section_t *Section, int Use_table) {
  unsigned int i;
  lset_symbol_t *sym;

  if ((Section == NULL) || (Section->symbol_number == 0) || ((sym = Section->symbol_first) == NULL)) {
    return;
  }

  if (Use_table && (Section->symbol_table != NULL)) {
    i = 0;
    
    do {
      lset_symbol_list(Section->symbol_table[i]);
      ++i;
    }
    while (i < Section->symbol_number);
  }
  else {
    do {
      lset_symbol_list(sym);
      sym = sym->next;
    }
    while (sym != NULL);
  }
}

/*------------------------------------------------------------------------------------------------*/

lset_symbol_t *
lset_symbol_find(const lset_section_t *Section, const char *Name) {
  lset_symbol_t *sym;

  if ((Section == NULL) || (Name == NULL) ||
      (Section->symbol_number == 0) || ((sym = Section->symbol_first) == NULL)) {
    return NULL;
  }

  do {
    if (strcmp(sym->name, Name) == 0) {
      break;
    }

    sym = sym->next;
  }
  while (sym != NULL);

  return sym;
}

/*------------------------------------------------------------------------------------------------*/

lset_symbol_t *
lset_symbol_find_addr(const lset_section_t *Section, long Start_addr, long End_addr, int Use_table)
{
  int i_min, i_mid, i_max;
  long sym_end, tmp;
  lset_symbol_t *sym;

  if ((Section == NULL) || (Section->symbol_number == 0)) {
    return NULL;
  }

  if (End_addr < 0) {
    End_addr = Start_addr;
  }
  else {
    if (Start_addr > End_addr) {
      tmp = Start_addr;
      Start_addr = End_addr;
      End_addr = tmp;
    }
  }

  if (Use_table) {
    if (Section->symbol_table == NULL) {
      return NULL;
    }

    i_min = 0;
    i_max = Section->symbol_number - 1;
    while (i_max >= i_min) {
      i_mid   = i_min + ((i_max - i_min) >> 1);
      sym     = Section->symbol_table[i_mid];
      sym_end = (sym->attr & CSYM_END) ? sym->end : sym->start;

      if (sym_end < End_addr) {
        i_min = i_mid + 1;
      }
      else if (sym->start > Start_addr) {
        i_max = i_mid - 1;
      }
      else {
        return sym;
      }
    }
  }
  else {
    if ((sym = Section->symbol_first) == NULL) {
      return NULL;
    }

    do {
      sym_end = (sym->attr & CSYM_END) ? sym->end : sym->start;

      if (((sym->start <= Start_addr) && (Start_addr <= sym->end)) ||
          ((sym->start <= End_addr)   && (End_addr   <= sym->end)) ||
          ((Start_addr <= sym->start) && (sym->start <= End_addr)) ||
          ((Start_addr <= sym->end)   && (sym->end   <= End_addr))) {
        /* Partial or complete overlap. */
        return sym;
      }

      sym = sym->next;
    }
    while (sym != NULL);
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

lset_symbol_t *
lset_symbol_new(lset_section_t *Section, const char *Name, long Start, long End, unsigned int Attr,
                int Line_number) {
  lset_symbol_t *sym;

  if (lset_symbol_find(Section, Name) != NULL) {
    yyerror("In line %i, already existing symbol: %s", Line_number, Name);
    exit(1);
  }

  if ((Attr & (CSYM_START | CSYM_END)) == (CSYM_START | CSYM_END)) {
    if (Start == End) {
      yyerror("In line %i, the \"start\" value (%li) equal the \"end\" value (%li) in the \"%s\" symbol!",
              Line_number, Start, End, Name);
      exit(1);
    }
    else if (Start > End) {
      yyerror("In line %i, the \"start\" value (%li) greater than the \"end\" value (%li) in the \"%s\" symbol!",
              Line_number, Start, End, Name);
      exit(1);
    }
  }

  if (Section == NULL) {
    yyerror("No existing section!");
    exit(1);
  }

  if (Section->symbol_number >= DSYMBOL_MAX) {
    yyerror("Out of memory.");
    exit(1);
  }

  sym = GP_Calloc(1, sizeof(lset_symbol_t));

  if (Section->symbol_first == NULL) {
    Section->symbol_first = sym;
  }

  if (Section->symbol_last != NULL) {
    Section->symbol_last->next = sym;
    sym->prev = Section->symbol_last;
  }

  Section->symbol_last   = sym;
  Section->symbol_actual = sym;
  ++Section->symbol_number;

  sym->name        = GP_Strdup(Name);
  sym->start       = Start;
  sym->end         = End;
  sym->attr        = Attr;
  sym->line_number = Line_number;
  return sym;
}

/*------------------------------------------------------------------------------------------------*/

static int
symbol_cmp(const void *P0, const void *P1) {
  const lset_symbol_t *sym0 = *(const lset_symbol_t **)P0;
  const lset_symbol_t *sym1 = *(const lset_symbol_t **)P1;
  long start0 = sym0->start;
  long start1 = sym1->start;

  if (start0 < start1) {
    return -1;
  }
  else if (start0 > start1) {
    return 1;
  }
  else {
    return 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

void lset_symbol_make_table(lset_section_t *Section) {
  unsigned int i;
  lset_symbol_t *sym;

  if ((Section == NULL) || (Section->symbol_number == 0) || ((sym = Section->symbol_first) == NULL)) {
    return;
  }

  if (Section->symbol_table != NULL) {
    free(Section->symbol_table);
  }

  Section->symbol_table = GP_Calloc(Section->symbol_number, sizeof(lset_symbol_t *));

  i = 0;
  do {
    Section->symbol_table[i] = sym;
    ++i;
    sym = sym->next;
  }
  while ((sym != NULL) && (i < Section->symbol_number));

  if (i != Section->symbol_number) {
    yyerror("Wrong the number of symbols: %u != %u", i != Section->symbol_number);
    exit(1);
  }

  qsort(Section->symbol_table, Section->symbol_number, sizeof(lset_symbol_t *), symbol_cmp);
  }

/*------------------------------------------------------------------------------------------------*/

static void
check_bounds(const lset_section_t *Section, const lset_symbol_t *Symbol) {
  unsigned int i;
  unsigned int k;
  const lset_symbol_t *sym;

  i = 0;
  do {
    sym = Section->symbol_table[i];
    ++i;

    if (sym != Symbol) {
      k  = (sym->attr    & CSYM_END) ? 2 : 0;
      k |= (Symbol->attr & CSYM_END) ? 1 : 0;

      switch (k) {
        case 0:
        default: {
	  if (sym->start == Symbol->start) {
            yyerror("Two symbols there is at same address: ([line %i]: \"%s\"{%li}) == ([line %i]: \"%s\"{%li})",
                    sym->line_number, sym->name, sym->start,
                    Symbol->line_number, Symbol->name, Symbol->start);
            exit(1);
          }
          break;
        }

        case 1: {
          /* The Symbol is a range. */
          if ((Symbol->start <= sym->start) && (sym->start <= Symbol->end)) {
            yyerror("There is a symbol the area of an other symbol: ([line %i]: \"%s\"{%li:%li}) == ([line %i]: \"%s\"{%li})",
                    Symbol->line_number, Symbol->name, Symbol->start, Symbol->end,
                    sym->line_number, sym->name, sym->start);
            exit(1);
          }
          break;
        }

        case 2: {
          /* The sym is a range. */
          if ((sym->start <= Symbol->start) && (Symbol->start <= sym->end)) {
            yyerror("There is a symbol the area of an other symbol: ([line %i]: \"%s\"{%li:%li}) == ([line %i]: \"%s\"{%li})",
                    sym->line_number, sym->name, sym->start, sym->end,
                    Symbol->line_number, Symbol->name, Symbol->start);
            exit(1);
          }
          break;
        }

        case 3: {
          /* The sym and the Symbol is a range. */
          if (((sym->start <= Symbol->start) && (Symbol->start <= sym->end)) ||
              ((sym->start <= Symbol->end)   && (Symbol->end   <= sym->end)) ||
              ((Symbol->start <= sym->start) && (sym->start <= Symbol->end)) ||
              ((Symbol->start <= sym->end)   && (sym->end   <= Symbol->end))) {
            yyerror("Overlap there is areas of this symbols: ([line %i]: \"%s\"{%li:%li}) -- ([line %i]: \"%s\"{%li:%li})",
                    sym->line_number, sym->name, sym->start, sym->end,
                    Symbol->line_number, Symbol->name, Symbol->start, Symbol->end);
            exit(1);
          }
          break;
        }
      }
    }
  } while (i < Section->symbol_number);
}

/*------------------------------------------------------------------------------------------------*/

void
lset_symbol_check_bounds(const lset_section_t *Section) {
  unsigned int i;

  if ((Section == NULL) || (Section->symbol_number == 0) || (Section->symbol_table == NULL)) {
    return;
  }

  i = 0;
  do {
    check_bounds(Section, Section->symbol_table[i]);
    ++i;
  } while (i < Section->symbol_number);
}

/*------------------------------------------------------------------------------------------------*/

void
lset_symbol_check_absolute_limits(const lset_section_t *Section, long Min, long Max) {
  unsigned int i;
  const lset_symbol_t *sym;

  if ((Section == NULL) || (Section->symbol_number == 0) || (Section->symbol_table == NULL)) {
    return;
  }

  i = 0;
  do {
    sym = Section->symbol_table[i];
    ++i;

    if (sym->attr & CSYM_END) {
      if (sym->start < Min) {
        yyerror("The starting address of area of the symbol is less than the minimum value: ([line %i]: \"%s\"{%li}) < %li",
                sym->line_number, sym->name, sym->start, Min);
        exit(1);
      }
      else if (sym->start > Max) {
        yyerror("The starting address of area of the symbol is greater than the minimum value: ([line %i]: \"%s\"{%li}) > %li",
                sym->line_number, sym->name, sym->start, Max);
        exit(1);
      }
      else if (sym->end < Min) {
        yyerror("The ultimate address of area of the symbol is less than the maximum value: ([line %i]: \"%s\"{%li}) < %li",
                sym->line_number, sym->name, sym->end, Min);
        exit(1);
      }
      else if (sym->end > Max) {
        yyerror("The ultimate address of area of the symbol is greater than the maximum value: ([line %i]: \"%s\"{%li}) > %li",
                sym->line_number, sym->name, sym->end, Max);
        exit(1);
      }
    }
    else {
      if (sym->start < Min) {
        yyerror("The address of symbol is less than the minimum value: ([line %i]: \"%s\"{%li}) < %li",
                sym->line_number, sym->name, sym->start, Min);
        exit(1);
      }
      else if (sym->start > Max) {
        yyerror("The address of symbol is greater than the maximum value: ([line %i]: \"%s\"{%li}) > %li",
                sym->line_number, sym->name, sym->start, Max);
        exit(1);
      }
    }
  }
  while (i < Section->symbol_number);
}

/*------------------------------------------------------------------------------------------------*/

void
lset_symbol_check_align(const lset_section_t *Section, long Align) {
  unsigned int i;
  long aligned;
  const lset_symbol_t *sym;

  if ((Section == NULL) || (Section->symbol_number == 0) || (Section->symbol_table == NULL)) {
    return;
  }

  i = 0;
  do {
    sym = Section->symbol_table[i];
    ++i;

    if (sym->attr & CSYM_END) {
      if ((sym->start % Align) != 0) {
        yyerror("The starting address of area of symbol not aligned: ([line %i]: \"%s\"{%li}) != %li",
                sym->line_number, sym->name, sym->start, (sym->start / Align) * Align);
        exit(1);
      }

      aligned = ((sym->end / Align) * Align) + Align - 1;

      if (sym->end != aligned) {
        yyerror("The ultimate address of area of the symbol not aligned: ([line %i]: \"%s\"{%li}) != %li",
                sym->line_number, sym->name, sym->end, aligned);
        exit(1);
      }
    }
    else {
      if ((sym->start % Align) != 0) {
        yyerror("The address of symbol not aligned: ([line %i]: \"%s\"{%li}) != %li",
                sym->line_number, sym->name, sym->start, (sym->start / Align) * Align);
        exit(1);
      }
    }
  }
  while (i < Section->symbol_number);
}

/*------------------------------------------------------------------------------------------------*/

void
lset_symbol_free_all(lset_section_t *Section) {
  lset_symbol_t *sym, *s;

  if (Section == NULL) {
    return;
  }

  if (Section->symbol_number > 0) {
    if ((sym = Section->symbol_first) != NULL) {
      do {
        s = sym->next;
        free(sym->name);
        free(sym);
        sym = s;
      }
      while (sym != NULL);
    }

    Section->symbol_number = 0;
  }

  Section->symbol_first  = NULL;
  Section->symbol_actual = NULL;
  Section->symbol_last   = NULL;

  if (Section->symbol_table != NULL) {
    free(Section->symbol_table);
    Section->symbol_table = NULL;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
lset_section_list(const lset_section_root_t *Root) {
  const lset_section_t *sect;

  if ((Root == NULL) || (Root->section_number == 0) || ((sect = Root->section_first) == NULL)) {
    return;
  }

  do {
    printf("\"%s\"\n", sect->name);
    sect = sect->next;
  }
  while (sect != NULL);
}

/*------------------------------------------------------------------------------------------------*/

void
lset_section_full_list(const lset_section_root_t *Root, int Use_table) {
  const lset_section_t *sect;

  if (Root == NULL) {
    return;
  }

  if ((sect = Root->section_global) != NULL) {
    printf("\"%s\"\n", sect->name);
    lset_symbol_list_all(sect, Use_table);
  }

  if ((Root->section_number == 0) || ((sect = Root->section_first) == NULL)) {
    return;
  }

  do {
    printf("\"%s\"\n", sect->name);
    lset_symbol_list_all(sect, Use_table);
    sect = sect->next;
  }
  while (sect != NULL);
}

/*------------------------------------------------------------------------------------------------*/

lset_section_t *
lset_section_find(const lset_section_root_t *Root, const char *Name) {
  lset_section_t *sect;

  if ((Root == NULL) || (Root->section_number == 0) || ((sect = Root->section_first) == NULL)) {
    return NULL;
  }

  do {
    if (strcmp(sect->name, Name) == 0) {
      break;
    }

    sect = sect->next;
  }
  while (sect != NULL);

  return sect;
}

/*------------------------------------------------------------------------------------------------*/

lset_section_t *
lset_section_make_global(lset_section_root_t *Root) {
  lset_section_t *sect;

  if (Root == NULL) {
    return NULL;
  }

  if (Root->section_global != NULL) {
    return Root->section_global;
  }

  sect = GP_Calloc(1, sizeof(lset_section_t));

  Root->section_global = sect;
  sect->name = GP_Strdup(SECT_NAME_GLOBAL);
  return sect;
}

/*------------------------------------------------------------------------------------------------*/

static unsigned int
find_section_name(const char *Name) {
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE(section_names); ++i) {
    if (strcmp(Name, section_names[i]) == 0) {
      return 1;
    }
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

static unsigned int
print_section_names(char *Text, unsigned int MaxLen) {
  unsigned int i, len;
  const char *fmt;

  len = 0;
  for (i = 0; i < ARRAY_SIZE(section_names); ++i) {
    switch (i) {
      case (ARRAY_SIZE(section_names) - 1): {
        fmt = "%s";
        break;
      }

      case (ARRAY_SIZE(section_names) - 2): {
        fmt = "%s or ";
        break;
      }

      default: {
        fmt = "%s, ";
      }
    }

    len += snprintf(&Text[len], MaxLen - len, fmt, section_names[i]);
  }

  return len;
}

/*------------------------------------------------------------------------------------------------*/

lset_section_t *
lset_section_new(lset_section_root_t *Root, const char *Name, int Line_number) {
  lset_section_t *sect;
  char buf[BUFSIZ];

  if ((Root == NULL) || (Root->section_number >= DSECTION_MAX) || (Name == NULL)) {
    return NULL;
  }

  if (strcmp(Name, SECT_NAME_GLOBAL) == 0) {
    yyerror("In line %i, the section name is reserved: %s", Line_number, Name);
    exit(1);
  }

  if (! find_section_name(Name)) {
    print_section_names(buf, sizeof(buf));
    yyerror("In line %i, the name of section (%s) only can be: %s", Line_number, Name, buf);
    exit(1);
  }

  if (lset_section_find(Root, Name) != NULL) {
    return NULL;
  }

  if ((sect = GP_Calloc(1, sizeof(lset_section_t))) == NULL) {
    return NULL;
  }

  if (Root->section_first == NULL) {
    Root->section_first = sect;
  }

  if (Root->section_last != NULL) {
    Root->section_last->next = sect;
    sect->prev = Root->section_last;
  }

  Root->section_last   = sect;
  Root->section_actual = sect;
  ++Root->section_number;

  sect->name = GP_Strdup(Name);
  sect->line_number = Line_number;
  return sect;
}

/*------------------------------------------------------------------------------------------------*/

void
lset_section_make_symbol_tables(lset_section_root_t *Root) {
  lset_section_t *sect, *s;

  if (Root == NULL) {
    return;
  }

  if ((sect = Root->section_global) != NULL) {
    lset_symbol_make_table(sect);
  }

  if (Root->section_number > 0) {
    if ((sect = Root->section_first) != NULL) {
      do {
        s = sect->next;
        lset_symbol_make_table(sect);
        sect = s;
      }
      while (sect != NULL);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
lset_section_check_bounds(const lset_section_root_t *Root) {
  const lset_section_t *sect;

  if (Root == NULL) {
    return;
  }

  if (Root->section_number > 0) {
    if ((sect = Root->section_first) != NULL) {
      do {
        lset_symbol_check_bounds(sect);
        sect = sect->next;
      }
    while (sect != NULL);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
lset_sections_choose(lset_section_root_t *Root)
  {
  unsigned int i;

  if (Root == NULL) {
    return;
  }

  for (i = 0; i < ARRAY_SIZE(section_names); ++i) {
    Root->sections[i] = lset_section_find(Root, section_names[i]);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
delete_section(lset_section_t *Section) {
  lset_symbol_free_all(Section);
  free(Section->name);
  free(Section);
}

/*------------------------------------------------------------------------------------------------*/

void
lset_delete(lset_section_root_t *Root) {
  lset_section_t *sect, *s;

  if (Root == NULL) {
    return;
  }

  if ((sect = Root->section_global) != NULL) {
    delete_section(sect);
    Root->section_global = NULL;
  }

  if (Root->section_number > 0) {
    if ((sect = Root->section_first) != NULL) {
      do {
        s = sect->next;
        delete_section(sect);
        sect = s;
      }
      while (sect != NULL);
    }

    Root->section_number = 0;
  }

  if (Root->file_name != NULL) {
    free(Root->file_name);
    Root->file_name = NULL;
  }

  Root->is_data        = 0;
  Root->section_first  = NULL;
  Root->section_actual = NULL;
  Root->section_last   = NULL;
  Root->sections[SECT_SPEC_CODE]   = NULL;
  Root->sections[SECT_SPEC_DATA]   = NULL;
  Root->sections[SECT_SPEC_EEDATA] = NULL;
}

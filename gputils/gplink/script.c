/* Linker script processing
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
#include "gplink.h"
#include "scan.h"
#include "script.h"

#define HEAD(L) (L)->value.list.head
#define TAIL(L) (L)->value.list.tail

#define NELEM(x)  (sizeof(x) / sizeof(x)[0])

void
script_error(const char *messg, const char *detail)
{
  gp_num_errors++;
  if (!gp_quiet) {
    if (state.src == NULL) {
      printf("%s\n", messg);
    }
    else if (detail == NULL) {
      printf("%s:%d:Error %s\n",
             state.src->name,
             state.src->line_number,
             messg);
    }
    else {
      printf("%s:%d:Error %s (%s)\n",
             state.src->name,
             state.src->line_number,
             messg,
             detail);
    }
  }
}

static int enforce_simple(struct pnode *p)
{
  if (p->tag == PTAG_SYMBOL) {
    return 1;
  }
  else {
    script_error("illegal argument", NULL);
    return 0;
  }
}

static long evaluate(struct pnode *p)
{

  switch (p->tag) {
  case PTAG_CONSTANT:
    return p->value.constant;

  case PTAG_SYMBOL:
    return get_script_macro(p->value.symbol);

  default:
    script_error("illegal argument", NULL);
    return 0;
  }

}

static int do_files(const char *name, enum section_type type, struct pnode *parms)
{
  struct pnode *p;

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if (enforce_simple(p)) {
      gplink_open_coff(p->value.symbol);
    }
  }

  return 0;
}

static int do_include(const char *name, enum section_type type, struct pnode *parms)
{
  struct pnode *p;

  /* FIXME: make sure only one argument is passed */
  p = HEAD(parms);

  if (enforce_simple(p)) {
    open_src(p->value.symbol, true);
  }

  return 0;
}

int
add_path(struct pnode *parms)
{
  struct pnode *p;

  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if (enforce_simple(p)) {
      /* gplink doesn't distinguish between lib pathes and lkr pathes.  There
         is one pathes list.  This shouldn't cause any problems. */
      gplink_add_path(p->value.symbol);
    }
  }

  return 0;
}

void add_script_macro(const char *name, long value)
{
  struct symbol *sym;
  long *val;

  sym = add_symbol(state.script_symbols, name);
  val = get_symbol_annotation(sym);

  if (val == NULL) {
    val = GP_Malloc(sizeof value);
    annotate_symbol(sym, val);
  }

  *val = value;
}

long get_script_macro(const char *name)
{
  struct symbol *sym;
  long *val;

  sym = get_symbol(state.script_symbols, name);

  if (sym == NULL) {
    return 0;
  }

  val = get_symbol_annotation(sym);
  return *val;
}

/* FIXME:  These functions were written to allow the user the greatest
   flexibility.  The arguments can appear in any order.  In hind sight
   this may not have been necessary and resulted in overly complicated
   code. */

/* logical section definition */
static int do_logsec(const char *name, enum section_type type, struct pnode *parms)
{
  enum arg_id_e {
    ID_UNKNOWN = 0,
    ID_NAME = 1,
    ID_RAM,
    ID_ROM
  } arg;
  static struct {
    enum arg_id_e id;
    const char *name;
  }
  arg_tbl[] = {
    { ID_NAME, "name" },
    { ID_RAM,  "ram"  },
    { ID_ROM,  "rom"  }
  };
  struct pnode *p;
  gp_boolean found_secname = false;
  gp_boolean found_ram = false;
  gp_boolean found_rom = false;
  const char *logical_section_name = NULL;
  char *section_name = NULL;
  struct linker_section *section = NULL;
  struct symbol *sym;

  /* read the options */
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        const char *lhs = p->value.binop.p0->value.symbol;
        int i;

        arg = ID_UNKNOWN;
        for (i = 0; i < NELEM(arg_tbl); ++i) {
          if (strcasecmp(arg_tbl[i].name, lhs) == 0) {
            arg = arg_tbl[i].id;
          }
        }

        switch (arg) {
        case ID_NAME:
          if (enforce_simple(p->value.binop.p1)) {
            found_secname = true;
            logical_section_name = p->value.binop.p1->value.symbol;
          }
          break;

        case ID_RAM:
          if (enforce_simple(p->value.binop.p1)) {
            found_ram = true;
            section_name = GP_Strdup(p->value.binop.p1->value.symbol);
          }
          break;

        case ID_ROM:
          if (enforce_simple(p->value.binop.p1)) {
            found_rom = true;
            section_name = GP_Strdup(p->value.binop.p1->value.symbol);
          }
          break;

        default:
          script_error("illegal argument", lhs);
        }
      }
    }
    else {
      if (enforce_simple(p)) {
        script_error("illegal argument", p->value.symbol);
      }
    }
  }

  /* process the options */
  if (!found_secname) {
    script_error("missing argument", "name");
  }
  else if (found_rom && found_ram) {
    script_error("too many arguments", "ram or rom");
  }
  else if ((!found_rom) && (!found_ram)) {
    script_error("missing argument", "ram or rom");
  }
  else {
    sym = get_symbol(state.section.definition, section_name);

    if (sym == NULL) {
      script_error("undefined section", section_name);
    }
    else {
      section = get_symbol_annotation(sym);
      assert(section != NULL);

      if ((found_ram == 1) && (section->type == SECT_CODEPAGE)) {
        script_error("invalid argument", "ram");
      }
      else if ((found_rom == 1) && (section->type != SECT_CODEPAGE)) {
        script_error("invalid argument", "rom");
      }
      else {
        sym = add_symbol(state.section.logical, logical_section_name);
        annotate_symbol(sym, section_name);
      }
    }
  }

  return 0;
}

/* general section definition processing */
static int do_secdef(const char *name, enum section_type type, struct pnode *parms)
{
  enum arg_id_e {
    ID_UNKNOWN = 0,
    ID_NAME = 1,
    ID_START,
    ID_END,
    ID_FILL,
    ID_SHADOW
  } arg;
  static struct {
    enum arg_id_e id;
    const char *name;
  }
  arg_tbl[] = {
    { ID_NAME,   "name"   },
    { ID_START,  "start"  },
    { ID_END,    "end"    },
    { ID_FILL,   "fill"   },
    { ID_SHADOW, "shadow" }
  };
  struct pnode *p;
  gp_boolean found_start = false;
  gp_boolean found_end = false;
  gp_boolean found_fill = false;
  gp_boolean found_protected = false;
  const char *section_name = NULL;
  const char *shadow_sym = NULL;
  long start = 0;
  long end = 0;
  long fill = 0;
  long shadow_val = 0;
  struct symbol *sym;
  struct linker_section *section_def;

  /* read the options */
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        const char *lhs = p->value.binop.p0->value.symbol;
        int i;

        arg = ID_UNKNOWN;
        for (i = 0; i < NELEM(arg_tbl); ++i) {
          if (strcasecmp(arg_tbl[i].name, lhs) == 0) {
            arg = arg_tbl[i].id;
          }
        }

        switch (arg) {
        case ID_NAME:
          if (enforce_simple(p->value.binop.p1)) {
            section_name = p->value.binop.p1->value.symbol;
          }
          break;

        case ID_START:
          found_start = true;
          start = evaluate(p->value.binop.p1);
          break;

        case ID_END:
          found_end = true;
          end = evaluate(p->value.binop.p1);
          break;

        case ID_FILL:
          found_fill = true;
          fill = evaluate(p->value.binop.p1);
          break;

        case ID_SHADOW:
          if (enforce_simple(p->value.binop.p1)) {
            const char *begin;
            char *end;

            begin = p->value.binop.p1->value.symbol;
            if ((end = strchr(begin, ':')) != NULL) {
              if (end == begin) {
                script_error("bad shadow symbol", lhs);
              }
              else {
                char *shsym = (char *)GP_Malloc(end - begin + 1);

                memcpy(shsym, begin, end - begin);
                shsym[end - begin] = '\0';
                shadow_sym = shsym;
                begin = ++end;
                shadow_val = strtol(begin, &end, 0);

                if (*end != '\0') {
                  script_error("bad shadow value", lhs);
                }
              }
            }
            else {
              script_error("bad shadow argument", lhs);
            }
          }
          break;

        default:
          script_error("illegal argument", lhs);
        }
      }
    }
    else {
      if (enforce_simple(p)) {
        if (strcasecmp(p->value.symbol, "protected") == 0) {
          found_protected = true;
        }
        else {
          script_error("illegal argument", p->value.symbol);
        }
      }
    }
  }

  /* process the options */
  if (NULL == section_name) {
    script_error("missing argument", "name");
  }
  else if (!found_start) {
    script_error("missing argument", "start");
  }
  else if (!found_end) {
    script_error("missing argument", "end");
  }
  else {
    sym = get_symbol(state.section.definition, section_name);

    if (sym == NULL) {
      sym = add_symbol(state.section.definition, section_name);
      section_def = (struct linker_section *)GP_Calloc(1, sizeof(struct linker_section));
      annotate_symbol(sym, section_def);

      switch (type) {
      case SECT_ACCESSBANK:
        if (state.class != PROC_CLASS_PIC16E) {
          script_error("accessbank only valid with 18xx devices", name);
        }
        break;

      case SECT_CODEPAGE:
        start = gp_processor_org_to_byte(state.class, start);
        end = gp_processor_org_to_byte(state.class, end + 1) - 1;
        break;

      case SECT_NONE:
        script_error("invalid definition type", name);
        break;

      default:
        break;
      }

      section_def->type = type;
      section_def->start = start;
      section_def->end = end;
      section_def->protected = found_protected;
      section_def->shadow_sym = shadow_sym;
      section_def->shadow_val = shadow_val;
      section_def->fill = 0;
      section_def->use_fill = false;

      if (section_def->type == SECT_CODEPAGE) {
        if ((!state.fill_enable) || found_protected) {
          section_def->fill = fill;
          section_def->use_fill = found_fill;
        }
        else {
          section_def->fill = state.fill_value;
          section_def->use_fill = true;
        }
      }
      else if (found_fill == 1) {
        script_error("illegal argument", "fill");
      }
    }
    else if (type != SECT_SHAREBANK) {
      script_error("duplicate section definition", section_name);
    }
  }

  return 0;
}

static int do_stack(const char *name, enum section_type type, struct pnode *parms)
{
  enum arg_id_e {
    ID_UNKNOWN = 0,
    ID_SIZE = 1,
    ID_RAM
  } arg;
  static struct {
    enum arg_id_e id;
    const char *name;
  }
  arg_tbl[] = {
    { ID_SIZE, "size" },
    { ID_RAM,  "ram"  }
  };
  struct pnode *p;
  gp_boolean found_size  = false;
  char *ram_name = NULL;
  struct symbol *sym;

  if (state.has_stack) {
    script_error("multiple stack definitions", NULL);
    return 0;
  }
  else {
    state.has_stack = true;
  }

  /* FIXME: simplify this.  There are only two arguments */

  /* read the options */
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);

    if ((p->tag == PTAG_BINOP) && (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        const char *lhs = p->value.binop.p0->value.symbol;
        int i;

        arg = ID_UNKNOWN;
        for (i = 0; i < NELEM(arg_tbl); ++i) {
          if (strcasecmp(arg_tbl[i].name, lhs) == 0) {
            arg = arg_tbl[i].id;
          }
        }

        switch (arg) {
        case ID_SIZE:
          found_size = true;
          state.stack_size = evaluate(p->value.binop.p1);
          break;

        case ID_RAM:
          if (enforce_simple(p->value.binop.p1)) {
            ram_name = GP_Strdup(p->value.binop.p1->value.symbol);
          }
          break;

        default:
          script_error("illegal argument", lhs);
        }
      }
    }
    else {
      if (enforce_simple(p)) {
        script_error("illegal argument", p->value.symbol);
      }
    }
  }

  /* process the options */
  if (!found_size) {
    script_error("missing argument", "size");
  }
  else if (ram_name != NULL) {
    sym = add_symbol(state.section.logical, GP_Strdup(".stack"));
    annotate_symbol(sym, ram_name);
  }

  return 0;
}

int execute_command(const char *name, struct pnode *parms)
{
  static struct {
    const char *name;
    enum section_type type;
    int (*address)(const char *name, enum section_type type, struct pnode *parms);
  } commands[] = {
    { "accessbank", SECT_ACCESSBANK, do_secdef  },
    { "codepage",   SECT_CODEPAGE,   do_secdef  },
    { "databank",   SECT_DATABANK,   do_secdef  },
    { "files",      SECT_NONE,       do_files   },
    { "include",    SECT_NONE,       do_include },
    { "linearmem",  SECT_LINEARMEM,  do_secdef  },
    { "section",    SECT_NONE,       do_logsec  },
    { "sharebank",  SECT_SHAREBANK,  do_secdef  },
    { "stack",      SECT_NONE,       do_stack   }
  };
  int i;

  for (i = 0; i < NELEM(commands); ++i) {
    if (strcasecmp(name, commands[i].name) == 0) {
      return (*commands[i].address)(name, commands[i].type, parms);
    }
  }

  script_error("invalid script command", name);

  return 0;
}

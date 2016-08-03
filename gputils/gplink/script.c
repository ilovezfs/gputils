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

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_enforce_simple(const pnode_t *Pnode)
{
  if (PnIsSymbol(Pnode)) {
    return true;
  }

  script_error("Illegal argument.", NULL);
  return false;
}

/*------------------------------------------------------------------------------------------------*/

static long
_evaluate(const pnode_t *Pnode)
{
  switch (Pnode->tag) {
    case PTAG_CONSTANT:
      return PnConstant(Pnode);

    case PTAG_SYMBOL:
      return script_get_symbol_value(PnSymbol(Pnode));

    default:
      script_error("Illegal argument.", NULL);
      return 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

static int
_do_files(const char *Name, enum section_type Type, const pnode_t *Parms)
{
  const pnode_t *p;

  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (_enforce_simple(p)) {
      gplink_open_coff(PnSymbol(p));
    }
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

static int
_do_include(const char *Name, enum section_type Type, const pnode_t *Parms)
{
  const pnode_t *p;

  /* FIXME: make sure only one argument is passed */
  p = PnListHead(Parms);

  if (_enforce_simple(p)) {
    open_src(PnSymbol(p), true);
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

/* FIXME:  These functions were written to allow the user the greatest flexibility.
           The arguments can appear in any order. In hind sight this may not have
           been necessary and resulted in overly complicated code. */

/* logical section definition */
static int
_do_logsec(const char *Name, enum section_type Type, const pnode_t *Parms)
{
  enum arg_id_e {
    ID_UNKNOWN = 0,
    ID_NAME = 1,
    ID_RAM,
    ID_ROM
  } arg;
  static struct {
    enum arg_id_e  id;
    const char    *name;
  }
  arg_tbl[] = {
    { ID_NAME, "name" },
    { ID_RAM,  "ram"  },
    { ID_ROM,  "rom"  }
  };
  const pnode_t    *p;
  gp_boolean        found_secname = false;
  gp_boolean        found_ram = false;
  gp_boolean        found_rom = false;
  const char       *logical_section_name = NULL;
  char             *section_name = NULL;
  linker_section_t *section = NULL;
  symbol_t         *sym;
  const char       *sym_name;
  size_t            i;

  /* read the options */
  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (PnIsBinOp(p) && (PnBinOpOp(p) == '=')) {
      if (_enforce_simple(PnBinOpP0(p))) {
        sym_name = PnSymbol(PnBinOpP0(p));
        arg      = ID_UNKNOWN;
        for (i = 0; i < ARRAY_SIZE(arg_tbl); ++i) {
          if (strcasecmp(arg_tbl[i].name, sym_name) == 0) {
            arg = arg_tbl[i].id;
            break;
          }
        }

        switch (arg) {
          case ID_NAME: {
            if (_enforce_simple(PnBinOpP1(p))) {
              found_secname        = true;
              logical_section_name = PnSymbol(PnBinOpP1(p));
            }
            break;
          }

          case ID_RAM: {
            if (_enforce_simple(PnBinOpP1(p))) {
              found_ram    = true;
              section_name = GP_Strdup(PnSymbol(PnBinOpP1(p)));
            }
            break;
          }

          case ID_ROM: {
            if (_enforce_simple(PnBinOpP1(p))) {
              found_rom    = true;
              section_name = GP_Strdup(PnSymbol(PnBinOpP1(p)));
            }
            break;
          }

          default:
            script_error("Illegal argument.", sym_name);
        }
      }
    }
    else {
      if (_enforce_simple(p)) {
        script_error("Illegal argument.", PnSymbol(p));
      }
    }
  }

  /* process the options */
  if (!found_secname) {
    script_error("Missing argument.", "name");
  }
  else if (found_rom && found_ram) {
    script_error("Too many arguments.", "ram or rom");
  }
  else if ((!found_rom) && (!found_ram)) {
    script_error("Missing argument.", "ram or rom");
  }
  else {
    sym = gp_sym_get_symbol(state.section.definition, section_name);

    if (sym == NULL) {
      script_error("Undefined section.", section_name);
    }
    else {
      section = gp_sym_get_symbol_annotation(sym);
      assert(section != NULL);

      if (found_ram && (section->type == SECT_CODEPAGE)) {
        script_error("Invalid argument.", "ram");
      }
      else if (found_rom && (section->type != SECT_CODEPAGE)) {
        script_error("Invalid argument.", "rom");
      }
      else {
        sym = gp_sym_add_symbol(state.section.logical, logical_section_name);
        gp_sym_annotate_symbol(sym, section_name);
      }
    }
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

/* general section definition processing */

static int
_do_secdef(const char *Name, enum section_type Type, const pnode_t *Parms)
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
    enum arg_id_e  id;
    const char    *name;
  }
  arg_tbl[] = {
    { ID_NAME,   "name"   },
    { ID_START,  "start"  },
    { ID_END,    "end"    },
    { ID_FILL,   "fill"   },
    { ID_SHADOW, "shadow" }
  };
  const pnode_t    *p;
  gp_boolean        found_start = false;
  gp_boolean        found_end = false;
  gp_boolean        found_fill = false;
  gp_boolean        found_protected = false;
  const char       *section_name = NULL;
  const char       *shadow_sym = NULL;
  long              shadow_val = 0;
  long              start = 0;
  long              end = 0;
  long              fill = 0;
  symbol_t         *sym;
  linker_section_t *section_def;
  const char       *sym_name;
  size_t            i;
  const char       *s_begin;
  char             *s_end;

  /* read the options */
  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (PnIsBinOp(p) && (PnBinOpOp(p) == '=')) {
      if (_enforce_simple(PnBinOpP0(p))) {
        sym_name = PnSymbol(PnBinOpP0(p));
        arg      = ID_UNKNOWN;
        for (i = 0; i < ARRAY_SIZE(arg_tbl); ++i) {
          if (strcasecmp(arg_tbl[i].name, sym_name) == 0) {
            arg = arg_tbl[i].id;
            break;
          }
        }

        switch (arg) {
          case ID_NAME: {
            if (_enforce_simple(PnBinOpP1(p))) {
              section_name = PnSymbol(PnBinOpP1(p));
            }
            break;
          }

          case ID_START:
            found_start = true;
            start       = _evaluate(PnBinOpP1(p));
            break;

          case ID_END:
            found_end = true;
            end       = _evaluate(PnBinOpP1(p));
            break;

          case ID_FILL:
            found_fill = true;
            fill       = _evaluate(PnBinOpP1(p));
            break;

          case ID_SHADOW: {
            if (_enforce_simple(PnBinOpP1(p))) {
              s_begin = PnSymbol(PnBinOpP1(p));

              if ((s_end = strchr(s_begin, ':')) != NULL) {
                if (s_end == s_begin) {
                  script_error("Bad shadow symbol.", sym_name);
                }
                else {
                  shadow_sym = GP_Strndup(s_begin, s_end - s_begin);
                  s_begin    = ++s_end;
                  shadow_val = strtol(s_begin, &s_end, 0);

                  if (*s_end != '\0') {
                    script_error("Bad shadow value.", sym_name);
                  }
                }
              }
              else {
                script_error("Bad shadow argument.", sym_name);
              }
            }
            break;
          }

          default:
            script_error("Illegal argument.", sym_name);
        }
      }
    }
    else {
      if (_enforce_simple(p)) {
        if (strcasecmp(PnSymbol(p), "protected") == 0) {
          found_protected = true;
        }
        else {
          script_error("Illegal argument.", PnSymbol(p));
        }
      }
    }
  }

  /* process the options */
  if (section_name == NULL) {
    script_error("Missing argument.", "name");
  }
  else if (!found_start) {
    script_error("Missing argument.", "start");
  }
  else if (!found_end) {
    script_error("Missing argument.", "end");
  }
  else {
    sym = gp_sym_get_symbol(state.section.definition, section_name);

    if (sym == NULL) {
      sym         = gp_sym_add_symbol(state.section.definition, section_name);
      section_def = (struct linker_section *)GP_Calloc(1, sizeof(struct linker_section));
      gp_sym_annotate_symbol(sym, section_def);

      switch (Type) {
        case SECT_ACCESSBANK: {
          if (state.class != PROC_CLASS_PIC16E) {
            script_error("Accessbank only valid with 18xx devices.", Name);
          }
          break;
        }

        case SECT_CODEPAGE:
          start = gp_processor_byte_from_insn_c(state.class, start);
          end   = gp_processor_byte_from_insn_c(state.class, end + 1) - 1;
          break;

        case SECT_NONE:
          script_error("Invalid definition type.", Name);
          break;

        default:
          break;
      }

      section_def->type       = Type;
      section_def->start      = start;
      section_def->end        = end;
      section_def->protected  = found_protected;
      section_def->shadow_sym = shadow_sym;
      section_def->shadow_val = shadow_val;
      section_def->fill       = 0;
      section_def->use_fill   = false;

      if (section_def->type == SECT_CODEPAGE) {
        if ((!state.fill_enable) || found_protected) {
          section_def->fill     = fill;
          section_def->use_fill = found_fill;
        }
        else {
          section_def->fill     = state.fill_value;
          section_def->use_fill = true;
        }
      }
      else if (found_fill) {
        script_error("Illegal argument.", "fill");
      }
    }
    else if (Type != SECT_SHAREBANK) {
      script_error("Duplicate section definition.", section_name);
    }
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

static int
_do_stack(const char *Name, enum section_type Type, const pnode_t *Parms)
{
  enum arg_id_e {
    ID_UNKNOWN = 0,
    ID_SIZE = 1,
    ID_RAM
  } arg;
  static struct {
    enum arg_id_e  id;
    const char    *name;
  }
  arg_tbl[] = {
    { ID_SIZE, "size" },
    { ID_RAM,  "ram"  }
  };
  const pnode_t *p;
  gp_boolean     found_size  = false;
  char          *ram_name = NULL;
  symbol_t      *sym;
  const char    *sym_name;
  int            i;

  if (state.has_stack) {
    script_error("Multiple stack definitions.", NULL);
    return 0;
  }
  else {
    state.has_stack = true;
  }

  /* FIXME: Simplify this. There are only two arguments. */

  /* read the options */
  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (PnIsBinOp(p) && (PnBinOpOp(p) == '=')) {
      if (_enforce_simple(p->value.binop.p0)) {
        sym_name = PnSymbol(PnBinOpP0(p));
        arg      = ID_UNKNOWN;
        for (i = 0; i < ARRAY_SIZE(arg_tbl); ++i) {
          if (strcasecmp(arg_tbl[i].name, sym_name) == 0) {
            arg = arg_tbl[i].id;
            break;
          }
        }

        switch (arg) {
          case ID_SIZE:
            found_size       = true;
            state.stack_size = _evaluate(PnBinOpP1(p));
            break;

          case ID_RAM: {
            if (_enforce_simple(PnBinOpP1(p))) {
              ram_name = GP_Strdup(PnSymbol(PnBinOpP1(p)));
            }
            break;
          }

          default:
            script_error("Illegal argument.", sym_name);
        }
      }
    }
    else {
      if (_enforce_simple(p)) {
        script_error("illegal argument.", PnSymbol(p));
      }
    }
  }

  /* process the options */
  if (!found_size) {
    script_error("Missing argument.", "size");
  }
  else if (ram_name != NULL) {
    sym = gp_sym_add_symbol(state.section.logical, GP_Strdup(".stack"));
    gp_sym_annotate_symbol(sym, ram_name);
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

void
script_error(const char *Messg, const char *Detail)
{
  gp_num_errors++;

  if (!gp_quiet) {
    if (state.src == NULL) {
      printf("%s\n", Messg);
    }
    else if (Detail == NULL) {
      printf("%s:%d:Error: %s\n", state.src->name, state.src->line_number, Messg);
    }
    else {
      printf("%s:%d:Error: %s (%s)\n", state.src->name, state.src->line_number, Messg, Detail);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

int
script_add_path(const pnode_t *Parms)
{
  const pnode_t *p;

  for (; Parms != NULL; Parms = PnListTail(Parms)) {
    p = PnListHead(Parms);

    if (_enforce_simple(p)) {
      /* The gplink doesn't distinguish between lib pathes and lkr pathes.
         There is one pathes list. This shouldn't cause any problems. */
      gplink_add_path(PnSymbol(p));
    }
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

void
script_add_symbol_value(const char *Name, long Value)
{
  symbol_t *sym;
  long     *val;

  sym = gp_sym_add_symbol(state.script_symbols, Name);
  val = gp_sym_get_symbol_annotation(sym);

  if (val == NULL) {
    val = GP_Malloc(sizeof(Value));
    gp_sym_annotate_symbol(sym, val);
  }

  *val = Value;
}

/*------------------------------------------------------------------------------------------------*/

long
script_get_symbol_value(const char *Name)
{
  const symbol_t *sym;
  const long     *val;

  sym = gp_sym_get_symbol(state.script_symbols, Name);
  if (sym == NULL) {
    return 0;
  }

  val = (const long *)gp_sym_get_symbol_annotation(sym);
  assert(val != NULL);
  return *val;
}

/*------------------------------------------------------------------------------------------------*/

int
script_execute_command(const char *Name, const pnode_t *Parms)
{
  static struct {
    const char        *name;
    enum section_type  type;
    int              (*address)(const char *Name, enum section_type Type, const pnode_t *Parms);
  } commands[] = {
    { "accessbank", SECT_ACCESSBANK, _do_secdef  },
    { "codepage",   SECT_CODEPAGE,   _do_secdef  },
    { "databank",   SECT_DATABANK,   _do_secdef  },
    { "files",      SECT_NONE,       _do_files   },
    { "include",    SECT_NONE,       _do_include },
    { "linearmem",  SECT_LINEARMEM,  _do_secdef  },
    { "section",    SECT_NONE,       _do_logsec  },
    { "sharebank",  SECT_SHAREBANK,  _do_secdef  },
    { "stack",      SECT_NONE,       _do_stack   }
  };
  size_t i;

  for (i = 0; i < ARRAY_SIZE(commands); ++i) {
    if (strcasecmp(Name, commands[i].name) == 0) {
      return (*commands[i].address)(Name, commands[i].type, Parms);
    }
  }

  script_error("Invalid script command.", Name);
  return 0;
}

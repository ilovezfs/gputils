/* ".LST" file output for gplink
   Copyright (C) 2004, 2005
   Craig Franklin

    Copyright (C) 2016 Molnar Karoly <molnarkaroly@users.sf.net>

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
#include "cod.h"

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

#define LINESIZ     520

static gp_boolean          list_enabled;
static const gp_section_t *line_section;

/*------------------------------------------------------------------------------------------------*/

static void
_open_source(const char *name, gp_symbol_t *symbol)
{
  char                 file_name[PATH_MAX + 1];
  struct list_context *new;
  int                  i;
  const char          *p;

  assert(name != NULL);

  new = GP_Malloc(sizeof(*new));
  new->f = fopen(name, "rt");
  if (new->f == NULL) {
    /* Try searching include pathes. */
    for (i = 0; i < state.numpaths; i++) {
      snprintf(file_name, sizeof(file_name), "%s" PATH_SEPARATOR_STR "%s", state.paths[i], name);
      new->f = fopen(file_name, "rb");
      if (new->f != NULL) {
        name = file_name;
        break;
      }
    }
    if (new->f == NULL) {
      /* The path may belong to a build procedure other than this. */
      p = strrchr(name, PATH_SEPARATOR_CHAR);

      if (p != NULL) {
        for (i = 0; i < state.numpaths; i++) {
          snprintf(file_name, sizeof(file_name), "%s%s", state.paths[i], p);
          new->f = fopen(file_name, "rb");
          if (new->f != NULL) {
            name = file_name;
            break;
          }
        }
      }
    }
  }

  if (new->f != NULL) {
    new->name           = GP_Strdup(name);
    new->missing_source = false;
  }
  else {
    new->missing_source = true;

    if (getenv("GPUTILS_WARN_MISSING_SRC")) {
      gp_warning("Cannot find source file \"%s\".", name);
    }
  }

  new->symbol      = symbol;
  new->line_number = 1;
  new->prev        = state.lst.src;

  state.lst.src = new;
}

/*------------------------------------------------------------------------------------------------*/

static void
_close_source(void)
{
  struct list_context *old;

  assert(state.lst.src != NULL);

  old = state.lst.src;
  state.lst.src = state.lst.src->prev;
  free(old);
}

/*------------------------------------------------------------------------------------------------*/

static void
_lst_line(const char *format, ...)
{
  va_list args;

  if (state.lst.f != NULL) {
    va_start(args, format);
    vfprintf(state.lst.f, format, args);
    va_end(args);
    fputc('\n', state.lst.f);
  }
}

/*------------------------------------------------------------------------------------------------*/

static const gp_linenum_t *
_find_line_number(const gp_symbol_t *symbol, unsigned int line_number)
{
  const gp_section_t *section;
  const gp_linenum_t *linenum;

  section = state.object->section_list.first;
  while (section != NULL) {
    linenum = gp_coffgen_find_linenum(section, symbol, line_number);

    if (linenum != NULL) {
      if (section != line_section) {
        /* Switching sections, so update was_org with the new address. */
        state.lst.was_org = linenum->address;
        line_section      = section;
      }

      return linenum;
    }

    section = section->next;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

static char *
_expand_tabs(const char *buf)
{
  static char  dest[LINESIZ];

  unsigned int is;
  unsigned int id;
  char         c;
  unsigned int n;

  for (is = 0, id = 0; ((c = buf[is]) != '\0') && (id < (sizeof(dest) - 2)); ++is) {
    if (c == '\t') {
      n = TABULATOR_SIZE - (id % TABULATOR_SIZE);

      while (n-- && (id < (sizeof(dest) - 2))) {
        dest[id++] = ' ';
      }
    }
    else {
      dest[id++] = c;
    }
  }

  dest[id] = '\0';
  return dest;
}

/*------------------------------------------------------------------------------------------------*/

static void
_write_source(int last_line)
{
  char                linebuf[LINESIZ];
  char                dasmbuf[LINESIZ];
  char               *pc;
  const gp_linenum_t *line;
  gp_boolean          first_time;
  unsigned int        org;
  unsigned int        len;
  uint8_t             byte;
  uint16_t            word;
  int                 num_bytes;

  /* If the source file wasn't found, can't write it to the list file. */
  if (state.lst.src->missing_source) {
    return;
  }

  org = 0;

  while (true) {
    /* When last_line is 0 print all lines, else print to last_line. */
    if ((last_line > 0) && (state.lst.src->line_number > last_line)) {
      break;
    }

    if (fgets(linebuf, LINESIZ, state.lst.src->f) == NULL) {
      break;
    }

    state.lst.was_org = org;

    if (list_enabled) {
      /* Eat the trailing newline. */
      pc = strrchr(linebuf, '\n');

      if (pc != NULL) {
        *pc = '\0';
      }

      first_time = true;

      line = _find_line_number(state.lst.src->symbol, state.lst.src->line_number);
      while (line != NULL) {
        /* Print all instructions generated by this line of the source. */
        if (line->line_number != state.lst.src->line_number) {
          break;
        }

        if (!first_time) {
          /* Only print the source line the first time. */
          linebuf[0] = '\0';
        }

        state.cod.emitting = true;
        org = line->address;
        len = gp_mem_b_get_unlisted_size(line_section->data, org);

        if (len == 0) {
          if (linebuf[0] != '\0') {
            _lst_line("%42s %s", "", linebuf);
          }
          else {
            _lst_line("");
          }

          cod_lst_line(COD_NORMAL_LST_LINE);
        }
        else {
          if ((org & 1) || (len < 2)) {
            /* even address or less then two byts to disassemble: disassemble one byte */
            if (len != 0) {
              gp_mem_b_assert_get(line_section->data, org, &byte, NULL, NULL);
              gp_disassemble_byte(line_section->data, org, dasmbuf, sizeof(dasmbuf));
              _lst_line("%06lx   %02x       %-24s %s", gp_processor_byte_to_org(state.class, org),
                        (unsigned int)byte, _expand_tabs(dasmbuf), linebuf);
              gp_mem_b_set_listed(line_section->data, org, 1);
              state.lst.was_org = org;
              cod_lst_line(COD_NORMAL_LST_LINE);
              ++org;
            }
          }
          else {
            state.class->i_memory_get(line_section->data, org, &word, NULL, NULL);
            num_bytes = gp_disassemble_size(line_section->data, org, state.class,
                                            gp_processor_bsr_boundary(state.processor),
                                            state.processor->prog_mem_size, GPDIS_SHOW_ALL_BRANCH,
                                            dasmbuf, sizeof(dasmbuf), len);
            _lst_line("%06lx   %04x     %-24s %s", gp_processor_byte_to_org(state.class, org),
                      word, _expand_tabs(dasmbuf), linebuf);
            gp_mem_b_set_listed(line_section->data, org, num_bytes);
            state.lst.was_org = org;
            cod_lst_line(COD_NORMAL_LST_LINE);
            org += 2;

            if (num_bytes > 2) {
              state.lst.was_org = org;
              state.class->i_memory_get(line_section->data, org, &word, NULL, NULL);
              _lst_line("%06lx   %04x", gp_processor_byte_to_org(state.class, org), word);
              cod_lst_line(COD_NORMAL_LST_LINE);
              org += 2;

              if (line->next != NULL) {
                /* skip the line number for the other half of this instruction */
                line = line->next;
              }
            }
          }
        }

        first_time = false;
        line = line->next;
      } /* while (line != NULL) */

      if (first_time) {
        if (linebuf[0] != '\0') {
          _lst_line("%42s %s", "", linebuf);
        }
        else {
          _lst_line("");
        }

        state.cod.emitting = false;
        cod_lst_line(COD_NORMAL_LST_LINE);
      }
    }

    state.lst.src->line_number++;
  }
}

/*------------------------------------------------------------------------------------------------*/

/*
 * _lst_init - initialize the lst file
 */

static void
_lst_init(void)
{
  if (state.lstfile != OUT_NAMED) {
    snprintf(state.lstfilename, sizeof(state.lstfilename), "%s.lst", state.basefilename);
  }

  if ((gp_num_errors > 0) || (state.lstfile == OUT_SUPPRESS)) {
    state.lst.f       = NULL;
    state.lst.enabled = false;
    unlink(state.lstfilename);
  }
  else {
    state.lst.f = fopen(state.lstfilename, "wt");

    if (state.lst.f == NULL) {
      perror(state.lstfilename);
      exit(1);
    }
    state.lst.enabled = true;
  }

  if (!state.lst.enabled) {
    return;
  }

  state.lst.was_org  = 0;
  state.cod.emitting = false;

  _lst_line("%s", GPLINK_VERSION_STRING);
  _lst_line("%s", GPUTILS_COPYRIGHT_STRING);
  _lst_line("Listing File Generated: %s", state.startdate);
  _lst_line("");
  _lst_line("");
  _lst_line("Address  Value    Disassembly              Source");
  _lst_line("-------  -----    -----------              ------");
}

/*------------------------------------------------------------------------------------------------*/

void
lst_write(void)
{
  gp_symbol_t    *symbol;
  const gp_aux_t *aux;
  gp_boolean      first_time;

  _lst_init();

  if (!state.lst.enabled) {
    return;
  }

  symbol        = state.object->symbol_list.first;
  list_enabled  = true;
  first_time    = true;
  state.lst.src = NULL;

  /* scan through the file symbols */
  while (symbol != NULL) {
    if (symbol->class == C_FILE) {
      /* open a new file */
      aux = symbol->aux_list.first;
      assert(aux != NULL);

      if (aux->_aux_symbol._aux_file.line_number) {
        /* it is an include file, so print the current file
           until the line number is reached */
        assert(state.lst.src != NULL);
        _write_source(aux->_aux_symbol._aux_file.line_number);
      }
      else {
        /* it is not an include, so enable listing */
        list_enabled = true;
      }

      _open_source(aux->_aux_symbol._aux_file.filename, symbol);

      if (first_time) {
        /* write the line numbers for the lst file header */
        cod_lst_line(COD_FIRST_LST_LINE);
        cod_lst_line(COD_NORMAL_LST_LINE);
        cod_lst_line(COD_NORMAL_LST_LINE);
        cod_lst_line(COD_NORMAL_LST_LINE);
        cod_lst_line(COD_NORMAL_LST_LINE);
        cod_lst_line(COD_NORMAL_LST_LINE);
        cod_lst_line(COD_NORMAL_LST_LINE);
        first_time = false;
      }
    }
    else if (symbol->class == C_LIST) {
      if (strcasecmp(symbol->name, ".list") == 0) {
        _write_source(symbol->value);
        list_enabled = true;
      }
      else if (strcasecmp(symbol->name, ".nolist") == 0) {
        _write_source(symbol->value);
        list_enabled = false;
      }
      else {
        assert(0);
      }
    }
    else if (symbol->class == C_EOF) {
      /* print the rest of the current file then, close it */
      _write_source(0);
      _close_source();
    }

    symbol = symbol->next;
  }

  fclose(state.lst.f);
}

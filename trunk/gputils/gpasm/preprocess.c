/* gpasm preprocessor implementation
   Copyright (C) 2012 Borut Razem

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
#include "gpasm.h"
#include "evaluate.h"
#include "gpmsg.h"
#include "directive.h"
#include "gptypes.h"
#include "gpsymbol.h"
#include "preprocess.h"

#define DEBUG                   0
#define DBG_printf              (!DEBUG) ? (void)0 : (void)printf

#define BUF_REPLACE(buf, start, end, len, repl_buf, repl_len, buf_size) \
  do { \
    memmove(&(buf)[(start) + (repl_len)], &(buf)[(end)], (len) - (end)); \
    memcpy(&(buf)[(start)], (repl_buf), (repl_len)); \
    (len) += (repl_len) - ((end) - (start)); \
    (end) = (start) + (repl_len); \
  } \
  while (0)

typedef gp_boolean (*substitute_func_t)(char *Buf, int Begin, int *End, int *N, int Max_size, int Level);

static pnode_t *param_list = NULL;

struct arg_list_s {
  const char        *str;
  struct arg_list_s *next;
} *arg_list = NULL, *arg_list_tail = NULL;

/*------------------------------------------------------------------------------------------------*/

static const char *
_check_defines(char *Symbol, int Symlen, pnode_t **Param_list_p)
{
  symbol_t   *sym;
  pnode_t    *p;
  pnode_t    *p2;
  const char *subst;

  *Param_list_p = NULL;

  /* If not quoted, check for #define substitution. */
  sym = gp_sym_get_symbol_len(state.stDefines, Symbol, Symlen);

  if (sym == NULL) {
    return NULL;
  }

  p = (pnode_t *)gp_sym_get_symbol_annotation(sym);

  if (p == NULL) {
    return "";
  }

  assert(PnIsList(p));

  p2 = PnListHead(p);
  assert(PnIsString(p2));

  subst = PnString(p2);
  *Param_list_p = PnListTail(p);

  if (subst == NULL) {
    return "";
  }

  /* check for a bad subsitution */
  return (((strlen(subst) == Symlen) && (strncmp(Symbol, subst, Symlen) == 0)) ? NULL : subst);
}

/*------------------------------------------------------------------------------------------------*/

static int
_is_first_iden(int Ch)
{
  return (!isascii(Ch) || (Ch == '_') || (Ch == '.') || (Ch == '?') || (Ch == '@') || (Ch == '#') || isalpha(Ch));
}

/*------------------------------------------------------------------------------------------------*/

static int
_is_iden(int Ch)
{
  return (_is_first_iden(Ch) || isdigit(Ch));
}

/*------------------------------------------------------------------------------------------------*/

static inline void
_skip_spaces(const char *Buf, int *Num)
{
  while (isspace(Buf[*Num])) {
    ++(*Num);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_add_arg(const char *Str)
{
  struct arg_list_s *new;

  new = GP_Malloc(sizeof(struct arg_list_s));
  new->str  = Str;
  new->next = NULL;

  if (arg_list == NULL) {
    arg_list = new;
  }
  else {
    arg_list_tail->next = new;
  }

  arg_list_tail = new;
}

/*------------------------------------------------------------------------------------------------*/

static void
_free_list(struct arg_list_s *List)
{
  if (List != NULL) {
    if (List->next != NULL) {
      _free_list(List->next);
      free(List);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_free_arg_list(void)
{
  _free_list(arg_list);
  arg_list      = NULL;
  arg_list_tail = NULL;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_substitute_define_param(char *Buf, int Begin, int *End, int *Num, int Max_size, int Level)
{
  struct arg_list_s *argp;
  pnode_t           *parp;
  int                mlen;
  int                len;

  argp = arg_list;
  parp = param_list;
  mlen = *End - Begin;
  assert(mlen > 0);

  /* find argument */
  while (parp != NULL) {
    assert(PnIsSymbol(PnListHead(parp)));

    if (strncmp(&Buf[Begin], PnSymbol(PnListHead(parp)), mlen) == 0) {
      /* substitute */
      len = strlen(argp->str);

      DBG_printf("@@@substituting parameter %*.*s with %s\n", mlen, mlen, &Buf[Begin], argp->str);

      if ((*Num + len - mlen) >= Max_size) {
        gpmsg_verror(GPE_INTERNAL, NULL, "Flex buffer too small.");
        return false;
      }

      BUF_REPLACE(Buf, Begin, *End, *Num, argp->str, len, Max_size);
      return true;
    }

    parp = PnListTail(parp);
    argp = argp->next;
  }

  return false; /* no substitution */
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean _preprocess(char *Buf, int Begin, int *End, int *Num, int Max_size, substitute_func_t Substitute, int Level);

static gp_boolean
_substitute_define(char *Buf, int Begin, int *End, int *Num, int Max_size, int Level)
{
  int         mlen;
  const char *sub;
  int         size;
  int         n_params;
  gp_boolean  bracket;
  int         n_args;
  int         start1;
  int         end1;
  int         state;
  gp_boolean  prev_esc;
  int         brackdepth;
  int         len;

  mlen = *End - Begin;
  if (mlen <= 0) {
    /* nothing to substitute */
    return false;
  }

  if ((sub = _check_defines(&Buf[Begin], mlen, &param_list)) != NULL) {
    n_params = eval_list_length(param_list);

    DBG_printf("define %*.*s has %d parameters\n", mlen, mlen, &Buf[Begin], n_params);
    if (n_params != 0) {
      /* has parameters: collect arguments */
      bracket = false;
      n_args = 0;

      _skip_spaces(Buf, End);
      if (Buf[*End] == '(') {
        ++(*End);
        bracket = true;
      }

      for (; ; ) {
        state      = 0;
        prev_esc   = false;
        brackdepth = 0;

        _skip_spaces(Buf, End);
        start1 = *End;

        while ((*End < *Num) &&
               ((state != 0) ||
                (brackdepth != 0) ||
                ((Buf[*End] != ',') &&
                 (Buf[*End] != ';') &&
                 ((bracket && (Buf[*End] != ')')) ||
                  (!bracket && (Buf[*End] != '\n')))))) {
          switch (Buf[*End]) {
            case '(': {
              if (state == 0) {
                ++brackdepth;
              }
              break;
            }

            case ')': {
              if (state == 0) {
                --brackdepth;
              }
              break;
            }

            case '\\':
              prev_esc = (state != 0) ? !prev_esc : false;
              break;

            case '"':
            case '\'':
              if (!prev_esc) {
                state = (state == 0) ? Buf[*End] : ((state == Buf[*End]) ? 0 : state);
              }
            default:
              prev_esc = false;
              break;
          }

          ++(*End);
        }

        /* right trim */
        end1 = *End - 1;
        while ((end1 >= 0) && isspace(Buf[end1])) {
          --end1;
        }

        ++end1;

        _add_arg(GP_Strndup(&Buf[start1], end1 - start1));
        ++n_args;

        if (*End < *Num) {
          if ((bracket && (Buf[*End] == ')')) || (!bracket && ((Buf[*End] == '\n') || (Buf[*End] == ';')))) {
            if (Buf[*End] == ';') {
              /* skip to the trailing newline */
              *End = (Buf[*Num - 1] == '\n') ? (*Num - 1) : *Num;
            }
            else {
              /* don't eat newline! */
              if (Buf[*End] != '\n') {
                ++(*End);
              }
            }

            if (n_args == n_params) {
              len = strlen(sub);

              /* substitute define parameters */
              if ((*Num + len - mlen) >= Max_size) {
                gpmsg_verror(GPE_INTERNAL, NULL, "Flex buffer too small.");
                return false;
              }
              else {
                DBG_printf("@1@substituting define parameter %*.*s ", mlen, mlen, &Buf[Begin]);

                BUF_REPLACE(Buf, Begin, *End, *Num, sub, len, Max_size);

                /* recurse preprocess with increased level */
                _preprocess(Buf, Begin, End, Num, Max_size, &_substitute_define_param, Level + 1);
                _free_arg_list();

                /* substitute defines */
                /* recurse preprocess with increased level */
                _preprocess(Buf, Begin, End, Num, Max_size, &_substitute_define, Level + 1);

                size = *End - Begin;
                DBG_printf("with %*.*s\n", size, size, &Buf[Begin]);
                return true;
              }
            }
            else {
              /* error n_args != n_params: no substitution */
              _free_arg_list();
              return false;
            }
          }
          else if (Buf[*End] != ',') {
            /* error unknown delimiter: no substitution */
            _free_arg_list();
            return false;
          }

          ++(*End);
        }

        if (*End >= *Num) {
          /* error no ending bracket or newline: no substitution */
          _free_arg_list();
          return false;
        }
      } /* for each argument */
    } /* if has parameters */
    else {
      len = strlen(sub);

      /* substitute define */
      if ((*Num + len - mlen) >= Max_size) {
        gpmsg_verror(GPE_INTERNAL, NULL, "Flex buffer too small.");
        return false;
      }

      DBG_printf("@2@substituting define %*.*s ", mlen, mlen, &Buf[Begin]);

      BUF_REPLACE(Buf, Begin, *End, *Num, sub, len, Bax_size);
      /* recurse preprocess with increased level */
      _preprocess(Buf, Begin, End, Num, Max_size, &_substitute_define, Level + 1);

      size = *End - Begin;
      DBG_printf("with %*.*s\n", size, size, &Buf[Begin]);
    }
  }
  return false;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_no_process_iden(const char *Iden, int Len)
{
  static const char * const iden_tbl[] = {
    "#define",
    "#ifdef",
    "#ifndef",
    "#elifdef",
    "#elifndef",
    "#undefine",
    "define",
    "ifdef",
    "ifndef",
    "elifdef",
    "elifndef",
  };
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE(iden_tbl); ++i) {
    if ((strlen(iden_tbl[i]) == Len) && (strncasecmp(Iden, iden_tbl[i], Len) == 0)) {
      return true;
    }
  }
  return false;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_preprocess(char *Buf, int Begin, int *End, int *Num, int Max_size, substitute_func_t Substitute, int Level)
{
  int        start;
  int        state;             /* '"': in double quotes; '\'': in single quotes; ';': in comment */
  gp_boolean prev_esc;          /* true: prev char was escape */
  int        in_hv;             /* in #v */
  gp_boolean number_start;      /* true: possible start of a x'nnn' formatted number */
  gp_boolean substituted;       /* if there was a substitution in the preprocess run */
  int        i;
  int        c;
  int        size;
  int        end1;
  int        prev_n;

  if (Level >= PREPROC_MAX_DEPTH) {
    gpmsg_verror(GPE_STRCPLX, NULL);
    return false;
  }

  start        = -1;
  state        = 0;
  in_hv        = 0;
  prev_esc     = false;
  number_start = false;
  substituted  = false;

  size = *End - Begin;
  DBG_printf("---Preprocessing %*.*s\n", size, size, &Buf[Begin]);

  for (i = Begin; i < *End; ++i) {
    c = Buf[i];

    if (state == 0) {
      if (c == '#') {
        in_hv = '#';
      }
      else if ((in_hv == '#') && ((c == 'v') || (c == 'V'))) {
        in_hv = 'v';
      }
      else if ((in_hv == 'v') && (c == '(')) {
        if (start != -1) {
          if (start < (i - 2)) {
            /* preprocess the identifier before #v */
            end1 = i - 2;
            prev_n = *Num;

            size = end1 - start;
            DBG_printf("@1@Preprocessing identifier: %*.*s\n", size, size, &Buf[start]);
            substituted |= (*Substitute)(Buf, start, &end1, Num, Max_size, Level);
            *End += *Num - prev_n;
            i = end1 + 2;
          }
          start = -1;
        }
        in_hv = '(';
      }
      else {
        in_hv = 0;
      }

      if ((start == -1) && _is_first_iden(c)) {
        switch (c) {
          case 'a': case 'A':
          case 'b': case 'B':
          case 'd': case 'D':
          case 'h': case 'H':
          case 'o': case 'O':
            number_start = true;
            break;

          default:
            number_start = false;
            break;
        }

        start = i;
      }
      else {
        if ((start != -1) && !_is_iden(c)) {
          if ((Level == 0) && _no_process_iden(&Buf[start], i - start)) {
            start = -1;
            break;
          }

          if ((c != '\'') || !number_start) {
            prev_n = *Num;

            size = i - start;
            DBG_printf("@2@Preprocessing identifier: %*.*s\n", size, size, &Buf[start]);
            substituted |= (*Substitute)(Buf, start, &i, Num, Max_size, Level);
            *End += *Num - prev_n;
          }

          start = -1;
        }

        number_start = false;
      }
    }

    switch (c) {
      case '\\':
        prev_esc = ((state == '"') || (state == '\'')) ? (!prev_esc) : false;
        break;

      case ';': {
        if (state == 0) {
          state = c;
        }
        prev_esc = false;
        break;
      }

      case '"':
      case '\'':
        if (!prev_esc && (state != ';')) {
          state = (state == 0) ? c : ((state == c) ? 0 : state);
        }
      default:
        prev_esc = false;
        break;
    }
  }

  if (start != -1) {
    prev_n = *Num;

    size = i - start;
    DBG_printf("@3@Preprocessing identifier: %*.*s\n", size, size, &Buf[start]);
    substituted |= (*Substitute)(Buf, start, &i, Num, Max_size, Level);
    *End += *Num - prev_n;
  }

  size = *End - Begin;
  DBG_printf("+++Preprocessed %*.*s; substituted = %d\n", size, size, &Buf[Begin], substituted);

  return substituted;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_preprocess_hv(char *Buf, int Begin, int *End, int *Num, int Max_size)
{
  char       res_buf[11];
  gp_boolean substituted;
  int        size;
  int        res_len;
  int        prev_n;

  size        = *End - Begin;
  substituted = false;
  DBG_printf("---preprocess_hv: %*.*s\n", size, size, &Buf[Begin]);

  while (Begin < *End) {
    size = *End - Begin;
    DBG_printf("***Parsing chunk: %*.*s\n", size, size, &Buf[Begin]);

    if (ppparse_chunk(Buf, Begin, *End)) {
      substituted = true;
      DBG_printf("col_begin = %d; col_end = %d; result = %d\n", ppcol_begin, ppcol_end, ppresult);
      res_len = snprintf(res_buf, sizeof(res_buf), "%d", ppresult);

      if ((*Num + res_len - (ppcol_end - ppcol_begin)) >= Max_size) {
        gpmsg_verror(GPE_INTERNAL, NULL, "Flex buffer too small.");
        return false;
      }

      prev_n = *Num;

      DBG_printf ("@@@Subtituting %*.*s ", ppcol_end - ppcol_begin, ppcol_end - ppcol_begin, &Buf[ppcol_begin]);

      BUF_REPLACE(Buf, ppcol_begin, ppcol_end, *Num, res_buf, res_len, Max_size);
      *End += *Num - prev_n;
      DBG_printf("with %*.*s\n", res_len, res_len, &Buf[ppcol_begin]);
      Begin = ppcol_end;
      DBG_printf("buf = %*.*s\n", *Num, *Num, Buf);
    }
    else {
      Begin = ppcol_end;
    }
  }

  DBG_printf("+++preprocess_hv: %*.*s\n", *Num, *Num, Buf);

  return substituted;
}

/*------------------------------------------------------------------------------------------------*/

static void
_preprocess_hv_params(char *Buf, int Begin, int *End, int *Num, int Max_size)
{
  int        start;
  int        state;             /* '"': in double quotes; '\'': in single quotes; ';': in comment; '(' in #v argument */
  gp_boolean prev_esc;          /* true: prev char was escape */
  int        in_hv;             /* in #v */
  int        hv_parenth;        /* #v parenthesis nesting depth */
  int        i;
  int        c;
  int        prev_n;

  start      = -1;
  state      = 0;
  prev_esc   = false;
  in_hv      = 0;
  hv_parenth = 0;

  DBG_printf("---preprocess_hv_params: %*.*s\n", *End, *End, Buf);

  for (i = Begin; i < *End; ++i) {
    c = Buf[i];

    if (state == '(') {
      if (in_hv == '(') {
        start = i;
        in_hv = 0;
      }

      if (c == '(') {
        ++hv_parenth;
      }
      else if (c == ')') {
        if (--hv_parenth <= 0) {
          prev_n = *Num;

          _preprocess_hv_params(Buf, start, &i, Num, Max_size);
          _preprocess(Buf, start, &i, Num, Max_size, &_substitute_define, 0);
          _preprocess_hv(Buf, start, &i, Num, Max_size);
          *End += *Num - prev_n;
          start = -1;
          state = 0;
        }
      }
    }
    else if (state == 0) {
      if (c == '#') {
        in_hv = '#';
      }
      else if ((in_hv == '#') && ((c == 'v') || (c == 'V'))) {
        in_hv = 'v';
      }
      else if ((in_hv == 'v') && (c == '(')) {
        in_hv = '(';
        ++hv_parenth;
        state = '(';
      }
      else {
        in_hv = 0;
      }
    }

    if (state == 0) {
      if ((start == -1) && _is_first_iden(c)) {
        start = i;
      }
    }

    switch (c) {
      case '\\':
        prev_esc = ((state == '"') || (state == '\'')) ? (!prev_esc) : false;
        break;

      case ';': {
        if (state == 0) {
          state = c;
        }
        prev_esc = false;
        break;
      }

      case '"':
      case '\'':
        if (!prev_esc && (state != ';')) {
          state = (state == 0) ? c : ((state == c) ? 0 : state);
        }
      default:
        prev_esc = false;
        break;
    }
  }

  DBG_printf("+++preprocess_hv_params: %*.*s\n", *End, *End, Buf);
}

/*------------------------------------------------------------------------------------------------*/

static const char *
_check_macro_params(char *Symbol, int Symlen)
{
  const symbol_t *sym;
  const pnode_t  *p;
  const pnode_t  *p2;
  const char     *subst;

  sym = gp_sym_get_symbol_len(state.stMacroParams, Symbol, Symlen);

  if (sym == NULL) {
    return NULL;
  }

  p = (const pnode_t *)gp_sym_get_symbol_annotation(sym);

  if (p == NULL) {
    return "";
  }

  assert(PnIsList(p));
  assert(PnListTail(p) == NULL);

  p2 = PnListHead(p);
  assert(PnIsString(p2));

  subst = PnString(p2);

  if (subst == NULL) {
    return "";
  }

  /* check for a bad subsitution */
  return (((strlen(subst) == Symlen) && (strncmp(Symbol, subst, Symlen) == 0)) ? NULL : subst);
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_substitute_macro_param(char *Buf, int Begin, int *End, int *Num, int Max_size, int Level)
{
  int         mlen;
  int         len;
  const char *sub;

  mlen = *End - Begin;

  if (mlen <= 0) {
    /* nothing to substitute */
    return false;
  }

  if ((sub = _check_macro_params(&Buf[Begin], mlen)) != NULL) {
    len = strlen(sub);

    if ((*Num + len - mlen) >= Max_size) {
      gpmsg_verror(GPE_INTERNAL, NULL, "Flex buffer too small.");
      return false;
    }

    DBG_printf("@@@substituting macro parameter %*.*s with %*.*s\n", mlen, mlen, &Buf[Begin], len, len, sub);

    BUF_REPLACE(Buf, Begin, *End, *Num, sub, len, Max_size);
    return true;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

static void
_set_source_line(const char *Line, int Len, src_line_t *Src_line)
{
  if (Src_line->line == 0) {
    Src_line->size = 128;
    Src_line->line = GP_Malloc(Src_line->size);
  }

  if ((Len > 0) && Line[Len - 1] == '\n') {  /* ignore trailing newline */
    --Len;
  }
 
  if (Src_line->size <= Len) {
    do {
      Src_line->size *= 2;
    }
    while (Src_line->size <= Len);
    Src_line->line = GP_Realloc(Src_line->line, Src_line->size);
  }

  if (Len > 0) {
    memcpy(Src_line->line, Line, Len);
    Src_line->line[Len] = '\0';
  }
  else {
    Src_line->line[0] = '\0';
  }
}

/*------------------------------------------------------------------------------------------------*/

static inline gp_boolean
_in_macro_expansion(void)
{
  const source_context_t *p;

  for (p = state.src_list.last; p != NULL; p = p->prev) {
    if (p->type == SRC_MACRO) {
      return true;
    }
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

void
preprocess_line(char *Buf, int *Num, int Max_size)
{
  gp_boolean res;
  int        end;
  gp_boolean macro_expansion;

  end = *Num;

  if (IN_MACRO_WHILE_DEFINITION) {
    /* don't preprocess source line if in macro definition */
    _set_source_line(Buf, *Num, &state.src_list.last->curr_src_line);
  }
  else {
    macro_expansion = _in_macro_expansion();

    if (macro_expansion) {
      /* preprocess macro parameters */
      _preprocess(Buf, 0, &end, Num, Max_size, &_substitute_macro_param, 1);
    }

    /* preprocess #v parameters */
    _preprocess_hv_params(Buf, 0, &end, Num, Max_size);
    _preprocess_hv(Buf, 0, &end, Num, Max_size);

    if (!macro_expansion) {
      /* set only #v processed source line if not in macro expansion */
      _set_source_line(Buf, *Num, &state.src_list.last->curr_src_line);
    }

    /* preprocess line */
    do {
      res  = _preprocess(Buf, 0, &end, Num, Max_size, &_substitute_define, 0);
      res |= _preprocess_hv(Buf, 0, &end, Num, Max_size);
    }
    while (res);

    if (macro_expansion) {
      /* set processed source line if in macro expansion */
      _set_source_line(Buf, *Num, &state.src_list.last->curr_src_line);
    }
  }

  if (state.preproc.f != NULL) {
    /* set current preprocessed source line if preprocessed asm file emission enabled */
    _set_source_line(Buf, *Num, &state.preproc.curr_src_line);
  }
}

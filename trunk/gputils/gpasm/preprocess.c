/* gpasm preprocessor implementation
   Copyright (C) 1012 Borut Razem

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
#include "gperror.h"
#include "directive.h"
#include "gptypes.h"
#include "gpsymbol.h"
#include "preprocess.h"

#define DEBUG 0
#define DBG_printf !DEBUG ? (void)0 : (void)printf

#define BUF_REPLACE(buf, start, end, len, repl_buf, repl_len, buf_size) \
  do { \
    memmove(&(buf)[(start) + (repl_len)], &(buf)[(end)], (len) - (end)); \
    memcpy(&(buf)[(start)], (repl_buf), repl_len); \
    (len) += (repl_len) - ((end) - (start)); \
    (end) = (start) + (repl_len); \
  } \
  while (0)

typedef gp_boolean (*substitute_func_t)(char *buf, int begin, int *end, int *n, int max_size, int level);

static struct pnode *param_list = NULL;
struct arg_list_s {
  const char *str;
  struct arg_list_s *next;
} *arg_list = NULL, *arg_list_tail = NULL;

static const char *
check_defines(char *symbol, int symlen, struct pnode **param_list_p)
{
  struct symbol *sym;
  struct pnode *p;
  const char *subst = NULL;

  *param_list_p = NULL;

  /* If not quoted, check for #define substitution. */
  if ((sym = get_symbol_len(state.stDefines, symbol, symlen)) != NULL) {
    p = get_symbol_annotation(sym);

    if (p != NULL) {
      struct pnode *p2 = HEAD(p);

      assert(p->tag == PTAG_LIST);
      assert(p2->tag == PTAG_STRING);
      subst = p2->value.string;

      *param_list_p = TAIL(p);

      if (subst == NULL) {
        subst = "";
      }
      else if ((strlen(subst) == symlen) && (strncmp(symbol, subst, symlen) == 0)) {
        /* check for a bad subsitution */
        subst = NULL;
      }
    } else {
      subst = "";
    }
  }

  return subst;
}

static int
is_first_iden(int c)
{
  return (!isascii(c) || (c == '_') || (c == '.') || (c == '?') || (c == '@') || (c == '#') || isalpha(c));
}

static int
is_iden(int c)
{
  return (is_first_iden(c) || isdigit(c));
}

static inline void
skip_spaces(const char *buf, int *i)
{
  while (isspace(buf[*i])) {
    ++(*i);
  }
}

static void
add_arg(const char *str)
{
  struct arg_list_s *new = GP_Malloc(sizeof(struct arg_list_s));

  new->str  = str;
  new->next = NULL;

  if (arg_list_tail != NULL) {
    arg_list_tail->next = new;
  }
  else {
    arg_list = new;
  }

  arg_list_tail = new;
}

static void
fal(struct arg_list_s *p)
{
  if (p != NULL) {
    if (p->next != NULL) {
      fal(p->next);
      free(p);
    }
  }
}

static void
free_arg_list(void)
{
  fal(arg_list);
  arg_list_tail = arg_list = NULL;
}

static gp_boolean
substitute_define_param(char *buf, int begin, int *end, int *n, int max_size, int level)
{
  int mlen = *end - begin;
  struct arg_list_s *argp = arg_list;
  struct pnode *parp = param_list;

  assert(mlen > 0);

  /* find argument */
  while (parp != NULL) {
    assert(HEAD(parp)->tag == PTAG_SYMBOL);
    if (strncmp(&buf[begin], HEAD(parp)->value.symbol, mlen) == 0) {
      /* substitute */
      int len = strlen(argp->str);

      DBG_printf("@@@substituting parameter %*.*s with %s\n", mlen, mlen, &buf[begin], argp->str);

      if ((*n + len - mlen) >= max_size) {
        gpverror(GPE_INTERNAL, NULL, "Flex buffer too small.");
        return false;
      }
      else {
        BUF_REPLACE(buf, begin, *end, *n, argp->str, len, max_size);
        return true;
      }
    }
    parp = TAIL(parp);
    argp = argp->next;
  }

  return false; /* no substitution */
}

static gp_boolean preprocess(char *buf, int begin, int *end, int *n, int max_size, substitute_func_t substitute, int level);

static gp_boolean
substitute_define(char *buf, int begin, int *end, int *n, int max_size, int level)
{
  int mlen = *end - begin;
  const char *sub;

  if (mlen <= 0) {
    /* nothing to substitute */
    return false;
  }

  if ((sub = check_defines(&buf[begin], mlen, &param_list)) != NULL) {
    int n_params = list_length(param_list);

    DBG_printf("define %*.*s has %d parameters\n", mlen, mlen, &buf[begin], n_params);
    if (n_params != 0) {
      /* has parameters: collect arguments */
      int bracket = 0;
      int n_args = 0;

      skip_spaces(buf, end);
      if (buf[*end] == '(') {
        ++(*end);
        bracket = 1;
      }

      for (; ; ) {
        int start1;
        int end1;
        int state = 0;
        int prev_esc = 0;
        int brackdepth = 0;

        skip_spaces(buf, end);
        start1 = *end;

        while ((*end < *n) &&
               ((state != 0) ||
                (brackdepth != 0) ||
                ((buf[*end] != ',') &&
                 (buf[*end] != ';') &&
                 ((bracket && (buf[*end] != ')')) ||
                  (!bracket && (buf[*end] != '\n')))))) {
          switch (buf[*end]) {
          case '(':
            if (state == 0) {
              ++brackdepth;
            }
            break;

          case ')':
            if (state == 0) {
              --brackdepth;
            }
            break;

          case '\\':
            prev_esc = (state != 0) ? !prev_esc : 0;
            break;

          case '"':
          case '\'':
            if (!prev_esc) {
              state = (state == 0) ? buf[*end] : ((state == buf[*end]) ? 0 : state);
            }
          default:
            prev_esc = 0;
            break;
          }
          ++(*end);
        }

        /* right trim */
        end1 = *end - 1;
        while ((end1 >= 0) && isspace(buf[end1])) {
          --end1;
        }
        ++end1;

        add_arg(GP_Strndup(&buf[start1], end1 - start1));
        ++n_args;

        if (*end < *n) {
          if ((bracket && (buf[*end] == ')')) || (!bracket && ((buf[*end] == '\n') || (buf[*end] == ';')))) {
            if (buf[*end] == ';') {
              /* skip to the trailing newline */
              *end = (buf[*n - 1] == '\n') ? (*n - 1) : *n;
            }
            else {
              /* don't eat newline! */
              if (buf[*end] != '\n') {
                ++(*end);
              }
            }

            if (n_args == n_params) {
              int len = strlen(sub);

              /* substitute define parameters */
              if ((*n + len - mlen) >= max_size) {
                gpverror(GPE_INTERNAL, NULL, "Flex buffer too small.");
                return false;
              }
              else {
                DBG_printf("@1@substituting define parameter %*.*s ", mlen, mlen, &buf[begin]);

                BUF_REPLACE(buf, begin, *end, *n, sub, len, max_size);

                /* recurse preprocess with increased level */
                preprocess(buf, begin, end, n, max_size, &substitute_define_param, level + 1);
                free_arg_list();

                /* substitute defines */
                /* recurse preprocess with increased level */
                preprocess(buf, begin, end, n, max_size, &substitute_define, level + 1);

                DBG_printf("with %*.*s\n", *end - begin, *end - begin, &buf[begin]);
                return true;
              }
            }
            else {
              /* error n_args != n_params: no substitution */
              free_arg_list();
              return false;
            }
          }
          else if (buf[*end] != ',') {
            /* error unknown delimiter: no substitution */
            free_arg_list();
            return false;
          }
          else {
            ++(*end);
          }
        }
        if (*end >= *n) {
          /* error no ending bracket or newline: no substitution */
          free_arg_list();
          return false;
        }
      } /* for each argument */
    } /* if has parameters */
    else {
      int len = strlen(sub);

      /* substitute define */
      if ((*n + len - mlen) >= max_size) {
        gpverror(GPE_INTERNAL, NULL, "Flex buffer too small.");
        return false;
      }
      else {
        DBG_printf("@2@substituting define %*.*s ", mlen, mlen, &buf[begin]);

        BUF_REPLACE(buf, begin, *end, *n, sub, len, max_size);
        /* recurse preprocess with increased level */
        preprocess(buf, begin, end, n, max_size, &substitute_define, level + 1);

        DBG_printf("with %*.*s\n", *end - begin, *end - begin, &buf[begin]);
      }
    }
  }
  return false;
}

#define NELEM(x) (sizeof(x) / sizeof(*x))

static gp_boolean
no_process_iden(const char *iden, int len)
{
  static const char * const iden_tbl[] = {
    "#define",
    "#ifdef",
    "#ifndef",
    "#undefine",
    "define",
    "ifdef",
    "ifndef",
  };
  unsigned int i;

  for (i = 0; i < NELEM(iden_tbl); ++i) {
    if ((strlen(iden_tbl[i]) == len) && (strncasecmp(iden, iden_tbl[i], len) == 0)) {
      return true;
    }
  }
  return false;
}

static gp_boolean
preprocess(char *buf, int begin, int *end, int *n, int max_size, substitute_func_t substitute, int level)
{
  int start = -1;
  int state = 0;                   /* '"': in double quotes; '\'': in single quotes; ';': in comment */
  gp_boolean prev_esc = false;     /* true: prev char was escape */
  int in_hv = 0;                   /* in #v */
  gp_boolean number_start = false; /* true: possible start of a x'nnn' formatted number */
  gp_boolean substituted = false;  /* if there was a substitution in the preprocess run */
  int i;

  if (level >= PREPROC_MAX_DEPTH) {
    gpverror(GPE_STRCPLX, NULL);
    return false;
  }

  DBG_printf("---Preprocessing %*.*s\n", *end - begin, *end - begin, &buf[begin]);

  for (i = begin; i < *end; ++i) {
    int c = buf[i];

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
            int end1 = i - 2;
            int prev_n = *n;

            DBG_printf("@1@Preprocessing identifier: %*.*s\n", end1 - start, end1 - start, &buf[start]);
            substituted |= (*substitute)(buf, start, &end1, n, max_size, level);
            *end += *n - prev_n;
            i = end1 + 2;
          }
          start = -1;
        }
        in_hv = '(';
      }
      else {
        in_hv = 0;
      }

      if ((start == -1) && is_first_iden(c)) {
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
        if ((start != -1) && !is_iden(c)) {
          if ((level == 0) && no_process_iden(&buf[start], i - start)) {
            start = -1;
            break;
          }

          if ((c != '\'') || !number_start) {
            int prev_n = *n;

            DBG_printf("@2@Preprocessing identifier: %*.*s\n", i - start, i - start, &buf[start]);
            substituted |= (*substitute)(buf, start, &i, n, max_size, level);
            *end += *n - prev_n;
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

    case ';':
      if (state == 0) {
        state = c;
      }
      prev_esc = false;
      break;

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
    int prev_n = *n;

    DBG_printf("@3@Preprocessing identifier: %*.*s\n", i - start, i - start, &buf[start]);
    substituted |= (*substitute)(buf, start, &i, n, max_size, level);
    *end += *n - prev_n;
  }

  DBG_printf("+++Preprocessed %*.*s; substituted = %d\n", *end - begin, *end - begin, &buf[begin], substituted);

  return substituted;
}

static gp_boolean
preprocess_hv(char *buf, int begin, int *end, int *n, int max_size)
{
  char res_buf[11];
  gp_boolean substituted = false;

  DBG_printf("---preprocess_hv: %*.*s\n", *end - begin, *end - begin, &buf[begin]);

  while (begin < *end) {
    int res_len;

    DBG_printf("***Parsing chunck: %*.*s\n", *end - begin, *end - begin, &buf[begin]);
    if (ppparse_chunk(buf, begin, *end)) {
      substituted = true;
      DBG_printf ("col_begin = %d; col_end = %d; result = %d\n", ppcol_begin, ppcol_end, ppresult);
      res_len = snprintf(res_buf, sizeof(res_buf), "%d", ppresult);

      if ((*n + res_len - (ppcol_end - ppcol_begin)) >= max_size) {
        gpverror(GPE_INTERNAL, NULL, "Flex buffer too small.");
        return false;
      }
      else {
        int prev_n = *n;

        DBG_printf ("@@@Subtituting %*.*s ", ppcol_end - ppcol_begin, ppcol_end - ppcol_begin, &buf[ppcol_begin]);

        BUF_REPLACE(buf, ppcol_begin, ppcol_end, *n, res_buf, res_len, max_size);
        *end += *n - prev_n;
        DBG_printf ("with %*.*s\n", res_len, res_len, &buf[ppcol_begin]);
        begin = ppcol_end;
        DBG_printf("buf = %*.*s\n", *n, *n, buf);
      }
    }
    else {
      begin = ppcol_end;
    }
  }

  DBG_printf("+++preprocess_hv: %*.*s\n", *n, *n, buf);

  return substituted;
}

static void
preprocess_hv_params(char *buf, int begin, int *end, int *n, int max_size)
{
  int start = -1;
  int state = 0;            /* '"': in double quotes; '\'': in single quotes; ';': in comment; '(' in #v argument */
  gp_boolean prev_esc = 0;  /* true: prev char was escape */
  int in_hv = 0;            /* in #v */
  int hv_parenth = 0;       /* #v parenthesis nesting depth */
  int i;

  DBG_printf("---preprocess_hv_params: %*.*s\n", *end, *end, buf);

  for (i = begin; i < *end; ++i) {
    int c = buf[i];

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
          int prev_n = *n;

          preprocess_hv_params(buf, start, &i, n, max_size);
          preprocess(buf, start, &i, n, max_size, substitute_define, 0);
          preprocess_hv(buf, start, &i, n, max_size);
          *end += *n - prev_n;
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
      if ((start == -1) && is_first_iden(c)) {
        start = i;
      }
    }

    switch (c) {
    case '\\':
      prev_esc = ((state == '"') || (state == '\'')) ? (!prev_esc) : false;
      break;

    case ';':
      if (state == 0) {
        state = c;
      }
      prev_esc = false;
      break;

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

  DBG_printf("+++preprocess_hv_params: %*.*s\n", *end, *end, buf);
}

static const char *
check_macro_params(char *symbol, int symlen)
{
  const struct symbol *sym;
  const struct pnode *p;
  const char *subst = NULL;

  if ((sym = get_symbol_len(state.stMacroParams, symbol, symlen)) != NULL) {
    p = get_symbol_annotation(sym);
    if (p != NULL) {
      const struct pnode *p2 = HEAD(p);

      assert(p->tag == PTAG_LIST);
      assert(p2->tag == PTAG_STRING);
      subst = p2->value.string;

      assert(NULL == TAIL(p));

      if (subst == NULL) {
        subst = "";
      }
      else if ((strlen(subst) == symlen) && (strncmp(symbol, subst, symlen) == 0)) {
        /* check for a bad subsitution */
        subst = NULL;
      }
    } else {
      subst = "";
    }
  }

  return subst;
}

static gp_boolean
substitute_macro_param(char *buf, int begin, int *end, int *n, int max_size, int level)
{
  int mlen = *end - begin;
  const char *sub;

  if (mlen <= 0) {
    /* nothing to substitute */
    return false;
  }

  if ((sub = check_macro_params(&buf[begin], mlen)) != NULL) {
    int len = strlen(sub);

    if ((*n + len - mlen) >= max_size) {
      gpverror(GPE_INTERNAL, NULL, "Flex buffer too small.");
      return false;
    }
    else {
      DBG_printf("@@@substituting macro parameter %*.*s with %*.*s\n", mlen, mlen, &buf[begin], len, len, sub);

      BUF_REPLACE(buf, begin, *end, *n, sub, len, max_size);
      return true;
    }
  }
  return false;
}

static void
set_source_line(const char *line, int len, struct src_line_s *src_line)
{
  if (!src_line->line) {
    src_line->size = 128;
    src_line->line = GP_Malloc(src_line->size);
  }

  if (line[len - 1] == '\n') {  /* ignore trailing newline */
    --len;
  }
 
  if (src_line->size <= len) {
    do {
      src_line->size *= 2;
    }
    while (src_line->size <= len);
    src_line->line = GP_Realloc(src_line->line, src_line->size);
  }

  if (len > 0) {
    memcpy(src_line->line, line, len);
    src_line->line[len] = '\0';
  }
  else {
    src_line->line[0] = '\0';
  }
}

static inline gp_boolean
in_macro_expansion(void)
{
  struct source_context *p;

  for (p = state.src; p != NULL; p = p->prev) {
    if (p->type == SRC_MACRO) {
      return true;
    }
  }

  return false;
}

void
preprocess_line(char *buf, int *n, int max_size)
{
  gp_boolean res;
  int end = *n;

  if (IN_MACRO_WHILE_DEFINITION) {
    /* don't preprocess source line if in macro definition */
    set_source_line(buf, *n, &state.src->curr_src_line);
  }
  else {
    gp_boolean macro_expansion = in_macro_expansion();

    if (macro_expansion) {
      /* preprocess macro parameters */
      preprocess(buf, 0, &end, n, max_size, &substitute_macro_param, 1);
    }

    /* preprocess #v parameters */
    preprocess_hv_params(buf, 0, &end, n, max_size);
    preprocess_hv(buf, 0, &end, n, max_size);

    if (!macro_expansion) {
      /* set only #v processed source line if not in macro expansion */
      set_source_line(buf, *n, &state.src->curr_src_line);
    }

    /* preprocess line */
    do {
      res  = preprocess(buf, 0, &end, n, max_size, substitute_define, 0);
      res |= preprocess_hv(buf, 0, &end, n, max_size);
    }
    while (res);

    if (macro_expansion) {
      /* set processed source line if in macro expansion */
      set_source_line(buf, *n, &state.src->curr_src_line);
    }
  }

  if (state.preproc.f != NULL) {
    /* set current preprocessed source line if preprocessed asm file emission enabeled */
    set_source_line(buf, *n, &state.preproc.curr_src_line);
  }
}

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

static struct pnode *param_list = NULL;
struct arg_list_s {
  const char *str;
  struct arg_list_s *next;
} *arg_list = NULL, *arg_list_tail = NULL;

static char *
check_defines(char *symbol, int symlen, struct pnode **param_list_p)
{
  struct symbol *sym;
  struct pnode *p;
  char *subst = NULL;

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
      else if (strlen(subst) == symlen && strncmp(symbol, subst, symlen) == 0) {
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

  new->str = str;
  new->next = NULL;

  if (arg_list_tail) {
    arg_list_tail = arg_list_tail->next = new;
  }
  else {
    arg_list_tail = arg_list = new;
  }
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

static int
substitute_define_param(char *buf, int begin, int *end, int *n, int max_size, int level)
{
  int mlen = *end - begin;
  struct arg_list_s *argp = arg_list;
  struct pnode *parp = param_list;

  assert(mlen > 0);

  /* find argument */
  while (parp) {
    assert(HEAD(parp)->tag == PTAG_SYMBOL);
    if (0 == strncmp(&buf[begin], HEAD(parp)->value.symbol, mlen)) {
      /* substitute */
      int len = strlen(argp->str);

      DBG_printf("@@@substituting parameter %*.*s with %s\n", mlen, mlen, &buf[begin], argp->str);

      if (*n + len - mlen >= max_size) {
        gpverror(GPE_INTERNAL, NULL, "Flex buffer too small.");
        return 0;
      }
      else {
        BUF_REPLACE(buf, begin, *end, *n, argp->str, len, max_size);
        return 1;
      }
    }
    parp = TAIL(parp);
    argp = argp->next;
  }

  return 0; /* no substitutuon */
}

static int preprocess(char *buf, int begin, int *end, int *n, int max_size, int (*substitute)(char *buf, int begin, int *end, int *n, int max_size, int level), int level);

static int
substitute_define(char *buf, int begin, int *end, int *n, int max_size, int level)
{
  int mlen = *end - begin;
  char *sub;

  if (mlen <= 0) {
    /* nothing to substitute */
    return 0;
  }

  if (NULL != (sub = check_defines(&buf[begin], mlen, &param_list))) {
    int n_params = list_length(param_list);

    DBG_printf("define %*.*s has %d parameters\n", mlen, mlen, &buf[begin], n_params);
    if (0 != n_params) {
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

        while (*end < *n && (0 != state || 0 != brackdepth || (',' != buf[*end] && ';' != buf[*end] && ((bracket && ')' != buf[*end]) || (!bracket && '\n' != buf[*end]))))) {
          switch (buf[*end]) {
          case '(':
            if (0 == state) {
              ++brackdepth;
            }
            break;

          case ')':
            if (0 == state) {
              --brackdepth;
            }
            break;

          case '\\':
            prev_esc = (0 != state) ? !prev_esc : 0;
            break;

          case '"':
          case '\'':
            if (!prev_esc) {
              state = (0 == state) ? buf[*end] : ((state == buf[*end]) ? 0 : state);
            }
          default:
            prev_esc = 0;
            break;
          }
          ++(*end);
        }

        /* right trim */
        end1 = *end - 1;
        while (end1 >= 0 && isspace(buf[end1])) {
          --end1;
        }
        ++end1;

        add_arg(GP_Strndup(&buf[start1], end1 - start1));
        ++n_args;

        if (*end < *n) {
          if ((bracket && ')' == buf[*end]) || (!bracket && ('\n' == buf[*end] || ';' == buf[*end]))) {
            if (';' == buf[*end]) {
              /* skip to the trailing newline */
              *end = ('\n' == buf[*n - 1]) ? *n - 1 : *n;
            }
            else {
              /* don't eat newline! */
              if ('\n' != buf[*end]) {
                ++(*end);
              }
            }

            if (n_args == n_params) {
              int len = strlen(sub);

              /* substitute define parameters */
              if ((*n + len - mlen) >= max_size) {
                gpverror(GPE_INTERNAL, NULL, "Flex buffer too small.");
                return 0;
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
                return 1;
              }
            }
            else {
              /* error n_args != n_params: no substitution */
              free_arg_list();
              return 0;
            }
          }
          else if (',' != buf[*end]) {
            /* error unknown delimiter: no substitution */
            free_arg_list();
            return 0;
          }
          else {
            ++(*end);
          }
        }
        if (*end >= *n) {
          /* error no ending bracket or newline: no substitution */
          free_arg_list();
          return 0;
        }
      } /* for each argument */
    } /* if has parameters */
    else {
      int len = strlen(sub);

      /* substitute define */
      if ((*n + len - mlen) >= max_size) {
        gpverror(GPE_INTERNAL, NULL, "Flex buffer too small.");
        return 0;
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
  return 0;
}

#define NELEM(x) (sizeof(x) / sizeof(*x))

static int
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
  int i;

  for (i = 0; i < NELEM(iden_tbl); ++i) {
    if ((strlen(iden_tbl[i]) == len) && (strncasecmp(iden, iden_tbl[i], len) == 0)) {
      return 1;
    }
  }
  return 0;
}

static int
preprocess(char *buf, int begin, int *end, int *n, int max_size, int (*substitute)(char *buf, int begin, int *end, int *n, int max_size, int level), int level)
{
  int start = -1;
  int state = 0;        /* '"': in double quotes; '\'': in single quotes; ';': in comment */
  int prev_esc = 0;     /* 1: prev char was escape */
  int in_hv = 0;        /* in #v */
  int number_start = 0; /* 1: possible start of a x'nnn' formatted number */
  int substituted = 0;  /* if there was a substitution in the preprocess run */
  int i;

  if (level >= PREPROC_MAX_DEPTH) {
    gpverror(GPE_STRCPLX, NULL);
    return 0;
  }

  DBG_printf("---Preprocessing %*.*s\n", *end - begin, *end - begin, &buf[begin]);

  for (i = begin; i < *end; ++i) {
    int c = buf[i];

    if (0 == state) {
      if ('#' == c) {
        in_hv = '#';
      }
      else if (('#' == in_hv) && (('v' == c) || ('V' == c))) {
        in_hv = 'v';
      }
      else if (('v' == in_hv) && ('(' == c)) {
        if (-1 != start) {
          if (start < i - 2) {
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

      if (-1 == start && is_first_iden(c)) {
        switch (c) {
        case 'a': case 'A':
        case 'b': case 'B':
        case 'd': case 'D':
        case 'h': case 'H':
        case 'o': case 'O':
          number_start = 1;
          break;

        default:
          number_start = 0;
          break;
        }
        start = i;
      }
      else {
        if (-1 != start && !is_iden(c)) {
          if (0 == level && no_process_iden(&buf[start], i - start)) {
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
        number_start = 0;
      }
    }

    switch (c) {
    case '\\':
      prev_esc = (('"' == state) || ('\'' == state)) ? !prev_esc : 0;
      break;

    case ';':
      if (0 == state) {
        state = c;
      }
      prev_esc = 0;
      break;

    case '"':
    case '\'':
      if (!prev_esc && (state != ';')) {
        state = (0 == state) ? c : ((state == c) ? 0 : state);
      }
    default:
      prev_esc = 0;
      break;
    }
  }

  if (-1 != start) {
    int prev_n = *n;

    DBG_printf("@3@Preprocessing identifier: %*.*s\n", i - start, i - start, &buf[start]);
    substituted |= (*substitute)(buf, start, &i, n, max_size, level);
    *end += *n - prev_n;
  }

  DBG_printf("+++Preprocessed %*.*s; substituted = %d\n", *end - begin, *end - begin, &buf[begin], substituted);

  return substituted;
}

static int
preprocess_hv(char *buf, int begin, int *end, int *n, int max_size)
{
  char res_buf[11];
  int substituted = 0;

  DBG_printf("---preprocess_hv: %*.*s\n", *end - begin, *end - begin, &buf[begin]);

  while (begin < *end) {
    int res_len;

    DBG_printf("***Parsing chunck: %*.*s\n", *end - begin, *end - begin, &buf[begin]);
    if (ppparse_chunk(buf, begin, *end)) {
      substituted = 1;
      DBG_printf ("col_begin = %d; col_end = %d; result = %d\n", ppcol_begin, ppcol_end, ppresult);
      res_len = snprintf (res_buf, sizeof (res_buf), "%d", ppresult);

      if ((*n + res_len - (ppcol_end - ppcol_begin)) >= max_size) {
        gpverror(GPE_INTERNAL, NULL, "Flex buffer too small.");
        return 0;
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
  int state = 0;        /* '"': in double quotes; '\'': in single quotes; ';': in comment; '(' in #v argument */
  int prev_esc = 0;     /* 1: prev char was escape */
  int in_hv = 0;        /* in #v */
  int hv_parenth = 0;   /* #v parenthesis nesting depth */
  int i;

  DBG_printf("---preprocess_hv_params: %*.*s\n", *end, *end, buf);

  for (i = begin; i < *end; ++i) {
    int c = buf[i];

    if ('(' == state) {
      if ('(' == in_hv) {
        start = i;
        in_hv = 0;
      }

      if ('(' == c) {
        ++hv_parenth;
      }
      else if (')' == c) {
        if (0 >= --hv_parenth) {
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
    else if (0 == state) {
      if ('#' == c) {
        in_hv = '#';
      }
      else if (('#' == in_hv) && (('v' == c) || ('V' == c))) {
        in_hv = 'v';
      }
      else if (('v' == in_hv) && ('(' == c)) {
        in_hv = '(';
        ++hv_parenth;
        state = '(';
      }
      else {
        in_hv = 0;
      }
    }

    if (0 == state) {
      if (-1 == start && is_first_iden(c)) {
        start = i;
      }
    }

    switch (c) {
    case '\\':
      prev_esc =  (('"' == state) || ('\'' == state)) ? !prev_esc : 0;
      break;

    case ';':
      if (0 == state) {
        state = c;
      }
      prev_esc = 0;
      break;

    case '"':
    case '\'':
      if (!prev_esc && (state != ';')) {
        state = (0 == state) ? c : ((state == c) ? 0 : state);
      }
    default:
      prev_esc = 0;
      break;
    }
  }

  DBG_printf("+++preprocess_hv_params: %*.*s\n", *end, *end, buf);
}

static char *
check_macro_params(char *symbol, int symlen)
{
  struct symbol *sym;
  struct pnode *p;
  char *subst = NULL;

  if ((sym = get_symbol_len(state.stMacroParams, symbol, symlen)) != NULL) {
    p = get_symbol_annotation(sym);
    if (p != NULL) {
      struct pnode *p2 = HEAD(p);

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

static int
substitute_macro_param(char *buf, int begin, int *end, int *n, int max_size, int level)
{
  int mlen = *end - begin;
  char *sub;

  if (mlen <= 0) {
    /* nothing to substitute */
    return 0;
  }

  if ((sub = check_macro_params(&buf[begin], mlen)) != NULL) {
    int len = strlen(sub);

    if ((*n + len - mlen) >= max_size) {
      gpverror(GPE_INTERNAL, NULL, "Flex buffer too small.");
      return 0;
    }
    else {
      DBG_printf("@@@substituting macro parameter %*.*s with %*.*s\n", mlen, mlen, &buf[begin], len, len, sub);

      BUF_REPLACE(buf, begin, *end, *n, sub, len, max_size);
      return 1;
    }
  }
  return 0;
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
    src_line->line = realloc(src_line->line, src_line->size);
  }

  if (len > 0) {
    memcpy(src_line->line, line, len);
    src_line->line[len] = '\0';
  }
  else {
    src_line->line[0] = '\0';
  }
}

static inline int
in_macro_expansion(void)
{
  struct source_context *p;

  for (p = state.src; p != NULL; p = p->prev) {
    if (p->type == SRC_MACRO) {
      return 1;
    }
  }

  return 0;
}

void
preprocess_line(char *buf, int *n, int max_size)
{
  int res;
  int end = *n;

  if (IN_MACRO_WHILE_DEFINITION) {
    /* don't preprocess source line if in macro definition */
    set_source_line(buf, *n, &state.src->curr_src_line);
  }
  else {
    int macro_expansion = in_macro_expansion();

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
      res = preprocess(buf, 0, &end, n, max_size, substitute_define, 0);
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

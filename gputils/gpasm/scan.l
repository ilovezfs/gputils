/* %option debug */
%option noyywrap
%option always-interactive
%x bquote
%x lnquote
%x define
%x definition
%x defargs
%x macpar

%{
/* lexical analyser for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin

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

#if !defined(YY_FLEX_MAJOR_VERSION) || \
  !defined(YY_FLEX_MINOR_VERSION) || \
  !defined(YY_FLEX_SUBMINOR_VERSION) || \
  YY_FLEX_MAJOR_VERSION < 2 || \
  (YY_FLEX_MAJOR_VERSION == 2 && YY_FLEX_MINOR_VERSION < 5) || \
  (YY_FLEX_MAJOR_VERSION == 2 && YY_FLEX_MINOR_VERSION == 5 && YY_FLEX_SUBMINOR_VERSION < 35)
# error "flex 2.5.35 or newer required!"
#endif

#include "stdhdr.h"

#include "libgputils.h"
#include "gpasm.h"
#include "parse.h"
#include "scan.h"
#include "deps.h"
#include "gperror.h"
#include "directive.h"
#include "evaluate.h"
#include "macro.h"
#include "coff.h"
#include "preprocess.h"
#include "lst.h"

#define OPERATOR(x)  return (yylval.i = (x))

enum identtype {
  ID_UNKNOWN_TYPE,
  ID_MACRO_PARAMS,
  ID_DEFINES,
  ID_DIRECTIVES,
  ID_GLOBALS,
  ID_MACROS,
  ID_OPCODES
  };

gp_boolean force_decimal; /* Used to force radix to decimal for some directives. */
gp_boolean force_ident;   /* Used to force numbers to identifiers for processor names. */

static enum identtype identify(const char *);
static int found_eof(void);

static inline size_t
gp_input(char *buf, size_t max_size)
{
  if (IN_FILE_EXPANSION)
    {
      /* not in macro expansion */
      if (YY_CURRENT_BUFFER_LVALUE->yy_is_interactive)
        {
          int c = '*';
          size_t n;

          for (n = 0; (n < max_size) && ((c = getc(yyin)) != EOF) && (c != '\n'); ++n) {
            buf[n] = (char)c;
          }

          if (c == '\n') {
            /* skip CR followed by LF */
            if ((n > 0) && (buf[n - 1] == '\r')) {
              --n;
            }
            buf[n++] = (char)c;
          }

          if ((c == EOF) && ferror(yyin)) {
            gpverror(GPE_SCANNER, NULL, "Input in flex scanner failed.");
          }
          return n;
        }
      else
        {
          gpverror(GPE_SCANNER, NULL, "Interactive scanner should be used.");
          return 0;
        }
    }
  else
    {
      /* in macro expansion */
      if (state.src->mac_body != NULL)
        {
          size_t n = strlen(state.src->mac_body->src_line);

          if (n > (max_size - 1)) {
            n = max_size - 2;
          }

          strncpy(buf, state.src->mac_body->src_line, n);
          buf[n++] = '\n'; /* add newline */
          return n;
        }
      else
        {
          buf[0] = buf[1] = YY_END_OF_BUFFER_CHAR;
          return 0;
        }
    }
}

static inline size_t
gp_yyinput(char *buf, size_t max_size)
{
  int result = gp_input(buf, max_size);

  if (result)
    {
      /* preprocess line */
      preprocess_line(buf, &result, max_size);

      state.src->last_char_is_nl = (buf[result - 1] == '\n');
    }
  else if (!state.src->last_char_is_nl)
    {
      *buf = '\n';
      result = 1;
      state.src->last_char_is_nl = true;
    }
  return result;
}

#define YY_INPUT(buf, result, max_size) result = gp_yyinput(buf, max_size)
%}

IDENT  [.]?[a-z_\x80-\xff?@#][a-z_0-9\x80-\xff.?@#]*
ESCCH  \\([0abfnrtv\\?'"]|0[0-7]{2}|x[0-9a-f]{2})
STR_QCHAR  ([^"\r\n]|{ESCCH})
STR_BCHAR  ([^>\r\n]|{ESCCH})
NUMCHAR [0-9a-z]

%%
^[ \t]*#?include[ \t]+   {
                           yylval.s = "include";
                           BEGIN(bquote);
                           return IDENTIFIER;
                         }
<<EOF>>                  {
                           if (found_eof()) {
                             yyterminate();
                           }
                         }
^[ \t]*title[ \t]+       {
                           BEGIN(lnquote);
                           yylval.s = "title";
                           return IDENTIFIER;
                         }
^[ \t]*(subtitle?|stitle)[ \t]+ {
                           BEGIN(lnquote);
                           yylval.s = "subtitle";
                           return IDENTIFIER;
                         }
cblock                   {
                           return CBLOCK;
                         }
errorlevel               {
                           yylval.s = GP_Strdup(yytext);
                           return ERRORLEVEL;
                         }
endc                     {
                           return ENDC;
                         }
fill[ \t]*\(             {
                           /* fill with ( ) as first argument */
                           yylval.i = FILL;
                           return FILL;
                         }
^[ \t]*#define[ \t]*$    {
                           yylval.s = "#define";
                           return DEFINE;
                         }
^[ \t]*#define[ \t]+     {
                           BEGIN(define);
                           yylval.s = "#define";
                           return DEFINE;
                         }
<define>{IDENT}\(        {
                           BEGIN(defargs);
                           yylval.s = GP_Strndup(yytext, yyleng - 1);
                           return IDENT_BRACKET;
                         }
<defargs>{IDENT}         {
                           yylval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
<defargs>\)              {
                           BEGIN(definition);
                           yylval.i = yytext[0];
                           return ')';
                         }
<define>{IDENT}          {
                           BEGIN(definition);
                           yylval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
upper                    {
                           if (force_ident) {
                             yylval.s = GP_Strdup(yytext);
                             return IDENTIFIER;
                           }
                           else {
                             yylval.i = UPPER;
                             return UPPER;
                           }
                         }
high                     {
                           if (force_ident) {
                             yylval.s = GP_Strdup(yytext);
                             return IDENTIFIER;
                           }
                           else {
                             yylval.i = HIGH;
                             return HIGH;
                          }
                         }
low                      {
                           if (force_ident) {
                             yylval.s = GP_Strdup(yytext);
                             return IDENTIFIER;
                           }
                           else {
                             yylval.i = LOW;
                             return LOW;
                           }
                         }
list                     {
                           yylval.s = GP_Strdup(yytext);
                           return LIST;
                         }
config                   {
                           force_ident = true;
                           yylval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
processor                {
                           yylval.s = GP_Strdup(yytext);
                           return PROCESSOR;
                         }
#?if                     {
                           /* #if and if can appear in column 1 */
                           yylval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
#?else                   {
                           /* #else and else can appear in column 1 */
                           yylval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
#?endif                  {
                           /* #endif and endif can appear in column 1 */
                           yylval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
#?ifdef                  {
                           /* #ifdef and ifdef can appear in column 1 */
                           yylval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
#?ifndef                 {
                           /* #ifndef and ifndef can appear in column 1 */
                           yylval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
#undefine                {
                           /* #undefine can appear in column 1 */
                           yylval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
"."line                  {
                           yylval.s = GP_Strdup(yytext);
                           return DEBUG_LINE;
                         }
^{IDENT}:?               {
                           int has_collon = 0;
                           struct symbol *sym;
                           struct macro_head *h;

                           if (yytext[strlen(yytext) - 1] == ':') {
                             yytext[strlen(yytext) - 1] = '\0';
                             has_collon = 1;
                           }
                           yylval.s = GP_Strdup(yytext);
                           switch (identify(yytext)) {
                             case ID_DIRECTIVES:
                               gpvwarning(GPW_DIR_COLUMN_ONE, NULL, yytext);
                               if (has_collon) {
                                 gpverror(GPE_BADCHAR, NULL, ':');
                               }
                               return IDENTIFIER;
                               break;

                             case ID_MACROS:
                               /* Macro invocation */
                               BEGIN(macpar);
                               sym = get_symbol(state.stMacros, yytext);
                               h = get_symbol_annotation(sym);
                               /* TODO: this condition shoul be probably removed
                                * since the symbol table is reinitialized before
                                * the second pass */
                               if (h->line_number == state.src->line_number) {
                                 return LABEL;
                               }
                               else {
                                 gpvwarning(GPW_MACRO_COLUMN_ONE, NULL, yytext);
                                 if (has_collon) {
                                   gpverror(GPE_BADCHAR, NULL, ':');
                                 }
                                 return IDENTIFIER;
                               }
                               break;

                             case ID_OPCODES:
                               gpvwarning(GPW_OP_COLUMN_ONE, NULL, yytext);
                               if (has_collon)
                                 gpverror(GPE_BADCHAR, NULL, ':');
                               return IDENTIFIER;
                               break;

                             case ID_UNKNOWN_TYPE:
                               return LABEL;

                             default:
                               return LABEL;
                           }
                         }
{IDENT}                  {
                           if (NULL != get_symbol(state.stMacros, yytext)) {
                             /* Macro invocation */
                             BEGIN(macpar);
                           }
                           yylval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
0x{NUMCHAR}+             {
                           yylval.i = stringtolong(yytext + 2, 16);
                           return NUMBER;
                         }
{NUMCHAR}+b              {
                           if (force_ident) {
                             yylval.s = GP_Strdup(yytext);
                             return IDENTIFIER;
                           }
                           else if (state.radix == 16) {
                             yylval.i = stringtolong(yytext, 16);
                             return NUMBER;
                           }
                           else {
                             yytext[yyleng - 1] = '\0';
                             yylval.i = stringtolong(yytext, 2);
                             return NUMBER;
                           }
                         }
b'-?{NUMCHAR}+'          {
                           yytext[yyleng - 1] = '\0';
                           yylval.i = stringtolong(yytext + 2, 2);
                           return NUMBER;
                         }
{NUMCHAR}+[oq]           {
                           if (force_ident) {
                             yylval.s = GP_Strdup(yytext);
                             return IDENTIFIER;
                           }
                           else {
                             yytext[yyleng - 1] = '\0';
                             yylval.i = stringtolong(yytext, 8);
                             return NUMBER;
                           }
                         }
[oq]'-?{NUMCHAR}+'       {
                           yytext[yyleng - 1] = '\0';
                           yylval.i = stringtolong(yytext + 2, 8);
                           return NUMBER;
                         }
{NUMCHAR}+d              {
                           if (force_ident) {
                             yylval.s = GP_Strdup(yytext);
                             return IDENTIFIER;
                           }
                           else if (state.radix == 16) {
                             yylval.i = stringtolong(yytext, 16);
                             return NUMBER;
                           }
                           else {
                             yytext[yyleng - 1] = '\0';
                             yylval.i = stringtolong(yytext, 10);
                             return NUMBER;
                           }
                         }
d'-?[0-9]+'              {
                           yytext[yyleng - 1] = '\0';
                           yylval.i = stringtolong(yytext + 2, 10);
                           return NUMBER;
                         }
"."[0-9]+                {
                           yylval.i = stringtolong(yytext + 1, 10);
                           return NUMBER;
                         }
{NUMCHAR}+h              {
                           if (force_ident) {
                             yylval.s = GP_Strdup(yytext);
                             return IDENTIFIER;
                           }
                           else {
                             yytext[yyleng - 1] = '\0';
                             yylval.i = stringtolong(yytext, 16);
                             return NUMBER;
                           }
                         }
h'-?{NUMCHAR}+'          {
                           yytext[yyleng - 1] = '\0';
                           yylval.i = stringtolong(yytext + 2, 16);
                           return NUMBER;
                         }
{NUMCHAR}+               {
                           if (force_ident) {
                             yylval.s = GP_Strdup(yytext);
                             return IDENTIFIER;
                           } 
                           else if (force_decimal) {
                             yylval.i = stringtolong(yytext, 10);
                             return NUMBER;
                           }
                           else {
                             yylval.i = stringtolong(yytext, state.radix);
                             return NUMBER;
                           }
                         }
<INITIAL,bquote>\"{STR_QCHAR}*\"? {
                           if ((yyleng > 1) && (yytext[yyleng - 1] == '"')) {
                             --yyleng;
                           }
                           else {
                             gpwarning(GPW_MISSING_QUOTE, NULL);
                           }
                           yylval.s = GP_Strndup(yytext + 1, yyleng - 1);
                           BEGIN(INITIAL);
                           return STRING;
                         }
'{STR_QCHAR}'            {
                           const char *pc = convert_escape_chars(yytext + 1, &yylval.i);
                           if (pc != &yytext[yyleng - 1]) {
                             gpverror(GPE_ILLEGAL_ARGU, NULL, "Expected single character.");
                           }
                           return NUMBER;
                         }
a'{STR_QCHAR}'           {
                           yylval.i = yytext[2];
                           return NUMBER;
                         }
<bquote>\<{STR_BCHAR}*\> {
                           yylval.s = GP_Strndup(yytext + 1, yyleng - 2);
                           BEGIN(INITIAL);
                           return STRING;
                         }
<bquote>[^ \t<>";\r\n]+  { /*"*/
                           /* unquoted (special-case) string */
                           yylval.s = GP_Strdup(yytext);
                           BEGIN(INITIAL);
                           return STRING;
                         }
<lnquote>\"{STR_QCHAR}*\" { /*"*/
                           /* if valid, must match with length >= unquoted token below */
                           yylval.s = GP_Strndup(yytext + 1, yyleng - 2);
                           BEGIN(INITIAL);
                           return STRING;
                         }
<definition,lnquote>[^ \t;\r\n]+([ \t]+[^ \t;\r\n]+)* {
                           /* full-line (special-case) string */
                           /* must begin and end with non-whitespace */
                           yylval.s = GP_Strdup(yytext);
                           BEGIN(INITIAL);
                           return STRING;
                         }
"<<"                     OPERATOR(LSH);
">>"                     OPERATOR(RSH);
">="                     OPERATOR(GREATER_EQUAL);
"<="                     OPERATOR(LESS_EQUAL);
"=="                     OPERATOR(EQUAL);
"!="                     OPERATOR(NOT_EQUAL);
"&&"                     OPERATOR(LOGICAL_AND);
"||"                     OPERATOR(LOGICAL_OR);

"+="                     OPERATOR(ASSIGN_PLUS);
"-="                     OPERATOR(ASSIGN_MINUS);
"*="                     OPERATOR(ASSIGN_MULTIPLY);
"/="                     OPERATOR(ASSIGN_DIVIDE);
"%="                     OPERATOR(ASSIGN_MODULUS);
"<<="                    OPERATOR(ASSIGN_LSH);
">>="                    OPERATOR(ASSIGN_RSH);
"&="                     OPERATOR(ASSIGN_AND);
"|="                     OPERATOR(ASSIGN_OR);
"^="                     OPERATOR(ASSIGN_XOR);

"++"                     OPERATOR(INCREMENT);
"--"                     OPERATOR(DECREMENT);

"*+"                     OPERATOR(TBL_POST_INC);
"*-"                     OPERATOR(TBL_POST_DEC);
"+*"                     OPERATOR(TBL_PRE_INC);

<INITIAL,bquote,lnquote,definition,defargs,macpar>[ \t\r]*
<macpar>.                {
#define CHUNK  64
                           static gp_boolean emit_comma = false;

                           unsigned int len = CHUNK;
                           unsigned int i = 0;
                           char *buf = GP_Malloc(len);
                           int c = yytext[0];
                           gp_boolean prev_esc = false;
                           int state = 0;

                           if (emit_comma) {
                             unput(c);
                             emit_comma = false;
                             yylval.i = ',';
                             return ',';
                           }

                           /* skip leading spaces */
                           while (isspace(c)) {
                             c = input();
                           }

                           for (; ; ) {
                             switch (c) {
                             case ',':
                               if (state != 0) {
                                 goto no_esc;
                               }

                               emit_comma = true;
                               goto skip_trailing;

                             case ';':
                               if (state != 0) {
                                 goto no_esc;
                               }

                               /* eat the rest of line */
                               while (((c = input()) != EOF) && (c != '\n'))
                                 ;
                               /* fall through */
                             case '\n':
                               unput('\n');
                               /* fall through */
                             skip_trailing:
                               /* skip trailing spaces */
                               while ((i > 0) && isspace(buf[i - 1])) {
                                 --i;
                               }
                               /* fall through */
                             case EOF:
                               yylval.s = GP_Strndup(buf, i);
                               free(buf);
                               return IDENTIFIER;

                             case '\\':
                               prev_esc = !prev_esc;
                               break;

                             case '"':
                             case '\'':
                               /* escaping out of string doesn't have any special meaning */
                               if ((state == 0) || ((state != 0) && !prev_esc)) {
                                 state = (state == 0) ? c : ((state == c) ? 0 : state);
                               }
                               /* fall through */
                             default:
                             no_esc:
                               prev_esc = false;
                               break;
                             }

                             buf[i++] = c;
                             if (len <= i) {
                               len += CHUNK;
                               buf = GP_Realloc(buf, len);
                             }
                             c = input();
                           }
                         }
<*>[\n]                  {
                           force_decimal = false;
                           force_ident   = false;
                           BEGIN(INITIAL);  /* switch to INITIAL state */
                           return yytext[0];
                         }
<*>;.*                   {
                           BEGIN(INITIAL);  /* switch to INITIAL state */
                         }
<*>.                     {
                           yylval.i = yytext[0];
                           return yytext[0];
                         }
%%

static void
search_paths(struct source_context *new, const char *name)
{
  char tryname[PATH_MAX + 1];
  int i;

  for (i = 0; i < state.path_num; i++) {
    snprintf(tryname, sizeof(tryname), "%s" COPY_CHAR "%s", state.paths[i], name);
    new->f = fopen(tryname, "rt");

    if (new->f != NULL) {
      new->name = GP_Strdup(tryname);
      break;
    }
  }
}

void
open_src(const char *name, gp_boolean isinclude)
{
  extern FILE *yyin;
  struct source_context *new = GP_Calloc(1, sizeof(struct source_context));

  if (state.src != NULL) {
    state.src->yybuf = YY_CURRENT_BUFFER;
  }

  new->f = fopen(name, "rt");

  if (new->f != NULL) {
    new->name = GP_Strdup(name);
  }
  else if (isinclude && (strchr(name, PATH_CHAR) == 0)) {
    /* If include file and no PATH_CHAR in name, try searching include path. */
    search_paths(new, name);

    if (new->f == NULL) {
      /* We didn't find a match so check for lower case. This is mainly for
         Microchip examples and some includes in which filenames were written
         without regard to case. */
      char *lower_case_name = gp_lower_case(name);

      search_paths(new, lower_case_name);
      free(lower_case_name);

      if (new->f != NULL) {
        gpwarning(GPW_UNKNOWN, "Found lower case match for include filename.");
      }
    }
  }

  if (new->f == NULL) {
    if (state.src != NULL) {
      gpverror(GPE_NOENT, NULL, name);
    }
    else {
      perror(name);
      exit(1);
    }
  }
  else {
    yyin = new->f;

    if (state.src != NULL) {
      yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
    }

    if (state.use_absolute_path) {
      new->name = gp_absolute_path(new->name);
    }

    new->type = SRC_FILE;
    new->mac_head = NULL;
    new->line_number = 1;
    new->loop_number = 1;
    new->file_symbol = (state.debug_info) ? NULL : coff_add_filesym(new->name, isinclude);
    new->prev = state.src;

    state.src = new;
    state.src->fc = add_file(FT_SRC, new->name);
    deps_add(new->name);

    if (!isinclude) {
      /* it is the top level file so initialize the lexer */
      force_decimal = false;
      force_ident   = false;
    }
    state.src->last_char_is_nl = true;
    state.found_end = 0;
  }
}

void
execute_macro(struct macro_head *h, gp_boolean is_while)
{
  struct source_context *new = GP_Calloc(1, sizeof(struct source_context));

  assert(state.src != NULL);
  state.src->yybuf = YY_CURRENT_BUFFER;
  /* store the stack so it can be returned when the macro is complete */
  state.src->astack = state.astack;

  /* create new source_context */
  new->name = GP_Strdup(h->src_name);
  new->type = (is_while) ? SRC_WHILE : SRC_MACRO;

  new->line_number = h->line_number + 1;
  new->loop_number = 1;
  new->file_symbol = h->file_symbol;
  new->f = NULL;
  new->mac_head = h;
  new->mac_body = h->body;

  new->prev = state.src;
  state.src = new;

  state.src->fc = add_file(FT_SRC, new->name); /* scan list for fc */
  yy_switch_to_buffer(yy_create_buffer(NULL, YY_BUF_SIZE));
}

void
repeat_while(void)
{
  struct macro_head *h;

  h = state.src->mac_head;

  state.src->line_number = h->line_number + 1;
  state.src->loop_number++;
  state.src->mac_body = h->body;

  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer(yy_create_buffer(NULL, YY_BUF_SIZE));
}

void
close_file(void)
{
  struct source_context *old = state.src;

  if (IN_FILE_EXPANSION) {
    if (state.src->f != NULL) {
      fclose(state.src->f);
    }
    free(state.src->name);

    if (state.src->prev == NULL) {
      coff_cleanup_before_eof();
    }

    if (!state.debug_info) {
      coff_add_eofsym();
    }
    state.src = state.src->prev;
  }
  else if (state.src->type == SRC_MACRO) {
    state.src = state.src->prev;

    state.stTop = pop_symbol_table(state.stTop);
    state.stMacroParams = pop_symbol_table(state.stMacroParams);

    if (state.src->astack != state.astack) {
      gpverror(GPE_ILLEGAL_NESTING, NULL);
    }

    assert(state.stTop != NULL);
    assert(state.stMacroParams != NULL);
    free(old->name);
  }
  else if (state.src->type == SRC_WHILE) {
    free(state.src->name);
    state.src = state.src->prev;
  }
  free(old);
}

void
execute_exitm(void)
{
  struct amode *previous;
  struct amode *old;

  /* The macro is ended early, so return the stack to its previous state */
  previous = state.src->prev->astack;
  while ((state.astack != NULL) && (state.astack != previous)) {
    old = state.astack;
    state.astack = state.astack->prev;
    free(old);
  }

  close_file();

  if (state.src != NULL) {
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(state.src->yybuf);
  }
}

/* found end directive, close all files and stop the parser */
int
found_end(void)
{
  /* close all open files */
  while (state.src != NULL) {
    close_file();
  }

  /* make sure the buffer is empty when pass 2 starts */
  if (YY_CURRENT_BUFFER) {
    yy_flush_buffer(YY_CURRENT_BUFFER);
  }

  return 1;
}

static int
found_eof(void)
{
  int terminate = 0;
  enum src_types prev_type;

  if (IN_WHILE_EXPANSION) {
    if (maybe_evaluate(state.src->mac_head->parms)) {
      if (state.src->loop_number > 255) {
        gpverror(GPE_BAD_WHILE_LOOP, NULL);
      }
      else {
        /* repeat the while loop */
        repeat_while();
        return 0;
      }
    }
  }

  prev_type = state.src->type;
  close_file();

  if (state.src != NULL) {
    /* Just an include file. */
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(state.src->yybuf);

    if ((state.pass == 2) && (prev_type == SRC_WHILE)) {
      /* Force ENDW listing */
      state.lst.line.linetype = LTY_DIR;
      /* Line number was alreay incremented, so it has to be decremented,
       * source line listed and line number incremented again. */
      --state.src->line_number;
      lst_format_line(state.src->curr_src_line.line, 0);
      ++state.src->line_number;
    }
  }
  else {
    if (!state.found_end) {
      gperror(GPE_ILLEGAL_COND, "Illegal condition: EOF encountered before END.");
    }

    terminate = found_end();
  }

  return terminate;
}

static enum identtype
identify(const char *text)
{
  enum identtype type;
  struct symbol *sym;

  if ((sym = get_symbol(state.stMacroParams, text)) != NULL) {
    type = ID_MACRO_PARAMS;
  }
  else if ((sym = get_symbol(state.stDefines, text)) != NULL) {
    type = ID_DEFINES;
  }
  else if ((sym = get_symbol(state.stDirective, text)) != NULL) {
    type = ID_DIRECTIVES;

    if ((state.mpasm_compatible) && (strcasecmp(text, "idlocs") == 0)) {
      type = ID_UNKNOWN_TYPE;
    }
  }
  else if ((sym = get_symbol(state.stBuiltin, text)) != NULL) {
    type = ID_OPCODES;
  }
  else if ((sym = get_symbol(state.stGlobal, text)) != NULL) {
    type = ID_GLOBALS;
  }
  else if ((sym = get_symbol(state.stMacros, text)) != NULL) {
    type = ID_MACROS;
  }
  else {
    type = ID_UNKNOWN_TYPE;
  }

  return type;
}

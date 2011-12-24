%option noyywrap
%x bquote
%x lnquote
%x define
%x definition

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

#define OPERATOR(x)  return (yylval.i = (x))
/* YY_UNPUT not used, suppress the warning */
#define YY_NO_UNPUT

enum identtype { defines, directives, globals, macros, opcodes, unknown_type };
enum identtype identify(char *);

static void push_string(char *str);
static int found_eof(void);

static char *check_defines(char *symbol);

static int quoted; /* Used to prevent #define expansion in ifdef and
                      ifndef... */
int force_decimal; /* Used to force radix to decimal for some directives */
int force_ident; /* Used to force numbers to identifiers for processor names */

#define GP_INPUT(buf,result,max_size) \
        if ( YY_CURRENT_BUFFER_LVALUE->yy_is_interactive ) \
                { \
                int c = '*'; \
                size_t n; \
                for ( n = 0; n < max_size && \
                             (c = getc( yyin )) != EOF && c != '\n'; ++n ) \
                        buf[n] = (char) c; \
                if ( c == '\n' ) \
                        buf[n++] = (char) c; \
                if ( c == EOF && ferror( yyin ) ) \
                        YY_FATAL_ERROR( "input in flex scanner failed" ); \
                result = n; \
                } \
        else \
                { \
                errno=0; \
                while ( (result = fread(buf, 1, max_size, yyin))==0 && ferror(yyin)) \
                        { \
                        if( errno != EINTR) \
                                { \
                                YY_FATAL_ERROR( "input in flex scanner failed" ); \
                                break; \
                                } \
                        errno=0; \
                        clearerr(yyin); \
                        } \
                }

#define YY_INPUT(buf,result,max_size) \
  { \
    GP_INPUT(buf,result,max_size); \
    if (result) \
      { \
        state.src->last_char_is_nl = (buf[result - 1] == '\n'); \
      } \
    else if (!state.src->last_char_is_nl) \
      { \
        *buf = '\n'; \
        result = 1; \
        state.src->last_char_is_nl = true; \
      } \
  }
%}

IDENT  [.]?[a-z_\x80-\xff?@#][a-z_0-9\x80-\xff.?@#]*
ESCCH  \\([abfnrtv\\?'"]|0[0-7]{2}|x[0-9a-f]{2})
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
                           if (found_eof())
                             yyterminate();
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
                           yylval.s = strdup(yytext);
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
^[ \t]*#define[ \t]+     {
                           BEGIN(define);
                           yylval.s = "#define";
                           return DEFINE;
                         }
<define>{IDENT}          {
                           BEGIN(definition);
                           yylval.s = strdup(yytext);
                           return IDENTIFIER;
                         }
upper                    {
                           yylval.i = UPPER;
                           return UPPER;
                         }
high                     {
                           yylval.i = HIGH;
                           return HIGH;
                         }
low                      {
                           yylval.i = LOW;
                           return LOW;
                         }
list                     {
                           yylval.s = strdup(yytext);
                           return LIST;
                         }
processor                {
                           yylval.s = strdup(yytext);
                           return PROCESSOR;
                         }
#?if                     {
                           /* #if and if can appear in column 1 */
                           yylval.s = strdup(yytext);
                           return IDENTIFIER;
                         }
#?else                   {
                           /* #else and else can appear in column 1 */
                           yylval.s = strdup(yytext);
                           return IDENTIFIER;
                         }
#?endif                  {
                           /* #endif and endif can appear in column 1 */
                           yylval.s = strdup(yytext);
                           return IDENTIFIER;
                         }
#?ifdef                  {
                           /* #ifdef and ifdef can appear in column 1 */
                           quoted = 1;
                           yylval.s = strdup(yytext);
                           return IDENTIFIER;
                         }
#?ifndef                 {
                           /* #ifndef and ifndef can appear in column 1 */
                           quoted = 1;
                           yylval.s = strdup(yytext);
                           return IDENTIFIER;
                         }
#undefine                {
                           /* #undefine can appear in column 1 */
                           quoted = 1;
                           yylval.s = strdup(yytext);
                           return IDENTIFIER;
                         }
"."line                  {
                           yylval.s = strdup(yytext);
                           return DEBUG_LINE;
                         }
^{IDENT}#v\(             {
                           char *symbol;

                           yytext[strlen(yytext) - 3] = '\0';

                           symbol = check_defines(yytext);
                           if (symbol) {
                             yylval.s = strdup(symbol);
                           } else {
                             yylval.s = strdup(yytext);
                           }
                           return VARLAB_BEGIN;
                         }
^{IDENT}:?               {
                           int has_collon = 0;
                           struct symbol *sym;
                           struct macro_head *h;
                           char *subst;

                           if (yytext[strlen(yytext) - 1] == ':') {
                             yytext[strlen(yytext) - 1] = '\0';
                             has_collon = 1;
                           }
                           yylval.s = strdup(yytext);
                           switch(identify(yytext)) {
                             case defines:
                               sym = get_symbol(state.stTopDefines, yytext);
                               subst = get_symbol_annotation(sym);
                               push_string(subst);
                               break;
                             case directives:
                               gpwarning(GPW_DIR_COLUMN_ONE, NULL);
                               if (has_collon)
                                 gperror(GPE_BADCHAR, "Illegal character (:)");
                               return IDENTIFIER;
                               break;
                             case macros:
                               if(asm_enabled()) {
                                 /* make sure macro definition on second pass
                                    is ignored */
                                 sym = get_symbol(state.stMacros, yytext);
                                 h = get_symbol_annotation(sym);
                                 if (h->line_number == state.src->line_number) {
                                   return LABEL;
                                 } else {
                                   gpwarning(GPW_MACRO_COLUMN_ONE, NULL);
                                   if (has_collon)
                                    gperror(GPE_BADCHAR,
                                             "Illegal character (:)");
                                   return IDENTIFIER;
                                 }
                               } else {
                                 /* if assembly is not enabled don't issue
                                 warnings about macro calls in column 1, they
                                 could be an alternate definition */
                                 return LABEL;
                               }
                               break;
                             case opcodes:
                               gpwarning(GPW_OP_COLUMN_ONE, NULL);
                               if (has_collon)
                                 gperror(GPE_BADCHAR, "Illegal character (:)");
                               return IDENTIFIER;
                               break;
                             case unknown_type:
                               return LABEL;
                             default:
                               return LABEL;
                           }
                         }
{IDENT}#v\(              {
                           char *symbol;

                           yytext[strlen(yytext) - 3] = '\0';

                           symbol = check_defines(yytext);
                           if (symbol) {
                             yylval.s = strdup(symbol);
                           } else {
                             yylval.s = strdup(yytext);
                           }
                           return VAR_BEGIN;
                         }
\)[a-z_0-9\x80-\xff?.]+  {
                           char *symbol;

                           symbol = check_defines(yytext+1);
                           if (symbol) {
                             yylval.s = strdup(symbol);
                           } else {
                             yylval.s = strdup(yytext+1);
                           }
                           return VAR_END;
                         }
{IDENT}                  {
                           char *symbol;

                           symbol = check_defines(yytext);
                           if (symbol) {
                             char buffer[BUFSIZ];

                             /* Make the substitution with a leading space,
                                so it won't be a label */
                             sprintf(buffer, " %s", symbol);
                             push_string(buffer);
                           } else {
                             yylval.s = strdup(yytext);
                             return IDENTIFIER;
                           }
                         }
0x{NUMCHAR}+             {
                           yylval.i = stringtolong(yytext + 2, 16);
                           return NUMBER;
                         }
{NUMCHAR}+b              {
                           if (force_ident) {
                             yylval.s = strdup(yytext);
                             return IDENTIFIER;
                           } else if (state.radix == 16) {
                             yylval.i = stringtolong(yytext, 16);
                             return NUMBER;
                           } else {
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
                             yylval.s = strdup(yytext);
                             return IDENTIFIER;
                           } else {
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
                             yylval.s = strdup(yytext);
                             return IDENTIFIER;
                           } else if (state.radix == 16) {
                             yylval.i = stringtolong(yytext, 16);
                             return NUMBER;
                           } else {
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
                             yylval.s = strdup(yytext);
                             return IDENTIFIER;
                           } else {
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
                             yylval.s = strdup(yytext);
                             return IDENTIFIER;
                           } else if (force_decimal) {
                             yylval.i = stringtolong(yytext, 10);
                             return NUMBER;
                           } else {
                             yylval.i = stringtolong(yytext, state.radix);
                             return NUMBER;
                           }
                         }
<INITIAL,bquote>\"{STR_QCHAR}*\"?        {
                           char *pc = &yytext[yyleng - 1];
                           if ((yyleng > 1) && (*pc == '"'))
                             *pc = '\0';
                           else
                             gpwarning(GPW_MISSING_QUOTE, NULL);
                           yylval.s = strdup(yytext + 1);
                           BEGIN(INITIAL);
                           return STRING;
                         }
'{STR_QCHAR}'            {
                           const char *pc = convert_escape_chars(yytext + 1, &yylval.i);
                           assert(pc == &yytext[yyleng - 1]);
                           return NUMBER;
                         }
A'{STR_QCHAR}'           {
                           yylval.i = yytext[2];
                           return NUMBER;
                         }
<bquote>\<{STR_BCHAR}*\> {
                           yytext[yyleng - 1] = '\0';
                           yylval.s = strdup(&yytext[1]);
                           BEGIN(INITIAL);
                           return STRING;
                         }
<bquote>[^ \t<>";\r\n]+ {
                           /* unquoted (special-case) string */
                           yylval.s = strdup(yytext);
                           BEGIN(INITIAL);
                           return STRING;
                         }
<lnquote>\"{STR_QCHAR}*\" {
                           /* if valid, must match with length >= unquoted token below */
                           yytext[yyleng - 1] = '\0';
                           yylval.s = strdup(&yytext[1]);
                           BEGIN(INITIAL);
                           return STRING;
                         }
<definition,lnquote>[^ \t;\r\n]+([ \t]+[^ \t;\r\n]+)* {
                           /* full-line (special-case) string */
                           /* must begin and end with non-whitespace */
                           yylval.s = strdup(yytext);
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

<INITIAL,bquote,lnquote,definition>[ \t\r]*
<*>[\n]                  {
                           quoted = 0;
                           force_decimal = 0;
                           force_ident = 0;
                           BEGIN(INITIAL);  /* no multiline states yet */
                           return yytext[0];
                         }
<*>;.*                   {  }
<*>.                     {
                           yylval.i = yytext[0];
                           return yytext[0];
                         }
%%

static void
search_pathes(struct source_context *new, const char *name)
{
  char tryname[PATH_MAX + 1];
  int i;

  for(i = 0; i < state.path_num; i++) {
    snprintf(tryname, sizeof(tryname),
             "%s" COPY_CHAR "%s", state.paths[i], name);
    new->f = fopen(tryname, "rt");
    if(new->f) {
      new->name = strdup(tryname);
      break;
    }
  }
}

void open_src(const char *name, int isinclude)
{
  extern FILE *yyin;
  struct source_context *new = malloc(sizeof(*new));
  if (state.src)
    state.src->yybuf = YY_CURRENT_BUFFER;

  new->f = fopen(name, "rt");
  if(new->f)
    new->name = strdup(name);
  else if(isinclude && (strchr(name, PATH_CHAR) == 0)) {
    /* If include file and no PATH_CHAR in name, try searching include
       path */
    search_pathes(new, name);

    if (new->f == NULL) {
      /* We didn't find a match so check for lower case.  This is mainly for
         Microchip examples and some includes in which filenames were written
         without regard to case */
      char *lower_case_name = gp_lower_case(name);
      search_pathes(new, lower_case_name);
      free(lower_case_name);

      if (new->f != NULL)
        gpwarning(GPW_UNKNOWN, "found lower case match for include filename");
    }
  }
  if(new->f)
    new->lst.f = fopen(new->name, "rt");

  yyin = new->f;
  if (new->f == NULL) {
    if (state.src) {
      char complaint[BUFSIZ];

      snprintf(complaint, sizeof(complaint),
               "Unable to open file \"%s\" - %s",
               name,
               strerror(errno));
      state.pass = 2; /* Ensure error actually gets displayed */
      gperror(GPE_UNKNOWN, complaint);
    } else {
      perror(name);
    }
    exit(1);
  }

  if (state.src) {
    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
  }

  if (state.use_absolute_path) {
    new->name = gp_absolute_path(new->name);
  }
  new->type = src_file;
  new->h = NULL;
  new->line_number = 1;
  new->loop_number = 1;
  if (state.debug_info) {
    new->file_symbol = NULL;
  } else {
    new->file_symbol = coff_add_filesym(new->name, isinclude);
  }
  new->prev = state.src;

  state.src = new;
  state.src->fc = add_file(ft_src, new->name);
  deps_add(new->name);

  if (!isinclude) {
    /* it is the top level file so initialize the lexer */
    quoted = 0;
    force_decimal = 0;
    force_ident = 0;
  }
  state.src->last_char_is_nl = true;
  state.found_end = 0;
}

void execute_macro(struct macro_head *h, int is_while)
{
  struct source_context *new = malloc(sizeof(*new));
  yy_size_t macro_src_size = 0;
  char *macro_src;

  assert(state.src != NULL);
  state.src->yybuf = YY_CURRENT_BUFFER;
  /* store the stack so it can be returned when the macro is complete */
  state.src->astack = state.astack;

  macro_src = make_macro_buffer(h);
  macro_src_size = strlen(macro_src) + 2;
  /* Initialize the flex end marker */
  memset(macro_src + macro_src_size - 2, 0, 2);

  /* create new source_context */
  new->name = strdup(h->src_name);
  if (is_while) {
    new->type = src_while;
  } else {
    new->type = src_macro;
  }
  new->line_number = h->line_number + 1;
  new->loop_number = 1;
  new->file_symbol = h->file_symbol;
  new->f = NULL;
  new->h = h;
  new->lst.m = h->body;

  new->prev = state.src;
  state.src = new;

  state.src->fc = add_file(ft_src, new->name); /* scan list for fc */
  yy_scan_buffer(macro_src, macro_src_size);
}

void repeat_while(void)
{
  struct macro_head *h;
  yy_size_t macro_src_size = 0;
  char *macro_src;

  h = state.src->h;

  /* rebuild the macro buffer, because it may have been modified */
  macro_src = make_macro_buffer(h);
  macro_src_size = strlen(macro_src) + 2;

  state.src->line_number = h->line_number + 1;
  state.src->loop_number++;
  state.src->lst.m = h->body;

  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_scan_buffer(macro_src, macro_src_size);
}

static void push_string(char *str)
{
  struct source_context *new = malloc(sizeof(*new));

  assert(state.src != NULL);
  state.src->yybuf = YY_CURRENT_BUFFER;

  new->name = strdup(state.src->name);
  new->type = src_substitution;
  new->line_number = state.src->line_number;
  new->loop_number = 1;
  new->file_symbol = state.src->file_symbol;
  new->f = NULL;
  new->h = NULL;
  new->lst.f = NULL;
  new->fc = add_file(ft_src, new->name); /* scan list for fc */

  yy_scan_string(str);

  new->prev = state.src;
  state.src = new;
}

void close_file(void)
{
  struct source_context *old;

  old = state.src;
  state.src = state.src->prev;

  if (old->type == src_file) {
    if (old->f != NULL) {
      fclose(old->f);
    }
    if (old->lst.f != NULL)
      fclose(old->lst.f);
    free(old->name);
    if (!state.debug_info) {
      coff_add_eofsym();
    }
  } else if (old->type == src_macro) {
    state.stTop = pop_symbol_table(state.stTop);
    state.stTopDefines = pop_symbol_table(state.stTopDefines);
    if (state.src->astack != state.astack) {
      gperror(GPE_ILLEGAL_NESTING, NULL);
    }
    assert(state.stTop != NULL);
    assert(state.stTopDefines != NULL);
    free(old->name);
  } else if (old->type == src_while) {
    free(old->name);
  }
  free(old);
}

void execute_exitm(void)
{
  struct amode *previous;
  struct amode *old;

  /* The macro is ended early, so return the stack to its previous state */
  previous = state.src->prev->astack;
  while ((state.astack != NULL) &&
         (state.astack != previous)) {
    old = state.astack;
    state.astack = state.astack->prev;
    free(old);
  }

  close_file();
  if (state.src) {
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(state.src->yybuf);
  }
}

/* found end directive, close all files and stop the parser */
int found_end(void)
{
  /* close all open files */
  while(state.src) {
    close_file();
  }

  /* make sure the buffer is empty when pass 2 starts */
  if (YY_CURRENT_BUFFER)
    yy_flush_buffer(YY_CURRENT_BUFFER);

  return 1;
}

static int found_eof(void)
{
  int terminate = 0;

  if (state.src->type == src_while) {
    if (maybe_evaluate(state.src->h->parms)) {
      if (state.src->loop_number > 255) {
        gperror(GPE_BAD_WHILE_LOOP, NULL);
      } else {
        /* repeat the while loop */
        repeat_while();
        return 0;
      }
    }
  }

  close_file();
  if (state.src) {
    /* Just an include file */
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(state.src->yybuf);
  } else {
    if (!state.found_end)
      gperror(GPE_ILLEGAL_COND, "Illegal condition (EOF encountered before END)");
    terminate = found_end();
  }

  return terminate;
}

enum identtype identify(char *text)
{
  enum identtype type;
  struct symbol *sym;

  if ((sym = get_symbol(state.stTopDefines, text)) != NULL) {
    type = defines;
  } else if ((sym = get_symbol(state.stDirective, text)) != NULL) {
    type = directives;
  } else if ((sym = get_symbol(state.stBuiltin, text)) != NULL) {
    type = opcodes;
  } else if ((sym = get_symbol(state.stGlobal, text)) != NULL) {
    type = globals;
  } else if ((sym = get_symbol(state.stMacros, text)) != NULL) {
    type = macros;
  } else {
    type = unknown_type;
  }

  return type;
}

static char *
check_defines(char *symbol)
{
  struct symbol *sym;
  char *subst = NULL;

  /* If not quoted, check for #define substitution */
  if (!quoted &&
     (sym = get_symbol(state.stTopDefines, symbol)) != NULL) {
    subst = get_symbol_annotation(sym);
    if (subst == NULL)
      subst = "";
    if (strcmp(symbol, subst) == 0) {
      /* check for a bad subsitution */
      subst = NULL;
    }
  }

  return subst;
}

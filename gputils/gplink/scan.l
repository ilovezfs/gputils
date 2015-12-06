%option noinput
%option nounput
%option noyywrap
%option always-interactive

%{
/* lexical analyser for gplink
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
#include "parse.h"
#include "scan.h"
#include "script.h"


/* YY_UNPUT not used, suppress the warning */
#define YY_NO_UNPUT

static void bad_char(const char *character);

static inline size_t
gp_input(char *buf, size_t max_size)
{
  if (YY_CURRENT_BUFFER_LVALUE->yy_is_interactive)
    {
      int c = '*';
      size_t n;

      for (n = 0; (n < max_size) && ((c = getc(yyin)) != EOF) && (c != '\n'); ++n)
        {
          buf[n] = (char)c;
        }

      if (c == '\n')
        {
          /* skip CR followed by LF */
          if ((n > 0) && (buf[n - 1] == '\r'))
            {
              --n;
            }

          buf[n++] = (char)c;
        }

      if ((c == EOF) && ferror(yyin))
        {
          script_error("Internal error", "Input in flex scanner failed");
        }

      return n;
    }
  else
    {
      script_error("Internal error", "Interactive scanner should be used");
      return 0;
    }
}

static inline size_t
gp_yyinput(char *buf, size_t max_size)
{
  size_t result = gp_input(buf, max_size);

  if (result)
    {
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

SYM \/?([a-z0-9_.\\:]+\/?)+

%%
<<EOF>>                 {
                          if (close_file())
                            return LEXEOF;
                        }
\"[^\"]*\"              {
                          yyleng -= 2;
                          yylval.s = GP_Strndup(yytext + 1, yyleng);
                          return STRING;
                        }
\'[^\']*\'              {
                          yyleng -= 2;
                          yylval.s = GP_Strndup(yytext + 1, yyleng);
                          return STRING;
                        }
libpath                 {
                          return LIBPATH;
                        }
lkrpath                 {
                          return LKRPATH;
                        }
#define/[ \t]           {
                          return DEFINE;
                        }
#ifdef/[ \t]            {
                          return IFDEF;
                        }
#else/[^a-z0-9]         {
                          return ELSE;
                        }
#fi/[^a-z0-9]           {
                          return FI;
                        }
error([ \t].*)?         {
                          yylval.s = GP_Strdup(yytext);
                          return ERROR;
                        }
[0-9]+                  {
                          char *endptr;

                          yylval.i = strtol(yytext, &endptr, 10);
                          if ((endptr == NULL) || (*endptr != '\0')) {
                            bad_char(endptr);
                          }
                          return NUMBER;
                        }
0x[0-9a-f]+             {
                          char *endptr;

                          yylval.l = strtol(yytext + 2, &endptr, 16);
                          if ((endptr == NULL) || (*endptr != '\0')) {
                            bad_char(endptr);
                          }
                          return NUMBER;
                        }
{SYM}                   {
                          yylval.s = GP_Strdup(yytext);
                          return SYMBOL;
                        }
[ \t\r]*
[\n]                    {
                          return yytext[0];
                        }
\/\/.*                  {  }
.                       {
                          yylval.i = yytext[0];
                          return yytext[0];
                        }
%%

static void bad_char(const char *character)
{
  char complaint[80];

  snprintf(complaint, sizeof(complaint),
           isprint(*character) ? "Illegal character '%c' in numeric constant" :
                                 "Illegal character %#x in numeric constant",
           *character);

  script_error(complaint, NULL);
}

void open_src(const char *name, gp_boolean isinclude)
{
  extern FILE *yyin;
  struct source_context *new = GP_Malloc(sizeof(*new));

  if (state.src != NULL) {
    state.src->yybuf = YY_CURRENT_BUFFER;
  }

  new->f = fopen(name, "rt");

  if (new->f != NULL) {
    new->name = GP_Strdup(name);
  }
  else if(isinclude && (strchr(name, PATH_CHAR) == 0)) {
    /* If include file and no "/" in name, try searching include path */
    char tryname[BUFSIZ];
    int i;

    for (i = 0; i < state.numpaths; i++) {
      snprintf(tryname, sizeof(tryname), "%s" COPY_CHAR "%s", state.paths[i], name);
      new->f = fopen(tryname, "rt");

      if (new->f != NULL) {
        new->name = GP_Strdup(tryname);
        break;
      }
    }
  }

  yyin = new->f;

  if (new->f == NULL) {
    if (state.src != NULL) {
      gp_error("Unable to open file \"%s\" - %s", name, strerror(errno));
    } else {
      perror(name);
    }
    exit(1);
  }

  if (state.src != NULL) {
    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
  }

  new->line_number = 1;
  new->prev = state.src;
  new->last_char_is_nl = true;
  state.src = new;
}

gp_boolean close_file(void)
{
  struct source_context *old;
  gp_boolean terminate = false;

  old = state.src;
  state.src = state.src->prev;

  if (old->f != NULL) {
    fclose(old->f);
    free(old->name);
  }
  free(old);

  if (state.src != NULL) {
    /* Just an include file */
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(state.src->yybuf);
  } else {
    terminate = true;
  }

  return terminate;
}

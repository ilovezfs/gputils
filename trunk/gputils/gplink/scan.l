%option noyywrap

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

static void bad_char(char *character);

%}

SYM \/?([a-z0-9_.\\:]+\/?)+

%%
<<EOF>>		 	 {
			   if (close_file())
			     return LEXEOF;
			 }
libpath			 {
			   return LIBPATH;
			 }
lkrpath			 {
			   return LKRPATH;
			 }
#define/[ \t]		 {
			   return DEFINE;
			 }
#ifdef/[ \t]		 {
			   return IFDEF;
			 }
#else/[^a-z0-9]		 {
			   return ELSE;
			 }
#fi/[^a-z0-9]		 {
			   return FI;
			 }
error([ \t].*)?		 {
                           yylval.s = strdup(yytext);
			   return ERROR;
			 }
[0-9]+                   {
                           char *endptr;

		           yylval.i = strtol(yytext, &endptr, 10);
                           if ((endptr == NULL) || (*endptr != '\0')) {
                             bad_char(endptr);
                           }
			   return NUMBER;
                         }
0x[0-9a-f]+	         {
                           char *endptr;

		           yylval.l = strtol(yytext + 2, &endptr, 16);
                           if ((endptr == NULL) || (*endptr != '\0')) {
                             bad_char(endptr);
                           }
			   return NUMBER;
			 }
{SYM}			 {
                           yylval.s = strdup(yytext);
                           return SYMBOL;
			 }
[ \t\r]*
[\n]		         {
 			   return yytext[0];
			 }
\/\/.*			 {  }
.			 { 
			   yylval.i = yytext[0];
		           return yytext[0];
			 }
%%

static void bad_char(char *character)
{
  char complaint[80];
			 
  snprintf(complaint, sizeof(complaint),
    isprint(*character) ?
      "Illegal character '%c' in numeric constant" :
      "Illegal character %#x in numeric constant",
    *character);

  script_error(&complaint[0], NULL);

  return;
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
    /* If include file and no "/" in name, try searching include path */
    char tryname[BUFSIZ];
    int i;

    for(i = 0; i < state.numpaths; i++) {
      snprintf(tryname, sizeof(tryname),
	       "%s" COPY_CHAR "%s", state.paths[i], name);
      new->f = fopen(tryname, "rt");
      if(new->f) {
        new->name = strdup(tryname);
        break;
      }
    }
  }

  yyin = new->f;

  if (new->f == NULL) {
    if (state.src) {
      gp_error("Unable to open file \"%s\" - %s",
               name,
               strerror(errno));    
    } else {
      perror(name);
    }
    exit(1);
  }

  if (state.src) {
    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
  }

  new->line_number = 1;
  new->prev = state.src;
  state.src = new;
}

int close_file()
{
  struct source_context *old;
  int terminate = 0;

  old = state.src;
  state.src = state.src->prev;
  if (old->f != NULL) {
    fclose(old->f);
    free(old->name);
  }
  free(old);

  if (state.src) {
    /* Just an include file */
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(state.src->yybuf);
  } else {
    terminate = 1;
  }

  return terminate;
}

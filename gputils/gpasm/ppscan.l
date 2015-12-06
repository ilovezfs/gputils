/* %option debug */
%option noinput
%option noyywrap
%x hv
%x def

%{
/* lexical analyser for gpasm preprocessor #v() evaluation
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
#include "gperror.h"
#include "preprocess.h"
#include "ppparse.h"

#define OPERATOR(x)  return (pplval.i = (x))

#define YY_USER_ACTION ppcol_end += yyleng;

int ppcol_begin;
int ppcol_end;
static int brack_depth;
static int found_hv;
%}

IDENT  [.]?[a-z_\x80-\xff?@#][a-z_0-9\x80-\xff.?@#]*
ESCCH  \\([abfnrtv\\?'"]|0[0-7]{2}|x[0-9a-f]{2})
STR_QCHAR  ([^"\r\n]|{ESCCH})
NUMCHAR [0-9a-z]

%%
<INITIAL>#?define        |
<INITIAL>#?ifdef         |
<INITIAL>#?ifndef        |
<INITIAL>#undefine       {
                           BEGIN(def);
                         }
<INITIAL>#v\(            {
                           BEGIN(hv);
                           brack_depth = 1;
                           ppcol_begin = ppcol_end - yyleng;
                           found_hv = 1;
                           return HV;
                         }
<hv>#v                   /* ignore inner #v-s */
<hv>upper                {
                           pplval.i = UPPER;
                           return UPPER;
                         }
<hv>high                 {
                           pplval.i = HIGH;
                           return HIGH;
                         }
<hv>low                  {
                           pplval.i = LOW;
                           return LOW;
                         }
<hv>{IDENT}#v\(          {
                           int i = yyleng - 3;
                           pplval.s = GP_Strndup(yytext, i);
                           unput('(');
                           ++brack_depth;
                           return IDENTIFIER;
                         }
<hv>{IDENT}              {
                           pplval.s = GP_Strdup(yytext);
                           return IDENTIFIER;
                         }
<hv>0x{NUMCHAR}+         {
                           pplval.i = stringtolong(yytext + 2, 16);
                           return NUMBER;
                         }
<hv>{NUMCHAR}+b          {
                           if (state.radix == 16) {
                             pplval.i = stringtolong(yytext, 16);
                             return NUMBER;
                           } else {
                             yytext[yyleng - 1] = '\0';
                             pplval.i = stringtolong(yytext, 2);
                             return NUMBER;
                           }
                         }
<hv>b'-?{NUMCHAR}+'      {
                           yytext[yyleng - 1] = '\0';
                           pplval.i = stringtolong(yytext + 2, 2);
                           return NUMBER;
                         }
<hv>{NUMCHAR}+[oq]       {
                           yytext[yyleng - 1] = '\0';
                           pplval.i = stringtolong(yytext, 8);
                           return NUMBER;
                         }
<hv>[oq]'-?{NUMCHAR}+'   {
                           yytext[yyleng - 1] = '\0';
                           pplval.i = stringtolong(yytext + 2, 8);
                           return NUMBER;
                         }
<hv>{NUMCHAR}+d          {
                           if (state.radix == 16) {
                             pplval.i = stringtolong(yytext, 16);
                             return NUMBER;
                           } else {
                             yytext[yyleng - 1] = '\0';
                             pplval.i = stringtolong(yytext, 10);
                             return NUMBER;
                           }
                         }
<hv>d'-?[0-9]+'          {
                           yytext[yyleng - 1] = '\0';
                           pplval.i = stringtolong(yytext + 2, 10);
                           return NUMBER;
                         }
<hv>"."[0-9]+            {
                           pplval.i = stringtolong(yytext + 1, 10);
                           return NUMBER;
                         }
<hv>{NUMCHAR}+h          {
                           yytext[yyleng - 1] = '\0';
                           pplval.i = stringtolong(yytext, 16);
                           return NUMBER;
                         }
<hv>h'-?{NUMCHAR}+'      {
                           yytext[yyleng - 1] = '\0';
                           pplval.i = stringtolong(yytext + 2, 16);
                           return NUMBER;
                         }
<hv>{NUMCHAR}+           {
                           pplval.i = stringtolong(yytext, state.radix);
                           return NUMBER;
                         }
<hv>\"{STR_QCHAR}*\"?|<hv>'{STR_QCHAR}*'? {
                           const char *pc;

                           if ((yyleng > 1) && (yytext[yyleng - 1] == '"'))
                             --yyleng;
                           else
                             gpvwarning(GPW_MISSING_QUOTE, NULL);

                           pc = convert_escape_chars(yytext + 1, &pplval.i);
                           if (pc != &yytext[yyleng])
                             gpverror(GPE_ILLEGAL_ARGU, NULL, yytext);

                           return NUMBER;
                         }
<hv>a'{STR_QCHAR}'       {
                           const char *pc = convert_escape_chars(yytext + 2, &pplval.i);
                           assert(pc == &yytext[yyleng - 1]);
                           return NUMBER;
                         }
<hv>"<<"                 OPERATOR(LSH);
<hv>">>"                 OPERATOR(RSH);
<hv>">="                 OPERATOR(GREATER_EQUAL);
<hv>"<="                 OPERATOR(LESS_EQUAL);
<hv>"=="                 OPERATOR(EQUAL);
<hv>"!="                 OPERATOR(NOT_EQUAL);
<hv>"&&"                 OPERATOR(LOGICAL_AND);
<hv>"||"                 OPERATOR(LOGICAL_OR);

<hv>\(                   {
                           ++brack_depth;
                           return yytext[0];
                         }
<hv>\)                   {
                           if (--brack_depth <= 0) {
                             BEGIN(INITIAL);
                             yyterminate();
                           }
                           return yytext[0];
                         }

\"{STR_QCHAR}*\"?

<*>[ \t\r]*

<hv>.                    {
                           pplval.i = yytext[0];
                           return yytext[0];
                         }
<INITIAL,def>.
;.*                      |
<*>\n                    {
                           BEGIN(INITIAL);
                         }
%%

#if 0
static void
yyprint (FILE *file, int type, YYSTYPE value)
{
  printf("@%d:", ppcol_end);

  switch (type) {
  case IDENTIFIER:
    fprintf (file, "'%s' ", value.s);
    break;

  case NUMBER:
  case UPPER:
  case HIGH:
  case LOW:
  case LSH:
  case RSH:
  case GREATER_EQUAL:
  case LESS_EQUAL:
  case EQUAL:
  case NOT_EQUAL:
  case '<':
  case '>':
  case '&':
  case '|':
  case '^':
  case LOGICAL_AND:
  case LOGICAL_OR:
  case '=':
    fprintf (file, "'%d' ", value.i);
    break;

  case HV:
      fprintf (file, "'%d' ", type);
      break;

  default:
    if (type < 256) {
      fprintf (file, "'%c' ", type);
    }
    else {
      fprintf (file, "'?%d?' ", type);
    }
    break;
  }
}

void
lex_line(char *line)
{
  int type;

  yy_scan_string(line);

  while (0 != (type = pplex())) {
    yyprint(stdout, type, pplval);
  }

  yy_delete_buffer(YY_CURRENT_BUFFER);
  putchar('\n');
}
#endif

int
ppparse_chunk(char *buf, int begin, int end)
{
  int res;

  yy_scan_bytes(&buf[begin], end - begin);

  ppcol_end = ppcol_begin = 0;
  found_hv = 0;
  BEGIN(INITIAL);
  brack_depth = 0;

  res = !ppparse() && found_hv;

  ppcol_begin += begin;
  ppcol_end += begin;

  yy_delete_buffer(YY_CURRENT_BUFFER);

  return res;
}

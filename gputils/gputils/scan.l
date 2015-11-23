
/* Scanner to user label list.

   Copyright (C) 2014-2015 Molnar Karoly

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

%option noinput
%option nounput
%option noyywrap
%option yylineno
%option always-interactive

%{
#include "stdhdr.h"
#include "libgputils.h"

#include "gpdasm.h"
#include "labelset.h"
#include "parse.h"
#include "scan.h"

extern void yyerror(const char *Text, ...);

static int type_state = 0;

/*------------------------------------------------------------------------------------------------*/

void
open_label_source(const char *Src) {
  if ((yyin = fopen(Src, "rt")) == NULL) {
    yyerror("Unable to open file \"%s\": %s", Src, strerror(errno));
    exit(1);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
close_label_source(void) {
  if (yyin != NULL) {
    fclose(yyin);
    yyin = NULL;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
bad_char(const char *Str) {
  if (Str == NULL) {
    yyerror("Illegal character '%c' in a numeric constant in the \"%s\".%d.",
            state.lset_root.file_name, yylineno);
  }
  else {
    yyerror(isprint(*Str) ?
            "Illegal character '%c' in a numeric constant in the \"%s\".%d." :
            "Illegal character 0x%02X in a numeric constant in the \"%s\".%d.",
            *Str, state.lset_root.file_name, yylineno);
  }

  exit(1);
}
%}

SPACE   [ \t\f]
NWLN    (\r?\n)
XNWLN   [^\r\n]
NUM     [0-9]+
HEX     0[xX][0-9A-Fa-f]+
SYM     [a-zA-Z_][a-zA-Z0-9_]*

%x      comment
%x      sect_name
%x      sect_end
%x      sect_eol
%x      type_name
%x      type_start
%x      type_body
%x      type_end
%x      type_eol

%%

<<EOF>> {
        close_label_source();
        return LEXEOF;
        }

{SPACE}*";" {
        BEGIN(comment);
        return COMMENT;
        }

<comment>.*$ {
        BEGIN(INITIAL);
        }

^{SPACE}*"["{SPACE}* {
        BEGIN(sect_name);
        return '[';
        }

<sect_name>{SYM} {
        BEGIN(sect_end);
        yylval.string = GP_Strdup(yytext);
        return SECTION;
        }

<sect_end>{SYM} {
        yyerror("Unnecessary text in the \"%s\".%d ==> \"%s\"",
                state.lset_root.file_name, yylineno, yytext);
        close_label_source();
        exit(1);
        }

<sect_end>{SPACE}*"]" {
        BEGIN(sect_eol);
        return ']';
        }

<sect_eol>{SPACE}*$ {
        BEGIN(INITIAL);
        return EOL;
        }

<sect_eol>{XNWLN}+ {
        yyerror("Unnecessary text in the \"%s\".%d ==> \"%s\"",
                state.lset_root.file_name, yylineno, yytext);
        close_label_source();
        exit(1);
        }

^{SPACE}*"<"{SPACE}* {
        BEGIN(type_name);
        return '<';
        }

<type_name>{SYM} {
        BEGIN(type_start);
        yylval.string = GP_Strdup(yytext);
        return TYPE;
        }

<type_start>{SPACE}*">" {
        BEGIN(type_body);
        return '>';
        }

<type_body>[^{]+ {
        yyerror("Unnecessary text in the \"%s\".%d ==> \"%s\"",
                state.lset_root.file_name, yylineno, yytext);
        close_label_source();
        exit(1);
        }

<type_body>{SPACE}*({NWLN}{SPACE}*)?"{"{SPACE}* {
        BEGIN(INITIAL);

        if (type_state != 0)
          {
          yyerror("Another start of type in the \"%s\".%d ==> \"%s\"",
                  state.lset_root.file_name, yylineno, yytext);
          close_label_source();
          exit(1);
          }
        else
          {
          ++type_state;
          return '{';
          }
        }

"}"     {
        if (type_state <= 0)
          {
          yyerror("End of type without start in the \"%s\".%d ==> \"%s\"",
                  state.lset_root.file_name, yylineno, yytext);
          close_label_source();
          exit(1);
          }
        else
          {
          BEGIN(type_eol);
          --type_state;
          return '}';
          }
        }

<type_eol>{SPACE}*$ {
        BEGIN(INITIAL);
        return EOL;
        }

<type_eol>{XNWLN}+ {
        yyerror("Unnecessary text in the \"%s\".%d ==> \"%s\"",
                state.lset_root.file_name, yylineno, yytext);
        close_label_source();
        exit(1);
        }

{NUM}   {
        char *endptr;

        yylval.value = strtol(yytext, &endptr, 10);

        if ((endptr == NULL) || (*endptr != '\0'))
          {
          bad_char(endptr);
          }

        return NUMBER;
        }

{HEX}   {
        char *endptr;

        yylval.value = strtol(yytext + 2, &endptr, 16);

        if ((endptr == NULL) || (*endptr != '\0'))
          {
          bad_char(endptr);
          }

        return NUMBER;
        }

{SYM}   {
        yylval.string = GP_Strdup(yytext);
        return SYMBOL;
        }

"*"     { return MUL; }
"/"     { return DIV; }
"%"     { return MOD; }
"+"     { return PLUS; }
"-"     { return MINUS; }
"<<"    { return LSH; }
">>"    { return RSH; }
"&"     { return AND; }
"^"     { return XOR; }
"|"     { return OR; }
"="     { return ASSIGN; }

[():#\.] { return yytext[0]; }

{SPACE}+ ;

{NWLN}+ ;

%%

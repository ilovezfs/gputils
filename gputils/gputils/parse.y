
/* Parser to user label list.

   Copyright (C) 2014 Molnar Karoly

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

%{

#include <stdarg.h>

#include "stdhdr.h"
#include "libgputils.h"

#include "gpdasm.h"
#include "labelset.h"
#include "scan.h"

extern FILE *yyin;
extern int yylineno;
extern int yylex(void);

/*------------------------------------------------------------------------------------------------*/

void yyerror(const char *Text, ...) {
  va_list ap;
  char buf[BUFSIZ];

  snprintf(buf, sizeof(buf), "Error: %s\n", Text);
  va_start(ap, Text);
  vfprintf(stderr, buf, ap);
  va_end(ap);
}
%}

%union {
  unsigned int code;
  long value;
  char *string;
};

%token <string> SECTION
%token <string> TYPE
%token <string> SYMBOL
%token <value> NUMBER
%token <code> COMMENT
%token <code> EOL
%token <code> MUL
%token <code> DIV
%token <code> MOD
%token <code> PLUS
%token <code> MINUS
%token <code> LSH
%token <code> RSH
%token <code> AND
%token <code> XOR
%token <code> OR
%token <code> ASSIGN
%token <string> LEXEOF 0 "end of file"

%right ASSIGN
%left OR
%left XOR
%left AND
%left LSH RSH
%left PLUS MINUS
%left MUL DIV MOD
%left UMINUS
%left '('

%type <value> expr

%%

program:
        |
        program commands
        ;

commands:
        COMMENT
        |
        '[' SECTION ']' EOL
        {
          lset_section_new(&state.lset_root, $2, yylineno);
        }
        |
        '<' TYPE '>' '{'
        {
          if ((state.lset_root.section_actual == NULL) ||
              (strcmp(state.lset_root.section_actual->name, "CODE") != 0)) {
            yyerror("The CDATA type can be only use in the CODE section! (line %i)", yylineno);
            exit(1);
          }

          if (strcmp($2, "CDATA") == 0) {
            state.lset_root.is_data = 1;
          }
          else {
            yyerror("The type (%s) can be CDATA!", $2);
            exit(1);
          }
        }
        |
        '}' EOL
        {
          state.lset_root.is_data = 0;
        }
        |
        SYMBOL ASSIGN expr ':' expr
        {
          /* abcdefg = START : END */
          unsigned int attr = (state.lset_root.is_data) ? (CSYM_START | CSYM_END | CSYM_DATA) :
                                                          (CSYM_START | CSYM_END);

          if (state.lset_root.section_actual != NULL) {
            lset_symbol_new(state.lset_root.section_actual, $1, $3, $5, attr, yylineno);
          }
          else {
            if (state.lset_root.section_global == NULL) {
              lset_section_make_global(&state.lset_root);
            }

            lset_symbol_new(state.lset_root.section_global, $1, $3, $5, attr, yylineno);
          }
        }
        |
        SYMBOL ASSIGN expr '#' expr
        {
          /* abcdefg = START # SIZE */
          long _end = $3 + $5 - 1;
          unsigned int attr = (state.lset_root.is_data) ? (CSYM_START | CSYM_END | CSYM_DATA) :
                                                          (CSYM_START | CSYM_END);

          if (state.lset_root.section_actual != NULL) {
            lset_symbol_new(state.lset_root.section_actual, $1, $3, _end, attr, yylineno);
          }
          else {
            if (state.lset_root.section_global == NULL) {
              lset_section_make_global(&state.lset_root);
            }

            lset_symbol_new(state.lset_root.section_global, $1, $3, _end, attr, yylineno);
          }
        }
        |
        SYMBOL ASSIGN expr
        {
          /* abcdefg = START */
          unsigned int attr = (state.lset_root.is_data) ? (CSYM_START | CSYM_DATA) : CSYM_START;

          if (state.lset_root.section_actual != NULL) {
            lset_symbol_new(state.lset_root.section_actual, $1, $3, -1, attr, yylineno);
          }
          else {
            if (state.lset_root.section_global == NULL) {
              lset_section_make_global(&state.lset_root);
            }

            lset_symbol_new(state.lset_root.section_global, $1, $3, -1, attr, yylineno);
          }
        }
        ;

expr:
        NUMBER                  { $$ = $1; }
        |
        SYMBOL '.' SYMBOL
        {
          const lset_symbol_t *sym;

          if ((sym = lset_symbol_find(state.lset_root.section_global, $1)) == NULL) {
            sym = lset_symbol_find(state.lset_root.section_actual, $1);
          }

          if (sym == NULL) {
            yyerror("The symbol - in line %i - not exist: \"%s\"", yylineno, $1);
            exit(1);
          }

          if (strcmp($3, "start") == 0) {
            if (sym->attr & CSYM_START) {
              $$ = sym->start;
            }
            else {
              yyerror("The symbol - in line %i \"%s\" - has no \"start\" element!", yylineno, $1);
              exit(1);
            }
          }
          else if (strcmp($3, "end") == 0) {
            if (sym->attr & CSYM_END) {
              $$ = sym->end;
            }
            else {
              yyerror("The symbol - in line %i \"%s\" - has no \"end\" element!", yylineno, $1);
              exit(1);
            }
          }
          else if (strcmp($3, "size") == 0) {
            $$ = (sym->attr & CSYM_END) ? (sym->end - sym->start + 1) : 1;
          }
          else {
            yyerror("The name of element - in line %i \"%s\" - only can be: start or end", yylineno, $3);
            exit(1);
          }
        }
        |
        SYMBOL
        {
          const lset_symbol_t *sym;

          if ((sym = lset_symbol_find(state.lset_root.section_global, $1)) == NULL) {
            sym = lset_symbol_find(state.lset_root.section_actual, $1);
          }

          if (sym == NULL) {
            yyerror("The symbol - in line %i - not exist: \"%s\"", yylineno, $1);
            exit(1);
          }

          if (sym->attr & CSYM_START) {
            $$ = sym->start;
          }
          else {
            yyerror("The symbol - in line %i \"%s\" - has no \"start\" element!", yylineno, $1);
            exit(1);
          }
        }
        |
        expr OR expr            { $$ = $1 | $3; }
        |
        expr XOR expr           { $$ = $1 ^ $3; }
        |
        expr AND expr           { $$ = $1 & $3; }
        |
        expr RSH expr           { $$ = $1 >> $3; }
        |
        expr LSH expr           { $$ = $1 << $3; }
        |
        expr MINUS expr         { $$ = $1 - $3; }
        |
        expr PLUS expr          { $$ = $1 + $3; }
        |
        expr MOD expr           { $$ = $1 % $3; }
        |
        expr DIV expr           { $$ = $1 / $3; }
        |
        expr MUL expr           { $$ = $1 * $3; }
        |
        MINUS expr %prec UMINUS { $$ = -$2; }
        |
        '(' expr ')'            { $$ = $2; }
        ;
%%

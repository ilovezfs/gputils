%option noyywrap
%x incl
%x define
%x definition
%x title
%x subtitle

%{
/* lexical analyser for gpasm
   Copyright (C) 1998,1999,2000,2001 James Bowman, Craig Franklin

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

#include "gpasm.h"
#include "parse.h"
#include "scan.h"
#include "gperror.h"
#include "gpsymbol.h"
#include "directive.h"

#define OPERATOR(x)  return (yylval.i = (x))
/* YY_UNPUT not used, suppress the warning */
#define YY_NO_UNPUT

enum identtype { defines, directives, globals, macros, opcodes, unknown };
enum identtype identify(char *);

static void push_string(char *str);
static int found_end();
static int found_eof();

static struct symbol *current_definition;	/* Used in #define */
static int quoted; /* Used to prevent #define expansion in ifdef and 
                      ifndef... */
static int force_decimal; /* Used to force decimal in errorlevel */

%}

IDENT  ([a-z_\x80-\xff?@][a-z_0-9\x80-\xff.?@]*)|([.][a-z_\x80-\xff.?@]*)
ESCCH  \\([abfnrtv\\?'"]|0[0-7]{2}|x[0-9a-f]{2})
STR_QCHAR  ([^"\n]|{ESCCH})

%%
^[ \t]*#?include[ \t]*[<"]? { BEGIN(incl); }
<incl>[^<"\r\n]*[>"]?	 { /* got the include file name */
			   char *pc = &yytext[yyleng - 1];
			   if ((*pc == '"') || (*pc == '>'))
			     *pc = '\0';
			   open_src(yytext, 1);
			   BEGIN(INITIAL);
			 }
<<EOF>>		 	 {
			   if (found_eof())
			     yyterminate();
			 }
end		         {
			   found_end();
			   yyterminate();
			 }
^[ \t]*title[ \t]*[<"]?  { BEGIN(title); }
<title>[^<"\r\n]*[>"]?   { /* got the title text */
#define LEN sizeof(state.lst.title_name)
                           yytext[yyleng - 1] = '\0';
                           strncpy(state.lst.title_name, yytext, LEN - 1);
                           state.lst.title_name[LEN - 1] = '\0';
                           BEGIN(INITIAL);
#undef LEN
                         }
^[ \t]*subtitle[ \t]*[<"]?  { BEGIN(subtitle); }
^[ \t]*subtitl[ \t]*[<"]?   { BEGIN(subtitle); }
^[ \t]*stitle[ \t]*[<"]?    { BEGIN(subtitle); }
<subtitle>[^<"\r\n]*[>"]?   { /* got the subtitle text */
#define LEN sizeof(state.lst.subtitle_name)
                           yytext[yyleng - 1] = '\0';
                           strncpy(state.lst.subtitle_name, yytext, LEN - 1);
                           state.lst.subtitle_name[LEN - 1] = '\0';
                           BEGIN(INITIAL);
#undef LEN
                         }
cblock			 {
			   return CBLOCK;
			 }
errorlevel		 {
			   /* Force decimal interpretation for errorlevel*/
			   force_decimal = 1;
			   yylval.s = strdup(yytext);
			   return IDENTIFIER;
			 }
endc			 {
			   return ENDC;
			 }
fill[ \t]*\(     	 {
			   /* fill with ( ) as first argument */
			   yylval.i = FILL;
			   return FILL;
			 }
^[ \t]*#define[ \t]+ 	 {
                           BEGIN(define);
			 }
<define>{IDENT}		 {
                           if(asm_enabled()) {
			     if ((get_symbol(state.stDefines, yytext) != NULL)
                                  && (state.pass == 1)) {
			       /* FIXME: find a more elegant way to do this */
			       state.pass = 2;
			       gperror(GPE_DUPLAB, NULL);
			       exit(1);
			     }
			     current_definition = add_symbol(state.stDefines, 
                                                             yytext);
                           }
			   BEGIN(definition);
			 }
<definition>.*$	 	 {
                           if(asm_enabled()) {
			     char *string_ptr = yytext;

			     /* Should have a #define in progress */
			     assert(current_definition != NULL);

			     /* if there is a comment remove it */
			     while(*string_ptr != '\0') {
                               if (*string_ptr == ';') {
                                 *string_ptr = '\0';
                                 break;
                               }                                 
                               string_ptr++;
			     }

			     /* remove leading spaces or tabs */
			     string_ptr = yytext;
			     while(*string_ptr == '\t' || *string_ptr == ' ') {
			       string_ptr++;
			     }

                             annotate_symbol(current_definition, 
                                             strdup(string_ptr));
			     current_definition = NULL;
                           }
			   BEGIN(INITIAL);
                         }
high			 {
			   yylval.i = HIGH;
			   return HIGH;
			 }
low		     	 {
			   yylval.i = LOW;
			   return LOW;
			 }
list		     	 {
			  /* Force decimal interpretation for list*/
			  force_decimal = 1;
			  yylval.s = strdup(yytext);
			  return IDENTIFIER;
			 }
#?else   	 	 {
			   /* #else and else can appear in column 1 */
			   yylval.s = strdup(yytext);
			   return IDENTIFIER;
			 }
#endif  	 	 {
			   /* only #endif can appear in column 1 */
			   yylval.s = strdup(yytext);
			   return IDENTIFIER;
			 }
#?ifdef			 {
			   /* #ifdef and ifdef can appear in column 1 */
			   quoted = 1;
			   yylval.s = strdup(yytext);
			   return IDENTIFIER;
			 }
#?ifndef		 {
			   /* #ifndef and ifdef can appear in column 1 */
			   quoted = 1;
			   yylval.s = strdup(yytext);
			   return IDENTIFIER;
			 }
#undefine 	 	 {
			   /* #undefine can appear in column 1 */
			   quoted = 1;
			   yylval.s = strdup(yytext);
			   return IDENTIFIER;
			 }
^{IDENT}#v\(             {
                           yytext[strlen(yytext) - 3] = '\0';
                           yylval.s = strdup(yytext);
                           return VARLAB_BEGIN;
                         }
^{IDENT}:?		 { 
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
                               sym = get_symbol(state.stDefines, yytext);
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
                               break;
                             case opcodes:
			       gpwarning(GPW_OP_COLUMN_ONE, NULL);
                               if (has_collon)
                                 gperror(GPE_BADCHAR, "Illegal character (:)");
			       return IDENTIFIER;
                               break;
                             case unknown:
			       return LABEL;
                             default:
			       return LABEL;
                           }			   
			 }
{IDENT}#v\(              {
                           yytext[strlen(yytext) - 3] = '\0';
                           yylval.s = strdup(yytext);
                           return VAR_BEGIN;
                         }
\)[a-z_0-9\x80-\xff?.]+  {
                           yylval.s = strdup(yytext+1);
                           return VAR_END;
                         }
{IDENT}			 {
			   struct symbol *sym;

			   /* If not quoted, check for #define substitution */
			   if (!quoted &&
                               (sym = get_symbol(state.stDefines, yytext)) != NULL) {
			     char *subst = get_symbol_annotation(sym);
			     assert(subst != NULL);
 			     if (strcmp(yytext, subst) == 0) {
 			       /* if check for bad subsitution */
 			       yylval.s = strdup(yytext);
 			       return IDENTIFIER;
 			     } else {
 			       char buffer[BUFSIZ];
                               
                               /* Make the substitution with a leading space,
                                  that way it won't be a label */
                               buffer[0] = ' ';
                               buffer[1] = '\0';
                               strcat(buffer, subst);
                               push_string(buffer);
 			     }
			   } else {
                             yylval.s = strdup(yytext);
			     return IDENTIFIER;
			   }
			 }
1[24678][CFHL0][CERV]?[0-9X][0-9X][0-9]?[ABC]? {
/* Ugh.  As a special case, treat processor names, such as 16C84
as identifiers rather than as hex numbers. */
                           yylval.s = strdup(yytext);
			   return IDENTIFIER;
			 }
[bdhoq]'-?[0-9a-f]+'      {
			   yylval.i = gpasm_number(yytext);
			   return NUMBER;
			 }
0x[0-9a-f]+            {
                           char *endptr;

                           yylval.i = strtol(yytext + 2, &endptr, 16);
                           if ((endptr == NULL) || (*endptr != '\0')) {
    			     char complaint[80];
			 
    			     sprintf(complaint,
				     isprint(*endptr) ?
				     "Illegal character '%c' in numeric constant" :
				     "Illegal character %#x in numeric constant",
				     *endptr);
                             gperror(GPE_UNKNOWN, complaint);
                           }
			   return NUMBER;
			 }
[0-9a-f]+[0-9acef]?      {
                           /* If the number ends in b or d it is 
                           interpreted elsewhere */

                           char *endptr;

                           if (force_decimal) {
                             yylval.i = strtol(yytext, &endptr, 10);
                           }else {
                             yylval.i = strtol(yytext, &endptr, state.radix);
                           }
                           if ((endptr == NULL) || (*endptr != '\0')) {
    			     char complaint[80];
			 
    			     sprintf(complaint,
				     isprint(*endptr) ?
				     "Illegal character '%c' in numeric constant" :
				     "Illegal character %#x in numeric constant",
				     *endptr);
                             gperror(GPE_UNKNOWN, complaint);
                           }
			   return NUMBER;
			 }
"."[0-9]+	         {
                           char *endptr;
                           yylval.i = strtol(yytext + 1, &endptr, 10);
			   if ((endptr == NULL) || (*endptr != '\0')) {
    			     char complaint[80];
			 
    			     sprintf(complaint,
				     isprint(*endptr) ?
				     "Illegal character '%c' in numeric constant" :
				     "Illegal character %#x in numeric constant",
				     *endptr);
                             gperror(GPE_UNKNOWN, complaint);
                           }
			   return NUMBER;
			 }
[0-9a-f]+[bdhoq]         {
                           char *endptr;
			   char *pc = &yytext[yyleng - 1];
                           int tempradix = 16;

                           switch(tolower(pc[0])) {
                             case 'b':
                               tempradix = 2;
                               break;
                             case 'd':
                               tempradix = 10;
                               break;
                             case 'h':
                               tempradix = 16;
                               break;
                             case 'o':
                               tempradix = 8;
                               break;
                             case 'q':
                               tempradix = 8;
                               break;
                             default:
                               assert(0); /* This should have been caught */
                           }

                           yytext[yyleng - 1] = '\0';
                           yylval.i = strtol(yytext, &endptr, tempradix);                           
                           if ((endptr == NULL) || (*endptr != '\0')) {
                             char complaint[80];

                             sprintf(complaint,
                                     isprint(*endptr) ?
                                     "Illegal character '%c' in numeric constant " :
                                     "Illegal character %#x in numeric constant" ,
                                     *endptr);
                             gperror(GPE_UNKNOWN, complaint);
                           }
                           return NUMBER;
                         }
\"{STR_QCHAR}\"          {
                           yylval.i = gpasm_magic(yytext + 1);
			   return NUMBER;
                         }
\"{STR_QCHAR}*\"?        {
			   char *pc = &yytext[yyleng - 1];
			   if (*pc == '"')
			     *pc = '\0';
                           else
			     gpwarning(GPW_MISSING_QUOTE, NULL);                           
                           yylval.s = strdup(yytext + 1);
			   return STRING;
                         }
'{STR_QCHAR}'            {
                           yylval.i = gpasm_magic(yytext + 1);
			   return NUMBER;
                         }
A'{STR_QCHAR}'           {
                           yylval.i = yytext[2];
			   return NUMBER;
                         }
"<<"			 OPERATOR(LSH);
">>"			 OPERATOR(RSH);
">="			 OPERATOR(GREATER_EQUAL);
"<="			 OPERATOR(LESS_EQUAL);
"=="			 OPERATOR(EQUAL);
"!="			 OPERATOR(NOT_EQUAL);
"&&"			 OPERATOR(LOGICAL_AND);
"||"			 OPERATOR(LOGICAL_OR);

"+="			 OPERATOR(ASSIGN_PLUS);
"-="			 OPERATOR(ASSIGN_MINUS);
"*="			 OPERATOR(ASSIGN_MULTIPLY);
"/="			 OPERATOR(ASSIGN_DIVIDE);
"%="			 OPERATOR(ASSIGN_MODULUS);
"<<="			 OPERATOR(ASSIGN_LSH);
">>="			 OPERATOR(ASSIGN_RSH);
"&="			 OPERATOR(ASSIGN_AND);
"|="			 OPERATOR(ASSIGN_OR);
"^="			 OPERATOR(ASSIGN_XOR);

"++"			 OPERATOR(INCREMENT);
"--"			 OPERATOR(DECREMENT);

"*+"			 OPERATOR(TBL_POST_INC);
"*-"			 OPERATOR(TBL_POST_DEC);
"+*"			 OPERATOR(TBL_PRE_INC);

[ \t\r]*
[\n]		         {
			   quoted = 0;
			   force_decimal = 0;
 			   return yytext[0];
			 }
;.*			 {  }
.			 { 
			   yylval.i = yytext[0];
		           return yytext[0];
			 }
%%

void open_src(char *name, int isinclude)
{
  extern FILE *yyin;
  struct source_context *new = malloc(sizeof(*new));
  if (state.src)
    state.src->yybuf = YY_CURRENT_BUFFER;

  #ifdef __MSDOS__
    #define PATH_CHAR '\\'
    #define COPY_CHAR "\\"
  #else
    #define PATH_CHAR '/'
    #define COPY_CHAR "/"
  #endif

  new->f = fopen(name, "r");
  if(new->f)
	  new->name = strdup(name);
  else if(isinclude && (strchr(name, PATH_CHAR) == 0)) { 
	  /* If include file and no PATH_CHAR in name, try searching include 
             path */
	  char tryname[BUFSIZ];
	  int i;

	  for(i = 0; i < n_include_paths; i++) {
		  strcpy(tryname, include_paths[i]);
		  strcat(tryname, COPY_CHAR);
		  strcat(tryname, name);
		  new->f = fopen(tryname, "r");
		  if(new->f) {
			  new->name = strdup(tryname);
			  break;
		  }
	  }
  }
  if(new->f)
    new->f2 = fopen(new->name, "r");

  #undef PATH_CHAR
  #undef COPY_CHAR

  yyin = new->f;
  if (new->f == NULL) {
    if (state.src) {
      char complaint[BUFSIZ];

      sprintf(complaint,
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

  new->line_number = 1;	/* Files start at line 1, for some reason */
  new->prev = state.src;
  state.src = new;
  state.src->fc = add_file(ft_src, new->name); 
}

static void push_string(char *str)
{
  struct source_context *new = malloc(sizeof(*new));

  assert(state.src != NULL);
  state.src->yybuf = YY_CURRENT_BUFFER;

  new->name = state.src->name;
  new->line_number = state.src->line_number;
  new->f = NULL;
  new->f2 = NULL;

  yy_scan_string(str);

  new->prev = state.src;
  state.src = new;
}

void close_file()
{
  struct source_context *old;

  old = state.src;
  state.src = state.src->prev;
  if (old->f != NULL) {
    #ifndef __MSDOS__
      fseek(old->f, -1, SEEK_END);
      if (fgetc(old->f) != '\n') {
        char complaint[BUFSIZ];

        sprintf(complaint,
                "Missing newline before EOF in file \"%s\"",
                old->name);
        gpwarning(GPW_UNKNOWN, complaint);
      }
    #endif
    fclose(old->f);
    fclose(old->f2);
    free(old->name);
  }
  free(old);
}

/* found end directive, close all files and stop the parser */
static int found_end()
{
  if (state.while_head) {
    state.mac_body = NULL;    
    state.mac_prev = NULL;
    state.while_head = NULL;
    gperror(GPE_EXPECTED, "Expected (ENDW)");
  }

  if (state.astack != NULL &&
    state.astack->enabled == 1) {
    struct amode *old;

    while (state.astack) {
      old = state.astack;
      state.astack = state.astack->prev; 
      free(old);
    }
    gpwarning(GPW_EXPECTED, "Expected (ENDIF)");
  }

  /* close all open files */
  while(state.src) {
    close_file();
  }

  /* make sure the buffer is empty when pass 2 starts */
  if (YY_CURRENT_BUFFER)
    yy_flush_buffer(YY_CURRENT_BUFFER);

  return 1;
}

static int found_eof()
{
  int terminate = 0;

  close_file();
  if (state.src) {
    /* Just an include file */
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(state.src->yybuf);
  } else {
    gperror(GPE_ILLEGAL_COND, 
            "Illegal condition (EOF encountered before END)");
    terminate = found_end();
  }

  return terminate;
}

enum identtype identify(char *text)
{
  enum identtype type;  
  struct symbol *sym;

  if ((sym = get_symbol(state.stDefines, text)) != NULL) {
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
    type = unknown;
  }

  return type;
}

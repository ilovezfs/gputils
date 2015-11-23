%{
/* Parser for gpasm
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
#include "evaluate.h"
#include "gperror.h"
#include "directive.h"
#include "lst.h"
#include "macro.h"
#include "coff.h"
#include "scan.h"

/* #define DEBUG */

#ifdef DEBUG
/* enable debugging */
#include "parse.h"

int yydebug = 1;

#define YYPRINT(file, type, value)   yyprint (file, type, value)

static void
yyprint (FILE *file, int type, YYSTYPE value)
{
  switch (type) {
  case LABEL:
  case IDENTIFIER:
  case IDENT_BRACKET:
  case DEBUG_LINE:
  case ERRORLEVEL:
  case FILL:
  case LIST:
  case PROCESSOR:
  case DEFINE:
    fprintf (file, "%s", value.s);
    break;

  case STRING:
    fprintf (file, "\"%s\"", value.s);
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
  case ASSIGN_PLUS:
  case ASSIGN_MINUS:
  case ASSIGN_MULTIPLY:
  case ASSIGN_DIVIDE:
  case ASSIGN_MODULUS:
  case ASSIGN_LSH:
  case ASSIGN_RSH:
  case ASSIGN_AND:
  case ASSIGN_OR:
  case ASSIGN_XOR:
  case INCREMENT:
  case DECREMENT:
  case POSTINCREMENT:
  case POSTDECREMENT:
  case INDFOFFSET:
  case TBL_NO_CHANGE:
  case TBL_POST_INC:
  case TBL_POST_DEC:
  case TBL_PRE_INC:
  case '[':
  case ']':
    fprintf (file, "%d", value.i);
    break;

  case CBLOCK:
  case ENDC:
  default:
    break;
  }
}
#endif

int yylex(void);

/************************************************************************/

/* Some simple functions for building parse trees */

static struct pnode *mk_pnode(enum pnode_tag tag)
{
  struct pnode *new = (struct pnode *)GP_Malloc(sizeof(struct pnode));

  new->tag = tag;
  return new;
}

struct pnode *mk_constant(int value)
{
  struct pnode *new = mk_pnode(PTAG_CONSTANT);

  new->value.constant = value;
  return new;
}

struct pnode *mk_offset(struct pnode *p)
{
  struct pnode *new = mk_pnode(PTAG_OFFSET);

  new->value.offset = p;
  return new;
}

struct pnode *mk_symbol(char *value)
{
  struct pnode *new = mk_pnode(PTAG_SYMBOL);

  new->value.symbol = value;
  return new;
}

struct pnode *mk_string(char *value)
{
  struct pnode *new = mk_pnode(PTAG_STRING);

  new->value.string = value;
  return new;
}

struct pnode *mk_list(struct pnode *head, struct pnode *tail)
{
  struct pnode *new = mk_pnode(PTAG_LIST);

  new->value.list.head = head;
  new->value.list.tail = tail;
  return new;
}

struct pnode *mk_2op(int op, struct pnode *p0, struct pnode *p1)
{
  struct pnode *new = mk_pnode(PTAG_BINOP);

  new->value.binop.op = op;
  new->value.binop.p0 = p0;
  new->value.binop.p1 = p1;
  return new;
}

struct pnode *mk_1op(int op, struct pnode *p0)
{
  struct pnode *new = mk_pnode(PTAG_UNOP);

  new->value.unop.op = op;
  new->value.unop.p0 = p0;
  return new;
}

/************************************************************************/
/* shared functions */

gpasmVal set_label(char *label, struct pnode *parms)
{
  gpasmVal value = 0;

  if (asm_enabled()) {
    if ((state.mode == MODE_RELOCATABLE) && !IN_MACRO_WHILE_DEFINITION &&
        !(SECTION_FLAGS & (STYP_TEXT | STYP_DATA | STYP_BPACK | STYP_BSS)))
      gpverror(GPE_LABEL_IN_SECTION, NULL);

    value = do_or_append_insn("set", parms);
    if (!IN_MACRO_WHILE_DEFINITION) {
      set_global(label, value, TEMPORARY, GVT_CONSTANT);
    }
  }

  return value;
}

int return_op(int operation);

void next_line(int value)
{
  if ((state.pass == 2) && (state.lst.line.linetype == LTY_DOLIST_DIR)) {
    state.lst.line.linetype = LTY_NONE;
    lst_format_line(state.src->curr_src_line.line, value);
  }

  if (IN_WHILE_EXPANSION || IN_MACRO_EXPANSION) {
    if (!IN_WHILE_DEFINITION && state.lst.expand &&
        (state.pass == 2) &&
        (state.lst.line.linetype != LTY_DOLIST_DIR) &&
        (state.lst.line.linetype != LTY_NOLIST_DIR)) {

      if (state.src->curr_src_line.line != NULL) {
        /* Empty macro. */
        lst_format_line(state.src->curr_src_line.line, value);
      }
      preproc_emit();
    }

    /* while loops can be defined inside a macro or nested */
    if (IN_MACRO_WHILE_DEFINITION) {
      state.lst.line.linetype = LTY_NONE;

      if (state.mac_body != NULL) {
        /* Empty macro. */
        state.mac_body->src_line = GP_Strdup(state.src->mac_body->src_line);
      }
    }

    if (state.src->mac_body != NULL) {
      state.src->mac_body = state.src->mac_body->next;
    }
  } else if (IN_FILE_EXPANSION) {
    if (!IN_WHILE_DEFINITION && (state.pass == 2) &&
        (state.lst.line.linetype != LTY_DOLIST_DIR) &&
        (state.lst.line.linetype != LTY_NOLIST_DIR)) {
      lst_format_line(state.src->curr_src_line.line, value);

      if (!IN_MACRO_WHILE_DEFINITION) {
        preproc_emit();
      }
    }

    if (IN_MACRO_WHILE_DEFINITION) {
      state.lst.line.linetype = LTY_NONE;

      if (state.mac_body != NULL) {
        state.mac_body->src_line = GP_Strdup(state.src->curr_src_line.line);
      }
    }

   if (state.next_state == STATE_INCLUDE) {
      /* includes have to be evaluated here and not in the following
       * switch statetems so that the errors are reported correctly */
      state.src->line_number++;
      open_src(state.next_buffer.file, true);
      free(state.next_buffer.file);
    }
  }

  switch (state.next_state) {
    case STATE_EXITMACRO:
      state.src->line_number++;
      execute_exitm();
      break;

    case STATE_MACRO:
      state.src->line_number++;
      /* push the label for local directive */
      state.stTop = push_macro_symbol_table(state.stTop);
      execute_macro(state.next_buffer.macro, false);
      break;

    case STATE_SECTION:
      state.src->line_number++;
      /* create a new coff section */
      coff_new_section(state.obj.new_sec_name,
                       state.obj.new_sec_addr,
                       state.obj.new_sec_flags);
      break;

    case STATE_WHILE:
      state.src->line_number++;
      execute_macro(state.next_buffer.macro, true);
      break;

    case STATE_INCLUDE:
      /* already evaluated */
      break;

    default:
      state.src->line_number++;
      break;
  }
}

void yyerror(const char *message)
{
  if (!IN_MACRO_WHILE_DEFINITION) {
    /* throw error if not in macro definition */
    gpverror(GPE_PARSER, NULL, message);
  }
}

/************************************************************************/

%}

/* Bison declarations */

%union {
  gpasmVal i;
  char *s;
  struct pnode *p;
}

%token <s> LABEL
%token <s> IDENTIFIER
%token <s> IDENT_BRACKET
%token <s> CBLOCK
%token <s> DEBUG_LINE
%token <s> ENDC
%token <s> ERRORLEVEL
%token <s> FILL
%token <s> LIST
%token <i> NUMBER
%token <s> PROCESSOR
%token <s> STRING
%token <s> DEFINE
%token <i> UPPER
%token <i> HIGH
%token <i> LOW
%token <i> LSH
%token <i> RSH
%token <i> GREATER_EQUAL
%token <i> LESS_EQUAL
%token <i> EQUAL
%token <i> NOT_EQUAL
%token <i> '<'
%token <i> '>'
%token <i> '&'
%token <i> '|'
%token <i> '^'
%token <i> LOGICAL_AND
%token <i> LOGICAL_OR
%token <i> '='
%token <i> ASSIGN_PLUS
%token <i> ASSIGN_MINUS
%token <i> ASSIGN_MULTIPLY
%token <i> ASSIGN_DIVIDE
%token <i> ASSIGN_MODULUS
%token <i> ASSIGN_LSH
%token <i> ASSIGN_RSH
%token <i> ASSIGN_AND
%token <i> ASSIGN_OR
%token <i> ASSIGN_XOR
%token <i> INCREMENT
%token <i> DECREMENT
%token <i> POSTINCREMENT
%token <i> POSTDECREMENT
%token <i> INDFOFFSET
%token <i> TBL_NO_CHANGE
%token <i> TBL_POST_INC
%token <i> TBL_POST_DEC
%token <i> TBL_PRE_INC
%token <i> '['
%token <i> ']'

%type <i> '+'
%type <i> '-'
%type <i> '*'
%type <i> '/'
%type <i> '%'
%type <i> '!'
%type <i> '~'
%type <s> line
%type <s> decimal_ops
%type <i> statement
%type <p> parameter_list
%type <p> expr
%type <p> e0
%type <p> e1
%type <p> e2
%type <p> e3
%type <p> e4
%type <p> e5
%type <p> e6
%type <p> e7
%type <p> e8
%type <p> cidentifier
%type <i> e1op
%type <i> e2op
%type <i> e3op
%type <i> e4op
%type <i> e5op
%type <i> e6op
%type <i> e7op
%type <i> e8op
%type <i> e9op
%type <i> assign_equal_ops
%type <p> list_block
%type <p> list_expr
%type <p> list_args

%%
/* Grammar rules */

program:
        /* can be nothing */
        |
        program
        {
          state.lst.line.was_byte_addr = state.byte_addr;
          state.lst.line.linetype = LTY_NONE;
          state.next_state = STATE_NOCHANGE;
        } line
        | program error '\n'
        {
          state.lst.line.was_byte_addr = state.byte_addr;
          state.lst.line.linetype = LTY_NONE;
          state.next_state = STATE_NOCHANGE;

          yyerrok;  /* generate multiple errors */
          if (IN_MACRO_WHILE_DEFINITION) {
            /* in macro definition: append the macro */
            macro_append();
          }
          next_line(0);
        }
        ;

line:
        LABEL assign_equal_ops expr '\n'
        {
          struct pnode *parms;
          int exp_result;

          exp_result = do_insn("set", mk_list($3, NULL));
          parms = mk_list(mk_2op(return_op($2),
                                 mk_symbol($1),
                                 mk_constant(exp_result)), NULL);
          next_line(set_label($1, parms));
        }
        |
        LABEL '=' expr '\n'
        {
          struct pnode *parms;

          /* implements i = 6 + 1 */
          parms = mk_list($3, NULL);
          next_line(set_label($1, parms));
        }
        |
        LABEL DECREMENT '\n'
        {
          struct pnode *parms;

          /* implements i-- */
          parms = mk_list(mk_1op(DECREMENT, mk_symbol($1)), NULL);
          next_line(set_label($1, parms));
        }
        |
        LABEL INCREMENT '\n'
        {
          struct pnode *parms;

          /* implements i++ */
          parms = mk_list(mk_1op(INCREMENT, mk_symbol($1)), NULL);
          next_line(set_label($1, parms));
        }
        |
        LABEL statement
        {
          if (asm_enabled() && (state.lst.line.linetype == LTY_NONE)) {
            if (IS_RAM_ORG) {
              /* alias to next definition */
              state.lst.line.linetype = LTY_RES;
            }
            else {
              state.lst.line.linetype = LTY_INSN;
            }
          }

          if (asm_enabled()) {
            if (state.mac_head != NULL) {
              /* This is a macro definition. Set it up */
              struct symbol *mac;
              struct macro_head *h = NULL;

              mac = get_symbol(state.stMacros, $1);
              if (mac != NULL) {
                h = get_symbol_annotation(mac);
              }

              /* It's not an error if macro was defined on pass 1 and we're in pass 2. */
              if ((h != NULL) && !((h->pass == 1) && (state.pass == 2))) {
                gpverror(GPE_DUPLICATE_MACRO, NULL);
              } else {
                if (mac == NULL) {
                  mac = add_symbol(state.stMacros, $1);
                }
                annotate_symbol(mac, state.mac_head);
                h = state.mac_head;
                h->line_number = state.src->line_number;
                h->file_symbol = state.src->file_symbol;
              }
              h->pass = state.pass;

              /* The macro is defined so allow calls. */
              if (state.pass == 2) {
                h->defined = true;
              }

              state.mac_head = NULL;
            } else if (!IN_MACRO_WHILE_DEFINITION) {
              /* Outside a macro definition, just define the label. */
              switch (state.lst.line.linetype) {
              case LTY_SEC:
                gp_strncpy(state.obj.new_sec_name, $1, sizeof(state.obj.new_sec_name));
                break;

              case LTY_SET:
                set_global($1, $2, TEMPORARY, GVT_CONSTANT);
                break;

              case LTY_ORG:
              case LTY_EQU:
                set_global($1, $2, PERMANENT, GVT_CONSTANT);
                break;

              case LTY_INSN:
              case LTY_DATA:
              case LTY_RES:
                if ((state.mode == MODE_RELOCATABLE) && !IN_MACRO_WHILE_DEFINITION &&
                    !(SECTION_FLAGS & (STYP_TEXT | STYP_DATA | STYP_BPACK | STYP_BSS))) {
                  gpverror(GPE_LABEL_IN_SECTION, NULL);
                }

                if (IS_RAM_ORG) {
                  set_global($1, $2, PERMANENT, GVT_STATIC);
                }
                else {
                  set_global($1, $2, PERMANENT, GVT_ADDRESS);
                }
                break;

              case LTY_DIR:
                gpverror(GPE_ILLEGAL_LABEL, NULL, $1);
                break;

              default:
                break;
              }
            }
          }
          next_line($2);
        }
        |
        statement
        {
          if (state.mac_head != NULL) {
            /* This is a macro definition, but the label was missing. */
            state.mac_head = NULL;
            gpverror(GPE_NO_MACRO_NAME, NULL);
          } else {
            if (state.found_end) {
              switch (state.src->type) {
              case SRC_WHILE:
                gperror(GPE_EXPECTED, "Expected (ENDW)");
                break;

              case SRC_MACRO:
                gperror(GPW_EXPECTED,"Expected (ENDM)");
                /* fall through */

              default:
                break;
              }

              if (state.astack != NULL) {
                struct amode *old;

                while (state.astack != NULL) {
                  old = state.astack;
                  state.astack = state.astack->prev;
                  free(old);
                }
                gpwarning(GPW_EXPECTED, "Expected (ENDIF)");
              }
            }

            next_line($1);

            if (state.found_end) {
              found_end();
              YYACCEPT;
            }
          }
        }
        ;

decimal_ops: ERRORLEVEL | DEBUG_LINE;

statement:
        '\n'
        {
          if (!IN_MACRO_WHILE_DEFINITION) {
            $$ = (IS_RAM_ORG) ? state.byte_addr :
                                /* We want to have r as the value to assign to label. */
                                gp_processor_byte_to_real(state.processor, state.byte_addr);
          } else {
            macro_append();
          }
        }
        |
        PROCESSOR { force_ident = true; }
        IDENTIFIER '\n'
        {
          $$ = do_or_append_insn($1, mk_list(mk_symbol($3), NULL));
          force_ident = false;
        }
        |
        LIST '\n'
        {
          $$ = do_or_append_insn($1, NULL);
        }
        |
        LIST { force_decimal = true; }
        list_block '\n'
        {
          $$ = do_or_append_insn($1, $3);
          force_decimal = false;
        }
        |
        decimal_ops { force_decimal = true; }
        parameter_list '\n'
        {
          $$ = do_or_append_insn($1, $3);
          force_decimal = false;
        }
        |
        DEFINE IDENTIFIER STRING '\n'
        {
          $$ = do_or_append_insn($1, mk_list(mk_string($2),
                                 mk_list(mk_string($3), NULL)));
        }
        |
        DEFINE IDENT_BRACKET list_args ')' STRING '\n'
        {
          $$ = do_or_append_insn($1, mk_list(mk_string($2),
                                 mk_list(mk_string($5), $3)));
        }
        |
        DEFINE IDENT_BRACKET ')' STRING '\n'
        {
          $$ = do_or_append_insn($1, mk_list(mk_string($2),
                                 mk_list(mk_string($4), NULL)));
        }
        |
        DEFINE IDENTIFIER '\n'
        {
          $$ = do_or_append_insn($1, mk_list(mk_string($2), NULL));
        }
        |
        DEFINE '\n'
        {
          $$ = do_or_append_insn($1, NULL);
        }
        |
        IDENTIFIER '\n'
        {
          $$ = do_or_append_insn($1, NULL);
        }
        |
        IDENTIFIER parameter_list '\n'
        {
          $$ = do_or_append_insn($1, $2);
        }
        |
        FILL IDENTIFIER ')' ',' expr '\n'
        {
          int number;
          int i;

          if (!IN_MACRO_WHILE_DEFINITION) {
            number = eval_fill_number($5);

            for (i = 0; i < number; i++) {
              $$ = do_insn($2, NULL);
            }
          } else {
            macro_append();
          }
        }
        |
        FILL IDENTIFIER parameter_list ')' ',' expr '\n'
        {
          int number;
          int i;

          if (!IN_MACRO_WHILE_DEFINITION) {
            number = eval_fill_number($6);

            for (i = 0; i < number; i++) {
              $$ = do_insn($2, $3);
            }
          } else {
            macro_append();
          }
        }
        |
        CBLOCK expr '\n'
        {
          if (!IN_MACRO_WHILE_DEFINITION) {
            begin_cblock($2);
          } else {
            macro_append();
          }
          next_line(0);
        }
        const_block
        ENDC '\n'
        {
          state.lst.line.linetype = LTY_NONE;
          if (IN_MACRO_WHILE_DEFINITION) {
            macro_append();
          }
          $$ = 0;
        }
        |
        CBLOCK '\n'
        {
          if (!IN_MACRO_WHILE_DEFINITION) {
            continue_cblock();
          } else {
            macro_append();
          }
          next_line(0);
        }
        const_block
        ENDC '\n'
        {
          state.lst.line.linetype = LTY_NONE;
          if (IN_MACRO_WHILE_DEFINITION) {
            macro_append();
          }
          $$ = 0;
        }
        |
        CBLOCK error ENDC '\n'
        {
          state.lst.line.linetype = LTY_NONE;
          $$ = 0;
        }
        ;

const_block:
        |
        const_block const_line
        {
          state.lst.line.linetype = LTY_SET;
          next_line(state.lst.cblock_lst);
        }
        ;

const_line:
        '\n'
        |
        const_def_list '\n'
        {
          if (IN_MACRO_WHILE_DEFINITION) {
            macro_append();
          }
        }
        |
        LABEL '\n'
        {
          if (!IN_MACRO_WHILE_DEFINITION) {
            state.lst.cblock_lst = state.cblock;
            cblock_expr(mk_symbol($1));
          } else {
            macro_append();
          }
        }
        |
        LABEL expr '\n'
        {
          if (!IN_MACRO_WHILE_DEFINITION) {
            state.lst.cblock_lst = state.cblock;
            cblock_expr_incr(mk_symbol($1), $2);
          } else {
            macro_append();
          }
        }
        ;

const_def_list:
        {
          state.lst.cblock_lst = state.cblock;
        }
        const_def
        |
        const_def_list ',' const_def
        ;

const_def:
        cidentifier
        {
          if (!IN_MACRO_WHILE_DEFINITION) {
            cblock_expr($1);
          }
        }
        |
        cidentifier ':' expr
        {
          if (!IN_MACRO_WHILE_DEFINITION) {
            cblock_expr_incr($1, $3);
          }
        }
        ;

assign_equal_ops: ASSIGN_PLUS | ASSIGN_MINUS | ASSIGN_MULTIPLY |
                  ASSIGN_DIVIDE | ASSIGN_MODULUS | ASSIGN_LSH | ASSIGN_RSH |
                  ASSIGN_AND | ASSIGN_OR | ASSIGN_XOR;

parameter_list:
        expr '[' expr ']'
        {
          $$ = mk_list(mk_constant(INDFOFFSET), mk_list($3, $1));
        }
        |
        expr
        {
          $$ = mk_list($1, NULL);
        }
        |
        expr ','
        {
          $$ = mk_list($1, mk_list(mk_symbol(""), NULL));
        }
        |
        expr ',' parameter_list
        {
          $$ = mk_list($1, $3);
        }
        |
        ',' parameter_list
        {
          $$ = mk_list(mk_symbol(""), $2);
        }
        |
        ','
        {
          $$ = mk_list(mk_symbol(""), mk_list(mk_symbol(""), NULL));
        }
        |
        INCREMENT parameter_list
        {
          $$ = mk_list(mk_constant($1), $2);
        }
        |
        DECREMENT parameter_list
        {
          $$ = mk_list(mk_constant($1), $2);
        }
        |
        parameter_list INCREMENT
        {
          $$ = mk_list(mk_constant(POSTINCREMENT), $1);
        }
        |
        parameter_list DECREMENT
        {
          $$ = mk_list(mk_constant(POSTDECREMENT), $1);
        }
        ;

expr:
        e8
        |
        expr e9op e8
        {
          coerce_str1($1);
          coerce_str1($3);
          $$ = mk_2op($2, $1, $3);
        }
        ;

e9op:   '=' ;

e8:
        e7
        |
        e8 e8op e7
        {
          coerce_str1($1);
          coerce_str1($3);
          $$ = mk_2op($2, $1, $3);
        }
        ;

e8op:   LOGICAL_OR ;

e7:
        e6
        |
        e7 e7op e6
        {
          coerce_str1($1);
          coerce_str1($3);
          $$ = mk_2op($2, $1, $3);
        }
        ;

e7op:   LOGICAL_AND;

e6:
        e5
        |
        e6 e6op e5
        {
          coerce_str1($1);
          coerce_str1($3);
          $$ = mk_2op($2, $1, $3);
        }
        ;

e6op:   '&' | '|' | '^' ;

e5:
        e4
        |
        e5 e5op e4
        {
          coerce_str1($1);
          coerce_str1($3);
          $$ = mk_2op($2, $1, $3);
        }
        ;

e5op:   '<' | '>' | EQUAL | NOT_EQUAL | GREATER_EQUAL | LESS_EQUAL ;

e4:
        e3
        |
        e4 e4op e3
        {
          coerce_str1($1);
          coerce_str1($3);
          $$ = mk_2op($2, $1, $3);
        }
        ;

e4op:   LSH | RSH ;

e3:
        e2
        |
        e3 e3op e2
        {
          coerce_str1($1);
          coerce_str1($3);
          $$ = mk_2op($2, $1, $3);
        }
        ;

e3op:   '+' | '-' ;

e2:
        e1
        |
        e2 e2op e1
        {
          coerce_str1($1);
          coerce_str1($3);
          $$ = mk_2op($2, $1, $3);
        }
        ;

e2op:   '*' | '/' | '%';

e1:
        e0
        |
        e1op e0
        {
          coerce_str1($2);
          $$ = mk_1op($1, $2);
        }
        ;

e1op:   UPPER | HIGH | LOW | '-' | '!' | '~' | '+';

e0:
        cidentifier
        {
          $$ = $1;
        }
        |
        NUMBER
        {
          $$ = mk_constant($1);
        }
        |
        '$'
        {
          $$ = mk_symbol("$");
        }
        |
        STRING
        {
          $$ = mk_string($1);
        }
        |
        '(' expr ')'
        {
          $$ = $2;
        }
        |
        '[' expr ']'
        {
          $$ = mk_offset($2);
        }
        |
        '*'
        {
          $$ = mk_constant(TBL_NO_CHANGE);
        }
        |
        TBL_POST_INC
        {
          $$ = mk_constant($1);
        }
        |
        TBL_POST_DEC
        {
          $$ = mk_constant($1);
        }
        |
        TBL_PRE_INC
        {
          $$ = mk_constant($1);
        }
        |
        INCREMENT
        {
          $$ = mk_constant($1);
        }
        |
        DECREMENT
        {
          $$ = mk_constant($1);
        }
        ;

cidentifier:
        IDENTIFIER
        {
          $$ = mk_symbol($1);
        }
        ;

list_block:
        list_expr
        {
          $$ = mk_list($1, NULL);
        }
        |
        list_expr ',' list_block
        {
          $$ = mk_list($1, $3);
        }
        ;

list_expr:
        IDENTIFIER
        {
          if ((strcasecmp($1, "p") == 0) || (strcasecmp($1, "pe") == 0)) {
            force_ident = true;
          }
        }
        e9op e8
        {
          $$ = mk_2op($3, mk_symbol($1), $4);
          force_ident = false;
        }
        |
        e8
        {
          $$ = $1;
        }
        ;

list_args:
        IDENTIFIER
        {
          $$ = mk_list(mk_symbol($1), NULL);
        }
        |
        IDENTIFIER ',' list_args
        {
          $$ = mk_list(mk_symbol($1), $3);
        }
        ;
%%

int return_op(int operation)
{
  /* returns an operator for the replacement of i+=1 with i=i+1*/
  switch(operation) {
  case ASSIGN_PLUS:     return '+';
  case ASSIGN_MINUS:    return '-';
  case ASSIGN_MULTIPLY: return '*';
  case ASSIGN_DIVIDE:   return '/';
  case ASSIGN_MODULUS:  return '%';
  case ASSIGN_LSH:      return LSH;
  case ASSIGN_RSH:      return RSH;
  case ASSIGN_AND:      return '&';
  case ASSIGN_OR:       return '|';
  case ASSIGN_XOR:      return '^';
  default:
    assert(0); /* Unhandled operator */
  }

  return 0;
}

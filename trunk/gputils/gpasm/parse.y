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

void yyerror(char *message)
{
  gperror(103, message);
}

int yylex(void);

/************************************************************************/

/* Some simple functions for building parse trees */

static struct pnode *mk_pnode(enum pnode_tag tag)
{
  struct pnode *new = malloc(sizeof(*new));
  new->tag = tag;
  return new;
}

struct pnode *mk_constant(int value)
{
  struct pnode *new = mk_pnode(constant);
  new->value.constant = value;
  return new;
}

struct pnode *mk_offset(struct pnode *p)
{
  struct pnode *new = mk_pnode(offset);
  new->value.offset = p;
  return new;
}

static struct pnode *mk_symbol(char *value)
{
  struct pnode *new = mk_pnode(symbol);
  new->value.symbol = value;
  return new;
}

static struct pnode *mk_string(char *value)
{
  struct pnode *new = mk_pnode(string);
  new->value.string = value;
  return new;
}

struct pnode *mk_list(struct pnode *head, struct pnode *tail)
{
  struct pnode *new = mk_pnode(list);
  new->value.list.head = head;
  new->value.list.tail = tail;
  return new;
}

static struct pnode *mk_2op(int op, struct pnode *p0, struct pnode *p1)
{
  struct pnode *new = mk_pnode(binop);
  new->value.binop.op = op;
  new->value.binop.p0 = p0;
  new->value.binop.p1 = p1;
  return new;
}

static struct pnode *mk_1op(int op, struct pnode *p0)
{
  struct pnode *new = mk_pnode(unop);  new->value.unop.op = op;
  new->value.unop.p0 = p0;
  return new;
}

/************************************************************************/
/* shared functions */

gpasmVal set_label(char *label, struct pnode *parms)
{
  gpasmVal value = 0;

  if (asm_enabled()) {
    value = do_or_append_insn("set", parms);
    if (!state.mac_prev) {
      set_global(label, value, TEMPORARY, gvt_constant);
    }
  }

  return value;
}

int return_op(int operation);

void next_line(int value)
{
  char l[BUFSIZ];
  char *e = l;

  if ((state.src->type == src_macro) ||
      (state.src->type == src_while)) {
    /* while loops can be defined inside a macro or nested */
    if (state.mac_prev) {
      state.lst.line.linetype = none;
      if (state.mac_body)
        state.mac_body->src_line = strdup(state.src->lst.m->src_line);
    }

    if (((state.src->type == src_while) || (state.lst.expand)) &&
        (state.pass == 2)) {
      assert(state.src->lst.m->src_line != NULL);
      lst_format_line(state.src->lst.m->src_line, value);
    }

    if (state.src->lst.m->next) {
      state.src->lst.m = state.src->lst.m->next;
    }
  } else if ((state.src->type == src_file) &&
             (state.src->lst.f != NULL)) {
    fgets(l, BUFSIZ, state.src->lst.f);
    l[strlen(l) - 1] = '\0';    /* Eat the trailing newline */

    if (state.mac_prev) {
      state.lst.line.linetype = none;
      if (state.mac_body)
        state.mac_body->src_line = strdup(l);
    }

    if (state.pass == 2) {
      lst_format_line(e, value);
    }
  }

  state.src->line_number++;

  switch (state.next_state) {
    case state_exitmacro:
      execute_exitm();
      break;

    case state_include:
      open_src(state.next_buffer.file, 1);
      free(state.next_buffer.file);
      break;

    case state_macro:
      /* push the label for local directive */
      state.stTop = push_macro_symbol_table(state.stTop);
      execute_macro(state.next_buffer.macro, 0);
      break;

    case state_section:
      /* create a new coff section */
      coff_new_section(state.obj.new_sec_name,
                       state.obj.new_sec_addr,
                       state.obj.new_sec_flags);
      break;

    case state_while:
      execute_macro(state.next_buffer.macro, 1);
      break;

    default:
      break;
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
%token <i> CONCAT
%token <i> VAR
%token <s> VARLAB_BEGIN
%token <s> VAR_BEGIN
%token <s> VAR_END
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
%type <s> label_concat
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

%%
/* Grammar rules */

program:
        /* can be nothing */
        |
        program
        {
          state.lst.line.was_org = state.org;
          state.lst.line.linetype = none;
          state.next_state = state_nochange;
        } line
        | program error '\n'
        {
          next_line(0);
        }
        ;

line:
        label_concat assign_equal_ops expr '\n'
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
        label_concat '=' expr '\n'
        {
          struct pnode *parms;

          /* implements i = 6 + 1 */
          parms = mk_list($3, NULL);
          next_line(set_label($1, parms));
        }
        |
        label_concat DECREMENT '\n'
        {
          struct pnode *parms;

          /* implements i-- */
          parms = mk_list(mk_1op(DECREMENT, mk_symbol($1)), NULL);
          next_line(set_label($1, parms));
        }
        |
        label_concat INCREMENT '\n'
        {
          struct pnode *parms;

          /* implements i++ */
          parms = mk_list(mk_1op(INCREMENT, mk_symbol($1)), NULL);
          next_line(set_label($1, parms));
        }
        |
        label_concat statement
        {
          if (asm_enabled() && (state.lst.line.linetype == none)) {
            if (IS_RAM_ORG) {
              /* alias to next definition */
              state.lst.line.linetype = res;
            }
            else
              state.lst.line.linetype = insn;
          }

          if (asm_enabled()) {
            if (state.mac_head) {
              /* This is a macro definition.  Set it up */
              struct symbol *mac;
              struct macro_head *h = NULL;

              mac = get_symbol(state.stMacros, $1);
              if (mac)
                h = get_symbol_annotation(mac);

              /* It's not an error if macro was defined on pass 1 and
                 we're in pass 2. */
              if (h &&
                  !((h->pass == 1) && (state.pass == 2))) {
                gperror(GPE_DUPLICATE_MACRO, NULL);
              } else {
                if (!mac)
                  mac = add_symbol(state.stMacros, $1);
                annotate_symbol(mac, state.mac_head);
                h = state.mac_head;
                h->line_number = state.src->line_number;
                h->file_symbol = state.src->file_symbol;
              }
              h->pass = state.pass;

              /* The macro is defined so allow calls. */
              if (state.pass == 2)
                h->defined = 1;

              state.mac_head = NULL;
            } else if (!state.mac_prev) {
              /* Outside a macro, just define the label. */
              switch (state.lst.line.linetype) {
              case sec:
                strncpy(state.obj.new_sec_name, $1, 78);
                break;
              case set:
                set_global($1, $2, TEMPORARY, gvt_constant);
                break;
              case org:
              case equ:
                set_global($1, $2, PERMANENT, gvt_constant);
                break;
              case insn:
              case data:
              case res:
                if (IS_RAM_ORG)
                  set_global($1, $2, PERMANENT, gvt_static);
                else
                  set_global($1, $2, PERMANENT, gvt_address);
                break;
              case dir:
                gperror(GPE_ILLEGAL_LABEL, NULL);
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
          if (state.mac_head) {
            /* This is a macro definition, but the label was missing */
            state.mac_head = NULL;
            gperror(GPE_NO_MACRO_NAME, NULL);
          } else {
            if (state.found_end) {
              switch (state.src->type) {
              case src_while:
                gperror(GPE_EXPECTED, "Expected (ENDW)");
                break;

              case src_macro:
                gperror(GPW_EXPECTED,"Expected (ENDM)");
                /* fall through */

              default:
                break;
              }

              if (state.astack != NULL) {
                struct amode *old;

                while (state.astack) {
                  old = state.astack;
                  state.astack = state.astack->prev;
                  free(old);
                }
                gpwarning(GPW_EXPECTED, "Expected (ENDIF)");
              }
            }

            next_line(0);

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
          if (!state.mac_prev) {
            if (!IS_RAM_ORG)
              /* We want to have r as the value to assign to label */
              $$ = gp_processor_byte_to_org(state.device.class, state.org);
            else
              $$ = state.org;
          } else {
            macro_append();
          }
        }
        |
        PROCESSOR {  force_ident = 1; }
        IDENTIFIER '\n'
        {
          $$ = do_or_append_insn($1, mk_list(mk_symbol($3), NULL));
          force_ident = 0;
        }
        |
        LIST '\n'
        {
          $$ = do_or_append_insn($1, NULL);
        }
        |
        LIST {  force_decimal = 1; }
        list_block '\n'
        {
          $$ = do_or_append_insn($1, $3);
          force_decimal = 0;
        }
        |
        decimal_ops {  force_decimal = 1; }
        parameter_list '\n'
        {
          $$ = do_or_append_insn($1, $3);
          force_decimal = 0;
        }
        |
        DEFINE IDENTIFIER STRING '\n'
        {
          $$ = do_or_append_insn($1, mk_list(mk_string($2),
              mk_list(mk_string($3), NULL)));
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

          if (!state.mac_prev) {
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

          if (!state.mac_prev) {
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
          if (!state.mac_prev) {
            begin_cblock($2);
          } else {
            macro_append();
          }
          next_line(0);
        }
        const_block
        ENDC '\n'
        {
          if (state.mac_prev) {
            macro_append();
          }
          $$ = 0;
        }
        |
        CBLOCK '\n'
        {
          if (!state.mac_prev) {
            continue_cblock();
          } else {
            macro_append();
          }
          next_line(0);
        }
        const_block
        ENDC '\n'
        {
          if (state.mac_prev) {
            macro_append();
          }
          $$ = 0;
        }
        |
        CBLOCK error ENDC '\n'
        {
          $$ = 0;
        }
        ;

const_block:
        |
        const_block const_line
        {
          next_line(0);
        }
        ;

const_line:
        '\n'
        |
        const_def_list '\n'
        {
          if (state.mac_prev) {
            macro_append();
          }
        }
        |
        label_concat '\n'
        {
          if (!state.mac_prev) {
            cblock_expr(mk_symbol($1));
          } else {
            macro_append();
          }
        }
        |
        label_concat expr '\n'
        {
          if (!state.mac_prev) {
            cblock_expr_incr(mk_symbol($1), $2);
          } else {
            macro_append();
          }
        }
        ;

const_def_list:
        const_def
        |
        const_def_list ',' const_def
        ;

const_def:
        cidentifier
        {
          if (!state.mac_prev) {
            cblock_expr($1);
          }
        }
        |
        cidentifier ':' expr
        {
          if (!state.mac_prev) {
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
          $$ = mk_list(mk_constant(INDFOFFSET),
                       mk_list($3, $1));
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
        |
        VAR_BEGIN expr ')'
        {
          $$ = mk_2op(CONCAT, mk_symbol($1), mk_1op(VAR, $2));
        }
        |
        VAR_BEGIN expr VAR_END
        {
          $$ = mk_2op(CONCAT, mk_symbol($1),
                        mk_2op(CONCAT, mk_1op(VAR, $2), mk_symbol($3)));
        }
        ;

label_concat:
        LABEL
        {
          $$ = $1;
        }
        |
        VARLAB_BEGIN expr ')'
        {
          if (asm_enabled() && !state.mac_prev) {
            $$ = evaluate_concatenation(mk_2op(CONCAT,  mk_symbol($1),
                           mk_1op(VAR, $2)));
          }
        }
        |
        VARLAB_BEGIN expr VAR_END
        {
          if (asm_enabled() && !state.mac_prev) {
            $$ = evaluate_concatenation(mk_2op(CONCAT,  mk_symbol($1),
                      mk_2op(CONCAT, mk_1op(VAR, $2), mk_symbol($3))));
          }
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
            force_ident = 1;
          }
        }
        e9op e8
        {
          $$ = mk_2op($3, mk_symbol($1), $4);
          force_ident = 0;
        }
        |
        e8
        {
          $$ = $1;
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

/* evaluates variables
   Copyright (C) 2002, 2003, 2004, 2005
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
#include "gpasm.h"
#include "evaluate.h"
#include "directive.h"
#include "gperror.h"
#include "parse.h"
#include "coff.h"

int enforce_arity(int arity, int must_be)
{
  if (arity == must_be)
    return 1;
  else {
    if (arity < must_be) {
      gpverror(GPE_MISSING_ARGU, NULL);
    }
    else {
      gpverror(GPE_TOO_MANY_ARGU, NULL);
    }
    return 0;
  }
}

int enforce_simple(struct pnode *p)
{
  switch (p->tag) {
  case symbol:
    return 1;
    break;

  case string:
    gpverror(GPE_ILLEGAL_ARGU, NULL, p->value.string);
    break;

  default:
    gperror(GPE_ILLEGAL_ARGU, "Illegal argument");
  }
  return 0;
}

int list_length(struct pnode *L)
{
  struct pnode *p = L;
  int n = 0;

  while (p && list == p->tag) {
    ++n;
    p = TAIL(p);
  }

  return (NULL != p) ? n + 1: n;
}

int can_evaluate(struct pnode *p)
{
  switch (p->tag) {
  case constant:
    return 1;

  case offset:
    if (state.extended_pic16e == false) {
      gpverror(GPE_BADCHAR, NULL, '[');
    }
    return can_evaluate(p->value.offset);

  case symbol:
    {
      char buf[BUFSIZ];
      struct symbol *s;

      /* '$' means current org, which we can always evaluate */
      if (strcmp(p->value.symbol, "$") == 0) {
        return 1;
      }
      else {
        struct variable *var = NULL;

        /* Otherwise look it up */
        s = get_symbol(state.stTop, p->value.symbol);

        if (s == NULL) {
          if ('\0' == *p->value.symbol)
            gpverror(GPE_MISSING_ARGU, NULL);
          else
            gpverror(GPE_NOSYM, NULL, p->value.symbol);
        }
        else {
          var = get_symbol_annotation(s);

          if (var == NULL) {
            snprintf(buf,
                     sizeof(buf),
                     "Symbol not assigned a value (%s).",
                     p->value.symbol);
            gpwarning(GPW_UNKNOWN, buf);
          }
        }

        return ((s != NULL) && (var != NULL));
      }
    }

  case unop:
    return can_evaluate(p->value.unop.p0);

  case binop:
    return can_evaluate(p->value.binop.p0) && can_evaluate(p->value.binop.p1);

  case string:
    gpverror(GPE_ILLEGAL_ARGU, NULL, p->value.string);
    return 0;

  default:
    assert(0);
  }

  return 0;
}

int can_evaluate_value(struct pnode *p)
{
  switch (p->tag) {
  case constant:
    return 1;

  case offset:
    if (state.extended_pic16e == false) {
      gpverror(GPE_BADCHAR, NULL, '[');
    }
    return can_evaluate_value(p->value.offset);

  case symbol:
    /* '$' means current org, which we can evaluate if section at absolute address */
    if (strcmp(p->value.symbol, "$") == 0)
      return (0 != (state.obj.new_sec_flags & STYP_ABS));
    else {
      /* Otherwise look it up */
      struct symbol *s = get_symbol(state.stTop, p->value.symbol);

      if (s == NULL)
        return 0;
      else {
        struct variable *var = get_symbol_annotation(s);
        if (NULL == var) {
          return 0;
        }
        else {
          switch (var->type) {
          case gvt_extern:
          case gvt_global:
          case gvt_static:
          case gvt_absolute:
          case gvt_debug:
            return 0;
            
          default:
            return 1;
          }
        }
      }
    }

  case unop:
    return can_evaluate_value(p->value.unop.p0);

  case binop:
    return can_evaluate_value(p->value.binop.p0) && can_evaluate_value(p->value.binop.p1);

  case string:
    gpverror(GPE_ILLEGAL_ARGU, NULL, p->value.string);
    return 0;

  default:
    assert(0);
  }

  return 0;
}

static int is_program_segment(struct pnode *p)
{
  if (symbol == p->tag && 0 != strcmp(p->value.symbol, "$")) {
    struct symbol *s = get_symbol(state.stTop, p->value.symbol);
    struct variable *var = get_symbol_annotation(s);
    assert(var != NULL);
    /* if var type is gvt_cblock return 1, else return 0 */
    return gvt_cblock != var->type;
  }
  else
    return 0;
}

gpasmVal evaluate(struct pnode *p)
{
  struct variable *var;
  gpasmVal p0, p1;

  switch (p->tag) {
  case constant:
    return p->value.constant;

  case offset:
    return evaluate(p->value.offset);

  case symbol:
    {
      struct symbol *s;

      if (strcmp(p->value.symbol, "$") == 0) {
        if (IS_RAM_ORG)
          return state.org;
        return gp_processor_byte_to_org(state.device.class, state.org);
      }
      else {
        s = get_symbol(state.stTop, p->value.symbol);
        var = get_symbol_annotation(s);
        assert(var != NULL);
        return var->value;
      }
    }

  case unop:
    switch (p->value.unop.op) {
    case '!':
      return !evaluate(p->value.unop.p0);

    case '+':
      return  evaluate(p->value.unop.p0);

    case '-':
      return -evaluate(p->value.unop.p0);

    case '~':
      return ~evaluate(p->value.unop.p0);

    case UPPER:
      return (evaluate(p->value.unop.p0) >> 16) & 0xff;

    case HIGH:
      {
        gpasmVal val = (evaluate(p->value.unop.p0) >> 8) & 0xff;
        /* set 7th bit if in absolute mode and PROC_CLASS_PIC14E and not in cblock;
         * relative mode is handeled by the lonker */
        if (absolute == state.mode &&
          PROC_CLASS_PIC14E == state.device.class &&
          is_program_segment(p->value.unop.p0)) {
          val |= 0x80;
        }
        return val;
      }

    case LOW:
      return evaluate(p->value.unop.p0) & 0xff;

    case INCREMENT:
      return evaluate(p->value.unop.p0) + 1;

    case DECREMENT:
      return evaluate(p->value.unop.p0) - 1;

    default:
      assert(0);
    }

  case binop:
    p0 = evaluate(p->value.binop.p0);
    p1 = evaluate(p->value.binop.p1);
    switch (p->value.binop.op) {
    case '+':
      return p0 + p1;

    case '-':
      return p0 - p1;

    case '*':
      return p0 * p1;

    case '/':
      if (p1 == 0){
        gpverror(GPE_DIVBY0, NULL);
        return 0;
      }
      else {
        return p0 / p1;
      }

    case '%':
      if (p1 == 0){
        gpverror(GPE_DIVBY0, NULL);
        return 0;
      }
      else {
        return p0 % p1;
      }

    case '&':
      return p0 & p1;

    case '|':
      return p0 | p1;

    case '^':
      return p0 ^ p1;

    case LSH:
      if (state.mpasm_compatible) {
        /* MPASM compatible:
         * It seems that x << n is actually x << (n % (sizeof(int) * 8))
         * on x86 architectures, so 0x1234 << 32 results 0x1234
         * which is wrong but compatible with MPASM */
        return p0 << p1;
      }
      else {
      /* x << n results sign extension for n >= (sizeof(int) * 8) */
        return (p1 >= sizeof(int) * 8) ? ((p0 < 0) ? -1 : 0) : p0 << p1;
      }

    case RSH:
      if (state.mpasm_compatible) {
        /* MPASM compatible: see https://sourceforge.net/p/gputils/bugs/252/ 
         * It seems that x >> n is actually x >> (n % (sizeof(int) * 8))
         * on x86 architectures, so 0x1234 >> 32 results 0x1234
         * which is wrong but compatible with MPASM */
        return p0 >> p1;
      }
      else {
      /* x >> n results sign extension for n >= (sizeof(int) * 8) */
        return (p1 >= sizeof(int) * 8) ? ((p0 < 0) ? -1 : 0) : p0 >> p1;
      }

    case EQUAL:
      return p0 == p1;

    case '<':
      return p0 < p1;

    case '>':
      return p0 > p1;

    case NOT_EQUAL:
      return p0 != p1;

    case GREATER_EQUAL:
      return p0 >= p1;

    case LESS_EQUAL:
      return p0 <= p1;

    case LOGICAL_AND:
      return p0 && p1;

    case LOGICAL_OR:
      return p0 || p1;

    case '=':
      gpverror(GPE_BADCHAR, NULL, '=');
      return 0;
    default:
      assert(0); /* Unhandled binary operator */
    }

  default:
    assert(0); /* Unhandled parse node tag */
  }
  return (0); /* Should never reach here */
}

/* Attempt to evaluate expression 'p'.  Return its value if
 * successful, otherwise generate an error message and return 0.  */

gpasmVal maybe_evaluate(struct pnode *p)
{
  gpasmVal r;

  if (p && can_evaluate(p)) {
    r = evaluate(p);
  }
  else {
    r = 0;
  }

  return r;
}

/* count the number of relocatable addesses in the expression */

int count_reloc(struct pnode *p)
{
  struct symbol *s;
  struct variable *var;

  if (state.mode == absolute)
    return 0;

  switch (p->tag) {
  case constant:
    return 0;

  case offset:
    return count_reloc(p->value.offset);

  case symbol:
    if (strcmp(p->value.symbol, "$") == 0) {
      return 1;
    }
    else {
      s = get_symbol(state.stTop, p->value.symbol);
      if (s != NULL) {
        var = get_symbol_annotation(s);
        if (var != NULL) {
          switch(var->type) {
          case gvt_extern:
          case gvt_global:
          case gvt_static:
          case gvt_address:
            return 1;

          default:
            return 0;
          }
        }
      }
    }
    return 0;

  case unop:
    return count_reloc(p->value.unop.p0);

  case binop:
    return count_reloc(p->value.binop.p0) + count_reloc(p->value.binop.p1);

  default:
    assert(0);
  }

  return 0;
}

/* When generating object files, operands with relocatable addresses can only be
   [UPPER|HIGH|LOW]([<relocatable symbol>] + [<offs>]) */

static void
add_reloc(struct pnode *p, short offs, unsigned short type)
{
  struct symbol *s = NULL;
  struct variable *var = NULL;

  switch (p->tag) {
  case offset:
    add_reloc(p->value.offset, offs, type);
    break;

  case symbol:
    if (strcmp(p->value.symbol, "$") == 0) {
      char buffer[BUFSIZ];
      unsigned org;
      if (IS_RAM_ORG)
        org = state.org;
      else
        org = gp_processor_byte_to_org(state.device.class, state.org);

      snprintf(buffer, sizeof(buffer), "_%s_%04X", state.obj.new_sec_name, org);
      /* RELOCT_ACCESS has always also RELOCT_F, which has already
         created this symbol.*/
      if (type != RELOCT_ACCESS)
        set_global(buffer, org, PERMANENT, IS_RAM_ORG ? gvt_static : gvt_address);
      s = get_symbol(state.stTop, buffer);
    }
    else {
      s = get_symbol(state.stTop, p->value.symbol);
    }
    if (s != NULL) {
      var = get_symbol_annotation(s);
      if (var != NULL) {
        switch(var->type) {
        case gvt_extern:
        case gvt_global:
        case gvt_static:
        case gvt_address:
          coff_reloc(var->coff_num, offs, type);
          return;

        default:
          return;
        }
      }
    }
    return;

  case unop:
    switch (p->value.unop.op) {
    case UPPER:
      add_reloc(p->value.unop.p0, offs, RELOCT_UPPER);
      return;

    case HIGH:
      add_reloc(p->value.unop.p0, offs, RELOCT_HIGH);
      return;

    case LOW:
      add_reloc(p->value.unop.p0, offs, RELOCT_LOW);
      return;

    case '!':
    case '+':
    case '-':
    case '~':
    case INCREMENT:
    case DECREMENT:
      gpverror(GPE_UNRESOLVABLE, NULL);
      return;

    default:
      assert(0);
    }

  case binop:
    switch (p->value.binop.op) {
    case '+':
      /* The symbol can be in either position */
      if (count_reloc(p->value.binop.p0) == 1) {
        add_reloc(p->value.binop.p0, offs + maybe_evaluate(p->value.binop.p1), type);
      }
      else {
        add_reloc(p->value.binop.p1, offs + maybe_evaluate(p->value.binop.p0), type);
      }
      return;

    case '-':
      /* The symbol has to be first */
      if (count_reloc(p->value.binop.p0) == 1) {
        add_reloc(p->value.binop.p0, offs - maybe_evaluate(p->value.binop.p1), type);
      }
      else {
        gpverror(GPE_UNRESOLVABLE, NULL);
      }
      return;

    case '*':
    case '/':
    case '%':
    case '&':
    case '|':
    case '^':
    case LSH:
    case RSH:
    case EQUAL:
    case '<':
    case '>':
    case NOT_EQUAL:
    case GREATER_EQUAL:
    case LESS_EQUAL:
    case LOGICAL_AND:
    case LOGICAL_OR:
    case '=':
      gpverror(GPE_UNRESOLVABLE, NULL);
      return;

    default:
      assert(0); /* Unhandled binary operator */
    }
    return;

  case constant:
  default:
    assert(0);
  }
}

/* Determine if the expression is the difference between two symbols in
   the same section.  If so, calculate the offset and don't generate a
   relocation.

   [UPPER|HIGH|LOW]([<relocatable symbol>] - [<relocatable symbol>])

*/

static int
same_section(struct pnode *p)
{
  struct pnode *p0;
  struct pnode *p1;
  struct symbol *sym0;
  struct symbol *sym1;
  struct variable *var0;
  struct variable *var1;

  if (!state.obj.enabled)
    return 0;

  if ((p->tag == unop) &&
      ((p->value.unop.op == UPPER) ||
       (p->value.unop.op == HIGH) ||
       (p->value.unop.op == LOW))) {
    p = p->value.unop.p0;
  }

  if ((p->tag != binop) ||
      (p->value.binop.op != '-') ||
      (count_reloc(p->value.binop.p0) != 1))
    return 0;

  p0 = p->value.binop.p0;
  p1 = p->value.binop.p1;

  if ((p0->tag != symbol) ||
      (p1->tag != symbol))
    return 0;

  sym0 = get_symbol(state.stTop, p0->value.symbol);
  sym1 = get_symbol(state.stTop, p1->value.symbol);
  var0 = get_symbol_annotation(sym0);
  var1 = get_symbol_annotation(sym1);

  /* They must come from the same section. Debug symbols are not placed
     in the global symbol table, so don't worry about symbol type. */
  /* Fail if sections are not known (== 0) or not the same */
  if (0 == var0->coff_section_num ||
    0 == var1->coff_section_num ||
    var0->coff_section_num != var1->coff_section_num)
    return 0;

  return 1;

}

gpasmVal reloc_evaluate(struct pnode *p, unsigned short type)
{
  gpasmVal r = 0;
  int count = 0;

  if (state.mode == absolute) {
    r = maybe_evaluate(p);
  }
  else {
    count = count_reloc(p);
    if (count == 0) {
      /* no relocatable addresses */
      r = maybe_evaluate(p);
    }
    else if (count > 1) {
      if ((count == 2) && (same_section(p))) {
        /* It is valid to take the difference between two symbols in the same
           section.  Evaluate, but don't add a relocation. */
        r = maybe_evaluate(p);
      }
      else {
        /* too many relocatable addresses */
        gpverror(GPE_UNRESOLVABLE, NULL);
        r = 0;
      }
    }
    else {
      /* add the coff relocation */
      add_reloc(p, 0, type);
      r = 0;
    }
  }

  return r;
}

/* evaluate the number of passes for the "fill" directive*/
int eval_fill_number(struct pnode *p)
{
  int number;

  number = maybe_evaluate(p);
  if (state.device.class->rom_width == 8 && (number & 0x1) == 1)
    gpverror(GPE_FILL_ODD, NULL);
  number = gp_processor_org_to_byte(state.device.class, number) >> 1;

  return number;
}

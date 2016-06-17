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

/*------------------------------------------------------------------------------------------------*/

gp_boolean
enforce_arity(int arity, int must_be)
{
  if (arity == must_be) {
    return true;
  }
  else {
    if (arity < must_be) {
      gperror_verror(GPE_MISSING_ARGU, NULL);
    }
    else {
      gperror_verror(GPE_TOO_MANY_ARGU, NULL);
    }

    return false;
  }
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
enforce_simple(const pnode_t *p)
{
  switch (p->tag) {
  case PTAG_SYMBOL:
    return true;
    break;

  case PTAG_STRING:
    gperror_verror(GPE_ILLEGAL_ARGU, NULL, p->value.string);
    break;

  default:
    gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
  }
  return false;
}

/*------------------------------------------------------------------------------------------------*/

int
list_length(const pnode_t *L)
{
  const pnode_t *p = L;
  int            n = 0;

  while ((p != NULL) && (p->tag == PTAG_LIST)) {
    ++n;
    p = TAIL(p);
  }

  return ((p != NULL) ? (n + 1) : n);
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
can_evaluate(const pnode_t *p)
{
  char              buf[BUFSIZ];
  const symbol_t   *sym;
  const variable_t *var;

  switch (p->tag) {
  case PTAG_CONSTANT:
    return true;

  case PTAG_OFFSET:
    if (state.extended_pic16e == false) {
      gperror_verror(GPE_BADCHAR, NULL, '[');
    }
    return can_evaluate(p->value.offset);

  case PTAG_SYMBOL:
    {
      /* '$' means current org, which we can always evaluate */
      if (strcmp(p->value.symbol, "$") == 0) {
        return true;
      }
      else {
        /* Otherwise look it up. */
        sym = sym_get_symbol(state.stTop, p->value.symbol);

        if (sym == NULL) {
          var = NULL;

          if (p->value.symbol[0] == '\0') {
            gperror_verror(GPE_MISSING_ARGU, NULL);
          }
          else {
            gperror_verror(GPE_NOSYM, NULL, p->value.symbol);
          }
        }
        else {
          var = sym_get_symbol_annotation(sym);

          if (var == NULL) {
            snprintf(buf, sizeof(buf), "Symbol not assigned a value: \"%s\"", p->value.symbol);
            gperror_warning(GPW_UNKNOWN, buf);
          }
        }

        return ((sym != NULL) && (var != NULL));
      }
    }

  case PTAG_UNOP:
    return can_evaluate(p->value.unop.p0);

  case PTAG_BINOP:
    return (can_evaluate(p->value.binop.p0) && can_evaluate(p->value.binop.p1));

  case PTAG_STRING:
    gperror_verror(GPE_ILLEGAL_ARGU, NULL, p->value.string);
    return false;

  default:
    assert(0);
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
can_evaluate_value(const pnode_t *p)
{
  const symbol_t   *sym;
  const variable_t *var;

  switch (p->tag) {
  case PTAG_CONSTANT:
    return true;

  case PTAG_OFFSET:
    if (state.extended_pic16e == false) {
      gperror_verror(GPE_BADCHAR, NULL, '[');
    }
    return can_evaluate_value(p->value.offset);

  case PTAG_SYMBOL:
    /* '$' means current org, which we can evaluate if section at absolute address */
    if (strcmp(p->value.symbol, "$") == 0) {
      return (((state.obj.new_sect_flags & STYP_ABS) != 0) ? true : false);
    }
    else {
      /* Otherwise look it up */
      sym = sym_get_symbol(state.stTop, p->value.symbol);

      if (sym == NULL) {
        return false;
      }
      else {
        var = sym_get_symbol_annotation(sym);

        if (NULL == var) {
          return false;
        }
        else {
          switch (var->type) {
          case GVT_EXTERN:
          case GVT_GLOBAL:
          case GVT_STATIC:
          case GVT_ABSOLUTE:
          case GVT_DEBUG:
            return false;

          default:
            return true;
          }
        }
      }
    }

  case PTAG_UNOP:
    return can_evaluate_value(p->value.unop.p0);

  case PTAG_BINOP:
    return (can_evaluate_value(p->value.binop.p0) && can_evaluate_value(p->value.binop.p1));

  case PTAG_STRING:
    gperror_verror(GPE_ILLEGAL_ARGU, NULL, p->value.string);
    return false;

  default:
    assert(0);
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_is_program_segment(const pnode_t *p)
{
  const symbol_t   *sym;
  const variable_t *var;

  if ((p->tag == PTAG_SYMBOL) && (strcmp(p->value.symbol, "$") != 0)) {
    sym = sym_get_symbol(state.stTop, p->value.symbol);
    assert(sym != NULL);

    var = sym_get_symbol_annotation(sym);
    assert(var != NULL);
    /* If var type is GVT_ADDRESS return true, else return false. */
    return ((var->type == GVT_ADDRESS) ? true : false);
  }
  else {
    return false;
  }
}

/*------------------------------------------------------------------------------------------------*/

gpasmVal
evaluate(const pnode_t *p)
{
  const symbol_t   *sym;
  const variable_t *var;
  gpasmVal          p0;
  gpasmVal          p1;

  switch (p->tag) {
  case PTAG_CONSTANT:
    return p->value.constant;

  case PTAG_OFFSET:
    return evaluate(p->value.offset);

  case PTAG_SYMBOL:
    {
      if (strcmp(p->value.symbol, "$") == 0) {
        return (IS_RAM_ORG ? state.byte_addr :
                             gp_processor_byte_to_real(state.processor, state.byte_addr));
      }
      else {
        sym = sym_get_symbol(state.stTop, p->value.symbol);
        assert(sym != NULL);

        var = sym_get_symbol_annotation(sym);
        assert(var != NULL);

        return var->value;
      }
    }

  case PTAG_UNOP:
    switch (p->value.unop.op) {
    case '!':
      return (!evaluate(p->value.unop.p0));

    case '+':
      return  evaluate(p->value.unop.p0);

    case '-':
      return (-evaluate(p->value.unop.p0));

    case '~':
      return (~evaluate(p->value.unop.p0));

    case UPPER:
      return ((evaluate(p->value.unop.p0) >> 16) & 0xff);

    case HIGH:
      {
        gpasmVal val = (evaluate(p->value.unop.p0) >> 8) & 0xff;
        /* Set 7th bit if in absolute mode and PROC_CLASS_PIC14E or PROC_CLASS_PIC14EX and
         * address relative mode is handled by the linker. */
        if ((state.mode == MODE_ABSOLUTE) && (IS_PIC14E_CORE || IS_PIC14EX_CORE) &&
            _is_program_segment(p->value.unop.p0)) {
          val |= PIC14E_FSRxH_FLASH_SEL;
        }
        return val;
      }

    case LOW:
      return (evaluate(p->value.unop.p0) & 0xff);

    case INCREMENT:
      return (evaluate(p->value.unop.p0) + 1);

    case DECREMENT:
      return (evaluate(p->value.unop.p0) - 1);

    default:
      assert(0);
    }

  case PTAG_BINOP:
    p0 = evaluate(p->value.binop.p0);
    p1 = evaluate(p->value.binop.p1);
    switch (p->value.binop.op) {
    case '+':
      return (p0 + p1);

    case '-':
      return (p0 - p1);

    case '*':
      return (p0 * p1);

    case '/':
      if (p1 == 0){
        gperror_verror(GPE_DIVBY0, NULL);
        return 0;
      }
      else {
        return (p0 / p1);
      }

    case '%':
      if (p1 == 0){
        gperror_verror(GPE_DIVBY0, NULL);
        return 0;
      }
      else {
        return (p0 % p1);
      }

    case '&':
      return (p0 & p1);

    case '|':
      return (p0 | p1);

    case '^':
      return (p0 ^ p1);

    case LSH:
      if (state.mpasm_compatible) {
        /* MPASM compatible:
         * It seems that x << n is actually x << (n % (sizeof(int) * 8))
         * on x86 architectures, so 0x1234 << 32 results 0x1234
         * which is wrong but compatible with MPASM. */
        return (p0 << p1);
      }
      else {
        /* x << n results sign extension for n >= (sizeof(int) * 8) */
        return ((p1 >= (sizeof(int) * 8)) ? ((p0 < 0) ? -1 : 0) : (p0 << p1));
      }

    case RSH:
      if (state.mpasm_compatible) {
        /* MPASM compatible: see https://sourceforge.net/p/gputils/bugs/252/
         * It seems that x >> n is actually x >> (n % (sizeof(int) * 8))
         * on x86 architectures, so 0x1234 >> 32 results 0x1234
         * which is wrong but compatible with MPASM. */
        return (p0 >> p1);
      }
      else {
      /* x >> n results sign extension for n >= (sizeof(int) * 8) */
        return ((p1 >= (sizeof(int) * 8)) ? ((p0 < 0) ? -1 : 0) : (p0 >> p1));
      }

    case EQUAL:
      return (p0 == p1);

    case '<':
      return (p0 < p1);

    case '>':
      return (p0 > p1);

    case NOT_EQUAL:
      return (p0 != p1);

    case GREATER_EQUAL:
      return (p0 >= p1);

    case LESS_EQUAL:
      return (p0 <= p1);

    case LOGICAL_AND:
      return (p0 && p1);

    case LOGICAL_OR:
      return (p0 || p1);

    case '=':
      gperror_verror(GPE_BADCHAR, NULL, '=');
      return 0;

    default:
      assert(0); /* Unhandled binary operator. */
    }

  default:
    assert(0); /* Unhandled parse node tag. */
  }
  return 0;    /* Should never reach here. */
}

/*------------------------------------------------------------------------------------------------*/

/* Attempt to evaluate expression 'p'. Return its value if successful,
 * otherwise generate an error message and return 0.  */

gpasmVal
maybe_evaluate(const pnode_t *p)
{
  gpasmVal r;

  if ((p != NULL) && can_evaluate(p)) {
    r = evaluate(p);
  }
  else {
    r = 0;
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* Count the number of relocatable addesses in the expression. */

int
count_reloc(const pnode_t *p)
{
  const symbol_t   *sym;
  const variable_t *var;

  if (state.mode == MODE_ABSOLUTE) {
    return 0;
  }

  switch (p->tag) {
  case PTAG_CONSTANT:
    return 0;

  case PTAG_OFFSET:
    return count_reloc(p->value.offset);

  case PTAG_SYMBOL:
    if (strcmp(p->value.symbol, "$") == 0) {
      return 1;
    }
    else {
      sym = sym_get_symbol(state.stTop, p->value.symbol);

      if (sym != NULL) {
        var = sym_get_symbol_annotation(sym);

        if (var != NULL) {
          switch (var->type) {
          case GVT_EXTERN:
          case GVT_GLOBAL:
          case GVT_STATIC:
          case GVT_ADDRESS:
            return 1;

          default:
            return 0;
          }
        }
      }
    }
    return 0;

  case PTAG_UNOP:
    return count_reloc(p->value.unop.p0);

  case PTAG_BINOP:
    return (count_reloc(p->value.binop.p0) + count_reloc(p->value.binop.p1));

  default:
    assert(0);
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

/* When generating object files, operands with relocatable addresses can only be
   [UPPER|HIGH|LOW]([<relocatable symbol>] + [<offs>]) */

static void
_add_reloc(const pnode_t *p, short offs, uint16_t type)
{
  const symbol_t   *sym;
  const variable_t *var;
  char              buffer[BUFSIZ];
  int               digits;
  unsigned int      org;
  enum globalLife   life;

  switch (p->tag) {
  case PTAG_OFFSET:
    _add_reloc(p->value.offset, offs, type);
    break;

  case PTAG_SYMBOL:
    if (strcmp(p->value.symbol, "$") == 0) {
      if (IS_RAM_ORG) {
        digits = state.device.class->word_digits;
        org    = state.byte_addr;
        life   = GVT_STATIC;
      }
      else {
        digits = state.device.class->addr_digits;
        org    = gp_processor_byte_to_real(state.processor, state.byte_addr);
        life   = GVT_ADDRESS;
      }

      snprintf(buffer, sizeof(buffer), "_%s_%0*X", state.obj.new_sect_name, digits, org);
      /* RELOCT_ACCESS has always also RELOCT_F, which has already created this symbol. */
      if (type != RELOCT_ACCESS) {
        set_global(buffer, org, LFT_PERMANENT, life, false);
      }

      sym = sym_get_symbol(state.stTop, buffer);
    }
    else {
      sym = sym_get_symbol(state.stTop, p->value.symbol);
    }

    if (sym != NULL) {
      var = sym_get_symbol_annotation(sym);

      if (var != NULL) {
        switch(var->type) {
        case GVT_EXTERN:
        case GVT_GLOBAL:
        case GVT_STATIC:
        case GVT_ADDRESS:
          coff_reloc(var->coff_symbol_num, offs, type);
          return;

        default:
          return;
        }
      }
    }
    return;

  case PTAG_UNOP:
    switch (p->value.unop.op) {
    case UPPER:
      _add_reloc(p->value.unop.p0, offs, RELOCT_UPPER);
      return;

    case HIGH:
      _add_reloc(p->value.unop.p0, offs, RELOCT_HIGH);
      return;

    case LOW:
      _add_reloc(p->value.unop.p0, offs, RELOCT_LOW);
      return;

    case '!':
    case '+':
    case '-':
    case '~':
    case INCREMENT:
    case DECREMENT:
      gperror_verror(GPE_UNRESOLVABLE, NULL);
      return;

    default:
      assert(0);
    }

  case PTAG_BINOP:
    switch (p->value.binop.op) {
    case '+':
      /* The symbol can be in either position. */
      if (count_reloc(p->value.binop.p0) == 1) {
        _add_reloc(p->value.binop.p0, offs + maybe_evaluate(p->value.binop.p1), type);
      }
      else {
        _add_reloc(p->value.binop.p1, offs + maybe_evaluate(p->value.binop.p0), type);
      }
      return;

    case '-':
      /* The symbol has to be first. */
      if (count_reloc(p->value.binop.p0) == 1) {
        _add_reloc(p->value.binop.p0, offs - maybe_evaluate(p->value.binop.p1), type);
      }
      else {
        gperror_verror(GPE_UNRESOLVABLE, NULL);
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
      gperror_verror(GPE_UNRESOLVABLE, NULL);
      return;

    default:
      assert(0); /* Unhandled binary operator. */
    }
    return;

  case PTAG_CONSTANT:
  default:
    assert(0);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if the expression is the difference between two symbols in
   the same section. If so, calculate the offset and don't generate a relocation.

   [UPPER|HIGH|LOW]([<relocatable symbol>] - [<relocatable symbol>])
*/

static gp_boolean
_same_section(const pnode_t *p)
{
  const pnode_t    *p0;
  const pnode_t    *p1;
  const symbol_t   *sym0;
  const symbol_t   *sym1;
  const variable_t *var0;
  const variable_t *var1;

  if (!state.obj.enabled) {
    return false;
  }

  if ((p->tag == PTAG_UNOP) &&
      ((p->value.unop.op == UPPER) || (p->value.unop.op == HIGH) || (p->value.unop.op == LOW))) {
    p = p->value.unop.p0;
  }

  if ((p->tag != PTAG_BINOP) ||
      (p->value.binop.op != '-') || (count_reloc(p->value.binop.p0) != 1)) {
    return false;
  }

  p0 = p->value.binop.p0;
  p1 = p->value.binop.p1;

  if ((p0->tag != PTAG_SYMBOL) || (p1->tag != PTAG_SYMBOL)) {
    return false;
  }

  sym0 = sym_get_symbol(state.stTop, p0->value.symbol);
  sym1 = sym_get_symbol(state.stTop, p1->value.symbol);
  var0 = sym_get_symbol_annotation(sym0);
  var1 = sym_get_symbol_annotation(sym1);

  /* They must come from the same section. Debug symbols are not placed
     in the global symbol table, so don't worry about symbol type.
     Fail if sections are not known (== 0) or not the same. */
  if ((var0->coff_section_num == 0) ||
      (var1->coff_section_num == 0) ||
      (var0->coff_section_num != var1->coff_section_num)) {
    return false;
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

gpasmVal
reloc_evaluate(const pnode_t *p, uint16_t type)
{
  gpasmVal r = 0;
  int      count = 0;

  if (state.mode == MODE_ABSOLUTE) {
    r = maybe_evaluate(p);
  }
  else {
    count = count_reloc(p);
    if (count == 0) {
      /* no relocatable addresses */
      r = maybe_evaluate(p);
    }
    else if (count > 1) {
      if ((count == 2) && (_same_section(p))) {
        /* It is valid to take the difference between two symbols in the same
           section.  Evaluate, but don't add a relocation. */
        r = maybe_evaluate(p);
      }
      else {
        /* too many relocatable addresses */
        gperror_verror(GPE_UNRESOLVABLE, NULL);
        r = 0;
      }
    }
    else {
      /* add the coff relocation */
      _add_reloc(p, 0, type);
      r = 0;
    }
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* Evaluate the number of passes for the "fill" directive. */

int
eval_fill_number(const pnode_t *p)
{
  int number;

  number = maybe_evaluate(p);

  if ((state.device.class->rom_width == 8) && ((number & 0x1) == 1)) {
    gperror_verror(GPE_FILL_ODD, NULL);
  }

  return (gp_processor_org_to_byte(state.device.class, number) >> 1);
}

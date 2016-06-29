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
eval_enforce_arity(int arity, int must_be)
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
eval_enforce_simple(const pnode_t *p)
{
  switch (p->tag) {
  case PTAG_SYMBOL:
    return true;
    break;

  case PTAG_STRING:
    gperror_verror(GPE_ILLEGAL_ARGU, NULL, PnString(p));
    break;

  default:
    gperror_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
  }
  return false;
}

/*------------------------------------------------------------------------------------------------*/

int
eval_list_length(const pnode_t *L)
{
  const pnode_t *p = L;
  int            n = 0;

  while ((p != NULL) && PnIsList(p)) {
    ++n;
    p = PnListTail(p);
  }

  return ((p != NULL) ? (n + 1) : n);
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
eval_can_evaluate(const pnode_t *p)
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
    return eval_can_evaluate(PnOffset(p));

  case PTAG_SYMBOL:
    {
      /* '$' means current org, which we can always evaluate */
      if (strcmp(PnSymbol(p), "$") == 0) {
        return true;
      }
      else {
        /* Otherwise look it up. */
        sym = sym_get_symbol(state.stTop, PnSymbol(p));

        if (sym == NULL) {
          var = NULL;

          if (PnSymbol(p)[0] == '\0') {
            gperror_verror(GPE_MISSING_ARGU, NULL);
          }
          else {
            gperror_verror(GPE_NOSYM, NULL, PnSymbol(p));
          }
        }
        else {
          var = sym_get_symbol_annotation(sym);

          if (var == NULL) {
            snprintf(buf, sizeof(buf), "Symbol not assigned a value: \"%s\"", PnSymbol(p));
            gperror_warning(GPW_UNKNOWN, buf);
          }
        }

        return ((sym != NULL) && (var != NULL));
      }
    }

  case PTAG_UNOP:
    return eval_can_evaluate(PnUnOpP0(p));

  case PTAG_BINOP:
    return (eval_can_evaluate(PnBinOpP0(p)) && eval_can_evaluate(PnBinOpP1(p)));

  case PTAG_STRING:
    gperror_verror(GPE_ILLEGAL_ARGU, NULL, PnString(p));
    return false;

  default:
    assert(0);
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
eval_can_evaluate_value(const pnode_t *p)
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
    return eval_can_evaluate_value(PnOffset(p));

  case PTAG_SYMBOL:
    /* '$' means current org, which we can evaluate if section at absolute address */
    if (strcmp(PnSymbol(p), "$") == 0) {
      return (((state.obj.new_sect_flags & STYP_ABS) != 0) ? true : false);
    }
    else {
      /* Otherwise look it up */
      sym = sym_get_symbol(state.stTop, PnSymbol(p));

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
    return eval_can_evaluate_value(PnUnOpP0(p));

  case PTAG_BINOP:
    return (eval_can_evaluate_value(PnBinOpP0(p)) && eval_can_evaluate_value(PnBinOpP1(p)));

  case PTAG_STRING:
    gperror_verror(GPE_ILLEGAL_ARGU, NULL, PnString(p));
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

  if (PnIsSymbol(p) && (strcmp(PnSymbol(p), "$") != 0)) {
    sym = sym_get_symbol(state.stTop, PnSymbol(p));
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
eval_evaluate(const pnode_t *p)
{
  const symbol_t   *sym;
  const variable_t *var;
  gpasmVal          p0;
  gpasmVal          p1;

  switch (p->tag) {
  case PTAG_CONSTANT:
    return PnConstant(p);

  case PTAG_OFFSET:
    return eval_evaluate(PnOffset(p));

  case PTAG_SYMBOL:
    {
      if (strcmp(PnSymbol(p), "$") == 0) {
        return (IS_RAM_ORG ? state.byte_addr :
                             gp_processor_byte_to_real(state.processor, state.byte_addr));
      }
      else {
        sym = sym_get_symbol(state.stTop, PnSymbol(p));
        assert(sym != NULL);

        var = sym_get_symbol_annotation(sym);
        assert(var != NULL);

        return var->value;
      }
    }

  case PTAG_UNOP:
    switch (PnUnOpOp(p)) {
    case '!':
      return (!eval_evaluate(PnUnOpP0(p)));

    case '+':
      return  eval_evaluate(PnUnOpP0(p));

    case '-':
      return (-eval_evaluate(PnUnOpP0(p)));

    case '~':
      return (~eval_evaluate(PnUnOpP0(p)));

    case UPPER:
      return ((eval_evaluate(PnUnOpP0(p)) >> 16) & 0xff);

    case HIGH:
      {
        gpasmVal val = (eval_evaluate(PnUnOpP0(p)) >> 8) & 0xff;
        /* Set 7th bit if in absolute mode and PROC_CLASS_PIC14E or PROC_CLASS_PIC14EX and
         * address relative mode is handled by the linker. */
        if ((state.mode == MODE_ABSOLUTE) && (IS_PIC14E_CORE || IS_PIC14EX_CORE) &&
            _is_program_segment(PnUnOpP0(p))) {
          val |= PIC14E_FSRxH_FLASH_SEL;
        }
        return val;
      }

    case LOW:
      return (eval_evaluate(PnUnOpP0(p)) & 0xff);

    case INCREMENT:
      return (eval_evaluate(PnUnOpP0(p)) + 1);

    case DECREMENT:
      return (eval_evaluate(PnUnOpP0(p)) - 1);

    default:
      assert(0);
    }

  case PTAG_BINOP:
    p0 = eval_evaluate(PnBinOpP0(p));
    p1 = eval_evaluate(PnBinOpP1(p));
    switch (PnBinOpOp(p)) {
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
eval_maybe_evaluate(const pnode_t *p)
{
  gpasmVal r;

  if ((p != NULL) && eval_can_evaluate(p)) {
    r = eval_evaluate(p);
  }
  else {
    r = 0;
  }

  return r;
}

/*------------------------------------------------------------------------------------------------*/

/* Count the number of relocatable addesses in the expression. */

int
eval_count_reloc(const pnode_t *p)
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
    return eval_count_reloc(PnOffset(p));

  case PTAG_SYMBOL:
    if (strcmp(PnSymbol(p), "$") == 0) {
      return 1;
    }
    else {
      sym = sym_get_symbol(state.stTop, PnSymbol(p));

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
    return eval_count_reloc(PnUnOpP0(p));

  case PTAG_BINOP:
    return (eval_count_reloc(PnBinOpP0(p)) + eval_count_reloc(PnBinOpP1(p)));

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
    _add_reloc(PnOffset(p), offs, type);
    break;

  case PTAG_SYMBOL:
    if (strcmp(PnSymbol(p), "$") == 0) {
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
      sym = sym_get_symbol(state.stTop, PnSymbol(p));
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
    switch (PnUnOpOp(p)) {
    case UPPER:
      _add_reloc(PnUnOpP0(p), offs, RELOCT_UPPER);
      return;

    case HIGH:
      _add_reloc(PnUnOpP0(p), offs, RELOCT_HIGH);
      return;

    case LOW:
      _add_reloc(PnUnOpP0(p), offs, RELOCT_LOW);
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
    switch (PnBinOpOp(p)) {
    case '+':
      /* The symbol can be in either position. */
      if (eval_count_reloc(PnBinOpP0(p)) == 1) {
        _add_reloc(PnBinOpP0(p), offs + eval_maybe_evaluate(PnBinOpP1(p)), type);
      }
      else {
        _add_reloc(PnBinOpP1(p), offs + eval_maybe_evaluate(PnBinOpP0(p)), type);
      }
      return;

    case '-':
      /* The symbol has to be first. */
      if (eval_count_reloc(PnBinOpP0(p)) == 1) {
        _add_reloc(PnBinOpP0(p), offs - eval_maybe_evaluate(PnBinOpP1(p)), type);
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

  if (PnIsUnOp(p) &&
      ((PnUnOpOp(p) == UPPER) || (PnUnOpOp(p) == HIGH) || (PnUnOpOp(p) == LOW))) {
    p = PnUnOpP0(p);
  }

  if (!PnIsBinOp(p) || (PnBinOpOp(p) != '-') || (eval_count_reloc(PnBinOpP0(p)) != 1)) {
    return false;
  }

  p0 = PnBinOpP0(p);
  p1 = PnBinOpP1(p);

  if (!PnIsSymbol(p0) || !PnIsSymbol(p1)) {
    return false;
  }

  sym0 = sym_get_symbol(state.stTop, PnSymbol(p0));
  sym1 = sym_get_symbol(state.stTop, PnSymbol(p1));
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
eval_reloc_evaluate(const pnode_t *p, uint16_t type)
{
  gpasmVal r = 0;
  int      count = 0;

  if (state.mode == MODE_ABSOLUTE) {
    r = eval_maybe_evaluate(p);
  }
  else {
    count = eval_count_reloc(p);
    if (count == 0) {
      /* no relocatable addresses */
      r = eval_maybe_evaluate(p);
    }
    else if (count > 1) {
      if ((count == 2) && (_same_section(p))) {
        /* It is valid to take the difference between two symbols in the same
           section.  Evaluate, but don't add a relocation. */
        r = eval_maybe_evaluate(p);
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

  number = eval_maybe_evaluate(p);

  if ((state.device.class->rom_width == 8) && ((number & 0x1) == 1)) {
    gperror_verror(GPE_FILL_ODD, NULL);
  }

  return (gp_processor_org_to_byte(state.device.class, number) >> 1);
}

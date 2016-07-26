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
#include "gpmsg.h"
#include "parse.h"
#include "coff.h"

/*------------------------------------------------------------------------------------------------*/

gp_boolean
eval_enforce_arity(int Arity, int Must_be)
{
  if (Arity == Must_be) {
    return true;
  }

  if (Arity < Must_be) {
    gpmsg_verror(GPE_MISSING_ARGU, NULL);
  }
  else {
    gpmsg_verror(GPE_TOO_MANY_ARGU, NULL);
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
eval_enforce_simple(const pnode_t *Pnode)
{
  switch (Pnode->tag) {
   case PTAG_SYMBOL:
     return true;
     break;

   case PTAG_STRING:
     gpmsg_verror(GPE_ILLEGAL_ARGU, NULL, PnString(Pnode));
     break;

   default:
     gpmsg_error(GPE_ILLEGAL_ARGU, "Illegal argument.");
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

int
eval_list_length(const pnode_t *List)
{
  const pnode_t *p = List;
  int            n = 0;

  while ((p != NULL) && PnIsList(p)) {
    ++n;
    p = PnListTail(p);
  }

  return ((p != NULL) ? (n + 1) : n);
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
eval_can_evaluate(const pnode_t *Pnode)
{
  char              buf[BUFSIZ];
  const symbol_t   *sym;
  const variable_t *var;
  const char       *name;

  switch (Pnode->tag) {
    case PTAG_CONSTANT:
      return true;
      break;

    case PTAG_OFFSET: {
      if (state.extended_pic16e == false) {
        gpmsg_verror(GPE_BADCHAR, NULL, '[');
      }

      return eval_can_evaluate(PnOffset(Pnode));
      break;
    }

    case PTAG_SYMBOL: {
      name = PnSymbol(Pnode);

      /* '$' means current org, which we can always evaluate */
      if (strcmp(name, "$") == 0) {
        return true;
      }

      /* Otherwise look it up. */
      sym = gp_sym_get_symbol(state.stTop, name);

      if (sym == NULL) {
        var = NULL;

        if (name[0] == '\0') {
          gpmsg_verror(GPE_MISSING_ARGU, NULL);
        }
        else {
          gpmsg_verror(GPE_SYM_NOT_DEFINED, NULL, name);
        }
      }
      else {
        var = (const variable_t *)gp_sym_get_symbol_annotation(sym);

        if (var == NULL) {
          snprintf(buf, sizeof(buf), "Symbol not assigned a value: \"%s\"", name);
          gpmsg_warning(GPW_UNKNOWN, buf);
        }
      }

      return ((sym != NULL) && (var != NULL));
      break;
    }

    case PTAG_UNOP:
      return eval_can_evaluate(PnUnOpP0(Pnode));
      break;

    case PTAG_BINOP:
      return (eval_can_evaluate(PnBinOpP0(Pnode)) && eval_can_evaluate(PnBinOpP1(Pnode)));
      break;

    case PTAG_STRING:
      gpmsg_verror(GPE_ILLEGAL_ARGU, NULL, PnString(Pnode));
      return false;
      break;

    default:
      assert(0);
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
eval_can_evaluate_value(const pnode_t *Pnode)
{
  const symbol_t   *sym;
  const variable_t *var;
  const char       *name;

  switch (Pnode->tag) {
    case PTAG_CONSTANT:
      return true;
      break;

    case PTAG_OFFSET: {
      if (state.extended_pic16e == false) {
        gpmsg_verror(GPE_BADCHAR, NULL, '[');
      }

      return eval_can_evaluate_value(PnOffset(Pnode));
      break;
    }

    case PTAG_SYMBOL: {
      name = PnSymbol(Pnode);

      /* '$' means current org, which we can evaluate if section at absolute address. */
      if (strcmp(name, "$") == 0) {
        return (FlagIsSet(state.obj.new_sect_flags, STYP_ABS) ? true : false);
      }

      /* Otherwise look it up. */
      sym = gp_sym_get_symbol(state.stTop, name);

      if (sym == NULL) {
        return false;
      }

      var = gp_sym_get_symbol_annotation(sym);

      if (var == NULL) {
        return false;
      }

      if (FlagIsSet(var->flags, VATRR_HAS_NO_VALUE)) {
        msg_has_no_value(NULL, name);
      }

      switch (var->type) {
        case VAL_EXTERNAL:
        case VAL_GLOBAL:
        case VAL_STATIC:
        case VAL_ABSOLUTE:
        case VAL_DEBUG:
          return false;

        default:
          return true;
      }

      break;
    }

    case PTAG_UNOP:
      return eval_can_evaluate_value(PnUnOpP0(Pnode));
      break;

    case PTAG_BINOP:
      return (eval_can_evaluate_value(PnBinOpP0(Pnode)) && eval_can_evaluate_value(PnBinOpP1(Pnode)));
      break;

    case PTAG_STRING:
      gpmsg_verror(GPE_ILLEGAL_ARGU, NULL, PnString(Pnode));
      return false;
      break;

    default:
      assert(0);
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_is_program_segment(const pnode_t *Pnode)
{
  const symbol_t   *sym;
  const variable_t *var;

  if (PnIsSymbol(Pnode) && (strcmp(PnSymbol(Pnode), "$") != 0)) {
    sym = gp_sym_get_symbol(state.stTop, PnSymbol(Pnode));
    assert(sym != NULL);

    var = gp_sym_get_symbol_annotation(sym);
    assert(var != NULL);

    if (FlagIsSet(var->flags, VATRR_HAS_NO_VALUE)) {
      msg_has_no_value(NULL, PnSymbol(Pnode));
    }

    return ((var->type == VAL_ADDRESS) ? true : false);
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gpasmVal
eval_evaluate(const pnode_t *Pnode)
{
  const symbol_t   *sym;
  const variable_t *var;
  const char       *name;
  gpasmVal          p0;
  gpasmVal          p1;
  gpasmVal          val;

  switch (Pnode->tag) {
    case PTAG_CONSTANT:
      return PnConstant(Pnode);
      break;

    case PTAG_OFFSET:
      return eval_evaluate(PnOffset(Pnode));
      break;

    case PTAG_SYMBOL: {
      name = PnSymbol(Pnode);

      if (strcmp(name, "$") == 0) {
        return (IS_RAM_ORG ? state.byte_addr :
                             gp_processor_insn_from_byte_p(state.processor, state.byte_addr));
      }

      sym = gp_sym_get_symbol(state.stTop, name);
      assert(sym != NULL);

      var = gp_sym_get_symbol_annotation(sym);
      assert(var != NULL);

      if (FlagIsSet(var->flags, VATRR_HAS_NO_VALUE)) {
        msg_has_no_value(NULL, name);
      }

      return var->value;
      break;
    }

    case PTAG_UNOP: {
      switch (PnUnOpOp(Pnode)) {
        case '!':
          return (!eval_evaluate(PnUnOpP0(Pnode)));
          break;

        case '+':
          return  eval_evaluate(PnUnOpP0(Pnode));
          break;

        case '-':
          return (-eval_evaluate(PnUnOpP0(Pnode)));
          break;

        case '~':
          return (~eval_evaluate(PnUnOpP0(Pnode)));
          break;

        case UPPER:
          return ((eval_evaluate(PnUnOpP0(Pnode)) >> 16) & 0xff);
          break;

        case HIGH: {
          val = (eval_evaluate(PnUnOpP0(Pnode)) >> 8) & 0xff;
          /* Set 7th bit if in absolute mode and PROC_CLASS_PIC14E or PROC_CLASS_PIC14EX and
           * address relative mode is handled by the linker. */
          if ((state.mode == MODE_ABSOLUTE) && (IS_PIC14E_CORE || IS_PIC14EX_CORE) &&
              _is_program_segment(PnUnOpP0(Pnode))) {
            val |= PIC14E_FSRxH_FLASH_SEL;
          }

          return val;
          break;
        }

        case LOW:
          return (eval_evaluate(PnUnOpP0(Pnode)) & 0xff);
          break;

        case INCREMENT:
          return (eval_evaluate(PnUnOpP0(Pnode)) + 1);
          break;

        case DECREMENT:
          return (eval_evaluate(PnUnOpP0(Pnode)) - 1);
          break;

        default:
          assert(0);
      }
      break;
    }

    case PTAG_BINOP: {
      p0 = eval_evaluate(PnBinOpP0(Pnode));
      p1 = eval_evaluate(PnBinOpP1(Pnode));
      switch (PnBinOpOp(Pnode)) {
        case '+':
          return (p0 + p1);
          break;

        case '-':
          return (p0 - p1);
          break;

        case '*':
          return (p0 * p1);
          break;

        case '/': {
          if (p1 == 0){
            gpmsg_verror(GPE_DIVBY0, NULL);
            return 0;
          }
          else {
            return (p0 / p1);
          }

          break;
        }

        case '%': {
          if (p1 == 0){
            gpmsg_verror(GPE_DIVBY0, NULL);
            return 0;
          }
          else {
            return (p0 % p1);
          }
          break;
        }

        case '&':
          return (p0 & p1);
          break;

        case '|':
          return (p0 | p1);
          break;

        case '^':
          return (p0 ^ p1);
          break;

        case LSH: {
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

          break;
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
          break;

        case EQUAL:
          return (p0 == p1);
          break;

        case '<':
          return (p0 < p1);
          break;

        case '>':
          return (p0 > p1);
          break;

        case NOT_EQUAL:
          return (p0 != p1);
          break;

        case GREATER_EQUAL:
          return (p0 >= p1);
          break;

        case LESS_EQUAL:
          return (p0 <= p1);
          break;

        case LOGICAL_AND:
          return (p0 && p1);
          break;

        case LOGICAL_OR:
          return (p0 || p1);
          break;

        case '=':
          gpmsg_verror(GPE_BADCHAR, NULL, '=');
          return 0;
          break;

        default:
          assert(0); /* Unhandled binary operator. */
      }

      break;
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
eval_maybe_evaluate(const pnode_t *Pnode)
{
  if ((Pnode != NULL) && eval_can_evaluate(Pnode)) {
    return eval_evaluate(Pnode);
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

/* Count the number of relocatable addesses in the expression. */

int
eval_count_reloc(const pnode_t *Pnode)
{
  const symbol_t   *sym;
  const variable_t *var;
  const char       *name;

  if (state.mode == MODE_ABSOLUTE) {
    return 0;
  }

  switch (Pnode->tag) {
    case PTAG_CONSTANT:
      return 0;
      break;

    case PTAG_OFFSET:
      return eval_count_reloc(PnOffset(Pnode));
      break;

    case PTAG_SYMBOL: {
      name = PnSymbol(Pnode);

      if (strcmp(name, "$") == 0) {
        return 1;
      }

      sym = gp_sym_get_symbol(state.stTop, name);

      if (sym != NULL) {
        var = gp_sym_get_symbol_annotation(sym);

        if (var != NULL) {
          if (FlagIsSet(var->flags, VATRR_HAS_NO_VALUE)) {
            msg_has_no_value(NULL, name);
          }

          switch (var->type) {
            case VAL_EXTERNAL:
            case VAL_GLOBAL:
            case VAL_STATIC:
            case VAL_ADDRESS:
              return 1;

            default:
              return 0;
          }
        }
      }

      return 0;
      break;
    }

    case PTAG_UNOP:
      return eval_count_reloc(PnUnOpP0(Pnode));
      break;

    case PTAG_BINOP:
      return (eval_count_reloc(PnBinOpP0(Pnode)) + eval_count_reloc(PnBinOpP1(Pnode)));
      break;

    default:
      assert(0);
      break;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

/* When generating object files, operands with relocatable addresses can only be
   [UPPER|HIGH|LOW]([<relocatable symbol>] + [<offs>]) */

static gpasmVal
_add_reloc(const pnode_t *Pnode, int16_t Offset, uint16_t Type, gp_boolean Add_coff)
{
  const symbol_t     *sym;
  const variable_t   *var;
  const char         *name;
  char                buffer[BUFSIZ];
  int                 digits;
  unsigned int        org;
  enum gpasmValTypes  type;

  switch (Pnode->tag) {
    case PTAG_OFFSET:
      return _add_reloc(PnOffset(Pnode), Offset, Type, Add_coff);
      break;

    case PTAG_SYMBOL: {
      name = PnSymbol(Pnode);

      if (strcmp(name, "$") == 0) {
        if (IS_RAM_ORG) {
          digits = state.device.class->word_digits;
          org    = state.byte_addr;
          type   = VAL_STATIC;
        }
        else {
          digits = state.device.class->addr_digits;
          org    = gp_processor_insn_from_byte_p(state.processor, state.byte_addr);
          type   = VAL_ADDRESS;
        }

        snprintf(buffer, sizeof(buffer), "_%s_%0*X", state.obj.new_sect_name, digits, org);
        /* RELOCT_ACCESS has always also RELOCT_F, which has already created this symbol. */
        if (Type != RELOCT_ACCESS) {
          set_global(buffer, org, type, false, false);
        }

        sym = gp_sym_get_symbol(state.stTop, buffer);
      }
      else {
        sym = gp_sym_get_symbol(state.stTop, name);
      }

      if (sym != NULL) {
        var = gp_sym_get_symbol_annotation(sym);

        if (var != NULL) {
          if (FlagIsSet(var->flags, VATRR_HAS_NO_VALUE)) {
            msg_has_no_value(NULL, name);
          }

          switch (var->type) {
            case VAL_EXTERNAL:
            case VAL_GLOBAL:
            case VAL_STATIC:
            case VAL_ADDRESS: {
              if (Add_coff) {
                coff_add_reloc(var->coff_symbol_num, Offset, Type);
              }
              return ((var->coff_section_flags & STYP_ABS) ? var->value : -1);
              break;
            }

            default:
              return -1;
          }
        }
      }

      return -1;
      break;
    }

    case PTAG_UNOP: {
      switch (PnUnOpOp(Pnode)) {
        case UPPER:
          return _add_reloc(PnUnOpP0(Pnode), Offset, RELOCT_UPPER, Add_coff);

        case HIGH:
          return _add_reloc(PnUnOpP0(Pnode), Offset, RELOCT_HIGH, Add_coff);

        case LOW:
          return _add_reloc(PnUnOpP0(Pnode), Offset, RELOCT_LOW, Add_coff);

        case '!':
        case '+':
        case '-':
        case '~':
        case INCREMENT:
        case DECREMENT:
          gpmsg_verror(GPE_UNRESOLVABLE, NULL);
          return -1;

        default:
          assert(0);
      }

      break;
    }

    case PTAG_BINOP: {
      switch (PnBinOpOp(Pnode)) {
        case '+': {
          /* The symbol can be in either position. */
          if (eval_count_reloc(PnBinOpP0(Pnode)) == 1) {
            return _add_reloc(PnBinOpP0(Pnode), Offset + eval_maybe_evaluate(PnBinOpP1(Pnode)), Type, Add_coff);
          }
          else {
            return _add_reloc(PnBinOpP1(Pnode), Offset + eval_maybe_evaluate(PnBinOpP0(Pnode)), Type, Add_coff);
          }

          break;
        }

        case '-': {
          /* The symbol has to be first. */
          if (eval_count_reloc(PnBinOpP0(Pnode)) == 1) {
            return _add_reloc(PnBinOpP0(Pnode), Offset - eval_maybe_evaluate(PnBinOpP1(Pnode)), Type, Add_coff);
          }
          else {
            gpmsg_verror(GPE_UNRESOLVABLE, NULL);
            return -1;
          }

          break;
        }

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
          gpmsg_verror(GPE_UNRESOLVABLE, NULL);
          return -1;
          break;

        default:
          assert(0); /* Unhandled binary operator. */
      }

      return -1;
      break;
    }

    case PTAG_CONSTANT:
    default:
      assert(0);
      break;
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Determine if the expression is the difference between two symbols in
   the same section. If so, calculate the offset and don't generate a relocation.

   [UPPER|HIGH|LOW]([<relocatable symbol>] - [<relocatable symbol>])
*/

static gp_boolean
_same_section(const pnode_t *Pnode)
{
  const pnode_t    *p0;
  const pnode_t    *p1;
  const symbol_t   *sym0;
  const symbol_t   *sym1;
  const variable_t *var0;
  const variable_t *var1;
  const char       *name0;
  const char       *name1;
  unsigned int      section_num0;
  unsigned int      section_num1;

  if (!state.obj.enabled) {
    return false;
  }

  if (PnIsUnOp(Pnode) &&
      ((PnUnOpOp(Pnode) == UPPER) || (PnUnOpOp(Pnode) == HIGH) || (PnUnOpOp(Pnode) == LOW))) {
    Pnode = PnUnOpP0(Pnode);
  }

  if (!PnIsBinOp(Pnode) || (PnBinOpOp(Pnode) != '-') || (eval_count_reloc(PnBinOpP0(Pnode)) != 1)) {
    return false;
  }

  p0 = PnBinOpP0(Pnode);
  p1 = PnBinOpP1(Pnode);

  if (!PnIsSymbol(p0) || !PnIsSymbol(p1)) {
    return false;
  }

  name0 = PnSymbol(p0);
  name1 = PnSymbol(p1);

  if (strcmp(name0, "$") == 0) {
    section_num0 = state.obj.section_num;
  }
  else {
    sym0 = gp_sym_get_symbol(state.stTop, name0);
    assert(sym0 != NULL);

    var0 = gp_sym_get_symbol_annotation(sym0);
    section_num0 = var0->coff_section_num;
  }

  if (strcmp(name1, "$") == 0) {
    section_num1 = state.obj.section_num;
  }
  else {
    sym1 = gp_sym_get_symbol(state.stTop, name1);
    assert(sym1 != NULL);

    var1 = gp_sym_get_symbol_annotation(sym1);
    section_num1 = var1->coff_section_num;
  }

  /* They must come from the same section. Debug symbols are not placed
     in the global symbol table, so don't worry about symbol type.
     Fail if sections are not known (== 0) or not the same. */
  if ((section_num0 == 0) || (section_num1 == 0) || (section_num0 != section_num1)) {
    return false;
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

gpasmVal
eval_reloc_evaluate(const pnode_t *Pnode, uint16_t Type, gp_boolean *Is_reloc, gpasmVal *Reloc_value,
                    gp_boolean Add_coff)
{
  int      count;
  gpasmVal value;

  if (Is_reloc != NULL) {
    *Is_reloc = false;
  }

  if (Reloc_value != NULL) {
    *Reloc_value = -1;
  }

  if (state.mode == MODE_ABSOLUTE) {
    return eval_maybe_evaluate(Pnode);
  }

  count = eval_count_reloc(Pnode);
  if (count == 0) {
    /* no relocatable addresses */
    return eval_maybe_evaluate(Pnode);
  }

  if (count > 1) {
    if ((count == 2) && (_same_section(Pnode))) {
      /* It is valid to take the difference between two symbols in the same
         section. Evaluate, but don't add a relocation. */
      return eval_maybe_evaluate(Pnode);
    }

    /* too many relocatable addresses */
    gpmsg_verror(GPE_UNRESOLVABLE, NULL);
    return 0;
  }

  /* add the coff relocation */
  value = _add_reloc(Pnode, 0, Type, Add_coff);

  if (Is_reloc != NULL) {
    *Is_reloc = true;
  }

  if (Reloc_value != NULL) {
    *Reloc_value = value;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

/* Evaluate the number of passes for the "fill" directive. */

int
eval_fill_number(const pnode_t *Pnode)
{
  int number;

  number = eval_maybe_evaluate(Pnode);

  if ((state.device.class->rom_width == 8) && ((number & 0x1) == 1)) {
    gpmsg_verror(GPE_FILL_ODD, NULL);
  }

  return (gp_processor_byte_from_insn_c(state.device.class, number) >> 1);
}

/* optimize tree
   Copyright (C) 2003, 2004, 2005
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
#include "gpal.h"
#include "analyze.h"

/* prototypes */
static tree *opt_expr(tree *expr);

/* Determine the relative complexity of the nodes for tree shaping.  The
   numbers are arbitary, but are related to the number of temporary 
   registers required.  */

static int
node_complexity(tree *node)
{
  int complexity = 0;

  switch(node->tag) {
  case tag_attrib:
    if ((ATTRIB_TYPE(node) == attrib_access) ||
        (ATTRIB_TYPE(node) == attrib_address)) {
      complexity = 2;    
    } else { 
      complexity = 1;
    }
    break;
  case tag_call:
    complexity = 2;    
    break;
  case tag_constant:
    complexity = 1;
    break;
  case tag_symbol:
    if (SYM_OFST(node)) {
      /* it is an array */
      complexity = 3;
    } else {
      complexity = 2;    
    }
    break;
  case tag_unop:
    complexity = 1 + node_complexity(node->value.unop.p0);
    break;
  case tag_binop:
    complexity = 2 + 
                 node_complexity(node->value.binop.p0) +
                 node_complexity(node->value.binop.p1);
    break;
  default:
    assert(0);
  }

  return complexity;
}

/* left and right can be switched */

static int
switchable(enum node_op op)
{
  int can_shape = 0;

  switch (op) {
  case op_add:
  case op_and:
  case op_or:
  case op_xor:
  case op_eq:
  case op_ne:
  case op_mult:
    can_shape = 1;
    break;
  default:
    can_shape = 0;
  }

  return can_shape;
}

static gp_boolean
is_constant(tree *p)
{

  if (p->tag == tag_constant) {
    return true;
  } else {
    return false;
  }

}

static gp_boolean
is_symbol(tree *p)
{

  if (p->tag == tag_symbol) {
    return true;
  } else {
    return false;
  }

}

static gp_boolean
is_binop(tree *p)
{

  if (p->tag == tag_binop) {
    return true;
  } else {
    return false;
  }

}

static gp_boolean
symbols_equal(tree *first, tree *second)
{

  if (SYM_OFST(first) || SYM_OFST(second)) {
    return false;
  }

  if (strcasecmp(SYM_NAME(first), SYM_NAME(second)) == 0)
    return true;


  return false;
} 

static tree *
optimize_unop(tree *expr)
{

  /* optimize the arguent */
  UNOP_ARG(expr) = opt_expr(UNOP_ARG(expr));

  switch (UNOP_OP(expr)) {
  case op_add:
    /* remove the unop */
    expr = UNOP_ARG(expr);
    break;
  case op_com:
  case op_not:
  case op_neg:
    if (is_constant(UNOP_ARG(expr))) {
      expr = mk_constant(evaluate(expr));
    }
    break;
  default:
    assert(0);
  }

  return expr;
}

static tree *
optimize_binop(tree *expr)
{
  tree *temp;
  gp_boolean left_constant;
  gp_boolean right_constant;
  gp_boolean left_symbol;
  gp_boolean right_symbol;
  gp_boolean left_binop;
  gp_boolean right_binop;
  int value;

  /* optimize the left and right */
  BINOP_LEFT(expr) = opt_expr(BINOP_LEFT(expr));
  BINOP_RIGHT(expr) = opt_expr(BINOP_RIGHT(expr));

  left_constant = is_constant(BINOP_LEFT(expr));
  right_constant = is_constant(BINOP_RIGHT(expr));

  left_symbol = is_symbol(BINOP_LEFT(expr));
  right_symbol = is_symbol(BINOP_RIGHT(expr));

  left_binop = is_binop(BINOP_LEFT(expr));
  right_binop = is_binop(BINOP_RIGHT(expr));

  /* replace the operators that the code generator doesn't support */
  if ((BINOP_OP(expr) == op_gt) || (BINOP_OP(expr) == op_gte)) {
    if (BINOP_OP(expr) == op_gt) {
      BINOP_OP(expr) = op_lt;
    } else {
      BINOP_OP(expr) = op_lte;
    }
    temp = BINOP_RIGHT(expr);
    BINOP_RIGHT(expr) = BINOP_LEFT(expr);
    BINOP_LEFT(expr) = temp;
    return expr;
  }

  /* evaluate constant expressions */
  if (state.optimize.constant_folding) {

    if (left_constant && right_constant) {
      return mk_constant(evaluate(expr));
    }

  }

  /* replace trival expressions */
  if (state.optimize.trival_expressions) {

    /* symbol * 2 ==> symbol << 2 */
    if ((BINOP_OP(expr) == op_mult) &&
        (left_constant || right_constant)) {
      int shift = 0;
      
      if (left_constant) {
        value = evaluate(BINOP_LEFT(expr));
        temp = BINOP_RIGHT(expr);
      } else {
        value = evaluate(BINOP_RIGHT(expr));
        temp = BINOP_LEFT(expr);      
      }

      switch (value) {
      case 0:
        /* anything multiplied by 0 */
        return mk_constant(0);
        break;
      case 1:
        /* anything multiplied by 1 */
        return temp;
        break;
      case 2:
        shift = 1;
        break;
      case 4:
        shift = 2;
        break;
      case 8:
        shift = 3;
        break;
      case 16:
        shift = 4;
        break;
      case 32:
        shift = 5;
        break;
      case 64:
        shift = 6;
        break;
      case 128:
        shift = 7;
        break;      
      }

      if (shift) {
        return mk_binop(op_lsh, temp, mk_constant(shift));
      }
    }

    /* symbol / 2 ==> symbol >> 2 */
    if ((BINOP_OP(expr) == op_div) && right_constant) {
      int shift = 0;
      
      value = evaluate(BINOP_RIGHT(expr));

      switch (value) {
      case 0:
        analyze_error(expr, "divide by zero");
        break;
      case 1:
        /* anything divided by 1 */
        return BINOP_LEFT(expr);
        break;
      case 2:
        shift = 1;
        break;
      case 4:
        shift = 2;
        break;
      case 8:
        shift = 3;
        break;
      case 16:
        shift = 4;
        break;
      case 32:
        shift = 5;
        break;
      case 64:
        shift = 6;
        break;
      case 128:
        shift = 7;
        break;      
      }

      if (shift) {
        return mk_binop(op_rsh, BINOP_LEFT(expr), mk_constant(shift));
      }
    }    

    /* symbol + 0 ==> symbol */
    if ((BINOP_OP(expr) == op_add) &&
        (left_constant || right_constant)) {

      if (left_constant) {
        value = evaluate(BINOP_LEFT(expr));
        temp = BINOP_RIGHT(expr);
      } else {
        value = evaluate(BINOP_RIGHT(expr));
        temp = BINOP_LEFT(expr);      
      }
    
      if (value == 0) {
        return temp;
      }
    
    }

    /* symbol - symbol ==> 0 */
    if ((BINOP_OP(expr) == op_sub) && left_symbol && right_symbol) {
      if (symbols_equal(BINOP_LEFT(expr), BINOP_RIGHT(expr))) {
        return mk_constant(0);
      }
    }

  }

  /* reshape the tree to generate better code */
  if (state.optimize.tree_shape) {
   
    if (switchable(BINOP_OP(expr))) {
      tree *left;
      tree *right;
      gp_boolean shape_modified = false;      
      int left_complexity;
      int right_complexity;

      left = BINOP_LEFT(expr);
      right = BINOP_RIGHT(expr);

      /* Replace a trinop containing two constants with one binop.  Three
         constants should have been replaced in eariler optimizations. */ 
      if ((left_binop) && 
          (right_constant) && 
          (BINOP_OP(left) == BINOP_OP(expr))) {
        
        if (is_constant(BINOP_LEFT(left))) {
          temp = BINOP_RIGHT(left);
          BINOP_RIGHT(left) = BINOP_RIGHT(expr);
          BINOP_RIGHT(expr) = temp;
          shape_modified = true;
        } else if (is_constant(BINOP_RIGHT(left))) {
          temp = BINOP_LEFT(left);
          BINOP_LEFT(left) = BINOP_RIGHT(expr);
          BINOP_RIGHT(expr) = temp;
          shape_modified = true;
        }
      }

      /* Do the same for the other side */      
      if ((left_constant) && 
          (right_binop) &&
          (BINOP_OP(right) == BINOP_OP(expr))) {
        if (is_constant(BINOP_LEFT(right))) {
          temp = BINOP_RIGHT(right);
          BINOP_RIGHT(right) = BINOP_RIGHT(expr);
          BINOP_RIGHT(expr) = temp;
          shape_modified = true;
        } else if (is_constant(BINOP_RIGHT(right))) {
          temp = BINOP_LEFT(right);
          BINOP_LEFT(right) = BINOP_RIGHT(expr);
          BINOP_RIGHT(expr) = temp;
          shape_modified = true;
        }   
      } 
      
      /* Replace a quadnop containing two constants with one trinop. */ 
      if ((left_binop) &&
          (right_binop) &&
          (BINOP_OP(left) == BINOP_OP(expr)) &&
          (BINOP_OP(right) == BINOP_OP(expr))) {
        gp_boolean left_left_constant = is_constant(BINOP_LEFT(left));
        gp_boolean left_right_constant = is_constant(BINOP_RIGHT(left));
        gp_boolean right_left_constant = is_constant(BINOP_LEFT(right));
        gp_boolean right_right_constant = is_constant(BINOP_RIGHT(right));
        
        if (left_left_constant && right_left_constant) {
          temp = BINOP_RIGHT(left);
          BINOP_RIGHT(left) = BINOP_LEFT(right);
          BINOP_LEFT(right) = temp;
          shape_modified = true;
        } else if (left_left_constant && right_right_constant) {
          temp = BINOP_RIGHT(left);
          BINOP_RIGHT(left) = BINOP_RIGHT(right);
          BINOP_RIGHT(right) = temp;
          shape_modified = true;
        } else if (left_right_constant && right_left_constant) {
          temp = BINOP_LEFT(left);
          BINOP_LEFT(left) = BINOP_LEFT(right);
          BINOP_LEFT(right) = temp;
          shape_modified = true;
        } else if (left_right_constant && right_right_constant) {
          temp = BINOP_LEFT(left);
          BINOP_LEFT(left) = BINOP_RIGHT(right);
          BINOP_RIGHT(right) = temp;
          shape_modified = true;
        }        

      }

      if ((shape_modified) && (state.optimize.constant_folding)) {
        /* Take another pass to combine the constants. */
        return opt_expr(expr);
      }
 
      /* Because of how subtract works (memory - w) right is calculated
         first, so keep the complex expressions on the right. */

      left_complexity = node_complexity(BINOP_LEFT(expr));
      right_complexity = node_complexity(BINOP_RIGHT(expr));

      if (left_complexity > right_complexity) {
        temp = BINOP_RIGHT(expr);
        BINOP_RIGHT(expr) = BINOP_LEFT(expr);
        BINOP_LEFT(expr) = temp;
        return expr;
      }
    }

  }

  return expr;
}

static tree *
opt_expr(tree *expr)
{
  struct variable *var;

  switch(expr->tag) {
  case tag_attrib:
    if ((ATTRIB_TYPE(expr) != attrib_access) &&
        (ATTRIB_TYPE(expr) != attrib_address))  {
      /* convert all attribs into constants if possible */
      expr = mk_constant(maybe_evaluate(expr));
    }   
    break;
  case tag_call:
  case tag_constant:
    /* do nothing */
    break;
  case tag_symbol:
    var = get_global(SYM_NAME(expr));
    if (var->tag == sym_const) {
      /* remove all symbols that are constant */
      expr = mk_constant(var->value);
    } else if (var->tag == sym_alias) {
      expr = opt_expr(var->node);
    }
    break;
  case tag_unop:
    expr = optimize_unop(expr);
    break;
  case tag_binop:
    expr = optimize_binop(expr);
    break;
  default:
    assert(0);
  }

  return expr;
}

/* optimize the expression tree */ 

tree *
optimize_expr(tree *expr)
{

  if (!gp_debug_disable) {
    gp_debug("====================================================");
    gp_debug("original:");
    print_node(expr, 0, false);
  }

  expr = opt_expr(expr);

  if (!gp_debug_disable) {
    gp_debug("optimized:");
    print_node(expr, 0, false);
  }

  if (state.optimize.second_pass) {
    expr = opt_expr(expr);
    if (!gp_debug_disable) {
      gp_debug("optimized (second pass):");
      print_node(expr, 0, false);
    }  
  }

  return expr;
}

/* Test if the expression can be expressed as an unop */ 

tree *
optimize_unop_expr(struct variable *dest, tree *left, tree *right)
{
  tree *binop_left;
  tree *binop_right;
  tree *unop = NULL;
  struct variable *arg;

  if ((right->tag == tag_constant) && (right->value.constant == 0)) {
    /* i = 0 */
    unop = mk_unop(op_clr, left);
  } else if (right->tag == tag_binop) {
     binop_left = BINOP_LEFT(right);
     binop_right = BINOP_RIGHT(right);
     
     if (BINOP_OP(right) == op_add) {
       if ((binop_left->tag == tag_constant) && 
           (binop_left->value.constant == 1) &&
           (binop_right->tag == tag_symbol)) {
         /* i = 1 + i ==> i++ */
         arg = get_global(SYM_NAME(binop_right));
         if (arg == dest) {
           unop = mk_unop(op_inc, left);
         }
       } else if ((binop_right->tag == tag_constant) && 
                  (binop_right->value.constant == 1) &&
                  (binop_left->tag == tag_symbol)) {
         /* i = i + 1 ==> i++ */
         arg = get_global(SYM_NAME(binop_left));
         if (arg == dest) {
           unop = mk_unop(op_inc, left);
         }
       }
     } else if (BINOP_OP(right) == op_sub) {
       /* i = i - 1 ==> i-- */
       if ((binop_right->tag == tag_constant) && 
           (binop_right->value.constant == 1) &&
           (binop_left->tag == tag_symbol)) {
         arg = get_global(SYM_NAME(binop_left));
         if (arg == dest) {
           unop = mk_unop(op_dec, left);
         }
       }
     }
   }

  /* i = i | 0x1 ==> bitset(i, 0) */

  /* i = i & 0xfe ==> bitclear(i, 0) */

  if ((!gp_debug_disable) && (unop != NULL)) {
    gp_debug("====================================================");
    gp_debug("original binop:");
    print_node(left, 0, false);
    print_node(right, 0, false);
    gp_debug("optimized unop:");
    print_node(unop, 0, false);
  }
  
  return unop;
}

/* analyze semantics
   Copyright (C) 2003, 2004
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
#include "codegen.h"
#include "deps.h"
#include "optimize.h"
#include "scan.h"

/* prototypes */
void analyze_statements(tree *statement);
static void analyze_elements(tree *current,
                             char *root_name,
                             enum node_storage data_storage,
                             enum node_storage prog_storage,
                             gp_boolean add_alias);

/* function return data */
struct variable *subprogram_var;
static gp_boolean generating_function;
static struct variable *return_var;
static gp_boolean found_return;
static enum size_tag return_size;

/* test for default expression size */ 
static enum size_tag scan_size;

void 
analyze_error(tree *node, const char *format, ...)
{
  va_list args;
  char buffer[BUFSIZ]; 

  gp_num_errors++;

  if (gp_quiet)
    return;

  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  if ((node) && (node->file_id)) {
    printf("%s:%d: error: %s\n",
           get_file_name(node->file_id),
           node->line_number,
           buffer);
  } else {
    printf("error %s\n", buffer);
  }

  return;
}

void 
analyze_warning(tree *node, const char *format, ...)
{
  va_list args;
  char buffer[BUFSIZ]; 

  gp_num_warnings++;

  if (gp_quiet)
    return;

  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  if ((node) && (node->file_id)) {
    printf("%s:%d: warning: %s\n",
           get_file_name(node->file_id),
           node->line_number,
           buffer);
  } else {
    printf("warning %s\n", buffer);
  }


  return;
}

static
int list_length(tree *L)
{
  if (L == NULL) {
    return 0;
  } else {
    return 1 + list_length(L->next);
  }
}

/* can the expression be evaluated at compile time */

gp_boolean
can_evaluate(tree *p, gp_boolean gen_errors)
{
  struct variable *var;

  switch (p->tag) {
  case tag_attrib:
    var = get_global(ATTRIB_NAME(p));
    if (is_data(var)) {
      switch (ATTRIB_TYPE(p)) {
      case attrib_first:    
      case attrib_last:    
        if (var->type->tag == type_array) {
          return true;
        } else {
          if (gen_errors) {
            analyze_error(p, "symbol %s is not an array", ATTRIB_NAME(p));    
          }
          return false;
        }           
      case attrib_size:    
        return true;
      default:
        return false;
      }
    } else {
      if (gen_errors) {
        analyze_error(p, "symbol %s is not data", ATTRIB_NAME(p));    
      }
      return false;
    }
    return false;
  case tag_call:
    return false;
  case tag_constant:
    return true;
  case tag_symbol:
    var = get_global(SYM_NAME(p));
    if (var) {
      if (var->tag == sym_const) {
        return true;
      } else {
        if (gen_errors) { 
          analyze_error(p, "symbol %s is not a constant", SYM_NAME(p));    
        }
        return false;
      }
    } else {
      if (gen_errors) {
        analyze_error(p, "symbol %s not previously defined", SYM_NAME(p));    
      }
      return false;
    }
  case tag_unop:
    return can_evaluate(p->value.unop.p0, gen_errors);
  case tag_binop:
    return can_evaluate(p->value.binop.p0, gen_errors) && 
           can_evaluate(p->value.binop.p1, gen_errors);
  case tag_string:
    if (gen_errors) {
      analyze_error(p, "illegal argument %s", p->value.string);
    }
    return false;
  default:
    assert(0);
  }

  return 0;
}

int
evaluate(tree *p)
{
  struct variable *var;
  int p0, p1;

  switch (p->tag) {
  case tag_attrib:
    var = get_global(ATTRIB_NAME(p));
    switch (ATTRIB_TYPE(p)) {
    case attrib_first:    
      return var->type->start;
    case attrib_last:    
      return var->type->end;
    case attrib_size:    
      return type_bits(var->type);
    default:
      assert(0);
    }
  case tag_constant:
    return p->value.constant;
  case tag_symbol:
    var = get_global(SYM_NAME(p));
    assert (var != NULL);
    return var->value;
  case tag_unop:
    switch (p->value.unop.op) {
    case op_not:
      return !evaluate(p->value.unop.p0);
    case op_add:
      return  evaluate(p->value.unop.p0);
    case op_neg:
      return -evaluate(p->value.unop.p0);
    case op_com:
      return ~evaluate(p->value.unop.p0);
    default:
      assert(0);
    }
  case tag_binop:
    p0 = evaluate(p->value.binop.p0);
    p1 = evaluate(p->value.binop.p1);
    switch (p->value.binop.op) {
    case op_add:   return p0 + p1;
    case op_sub:   return p0 - p1;
    case op_mult:  return p0 * p1;
    case op_div:
      if (p1 == 0){
        analyze_error(p->value.binop.p1, "divide by 0");
        return 0;
      } else {
        return p0 / p1;
      }
    case op_mod:   return p0 % p1;
    case op_and:   return p0 & p1;
    case op_or:    return p0 | p1;
    case op_xor:   return p0 ^ p1;
    case op_lsh:   return p0 << p1;
    case op_rsh:   return p0 >> p1;
    case op_eq:    return p0 == p1;
    case op_lt:    return p0 < p1;
    case op_gt:    return p0 > p1;
    case op_ne:    return p0 != p1;
    case op_gte:   return p0 >= p1;
    case op_lte:   return p0 <= p1;
    case op_land:  return p0 && p1;
    case op_lor:   return p0 || p1;
    default:
      assert(0); /* Unhandled binary operator */
    }
  default:
    assert(0); /* Unhandled parse node tag */
  }

  return (0); /* Should never reach here */
}

int
maybe_evaluate(tree *p)
{
  int r;

  if (p && can_evaluate(p, true)) {
    r = evaluate(p);
  } else {
    r = 0;
  }

  return r;
}

static gp_boolean
test_symbol(tree *node, char *name, enum size_tag size)
{
  struct variable *var;
  enum size_tag arg_size = size_unknown;

  var = get_global(name);
  if (var == NULL) {
    analyze_error(node, "unknown symbol %s", name);
    return true;
  }

  var->is_used = true;
  
  assert(var->type != NULL);
  
  if (node->tag == tag_attrib) {
    switch (ATTRIB_TYPE(node)) {
    case attrib_address:    
      arg_size = target->data_ptr_size;
      break;
    case attrib_first:
    case attrib_last:
    case attrib_size:
      arg_size = size;
      break;
    default:
      assert(0);
    }
  } else {
    arg_size = prim_size(var->type);
  }

  /* If it is not an unchecked conversion, verify the sizes of the types match */
  if ((size != size_unknown) &&
      (var->tag != sym_const) &&
      (arg_size != size)) {
    analyze_error(node, "type mismatch in symbol %s", name);
    return true;
  }

  if ((node->tag != tag_attrib) && 
      ((var->tag == sym_idata) || (var->tag == sym_udata))) {

    if ((var->is_assigned == false) && (var->storage == storage_local)) {
      analyze_warning(node, "%s might be used uninitialized", name);
    }

    /* Save the size for scanning expressions with an unknown type */
    if ((size == size_unknown) && (var->type)) {     
      enum size_tag temp_size = prim_size(var->type);

      if (temp_size != size_unknown) {
        scan_size = temp_size;
      }
    }

  }

  return false;
}

/* scan the tree and make sure all of the symbols are valid */

static gp_boolean
scan_tree(tree *expr, enum size_tag size)
{

  if (expr == NULL)
    return true;

  switch (expr->tag) {
  case tag_arg:
    return test_symbol(expr, ARG_NAME(expr), size);
  case tag_attrib:
    return test_symbol(expr, ATTRIB_NAME(expr), size);
  case tag_binop:
    return scan_tree(BINOP_LEFT(expr), size) ||
           scan_tree(BINOP_RIGHT(expr), size);
  case tag_call:
    return test_symbol(expr, CALL_NAME(expr), size);
  case tag_constant:
    return false;
  case tag_unop:
    return scan_tree(UNOP_ARG(expr), size);
  case tag_symbol:
    return test_symbol(expr, SYM_NAME(expr), size);
  default:
    assert(0);
  }

  return 1;
}

/* scan the test and make sure the expression returns a boolean */

static gp_boolean
scan_test(tree *p)
{

  if (p == NULL)
    return true;

  switch (p->tag) {
  case tag_arg:
    return true;
  case tag_constant:
    return true;
  case tag_symbol:
    return true;
  case tag_unop:
    if (p->value.unop.op == op_not) {
      return scan_test(p->value.unop.p0);
    } else {
      return true;
    }
  case tag_binop:
    switch (p->value.binop.op) {
    case op_eq:
    case op_lt:
    case op_gt:
    case op_ne:
    case op_gte:
    case op_lte:
      return false;
    case op_land:
    case op_lor:
      return scan_test(p->value.binop.p0) && 
             scan_test(p->value.binop.p1);
    default:
      return true;
    }
  default:
    assert(0); /* Unhandled parse node tag */
  }

  return true;
}

typedef void analyze_func_type(tree *file);
#define ANALYZE_FUNC (*(analyze_func_type*)func_ptr)

static void
analyze_preprocess(tree *cond, long int func_ptr)
{

  if (COND_TEST(cond)) {
    if (maybe_evaluate(COND_TEST(cond))) {
      /* the test condition is true, process the body and return */
      ANALYZE_FUNC(COND_BODY(cond));
    } else if (COND_NEXT(cond)) { 
      /* the test condition is false, process the next clause and return */
      analyze_preprocess(COND_NEXT(cond), func_ptr);
    }
  } else {
    /* there is no test, else equivalent, process the body */
    ANALYZE_FUNC(COND_BODY(cond));
  }
  
  return;
}

int
analyze_check_array(tree *symbol, struct variable *var)
{
  int offset;

  offset = evaluate(SYM_OFST(symbol));
  if ((offset < var->type->start) || (offset > var->type->end)) {
    analyze_error(symbol, "array index %i is out of range", offset);
  }

  /* In memory, the arrays always start at 0. */
  offset = offset - var->type->start;

  return offset;
}

static tree *
arg_to_symbol(char *proc_name, tree *arg)
{
  char *symbol_name;
  tree *symbol;

  symbol_name = mangle_name2(proc_name, ARG_NAME(arg));
  symbol = mk_symbol(symbol_name, NULL);
  COPY_DEBUG(arg, symbol);

  return symbol;
}

void
analyze_call(tree *call, gp_boolean in_expr, enum size_tag codegen_size)
{
  struct variable *var;
  tree *def;
  tree *head;
  tree *def_args;
  tree *call_args;
  tree *assignment;
  char *return_name;
  struct variable *return_var;
  enum size_tag call_size;
  gp_boolean load_result = false;

  var = get_global(CALL_NAME(call));
  if (var) {
    def = var->node;
    if (def->tag == tag_subprogram) {
      head = SUB_HEAD(def);
      if (SUB_RET(def)) {
        load_result = true;
        call_size = prim_size(get_type(SUB_RET(def)));
        if (!in_expr) {
          analyze_error(call, "functions can only be called in expressions");
          return;
        }
        if (codegen_size != call_size) {
          analyze_error(call, 
                        "mismatch between %s return type and current expression",
                        CALL_NAME(call));
          return;      
        }
      } else if (in_expr) {
        analyze_error(call, "procedures can not be called in expressions");
        return;
      }
    } else {
      analyze_error(call, "name in the call is not a subprogram");
      return;
    }
  } else {
    analyze_error(call, "unknown symbol %s", CALL_NAME(call));
    return;
  }

  call_args = CALL_ARGS(call);
  def_args = HEAD_ARGS(head);

  if (list_length(call_args) != list_length(def_args)) {
    analyze_error(call, "incorrect number of arguments in call");
    return;
  }

  codegen_line_number(call);

  /* write data into the in/inout of the function or procedure */
  while (call_args) {
    assert(def_args->tag == tag_arg);

    if ((ARG_DIR(def_args) == dir_in) ||
        (ARG_DIR(def_args) == dir_inout)) {
      assignment = mk_binop(op_assign,
                            arg_to_symbol(var->name, def_args),
                            call_args);
      COPY_DEBUG(call_args, assignment);
      analyze_expr(assignment);
    }

    def_args = def_args->next;
    call_args = call_args->next;
  }

  codegen_call(var->name, var->storage);

  /* read data from the inout/out of the function or procedure */
  call_args = CALL_ARGS(call);
  def_args = HEAD_ARGS(head);
  while (call_args) {
    assert(def_args->tag == tag_arg);

    if ((ARG_DIR(def_args) == dir_inout) ||
        (ARG_DIR(def_args) == dir_out)) {
      if (call_args->tag == tag_symbol) {
        assignment = mk_binop(op_assign,
                              call_args,
                              arg_to_symbol(var->name, def_args));
        COPY_DEBUG(call_args, assignment);
        analyze_expr(assignment);
      } else {
        analyze_error(call_args, "call argument must be a symbol");
      }
    }

    def_args = def_args->next;
    call_args = call_args->next;
  }

  /* put the result in the working register */
  if (load_result) {
    tree *symbol;

    return_name = mangle_name2(var->name, "return");

    return_var = get_global(return_name);
    assert(return_var != NULL);
    return_var->is_used = true;
 
    symbol = mk_symbol(return_name, NULL);
    COPY_DEBUG(call, symbol);
    codegen_expr(symbol, codegen_size);
    free(return_name);
  }

  /* mark the procedure or function as used */
  var->is_used = true;

  return;
}

static int
analyze_test(tree *test, char *end_label)
{
  enum size_tag size;

  scan_size = size_unknown;

  /* Scan to check for valid symbols and to determine what the default
     size is. */
  if (scan_tree(test, size_unknown)) {
    return 1;
  }

  size = scan_size;

  if (size == size_unknown) {
    /* the size couldn't be determined (i. e. 0 == 1), so use the default */
    size = size_uint8;
  }

  /* check the symbol sizes */
  if (scan_tree(test, size)) {
    return 1;
  }

  if (scan_test(test)) {
    analyze_error(test, "expression doesn't return a boolean");
    return 1;
  }

  /* optimize the expression */
  test = optimize_expr(test);
 
  codegen_test(test, end_label, size);

  return 0;
}

void
analyze_cond(tree *cond, char *last_label)
{
  char *end_label = NULL;
  char *local_label = NULL;

  /* conditional with a constant expression */
  if (state.optimize.constant_folding &&
      (COND_TEST(cond)) && 
      (can_evaluate(COND_TEST(cond), false))) {
    if (evaluate(COND_TEST(cond))) {
      analyze_statements(COND_BODY(cond));
    }
    return;
  }

  /* create a label at the end of all the conditional blocks */  
  if (last_label) {
    local_label = last_label;
  } else {
    local_label = codegen_next_label();
  }

  /* else doesn't have a condition */
  if (COND_TEST(cond)) {
    end_label = codegen_next_label();
    if (analyze_test(COND_TEST(cond), end_label))
      return;
  }

  /* write the body of the code */
  analyze_statements(COND_BODY(cond));

  /* executed one clause so jump to the end */
  if (COND_NEXT(cond)) {
    codegen_jump(local_label);
  }

  /* if there is a condition generate a label at the end of the body */
  if (COND_TEST(cond)) {
    codegen_write_label(end_label);
    if (end_label)
      free(end_label);
  }

  /* generate next conditional block, if there is one */
  if (COND_NEXT(cond)) {
    analyze_cond(COND_NEXT(cond), local_label);
  }

  /* this is the top level cond block so write the last label */
  if (last_label == NULL) {
    codegen_write_label(local_label);
    if (local_label)
      free(local_label);
  }

  return;
}

void
analyze_loop(tree *loop)
{
  char *start_label = NULL;
  char *end_label = NULL;
  gp_boolean loop_forever = false;

  analyze_statements(LOOP_INIT(loop));

  /* loop with a constant exit expression */
  if (state.optimize.constant_folding &&
      (LOOP_EXIT(loop)) && 
      (can_evaluate(LOOP_EXIT(loop), false))) {
    if (evaluate(LOOP_EXIT(loop))) {
      loop_forever = true;
    } else {
      return;
    }
  }

  start_label = codegen_next_label();

  /* place the label for looping */
  codegen_write_label(start_label);

  /* write the exit statements */
  if (!(loop_forever) && (LOOP_EXIT(loop))) {
    end_label = codegen_next_label();
    if (analyze_test(LOOP_EXIT(loop), end_label))
      return;
  }

  /* write the loop body */
  analyze_statements(LOOP_BODY(loop));

  /* write the increment statements*/
  analyze_statements(LOOP_INCR(loop));
  
  /* jump to the beginning of the loop */
  codegen_jump(start_label);

  /* place the label for exiting */
  if ((!loop_forever) && (LOOP_EXIT(loop))) {  
    codegen_write_label(end_label);
  }

  if (start_label)
    free(start_label);

  return;
}

void
analyze_expr(tree *expr)
{
  tree *left;
  tree *right;
  gp_boolean access_type = false;
  char *name;
  struct variable *var;
  enum size_tag size = size_unknown;
  gp_boolean constant_offset = true;
  int element_size;
  int offset = 0;
  tree *unop = NULL;

  if ((expr->tag != tag_binop) || (BINOP_OP(expr) != op_assign)) {
    analyze_error(expr, "expression is missing =");
    return;
  }

  left = BINOP_LEFT(expr);
  right = BINOP_RIGHT(expr);

  if ((left->tag == tag_attrib) && (ATTRIB_TYPE(left) == attrib_access)) {
    name = ATTRIB_NAME(left);
    access_type = true;
  } else {
    name = SYM_NAME(left);
  }
 
  if ((left->tag != tag_symbol) && (!access_type)) {
    analyze_error(expr, "invalid lvalue in assignment");
    return;
  }

  var = get_global(name);
  if (var == NULL) {
    analyze_error(left, "unknown symbol %s", name);  
    return;
  }

  /* fetch the symbol's primative type */
  assert(var->type != NULL);
  if (access_type) {
    /* FIXME: allow access to entries in a record. */ 
    if (var->type->tag == type_access) {
      size = prim_size(var->type->access);
    } else {
      analyze_error(left, "symbol %s is not an access type", name);  
    }
  } else {
    size = prim_size(var->type);
  }

  /* Verify all symbols in the expression are in the symbol table and
     that they are the same type as the lvalue. */
  if (scan_tree(right, size))
    return;

  if (access_type) {
    codegen_load_indirect(var);
  } else if (SYM_OFST(left)) {
    element_size = type_bytes(var->type->prim);

    /* calculate the offset for indirect accesses */
    if (var->type->tag == type_array) {
      if (can_evaluate(SYM_OFST(left), false)) {
        /* direct access with an offset */
        constant_offset = true;
        offset = analyze_check_array(left, var) * element_size;
      } else {
        /* indirect access */
        constant_offset = false;
        codegen_calc_indirect(SYM_OFST(left), var, element_size, true);
      }
    } else {
      analyze_error(left, "lvalue %s is not an array", SYM_NAME(left));
      return;
    }
  }

  codegen_line_number(right);

  /* optimize the expression */
  right = optimize_expr(right);

  if (access_type) {
    /* write the expression */
    codegen_expr(right, size);

    /* write indirectly to memory */
    codegen_store(var, true, false, 0, NULL);

    /* mark the symbol as being used */
    var->is_used = true;

  } else {
    unop = optimize_unop_expr(var, left, right);
    if (unop) {
      /* write the simplified expression */
      codegen_unop(var, constant_offset, offset, SYM_OFST(left), unop, size);
    } else {
      /* write the expression */
      codegen_expr(right, size);

      /* put the result in memory */
      codegen_store(var, false, constant_offset, offset, SYM_OFST(left));
    }
  }

  /* mark the symbol as having a valid value */
  var->is_assigned = true;

}

static void
analyze_return(tree *ret)
{
  found_return = true;
 
  if (generating_function) {
    if (scan_tree(ret->value.ret, return_size)) {
      return;
    }
    codegen_line_number(ret);
    ret->value.ret = optimize_expr(ret->value.ret);
    codegen_expr(ret->value.ret, return_size);
    codegen_store(return_var, false, false, 0, NULL);
    return_var->is_assigned = true;
    codegen_write_asm("return");
  } else {
    analyze_error(ret, "returns can only appear in a function body");
  }
}

static void
analyze_assembly(tree *assembly)
{
  codegen_assembly(assembly);

  /* Clear the settings, because user could have changed them. */
  state.current_bank = NULL;
  state.current_ibank = NULL;
  state.current_page = NULL;

}

void
analyze_statements(tree *statement)
{
  struct variable *label_var;

  if (statement == NULL)
    return;

  while(statement) {
    switch(statement->tag) {
    case tag_assembly:
      analyze_assembly(statement);
      break;
    case tag_call:
      analyze_call(statement, false, size_unknown);
      break;
    case tag_cond:
      analyze_cond(statement, NULL);  
      break;
    case tag_goto:
      label_var = get_global(GOTO_NAME(statement));
      if (label_var != NULL) {
        if (label_var->tag == sym_label) {
          codegen_jump(label_var->name);
        } else {
          analyze_error(statement, "%s is not a label", GOTO_NAME(statement));  
        }
      } else {
        analyze_error(statement, "unknown label %s", GOTO_NAME(statement));  
      }
      break;
    case tag_label:
      label_var = get_global(LABEL_NAME(statement));
      if (label_var != NULL) {
        /* errors will be generated in scan_labels() */
        codegen_write_label(label_var->name);
        if (label_var->storage == storage_public) {
          codegen_write_asm("global %s", label_var->name);
        }
      }
      break; 
    case tag_loop:
      analyze_loop(statement);
      break; 
    case tag_return:
      analyze_return(statement);
      break; 
    default:
      analyze_expr(statement);
    }
    statement = statement->next;
  }

  return;
}

void
scan_labels(tree *statement, char *base_name)
{
  char *label;
  struct variable *label_var;

  while(statement) {
    switch(statement->tag) {
    case tag_cond:
      scan_labels(COND_BODY(statement), base_name);  
      break;
    case tag_label:
      label = mangle_name2(base_name, LABEL_NAME(statement));      
      label_var = add_global_symbol(label, 
                                    statement,
                                    sym_label,
                                    storage_local,
                                    NULL);
      add_symbol_pointer(LABEL_NAME(statement), statement, label_var);
      break; 
    case tag_loop:
      scan_labels(LOOP_BODY(statement), base_name);  
      break; 
    default:
      /* do nothing */
      break;
    }
    statement = statement->next;
  }

  return;
}

static tree *init;
static tree *last_init;

static void
append_init(char *name, tree *expr)
{
  tree *constant;
  tree *symbol;
  tree *assignment;

  constant = mk_constant(maybe_evaluate(expr));
  COPY_DEBUG(expr, constant);
  
  symbol = mk_symbol(name, NULL);
  COPY_DEBUG(expr, symbol);
  
  assignment = mk_binop(op_assign, symbol, constant);
  COPY_DEBUG(expr, assignment);

  if (init) {
    /* append to existing list */
    node_list(last_init, assignment);
    last_init = assignment;
  } else {
    /* start of list */
    init = assignment;
    last_init = assignment;
  }
  
  return;
}

/* Add all the the arguments from a procedure or function to the 
   global symbol table */

static void
add_arg_symbols(tree *node,
                char *subprogram_name,
                enum node_storage storage,
                gp_boolean add_alias)
{
  tree *head = NULL;
  tree *arg = NULL;
  char *arg_name;
  struct variable *var;

  if (node->tag == tag_subprogram) {
    head = SUB_HEAD(node);
  } else {
    assert(0);
  }

  arg = HEAD_ARGS(head);

  while (arg) {
    assert(arg->tag == tag_arg);
    arg_name = mangle_name2(subprogram_name, ARG_NAME(arg));
    var = add_global_symbol(arg_name,
                            arg,
                            sym_udata,
                            storage,
                            ARG_TYPE(arg));
    if (add_alias) {
      add_symbol_pointer(ARG_NAME(arg), arg, var);
    }
    if ((var->type->tag == type_array) ||
        (var->type->tag == type_record))  {
      analyze_error(arg, "arguments can not be arrays or records");     
    }
    arg = arg->next;
  }

  return;
}

void
analyze_subprogram(tree *subprogram)
{
  tree *head;
  tree *body;
  struct type *return_type;
  char *subprogram_name;
  char *return_name;
  tree *args;
  tree *decl;
  tree *statements;

  if (!state.processor_chosen) {
    analyze_error(subprogram, "processor not selected");
    return;
  }

  head = SUB_HEAD(subprogram);
  body = SUB_BODY(subprogram);
  if (SUB_RET(subprogram)) {
    generating_function = true;
    return_type = get_type(SUB_RET(subprogram));
    if (return_type == NULL) {
      analyze_error(subprogram, "unknown return type");
      return_size = size_unknown;
    } else {
      return_size = prim_size(return_type);
    }
  } else {
    generating_function = false;
  }

  assert(head->tag == tag_head);  
  assert(body->tag == tag_body); 
  args = HEAD_ARGS(head);
  decl = BODY_DECL(body);
  statements = BODY_STATEMENTS(body); 

  subprogram_name = find_node_name(subprogram);
  subprogram_var = get_global(subprogram_name);
  assert(subprogram_var != NULL);

  /* local symbol table */
  state.top = push_symbol_table(state.top, true);
  state.type_top = push_symbol_table(state.type_top, true);
  found_return = false;
  init = NULL;

  /* local data */
  analyze_elements(decl,
                   subprogram_var->name,
                   storage_local,
                   storage_local,
                   true);

  if (init) {
    node_list(last_init, statements);
    statements = init;
  }

  /* add the return */
  if (generating_function) {
    return_name = mangle_name2(subprogram_var->name, "return");
    return_var = add_global_symbol(return_name,
                                   subprogram,
                                   sym_udata,
                                   subprogram_var->storage,
                                   SUB_RET(subprogram));
  }

  /* add the procedure arguments */
  add_arg_symbols(subprogram,
                  subprogram_var->name,
                  subprogram_var->storage,
                  true);     

  /* write the procedure to the assembly file */
  state.current_bank = NULL;
  state.current_ibank = NULL;
  codegen_init_proc(subprogram_var->name,
                    subprogram_var->storage,
                    generating_function);
  scan_labels(statements, subprogram_var->name);
  analyze_statements(statements);
  codegen_finish_proc(!generating_function);

  if ((generating_function) && (!found_return)) {
    analyze_error(subprogram, "function is missing return");
  }

  /* scan the local data, if a location is used add it to the list */
  if (state.optimize.trival_expressions) {

  }

  /* remove the local table */
  state.top = pop_symbol_table(state.top);
  state.type_top = pop_symbol_table(state.type_top);

  return;
}

void
write_constants(void)
{
  int i;
  struct symbol *sym;
  struct variable *var;
  gp_boolean first_time = true;

  for (i = 0; i < HASH_SIZE; i++) {
    for (sym = state.global->hash_table[i]; sym; sym = sym->next) {
      var = get_symbol_annotation(sym);
      if (var && ((var->tag == sym_const) || 
                  (var->is_absolute && is_extern(var)))) {
         if (first_time) {
           codegen_write_comment("constants");
           first_time = false;
         }
         if (var->tag == sym_const) {
           codegen_write_equ(var->name, var->value);
         } else if (var->is_absolute) {
           codegen_write_equ(var->name, var->address);
         }
      }      
    }
  }

  if (first_time == false)
    fprintf(state.output.f, "\n");

}

void
write_declarations(void)
{
  int i;
  struct symbol *sym;
  struct variable *var;
  gp_boolean first_time;
  gp_boolean write_mem;

  /* write the data memory section */

  first_time = true;

  for (i = 0; i < HASH_SIZE; i++) {
    for (sym = state.global->hash_table[i]; sym; sym = sym->next) {
      var = get_symbol_annotation(sym);
      if (var && is_data(var) && in_module(var) && !var->is_absolute) {
        write_mem = true;
        if ((!var->is_used) && (var->storage != storage_public)) {
          analyze_warning(var->node, "unused variable %s", var->name);
          if ((state.optimize.unused_mem) && (!var->is_assigned)) {
            write_mem = false;
          }
        }
        if (write_mem) {
          codegen_write_data(var);
          first_time = false;
        }
      }      
    }
  }

  codegen_temp_data();

  if (first_time == false)
    fprintf(state.output.f, "\n");

  /* write the absolute sections memory section */

  first_time = true;

  for (i = 0; i < HASH_SIZE; i++) {
    for (sym = state.global->hash_table[i]; sym; sym = sym->next) {
      var = get_symbol_annotation(sym);
      if (var && is_data(var) && in_module(var) && var->is_absolute) {
        write_mem = true;
        if ((!var->is_used) && (var->storage != storage_public)) {
          analyze_warning(var->node, "unused variable %s", var->name);
          if ((state.optimize.unused_mem) && (!var->is_assigned)) {
            write_mem = false;
          }
        }
        if (write_mem) {
          fprintf(state.output.f, ".udata_%s udata %#x\n",
                  var->name,
                  var->address);
          codegen_write_data(var);
          fprintf(state.output.f, "\n");
        }
      }      
    }
  }

  /* write the external symbols */

  first_time = true;

  for (i = 0; i < HASH_SIZE; i++) {
    for (sym = state.global->hash_table[i]; sym; sym = sym->next) {
      var = get_symbol_annotation(sym);
      assert(var != NULL);
      if (var && 
          ((var->is_used) || (var->is_assigned)) &&
          is_extern(var) && !var->is_absolute) {
        if (first_time == true) {
          codegen_write_comment("external subprograms and data");
          first_time = false;
        }
        fprintf(state.output.f, "  .def %s, extern, type=%#x\n",
                var->name,
                var_coff_type(var));
      }
    }
  }

  if (first_time == false)
    fprintf(state.output.f, "\n");

}

#define LINE_SIZ 520

void
analyze_select_processor(tree *expr, char *name)
{
  struct px *found = NULL;

  if (state.cmd_processor) {
    analyze_warning(expr, "processor superseded by command line");
  } else {
    found = gp_find_processor(name);
    if (found) {
      if (state.processor == no_processor) {
        state.processor = found->tag;
        state.class = gp_processor_class(state.processor);
        state.processor_info = found;
      } else if (state.processor != found->tag ) {
        analyze_warning(expr, "redefining processor");
      }
    } else {
      analyze_error(expr, "unknown processor %s", name);
    }
   
    /* load the instruction sets if necessary */
    if ((state.processor_chosen == false) && 
        (state.processor != no_processor)) {
      state.processor_chosen = true;
      codegen_select(expr);
    }
  }
}

static void
analyze_pragma(tree *expr,
               enum node_storage *data_storage,
               enum node_storage *prog_storage)
{
  tree *lhs;
  tree *rhs;

  switch (expr->tag) {
  case tag_binop:
    lhs = expr->value.binop.p0;
    rhs = expr->value.binop.p1;
    if ((expr->value.binop.op != op_assign) ||
        (lhs->tag != tag_symbol)) {
      analyze_error(expr, "unknown pragma");
    } else {
      if (strcasecmp(SYM_NAME(lhs), "code_address") == 0) {
        if (*prog_storage != storage_private) {
          analyze_error(expr, "code section addresses can only be in modules");
        } else {            
          state.section.code_addr = maybe_evaluate(rhs);
          state.section.code_addr_valid = true;
        }
      } else if (strcasecmp(SYM_NAME(lhs), "code_section") == 0) {
        if (rhs->tag != tag_string) {
          analyze_error(expr, "code section name must be a string");
        } else {
          if (*prog_storage == storage_public) {
            if (state.section.code) {
              analyze_error(expr, "duplicate code section name");
            } else {
              state.section.code = rhs->value.string;
            }         
          } else if (*prog_storage == storage_far) {
            if ((state.section.code) && 
                (strcmp(rhs->value.string, state.section.code) == 0)) {
              *prog_storage = storage_near;
            }
          } else {
            analyze_warning(expr, "section pragmas can only be in a public");
          }
        }
      } else if (strcasecmp(lhs->value.string, "processor") == 0) {
        if (rhs->tag != tag_string) {
          analyze_error(expr, "processor name must be a string");        
        } else {
          analyze_select_processor(rhs, rhs->value.string);
        }
      } else if (strcasecmp(SYM_NAME(lhs), "data_address") == 0) {
        if (*data_storage != storage_private) {
          analyze_error(expr, "data section addresses can only be in modules");
        } else {            
          state.section.data_addr = maybe_evaluate(rhs);
          state.section.data_addr_valid = true;
        }
      } else if (strcasecmp(lhs->value.string, "data_section") == 0) {
        if (rhs->tag != tag_string) {
          analyze_error(expr, "data section name must be a string");
        } else {
          if (*data_storage == storage_public) {
            if (state.section.data) {
              analyze_error(expr, "duplicate data section name");
            } else {
              state.section.data = rhs->value.string;
            }         
          } else if (*data_storage == storage_far) {
            if ((state.section.data) && 
                (strcmp(rhs->value.string, state.section.data) == 0)) {
              *data_storage = storage_near;
            }
          } else {
            analyze_warning(expr, "section pragmas can only be in a public");
          }
        }
      } else {
        analyze_error(expr, "unknown pragma %s", SYM_NAME(lhs));
      }
    }
    break;
  default:
    analyze_error(expr, "unknown pragma");
  }
  
  return;
}

/* add all of the decl to the symbol table */

static void
analyze_decl(tree *decl,
             char *root_name, 
             enum node_storage data_storage,
             gp_boolean add_alias)
{
  struct variable *var;
  char *name;
  char *alias;

  alias = DECL_NAME(decl);
  name = mangle_name2(root_name, alias);
 
  if (DECL_CONSTANT(decl)) {
    if (DECL_INIT(decl)) {
      var = add_constant(name,
                         maybe_evaluate(DECL_INIT(decl)),
                         decl,
                         DECL_TYPE(decl));
      if (add_alias) {
        add_symbol_pointer(alias, decl, var);
      }
    } else {
      analyze_error(decl, "missing constant value");
    }
  } else {
    /* FIXME: add symbol for each entry in a record */
    var = get_global(name);
    if (!var) {
      /* The data has not been declared or defined */
      var = add_global_symbol(name,
                              decl,
                              sym_udata,
                              data_storage,
                              DECL_TYPE(decl));
      if (add_alias) {
        add_symbol_pointer(alias, decl, var);
      }
    } else if ((var->storage != storage_public) ||
               (data_storage != storage_private)) {
      analyze_error(decl, 
                    "duplicate declaration previously defined in %s:%i:",
                    get_file_name(decl->file_id),
                    decl->line_number);
    }   
    if (DECL_INIT(decl)) {
      /* FIXME: implement the record initialization (value <, value>*) */
      if (data_storage == storage_local) {
        append_init(name, DECL_INIT(decl));
      } else if (data_storage != storage_private) {
        analyze_error(decl, "declarations can not be initialized");
      } else {
        var->value = maybe_evaluate(DECL_INIT(decl));
        analyze_error(decl, "initialized data not yet supported");
      }
    }
    if (DECL_ADDR(decl)) {
      int address = maybe_evaluate(DECL_ADDR(decl));
      
      if ((var->is_absolute) && (var->address != address)) {
        analyze_error(decl, "mismatch in declared absolute address",
                      name); 
      }
      if (data_storage == storage_local) {
        analyze_error(decl, "address can't be specified on local data");
      } else {
        var->is_absolute = true;
        var->address = address;
      }
    }
    if (DECL_VOLATILE(decl)) {
      var->is_volatile = true;
    }

    /* FIXME: figure out how to add this one:
      analyze_error(decl, "missing definition for %s", name); 
    */
  }

  return;
}

/* add all of the types to the type table */

static void
analyze_type(tree *type,
             char *root_name,
             gp_boolean add_alias)
{
  tree *list;
  char *name;
  char *enum_name;
  int enum_num;
  struct variable *var;
  int start;
  int end;
  
  name = mangle_name2(root_name, TYPE_TYPE(type));
  
  if (TYPE_ACCESS(type)) {
    /* access type, for now it is treated like an alias */
    if (state.processor_chosen) {
      /* FIXME: implement access types to program memory space */
      add_type_access(name, target->data_ptr_type, TYPE_OF(type));
      if (add_alias) {
        add_type_alias(TYPE_TYPE(type), name);
      }
    } else {
      analyze_error(type, 
                    "processor must be selected before access types declared");
    }
  } else if (TYPE_LIST(type)) {
    /* enumerated type */
    add_type_enum(name);
    if (add_alias) {
      add_type_alias(TYPE_TYPE(type), name);
    }
    enum_num = 0;
    list = TYPE_LIST(type);
    while (list) {
      if (list->tag == tag_symbol) {
        /* add the constant to the global symbol table */
        enum_name = mangle_name2(root_name, SYM_NAME(list));
        var = add_constant(enum_name, enum_num++, list, name);
        if (add_alias) {
          add_symbol_pointer(SYM_NAME(list), list, var);
        }
      } else {
        analyze_error(list, "enumerated list must be symbols");
      }
      list = list->next;
    }
  } else if (TYPE_START(type)) {
    /* array type */
    if ((can_evaluate(TYPE_START(type), true)) && 
         can_evaluate(TYPE_END(type), true)) {
      start = evaluate(TYPE_START(type));
      end = evaluate(TYPE_END(type));
      add_type_array(name, start, end, TYPE_OF(type));
      if (add_alias) {
        add_type_alias(TYPE_TYPE(type), name);
      }
    }
  } else {
    /* alias */
    add_type_alias(name, TYPE_OF(type));
    if (add_alias) {
      add_type_alias(TYPE_TYPE(type), name);
    } 
  }


}

static tree *
find_public(char *public_name)
{
  char file_name[BUFSIZ];
  struct symbol *sym;
  tree *public = NULL;

  sym = get_symbol(state.publics, public_name);
  if (sym == NULL) {
    /* couldn't find the public, attempt to open and parse a .pub file
       containing the public */
    strncpy(file_name, public_name, sizeof(file_name));
    strncat(file_name, ".pub", sizeof(file_name));
    parse(file_name);
  }

  sym = get_symbol(state.publics, public_name);
  if (sym) {
    public = get_symbol_annotation(sym);
    assert(public != NULL);
  } else {
    analyze_error(NULL, "unknown public %s", public_name);
  }

  return public;
}

/* verify the declaration and the definition match */

static void
check_subprogram_declaration(tree *def, tree *prot)
{
  tree *def_head;
  tree *def_args;
  tree *prot_head;
  tree *prot_args;

  def_head = SUB_HEAD(def);
  prot_head = SUB_HEAD(prot);
  
  if (SUB_RET(def)) {
    if (SUB_RET(prot) == NULL) {
      analyze_error(prot, "the definition is a function");
      return;
    }
    if (strcasecmp(SUB_RET(def), SUB_RET(prot)) != 0) {
      analyze_error(prot, "the definition returns %s", SUB_RET(def));
    }
  } else if (SUB_RET(prot)) {
    analyze_error(prot, "the definition is a procedure");
    return;
  }
    
  def_args = HEAD_ARGS(def_head);
  prot_args = HEAD_ARGS(prot_head);
 
  while (def_args) {
    assert(def_args->tag == tag_arg);
 
    if (prot_args == NULL) {
      analyze_error(prot_head, "the declaration is missing arguments");
      return;
    }

    if (strcasecmp(ARG_NAME(def_args), ARG_NAME(prot_args)) != 0) {
      analyze_error(prot,
                    "argument %s,\n\t defined as %s in %s:%i:",
                    ARG_NAME(prot_args),
                    ARG_NAME(def_args),
                    get_file_name(def->file_id),
                    def->line_number);
      /* The argument name didn't match, so skip the rest of the tests.  
         This should prevent generating spurious errors. */
      return;
    }

    if (strcasecmp(ARG_TYPE(def_args), ARG_TYPE(prot_args)) != 0) {
      analyze_error(prot,
                    "type %s of argument %s,\n\t defined as %s in %s:%i:",
                    ARG_TYPE(prot_args),
                    ARG_NAME(prot_args),
                    ARG_TYPE(def_args),
                    get_file_name(def->file_id),
                    def->line_number);
    }

    if (ARG_DIR(def_args) != ARG_DIR(prot_args)) {
      analyze_error(prot,
                    "direction mismatch of argument %s,\n\t defined in %s:%i:",
                    ARG_NAME(prot_args),
                    get_file_name(def->file_id),
                    def->line_number);
    }

    def_args = def_args->next;
    prot_args = prot_args->next;
  }

  if (prot_args) {
    analyze_error(prot_args, "the declaration has extra arguments");
  }

  return;
}

static void
analyze_elements(tree *current,
                 char *root_name,
                 enum node_storage data_storage,
                 enum node_storage prog_storage,
                 gp_boolean add_alias)
{
  struct variable *var;
  char *name;
  char *alias;
  tree *public = NULL;

  while (current) {
    switch (current->tag) {
    case tag_alias:
      add_global_symbol(ALIAS_ALIAS(current),
                        ALIAS_EXPR(current),
                        sym_alias,
                        storage_unknown,
                        NULL);
      break;
    case tag_cond:
      analyze_preprocess(current, (long int)analyze_elements);
      break;
    case tag_pragma:
      analyze_pragma(current->value.pragma, &data_storage, &prog_storage);
      break;
    case tag_decl:
      analyze_decl(current, root_name, data_storage, add_alias);
      break;
    case tag_record:
      name = mangle_name2(root_name, RECORD_NAME(current));
      add_type_record(name, RECORD_LIST(current));
      if (add_alias) {
        add_type_alias(RECORD_NAME(current), name);
      }
      break;
    case tag_type:
      analyze_type(current, root_name, add_alias);
      break;
    case tag_subprogram:
      alias = find_node_name(current);
      name = mangle_name2(root_name, alias);
      var = get_global(name);
      if (!var) {
        /* The subprogram has not been declared or defined */
        var = add_global_symbol(name,
                                current,
                                sym_subprogram,
                                prog_storage,
                                NULL);
        if (add_alias) {
          add_symbol_pointer(alias, current, var);
        }
        if ((prog_storage == storage_near) ||
            (prog_storage == storage_far)) {
          if (SUB_RET(current)) {
            add_global_symbol(mangle_name2(var->name, "return"),
                              current,
                              sym_udata,
                              data_storage,
                              SUB_RET(current));
          }        
          add_arg_symbols(current,
                          var->name,
                          data_storage,
                          false);
          if (SUB_BODY(current)) {
            analyze_error(current, "illegal subprogram definition");
          }
        }
      } else if (var->tag != sym_subprogram) {
        analyze_error(current, "duplicate symbol %s also in %s:%i:",
                      alias,
                      get_file_name(var->file_id),
                      var->line_number);
      } else if (SUB_BODY(current) == NULL) {
        analyze_error(current, 
                      "subprogram %s previously declared or defined in %s:%i:",
                      alias,
                      get_file_name(var->file_id),
                      var->line_number);
      } else if (SUB_BODY(var->node)) {
        analyze_error(current, "subprogram %s previously defined in %s:%i:",
                      alias,
                      get_file_name(var->file_id),
                      var->line_number);
      } else {
        check_subprogram_declaration(current, var->node);       
      }
      if (prog_storage == storage_local) {
       analyze_error(current, "nested subprograms are not supported");
      }
      if ((gp_num_errors == 0) && (SUB_BODY(current))) {
        /* It is a definition so scan the subprogram */
        analyze_subprogram(current);
      }
      break;
    case tag_with:
      public = find_public(WITH_NAME(current));
      if (public) {
        deps_add(get_file_name(public->file_id));

        add_global_symbol(FILE_DATA_ADDR(public),
                          current,
                          sym_addr,
                          FILE_DATA(public),
                          NULL);

        add_global_symbol(FILE_CODE_ADDR(public),
                          current,
                          sym_addr,
                          FILE_CODE(public),
                          NULL);

        analyze_elements(FILE_BODY(public),
                         FILE_NAME(public),
                         FILE_DATA(public),
                         FILE_CODE(public),
                         false);
      }
      break;
    default:
      assert(0);
    }

    current = current->next;
  }

  return;
}

void
analyze(tree *module)
{
  tree *public = NULL;

  generating_function = false;
  found_return = false;
  return_size = size_unknown;
  state.current_page = NULL;
  state.module = module;

  deps_init(module);

  /* locate the public for the module being compiled */ 
  public = find_public(FILE_NAME(module));
  if (public == NULL)
    return;

  /* scan the public */
  analyze_elements(FILE_BODY(public),
                   FILE_NAME(public),
                   storage_public,
                   storage_public,
                   true);

  /* create symbols for the start of the module's program and data memory */
  add_global_symbol(FILE_DATA_ADDR(module),
                    module,
                    sym_addr,
                    storage_public,
                    NULL);

  add_global_symbol(FILE_CODE_ADDR(module),
                    module,
                    sym_addr,
                    storage_public,
                    NULL);

  /* FIXME: difficult to select processor without scanning all the publics */
  codegen_init_asm(module);

  /* scan the module */
  analyze_elements(FILE_BODY(module),
                   FILE_NAME(module),
                   storage_private,
                   storage_private,
                   true);

  /* finish the assembly output */
  codegen_init_data();
  write_declarations();
  write_constants();
  codegen_close_asm();
  deps_close();

  return;
}

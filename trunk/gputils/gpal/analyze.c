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
#include "symbol.h"
#include "analyze.h"
#include "codegen.h"
#include "optimize.h"
#include "scan.h"

/* prototypes */
void analyze_statements(tree *statement);

/* function return data */
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

  if (gp_quiet != 0)
    return;

  va_start(args, format);
  vsprintf(buffer, format, args);
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

  if (gp_quiet != 0)
    return;

  va_start(args, format);
  vsprintf(buffer, format, args);
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

/* convert a private procedure to public */

static void
make_proc_public(struct variable *var, tree *prot)
{
  tree *def;
  tree *def_head;
  tree *def_args;
  tree *prot_head;
  tree *prot_args;

  /* make the procedure public */
  var->storage = storage_public;
  
  /* verify that the procedure definition and declaration match */
  def = var->node;
  
  if (var->tag == sym_proc) {
    if (prot->tag != node_proc) {
      analyze_error(prot, "the definition is a procedure");
      return;
    }
    def_head = PROC_HEAD(def);
    prot_head = PROC_HEAD(prot);
  } else {
    if (prot->tag != node_func) {
      analyze_error(prot, "the definition is a function");
      return;
    }
    def_head = FUNC_HEAD(def);
    prot_head = FUNC_HEAD(prot);
    if (strcasecmp(FUNC_RET(def), FUNC_RET(prot)) != 0) {
      analyze_error(prot, "the definition returns %s", FUNC_RET(def));
    }
  }
    
  def_args = HEAD_ARGS(def_head);
  prot_args = HEAD_ARGS(prot_head);
 
  while (def_args) {
    assert(def_args->tag == node_arg);
 
    if (prot_args == NULL) {
      analyze_error(prot_head, "the declaration is missing arguments");
      return;
    }

    if (strcasecmp(ARG_NAME(def_args), ARG_NAME(prot_args)) != 0) {
      analyze_error(prot,
                    "argument %s,\n\t defined as %s in %s:%i:",
                    ARG_NAME(prot_args),
                    ARG_NAME(def_args),
                    get_file_name(var->file_id),
                    var->line_number);
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
                    get_file_name(var->file_id),
                    var->line_number);
    }

    if (ARG_DIR(def_args) != ARG_DIR(prot_args)) {
      analyze_error(prot,
                    "direction mismatch of argument %s,\n\t defined in %s:%i:",
                    ARG_NAME(prot_args),
                    get_file_name(var->file_id),
                    var->line_number);
    }

    def_args = def_args->next;
    prot_args = prot_args->next;
  }

  if (prot_args) {
    analyze_error(prot_args, "the declaration has extra arguments");
  }

  return;
}

/* can the expression be evaluated at compile time */

gp_boolean
can_evaluate(tree *p, gp_boolean gen_errors)
{
  struct variable *var;

  switch (p->tag) {
  case node_constant:
    return true;
  case node_symbol:
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
  case node_unop:
    return can_evaluate(p->value.unop.p0, gen_errors);
  case node_binop:
    return can_evaluate(p->value.binop.p0, gen_errors) && 
           can_evaluate(p->value.binop.p1, gen_errors);
  case node_string:
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
  case node_constant:
    return p->value.constant;
  case node_symbol:
    var = get_global(SYM_NAME(p));
    assert (var != NULL);
    return var->value;
  case node_unop:
    switch (p->value.unop.op) {
    case op_not:
      return !evaluate(p->value.unop.p0);
    case op_add:
      return  evaluate(p->value.unop.p0);
    case op_neg:
      return -evaluate(p->value.unop.p0);
    default:
      assert(0);
    }
  case node_binop:
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

  var = get_global(name);
  if (var == NULL) {
    analyze_error(node, "unknown symbol %s", name);
    return true;
  }

  var->is_used = true;

  /* If it is not an unchecked conversion, verify the sizes of the types match */
  if ((size != size_unknown) &&
      (var->tag != sym_const) &&
      (var->type) &&
      (prim_type(var->type) != size)) {
    analyze_error(node, "type mismatch in symbol %s", name);
    return true;
  }

  if ((var->tag == sym_idata) || (var->tag == sym_udata)) {

    if ((var->is_assigned == false) && (var->storage == storage_local)) {
      analyze_warning(node, "%s might be used uninitialized", name);
    }

    /* Save the size for scanning expressions with an unknown type */
    if ((size == size_unknown) && (var->type)) {     
      enum size_tag temp_size = prim_type(var->type);

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
  case node_arg:
    return test_symbol(expr, ARG_NAME(expr), size);
  case node_binop:
    return scan_tree(BINOP_LEFT(expr), size) ||
           scan_tree(BINOP_RIGHT(expr), size);
  case node_call:
    return test_symbol(expr, CALL_NAME(expr), size);
  case node_constant:
    return false;
  case node_unop:
    return scan_tree(UNOP_ARG(expr), size);
  case node_symbol:
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
  case node_arg:
    return true;
  case node_constant:
    return true;
  case node_symbol:
    return true;
  case node_unop:
    if (p->value.unop.op == op_not) {
      return scan_test(p->value.unop.p0);
    } else {
      return true;
    }
  case node_binop:
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
  enum size_tag call_size;
  gp_boolean load_result = false;

  var = get_global(CALL_NAME(call));
  if (var) {
    def = var->node;
    if (def->tag == node_proc) {
      head = PROC_HEAD(var->node);
      if (in_expr) {
        analyze_error(call, "procedures can not be called in expressions");
        return;
      }
    } else if (def->tag == node_func) {
      head = FUNC_HEAD(var->node);
      load_result = true;
      call_size = prim_type(get_type(FUNC_RET(var->node)));
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
    } else {
      analyze_error(call, "name in the call is not a function or procedure");
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
    assert(def_args->tag == node_arg);

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
    assert(def_args->tag == node_arg);

    if ((ARG_DIR(def_args) == dir_inout) ||
        (ARG_DIR(def_args) == dir_out)) {
      if (call_args->tag == node_symbol) {
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
  struct variable *var;
  enum size_tag size;
  gp_boolean constant_offset = true;
  int element_size;
  int offset = 0;
  tree *unop = NULL;

  if ((expr->tag != node_binop) || (BINOP_OP(expr) != op_assign)) {
    analyze_error(expr, "expression is missing =");
    return;
  }

  left = BINOP_LEFT(expr);
  right = BINOP_RIGHT(expr);
 
  if (left->tag != node_symbol) {
    analyze_error(expr, "invalid lvalue in assignment");
    return;
  }
  
  var = get_global(SYM_NAME(left));
  if (var == NULL) {
    analyze_error(left, "unknown symbol %s", SYM_NAME(left));  
    return;
  }

  /* fetch the symbol's primative type */
  assert(var->type != NULL);
  size = prim_type(var->type);

  /* Verify all symbols in the expression are in the symbol table and
     that they are the same type as the lvalue. */
  if (scan_tree(right, size))
    return;

  /* calculate the offset for indirect accesses if necessary */
  if (SYM_OFST(left)) {
    element_size = type_size(var->type->prim);
    
    if (var->type->tag == type_array) {
      if (can_evaluate(SYM_OFST(left), false)) {
        /* direct access with an offset */
        constant_offset = true;
        offset = analyze_check_array(left, var) * element_size;
      } else {
        /* indirect access */
        constant_offset = false;
        codegen_indirect(SYM_OFST(left), var, element_size, true);
      }
    } else {
      analyze_error(left, "lvalue %s is not an array", SYM_NAME(left));
      return;
    }
  }

  codegen_line_number(right);

  /* optimize the expression */
  right = optimize_expr(right);

  unop = optimize_unop_expr(var, left, right);
  if (unop) {
    /* write the simplified expression */
    codegen_unop(var, constant_offset, offset, SYM_OFST(left), unop, size);
  } else {
    /* write the expression */
    codegen_expr(right, size);

    /* put the result in memory */
    codegen_store(var, constant_offset, offset, SYM_OFST(left));
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
    codegen_store(return_var, false, 0, NULL);
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
}

void
analyze_statements(tree *statement)
{
  struct variable *label_var;

  if (statement == NULL)
    return;

  while(statement) {
    switch(statement->tag) {
    case node_assembly:
      analyze_assembly(statement);
      break;
    case node_call:
      analyze_call(statement, false, size_unknown);
      break;
    case node_cond:
      analyze_cond(statement, NULL);  
      break;
    case node_goto:
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
    case node_label:
      label_var = get_global(LABEL_NAME(statement));
      if (label_var != NULL) {
        /* errors will be generated in scan_labels() */
        codegen_write_label(label_var->name);
        if (label_var->storage == storage_public) {
          codegen_write_asm("global %s", label_var->name);
        }
      }
      break; 
    case node_loop:
      analyze_loop(statement);
      break; 
    case node_return:
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
    case node_cond:
      scan_labels(COND_BODY(statement), base_name);  
      break;
    case node_label:
      label = mangle_name2(base_name, LABEL_NAME(statement));      
      label_var = add_global_symbol(label, 
                                    statement,
                                    sym_label,
                                    storage_local,
                                    NULL);
      add_symbol_alias(LABEL_NAME(statement), statement, label_var);
      break; 
    case node_loop:
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
                char *proc_name,
                enum node_storage storage,
                gp_boolean add_alias)
{
  tree *head = NULL;
  tree *arg = NULL;
  char *arg_name;
  struct variable *var;

  if (node->tag == node_proc) {
    head = PROC_HEAD(node);
  } else if (node->tag == node_func) {
    head = FUNC_HEAD(node);
  } else {
    assert(0);
  }

  arg = HEAD_ARGS(head);

  while (arg) {
    assert(arg->tag == node_arg);
    arg_name = mangle_name2(proc_name, ARG_NAME(arg));
    var = add_global_symbol(arg_name,
                            arg,
                            sym_udata,
                            storage,
                            ARG_TYPE(arg));
    if (add_alias) {
      add_symbol_alias(ARG_NAME(arg), arg, var);
    }
    if (var->type->tag == type_array) {
      analyze_error(arg, "arguments can not be arrays");     
    }
    arg = arg->next;
  }

  return;
}

void
analyze_procedure(tree *procedure, gp_boolean is_func)
{
  tree *head;
  tree *body;
  struct type *return_type;
  struct variable *var;
  char *proc_name;
  char *return_name;
  tree *args;
  tree *decl;
  char *decl_name;
  struct variable *decl_var;
  tree *statements;

  /* local symbol alias table */
  state.top = push_symbol_table(state.top, 1);
  found_return = false;
  init = NULL;  

  if (is_func) {
    head = FUNC_HEAD(procedure);
    body = FUNC_BODY(procedure);
    generating_function = true;
    return_type = get_type(FUNC_RET(procedure));
    if (return_type == NULL) {
      analyze_error(procedure, "unknown return type");
      return_size = size_unknown;
    } else {
      return_size = prim_type(return_type);
    }
  } else {
    head = PROC_HEAD(procedure);
    body = PROC_BODY(procedure);
  }

  assert(head->tag == node_head);  
  assert(body->tag == node_body); 
  args = HEAD_ARGS(head);
  decl = BODY_DECL(body);
  statements = BODY_STATEMENTS(body); 

  proc_name = find_node_name(procedure);
  var = get_global(proc_name);
  assert(var != NULL);

  /* add the procedure arguments */
  add_arg_symbols(procedure,
                  var->name,
                  var->storage,
                  true);     

  /* add the return */
  if (is_func) {
    return_name = mangle_name2(var->name, "return");
    return_var = add_global_symbol(return_name,
                                   procedure,
                                   sym_udata,
                                   var->storage,
                                   FUNC_RET(procedure));
  }

  /* local data */
  while (decl) {
    assert(decl->tag == node_decl);
    decl_name = mangle_name2(var->name, DECL_NAME(decl));
    if (DECL_CONSTANT(decl)) {
      if (DECL_INIT(decl)) {
        decl_var = add_constant(decl_name,
                                maybe_evaluate(DECL_INIT(decl)),
                                decl,
                                DECL_TYPE(decl));
        add_symbol_alias(DECL_NAME(decl), decl, decl_var);
      } else {
        analyze_error(decl, "missing constant value");
      }    
    } else {
      decl_var = add_global_symbol(decl_name, 
                                   decl,
                                   sym_udata,
                                   storage_local,
                                   DECL_TYPE(decl));
      add_symbol_alias(DECL_NAME(decl), decl, decl_var);
      if (DECL_INIT(decl)) {
        /* initialize the local data */
        append_init(decl_name, DECL_INIT(decl));
      }
    }

    decl = decl->next;
  }

  if (init) {
    node_list(last_init, statements);
    statements = init;
  }

  /* write the procedure to the assembly file */
  codegen_init_proc(var->name, var->storage, is_func);
  if (var->storage == storage_public) {
    codegen_write_asm("banksel %s", LOCAL_DATA_LABEL);
    codegen_write_asm("bankisel %s", LOCAL_DATA_LABEL);
  }
  scan_labels(statements, var->name);
  analyze_statements(statements);
  codegen_finish_proc(!generating_function);

  if ((is_func) && (!found_return)) {
    analyze_error(procedure, "function is missing return");
  }

  /* scan the local data, if a location is used add it to the list */
  if (state.optimize.trival_expressions) {

  }

  /* remove the local table */
  state.top = pop_symbol_table(state.top);
  generating_function = false;

  return;
}

void
analyze_constants(void)
{
  int i;
  struct symbol *sym;
  struct variable *var;
  gp_boolean first_time = true;

  for (i = 0; i < HASH_SIZE; i++) {
    for (sym = state.global->hash_table[i]; sym; sym = sym->next) {
      var = get_symbol_annotation(sym);
      if (var && (var->tag == sym_const)) {
         if (first_time) {
           codegen_write_comment("constants");
           first_time = false;
         }
         codegen_write_equ(var->name, var->value); 
      }      
    }
  }

  if (first_time == false)
    fprintf(state.output.f, "\n");

}

void
analyze_declarations(void)
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
      if (var && is_data(var) && in_module(var)) {
        write_mem = true;
        if ((!var->is_used) && (var->storage != storage_public)) {
          analyze_warning(var->node, "unused variable %s", var->name);
          if ((state.optimize.unused_mem) && (!var->is_assigned)) {
            write_mem = false;
          }
        }
        if (write_mem) {
          codegen_write_data(var->name, type_size(var->type), var->storage);
          first_time = false;
        }
      }      
    }
  }

  codegen_temp_data();

  if (first_time == false)
    fprintf(state.output.f, "\n");

  /* write the external symbols */

  first_time = true;

  for (i = 0; i < HASH_SIZE; i++) {
    for (sym = state.global->hash_table[i]; sym; sym = sym->next) {
      var = get_symbol_annotation(sym);
      assert(var != NULL);
      if (var && 
          ((var->is_used) || (var->is_assigned)) &&
          is_extern(var)) {
        if (first_time == true) {
          codegen_write_comment("external subprograms and data");
          first_time = false;
        }
        fprintf(state.output.f, "  extern %s\n", var->name);
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
analyze_pragma(tree *expr, enum source_type type)
{
  tree *lhs;
  tree *rhs;

  switch (expr->tag) {
  case node_binop:
    lhs = expr->value.binop.p0;
    rhs = expr->value.binop.p1;
    if ((expr->value.binop.op != op_assign) ||
        (lhs->tag != node_symbol)) {
      analyze_error(expr, "unknown pragma");
    } else {
      if (strcasecmp(SYM_NAME(lhs), "code_address") == 0) {
        if (rhs->tag != node_constant) {
          analyze_error(expr, "code address must be a constant");
        } else if (type == source_module) {            
          state.section.code_addr = rhs->value.constant;
          state.section.code_addr_valid = true;
        } else {
          analyze_error(expr, "code section addresses can only be in modules");
        }
      } else if (strcasecmp(SYM_NAME(lhs), "code_section") == 0) {
        if (rhs->tag != node_string) {
          analyze_error(expr, "code section name must be a string");
        } else {
          if (type == source_with) {            
            if ((state.section.code) && 
                (strcmp(rhs->value.string, state.section.code) == 0)) {
              FILE_CODE(state.module) = storage_near;
            }
          } else {
            if (type == source_module) {
              analyze_warning(expr, "section pragma's should be in a public");
            }
            if (state.section.code) {
              analyze_error(expr, "duplicate code section name");
            } else {
              state.section.code = rhs->value.string;
            }
          }        
        }
      } else if (strcasecmp(lhs->value.string, "processor") == 0) {
        if (rhs->tag != node_string) {
          analyze_error(expr, "processor name must be a string");        
        } else {
          analyze_select_processor(rhs, rhs->value.string);
        }

      } else if (strcasecmp(SYM_NAME(lhs), "udata_address") == 0) {
        if (rhs->tag != node_constant) {
          analyze_error(expr, "udata address must be a constant");
        } else if (type == source_module) {            
          state.section.udata_addr = rhs->value.constant;
          state.section.udata_addr_valid = true;
        } else {
          analyze_error(expr, "udata section addresses can only be in modules");
        }
      } else if (strcasecmp(lhs->value.string, "udata_section") == 0) {
        if (rhs->tag != node_string) {
          analyze_error(expr, "udata section name must be a string");
        } else {
          if (type == source_with) {            
            if ((state.section.udata) &&
                (strcmp(rhs->value.string, state.section.udata) == 0)) {
              FILE_UDATA(state.module) = storage_near;
            }
          } else {
            if (type == source_module) {
              analyze_warning(expr, "section pragma's should be in a public");
            }
            if (state.section.code) {
              analyze_error(expr, "duplicate udata section name");
            } else {
              state.section.udata = rhs->value.string;
            }
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

/* add all of the types to the type table */

static void
analyze_type(tree *file, tree *type, gp_boolean add_alias)
{
  tree *list;
  char *name;
  char *enum_name;
  int enum_num;
  struct variable *var;
  int start;
  int end;

  name = mangle_name2(FILE_NAME(file), TYPE_TYPE(type));

  if (TYPE_LIST(type)) {
    /* enumerated type */
    add_type_enum(name);
    if (add_alias) {
      add_type_alias(TYPE_TYPE(type), name);
    }
    enum_num = 0;
    list = TYPE_LIST(type);
    while (list) {
      if (list->tag == node_symbol) {
        /* add the constant to the global symbol table */
        enum_name = mangle_name2(FILE_NAME(file), SYM_NAME(list));
        var = add_constant(enum_name, enum_num++, list, name);
        if (add_alias) {
          add_symbol_alias(SYM_NAME(list), list, var);
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

static void
analyze_module(tree *file)
{
  tree *current;
  char *name;
  char *alias;
  struct variable *var;

  state.module = file;

  current = FILE_BODY(file);
  while (current) {
    switch (current->tag) {
    case node_pragma:
      analyze_pragma(current->value.pragma, FILE_TYPE(file));
      break;
    case node_type:
      analyze_type(file, current, true);
      break;
    case node_decl:
      alias = DECL_NAME(current);
      name = mangle_name2(FILE_NAME(file), alias);
      if (DECL_CONSTANT(current)) {
        if (DECL_INIT(current)) {
          var = add_constant(name,
                             maybe_evaluate(DECL_INIT(current)),
                             current,
                             DECL_TYPE(current));
          add_symbol_alias(alias, current, var);
        } else {
          analyze_error(current, "missing constant value");
        }     
      } else {
        var = add_global_symbol(name,
                                current, 
                                sym_udata,
                                storage_private,
                                DECL_TYPE(current));
        add_symbol_alias(alias, current, var);
        if (DECL_INIT(current)) {
          analyze_error(current, "initialized data not yet supported");
        }
      }
      break;
    case node_proc:
      alias = find_node_name(current);
      name = mangle_name2(FILE_NAME(file), alias);
      var = add_global_symbol(name, current, sym_proc, storage_private, NULL);
      add_symbol_alias(alias, current, var);
      if (PROC_BODY(current) == NULL) {
        analyze_error(current,
                      "only procedure definitions are allowed in a module");
      }
      break;
    case node_func:
      alias = find_node_name(current);
      name = mangle_name2(FILE_NAME(file), alias);
      var = add_global_symbol(name, current, sym_func, storage_private, NULL);
      add_symbol_alias(alias, current, var);
      if (FUNC_BODY(current) == NULL) {
        analyze_error(current,
                      "only function definitions are allowed in a module");
      }
      break;
    default:
      assert(0);
    }
    current = current->next;
  }

  return;
}

static void
analyze_public(tree *file)
{
  tree *current;
  char *name;
  char *alias;
  struct variable *var;

  state.module = file;

  current = FILE_BODY(file);
  while (current) {
    switch (current->tag) {
    case node_pragma:
      analyze_pragma(current->value.pragma,  FILE_TYPE(file));
      break;
    case node_type:
      if (FILE_TYPE(file) == source_public) {
        analyze_type(file, current, true);
      } else {
        analyze_type(file, current, false);
      }
      break;
    case node_decl:
    case node_proc:
    case node_func:
      break;
    default:
      assert(0);
    }
    current = current->next;
  }

  current = FILE_BODY(file);
  while (current) {
    switch (current->tag) {
    case node_pragma:
    case node_type:
      break;
    case node_decl:
      alias = find_node_name(current);
      name = mangle_name2(FILE_NAME(file), alias);
      if (DECL_CONSTANT(current)) {
        if (DECL_INIT(current)) {
          var = add_constant(name,
                             maybe_evaluate(DECL_INIT(current)),
                             current,
                             DECL_TYPE(current));
          if (FILE_TYPE(file) == source_with) {
            add_symbol_alias(alias, current, var);
          }
        } else {
          analyze_error(current, "missing constant value");
        }
      } else if (FILE_TYPE(file) == source_public) {
        var = get_global(name);
        if (var) {
          var->storage = storage_public;       
        } else {
          analyze_error(current, "missing definition for %s", name); 
        }
        if (DECL_INIT(current)) {
          analyze_error(current, "declarations can not be initialized");
        }
      } else if (FILE_TYPE(file) == source_with) {
        var = add_global_symbol(name,
                                current,
                                sym_udata,
                                FILE_UDATA(file),
                                DECL_TYPE(current));
        if (DECL_INIT(current)) {
          analyze_error(current, "declarations can not be initialized");
        }
      } else {
        assert(0);
      }
      break;
    case node_proc:
      alias = find_node_name(current);
      name = mangle_name2(FILE_NAME(file), alias);
      if (PROC_BODY(current)) {
        analyze_error(current,
                      "only procedure declarations are allowed in a public");
      }
      if (FILE_TYPE(file) == source_public) {
        var = get_global(name);
        if (var) {
          make_proc_public(var, current);
        } else {
          analyze_error(current, "missing definition for %s", name); 
        }
      } else if (FILE_TYPE(file) == source_with) {
        var = add_global_symbol(name,
                                current,
                                sym_proc,
                                FILE_CODE(file),
                                NULL);
        add_arg_symbols(current,
                        var->name,
                        FILE_UDATA(file),
                        false);
      } else {
        assert(0);
      }
      break;
    case node_func:
      alias = find_node_name(current);
      name = mangle_name2(FILE_NAME(file), alias);
      if (FUNC_BODY(current)) {
        analyze_error(current,
                      "only function declarations are allowed in a public");
      }
      if (FILE_TYPE(file) == source_public) {
        var = get_global(name);
        if (var) {
          make_proc_public(var, current);
        } else {
          analyze_error(current, "missing definition for %s", name); 
        }
      } else if (FILE_TYPE(file) == source_with) {
        var = add_global_symbol(name,
                                current,
                                sym_func,
                                FILE_CODE(file),
                                NULL);
        add_arg_symbols(current,
                        var->name,
                        FILE_UDATA(file),
                        false);
      } else {
        assert(0);
      }

      break;
    default:
      assert(0);
    }
    current = current->next;
  }

  return;
}

static void
analyze_module_contents(tree *file)
{
  tree *current = FILE_BODY(file);

  while (current) {
    switch (current->tag) {
    case node_pragma:
    case node_decl:
    case node_type:
      /* do nothing */
      break;
    case node_proc:
      analyze_procedure(current, 0);
      break;
    case node_func:
      analyze_procedure(current, 1);
      break;
    default:
      assert(0);
    }
    current = current->next;
  }

  return;
}

void
analyze(void)
{
  tree *current;
  tree *module = NULL;
  gp_boolean found_module = false;

  generating_function = false;
  found_return = false;
  return_size = size_unknown;

  /* add all procedures and data to the global symbol table */
  current = state.root;
  while (current) {
    if (FILE_TYPE(current) == source_module) {
      if (found_module) {
        analyze_error(current, "found multiple modules in one file");
      } else {
        analyze_module(current);
        module = current;
      }
      found_module = true;
    }
    current = current->next;
  }

  /* add all prototypes and externs to global symbol table */
  current = state.root;
  while (current) {
    if ((FILE_TYPE(current) == source_public) ||
        (FILE_TYPE(current) == source_with)) {
      analyze_public(current);
    }
    current = current->next;
  }

  if (!state.processor_chosen) {
    analyze_error(module, "processor not selected");
    return;
  }

  /* don't bother generating code if there are errors */
  if (gp_num_errors)
    return;

  state.module = module;

  /* open the output file */
  codegen_init_asm();

  /* scan the module */
  analyze_module_contents(module);

  /* finish the assembly output */
  codegen_init_data();
  analyze_declarations();
  analyze_constants();
  codegen_close_asm();

  return;
}

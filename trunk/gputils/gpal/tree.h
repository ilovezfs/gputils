/* parse tree
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

#ifndef __TREE_H__
#define __TREE_H__

enum node_tag { 
  tag_unknown,
  tag_alias,
  tag_arg,
  tag_assembly,
  tag_attrib,
  tag_body,
  tag_binop,
  tag_call,
  tag_constant,
  tag_cond,
  tag_decl,
  tag_file,
  tag_goto,  
  tag_head,
  tag_label,
  tag_loop,
  tag_pragma,
  tag_record,
  tag_return,
  tag_string,
  tag_subprogram,
  tag_symbol,
  tag_type,
  tag_unop,
  tag_with
};

enum node_op { 
  op_unknown,  /* unknown operation */
  /* valid during all phases of compilation */
  op_assign,   /* assign a symbol a value */
  op_add,      /* add */
  op_sub,      /* subtract */
  op_mult,     /* multiply */
  op_div,      /* divide */
  op_mod,      /* modulus */
  op_neg,      /* negative */
  op_com,      /* complement */
  op_and,      /* and */
  op_or,       /* or */
  op_xor,      /* exclusive or */
  op_not,      /* not */
  op_lsh,      /* left shift */
  op_rsh,      /* right shift */
  op_land,     /* logical and */
  op_lor,      /* logical or */
  op_eq,       /* equal */
  op_ne,       /* not equal */
  op_lt,       /* less than */
  op_lte,      /* less than or equal */
  /* valid only during parse */
  op_gt,       /* branch if greater than */
  op_gte,      /* branch if greater than or equal */
  /* valid only during code generation */
  op_clr,      /* zero memory */
  op_inc,      /* increment memory */
  op_dec       /* decrement memory */
  /* add assign ops from gpasm/scan.l */
};

enum node_dir { 
  dir_unknown,
  dir_in,
  dir_inout,
  dir_out
};

enum node_storage { 
  storage_unknown,
  storage_public,  /* data which is visible to other modules */
  storage_private, /* data which is not visible to other modules */
  storage_local,   /* data which is local to a subprogram */
  storage_near,    /* external data on the same page or bank with module */
  storage_far      /* external data which is on an unknown page or bank */
};

enum source_type {
  source_unknown,
  source_module,   /* a source file */
  source_with      /* a public file */ 
};

enum attrib_type {
  attrib_unknown,
  attrib_access,
  attrib_address,
  attrib_first,
  attrib_last,
  attrib_range,
  attrib_size
};

typedef struct node_struct tree;

typedef struct node_struct {
  enum node_tag tag;
  union {
    struct {
      char *alias;
      tree *expr;
    } alias;
    struct {
      char *name;
      enum node_dir dir;
      char *type;
    } arg;
    char *assembly;
    struct {
      char *name;
      enum attrib_type type;
    } attrib;
    struct {
      tree *decl;
      tree *statements;
    } body;
    struct {
      int op;
      tree *p0, *p1;
    } binop;
    struct {
      char *name;
      tree *args;
    } call;
    int constant;
    struct {
      /* conditional (if) */
      tree *cond;
      tree *body;
      tree *next; /* else or elsif */
    } cond;    
    struct {
      char *name;
      gp_boolean is_volatile;
      gp_boolean is_constant;
      char *type;
      tree *init;
      tree *addr;
    } decl;
    struct {
      tree *body;
      char *name;
      enum source_type type;
      enum node_storage code_default;
      enum node_storage data_default;
      char *page_address;
      char *bank_address;
    } file;
    struct {
      char *name;
    } _goto;
    struct {
      char *name;
      tree *args;
    } head;
    struct {
      char *name;
    } label;
    struct {
      /* for, while, ... */
      tree *init;   /* initalization code */
      tree *exit;   /* exit loop if not true */
      tree *incr;   /* code executed at the end of each pass (for loops) */
      tree *body;
    } loop;
    tree *pragma;
    struct {
      char *name;
      tree *list;
    } record;
    tree *ret;
    char *string;
    struct {
      tree *head;
      char *ret;
      tree *body;
    } subprogram;
    struct {
      char *name;
      tree *offset;
    } symbol;
    struct {
      char *type;
      gp_boolean is_access;
      tree *start;
      tree *end;
      tree *list;
      char *of;
    } type;
    struct {
      char *name;
    } with;
    struct {
      int op;
      tree *p0;
    } unop;
  } value;

  /* node list */
  tree *prev;
  tree *next;

  /* debug information */
  int file_id;
  int line_number;

} node;

#define ALIAS_ALIAS(A)     (A)->value.alias.alias
#define ALIAS_EXPR(A)      (A)->value.alias.expr
#define ARG_NAME(A)        (A)->value.arg.name
#define ARG_DIR(A)         (A)->value.arg.dir
#define ARG_TYPE(A)        (A)->value.arg.type
#define ATTRIB_NAME(A)     (A)->value.attrib.name
#define ATTRIB_TYPE(A)     (A)->value.attrib.type
#define BODY_DECL(B)       (B)->value.body.decl
#define BODY_STATEMENTS(B) (B)->value.body.statements
#define BINOP_OP(B)        (B)->value.binop.op
#define BINOP_LEFT(B)      (B)->value.binop.p0
#define BINOP_RIGHT(B)     (B)->value.binop.p1
#define CALL_NAME(C)       (C)->value.call.name
#define CALL_ARGS(C)       (C)->value.call.args
#define COND_TEST(C)       (C)->value.cond.cond
#define COND_BODY(C)       (C)->value.cond.body
#define COND_NEXT(C)       (C)->value.cond.next
#define DECL_NAME(D)       (D)->value.decl.name
#define DECL_VOLATILE(D)   (D)->value.decl.is_volatile
#define DECL_CONSTANT(D)   (D)->value.decl.is_constant
#define DECL_TYPE(D)       (D)->value.decl.type
#define DECL_INIT(D)       (D)->value.decl.init
#define DECL_ADDR(D)       (D)->value.decl.addr
#define FILE_BODY(F)       (F)->value.file.body
#define FILE_NAME(F)       (F)->value.file.name
#define FILE_TYPE(F)       (F)->value.file.type
#define FILE_CODE(F)       (F)->value.file.code_default
#define FILE_DATA(F)       (F)->value.file.data_default
#define FILE_CODE_ADDR(F)  (F)->value.file.page_address
#define FILE_DATA_ADDR(F)  (F)->value.file.bank_address
#define GOTO_NAME(G)       (G)->value._goto.name
#define HEAD_NAME(H)       (H)->value.head.name
#define HEAD_ARGS(H)       (H)->value.head.args
#define LABEL_NAME(L)      (L)->value.label.name
#define LOOP_INIT(L)       (L)->value.loop.init
#define LOOP_EXIT(L)       (L)->value.loop.exit
#define LOOP_INCR(L)       (L)->value.loop.incr
#define LOOP_BODY(L)       (L)->value.loop.body
#define RECORD_NAME(R)     (R)->value.record.name 
#define RECORD_LIST(R)     (R)->value.record.list
#define SUB_HEAD(S)        (S)->value.subprogram.head
#define SUB_RET(S)         (S)->value.subprogram.ret
#define SUB_BODY(S)        (S)->value.subprogram.body
#define SYM_NAME(T)        (T)->value.symbol.name
#define SYM_OFST(T)        (T)->value.symbol.offset
#define TYPE_TYPE(T)       (T)->value.type.type
#define TYPE_ACCESS(T)     (T)->value.type.is_access
#define TYPE_START(T)      (T)->value.type.start
#define TYPE_END(T)        (T)->value.type.end
#define TYPE_LIST(T)       (T)->value.type.list
#define TYPE_OF(T)         (T)->value.type.of
#define UNOP_OP(B)         (B)->value.unop.op
#define UNOP_ARG(B)        (B)->value.unop.p0
#define WITH_NAME(B)       (B)->value.with.name

#define COPY_DEBUG(x, y) y->line_number = x->line_number;\
                         y->file_id = x->file_id;

void init_nodes(void);
void free_nodes(void);

tree *mk_node(enum node_tag tag);

tree *mk_alias(char *alias, tree *expr);
tree *mk_arg(char *name, enum node_dir dir, char *type);
tree *mk_assembly(char *assembly);
tree *mk_attrib(char *name, enum attrib_type type);
tree *mk_body(tree *decl, tree *statements);
tree *mk_binop(enum node_op op, tree *p0, tree *p1);
tree *mk_call(char *name, tree *args);
tree *mk_constant(int value);
tree *mk_cond(tree *cond, tree *body, tree *next);
tree *mk_decl(char *name,
              gp_boolean is_volatile,
              gp_boolean is_constant,
              char *type,
              tree *init,
              tree *addr);
tree *mk_file(tree *body, char *name, enum source_type type);
tree *mk_goto(char *name);
tree *mk_head(char *name, tree *args);
tree *mk_label(char *name);
tree *mk_loop(tree *init, tree *exit, tree *incr, tree *body);
tree *mk_pragma(tree *pragma);
tree *mk_record(char *name, tree *list);
tree *mk_return(tree *ret);
tree *mk_string(char *value);
tree *mk_subprogram(tree *head, char *ret, tree *body);
tree *mk_symbol(char *name, tree *offset);
tree *mk_type(char *type, 
              gp_boolean is_access,
              tree *start,
              tree *end,
              tree *list,
              char *of);
tree *mk_unop(enum node_op op, tree *p0);
tree *mk_with(char *name);

tree *node_list(tree *head, tree *tail);

char *find_node_name(tree *node);
tree *find_node(tree *search, char *name, enum node_tag tag);

void print_node(tree *node, int level, gp_boolean print_list);

#endif

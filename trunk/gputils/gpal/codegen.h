/* code generation
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

#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#define WORKING_LABEL "_working_data"

extern int codegen_bytes;

void codegen_write_asm(const char *format, ...);
void codegen_write_comment(const char *format, ...);
void codegen_line_number(tree *node);
char *codegen_next_label(void);
void codegen_write_label(char *label);
void codegen_write_equ(char *label, int value);
void codegen_set_bank(char *bank_name);
void codegen_set_ibank(char *ibank_name);
void codegen_set_page(char *page_name);
char *codegen_get_offset_buffer(int offset);

void codegen_jump(char *label);
void codegen_call(char *label, enum node_storage storage);
void codegen_assembly(tree *assembly);

void codegen_test(tree *node, char *label, enum size_tag size);
void codegen_expr(tree *statement, enum size_tag size);
void codegen_unop(struct variable *var,
                  gp_boolean constant_offset,
                  int offset,
                  tree *offset_expr,
                  tree *unop,
                  enum size_tag size);
void codegen_load_indirect(struct variable *var);
void codegen_calc_indirect(tree *offset,
                           struct variable *var,
                           int element_size,
                           gp_boolean new_expr);
void codegen_store(struct variable *var,
                   gp_boolean access,
                   gp_boolean constant_offset,
                   int offset,
                   tree *offset_expr);

void codegen_init_proc(char *name, 
                       enum node_storage storage,
                       gp_boolean is_func);
void codegen_finish_proc(gp_boolean add_return);


void codegen_init_data(void);
void codegen_write_data(struct variable *var);
void codegen_temp_data(void);
char *codegen_get_temp(enum size_tag size);

void codegen_init_asm(tree *module);
void codegen_close_asm(void);
void codegen_select(tree *expr);

typedef struct {
  /* function pointers */
  long int codegen;
  long int unopgen;
  long int load_constant;
  long int load_file;
  long int store_file;
  long int load_address;
  long int load_fsr;
  long int offset_fsr;
  long int load_indirect;
  long int store_indirect;
  long int reset_vector;
  long int interrupt_vector;      
  /* device data */
  enum size_tag data_ptr_size;
  char *data_ptr_type;
  enum size_tag prog_ptr_size;
  char *prog_ptr_type;
  gp_boolean use_bankisel;
  gp_boolean use_pagesel;
} target_type;

extern target_type *target;

#endif

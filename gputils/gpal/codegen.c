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

#include "stdhdr.h"

#include "libgputils.h"
#include "gpal.h"
#include "analyze.h" 
#include "codegen.h"
#include "codegen14.h"
#include "codegen16e.h"

/* prototypes */
static void gen_expr(tree *expr);

static int label_number;
static int temp_number;
static int max_temp_number;
static int codegen_working_bytes;
static enum size_tag codegen_size;
int codegen_bytes;

target_type *target = NULL;

/* code generation function pointer */
typedef void code_func(enum node_op op, 
                       enum size_tag size,
                       gp_boolean is_const,
                       int value,
                       char *name,
                       char *bank_addr);

/* unop code generation function pointer */
typedef void code_unop(enum node_op op,
                       gp_boolean direct,
                       char *name,
                       char *bank_addr,
                       enum size_tag size,
                       int offset);

/* load constant function pointer */
typedef void data_func1(int value, enum size_tag size);

/* data movement function pointers */
typedef void data_func2(char *name,
                        char *bank_addr,
                        enum size_tag size,
                        int offset);

/* load address function pointer */
typedef void data_func3(char *name, enum size_tag size);

/* variable operation function pointers */
typedef void var_func(struct variable *var);

#define CODEGEN (*(code_func*)target->codegen)
#define UNOPGEN (*(code_unop*)target->unopgen)
#define LOAD_CONSTANT (*(data_func1*)target->load_constant)
#define LOAD_FILE (*(data_func2*)target->load_file)
#define STORE_FILE (*(data_func2*)target->store_file)
#define LOAD_ADDRESS (*(data_func3*)target->load_address)
#define LOAD_FSR (*(var_func*)target->load_fsr)
#define OFFSET_FSR (*(var_func*)target->offset_fsr)
#define LOAD_INDIRECT (*(data_func2*)target->load_indirect)
#define STORE_INDIRECT (*(data_func2*)target->store_indirect)
#define RESET_VECTOR (*(var_func*)target->reset_vector)
#define INT_VECTOR (*(var_func*)target->interrupt_vector)

#define LOCAL_BANK FILE_DATA_ADDR(state.module)

/****************************************************************************/
/* Common Assembly Functions                                                */
/****************************************************************************/

void 
codegen_write_asm(const char *format, ...)
{
  va_list args;
  char buffer[BUFSIZ]; 

  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  fprintf(state.output.f, "  %s\n", buffer);

  return;
}

void 
codegen_write_comment(const char *format, ...)
{
  va_list args;
  char buffer[BUFSIZ]; 

  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
   
  fprintf(state.output.f, "; %s\n", buffer);

  return;
}

void
codegen_line_number(tree *node)
{
  fprintf(state.output.f, "  .line %d\n", node->line_number);
}

char *
codegen_next_label(void)
{
  char label[BUFSIZ];
  snprintf(label, sizeof(label), "_%i", label_number++);
  return strdup(label);
}

void
codegen_write_label(char *label)
{

  /* Clear the bank setting, because current bank is unknown. */
  state.current_bank = NULL;
  state.current_ibank = NULL;

  fprintf(state.output.f, "%s:\n", label);
}

void
codegen_write_equ(char *label, int value)
{
  fprintf(state.output.f, "%-30s equ %#x\n", label, value);
}

void
codegen_set_bank(char *bank_name)
{
  struct variable *var;

  assert(bank_name != NULL);

  if ((state.current_bank == NULL) || 
      (strcasecmp(bank_name, state.current_bank) != 0)) {
    codegen_write_asm("banksel %s", bank_name);
    state.current_bank = bank_name;
    var = get_global(bank_name);
    if (var) {
      var->is_used = true;
    }
  }

}

void
codegen_set_ibank(char *ibank_name)
{
  struct variable *var;

  assert(ibank_name != NULL);

  if ((target->use_bankisel) &&
      ((state.current_ibank == NULL) || 
       (strcasecmp(ibank_name, state.current_ibank) != 0))) {
    codegen_write_asm("bankisel %s", ibank_name);
    state.current_ibank = ibank_name;
    var = get_global(ibank_name);
    if (var) {
      var->is_used = true;
    }
  }

}

void
codegen_set_page(char *page_name)
{
  struct variable *var;

  assert(page_name != NULL);

  if ((target->use_pagesel) &&
      ((state.current_page == NULL) || 
       (strcasecmp(page_name, state.current_page) != 0))) {
    codegen_write_asm("pagesel %s", page_name);
    state.current_page = page_name;
    var = get_global(page_name);
    if (var) {
      var->is_used = true;
    }
  }

}

/* Generate offset buffer string.  Static buffer used, so not thread-safe */

char *
codegen_get_offset_buffer(int offset)
{
  static char buffer[64];

  if (offset == 0) {
    buffer[0] = '\0';
  } else if (offset < 0) {
    snprintf(buffer, sizeof(buffer), " - %#x", -offset);
  } else {
    snprintf(buffer, sizeof(buffer), " + %#x", offset);
  }

  return buffer;
}

/****************************************************************************/
/* Common directives and instructions                                       */
/****************************************************************************/

void
codegen_jump(char *label)
{
  codegen_write_asm("goto %s", label);
}

void
codegen_call(char *label, enum node_storage storage)
{
 
  if (storage == storage_far) {
    codegen_write_asm("pagesel %s", label);
  }
  codegen_write_asm("call %s", label);

}

void
codegen_assembly(tree *assembly)
{

  fprintf(state.output.f, "%s\n", assembly->value.assembly);

  return;
}

/****************************************************************************/
/* Memory access                                                            */
/****************************************************************************/

/* load the address for indirect access */

void
codegen_load_indirect(struct variable *var)
{

  LOAD_FSR(var);

  return;
}

/* calculate the byte address of the data and put it in FSR */

void
codegen_calc_indirect(tree *offset,
                      struct variable *var,
                      int element_size,
                      gp_boolean new_expr)
{

  /* indirect access */
  if (new_expr) {
    codegen_expr(offset, target->data_ptr_size);
  } else {
    gen_expr(offset);
  }

  if (var->type->start != 0) {
    /* shift the offset for the first element */
    CODEGEN(op_add, target->data_ptr_size, true, 0 - var->type->start, NULL, NULL);
  }

  if (element_size != 1) {
    /* scale the offset by the element size */
    CODEGEN(op_mult, target->data_ptr_size, true, element_size, NULL, NULL);
  }

  OFFSET_FSR(var);

  return;
}

/* load the register file into the working register */

static void
codegen_load_file(tree *symbol, struct variable *var)
{
  int offset;
  int element_size;
  char *bank_addr = var_bank(var);

  if ((symbol->tag == tag_symbol) && (SYM_OFST(symbol))) {
    /* access an array */
    element_size = type_bytes(var->type->prim);

    if ((var) && (var->type) && (var->type->tag == type_array)) {
      if (can_evaluate(SYM_OFST(symbol), false)) {
        /* direct access */
        offset = analyze_check_array(symbol, var) * element_size;
        LOAD_FILE(var->name, bank_addr, codegen_size, offset);
      } else {
        codegen_calc_indirect(SYM_OFST(symbol), var, element_size, false);
        LOAD_INDIRECT(var->name, bank_addr, codegen_size, 0);
      }
    } else {
      analyze_error(symbol, "symbol %s is not an array",
                    SYM_NAME(symbol));
    }
  } else {
    LOAD_FILE(var->name, bank_addr, codegen_size, 0);
  }

  return;
}

/* store the working register in memory */

void
codegen_store(struct variable *var,
              gp_boolean access,
              gp_boolean constant_offset,
              int offset,
              tree *offset_expr)
{
  char *bank_addr = var_bank(var);

  if (access) {
    STORE_INDIRECT(var->name, bank_addr, codegen_size, 0);
  } else if (offset_expr) {
    if (constant_offset) {
      STORE_FILE(var->name, bank_addr, codegen_size, offset);
    } else {
      STORE_INDIRECT(var->name, bank_addr, codegen_size, 0);
    }
  } else {
    STORE_FILE(var->name, bank_addr, codegen_size, 0);
  }

}

/****************************************************************************/
/* Expression generation                                                    */
/****************************************************************************/

static void
gen_unop_expr(tree *expr)
{

  gen_expr(expr->value.unop.p0);
  switch (expr->value.unop.op) {
  case op_add:
    break;
  case op_not:
  case op_neg:
  case op_com:
    CODEGEN(expr->value.unop.op, codegen_size, true, 0, NULL, NULL);
    break;
  default:
    assert(0);
  }

}

static void
gen_binop_expr(enum node_op op, tree *p0, tree *p1)
{
  char *reg1 = NULL;
  char *reg2 = NULL;
  struct variable *var;
  char *local_bank_addr = LOCAL_BANK;

  gen_expr(p1);

  if (p0->tag == tag_call) {
    reg1 = codegen_get_temp(codegen_size);
    STORE_FILE(reg1, local_bank_addr, codegen_size, 0);
    analyze_call(p0, true, codegen_size);    
    CODEGEN(op, codegen_size, false, 0, reg1, local_bank_addr);
  } else if (p0->tag == tag_constant) {
    CODEGEN(op, codegen_size, true, p0->value.constant, NULL, NULL);
  } else if (p0->tag == tag_symbol) { 
    var = get_global(SYM_NAME(p0));
    if (var->tag == sym_const) {
      CODEGEN(op, codegen_size, true, var->value, NULL, NULL);
    } else if (SYM_OFST(p0)) {
      /* it is a complex expression, so save temp data */
      reg1 = codegen_get_temp(codegen_size);
      reg2 = codegen_get_temp(codegen_size);
      STORE_FILE(reg1, local_bank_addr, codegen_size, 0);
      codegen_load_file(p0, var);
      STORE_FILE(reg2, local_bank_addr, codegen_size, 0);
      LOAD_FILE(reg1, local_bank_addr, codegen_size, 0);
      CODEGEN(op, codegen_size, false, 0, reg2, local_bank_addr);
    } else {
      CODEGEN(op, codegen_size, false, 0, var->name, var_bank(var));
    }
  } else {
    /* it is a complex expression so save temp data */
    reg1 = codegen_get_temp(codegen_size);
    STORE_FILE(reg1, local_bank_addr, codegen_size, 0);
    gen_expr(p0);
    CODEGEN(op, codegen_size, false, 0, reg1, local_bank_addr);
  }

  if (reg1)
    free(reg1);
  if (reg2)
    free(reg2);

}

static void
gen_expr(tree *expr)
{
  struct variable *var;
  char *bank_addr;

  switch(expr->tag) {
  case tag_arg:
    var = get_global(ARG_NAME(expr));
    codegen_load_file(expr, var);
    break;
  case tag_attrib:
    var = get_global(ATTRIB_NAME(expr));
    if (ATTRIB_TYPE(expr) == attrib_access) {
      LOAD_FSR(var);
      bank_addr = var_bank(var);
      LOAD_INDIRECT(var->name, bank_addr, codegen_size, 0);
    } else if (ATTRIB_TYPE(expr) == attrib_address) {
      LOAD_ADDRESS(var->name, codegen_size);
    } else {
      LOAD_CONSTANT(maybe_evaluate(expr), codegen_size);
    }
    break;
  case tag_call:
    analyze_call(expr, true, codegen_size);    
    break;
  case tag_constant:
    LOAD_CONSTANT(expr->value.constant, codegen_size);
    break;
  case tag_symbol:
    var = get_global(SYM_NAME(expr));
    if (var->tag == sym_const) {
      LOAD_CONSTANT(var->value, codegen_size); 
    } else if (var->tag == sym_alias) {
      gen_expr(var->node);
    } else {
      codegen_load_file(expr, var);
    }
    break;
  case tag_unop:
    gen_unop_expr(expr);
    break;
  case tag_binop:
    if (expr->value.binop.op == op_assign) {
      analyze_error(expr, "assign operator = should be equal operator ==");
    } else if ((expr->value.binop.op == op_lsh) ||
               (expr->value.binop.op == op_rsh)) {
      /* for shifts it is best to calculate the left side first */
      gen_binop_expr(expr->value.binop.op,
                     expr->value.binop.p1,
                     expr->value.binop.p0);
    } else {
      /* for all others calculate the right side first */
      gen_binop_expr(expr->value.binop.op,
                     expr->value.binop.p0,
                     expr->value.binop.p1);
    }
    break;
  default:
    assert(0);
  }

}

/* Store the number of bytes required for the working register */

static int
codegen_setup(enum size_tag size)
{
  codegen_size = size;
  codegen_bytes = prim_bytes(size);

  if (codegen_bytes > codegen_working_bytes) {
    codegen_working_bytes = codegen_bytes;
  }
  
  if (codegen_bytes != 1) {
    /* The w register isn't used as the working register so save data
       memory. */
    return codegen_bytes;
  }

  return 0;
}

void
codegen_test(tree *node, char *label, enum size_tag size)
{

  temp_number = codegen_setup(size);

  gen_expr(node);
  codegen_write_asm("btfsc STATUS, Z");
  codegen_write_asm("goto %s", label);

  if (temp_number > max_temp_number)
    max_temp_number = temp_number;

}

void
codegen_expr(tree *statement, enum size_tag size)
{

  temp_number = codegen_setup(size);

  gen_expr(statement);

  if (temp_number > max_temp_number)
    max_temp_number = temp_number;

  return;
}

/* perform the unop on the file register not the working register */

void
codegen_unop(struct variable *var,
             gp_boolean constant_offset,
             int offset,
             tree *offset_expr,
             tree *unop,
             enum size_tag size)
{
  char *bank_addr = var_bank(var);

  temp_number = codegen_setup(size);

  if (offset_expr) {
    if (constant_offset) {
      UNOPGEN(UNOP_OP(unop), true, var->name, bank_addr, codegen_size, offset);
    } else {
      UNOPGEN(UNOP_OP(unop), false, var->name, bank_addr, codegen_size, 0);
    }
  } else {
    UNOPGEN(UNOP_OP(unop), true, var->name, bank_addr, codegen_size, 0);
  }

  if (temp_number > max_temp_number)
    max_temp_number = temp_number;

}

/****************************************************************************/
/* Write procedures and functions                                           */
/****************************************************************************/

void
codegen_init_proc(char *name, enum node_storage storage, gp_boolean is_func)
{
  if (is_func) {
    codegen_write_comment("function %s", name);
  } else {
    codegen_write_comment("procedure %s", name);
  }

  codegen_write_label(name);
  if (storage == storage_public)
    codegen_write_asm("global %s", name);

}

void
codegen_finish_proc(gp_boolean add_return)
{
  if (add_return) {
    codegen_write_asm("return");
  }
  fprintf(state.output.f, "\n");
}

/****************************************************************************/
/* Write data memory section and externs                                    */
/****************************************************************************/

void
codegen_init_data()
{
  codegen_write_comment("declarations");

  if (state.section.data) {
    if (state.section.data_addr_valid) {
      fprintf(state.output.f, "%s udata %#x\n", 
              state.section.data,
              state.section.data_addr);
    } else {
      fprintf(state.output.f, "%s udata\n", state.section.data);
    }
  } else {
    if (state.section.data_addr_valid) {
      fprintf(state.output.f, ".udata_%s udata %#x\n",
              FILE_NAME(state.module),
              state.section.data_addr);
    } else {
      fprintf(state.output.f, ".udata_%s udata\n", FILE_NAME(state.module));
    }  
  }

  codegen_write_label(LOCAL_BANK);
  codegen_write_asm("global %s", LOCAL_BANK);
}

void
codegen_write_data(struct variable *var)
{

  fprintf(state.output.f, "  .def %s, ", var->name);

  if (var->storage == storage_public) {
    fprintf(state.output.f, "global, ");
  } else {
    fprintf(state.output.f, "static, ");
  }

  fprintf(state.output.f, "size=%#x, type=%#x\n",
          type_bytes(var->type),
          var_coff_type(var));

}

void
codegen_temp_data()
{
  int i;

  /* extended math used so a working register is required */
  if (codegen_working_bytes != 1) {  
    codegen_write_label(WORKING_LABEL);
  }

  for (i = 0; i < max_temp_number; i++) 
    fprintf(state.output.f, "_%s_temp_%d res 1\n", FILE_NAME(state.module), i);

}

char *
codegen_get_temp(enum size_tag size)
{
  char temp_name[BUFSIZ];
 
  snprintf(temp_name,
           sizeof(temp_name),
           "_%s_temp_%d",
           FILE_NAME(state.module),
           temp_number);

  temp_number += prim_bytes(size);

  return strdup(temp_name);
}

/****************************************************************************/
/* File operations                                                          */
/****************************************************************************/

void
codegen_init_asm(tree *module)
{
  char asmfilename[BUFSIZ];
  char buffer[BUFSIZ];

  /* open output filename */
  strncpy(asmfilename, FILE_NAME(module), sizeof(asmfilename));
  strncat(asmfilename, ".asm", sizeof(asmfilename));
  state.output.f = fopen(asmfilename, "w");
  if (state.output.f == NULL) {
    perror(asmfilename);
    exit(1);
  }

  label_number = 0;
  max_temp_number = 0;
  codegen_working_bytes = 1;
  codegen_size = size_uint8;
  codegen_bytes = prim_bytes(codegen_size);

  gp_date_string(buffer, sizeof(buffer));

  fprintf(state.output.f, "; %s\n", asmfilename);
  fprintf(state.output.f, "; generated by %s on %s\n\n",
          GPAL_VERSION_STRING,
          buffer);

  fprintf(state.output.f, "  .ident \"%s\"\n\n", GPAL_VERSION_STRING);

  fprintf(state.output.f, "  .file \"%s\"\n\n",
          get_file_name(module->file_id));

  if (state.processor_chosen) {
    fprintf(state.output.f, "  processor %s\n", 
            state.processor_info->names[1]);
    fprintf(state.output.f, "  include \"%s.inc\"\n\n",
            state.processor_info->names[1]);
  } else {
    gp_error("processor not selected");
  } 

  if (state.section.code) {
    if (state.section.code_addr_valid) {
      fprintf(state.output.f, "%s code %#x\n", 
              state.section.code,
              state.section.code_addr);
    } else {
      fprintf(state.output.f, "%s code\n", state.section.code);
    }
  } else {
    if (state.section.code_addr_valid) {
      fprintf(state.output.f, ".code_%s code %#x\n", 
              FILE_NAME(module),
              state.section.code_addr);
    } else {
      fprintf(state.output.f, ".code_%s code\n", FILE_NAME(module));
    }
  }

  codegen_write_label(FILE_CODE_ADDR(module));
  codegen_write_asm("global %s", FILE_CODE_ADDR(module));

  return;
}

void
codegen_close_asm(void)
{
  struct variable *var;

  var = get_global("main");
  if ((var) && (var->node->tag == tag_subprogram)) {
    RESET_VECTOR(var);
  }

  var = get_global("isr");
  if ((var) && (var->node->tag == tag_subprogram)) {
    INT_VECTOR(var);
  }

  fprintf(state.output.f, "  .eof\n\n");
  fprintf(state.output.f, "  end\n\n");

  fclose(state.output.f);

  return;
}

void
codegen_select(tree *expr)
{

  switch (state.class) {
  case PROC_CLASS_EEPROM8:
  case PROC_CLASS_GENERIC:
  case PROC_CLASS_PIC12:
  case PROC_CLASS_SX:
    analyze_error(expr, "unsupported device");
    break;
  case PROC_CLASS_PIC14:
    target = &pic14;
    break;
  case PROC_CLASS_PIC16:
    analyze_error(expr, "unsupported device");
    break;
  case PROC_CLASS_PIC16E:
    target = &pic16e;
    break;
  default:
    assert(0);
  }

}

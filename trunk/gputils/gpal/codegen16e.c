/* code generation for enhanced 16 bit pics
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
#include "codegen.h"
#include "codegen16e.h"

/****************************************************************************/
/* Common                                                                   */
/****************************************************************************/

#define BANKSEL       codegen_set_bank(bank_addr)
#define BANKSEL_LOCAL codegen_set_bank(FILE_DATA_ADDR(state.module))
#define BANKISEL      codegen_set_ibank(bank_addr)

/* load a constant into the working register */

static void
load_constant16e(int value, enum size_tag size)
{
  int num_bytes;
  
  if ((size == size_int8) || (size == size_uint8)) {
    codegen_write_asm("movlw %#x", value & 0xff);
    return;
  }

  /* FIXME: use codegen_bytes */
  num_bytes = prim_bytes(size);

  BANKSEL_LOCAL;

  switch (num_bytes) {
  case 4:
    codegen_write_asm("movlw %#x", (value >> 24) & 0xff);
    codegen_write_asm("movwf %s + 3", WORKING_LABEL);
    /* fall through */
  case 3:
    codegen_write_asm("movlw %#x", (value >> 16) & 0xff);
    codegen_write_asm("movwf %s + 2", WORKING_LABEL);
    /* fall through */
  case 2:
    codegen_write_asm("movlw %#x", (value >> 8) & 0xff);
    codegen_write_asm("movwf %s + 1", WORKING_LABEL);
    /* fall through */
  case 1:
    codegen_write_asm("movlw %#x", value & 0xff);
    codegen_write_asm("movwf %s", WORKING_LABEL);
  }

}

/* load a file into the working register */

static void
load_file16e(char *name,
             char *bank_addr,
             enum size_tag size,
             int offset)
{
  int num_bytes;
  char *offset_buffer = codegen_get_offset_buffer(offset);
  
  /* W is used as the working register for single byte types. */
  if ((size == size_int8) || (size == size_uint8)) {
    BANKSEL;
    codegen_write_asm("movf %s%s, w", name, offset_buffer);
    return;
  }

  num_bytes = prim_bytes(size);

  switch (num_bytes) {
  case 4:
    codegen_write_asm("movff %s%s + 3, %s + 3",
                      name,
                      offset_buffer,
                      WORKING_LABEL);
    /* fall through */
  case 3:
    codegen_write_asm("movff %s%s + 2, %s + 2",
                      name,
                      offset_buffer,
                      WORKING_LABEL);
    /* fall through */
  case 2:
    codegen_write_asm("movff %s%s + 1, %s + 1",
                      name,
                      offset_buffer,
                      WORKING_LABEL);
    /* fall through */
  case 1:
    codegen_write_asm("movff %s%s, %s",
                      name,
                      offset_buffer,
                      WORKING_LABEL);
  }

}

/* store the working register to a file */

static void
store_file16e(char *name,
              char *bank_addr,
              enum size_tag size,
              int offset)
{
  int num_bytes;
  char *offset_buffer = codegen_get_offset_buffer(offset);
  
  if ((size == size_int8) || (size == size_uint8)) {
    BANKSEL;
    codegen_write_asm("movwf %s%s", name, offset_buffer);
    return;
  }

  num_bytes = prim_bytes(size);

  switch (num_bytes) {
  case 4:
    codegen_write_asm("movff %s + 3, %s%s + 3",
                      WORKING_LABEL,
                      name,
                      offset_buffer);
    /* fall through */
  case 3:
    codegen_write_asm("movff %s + 2, %s%s + 2",
                      WORKING_LABEL,
                      name,
                      offset_buffer);
    /* fall through */
  case 2:
    codegen_write_asm("movff %s + 1, %s%s + 1",
                      WORKING_LABEL,
                      name,
                      offset_buffer);
    /* fall through */
  case 1:
    codegen_write_asm("movff %s, %s%s",
                      WORKING_LABEL,
                      name,
                      offset_buffer);
  }

}

/* load an address into the working register */

static void
load_addr16e(char *name, enum size_tag size)
{
  int num_bytes;
  
  if ((size == size_int8) || (size == size_uint8)) {
    codegen_write_asm("movlw low %s", name);
    return;
  }

  /* FIXME: use codegen_bytes */
  num_bytes = prim_bytes(size);

  BANKSEL_LOCAL;

  switch (num_bytes) {
  case 4:
    assert(0);
    /* fall through */
  case 3:
    codegen_write_asm("movlw upper %s", name);
    codegen_write_asm("movwf %s + 2", WORKING_LABEL);
    /* fall through */
  case 2:
    codegen_write_asm("movlw high %s", name);
    codegen_write_asm("movwf %s + 1", WORKING_LABEL);
    /* fall through */
  case 1:
    codegen_write_asm("movlw low %s", name);
    codegen_write_asm("movwf %s", WORKING_LABEL);
  }

}

/* Load the address contained in memory to the FSR */

static void
load_fsr16e(struct variable *var)
{
  codegen_write_asm("movff %s, FSR0L", var->name);
  codegen_write_asm("movff %s, FSR0H", var->name);
}

/* Offset the FSR with a variable address */

static void
offset_fsr16e(struct variable *var)
{
  codegen_write_asm("lfsr FSR0, %s", var->name);
  codegen_write_asm("movf %s, w", WORKING_LABEL);
  codegen_write_asm("addwf FSR0L, f");
  codegen_write_asm("movf %s + 1, w", WORKING_LABEL);
  codegen_write_asm("addwfc FSR0H, f");
}

/* The byte address is in FSR.  Load the data to the working register */

static void
load_indirect16e(char *name,
                 char *bank_addr,
                 enum size_tag size,
                 int offset)
{
  int num_bytes;
  int i;

  if ((size == size_int8) || (size == size_uint8)) {
    codegen_write_asm("movf INDF0, W");
    return;
  }

  num_bytes = prim_bytes(size);

  codegen_write_asm("movlw 0");

  for (i = 0; i < num_bytes; i++) {
    codegen_write_asm("movff POSTINC0, %s + %i", WORKING_LABEL, i);
  }
}

/* The byte address is in FSR.  Store the working register in memory. */

static void
store_indirect16e(char *name,
                  char *bank_addr,
                  enum size_tag size,
                  int offset)
{
  int num_bytes;
  int i;

  if ((size == size_int8) || (size == size_uint8)) {
    codegen_write_asm("movwf INDF0");
    return;
  }

  num_bytes = prim_bytes(size);

  codegen_write_asm("movlw 0");

  for (i = 0; i < num_bytes; i++) {
    codegen_write_asm("movff %s + %i, POSTINC0", WORKING_LABEL, i);
  }
}

/* convert the working register into a boolean */

static void
gen_boolean(enum size_tag size)
{
  int num_bytes;
  int i;

  if ((size == size_uint8) || (size == size_uint8)) {
    codegen_write_asm("btfss STATUS, Z");
    codegen_write_asm("movlw 1");
    return;
  }

  num_bytes = prim_bytes(size);

  BANKSEL_LOCAL;
  codegen_write_asm("movf %s, w", WORKING_LABEL);

  for (i = 1; i < num_bytes; i++) {
    codegen_write_asm("iorwf %s + %i, w", WORKING_LABEL, i);
  }

  codegen_write_asm("btfss STATUS, Z");
  codegen_write_asm("movlw 1");

}

/* move w to the working register */

static void
move_to_working(enum size_tag size)
{
  int num_bytes;

  num_bytes = prim_bytes(size);

  BANKSEL_LOCAL;
  switch (num_bytes) {
  case 4:
    codegen_write_asm("clrf %s + 3", WORKING_LABEL);
    /* fall through */
  case 3:
    codegen_write_asm("clrf %s + 2", WORKING_LABEL);
    /* fall through */
  case 2:
    codegen_write_asm("clrf %s + 1", WORKING_LABEL);
    codegen_write_asm("movwf %s", WORKING_LABEL);
    codegen_write_asm("xorlw 0"); /* set the Z flag properly */
  }

}

/****************************************************************************/
/* Unops                                                                    */
/****************************************************************************/

static void
clr_direct16e(char *name,
              char *bank_addr,
              enum size_tag size,
              int offset)
{
  char *offset_buffer = codegen_get_offset_buffer(offset);

  BANKSEL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    codegen_write_asm("clrf %s%s", name, offset_buffer);
    break;
  case size_uint16:
  case size_int16:
    codegen_write_asm("clrf %s%s", name, offset_buffer);
    codegen_write_asm("clrf %s%s + 1", name, offset_buffer);
    break;
  case size_uint24:
  case size_int24:
    codegen_write_asm("clrf %s%s", name, offset_buffer);
    codegen_write_asm("clrf %s%s + 1", name, offset_buffer);
    codegen_write_asm("clrf %s%s + 2", name, offset_buffer);
    break;
  case size_uint32:
  case size_int32:
    codegen_write_asm("clrf %s%s", name, offset_buffer);
    codegen_write_asm("clrf %s%s + 1", name, offset_buffer);
    codegen_write_asm("clrf %s%s + 2", name, offset_buffer);
    codegen_write_asm("clrf %s%s + 3", name, offset_buffer);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
clr_indirect16e(char *name,
                char *bank_addr,
                enum size_tag size,
                int offset)
{
  int num_bytes;
  int i;

  num_bytes = prim_bytes(size);

  for (i = 0; i < num_bytes; i++) {
    codegen_write_asm("clrf POSTINC0");
  }

}

static void
inc_direct16e(char *name,
              char *bank_addr,
              enum size_tag size,
              int offset)
{
  char *offset_buffer = codegen_get_offset_buffer(offset);
  char *label = NULL;

  BANKSEL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    codegen_write_asm("incf %s%s, f", name, offset_buffer);
    break;
  case size_uint16:
  case size_int16:
    codegen_write_asm("infsnz %s%s, f", name, offset_buffer);
    codegen_write_asm("incf %s%s + 1, f", name, offset_buffer);
    break;
  case size_uint24:
  case size_int24:
    label = codegen_next_label();
    codegen_write_asm("incfsz %s%s, f", name, offset_buffer);
    codegen_write_asm("bra %s", label);
    codegen_write_asm("infsnz %s%s + 1, f", name, offset_buffer);
    codegen_write_asm("incf %s%s + 2, f", name, offset_buffer);
    codegen_write_label(label);
    break;
  case size_uint32:
  case size_int32:
    label = codegen_next_label();
    codegen_write_asm("incf %s%s, f", name, offset_buffer);
    codegen_write_asm("bnc %s", label);
    codegen_write_asm("clrf WREG");
    codegen_write_asm("addwfc %s%s + 1, f", name, offset_buffer);
    codegen_write_asm("addwfc %s%s + 2, f", name, offset_buffer);
    codegen_write_asm("addwfc %s%s + 3, f", name, offset_buffer);
    codegen_write_label(label);
    break;
  case size_float:
  default:
    assert(0);
  }

  if (label)
    free(label);

}

static void
inc_indirect16e(char *name,
                char *bank_addr,
                enum size_tag size,
                int offset)
{
  char *label = codegen_next_label();
  
  switch (prim_bytes(size)) {
  case 1:
    codegen_write_asm("incf INDF0, f");
    break;
  case 2:
    codegen_write_asm("infsnz POSTINC0, f");
    codegen_write_asm("incf INDF0, f");
    break;
  case 3:
    codegen_write_asm("incfsz POSTINC0, f");
    codegen_write_asm("bra %s", label);
    codegen_write_asm("infsnz POSTINC0, f");
    codegen_write_asm("incf INDF0, f");
    codegen_write_label(label);
    break;
  case 4:
    codegen_write_asm("incf POSTINC0, f");
    codegen_write_asm("bnc %s", label);
    codegen_write_asm("clrf WREG");
    codegen_write_asm("addwfc POSTINC0, f");
    codegen_write_asm("addwfc POSTINC0, f");
    codegen_write_asm("addwfc INDF0, f");
    codegen_write_label(label);
    break;
  default:
    assert(0);
  }

  if (label)
    free(label);
}

static void
dec_direct16e(char *name,
              char *bank_addr,
              enum size_tag size,
              int offset)
{
  char *offset_buffer = codegen_get_offset_buffer(offset);
  char *label = NULL;

  BANKSEL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    codegen_write_asm("decf %s%s, f", name, offset_buffer);
    break;
  case size_uint16:
  case size_int16:
    codegen_write_asm("decf %s%s, f", name, offset_buffer);
    codegen_write_asm("btfss STATUS, C");
    codegen_write_asm("decf %s%s + 1, f", name, offset_buffer);
    break;
  case size_uint24:
  case size_int24:
    label = codegen_next_label();
    codegen_write_asm("decf %s%s, f", name, offset_buffer);
    codegen_write_asm("bc %s", label);
    codegen_write_asm("decf %s%s + 1, f", name, offset_buffer);
    codegen_write_asm("bc %s", label);
    codegen_write_asm("decf %s%s + 2, f", name, offset_buffer);
    codegen_write_label(label);
    break;
  case size_uint32:
  case size_int32:
    label = codegen_next_label();
    codegen_write_asm("decf %s%s, f", name, offset_buffer);
    codegen_write_asm("bc %s", label);
    codegen_write_asm("clrf WREG");
    codegen_write_asm("subwfb %s%s + 1, f", name, offset_buffer);
    codegen_write_asm("subwfb %s%s + 2, f", name, offset_buffer);
    codegen_write_asm("subwfb %s%s + 3, f", name, offset_buffer);
    codegen_write_label(label);
    break;
  case size_float:
  default:
    assert(0);
  }

  if (label)
    free(label);

}

static void
dec_indirect16e(char *name,
                char *bank_addr,
                enum size_tag size,
                int offset)
{
  char *label = NULL;

  switch (prim_bytes(size)) {
  case 1:
    codegen_write_asm("decf INDF0, f");
    break;
  case 2:
    codegen_write_asm("decf POSTINC0, f");
    codegen_write_asm("btfss STATUS, C");
    codegen_write_asm("decf INFD0, f");
    break;
  case 3:
    label = codegen_next_label();
    codegen_write_asm("decf POSTINC0, f");
    codegen_write_asm("bc %s", label);
    codegen_write_asm("decf POSTINC0, f");
    codegen_write_asm("bc %s", label);
    codegen_write_asm("decf INDF0, f");
    codegen_write_label(label);
    break;
  case 4:
    label = codegen_next_label();
    codegen_write_asm("decf POSTINC0, f");
    codegen_write_asm("bc %s", label);
    codegen_write_asm("clrf WREG");
    codegen_write_asm("subwfb POSTINC0, f");
    codegen_write_asm("subwfb POSTINC0, f");
    codegen_write_asm("subwfb INFD0, f");
    codegen_write_label(label);
    break;
  default:
    assert(0);
  }

  if (label)
    free(label);

}


/****************************************************************************/
/* Arithmetic                                                               */
/****************************************************************************/

/* add the lowest byte */
static void
add_setup(gp_boolean is_const, int value, char *name, char *bank_addr)
{
  codegen_write_asm("movf %s, w", WORKING_LABEL);
  if (is_const) {
    codegen_write_asm("addlw %#x", value);
  } else {
    BANKSEL;
    codegen_write_asm("addwf %s, w", name);
    BANKSEL_LOCAL;
  }
  codegen_write_asm("movwf %s", WORKING_LABEL);
}

/* add the next byte */
static void
add_next(gp_boolean is_const, int value, char *name, char *bank_addr, int byte)
{
  codegen_write_asm("movf %s + %i, w", WORKING_LABEL, byte);
  codegen_write_asm("btfss STATUS, C");
  codegen_write_asm("incfsz %s + %i, w", WORKING_LABEL, byte);
  if (is_const) {
    codegen_write_asm("addlw %#x", value);    
  } else {
    BANKSEL;
    codegen_write_asm("addwf %s + %i, w", name, byte);
    BANKSEL_LOCAL;
  }
  codegen_write_asm("movwf %s + %i", WORKING_LABEL, byte);
}

static void
do_add(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      codegen_write_asm("addlw %#x", value & 0xff);
    } else {
      BANKSEL;
      codegen_write_asm("addwf %s, w", name);
    }
    break;
  case size_uint16:
  case size_int16:
    BANKSEL_LOCAL;
    add_setup(is_const, (value & 0xff), name, bank_addr);
    add_next(is_const, (value >> 8) && 0xff, name, bank_addr, 1);
    break;
  case size_uint24:
  case size_int24:
    BANKSEL_LOCAL;
    add_setup(is_const, (value & 0xff), name, bank_addr);
    add_next(is_const, (value >> 8) && 0xff, name, bank_addr, 1);
    add_next(is_const, (value >> 16) && 0xff, name, bank_addr, 2);
    break;
  case size_uint32:
  case size_int32:
    BANKSEL_LOCAL;
    add_setup(is_const, (value & 0xff), name, bank_addr);
    add_next(is_const, (value >> 8) && 0xff, name, bank_addr, 1);
    add_next(is_const, (value >> 16) && 0xff, name, bank_addr, 2);
    add_next(is_const, (value >> 24) && 0xff, name, bank_addr, 3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

/* subtract the lowest byte */
static void
sub_setup(gp_boolean is_const, int value, char *name, char *bank_addr)
{
  codegen_write_asm("movf %s, w", WORKING_LABEL);
  if (is_const) {
    codegen_write_asm("sublw %#x", value);    
  } else {
    BANKSEL;
    codegen_write_asm("subwf %s, w", name);
    BANKSEL_LOCAL;
  }
  codegen_write_asm("movwf %s", WORKING_LABEL);
}

/* subtract the next byte */
static void
sub_next(gp_boolean is_const, int value, char *name, char *bank_addr, int byte)
{
  codegen_write_asm("movf %s + %i, w", WORKING_LABEL, byte);
  codegen_write_asm("btfss STATUS, C");
  codegen_write_asm("incfsz %s + %i, w", WORKING_LABEL, byte);
  if (is_const) {
    codegen_write_asm("sublw %#x", value);    
  } else {
    BANKSEL;
    codegen_write_asm("subwf %s + %i, w", name, byte);
    BANKSEL_LOCAL;
  }
  codegen_write_asm("movwf %s + %i", WORKING_LABEL, byte);
}

static void
do_sub(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      codegen_write_asm("sublw %#x", value);
    } else {
      BANKSEL;
      codegen_write_asm("subwf %s, w", name);
    }
    break;
  case size_uint16:
  case size_int16:
    BANKSEL_LOCAL;
    sub_setup(is_const, (value & 0xff), name, bank_addr);
    sub_next(is_const, (value >> 8) && 0xff, name, bank_addr, 1);
    break;
  case size_uint24:
  case size_int24:
    BANKSEL_LOCAL;
    sub_setup(is_const, (value & 0xff), name, bank_addr);
    sub_next(is_const, (value >> 8) && 0xff, name, bank_addr, 1);
    sub_next(is_const, (value >> 16) && 0xff, name, bank_addr, 2);
    break;
  case size_uint32:
  case size_int32:
    BANKSEL_LOCAL;
    sub_setup(is_const, (value & 0xff), name, bank_addr);
    sub_next(is_const, (value >> 8) && 0xff, name, bank_addr, 1);
    sub_next(is_const, (value >> 16) && 0xff, name, bank_addr, 2);
    sub_next(is_const, (value >> 24) && 0xff, name, bank_addr, 3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
do_neg(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
    do_sub(size, true, 0, NULL, NULL);
    break;
  case size_float:
  default:
    assert(0);
  }

}

/****************************************************************************/
/* Logical                                                                  */
/****************************************************************************/

static void
and_byte(gp_boolean is_const, int value, char *name, char *bank_addr, int byte)
{

  codegen_write_asm("movf %s + %i, w", WORKING_LABEL, byte);
  if (is_const) {
    codegen_write_asm("andlw %#x", value);
  } else {
    BANKSEL;
    codegen_write_asm("andwf %s, w", name);
    BANKSEL_LOCAL;
  }
  codegen_write_asm("movwf %s + %i", WORKING_LABEL, byte);

}

static void
do_and(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      codegen_write_asm("andlw %#x", value);
    } else {
      BANKSEL;
      codegen_write_asm("andwf %s, w", name);
    }
    break;
  case size_uint16:
  case size_int16:
    BANKSEL_LOCAL;
    and_byte(is_const, value & 0xff, name, bank_addr, 0);
    and_byte(is_const, (value >> 8) & 0xff, name, bank_addr, 1);
    break;
  case size_uint24:
  case size_int24:
    BANKSEL_LOCAL;
    and_byte(is_const, value & 0xff, name, bank_addr, 0);
    and_byte(is_const, (value >> 8) & 0xff, name, bank_addr, 1);
    and_byte(is_const, (value >> 16) & 0xff, name, bank_addr, 2);
    break;
  case size_uint32:
  case size_int32:
    BANKSEL_LOCAL;
    and_byte(is_const, value & 0xff, name, bank_addr, 0);
    and_byte(is_const, (value >> 8) & 0xff, name, bank_addr, 1);
    and_byte(is_const, (value >> 16) & 0xff, name, bank_addr, 2);
    and_byte(is_const, (value >> 24) & 0xff, name, bank_addr, 3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
or_byte(gp_boolean is_const, int value, char *name, char *bank_addr, int byte)
{

  codegen_write_asm("movf %s + %i, w", WORKING_LABEL, byte);
  if (is_const) {
    codegen_write_asm("iorlw %#x", value);
  } else {
    BANKSEL;
    codegen_write_asm("iorwf %s, w", name);
    BANKSEL_LOCAL;
  }
  codegen_write_asm("movwf %s + %i", WORKING_LABEL, byte);

}

static void
do_or(enum size_tag size,
      gp_boolean is_const,
      int value,
      char *name,
      char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      codegen_write_asm("iorlw %#x", value);
    } else {
      BANKSEL;
      codegen_write_asm("iorwf %s, w", name);
    }
    break;
  case size_uint16:
  case size_int16:
    BANKSEL_LOCAL;
    or_byte(is_const, value & 0xff, name, bank_addr, 0);
    or_byte(is_const, (value >> 8) & 0xff, name, bank_addr, 1);
    break;
  case size_uint24:
  case size_int24:
    BANKSEL_LOCAL;
    or_byte(is_const, value & 0xff, name, bank_addr, 0);
    or_byte(is_const, (value >> 8) & 0xff, name, bank_addr, 1);
    or_byte(is_const, (value >> 16) & 0xff, name, bank_addr, 2);
    break;
  case size_uint32:
  case size_int32:
    BANKSEL_LOCAL;
    or_byte(is_const, value & 0xff, name, bank_addr, 0);
    or_byte(is_const, (value >> 8) & 0xff, name, bank_addr, 1);
    or_byte(is_const, (value >> 16) & 0xff, name, bank_addr, 2);
    or_byte(is_const, (value >> 24) & 0xff, name, bank_addr, 3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
xor_byte(gp_boolean is_const, int value, char *name, char *bank_addr, int byte)
{

  codegen_write_asm("movf %s + %i, w", WORKING_LABEL, byte);
  if (is_const) {
    codegen_write_asm("xorlw %#x", value);
  } else {
    BANKSEL;
    codegen_write_asm("xorwf %s, w", name);
    BANKSEL_LOCAL;
  }
  codegen_write_asm("movwf %s + %i", WORKING_LABEL, byte);

}

static void
do_xor(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      codegen_write_asm("xorlw %#x", value);
    } else {
      BANKSEL;
      codegen_write_asm("xorwf %s, w", name);
    }
    break;
  case size_uint16:
  case size_int16:
    BANKSEL_LOCAL;
    xor_byte(is_const, value & 0xff, name, bank_addr, 0);
    xor_byte(is_const, (value >> 8) & 0xff, name, bank_addr, 1);
    break;
  case size_uint24:
  case size_int24:
    BANKSEL_LOCAL;
    xor_byte(is_const, value & 0xff, name, bank_addr, 0);
    xor_byte(is_const, (value >> 8) & 0xff, name, bank_addr, 1);
    xor_byte(is_const, (value >> 16) & 0xff, name, bank_addr, 2);
    break;
  case size_uint32:
  case size_int32:
    BANKSEL_LOCAL;
    xor_byte(is_const, value & 0xff, name, bank_addr, 0);
    xor_byte(is_const, (value >> 8) & 0xff, name, bank_addr, 1);
    xor_byte(is_const, (value >> 16) & 0xff, name, bank_addr, 2);
    xor_byte(is_const, (value >> 24) & 0xff, name, bank_addr, 3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
do_not(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    codegen_write_asm("xorlw 1");
    break;
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
    codegen_write_asm("movlw 1");
    BANKSEL_LOCAL;
    codegen_write_asm("xorwf %s, f", WORKING_LABEL);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
com_byte(int byte)
{
  codegen_write_asm("xorwf %s + %i, f", WORKING_LABEL, byte);
}

static void
do_com(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{

  BANKSEL_LOCAL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    codegen_write_asm("xorlw 0xff"); 
    break;
  case size_uint16:
  case size_int16:
    codegen_write_asm("movlw 0xff");
    com_byte(0);
    com_byte(1);
    break;
  case size_uint24:
  case size_int24:
    codegen_write_asm("movlw 0xff");
    com_byte(0);
    com_byte(1);
    com_byte(2);
    break;
  case size_uint32:
  case size_int32:
    codegen_write_asm("movlw 0xff");
    com_byte(0);
    com_byte(1);
    com_byte(2);
    com_byte(3);
    break;
  case size_float:
  default:
    assert(0);
  }

}

/****************************************************************************/
/* Shift                                                                    */
/****************************************************************************/

static void
left_shift(enum size_tag size)
{
  int i;
  int num_bytes;

  num_bytes = prim_bytes(size);

  for (i = 0; i < num_bytes; i++) {
    if (i == 0) {
      codegen_write_asm("bcf STATUS, C");
    }
    codegen_write_asm("rlf %s + %i, f", WORKING_LABEL, i);
  }

}

static void
do_lsh(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{
  char *reg1 = NULL;
  char *reg2 = NULL;
  char *label1 = NULL;
  char *label2 = NULL;
  char *label3 = NULL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
    if (is_const) {
      if (value < 8) {
        codegen_write_asm("mullw %#x", 1<<value);
        codegen_write_asm("movf PRODL, W");
      } else {
        codegen_write_asm("movlw 0");
      }
    } else {
      reg1 = codegen_get_temp(size);
      label1 = codegen_next_label();
      label2 = codegen_next_label();
      label3 = codegen_next_label();

      /* Save a copy of WREG */
      BANKSEL_LOCAL;
      codegen_write_asm("movwf %s", reg1);
      /* If "name" > 7, clear WREG and finish */              
      codegen_write_asm("movlw 0xf8");                        
      BANKSEL;
      codegen_write_asm("andwf %s, w", name);
      codegen_write_asm("bz %s", label1);
      codegen_write_asm("movlw 0");
      codegen_write_asm("bra %s", label3);
      /* Restore saved WREG; don't shift if "name" == 0 */
      codegen_write_label(label1);
      BANKSEL_LOCAL;
      codegen_write_asm("movf %s, w", reg1);
      BANKSEL;
      codegen_write_asm("movf %s, f", name);
      codegen_write_asm("bz %s", label3);
      codegen_write_asm("movff %s, %s", name, reg1);
      /* Perform the shift here */
      codegen_write_label(label2);
      codegen_write_asm("bcf STATUS, C");
      codegen_write_asm("rlcf WREG, f");
      BANKSEL_LOCAL;
      codegen_write_asm("decfsz %s, f", reg1);
      codegen_write_asm("bra %s", label2);
      codegen_write_label(label3);
    }
    break;
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
    reg1 = codegen_get_temp(size_uint8);
    label1 = codegen_next_label();
    label2 = codegen_next_label();
    if (is_const) {
      codegen_write_asm("movlw %#x", value);
      codegen_write_asm("addlw 0", value);
    } else {
      /* never can shift more than 31 so read the bottom byte only */
      BANKSEL;
      codegen_write_asm("movf %s, w", name);
    }
    BANKSEL_LOCAL;
    codegen_write_asm("movwf %s", reg1);
    codegen_write_label(label1);
    codegen_write_asm("btfsc STATUS, Z");
    codegen_write_asm("bra %s", label2);
    codegen_write_asm("bcf STATUS, C");
    left_shift(size);
    codegen_write_asm("decf %s, f", reg1);
    codegen_write_asm("bra %s", label1);
    codegen_write_label(label2);
    break;
  case size_float:
  default:
    assert(0);
  }

  if (reg1)
    free(reg1);

  if (reg2)
    free(reg2);

  if (label1)
    free(label1);

  if (label2)
    free(label2);

  if (label3)
    free(label3);

}

static void
right_shift(enum size_tag size, gp_boolean is_signed)
{
  int i;
  int num_bytes;

  num_bytes = prim_bytes(size);

  i = num_bytes - 1;

  if (is_signed) {
    /* put the sign in the carry */
    codegen_write_asm("rlf %s + %i, w", WORKING_LABEL, i);
  }

  for ( ; i >= 0 ; i--) {
    codegen_write_asm("rrf %s + %i, f", WORKING_LABEL, i);
  }

}

static void
do_rsh(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{
  int i;
  gp_boolean is_signed = false;
  char *reg1 = NULL;
  char *reg2 = NULL;
  char *label1 = NULL;
  char *label2 = NULL;

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_int8:
    is_signed = true;
    /* fall through */
  case size_uint8:
    reg1 = codegen_get_temp(size);
    if (is_const) {
      BANKSEL_LOCAL;
      codegen_write_asm("movwf %s", reg1);
      for (i = 0; i < value; i++) {
        if (is_signed) {
          /* put the sign in the carry */
          codegen_write_asm("rlf %s, w", reg1);
        } else {
          codegen_write_asm("bcf STATUS, C");
        }
        codegen_write_asm("rrf %s, f", reg1, i);    
      }
      codegen_write_asm("movf %s, w", reg1);  /* move the result into w */
    } else {
      reg2 = codegen_get_temp(size);
      label1 = codegen_next_label();
      label2 = codegen_next_label();

      BANKSEL_LOCAL;
      codegen_write_asm("movwf %s", reg1);
      BANKSEL;
      codegen_write_asm("movf %s, w", name);
      BANKSEL_LOCAL;
      codegen_write_asm("movwf %s", reg2);
      codegen_write_label(label1);
      codegen_write_asm("btfsc STATUS, Z");
      codegen_write_asm("bra %s", label2);
      if (is_signed) {
        /* put the sign in the carry */
        codegen_write_asm("rlf %s, w", reg1);
      } else {
        codegen_write_asm("bcf STATUS, C");
      }
      codegen_write_asm("rrf %s, f", reg1);
      codegen_write_asm("decf %s, f", reg2);
      codegen_write_asm("bra %s", label1);
      codegen_write_label(label2);
      codegen_write_asm("movf %s, w", reg1);  /* move the result into w */
    }
    break;
  case size_int16:
  case size_int24:
  case size_int32:
    is_signed = true;
    /* fall through */
  case size_uint16:
  case size_uint24:
  case size_uint32:
    reg1 = codegen_get_temp(size_uint8);
    label1 = codegen_next_label();
    label2 = codegen_next_label();
    if (is_const) {
      codegen_write_asm("movlw %#x", value);
      codegen_write_asm("addlw 0", value);
    } else {
      /* never can shift more than 31 so read the bottom byte only */
      BANKSEL;
      codegen_write_asm("movf %s, w", name);
    }
    BANKSEL_LOCAL;
    codegen_write_asm("movwf %s", reg1);
    codegen_write_label(label1);
    codegen_write_asm("btfsc STATUS, Z");
    codegen_write_asm("bra %s", label2);
    codegen_write_asm("bcf STATUS, C");
    right_shift(size, is_signed);
    codegen_write_asm("decf %s, f", reg1);
    codegen_write_asm("bra %s", label1);
    codegen_write_label(label2);
    break;
  case size_float:
  default:
    assert(0);
  }

  if (reg1)
    free(reg1);

  if (reg2)
    free(reg2);

  if (label1)
    free(label1);

  if (label2)
    free(label2);

}

/****************************************************************************/
/* Compare                                                                  */
/****************************************************************************/

static void
do_eq(enum size_tag size,
      gp_boolean is_const,
      int value,
      char *name,
      char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
    do_xor(size, is_const, value, name, bank_addr);    
    gen_boolean(size);
    codegen_write_asm("xorlw 1");
    move_to_working(size);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
do_ne(enum size_tag size,
      gp_boolean is_const,
      int value,
      char *name,
      char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
    do_xor(size, is_const, value, name, bank_addr);    
    gen_boolean(size);
    move_to_working(size);
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
offset_working(enum size_tag size)
{
  int num_bytes;

  num_bytes = prim_bytes(size);

  BANKSEL_LOCAL;

  switch (num_bytes) {
  case 4:
    codegen_write_asm("movlw 0x80");
    codegen_write_asm("xorwf %s + 3, f", WORKING_LABEL);
    break;
  case 3:
    codegen_write_asm("movlw 0x80");
    codegen_write_asm("xorwf %s + 2, f", WORKING_LABEL);
    break;
  case 2:
    codegen_write_asm("movlw 0x80");
    codegen_write_asm("xorwf %s + 1, f", WORKING_LABEL);
    break;
  case 1:
    codegen_write_asm("xorlw 0x80");
    break;

  }

}

static void
offset_reg(char *name, char *bank_addr, enum size_tag size)
{
  int num_bytes;

  BANKSEL;

  num_bytes = prim_bytes(size);
  codegen_write_asm("movlw 0x80");
  codegen_write_asm("xorwf %s + %i, f", name, num_bytes - 1);

}

static int
offset_constant(int number, enum size_tag size)
{
  int result = 0;
  int num_bytes;

  num_bytes = prim_bytes(size);

  switch (num_bytes) {
  case 4:
    result = number ^ 0x800000;
    break;
  case 3:
    result = number ^ 0x800000;
    break;
  case 2:
    result = number ^ 0x8000;
    break;
  case 1:
    result = number ^ 0x80;
    break;

  }

  return result;
}

static void
do_lt(enum size_tag size,
      gp_boolean is_const,
      int value,
      char *name,
      char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_uint16:
  case size_uint24:
  case size_uint32:
    do_sub(size, is_const, value, name, bank_addr);    
    codegen_write_asm("movf STATUS, w");
    codegen_write_asm("andlw 1");
    move_to_working(size);
    break;
  case size_int8:
    offset_working(size);
    do_sub(size, is_const, value, name, bank_addr);    
    /* offset the input */
    codegen_write_asm("andlw 0x80");
    codegen_write_asm("movf STATUS, w");
    codegen_write_asm("andlw 1");
    break;
  case size_int16:
  case size_int24:
  case size_int32:
    offset_working(size);
    if (is_const) {
      value = offset_constant(value, size);
    } else {
      offset_reg(name, bank_addr, size);
    }
    do_sub(size, is_const, value, name, bank_addr);    
    codegen_write_asm("movf STATUS, w");
    codegen_write_asm("andlw 1");
    move_to_working(size);
    if (!is_const) {
      /* restore name to original value */
      offset_reg(name, bank_addr, size);
    }
    break;
  case size_float:
  default:
    assert(0);
  }

}

static void
do_lte(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{

  switch (size) {
  case size_bit:
    assert(0);
    break;
  case size_uint8:
  case size_uint16:
  case size_uint24:
  case size_uint32:
    do_sub(size, is_const, value, name, bank_addr);    
    codegen_write_asm("movf STATUS, w");
    codegen_write_asm("andlw 5");
    codegen_write_asm("btfss STATUS, Z");
    codegen_write_asm("movlw 1");
    move_to_working(size);
    break;
  case size_int8:
    offset_working(size);
    do_sub(size, is_const, value, name, bank_addr);    
    /* offset the input */
    codegen_write_asm("addlw 0x80");
    codegen_write_asm("movf STATUS, w");
    codegen_write_asm("andlw 5");
    codegen_write_asm("btfss STATUS, Z");
    codegen_write_asm("movlw 1");
    break;
  case size_int16:
  case size_int24:
  case size_int32:
    offset_working(size);
    if (is_const) {
      value = offset_constant(value, size);
    } else {
      offset_reg(name, bank_addr, size);
    }
    do_sub(size, is_const, value, name, bank_addr);    
    codegen_write_asm("movf STATUS, w");
    codegen_write_asm("andlw 5");
    codegen_write_asm("btfss STATUS, Z");
    codegen_write_asm("movlw 1");
    move_to_working(size);
    if (!is_const) {
      /* restore name to original value */
      offset_reg(name, bank_addr, size);
    }
    break;
  case size_float:
  default:
    assert(0);
  }

}

/****************************************************************************/
/* External Functions                                                       */
/****************************************************************************/

static void
do_mult(enum size_tag size,
        gp_boolean is_const,
        int value,
        char *name,
        char *bank_addr)
{

  switch (size) {
  case size_bit:
  case size_uint8:
  case size_uint16:
  case size_uint24:
  case size_uint32:
  case size_int8:
  case size_int16:
  case size_int24:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

}

static void
do_div(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{

  switch (size) {
  case size_bit:
  case size_uint8:
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

}

static void
do_mod(enum size_tag size,
       gp_boolean is_const,
       int value,
       char *name,
       char *bank_addr)
{

  switch (size) {
  case size_bit:
  case size_uint8:
  case size_int8:
  case size_uint16:
  case size_int16:
  case size_uint24:
  case size_int24:
  case size_uint32:
  case size_int32:
  case size_float:
  default:
    assert(0);
  }

}

static void
codegen16e(enum node_op op, 
           enum size_tag size,
           gp_boolean is_const,
           int value,
           char *name,
           char *bank_addr)
{
  switch (op) {
  case op_assign:
    assert(0);
    break;
  case op_add:
    do_add(size, is_const, value, name, bank_addr);
    break;
  case op_sub:
    do_sub(size, is_const, value, name, bank_addr);
    break;
  case op_neg:
    do_neg(size, is_const, value, name, bank_addr);
    break;
  case op_com:
    do_com(size, is_const, value, name, bank_addr);
    break;
  case op_and:
    do_and(size, is_const, value, name, bank_addr);
    break;
  case op_or:
    do_or(size, is_const, value, name, bank_addr);
    break;
  case op_xor:
    do_xor(size, is_const, value, name, bank_addr);
    break;
  case op_not:
    do_not(size, is_const, value, name, bank_addr);
    break;
  case op_lsh:
    do_lsh(size, is_const, value, name, bank_addr);
    break;
  case op_rsh:
    do_rsh(size, is_const, value, name, bank_addr);
    break;
  case op_land:
    do_and(size_uint8, is_const, value, name, bank_addr);
    break;
  case op_lor:
    do_or(size_uint8, is_const, value, name, bank_addr);
    break;
  case op_eq:
    do_eq(size, is_const, value, name, bank_addr);
    break;
  case op_ne:
    do_ne(size, is_const, value, name, bank_addr);
    break;
  case op_lt:
    do_lt(size, is_const, value, name, bank_addr);
    break;
  case op_lte:
    do_lte(size, is_const, value, name, bank_addr);
    break;
  case op_gt:
  case op_gte:
    /* This is replaced in the optimizer.*/
    assert(0);
    break;
  case op_mult:
    do_mult(size, is_const, value, name, bank_addr);
    break;
  case op_div:
    do_div(size, is_const, value, name, bank_addr);
    break;
  case op_mod:
    do_mod(size, is_const, value, name, bank_addr);
    break;
  case op_clr:
  case op_inc:
  case op_dec:
    /* Shoud use unopgen16e.*/
    assert(0);
    break;
  default:
    assert(0); /* Unhandled binary operator */
  }

}

static void
unopgen16e(enum node_op op, 
           gp_boolean direct,
           char *name,
           char *bank_addr,
           enum size_tag size,
           int offset)
{
  switch (op) {
  case op_clr:
    if (direct) {
      clr_direct16e(name, bank_addr, size, offset);
    } else {
      clr_indirect16e(name, bank_addr, size, offset);
    }
    break;
  case op_inc:
    if (direct) {
      inc_direct16e(name, bank_addr, size, offset);
    } else {
      inc_indirect16e(name, bank_addr, size, offset);
    }
   break;
  case op_dec:
    if (direct) {
      dec_direct16e(name, bank_addr, size, offset);
    } else {
      dec_indirect16e(name, bank_addr, size, offset);
    }
    break;
  default:
    assert(0); /* Unhandled unary operator */
  }

}

static void
reset_vector16e(struct variable *var)
{
  codegen_write_comment("reset/startup vector");
  fprintf(state.output.f, "RESET_VECTOR code 0x0\n");
  codegen_line_number(var->node);
  fprintf(state.output.f, "  goto %s\n\n", var->name);
}

static void
interrupt_vector16e(struct variable *var)
{
  codegen_write_comment("hi-priority interrupt vector");
  fprintf(state.output.f, "HIPRI_INT_VECTOR code 0x8\n");
  codegen_line_number(var->node);

  /*
   * Save the FSR0 and PRODL:PRODH registers since they may be used
   * in the ISR and are likely used in the mainline code as well.
   */
  fprintf(state.output.f, "  movff FSR0L, _saved_fsr0\n");
  fprintf(state.output.f, "  movff FSR0H, _saved_fsr0 + 1\n");
  fprintf(state.output.f, "  movff PRODL, _saved_mul\n");
  fprintf(state.output.f, "  movff PRODH, _saved_mul + 1\n");
  
  fprintf(state.output.f, "  call %s\n", var->name);

  fprintf(state.output.f, "  movff _saved_fsr0, FSR0L\n");
  fprintf(state.output.f, "  movff _saved_fsr0 + 1, FSR0H\n");
  fprintf(state.output.f, "  movff _saved_mul, PRODL\n");
  fprintf(state.output.f, "  movff _saved_mul + 1, PRODH\n");
  
  fprintf(state.output.f, "  retfie FAST\n\n");

  fprintf(state.output.f, "HIPRI_INT_VAR udata_shr\n");
  codegen_write_comment("FIXME: not all processors have shared memory");
  fprintf(state.output.f, "_saved_fsr0 res 2\n");
  fprintf(state.output.f, "_saved_mul res 2\n");
}

target_type pic16e = {
  (long int)codegen16e,
  (long int)unopgen16e,
  (long int)load_constant16e,
  (long int)load_file16e,
  (long int)store_file16e,
  (long int)load_addr16e,
  (long int)load_fsr16e,
  (long int)offset_fsr16e,
  (long int)load_indirect16e,
  (long int)store_indirect16e,
  (long int)reset_vector16e,
  (long int)interrupt_vector16e,
  size_uint16,
  "uint16",
  size_uint24,
  "uint24",
  false,
  false
};

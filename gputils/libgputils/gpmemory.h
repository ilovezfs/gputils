/* memory header file.
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Scott Dattalo

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

#ifndef __GPMEMORY_H__
#define __GPMEMORY_H__

#define MAX_RAM                 0x2000              /* Maximum RAM. */
/* Choose bases such that each base has different hex 04 record. */
#define I_MEM_BITS              16                  /* MemBlock base bit alignment. */
#define MAX_I_MEM               (1 << I_MEM_BITS)   /* MemBlock base alignment. */
#define I_MEM_MASK              (MAX_I_MEM - 1)

#define MAX_C_MEM               0x100               /* Maximum configuration memory
                                                       (only a few bytes are used). */

#define BYTE_USED_MASK          (1 << 31)           /* Means occupied in MemBlock.memory.data. */
#define BYTE_LISTED_MASK        (1 << 30)           /* Means already listed. */
#define BYTE_ATTR_MASK          (BYTE_USED_MASK | BYTE_LISTED_MASK)

#define W_CONST_DATA            (1 << 14)           /* Data in the code area. */
#define W_SECOND_WORD           (1 << 13)           /* PIC16E family, second word of 32 bits instruction. (movff, ...) */

#define W_ARG_T_FIRST           (1 << 12)           /* The first argumentum of instruction a known register. */
#define W_ARG_T_SECOND          (1 << 11)           /* The second argumentum of instruction a known register. */
#define W_ARG_T_BOTH            (W_ARG_T_FIRST | W_ARG_T_SECOND) /* Both argumentum of instruction a known register or bit. */
#define W_ARG_T_MASK            W_ARG_T_BOTH

#define W_ADDR_T_FUNC           (1 << 10)           /* A function starts at this address. */
#define W_ADDR_T_LABEL          (1 <<  9)           /* A label there is at this address. */
#define W_ADDR_T_BRANCH_SRC     (1 <<  8)           /* Source of a branch there is at this address. */
#define W_ADDR_T_MASK           (W_ADDR_T_FUNC | W_ADDR_T_LABEL | W_ADDR_T_BRANCH_SRC)

#define W_TYPE_MASK             (UINT_MAX << 8)


#define W_USED_H                (1 << 1)            /* Used top half of the word. */
#define W_USED_L                (1 << 0)            /* Used bottom half of the word. */
#define W_USED_ALL              (W_USED_H | W_USED_L)

struct proc_class;

/* See beginning of gpmemory.c for documentation. */

typedef struct MemArg {
  const char *arg;
  int val;                      /* The value of the first argument. */
  int offs;                     /* If the argument is area then this the offset of the address. */
} MemArg;

typedef struct MemArgList {
  MemArg first;
  MemArg second;
} MemArgList;

typedef struct MemWord {
  unsigned int data;
  char *section_name;
  char *symbol_name;
  unsigned int dest_byte_addr;
  MemArgList args;
} MemWord;

typedef struct MemBlock {
  unsigned int base;
  MemWord *memory;
  struct MemBlock *next;
} MemBlock;

MemBlock *i_memory_create(void);
void i_memory_free(MemBlock *m);

gp_boolean b_memory_get(const MemBlock *m, unsigned int byte_address, unsigned char *byte,
                        const char **section_name, const char **symbol_name);

#ifndef NDEBUG

#define b_memory_assert_get(m, byte_address, byte, section_name, symbol_name) \
    assert(b_memory_get(m, byte_address, byte, section_name, symbol_name))

#else

#define b_memory_assert_get(m, byte_address, byte, section_name, symbol_name) \
    b_memory_get(m, byte_address, byte, section_name, symbol_name)

#endif

void b_memory_put(MemBlock *b_memory, unsigned int byte_address, unsigned char value,
                  const char *section_name, const char *symbol_name);

void b_memory_clear(MemBlock *b_memory, unsigned int byte_address);
int b_range_memory_used(const MemBlock *m, int from, int to);
int b_memory_used(const MemBlock *m);

struct px;

void print_i_memory(const MemBlock *m, const struct px *processor);

unsigned int i_memory_get_le(const MemBlock *m, unsigned int byte_addr, unsigned short *word,
                             const char **section_name, const char **symbol_name);

unsigned int i_memory_get_be(const MemBlock *m, unsigned int byte_addr, unsigned short *word,
                             const char **section_name, const char **symbol_name);

void i_memory_put_le(MemBlock *m, unsigned int byte_addr, unsigned short word,
                     const char *section_name, const char *symbol_name);

void i_memory_put_be(MemBlock *m, unsigned int byte_addr, unsigned short word,
                     const char *section_name, const char *symbol_name);

void b_memory_set_listed(MemBlock *m, unsigned int address, unsigned int n_bytes);
unsigned int b_memory_get_unlisted_size(const MemBlock *m, unsigned int address);

gp_boolean b_memory_set_addr_type(MemBlock *m, unsigned int address, unsigned int type, unsigned int dest_byte_addr);
unsigned int b_memory_get_addr_type(const MemBlock *m, unsigned int address, const char **label_name, unsigned int *dest_byte_addr);

gp_boolean b_memory_set_addr_name(MemBlock *m, unsigned int address, const char *name);

gp_boolean b_memory_set_args(MemBlock *m, unsigned int address, unsigned int type, const MemArgList *Args);

unsigned int b_memory_get_args(const MemBlock *m, unsigned int address, MemArgList *Args);

gp_boolean b_memory_set_type(MemBlock *m, unsigned int address, unsigned int type);
gp_boolean b_memory_clear_type(MemBlock *m, unsigned int address, unsigned int type);
unsigned int b_memory_get_type(const MemBlock *m, unsigned int address);

#endif

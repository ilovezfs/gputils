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
#define I_MEM_MAX               (1 << I_MEM_BITS)   /* MemBlock base alignment. */
#define I_MEM_MASK              (I_MEM_MAX - 1)
#define I_BASE_MASK             I_MEM_MASK

#define IMemAddrFromBase(Base)  ((Base) << I_MEM_BITS)
#define IMemBaseFromAddr(Addr)  (((Addr) >> I_MEM_BITS) & I_BASE_MASK)
#define IMemOffsFromAddr(Addr)  ((Addr) & I_MEM_MASK)

#define MAX_C_MEM               0x100               /* Maximum configuration memory (only a few bytes are used). */

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
  int         val;              /* The value of the first argument. */
  int         offs;             /* If the argument is area then this the offset of the address. */
} MemArg_t;

typedef struct MemArgList {
  MemArg_t first;
  MemArg_t second;
} MemArgList_t;

typedef struct MemByte {
  unsigned int  data;
  char         *section_name;
  char         *symbol_name;
  unsigned int  dest_byte_addr;
  MemArgList_t  args;
} MemByte_t;

typedef struct MemBlock {
  unsigned int     base;
  MemByte_t       *memory;
  struct MemBlock *next;
} MemBlock_t;

extern MemBlock_t *gp_mem_i_create(void);
extern void gp_mem_i_free(MemBlock_t *M);

extern gp_boolean gp_mem_b_get(const MemBlock_t *M, unsigned int Byte_address, uint8_t *Byte,
                               const char **Section_name, const char **Symbol_name);

#ifndef NDEBUG

#define gp_mem_b_assert_get(M, Byte_address, Byte, Section_name, Symbol_name) \
    assert(gp_mem_b_get(M, Byte_address, Byte, Section_name, Symbol_name) != 0)

#else

#define gp_mem_b_assert_get(M, Byte_address, Byte, Section_name, Symbol_name) \
    gp_mem_b_get(M, Byte_address, Byte, Section_name, Symbol_name)

#endif

extern void gp_mem_b_put(MemBlock_t *M, unsigned int Byte_address, uint8_t Value,
                         const char *Section_name, const char *Symbol_name);

extern void gp_mem_b_clear(MemBlock_t *M, unsigned int Byte_address);

extern void gp_mem_b_move(MemBlock_t *M, unsigned int From_byte_address, unsigned int To_byte_address,
                          unsigned int Byte_size);

extern void gp_mem_b_delete(MemBlock_t *M, unsigned int Byte_address);

extern void gp_mem_b_delete_area(MemBlock_t *M, unsigned int Byte_address, unsigned int Byte_number);

extern unsigned int b_range_memory_used(const MemBlock_t *M, unsigned int From_byte_address,
                                        unsigned int To_byte_address);

extern unsigned int gp_mem_b_used(const MemBlock_t *M);

struct px;

extern void gp_mem_i_print(const MemBlock_t *M, const struct px *Processor);

extern unsigned int gp_mem_i_get_le(const MemBlock_t *M, unsigned int Byte_address, uint16_t *Word,
                                    const char **Section_name, const char **Symbol_name);

extern unsigned int gp_mem_i_get_be(const MemBlock_t *M, unsigned int Byte_address, uint16_t *Word,
                                    const char **Section_name, const char **Symbol_name);

extern void gp_mem_i_put_le(MemBlock_t *M, unsigned int Byte_address, uint16_t Word,
                            const char *Section_name, const char *Symbol_name);

extern void gp_mem_i_put_be(MemBlock_t *M, unsigned int Byte_address, uint16_t Word,
                            const char *Section_name, const char *Symbol_name);

extern void gp_mem_i_delete(MemBlock_t *M, unsigned int Byte_address);

extern void gp_mem_b_set_listed(MemBlock_t *M, unsigned int Byte_address, unsigned int N_bytes);
extern unsigned int gp_mem_b_get_unlisted_size(const MemBlock_t *M, unsigned int Byte_address);

extern gp_boolean gp_mem_b_set_addr_type(MemBlock_t *M, unsigned int Byte_address, unsigned int Type,
                                         unsigned int Dest_byte_addr);

extern unsigned int gp_mem_b_get_addr_type(const MemBlock_t *M, unsigned int Byte_address,
                                           const char **Label_name, unsigned int *Dest_byte_addr);

extern gp_boolean gp_mem_b_set_addr_name(MemBlock_t *M, unsigned int Byte_address, const char *Name);

extern gp_boolean gp_mem_b_set_args(MemBlock_t *M, unsigned int Byte_address, unsigned int Type,
                                    const MemArgList_t *Args);

extern unsigned int gp_mem_b_get_args(const MemBlock_t *M, unsigned int Byte_address, MemArgList_t *Args);

extern gp_boolean gp_mem_b_set_type(MemBlock_t *M, unsigned int Byte_address, unsigned int Type);
extern gp_boolean gp_mem_b_clear_type(MemBlock_t *M, unsigned int Byte_address, unsigned int Type);
extern unsigned int gp_mem_b_get_type(const MemBlock_t *M, unsigned int Byte_address);

#endif

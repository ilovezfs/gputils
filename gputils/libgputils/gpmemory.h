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

#define MAX_RAM         0x1000          /* Maximum RAM */
/* Choose bases such that each base has different hex 04 record */
#define I_MEM_BITS      16              /* MemBlock base bit alignment */
#define MAX_I_MEM       (1<<I_MEM_BITS) /* MemBlock base alignment */
#define I_MEM_MASK      (MAX_I_MEM-1)

#define MAX_C_MEM       0x100           /* Maximum configuration memory
                                           (only a few bytes are used) */

#define BYTE_USED_MASK    0x8000 /* Means occupied in MemBlock.memory */
#define BYTE_LISTED_MASK  0x4000 /* Means already listed */

struct proc_class;

/* See beginning of gpmemory.c for documentation. */
typedef struct MemBlock {
  unsigned int base;
  unsigned short *memory;
  struct MemBlock *next;
} MemBlock;

MemBlock *i_memory_create(void);
void i_memory_free(MemBlock *m);
int b_memory_get(MemBlock *m, unsigned int byte_address, unsigned char *byte);
#ifndef NDEBUG
#define b_memory_assert_get(m, byte_address, byte) assert(b_memory_get(m, byte_address, byte))
#else
#define b_memory_assert_get(m, byte_address, byte) b_memory_get(m, byte_address, byte)
#endif
void b_memory_put(MemBlock *b_memory, unsigned int byte_address, unsigned char value);
void b_memory_clear(MemBlock *b_memory, unsigned int byte_address);
int b_range_memory_used(MemBlock *m, int from, int to);
int b_memory_used(MemBlock *m);
struct proc_class;
void print_i_memory(MemBlock *m, const struct proc_class *class);

int i_memory_get_le(MemBlock *m, unsigned int byte_addr, unsigned short *word);
int i_memory_get_be(MemBlock *m, unsigned int byte_addr, unsigned short *word);
void i_memory_put_le(MemBlock *m, unsigned int byte_addr, unsigned short word);
void i_memory_put_be(MemBlock *m, unsigned int byte_addr, unsigned short word);

void b_memory_set_listed(MemBlock *m, unsigned int address, unsigned int n_bytes);
unsigned int b_memory_get_unlisted_size(MemBlock *m, unsigned int address);

#endif

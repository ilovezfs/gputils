/* memory header file.
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004
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

#define MAX_RAM		0x1000	        /* Maximum RAM */
#define I_MEM_BITS      15
#define MAX_I_MEM	(1<<I_MEM_BITS) /* Maximum instruction memory */
#define I_MEM_MASK      (MAX_I_MEM-1)

#define MAX_C_MEM       0x100           /* Maximum configuration memory 
                                           (only a few bytes are used) */

#define MEM_USED_MASK 0x80000000 /* Means occupied in state.i_memory above */


typedef struct MemBlock {
  unsigned int base;
  unsigned int *memory;
  struct MemBlock *next;
} MemBlock;

MemBlock *i_memory_create(void);
void i_memory_free(MemBlock *m);
int i_memory_get(MemBlock *m, unsigned int address);
void i_memory_put(MemBlock *i_memory, unsigned int address, unsigned int value);
int i_memory_used(MemBlock *m);
void print_i_memory(MemBlock *m, int byte_addr);

#endif

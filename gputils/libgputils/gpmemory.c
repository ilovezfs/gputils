/* Supports instruction memory.
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

#include <limits.h>

#include "stdhdr.h"
#include "libgputils.h"

/************************************************************************

 gpmemory.c

    This file provides the functions used to manipulate the PIC memory.
    The memory is stored in 'memory blocks' which are implemented
 with the 'MemBlock' structure:

     typedef struct MemBlock {
       unsigned int base;
       unsigned short *memory;
       struct MemBlock *next;
     } MemBlock;

 Each MemBlock can hold up to `MAX_I_MEM' (32k currently) bytes. The `base'
 is the base address of the memory block. If the instruction memory spans
 more than 32k, then additional memory blocks can be allocated and linked
 together in a singly linked list (`next'). The last memory block in a
 linked list of blocks has its next ptr set to NULL.  32k is left over
 from when it was number of two byte instructions and it corresponded
 to 64K bytes which is the upper limit on inhx8m files.

 ************************************************************************/

MemBlock *i_memory_create(void)
{
  MemBlock *m;

  m         = (MemBlock *)malloc(sizeof(MemBlock));
  m->base   = 0;
  m->memory = (unsigned short *)calloc(MAX_I_MEM, sizeof(unsigned short));
  m->next   = NULL;

  return m;
}

void i_memory_free(MemBlock *m)
{
  MemBlock *n;

  do {

    if(m->memory)
      free(m->memory);

    n = m->next;
    free(m);
    m = n;

  } while(m);
}

/************************************************************************
 * i_memory_new
 *
 *  Create memory for a new memory block.
 *
 *  Inputs:
 *   m - start of the instruction memory
 *   mpb  - pointer to the memory block structure (MemBlock)
 *   base_address - where this new block of memory is based
 *
 ************************************************************************/

MemBlock * i_memory_new(MemBlock *m, MemBlock *mbp, unsigned int base_address)
{
  unsigned int base;

  base = (base_address >> I_MEM_BITS) & 0xFFFF;

  mbp->memory = (unsigned short *)calloc(MAX_I_MEM, sizeof(unsigned short));
  mbp->base   = base;

  do {

    if((m->next == NULL) || (m->next->base > base)) {
      /* Insert after this block */
      mbp->next = m->next;
      m->next   = mbp;
      return mbp;
    }

    m = m->next;
  } while(m);

  assert(0);

  return NULL;
}

/************************************************************************
 * b_memory_get
 *
 * Fetch a byte from the pic memory. This function will traverse through
 * the linked list of memory blocks searching for the address from the
 * word will be fetched. If the address is not found, then `0' will be
 * returned.
 *
 * Inputs:
 *  address -
 *  m - start of the instruction memory
 * Returns
 *  the byte from that address combined with status bits
 *
 ************************************************************************/
int b_memory_get(MemBlock *m, unsigned int address, unsigned char *byte)
{

  do {
    assert(m->memory != NULL);

    if( ((address >> I_MEM_BITS) & 0xFFFF) == m->base ) {
      *byte = m->memory[address & I_MEM_MASK] & 0xFF;
      return (m->memory[address & I_MEM_MASK] & BYTE_USED_MASK) != 0;
    }

    m = m->next;
  } while(m);

  return 0;
}

/************************************************************************
 *  b_memory_put
 *
 * This function will write one byte to a pic memory address. If the
 * destination memory block is non-existant, a new one will be created.
 *
 * inputs:
 *   i_memory - start of the instruction memory
 *   address - destination address of the write
 *   value   - the value to be written at that address
 * returns:
 *   none
 *
 ************************************************************************/
void b_memory_put(MemBlock *i_memory, unsigned int address, unsigned char value)
{
  MemBlock *m = NULL;

  do {

    if(m)
      m = m->next;
    else
      m = i_memory;

    if(m->memory == NULL) {
      i_memory_new(i_memory, m, address);
    }

    if( ((address >> I_MEM_BITS) & 0xFFFF) == m->base ) {
      m->memory[address & I_MEM_MASK] = value | BYTE_USED_MASK;
      return;
    }
  } while (m->next);

  /* Couldn't find an address to write this value. This must be
     the first time we've tried to write to high memory some place. */

  m = i_memory_new(i_memory, (MemBlock *) malloc(sizeof(MemBlock)), address);
  m->memory[address & I_MEM_MASK] = value | BYTE_USED_MASK;

}

/************************************************************************
 *  b_memory_clear
 *
 * This function will clear one byte of a pic memory address.
 *
 * inputs:
 *   i_memory - start of the instruction memory
 *   address - destination address of the write
 * returns:
 *   none
 *
 ************************************************************************/
void b_memory_clear(MemBlock *m, unsigned int address)
{
  do {
    assert(m->memory != NULL);

    if( ((address >> I_MEM_BITS) & 0xFFFF) == m->base ) {
      m->memory[address & I_MEM_MASK] = 0;
      break;
    }

    m = m->next;
  } while(m);
}

int b_range_memory_used(MemBlock *m, int from, int to)
{
  int i, j = 0, page = 0, bytes = 0;

  /* find the starting page */
  while (m && (page < from / MAX_I_MEM)) {
    j += MAX_I_MEM;
    m = m->next;
  }

  /* count used bytes */
  while (m && j < to) {
    for (i = 0; i < MAX_I_MEM && j < to; ++i) {
      if (m->memory[i & I_MEM_MASK] & BYTE_USED_MASK) {
        ++bytes;
      }
      ++j;
    }
    m = m->next;
  }

  return bytes;
}

int b_memory_used(MemBlock *m)
{
  return b_range_memory_used(m, 0, INT_MAX);
}

/************************************************************************
 *
 *
 *  These functions are used to read and write instruction memory.
 *
 *
 ************************************************************************/
int i_memory_get_le(MemBlock *m, unsigned int byte_addr, unsigned short *word)
{
  unsigned char bytes[2];
  if (b_memory_get(m, byte_addr, bytes) &&
      b_memory_get(m, byte_addr+1, bytes+1)) {
    *word = bytes[0] + (bytes[1] << 8);
    return 1;
  }
  return 0;
}

void i_memory_put_le(MemBlock *m, unsigned int byte_addr, unsigned short word)
{
  b_memory_put(m, byte_addr, word & 0xFF);
  b_memory_put(m, byte_addr+1, word >> 8);
}

int i_memory_get_be(MemBlock *m, unsigned int byte_addr, unsigned short *word)
{
  unsigned char bytes[2];
  if (b_memory_get(m, byte_addr, bytes) &&
      b_memory_get(m, byte_addr+1, bytes+1)) {
    *word = bytes[1] + (bytes[0] << 8);
    return 1;
  }
  return 0;
}
void i_memory_put_be(MemBlock *m, unsigned int byte_addr, unsigned short word)
{
  b_memory_put(m, byte_addr, word >> 8);
  b_memory_put(m, byte_addr+1, word & 0xFF);
}

void print_i_memory(MemBlock *m, proc_class_t class)
{
  int base,i,j,row_used;
  char c;

#define WORDS_IN_ROW 8

  do {
    assert(m->memory != NULL);

    base = m->base << I_MEM_BITS;

    for(i = 0; i<MAX_I_MEM; i+=2*WORDS_IN_ROW) {
      row_used = 0;

      for(j = 0; j<2*WORDS_IN_ROW; j++)
        if( m->memory[i+j] )
          row_used = 1;

      if(row_used) {
        printf("%08X  ", gp_processor_byte_to_org(class, base+i));

        for(j = 0; j<WORDS_IN_ROW; j+=2) {
          unsigned short data;
          class->i_memory_get(m, i+2*j, &data);
          printf("%04X ", data);
        }

        for(j = 0; j<2*WORDS_IN_ROW; j++) {
          c = m->memory[i+j] & 0xff;
          putchar( (isprint(c)) ? c : '.');
        }
        putchar('\n');
      }
    }

    m = m->next;
  } while(m);
}

/************************************************************************
 *
 *
 *  These functions are used to mark memory as listed.
 *
 *
 ************************************************************************/
void b_memory_set_listed(MemBlock *m, unsigned int address, unsigned int n_bytes)
{
  assert(m->memory != NULL);

  while (n_bytes--) {
    do {
      if (((address >> I_MEM_BITS) & 0xFFFF) == m->base) {
        m->memory[address & I_MEM_MASK] |= BYTE_LISTED_MASK;
        break;
      }

      m = m->next;
    } while(m);
    ++address;
  }
}

unsigned int b_memory_get_unlisted_size(MemBlock *m, unsigned int address)
{
  unsigned int n_bytes = 0;

  assert(m->memory != NULL);

  do {
    do {
      if (((address >> I_MEM_BITS) & 0xFFFF) == m->base) {
        if (BYTE_USED_MASK == (m->memory[address & I_MEM_MASK] & (BYTE_LISTED_MASK | BYTE_USED_MASK)))
          break;
        else
          return n_bytes;
      }

      m = m->next;
    } while(m);
    ++address;
    ++n_bytes;
  } while (n_bytes < 4);

  return n_bytes;
}

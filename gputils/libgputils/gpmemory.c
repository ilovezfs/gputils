/* Supports instruction memory.
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003
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

#include "stdhdr.h"
#include "libgputils.h"

/************************************************************************

 gpmemory.c

    This file provides the functions used to manipulate the instruction
 memory. 
    The instruction memory is stored in 'memory blocks' which are implemented
 with the 'MemBlock' structure:

     typedef struct MemBlock {
       unsigned int base;
       unsigned int *memory;
       struct MemBlock *next;
     } MemBlock;

 Each MemBlock can hold up to `MAX_I_MEM' (32k currently) words. The `base'
 is the base address of the memory block. If the instruction memory spans
 more than 32k, then additional memory blocks can be allocated and linked
 together in a singly linked list (`next'). The last memory block in a 
 linked list of blocks has its next ptr set to NULL.  32k was chose because
 it corresponds to 64K bytes which is the upper limit on inhx8m files.
 
 ************************************************************************/

MemBlock *i_memory_create(void)
{
  MemBlock *m;

  m         = (MemBlock *)malloc(sizeof(MemBlock));
  m->base   = 0;
  m->memory = (unsigned int *)calloc(MAX_I_MEM, sizeof(unsigned int));
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

MemBlock * i_memory_new(MemBlock *m, MemBlock *mbp, int base_address)
{
  int base;

  base = base_address >> I_MEM_BITS;

  mbp->memory = (unsigned int *)calloc(MAX_I_MEM, sizeof(unsigned int));
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
 * i_memory_get
 *
 * Fetch a word from the pic memory. This function will traverse through
 * the linked list of memory blocks searching for the address from the 
 * word will be fetched. If the address is not found, then `0' will be
 * returned.
 *
 * Inputs:
 *  address - 
 *  m - start of the instruction memory
 * Returns
 *  the word from that address
 *
 ************************************************************************/
int i_memory_get(MemBlock *m, unsigned int address)
{

  do {
    assert(m->memory != NULL);

    if( (address >> I_MEM_BITS) == m->base )
      return m->memory[address & I_MEM_MASK];

    m = m->next;
  } while(m);

  return 0;  
}

/************************************************************************
 *  i_memory_put
 *
 * This function will write one word to a pic memory address. If the
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
void i_memory_put(MemBlock *i_memory, unsigned int address, unsigned int value)
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

    if( (address >> I_MEM_BITS) == m->base ) {
      m->memory[address & I_MEM_MASK] = value;
      return;
    }
  } while (m->next);

  /* Couldn't find an address to write this value. This must be
     the first time we've tried to write to high memory some place. */

  m = i_memory_new(i_memory, (MemBlock *) malloc(sizeof(MemBlock)), address);
  m->memory[address & I_MEM_MASK] = value;

}

int i_memory_used(MemBlock *m)
{
  int words=0;
  int i;
  int maximum;

  while(m) {
    i = m->base << I_MEM_BITS;

    maximum = i + MAX_I_MEM;

    while (i < maximum) {
      if ((i_memory_get(m, i) & MEM_USED_MASK)) {
	words++;
      } 
      i++;
    }
    m = m->next;
  }

  return words;
}

/************************************************************************
 * 
 *
 *  These two functions are used to read and write instruction memory.
 * 
 *
 ************************************************************************/
void print_i_memory(MemBlock *m, int byte_addr)
{
  int base,i,j,row_used;
  char c;

#define WORDS_IN_ROW 8

  do {
    assert(m->memory != NULL);

    base = (m->base << I_MEM_BITS);

    for(i = 0; i<MAX_I_MEM; i+=WORDS_IN_ROW) {
      row_used = 0;

      for(j = 0; j<WORDS_IN_ROW; j++)
	if( m->memory[i+j] )
	  row_used = 1;

      if(row_used) {
	printf("%08X  ",(base+i) << byte_addr );

	for(j = 0; j<WORDS_IN_ROW; j++)
	  printf("%04X ", m->memory[i+j] & 0xffff );

	for(j = 0; j<WORDS_IN_ROW; j++) {
	  c = m->memory[i+j] & 0xff;
	  putchar( (isprint(c)) ? c : '.');

	  c = (m->memory[i+j]>>8) & 0xff;
	  putchar( (isprint(c)) ? c : '.');
	}
	putchar('\n');
      }
    }

    m = m->next;
  } while(m);

}


/* Supports instruction memory.
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Scott Dattalo
   Copyright (C) 2013 Borut Razem

    Copyright (C) 2014-2016 Molnar Karoly <molnarkaroly@users.sf.net>

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

typedef union
{
  uint8_t  b[2];
  uint16_t w;
} bword_t;

/**************************************************************************************************

 gpmemory.c

    This file provides the functions used to manipulate the PIC memory.
    The memory is stored in 'memory blocks' which are implemented
    with the 'MemBlock_t' structure:

      typedef struct MemArg {
        const char *arg;
        int         val;              The value of the first argument.
        int         offs;             If the argument is area then this the offset of the address.
      } MemArg_t;

      typedef struct MemArgList {
        MemArg_t first;
        MemArg_t second;
      } MemArgList_t;

      typedef struct MemByte {
        unsigned int  data;           The data byte and the attributes of.

        char         *section_name;   During assembly or linking shows the name of section.

        char         *symbol_name;    During assembly or linking shows the name of symbol.
	                              After disassembly shows the name of function or label.

        unsigned int  dest_byte_addr; After disassembly shows the target byte-address (not org) of a branch.

        MemArgList_t  args;
      } MemByte_t;

      typedef struct MemBlock {
        unsigned int       base;
        MemByte_t         *memory;
        struct MemBlock_t *next;
      } MemBlock_t;

 Each MemBlock_t can hold up to 'I_MEM_MAX' (64kB currently) bytes. The 'base'
 is the base address of the memory block. If the instruction memory spans
 more than 64kB, then additional memory blocks can be allocated and linked
 together in a singly linked list ('next'). The last memory block in a
 linked list of blocks has its next ptr set to NULL. 64kB is left over
 from when it was number of two byte instructions and it corresponded
 to 64k bytes which is the upper limit on inhx8m files.

 **************************************************************************************************/

/**************************************************************************************************
 * _memory_new
 *
 *  Create memory for a new memory block.
 *
 *  Inputs:
 *   m            - start of the instruction memory
 *   mpb          - pointer to the memory block structure (MemBlock_t)
 *   base_address - where this new block of memory is based
 *
 **************************************************************************************************/

static MemBlock_t *
_memory_new(MemBlock_t *M, MemBlock_t *Mbp, unsigned int Base_address)
{
  unsigned int block = IMemBaseFromAddr(Base_address);

  Mbp->base   = block;
  Mbp->memory = (MemByte_t *)GP_Calloc(I_MEM_MAX, sizeof(MemByte_t));

  do {
    if ((M->next == NULL) || (M->next->base > block)) {
      /* Insert after this block. */
      Mbp->next = M->next;
      M->next   = Mbp;
      return Mbp;
    }

    M = M->next;
  } while (M != NULL);

  assert(0);
  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

static void
_store_section_name(MemByte_t *Mb, const char *Name)
{
  if ((Name != NULL) && (*Name != '\0')) {
    Mb->section_name = GP_Strdup(Name);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_store_symbol_name(MemByte_t *Mb, const char *Name)
{
  if ((Name != NULL) && (*Name != '\0')) {
    Mb->symbol_name = GP_Strdup(Name);
  }
}

/*------------------------------------------------------------------------------------------------*/

MemBlock_t *
gp_mem_i_create(void)
{
  return (MemBlock_t *)GP_Calloc(1, sizeof(MemBlock_t));
}

/*------------------------------------------------------------------------------------------------*/

void
gp_mem_i_free(MemBlock_t *M)
{
  MemBlock_t   *next;
  MemByte_t    *b;
  unsigned int  i;

  if (M == NULL) {
    return;
  }

  do {
    if (M->memory != NULL) {
      b = M->memory;
      for (i = I_MEM_MAX; i; ++b, --i) {
        if (b->section_name != NULL) {
          free(b->section_name);
        }

        if (b->symbol_name != NULL) {
          free(b->symbol_name);
        }
      }

      free(M->memory);
    }

    next = M->next;
    free(M);
    M = next;
  } while (M != NULL);
}

/**************************************************************************************************
 * gp_mem_b_is_used
 *
 * Check if byte at address is used. This function will traverse through
 * the linked list of memory blocks searching for the address from the
 * word will be fetched.
 *
 * Inputs:
 *  M - start of the instruction memory
 *  Byte_address -
 * Returns
 *  true if byte at byte_address is used, false if not
 *
 **************************************************************************************************/

gp_boolean
gp_mem_b_is_used(MemBlock_t *M, unsigned int Byte_address)
{
  unsigned int block  = IMemBaseFromAddr(Byte_address);
  unsigned int offset = IMemOffsFromAddr(Byte_address);

  do {
    if (M->base == block) {
      return (((M->memory != NULL) && (M->memory[offset].data & BYTE_USED_MASK)) ? true : false);
    }

    M = M->next;
  } while (M != NULL);

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_mem_b_offset_is_used(MemBlock_t *M, unsigned int Byte_offset)
{
  if ((M == NULL) || (M->memory == NULL)) {
    return false;
  }

  return ((M->memory[Byte_offset].data & BYTE_USED_MASK) != 0);
}

/**************************************************************************************************
 * gp_mem_b_get
 *
 * Fetch a byte from the pic memory. This function will traverse through
 * the linked list of memory blocks searching for the address from the
 * word will be fetched. If the address is not found, then `0' will be
 * returned.
 *
 * Inputs:
 *  M - start of the instruction memory
 *  Byte_address -
 * Returns
 *  If is used the address the byte at address and true.
 *  Otherwise 0 and false.
 *
 **************************************************************************************************/

gp_boolean
gp_mem_b_get(const MemBlock_t *M, unsigned int Byte_address, uint8_t *Byte,
             const char **Section_name, const char **Symbol_name)
{
  unsigned int  block  = IMemBaseFromAddr(Byte_address);
  unsigned int  offset = IMemOffsFromAddr(Byte_address);
  MemByte_t    *b;

  while (M != NULL) {
    if (M->base == block) {
      if (M->memory != NULL) {
        b = &M->memory[offset];
        *Byte = b->data & 0xff;

        if (Section_name != NULL) {
          *Section_name = b->section_name;
        }

        if (Symbol_name != NULL) {
          *Symbol_name = b->symbol_name;
        }

        return ((b->data & BYTE_USED_MASK) ? true : false);
      }
      else {
        *Byte = 0;

        if (Section_name != NULL) {
          *Section_name = NULL;
        }

        if (Symbol_name != NULL) {
          *Symbol_name = NULL;
        }

        return false;
      }
    }

    M = M->next;
  }

  if (Section_name != NULL) {
    *Section_name = NULL;
  }

  if (Symbol_name != NULL) {
    *Symbol_name = NULL;
  }

  *Byte = 0;
  return false;
}

/**************************************************************************************************
 *  gp_mem_b_put
 *
 * This function will write one byte to a pic memory address. If the
 * destination memory block is non-existant, a new one will be created.
 *
 * inputs:
 *   M            - start of the instruction memory
 *   Byte_address - destination address of the write
 *   Value        - the value to be written at that address
 *   Section_name - section_name of the memory block
 *   Symbol_name  - symbol_name in the memory block
 * returns:
 *   none
 *
 **************************************************************************************************/

void
gp_mem_b_put(MemBlock_t *I_memory, unsigned int Byte_address, uint8_t Value,
	     const char *Section_name, const char *Symbol_name)
{
  unsigned int  block  = IMemBaseFromAddr(Byte_address);
  unsigned int  offset = IMemOffsFromAddr(Byte_address);
  MemBlock_t   *m = I_memory;
  MemByte_t    *b;

  while (m != NULL) {
    if (m->base == block) {
      if (m->memory == NULL) {
        m->memory = (MemByte_t *)GP_Calloc(I_MEM_MAX, sizeof(MemByte_t));
      }

      b = &m->memory[offset];

      if (b->section_name == NULL) {
        _store_section_name(b, Section_name);
      }

      if (b->symbol_name == NULL) {
        _store_symbol_name(b, Symbol_name);
      }

      b->data = Value | BYTE_USED_MASK;
      return;
    }

    m = m->next;
  }

  /* Couldn't find an address to write this value. This must be
     the first time we've tried to write to high memory some place. */

  m = _memory_new(I_memory, (MemBlock_t *)GP_Malloc(sizeof(MemBlock_t)), Byte_address);
  b = &m->memory[offset];
  b->data = Value | BYTE_USED_MASK;
  _store_section_name(b, Section_name);
  _store_symbol_name(b, Symbol_name);
}

/**************************************************************************************************
 *  gp_mem_b_clear
 *
 * This function will clear one byte of a pic memory address.
 *
 * inputs:
 *   M            - start of the instruction memory
 *   Byte_address - destination address of the clear
 * returns:
 *   none
 *
 **************************************************************************************************/

void
gp_mem_b_clear(MemBlock_t *M, unsigned int Byte_address)
{
  unsigned int  block  = IMemBaseFromAddr(Byte_address);
  unsigned int  offset = IMemOffsFromAddr(Byte_address);
  MemByte_t    *b;

  while (M != NULL) {
    if (M->base == block) {
      if (M->memory != NULL) {
        b = &M->memory[offset];
        b->data = 0;

        if (b->section_name != NULL) {
          free(b->section_name);
        }

        b->section_name = NULL;

        if (b->symbol_name != NULL) {
          free(b->symbol_name);
        }

        b->symbol_name = NULL;
      }

      return;
    }

    M = M->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_mem_b_move(MemBlock_t *M, unsigned int From_byte_address, unsigned int To_byte_address,
              unsigned int Byte_size)
{
  unsigned int from_block  = IMemBaseFromAddr(From_byte_address);
  unsigned int from_offset = IMemOffsFromAddr(From_byte_address);
  unsigned int to_block    = IMemBaseFromAddr(To_byte_address);
  unsigned int to_offset   = IMemOffsFromAddr(To_byte_address);
  size_t       size;

  if ((From_byte_address == To_byte_address) || (Byte_size == 0)) {
    return;
  }

  assert(from_block == to_block);
  assert((from_offset + Byte_size) <= I_MEM_MAX);
  assert((to_offset + Byte_size) <= I_MEM_MAX);

  while (M != NULL) {
    if (M->base == from_block) {
      size = Byte_size * sizeof(MemByte_t);
      memmove(&M->memory[to_offset], &M->memory[from_offset], size);

      /* Clear the unused area. */
      if (from_offset > to_offset) {
        /*
         *      from_offset
         *        |
         *        v
         *        +-------+
         *        |       |
         *        +-------+
         *
         *    move direction
         *      <<-------
         *
         *  to_offset  unused area
         *   |          |
         *   v          v
         *   +-------+----+
         *   |       |XXXX|
         *   +-------+----+
         */
        size = (from_offset - to_offset) * sizeof(MemByte_t);
        memset(&M->memory[to_offset + Byte_size], 0, size);
      }
      else {
        /*
         *  from_offset
         *    |
         *    v
         *    +-------+
         *    |       |
         *    +-------+
         *
         *    move direction
         *      ------->>
         *
         *     unused area
         *      |
         *      | to_offset
         *      |  |
         *      v  v
         *    +----+-------+
         *    |XXXX|       |
         *    +----+-------+ 
         */
        size = (to_offset - from_offset) * sizeof(MemByte_t);
        memset(&M->memory[from_offset], 0, size);
      }

      return;
    }

    M = M->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_mem_b_delete(MemBlock_t *M, unsigned int Byte_address)
{
  unsigned int  block  = IMemBaseFromAddr(Byte_address);
  unsigned int  offset = IMemOffsFromAddr(Byte_address);
  MemByte_t    *b;
  size_t        size;

  while (M != NULL) {
    if (M->base == block) {
      if (M->memory != NULL) {
        b = &M->memory[offset];

        if (b->section_name != NULL) {
          free(b->section_name);
        }

        if (b->symbol_name != NULL) {
          free(b->symbol_name);
        }

        size = (I_MEM_MAX - offset) * sizeof(MemByte_t);

        if (size != 0) {
	  memmove(b, &M->memory[offset + 1], size);
	}

	memset(&M->memory[I_MEM_MAX], 0, sizeof(MemByte_t));
      }

      return;
    }

    M = M->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_mem_b_delete_area(MemBlock_t *M, unsigned int Byte_address, unsigned int Byte_number)
{
  unsigned int  block  = IMemBaseFromAddr(Byte_address);
  unsigned int  offset = IMemOffsFromAddr(Byte_address);
  MemByte_t    *b;
  unsigned int  remnant_byte_num;
  unsigned int  i;

  if (Byte_number == 0) {
    return;
  }

  while (M != NULL) {
    if (M->base == block) {
      if (M->memory != NULL) {
        remnant_byte_num = I_MEM_MAX - offset;
        assert(Byte_number <= remnant_byte_num);

        remnant_byte_num -= Byte_number;

        for (b = &M->memory[offset], i = Byte_number; i > 0; ++b, --i) {
          if (b->section_name != NULL) {
            free(b->section_name);
          }

          if (b->symbol_name != NULL) {
            free(b->symbol_name);
          }
        }

	/*
	 *  Before the deleting.
         *
	 * block (from byte_address)
	 *  |   offset (from byte_address)
	 *  |    | byte_number
	 *  v    v   |          remnant_byte_num
         *  +----+---|----+---------|---------+
         *  |    |<--^--->|<--------^-------->|
         *  +----+--------+-------------------+
         *   <---------------v--------------->
         *                   |
         *                I_MEM_MAX
	 */
        if (remnant_byte_num != 0) {
          /* Delete the designated area. */
          memmove(&M->memory[offset], &M->memory[offset + Byte_number], remnant_byte_num * sizeof(MemByte_t));
        }

	/*
	 *  After the deleting.
         *
	 * block (from byte_address)
	 *  |   offset (from byte_address)
	 *  |    |     remnant_byte_num
	 *  v    v         |           empty_area = byte_number
         *  +----+---------|---------+---|----+
         *  |    |<--------^-------->|<--^--->|
         *  +----+-------------------+--------+
         *   <---------------v--------------->
         *                   |
         *                I_MEM_MAX
	 */
	/* Clear the empty area. */
        memset(&M->memory[offset + remnant_byte_num], 0, Byte_number * sizeof(MemByte_t));
      }

      return;
    }

    M = M->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
b_range_memory_used(const MemBlock_t *M, unsigned int From_byte_address, unsigned int To_byte_address)
{
  unsigned int i;
  unsigned int j;
  unsigned int starting_block;
  unsigned int block;
  unsigned int n_bytes;

  j              = 0;
  block          = 0;
  starting_block = IMemBaseFromAddr(From_byte_address);
  /* find the starting block */
  while ((M != NULL) && (block < starting_block)) {
    j += I_MEM_MAX;
    M = M->next;
    ++block;
  }

  n_bytes = 0;
  /* count used bytes */
  while ((M != NULL) && (j < To_byte_address)) {
    for (i = 0; (i < I_MEM_MAX) && (j < To_byte_address); ++i) {
      if ((M->memory != NULL) && (M->memory[i].data & BYTE_USED_MASK)) {
        ++n_bytes;
      }
      ++j;
    }
    M = M->next;
  }

  return n_bytes;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_mem_b_used(const MemBlock_t *M)
{
  return b_range_memory_used(M, 0, UINT_MAX);
}

/**************************************************************************************************
 *
 *
 *  These functions are used to read and write instruction memory.
 *
 *
 **************************************************************************************************/

unsigned int
gp_mem_i_offset_is_used(MemBlock_t *M, unsigned int Byte_offset)
{
  unsigned int ret;

  ret  = (gp_mem_b_offset_is_used(M, Byte_offset))     ? W_USED_L : 0;
  ret |= (gp_mem_b_offset_is_used(M, Byte_offset + 1)) ? W_USED_H : 0;
  return ret;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_mem_i_get_le(const MemBlock_t *M, unsigned int Byte_address, uint16_t *Word,
                const char **Section_name, const char **Symbol_name)
{
  unsigned int ret;
  bword_t      bw;

  ret  = (gp_mem_b_get(M, Byte_address,     &bw.b[0], Section_name, Symbol_name)) ? W_USED_L : 0;
  ret |= (gp_mem_b_get(M, Byte_address + 1, &bw.b[1], NULL,         NULL))        ? W_USED_H : 0;
  *Word = bw.w;
  return ret;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_mem_i_put_le(MemBlock_t *M, unsigned int Byte_address, uint16_t Word,
                const char *Section_name, const char *Symbol_name)
{
  gp_mem_b_put(M, Byte_address,     Word & 0xff, Section_name, Symbol_name);
  gp_mem_b_put(M, Byte_address + 1, Word >> 8,   Section_name, Symbol_name);
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_mem_i_get_be(const MemBlock_t *M, unsigned int Byte_address, uint16_t *Word,
                const char **Section_name, const char **Symbol_name)
{
  unsigned int ret;
  bword_t      bw;

  ret  = (gp_mem_b_get(M, Byte_address,     &bw.b[1], Section_name, Symbol_name)) ? W_USED_H : 0;
  ret |= (gp_mem_b_get(M, Byte_address + 1, &bw.b[0], NULL,         NULL))        ? W_USED_L : 0;
  *Word = bw.w;
  return ret;
}

/*------------------------------------------------------------------------------------------------*/

void
gp_mem_i_put_be(MemBlock_t *M, unsigned int Byte_address, uint16_t Word,
                const char *Section_name, const char *Symbol_name)
{
  gp_mem_b_put(M, Byte_address,     Word >> 8,   Section_name, Symbol_name);
  gp_mem_b_put(M, Byte_address + 1, Word & 0xff, Section_name, Symbol_name);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_mem_i_delete(MemBlock_t *m, unsigned int byte_address)
{
  gp_mem_b_delete(m, byte_address);
  gp_mem_b_delete(m, byte_address);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_mem_i_print(const MemBlock_t *M, pic_processor_t Processor)
{
  proc_class_t class;
  unsigned int byte_addr;
  unsigned int i;
  unsigned int j;
  unsigned int org;
  gp_boolean   row_used;
  unsigned int w_used;
  bword_t      data;
  uint8_t      c;

#define WORDS_IN_ROW 8

  class = Processor->class;

  while (M != NULL) {
    if (M->memory != NULL) {
      byte_addr = IMemAddrFromBase(M->base);

      for (i = 0; i < I_MEM_MAX; i += 2 * WORDS_IN_ROW) {
        row_used = false;

        for (j = 0; j < (2 * WORDS_IN_ROW); j++) {
          if (M->memory[i + j].data != 0) {
            row_used = true;
            break;
          }
        }

        if (row_used) {
          org = gp_processor_insn_from_byte_p(Processor, byte_addr + i);
          printf("%08X  ", org);

          if ((gp_processor_is_eeprom_org(Processor, org) >= 0) ||
              ((class == PROC_CLASS_PIC16E) &&
               ((gp_processor_is_idlocs_org(Processor, org) >= 0) ||
                (gp_processor_is_config_org(Processor, org) >= 0)))) {
            /* The row should be shown byte by byte. */
            for (j = 0; j < (2 * WORDS_IN_ROW); j++) {
              if (gp_mem_b_get(M, byte_addr + i + j, &data.b[0], NULL, NULL)) {
                printf("%02X ", data.b[0]);
              }
              else {
                printf("-- ");
              }
            }

            for (j = 0; j < (2 * WORDS_IN_ROW); j++) {
              c = M->memory[i + j].data & 0xff;
              putchar(isprint(c) ? c : '.');
            }
          }
          else {
            /* The row should be shown word by word. */
            for (j = 0; j < WORDS_IN_ROW; j++) {
              w_used = class->i_memory_get(M, byte_addr + i + (j * 2), &data.w, NULL, NULL);
              switch (w_used & W_USED_ALL) {
              case W_USED_ALL:
                printf("%04X  ", data.w);
                break;

              case W_USED_H:
                printf("%02X--  ", data.b[1]);
                break;

              case W_USED_L:
                printf("--%02X  ", data.b[0]);
                break;

              default:
                printf("----  ");
              }
            }

            for (j = 0; j < (2 * WORDS_IN_ROW); j++) {
              c = M->memory[i + j].data & 0xff;
              putchar(isprint(c) ? c : '.');
            }
          }

          putchar('\n');
        }
      }
    }

    M = M->next;
  }
}

/**************************************************************************************************
 *
 *
 *  These functions are used to mark memory as listed.
 *
 *
 **************************************************************************************************/

void
gp_mem_b_set_listed(MemBlock_t *M, unsigned int Byte_address, unsigned int N_bytes)
{
  unsigned int block = IMemBaseFromAddr(Byte_address);

  while (N_bytes--) {
    while (M != NULL) {
      if (M->base == block) {
        if (M->memory == NULL) {
          M->memory = (MemByte_t *)GP_Calloc(I_MEM_MAX, sizeof(MemByte_t));
        }

        M->memory[IMemOffsFromAddr(Byte_address)].data |= BYTE_LISTED_MASK;
        break;
      }

      M = M->next;
    }

    ++Byte_address;
  }
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_mem_b_get_unlisted_size(const MemBlock_t *M, unsigned int Byte_address)
{
  unsigned int block   = IMemBaseFromAddr(Byte_address);
  unsigned int n_bytes = 0;

  if ((M != NULL) && (M->memory != NULL)) {
    while (n_bytes < 4) {
      /* find memory block belonging to the byte_address */
      while (block != M->base) {
        M = M->next;

        if (M == NULL) {
          return n_bytes;
        }
      }

      if ((M->memory != NULL) &&
          ((M->memory[IMemOffsFromAddr(Byte_address)].data & BYTE_ATTR_MASK) == BYTE_USED_MASK)) {
        /* byte at byte_address not listed */
        ++Byte_address;
        ++n_bytes;
      }
      else {
        /* byte at byte_address already listed */
        break;
      }
    }
  }

  return n_bytes;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_mem_b_set_addr_type(MemBlock_t *M, unsigned int Byte_address, unsigned int Type,
                       unsigned int Dest_byte_addr)
{
  unsigned int  block  = IMemBaseFromAddr(Byte_address);
  unsigned int  offset = IMemOffsFromAddr(Byte_address);
  MemByte_t    *b;

  while (M != NULL) {
    if ((M->base == block) && (M->memory != NULL)) {
      b = &M->memory[offset];

      if (b->data & BYTE_USED_MASK) {
        b->data |= Type & W_ADDR_T_MASK;

	if (Type & W_ADDR_T_BRANCH_SRC) {
	  b->dest_byte_addr = Dest_byte_addr;
        }

        return true;
      }

      break;
    }

    M = M->next;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_mem_b_get_addr_type(const MemBlock_t *M, unsigned int Byte_address, const char **Label_name,
                       unsigned int *Dest_byte_addr)
{
  unsigned int  block  = IMemBaseFromAddr(Byte_address);
  unsigned int  offset = IMemOffsFromAddr(Byte_address);
  MemByte_t    *b;

  while (M != NULL) {
    if ((M->base == block) && (M->memory != NULL)) {
      b = &M->memory[offset];

      if (Label_name != NULL) {
	*Label_name = (b->data & (W_ADDR_T_FUNC | W_ADDR_T_LABEL)) ? b->symbol_name : NULL;
      }

      if (Dest_byte_addr != NULL) {
	*Dest_byte_addr = (b->data & W_ADDR_T_BRANCH_SRC) ? b->dest_byte_addr : 0;
      }

      return (b->data & W_ADDR_T_MASK);
    }

    M = M->next;
  }

  if (Label_name != NULL) {
    *Label_name = NULL;
  }

  if (Dest_byte_addr != NULL) {
    *Dest_byte_addr = 0;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_mem_b_set_addr_name(MemBlock_t *M, unsigned int Byte_address, const char *Name)
{
  unsigned int  block  = IMemBaseFromAddr(Byte_address);
  unsigned int  offset = IMemOffsFromAddr(Byte_address);
  MemByte_t    *b;

  while (M != NULL) {
    if ((M->base == block) && (M->memory != NULL)) {
      b = &M->memory[offset];

      if (b->symbol_name == NULL) {
        _store_symbol_name(b, Name);
      }

      return true;
    }

    M = M->next;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_mem_b_set_args(MemBlock_t *M, unsigned int Byte_address, unsigned int Type, const MemArgList_t *Args)
{
  unsigned int  block  = IMemBaseFromAddr(Byte_address);
  unsigned int  offset = IMemOffsFromAddr(Byte_address);
  MemByte_t    *b;

  while (M != NULL) {
    if ((M->base == block) && (M->memory != NULL)) {
      b = &M->memory[offset];

      if (b->data & BYTE_USED_MASK) {
        b->data |= Type & W_ARG_T_MASK;

        if (Type & W_ARG_T_FIRST) {
          b->args.first.arg  = Args->first.arg;
          b->args.first.val  = Args->first.val;
          b->args.first.offs = Args->first.offs;
        }

        if (Type & W_ARG_T_SECOND) {
          b->args.second.arg  = Args->second.arg;
          b->args.second.val  = Args->second.val;
          b->args.second.offs = Args->second.offs;
        }

        return true;
      }

      break;
    }

    M = M->next;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_mem_b_get_args(const MemBlock_t *M, unsigned int Byte_address, MemArgList_t *Args)
{
  unsigned int  block  = IMemBaseFromAddr(Byte_address);
  unsigned int  offset = IMemOffsFromAddr(Byte_address);
  MemByte_t    *b;

  while (M != NULL) {
    if ((M->base == block) && (M->memory != NULL)) {
      b = &M->memory[offset];

      if (b->data & BYTE_USED_MASK) {
        if (Args != NULL) {
          if (b->data & W_ARG_T_FIRST) {
            Args->first.arg  = b->args.first.arg;
            Args->first.val  = b->args.first.val;
            Args->first.offs = b->args.first.offs;
          }
          else {
            Args->first.arg  = NULL;
            Args->first.val  = 0;
            Args->first.offs = 0;
          }

          if (b->data & W_ARG_T_SECOND) {
            Args->second.arg  = b->args.second.arg;
            Args->second.val  = b->args.second.val;
            Args->second.offs = b->args.second.offs;
          }
          else {
            Args->second.arg  = NULL;
            Args->second.val  = 0;
            Args->second.offs = 0;
          }
        }

        return (b->data & W_ARG_T_MASK);
      }
    }

    M = M->next;
  }

  if (Args != NULL) {
    Args->first.arg   = NULL;
    Args->first.val   = 0;
    Args->first.offs  = 0;
    Args->second.arg  = NULL;
    Args->second.val  = 0;
    Args->second.offs = 0;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_mem_b_set_type(MemBlock_t *M, unsigned int Byte_address, unsigned int Type)
{
  unsigned int block  = IMemBaseFromAddr(Byte_address);
  unsigned int offset = IMemOffsFromAddr(Byte_address);

  while (M != NULL) {
    if ((M->base == block) && (M->memory != NULL)) {
      M->memory[offset].data |= Type & W_TYPE_MASK;
      return true;
    }

    M = M->next;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
gp_mem_b_clear_type(MemBlock_t *M, unsigned int Byte_address, unsigned int Type)
{
  unsigned int block  = IMemBaseFromAddr(Byte_address);
  unsigned int offset = IMemOffsFromAddr(Byte_address);

  while (M != NULL) {
    if ((M->base == block) && (M->memory != NULL)) {
      M->memory[offset].data &= ~(Type & W_TYPE_MASK);
      return true;
    }

    M = M->next;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
gp_mem_b_get_type(const MemBlock_t *M, unsigned int Byte_address)
{
  unsigned int block  = IMemBaseFromAddr(Byte_address);
  unsigned int offset = IMemOffsFromAddr(Byte_address);

  while (M != NULL) {
    if ((M->base == block) && (M->memory != NULL)) {
      return (M->memory[offset].data & W_TYPE_MASK);
    }

    M = M->next;
  }

  return 0;
}

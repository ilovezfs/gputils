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
    with the 'MemBlock' structure:

      typedef struct MemArg {
        const char *arg;
        int         val;              The value of the first argument.
        int         offs;             If the argument is area then this the offset of the address.
      } MemArg;

      typedef struct MemArgList {
        MemArg first;
        MemArg second;
      } MemArgList;

      typedef struct MemWord {
        unsigned int  data;           The data byte and the attributes of.

        char         *section_name;   During assembly or linking shows the name of section.

        char         *symbol_name;    During assembly or linking shows the name of symbol.
	                              After disassembly shows the name of function or label.

        unsigned int  dest_byte_addr; After disassembly shows the target byte-address (not org) of a branch.

        MemArgList args;
      } MemWord;

      typedef struct MemBlock {
        unsigned int     base;
        MemWord         *memory;
        struct MemBlock *next;
      } MemBlock;

 Each MemBlock can hold up to `I_MEM_MAX' (32k currently) bytes. The `base'
 is the base address of the memory block. If the instruction memory spans
 more than 32k, then additional memory blocks can be allocated and linked
 together in a singly linked list (`next'). The last memory block in a
 linked list of blocks has its next ptr set to NULL. 32k is left over
 from when it was number of two byte instructions and it corresponded
 to 64K bytes which is the upper limit on inhx8m files.

 **************************************************************************************************/

/**************************************************************************************************
 * _memory_new
 *
 *  Create memory for a new memory block.
 *
 *  Inputs:
 *   m            - start of the instruction memory
 *   mpb          - pointer to the memory block structure (MemBlock)
 *   base_address - where this new block of memory is based
 *
 **************************************************************************************************/

static MemBlock *
_memory_new(MemBlock *m, MemBlock *mbp, unsigned int base_address)
{
  unsigned int block = IMemBaseAddr(base_address);

  mbp->base   = block;
  mbp->memory = (MemWord *)GP_Calloc(I_MEM_MAX, sizeof(MemWord));

  do {
    if ((m->next == NULL) || (m->next->base > block)) {
      /* Insert after this block. */
      mbp->next = m->next;
      m->next   = mbp;
      return mbp;
    }

    m = m->next;
  } while (m != NULL);

  assert(0);

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

static void
_store_section_name(MemWord *mw, const char *name)
{
  if ((name != NULL) && (*name != '\0')) {
    mw->section_name = GP_Strdup(name);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_store_symbol_name(MemWord *mw, const char *name)
{
  if ((name != NULL) && (*name != '\0')) {
    mw->symbol_name = GP_Strdup(name);
  }
}

/*------------------------------------------------------------------------------------------------*/

MemBlock *
i_memory_create(void)
{
  return (MemBlock *)GP_Calloc(1, sizeof(MemBlock));
}

/*------------------------------------------------------------------------------------------------*/

void
i_memory_free(MemBlock *m)
{
  MemBlock     *next;
  MemWord      *w;
  unsigned int  i;

  do {
    if (m->memory != NULL) {
      w = m->memory;
      for (i = I_MEM_MAX; i; ++w, --i) {
        if (w->section_name != NULL) {
          free(w->section_name);
        }

        if (w->symbol_name != NULL) {
          free(w->symbol_name);
        }
      }

      free(m->memory);
    }

    next = m->next;
    free(m);
    m = next;
  } while (m != NULL);
}

/**************************************************************************************************
 * b_memory_is_used
 *
 * Check if byte at address is used. This function will traverse through
 * the linked list of memory blocks searching for the address from the
 * word will be fetched.
 *
 * Inputs:
 *  m - start of the instruction memory
 *  byte_address -
 * Returns
 *  true if byte at byte_address is used, false if not
 *
 **************************************************************************************************/

gp_boolean
b_memory_is_used(MemBlock *m, unsigned int byte_address)
{
  unsigned int block  = IMemBaseAddr(byte_address);
  unsigned int offset = IMemOffsetAddr(byte_address);

  do {
    if (m->base == block) {
      return (((m->memory != NULL) && (m->memory[offset].data & BYTE_USED_MASK)) ? true : false);
    }

    m = m->next;
  } while (m != NULL);

  return false;
}

/**************************************************************************************************
 * b_memory_get
 *
 * Fetch a byte from the pic memory. This function will traverse through
 * the linked list of memory blocks searching for the address from the
 * word will be fetched. If the address is not found, then `0' will be
 * returned.
 *
 * Inputs:
 *  m - start of the instruction memory
 *  address -
 * Returns
 *  If is used the address the byte at address and true.
 *  Otherwise 0 and false.
 *
 **************************************************************************************************/

gp_boolean
b_memory_get(const MemBlock *m, unsigned int byte_address, uint8_t *byte,
             const char **section_name, const char **symbol_name)
{
  unsigned int  block  = IMemBaseAddr(byte_address);
  unsigned int  offset = IMemOffsetAddr(byte_address);
  MemWord      *w;

  while (m != NULL) {
    if (m->base == block) {
      if (m->memory != NULL) {
        w = &m->memory[offset];
        *byte = w->data & 0xff;

        if (section_name != NULL) {
          *section_name = w->section_name;
        }

        if (symbol_name != NULL) {
          *symbol_name = w->symbol_name;
        }

        return ((w->data & BYTE_USED_MASK) ? true : false);
      }
      else {
        *byte = 0;

        if (section_name != NULL) {
          *section_name = NULL;
        }

        if (symbol_name != NULL) {
          *symbol_name = NULL;
        }

        return false;
      }
    }

    m = m->next;
  }

  if (section_name != NULL) {
    *section_name = NULL;
  }

  if (symbol_name != NULL) {
    *symbol_name = NULL;
  }

  *byte = 0;
  return false;
}

/**************************************************************************************************
 *  b_memory_put
 *
 * This function will write one byte to a pic memory address. If the
 * destination memory block is non-existant, a new one will be created.
 *
 * inputs:
 *   i_memory     - start of the instruction memory
 *   byte_address      - destination address of the write
 *   value        - the value to be written at that address
 *   section_name - section_name of the memory block
 *   symbol_name  - symbol_name in the memory block
 * returns:
 *   none
 *
 **************************************************************************************************/

void
b_memory_put(MemBlock *i_memory, unsigned int byte_address, uint8_t value,
	     const char *section_name, const char *symbol_name)
{
  unsigned int  block  = IMemBaseAddr(byte_address);
  unsigned int  offset = IMemOffsetAddr(byte_address);
  MemBlock     *m = i_memory;
  MemWord      *w;

  while (m != NULL) {
    if (m->base == block) {
      if (m->memory == NULL) {
        m->memory = (MemWord *)GP_Calloc(I_MEM_MAX, sizeof(MemWord));
      }

      w = &m->memory[offset];

      if (w->section_name == NULL) {
        _store_section_name(w, section_name);
      }

      if (w->symbol_name == NULL) {
        _store_symbol_name(w, symbol_name);
      }

      w->data = value | BYTE_USED_MASK;
      return;
    }

    m = m->next;
  }

  /* Couldn't find an address to write this value. This must be
     the first time we've tried to write to high memory some place. */

  m = _memory_new(i_memory, (MemBlock *)GP_Malloc(sizeof(MemBlock)), byte_address);
  w = &m->memory[offset];
  w->data = value | BYTE_USED_MASK;
  _store_section_name(w, section_name);
  _store_symbol_name(w, symbol_name);
}

/**************************************************************************************************
 *  b_memory_clear
 *
 * This function will clear one byte of a pic memory address.
 *
 * inputs:
 *   m       - start of the instruction memory
 *   byte_address - destination address of the clear
 * returns:
 *   none
 *
 **************************************************************************************************/

void
b_memory_clear(MemBlock *m, unsigned int byte_address)
{
  unsigned int  block  = IMemBaseAddr(byte_address);
  unsigned int  offset = IMemOffsetAddr(byte_address);
  MemWord      *w;

  while (m != NULL) {
    if (m->base == block) {
      if (m->memory != NULL) {
        w = &m->memory[offset];
        w->data = 0;

        if (w->section_name != NULL) {
          free(w->section_name);
        }

        w->section_name = NULL;

        if (w->symbol_name != NULL) {
          free(w->symbol_name);
        }

        w->symbol_name = NULL;
      }

      return;
    }

    m = m->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
b_memory_move(MemBlock *m, unsigned int from_byte_address, unsigned int to_byte_address,
              unsigned int byte_size)
{
  unsigned int from_block  = IMemBaseAddr(from_byte_address);
  unsigned int from_offset = IMemOffsetAddr(from_byte_address);
  unsigned int to_block    = IMemBaseAddr(to_byte_address);
  unsigned int to_offset   = IMemOffsetAddr(to_byte_address);
  size_t       size;

  if ((from_byte_address == to_byte_address) || (byte_size == 0)) {
    return;
  }

  assert(from_block == to_block);
  assert((from_offset + byte_size) <= I_MEM_MAX);
  assert((to_offset + byte_size) <= I_MEM_MAX);

  while (m != NULL) {
    if (m->base == from_block) {
      size = byte_size * sizeof(MemWord);
      memmove(&m->memory[to_offset], &m->memory[from_offset], size);

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
        size = (from_offset - to_offset) * sizeof(MemWord);
        memset(&m->memory[to_offset + byte_size], 0, size);
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
        size = (to_offset - from_offset) * sizeof(MemWord);
        memset(&m->memory[from_offset], 0, size);
      }

      return;
    }

    m = m->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
b_memory_delete(MemBlock *m, unsigned int byte_address)
{
  unsigned int  block  = IMemBaseAddr(byte_address);
  unsigned int  offset = IMemOffsetAddr(byte_address);
  MemWord      *w;
  size_t        size;

  while (m != NULL) {
    if (m->base == block) {
      if (m->memory != NULL) {
        w = &m->memory[offset];

        if (w->section_name != NULL) {
          free(w->section_name);
        }

        if (w->symbol_name != NULL) {
          free(w->symbol_name);
        }

        size = (I_MEM_MAX - offset) * sizeof(MemWord);

        if (size != 0) {
	  memmove(w, &m->memory[offset + 1], size);
	}

	memset(&m->memory[I_MEM_MAX], 0, sizeof(MemWord));
      }

      return;
    }

    m = m->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
b_memory_delete_area(MemBlock *m, unsigned int byte_address, unsigned int byte_number)
{
  unsigned int  block  = IMemBaseAddr(byte_address);
  unsigned int  offset = IMemOffsetAddr(byte_address);
  MemWord      *w;
  unsigned int  remnant_byte_num;

  if (byte_number == 0) {
    return;
  }

  while (m != NULL) {
    if (m->base == block) {
      if (m->memory != NULL) {
        w = &m->memory[offset];

        if (w->section_name != NULL) {
          free(w->section_name);
        }

        if (w->symbol_name != NULL) {
          free(w->symbol_name);
        }

        remnant_byte_num = I_MEM_MAX - offset;
        assert(byte_number <= remnant_byte_num);

        remnant_byte_num -= byte_number;

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
          memmove(w, &m->memory[offset + byte_number], remnant_byte_num * sizeof(MemWord));
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
        memset(&m->memory[offset + remnant_byte_num], 0, byte_number * sizeof(MemWord));
      }

      return;
    }

    m = m->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
b_range_memory_used(const MemBlock *m, unsigned int from_byte_address, unsigned int to_byte_address)
{
  unsigned int i;
  unsigned int j;
  unsigned int starting_block;
  unsigned int block;
  unsigned int bytes;

  j              = 0;
  block          = 0;
  starting_block = IMemBaseAddr(from_byte_address);
  /* find the starting block */
  while ((m != NULL) && (block < starting_block)) {
    j += I_MEM_MAX;
    m = m->next;
    ++block;
  }

  bytes = 0;
  /* count used bytes */
  while ((m != NULL) && (j < to_byte_address)) {
    for (i = 0; (i < I_MEM_MAX) && (j < to_byte_address); ++i) {
      if ((m->memory != NULL) && (m->memory[i].data & BYTE_USED_MASK)) {
        ++bytes;
      }
      ++j;
    }
    m = m->next;
  }

  return bytes;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
b_memory_used(const MemBlock *m)
{
  return b_range_memory_used(m, 0, UINT_MAX);
}

/**************************************************************************************************
 *
 *
 *  These functions are used to read and write instruction memory.
 *
 *
 **************************************************************************************************/

unsigned int
i_memory_get_le(const MemBlock *m, unsigned int byte_address, uint16_t *word,
                const char **section_name, const char **symbol_name)
{
  unsigned int ret;
  bword_t      bw;

  ret  = (b_memory_get(m, byte_address,     &bw.b[0], section_name, symbol_name)) ? W_USED_L : 0;
  ret |= (b_memory_get(m, byte_address + 1, &bw.b[1], NULL,         NULL))        ? W_USED_H : 0;
  *word = bw.w;
  return ret;
}

/*------------------------------------------------------------------------------------------------*/

void
i_memory_put_le(MemBlock *m, unsigned int byte_address, uint16_t word,
                const char *section_name, const char *symbol_name)
{
  b_memory_put(m, byte_address,     word & 0xff, section_name, symbol_name);
  b_memory_put(m, byte_address + 1, word >> 8,   section_name, symbol_name);
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
i_memory_get_be(const MemBlock *m, unsigned int byte_address, uint16_t *word,
                const char **section_name, const char **symbol_name)
{
  unsigned int ret;
  bword_t      bw;

  ret  = (b_memory_get(m, byte_address,     &bw.b[1], section_name, symbol_name)) ? W_USED_H : 0;
  ret |= (b_memory_get(m, byte_address + 1, &bw.b[0], NULL,         NULL))        ? W_USED_L : 0;
  *word = bw.w;
  return ret;
}

/*------------------------------------------------------------------------------------------------*/

void
i_memory_put_be(MemBlock *m, unsigned int byte_address, uint16_t word,
                const char *section_name, const char *symbol_name)
{
  b_memory_put(m, byte_address,     word >> 8,   section_name, symbol_name);
  b_memory_put(m, byte_address + 1, word & 0xff, section_name, symbol_name);
}

/*------------------------------------------------------------------------------------------------*/

void
i_memory_delete(MemBlock *m, unsigned int byte_address)
{
  b_memory_delete(m, byte_address);
  b_memory_delete(m, byte_address);
}

/*------------------------------------------------------------------------------------------------*/

void
i_memory_print(const MemBlock *m, pic_processor_t processor)
{
  proc_class_t class;
  unsigned int base;
  unsigned int i;
  unsigned int j;
  unsigned int org;
  gp_boolean   row_used;
  unsigned int w_used;
  bword_t      data;
  uint8_t      c;

#define WORDS_IN_ROW 8

  class = processor->class;

  while (m != NULL) {
    if (m->memory != NULL) {
      base = m->base << I_MEM_BITS;

      for (i = 0; i < I_MEM_MAX; i += 2 * WORDS_IN_ROW) {
        row_used = false;

        for (j = 0; j < (2 * WORDS_IN_ROW); j++) {
          if (m->memory[i + j].data != 0) {
            row_used = true;
            break;
          }
        }

        if (row_used) {
          org = gp_processor_byte_to_real(processor, base + i);
          printf("%08X  ", org);

          if ((gp_processor_is_eeprom_org(processor, org) >= 0) ||
              ((class == PROC_CLASS_PIC16E) &&
               ((gp_processor_is_idlocs_org(processor, org) >= 0) ||
                (gp_processor_is_config_org(processor, org) >= 0)))) {
            /* The row should be shown byte by byte. */
            for (j = 0; j < (2 * WORDS_IN_ROW); j++) {
              if (b_memory_get(m, base + i + j, &data.b[0], NULL, NULL)) {
                printf("%02X ", data.b[0]);
              }
              else {
                printf("-- ");
              }
            }

            for (j = 0; j < (2 * WORDS_IN_ROW); j++) {
              c = m->memory[i + j].data & 0xff;
              putchar(isprint(c) ? c : '.');
            }
          }
          else {
            /* The row should be shown word by word. */
            for (j = 0; j < WORDS_IN_ROW; j++) {
              w_used = class->i_memory_get(m, base + i + (j * 2), &data.w, NULL, NULL);
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
              c = m->memory[i + j].data & 0xff;
              putchar(isprint(c) ? c : '.');
            }
          }

          putchar('\n');
        }
      }
    }

    m = m->next;
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
b_memory_set_listed(MemBlock *m, unsigned int byte_address, unsigned int n_bytes)
{
  unsigned int block = IMemBaseAddr(byte_address);

  while (n_bytes--) {
    while (m != NULL) {
      if (m->base == block) {
        if (m->memory == NULL) {
          m->memory = (MemWord *)GP_Calloc(I_MEM_MAX, sizeof(MemWord));
        }

        m->memory[IMemOffsetAddr(byte_address)].data |= BYTE_LISTED_MASK;
        break;
      }

      m = m->next;
    }

    ++byte_address;
  }
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
b_memory_get_unlisted_size(const MemBlock *m, unsigned int byte_address)
{
  unsigned int block   = IMemBaseAddr(byte_address);
  unsigned int n_bytes = 0;

  if ((m != NULL) && (m->memory != NULL)) {
    while (n_bytes < 4) {
      /* find memory block belonging to the byte_address */
      while (block != m->base) {
        m = m->next;

        if (m == NULL) {
          return n_bytes;
        }
      }

      if ((m->memory != NULL) &&
          ((m->memory[IMemOffsetAddr(byte_address)].data & BYTE_ATTR_MASK) == BYTE_USED_MASK)) {
        /* byte at byte_address not listed */
        ++byte_address;
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
b_memory_set_addr_type(MemBlock *m, unsigned int byte_address, unsigned int type,
                       unsigned int dest_byte_addr)
{
  unsigned int  block  = IMemBaseAddr(byte_address);
  unsigned int  offset = IMemOffsetAddr(byte_address);
  MemWord      *w;

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      w = &m->memory[offset];

      if (w->data & BYTE_USED_MASK) {
        w->data |= type & W_ADDR_T_MASK;

	if (type & W_ADDR_T_BRANCH_SRC) {
	  w->dest_byte_addr = dest_byte_addr;
        }

        return true;
      }

      break;
    }

    m = m->next;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
b_memory_get_addr_type(const MemBlock *m, unsigned int byte_address, const char **label_name,
                       unsigned int *dest_byte_addr)
{
  unsigned int  block  = IMemBaseAddr(byte_address);
  unsigned int  offset = IMemOffsetAddr(byte_address);
  MemWord      *w;

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      w = &m->memory[offset];

      if (label_name != NULL) {
	*label_name = (w->data & (W_ADDR_T_FUNC | W_ADDR_T_LABEL)) ? w->symbol_name : NULL;
      }

      if (dest_byte_addr != NULL) {
	*dest_byte_addr = (w->data & W_ADDR_T_BRANCH_SRC) ? w->dest_byte_addr : 0;
      }

      return (w->data & W_ADDR_T_MASK);
    }

    m = m->next;
  }

  if (label_name != NULL) {
    *label_name = NULL;
  }

  if (dest_byte_addr != NULL) {
    *dest_byte_addr = 0;
  }

  return 0;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
b_memory_set_addr_name(MemBlock *m, unsigned int byte_address, const char *name)
{
  unsigned int  block  = IMemBaseAddr(byte_address);
  unsigned int  offset = IMemOffsetAddr(byte_address);
  MemWord      *w;

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      w = &m->memory[offset];

      if (w->symbol_name == NULL) {
        _store_symbol_name(w, name);
      }

      return true;
    }

    m = m->next;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
b_memory_set_args(MemBlock *m, unsigned int byte_address, unsigned int type, const MemArgList *Args)
{
  unsigned int  block  = IMemBaseAddr(byte_address);
  unsigned int  offset = IMemOffsetAddr(byte_address);
  MemWord      *w;

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      w = &m->memory[offset];

      if (w->data & BYTE_USED_MASK) {
        w->data |= type & W_ARG_T_MASK;

        if (type & W_ARG_T_FIRST) {
          w->args.first.arg  = Args->first.arg;
          w->args.first.val  = Args->first.val;
          w->args.first.offs = Args->first.offs;
        }

        if (type & W_ARG_T_SECOND) {
          w->args.second.arg  = Args->second.arg;
          w->args.second.val  = Args->second.val;
          w->args.second.offs = Args->second.offs;
        }

        return true;
      }

      break;
    }

    m = m->next;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
b_memory_get_args(const MemBlock *m, unsigned int byte_address, MemArgList *Args)
{
  unsigned int  block  = IMemBaseAddr(byte_address);
  unsigned int  offset = IMemOffsetAddr(byte_address);
  MemWord      *w;

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      w = &m->memory[offset];

      if (w->data & BYTE_USED_MASK) {
        if (Args != NULL) {
          if (w->data & W_ARG_T_FIRST) {
            Args->first.arg  = w->args.first.arg;
            Args->first.val  = w->args.first.val;
            Args->first.offs = w->args.first.offs;
          }
          else {
            Args->first.arg  = NULL;
            Args->first.val  = 0;
            Args->first.offs = 0;
          }

          if (w->data & W_ARG_T_SECOND) {
            Args->second.arg  = w->args.second.arg;
            Args->second.val  = w->args.second.val;
            Args->second.offs = w->args.second.offs;
          }
          else {
            Args->second.arg  = NULL;
            Args->second.val  = 0;
            Args->second.offs = 0;
          }
        }

        return (w->data & W_ARG_T_MASK);
      }
    }

    m = m->next;
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
b_memory_set_type(MemBlock *m, unsigned int byte_address, unsigned int type)
{
  unsigned int block  = IMemBaseAddr(byte_address);
  unsigned int offset = IMemOffsetAddr(byte_address);

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      m->memory[offset].data |= type & W_TYPE_MASK;
      return true;
    }

    m = m->next;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
b_memory_clear_type(MemBlock *m, unsigned int byte_address, unsigned int type)
{
  unsigned int block  = IMemBaseAddr(byte_address);
  unsigned int offset = IMemOffsetAddr(byte_address);

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      m->memory[offset].data &= ~(type & W_TYPE_MASK);
      return true;
    }

    m = m->next;
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

unsigned int
b_memory_get_type(const MemBlock *m, unsigned int byte_address)
{
  unsigned int block  = IMemBaseAddr(byte_address);
  unsigned int offset = IMemOffsetAddr(byte_address);

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      return (m->memory[offset].data & W_TYPE_MASK);
    }

    m = m->next;
  }

  return 0;
}

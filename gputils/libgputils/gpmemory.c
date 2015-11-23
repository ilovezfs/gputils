/* Supports instruction memory.
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Scott Dattalo
   Copyright (C) 2013 Borut Razem

    Copyright (C) 2014-2015 Molnar Karoly <molnarkaroly@users.sf.net>

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
  unsigned char b[2];
  unsigned short w;
} bword_t;

/************************************************************************

 gpmemory.c

    This file provides the functions used to manipulate the PIC memory.
    The memory is stored in 'memory blocks' which are implemented
    with the 'MemBlock' structure:

      typedef struct MemArg {
        const char *arg;
        int val;                      The value of the first argument.
        int offs;                     If the argument is area then this the offset of the address.
      } MemArg;

      typedef struct MemArg {
        MemArg first;
        MemArg second;
      } MemArgList;

      typedef struct MemWord {
        unsigned int data;            The data byte and the attributes of.

        char *section_name;           During assembly or linking shows the name of section.

        char *symbol_name;            During assembly or linking shows the name of symbol.
	                              After disassembly shows the name of function or label.

        unsigned int dest_byte_addr;  After disassembly shows the target byte-address (not org) of a branch.

        MemArgList args;
      } MemWord;

      typedef struct MemBlock {
        unsigned int base;
        MemWord *memory;
        struct MemBlock *next;
      } MemBlock;

 Each MemBlock can hold up to `MAX_I_MEM' (32k currently) bytes. The `base'
 is the base address of the memory block. If the instruction memory spans
 more than 32k, then additional memory blocks can be allocated and linked
 together in a singly linked list (`next'). The last memory block in a
 linked list of blocks has its next ptr set to NULL. 32k is left over
 from when it was number of two byte instructions and it corresponded
 to 64K bytes which is the upper limit on inhx8m files.

 ************************************************************************/

MemBlock *
i_memory_create(void)
{
  return (MemBlock *)GP_Calloc(1, sizeof(MemBlock));
}

/*----------------------------------------------------------------------*/

void
i_memory_free(MemBlock *m)
{
  MemBlock *n;
  MemWord *w;
  unsigned int i;

  do {
    if (m->memory != NULL) {
      w = m->memory;
      for (i = MAX_I_MEM; i; ++w, --i) {
        if (w->section_name != NULL) {
          free((void *)w->section_name);
        }

        if (w->symbol_name != NULL) {
          free((void *)w->symbol_name);
        }
      }

      free(m->memory);
    }

    n = m->next;
    free(m);
    m = n;
  } while (m != NULL);
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

static MemBlock *
i_memory_new(MemBlock *m, MemBlock *mbp, unsigned int base_address)
{
  unsigned int base = (base_address >> I_MEM_BITS) & 0xffff;

  mbp->base   = base;
  mbp->memory = (MemWord *)GP_Calloc(MAX_I_MEM, sizeof(MemWord));

  do {
    if ((m->next == NULL) || (m->next->base > base)) {
      /* Insert after this block. */
      mbp->next = m->next;
      m->next = mbp;
      return mbp;
    }

    m = m->next;
  } while (m != NULL);

  assert(0);

  return NULL;
}

/************************************************************************
 * b_memory_is_used
 *
 * Check if byte at address is used. This function will traverse through
 * the linked list of memory blocks searching for the address from the
 * word will be fetched.
 *
 * Inputs:
 *  address -
 *  m - start of the instruction memory
 * Returns
 *  true if byte at address is used, false if not
 *
 ************************************************************************/
gp_boolean
b_memory_is_used(MemBlock *m, unsigned int address)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;

  do
  {
    if (m->base == block) {
      return (((m->memory != NULL) && (m->memory[address & I_MEM_MASK].data & BYTE_USED_MASK)) ? true : false);
    }

    m = m->next;
  } while (m != NULL);

  return false;
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
 *  If is used the address the byte at address and true.
 *  Otherwise 0 and false.
 *
 ************************************************************************/
gp_boolean
b_memory_get(const MemBlock *m, unsigned int address, unsigned char *byte,
             const char **section_name, const char **symbol_name)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;
  MemWord *w;

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

/*----------------------------------------------------------------------*/

static void
i_memory_store_section_name(MemWord *mw, const char *name)
{
  if ((name != NULL) && (*name != '\0')) {
    mw->section_name = GP_Strdup(name);
  }
}

/*----------------------------------------------------------------------*/

static void
i_memory_store_symbol_name(MemWord *mw, const char *name)
{
  if ((name != NULL) && (*name != '\0')) {
    mw->symbol_name = GP_Strdup(name);
  }
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
 *   section_name - section_name of the memory block
 *   symbol_name - symbol_name in the memory block
 * returns:
 *   none
 *
 ************************************************************************/
void
b_memory_put(MemBlock *i_memory, unsigned int address, unsigned char value,
	     const char *section_name, const char *symbol_name)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;
  MemBlock *m = NULL;
  MemWord *w;

  do {
    m = (m != NULL) ? m->next : i_memory;

    if (m->base == block) {
      if (m->memory == NULL) {
        m->memory = (MemWord *)GP_Calloc(MAX_I_MEM, sizeof(MemWord));
      }

      w = &m->memory[offset];

      if (w->section_name == NULL) {
        i_memory_store_section_name(w, section_name);
      }

      if (w->symbol_name == NULL) {
        i_memory_store_symbol_name(w, symbol_name);
      }

      w->data = value | BYTE_USED_MASK;
      return;
    }
  } while (m->next != NULL);

  /* Couldn't find an address to write this value. This must be
     the first time we've tried to write to high memory some place. */

  m = i_memory_new(i_memory, (MemBlock *)GP_Malloc(sizeof(MemBlock)), address);
  w = &m->memory[offset];
  w->data = value | BYTE_USED_MASK;
  i_memory_store_section_name(w, section_name);
  i_memory_store_symbol_name(w, symbol_name);
}

/************************************************************************
 *  b_memory_clear
 *
 * This function will clear one byte of a pic memory address.
 *
 * inputs:
 *   i_memory - start of the instruction memory
 *   address - destination address of the clear
 * returns:
 *   none
 *
 ************************************************************************/
void
b_memory_clear(MemBlock *m, unsigned int address)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;
  MemWord *w;

  do
  {
    if (m->base == block) {
      if (m->memory != NULL) {
        w = &m->memory[offset];
        w->data = 0;

        if (w->section_name != NULL) {
          free((void *)w->section_name);
        }

        w->section_name = NULL;

        if (w->symbol_name != NULL) {
          free((void *)w->symbol_name);
        }

        w->symbol_name = NULL;
      }
      break;
    }

    m = m->next;
  } while (m != NULL);
}

/*----------------------------------------------------------------------*/

int
b_range_memory_used(const MemBlock *m, int from, int to)
{
  int i, j = 0, page = 0, bytes = 0;

  /* find the starting page */
  while ((m != NULL) && (page < (from / MAX_I_MEM))) {
    j += MAX_I_MEM;
    m = m->next;
  }

  /* count used bytes */
  while ((m != NULL) && (j < to)) {
    for (i = 0; (i < MAX_I_MEM) && (j < to); ++i) {
      if ((m->memory != NULL) && (m->memory[i & I_MEM_MASK].data & BYTE_USED_MASK)) {
        ++bytes;
      }
      ++j;
    }
    m = m->next;
  }

  return bytes;
}

/*----------------------------------------------------------------------*/

int
b_memory_used(const MemBlock *m)
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
unsigned int
i_memory_get_le(const MemBlock *m, unsigned int byte_addr, unsigned short *word,
                const char **section_name, const char **symbol_name)
{
  unsigned int ret;
  bword_t bw;

  ret  = (b_memory_get(m, byte_addr,     &bw.b[0], section_name, symbol_name)) ? W_USED_L : 0;
  ret |= (b_memory_get(m, byte_addr + 1, &bw.b[1], NULL,         NULL))        ? W_USED_H : 0;
  *word = bw.w;
  return ret;
}

/*----------------------------------------------------------------------*/

void
i_memory_put_le(MemBlock *m, unsigned int byte_addr, unsigned short word,
                const char *section_name, const char *symbol_name)
{
  b_memory_put(m, byte_addr,     word & 0xff, section_name, symbol_name);
  b_memory_put(m, byte_addr + 1, word >> 8,   section_name, symbol_name);
}

/*----------------------------------------------------------------------*/

unsigned int
i_memory_get_be(const MemBlock *m, unsigned int byte_addr, unsigned short *word,
                const char **section_name, const char **symbol_name)
{
  unsigned int ret;
  bword_t bw;

  ret  = (b_memory_get(m, byte_addr,     &bw.b[1], section_name, symbol_name)) ? W_USED_H : 0;
  ret |= (b_memory_get(m, byte_addr + 1, &bw.b[0], NULL,         NULL))        ? W_USED_L : 0;
  *word = bw.w;
  return ret;
}

/*----------------------------------------------------------------------*/

void
i_memory_put_be(MemBlock *m, unsigned int byte_addr, unsigned short word,
                const char *section_name, const char *symbol_name)
{
  b_memory_put(m, byte_addr,     word >> 8,   section_name, symbol_name);
  b_memory_put(m, byte_addr + 1, word & 0xff, section_name, symbol_name);
}

/*----------------------------------------------------------------------*/

void
print_i_memory(const MemBlock *m, pic_processor_t processor)
{
  proc_class_t class;
  int base, i, j;
  int org;
  gp_boolean row_used;
  unsigned int w_used;
  bword_t data;
  unsigned char c;

#define WORDS_IN_ROW 8

  class = processor->class;

  while (m != NULL) {
    if (m->memory != NULL) {
      base = m->base << I_MEM_BITS;

      for (i = 0; i < MAX_I_MEM; i += 2 * WORDS_IN_ROW) {
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

/************************************************************************
 *
 *
 *  These functions are used to mark memory as listed.
 *
 *
 ************************************************************************/
void
b_memory_set_listed(MemBlock *m, unsigned int address, unsigned int n_bytes)
{
  unsigned int block = (address >> I_MEM_BITS) & 0xffff;

  while (n_bytes--) {
    while (m != NULL) {
      if (m->base == block) {
        if (m->memory == NULL) {
          m->memory = (MemWord *)GP_Calloc(MAX_I_MEM, sizeof(MemWord));
        }

        m->memory[address & I_MEM_MASK].data |= BYTE_LISTED_MASK;
        break;
      }

      m = m->next;
    }

    ++address;
  }
}

/*----------------------------------------------------------------------*/

unsigned int
b_memory_get_unlisted_size(const MemBlock *m, unsigned int address)
{
  unsigned int n_bytes = 0;

  if ((m != NULL) && (m->memory != NULL)) {
    while (n_bytes < 4) {
      /* find memory block belonging to the address */
      while (((address >> I_MEM_BITS) & 0xffff) != m->base) {
        m = m->next;

        if (m == NULL) {
          return n_bytes;
        }
      }

      if ((m->memory != NULL) &&
          ((m->memory[address & I_MEM_MASK].data & BYTE_ATTR_MASK) == BYTE_USED_MASK)) {
        /* byte at address not listed */
        ++address;
        ++n_bytes;
      }
      else {
        /* byte at address already listed */
        break;
      }
    }
  }

  return n_bytes;
}


/***********************************************************************/

gp_boolean
b_memory_set_addr_type(MemBlock *m, unsigned int address, unsigned int type,
                       unsigned int dest_byte_addr)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;
  MemWord *w;

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

/*----------------------------------------------------------------------*/

unsigned int
b_memory_get_addr_type(const MemBlock *m, unsigned int address, const char **label_name,
                       unsigned int *dest_byte_addr)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;
  MemWord *w;

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
/*----------------------------------------------------------------------*/

gp_boolean
b_memory_set_addr_name(MemBlock *m, unsigned int address, const char *name)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;
  MemWord *w;

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      w = &m->memory[offset];

      if (w->symbol_name == NULL) {
        i_memory_store_symbol_name(w, name);
      }

      return true;
    }

    m = m->next;
  }

  return false;
}

/***********************************************************************/

gp_boolean
b_memory_set_args(MemBlock *m, unsigned int address, unsigned int type, const MemArgList *Args)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;
  MemWord *w;

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

/*----------------------------------------------------------------------*/

unsigned int
b_memory_get_args(const MemBlock *m, unsigned int address, MemArgList *Args)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;
  MemWord *w;

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

/***********************************************************************/

gp_boolean
b_memory_set_type(MemBlock *m, unsigned int address, unsigned int type)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      m->memory[offset].data |= type & W_TYPE_MASK;
      return true;
    }

    m = m->next;
  }

  return false;
}

/***********************************************************************/

gp_boolean
b_memory_clear_type(MemBlock *m, unsigned int address, unsigned int type)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      m->memory[offset].data &= ~(type & W_TYPE_MASK);
      return true;
    }

    m = m->next;
  }

  return false;
}

/***********************************************************************/

unsigned int
b_memory_get_type(const MemBlock *m, unsigned int address)
{
  unsigned int block  = (address >> I_MEM_BITS) & 0xffff;
  unsigned int offset = address & I_MEM_MASK;

  while (m != NULL) {
    if ((m->base == block) && (m->memory != NULL)) {
      return (m->memory[offset].data & W_TYPE_MASK);
    }

    m = m->next;
  }

  return 0;
}

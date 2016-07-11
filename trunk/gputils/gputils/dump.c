/* Displays contents of ".COD" files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Scott Dattalo
   Copyright (C) 2012 Borut Razem

    Copyright (C) 2016 Molnar Karoly <molnarkaroly@users.sf.net>

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
#include "gpvc.h"
#include "dump.h"
#include "block.h"

typedef struct memmap_info {
  unsigned int        start_addr;
  unsigned int        end_addr;
  struct memmap_info *next;
} memmap_info_t;

int number_of_source_files = 0;

static uint8_t  temp[COD_BLOCK_SIZE];
static uint8_t  used_map[COD_BLOCK_SIZE];
static FILE    *source_files[MAX_SOURCE_FILES];

static const char *SymbolType4[154] = {
  "a_reg          ",   "x_reg          ",   "c_short        ",   "c_long         ",
  "c_ushort       ",   "c_ulong        ",   "c_pointer      ",   "c_upointer     ",
  "table          ",   "m_byte         ",   "m_boolean      ",   "m_index        ",
  "byte_array     ",   "u_byte_array   ",   "word_array     ",   "u_word_array   ",
  "func_void_none ",   "func_void_byte ",   "funcVoidTwobyte",   "func_void_long ",
  "func_void_undef",   "func_int_none  ",   "func_int_byte  ",   "func_int_twobyt",
  "func_int_long  ",   "func_int_undef ",   "func_long_none ",   "func_long_byte ",
  "funcLongTwobyte",   "func_long_long ",   "func_long_undef",   "pfun_void_none ",
  "pfun_void_byte ",   "pfunVoidTwobyte",   "pfun_void_long ",   "pfun_void_undef",
  "pfun_int_none  ",   "pfun_int_byte  ",   "pfunIntTwobyte ",   "pfun_int_long  ",
  "pfun_int_undef ",   "pfun_long_none ",   "pfun_long_byte ",   "pfun_long_twoby",
  "pfun_long_long ",   "pfun_long_undef",   "address        ",   "constant       ",
  "bad_und        ",   "br_und         ",   "upper_und      ",   "byte_und       ",
  "add_und        ",   "m_keyword      ",   "add_mi_und     ",   "vector         ",
  "port_w         ",   "port_r         ",   "port_rw        ",   "port_rmw       ",
  "endif          ",   "exundef        ",   "macro_t        ",   "macro_s        ",
  "macro_a        ",   "macro_c        ",   "c_keyword      ",   "void           ",
  "c_enum         ",   "c_typedef1     ",   "c_utypedef1    ",   "c_typedef2     ",
  "c_utypedef2    ",   "cp_typedef1    ",   "cp_utypedef1   ",   "cp_typedef2    ",
  "cp_utypedef2   ",   "c_struct       ",   "i_struct       ",   "l_const        ",
  "s_short        ",   "s_ushort       ",   "s_long         ",   "s_ulong        ",
  "sa_short       ",   "sa_ushort      ",   "sa_long        ",   "sa_ulong       ",
  "sp_short       ",   "sp_ushort      ",   "sp_long        ",   "sp_ulong       ",
  "FixedPointer   ",   "PointerFunction",   "cc_reg         ",   "PtrF_void_none ",
  "PtrF_void_byte ",   "PtrF_void_twobyt",  "PtrF_void_long ",   "PtrF_void_undef",
  "PtrF_int_none  ",   "PtrF_int_byte  ",   "PtrF_int_twobyte",  "PtrF_int_long  ",
  "PtrF_int_undef ",   "PtrF_long_none ",   "PtrF_long_byte ",
  "PtrF_long_twobyte", "PtrF_long_long ",   "PtrF_long_undef",
  "PfAR_void_none ",   "PfAR_void_byte ",   "PfAR_void_twobyte",
  "PfAR_void_long ",   "PfAR_void_undef",   "PfAR_int_none  ",   "PfAR_int_byte  ",
  "PfAR_int_twobyte",  "PfAR_int_long  ",   "PfAR_int_undef ",   "PfAR_long_none ",
  "PfAR_long_byte ",   "PfAR_long_twobyte", "PfAR_long_long ",
  "PfAR_long_undef",   "FWDlit         ",   "Pfunc          ",   "mgoto          ",
  "mcgoto         ",   "mcgoto2        ",   "mcgoto3        ",   "mcgoto4        ",
  "mcgoto74       ",   "mcgoto17       ",   "mccall17       ",   "mcall          ",
  "mc_call        ",   "res_word       ",   "local          ",   "unknown        ",
  "varlabel       ",   "external       ",   "global         ",   "segment        ",
  "Bankaddr       ",   "bit_0          ",   "bit_1          ",   "bit_2          ",
  "bit_3          ",   "bit_4          ",   "bit_5          ",   "bit_6          ",
  "bit_7          ",   "debug          "
};

static memmap_info_t *memmap_info_list      = NULL;
static memmap_info_t *memmap_info_list_last = NULL;

/*------------------------------------------------------------------------------------------------*/

void
_memmap_add(unsigned int Start_addr, unsigned int End_addr)
{
  memmap_info_t *new;

  new = (memmap_info_t *)GP_Calloc(1, sizeof(memmap_info_t));
  new->start_addr = Start_addr;
  new->end_addr   = End_addr;

  if (memmap_info_list == NULL) {
    /* The list is empty. */
    memmap_info_list = new;
  }
  else {
    /* Append the new node to the end of the list. */
    memmap_info_list_last->next = new;
  }

  memmap_info_list_last = new;
}

/*------------------------------------------------------------------------------------------------*/

static void
_memmap_free(void)
{
  memmap_info_t *info;
  memmap_info_t *next;

  info = memmap_info_list;
  while (info != NULL) {
    next = info->next;
    free(info);
    info = next;
  }

  memmap_info_list      = NULL;
  memmap_info_list_last = NULL;
}

/*------------------------------------------------------------------------------------------------*/

static void
_memmap_create_used_map(unsigned int Start_address)
{
  memmap_info_t *info;
  unsigned int   block_start;
  unsigned int   block_end;
  unsigned int   offset;
  unsigned int   size;

  info = memmap_info_list;
  if (info == NULL) {
    memset(used_map, true, sizeof(used_map));
    return;
  }

  memset(used_map, false, sizeof(used_map));

  block_start = (Start_address / COD_BLOCK_SIZE) * COD_BLOCK_SIZE;
  block_end   = block_start + COD_BLOCK_SIZE;
  while (info != NULL) {
    if ((info->start_addr >= block_start) && (info->start_addr < block_end)) {
      offset = info->start_addr - block_start;
      size   = info->end_addr   - info->start_addr;

      if ((offset + size) > COD_BLOCK_SIZE) {
        size = COD_BLOCK_SIZE - offset;
      }

      if (size > 0) {
        /* This is a used ROM range. */
        memset(&used_map[offset], true, size);
      }
    }
    else if (info->start_addr >= block_end) {
      /* The further values will be too high. */
      break;
    }

    info = info->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

/*
  _fget_line - read a line from a file.
*/

static char *
_fget_line(int line, char *buffer, int size, FILE *pFile)
{
  static FILE *plastFile = NULL;
  static int   lastline  = -1;
  static long  lastPos   = -1;

  char        *ps;

  if (pFile == NULL) {
    return NULL;
  }

  /*
    If we read a line from the same file the last time we were called
    then see if we can take advantage of the file state:
   */
  if ((pFile != plastFile) ||       /* if the file is the same */
      (line < (lastline - 1)) ||    /* and the line is past the previous */
      (ftell(pFile) != lastPos)) {  /* and the file hasn't been touched */
    plastFile = pFile;
    lastline = 1;
    rewind(pFile);
    lastPos = -1;
  }

  while (line >= ++lastline) {
    ps = fgets(buffer, size, plastFile);
    assert(ps == buffer);
  }

  ps = fgets(buffer, size, plastFile);
  assert(ps == buffer);

  lastPos = ftell(plastFile);

  return buffer;
}

/*------------------------------------------------------------------------------------------------*/

/*
  substr - copy first n characters from b to a
*/

char *
substr(char *dst, size_t sizeof_dst, const uint8_t *src, size_t sizeof_src)
{
  size_t size = (sizeof_src < sizeof_dst) ? sizeof_src : (sizeof_dst - 1);

  memcpy(dst, src, size);
  dst[size] = '\0';
  return dst;
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump directory block.
 */

static void
_dump_directory_block(const uint8_t *block, unsigned block_num)
{
  char         temp_buf[256];
  unsigned int time;
  unsigned int bytes_for_address;

  printf("Directory block:                %04x\n"
         "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", block_num);

  printf("%03x - Source file:              %s\n",
         COD_DIR_SOURCE, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_SOURCE], COD_FILE_SIZE - 1));
  printf("%03x - Date:                     %s\n",
         COD_DIR_DATE, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_DATE], 7));

  time = gp_getl16(&block[COD_DIR_TIME]);
  printf("%03x - Time:                     %02u:%02u\n",
         COD_DIR_TIME, time / 100, time % 100);

  printf("%03x - Compiler version:         %s\n",
         COD_DIR_VERSION, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_VERSION], 19));
  printf("%03x - Compiler:                 %s\n",
         COD_DIR_COMPILER, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_COMPILER], 11));
  printf("%03x - Notice:                   %s\n",
         COD_DIR_NOTICE, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_NOTICE], COD_FILE_SIZE - 1));

  bytes_for_address = block[COD_DIR_ADDRSIZE];
  printf("%03x - Bytes for address:        %u\n", COD_DIR_ADDRSIZE, bytes_for_address);

  if (bytes_for_address != 0) {
    printf("WARNING: Address size looks suspicious.\n");
  }

  printf("%03x - High word of 64k address: %04x\n",
         COD_DIR_HIGHADDR, gp_getl16(&block[COD_DIR_HIGHADDR]));
  printf("%03x - Next directory block:     %04x\n",
         COD_DIR_NEXTDIR, gp_getl16(&block[COD_DIR_NEXTDIR]));
  printf("%03x - COD file version:         %d\n",
         COD_DIR_CODTYPE, gp_getl16(&block[COD_DIR_CODTYPE]));
  printf("%03x - Processor:                %s\n",
         COD_DIR_PROCESSOR, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_PROCESSOR], 8));

  printf("%03x,%03x - Short symbol table start block: %04x  end block: %04x\n",
         COD_DIR_SYMTAB, COD_DIR_SYMTAB + 2,
         gp_getl16(&block[COD_DIR_SYMTAB]),
         gp_getl16(&block[COD_DIR_SYMTAB + 2]));
  printf("%03x,%03x - File name table start block:    %04x  end block: %04x\n",
         COD_DIR_NAMTAB, COD_DIR_NAMTAB + 2,
         gp_getl16(&block[COD_DIR_NAMTAB]),
         gp_getl16(&block[COD_DIR_NAMTAB + 2]));
  printf("%03x,%03x - Source info table start block:  %04x  end block: %04x\n",
         COD_DIR_LSTTAB, COD_DIR_LSTTAB + 2,
         gp_getl16(&block[COD_DIR_LSTTAB]),
         gp_getl16(&block[COD_DIR_LSTTAB + 2]));
  printf("%03x,%03x - Rom table start block:          %04x  end block: %04x\n",
         COD_DIR_MEMMAP, COD_DIR_MEMMAP + 2,
         gp_getl16(&block[COD_DIR_MEMMAP]),
         gp_getl16(&block[COD_DIR_MEMMAP + 2]));
  printf("%03x,%03x - Local scope table start block:  %04x  end block: %04x\n",
         COD_DIR_LOCALVAR, COD_DIR_LOCALVAR + 2,
         gp_getl16(&block[COD_DIR_LOCALVAR]),
         gp_getl16(&block[COD_DIR_LOCALVAR + 2]));
  printf("%03x,%03x - Long symbol table start block:  %04x  end block: %04x\n",
         COD_DIR_LSYMTAB, COD_DIR_LSYMTAB + 2,
         gp_getl16(&block[COD_DIR_LSYMTAB]),
         gp_getl16(&block[COD_DIR_LSYMTAB + 2]));
  printf("%03x,%03x - Debug messages start block:     %04x  end block: %04x\n",
         COD_DIR_MESSTAB, COD_DIR_MESSTAB + 2,
         gp_getl16(&block[COD_DIR_MESSTAB]),
         gp_getl16(&block[COD_DIR_MESSTAB + 2]));
  putchar('\n');
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump directory block code index.
 */

static void
_dump_index(proc_class_t class, uint8_t *block)
{
  unsigned int _64k_base;
  unsigned int i;
  unsigned int curr_block;
  int          addr_digits;

  printf("Code blocks index:\n"
         "Block range    Block number\n"
         "---------------------------\n");

  _64k_base   = IMemAddrFromBase((unsigned int)gp_getu16(&block[COD_DIR_HIGHADDR]));
  addr_digits = class->addr_digits;

  for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
    if ((curr_block = gp_getu16(&block[i * 2])) != 0) {
      printf("%0*x-%0*x:    %u\n",
             addr_digits, _64k_base | (i << COD_BLOCK_BITS),
             addr_digits, (_64k_base | ((i + 1) << COD_BLOCK_BITS)) - 1,
             curr_block);
    }
  }

  putchar('\n');
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump directory block.
 */

void
dump_directory_blocks(proc_class_t class)
{
  DirBlockInfo *dbi;
  unsigned int  block_num;

  block_num = 0;
  dbi       = main_dir;

  do {
    _dump_directory_block(dbi->dir, block_num);
    _dump_index(class, dbi->dir);
    block_num = gp_getl16(&dbi->dir[COD_DIR_NEXTDIR]);
    dbi = dbi->next;
  } while (dbi != NULL);
}

/*------------------------------------------------------------------------------------------------*/

/*
 * ROM usage information.
 */

void
dump_memmap(proc_class_t class, gp_boolean make_list)
{
  DirBlockInfo *dbi;
  unsigned int  _64k_base;
  int           addr_digits;
  uint16_t      i;
  uint16_t      j;
  uint16_t      start_map_block;
  uint16_t      end_map_block;
  uint16_t      first_offset;
  uint16_t      last_offset;
  unsigned int  start_addr;
  unsigned int  end_addr;
  gp_boolean    first;

  first       = true;
  addr_digits = class->addr_digits;
  dbi         = main_dir;

  do {
    _64k_base       = IMemAddrFromBase((unsigned int)gp_getu16(&dbi->dir[COD_DIR_HIGHADDR]));
    start_map_block = gp_getu16(&dbi->dir[COD_DIR_MEMMAP]);

    if (start_map_block != 0) {
      end_map_block = gp_getu16(&dbi->dir[COD_DIR_MEMMAP + 2]);

      if (first) {
        printf("ROM Usage:\n"
               "----------------------------\n");
        first = false;
      }

      for (j = start_map_block; j <= end_map_block; j++) {
        read_block(temp, j);

        for (i = 0; i < COD_CODE_IMAGE_BLOCKS; i++) {
          first_offset = gp_getl16(&temp[i * COD_MAPENTRY_SIZE + COD_MAPTAB_START]);
          last_offset  = gp_getl16(&temp[i * COD_MAPENTRY_SIZE + COD_MAPTAB_LAST]);

          if ((first_offset != 0) || (last_offset != 0)) {
            start_addr = _64k_base + first_offset;
            end_addr   = _64k_base + last_offset + 1;
            printf("using ROM %0*x to %0*x\n",
                   addr_digits, gp_processor_byte_to_org(class, start_addr),
                   addr_digits, gp_processor_byte_to_org(class, end_addr) - 1);

            if (make_list) {
              _memmap_add(start_addr, end_addr);
            }
          }
        }
      }
    }
    else if (first) {
      printf("No ROM usage information available.\n");
    }

    putchar('\n');

    dbi = dbi->next;
  } while (dbi != NULL);
}

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_is_empty_to_last(unsigned int Index, unsigned int End)
{
  while (Index < End) {
    if (used_map[Index * WORD_SIZE]) {
      return false;
    }

    ++Index;
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump all of the machine code in the .cod file.
 */

#define CODE_COLUMN_NUM         8
#define CODE_COLUMN_NUM_WIDE    16

void
dump_code(proc_class_t class, pic_processor_t processor, gp_boolean wide_dump)
{
  char          border_gap0[16];
  char          border_gap1[16];
  char          buffer[BUFSIZ];
  DirBlockInfo *dbi;
  MemBlock_t   *data;
  int           addr_digits;
  unsigned int  _64k_base;
  unsigned int  block_index;
  unsigned int  byte_address;
  int           num_words;
  unsigned int  bsr_boundary;
  unsigned int  i;
  unsigned int  j;
  unsigned int  k;
  unsigned int  column_num;
  uint16_t      word;
  gp_boolean    used_prev;
  gp_boolean    used_act;
  gp_boolean    empty_signal;
  gp_boolean    empty_line;

  dump_memmap(class, true);

  bsr_boundary = gp_processor_bsr_boundary(processor);
  addr_digits  = class->addr_digits;
  column_num   = (wide_dump) ? CODE_COLUMN_NUM_WIDE : CODE_COLUMN_NUM;
  dbi          = main_dir;

  i = addr_digits + 2;

  j = (i < (sizeof(border_gap0) - 1)) ? i : (sizeof(border_gap0) - 1);
  if (j > 0) {
    memset(border_gap0, ' ', j);
  }

  border_gap0[j] = '\0';

  j = (i < (sizeof(border_gap1) - 1)) ? i : (sizeof(border_gap1) - 1);
  if (j > 0) {
    memset(border_gap1, '-', j);
  }

  border_gap1[j] = '\0';

  printf("Formatted Code Dump:\n");

  do {
    _64k_base = IMemAddrFromBase((unsigned int)gp_getu16(&dbi->dir[COD_DIR_HIGHADDR]));
    for (k = 0; k < COD_CODE_IMAGE_BLOCKS; k++) {
      block_index = gp_getu16(&dbi->dir[(k + COD_DIR_CODE) * WORD_SIZE]);

      if (block_index != 0) {
        read_block(temp, block_index);

        byte_address = _64k_base + (k * COD_BLOCK_N_WORDS) * WORD_SIZE;
        if (wide_dump) {
          printf("%s---------------------------------------------------------------------------------\n"
                 " Block %u -- %0*x-%0*x\n"
                 "%s+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+\n"
                 "%s|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  a |  b |  c |  d |  e |  f |\n"
                 "%s+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+\n",
                 border_gap1,
                 block_index,
                 addr_digits, byte_address,
                 addr_digits, byte_address + COD_BLOCK_SIZE - 1,
                 border_gap1,
                 border_gap0,
                 border_gap1);
        }
        else {
          printf("%s-----------------------------------------\n"
                 " Block %u -- %0*x-%0*x\n"
                 "%s+----+----+----+----+----+----+----+----+\n"
                 "%s| 0;8| 1;9| 2;a| 3;b| 4;c| 5;d| 6;e| 7;f|\n"
                 "%s+----+----+----+----+----+----+----+----+\n",
                 border_gap1,
                 block_index,
                 addr_digits, byte_address,
                 addr_digits, byte_address + COD_BLOCK_SIZE - 1,
                 border_gap1,
                 border_gap0,
                 border_gap1);
        }

        _memmap_create_used_map(byte_address);

	/* Shows the COD block. */

	if (memmap_info_list != NULL) {
	  /* Use the informations of memory map. */
          i = 0;
          do {
            empty_line = true;
            for (j = 0; j < column_num; j++) {
              if (used_map[(i + j) * WORD_SIZE]) {
                empty_line = false;
                break;
              }
            }

            if (! empty_line) {
              byte_address = _64k_base + (k * COD_BLOCK_N_WORDS + i) * WORD_SIZE;
              printf("%0*x: ", addr_digits, gp_processor_byte_to_org(class, byte_address));

              for (j = 0; j < column_num; j++) {
                if (used_map[(i + j) * WORD_SIZE]) {
                  /* This is a used ROM word. */
                  printf(" %04x", gp_getu16(&temp[(i + j) * WORD_SIZE]));
                }
                else if (!_is_empty_to_last(i + j, i + column_num)) {
                  /* In this line there is also at least one used ROM word. */
                  printf(" ....");
                }
                else {
                  /* Sooner ends the line. */
                  break;
                }
              }
              putchar('\n');
            }

            i += column_num;
          } while (i < COD_BLOCK_N_WORDS);
	}
	else {
	  /* No memory map. */
          i = 0;
          do {
            empty_line = true;
            for (j = 0; j < column_num; j++) {
              if (gp_getu16(&temp[(i + j) * WORD_SIZE]) != 0) {
                empty_line = false;
                break;
              }
            }

            if (! empty_line) {
              byte_address = _64k_base + (k * COD_BLOCK_N_WORDS + i) * WORD_SIZE;
              printf("%0*x: ", addr_digits, gp_processor_byte_to_org(class, byte_address));

              for (j = 0; j < column_num; j++) {
                printf(" %04x", gp_getu16(&temp[(i + j) * WORD_SIZE]));
              }

              putchar('\n');
            }

            i += column_num;
          } while (i < COD_BLOCK_N_WORDS);
        }

        putchar('\n');

        /* Create the instruction memory and populates from COD block. */

        byte_address = _64k_base + k * COD_BLOCK_N_WORDS * WORD_SIZE;
        data         = gp_mem_i_create();
        for (j = 0; j < COD_BLOCK_SIZE; j++) {
          gp_mem_b_put(data, byte_address + j, temp[j], NULL, NULL);
        }

        /* Disassembles this code array. */

        i            = 0;
        used_prev    = false;
        empty_signal = false;
        do {
          used_act = used_map[i * WORD_SIZE];

          if (used_act) {
            if (empty_signal) {
              /* This is a previous empty block. */
              printf("  . . . . . . . . . . .\n");
              empty_signal = false;
            }

            if (class->i_memory_get(data, byte_address, &word, NULL, NULL) != W_USED_ALL) {
              /* Internal memory handling error. */
              assert(0);
            }

            num_words = gp_disassemble(data, byte_address, class, bsr_boundary, 0,
                                       GPDIS_SHOW_ALL_BRANCH, buffer, sizeof(buffer), 0);
            printf("%0*x:  %04x  %s\n", addr_digits, gp_processor_byte_to_org(class, byte_address), word, buffer);

            if (num_words != 1) {
              if (class->i_memory_get(data, byte_address + WORD_SIZE, &word, NULL, NULL) != W_USED_ALL) {
                /* Internal memory handling error. */
                assert(0);
              }

              printf("%0*x:  %04x\n", addr_digits, gp_processor_byte_to_org(class, byte_address + WORD_SIZE), word);
            }

            empty_signal = false;
          }
          else if (used_prev && (!used_act)) {
            empty_signal = true;
          }

          used_prev = used_act;

          byte_address += num_words * WORD_SIZE;
          i            += num_words;
        } while (i < COD_BLOCK_N_WORDS);

        gp_mem_i_free(data);
      } /* if (block_index != 0) */
    } /* for (k = 0; k < COD_CODE_IMAGE_BLOCKS; k++) */

    dbi = dbi->next;
  } while (dbi != NULL);

  putchar('\n');
  _memmap_free();
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump all of the (short) Symbol Table stuff in the .cod file.
 */

void
dump_symbols(void)
{
  char         buf[16];
  unsigned int start_block;
  unsigned int end_block;
  unsigned int i;
  unsigned int j;

  start_block = gp_getu16(&main_dir->dir[COD_DIR_SYMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&main_dir->dir[COD_DIR_SYMTAB + 2]);

    printf("Symbol Table Information:\n"
           "-------------------------\n");

    for (j = start_block; j <= end_block; j++) {
      read_block(temp, j);

      for (i = 0; i < SYMBOLS_PER_BLOCK; i++) {
        if (temp[i * SSYMBOL_SIZE + COD_SSYMBOL_NAME]) {
          printf("%-12s = %04x, type = %s\n",
                 substr(buf, sizeof(buf), &temp[i * SSYMBOL_SIZE + COD_SSYMBOL_NAME], 12),
                 gp_getu16(&temp[i * SSYMBOL_SIZE + COD_SSYMBOL_SVALUE]),
                 SymbolType4[(unsigned int)temp[i * SSYMBOL_SIZE + COD_SSYMBOL_STYPE]]);
        }
      }
    }
  }
  else {
    printf("No symbol table info.\n");
  }

  putchar('\n');
}

/*------------------------------------------------------------------------------------------------*/

static unsigned int
_lsymbols_max_length(void)
{
  unsigned int   start_block;
  unsigned int   end_block;
  unsigned int   i;
  unsigned int   j;
  const uint8_t *sym;
  unsigned int   length;
  unsigned int   max_length;

  start_block = gp_getu16(&main_dir->dir[COD_DIR_LSYMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&main_dir->dir[COD_DIR_LSYMTAB + 2]);

    max_length = 0;
    for (j = start_block; j <= end_block; j++) {
      read_block(temp, j);

      for (i = 0; i < COD_BLOCK_SIZE; ) {
        sym    = &temp[i];
        length = *sym;

        if (length == 0) {
          break;
        }

        if (max_length < length) {
          max_length = length;
        }

        i += length + 7;
      }
    }
  }

  return max_length;
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump all of the Long Symbol Table stuff in the .cod file.
 */

void
dump_lsymbols(void)
{
  char           buf[256];
  unsigned int   start_block;
  unsigned int   end_block;
  unsigned int   i;
  unsigned int   j;
  const uint8_t *sym;
  unsigned int   length;
  uint16_t       type;
  unsigned int   value;
  int            symbol_align;

  symbol_align = _lsymbols_max_length();
  start_block  = gp_getu16(&main_dir->dir[COD_DIR_LSYMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&main_dir->dir[COD_DIR_LSYMTAB + 2]);

    printf("Long Symbol Table Information:\n"
           "-----------------------------------------------\n");

    for (j = start_block; j <= end_block; j++) {
      read_block(temp, j);

      for (i = 0; i < COD_BLOCK_SIZE; ) {
        sym    = &temp[i];
        length = *sym;

        if (length == 0) {
          break;
        }

        type = gp_getl16(&sym[length + 1]);

        if (type > 128) {
          type = 0;
        }
        /* read big endian */
        value = gp_getb32(&sym[length + 3]);

	++sym;
        printf("%-*s = %08x, type = %s\n", symbol_align, substr(buf, sizeof(buf), sym, length), value, SymbolType4[type]);
        i += length + 7;
      }
    }
  }
  else {
    printf("No long symbol table info.\n");
  }

  putchar('\n');
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Source files.
 */

void
dump_source_files(void)
{
  unsigned int  start_block;
  unsigned int  end_block;
  unsigned int  i;
  unsigned int  j;
  unsigned int  offset;
  char          b[FILE_SIZE];
  char         *name;

  start_block = gp_getu16(&main_dir->dir[COD_DIR_NAMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&main_dir->dir[COD_DIR_NAMTAB + 2]);

    printf("Source File Information:\n"
           "------------------------\n");

    for (j = start_block; j <= end_block; j++) {
      read_block(temp, j);

      for (i = 0, offset = 0; i < FILES_PER_BLOCK; ++i, offset += FILE_SIZE) {
        substr(b, sizeof (b), &temp[offset + 1], FILE_SIZE);

        if (temp[offset] != 0) {
          name = (char *)GP_Strdup(b);
          source_file_names[number_of_source_files] = name;
          printf("%s\n", name);
          source_files[number_of_source_files] = fopen(name, "rt");
          number_of_source_files++;

          if (number_of_source_files >= MAX_SOURCE_FILES) {
            fprintf(stderr, "Too many source files, increase MAX_SOURCE_FILES and recompile the program.\n");
            exit(1);
          }
        }
      }
    }
  }
  else {
    printf("No source file info.\n");
  }

  putchar('\n');
}

/*------------------------------------------------------------------------------------------------*/

static char *
_smod_flags(int smod)
{
  static char f[9];

  f[0] = (smod & COD_LS_SMOD_FLAG_C1) ? 'C' : '.';
  f[1] = (smod & COD_LS_SMOD_FLAG_F)  ? 'F' : '.';
  f[2] = (smod & COD_LS_SMOD_FLAG_I)  ? 'I' : '.';
  f[3] = (smod & COD_LS_SMOD_FLAG_D)  ? 'D' : '.';
  f[4] = (smod & COD_LS_SMOD_FLAG_C0) ? 'C' : '.';
  f[5] = (smod & COD_LS_SMOD_FLAG_L)  ? 'L' : '.';
  f[6] = (smod & COD_LS_SMOD_FLAG_N)  ? 'N' : '.';
  f[7] = (smod & COD_LS_SMOD_FLAG_A)  ? 'A' : '.';
  f[8] = '\0';

  return f;
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Line number info from the source files.
 */

void
dump_line_symbols(void)
{
  static int    lst_line_number = 1;
  static int    last_src_line = 0;

  char          tline[2048];
  char          nbuf[128];
  const char   *source_file_name;
  DirBlockInfo *dbi;
  gp_boolean    has_line_num_info;
  unsigned int  _64k_base;
  unsigned int  offset;
  unsigned int  start_block;
  unsigned int  end_block;
  unsigned int  i;
  unsigned int  j;
  uint8_t       sfile;
  uint8_t       smod;
  uint16_t      sline;
  uint16_t      sloc;

  has_line_num_info = false;
  dbi = main_dir;
  do {
    _64k_base = gp_getu16(&dbi->dir[COD_DIR_HIGHADDR]);

    start_block = gp_getu16(&dbi->dir[COD_DIR_LSTTAB]);

    if (start_block) {
      end_block = gp_getu16(&dbi->dir[COD_DIR_LSTTAB + 2]);

      if (!has_line_num_info) {
        has_line_num_info = true;

        printf("Line Number Information:\n"
               " LstLn  SrcLn  Addr    Flags        FileName\n"
               " -----  -----  ------  -----------  ---------------------------------------\n");
      }

      for (j = start_block; j <= end_block; j++) {
        read_block(temp, j);

        for (i = 0; i < COD_MAX_LINE_SYM; i++) {
          offset = i * COD_LINE_SYM_SIZE;
          sfile  = temp[offset + COD_LS_SFILE];
          smod   = temp[offset + COD_LS_SMOD];
          sline  = gp_getl16(&temp[offset + COD_LS_SLINE]);
          sloc   = gp_getl16(&temp[offset + COD_LS_SLOC]);

          if (((sfile != 0) || (smod != 0) || (sline != 0) || (sloc != 0)) &&
              ((smod & COD_LS_SMOD_FLAG_L) == 0)) {
            if (sfile < number_of_source_files) {
              source_file_name = source_file_names[sfile];
            }
            else {
              snprintf(nbuf, sizeof(nbuf), "Bad source file index: %d", sfile);
              source_file_name = nbuf;
            }

            printf(" %5d  %5d  %06x  %2x %s  %-50s\n",
                   lst_line_number++, sline,
                   IMemAddrFromBase(_64k_base) | sloc,
                   smod, _smod_flags(smod), source_file_name);

            if ((sfile < number_of_source_files) && (sline != last_src_line)) {
              if (source_files[sfile] != NULL) {
                /*fgets(tline, sizeof(tline), source_files[sfile]);*/
                _fget_line(sline, tline, sizeof(tline), source_files[sfile]);
                printf("%s", tline);
              }
              else {
                printf("ERROR: Source file \"%s\" does not exist.\n", source_file_names[sfile]);
              }
            }
          }
          last_src_line = sline;
        }
      } /* for */
    } /* if */
    dbi = dbi->next;
  } while (dbi != NULL);

  if (!has_line_num_info) {
    printf("No line number info.\n");
  }

  putchar('\n');
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Debug Message area.
 */

void
dump_message_area(void)
{
  char         DebugType;
  char         DebugMessage[MAX_STRING_LEN];
  unsigned int start_block;
  unsigned int end_block;
  unsigned int i;
  unsigned int j;
  uint32_t     laddress;

  start_block = gp_getu16(&main_dir->dir[COD_DIR_MESSTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&main_dir->dir[COD_DIR_MESSTAB + 2]);

    printf("Debug Message area:\n"
           "     Addr  Cmd  Message\n"
           " --------  ---  -------------------------------------\n");

    for (i = start_block; i <= end_block; i++) {
      read_block(temp, i);

      for (j = 0; j < 504; ) {
        /* read big endian */
        laddress = gp_getb32(&temp[j]);
        j += 4;

        DebugType = temp[j++];

        if (DebugType == 0) {
          break;
        }

        substr(DebugMessage, sizeof(DebugMessage), &temp[j], MAX_STRING_LEN);
        j += strlen(DebugMessage);

        printf(" %8x    %c  %s\n", laddress, DebugType, DebugMessage);
      }
    }
  }
  else {
    printf("No Debug Message information available.\n");
  }

  putchar('\n');
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Display the local symbol table information.
 */

void
dump_local_vars(proc_class_t proc_class)
{
  uint8_t      *sh; /* scope_head */
  unsigned int  start_block;
  unsigned int  end_block;
  unsigned int  i;
  unsigned int  j;
  unsigned int  start;
  unsigned int  stop;

  start_block = gp_getu16(&main_dir->dir[COD_DIR_LOCALVAR]);

  if (start_block != 0) {
    end_block = gp_getu16(&main_dir->dir[COD_DIR_LOCALVAR + 2]);

    printf("Local Symbol Scoping Information:\n"
           "---------------------------------\n");

    for (i = start_block; i <= end_block; i++) {
      read_block(temp, i);

      for (j = 0; j < SYMBOLS_PER_BLOCK; j++) {
        sh = &temp[j * SSYMBOL_SIZE];

        if (sh[COD_SSYMBOL_NAME] != '\0') {
          if (memcmp(&sh[COD_SSYMBOL_NAME], "__LOCAL", 7) == 0) {
            start = gp_getl32(&sh[COD_SSYMBOL_START]);
            stop  = gp_getl32(&sh[COD_SSYMBOL_STOP]);

            printf("Local symbols between %06x and %06x:  ",
                   gp_processor_byte_to_org(proc_class, start),
                   gp_processor_byte_to_org(proc_class, stop + 1) - 1);
          }
          else {
            printf("%.12s = %04x, type = %s\n", &sh[COD_SSYMBOL_NAME],
                   gp_getl16(&sh[COD_SSYMBOL_SVALUE]),
                   SymbolType4[(unsigned int)sh[COD_SSYMBOL_STYPE]]);
          }
        }
      }
    }
  }
  else {
    printf("No local variable scoping info available.\n");
  }

  putchar('\n');
}

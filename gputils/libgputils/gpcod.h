/* .cod file support
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Scott Dattalo
   Copyright (C) 2012 Borut Razem
   Copyright (C) 2015 Molnar Karoly

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

#ifndef __GPCOD_H__
#define __GPCOD_H__

/*
 * .cod definitions
 *
 * A .cod file consists of an array of 512 byte blocks. There are two types
 * of blocks: a "directory" block and a "data" block. The directory block
 * describes the miscellaneous stuff like the compiler, the date, copy right
 * and it also describes the type of information that's available in the .cod
 * file. The "type of information" is specified by a range of blocks. For
 * example, if there are symbols in the .cod file then the directory block
 * tells the starting and ending blocks that contain the symbols.
 *
 * Types of data blocks:
 * short symbol table - a list of symbols in the "short format", which
 *     means that the symbol name is restricted to 12 characters. This
 *     is an old format and is not provided by gpasm.
 * long symbol table - a list of symbols in the "long format". Like the
 *     short symbol table except the symbol names can be up to 255 chars.
 * list table - a cross reference between the source line numbers, list
 *     line numbers, and the program memory.
 * Memory map table - describes the ranges of memory used in the processor.
 * Local variables table - [c files - not supported by gpasm] this describes
 *     the memory locations used by functions.
 * Source file names - a list of the files used to assemble the source file.
 * Debug messages - [not supported by gpasm] this provides a list of messages
 *     that can control the simulator or emulator.
 */

#define COD_BLOCK_BITS     9       /* COD_BLOCK_SIZE = 2^COD_BLOCK_BITS */
                                   /* number of bytes in one cod block */
#define COD_BLOCK_SIZE     (1 << COD_BLOCK_BITS)

/*
 * Here's a list of the offsets for the directory block. In each case the
 * offset is the number of bytes from the beginning of the block. Note that
 * it would be much more clever to alias a properly sized structure onto the
 * block. However, without using compiler dependent flags, it's not possible
 * to control how the data members of a structure are packed. Portability
 * has its costs.
 */

#define COD_DIR_CODE       0        /* Code block indices are at the start. */
#define COD_DIR_SOURCE     257      /* Source file name. */
#define COD_DIR_DATE       321      /* Date .cod file was created. */
#define COD_DIR_TIME       328      /* Time .cod file was created. */
#define COD_DIR_VERSION    331      /* Compiler version. */
#define COD_DIR_COMPILER   351      /* Compiler name. */
#define COD_DIR_NOTICE     363      /* Compiler copyright. */
#define COD_DIR_SYMTAB     426      /* Start block of short symbol table. */
#define COD_DIR_NAMTAB     430      /* Start block of file name table. */
#define COD_DIR_LSTTAB     434      /* Start block of list file cross reference. */
#define COD_DIR_ADDRSIZE   438      /* # of bytes for an address. */
#define COD_DIR_HIGHADDR   439      /* High word of address for 64K Code block. */
#define COD_DIR_NEXTDIR    441      /* Next directory block. */
#define COD_DIR_MEMMAP     443      /* Start block of memory map. */
#define COD_DIR_LOCALVAR   447      /* Start block of local variables. */
#define COD_DIR_CODTYPE    451      /* Type of .cod file. */
#define COD_DIR_PROCESSOR  454      /* Target processor. */
#define COD_DIR_LSYMTAB    462      /* Start block of long symbol table. */
#define COD_DIR_MESSTAB    466      /* Start block of debug message area. */

/*
 * Here's a list of sizes of various objects in a .cod file.
 */
#define COD_FILE_SIZE      64       /* Length of filename strings. */

/*
 * MemMapOFS / MemMapend
 */
#define COD_MAPENTRY_SIZE  4

/* MemMapOFS / MemMapend offsets */
#define COD_MAPTAB_START   0
#define COD_MAPTAB_LAST    2

/*
 * Symbol Table
 */
#define SSYMBOL_SIZE       16
#define SYMBOLS_PER_BLOCK  (COD_BLOCK_SIZE / SSYMBOL_SIZE)

/* Symbol Table offsets */
#define COD_SSYMBOL_LEN    0
#define COD_SSYMBOL_NAME   1
#define COD_SSYMBOL_STYPE  13
#define COD_SSYMBOL_SVALUE 14

/* Symbol types */
#define COD_ST_C_SHORT     2
#define COD_ST_ADDRESS     46
#define COD_ST_CONSTANT    47

/* LocalVARS offsets */
#define COD_SSYMBOL_START  8
#define COD_SSYMBOL_STOP   12

/*
 * Source File Name
 */
#define FILE_SIZE          64
#define FILES_PER_BLOCK    (COD_BLOCK_SIZE / FILE_SIZE)

/*
 * Line number info
 */
#define COD_MAX_LINE_SYM   84       /* Number of source lines per cod block. */
#define COD_LINE_SYM_SIZE  6        /* Line symbol structure size. */

/* Line number info offsets */
#define COD_LS_SFILE       0        /* Source file number offset. */
#define COD_LS_SMOD        1        /* Byte of flag info offset. */
#define COD_LS_SLINE       2        /* Line number in source file offset. */
#define COD_LS_SLOC        4        /* Relevant value offset. */

#define COD_LS_SMOD_FLAG_A      0x01
#define COD_LS_SMOD_FLAG_N      0x02
#define COD_LS_SMOD_FLAG_L      0x04
#define COD_LS_SMOD_FLAG_C0     0x08
#define COD_LS_SMOD_FLAG_D      0x10
#define COD_LS_SMOD_FLAG_I      0x20
#define COD_LS_SMOD_FLAG_F      0x40
#define COD_LS_SMOD_FLAG_C1     0x80
#define COD_LS_SMOD_FLAG_ALL    0xff

/*
 * Long Symbol Table
 */
#define COD_SYM_TYPE       1        /* Type info is 1 byte after the length. */
#define COD_SYM_VALUE      3        /* Value info is 3 bytes after the length. */
#define COD_SYM_EXTRA      7        /* Symbol name length + 7 is total structure size. */
#define MAX_SYM_LEN        255      /* Maximum length of a symbol name. */

/*
 * Messages to Source Level Debuggers
 */
#define COD_DEBUG_ADDR     0        /* Type info is first. */
#define COD_DEBUG_CMD      4        /* Value info is 4 bytes after the address. */
#define COD_DEBUG_MSG      6        /* Message is 6 bytes after the address. */
#define COD_DEBUG_EXTRA    6        /* Symbol name length + 6 is total structure size. */
#define MAX_STRING_LEN     255      /* Maximum length of a debug message. */

typedef struct {
  unsigned char *block;
} Block;

typedef struct block_list_struct {
  unsigned char block[COD_BLOCK_SIZE];
  struct block_list_struct *next;
} BlockList;

#define COD_CODE_IMAGE_BLOCKS 128  /* Max # of blocks for the opcodes. */

typedef struct {
  BlockList *first;             /* pointer to the first element of list of blocks */
  BlockList *last;              /* pointer to the last element of list of blocks */
  unsigned int count;           /* number of elements in list of blocks */
  unsigned int offset;          /* offset to empty slot in last block */
} Blocks;

typedef struct dir_block_info {
  unsigned char dir[COD_BLOCK_SIZE];
  Block cod_image_blocks[COD_CODE_IMAGE_BLOCKS];
  Blocks src;                   /* source files blocks */
  Blocks lst;                   /* pointer to the list of line number information blocks */
  Blocks sym;                   /* pointer to the list of long symbol blocks */
  Blocks rng;                   /* pointer to the list of range blocks */
  Blocks dbg;                   /* pointer to the list of debug messages blocks */
  struct dir_block_info *next;  /* pointer to the next dirdctory info block */
} DirBlockInfo;

/* common cod functions */
void gp_cod_strncpy(unsigned char *dest, const char *src, int max_len);
void gp_cod_date(unsigned char *buffer, size_t sizeof_buffer);
void gp_cod_time(unsigned char *buffer, size_t sizeof_buffer);
void gp_cod_create(Block *b);
BlockList *gp_blocks_new(void);
BlockList *gp_blocks_append(Blocks *bl, BlockList *b);
BlockList *gp_blocks_get_last(Blocks *bl);
BlockList *gp_blocks_get_last_or_new(Blocks *bl);
int gp_blocks_count(const Blocks *bl);
void gp_blocks_enumerate(DirBlockInfo *dir, unsigned int offset, Blocks *bl, unsigned int *block_num);
void gp_blocks_enumerate_directory(DirBlockInfo *main_dir);
void gp_blocks_write(FILE *f, Blocks *bl);
void gp_blocks_write_directory(FILE *f, DirBlockInfo *main_dir);
void gp_blocks_free(Blocks *bl);
void gp_blocks_free_directory(DirBlockInfo *main_dir);

#endif

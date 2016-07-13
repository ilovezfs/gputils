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
 * short symbol table    - A list of symbols in the "short format", which means that
 *                         the symbol name is restricted to 12 characters. This is an
 *                         old format and is not provided by gpasm.
 * long symbol table     - A list of symbols in the "long format". Like the short
 *                         symbol table except the symbol names can be up to 255 chars.
 * list table            - A cross reference between the source line numbers, list
 *                         line numbers, and the program memory.
 * Memory map table      - Describes the ranges of memory used in the processor.
 * Local variables table - [c files - not supported by gpasm] this describes
 *                         the memory locations used by functions.
 * Source file names     - A list of the files used to assemble the source file.
 * Debug messages        - [not supported by gpasm] this provides a list of messages
 *     that can control the simulator or emulator.
 */

#define COD_BLOCK_BITS          9           /* COD_BLOCK_SIZE = 2^COD_BLOCK_BITS */

/* Number of bytes in one cod block. */
#define COD_BLOCK_SIZE          (1 << COD_BLOCK_BITS)

/* Number of words in one cod block. */
#define COD_BLOCK_N_WORDS       (COD_BLOCK_SIZE / WORD_SIZE)

/*
 * Here's a list of the offsets for the directory block. In each case the
 * offset is the number of bytes from the beginning of the block. Note that
 * it would be much more clever to alias a properly sized structure onto the
 * block. However, without using compiler dependent flags, it's not possible
 * to control how the data members of a structure are packed. Portability
 * has its costs.
 */

#define COD_DIR_CODE            0           /* Code block indices are at the start. */
#define COD_DIR_SOURCE          257         /* Source file name. */
#define COD_DIR_DATE            321         /* Date .cod file was created. */
#define COD_DIR_TIME            328         /* Time .cod file was created. */
#define COD_DIR_VERSION         331         /* Compiler version. */
#define COD_DIR_COMPILER        351         /* Compiler name. */
#define COD_DIR_NOTICE          363         /* Compiler copyright. */
#define COD_DIR_SYMTAB          426         /* Start block of short symbol table. */
#define COD_DIR_NAMTAB          430         /* Start block of file name table. */
#define COD_DIR_LSTTAB          434         /* Start block of list file cross reference. */
#define COD_DIR_ADDRSIZE        438         /* # of bytes for an address. */
#define COD_DIR_HIGHADDR        439         /* High word of address for 64kB Code block. */
#define COD_DIR_NEXTDIR         441         /* Next directory block. */
#define COD_DIR_MEMMAP          443         /* Start block of memory map. */
#define COD_DIR_LOCALVAR        447         /* Start block of local variables. */
#define COD_DIR_CODTYPE         451         /* Type of .cod file. */
#define COD_DIR_PROCESSOR       454         /* Target processor. */
#define COD_DIR_LSYMTAB         462         /* Start block of long symbol table. */
#define COD_DIR_MESSTAB         466         /* Start block of debug message area. */

/*
 * Here's a list of sizes of various objects in a .cod file.
 */
#define COD_FILE_SIZE           64          /* Length of filename strings. */

/*
 * MemMapOFS / MemMapend
 */
#define COD_MAPENTRY_SIZE       4

/* MemMapOFS / MemMapend offsets */
#define COD_MAPTAB_START        0
#define COD_MAPTAB_LAST         2

/*
 * Symbol Table
 */
#define SSYMBOL_SIZE            16
#define SYMBOLS_PER_BLOCK       (COD_BLOCK_SIZE / SSYMBOL_SIZE)

/* Symbol Table offsets */
#define COD_SSYMBOL_LEN         0
#define COD_SSYMBOL_NAME        1
#define COD_SSYMBOL_STYPE       13
#define COD_SSYMBOL_SVALUE      14

/* Symbol types */
#define COD_ST_C_SHORT          2
#define COD_ST_ADDRESS          46
#define COD_ST_CONSTANT         47

/* LocalVARS offsets */
#define COD_SSYMBOL_START       8
#define COD_SSYMBOL_STOP        12

/*
 * Source File Name
 */
#define FILE_SIZE               64
#define FILES_PER_BLOCK         (COD_BLOCK_SIZE / FILE_SIZE)

/*
 * Line number info
 */
#define COD_MAX_LINE_SYM        84          /* Number of source lines per cod block. */
#define COD_LINE_SYM_SIZE       6           /* Line symbol structure size. */

/* Line number info offsets */
#define COD_LS_SFILE            0           /* Source file number offset. */
#define COD_LS_SMOD             (1 << 0)    /* Byte of flag info offset. */
#define COD_LS_SLINE            (1 << 1)    /* Line number in source file offset. */
#define COD_LS_SLOC             (1 << 2)    /* Relevant value offset. */

#define COD_LS_SMOD_FLAG_A      (1 << 0)
#define COD_LS_SMOD_FLAG_N      (1 << 1)
#define COD_LS_SMOD_FLAG_L      (1 << 2)
#define COD_LS_SMOD_FLAG_C0     (1 << 3)
#define COD_LS_SMOD_FLAG_D      (1 << 4)
#define COD_LS_SMOD_FLAG_I      (1 << 5)
#define COD_LS_SMOD_FLAG_F      (1 << 6)
#define COD_LS_SMOD_FLAG_C1     (1 << 7)
#define COD_LS_SMOD_FLAG_ALL    0xff

/*
 * Long Symbol Table
 */
#define COD_SYM_TYPE            1           /* Type info is 1 byte after the length. */
#define COD_SYM_VALUE           3           /* Value info is 3 bytes after the length. */
#define COD_SYM_EXTRA           7           /* Symbol name length + 7 is total structure size. */
#define MAX_SYM_LEN             255         /* Maximum length of a symbol name. */

/*
 * Messages to Source Level Debuggers
 */
#define COD_DEBUG_ADDR          0           /* Type info is first. */
#define COD_DEBUG_CMD           4           /* Value info is 4 bytes after the address. */
#define COD_DEBUG_MSG           6           /* Message is 6 bytes after the address. */
#define COD_DEBUG_EXTRA         6           /* Symbol name length + 6 is total structure size. */
#define MAX_STRING_LEN          255         /* Maximum length of a debug message. */

typedef struct {
  uint8_t *block;
} Block;

typedef struct block_list_struct {
  uint8_t                   block[COD_BLOCK_SIZE];
  struct block_list_struct *next;
} BlockList;

#define COD_CODE_IMAGE_BLOCKS   128      /* Max # of blocks for the opcodes. */

typedef struct {
  BlockList    *first;          /* pointer to the first element of list of blocks */
  BlockList    *last;           /* pointer to the last element of list of blocks */
  unsigned int  count;          /* number of elements in list of blocks */
  unsigned int  offset;         /* offset to empty slot in last block */
} Blocks;

typedef struct dir_block_info {
  uint8_t                dir[COD_BLOCK_SIZE];
  Block                  cod_image_blocks[COD_CODE_IMAGE_BLOCKS];
  Blocks                 src;   /* source files blocks */
  Blocks                 lst;   /* pointer to the list of line number information blocks */
  Blocks                 sym;   /* pointer to the list of long symbol blocks */
  Blocks                 rng;   /* pointer to the list of range blocks */
  Blocks                 dbg;   /* pointer to the list of debug messages blocks */
  struct dir_block_info *next;  /* pointer to the next dirdctory info block */
} DirBlockInfo;

/* common cod functions */
extern void gp_cod_strncpy(uint8_t *Dest, const char *Src, int Max_len);
extern void gp_cod_date(uint8_t *Buffer, size_t Sizeof_buffer);
extern void gp_cod_time(uint8_t *Buffer, size_t Sizeof_buffer);
extern void gp_cod_create(Block *B);

extern DirBlockInfo *gp_cod_new_dir_block(void);
extern DirBlockInfo *gp_cod_init_dir_block(const char *File_name, const char *Compiler);
extern DirBlockInfo *gp_cod_find_dir_block_by_high_addr(DirBlockInfo *Main, unsigned int High_addr);
extern void gp_cod_emit_opcode(DirBlockInfo *Dbi, unsigned int Address, unsigned int Opcode);
extern void gp_cod_write_code(proc_class_t Class, const MemBlock_t *Mem, DirBlockInfo *Main);

extern BlockList *gp_blocks_new(void);
extern BlockList *gp_blocks_append(Blocks *Bl, BlockList *B);
extern BlockList *gp_blocks_get_last(Blocks *Bl);
extern BlockList *gp_blocks_get_last_or_new(Blocks *Bl);
extern int gp_blocks_count(const Blocks *Bl);
extern void gp_blocks_enumerate(DirBlockInfo *Dir, unsigned int Offset, Blocks *Bl, unsigned int *Block_num);
extern void gp_blocks_enumerate_directory(DirBlockInfo *Main_dir);
extern void gp_blocks_write(FILE *F, Blocks *Bl);
extern void gp_blocks_write_directory(FILE *F, DirBlockInfo *Main_dir);
extern void gp_blocks_free(Blocks *Bl);
extern void gp_blocks_free_directory(DirBlockInfo *Main_dir);

#endif

/* .cod file support
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Scott Dattalo
   Copyright (C) 2012 Borut Razem
   Copyright (C) 2015-2016 Molnar Karoly

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
 * Short symbol table    - A list of symbols in the "short format", which means that
 *                         the symbol name is restricted to 12 characters. This is an
 *                         old format and is not provided by gpasm.
 * Long symbol table     - A list of symbols in the "long format". Like the short
 *                         symbol table except the symbol names can be up to 255 chars.
 * List table            - A cross reference between the source line numbers, list
 *                         line numbers and the program memory.
 * Memory map table      - Describes the ranges of memory used in the processor.
 * Local variables table - This describes the memory locations used by functions.
 *                         [C files - not supported by gpasm]
 * Source file names     - A list of the files used to assemble the source file.
 * Debug messages        - [not supported by gpasm] this provides a list of messages
 *     that can control the simulator or emulator.
 */

#define COD_BLOCK_BITS                  9       /* COD_BLOCK_SIZE = 2^COD_BLOCK_BITS */

/* Number of bytes in one cod block. */
#define COD_BLOCK_SIZE                  (1 << COD_BLOCK_BITS)

/* Number of words in one cod block. */
#define COD_BLOCK_N_WORDS               (COD_BLOCK_SIZE / WORD_SIZE)

#define MemOffsFromBlock(Block)         ((Block) << COD_BLOCK_BITS)

/*
 * Here's a list of the offsets for the directory block. In each case the
 * offset is the number of bytes from the beginning of the block. Note that
 * it would be much more clever to alias a properly sized structure onto the
 * block. However, without using compiler dependent flags, it's not possible
 * to control how the data members of a structure are packed. Portability
 * has its costs.
 */

#define COD_DIR_CODE                    0       /* [0x000] Code block indices are at the start. */
    /* The "COD_DIR_SOURCE" is a Pascal style string. */
#define COD_DIR_SOURCE                  256     /* [0x100] Source file name. */
    /* The "COD_DIR_DATE" is a Pascal style string. */
#define COD_DIR_DATE                    320     /* [0x140] Date .cod file was created. */
#define COD_DIR_TIME                    328     /* [0x148] Time .cod file was created. */
    /* The "COD_DIR_VERSION" is a Pascal style string. */
#define COD_DIR_VERSION                 330     /* [0x14A] Compiler version. */
    /* The "" is a Pascal style string. */
#define COD_DIR_COMPILER                350     /* [0x15E] Compiler name. */
    /* The "COD_DIR_COMPILER" is a Pascal style string. */
#define COD_DIR_NOTICE                  362     /* [0x16A] Compiler copyright. */
#define COD_DIR_SYMTAB                  426     /* [0x1AA] Start and end blocks of short symbol table. */
#define COD_DIR_NAMTAB                  430     /* [0x1AE] Start and end blocks of file name table. */
#define COD_DIR_LSTTAB                  434     /* [0x1B2] Start and end blocks of list file cross reference. */
#define COD_DIR_ADDRSIZE                438     /* [0x1B6] Number of bytes for an address. */
#define COD_DIR_HIGHADDR                439     /* [0x1B7] High word of address for 64kB Code block. */
#define COD_DIR_NEXTDIR                 441     /* [0x1B9] Next directory block. */
#define COD_DIR_MEMMAP                  443     /* [0x1BB] Start and end blocks of memory map. */
#define COD_DIR_LOCALVAR                447     /* [0x1BF] Start and end blocks of local variables. */
#define COD_DIR_CODTYPE                 451     /* [0x1C3] Type of .cod file. */
    /* The "COD_DIR_PROCESSOR" is a Pascal style string. */
#define COD_DIR_PROCESSOR               453     /* [0x1C5] Target processor. */
#define COD_DIR_LSYMTAB                 462     /* [0x1CE] Start and end blocks of long symbol table. */
#define COD_DIR_MESSTAB                 466     /* [0x1D2] Start and end blocks of debug message area. */

#define COD_DIR_SOURCE_SIZE             (COD_DIR_DATE - COD_DIR_SOURCE)         /* 64 */
#define COD_DIR_SOURCE_LENGTH           (COD_DIR_SOURCE_SIZE - 1)               /* 63 */

#define COD_DIR_DATE_SIZE               (COD_DIR_TIME - COD_DIR_DATE)           /* 8 */
#define COD_DIR_DATE_LENGTH             (COD_DIR_DATE_SIZE - 1)                 /* 7 */

#define COD_DIR_TIME_SIZE               (COD_DIR_VERSION - COD_DIR_TIME)        /* 2 */

#define COD_DIR_VERSION_SIZE            (COD_DIR_COMPILER - COD_DIR_VERSION)    /* 20 */
#define COD_DIR_VERSION_LENGTH          (COD_DIR_VERSION_SIZE - 1)              /* 19 */

#define COD_DIR_COMPILER_SIZE           (COD_DIR_NOTICE - COD_DIR_COMPILER)     /* 12 */
#define COD_DIR_COMPILER_LENGTH         (COD_DIR_COMPILER_SIZE)                 /* 11 */

#define COD_DIR_NOTICE_SIZE             (COD_DIR_SYMTAB - COD_DIR_NOTICE)       /* 64 */
#define COD_DIR_NOTICE_LENGTH           (COD_DIR_NOTICE_SIZE - 1)               /* 63 */

#define COD_DIR_PROCESSOR_SIZE          (COD_DIR_LSYMTAB - COD_DIR_PROCESSOR)   /* 9 */
#define COD_DIR_PROCESSOR_LENGTH        (COD_DIR_PROCESSOR_SIZE - 1)            /* 8 */

/*
 * MemMapOFS / MemMapend
 */
#define COD_MAPENTRY_SIZE               4

/* MemMapOFS / MemMapend offsets */
#define COD_MAPTAB_START                0
#define COD_MAPTAB_LAST                 2

/* (Short) Symbol Table
 *
 * List of fields in a record (between the square brackets there is the size of field):
 *
 * length of name --- (COD_SSYMBOL_NAME)
 *  | name
 *  |  |  type ------ (COD_SSYMBOL_STYPE)
 *  |  |   | value -- (COD_SSYMBOL_SVALUE)
 *  |  |   |  |
 *  v  v   v  v
 * [1][12][1][2]
 *
 * SSYMBOL_SIZE = 1 + 12 + 1 + 2 = 16
 */
    /* The name of symbol is a Pascal style string, the first byte the length of string. */
#define COD_SSYMBOL_NAME                0
#define COD_SSYMBOL_STYPE               13
#define COD_SSYMBOL_SVALUE              14

#define COD_SSYMBOL_NAME_SIZE           (COD_SSYMBOL_STYPE - COD_SSYMBOL_NAME)
#define COD_SSYMBOL_NAME_LENGTH         (COD_SSYMBOL_NAME_SIZE - 1)

#define SSYMBOL_SIZE                    16
#define SYMBOLS_PER_BLOCK               (COD_BLOCK_SIZE / SSYMBOL_SIZE)

/* Symbol types */
#define COD_ST_C_SHORT                  2
#define COD_ST_ADDRESS                  46
#define COD_ST_CONSTANT                 47

/* LocalVARS offsets */
#define COD_SSYMBOL_START               8
#define COD_SSYMBOL_STOP                12

/*
 * Source File Name
 */
#define COD_SHORT_FILE_NAME_SIZE        64      /* Short length of filename strings. */
#define COD_SHORT_FILE_NAME_LENGTH      (COD_SHORT_FILE_NAME_SIZE - 1)

#define COD_FILE_NAME_SIZE              256     /* Length of filename strings. */
#define COD_FILE_NAME_LENGTH            (COD_FILE_NAME_SIZE - 1)

#define COD_SHORT_FILE_NAMES_PER_BLOCK  (COD_BLOCK_SIZE / COD_SHORT_FILE_NAME_SIZE)
#define COD_FILE_NAMES_PER_BLOCK        (COD_BLOCK_SIZE / COD_FILE_NAME_SIZE)

/*
 * Line number info
 *
 * List of fields in a record (between the square brackets there is the size of field):
 *
 * file number -------- (COD_LS_SFILE)
 *  | flags ----------- (COD_LS_SMOD)
 *  |  | line number -- (COD_LS_SLINE)
 *  |  |  | location -- (COD_LS_SLOC)
 *  |  |  |  |
 *  v  v  v  v
 * [1][1][2][2]
 *
 * COD_LINE_SYM_SIZE = 1 + 1 + 2 + 2 = 6
 */
#define COD_MAX_LINE_SYM                84          /* Number of source lines per cod block. */
#define COD_LINE_SYM_SIZE               6           /* Line symbol structure size. */

/* Line number info offsets */
#define COD_LS_SFILE                    0           /* Source file number offset. */
#define COD_LS_SMOD                     1           /* Byte of flag info offset. */
#define COD_LS_SLINE                    2           /* Line number in source file offset. */
#define COD_LS_SLOC                     4           /* Relevant value offset. */

#define COD_LS_SMOD_FLAG_A              (1 << 0)
#define COD_LS_SMOD_FLAG_N              (1 << 1)
#define COD_LS_SMOD_FLAG_L              (1 << 2)
#define COD_LS_SMOD_FLAG_C0             (1 << 3)
#define COD_LS_SMOD_FLAG_D              (1 << 4)
#define COD_LS_SMOD_FLAG_I              (1 << 5)
#define COD_LS_SMOD_FLAG_F              (1 << 6)
#define COD_LS_SMOD_FLAG_C1             (1 << 7)
#define COD_LS_SMOD_FLAG_ALL            0xff

/*
 * Long Symbol Table
 *
 * List of fields in a record (between the square brackets there is the size of field, if known):
 *
 * length of name
 *  | name
 *  |  |    type
 *  |  |     | value
 *  |  |     |  |
 *  v  v     v  v
 * [1][....][2][4]
 *
 * COD_LSYMBOL_EXTRA = 1 + 2 + 4 = 7
 */
    /* The name of symbol is a Pascal style string. */
#define COD_LSYMBOL_NAME                0           /* Symbol name in a Pascal style string. */
#define COD_LSYMBOL_NAME_MAX_LENGTH     255         /* Maximum length of a symbol name. */
#define COD_LSYMBOL_NAME_MAX_SIZE       (COD_LSYMBOL_NAME_MAX_LENGTH + 1)

#define COD_LSYMBOL_TYPE                1           /* Type info is 1 byte after the length. */
#define COD_LSYMBOL_VALUE               3           /* Value info is 3 bytes after the length. */
#define COD_LSYMBOL_EXTRA               7           /* Symbol name length + 7 is total structure size. */
#define COD_LSYMBOL_MIN_SIZE            (COD_LSYMBOL_EXTRA + 1) /* Extra bytes + one character length. */

/*
 * Messages to Source Level Debuggers
 *
 * List of fields in a record (between the square brackets there is the size of field, if known):
 *
 * address ------------------- (COD_DEBUG_ADDR)
 *   | command --------------- (COD_DEBUG_CMD)
 *   |  | length of message -- (COD_DEBUG_MSG)
 *   |  |  | message
 *   |  |  |  |
 *   v  v  v  v
 *  [4][1][1][....]
 *
 * COD_DEBUG_EXTRA = 4 + 1 + 1 = 6
 */
#define COD_DEBUG_ADDR                  0           /* Type info is first. */
#define COD_DEBUG_CMD                   4           /* Value info is 4 bytes after the address. */
    /* The "COD_DEBUG_MSG" is a Pascal style string. */
#define COD_DEBUG_MSG                   5           /* Message is 5 bytes after the address. */
#define COD_DEBUG_MSG_MAX_LENGTH        255         /* Maximum length of a debug message. */
#define COD_DEBUG_MSG_MAX_SIZE          (COD_DEBUG_MSG_MAX_LENGTH + 1)
#define COD_DEBUG_EXTRA                 6           /* Symbol name length + 6 is total structure size. */
#define COD_DEBUG_MIN_SIZE              (COD_DEBUG_EXTRA + 1) /* Extra bytes + one character length. */

typedef struct {
  uint8_t *block;
} Block;

typedef struct block_list_struct {
  uint8_t                   block[COD_BLOCK_SIZE];
  struct block_list_struct *next;
} BlockList;

#define COD_CODE_IMAGE_BLOCKS           128         /* Max # of blocks for the opcodes. */

typedef struct {
  BlockList    *first;          /* pointer to the first element of list of blocks */
  BlockList    *last;           /* pointer to the last element of list of blocks */
  unsigned int  count;          /* number of elements in list of blocks */
  unsigned int  offset;         /* offset to empty slot in last block */
} Blocks;

typedef struct dir_block_info {
  uint8_t                dir[COD_BLOCK_SIZE];
  Block                  cod_image_blocks[COD_CODE_IMAGE_BLOCKS];
  Blocks                 file;  /* pointer to the list of source files blocks */
  Blocks                 list;  /* pointer to the list of source line number information blocks */
  Blocks                 range; /* pointer to the list of ROM range blocks */
  Blocks                 lsym;  /* pointer to the list of long symbol blocks */
  Blocks                 debug; /* pointer to the list of debug messages blocks */
  struct dir_block_info *next;  /* pointer to the next directory info block */
} DirBlockInfo;

/* common cod functions */
extern void gp_cod_create(Block *B);

extern DirBlockInfo *gp_cod_new_dir_block(void);
extern DirBlockInfo *gp_cod_init_dir_block(const char *File_name, const char *Compiler);
extern DirBlockInfo *gp_cod_find_dir_block_by_high_addr(DirBlockInfo *Main, unsigned int High_addr);
extern void gp_cod_emit_opcode(DirBlockInfo *Dbi, unsigned int Address, unsigned int Opcode);
extern void gp_cod_write_code(proc_class_t Class, const MemBlock_t *Mem, DirBlockInfo *Main);
extern size_t gp_cod_put_long_symbol(uint8_t *Record, const char *Name, gp_symvalue_t Value, unsigned int Type);
extern size_t gp_cod_put_debug_symbol(uint8_t *Record, const char *String, gp_symvalue_t Value, char Command);

extern size_t gp_cod_put_line_number(uint8_t *Record, unsigned int File_id, unsigned int Line_number,
                                     unsigned int Address, unsigned int Flag);

extern BlockList *gp_cod_block_new(void);
extern BlockList *gp_cod_block_append(Blocks *Bl, BlockList *B);
extern BlockList *gp_cod_block_get_last(Blocks *Bl);
extern BlockList *gp_cod_block_get_last_or_new(Blocks *Bl);
extern int gp_cod_block_count(const Blocks *Bl);
extern void gp_cod_block_enumerate(DirBlockInfo *Dir, unsigned int Offset, Blocks *Bl, unsigned int *Block_num);
extern void gp_cod_enumerate_directory(DirBlockInfo *Main_dir);
extern void gp_cod_block_write(FILE *F, const Blocks *Bl);
extern void gp_cod_write_directory(FILE *F, const DirBlockInfo *Main_dir);
extern void gp_cod_block_free(Blocks *Bl);
extern void gp_cod_free_directory(DirBlockInfo *Main_dir);

#endif

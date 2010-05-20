/* .cod file support
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
#define COD_BLOCK_SIZE     (1<<COD_BLOCK_BITS)

/*
 * Here's a list of the offsets for the directory block. In each case the
 * offset is the number of bytes from the beginning of the block. Note that
 * it would be much more clever to alias a properly sized structure onto the
 * block. However, without using compiler dependent flags, it's not possible
 * to control how the data members of a structure are packed. Portability 
 * has its costs.
 */

#define COD_DIR_CODE       0       /* code block indices are at the start */
#define COD_DIR_SOURCE     257     /* source file name */
#define COD_DIR_DATE       321     /* date .cod file was created */
#define COD_DIR_TIME       328     /* time .cod file was created */
#define COD_DIR_VERSION    331     /* Compiler version */
#define COD_DIR_COMPILER   351     /* Compiler name */
#define COD_DIR_NOTICE     363     /* Compiler copyright */
#define COD_DIR_SYMTAB     426     /* Start block of short symbol table */
#define COD_DIR_NAMTAB     430     /* Start block of file name table */
#define COD_DIR_LSTTAB     434     /* Start block of list file cross reference */
#define COD_DIR_ADDRSIZE   438     /* # of bytes for an address */
#define COD_DIR_HIGHADDR   439     /* High word of address for 64K Code block */
#define COD_DIR_NEXTDIR    441     /* Next directory block */
#define COD_DIR_MEMMAP     443     /* Start block of memory map */
#define COD_DIR_LOCALVAR   447     /* Start block of local variables */
#define COD_DIR_CODTYPE    451     /* Type of .cod file */
#define COD_DIR_PROCESSOR  454     /* Target processor */
#define COD_DIR_LSYMTAB    462     /* Start block of long symbol table */
#define COD_DIR_MESSTAB    466     /* Start block of debug message area */

/*
 * Here's a list of sizes of various objects in a .cod file.
 */
#define COD_FILE_SIZE      64      /* Length of filename strings */
#define COD_MAX_LINE_SYM   84      /* Number of source lines per cod block */
#define COD_LINE_SYM_SIZE   6      /* Line symbol structure size */

enum cod_block_types {
  cb_nobody,
  cb_dir,
  cb_file,
  cb_list,
  cb_ssymbols,
  cb_lsymbols,
  cb_code,
  cb_range
};

/* If you're using gnu's gcc compiler you could get direct access to the
 * block structure by specifying the '-fpack-struct' compiler option and
 * overlaying (i.e. alias or typecast) the structures on top of the blocks.
 */
#define notice_len      63
#define version_len     19
#define compiler_len    11
#define source_len      63
#define spare_len       42

typedef struct directory {
  unsigned short index[128];       /* code block pointers */
  char source_strlen;              /* Length of the source string */
  char source[source_len];         /* Source file name */
  char date_strlen;                /* Length of the date string */
  char date[7];                    /* Date string */
  unsigned short time;             /* Hours*100 + Minutes */
  char version_strlen;             /* Length of the version string */
  char version[version_len];       /* Compiler Version number */
  char compiler_strlen;            /* Length of the compiler string */
  char compiler[compiler_len];     /* Name of the compiler */
  char notice_strlen;              /* Length of the notice string */
  char notice[notice_len];         /* Notice */

  unsigned short 
    symtab, symend, 
    namtab, namend, 
    lsttab, lstend;
  char AddrSize;                   /* # of bytes for address: 2,3,4   0 = 2   */
  unsigned short HighAddr;         /* High word of address for 64K Code block */
  unsigned short NextDir;          /* Block number of the next directory      */

  unsigned short 
  MemMapOFS, MemMapend;            /* Memory map block number  0 = all used   */
  unsigned short
  LocalVARS,Localend;              /* Local variables and scope start block   */

  unsigned short codtype;

  char processor_strlen;           /* Length of the processor string */
  char processor[8];

  unsigned short 
  Lsymtab, Lsymend;                /* long symbol table        */

  unsigned short 
  MessTab, MessEnd;                /* Debug Message area */

  unsigned char spare[spare_len + 1];
} Directory;

/*
 * LineSymbol
 *
 * An array of Line symbols provides cross reference info between
 * the assembled object code, the source asm file(s), and the lst
 * file. The index into the array corresponds to the lst file line
 * number, hence the lst file cross reference information is not
 * explicitly written to the cod file. The symbol provides information
 * about the source file and its current line number and to which
 * program memory address within the pic that these correspond.
 *
 */

typedef struct LineSymbol {
  unsigned char sfile;
  unsigned char smod;
  unsigned short sline;
  unsigned short sloc;
} LineSymbol;

typedef struct block_struct {
  unsigned char *block;
  int  block_number;
} Block;

#define COD_CODE_IMAGE_BLOCKS 128  /* Max # of blocks for the opcodes */

typedef struct dir_block_info {
  Block dir;
  Block cod_image_blocks[COD_CODE_IMAGE_BLOCKS];
  struct dir_block_info *next_dir_block_info;
} DirBlockInfo;


#define COD_LS_SFILE        0      /* offset of sfile in LineSymbol struct */
#define COD_LS_SMOD         1      /*  "        smod  " */
#define COD_LS_SLINE        2      /*  "        sline " */
#define COD_LS_SLOC         4      /*  "        sloc  " */

/*
 * Symbol types
 */
#define COD_ST_C_SHORT       2
#define COD_ST_ADDRESS      46
#define COD_ST_CONSTANT     47

/* common cod functions */
void gp_cod_strncpy(unsigned char *dest, const char *src, int max_len);
void gp_cod_clear(Block *b);
void gp_cod_delete(Block *b);
void gp_cod_next(Block *b, int *block_number);
void gp_cod_create(Block *b, int *block_number);
void gp_cod_date(unsigned char *buffer, size_t sizeof_buffer);
void gp_cod_time(unsigned char *buffer, size_t sizeof_buffer);

#endif

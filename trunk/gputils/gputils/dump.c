/* Displays contents of ".COD" files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Scott Dattalo
   Copyright (C) 2012 Borut Razem

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

int number_of_source_files = 0;

static uint8_t  temp[COD_BLOCK_SIZE];
static FILE    *source_files[MAX_SOURCE_FILES];

static const char *SymbolType4[154] = {
  "a_reg          ", "x_reg          ", "c_short        ", "c_long         ",
  "c_ushort       ", "c_ulong        ", "c_pointer      ", "c_upointer     ",
  "table          ", "m_byte         ", "m_boolean      ", "m_index        ",
  "byte_array     ", "u_byte_array   ", "word_array     ", "u_word_array   ",
  "func_void_none ", "func_void_byte ", "funcVoidTwobyte", "func_void_long ",
  "func_void_undef", "func_int_none  ", "func_int_byte  ", "func_int_twobyt",
  "func_int_long  ", "func_int_undef ", "func_long_none ", "func_long_byte ",
  "funcLongTwobyte", "func_long_long ", "func_long_undef", "pfun_void_none ",
  "pfun_void_byte ", "pfunVoidTwobyte", "pfun_void_long ", "pfun_void_undef",
  "pfun_int_none  ", "pfun_int_byte  ", "pfunIntTwobyte ", "pfun_int_long  ",
  "pfun_int_undef ", "pfun_long_none ", "pfun_long_byte ", "pfun_long_twoby",
  "pfun_long_long ", "pfun_long_undef", "address        ", "constant       ",
  "bad_und        ", "br_und         ", "upper_und      ", "byte_und       ",
  "add_und        ", "m_keyword      ", "add_mi_und     ", "vector         ",
  "port_w         ", "port_r         ", "port_rw        ", "port_rmw       ",
  "endif          ", "exundef        ", "macro_t        ", "macro_s        ",
  "macro_a        ", "macro_c        ", "c_keyword      ", "void           ",
  "c_enum         ", "c_typedef1     ", "c_utypedef1    ", "c_typedef2     ",
  "c_utypedef2    ", "cp_typedef1    ", "cp_utypedef1   ", "cp_typedef2    ",
  "cp_utypedef2   ", "c_struct       ", "i_struct       ", "l_const        ",
  "s_short        ", "s_ushort       ", "s_long         ", "s_ulong        ",
  "sa_short       ", "sa_ushort      ", "sa_long        ", "sa_ulong       ",
  "sp_short       ", "sp_ushort      ", "sp_long        ", "sp_ulong       ",
  "FixedPointer   ", "PointerFunction", "cc_reg         ", "PtrF_void_none ",
  "PtrF_void_byte ", "PtrF_void_twobyt", "PtrF_void_long ", "PtrF_void_undef",
  "PtrF_int_none  ", "PtrF_int_byte  ", "PtrF_int_twobyte", "PtrF_int_long  ",
  "PtrF_int_undef ", "PtrF_long_none ", "PtrF_long_byte ",
  "PtrF_long_twobyte", "PtrF_long_long ", "PtrF_long_undef",
  "PfAR_void_none ", "PfAR_void_byte ", "PfAR_void_twobyte",
  "PfAR_void_long ", "PfAR_void_undef", "PfAR_int_none  ", "PfAR_int_byte  ",
  "PfAR_int_twobyte", "PfAR_int_long  ", "PfAR_int_undef ", "PfAR_long_none ",
  "PfAR_long_byte ", "PfAR_long_twobyte", "PfAR_long_long ",
  "PfAR_long_undef", "FWDlit         ", "Pfunc          ", "mgoto          ",
  "mcgoto         ", "mcgoto2        ", "mcgoto3        ", "mcgoto4        ",
  "mcgoto74       ", "mcgoto17       ", "mccall17       ", "mcall          ",
  "mc_call        ", "res_word       ", "local          ", "unknown        ",
  "varlabel       ", "external       ", "global         ", "segment        ",
  "Bankaddr       ", "bit_0          ", "bit_1          ", "bit_2          ",
  "bit_3          ", "bit_4          ", "bit_5          ", "bit_6          ",
  "bit_7          ", "debug          "
};

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
substr(char *a, size_t sizeof_a, const uint8_t *b, size_t n)
{
  size_t m = (n < sizeof_a) ? n : (sizeof_a - 1);

  memcpy(a, b, m);
  a[m] = '\0';
  return a;
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump directory block.
 */

static void
_dump_directory_block(const uint8_t *block, unsigned block_num)
{
  char temp_buf[256];
  int  time;
  int  bytes_for_address;

  printf("Directory block:                %04x\n"
         "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", block_num);

  printf("%03x - Source file:              %s\n",
         COD_DIR_SOURCE, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_SOURCE], 63));
  printf("%03x - Date:                     %s\n",
         COD_DIR_DATE, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_DATE], 7));

  time = gp_getl16(&block[COD_DIR_TIME]);
  printf("%03x - Time:                     %02d:%02d\n",
         COD_DIR_TIME, time / 100, time % 100);

  printf("%03x - Compiler version:         %s\n",
         COD_DIR_VERSION, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_VERSION], 19));
  printf("%03x - Compiler:                 %s\n",
         COD_DIR_COMPILER, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_COMPILER], 11));
  printf("%03x - Notice:                   %s\n",
         COD_DIR_NOTICE, substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_NOTICE], 63));

  bytes_for_address = block[COD_DIR_ADDRSIZE];
  printf("%03x - Bytes for address:        %d\n", COD_DIR_ADDRSIZE, bytes_for_address);

  if (bytes_for_address != 0) {
    printf("WARNING: address size looks suspicious\n");
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
_dump_index(uint8_t *block)
{
  unsigned int _64k_base = IMemAddrFromBase((unsigned int)gp_getu16(&block[COD_DIR_HIGHADDR]));
  int          i;
  int          curr_block;

  printf("Code blocks index:\n"
         "Block range    Block number\n"
         "---------------------------\n");

  for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
    if ((curr_block = gp_getu16(&block[i * 2])) != 0) {
      printf("%06x-%06x: %d\n", _64k_base | (i << COD_BLOCK_BITS),
             (_64k_base | ((i + 1) << COD_BLOCK_BITS)) - 1, curr_block);
    }
  }
  putchar('\n');
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump directory block.
 */

void
dump_directory_blocks(void)
{
  DirBlockInfo *dbi = main_dir;
  unsigned int  block_num = 0;

  do {
    _dump_directory_block(dbi->dir, block_num);
    _dump_index(dbi->dir);
    block_num = gp_getl16(&dbi->dir[COD_DIR_NEXTDIR]);
    dbi = dbi->next;
  } while (dbi != NULL);
}

/*------------------------------------------------------------------------------------------------*/

/*
 * ROM usage information.
 */

void
dump_memmap(proc_class_t proc_class)
{
  unsigned int  _64k_base;
  DirBlockInfo *dbi;
  uint16_t      i;
  uint16_t      j;
  uint16_t      start_block;
  uint16_t      end_block;
  uint16_t      start;
  uint16_t      last;
  gp_boolean    first = true;

  dbi = main_dir;

  do {
    _64k_base   = IMemAddrFromBase((unsigned int)gp_getu16(&dbi->dir[COD_DIR_HIGHADDR]));
    start_block = gp_getu16(&dbi->dir[COD_DIR_MEMMAP]);

    if (start_block) {
      end_block = gp_getu16(&dbi->dir[COD_DIR_MEMMAP + 2]);

      if (first) {
        printf("ROM Usage:\n"
               "--------------------------------\n");
        first = false;
      }
      for (j = start_block; j <= end_block; j++) {
        read_block(temp, j);

        for (i = 0; i < COD_CODE_IMAGE_BLOCKS; i++) {
          start = gp_getl16(&temp[i * COD_MAPENTRY_SIZE + COD_MAPTAB_START]);
          last  = gp_getl16(&temp[i * COD_MAPENTRY_SIZE + COD_MAPTAB_LAST]);

          if (!((start == 0) && (last == 0))) {
            printf("using ROM %06x to %06x\n",
                   gp_processor_byte_to_org(proc_class, _64k_base + start),
                   gp_processor_byte_to_org(proc_class, _64k_base + last + 1) - 1);
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

/*
 * Dump all of the machine code in the .cod file.
 */

void
dump_code(proc_class_t proc_class)
{
  unsigned int  _64k_base;
  uint16_t      i;
  uint16_t      j;
  uint16_t      k;
  uint16_t      index;
  gp_boolean    all_zero_line;
  DirBlockInfo *dbi;

  dump_memmap(proc_class);

  printf("Formatted Code Dump:\n"
         "--------------------\n");

  dbi = main_dir;

  do {
    _64k_base = IMemAddrFromBase((unsigned int)gp_getu16(&dbi->dir[COD_DIR_HIGHADDR]));
    for (k = 0; k < COD_CODE_IMAGE_BLOCKS; k++) {
      index = gp_getu16(&dbi->dir[2 * (COD_DIR_CODE + k)]);

      if (index != 0) {
        read_block(temp, index);

        i = 0;
        do {
          for (j = 0, all_zero_line = true; j < 8; j++) {
            if (gp_getu16(&temp[(i + j) * 2])) {
              all_zero_line = false;
            }
          }

          if (all_zero_line) {
            i += 8;
          }
          else {
            printf("%06x:  ", gp_processor_byte_to_org(proc_class, _64k_base + 2 * (i + k * 256)));

            for (j = 0; j < 8; j++) {
              printf("%04x ", gp_getu16(&temp[2 * i++]));
            }

            putchar('\n');
          }
        } while (i < (COD_BLOCK_SIZE / 2));

        putchar('\n');
      }
    }

    putchar('\n');

    dbi = dbi->next;
  } while (dbi != NULL);
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump all of the (short) Symbol Table stuff in the .cod file.
 */

void
dump_symbols(void)
{
  uint16_t i;
  uint16_t j;
  uint16_t start_block;
  uint16_t end_block;
  char     b[16];

  start_block = gp_getu16(&main_dir->dir[COD_DIR_SYMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&main_dir->dir[COD_DIR_SYMTAB + 2]);

    printf("Symbol Table Information:\n"
           "-------------------------\n");

    for (j = start_block; j <= end_block; j++) {
      read_block(temp, j);

      for (i = 0; i < SYMBOLS_PER_BLOCK; i++) {
        if (temp[i * SSYMBOL_SIZE + COD_SSYMBOL_NAME]) {
          printf("%s = %x, type = %s\n",
                 substr(b, sizeof(b), &temp[i * SSYMBOL_SIZE + COD_SSYMBOL_NAME], 12),
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

/*
 * Dump all of the Long Symbol Table stuff in the .cod file.
 */

void
dump_lsymbols(void)
{
  uint8_t      *s;
  uint8_t       length;
  short         type;
  uint16_t      i;
  uint16_t      j;
  uint16_t      start_block;
  uint16_t      end_block;
  unsigned int  value;
  char          b[256];

  start_block = gp_getu16(&main_dir->dir[COD_DIR_LSYMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&main_dir->dir[COD_DIR_LSYMTAB + 2]);
    end_block = gp_getu16(&main_dir->dir[COD_DIR_LSYMTAB + 2]);

    printf("Long Symbol Table Information:\n"
           "------------------------------\n");

    for (j = start_block; j <= end_block; j++) {
      read_block(temp, j);

      for (i = 0; i < COD_BLOCK_SIZE; ) {
        s = &temp[i];

        if (*s == 0) {
          break;
        }

        length = *s;
        type   = gp_getl16(&s[length + 1]);

        if (type > 128) {
          type = 0;
        }
        /* read big endian */
        value = gp_getb32(&s[length + 3]);

        printf("%s = %x, type = %s\n", substr(b, sizeof(b), &s[1], length), value, SymbolType4[type]);
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
  uint16_t  i;
  uint16_t  j;
  uint16_t  start_block;
  uint16_t  end_block;
  uint16_t  offset;
  char      b[FILE_SIZE];
  char     *name;

  start_block = gp_getu16(&main_dir->dir[COD_DIR_NAMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&main_dir->dir[COD_DIR_NAMTAB + 2]);
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
  uint16_t      i;
  uint16_t      j;
  uint16_t      start_block;
  uint16_t      end_block;
  DirBlockInfo *dbi = main_dir;
  gp_boolean    has_line_num_info = false;
  int           _64k_base;
  char          nbuf[128];
  char         *source_file_name;

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
          unsigned int offset = i * COD_LINE_SYM_SIZE;

          uint8_t  sfile = temp[offset + COD_LS_SFILE];
          uint8_t  smod  = temp[offset + COD_LS_SMOD];
          uint16_t sline = gp_getl16(&temp[offset + COD_LS_SLINE]);
          uint16_t sloc  = gp_getl16(&temp[offset + COD_LS_SLOC]);

          if (((sfile != 0) || (smod != 0) || (sline != 0) || (sloc != 0)) &&
              ((smod & COD_LS_SMOD_FLAG_L) == 0)) {
            if (sfile < number_of_source_files) {
              source_file_name = source_file_names[sfile];
            }
            else {
              snprintf(nbuf, sizeof(nbuf), "Bad source file index: %d", sfile);
              source_file_name = nbuf;
            }

            printf(" %5d  %5d  %06X  %2x %s  %-50s\n",
                   lst_line_number++, sline, IMemAddrFromBase(_64k_base) | sloc,
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
  char     DebugType;
  char     DebugMessage[MAX_STRING_LEN];
  uint16_t i;
  uint16_t j;
  uint16_t start_block;
  uint16_t end_block;
  uint16_t laddress;

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
  uint16_t      i;
  uint16_t      j;
  uint16_t      start_block;
  uint16_t      end_block;
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
          } else {
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

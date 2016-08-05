/* Displays contents of ".COD" files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Scott Dattalo
   Copyright (C) 2012 Borut Razem
   Copyright (C) 2016 Molnar Karoly

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

#define MAX_SOURCE_FILES        100

typedef struct memmap_info {
  unsigned int        start_addr;
  unsigned int        end_addr;
  struct memmap_info *next;
} memmap_info_t;

static uint8_t       cod_block[COD_BLOCK_SIZE];
static uint8_t       used_map[COD_BLOCK_SIZE];

static unsigned int  number_of_source_files              = 0;
static char         *source_file_names[MAX_SOURCE_FILES] = { NULL, };
static FILE         *source_files[MAX_SOURCE_FILES]      = { NULL, };

static const char *symbol_type_str[] = {
  "a_reg"            , "x_reg"             , "c_short"          , "c_long"            , /*   0 -   3 */
  "c_ushort"         , "c_ulong"           , "c_pointer"        , "c_upointer"        , /*   4 -   7 */
  "table"            , "m_byte"            , "m_boolean"        , "m_index"           , /*   8 -  11 */
  "byte_array"       , "u_byte_array"      , "word_array"       , "u_word_array"      , /*  12 -  15 */
  "func_void_none"   , "func_void_byte"    , "func_void_twobyte", "func_void_long"    , /*  16 -  19 */
  "func_void_undef"  , "func_int_none"     , "func_int_byte"    , "func_int_twobyte"  , /*  20 -  23 */
  "func_int_long"    , "func_int_undef"    , "func_long_none"   , "func_long_byte"    , /*  24 -  27 */
  "func_long_twobyte", "func_long_long"    , "func_long_undef"  , "pfunc_void_none"   , /*  28 -  31 */
  "pfunc_void_byte"  , "pfunc_void_twobyte", "pfunc_void_long"  , "pfunc_void_undef"  , /*  32 -  35 */
  "pfunc_int_none"   , "pfunc_int_byte"    , "pfunc_int_twobyte", "pfunc_int_long"    , /*  36 -  39 */
  "pfunc_int_undef"  , "pfunc_long_none"   , "pfunc_long_byte"  , "pfunc_long_twobyte", /*  40 -  43 */
  "pfunc_long_long"  , "pfunc_long_undef"  , "address"          , "constant"          , /*  44 -  47 */
  "bad_und"          , "br_und"            , "upper_und"        , "byte_und"          , /*  48 -  51 */
  "add_und"          , "m_keyword"         , "add_mi_und"       , "vector"            , /*  52 -  55 */
  "port_w"           , "port_r"            , "port_rw"          , "port_rmw"          , /*  56 -  59 */
  "endif"            , "exundef"           , "macro_t"          , "macro_s"           , /*  60 -  63 */
  "macro_a"          , "macro_c"           , "c_keyword"        , "void"              , /*  64 -  67 */
  "c_enum"           , "c_typedef1"        , "c_utypedef1"      , "c_typedef2"        , /*  68 -  71 */
  "c_utypedef2"      , "cp_typedef1"       , "cp_utypedef1"     , "cp_typedef2"       , /*  72 -  75 */
  "cp_utypedef2"     , "c_struct"          , "i_struct"         , "l_const"           , /*  76 -  79 */
  "s_short"          , "s_ushort"          , "s_long"           , "s_ulong"           , /*  80 -  83 */
  "sa_short"         , "sa_ushort"         , "sa_long"          , "sa_ulong"          , /*  84 -  87 */
  "sp_short"         , "sp_ushort"         , "sp_long"          , "sp_ulong"          , /*  88 -  91 */
  "fixed_pointer"    , "pointer_function"  , "cc_reg"           , "PtrF_void_none"    , /*  92 -  95 */
  "PtrF_void_byte"   , "PtrF_void_twobyte" , "PtrF_void_long"   , "PtrF_void_undef"   , /*  96 -  99 */
  "PtrF_int_none"    , "PtrF_int_byte"     , "PtrF_int_twobyte" , "PtrF_int_long"     , /* 100 - 103 */
  "PtrF_int_undef"   , "PtrF_long_none"    , "PtrF_long_byte"   , "PtrF_long_twobyte" , /* 104 - 107 */
  "PtrF_long_long"   , "PtrF_long_undef"   , "PfAR_void_none"   , "PfAR_void_byte"    , /* 108 - 111 */
  "PfAR_void_twobyte", "PfAR_void_long"    , "PfAR_void_undef"  , "PfAR_int_none"     , /* 112 - 115 */
  "PfAR_int_byte"    , "PfAR_int_twobyte"  , "PfAR_int_long"    , "PfAR_int_undef"    , /* 116 - 119 */
  "PfAR_long_none"   , "PfAR_long_byte"    , "PfAR_long_twobyte", "PfAR_long_long"    , /* 120 - 123 */
  "PfAR_long_undef"  , "FWDlit"            , "Pfunc"            , "mgoto"             , /* 124 - 127 */
  "mcgoto"           , "mcgoto2"           , "mcgoto3"          , "mcgoto4"           , /* 128 - 131 */
  "mcgoto74"         , "mcgoto17"          , "mccall17"         , "mcall"             , /* 132 - 135 */
  "mc_call"          , "res_word"          , "local"            , "unknown"           , /* 136 - 139 */
  "varlabel"         , "external"          , "global"           , "segment"           , /* 140 - 143 */
  "bank_addr"        , "bit_0"             , "bit_1"            , "bit_2"             , /* 144 - 147 */
  "bit_3"            , "bit_4"             , "bit_5"            , "bit_6"             , /* 148 - 151 */
  "bit_7"            , "debug"                                                          /* 152 - 153 */
};

static memmap_info_t *memmap_info_list      = NULL;
static memmap_info_t *memmap_info_list_last = NULL;

/*------------------------------------------------------------------------------------------------*/

static const char *
_symbol_type_to_str(unsigned int Type)
{
  static char err[64];

  if (Type < ARRAY_SIZE(symbol_type_str)) {
    return symbol_type_str[Type];
  }

  snprintf(err, sizeof(err), "invalid type: %u", Type);
  return err;
}

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
  const memmap_info_t *info;
  unsigned int         block_start;
  unsigned int         block_end;
  unsigned int         offset;
  unsigned int         size;

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
    else if ((info->start_addr < block_start) && (info->start_addr < block_end)) {
      size = info->end_addr - block_start;

      if (size > COD_BLOCK_SIZE) {
        size = COD_BLOCK_SIZE;
      }

      if (size > 0) {
        /* This is a used ROM range. */
        memset(used_map, true, size);
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
_fget_line(int Line, char *Buffer, int Size, FILE *pFile)
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
      (Line < (lastline - 1)) ||    /* and the line is past the previous */
      (ftell(pFile) != lastPos)) {  /* and the file hasn't been touched */
    plastFile = pFile;
    lastline  = 1;
    lastPos   = -1;
    rewind(pFile);
  }

  while (Line >= ++lastline) {
    ps = fgets(Buffer, Size, plastFile);
    assert(ps == Buffer);
  }

  ps = fgets(Buffer, Size, plastFile);
  assert(ps == Buffer);

  lastPos = ftell(plastFile);

  return Buffer;
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump directory block.
 */

static void
_dump_directory_block(const uint8_t *Block, unsigned int Block_num)
{
  char         temp_buf[256];
  unsigned int year;
  char         month[8];
  unsigned int day;
  unsigned int time;
  unsigned int bytes_for_address;
#if defined(HAVE_LOCALE_H) && defined(HAVE_LANGINFO_H)
  struct tm    tm;
#endif

  /* Pascal style string. */
  gp_str_from_Pstr(temp_buf, sizeof(temp_buf), &Block[COD_DIR_DATE], COD_DIR_DATE_SIZE, NULL);
  time = gp_getl16(&Block[COD_DIR_TIME]);
  sscanf(temp_buf, "%u%3s%u", &day, month, &year);

#if defined(HAVE_LOCALE_H) && defined(HAVE_LANGINFO_H)
  memset(&tm, 0, sizeof(tm));
  snprintf(temp_buf, sizeof(temp_buf), "%u %s %u %u %u", year + 2000, month, day, time / 100, time % 100);
  strptime(temp_buf, "%Y %b %d %H %M", &tm);
  setlocale(LC_ALL, "");
#endif

  printf("Directory block:                %04x\n"
         "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", Block_num);

  /* Pascal style string. */
  printf("%03x - Source file:              %s\n",
         COD_DIR_SOURCE,
         gp_str_from_Pstr(temp_buf, sizeof(temp_buf), &Block[COD_DIR_SOURCE], COD_DIR_SOURCE_SIZE, NULL));

#if defined(HAVE_LOCALE_H) && defined(HAVE_LANGINFO_H)
  strftime(temp_buf, sizeof(temp_buf), nl_langinfo(D_FMT), &tm);
  printf("%03x - Date:                     %s\n",
         COD_DIR_DATE, temp_buf);
#else
  printf("%03x - Date:                     %u %s %u\n",
         COD_DIR_DATE, day, month, year + 2000);
#endif

#if defined(HAVE_LOCALE_H) && defined(HAVE_LANGINFO_H)
  strftime(temp_buf, sizeof(temp_buf), nl_langinfo(T_FMT), &tm);
  setlocale(LC_ALL, "C");
  printf("%03x - Time:                     %s\n",
         COD_DIR_TIME, temp_buf);
#else
  printf("%03x - Time:                     %02u:%02u\n",
         COD_DIR_TIME, time / 100, time % 100);
#endif

  /* Pascal style string. */
  printf("%03x - Compiler version:         %s\n",
         COD_DIR_VERSION,
         gp_str_from_Pstr(temp_buf, sizeof(temp_buf), &Block[COD_DIR_VERSION], COD_DIR_VERSION_SIZE, NULL));
  /* Pascal style string. */
  printf("%03x - Compiler:                 %s\n",
         COD_DIR_COMPILER,
         gp_str_from_Pstr(temp_buf, sizeof(temp_buf), &Block[COD_DIR_COMPILER], COD_DIR_COMPILER_SIZE, NULL));
  /* Pascal style string. */
  printf("%03x - Notice:                   %s\n",
         COD_DIR_NOTICE,
         gp_str_from_Pstr(temp_buf, sizeof(temp_buf), &Block[COD_DIR_NOTICE], COD_DIR_NOTICE_SIZE, NULL));

  bytes_for_address = Block[COD_DIR_ADDRSIZE];
  printf("%03x - Bytes for address:        %u\n", COD_DIR_ADDRSIZE, bytes_for_address);

  if (bytes_for_address != 0) {
    printf("WARNING: Address size looks suspicious.\n");
  }

  printf("%03x - High word of 64k address: %04x\n",
         COD_DIR_HIGHADDR, gp_getl16(&Block[COD_DIR_HIGHADDR]));
  printf("%03x - Next directory block:     %04x\n",
         COD_DIR_NEXTDIR, gp_getl16(&Block[COD_DIR_NEXTDIR]));
  printf("%03x - COD file version:         %d\n",
         COD_DIR_CODTYPE, gp_getl16(&Block[COD_DIR_CODTYPE]));
  /* Pascal style string. */
  printf("%03x - Processor:                %s\n",
         COD_DIR_PROCESSOR,
         gp_str_from_Pstr(temp_buf, sizeof(temp_buf), &Block[COD_DIR_PROCESSOR], COD_DIR_PROCESSOR_SIZE, NULL));

  printf("%03x,%03x - Short symbol table start block: %04x  end block: %04x\n",
         COD_DIR_SYMTAB, COD_DIR_SYMTAB + 2,
         gp_getl16(&Block[COD_DIR_SYMTAB]),
         gp_getl16(&Block[COD_DIR_SYMTAB + 2]));
  printf("%03x,%03x - File name table start block:    %04x  end block: %04x\n",
         COD_DIR_NAMTAB, COD_DIR_NAMTAB + 2,
         gp_getl16(&Block[COD_DIR_NAMTAB]),
         gp_getl16(&Block[COD_DIR_NAMTAB + 2]));
  printf("%03x,%03x - Source info table start block:  %04x  end block: %04x\n",
         COD_DIR_LSTTAB, COD_DIR_LSTTAB + 2,
         gp_getl16(&Block[COD_DIR_LSTTAB]),
         gp_getl16(&Block[COD_DIR_LSTTAB + 2]));
  printf("%03x,%03x - Rom table start block:          %04x  end block: %04x\n",
         COD_DIR_MEMMAP, COD_DIR_MEMMAP + 2,
         gp_getl16(&Block[COD_DIR_MEMMAP]),
         gp_getl16(&Block[COD_DIR_MEMMAP + 2]));
  printf("%03x,%03x - Local scope table start block:  %04x  end block: %04x\n",
         COD_DIR_LOCALVAR, COD_DIR_LOCALVAR + 2,
         gp_getl16(&Block[COD_DIR_LOCALVAR]),
         gp_getl16(&Block[COD_DIR_LOCALVAR + 2]));
  printf("%03x,%03x - Long symbol table start block:  %04x  end block: %04x\n",
         COD_DIR_LSYMTAB, COD_DIR_LSYMTAB + 2,
         gp_getl16(&Block[COD_DIR_LSYMTAB]),
         gp_getl16(&Block[COD_DIR_LSYMTAB + 2]));
  printf("%03x,%03x - Debug messages start block:     %04x  end block: %04x\n",
         COD_DIR_MESSTAB, COD_DIR_MESSTAB + 2,
         gp_getl16(&Block[COD_DIR_MESSTAB]),
         gp_getl16(&Block[COD_DIR_MESSTAB + 2]));
  putchar('\n');
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Dump directory block code index.
 */

static void
_dump_index(proc_class_t Class, const uint8_t *Block)
{
  unsigned int _64k_base;
  unsigned int i;
  unsigned int curr_block;
  int          addr_digits;

  printf("Code blocks index:\n"
         "Block range    Block number\n"
         "---------------------------\n");

  _64k_base   = IMemAddrFromBase((unsigned int)gp_getu16(&Block[COD_DIR_HIGHADDR]));
  addr_digits = Class->addr_digits;

  for (i = 0; i < COD_CODE_IMAGE_BLOCKS; ++i) {
    curr_block = gp_getu16(&Block[i * 2]);

    if (curr_block != 0) {
      printf("%0*x-%0*x:    %u\n",
             addr_digits,  _64k_base |  MemOffsFromBlock(i),
             addr_digits, (_64k_base | (MemOffsFromBlock(i + 1) - 1)),
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
dump_directory_blocks(const DirBlockInfo *Main_dir, proc_class_t Class)
{
  const DirBlockInfo *dbi;
  unsigned int        block_num;

  block_num = 0;
  dbi       = Main_dir;

  do {
    _dump_directory_block(dbi->dir, block_num);
    _dump_index(Class, dbi->dir);
    block_num = gp_getl16(&dbi->dir[COD_DIR_NEXTDIR]);
    dbi = dbi->next;
  } while (dbi != NULL);
}

/*------------------------------------------------------------------------------------------------*/

/*
 * ROM usage information.
 */

void
dump_memmap(FILE *Code_file, const DirBlockInfo *Main_dir, proc_class_t Class, gp_boolean Make_list)
{
  const DirBlockInfo *dbi;
  unsigned int        _64k_base;
  int                 addr_digits;
  unsigned int        start_map_block;
  unsigned int        end_map_block;
  unsigned int        i;
  unsigned int        j;
  unsigned int        offset;
  const uint8_t      *record;
  unsigned int        first_offset;
  unsigned int        last_offset;
  unsigned int        start_addr;
  unsigned int        end_addr;
  gp_boolean          first;

  first       = true;
  addr_digits = Class->addr_digits;
  dbi         = Main_dir;

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
        read_block(Code_file, cod_block, j);

        for (i = 0, offset = 0; i < COD_CODE_IMAGE_BLOCKS; offset += COD_MAPENTRY_SIZE, ++i) {
          record = &cod_block[offset];
          first_offset = gp_getl16(&record[COD_MAPTAB_START]);
          last_offset  = gp_getl16(&record[COD_MAPTAB_LAST]);

          if ((first_offset != 0) || (last_offset != 0)) {
            start_addr = _64k_base + first_offset;
            end_addr   = _64k_base + last_offset + 1;
            printf("using ROM %0*x to %0*x\n",
                   addr_digits, gp_processor_insn_from_byte_c(Class, start_addr),
                   addr_digits, gp_processor_insn_from_byte_c(Class, end_addr) - 1);

            if (Make_list) {
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
dump_code(FILE *Code_file, const DirBlockInfo *Main_dir, pic_processor_t Processor, gp_boolean Wide_dump)
{
  proc_class_t        class;
  char                border_gap0[16];
  char                border_gap1[16];
  char                buffer[BUFSIZ];
  const DirBlockInfo *dbi;
  MemBlock_t         *data;
  int                 addr_digits;
  unsigned int        _64k_base;
  unsigned int        block_index;
  unsigned int        byte_address;
  int                 num_words;
  unsigned int        bsr_boundary;
  unsigned int        i;
  unsigned int        j;
  unsigned int        k;
  unsigned int        column_num;
  uint16_t            word;
  gp_boolean          used_prev;
  gp_boolean          used_act;
  gp_boolean          empty_signal;
  gp_boolean          empty_line;

  class = Processor->class;
  dump_memmap(Code_file, Main_dir, class, true);

  bsr_boundary = gp_processor_bsr_boundary(Processor);
  addr_digits  = class->addr_digits;
  column_num   = (Wide_dump) ? CODE_COLUMN_NUM_WIDE : CODE_COLUMN_NUM;
  dbi          = Main_dir;

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
        read_block(Code_file, cod_block, block_index);

        byte_address = _64k_base + (k * COD_BLOCK_N_WORDS) * WORD_SIZE;
        if (Wide_dump) {
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
              printf("%0*x: ", addr_digits, gp_processor_insn_from_byte_c(class, byte_address));

              for (j = 0; j < column_num; j++) {
                if (used_map[(i + j) * WORD_SIZE]) {
                  /* This is a used ROM word. */
                  printf(" %04x", gp_getu16(&cod_block[(i + j) * WORD_SIZE]));
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
              if (gp_getu16(&cod_block[(i + j) * WORD_SIZE]) != 0) {
                empty_line = false;
                break;
              }
            }

            if (! empty_line) {
              byte_address = _64k_base + (k * COD_BLOCK_N_WORDS + i) * WORD_SIZE;
              printf("%0*x: ", addr_digits, gp_processor_insn_from_byte_c(class, byte_address));

              for (j = 0; j < column_num; j++) {
                printf(" %04x", gp_getu16(&cod_block[(i + j) * WORD_SIZE]));
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
          gp_mem_b_put(data, byte_address + j, cod_block[j], NULL, NULL);
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
            printf("%0*x:  %04x  %s\n", addr_digits, gp_processor_insn_from_byte_c(class, byte_address), word, buffer);

            if (num_words != 1) {
              if (class->i_memory_get(data, byte_address + WORD_SIZE, &word, NULL, NULL) != W_USED_ALL) {
                /* Internal memory handling error. */
                assert(0);
              }

              printf("%0*x:  %04x\n", addr_digits, gp_processor_insn_from_byte_c(class, byte_address + WORD_SIZE), word);
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
dump_symbols(FILE *Code_file, const DirBlockInfo *Main_dir)
{
  unsigned int   start_block;
  unsigned int   end_block;
  unsigned int   i;
  unsigned int   j;
  unsigned int   offset;
  const uint8_t *record;
  unsigned int   length;
  char           name[COD_SSYMBOL_NAME_LENGTH + 1];
  unsigned int   type;
  unsigned int   value;

  start_block = gp_getu16(&Main_dir->dir[COD_DIR_SYMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&Main_dir->dir[COD_DIR_SYMTAB + 2]);

    printf("Symbol Table Information:\n"
           "-------------------------\n");

    for (j = start_block; j <= end_block; j++) {
      read_block(Code_file, cod_block, j);

      for (i = 0, offset = 0; i < SYMBOLS_PER_BLOCK; offset += SSYMBOL_SIZE, ++i) {
        record = &cod_block[offset];
        length = record[COD_SSYMBOL_NAME];

        if (length != 0) {
          gp_str_from_Pstr(name, sizeof(name), &record[COD_SSYMBOL_NAME], COD_SSYMBOL_NAME_SIZE, NULL);
          type  = record[COD_SSYMBOL_STYPE];
          value = gp_getu16(&record[COD_SSYMBOL_SVALUE]);
          printf("%-12s = %04x (%6d), type = %s\n", name, value, value, _symbol_type_to_str(type));
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
_lsymbols_max_length(FILE *Code_file, const DirBlockInfo *Main_dir)
{
  unsigned int start_block;
  unsigned int end_block;
  unsigned int i;
  unsigned int j;
  unsigned int length;
  unsigned int max_length;

  start_block = gp_getu16(&Main_dir->dir[COD_DIR_LSYMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&Main_dir->dir[COD_DIR_LSYMTAB + 2]);

    max_length = 0;
    for (j = start_block; j <= end_block; j++) {
      read_block(Code_file, cod_block, j);

      for (i = 0; i < COD_BLOCK_SIZE; ) {
        /* Pascal style string. */
        length = cod_block[i];

        if (length == 0) {
          break;
        }

        if (max_length < length) {
          max_length = length;
        }

        i += length + COD_LSYMBOL_EXTRA;
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
dump_lsymbols(FILE *Code_file, const DirBlockInfo *Main_dir)
{
  unsigned int   start_block;
  unsigned int   end_block;
  unsigned int   i;
  unsigned int   j;
  const uint8_t *sym;
  unsigned int   length;
  char           name[COD_LSYMBOL_NAME_MAX_LENGTH + 1];
  unsigned int   type;
  unsigned int   value;
  int            symbol_align;

  symbol_align = _lsymbols_max_length(Code_file, Main_dir);
  start_block  = gp_getu16(&Main_dir->dir[COD_DIR_LSYMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&Main_dir->dir[COD_DIR_LSYMTAB + 2]);

    printf("Long Symbol Table Information:\n"
           "------------------------------------------------------------------------\n");

    for (j = start_block; j <= end_block; j++) {
      read_block(Code_file, cod_block, j);

      for (i = 0; i < COD_BLOCK_SIZE; ) {
        /* Pascal style string. */
        sym    = &cod_block[i + COD_LSYMBOL_NAME];
        length = *sym;

        if (length == 0) {
          break;
        }

        gp_str_from_Pstr(name, sizeof(name), sym, COD_LSYMBOL_NAME_MAX_SIZE, NULL);
        type  = gp_getu16(&sym[length + COD_LSYMBOL_TYPE]);
        /* read big endian */
        value = gp_getb32(&sym[length + COD_LSYMBOL_VALUE]);
        printf("%-*s = %08x (%11d), type = %s\n", symbol_align, name, value, value, _symbol_type_to_str(type));
        i += length + COD_LSYMBOL_EXTRA;
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
dump_source_files(FILE *Code_file, const DirBlockInfo *Main_dir)
{
  unsigned int  start_block;
  unsigned int  end_block;
  unsigned int  i;
  unsigned int  offset;
  unsigned int  length;
  char          name_str[COD_FILE_NAME_LENGTH + 1];
  char         *name;

  start_block = gp_getu16(&Main_dir->dir[COD_DIR_NAMTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&Main_dir->dir[COD_DIR_NAMTAB + 2]);

    printf("Source File Information:\n"
           "------------------------\n");

    for (i = start_block; i <= end_block; ++i) {
      read_block(Code_file, cod_block, i);

      for (offset = 0; offset < COD_BLOCK_SIZE; ) {
        length = cod_block[offset];

        if (length > 0) {
          gp_str_from_Pstr(name_str, sizeof(name_str), &cod_block[offset], COD_FILE_NAME_SIZE, NULL);
          name = GP_Strdup(name_str);
          source_file_names[number_of_source_files] = name;
          printf("%s\n", name);
          source_files[number_of_source_files] = fopen(name, "rt");
          number_of_source_files++;

          if (number_of_source_files >= MAX_SOURCE_FILES) {
            fprintf(stderr, "Too many source files, increase MAX_SOURCE_FILES and recompile the program.\n");
            exit(1);
          }

          /* In this way will find the shorter (64 bytes length) names also. */
	  length += COD_SHORT_FILE_NAME_SIZE - 1;
	  length /= COD_SHORT_FILE_NAME_SIZE;
	  length *= COD_SHORT_FILE_NAME_SIZE;
          offset += length;
        }
        else {
          offset += COD_SHORT_FILE_NAME_SIZE;
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

static const char *
_mod_flags_to_str(int Mode)
{
  static char fl[9];

  fl[0] = (Mode & COD_LS_SMOD_FLAG_C1) ? 'C' : '.';
  fl[1] = (Mode & COD_LS_SMOD_FLAG_F)  ? 'F' : '.';
  fl[2] = (Mode & COD_LS_SMOD_FLAG_I)  ? 'I' : '.';
  fl[3] = (Mode & COD_LS_SMOD_FLAG_D)  ? 'D' : '.';
  fl[4] = (Mode & COD_LS_SMOD_FLAG_C0) ? 'C' : '.';
  fl[5] = (Mode & COD_LS_SMOD_FLAG_L)  ? 'L' : '.';
  fl[6] = (Mode & COD_LS_SMOD_FLAG_N)  ? 'N' : '.';
  fl[7] = (Mode & COD_LS_SMOD_FLAG_A)  ? 'A' : '.';
  fl[8] = '\0';

  return fl;
}

/*------------------------------------------------------------------------------------------------*/

/*
 * Line number info from the source files.
 */

void
dump_line_symbols(FILE *Code_file, const DirBlockInfo *Main_dir)
{
  static unsigned int  lst_line_number = 1;
  static unsigned int  last_src_line   = 0;

  char                 line[2048];
  char                 nbuf[128];
  const char          *source_file_name;
  const DirBlockInfo  *dbi;
  gp_boolean           has_line_num_info;
  unsigned int         _64k_base;
  unsigned int         start_block;
  unsigned int         end_block;
  unsigned int         i;
  unsigned int         j;
  unsigned int         offset;
  const uint8_t       *record;
  unsigned int         src_file_num;
  unsigned int         src_mod_flag;
  unsigned int         src_line_num;
  unsigned int         insn_offset;
  unsigned int         _64k_seg_address;

  has_line_num_info = false;
  dbi               = Main_dir;
  while (dbi != NULL) {
    _64k_base        = gp_getu16(&dbi->dir[COD_DIR_HIGHADDR]);
    _64k_seg_address = IMemAddrFromBase(_64k_base);

    start_block = gp_getu16(&dbi->dir[COD_DIR_LSTTAB]);

    if (start_block != 0) {
      end_block = gp_getu16(&dbi->dir[COD_DIR_LSTTAB + 2]);

      if (!has_line_num_info) {
        has_line_num_info = true;

        printf("Line Number Information:\n"
               " LstLn  SrcLn  Addr    Flags        FileName\n"
               " -----  -----  ------  -----------  ---------------------------------------------------------------\n");
      }

      for (j = start_block; j <= end_block; j++) {
        read_block(Code_file, cod_block, j);

        for (i = 0, offset = 0; i < COD_MAX_LINE_SYM; offset += COD_LINE_SYM_SIZE, ++i) {
          record       = &cod_block[offset];
          src_file_num = record[COD_LS_SFILE];
          src_mod_flag = record[COD_LS_SMOD];
          src_line_num = gp_getu16(&record[COD_LS_SLINE]);
          insn_offset  = gp_getu16(&record[COD_LS_SLOC]);

          if (((src_file_num != 0) || (src_mod_flag != 0) || (src_line_num != 0) || (insn_offset != 0)) &&
              (FlagIsClr(src_mod_flag, COD_LS_SMOD_FLAG_L))) {
            if (src_file_num < number_of_source_files) {
              source_file_name = source_file_names[src_file_num];
            }
            else {
              snprintf(nbuf, sizeof(nbuf), "Bad source file index: %u", src_file_num);
              source_file_name = nbuf;
            }

            if ((source_file_name != NULL) && (source_file_name[0] != '\0')) {
              printf(" %5u  %5u  %06x  %2x %s  %s\n",
                     lst_line_number, src_line_num, _64k_seg_address | insn_offset,
                     src_mod_flag, _mod_flags_to_str(src_mod_flag), source_file_name);
            }
            else {
              printf(" %5u  %5u  %06x  %2x %s\n",
                     lst_line_number, src_line_num, _64k_seg_address | insn_offset,
                     src_mod_flag, _mod_flags_to_str(src_mod_flag));
            }

            lst_line_number++;

            if ((src_file_num < number_of_source_files) && (src_line_num != last_src_line)) {
              if (source_files[src_file_num] != NULL) {
                _fget_line(src_line_num, line, sizeof(line), source_files[src_file_num]);
                printf("%s", line);
              }
              else {
                printf("ERROR: Source file \"%s\" does not exist.\n", source_file_names[src_file_num]);
              }
            }
          }

          last_src_line = src_line_num;
        } /* for (i = 0, offset = 0; i < COD_MAX_LINE_SYM; ...  */
      } /* for (j = start_block; j <= end_block; j++) */
    } /* if (start_block != 0) */

    dbi = dbi->next;
  } /* while (dbi != NULL) */

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
dump_debug_message_area(FILE *Code_file, const DirBlockInfo *Main_dir)
{
  unsigned int   start_block;
  unsigned int   end_block;
  unsigned int   i;
  unsigned int   j;
  const uint8_t *record;
  unsigned int   address;
  char           type;
  unsigned int   length;
  char           message[COD_DEBUG_MSG_MAX_LENGTH + 1];

  start_block = gp_getu16(&Main_dir->dir[COD_DIR_MESSTAB]);

  if (start_block != 0) {
    end_block = gp_getu16(&Main_dir->dir[COD_DIR_MESSTAB + 2]);

    printf("Debug Message area:\n"
           "     Addr  Cmd  Message\n"
           " --------  ---  -------------------------------------\n");

    for (i = start_block; i <= end_block; i++) {
      read_block(Code_file, cod_block, i);

      for (j = 0; j < (COD_BLOCK_SIZE - COD_DEBUG_MIN_SIZE); ) {
        /* read big endian */
        record  = &cod_block[j];
        address = gp_getb32(&record[COD_DEBUG_ADDR]);
        type    = record[COD_DEBUG_CMD];

        if (type == '\0') {
          break;
        }

        /* Pascal style string. */
        length = record[COD_DEBUG_MSG];
        gp_str_from_Pstr(message, sizeof(message), &record[COD_DEBUG_MSG], COD_DEBUG_MSG_MAX_SIZE, NULL);
        printf(" %8x    %c  %s\n", address, type, message);
        j += length + COD_DEBUG_EXTRA;
      } /* for (j = 0; ... */
    } /* for (i = start_block; ... */
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
dump_local_vars(FILE *Code_file, const DirBlockInfo *Main_dir, proc_class_t Class)
{
  unsigned int   start_block;
  unsigned int   end_block;
  unsigned int   i;
  unsigned int   j;
  unsigned int   offset;
  const uint8_t *record;
  unsigned int   length;
  unsigned int   start;
  unsigned int   stop;
  char           name[COD_SSYMBOL_NAME_LENGTH + 1];
  unsigned int   type;
  unsigned int   value;

  start_block = gp_getu16(&Main_dir->dir[COD_DIR_LOCALVAR]);

  if (start_block != 0) {
    end_block = gp_getu16(&Main_dir->dir[COD_DIR_LOCALVAR + 2]);

    printf("Local Symbol Scoping Information:\n"
           "---------------------------------\n");

    for (i = start_block; i <= end_block; i++) {
      read_block(Code_file, cod_block, i);

      for (j = 0, offset = 0; j < SYMBOLS_PER_BLOCK; offset += SSYMBOL_SIZE, ++j) {
        record = &cod_block[offset];
        length = record[COD_SSYMBOL_NAME];

        if (length != 0) {
          if (memcmp(&record[COD_SSYMBOL_NAME + 1], "__LOCAL", length) == 0) {
            start = gp_getl32(&record[COD_SSYMBOL_START]);
            stop  = gp_getl32(&record[COD_SSYMBOL_STOP]);

            printf("Local symbols between %06x and %06x:  ",
                   gp_processor_insn_from_byte_c(Class, start),
                   gp_processor_insn_from_byte_c(Class, stop + 1) - 1);
          }
          else {
            gp_str_from_Pstr(name, sizeof(name), &record[COD_SSYMBOL_NAME], COD_SSYMBOL_NAME_SIZE, NULL);
            type  = record[COD_SSYMBOL_STYPE];
            value = gp_getl16(&record[COD_SSYMBOL_SVALUE]);
            printf("%-12s = %04x (%6d), type = %s\n", name, value, value, _symbol_type_to_str(type));
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

/*------------------------------------------------------------------------------------------------*/

void
dump_free(void)
{
  unsigned int i;

  if (number_of_source_files > 0) {
    for (i = 0; i < number_of_source_files; ++i) {
      if (source_file_names[i] != NULL) {
        free(source_file_names[i]);
        source_file_names[i] = NULL;
      }

      if (source_files[i] != NULL) {
        fclose(source_files[i]);
        source_files[i] = NULL;
      }
    }

    number_of_source_files = 0;
  }
}

/* gpcoff.h - header file for pic object files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

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

#ifndef __GPCOFF_H__
#define __GPCOFF_H__


/* These definitions are for the COFF as stored in a file. */

/* define the typical values, if they aren't found warn the user */
#define MICROCHIP_MAGIC_v1 0x1234
#define MICROCHIP_MAGIC_v2 0x1240

#define OPTMAGIC_v1 0x5678
#define OPTMAGIC_v2 0x5678

/* coff file header format */
struct filehdr
{
  unsigned short  f_magic;   /* magic number */
  unsigned short  f_nscns;   /* number of sections */
  unsigned long   f_timdat;  /* time and date stamp */
  unsigned long   f_symptr;  /* file ptr to symtab */
  unsigned long   f_nsyms;   /* # symtab entries */
  unsigned short  f_opthdr;  /* sizeof(opt hdr) */
  unsigned short  f_flags;   /* flags */
};

/* define the size in the file, don't use sizeof() !! */
#define FILE_HDR_SIZ_v1 20
#define FILE_HDR_SIZ_v2 20

/* relocation info has been stripped */
#define F_RELFLG      0x0001
/* file is executable - has no unresolved external symbols */
#define F_EXEC        0x0002
/* line numbers have been stripped */
#define F_LINENO      0x0004
/* the MPASM assembler object file is from absolute (as opposed to relocatable) assembly code */
#define F_ABSOLUTE    0x0010
/* local symbols have been stripped */
#define L_SYMS        0x0080
/* the COFF file produced utilizing the Extended mode */
#define F_EXTENDED18  0x4000
/* processor independent file for a core */
#define F_GENERIC     0x8000

/* optional header format */
struct opthdr
{
  unsigned short  opt_magic;
  unsigned long   vstamp;         /* version of the compiler assembler */
  unsigned long   proc_type;
  unsigned long   rom_width_bits;
  unsigned long   ram_width_bits;
};

#define OPT_HDR_SIZ_v1 16
#define OPT_HDR_SIZ_v2 18

/* section header format */
struct scnhdr
{
  union {
    char          name[8];   /* section name if less then 8 characters */
    struct {
      unsigned long s_zeros;   /* first four characters are 0 */
      unsigned long s_offset;  /* pointer to the string table */
    } ptr;
  } s_name;
  unsigned long   s_paddr;   /* physical address */
  unsigned long   s_vaddr;   /* virtual address */
  unsigned long   s_size;    /* section size */
  unsigned long   s_scnptr;  /* file ptr to raw data */
  unsigned long   s_relptr;  /* file ptr to relocation */
  unsigned long   s_lnnoptr; /* file ptr to line numbers */
  unsigned short  s_nreloc;  /* # reloc entries */
  unsigned short  s_nlnno;   /* # line number entries */
  unsigned long   s_flags;   /* flags */
};

#define SEC_HDR_SIZ_v1 40
#define SEC_HDR_SIZ_v2 40

/* Section contains executable code. */
#define STYP_TEXT     0x00000020
/* Section contains initialized data. */
#define STYP_DATA     0x00000040
/* Section contains uninitialized data. */
#define STYP_BSS      0x00000080
/* Section contains initialized data for ROM. */
#define STYP_DATA_ROM 0x00000100
/* Section is absolute. */
#define STYP_ABS      0x00001000
/* Section is shared across banks. */
#define STYP_SHARED   0x00002000
/* Section is overlaid with other sections of the same name from different objects modules. */
#define STYP_OVERLAY  0x00004000
/* Section is available using access bit. */
#define STYP_ACCESS   0x00008000
/* Section contains the activation record for a function. */
#define STYP_ACTREC   0x00010000
/* Section has been relocated.  This is a temporary flag used by the linker. */
#define STYP_RELOC    0x00020000
/* Section is byte packed on 16bit devices. */
#define STYP_BPACK    0x00040000

/* relocation entry */
struct reloc
{
  unsigned long   r_vaddr;     /* entry relative virtual address */
  unsigned long   r_symndx;    /* index into symbol table */
  short           r_offset;    /* offset to be added to address of symbol 'r_symndx' */
  unsigned short  r_type;      /* relocation type */
};

#define RELOC_SIZ     12

/* relocation for the CALL instruction (first word only on 18cxx) */
#define RELOCT_CALL             1
/* relocation for the GOTO instruction (first word only on 18cxx) */
#define RELOCT_GOTO             2
/* relocation for the second 8 bits of an address */
#define RELOCT_HIGH             3
/* relocation for the low order 8 bits of an address */
#define RELOCT_LOW              4
/* relocation for the 5 bits of address for the P operand of a 17cxx MOVFP
   or MOVPF instruction */
#define RELOCT_P                5
/* relocation to generate the appropriate instruction to bank switch for a symbol */
#define RELOCT_BANKSEL          6
/* relocation to generate the appropriate instruction to page switch for a symbol */
#define RELOCT_PAGESEL          7
/* FIXME */
#define RELOCT_ALL              8
/* FIXME */
#define RELOCT_IBANKSEL         9
/* relocation for the 8 bits of address for the F operand of a 17cxx MOVFP or
   MOVPF instruction */
#define RELOCT_F                10
/* FIXME */
#define RELOCT_TRIS             11
/* relocation for the MOVLR bank 17cxx banking instruction */
#define RELOCT_MOVLR            12
/* relocation for the MOVLB 17cxx and 18cxx banking instruction */
#define RELOCT_MOVLB            13
/* relocation for the second word of an 18cxx goto instruction */
#define RELOCT_GOTO2            14
/* relocation for the second word of an 18cxx call instruction */
#define RELOCT_CALL2            RELOCT_GOTO2
/* relocation for the source register of the 18cxx MOVFF instruction */
#define RELOCT_FF1              15
/* relocation for the destination register of the 18cxx MOVFF instruction */
#define RELOCT_FF2              16
/* relocation for the first word of the 18cxx LFSR instruction */
#define RELOCT_LFSR1            17
/* relocation for the second word of the 18cxx LFSR instruction */
#define RELOCT_LFSR2            18
/* relocation for the 18cxx BRA instruction */
#define RELOCT_BRA              19
/* relocation for the 18cxx RCALL instruction */
#define RELOCT_RCALL            RELOCT_BRA
/* relocation for the 18cxx relative conditional branch instructions */
#define RELOCT_CONDBRA          20
/* relocation for the highest order 8 bits of a 24-bit address */
#define RELOCT_UPPER            21
/* relocation for the 18cxx access bit */
#define RELOCT_ACCESS           22
/* relocation for selecting the correct page using WREG as scratch */
#define RELOCT_PAGESEL_WREG     23
/* relocation for selecting the correct page using bit set/clear instructions */
#define RELOCT_PAGESEL_BITS     24
/* relocation for the size of a section */
#define RELOCT_SCNSZ_LOW        25
#define RELOCT_SCNSZ_HIGH       26
#define RELOCT_SCNSZ_UPPER      27
/* relocation for the address of the end of a section */
#define RELOCT_SCNEND_LOW       28
#define RELOCT_SCNEND_HIGH      29
#define RELOCT_SCNEND_UPPER     30
/* relocation for the address of the end of a section on LFSR */
#define RELOCT_SCNEND_LFSR1     31
#define RELOCT_SCNEND_LFSR2     32
#define RELOCT_TRIS_3BIT        33
/* relocation for selecting the correct page using pic14 enhanced MOVLP instruction */
#define RELOCT_PAGESEL_MOVLP    34

/* linenumber entry */
struct lineno
{
  unsigned long  l_srcndx;     /* symbol table index of associated source file */
  unsigned short l_lnno;       /* line number */
  unsigned long  l_paddr;      /* address of code for this lineno */
  unsigned short l_flags;      /* bit flags for the line number */
  unsigned long  l_fcnndx;     /* symbol table index of associated function, if
                                  there is one */
};

#define LINENO_SIZ      16

/* Set if l_fcnndx is valid */
#define LINENO_HASFCN   0x01

/* symbol table entry */
struct syment
{
  union {
    char            name[8];   /* symbol name if less than 8 characters */
    struct {
      unsigned long s_zeros;   /* first four characters are 0 */
      unsigned long s_offset;  /* pointer to the string table */
    } ptr;
  } sym_name;
  unsigned long   value;       /* symbol value */
  short           sec_num;     /* section number */
  unsigned long   type;        /* type */
  char            st_class;    /* storage class */
  unsigned char   num_auxsym;  /* number of auxiliary symbols */
};

#define SYMBOL_SIZE_v1 18
#define SYMBOL_SIZE_v2 20

/* Symbol section numbers. */
#define N_DEBUG    -2
#define N_ABS      -1
#define N_UNDEF     0
#define N_SCNUM     1

/* Symbol types

  Type in an unsigned short (16 bits).  The lowest four or five bits contains
  the basic type.  The next higher three bits contain the derived symbol type.
  The rest of the bits are unused. */

/* Basic symbol types  */
#define T_NULL      0   /* null */
#define T_VOID      1   /* void */
#define T_CHAR      2   /* character */
#define T_SHORT     3   /* short integer */
#define T_INT       4   /* integer */
#define T_LONG      5   /* long integer */
#define T_FLOAT     6   /* floating point */
#define T_DOUBLE    7   /* double length floating point */
#define T_STRUCT    8   /* structure */
#define T_UNION     9   /* union */
#define T_ENUM      10  /* enumeration */
#define T_MOE       11  /* member of enumeration */
#define T_UCHAR     12  /* unsigned character */
#define T_USHORT    13  /* unsigned short */
#define T_UINT      14  /* unsigned integer */
#define T_ULONG     15  /* unsigned long */
/* Basic symbol types for new format only */
#define T_LNGDBL    16  /* long double floating point */
#define T_SLONG     17  /* short long */
#define T_USLONG    18  /* unsigned short long */

/* Derived types */
#define DT_NON      0  /* no derived type */
#define DT_PTR      1  /* pointer */
#define DT_FCN      2  /* function */
#define DT_ARY      3  /* array */
#define DT_ROMPTR   4
#define DT_FARROMPTR 5

/* Storage classes */
#define C_EFCN      0xff /* physical end of function */
#define C_NULL      0    /* null */
#define C_AUTO      1    /* automatic variable */
#define C_EXT       2    /* external symbol */
#define C_STAT      3    /* static */
#define C_REG       4    /* register variable */
#define C_EXTDEF    5    /* external definition */
#define C_LABEL     6    /* label */
#define C_ULABEL    7    /* undefined label */
#define C_MOS       8    /* member of structure */
#define C_ARG       9    /* function argument */
#define C_STRTAG    10   /* structure tag */
#define C_MOU       11   /* member of union */
#define C_UNTAG     12   /* union tag */
#define C_TPDEF     13   /* type definition */
#define C_USTATIC   14   /* undefined static */
#define C_ENTAG     15   /* enumeration tag */
#define C_MOE       16   /* member of enumeration */
#define C_REGPARM   17   /* register parameter */
#define C_FIELD     18   /* bit field */
#define C_AUTOARG   19   /* auto argument */
#define C_LASTENT   20   /* dummy entry (end of block) */
#define C_BLOCK     100  /* ".bb" or ".eb" */
#define C_FCN       101  /* ".bf" or ".ef" */
#define C_EOS       102  /* end of structure */
#define C_FILE      103  /* file name */
#define C_LINE      104  /* line number reformatted as symbol table entry */
#define C_ALIAS     105  /* duplicate tag */
#define C_HIDDEN    106  /* ext symbol in dmert public lib */
#define C_EOF       107  /* end of file */
#define C_LIST      108  /* absoulte listing on or off */
#define C_SECTION   109  /* section */

/* Auxiliary symbol table entry for a file */
struct aux_file
{
  unsigned long  x_offset;  /* String table offset for filename */
  unsigned long  x_incline; /* Line number at which this file was included,
                              0->not included */
  unsigned char  x_flags;
  char _unused[11];
};

/* Auxiliary symbol table entry for a section */
struct aux_scn
{
  unsigned long  x_scnlen;  /* Section Length */
  unsigned short x_nreloc;  /* Number of relocation entries */
  unsigned short x_nlinno;  /* Number of line numbers */
  char _unused[12];
};

/* Auxiliary symbol table entry for the tagname of a struct/union/enum */
struct aux_tag
{
  char _unused[6];
  unsigned short x_size;     /* Size of struct/union/enum */
  char _unused2[4];
  unsigned long x_endndx;    /* Symbol index of next entry beyond this
                                struct/union/enum */
  char _unused3[4];
};

/* Auxiliary symbol table entry for an end of struct/union/enum */
struct aux_eos
{
  unsigned long x_tagndx;    /* Symbol index of struct/union/enum tag */
  char _unused[2];
  unsigned short x_size;     /* Size of struct/union/enum */
  char _unused2[12];
};

/* Auxiliary symbol table entry for a function name */
struct aux_fcn
{
  unsigned long x_tagndx;   /* Unused??  Tag Index */
  unsigned long x_size;     /* Unused??  Size of function in bits */
  unsigned long x_lnnoptr;  /* File pointer to line numbers for this function */
  unsigned long x_endndx;   /* Symbol Index of next entry beyond this
                               function */
  short x_actscnum; /* size of static activation record to allocate */
  char _unused[2];
};

/* Auxiliary symbol table entry for an array */
struct aux_arr
{
  unsigned long  x_tagndx;   /* Unused??  Tag Index */
  unsigned short x_lnno;     /* Unused??  Line number declaration */
  unsigned short x_size;     /* Size of array */
  unsigned short x_dimen[4]; /* Size of first four dimensions */
  char _unused[4];
};

/* Auxiliary symbol table entry for the end of a block or function */
struct aux_eobf
{
  char _unused[4];
  unsigned short x_lnno;   /* C Source line number of the end, relative to
                              start of block/func */
  char _unused2[14];
};

/* Auxiliary symbol table entry for the beginning of a block or function */
struct aux_bobf
{
  char _unused[4];
  unsigned short x_lnno;   /* C Source line number of the beginning, relative
                              to start enclosing scope */
  char _unused2[6];
  unsigned long x_endndx;  /* Symbol Index of next entry past this block/func */
  char _unused3[4];
};

/* Auxiliary symbol table entry for a variable of type struct/union/enum */
struct aux_var
{
  unsigned long x_tagndx;  /* Symbol Index of struct/union/enum tagname */
  char _unused[2];
  unsigned short x_size;   /* Size of the struct/union/enum */
  char _unused2[12];
};

struct aux_field {
  char _unused[6];
  unsigned short x_size;
  char _unused2[12];
};

struct aux_fcn_calls {
  unsigned long x_calleendx;    /* Symbol table entry of callee - 1 */
  unsigned long x_is_interrupt; /* 0 not, 1 low, 2 high */
  char _unused[12];
};



/* Auxiliary entries */
#define X_DIMNUM      4
#define AUX_NONE      0
#define AUX_FILE      1   /* detail information for a source file */
#define AUX_SCN       2   /* detail information for a section */
#define AUX_TAG       3   /* detail informationfor a struct/union/enum tag */
#define AUX_EOS       4   /* end of struct/union/enum */
#define AUX_FCN       5   /* detail information for a function */
#define AUX_ARR       6   /* FIXME */
#define AUX_EOBF      7   /* end of block or function */
#define AUX_BOBF      8   /* beginning of block or function */
#define AUX_VAR       9   /* variable */
#define AUX_DIRECT    10  /* direct message */
#define AUX_IDENT     11  /* ident */
#define AUX_FCN_CALLS 12  /* function called by this function */

/* These definitions are for the COFF as stored in memory. */

/* relocation linked list */

typedef struct gp_reloc_type
{
  /* entry relative address in bytes */
  long address;

  /* symbol */
  struct gp_symbol_type *symbol;

  /* symbol number, only valid when generating a coff file */
  long symbol_number;

  /* offset added to address of symbol */
  short offset;

  /* relocation type */
  unsigned short type;

  struct gp_reloc_type *next;
} gp_reloc_type;

/* line number linked list */

typedef struct gp_linenum_type
{
  /* source file symbol */
  struct gp_symbol_type *symbol;

  /* line number */
  unsigned short line_number;

  /* byte address for this line number */
  unsigned int address;

  struct gp_linenum_type *next;
} gp_linenum_type;

/* auxiliary symbol linked list */

typedef struct gp_aux_type
{
  /* auxiliary symbol type */
  long type;

  /* FIXME: Finish the aux entries. */
  union {
    struct {
      unsigned char command;
      const char *string;
    } _aux_direct;
    struct {
      const char *filename;
      unsigned long line_number;
      unsigned char flags;
    } _aux_file;
    struct {
      const char *string;
    } _aux_ident;
    struct {
      unsigned long  length;
      unsigned short nreloc;
      unsigned short nlineno;
    } _aux_scn;
    struct {
      struct gp_symbol_type *callee; /* NULL for call through pointer */
      unsigned long is_interrupt;    /* 0 not, 1 low, 2 high */
    } _aux_fcn_calls;
    char data[SYMBOL_SIZE_v2];
  } _aux_symbol;

  struct gp_aux_type *next;
} gp_aux_type;

/* symbol linked list */

typedef struct gp_symbol_type
{
  /* symbol name */
  char *name;

  /* symbol value */
  gp_symvalue_t value;

  /* section name of symbol */
  const char *section_name;

  /* section number, only for used for determining symbol type:
     N_DEBUG = -2, N_ABS = -1, N_UNDEF = 0, or N_SCNUM = 1 if defined */
  short section_number;

  /* defining section */
  struct gp_section_type *section;

  /* type */
  unsigned char type;

  /* derived type */
  unsigned long derived_type;

  /* storage class */
  signed char class;

   /* number of auxiliary symbols */
  char num_auxsym;

  /* auxiliary symbols */
  struct gp_aux_type *aux_list;

  /* symbol number, only valid when writing coff or cod file */
  unsigned long number;

  struct gp_symbol_type *next;
} gp_symbol_type;

/* hash table for find symbol name */

typedef struct gp_hash
{
  unsigned int hash;
  const gp_symbol_type *symbol;
} gp_hash_type;

/* section linked list */

typedef struct gp_section_type
{
  /* section name */
  char *name;

  /* section symbol */
  struct gp_symbol_type *symbol;

  /* flags */
  unsigned long flags;

  /* section physical address in bytes */
  unsigned int address;

  /* section virtual address in bytes */
  unsigned int virtual_address;

  /* section shadow address in bytes */
  unsigned int shadow_address;

  /* section size in bytes */
  unsigned long size;

  /* memory linked list */
  MemBlock *data;

  /* number of relocations */
  unsigned short num_reloc;

  /* head of relocations */
  gp_reloc_type *relocations;

  /* tail of relocations */
  gp_reloc_type *relocations_tail;

  /* number of line numbers */
  unsigned short num_lineno;

  /* head of line numbers */
  gp_linenum_type *line_numbers;

  /* tail of line numbers */
  gp_linenum_type *line_numbers_tail;

  /* this section required for symbol resolution, only valid when linking */
  gp_boolean is_used;

  /* section number, only valid when writing coff file */
  unsigned long number;

  /* data pointer, only valid when writing coff file */
  unsigned long data_ptr;

  /* relocations pointer, only valid when writing coff file */
  unsigned long reloc_ptr;

  /* linenumber pointer, only valid when writing coff file */
  unsigned long lineno_ptr;

  struct gp_section_type *next;
} gp_section_type;

typedef struct gp_object_type
{
  /* object filename */
  char *filename;

  /* format version/magic number */
  int version;

  /* to reduce conditionals, store the size of symbols in this object */
  size_t symbol_size;

  /* new style coff file? */
  gp_boolean isnew;

  /* processor */
  pic_processor_t processor;

  /* processor class */
  proc_class_t class;

  /* time object was created */
  time_t time;

  /* flags */
  unsigned short flags;

  /* number of sections */
  long num_sections;

  /* head of sections */
  gp_section_type *sections;

  /* tail of sections */
  gp_section_type *sections_tail;

  /* number of symbols */
  long num_symbols;

  /* head of symbols */
  gp_symbol_type *symbols;

  /* tail of symbols */
  gp_symbol_type *symbols_tail;

  /* hash table of symbols */
  gp_hash_type *symbol_hashtable;
  unsigned int symbol_hashtable_size;

  /* symbol table pointer, only valid when writing coff file */
  unsigned long symbol_ptr;

  /* next object in the linked list */
  struct gp_object_type *next;
} gp_object_type;

#endif

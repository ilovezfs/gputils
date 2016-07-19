/* gpcoff.h - header file for pic object files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

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

#ifndef __GPCOFF_H__
#define __GPCOFF_H__

/* These definitions are for the COFF as stored in a file. */

/* define the typical values, if they aren't found warn the user */
#define MICROCHIP_MAGIC_v1      0x1234
#define MICROCHIP_MAGIC_v2      0x1240

#define OPTMAGIC_v1             0x5678
#define OPTMAGIC_v2             0x5678

/* coff file header format */
struct __attribute__ ((packed)) filehdr
{
  uint16_t f_magic;             /* magic number */
  uint16_t f_nscns;             /* number of sections */
  uint32_t f_timdat;            /* time and date stamp */
  uint32_t f_symptr;            /* file ptr to symtab */
  uint32_t f_nsyms;             /* # symtab entries */
  uint16_t f_opthdr;            /* sizeof(opt hdr) */
  uint16_t f_flags;             /* flags */
};

/* define the size in the file, don't use sizeof() !! */
#define FILE_HDR_SIZ_v1         20
#define FILE_HDR_SIZ_v2         20

/* relocation info has been stripped */
#define F_RELFLG                0x0001
/* file is executable - has no unresolved external symbols */
#define F_EXEC                  0x0002
/* line numbers have been stripped */
#define F_LINENO                0x0004
/* the MPASM assembler object file is from absolute (as opposed to relocatable) assembly code */
#define F_ABSOLUTE              0x0010
/* local symbols have been stripped */
#define L_SYMS                  0x0080
/* the COFF file produced utilizing the Extended mode */
#define F_EXTENDED18            0x4000
/* processor independent file for a core */
#define F_GENERIC               0x8000

/* optional header format */
struct __attribute__ ((packed)) opthdr
{
  uint16_t opt_magic;
  uint32_t vstamp;              /* version of the compiler assembler */
  uint32_t proc_type;
  uint32_t rom_width_bits;
  uint32_t ram_width_bits;
};

#define OPT_HDR_SIZ_v1          16
#define OPT_HDR_SIZ_v2          18

/* section header format */
struct __attribute__ ((packed)) scnhdr
{
  union __attribute__ ((packed)) {
    char       name[8];         /* section name if less then 8 characters */
    struct __attribute__ ((packed)) {
      uint32_t s_zeros;         /* first four characters are 0 */
      uint32_t s_offset;        /* pointer to the string table */
    } ptr;
  } s_name;

  uint32_t     s_paddr;         /* physical address */
  uint32_t     s_vaddr;         /* virtual address */
  uint32_t     s_size;          /* section size */
  uint32_t     s_scnptr;        /* file ptr to raw data */
  uint32_t     s_relptr;        /* file ptr to relocation */
  uint32_t     s_lnnoptr;       /* file ptr to line numbers */
  uint16_t     s_nreloc;        /* # reloc entries */
  uint16_t     s_nlnno;         /* # line number entries */
  uint32_t     s_flags;         /* flags */
};

#define SEC_HDR_SIZ_v1          40
#define SEC_HDR_SIZ_v2          40

/* Section contains executable code. (.text) */
#define STYP_TEXT               0x00000020
/* Section contains initialized data. (.data) */
#define STYP_DATA               0x00000040
/* Section contains uninitialized data. (.bss) */
#define STYP_BSS                0x00000080
/* Section contains initialized data for ROM. */
#define STYP_DATA_ROM           0x00000100
/* Section is absolute. */
#define STYP_ABS                0x00001000
/* Section is shared across banks. */
#define STYP_SHARED             0x00002000
/* Section is overlaid with other sections of the same name from different objects modules. */
#define STYP_OVERLAY            0x00004000
/* Section is available using access bit. */
#define STYP_ACCESS             0x00008000
/* Section contains the activation record for a function. */
#define STYP_ACTREC             0x00010000
/* Section has been relocated. This is a temporary flag used by the linker. */
#define STYP_RELOC              0x00020000
/* Section is byte packed on 16bit devices. */
#define STYP_BPACK              0x00040000

#define STYP_RAM_AREA           (STYP_DATA | STYP_BSS)
#define STYP_ROM_AREA           (STYP_TEXT | STYP_DATA_ROM)

/* relocation entry */
struct __attribute__ ((packed)) reloc
{
  uint32_t r_vaddr;             /* entry relative virtual address */
  uint32_t r_symndx;            /* index into symbol table */
  int16_t  r_offset;            /* offset to be added to address of symbol 'r_symndx' */
  uint16_t r_type;              /* relocation type */
};

#define RELOC_SIZ               12

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
struct __attribute__ ((packed)) lineno {
  uint32_t l_srcndx;            /* symbol table index of associated source file */
  uint16_t l_lnno;              /* line number */
  uint32_t l_paddr;             /* address of code for this lineno */
  uint16_t l_flags;             /* bit flags for the line number */
  uint32_t l_fcnndx;            /* symbol table index of associated function, if there is one */
};

#define LINENO_SIZ              16

/* Set if l_fcnndx is valid. */
#define LINENO_HASFCN           1

/* symbol table entry */
struct __attribute__ ((packed)) syment {
  union __attribute__ ((packed)) {
    char       name[8];         /* symbol name if less than 8 characters */
    struct __attribute__ ((packed)) {
      uint32_t s_zeros;         /* first four characters are 0 */
      uint32_t s_offset;        /* pointer to the string table */
    } ptr;
  } sym_name;

  uint32_t     value;           /* symbol value */
  int16_t      sec_num;         /* section number */
  uint32_t     type;            /* type */
  int8_t       st_class;        /* storage class */
  uint8_t      num_auxsym;      /* number of auxiliary symbols */
};

#define SYMBOL_SIZE_v1          18
#define SYMBOL_SIZE_v2          20

/* Symbol section numbers. -- "sec_num" */
#define N_DEBUG                -2
#define N_ABS                  -1
#define N_UNDEF                 0
#define N_SCNUM                 1

/* Symbol types

  Type in an unsigned short (16 bits). The lowest four or five bits contains
  the basic type. The next higher three bits contain the derived symbol type.
  The rest of the bits are unused. */

/* Basic symbol types  */
#define T_NULL                  0   /* null */
#define T_VOID                  1   /* void */
#define T_CHAR                  2   /* character */
#define T_SHORT                 3   /* short integer */
#define T_INT                   4   /* integer */
#define T_LONG                  5   /* long integer */
#define T_FLOAT                 6   /* floating point */
#define T_DOUBLE                7   /* double floating point */
#define T_STRUCT                8   /* structure */
#define T_UNION                 9   /* union */
#define T_ENUM                  10  /* enumeration */
#define T_MOE                   11  /* member of enumeration */
#define T_UCHAR                 12  /* unsigned character */
#define T_USHORT                13  /* unsigned short */
#define T_UINT                  14  /* unsigned integer */
#define T_ULONG                 15  /* unsigned long */
/* Basic symbol types for new format only */
#define T_LNGDBL                16  /* long double floating point */
#define T_SLONG                 17  /* short long */
#define T_USLONG                18  /* unsigned short long */

/* Derived types */
#define DT_NON                  0   /* no derived type */
#define DT_PTR                  1   /* pointer */
#define DT_FCN                  2   /* function */
#define DT_ARY                  3   /* array */
#define DT_ROMPTR               4
#define DT_FARROMPTR            5

#define T_SHIFT_v1              4
#define T_MASK_v1               ((UINT32_MAX << T_SHIFT_v1) ^ UINT32_MAX)

#define T_SHIFT_v2              5
#define T_MASK_v2               ((UINT32_MAX << T_SHIFT_v2) ^ UINT32_MAX)

/* Storage classes */
#define C_NULL                  0   /* null */
#define C_AUTO                  1   /* automatic variable */
#define C_EXT                   2   /* external symbol */
#define C_STAT                  3   /* static */
#define C_REG                   4   /* register variable */
#define C_EXTDEF                5   /* external definition */
#define C_LABEL                 6   /* label */
#define C_ULABEL                7   /* undefined label */
#define C_MOS                   8   /* member of structure */
#define C_ARG                   9   /* function argument */
#define C_STRTAG                10  /* structure tag */
#define C_MOU                   11  /* member of union */
#define C_UNTAG                 12  /* union tag */
#define C_TPDEF                 13  /* type definition */
#define C_USTATIC               14  /* undefined static */
#define C_ENTAG                 15  /* enumeration tag */
#define C_MOE                   16  /* member of enumeration */
#define C_REGPARM               17  /* register parameter */
#define C_FIELD                 18  /* bit field */
#define C_AUTOARG               19  /* auto argument */
#define C_LASTENT               20  /* dummy entry (end of block) */
#define C_BLOCK                 100 /* ".bb" or ".eb" */
#define C_FCN                   101 /* ".bf" or ".ef" */
#define C_EOS                   102 /* end of structure */
#define C_FILE                  103 /* file name */
#define C_LINE                  104 /* line number reformatted as symbol table entry */
#define C_ALIAS                 105 /* duplicate tag */
#define C_HIDDEN                106 /* ext symbol in dmert public lib */
#define C_EOF                   107 /* end of file */
#define C_LIST                  108 /* absoulte listing on or off */
#define C_SECTION               109 /* section */
#define C_EFCN                  255 /* physical end of function */

/* Auxiliary symbol table entry for a file. */
struct __attribute__ ((packed)) aux_file {
  /* AUX_FILE */
  uint32_t x_offset;        /* String table offset for file name. */
  uint32_t x_incline;       /* Line number at which this file was included, 0->not included. */
  uint8_t  x_flags;
  uint8_t  _unused[11];
};

/* Auxiliary symbol table entry for a section. */
struct __attribute__ ((packed)) aux_scn {
  /* AUX_SCN */
  uint32_t x_scnlen;        /* Section Length. */
  uint16_t x_nreloc;        /* Number of relocation entries. */
  uint16_t x_nlinno;        /* Number of line numbers. */
  uint8_t  _unused[12];
};

/* Auxiliary symbol table entry for the tagname of a struct/union/enum. */
struct __attribute__ ((packed)) aux_tag {
  /* AUX_TAG */
  uint8_t  _unused0[6];
  uint16_t x_size;          /* Size of struct/union/enum. */
  uint8_t  _unused1[4];
  uint32_t x_endndx;        /* Symbol index of next entry beyond this struct/union/enum. */
  uint8_t  _unused2[4];
};

/* Auxiliary symbol table entry for an end of struct/union/enum. */
struct __attribute__ ((packed)) aux_eos {
  /* AUX_EOS */
  uint32_t x_tagndx;        /* Symbol index of struct/union/enum tag. */
  uint8_t  _unused0[2];
  uint16_t x_size;          /* Size of struct/union/enum. */
  uint8_t  _unused1[12];
};

/* Auxiliary symbol table entry for a function name. */
struct __attribute__ ((packed)) aux_fcn {
  /* AUX_FCN */
  uint32_t x_tagndx;        /* Unused?? Tag Index. */
  uint32_t x_size;          /* Unused?? Size of function in bits. */
  uint32_t x_lnnoptr;       /* File pointer to line numbers for this function. */
  uint32_t x_endndx;        /* Symbol Index of next entry beyond this function. */
  uint16_t x_actscnum;      /* Size of static activation record to allocate. */
  uint8_t  _unused[2];
};

/* Auxiliary symbol table entry for an array. */
struct __attribute__ ((packed)) aux_arr {
  /* AUX_ARR */
  uint32_t x_tagndx;        /* Unused?? Tag Index. */
  uint16_t x_lnno;          /* Unused?? Line number declaration. */
  uint16_t x_size;          /* Size of array. */
  uint16_t x_dimen[4];      /* Size of first four dimensions. */
  uint8_t  _unused[4];
};

/* Auxiliary symbol table entry for the end of a block or function. */
struct __attribute__ ((packed)) aux_eobf {
  /* AUX_EOBF */
  uint8_t  _unused0[4];
  uint16_t x_lnno;          /* C source line number of the end, relative to start of block/func. */
  uint8_t  _unused1[14];
};

/* Auxiliary symbol table entry for the beginning of a block or function. */
struct __attribute__ ((packed)) aux_bobf {
  /* AUX_BOBF */
  uint8_t  _unused0[4];
  uint16_t x_lnno;          /* C source line number of the beginning, relative to start enclosing scope. */
  uint8_t  _unused1[6];
  uint32_t x_endndx;        /* Symbol Index of next entry past this block/func. */
  uint8_t  _unused2[4];
};

/* Auxiliary symbol table entry for a variable of type struct/union/enum. */
struct __attribute__ ((packed)) aux_var {
  /* AUX_VAR */
  uint32_t x_tagndx;        /* Symbol index of struct/union/enum tagname. */
  uint8_t  _unused0[2];
  uint16_t x_size;          /* Size of the struct/union/enum. */
  uint8_t  _unused1[12];
};

struct __attribute__ ((packed)) aux_field {
  uint8_t  _unused0[6];
  uint16_t x_size;
  uint8_t  _unused1[12];
};

struct __attribute__ ((packed)) aux_fcn_calls {
  /* AUX_FCN_CALLS */
  uint32_t x_calleendx;     /* Symbol table entry of callee - 1. */
  uint32_t x_is_interrupt;  /* 0: not, 1: low, 2: high */
  uint8_t  _unused[12];
};

/* Auxiliary entries */
#define X_DIMNUM                4

#define AUX_NONE                0
#define AUX_FILE                1   /* detail information for a source file */
#define AUX_SCN                 2   /* detail information for a section */
#define AUX_TAG                 3   /* detail informationfor a struct/union/enum tag */
#define AUX_EOS                 4   /* end of struct/union/enum */
#define AUX_FCN                 5   /* detail information for a function */
#define AUX_ARR                 6   /* FIXME */
#define AUX_EOBF                7   /* end of block or function */
#define AUX_BOBF                8   /* beginning of block or function */
#define AUX_VAR                 9   /* variable */
#define AUX_DIRECT              10  /* direct message */
#define AUX_IDENT               11  /* ident */
#define AUX_FCN_CALLS           12  /* function called by this function */

/* These definitions are for the COFF as stored in memory. */

/* relocation linked list */

typedef struct gp_reloc {
  /* This always should be the first item! (gplist.c) */
  GPNodeHeader(struct gp_reloc);

  /* entry relative address in bytes */
  uint32_t          address;

  /* symbol */
  struct gp_symbol *symbol;

  /* symbol number, only valid when generating a coff file */
  uint32_t          symbol_number;

  /* offset added to address of symbol */
  int16_t           offset;

  /* relocation type */
  uint16_t          type;

  /* unique identification number of owner section */
  unsigned int      section_id;
} gp_reloc_t;

typedef struct gp_reloc_list {
  /* head of relocations
   * tail of relocations
   * number of relocations */
  GPListHeader(gp_reloc_t);
} gp_reloc_list_t;

/* line number linked list */

typedef struct gp_linenum {
  /* This always should be the first item! (gplist.c) */
  GPNodeHeader(struct gp_linenum);

  /* source file symbol */
  const struct gp_symbol *symbol;

  /* line number */
  uint16_t                line_number;

  /* byte address for this line number */
  uint32_t                address;

  /* unique identification number of owner section */
  unsigned int            section_id;
} gp_linenum_t;

typedef struct gp_linenum_list {
  /* head of line numbers
   * tail of line numbers
   * number of line numbers */
  GPListHeader(gp_linenum_t);
} gp_linenum_list_t;

/* auxiliary symbol linked list */

typedef struct gp_aux {
  /* This always should be the first item! (gplist.c) */
  GPNodeHeader(struct gp_aux);

  /* auxiliary symbol type */
  uint32_t type;

  /* FIXME: Finish the aux entries. */
  union {
    struct {
      uint8_t     command;
      const char *string;
    } _aux_direct;

    struct {
      const char *filename;
      uint32_t    line_number;
      uint8_t     flags;
    } _aux_file;

    struct {
      const char *string;
    } _aux_ident;

    struct {
      uint32_t    length;
      uint16_t    nreloc;
      uint16_t    nlineno;
    } _aux_scn;

    struct {
      struct gp_symbol *callee;         /* NULL for call through pointer */
      uint32_t          is_interrupt;   /* 0 not, 1 low, 2 high */
    } _aux_fcn_calls;

    uint8_t data[SYMBOL_SIZE_v2];
  } _aux_symbol;
} gp_aux_t;

typedef struct gp_aux_list {
  /* head of auxiliary symbols
   * tail of auxiliary symbols
   * number of auxiliary symbols */
  GPListHeader(gp_aux_t);
} gp_aux_list_t;

/* A optimize constant for the gpcoffgen.c module. */
#define OPT_FLAGS_GPCOFFGEN_MODULE      (1 << 0)

/* A optimize constant for the gpcoffopt.c module. */
#define OPT_FLAGS_GPCOFFOPT_MODULE      (1 << 1)

/* A optimize constant for the gpsymbol.c module. */
#define OPT_FLAGS_GPSYMBOL_MODULE       (1 << 2)

/* This is a non-erasable section. */
#define OPT_FLAGS_PROTECTED_SECTION     (1 << 16)

/* symbol linked list */

typedef struct gp_symbol {
  /* This always should be the first item! (gplist.c) */
  GPNodeHeader(struct gp_symbol);

  /* symbol name */
  char              *name;

  /* symbol value */
  gp_symvalue_t      value;

  /* section name of symbol */
  const char        *section_name;

  /* Section number, only for used for determining symbol type:
     N_DEBUG = -2, N_ABS = -1, N_UNDEF = 0, or N_SCNUM = 1 if defined */
  int16_t            section_number;

  /* defining section */
  struct gp_section *section;

  /* type */
  uint8_t            type;

  /* derived type */
  uint32_t           derived_type;

  /* storage class */
  uint8_t            class;

  /* list of auxiliary symbols (See above.) */
  gp_aux_list_t      aux_list;

  /* symbol number, only valid when writing coff or cod file */
  uint32_t           number;

  /* Number of relocation references from all section. */
  unsigned int       reloc_count_all_section;
  /* Number of relocation references from own section. */
  unsigned int       reloc_count_own_section;
  /* Number of relocation references from another section. */
  unsigned int       reloc_count_other_section;

  /* unique identification number of owner object */
  unsigned int       object_id;

  /*****************************************************************************
   *
   *                    Optimization part of this structure.
   *
   *****************************************************************************/

  /* use optimization -- "OPT_FLAGS_..."
     or  other        -- "WARN_..." */
  unsigned int       opt_flags;
} gp_symbol_t;

typedef struct gp_symbol_list {
  /* head of symbols
   * tail of symbols
   * number of symbols */
  GPListHeader(gp_symbol_t);
} gp_symbol_list_t;

/* hash table for find symbol name */

typedef struct gp_hash {
  hash128_t          hash;
  const gp_symbol_t *symbol;
} gp_hash_t;

/* section linked list */

typedef struct gp_section {
  /* This always should be the first item! (gplist.c) */
  GPNodeHeader(struct gp_section);

  /* section name */
  char               *name;

  /* section symbol */
  gp_symbol_t        *symbol;

  /* flags -- "STYP_..." */
  uint32_t            flags;

  /* section physical address in bytes */
  uint32_t            address;

  /* section virtual address in bytes */
  uint32_t            virtual_address;

  /* section shadow address in bytes */
  uint32_t            shadow_address;

  /* section size in bytes */
  uint32_t            size;

  /* memory linked list */
  MemBlock_t         *data;

  /* list of relocations (See above.) */
  gp_reloc_list_t     relocation_list;

  /* list of line numbers (See above.) */
  gp_linenum_list_t   line_number_list;

  /* Ordered, filtered array unto binary searches. */
  gp_linenum_t      **line_numbers_array;
  uint16_t            line_numbers_array_length;

  /* section number, only valid when writing coff file */
  uint32_t            number;

  /* data pointer, only valid when writing coff file */
  uint32_t            data_ptr;

  /* relocations pointer, only valid when writing coff file */
  uint32_t            reloc_ptr;

  /* linenumber pointer, only valid when writing coff file */
  uint32_t            lineno_ptr;

  /* number of relocation references */
  unsigned int        reloc_count;

  /* unique identification number of owner object */
  unsigned int        object_id;

  /* unique identification number */
  unsigned int        serial_id;

  /*****************************************************************************
   *
   *                    Optimization part of this structure.
   *
   *****************************************************************************/

  /* Label array of this section.
     The "label" mean: A symbol which has type of C_EXT or C_LABEL,
                       in a STYP_TEXT or STYP_ROM_DATA type of section. */
  gp_symbol_t      **label_array;
  unsigned int       num_labels;

  /* use the optimization -- "OPT_FLAGS_..." */
  uint32_t           opt_flags;
} gp_section_t;

typedef struct gp_section_list {
  /* head of sections
   * tail of sections
   * number of sections */
  GPListHeader(gp_section_t);
} gp_section_list_t;

typedef struct gp_object {
  /* object filename */
  char              *filename;

  /* format version/magic number */
  uint16_t           version;

  /* to reduce conditionals, store the size of symbols in this object */
  size_t             symbol_size;

  /* new style coff file? */
  gp_boolean         isnew;

  /* processor */
  pic_processor_t    processor;

  /* processor class */
  proc_class_t       class;

  /* time object was created */
  uint32_t           time;

  /* flags */
  uint16_t           flags;

  /* block of section pointers: Created by gp_coffgen_make_block_section function. */
  gp_section_t     **section_ptr_array;

  /* list of sections (See above.) */
  gp_section_list_t  section_list;

  /* list of reserved sections: It is necessary for the gplink/cod.c. */
  gp_section_list_t  dead_section_list;

  /* number of symbols */
  uint32_t           num_symbols;

  /* block of symbol pointers: Created by gp_coffgen_make_block_symbol function. */
  gp_symbol_t      **symbol_ptr_array;

  /* list of symbols (See above.) */
  gp_symbol_list_t   symbol_list;

  /* list of reserved symbols: It is necessary for the gplink/cod.c. */
  gp_symbol_list_t   dead_symbol_list;

  /* pointer array for relocation symbols without section */
  gp_symbol_t      **orphan_reloc_symbol_ptr_array;
  unsigned int       num_orphan_reloc_symbols;

  /* hash table of symbols */
  gp_hash_t         *symbol_hashtable;
  unsigned int       symbol_hashtable_size;

  /* symbol table pointer, only valid when writing coff file */
  uint32_t           symbol_ptr;

  /* unique identification number */
  unsigned int       serial_id;

  /* next object in the linked list */
  struct gp_object  *next;
} gp_object_t;

#endif

/* GNU PIC processor definitions
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

#ifndef __GPPROCESSOR_H__
#define __GPPROCESSOR_H__

/******************************************
        PIC12 definitions
******************************************/

#define PIC12_BANK_SIZE             32
#define PIC12_BANK_SHIFT            5
#define PIC12_RAM_ADDR_BITS         PIC12_BANK_SHIFT
#define PIC12_BMSK_BANK             0x003
#define PIC12_PAGE_SIZE             512

#define PIC12_REG_STATUS            0x03
#define PIC12_REG_FSR               0x04

#define PIC12_IDLOCS_SIZE           4

/******************************************
        PIC12E definitions
******************************************/

#define PIC12E_BMSK_BANK            0x007

/******************************************
        SX definitions
******************************************/

#define MASK_SX_PAGE                0x007

/******************************************
        PIC14 definitions
******************************************/

#define PIC14_BANK_SIZE             128
#define PIC14_BANK_SHIFT            7
#define PIC14_RAM_ADDR_BITS         PIC14_BANK_SHIFT
#define PIC14_PAGE_SIZE             2048

#define PIC14_BMSK_BANK             0x0003
#define PIC14_BMSK_PAGE             0x0003

#define PIC14_REG_STATUS            0x03

#define PIC14_BIT_STATUS_RP0        5
#define PIC14_BIT_STATUS_RP1        6
#define PIC14_BIT_STATUS_IRP        7

#define PIC14_REG_PCLATH            0x0A

#define PIC14_IDLOCS_SIZE           PIC12_IDLOCS_SIZE

/******************************************
        PIC14E definitions
******************************************/

#define PIC14E_BMSK_BANK            0x001F
#define PIC14E_BMSK_PAGE512         0x007F

#define PIC14E_REG_INDF0            0x00
#define PIC14E_REG_FSR0             0x04
#define PIC14E_REG_FSR0L            0x04
#define PIC14E_REG_FSR0H            0x05

#define PIC14E_REG_INDF1            0x01
#define PIC14E_REG_FSR1             0x06
#define PIC14E_REG_FSR1L            0x06
#define PIC14E_REG_FSR1H            0x07
#define PIC14E_REG_BSR              0x08
#define PIC14E_REG_WREG             0x09

/******************************************
        PIC16 definitions
******************************************/

#define PIC16_BANK_SIZE             256
#define PIC16_BANK_SHIFT            8
#define PIC16_RAM_ADDR_BITS         PIC16_BANK_SHIFT

#define PIC16_BMSK_BANK             0x00FF
#define PIC16_BMSK_PAGE             0x00FF

#define PIC16_REG_PCLATH            0x03
#define PIC16_REG_WREG              0x0A
#define PIC16_REG_BSR               0x0F

/******************************************
        PIC16E definitions
******************************************/

    /* Call and goto address mask for lower 8 bits. */
#define PIC16E_BMSK_BRANCH_LOWER    0x00FF
    /* Call and goto address mask for higher 12 bits. */
#define PIC16E_BMSK_BRANCH_HIGHER   0x0FFF

#define PIC16E_BMSK_SEC_INSN_WORD   0xF000

#define PIC16E_BMSK_BANK            0x000F

#define PIC16E_REG_BSR              0xFE0
#define PIC16E_REG_WREG             0xFE8
#define PIC16E_REG_PCL              0xFF9
#define PIC16E_REG_TOSL             0xFFD
#define PIC16E_REG_TOSH             0xFFE
#define PIC16E_REG_TOSU             0xFFF

#define PIC16E_IDLOCS_SIZE          8

/******************************************************************************/

struct px;
struct gp_section_type;

typedef struct _core_sfr_ {
  int address;
  const char *name;
} core_sfr_t;

typedef struct _vector_ {
  int address;
  const char *name;
} vector_t;

struct proc_class {
  /* Instruction used in making initialization data sections. */
  int retlw;
  /* Value in COFF header. */
  int rom_width;
  /* The page size of the program memory. */
  int page_size;
  /* The bank size of the RAM memory. */
  int bank_size;
  /* The shift value of bank bits in a RAM address. */
  int bank_bits_shift;
  /* The number of address bits inside a bank. */
  int addr_bits_in_bank;
  /* Bits to shift assembly code address for the COFF file byte address. */
  unsigned int org_to_byte_shift;
  /* Mask of address bits for the Program Counter. */
  unsigned int pc_mask;
  /* Mask of address bits for pages. */
  unsigned int page_mask;
  /* Mask of address bits for banks. */
  unsigned int bank_mask;
  /* Bitmask of bits that can be stored in the code section address. */
  unsigned int core_mask;
  /* Bitmask of bits that can be stored in the config section address. */
  unsigned int config_mask;
  /* Number of digits of the maximum possible flash address. */
  int addr_digits;
  /* Number of digits of the instruction word. */
  int word_digits;
  /* Number of digits of the config word. */
  int config_digits;
  /* These SFRs exist in each MCU which fall within into the PIC1xx family. */
  const core_sfr_t *core_sfr_table;
  /* Number of the core SFRs. */
  unsigned int core_sfr_number;
  /* This table contains traits of the interrupt vectors. */
  const vector_t *vector_table;
  /* Number of the interrupt vectors. */
  unsigned int vector_number;
  /* Get the start address for ID location. */
  unsigned int (*id_location)(const struct px *processor);

  /* Determine which bank of data memory the address is located. */
  int (*check_bank)(unsigned int address);

  /* Set the bank bits, return the number of instructions required. */
  int (*set_bank)(int num_banks, int bank, MemBlock *m,
                  unsigned int address);

  /* Determine which ibank of data memory the address is located. */
  int (*check_ibank)(unsigned int address);

  /* Set the ibank bits, return the number of instructions required. */
  int (*set_ibank)(int num_banks, int bank, MemBlock *m,
                   unsigned int address);

  /* Determine which page of program memory the address is located. */
  int (*check_page)(unsigned int address);

  /* Set the page bits, return the number of instructions required. */
  int (*set_page)(int num_pages, int page, MemBlock *m,
                  unsigned int address, gp_boolean use_wreg);

  /* These return the bits to set in instruction for given address. */
  int (*reloc_call)(unsigned int address);
  int (*reloc_goto)(unsigned int address);
  int (*reloc_f)(unsigned int address);
  int (*reloc_tris)(unsigned int address);
  int (*reloc_movlb)(unsigned int address);
  int (*reloc_bra)(struct gp_section_type *section, unsigned int value, unsigned int byte_org);
  int (*reloc_high)(gp_boolean is_code, int value);

  const struct insn *instructions;
  const int *num_instructions;
  const struct insn *(*find_insn)(const struct proc_class *cls, unsigned int opcode);

  unsigned int (*i_memory_get)(const MemBlock *m, unsigned int byte_address, unsigned short *word,
                               const char **section_name, const char **symbol_name);

  void (*i_memory_put)(MemBlock *m, unsigned int byte_address, unsigned short value,
                       const char *section_name, const char *symbol_name);

  void (*patch_strict)(void);
};

typedef const struct proc_class *proc_class_t;

#define PROC_CLASS_UNKNOWN  ((proc_class_t)0)        /* Unknown device. */

extern const struct proc_class proc_class_eeprom8;   /* 8 bit EEPROM */
extern const struct proc_class proc_class_eeprom16;  /* 16 bit EEPROM */
extern const struct proc_class proc_class_generic;   /* 12 bit device */
extern const struct proc_class proc_class_pic12;     /* 12 bit devices */
extern const struct proc_class proc_class_pic12e;    /* extended 12 bit devices */
extern const struct proc_class proc_class_sx;        /* 12 bit devices */
extern const struct proc_class proc_class_pic14;     /* 14 bit devices */
extern const struct proc_class proc_class_pic14e;    /* enhanced 14 bit devices */
extern const struct proc_class proc_class_pic14ex;   /* enhanced 14 bit devices */
extern const struct proc_class proc_class_pic16;     /* 16 bit devices */
extern const struct proc_class proc_class_pic16e;    /* enhanced 16 bit devices */

#define PROC_CLASS_EEPROM8  (&proc_class_eeprom8)
#define PROC_CLASS_EEPROM16 (&proc_class_eeprom16)
#define PROC_CLASS_GENERIC  (&proc_class_generic)
#define PROC_CLASS_PIC12    (&proc_class_pic12)
#define PROC_CLASS_PIC12E   (&proc_class_pic12e)
#define PROC_CLASS_SX       (&proc_class_sx)
#define PROC_CLASS_PIC14    (&proc_class_pic14)
#define PROC_CLASS_PIC14E   (&proc_class_pic14e)
#define PROC_CLASS_PIC14EX  (&proc_class_pic14ex)
#define PROC_CLASS_PIC16    (&proc_class_pic16)
#define PROC_CLASS_PIC16E   (&proc_class_pic16e)

#define IS_EEPROM8          (state.device.class == PROC_CLASS_EEPROM8)
#define IS_EEPROM16         (state.device.class == PROC_CLASS_EEPROM16)
#define IS_PIC12_CORE       (state.device.class == PROC_CLASS_PIC12)
#define IS_PIC12E_CORE      (state.device.class == PROC_CLASS_PIC12E)
#define IS_SX_CORE          (state.device.class == PROC_CLASS_SX)
#define IS_PIC14_CORE       (state.device.class == PROC_CLASS_PIC14)
#define IS_PIC14E_CORE      (state.device.class == PROC_CLASS_PIC14E)
#define IS_PIC14EX_CORE     (state.device.class == PROC_CLASS_PIC14EX)
#define IS_PIC16_CORE       (state.device.class == PROC_CLASS_PIC16)
#define IS_PIC16E_CORE      (state.device.class == PROC_CLASS_PIC16E)

#define MAX_NAMES                  3          /* Maximum number of names a processor can have. */
#define MAX_BADROM                 (1 * 2)    /* Maximum number of BADROM ranges a processor can be initialized with. */

#define PIC16E_FLAG_HAVE_EXTINST   (1 << 0)   /* The device supports the 16 bit extended instruction set. */
#define PIC16E_FLAG_J_SUBFAMILY    (1 << 1)   /* The device member of the "J" series. (18f..J..) */

struct px {
  proc_class_t class;
  const char *defined_as;
  const char *names[MAX_NAMES];
  unsigned int coff_type;
  int num_pages;
  int num_banks;
  /* These bank bits exists in the reality. */
  int bank_bits;
  /* The bounds of common (access) RAM, if exist in the PIC12, PIC12E, PIC14 and PIC14E families. */
  int common_ram_addrs[2];
  int common_ram_max;
  /* The bounds of linear RAM in the PIC14E family. */
  int linear_ram_addrs[2];
  /* These are in org to make it easier to fill from the datasheet. */
  int maxram;
  int maxrom;
  int prog_mem_size;
  int badrom[MAX_BADROM];
  int idlocs_addrs[2];
  int config_addrs[2];
  int eeprom_addrs[2];
  /* This is an OR mask for the PIC12, PIC12E, PIC14 and PIC14E families. PIC12x: 0x0FF0, PIC14x: 0x3F80 */
  int idlocs_mask;
  /* Use the gpdasm. */
  const char *header;
  const char *script;
  /* Used ony for the PROC_CLASS_PIC16E class. (PIC16E_FLAG_yyyyy) */
  unsigned int pic16e_flags;
};

typedef const struct px *pic_processor_t;

/* CONFIG addresses for the 18xx parts */
#define CONFIG1L  0x300000
#define CONFIG1H  0x300001
#define CONFIG2L  0x300002
#define CONFIG2H  0x300003
#define CONFIG3L  0x300004
#define CONFIG3H  0x300005
#define CONFIG4L  0x300006
#define CONFIG4H  0x300007
#define CONFIG5L  0x300008
#define CONFIG5H  0x300009
#define CONFIG6L  0x30000a
#define CONFIG6H  0x30000b
#define CONFIG7L  0x30000c
#define CONFIG7H  0x30000d

#define DEVID1    0x3ffffe
#define DEVID2    0x3fffff

void gp_dump_processor_list(gp_boolean list_all, proc_class_t class0, proc_class_t class1, proc_class_t class2);

void gp_processor_invoke_custom_lister(proc_class_t class0, proc_class_t class1, proc_class_t class2,
                                       void (*custom_lister)(pic_processor_t));

pic_processor_t gp_find_processor(const char *name);
proc_class_t gp_processor_class(pic_processor_t);
const char *gp_processor_class_to_str(proc_class_t class);
int gp_processor_bsr_boundary(pic_processor_t processor);
unsigned long gp_processor_coff_type(pic_processor_t processor);
int gp_processor_num_pages(pic_processor_t processor);
int gp_processor_num_banks(pic_processor_t processor);
pic_processor_t gp_processor_coff_proc(unsigned long coff_type);
const char *gp_processor_name(pic_processor_t processor, unsigned int choice);
const char *gp_processor_coff_name(unsigned long coff_type, unsigned int choice);
const char *gp_processor_header(pic_processor_t processor);
const char *gp_processor_script(pic_processor_t processor);
unsigned int gp_processor_id_location(pic_processor_t processor);

int gp_org_to_byte(unsigned int shift, int org);
int gp_byte_to_org(unsigned int shift, int byte);

const int *gp_processor_common_ram_exist(pic_processor_t processor);
int gp_processor_is_common_ram_addr(pic_processor_t processor, int address);

const int *gp_processor_linear_ram_exist(pic_processor_t processor);
int gp_processor_is_linear_ram_addr(pic_processor_t processor, int address);

const int *gp_processor_idlocs_exist(pic_processor_t processor);
int gp_processor_is_idlocs_org(pic_processor_t processor, int org);
int gp_processor_is_idlocs_byte_addr(pic_processor_t processor, int byte_address);

const int *gp_processor_config_exist(pic_processor_t processor);
int gp_processor_is_config_org(pic_processor_t processor, int org);
int gp_processor_is_config_byte_addr(pic_processor_t processor, int byte_address);

const int *gp_processor_eeprom_exist(pic_processor_t processor);
int gp_processor_is_eeprom_org(pic_processor_t processor, int org);
int gp_processor_is_eeprom_byte_addr(pic_processor_t processor, int byte_address);

int gp_processor_rom_width(proc_class_t class);
int gp_processor_check_bank(proc_class_t class, unsigned int address);

int gp_processor_set_bank(proc_class_t class,
                          int num_banks,
                          int bank,
                          MemBlock *m,
                          unsigned int address);

int gp_processor_check_ibank(proc_class_t class, unsigned int address);

int gp_processor_set_ibank(proc_class_t class,
                           int num_banks,
                           int bank,
                           MemBlock *m,
                           unsigned int address);

int gp_processor_check_page(proc_class_t class, unsigned int address);

int gp_processor_set_page(proc_class_t class,
                          int num_pages,
                          int page,
                          MemBlock *m,
                          unsigned int address,
                          gp_boolean use_wreg);

int gp_processor_retlw(proc_class_t class);

int gp_processor_org_to_byte(proc_class_t class, int org);
int gp_processor_real_to_byte(pic_processor_t processor, int org);

int gp_processor_byte_to_org(proc_class_t class, int byte_address);
int gp_processor_byte_to_real(pic_processor_t processor, int byte_address);

const core_sfr_t *gp_processor_find_sfr(proc_class_t class, int address);
const char *gp_processor_find_sfr_name(proc_class_t class, int address);
const vector_t *gp_processor_find_vector(proc_class_t class, int address);

#endif /* __GPPROCESSOR_H__ */

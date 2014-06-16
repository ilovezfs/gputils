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

#define MASK_PIC12_FILE             0x1F
#define MASK_PIC12_TRIS             0x07

    /* Call address mask. */
#define MASK_PIC12_CALL             0x0FF
    /* Goto address mask. */
#define MASK_PIC12_GOTO             0x1FF

#define REG_PIC12_STATUS            0x03
#define REG_PIC12_FSR               0x04

/******************************************
        PIC12E definitions
******************************************/

#define MASK_PIC12E_BANK            0x007

/******************************************
        PIC14 definitions
******************************************/

#define MASK_PIC14_FILE             0x7F
#define MASK_PIC14_TRIS             0x07

    /* Call and goto address mask. */
#define MASK_PIC14_BRANCH           0x07FF

#define MASK_PIC14_BANK             0x0003
#define MASK_PIC14_PAGE             0x0003

#define REG_PIC14_STATUS            0x03

#define BIT_PIC14_STATUS_RP0        5
#define BIT_PIC14_STATUS_RP1        6
#define BIT_PIC14_STATUS_IRP        7

#define REG_PIC14_PCLATH            0x0A

/******************************************
        PIC14E definitions
******************************************/

#define MASK_PIC14E_RBRA9           0x01FF

#define MASK_PIC14E_BANK            0x001F
#define MASK_PIC14E_PAGE            0x007F

#define REG_PIC14E_FSR0             0x04
#define REG_PIC14E_FSR0L            0x04
#define REG_PIC14E_FSR0H            0x05

#define REG_PIC14E_FSR1             0x06
#define REG_PIC14E_FSR1L            0x06
#define REG_PIC14E_FSR1H            0x07

/******************************************
        PIC16 definitions
******************************************/

#define MASK_PIC16_FILE             0xFF

    /* Call and goto address mask. */
#define MASK_PIC16_BRANCH           0x1FFF

#define MASK_PIC16_BANK             0x00FF
#define MASK_PIC16_PAGE             0x00FF

#define REG_PIC16_PCLATH            0x03

/******************************************
        PIC16E definitions
******************************************/

    /* Call and goto address mask for lower 8 bits. */
#define MASK_PIC16E_BRANCH_LOWER    0x00FF
    /* Call and goto address mask for higher 12 bits. */
#define MASK_PIC16E_BRANCH_HIGHER   0x0FFF

#define MASK_PIC16E_RBRA11          0x07FF

#define MASK_PIC16E_BANK            0x000F

#define REG_PIC16E_PCL              0xFF9
#define REG_PIC16E_TOSL             0xFFD
#define REG_PIC16E_TOSH             0xFFE
#define REG_PIC16E_TOSU             0xFFF

/******************************************************************************/

struct px;
struct gp_section_type;

typedef struct _core_sfr_ {
  int address;
  const char *name;
} core_sfr_t;

struct proc_class {
  /* Instruction used in making initialization data sections. */
  int retlw;
  /* Value in COFF header. */
  int rom_width;
  /* The page size of the program memory. */
  int page_size;
  /* The bank size of the RAM memory. */
  int bank_size;
  /* Bits to shift assembly code address for COFF file byte address. */
  unsigned int org_to_byte_shift;
  /* Mask of address bits for bank. */
  unsigned int bank_mask;
  /* Bitmask of bits that can be stored in code section address. */
  unsigned int core_size;
  /* Bitmask of bits that can be stored in config section address. */
  unsigned int config_size;
  /* These SFRs exist in each MCU which fall within into the PIC1xx family. */
  const core_sfr_t *core_sfr_table;
  /* Number of core SFRs. */
  unsigned int core_sfr_number;
  /* Get the address for ID location. */
  unsigned int (*id_location)(const struct px *processor);

  /* Determine which bank of data memory the address is located. */
  int (*check_bank)(unsigned int address);

  /* Set the bank bits, return the number of instructions required. */
  int (*set_bank)(int num_banks,
                  int bank,
                  MemBlock *m,
                  unsigned int address);

  /* Determine which ibank of data memory the address is located. */
  int (*check_ibank)(unsigned int address);

  /* Set the ibank bits, return the number of instructions required. */
  int (*set_ibank)(int num_banks,
                  int bank,
                  MemBlock *m,
                  unsigned int address);

  /* Determine which page of program memory the address is located. */
  int (*check_page)(unsigned int address);

  /* Set the page bits, return the number of instructions required. */
  int (*set_page)(int num_pages,
                  int page,
                  MemBlock *m,
                  unsigned int address,
                  int use_wreg);

  /* These return the bits to set in instruction for given address. */
  int (*reloc_call)(unsigned int address);
  int (*reloc_goto)(unsigned int address);
  int (*reloc_f)(unsigned int address);
  int (*reloc_tris)(unsigned int address);
  int (*reloc_movlb)(unsigned int address);
  int (*reloc_bra)(struct gp_section_type *section, unsigned value, unsigned int byte_org);
  int (*reloc_high)(gp_boolean is_code, int value);

  const struct insn *instructions;
  const int *num_instructions;
  const struct insn *(*find_insn)(const struct proc_class *cls, long int opcode);

  gp_boolean (*i_memory_get)(MemBlock *m, unsigned int byte_address, unsigned short *word,
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
#define PROC_CLASS_PIC16    (&proc_class_pic16)
#define PROC_CLASS_PIC16E   (&proc_class_pic16e)

#define IS_EEPROM8          (state.device.class == PROC_CLASS_EEPROM8)
#define IS_EEPROM16         (state.device.class == PROC_CLASS_EEPROM16)
#define IS_SX_CORE          (state.device.class == PROC_CLASS_SX)
#define IS_PIC12_CORE       (state.device.class == PROC_CLASS_PIC12)
#define IS_PIC12E_CORE      (state.device.class == PROC_CLASS_PIC12E)
#define IS_PIC14_CORE       (state.device.class == PROC_CLASS_PIC14)
#define IS_PIC14E_CORE      (state.device.class == PROC_CLASS_PIC14E)
#define IS_PIC16_CORE       (state.device.class == PROC_CLASS_PIC16)
#define IS_PIC16E_CORE      (state.device.class == PROC_CLASS_PIC16E)

typedef const struct px *pic_processor_t;

#define MAX_NAMES 3 /* Maximum number of names a processor can have. */
#define MAX_BADROM (1*2)  /* Maximum number of BADROM ranges a processor
                             can be initialized with. */

struct px {
  proc_class_t class;
  const char *defined_as;
  const char *names[MAX_NAMES];
  unsigned int coff_type;
  int num_pages;
  int num_banks;
  /* These are in org to make it easier to fill from datasheet. */
  int maxrom;
  int prog_mem_size;
  int badrom[MAX_BADROM];
  int config_addrs[2];
  /* */
  const char *script;
  int is_16bit_extended;  /* 1 if device supports 16 bit extended instruction set,
                             0 otherwise; Used ony for PROC_CLASS_PIC16E class. */
};

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

/* ID Locations for the 18xx parts. */
#define IDLOC0    0x200000
#define IDLOC1    0x200001
#define IDLOC2    0x200002
#define IDLOC3    0x200003
#define IDLOC4    0x200004
#define IDLOC5    0x200005
#define IDLOC6    0x200006
#define IDLOC7    0x200007

/* Config address for everything else. */
#define CONFIG_17CXX 0xfe00
#define CONFIG_ADDRESS_14  0x2007
#define CONFIG_ADDRESS_12  0x0fff

void gp_dump_processor_list(gp_boolean list_all, proc_class_t class);
const struct px *gp_find_processor(const char *name);
proc_class_t gp_processor_class(pic_processor_t);
int gp_processor_bsr_boundary(pic_processor_t processor);
unsigned long gp_processor_coff_type(pic_processor_t processor);
int gp_processor_num_pages(pic_processor_t processor);
int gp_processor_num_banks(pic_processor_t processor);
pic_processor_t gp_processor_coff_proc(unsigned long coff_type);
const char *gp_processor_name(pic_processor_t processor, unsigned int choice);
const char *gp_processor_coff_name(unsigned long coff_type, unsigned int choice);
const char *gp_processor_script(pic_processor_t processor);
unsigned int gp_processor_id_location(pic_processor_t processor);
gp_boolean gp_processor_is_config_addr(pic_processor_t processor, int address);
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
                          int use_wreg);

int gp_processor_retlw(proc_class_t class);

int gp_processor_org_to_byte(proc_class_t class, int org);
int gp_processor_byte_to_org(proc_class_t class, int byte);

int gp_org_to_byte(unsigned shift, int org);
int gp_byte_to_org(unsigned shift, int byte);

const core_sfr_t *gp_processor_find_sfr(proc_class_t class, int address);

#endif

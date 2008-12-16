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

enum proc_class {
  PROC_CLASS_UNKNOWN,   /* Unknown device */
  PROC_CLASS_EEPROM8,   /* 8 bit EEPROM */
  PROC_CLASS_GENERIC,   /* 12 bit device */
  PROC_CLASS_PIC12,     /* 12 bit devices */
  PROC_CLASS_SX,        /* 12 bit devices */
  PROC_CLASS_PIC14,     /* 14 bit devices */
  PROC_CLASS_PIC16,     /* 16 bit devices */
  PROC_CLASS_PIC16E     /* enhanced 16 bit devices */
};

typedef struct px *pic_processor_t;
#define no_processor ((struct px *)0)

#define MAX_NAMES 3 /* Maximum number of names a processor can have */
#define MAX_BADROM 1*2 /* Maximum number of BADROM ranges a processor can be */
                       /* initialized with */

struct px {
  enum proc_class class;
  char *defined_as;
  char *names[MAX_NAMES];
  unsigned long coff_type; 
  int num_pages;
  int num_banks;
  long maxrom;
  long badrom[MAX_BADROM];
  long config_addrs[2];
  char *script;
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

/* ID Locations for the 18xx parts */
#define IDLOC0    0x200000
#define IDLOC1    0x200001
#define IDLOC2    0x200002
#define IDLOC3    0x200003
#define IDLOC4    0x200004
#define IDLOC5    0x200005
#define IDLOC6    0x200006
#define IDLOC7    0x200007

/* Config address for everything else */
#define CONFIG_17CXX 0xfe00
#define CONFIG_ADDRESS_14  0x2007
#define CONFIG_ADDRESS_12  0x0fff

/* ID Locations */
#define IDLOC_ADDRESS_12  0x200
#define IDLOC_ADDRESS_14  0x2000

void gp_dump_processor_list(gp_boolean list_all, enum proc_class class);
struct px *gp_find_processor(char *name);
enum proc_class gp_processor_class(pic_processor_t);
int gp_processor_bsr_boundary(pic_processor_t processor);
unsigned long gp_processor_coff_type(pic_processor_t processor);
int gp_processor_num_pages(pic_processor_t processor);
int gp_processor_num_banks(pic_processor_t processor);
pic_processor_t gp_processor_coff_proc(unsigned long coff_type);
char *gp_processor_name(pic_processor_t processor, unsigned int choice);
char *gp_processor_coff_name(unsigned long coff_type, unsigned int choice);
char *gp_processor_script(pic_processor_t processor);
int gp_processor_rom_width(enum proc_class class);
int gp_processor_check_page(enum proc_class class, int address);
int gp_processor_check_bank(enum proc_class class, int address);
int gp_processor_set_page(enum proc_class class, 
                          int num_pages,
                          int page,
                          MemBlock *m, 
                          int address,
                          int use_wreg);
int gp_processor_set_bank(enum proc_class class, 
                          int num_banks,
                          int bank,
                          MemBlock *m, 
                          int address);

int gp_processor_retlw(enum proc_class class);

#endif

/* GNU PIC processor definitions
   Copyright (C) 2001, 2002, 2003, 2004
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

/* XXXPRO: Need to add an entry for any extra processors.  Please keep
   this list sorted! */

enum pic_processor {
  no_processor,
  eeprom8,
  generic,
  pic12c508,
  pic12c508a,
  pic12c509,
  pic12c509a,
  pic12c671,
  pic12c672,
  pic12ce518,
  pic12ce519,
  pic12ce673,
  pic12ce674,
  pic12cr509a,
  pic12f508,
  pic12f509,
  pic12f629,
  pic12f635,
  pic12f675,
  pic12f683,
  pic14000,
  pic16c5x,
  pic16cxx,
  pic16c432,
  pic16c433,
  pic16c505,
  pic16c54,
  pic16c52,
  pic16c54a,
  pic16c54b,
  pic16c54c,
  pic16c55,
  pic16c55a,
  pic16c554,
  pic16c557,
  pic16c558,
  pic16c56,
  pic16c56a,
  pic16c57,
  pic16c57c,
  pic16c58a,
  pic16c58b, 
  pic16c61,
  pic16c62,
  pic16c62a,
  pic16c62b,
  pic16c620,
  pic16c620a,
  pic16c621,
  pic16c621a,
  pic16c622,
  pic16c622a,
  pic16c63,
  pic16c63a,
  pic16c64,
  pic16c64a,
  pic16c642,
  pic16c65,
  pic16c65a,   
  pic16c65b,
  pic16c66,
  pic16c662,
  pic16c67,
  pic16c71,
  pic16c710,
  pic16c711,
  pic16c712,
  pic16c715,
  pic16c716,
  pic16c717,
  pic16c72,
  pic16c72a,
  pic16c73,
  pic16c73a,
  pic16c73b,
  pic16c74,
  pic16c745,
  pic16c747,
  pic16c74a,
  pic16c74b,
  pic16c76,
  pic16c765,
  pic16c77,
  pic16c770,
  pic16c771,
  pic16c773,
  pic16c774,
  pic16c781,
  pic16c782,
  pic16c84,
  pic16c923,
  pic16c924,
  pic16c925,
  pic16c926,
  pic16ce623,
  pic16ce624,
  pic16ce625,
  pic16cr54,
  pic16cr54a,
  pic16cr54b,
  pic16cr54c,
  pic16cr56a,
  pic16cr57a,
  pic16cr57b,
  pic16cr57c,
  pic16cr58a,
  pic16cr58b,
  pic16cr62,
  pic16cr620a,
  pic16cr63,
  pic16cr64,  
  pic16cr65,
  pic16cr72,
  pic16cr83,
  pic16cr84,
  pic16f505,
  pic16f54,
  pic16f57,
  pic16f627,
  pic16f627a,
  pic16f628,
  pic16f628a,
  pic16f630,
  pic16f636,
  pic16f648a,
  pic16f676,
  pic16f684,
  pic16f688,
  pic16f716,
  pic16f72,
  pic16f73,
  pic16f737,
  pic16f74,
  pic16f76,
  pic16f767,
  pic16f77,
  pic16f777,
  pic16f785,
  pic16f818,
  pic16f819,
  pic16f83,
  pic16f84,
  pic16f87,
  pic16f84a,
  pic16f870,
  pic16f871,
  pic16f872,
  pic16f873,
  pic16f873a,
  pic16f874,
  pic16f874a,
  pic16f876,
  pic16f876a,
  pic16f877,
  pic16f877a,
  pic16f88,
  pic16f913,
  pic16f914,
  pic16f916,
  pic16f917,
  pic16hv540,
  pic17cxx,
  pic17c42,
  pic17c42a,
  pic17c43,
  pic17c44,
  pic17c752,
  pic17c756,
  pic17c756a,
  pic17c762,
  pic17c766,
  pic17cr42,
  pic17cr43,
  pic18cxx,
  pic18c242,
  pic18c252,
  pic18c442,
  pic18c452,
  pic18c601,
  pic18c658,
  pic18c801,
  pic18c858,
  pic18f1220,
  pic18f1320,
  pic18f2220,
  pic18f2320,
  pic18f2331,
  pic18f2410,
  pic18f242,
  pic18f2420,
  pic18f2431,
  pic18f2439,
  pic18f2455,
  pic18f248,
  pic18f2480,
  pic18f2510,
  pic18f2515,
  pic18f252,
  pic18f2525,
  pic18f2539,
  pic18f2550,
  pic18f2520,
  pic18f258,
  pic18f2580,
  pic18f2585,
  pic18f2610,
  pic18f2620,
  pic18f2680,
  pic18f2681,
  pic18f4220,
  pic18f4320,
  pic18f4331,
  pic18f4410,
  pic18f442,
  pic18f4420,
  pic18f4431,
  pic18f4439,
  pic18f4455,
  pic18f448,
  pic18f4480,
  pic18f4510,
  pic18f4515,
  pic18f452,
  pic18f4520,
  pic18f4525,
  pic18f4539,
  pic18f4550,
  pic18f458,
  pic18f4580,
  pic18f4585,
  pic18f4610,
  pic18f4620,
  pic18f4680,
  pic18f4681,
  pic18f6410,
  pic18f6490,
  pic18f6520,
  pic18f6525,
  pic18f6585,
  pic18f6620,
  pic18f6621,
  pic18f6680,
  pic18f6720,
  pic18f8410,
  pic18f8490,
  pic18f8520,
  pic18f8525,
  pic18f8585,
  pic18f8620,
  pic18f8621,
  pic18f8680,
  pic18f8720,
  rf509af,	
  rf509ag,	
  rf675f,
  rf675h,
  rf675k,
  sx18,
  sx20,
  sx28
};

#define MAX_NAMES 3 /* Maximum number of names a processor can have */

struct px {
  enum pic_processor tag;
  enum proc_class class;
  char *defined_as;
  char *names[MAX_NAMES];
  unsigned long coff_type; 
  int num_pages;
  int num_banks;
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
enum proc_class gp_processor_class(enum pic_processor);
int gp_processor_bsr_boundary(enum pic_processor processor);
unsigned long gp_processor_coff_type(enum pic_processor processor);
int gp_processor_num_pages(enum pic_processor processor);
int gp_processor_num_banks(enum pic_processor processor);
enum pic_processor gp_processor_coff_proc(unsigned long coff_type);
char *gp_processor_name(enum pic_processor processor, unsigned int choice);
char *gp_processor_coff_name(unsigned long coff_type, unsigned int choice);
char *gp_processor_script(enum pic_processor processor);
int gp_processor_rom_width(enum proc_class class);
int gp_processor_check_page(enum proc_class class, int address);
int gp_processor_check_bank(enum proc_class class, int address);
int gp_processor_set_page(enum proc_class class, 
                          int num_pages,
                          int page,
                          MemBlock *m, 
                          int address);
int gp_processor_set_bank(enum proc_class class, 
                          int num_banks,
                          int bank,
                          MemBlock *m, 
                          int address);

int gp_processor_retlw(enum proc_class class);

#endif

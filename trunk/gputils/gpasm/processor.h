/* PIC Processor selection
   Copyright (C) 1998,1999,2000,2001 James Bowman, Craig Franklin

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

#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

void select_processor(char *name);
void dump_processor_list(void);

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
  pic12f629,
  pic12f675,
  pic14000,
  pic16c5x,
  pic16cxx,
  pic16c432,
  pic16c433,
  pic16c461,
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
  pic16c58,
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
  pic16c641,
  pic16c642,
  pic16c65,
  pic16c65a,   
  pic16c65b,
  pic16c66,
  pic16c661,
  pic16c662,
  pic16c67,
  pic16c70,
  pic16c71,
  pic16c71a,
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
  pic16c83,
  pic16c84,
  pic16c85, 
  pic16c86,   
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
  pic16cr56,
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
  pic16f627,
  pic16f627a,
  pic16f628,
  pic16f628a,
  pic16f630,
  pic16f676,
  pic16f72,
  pic16f73,
  pic16f74,
  pic16f76,
  pic16f77,
  pic16f818,
  pic16f819,
  pic16f83,
  pic16f84,
  pic16f85,
  pic16f86,
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
  pic16hv540,
  pic16lc74b,
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
  pic18f010,
  pic18f012,
  pic18f020,
  pic18f022,
  pic18f1220,
  pic18f1320,
  pic18f2220,
  pic18f2320,
  pic18f242,
  pic18f248,
  pic18f252,
  pic18f258,
  pic18f4220,
  pic18f4320,
  pic18f442,
  pic18f448,
  pic18f452,
  pic18f458,
  pic18f6620,
  pic18f6720,
  pic18f8620,
  pic18f8720,
  rf509af,	
  rf509ag,	
  sx18,
  sx20,
  sx28
};

#define MAX_NAMES 3 /* Maximum number of names a processor can have */

struct px {
  enum pic_processor tag;
  char *defined_as;
  char *names[MAX_NAMES];
};

/* CONFIG addresses for the 18cxxx parts */
#define CONFIG1L  0x300000
#define CONFIG1H  0x300001
#define CONFIG2L  0x300002
#define CONFIG2H  0x300003
#define CONFIG3H  0x300005
#define CONFIG4L  0x300006
#define DEVID1    0x3ffffe
#define DEVID2    0x3fffff

/* Config address for everything else */
#define CONFIG_17CXX 0xfe00
#define CONFIG_ADDRESS_14  0x2007
#define CONFIG_ADDRESS_12  0x0fff

/* ID Locations */
#define IDLOC_ADDRESS_12  0x200
#define IDLOC_ADDRESS_14  0x2000

#endif


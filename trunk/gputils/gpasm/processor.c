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

#include "stdhdr.h"

#include "gpasm.h"
#include "directive.h"
#include "gpsymbol.h"
#include "gperror.h"

extern int _16bit_core;
extern int _17cxx_core;

/* XXXPRO: Need to add a line here for any extra processors.  Please
   keep this list sorted! */

static struct px pics[] = {
  { eeprom8,    "__EEPROM8",  { "eeprom8",     "eeprom8",   "eeprom8"  } },
  { generic,    "__GEN",      { "generic",     "gen",       "unknown"  } },
  { pic12c508,  "__12C508",   { "pic12c508",   "p12c508",   "12c508"   } },
  { pic12c508a, "__12C508A",  { "pic12c508a",  "p12c508a",  "12c508a"  } },
  { pic12c509,  "__12C509",   { "pic12c509",   "p12c509",   "12c509"   } },
  { pic12c509a, "__12C509A",  { "pic12c509a",  "p12c509a",  "12c509a"  } },
  { pic12c671,  "__12C671",   { "pic12c671",   "p12c671",   "12c671"   } },
  { pic12c672,  "__12C672",   { "pic12c672",   "p12c672",   "12c672"   } },
  { pic12ce518, "__12CE518",  { "pic12ce518",  "p12ce518",  "12ce518"  } },
  { pic12ce519, "__12CE519",  { "pic12ce519",  "p12ce519",  "12ce519"  } },
  { pic12ce673, "__12CE673",  { "pic12ce673",  "p12ce673",  "12ce673"  } },
  { pic12ce674, "__12CE674",  { "pic12ce674",  "p12ce674",  "12ce674"  } },
  { pic12cr509a,"__12CR509A", { "pic12cr509a", "p12cr509a", "12cr509a" } },
  { pic12f629,  "__12F629",   { "pic12f629",   "p12f629",   "12f629"   } },
  { pic12f675,  "__12F675",   { "pic12f675",   "p12f675",   "12f675"   } },
  { pic14000,   "__14000",    { "pic14000",    "p14000",    "14000"    } },
  { pic16c5x,   "__16C5X",    { "pic16c5x",    "p16c5x",    "16c5x"    } },
  { pic16cxx,   "__16CXX",    { "pic16cxx",    "p16cxx",    "16cxx"    } },
  { pic16c432,  "__16C432",   { "pic16c432",   "p16c432",   "16c432"   } },
  { pic16c433,  "__16C433",   { "pic16c433",   "p16c433",   "16c433"   } },
  { pic16c461,  "__16C461",   { "pic16c461",   "p16c461",   "16c461"   } },
  { pic16c505,  "__16C505",   { "pic16c505",   "p16c505",   "16c505"   } },
  { pic16c52,   "__16C52",    { "pic16c52",    "p16c52",    "16c52"    } },
  { pic16c54,   "__16C54",    { "pic16c54",    "p16c54",    "16c54"    } },
  { pic16c54a,  "__16C54A",   { "pic16c54a",   "p16c54a",   "16c54a"   } },
  { pic16c54b,  "__16C54B",   { "pic16c54b",   "p16c54b",   "16c54b"   } },
  { pic16c54c,  "__16C54C",   { "pic16c54c",   "p16c54c",   "16c54c"   } },
  { pic16c55,   "__16C55",    { "pic16c55",    "p16c55",    "16c55"    } },
  { pic16c55a,  "__16C55A",   { "pic16c55a",   "p16c55a",   "16c55a"   } },
  { pic16c554,  "__16C554",   { "pic16c554",   "p16c554",   "16c554"   } },
  { pic16c557,  "__16C557",   { "pic16c557",   "p16c557",   "16c557"   } },
  { pic16c558,  "__16C558",   { "pic16c558",   "p16c558",   "16c558"   } },
  { pic16c56,   "__16C56",    { "pic16c56",    "p16c56",    "16c56"    } },
  { pic16c56a,  "__16C56A",   { "pic16c56a",   "p16c56a",   "16c56a"   } },
  { pic16c57,   "__16C57",    { "pic16c57",    "p16c57",    "16c57"    } },
  { pic16c57c,  "__16C57C",   { "pic16c57c",   "p16c57c",   "16c57c"   } },
  { pic16c58,   "__16C58",    { "pic16c58",    "p16c58",    "16c58"    } },
  { pic16c58a,  "__16C58A",   { "pic16c58a",   "p16c58a",   "16c58a"   } },
  { pic16c58b,  "__16C58B",   { "pic16c58b",   "p16c58b",   "16c58b"   } },
  { pic16c61,   "__16C61",    { "pic16c61",    "p16c61",    "16c61"    } },
  { pic16c62,   "__16C62",    { "pic16c62",    "p16c62",    "16c62"    } },
  { pic16c62a,  "__16C62A",   { "pic16c62a",   "p16c62a",   "16c62a"   } },
  { pic16c62b,  "__16C62B",   { "pic16c62b",   "p16c62b",   "16c62b"   } },
  { pic16c620,  "__16C620",   { "pic16c620",   "p16c620",   "16c620"   } },
  { pic16c620a, "__16C620A",  { "pic16c620a",  "p16c620a",  "16c620a"  } },
  { pic16c621,  "__16C621",   { "pic16c621",   "p16c621",   "16c621"   } },
  { pic16c621a, "__16C621A",  { "pic16c621a",  "p16c621a",  "16c621a"  } },
  { pic16c622,  "__16C622",   { "pic16c622",   "p16c622",   "16c622"   } },
  { pic16c622a, "__16C622A",  { "pic16c622a",  "p16c622a",  "16c622a"  } },
  { pic16c63,   "__16C63",    { "pic16c63",    "p16c63",    "16c63"    } },
  { pic16c63a,  "__16C63A",   { "pic16c63a",   "p16c63a",   "16c63a"   } },
  { pic16c64,   "__16C64",    { "pic16c64",    "p16c64",    "16c64"    } },
  { pic16c64a,  "__16C64A",   { "pic16c64a",   "p16c64a",   "16c64a"   } },
  { pic16c641,  "__16C641",   { "pic16c641",   "p16c641",   "16c641"   } },
  { pic16c642,  "__16C642",   { "pic16c642",   "p16c642",   "16c642"   } },
  { pic16c65,   "__16C65",    { "pic16c65",    "p16c65",    "16c65"    } },
  { pic16c65a,  "__16C65A",   { "pic16c65a",   "p16c65a",   "16c65a"   } },
  { pic16c65b,  "__16C65B",   { "pic16c65b",   "p16c65b",   "16c65b"   } },
  { pic16c66,   "__16C66",    { "pic16c66",    "p16c66",    "16c66"    } },
  { pic16c661,  "__16C661",   { "pic16c661",   "p16c661",   "16c661"   } },
  { pic16c662,  "__16C662",   { "pic16c662",   "p16c662",   "16c662"   } },
  { pic16c67,   "__16C67",    { "pic16c67",    "p16c67",    "16c67"    } },
  { pic16c70,   "__16C70",    { "pic16c70",    "p16c70",    "16c70"    } },
  { pic16c71,   "__16C71",    { "pic16c71",    "p16c71",    "16c71"    } },
  { pic16c71a,  "__16C71A",   { "pic16c71a",   "p16c71a",   "16c71a"   } },
  { pic16c710,  "__16C710",   { "pic16c710",   "p16c710",   "16c710"   } },
  { pic16c711,  "__16C711",   { "pic16c711",   "p16c711",   "16c711"   } },
  { pic16c712,  "__16C712",   { "pic16c712",   "p16c712",   "16c712"   } },  
  { pic16c715,  "__16C715",   { "pic16c715",   "p16c715",   "16c715"   } },  
  { pic16c716,  "__16C716",   { "pic16c716",   "p16c716",   "16c716"   } },
  { pic16c717,  "__16C717",   { "pic16c717",   "p16c717",   "16c717"   } },  
  { pic16c72,   "__16C72",    { "pic16c72",    "p16c72",    "16c72"    } },
  { pic16c72a,  "__16C72A",   { "pic16c72a",   "p16c72a",   "16c72a"   } },
  { pic16c73,   "__16C73",    { "pic16c73",    "p16c73",    "16c73"    } },
  { pic16c73a,  "__16C73A",   { "pic16c73a",   "p16c73a",   "16c73a"   } },
  { pic16c73b,  "__16C73B",   { "pic16c73b",   "p16c73b",   "16c73b"   } },
  { pic16c74,   "__16C74",    { "pic16c74",    "p16c74",    "16c74"    } },
  { pic16c745,  "__16C745",   { "pic16c745",   "p16c745",   "16c745"   } },
  { pic16c74a,  "__16C74A",   { "pic16c74a",   "p16c74a",   "16c74a"   } },
  { pic16c74b,  "__16C74B",   { "pic16c74b",   "p16c74b",   "16c74b"   } },
  { pic16c76,   "__16C76",    { "pic16c76",    "p16c76",    "16c76"    } },
  { pic16c765,  "__16C765",   { "pic16c765",   "p16c765",   "16c765"   } },
  { pic16c77,   "__16C77",    { "pic16c77",    "p16c77",    "16c77"    } },  
  { pic16c770,  "__16C770",   { "pic16c770",   "p16c770",   "16c770"   } },  
  { pic16c771,  "__16C771",   { "pic16c771",   "p16c771",   "16c771"   } },  
  { pic16c773,  "__16C773",   { "pic16c773",   "p16c773",   "16c773"   } },  
  { pic16c774,  "__16C774",   { "pic16c774",   "p16c774",   "16c774"   } },  
  { pic16c781,  "__16C781",   { "pic16c781",   "p16c781",   "16c781"   } },  
  { pic16c782,  "__16C782",   { "pic16c782",   "p16c782",   "16c782"   } },  
  { pic16c83,   "__16C83",    { "pic16c83",    "p16c83",    "16c83"    } },
  { pic16c84,   "__16C84",    { "pic16c84",    "p16c84",    "16c84"    } },
  { pic16c85,   "__16C85",    { "pic16c85",    "p16c85",    "16c85"    } },
  { pic16c86,   "__16C86",    { "pic16c86",    "p16c86",    "16c86"    } },
  { pic16c923,  "__16C923",   { "pic16c923",   "p16c923",   "16c923"   } },  
  { pic16c924,  "__16C924",   { "pic16c924",   "p16c924",   "16c924"   } },  
  { pic16c925,  "__16C925",   { "pic16c925",   "p16c925",   "16c925"   } },  
  { pic16c926,  "__16C926",   { "pic16c926",   "p16c926",   "16c926"   } },  
  { pic16ce623, "__16CE623",  { "pic16ce623",  "p16ce623",  "16ce623"  } },
  { pic16ce624, "__16CE624",  { "pic16ce624",  "p16ce624",  "16ce624"  } },
  { pic16ce625, "__16CE625",  { "pic16ce625",  "p16ce625",  "16ce625"  } },
  { pic16cr54,  "__16CR54",   { "pic16cr54",   "p16cr54",   "16cr54"   } },
  { pic16cr54a, "__16CR54A",  { "pic16cr54a",  "p16cr54a",  "16cr54a"  } },
  { pic16cr54b, "__16CR54B",  { "pic16cr54b",  "p16cr54b",  "16cr54b"  } },
  { pic16cr54c, "__16CR54C",  { "pic16cr54c",  "p16cr54c",  "16cr54c"  } },
  { pic16cr56,  "__16CR56",   { "pic16cr56",   "p16cr56",   "16cr56"   } },
  { pic16cr56a, "__16CR56A",  { "pic16cr56a",  "p16cr56a",  "16cr56a"  } },
  { pic16cr57a, "__16CR57A",  { "pic16cr57a",  "p16cr57a",  "16cr57a"  } },
  { pic16cr57b, "__16CR57B",  { "pic16cr57b",  "p16cr57b",  "16cr57b"  } },
  { pic16cr57c, "__16CR57C",  { "pic16cr57c",  "p16cr57c",  "16cr57c"  } },
  { pic16cr58a, "__16CR58A",  { "pic16cr58a",  "p16cr58a",  "16cr58a"  } },
  { pic16cr58b, "__16CR58B",  { "pic16cr58b",  "p16cr58b",  "16cr58b"  } },
  { pic16cr62,  "__16CR62",   { "pic16cr62",   "p16cr62",   "16cr62"   } },
  { pic16cr620a,"__16CR620A", { "pic16cr620a", "p16cr620a", "16cr620a" } },
  { pic16cr63,  "__16CR63",   { "pic16cr63",   "p16cr63",   "16cr63"   } },
  { pic16cr64,  "__16CR64",   { "pic16cr64",   "p16cr64",   "16cr64"   } },
  { pic16cr65,  "__16CR65",   { "pic16cr65",   "p16cr65",   "16cr65"   } },
  { pic16cr72,  "__16CR72",   { "pic16cr72",   "p16cr72",   "16cr72"   } },
  { pic16cr83,  "__16CR83",   { "pic16cr83",   "p16cr83",   "16cr83"   } },
  { pic16cr84,  "__16CR84",   { "pic16cr84",   "p16cr84",   "16cr84"   } },
  { pic16f627,  "__16F627",   { "pic16f627",   "p16f627",   "16f627"   } },
  { pic16f627a, "__16F627A",  { "pic16f627a",  "p16f627a",  "16f627a"  } },
  { pic16f628,  "__16F628",   { "pic16f628",   "p16f628",   "16f628"   } },  
  { pic16f628a, "__16F628A",  { "pic16f628a",  "p16f628a",  "16f628a"  } },
  { pic16f630,  "__16F630",   { "pic16f630",   "p16f630",   "16f630"   } },
  { pic16f676,  "__16F676",   { "pic16f676",   "p16f676",   "16f676"   } },
  { pic16f72,   "__16F72",    { "pic16f72",    "p16f72",    "16f72"    } },
  { pic16f73,   "__16F73",    { "pic16f73",    "p16f73",    "16f73"    } },
  { pic16f74,   "__16F74",    { "pic16f74",    "p16f74",    "16f74"    } },
  { pic16f76,   "__16F76",    { "pic16f76",    "p16f76",    "16f76"    } },
  { pic16f77,   "__16F77",    { "pic16f77",    "p16f77",    "16f77"    } },
  { pic16f818,  "__16F818",   { "pic16f818",   "p16f818",   "16f818"   } },
  { pic16f819,  "__16F819",   { "pic16f819",   "p16f819",   "16f819"   } },
  { pic16f83,   "__16F83",    { "pic16f83",    "p16f83",    "16f83"    } },
  { pic16f84,   "__16F84",    { "pic16f84",    "p16f84",    "16f84"    } },
  { pic16f84a,  "__16F84A",   { "pic16f84a",   "p16f84a",   "16f84a"   } },
  { pic16f85,   "__16F85",    { "pic16f85",    "p16f85",    "16f85"    } },
  { pic16f86,   "__16F86",    { "pic16f86",    "p16f86",    "16f86"    } },
  { pic16f87,   "__16F87",    { "pic16f87",    "p16f87",    "16f87"    } },
  { pic16f870,  "__16F870",   { "pic16f870",   "p16f870",   "16f870"   } },
  { pic16f871,  "__16F871",   { "pic16f871",   "p16f871",   "16f871"   } },
  { pic16f872,  "__16F872",   { "pic16f872",   "p16f872",   "16f872"   } },
  { pic16f873,  "__16F873",   { "pic16f873",   "p16f873",   "16f873"   } },
  { pic16f873a, "__16F873A",  { "pic16f873a",  "p16f873a",  "16f873a"  } },
  { pic16f874,  "__16F874",   { "pic16f874",   "p16f874",   "16f874"   } },
  { pic16f874a, "__16F874A",  { "pic16f874a",  "p16f874a",  "16f874a"  } },
  { pic16f876,  "__16F876",   { "pic16f876",   "p16f876",   "16f876"   } },
  { pic16f876a, "__16F876A",  { "pic16f876a",  "p16f876a",  "16f876a"  } },
  { pic16f877,  "__16F877",   { "pic16f877",   "p16f877",   "16f877"   } },
  { pic16f877a, "__16F877A",  { "pic16f877a",  "p16f877a",  "16f877a"  } },
  { pic16f88,   "__16F88",    { "pic16f88",    "p16f88",    "16f88"    } },
  { pic16hv540, "__16HV540",  { "pic16hv540",  "p16hv540",  "16hv540"  } },
  { pic16lc74b, "__16LC74B",  { "pic16lc74b",  "p16lc74b",  "16lc74b"  } },
  { pic17cxx,   "__17CXX",    { "pic17cxx",    "p17cxx",    "17cxx"    } },  
  { pic17c42,   "__17C42",    { "pic17c42",    "p17c42",    "17c42"    } },  
  { pic17c42a,  "__17C42A",   { "pic17c42a",   "p17c42a",   "17c42a"   } },  
  { pic17c43,   "__17C43",    { "pic17c43",    "p17c43",    "17c43"    } },  
  { pic17c44,   "__17C44",    { "pic17c44",    "p17c44",    "17c44"    } },  
  { pic17c752,  "__17C752",   { "pic17c752",   "p17c752",   "17c752"   } },  
  { pic17c756,  "__17C756",   { "pic17c756",   "p17c756",   "17c756"   } },  
  { pic17c756a, "__17C756A",  { "pic17c756a",  "p17c756a",  "17c756a"  } },  
  { pic17c762,  "__17C762",   { "pic17c762",   "p17c762",   "17c762"   } },
  { pic17c766,  "__17C766",   { "pic17c766",   "p17c766",   "17c766"   } },
  { pic17cr42,  "__17CR42",   { "pic17cr42",   "p17cr42",   "17cr42"   } },  
  { pic17cr43,  "__17CR43",   { "pic17cr43",   "p17cr43",   "17cr43"   } },  
  { pic18cxx,   "__18CXX",    { "pic18cxx",    "p18cxx",    "18cxx"    } },
  { pic18c242,  "__18C242",   { "pic18c242",   "p18c242",   "18c242"   } },
  { pic18c252,  "__18C252",   { "pic18c252",   "p18c252",   "18c252"   } },
  { pic18c442,  "__18C442",   { "pic18c442",   "p18c442",   "18c442"   } },
  { pic18c452,  "__18C452",   { "pic18c452",   "p18c452",   "18c452"   } },
  { pic18c601,  "__18C601",   { "pic18c601",   "p18c601",   "18c601"   } },
  { pic18c658,  "__18C658",   { "pic18c658",   "p18c658",   "18c658"   } },
  { pic18c801,  "__18C801",   { "pic18c801",   "p18c801",   "18c801"   } },
  { pic18c858,  "__18C858",   { "pic18c858",   "p18c858",   "18c858"   } },
  { pic18f010,  "__18F010",   { "pic18f010",   "p18f010",   "18f010"   } },
  { pic18f012,  "__18F012",   { "pic18f012",   "p18f012",   "18f012"   } },
  { pic18f020,  "__18F020",   { "pic18f020",   "p18f020",   "18f020"   } },
  { pic18f022,  "__18F022",   { "pic18f022",   "p18f022",   "18f022"   } },
  { pic18f1220, "__18F1220",  { "pic18f1220",  "p18f1220",  "18f1220"  } },
  { pic18f1320, "__18F1320",  { "pic18f1320",  "p18f1320",  "18f1320"  } },
  { pic18f2220, "__18F2220",  { "pic18f2220",  "p18f2220",  "18f2220"  } },
  { pic18f2320, "__18F2320",  { "pic18f2320",  "p18f2320",  "18f2320"  } },
  { pic18f242,  "__18F242",   { "pic18f242",   "p18f242",   "18f242"   } },
  { pic18f248,  "__18F248",   { "pic18f248",   "p18f248",   "18f248"   } },
  { pic18f252,  "__18F252",   { "pic18f252",   "p18f252",   "18f252"   } },
  { pic18f258,  "__18F258",   { "pic18f258",   "p18f258",   "18f258"   } },
  { pic18f4220, "__18F4220",  { "pic18f4220",  "p18f4220",  "18f4220"  } },
  { pic18f4320, "__18F4320",  { "pic18f4320",  "p18f4320",  "18f4320"  } },
  { pic18f442,  "__18F442",   { "pic18f442",   "p18f442",   "18f442"   } },
  { pic18f448,  "__18F448",   { "pic18f448",   "p18f448",   "18f448"   } },
  { pic18f452,  "__18F452",   { "pic18f452",   "p18f452",   "18f452"   } },
  { pic18f458,  "__18F458",   { "pic18f458",   "p18f458",   "18f458"   } },
  { pic18f6620, "__18F6620",  { "pic18f6620",  "p18f6620",  "18f6620"  } },
  { pic18f6720, "__18F6720",  { "pic18f6720",  "p18f6720",  "18f6720"  } },
  { pic18f8620, "__18F8620",  { "pic18f8620",  "p18f8620",  "18f8620"  } },
  { pic18f8720, "__18F8720",  { "pic18f8720",  "p18f8720",  "18f8720"  } },
  { rf509af,    "__RF509AF",  { "rf509af",     "rf509af",   "rf509af"  } },
  { rf509ag,    "__RF509AG",  { "rf509ag",     "rf509ag",   "rf509ag"  } },
  { sx18,       "__SX18",     { "sx18ac",      "sx18",      "sx18"     } },
  { sx20,       "__SX20",     { "sx20ac",      "sx20",      "sx20"     } },
  { sx28,       "__SX28",     { "sx28ac",      "sx28",      "sx28"     } }
};

#define NUM_PICS	(sizeof(pics) / sizeof(pics[0]))

/*
 * Display a list of the processor names
 */
void dump_processor_list(void)
{
#define COLUMNS  6
#define SPACE_BETWEEN 2   /* number of chars between columns */
#define FAVORITE 1        /* there are 3 names to choose from */

  int i,j,k,l,longest;

  
  for(i=0,longest=0; i<NUM_PICS; i++) {
    k = strlen(pics[i].names[FAVORITE]);
    if(k>longest)
      longest = k;
  }

  k=0;

  do {

    for(i=0; (i<COLUMNS) && (k<NUM_PICS); i++) {

      printf("%s",pics[k].names[FAVORITE]);
      if(i<(COLUMNS-1)) {
	l = longest + SPACE_BETWEEN - strlen(pics[k].names[FAVORITE]);
	for(j=0; j<l; j++)
	  putchar(' ');
      }
      k++;
    }
    putchar('\n');
  } while (k < NUM_PICS);
}


void select_processor(char *name)
{
  int i, j;
  struct px *found = NULL;

  if (state.cmd_line.processor == 1) {
    gpwarning(GPW_CMDLINE_PROC, NULL);
  } else {
    for (i = 0; i < NUM_PICS; i++) {
      for (j = 0; (j < MAX_NAMES) && (pics[i].names[j] != NULL); j++) {
        if (strcasecmp(name, pics[i].names[j]) == 0) {
          found = &pics[i];
        }
      }
    }
    if (found) {
      if (state.processor == none) {
        state.processor = found->tag;
        state.processor_info = found;
        set_global(found->defined_as, 1, PERMANENT, gvt_constant);
      } else if (state.processor != found->tag ) {
        gpwarning(GPW_REDEFINING_PROC, NULL);
        gperror(GPE_EXTRA_PROC, NULL);
      }
    } else {
      if (state.pass) {
        gperror(GPE_UNKNOWN_PROC, NULL);
      } else {
        printf("Didn't find any processor named: %s\nHere are the supported processors:\n",name);
        dump_processor_list();
        exit(1);
      }
    }
    /* load the instruction sets if necessary */
    if ((state.processor_chosen == 0) && 
        (state.processor != no_processor)) {
      opcode_init(1);	/* General directives */
      /* seperate the directives from the opcodes */
      state.stBuiltin = push_symbol_table(state.stBuiltin, 1);      
      opcode_init(2);	/* Processor-specific */
      if (!_16bit_core && !_17cxx_core) {
        opcode_init(3);   /* Special pseudo ops for 12 and 14 bit devices */
      }
      state.processor_chosen = 1;
    }
  }
}

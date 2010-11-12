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

#include "stdhdr.h"
#include "libgputils.h"

/* XXXPRO: Need to add a line here for any extra processors.  Please
   keep this list sorted primarily by number, secondarily sorting
   alphabetically. */

static struct px pics[] = {
  { PROC_CLASS_EEPROM8, "__EEPROM8",  { "eeprom8",     "eeprom8",   "eeprom8"  }, 0x1fff, 0, 0, 0x7f,     { -1, -1 },            { -1, -1 },             NULL           },
  { PROC_CLASS_GENERIC, "__GEN",      { "generic",     "gen",       "unknown"  }, 0x0000, 0, 0, -1,       { -1, -1 },            { -1, -1 },             NULL           },
  { PROC_CLASS_PIC12,   "__HCS1365",  { "hcs1365",     "hcs1365",   "hcs1365"  }, 0xf365, 2, 4, 0x3ff,    { -1, -1 },            { 0x7FF, 0x7FF },       "hcs1365.lkr"  },
  { PROC_CLASS_PIC12,   "__HCS1370",  { "hcs1370",     "hcs1370",   "hcs1370"  }, 0xf370, 2, 4, 0x3ff,    { -1, -1 },            { 0x7FF, 0x7FF },       "hcs1370.lkr"  },
  { PROC_CLASS_PIC12,   "__MCV08A",   { "mcv08a",      "mcv08a",    "mcv08a"   }, 0xa510, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "mcv08a.lkr"   },
  { PROC_CLASS_PIC12,   "__MCV14A",   { "mcv14a",      "mcv14a",    "mcv14a"   }, 0xc526, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "mcv14a.lkr"   },
  { PROC_CLASS_PIC12,   "__MCV18A",   { "mcv18a",      "mcv18a",    "mcv18a"   }, 0xcf54, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "mcv18a.lkr"   },
  { PROC_CLASS_PIC12,   "__MCV28A",   { "mcv28a",      "mcv28a",    "mcv28a"   }, 0xcf57, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "mcv28a.lkr"   },
  { PROC_CLASS_PIC12,   "__10F200",   { "pic10f200",   "p10f200",   "10f200"   }, 0xf200, 1, 1, 0xff,     { -1, -1 },            { 0xFFF, 0xFFF },       "10f200.lkr"   },
  { PROC_CLASS_PIC12,   "__10F202",   { "pic10f202",   "p10f202",   "10f202"   }, 0xf202, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "10f202.lkr"   },
  { PROC_CLASS_PIC12,   "__10F204",   { "pic10f204",   "p10f204",   "10f204"   }, 0xf204, 1, 1, 0xff,     { -1, -1 },            { 0xFFF, 0xFFF },       "10f204.lkr"   },
  { PROC_CLASS_PIC12,   "__10F206",   { "pic10f206",   "p10f206",   "10f206"   }, 0xf206, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "10f206.lkr"   },
  { PROC_CLASS_PIC12,   "__10F220",   { "pic10f220",   "p10f220",   "10f220"   }, 0xf220, 1, 1, 0xff,     { -1, -1 },            { 0xFFF, 0xFFF },       "10f220.lkr"   },
  { PROC_CLASS_PIC12,   "__10F222",   { "pic10f222",   "p10f222",   "10f222"   }, 0xf222, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "10f222.lkr"   },
  { PROC_CLASS_PIC12,   "__12C508",   { "pic12c508",   "p12c508",   "12c508"   }, 0x2508, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "12c508.lkr"   },
  { PROC_CLASS_PIC12,   "__12C508A",  { "pic12c508a",  "p12c508a",  "12c508a"  }, 0x508a, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "12c508a.lkr"  },
  { PROC_CLASS_PIC12,   "__12F508",   { "pic12f508",   "p12f508",   "12f508"   }, 0xf508, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "12f508.lkr"   },
  { PROC_CLASS_PIC12,   "__12C509",   { "pic12c509",   "p12c509",   "12c509"   }, 0x2509, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "12c509.lkr"   },
  { PROC_CLASS_PIC12,   "__12C509A",  { "pic12c509a",  "p12c509a",  "12c509a"  }, 0x509a, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "12c509a.lkr"  },
  { PROC_CLASS_PIC12,   "__12CR509A", { "pic12cr509a", "p12cr509a", "12cr509a" }, 0xd09a, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "12cr509a.lkr" },
  { PROC_CLASS_PIC12,   "__12F509",   { "pic12f509",   "p12f509",   "12f509"   }, 0xf509, 2, 4, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "12f509.lkr"   },
  { PROC_CLASS_PIC12,   "__12F510",   { "pic12f510",   "p12f510",   "12f510"   }, 0xf510, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "12f510.lkr"   },
  { PROC_CLASS_PIC12,   "__12CE518",  { "pic12ce518",  "p12ce518",  "12ce518"  }, 0x2518, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "12ce518.lkr"  },
  { PROC_CLASS_PIC12,   "__12CE519",  { "pic12ce519",  "p12ce519",  "12ce519"  }, 0x2519, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "12ce519.lkr"  },
  { PROC_CLASS_PIC12,   "__12F519",   { "pic12f519",   "p12f519",   "12f519"   }, 0xf519, 2, 4, 0x3fe,    { -1, -1 },            { 0xFFF, 0xFFF },       "12f519.lkr"   },
  { PROC_CLASS_PIC14,   "__12F609",   { "pic12f609",   "p12f609",   "12f609"   }, 0xf609, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "12f609.lkr"   },
  { PROC_CLASS_PIC14,   "__12HV609",  { "pic12hv609",  "p12hv609",  "12hv609"  }, 0x6609, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "12hv609.lkr"  },
  { PROC_CLASS_PIC14,   "__12F615",   { "pic12f615",   "p12f615",   "12f615"   }, 0xf615, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "12f615.lkr"   },
  { PROC_CLASS_PIC14,   "__12HV615",  { "pic12hv615",  "p12hv615",  "12hv615"  }, 0x6615, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "12hv615.lkr"  },
  { PROC_CLASS_PIC14,   "__12F629",   { "pic12f629",   "p12f629",   "12f629"   }, 0x2629, 1, 2, 0x217f,   { 0x3ff, 0x20ff },     { 0x2007, 0x2007 },     "12f629.lkr"   },
  { PROC_CLASS_PIC14,   "__12F635",   { "pic12f635",   "p12f635",   "12f635"   }, 0xf635, 1, 4, 0x217f,   { 0x400, 0x20ff },     { 0x2007, 0x2007 },     "12f635.lkr"   },
  { PROC_CLASS_PIC14,   "__12C671",   { "pic12c671",   "p12c671",   "12c671"   }, 0x2671, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "12c671.lkr"   },
  { PROC_CLASS_PIC14,   "__12C672",   { "pic12c672",   "p12c672",   "12c672"   }, 0x2672, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "12c672.lkr"   },
  { PROC_CLASS_PIC14,   "__12CE673",  { "pic12ce673",  "p12ce673",  "12ce673"  }, 0x2673, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "12ce673.lkr"  },
  { PROC_CLASS_PIC14,   "__12CE674",  { "pic12ce674",  "p12ce674",  "12ce674"  }, 0x2674, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "12ce674.lkr"  },
  { PROC_CLASS_PIC14,   "__12F675",   { "pic12f675",   "p12f675",   "12f675"   }, 0x2675, 1, 2, 0x217f,   { 0x3ff, 0x20ff },     { 0x2007, 0x2007 },     "12f675.lkr"   },
  { PROC_CLASS_PIC14,   "__12F683",   { "pic12f683",   "p12f683",   "12f683"   }, 0xf683, 1, 2, 0x21ff,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "12f683.lkr"   },
  { PROC_CLASS_PIC14,   "__14000",    { "pic14000",    "p14000",    "14000"    }, 0x4000, 2, 2, 0xfbf,    { -1, -1 },            { 0x2007, 0x2007 },     "14000.lkr"    },
  { PROC_CLASS_PIC14,   "__16CXX",    { "pic16cxx",    "p16cxx",    "16cxx"    }, 0x6c77, 4, 4, -1,       { -1, -1 },            { -1, -1 },             NULL           },
  { PROC_CLASS_PIC14,   "__16F1933",  { "pic16f1933",  "p16f1933",  "16f1933"  }, 0x1933, 2, 2, 0xf0ff,   { 0x1000, 0xefff },    { 0x8007, 0x8008 },     "16f1933.lkr"  },
  { PROC_CLASS_PIC14,   "__16LF1933", { "pic16lf1933", "p16lf1933", "16lf1933" }, 0xa933, 2, 2, 0xf0ff,   { 0x1000, 0xefff },    { 0x8007, 0x8008 },     "16lf1933.lkr" },
  { PROC_CLASS_PIC14,   "__16F1934",  { "pic16f1934",  "p16f1934",  "16f1934"  }, 0x1934, 2, 2, 0xf0ff,   { 0x1000, 0xefff },    { 0x8007, 0x8008 },     "16f1934.lkr"  },
  { PROC_CLASS_PIC14,   "__16LF1934", { "pic16lf1934", "p16lf1934", "16lf1934" }, 0xa934, 2, 2, 0xf0ff,   { 0x1000, 0xefff },    { 0x8007, 0x8008 },     "16lf1934.lkr" },
  { PROC_CLASS_PIC14,   "__16F1936",  { "pic16f1936",  "p16f1936",  "16f1936"  }, 0x1936, 2, 2, 0xf0ff,   { 0x2000, 0xefff },    { 0x8007, 0x8008 },     "16f1936.lkr"  },
  { PROC_CLASS_PIC14,   "__16LF1936", { "pic16lf1936", "p16lf1936", "16lf1936" }, 0xa936, 2, 2, 0xf0ff,   { 0x2000, 0xefff },    { 0x8007, 0x8008 },     "16lf1936.lkr" },
  { PROC_CLASS_PIC14,   "__16F1937",  { "pic16f1937",  "p16f1937",  "16f1937"  }, 0x1937, 2, 2, 0xf0ff,   { 0x2000, 0xefff },    { 0x8007, 0x8008 },     "16f1937.lkr"  },
  { PROC_CLASS_PIC14,   "__16LF1937", { "pic16lf1937", "p16lf1937", "16lf1937" }, 0xa937, 2, 2, 0xf0ff,   { 0x2000, 0xefff },    { 0x8007, 0x8008 },     "16lf1937.lkr" },
  { PROC_CLASS_PIC14,   "__16C432",   { "pic16c432",   "p16c432",   "16c432"   }, 0x6432, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c432.lkr"   },
  { PROC_CLASS_PIC14,   "__16C433",   { "pic16c433",   "p16c433",   "16c433"   }, 0x6433, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c433.lkr"   },
  { PROC_CLASS_PIC12,   "__16C5X",    { "pic16c5x",    "p16c5x",    "16c5x"    }, 0x658a, 4, 4, -1,       { -1, -1 },            { -1, -1 },             NULL           },
  { PROC_CLASS_PIC12,   "__16C505",   { "pic16c505",   "p16c505",   "16c505"   }, 0x2505, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c505.lkr"   },
  { PROC_CLASS_PIC12,   "__16F505",   { "pic16f505",   "p16f505",   "16f505"   }, 0xf505, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16f505.lkr"   },
  { PROC_CLASS_PIC12,   "__16F506",   { "pic16f506",   "p16f506",   "16f506"   }, 0xf506, 2, 4, 0x3fe,    { -1, -1 },            { 0xFFF, 0xFFF },       "16f506.lkr"   },
  { PROC_CLASS_PIC12,   "__16C52",    { "pic16c52",    "p16c52",    "16c52"    }, 0x6c52, 1, 1, 0x17f,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c52.lkr"    },
  { PROC_CLASS_PIC12,   "__16F526",   { "pic16f526",   "p16f526",   "16f526"   }, 0xf526, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16f526.lkr"   },
  { PROC_CLASS_PIC12,   "__16C54",    { "pic16c54",    "p16c54",    "16c54"    }, 0x6c54, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c54.lkr"    },
  { PROC_CLASS_PIC12,   "__16C54A",   { "pic16c54a",   "p16c54a",   "16c54a"   }, 0x654a, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c54a.lkr"   },
  { PROC_CLASS_PIC12,   "__16C54B",   { "pic16c54b",   "p16c54b",   "16c54b"   }, 0x654b, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c54b.lkr"   },
  { PROC_CLASS_PIC12,   "__16C54C",   { "pic16c54c",   "p16c54c",   "16c54c"   }, 0x654c, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c54c.lkr"   },
  { PROC_CLASS_PIC12,   "__16CR54",   { "pic16cr54",   "p16cr54",   "16cr54"   }, 0xdc54, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16cr54.lkr"   },
  { PROC_CLASS_PIC12,   "__16CR54A",  { "pic16cr54a",  "p16cr54a",  "16cr54a"  }, 0xd54a, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16cr54a.lkr"  },
  { PROC_CLASS_PIC12,   "__16CR54B",  { "pic16cr54b",  "p16cr54b",  "16cr54b"  }, 0xd54b, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16cr54b.lkr"  },
  { PROC_CLASS_PIC12,   "__16CR54C",  { "pic16cr54c",  "p16cr54c",  "16cr54c"  }, 0xdc54, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16cr54c.lkr"  },
  { PROC_CLASS_PIC12,   "__16F54",    { "pic16f54",    "p16f54",    "16f54"    }, 0x6f54, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16f54.lkr"    },
  { PROC_CLASS_PIC12,   "__16HV540",  { "pic16hv540",  "p16hv540",  "16hv540"  }, 0x6540, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16hv540.lkr"  },
  { PROC_CLASS_PIC12,   "__16C55",    { "pic16c55",    "p16c55",    "16c55"    }, 0x6c55, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c55.lkr"    },
  { PROC_CLASS_PIC12,   "__16C55A",   { "pic16c55a",   "p16c55a",   "16c55a"   }, 0x655a, 1, 1, 0x1ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c55a.lkr"   },
  { PROC_CLASS_PIC14,   "__16C554",   { "pic16c554",   "p16c554",   "16c554"   }, 0x6554, 1, 2, 0x1ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c554.lkr"   },
  { PROC_CLASS_PIC14,   "__16C557",   { "pic16c557",   "p16c557",   "16c557"   }, 0x6557, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c557.lkr"   },
  { PROC_CLASS_PIC14,   "__16C558",   { "pic16c558",   "p16c558",   "16c558"   }, 0x6558, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c558.lkr"   },
  { PROC_CLASS_PIC12,   "__16C56",    { "pic16c56",    "p16c56",    "16c56"    }, 0x6c56, 2, 1, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c56.lkr"    },
  { PROC_CLASS_PIC12,   "__16C56A",   { "pic16c56a",   "p16c56a",   "16c56a"   }, 0x656a, 2, 1, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c56a.lkr"   },
  { PROC_CLASS_PIC12,   "__16CR56A",  { "pic16cr56a",  "p16cr56a",  "16cr56a"  }, 0xd56a, 2, 1, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16cr56a.lkr"  },
  { PROC_CLASS_PIC12,   "__16C57",    { "pic16c57",    "p16c57",    "16c57"    }, 0x6c57, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c57.lkr"    },
  { PROC_CLASS_PIC12,   "__16C57C",   { "pic16c57c",   "p16c57c",   "16c57c"   }, 0x657c, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c57c.lkr"   },
  { PROC_CLASS_PIC12,   "__16CR57A",  { "pic16cr57a",  "p16cr57a",  "16cr57a"  }, 0xd57a, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16cr57a.lkr"  },
  { PROC_CLASS_PIC12,   "__16CR57B",  { "pic16cr57b",  "p16cr57b",  "16cr57b"  }, 0xd57b, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16cr57b.lkr"  },
  { PROC_CLASS_PIC12,   "__16CR57C",  { "pic16cr57c",  "p16cr57c",  "16cr57c"  }, 0xd57c, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16cr57c.lkr"  },
  { PROC_CLASS_PIC12,   "__16F57",    { "pic16f57",    "p16f57",    "16f57"    }, 0x6f57, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16f57.lkr"    },
  { PROC_CLASS_PIC12,   "__16C58A",   { "pic16c58a",   "p16c58a",   "16c58a"   }, 0x658a, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c58a.lkr"   },
  { PROC_CLASS_PIC12,   "__16C58B",   { "pic16c58b",   "p16c58b",   "16c58b"   }, 0x658b, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16c58b.lkr"   },
  { PROC_CLASS_PIC12,   "__16CR58A",  { "pic16cr58a",  "p16cr58a",  "16cr58a"  }, 0xd58a, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16cr58a.lkr"  },
  { PROC_CLASS_PIC12,   "__16CR58B",  { "pic16cr58b",  "p16cr58b",  "16cr58b"  }, 0xd58b, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16cr58b.lkr"  },
  { PROC_CLASS_PIC12,   "__16F59",    { "pic16f59",    "p16f59",    "16f59"    }, 0x6f59, 4, 4, 0x7ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "16f59.lkr"    },
  { PROC_CLASS_PIC14,   "__16C61",    { "pic16c61",    "p16c61",    "16c61"    }, 0x6c61, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c61.lkr"    },
  { PROC_CLASS_PIC14,   "__16F610",   { "pic16f610",   "p16f610",   "16f610"   }, 0xf610, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16f610.lkr"   },
  { PROC_CLASS_PIC14,   "__16HV610",  { "pic16hv610",  "p16hv610",  "16hv610"  }, 0x6610, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16hv610.lkr"  },
  { PROC_CLASS_PIC14,   "__16F616",   { "pic16f616",   "p16f616",   "16f616"   }, 0xf616, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16f616.lkr"   },
  { PROC_CLASS_PIC14,   "__16HV616",  { "pic16hv616",  "p16hv616",  "16hv616"  }, 0x6616, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16hv616.lkr"  },
  { PROC_CLASS_PIC14,   "__16C62",    { "pic16c62",    "p16c62",    "16c62"    }, 0x6c62, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c62.lkr"    },
  { PROC_CLASS_PIC14,   "__16C62A",   { "pic16c62a",   "p16c62a",   "16c62a"   }, 0x662a, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c62a.lkr"   },
  { PROC_CLASS_PIC14,   "__16C62B",   { "pic16c62b",   "p16c62b",   "16c62b"   }, 0x662b, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c62b.lkr"   },
  { PROC_CLASS_PIC14,   "__16CR62",   { "pic16cr62",   "p16cr62",   "16cr62"   }, 0xdc62, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16cr62.lkr"   },
  { PROC_CLASS_PIC14,   "__16C620",   { "pic16c620",   "p16c620",   "16c620"   }, 0x6620, 1, 2, 0x1ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c620.lkr"   },
  { PROC_CLASS_PIC14,   "__16C620A",  { "pic16c620a",  "p16c620a",  "16c620a"  }, 0x620a, 1, 2, 0x1ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c620a.lkr"  },
  { PROC_CLASS_PIC14,   "__16CR620A", { "pic16cr620a", "p16cr620a", "16cr620a" }, 0xd20a, 1, 2, 0x1ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16cr620a.lkr" },
  { PROC_CLASS_PIC14,   "__16C621",   { "pic16c621",   "p16c621",   "16c621"   }, 0x6621, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c621.lkr"   },
  { PROC_CLASS_PIC14,   "__16C621A",  { "pic16c621a",  "p16c621a",  "16c621a"  }, 0x621a, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c621a.lkr"  },
  { PROC_CLASS_PIC14,   "__16C622",   { "pic16c622",   "p16c622",   "16c622"   }, 0x6622, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c622.lkr"   },
  { PROC_CLASS_PIC14,   "__16C622A",  { "pic16c622a",  "p16c622a",  "16c622a"  }, 0x622a, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c622a.lkr"  },
  { PROC_CLASS_PIC14,   "__16CE623",  { "pic16ce623",  "p16ce623",  "16ce623"  }, 0x6623, 1, 2, 0x1ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16ce623.lkr"  },
  { PROC_CLASS_PIC14,   "__16CE624",  { "pic16ce624",  "p16ce624",  "16ce624"  }, 0x6624, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16ce624.lkr"  },
  { PROC_CLASS_PIC14,   "__16CE625",  { "pic16ce625",  "p16ce625",  "16ce625"  }, 0x6625, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16ce625.lkr"  },
  { PROC_CLASS_PIC14,   "__16F627",   { "pic16f627",   "p16f627",   "16f627"   }, 0x6627, 1, 4, 0x217f,   { 0x400, 0x20ff },     { 0x2007, 0x2007 },     "16f627.lkr"   },
  { PROC_CLASS_PIC14,   "__16F627A",  { "pic16f627a",  "p16f627a",  "16f627a"  }, 0x627a, 1, 4, 0x217f,   { 0x400, 0x20ff },     { 0x2007, 0x2007 },     "16f627a.lkr"  },
  { PROC_CLASS_PIC14,   "__16F628",   { "pic16f628",   "p16f628",   "16f628"   }, 0x6628, 1, 4, 0x217f,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f628.lkr"   },
  { PROC_CLASS_PIC14,   "__16F628A",  { "pic16f628a",  "p16f628a",  "16f628a"  }, 0x628a, 1, 4, 0x217f,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f628a.lkr"  },
  { PROC_CLASS_PIC14,   "__16C63",    { "pic16c63",    "p16c63",    "16c63"    }, 0x6c63, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c63.lkr"    },
  { PROC_CLASS_PIC14,   "__16C63A",   { "pic16c63a",   "p16c63a",   "16c63a"   }, 0x663a, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c63a.lkr"   },
  { PROC_CLASS_PIC14,   "__16CR63",   { "pic16cr63",   "p16cr63",   "16cr63"   }, 0x6d63, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16cr63.lkr"   },
  { PROC_CLASS_PIC14,   "__16F630",   { "pic16f630",   "p16f630",   "16f630"   }, 0x6630, 1, 4, 0x217f,   { 0x3ff, 0x20ff },     { 0x2007, 0x2007 },     "16f630.lkr"   },
  { PROC_CLASS_PIC14,   "__16F631",   { "pic16f631",   "p16f631",   "16f631"   }, 0x6631, 1, 4, 0x217f,   { 0x400, 0x20ff },     { 0x2007, 0x2007 },     "16f631.lkr"   },
  { PROC_CLASS_PIC14,   "__16F636",   { "pic16f636",   "p16f636",   "16f636"   }, 0xf636, 1, 4, 0x21ff,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f636.lkr"   },
  { PROC_CLASS_PIC14,   "__16F639",   { "pic16f639",   "p16f639",   "16f639"   }, 0xf639, 1, 4, 0x21ff,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f639.lkr"   },
  { PROC_CLASS_PIC14,   "__16C64",    { "pic16c64",    "p16c64",    "16c64"    }, 0x6c64, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c64.lkr"    },
  { PROC_CLASS_PIC14,   "__16C64A",   { "pic16c64a",   "p16c64a",   "16c64a"   }, 0x664a, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c64a.lkr"   },
  { PROC_CLASS_PIC14,   "__16CR64",   { "pic16cr64",   "p16cr64",   "16cr64"   }, 0xdc64, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16cr64.lkr"   },
  { PROC_CLASS_PIC14,   "__16C642",   { "pic16c642",   "p16c642",   "16c642"   }, 0x6642, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c642.lkr"   },
  { PROC_CLASS_PIC14,   "__16F648A",  { "pic16f648a",  "p16f648a",  "16f648a"  }, 0x648a, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16f648a.lkr"  },
  { PROC_CLASS_PIC14,   "__16C65",    { "pic16c65",    "p16c65",    "16c65"    }, 0x6c65, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c65.lkr"    },
  { PROC_CLASS_PIC14,   "__16C65A",   { "pic16c65a",   "p16c65a",   "16c65a"   }, 0x665a, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c65a.lkr"   },
  { PROC_CLASS_PIC14,   "__16C65B",   { "pic16c65b",   "p16c65b",   "16c65b"   }, 0x665b, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c65b.lkr"   },
  { PROC_CLASS_PIC14,   "__16CR65",   { "pic16cr65",   "p16cr65",   "16cr65"   }, 0x6d65, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16cr65.lkr"   },
  { PROC_CLASS_PIC14,   "__16C66",    { "pic16c66",    "p16c66",    "16c66"    }, 0x6c66, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16c66.lkr"    },
  { PROC_CLASS_PIC14,   "__16C662",   { "pic16c662",   "p16c662",   "16c662"   }, 0x6662, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c662.lkr"   },
  { PROC_CLASS_PIC14,   "__16C67",    { "pic16c67",    "p16c67",    "16c67"    }, 0x6c67, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16c67.lkr"    },
  { PROC_CLASS_PIC14,   "__16F676",   { "pic16f676",   "p16f676",   "16f676"   }, 0x6676, 1, 4, 0x217f,   { 0x3ff, 0x20ff },     { 0x2007, 0x2007 },     "16f676.lkr"   },
  { PROC_CLASS_PIC14,   "__16F677",   { "pic16f677",   "p16f677",   "16f677"   }, 0x6677, 1, 4, 0x21ff,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f677.lkr"   },
  { PROC_CLASS_PIC14,   "__16F684",   { "pic16f684",   "p16f684",   "16f684"   }, 0x6684, 1, 2, 0x21ff,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f684.lkr"   },
  { PROC_CLASS_PIC14,   "__16F685",   { "pic16f685",   "p16f685",   "16f685"   }, 0x6685, 2, 4, 0x21ff,   { 0x1000, 0x20ff },    { 0x2007, 0x2007 },     "16f685.lkr"   },
  { PROC_CLASS_PIC14,   "__16F687",   { "pic16f687",   "p16f687",   "16f687"   }, 0x6687, 1, 4, 0x21ff,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f687.lkr"   },
  { PROC_CLASS_PIC14,   "__16F688",   { "pic16f688",   "p16f688",   "16f688"   }, 0x6688, 2, 4, 0x21ff,   { 0x1000, 0x20ff },    { 0x2007, 0x2007 },     "16f688.lkr"   },
  { PROC_CLASS_PIC14,   "__16F689",   { "pic16f689",   "p16f689",   "16f689"   }, 0x6689, 2, 4, 0x21ff,   { 0x1000, 0x20ff },    { 0x2007, 0x2007 },     "16f689.lkr"   },
  { PROC_CLASS_PIC14,   "__16F690",   { "pic16f690",   "p16f690",   "16f690"   }, 0x6690, 2, 4, 0x21ff,   { 0x1000, 0x20ff },    { 0x2007, 0x2007 },     "16f690.lkr"   },
  { PROC_CLASS_PIC14,   "__16C71",    { "pic16c71",    "p16c71",    "16c71"    }, 0x6c71, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c71.lkr"    },
  { PROC_CLASS_PIC14,   "__16C710",   { "pic16c710",   "p16c710",   "16c710"   }, 0x6710, 1, 2, 0x1ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c710.lkr"   },
  { PROC_CLASS_PIC14,   "__16C711",   { "pic16c711",   "p16c711",   "16c711"   }, 0x6711, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c711.lkr"   },
  { PROC_CLASS_PIC14,   "__16C712",   { "pic16c712",   "p16c712",   "16c712"   }, 0x6712, 1, 2, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c712.lkr"   },
  { PROC_CLASS_PIC14,   "__16C715",   { "pic16c715",   "p16c715",   "16c715"   }, 0x6715, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c715.lkr"   },
  { PROC_CLASS_PIC14,   "__16C716",   { "pic16c716",   "p16c716",   "16c716"   }, 0x6716, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c716.lkr"   },
  { PROC_CLASS_PIC14,   "__16F716",   { "pic16f716",   "p16f716",   "16f716"   }, 0xf716, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16f716.lkr"   },
  { PROC_CLASS_PIC14,   "__16C717",   { "pic16c717",   "p16c717",   "16c717"   }, 0x6717, 1, 4, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c717.lkr"   },
  { PROC_CLASS_PIC14,   "__16C72",    { "pic16c72",    "p16c72",    "16c72"    }, 0x6c72, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c72.lkr"    },
  { PROC_CLASS_PIC14,   "__16C72A",   { "pic16c72a",   "p16c72a",   "16c72a"   }, 0x672a, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c72a.lkr"   },
  { PROC_CLASS_PIC14,   "__16CR72",   { "pic16cr72",   "p16cr72",   "16cr72"   }, 0x6d72, 1, 2, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16cr72.lkr"   },
  { PROC_CLASS_PIC14,   "__16F72",    { "pic16f72",    "p16f72",    "16f72"    }, 0x672f, 1, 4, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16f72.lkr"    },
  { PROC_CLASS_PIC14,   "__16F722",   { "pic16f722",   "p16f722",   "16f722"   }, 0xf722, 1, 4, 0x7ff,    { -1, -1 },            { 0x2007, 0x2008 },     "16f722.lkr"   },
  { PROC_CLASS_PIC14,   "__16LF722",  { "pic16lf722",  "p16lf722",  "16lf722"  }, 0xd722, 1, 4, 0x7ff,    { -1, -1 },            { 0x2007, 0x2008 },     "16lf722.lkr"  },
  { PROC_CLASS_PIC14,   "__16F723",   { "pic16f723",   "p16f723",   "16f723"   }, 0xf723, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2008 },     "16f723.lkr"   },
  { PROC_CLASS_PIC14,   "__16LF723",  { "pic16lf723",  "p16lf723",  "16lf723"  }, 0xd723, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2008 },     "16lf723.lkr"  },
  { PROC_CLASS_PIC14,   "__16F724",   { "pic16f724",   "p16f724",   "16f724"   }, 0xf724, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2008 },     "16f724.lkr"   },
  { PROC_CLASS_PIC14,   "__16LF724",  { "pic16lf724",  "p16lf724",  "16lf724"  }, 0xd724, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2008 },     "16lf724.lkr"  },
  { PROC_CLASS_PIC14,   "__16F726",   { "pic16f726",   "p16f726",   "16f726"   }, 0xf726, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2008 },     "16f726.lkr"   },
  { PROC_CLASS_PIC14,   "__16LF726",  { "pic16lf726",  "p16lf726",  "16lf726"  }, 0xd726, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2008 },     "16lf726.lkr"  },
  { PROC_CLASS_PIC14,   "__16F727",   { "pic16f727",   "p16f727",   "16f727"   }, 0xf727, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2008 },     "16f727.lkr"   },
  { PROC_CLASS_PIC14,   "__16LF727",  { "pic16lf727",  "p16lf727",  "16lf727"  }, 0xd727, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2008 },     "16lf727.lkr"  },
  { PROC_CLASS_PIC14,   "__16C73",    { "pic16c73",    "p16c73",    "16c73"    }, 0x6c73, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c73.lkr"    },
  { PROC_CLASS_PIC14,   "__16C73A",   { "pic16c73a",   "p16c73a",   "16c73a"   }, 0x673a, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c73a.lkr"   },
  { PROC_CLASS_PIC14,   "__16C73B",   { "pic16c73b",   "p16c73b",   "16c73b"   }, 0x673b, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c73b.lkr"   },
  { PROC_CLASS_PIC14,   "__16F73",    { "pic16f73",    "p16f73",    "16f73"    }, 0x673f, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16f73.lkr"    },
  { PROC_CLASS_PIC14,   "__16F737",   { "pic16f737",   "p16f737",   "16f737"   }, 0x6737, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2008 },     "16f737.lkr"   },
  { PROC_CLASS_PIC14,   "__16C74",    { "pic16c74",    "p16c74",    "16c74"    }, 0x6c74, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c74.lkr"    },
  { PROC_CLASS_PIC14,   "__16C74A",   { "pic16c74a",   "p16c74a",   "16c74a"   }, 0x674a, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c74a.lkr"   },
  { PROC_CLASS_PIC14,   "__16C74B",   { "pic16c74b",   "p16c74b",   "16c74b"   }, 0x674b, 2, 2, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c74b.lkr"   },
  { PROC_CLASS_PIC14,   "__16F74",    { "pic16f74",    "p16f74",    "16f74"    }, 0x674f, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16f74.lkr"    },
  { PROC_CLASS_PIC14,   "__16C745",   { "pic16c745",   "p16c745",   "16c745"   }, 0x6745, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16c745.lkr"   },
  { PROC_CLASS_PIC14,   "__16F747",   { "pic16f747",   "p16f747",   "16f747"   }, 0x6747, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2008 },     "16f747.lkr"   },
  { PROC_CLASS_PIC14,   "__16C76",    { "pic16c76",    "p16c76",    "16c76"    }, 0x6c76, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16c76.lkr"    },
  { PROC_CLASS_PIC14,   "__16F76",    { "pic16f76",    "p16f76",    "16f76"    }, 0x676f, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16f76.lkr"    },
  { PROC_CLASS_PIC14,   "__16C765",   { "pic16c765",   "p16c765",   "16c765"   }, 0x6765, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16c765.lkr"   },
  { PROC_CLASS_PIC14,   "__16F767",   { "pic16f767",   "p16f767",   "16f767"   }, 0x6767, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2008 },     "16f767.lkr"   },
  { PROC_CLASS_PIC14,   "__16C77",    { "pic16c77",    "p16c77",    "16c77"    }, 0x6c77, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16c77.lkr"    },
  { PROC_CLASS_PIC14,   "__16F77",    { "pic16f77",    "p16f77",    "16f77"    }, 0x677f, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16f77.lkr"    },
  { PROC_CLASS_PIC14,   "__16C770",   { "pic16c770",   "p16c770",   "16c770"   }, 0x6770, 1, 4, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c770.lkr"   },
  { PROC_CLASS_PIC14,   "__16C771",   { "pic16c771",   "p16c771",   "16c771"   }, 0x6771, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c771.lkr"   },
  { PROC_CLASS_PIC14,   "__16C773",   { "pic16c773",   "p16c773",   "16c773"   }, 0x6773, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c773.lkr"   },
  { PROC_CLASS_PIC14,   "__16C774",   { "pic16c774",   "p16c774",   "16c774"   }, 0x6774, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c774.lkr"   },
  { PROC_CLASS_PIC14,   "__16F777",   { "pic16f777",   "p16f777",   "16f777"   }, 0x6777, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2008 },     "16f777.lkr"   },
  { PROC_CLASS_PIC14,   "__16C781",   { "pic16c781",   "p16c781",   "16c781"   }, 0x6781, 1, 4, 0x3ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c781.lkr"   },
  { PROC_CLASS_PIC14,   "__16C782",   { "pic16c782",   "p16c782",   "16c782"   }, 0x6782, 1, 4, 0x7ff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c782.lkr"   },
  { PROC_CLASS_PIC14,   "__16F785",   { "pic16f785",   "p16f785",   "16f785"   }, 0xf785, 1, 4, 0x21ff,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f785.lkr"   },
  { PROC_CLASS_PIC14,   "__16HV785",  { "pic16hv785",  "p16hv785",  "16hv785"  }, 0x6785, 1, 4, 0x21ff,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16hv785.lkr"  },
  { PROC_CLASS_PIC14,   "__16F818",   { "pic16f818",   "p16f818",   "16f818"   }, 0x818f, 1, 4, 0x217f,   { 0x400, 0x20ff },     { 0x2007, 0x2007 },     "16f818.lkr"   },
  { PROC_CLASS_PIC14,   "__16F819",   { "pic16f819",   "p16f819",   "16f819"   }, 0x819f, 1, 4, 0x21ff,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f819.lkr"   },
  { PROC_CLASS_PIC14,   "__16CR83",   { "pic16cr83",   "p16cr83",   "16cr83"   }, 0xdc83, 1, 2, 0x213f,   { 0x200, 0x20ff },     { 0x2007, 0x2007 },     "16cr83.lkr"   },
  { PROC_CLASS_PIC14,   "__16F83",    { "pic16f83",    "p16f83",    "16f83"    }, 0x6c83, 1, 2, 0x213f,   { 0x200, 0x20ff },     { 0x2007, 0x2007 },     "16f83.lkr"    },
  { PROC_CLASS_PIC14,   "__16C84",    { "pic16c84",    "p16c84",    "16c84"    }, 0x6c84, 1, 2, 0x213f,   { 0x400, 0x20ff },     { 0x2007, 0x2007 },     "16c84.lkr"    },
  { PROC_CLASS_PIC14,   "__16CR84",   { "pic16cr84",   "p16cr84",   "16cr84"   }, 0xdc84, 1, 2, 0x213f,   { 0x400, 0x20ff },     { 0x2007, 0x2007 },     "16cr84.lkr"   },
  { PROC_CLASS_PIC14,   "__16F84",    { "pic16f84",    "p16f84",    "16f84"    }, 0x684a, 1, 2, 0x213f,   { 0x400, 0x20ff },     { 0x2007, 0x2007 },     "16f84.lkr"    },
  { PROC_CLASS_PIC14,   "__16F84A",   { "pic16f84a",   "p16f84a",   "16f84a"   }, 0x6f84, 1, 2, 0x213f,   { 0x400, 0x20ff },     { 0x2007, 0x2007 },     "16f84a.lkr"   },
  { PROC_CLASS_PIC14,   "__16F87",    { "pic16f87",    "p16f87",    "16f87"    }, 0x687f, 2, 4, 0x21ff,   { 0x1000, 0x20ff },    { 0x2007, 0x2008 },     "16f87.lkr"    },
  { PROC_CLASS_PIC14,   "__16F870",   { "pic16f870",   "p16f870",   "16f870"   }, 0x870f, 1, 4, 0x213f,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f870.lkr"   },
  { PROC_CLASS_PIC14,   "__16F871",   { "pic16f871",   "p16f871",   "16f871"   }, 0x871f, 1, 4, 0x213f,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f871.lkr"   },
  { PROC_CLASS_PIC14,   "__16F872",   { "pic16f872",   "p16f872",   "16f872"   }, 0x872f, 1, 4, 0x213f,   { 0x800, 0x20ff },     { 0x2007, 0x2007 },     "16f872.lkr"   },
  { PROC_CLASS_PIC14,   "__16F873",   { "pic16f873",   "p16f873",   "16f873"   }, 0x873f, 2, 4, 0x217f,   { 0x1000, 0x20ff },    { 0x2007, 0x2007 },     "16f873.lkr"   },
  { PROC_CLASS_PIC14,   "__16F873A",  { "pic16f873a",  "p16f873a",  "16f873a"  }, 0x873a, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16f873a.lkr"  },
  { PROC_CLASS_PIC14,   "__16F874",   { "pic16f874",   "p16f874",   "16f874"   }, 0x874f, 2, 4, 0x217f,   { 0x1000, 0x20ff },    { 0x2007, 0x2007 },     "16f874.lkr"   },
  { PROC_CLASS_PIC14,   "__16F874A",  { "pic16f874a",  "p16f874a",  "16f874a"  }, 0x874a, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16f874a.lkr"  },
  { PROC_CLASS_PIC14,   "__16F876",   { "pic16f876",   "p16f876",   "16f876"   }, 0x876f, 4, 4, 0x21ff,   { 0x2000, 0x20ff },    { 0x2007, 0x2007 },     "16f876.lkr"   },
  { PROC_CLASS_PIC14,   "__16F876A",  { "pic16f876a",  "p16f876a",  "16f876a"  }, 0x876a, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16f876a.lkr"  },
  { PROC_CLASS_PIC14,   "__16F877",   { "pic16f877",   "p16f877",   "16f877"   }, 0x877f, 4, 4, 0x21ff,   { 0x2000, 0x20ff },    { 0x2007, 0x2007 },     "16f877.lkr"   },
  { PROC_CLASS_PIC14,   "__16F877A",  { "pic16f877a",  "p16f877a",  "16f877a"  }, 0x877a, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16f877a.lkr"  },
  { PROC_CLASS_PIC14,   "__16F88",    { "pic16f88",    "p16f88",    "16f88"    }, 0x688f, 2, 4, 0x21ff,   { 0x1000, 0x20ff },    { 0x2007, 0x2008 },     "16f88.lkr"    },
  { PROC_CLASS_PIC14,   "__16F882",   { "pic16f882",   "p16f882",   "16f882"   }, 0x882f, 1, 4, 0x21ff,   { 0x800, 0x20ff },     { 0x2007, 0x2008 },     "16f882.lkr"   },
  { PROC_CLASS_PIC14,   "__16F883",   { "pic16f883",   "p16f883",   "16f883"   }, 0x883f, 2, 4, 0x21ff,   { 0x1000, 0x20ff },    { 0x2007, 0x2008 },     "16f883.lkr"   },
  { PROC_CLASS_PIC14,   "__16F884",   { "pic16f884",   "p16f884",   "16f884"   }, 0x884f, 2, 4, 0x21ff,   { 0x1000, 0x20ff },    { 0x2007, 0x2008 },     "16f884.lkr"   },
  { PROC_CLASS_PIC14,   "__16F886",   { "pic16f886",   "p16f886",   "16f886"   }, 0x886f, 4, 4, 0x21ff,   { 0x2000, 0x20ff },    { 0x2007, 0x2008 },     "16f886.lkr"   },
  { PROC_CLASS_PIC14,   "__16F887",   { "pic16f887",   "p16f887",   "16f887"   }, 0x887f, 4, 4, 0x21ff,   { 0x2000, 0x20ff },    { 0x2007, 0x2008 },     "16f887.lkr"   },
  { PROC_CLASS_PIC14,   "__16F913",   { "pic16f913",   "p16f913",   "16f913"   }, 0xf913, 2, 4, 0x21ff,   { 0x1000, 0x20ff },    { 0x2007, 0x2007 },     "16f913.lkr"   },
  { PROC_CLASS_PIC14,   "__16F914",   { "pic16f914",   "p16f914",   "16f914"   }, 0xf914, 2, 4, 0x21ff,   { 0x1000, 0x20ff },    { 0x2007, 0x2007 },     "16f914.lkr"   },
  { PROC_CLASS_PIC14,   "__16F916",   { "pic16f916",   "p16f916",   "16f916"   }, 0xf916, 4, 4, 0x21ff,   { 0x2000, 0x20ff },    { 0x2007, 0x2007 },     "16f916.lkr"   },
  { PROC_CLASS_PIC14,   "__16F917",   { "pic16f917",   "p16f917",   "16f917"   }, 0xf917, 4, 4, 0x21ff,   { 0x2000, 0x20ff },    { 0x2007, 0x2007 },     "16f917.lkr"   },
  { PROC_CLASS_PIC14,   "__16C923",   { "pic16c923",   "p16c923",   "16c923"   }, 0x6923, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c923.lkr"   },
  { PROC_CLASS_PIC14,   "__16C924",   { "pic16c924",   "p16c924",   "16c924"   }, 0x6924, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c924.lkr"   },
  { PROC_CLASS_PIC14,   "__16C925",   { "pic16c925",   "p16c925",   "16c925"   }, 0x6925, 2, 4, 0xfff,    { -1, -1 },            { 0x2007, 0x2007 },     "16c925.lkr"   },
  { PROC_CLASS_PIC14,   "__16C926",   { "pic16c926",   "p16c926",   "16c926"   }, 0x6926, 4, 4, 0x1fff,   { -1, -1 },            { 0x2007, 0x2007 },     "16c926.lkr"   },
  { PROC_CLASS_PIC14,   "__16F946",   { "pic16f946",   "p16f946",   "16f946"   }, 0xf946, 4, 4, 0x21ff,   { 0x2000, 0x20ff },    { 0x2007, 0x2007 },     "16f946.lkr"   },
  { PROC_CLASS_PIC16,   "__17CXX",    { "pic17cxx",    "p17cxx",    "17cxx"    }, 0x7756, 0, 0, -1,       { -1, -1 },            { -1, -1 },             NULL           },
  { PROC_CLASS_PIC16,   "__17C42",    { "pic17c42",    "p17c42",    "17c42"    }, 0x7c42, 0, 0, 0xffff,   { -1, -1 },            { 0xFE00, 0xFE0F },     "17c42.lkr"    },
  { PROC_CLASS_PIC16,   "__17C42A",   { "pic17c42a",   "p17c42a",   "17c42a"   }, 0x742a, 0, 0, 0xffff,   { -1, -1 },            { 0xFE00, 0xFE0F },     "17c42a.lkr"   },
  { PROC_CLASS_PIC16,   "__17CR42",   { "pic17cr42",   "p17cr42",   "17cr42"   }, 0xe42a, 0, 0, 0x7ff,    { -1, -1 },            { 0xFE00, 0xFE0F },     "17cr42.lkr"   },
  { PROC_CLASS_PIC16,   "__17C43",    { "pic17c43",    "p17c43",    "17c43"    }, 0x7c43, 0, 0, 0xfff,    { -1, -1 },            { 0xFE00, 0xFE0F },     "17c43.lkr"    },
  { PROC_CLASS_PIC16,   "__17CR43",   { "pic17cr43",   "p17cr43",   "17cr43"   }, 0xec43, 0, 0, 0xfff,    { -1, -1 },            { 0xFE00, 0xFE0F },     "17cr43.lkr"   },
  { PROC_CLASS_PIC16,   "__17C44",    { "pic17c44",    "p17c44",    "17c44"    }, 0x7c44, 0, 0, 0x1fff,   { -1, -1 },            { 0xFE00, 0xFE0F },     "17c44.lkr"    },
  { PROC_CLASS_PIC16,   "__17C752",   { "pic17c752",   "p17c752",   "17c752"   }, 0x7752, 0, 0, 0x1fff,   { -1, -1 },            { 0xFE00, 0xFE0F },     "17c752.lkr"   },
  { PROC_CLASS_PIC16,   "__17C756",   { "pic17c756",   "p17c756",   "17c756"   }, 0x7756, 0, 0, 0x3fff,   { -1, -1 },            { 0xFE00, 0xFE0F },     "17c756.lkr"   },
  { PROC_CLASS_PIC16,   "__17C756A",  { "pic17c756a",  "p17c756a",  "17c756a"  }, 0x756a, 0, 0, 0x3fff,   { -1, -1 },            { 0xFE00, 0xFE0F },     "17c756a.lkr"  },
  { PROC_CLASS_PIC16,   "__17C762",   { "pic17c762",   "p17c762",   "17c762"   }, 0x7762, 0, 0, 0x1fff,   { -1, -1 },            { 0xFE00, 0xFE0F },     "17c762.lkr"   },
  { PROC_CLASS_PIC16,   "__17C766",   { "pic17c766",   "p17c766",   "17c766"   }, 0x7766, 0, 0, 0x3fff,   { -1, -1 },            { 0xFE00, 0xFE0F },     "17c766.lkr"   },
  { PROC_CLASS_PIC16E,  "__18CXX",    { "pic18cxx",    "p18cxx",    "18cxx"    }, 0x8452, 0, 0x80, -1,       { -1, -1 },            { -1, -1 },             NULL           },
  { PROC_CLASS_PIC16E,  "__18F1220",  { "pic18f1220",  "p18f1220",  "18f1220"  }, 0xa122, 0, 0x80, 0xf000ff, { 0x1000, 0xefffff },  { 0x300000, 0x30000D }, "18f1220.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F1230",  { "pic18f1230",  "p18f1230",  "18f1230"  }, 0x1230, 0, 0x80, 0xf0007f, { 0x1000, 0xefffff },  { 0x300000, 0x30000D }, "18f1230.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F1320",  { "pic18f1320",  "p18f1320",  "18f1320"  }, 0xa132, 0, 0x80, 0xf000ff, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18f1320.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F13K22", { "pic18f13k22", "p18f13k22", "18f13k22" }, 0xb132, 0, 0x60, 0xf000ff, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18f13k22.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF13K22",{ "pic18lf13k22","p18lf13k22","18lf13k22"}, 0xa133, 0, 0x60, 0xf000ff, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18lf13k22.lkr"},
  { PROC_CLASS_PIC16E,  "__18F1330",  { "pic18f1330",  "p18f1330",  "18f1330"  }, 0x1330, 0, 0x80, 0xf0007f, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18f1330.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F13K50", { "pic18f13k50", "p18f13k50", "18f13k50" }, 0xa135, 0, 0x60, 0xf0007f, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18f13k50.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF13K50",{ "pic18lf13k50","p18lf13k50","18lf13k50"}, 0xd135, 0, 0x60, 0xf0007f, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18lf13k50.lkr" },
  { PROC_CLASS_PIC16E,  "__18F14K22", { "pic18f14k22", "p18f14k22", "18f14k22" }, 0xb142, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f14k22.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF14K22",{ "pic18lf14k22","p18lf14k22","18lf14k22"}, 0xa142, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18lf14k22.lkr"},
  { PROC_CLASS_PIC16E,  "__18F14K50", { "pic18f14k50", "p18f14k50", "18f14k50" }, 0xa145, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f14k50.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF14K50",{ "pic18lf14k50","p18lf14k50","18lf14k50"}, 0xd145, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18lf14k50.lkr"},
  { PROC_CLASS_PIC16E,  "__18F2220",  { "pic18f2220",  "p18f2220",  "18f2220"  }, 0xa222, 0, 0x80, 0xf000ff, { 0x1000, 0xefffff },  { 0x300000, 0x30000D }, "18f2220.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2221",  { "pic18f2221",  "p18f2221",  "18f2221"  }, 0x2221, 0, 0x80, 0xf000ff, { 0x1000, 0xefffff },  { 0x300000, 0x30000D }, "18f2221.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2320",  { "pic18f2320",  "p18f2320",  "18f2320"  }, 0xa232, 0, 0x80, 0xf000ff, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18f2320.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F23K20", { "pic18f23k20", "p18f23k20", "18f23k20" }, 0xd320, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f23k20.lkr" },
  { PROC_CLASS_PIC16E,  "__18F2321",  { "pic18f2321",  "p18f2321",  "18f2321"  }, 0x2321, 0, 0x80, 0xf000ff, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18f2321.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2331",  { "pic18f2331",  "p18f2331",  "18f2331"  }, 0x2331, 0, 0x60, 0xf000ff, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18f2331.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2410",  { "pic18f2410",  "p18f2410",  "18f2410"  }, 0x2410, 0, 0x80, 0x3fff,   { -1, -1 },            { 0x300000, 0x30000D }, "18f2410.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F24J10", { "pic18f24j10", "p18f24j10", "18f24j10" }, 0xd410, 0, 0x80, 0x3ff7,   { -1, -1 },            { 0x3FF8, 0x3FFD },     "18f24j10.lkr" },
  { PROC_CLASS_PIC16E,  "__18F24J11", { "pic18f24j11", "p18f24j11", "18f24j11" }, 0xd411, 0, 0x60, 0x3ff7,   { -1, -1 },            { 0x3FF8, 0x3FFF },     "18f24j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF24J11",{ "pic18lf24j11","p18lf24j11","18lf24j11"}, 0xb411, 0, 0x60, 0x3ff7,   { -1, -1 },            { 0x3FF8, 0x3FFF },     "18lf24j11.lkr"},
  { PROC_CLASS_PIC16E,  "__18C242",   { "pic18c242",   "p18c242",   "18c242"   }, 0x8242, 0, 0x80, 0x3fff,   { -1, -1 },            { 0x300000, 0x300006 }, "18c242.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F242",   { "pic18f242",   "p18f242",   "18f242"   }, 0x242f, 0, 0x80, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f242.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F2420",  { "pic18f2420",  "p18f2420",  "18f2420"  }, 0x2420, 0, 0x80, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f2420.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F24K20", { "pic18f24k20", "p18f24k20", "18f24k20" }, 0xd420, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f24k20.lkr" },
  { PROC_CLASS_PIC16E,  "__18F2423",  { "pic18f2423",  "p18f2423",  "18f2423"  }, 0x2423, 0, 0x80, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f2423.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2431",  { "pic18f2431",  "p18f2431",  "18f2431"  }, 0x2431, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f2431.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2439",  { "pic18f2439",  "p18f2439",  "18f2439"  }, 0x2439, 0, 0x80, 0xf000ff, { 0x3000, 0xefffff },  { 0x300000, 0x30000D }, "18f2439.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2450",  { "pic18f2450",  "p18f2450",  "18f2450"  }, 0x2450, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f2450.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F24J50", { "pic18f24j50", "p18f24j50", "18f24j50" }, 0xd450, 0, 0x60, 0x3ff7,   { -1, -1 },            { 0x3FF8, 0x3FFF },     "18f24j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF24J50",{ "pic18lf24j50","p18lf24j50","18lf24j50"}, 0xb450, 0, 0x60, 0x3ff7,   { -1, -1 },            { 0x3FF8, 0x3FFF },     "18lf24j50.lkr"},
  { PROC_CLASS_PIC16E,  "__18F2455",  { "pic18f2455",  "p18f2455",  "18f2455"  }, 0x2455, 0, 0x60, 0xf000ff, { 0x6000, 0xefffff },  { 0x300000, 0x30000D }, "18f2455.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2458",  { "pic18f2458",  "p18f2458",  "18f2458"  }, 0x2458, 0, 0x60, 0xf000ff, { 0x6000, 0xefffff },  { 0x300000, 0x30000D }, "18f2458.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F248",   { "pic18f248",   "p18f248",   "18f248"   }, 0x8248, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f248.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F2480",  { "pic18f2480",  "p18f2480",  "18f2480"  }, 0x2480, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f2480.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2510",  { "pic18f2510",  "p18f2510",  "18f2510"  }, 0x2510, 0, 0x80, 0x7fff,   { -1, -1 },            { 0x300000, 0x30000D }, "18f2510.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F25J10", { "pic18f25j10", "p18f25j10", "18f25j10" }, 0xd510, 0, 0x80, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFD },     "18f25j10.lkr" },
  { PROC_CLASS_PIC16E,  "__18F25J11", { "pic18f25j11", "p18f25j11", "18f25j11" }, 0xd511, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFF },     "18f25j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF25J11",{ "pic18lf25j11","p18lf25j11","18lf25j11"}, 0xb511, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFF },     "18lf25j11.lkr"},
  { PROC_CLASS_PIC16E,  "__18F2515",  { "pic18f2515",  "p18f2515",  "18f2515"  }, 0x2515, 0, 0x80, 0xbfff,   { -1, -1 },            { 0x300000, 0x30000D }, "18f2515.lkr"  },
  { PROC_CLASS_PIC16E,  "__18C252",   { "pic18c252",   "p18c252",   "18c252"   }, 0x8252, 0, 0x80, 0x7fff,   { -1, -1 },            { 0x300000, 0x300006 }, "18c252.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F252",   { "pic18f252",   "p18f252",   "18f252"   }, 0x252f, 0, 0x80, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f252.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F2520",  { "pic18f2520",  "p18f2520",  "18f2520"  }, 0x2520, 0, 0x80, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f2520.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F25K20", { "pic18f25k20", "p18f25k20", "18f25k20" }, 0xd520, 0, 0x60, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f25k20.lkr" },
  { PROC_CLASS_PIC16E,  "__18F2523",  { "pic18f2523",  "p18f2523",  "18f2523"  }, 0x2523, 0, 0x80, 0x7fff,   { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f2523.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2525",  { "pic18f2525",  "p18f2525",  "18f2525"  }, 0x2525, 0, 0x80, 0xf003ff, { 0xc000, 0xefffff },  { 0x300000, 0x30000D }, "18f2525.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2539",  { "pic18f2539",  "p18f2539",  "18f2539"  }, 0x2539, 0, 0x80, 0xf000ff, { 0x6000, 0xefffff },  { 0x300000, 0x30000D }, "18f2539.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2550",  { "pic18f2550",  "p18f2550",  "18f2550"  }, 0x2550, 0, 0x60, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f2550.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F25J50", { "pic18f25j50", "p18f25j50", "18f25j50" }, 0xd550, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFF },     "18f25j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF25J50",{ "pic18lf25j50","p18lf25j50","18lf25j50"}, 0xb551, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFF },     "18lf25j50.lkr"},
  { PROC_CLASS_PIC16E,  "__18F2553",  { "pic18f2553",  "p18f2553",  "18f2553"  }, 0x2553, 0, 0x60, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f2553.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F258",   { "pic18f258",   "p18f258",   "18f258"   }, 0x8258, 0, 0x60, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f258.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F2580",  { "pic18f2580",  "p18f2580",  "18f2580"  }, 0x2580, 0, 0x60, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f2580.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2585",  { "pic18f2585",  "p18f2585",  "18f2585"  }, 0x2585, 0, 0x60, 0xf003ff, { 0xc000, 0xefffff },  { 0x300000, 0x30000D }, "18f2585.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2610",  { "pic18f2610",  "p18f2610",  "18f2610"  }, 0x2610, 0, 0x80, 0xffff,   { -1, -1 },            { 0x300000, 0x30000D }, "18f2610.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F26J11", { "pic18f26j11", "p18f26j11", "18f26j11" }, 0xd611, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFF },     "18f26j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF26J11",{ "pic18lf26j11","p18lf26j11","18lf26j11"}, 0xb612, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFF },     "18lf26j11.lkr"},
  { PROC_CLASS_PIC16E,  "__18F2620",  { "pic18f2620",  "p18f2620",  "18f2620"  }, 0x2620, 0, 0x80, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f2620.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F26K20", { "pic18f26k20", "p18f26k20", "18f26k20" }, 0xd620, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f26k20.lkr" },
  { PROC_CLASS_PIC16E,  "__18F26J50", { "pic18f26j50", "p18f26j50", "18f26j50" }, 0xd650, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFF },     "18f26j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF26J50",{ "pic18lf26j50","p18lf26j50","18lf26j50"}, 0xb651, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFF },     "18lf26j50.lkr"},
  { PROC_CLASS_PIC16E,  "__18F2680",  { "pic18f2680",  "p18f2680",  "18f2680"  }, 0x2680, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f2680.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2681",  { "pic18f2681",  "p18f2681",  "18f2681"  }, 0x2681, 0, 0x60, -1,       { -1, -1 },            { -1, -1 },             "18f2681.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2682",  { "pic18f2682",  "p18f2682",  "18f2682"  }, 0x2682, 0, 0x60, 0xf003ff, { 0x14000, 0xefffff }, { 0x300000, 0x30000D }, "18f2682.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F2685",  { "pic18f2685",  "p18f2685",  "18f2685"  }, 0x2685, 0, 0x60, 0xf003ff, { 0x18000, 0xefffff }, { 0x300000, 0x30000D }, "18f2685.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4220",  { "pic18f4220",  "p18f4220",  "18f4220"  }, 0xa422, 0, 0x80, 0xf000ff, { 0x1000, 0xefffff },  { 0x300000, 0x30000D }, "18f4220.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4221",  { "pic18f4221",  "p18f4221",  "18f4221"  }, 0x4221, 0, 0x80, 0xf000ff, { 0x1000, 0xefffff },  { 0x300000, 0x30000D }, "18f4221.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4320",  { "pic18f4320",  "p18f4320",  "18f4320"  }, 0xa432, 0, 0x80, 0xf000ff, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18f4320.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F43K20", { "pic18f43k20", "p18f43k20", "18f43k20" }, 0xe320, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f43k20.lkr" },
  { PROC_CLASS_PIC16E,  "__18F4321",  { "pic18f4321",  "p18f4321",  "18f4321"  }, 0x4321, 0, 0x80, 0xf000ff, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18f4321.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4331",  { "pic18f4331",  "p18f4331",  "18f4331"  }, 0x4331, 0, 0x60, 0xf000ff, { 0x2000, 0xefffff },  { 0x300000, 0x30000D }, "18f4331.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4410",  { "pic18f4410",  "p18f4410",  "18f4410"  }, 0x4410, 0, 0x80, 0x3fff,   { -1, -1 },            { 0x300000, 0x30000D }, "18f4410.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F44J10", { "pic18f44j10", "p18f44j10", "18f44j10" }, 0xe410, 0, 0x80, 0x3ff7,   { -1, -1 },            { 0x3FF8, 0x3FFD },     "18f44j10.lkr" },
  { PROC_CLASS_PIC16E,  "__18F44J11", { "pic18f44j11", "p18f44j11", "18f44j11" }, 0xe411, 0, 0x60, 0x3ff7,   { -1, -1 },            { 0x3FF8, 0x3FFF },     "18f44j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF44J11",{ "pic18lf44j11","p18lf44j11","18lf44j11"}, 0xc411, 0, 0x60, 0x3ff7,   { -1, -1 },            { 0x3FF8, 0x3FFF },     "18lf44j11.lkr"},
  { PROC_CLASS_PIC16E,  "__18C442",   { "pic18c442",   "p18c442",   "18c442"   }, 0x8442, 0, 0x80, 0x3fff,   { -1, -1 },            { 0x300000, 0x300006 }, "18c442.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F442",   { "pic18f442",   "p18f442",   "18f442"   }, 0x442f, 0, 0x80, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f442.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F4420",  { "pic18f4420",  "p18f4420",  "18f4420"  }, 0x4420, 0, 0x80, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f4420.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F44K20", { "pic18f44k20", "p18f44k20", "18f44k20" }, 0xe420, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f44k20.lkr" },
  { PROC_CLASS_PIC16E,  "__18F4423",  { "pic18f4423",  "p18f4423",  "18f4423"  }, 0x4423, 0, 0x80, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f4423.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4431",  { "pic18f4431",  "p18f4431",  "18f4431"  }, 0x4431, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f4431.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4439",  { "pic18f4439",  "p18f4439",  "18f4439"  }, 0x4439, 0, 0x80, 0xf000ff, { 0x3000, 0xefffff },  { 0x300000, 0x30000D }, "18f4439.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4450",  { "pic18f4450",  "p18f4450",  "18f4450"  }, 0x4450, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f4450.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F44J50", { "pic18f44j50", "p18f44j50", "18f44j50" }, 0xe450, 0, 0x60, 0x3ff7,   { -1, -1 },            { 0x3FF8, 0x3FFF },     "18f44j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF44J50",{ "pic18lf44j50","p18lf44j50","18lf44j50"}, 0xc450, 0, 0x60, 0x3ff7,   { -1, -1 },            { 0x3FF8, 0x3FFF },     "18lf44j50.lkr"},
  { PROC_CLASS_PIC16E,  "__18F4455",  { "pic18f4455",  "p18f4455",  "18f4455"  }, 0x4455, 0, 0x60, 0xf000ff, { 0x6000, 0xefffff },  { 0x300000, 0x30000D }, "18f4455.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4458",  { "pic18f4458",  "p18f4458",  "18f4458"  }, 0x4458, 0, 0x60, 0xf000ff, { 0x6000, 0xefffff },  { 0x300000, 0x30000D }, "18f4458.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F448",   { "pic18f448",   "p18f448",   "18f448"   }, 0x8448, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f448.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F4480",  { "pic18f4480",  "p18f4480",  "18f4480"  }, 0x4480, 0, 0x60, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "18f4480.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4510",  { "pic18f4510",  "p18f4510",  "18f4510"  }, 0x4510, 0, 0x80, 0x7fff,   { -1, -1 },            { 0x300000, 0x30000D }, "18f4510.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F45J10", { "pic18f45j10", "p18f45j10", "18f45j10" }, 0xe510, 0, 0x80, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFD },     "18f45j10.lkr" },
  { PROC_CLASS_PIC16E,  "__18F45J11", { "pic18f45j11", "p18f45j11", "18f45j11" }, 0xe511, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFF },     "18f45j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF45J11",{ "pic18lf45j11","p18lf45j11","18lf45j11"}, 0xc511, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFF },     "18lf45j11.lkr"},
  { PROC_CLASS_PIC16E,  "__18F4515",  { "pic18f4515",  "p18f4515",  "18f4515"  }, 0x4515, 0, 0x80, 0xbfff,   { -1, -1 },            { 0x300000, 0x30000D }, "18f4515.lkr"  },
  { PROC_CLASS_PIC16E,  "__18C452",   { "pic18c452",   "p18c452",   "18c452"   }, 0x8452, 0, 0x80, 0x7fff,   { -1, -1 },            { 0x300000, 0x300006 }, "18c452.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F452",   { "pic18f452",   "p18f452",   "18f452"   }, 0x452f, 0, 0x80, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f452.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F4520",  { "pic18f4520",  "p18f4520",  "18f4520"  }, 0x4520, 0, 0x80, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f4520.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F45K20", { "pic18f45k20", "p18f45k20", "18f45k20" }, 0xe520, 0, 0x60, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f45k20.lkr" },
  { PROC_CLASS_PIC16E,  "__18F4523",  { "pic18f4523",  "p18f4523",  "18f4523"  }, 0x4523, 0, 0x80, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f4523.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4525",  { "pic18f4525",  "p18f4525",  "18f4525"  }, 0x4525, 0, 0x80, 0xf003ff, { 0xc000, 0xefffff },  { 0x300000, 0x30000D }, "18f4525.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4539",  { "pic18f4539",  "p18f4539",  "18f4539"  }, 0x4539, 0, 0x80, 0xf000ff, { 0x6000, 0xefffff },  { 0x300000, 0x30000D }, "18f4539.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4550",  { "pic18f4550",  "p18f4550",  "18f4550"  }, 0x4550, 0, 0x60, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f4550.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F45J50", { "pic18f45j50", "p18f45j50", "18f45j50" }, 0xe550, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFF },     "18f45j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF45J50",{ "pic18lf45j50","p18lf45j50","18lf45j50"}, 0xc551, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFF },     "18lf45j50.lkr"},
  { PROC_CLASS_PIC16E,  "__18F4553",  { "pic18f4553",  "p18f4553",  "18f4553"  }, 0x4553, 0, 0x60, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f4553.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F458",   { "pic18f458",   "p18f458",   "18f458"   }, 0x8458, 0, 0x60, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f458.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F4580",  { "pic18f4580",  "p18f4580",  "18f4580"  }, 0x4580, 0, 0x60, 0xf000ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f4580.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4585",  { "pic18f4585",  "p18f4585",  "18f4585"  }, 0x4585, 0, 0x60, 0xf003ff, { 0xc000, 0xefffff },  { 0x300000, 0x30000D }, "18f4585.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4610",  { "pic18f4610",  "p18f4610",  "18f4610"  }, 0x4610, 0, 0x80, 0xffff,   { -1, -1 },            { 0x300000, 0x30000D }, "18f4610.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F46J11", { "pic18f46j11", "p18f46j11", "18f46j11" }, 0xe611, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFF },     "18f46j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF46J11",{ "pic18lf46j11","p18lf46j11","18lf46j11"}, 0xc612, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFF },     "18lf46j11.lkr"},
  { PROC_CLASS_PIC16E,  "__18F4620",  { "pic18f4620",  "p18f4620",  "18f4620"  }, 0x4620, 0, 0x80, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f4620.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F46K20", { "pic18f46k20", "p18f46k20", "18f46k20" }, 0xe620, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f46k20.lkr" },
  { PROC_CLASS_PIC16E,  "__18F46J50", { "pic18f46j50", "p18f46j50", "18f46j50" }, 0xe650, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFF },     "18f46j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18LF46J50",{ "pic18lf46j50","p18lf46j50","18lf46j50"}, 0xc651, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFF },     "18lf46j50.lkr"},
  { PROC_CLASS_PIC16E,  "__18F4680",  { "pic18f4680",  "p18f4680",  "18f4680"  }, 0x4680, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f4680.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4681",  { "pic18f4681",  "p18f4681",  "18f4681"  }, 0x4681, 0, 0x60, -1,       { -1, -1 },            { -1, -1 },             "18f4681.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4682",  { "pic18f4682",  "p18f4682",  "18f4682"  }, 0x4682, 0, 0x60, 0xf003ff, { 0x14000, 0xefffff }, { 0x300000, 0x30000D }, "18f4682.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F4685",  { "pic18f4685",  "p18f4685",  "18f4685"  }, 0x4685, 0, 0x60, 0xf003ff, { 0x18000, 0xefffff }, { 0x300000, 0x30000D }, "18f4685.lkr"  },
  { PROC_CLASS_PIC16E,  "__18C601",   { "pic18c601",   "p18c601",   "18c601"   }, 0x8601, 0, 0x80, 0x3ffff,  { -1, -1 },            { 0x300000, 0x300006 }, "18c601.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F6310",  { "pic18f6310",  "p18f6310",  "18f6310"  }, 0x6310, 0, 0x60, 0x1fff,   { -1, -1 },            { 0x300000, 0x30000C }, "18f6310.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F63J11", { "pic18f63j11", "p18f63j11", "18f63j11" }, 0x6311, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x1FF8, 0x1FFD },     "18f63j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18F6390",  { "pic18f6390",  "p18f6390",  "18f6390"  }, 0x6390, 0, 0x60, 0x1fff,   { -1, -1 },            { 0x300000, 0x30000C }, "18f6390.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F63J90", { "pic18f63j90", "p18f63j90", "18f63j90" }, 0xb390, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x1FF8, 0x1FFD },     "18f63j90.lkr" },
  { PROC_CLASS_PIC16E,  "__18F6393",  { "pic18f6393",  "p18f6393",  "18f6393"  }, 0x6393, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x300000, 0x30000C }, "18f6393.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F6410",  { "pic18f6410",  "p18f6410",  "18f6410"  }, 0x6410, 0, 0x60, 0x3fff,   { -1, -1 },            { 0x300000, 0x30000C }, "18f6410.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F64J11", { "pic18f64j11", "p18f64j11", "18f64j11" }, 0x6411, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x3FF8, 0x3FFD },     "18f64j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18F64J15", { "pic18f64j15", "p18f64j15", "18f64j15" }, 0xb415, 0, 0x60, -1,       { -1, -1 },            { -1, -1 },             "18f64j15.lkr" },
  { PROC_CLASS_PIC16E,  "__18F6490",  { "pic18f6490",  "p18f6490",  "18f6490"  }, 0x6490, 0, 0x60, 0x3fff,   { -1, -1 },            { 0x300000, 0x30000C }, "18f6490.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F64J90", { "pic18f64j90", "p18f64j90", "18f64j90" }, 0xb490, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x3FF8, 0x3FFD },     "18f64j90.lkr" },
  { PROC_CLASS_PIC16E,  "__18F6493",  { "pic18f6493",  "p18f6493",  "18f6493"  }, 0x6493, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x300000, 0x30000C }, "18f6493.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F65J10", { "pic18f65j10", "p18f65j10", "18f65j10" }, 0xb510, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFD },     "18f65j10.lkr" },
  { PROC_CLASS_PIC16E,  "__18F65J11", { "pic18f65j11", "p18f65j11", "18f65j11" }, 0x6511, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x7FF8, 0x7FFD },     "18f65j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18F65J15", { "pic18f65j15", "p18f65j15", "18f65j15" }, 0xb515, 0, 0x60, 0xbff7,   { -1, -1 },            { 0xBFF8, 0xBFFD },     "18f65j15.lkr" },
  { PROC_CLASS_PIC16E,  "__18F6520",  { "pic18f6520",  "p18f6520",  "18f6520"  }, 0xa652, 0, 0x60, 0xf003ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f6520.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F6525",  { "pic18f6525",  "p18f6525",  "18f6525"  }, 0x6525, 0, 0x60, 0xf003ff, { 0xc000, 0xefffff },  { 0x300000, 0x30000D }, "18f6525.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F6527",  { "pic18f6527",  "p18f6527",  "18f6527"  }, 0x6527, 0, 0x60, 0xf003ff, { 0xc000, 0xefffff },  { 0x300000, 0x30000D }, "18f6527.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F65J50", { "pic18f65j50", "p18f65j50", "18f65j50" }, 0xb550, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFD },     "18f65j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18C658",   { "pic18c658",   "p18c658",   "18c658"   }, 0x8658, 0, 0x60, 0x7fff,   { -1, -1 },            { 0x300000, 0x300006 }, "18c658.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F6585",  { "pic18f6585",  "p18f6585",  "18f6585"  }, 0x6585, 0, 0x60, 0xf003ff, { 0xc000, 0xefffff },  { 0x300000, 0x30000D }, "18f6585.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F65J90", { "pic18f65j90", "p18f65j90", "18f65j90" }, 0xb590, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x7FF8, 0x7FFD },     "18f65j90.lkr" },
  { PROC_CLASS_PIC16E,  "__18F66J10", { "pic18f66j10", "p18f66j10", "18f66j10" }, 0xb610, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f66j10.lkr" },
  { PROC_CLASS_PIC16E,  "__18F66J11", { "pic18f66j11", "p18f66j11", "18f66j11" }, 0xb611, 0, 0x60, 0x1fffff, { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f66j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18F66J15", { "pic18f66j15", "p18f66j15", "18f66j15" }, 0xb615, 0, 0x60, 0x17ff7,  { -1, -1 },            { 0x17FF8, 0x17FFD },   "18f66j15.lkr" },
  { PROC_CLASS_PIC16E,  "__18F66J16", { "pic18f66j16", "p18f66j16", "18f66j16" }, 0xb616, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x17FF8, 0x17FFD },   "18f66j16.lkr" },
  { PROC_CLASS_PIC16E,  "__18F6620",  { "pic18f6620",  "p18f6620",  "18f6620"  }, 0xa662, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f6620.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F6621",  { "pic18f6621",  "p18f6621",  "18f6621"  }, 0xa621, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f6621.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F6622",  { "pic18f6622",  "p18f6622",  "18f6622"  }, 0xf622, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f6622.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F6627",  { "pic18f6627",  "p18f6627",  "18f6627"  }, 0xf625, 0, 0x60, 0xf003ff, { 0x18000, 0xefffff }, { 0x300000, 0x30000D }, "18f6627.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F6628",  { "pic18f6628",  "p18f6628",  "18f6628"  }, 0xa628, 0, 0x60, 0xf003ff, { 0x18000, 0xefffff }, { 0x300000, 0x30000D }, "18f6628.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F66J50", { "pic18f66j50", "p18f66j50", "18f66j50" }, 0xb650, 0, 0x60, 0x1fffff, { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f66j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18F66J55", { "pic18f66j55", "p18f66j55", "18f66j55" }, 0xb655, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x17FF8, 0x17FFD },   "18f66j55.lkr" },
  { PROC_CLASS_PIC16E,  "__18F66J60", { "pic18f66j60", "p18f66j60", "18f66j60" }, 0xb660, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f66j60.lkr" },
  { PROC_CLASS_PIC16E,  "__18F66J65", { "pic18f66j65", "p18f66j65", "18f66j65" }, 0xb665, 0, 0x60, 0x17ff7,  { -1, -1 },            { 0x17FF8, 0x17FFD },   "18f66j65.lkr" },
  { PROC_CLASS_PIC16E,  "__18F6680",  { "pic18f6680",  "p18f6680",  "18f6680"  }, 0x6680, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f6680.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F66J90", { "pic18f66j90", "p18f66j90", "18f66j90" }, 0xb690, 0, 0x60, 0x1fffff, { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f66j90.lkr" },
  { PROC_CLASS_PIC16E,  "__18F67J10", { "pic18f67j10", "p18f67j10", "18f67j10" }, 0xb710, 0, 0x60, 0x1fff7,  { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f67j10.lkr" },
  { PROC_CLASS_PIC16E,  "__18F67J11", { "pic18f67j11", "p18f67j11", "18f67j11" }, 0xb711, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f67j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18F6720",  { "pic18f6720",  "p18f6720",  "18f6720"  }, 0xa672, 0, 0x60, 0xf003ff, { 0x20000, 0xefffff }, { 0x300000, 0x30000D }, "18f6720.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F6722",  { "pic18f6722",  "p18f6722",  "18f6722"  }, 0x6721, 0, 0x60, 0xf003ff, { 0x20000, 0xefffff }, { 0x300000, 0x30000D }, "18f6722.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F6723",  { "pic18f6723",  "p18f6723",  "18f6723"  }, 0x6723, 0, 0x60, 0xf003ff, { 0x20000, 0xefffff }, { 0x300000, 0x30000D }, "18f6723.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F67J50", { "pic18f67j50", "p18f67j50", "18f67j50" }, 0xb750, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f67j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18F67J60", { "pic18f67j60", "p18f67j60", "18f67j60" }, 0xb760, 0, 0x60, 0x1fff7,  { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f67j60.lkr" },
  { PROC_CLASS_PIC16E,  "__18F67J90", { "pic18f67j90", "p18f67j90", "18f67j90" }, 0x6790, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f67j90.lkr" },
  { PROC_CLASS_PIC16E,  "__18C801",   { "pic18c801",   "p18c801",   "18c801"   }, 0x8801, 0, 0x80, 0x1fffff, { -1, -1 },            { 0x300000, 0x300006 }, "18c801.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F8310",  { "pic18f8310",  "p18f8310",  "18f8310"  }, 0x8310, 0, 0x60, 0x1fff,   { -1, -1 },            { 0x300000, 0x30000C }, "18f8310.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F83J11", { "pic18f83j11", "p18f83j11", "18f83j11" }, 0x8311, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x1FF8, 0x1FFD },     "18f83j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18F8390",  { "pic18f8390",  "p18f8390",  "18f8390"  }, 0x8390, 0, 0x60, 0x1fff,   { -1, -1 },            { 0x300000, 0x30000C }, "18f8390.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F83J90", { "pic18f83j90", "p18f83j90", "18f83j90" }, 0xc390, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x1FF8, 0x1FFD },     "18f83j90.lkr" },
  { PROC_CLASS_PIC16E,  "__18F8393",  { "pic18f8393",  "p18f8393",  "18f8393"  }, 0x8393, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x300000, 0x30000C }, "18f8393.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F8410",  { "pic18f8410",  "p18f8410",  "18f8410"  }, 0x8410, 0, 0x60, 0x3fff,   { -1, -1 },            { 0x300000, 0x30000C }, "18f8410.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F84J11", { "pic18f84j11", "p18f84j11", "18f84j11" }, 0x8411, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x3FF8, 0x3FFD },     "18f84j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18F84J15", { "pic18f84j15", "p18f84j15", "18f84j15" }, 0xc415, 0, 0x60, -1,       { -1, -1 },            { -1, -1 },             "18f84j15.lkr" },
  { PROC_CLASS_PIC16E,  "__18F8490",  { "pic18f8490",  "p18f8490",  "18f8490"  }, 0x8490, 0, 0x60, 0x3fff,   { -1, -1 },            { 0x300000, 0x30000C }, "18f8490.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F84J90", { "pic18f84j90", "p18f84j90", "18f84j90" }, 0xc490, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x3FF8, 0x3FFD },     "18f84j90.lkr" },
  { PROC_CLASS_PIC16E,  "__18F8493",  { "pic18f8493",  "p18f8493",  "18f8493"  }, 0x8493, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x300000, 0x30000C }, "18f8493.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F85J10", { "pic18f85j10", "p18f85j10", "18f85j10" }, 0xc510, 0, 0x60, 0x7ff7,   { -1, -1 },            { 0x7FF8, 0x7FFD },     "18f85j10.lkr" },
  { PROC_CLASS_PIC16E,  "__18F85J11", { "pic18f85j11", "p18f85j11", "18f85j11" }, 0x8511, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x7FF8, 0x7FFD },     "18f85j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18F85J15", { "pic18f85j15", "p18f85j15", "18f85j15" }, 0xc515, 0, 0x60, 0xbff7,   { -1, -1 },            { 0xBFF8, 0xBFFD },     "18f85j15.lkr" },
  { PROC_CLASS_PIC16E,  "__18F8520",  { "pic18f8520",  "p18f8520",  "18f8520"  }, 0xa852, 0, 0x60, 0xf003ff, { 0x8000, 0xefffff },  { 0x300000, 0x30000D }, "18f8520.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F8525",  { "pic18f8525",  "p18f8525",  "18f8525"  }, 0x8525, 0, 0x60, 0xf003ff, { 0xc000, 0xefffff },  { 0x300000, 0x30000D }, "18f8525.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F8527",  { "pic18f8527",  "p18f8527",  "18f8527"  }, 0x8527, 0, 0x60, 0xf003ff, { 0x200000, 0xefffff },{ 0x300000, 0x30000D }, "18f8527.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F85J50", { "pic18f85j50", "p18f85j50", "18f85j50" }, 0xc550, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x7FF8, 0x7FFD },     "18f85j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18C858",   { "pic18c858",   "p18c858",   "18c858"   }, 0x8858, 0, 0x60, 0x7fff,   { -1, -1 },            { 0x300000, 0x300006 }, "18c858.lkr"   },
  { PROC_CLASS_PIC16E,  "__18F8585",  { "pic18f8585",  "p18f8585",  "18f8585"  }, 0x8585, 0, 0x60, 0xf003ff, { 0xc000, 0xefffff },  { 0x300000, 0x30000D }, "18f8585.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F85J90", { "pic18f85j90", "p18f85j90", "18f85j90" }, 0xc590, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x7FF8, 0x7FFD },     "18f85j90.lkr" },
  { PROC_CLASS_PIC16E,  "__18F86J10", { "pic18f86j10", "p18f86j10", "18f86j10" }, 0xc610, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f86j10.lkr" },
  { PROC_CLASS_PIC16E,  "__18F86J11", { "pic18f86j11", "p18f86j11", "18f86j11" }, 0xc611, 0, 0x60, 0x1fffff, { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f86j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18F86J15", { "pic18f86j15", "p18f86j15", "18f86j15" }, 0xc615, 0, 0x60, 0x17ff7,  { -1, -1 },            { 0x17FF8, 0x17FFD },   "18f86j15.lkr" },
  { PROC_CLASS_PIC16E,  "__18F86J16", { "pic18f86j16", "p18f86j16", "18f86j16" }, 0xc616, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x17FF8, 0x17FFD },   "18f86j16.lkr" },
  { PROC_CLASS_PIC16E,  "__18F8620",  { "pic18f8620",  "p18f8620",  "18f8620"  }, 0xa862, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f8620.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F8621",  { "pic18f8621",  "p18f8621",  "18f8621"  }, 0x8621, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f8621.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F8622",  { "pic18f8622",  "p18f8622",  "18f8622"  }, 0x8622, 0, 0x60, 0xf003ff, { 0x200000, 0xefffff },{ 0x300000, 0x30000D }, "18f8622.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F8627",  { "pic18f8627",  "p18f8627",  "18f8627"  }, 0x8625, 0, 0x60, 0xf003ff, { 0x18000, 0xefffff }, { 0x300000, 0x30000D }, "18f8627.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F8628",  { "pic18f8628",  "p18f8628",  "18f8628"  }, 0x8628, 0, 0x60, 0xf003ff, { 0x200000, 0xefffff },{ 0x300000, 0x30000D }, "18f8628.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F86J50", { "pic18f86j50", "p18f86j50", "18f86j50" }, 0xc650, 0, 0x60, 0x1fffff, { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f86j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18F86J55", { "pic18f86j55", "p18f86j55", "18f86j55" }, 0xc655, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x17FF8, 0x17FFD },   "18f86j55.lkr" },
  { PROC_CLASS_PIC16E,  "__18F86J60", { "pic18f86j60", "p18f86j60", "18f86j60" }, 0xc660, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f86j60.lkr" },
  { PROC_CLASS_PIC16E,  "__18F86J65", { "pic18f86j65", "p18f86j65", "18f86j65" }, 0xc665, 0, 0x60, 0x17ff7,  { -1, -1 },            { 0x17FF8, 0x17FFD },   "18f86j65.lkr" },
  { PROC_CLASS_PIC16E,  "__18F8680",  { "pic18f8680",  "p18f8680",  "18f8680"  }, 0x8680, 0, 0x60, 0xf003ff, { 0x10000, 0xefffff }, { 0x300000, 0x30000D }, "18f8680.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F86J90", { "pic18f86j90", "p18f86j90", "18f86j90" }, 0x8690, 0, 0x60, 0x1fffff, { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f86j90.lkr" },
  { PROC_CLASS_PIC16E,  "__18F87J10", { "pic18f87j10", "p18f87j10", "18f87j10" }, 0xc710, 0, 0x60, 0x1fff7,  { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f87j10.lkr" },
  { PROC_CLASS_PIC16E,  "__18F87J11", { "pic18f87j11", "p18f87j11", "18f87j11" }, 0xc711, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f87j11.lkr" },
  { PROC_CLASS_PIC16E,  "__18F8720",  { "pic18f8720",  "p18f8720",  "18f8720"  }, 0xa872, 0, 0x60, 0xf003ff, { 0x20000, 0xefffff }, { 0x300000, 0x30000D }, "18f8720.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F8722",  { "pic18f8722",  "p18f8722",  "18f8722"  }, 0x8721, 0, 0x60, 0xf003ff, { 0x20000, 0xefffff }, { 0x300000, 0x30000D }, "18f8722.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F8723",  { "pic18f8723",  "p18f8723",  "18f8723"  }, 0x8723, 0, 0x60, 0xf003ff, { 0x200000, 0xefffff },{ 0x300000, 0x30000D }, "18f8723.lkr"  },
  { PROC_CLASS_PIC16E,  "__18F87J50", { "pic18f87j50", "p18f87j50", "18f87j50" }, 0xc750, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f87j50.lkr" },
  { PROC_CLASS_PIC16E,  "__18F87J60", { "pic18f87j60", "p18f87j60", "18f87j60" }, 0xc760, 0, 0x60, 0x1fff7,  { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f87j60.lkr" },
  { PROC_CLASS_PIC16E,  "__18F87J90", { "pic18f87j90", "p18f87j90", "18f87j90" }, 0x8790, 0, 0x60, 0x1fffff, { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f87j90.lkr" },
  { PROC_CLASS_PIC16E,  "__18F96J60", { "pic18f96j60", "p18f96j60", "18f96j60" }, 0xd660, 0, 0x60, 0xfff7,   { -1, -1 },            { 0xFFF8, 0xFFFD },     "18f96j60.lkr" },
  { PROC_CLASS_PIC16E,  "__18F96J65", { "pic18f96j65", "p18f96j65", "18f96j65" }, 0xd665, 0, 0x60, 0x17ff7,  { -1, -1 },            { 0x17FF8, 0x17FFD },   "18f96j65.lkr" },
  { PROC_CLASS_PIC16E,  "__18F97J60", { "pic18f97j60", "p18f97j60", "18f97j60" }, 0xd760, 0, 0x60, 0x1fff7,  { -1, -1 },            { 0x1FFF8, 0x1FFFD },   "18f97j60.lkr" },
  { PROC_CLASS_PIC16E,  "__PS500",    { "ps500",       "ps500",     "ps500"    }, 0x0500, 0, 0x80, 0xf000ff, { 0x4000, 0xefffff },  { 0x300000, 0x30000D }, "ps500.lkr"    },
  { PROC_CLASS_PIC16E,  "__PS810",    { "ps810",       "ps810",     "ps810"    }, 0x0810, 0, 0x80, 0x1fff,   { -1, -1 },            { 0x300000, 0x30000D }, "ps810.lkr"    },
  { PROC_CLASS_PIC12,   "__RF509AF",  { "rf509af",     "rf509af",   "rf509af"  }, 0x6509, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "rf509af.lkr"  },
  { PROC_CLASS_PIC12,   "__RF509AG",  { "rf509ag",     "rf509ag",   "rf509ag"  }, 0x7509, 2, 4, 0x3ff,    { -1, -1 },            { 0xFFF, 0xFFF },       "rf509ag.lkr"  },
  { PROC_CLASS_PIC14,   "__RF675F",   { "rf675f",      "rf675f",    "rf675f"   }, 0x3675, 1, 2, 0x3fe,    { -1, -1 },            { 0x2007, 0x2007 },     "rf675f.lkr"   },
  { PROC_CLASS_PIC14,   "__RF675H",   { "rf675h",      "rf675h",    "rf675h"   }, 0x4675, 1, 2, 0x3fe,    { -1, -1 },            { 0x2007, 0x2007 },     "rf675h.lkr"   },
  { PROC_CLASS_PIC14,   "__RF675K",   { "rf675k",      "rf675k",    "rf675k"   }, 0x5675, 1, 2, 0x3fe,    { -1, -1 },            { 0x2007, 0x2007 },     "rf675k.lkr"   },
  { PROC_CLASS_SX,      "__SX18",     { "sx18ac",      "sx18",      "sx18"     }, 0x0018, 0, 0, 0x7ff,    { -1, -1 },            { -1, -1 },             NULL           },
  { PROC_CLASS_SX,      "__SX20",     { "sx20ac",      "sx20",      "sx20"     }, 0x0020, 0, 0, 0x7ff,    { -1, -1 },            { -1, -1 },             NULL           },
  { PROC_CLASS_SX,      "__SX28",     { "sx28ac",      "sx28",      "sx28"     }, 0x0028, 0, 0, 0x7ff,    { -1, -1 },            { -1, -1 },             NULL           },
  { PROC_CLASS_SX,      "__SX48",     { "sx48bd",      "sx48",      "sx48"     }, 0x0048, 0, 0, 0xfff,    { -1, -1 },            { -1, -1 },             NULL           },
  { PROC_CLASS_SX,      "__SX52",     { "sx52bd",      "sx52",      "sx52"     }, 0x0052, 0, 0, 0xfff,    { -1, -1 },            { -1, -1 },             NULL           },
};

#define NUM_PICS	(sizeof(pics) / sizeof(pics[0]))

/*
 * Display a list of the processor names
 */
void gp_dump_processor_list(gp_boolean list_all, proc_class_t class)
{
#define COLUMNS  6
#define SPACE_BETWEEN 2   /* number of chars between columns */
#define FAVORITE 1        /* there are 3 names to choose from */

  int i;
  int j;
  int length;
  int num = 0;
  int longest = 0;
  
  for(i = 0; i < NUM_PICS; i++) {
    length = strlen(pics[i].names[FAVORITE]);
    if (length > longest)
      longest = length;
  }

  for(i = 0; i < NUM_PICS; i++) {
    if (list_all || (pics[i].class == class)) { 
      num++;
      printf("%s", pics[i].names[FAVORITE]);
      length = longest + SPACE_BETWEEN - strlen(pics[i].names[FAVORITE]);
      for(j = 0; j < length; j++) {
        putchar(' ');
      }
      if ((num % COLUMNS) == 0) {
        putchar('\n');
      }
    }
  }

  if ((num % COLUMNS) != 0) {
    putchar('\n');
  }

}

const struct px *
gp_find_processor(const char *name)
{
  int i, j;

  for (i = 0; i < NUM_PICS; i++) {
    for (j = 0; (j < MAX_NAMES) && (pics[i].names[j] != NULL); j++) {
      if (strcasecmp(name, pics[i].names[j]) == 0) {
        return &pics[i];
      }
    }
  }

  return NULL;
}

proc_class_t
gp_processor_class(pic_processor_t processor)
{
  if (processor)
    return processor->class;
  return PROC_CLASS_UNKNOWN;
}

/* 18xx bsr boundary location */

int
gp_processor_bsr_boundary(pic_processor_t processor)
{
  if (processor && processor->class == PROC_CLASS_PIC16E)
    return processor->num_banks;
  return 0;
}

unsigned long
gp_processor_coff_type(pic_processor_t processor)
{
  if (processor)
    return processor->coff_type;
  return 0;
}

int
gp_processor_num_pages(pic_processor_t processor)
{
  if (processor)
    return processor->num_pages;
  return 0;
}

int
gp_processor_num_banks(pic_processor_t processor)
{
  if (processor && processor->class != PROC_CLASS_PIC16E)
    return processor->num_banks;
  return 0;
}

pic_processor_t
gp_processor_coff_proc(unsigned long coff_type)
{
  int i;
  pic_processor_t processor = no_processor;

  for (i = 0; i < NUM_PICS; i++) {
    if (pics[i].coff_type == coff_type) {
      processor = &pics[i];
      break;
    }
  }

  return processor;
}

const char *
gp_processor_name(pic_processor_t processor, unsigned int choice)
{
  assert(!(choice > MAX_NAMES - 1));

  if (processor)
    return processor->names[choice];

  return NULL;
}

const char *
gp_processor_coff_name(unsigned long coff_type, unsigned int choice)
{
  int i;

  if (coff_type == 0)
    return NULL;

  assert(!(choice > MAX_NAMES - 1));

  for (i = 0; i < NUM_PICS; i++) {
    if (pics[i].coff_type == coff_type) {
      return pics[i].names[choice];
    }
  }

  return NULL;
}

const char *
gp_processor_script(pic_processor_t processor)
{
  if (processor)
    return processor->script;
  return NULL;
}

unsigned int
gp_processor_id_location(pic_processor_t processor)
{
  if (processor->class->id_location)
    return processor->class->id_location(processor);
  return 0;
}

int
gp_processor_rom_width(proc_class_t class)
{
  assert(class->rom_width > 0);
  return class->rom_width;
}

/* Set the page bits, return the number of instructions required. */

int 
gp_processor_set_page(proc_class_t class, 
                      int num_pages,
                      int page,
                      MemBlock *m, 
                      unsigned int address,
                      int use_wreg)
{
  return class->set_page(num_pages, page, m, address, use_wreg);
}

/* Set the bank bits, return the number of instructions required. */

int
gp_processor_set_bank(proc_class_t class,
                      int num_banks,
                      int bank,
                      MemBlock *m,
                      unsigned int address)
{
  return class->set_bank(num_banks, bank, m, address);
}

/* determine the value for retlw */

int 
gp_processor_retlw(proc_class_t class)
{
  assert(class->retlw >= 0);
  return class->retlw;
}

int
gp_processor_org_to_byte(proc_class_t class, int org)
{
  /* FIXME: In some places we use this value before we know what the
     processor is. Rather than fix those now, I'll just return some
     value. */
  if (!class)
    return org;

  return org << class->org_to_byte_shift;
}

int
gp_processor_byte_to_org(proc_class_t class, int byte)
{
  /* FIXME: In some places we use this value before we know what the
     processor is. Rather than fix those now, I'll just return some
     value. */
  if (!class)
    return byte;

  return byte >> class->org_to_byte_shift;
}

int
gp_org_to_byte(unsigned shift, int org)
{
  return org << shift;
}

int
gp_byte_to_org(unsigned shift, int byte)
{
  return byte >> shift;
}

/* determine which page of program memory the byte address is located */
int
gp_processor_check_page(proc_class_t class, unsigned int org)
{
  return class->check_page(org);
}

/* determine which bank of data memory the address is located */
int 
gp_processor_check_bank(proc_class_t class, unsigned int address)
{
  return class->check_bank(address);
}

/* When unsupported on the class */
static int
gp_processor_check_page_unsupported(unsigned int org)
{
  assert(0);
  return 0;
}

static int
gp_processor_check_bank_unsupported(unsigned int address)
{
  assert(0);
  return 0;
}

static int
gp_processor_set_page_unsupported(int num_pages,
				  int page,
				  MemBlock *m, 
				  unsigned int address,
				  int use_wreg)
{
    assert(0);
    return 0;
}

static int
gp_processor_set_bank_unsupported(int num_banks,
				  int bank,
				  MemBlock *m, 
				  unsigned int address)
{
  assert(0);
  return 0;
}

static int reloc_unsupported(unsigned int address)
{
  assert(0);
  return 0;
}

/* Common to most */

static const struct insn *
find_insn_generic(proc_class_t cls, long int opcode)
{
  const struct insn *base = cls->instructions;
  int count = base == NULL ? 0 : *cls->num_instructions;
  int i;
  for(i = 0; i < count; i++) {
    if((base[i].mask & opcode) == base[i].opcode) {
      return &base[i];
    }
  }
  return NULL;
}

/* Common to PIC12 and PIC14 */

static int
gp_processor_set_page_pic12_14(int num_pages,
			       int page,
			       MemBlock *m, 
			       unsigned int address,
			       int use_wreg,
			       int bcf_insn,
			       int bsf_insn,
			       int movlw_insn,
			       int movwf_insn,
			       int location,
			       int page0,
			       int page1)
{
  unsigned int data;
  int count = 4;
  if (num_pages == 1) {
    return 0;
  }

  if (use_wreg) {
    data = movlw_insn | page;
    i_memory_put_le(m, address, data);
    data = movwf_insn | location;
    i_memory_put_le(m, address + 2, data);
  }
  else {
    /* page low bit */
    data = (page & 1 ? bsf_insn : bcf_insn) | page0 | location;
    i_memory_put_le(m, address, data);

    /* page high bit */
    if (num_pages == 4) {
      data = (page & 2 ? bsf_insn : bcf_insn) | page1 | location;
      i_memory_put_le(m, address + 2, data);
    }

    if (num_pages == 2) {
      count = 2;
    } else {
      count = 4;
    }
  }
  return count;
}

static int
gp_processor_set_bank_pic12_14(int num_banks,
			       int bank,
			       MemBlock *m, 
			       unsigned int address,
			       int bcf_insn,
			       int bsf_insn,
			       int location,
			       int bank0,
			       int bank1)
{
  unsigned int data;
  int count;
  if (num_banks == 1) {
    return 0;
  }
  
  switch(bank) {
  case 0:
    /* bcf location, bank0 */
    data = bcf_insn | bank0 | location;
    i_memory_put_le(m, address, data);
    if (num_banks == 4) {
      /* bcf location, bank1 */
      data = bcf_insn | bank1 | location;
      i_memory_put_le(m, address + 2, data);
    }
    break;
  case 1:
    /* bsf location, bank0 */
    data = bsf_insn | bank0 | location;
    i_memory_put_le(m, address, data);
    if (num_banks == 4) {
      /* bcf location, bank1 */
      data = bcf_insn | bank1 | location;
      i_memory_put_le(m, address + 2, data);
    }
    break;
  case 2:
    /* bcf location, bank0 */
    data = bcf_insn | bank0 | location;
    i_memory_put_le(m, address, data);
    if (num_banks == 4) {
      /* bsf location, bank1 */
      data = bsf_insn | bank1 | location;
      i_memory_put_le(m, address + 2, data);
    }
    break;
  case 3:
    /* bsf location, bank0 */
    data = bsf_insn | bank0 | location;
    i_memory_put_le(m, address, data);
    if (num_banks == 4) {
      /* bsf location, bank1 */
      data = bsf_insn | bank1 | location;
      i_memory_put_le(m, address + 2, data);
    }
    break;
  default:
    assert(0);
    break;  
  }

  if (num_banks == 2) {
    count = 2;
  } else {
    count = 4;
  }

  return count;
}

/* PIC12 */

static unsigned int
id_location_pic12(const struct px *processor)
{
  if (processor->maxrom > 0) {
    /* Sometimes the last hex digit is e due to some calibration
       instruction or something. Therefore we look for the next
       multiple of 16. */
    /* We carry org in the struct px, but return byte address. */
    return ((processor->maxrom + 0xf) & ~0xf) << 1;
  }
  return 0;
}

static int
gp_processor_check_page_pic12(unsigned int org)
{
  return (org >> 9) & 0x3;
}

static int
gp_processor_check_bank_pic12(unsigned int address)
{
  return (address >> 5) & 0x3;
}

static int
gp_processor_set_page_pic12(int num_pages,
			    int page,
			    MemBlock *m, 
			    unsigned int address,
			    int use_wreg)

{
  return gp_processor_set_page_pic12_14(num_pages,
					page,
					m, 
					address,
					use_wreg,
					0x400,
					0x500,
					0xc00,
					0x020,
					0x3,
					5 << 5,
					6 << 5);
}

static int
gp_processor_set_bank_pic12(int num_banks,
			    int bank,
			    MemBlock *m, 
			    unsigned int address)
{
  return gp_processor_set_bank_pic12_14(num_banks, bank, m, address,
					0x400,
					0x500,
					0x4,
					5 << 5,
					6 << 5);
}

static int reloc_call_pic12(unsigned int org)
{
  return org & 0xff;
}
static int reloc_goto_pic12(unsigned int org)
{
  return org & 0x1ff;
}
static int reloc_f_pic12(unsigned int address)
{
  return address & 0x1f;
}
static int reloc_tris_pic12(unsigned int address)
{
  /* TODO This is not accurate, for example PIC12F510/16F506 only has
     three bits and allowed values of 6 and 7. MPASM 5.34 has
     Error[126]  : Argument out of range (0000 not between 0005 and 0009)
  */
  return address & 0x1f;
}

/* PIC14 */

static unsigned int
id_location_pic14(const struct px *processor)
{
  if (processor->config_addrs[0] > 0)
    /* I only know of 0x2007 to 0x2000 and 0x8007 to 0x8000 and this
       returns correct for both. */
    /* We carry org in the struct px, but return byte address. */
    return (processor->config_addrs[0] & ~0xFFF) << 1;
  return 0;
}

static int
gp_processor_check_page_pic14(unsigned int org)
{
  return (org >> 11) & 0x3;
}

static int
gp_processor_check_bank_pic14(unsigned int address)
{
  return (address >> 7) & 0x3;
}

static int
gp_processor_set_page_pic14(int num_pages,
			    int page,
			    MemBlock *m, 
			    unsigned int address,
			    int use_wreg)
{
  return gp_processor_set_page_pic12_14(num_pages,
					page,
					m, 
					address,
					use_wreg,
					0x1000,
					0x1400,
					0x3000,
					0x0080,
					0xa,
					3 << 7,
					4 << 7);
}

static int
gp_processor_set_bank_pic14(int num_banks,
			    int bank,
			    MemBlock *m, 
			    unsigned int address)
{
  return gp_processor_set_bank_pic12_14(num_banks, bank, m, address,
					0x1000,
					0x1400,
					0x3,
					5 << 7,
					6 << 7);
}

static int reloc_call_pic14(unsigned int org)
{
  return org & 0x7ff;
}
static int reloc_goto_pic14(unsigned int org)
{
  return org & 0x7ff;
}
static int reloc_ibanksel_pic14(unsigned int address)
{
  if (address < 0x100) {
    /* bcf 0x3, 0x7 */
    return 0x1383;
  } else {
    /* bsf 0x3, 0x7 */
    return 0x1783;
  }
}
static int reloc_f_pic14(unsigned int address)
{
  return address & 0x7f;
}
static int reloc_tris_pic14(unsigned int address)
{
  return address & 0x7f;
}

/* PIC16 */

static int
gp_processor_check_page_pic16(unsigned int org)
{
  return (org >> 8) & 0xff;
}


static int
gp_processor_check_bank_pic16(unsigned int address)
{
  if ((address & 0xFF) < 0x20)
    return (address >> 8) & 0xff;
  else
    /* 0x200 turns MOVLB to MOVLR for setting GPR RAM bank in
       set_bank */
    return 0x200 + ((address >> 8) & 0xff);
}


static int
gp_processor_set_page_pic16(int num_pages,
			    int page,
			    MemBlock *m, 
			    unsigned int address,
			    int use_wreg)
{
  unsigned int data;
  /* movlw <page> */
  data = 0xb000 | (page & 0xff);
  i_memory_put_le(m, address, data);
  /* movwf 0x3 */
  data = 0x0100 | 0x3;
  i_memory_put_le(m, address + 2, data);
  return 4;
}

static int
gp_processor_set_bank_pic16(int num_banks,
			    int bank,
			    MemBlock *m, 
			    unsigned int address)
{
  /* movlb bank */
  i_memory_put_le(m, address,
		  0xb800 | bank);
  return 2;
}

static int reloc_call_pic16(unsigned int org)
{
  return org & 0x1fff;
}
static int reloc_goto_pic16(unsigned int org)
{
  return org & 0x1fff;
}
static int reloc_ibanksel_pic16(unsigned int address)
{
  return 0xb800 | gp_processor_check_bank_pic16(address);
}
static int reloc_f_pic16(unsigned int address)
{
  return address & 0xff;
}

/* PIC16E */

static unsigned int
id_location_pic16e(const struct px *processor)
{
  return IDLOC0;
}

static int
gp_processor_set_bank_pic16e(int num_banks,
			     int bank,
			     MemBlock *m, 
			     unsigned int address)
{
  /* movlb bank */
  i_memory_put_le(m, address,
		  0x0100 | (bank & 0xff));
  return 2;
}

static int reloc_call_pic16e(unsigned int org)
{
  return (org >> 1) & 0xff;
}
static int reloc_goto_pic16e(unsigned int org)
{
  return (org >> 1) & 0xff;
}

static const struct insn *
find_insn_pic16e(proc_class_t cls, long int opcode)
{
  int i;
  if (gp_decode_mnemonics) {
    for(i = 0; i < num_op_18cxx_sp; i++) {
      if((op_18cxx_sp[i].mask & opcode) == op_18cxx_sp[i].opcode) {
	return &op_18cxx_sp[i];
      }
    }
  }
  for(i = 0; i < num_op_18cxx; i++) {
    if((op_18cxx[i].mask & opcode) == op_18cxx[i].opcode) {
      return &op_18cxx[i];
    }
  }
  if (gp_decode_extended) {
    /* might be from the extended instruction set */
    for(i = 0; i < num_op_18cxx_ext; i++) {
      if((op_18cxx_ext[i].mask & opcode) == op_18cxx_ext[i].opcode) {
	return &op_18cxx_ext[i];
      }
    }
  }
  return NULL;
}

const struct proc_class proc_class_eeprom8 = {
  -1,					/* retlw */
  8,					/* rom_width */
  0,					/* org_to_byte_shift */
  0,					/* bank_mask */
  (1<<8)-1,				/* core_size */
  0,					/* id_location */
  gp_processor_check_bank_unsupported,
  gp_processor_set_bank_unsupported,
  gp_processor_check_page_unsupported,
  gp_processor_set_page_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  NULL, NULL,
  NULL,
  i_memory_get_le, i_memory_put_le,
};

const struct proc_class proc_class_eeprom16 = {
  -1,					/* retlw */
  16,					/* rom_width */
  1,					/* org_to_byte_shift */
  0,					/* bank_mask */
  (1<<16)-1,				/* core_size */
  0,					/* id_location */
  gp_processor_check_bank_unsupported,
  gp_processor_set_bank_unsupported,
  gp_processor_check_page_unsupported,
  gp_processor_set_page_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  NULL, NULL,
  NULL,
  i_memory_get_be, i_memory_put_be,
};

const struct proc_class proc_class_generic = {
  -1,					/* retlw */
  12,					/* rom_width */
  1,					/* org_to_byte_shift */
  ~0x1fu,				/* bank_mask */
  (1<<12)-1,				/* core_size */
  id_location_pic12,			/* id_location */
  gp_processor_check_bank_pic12,
  gp_processor_set_bank_pic12,
  gp_processor_check_page_pic12,
  gp_processor_set_page_pic12,
  reloc_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  reloc_unsupported,
  NULL, NULL,
  NULL,
  i_memory_get_le, i_memory_put_le,
};

const struct proc_class proc_class_pic12 = {
  0x800,				/* retlw */
  12,					/* rom_width */
  1,					/* org_to_byte_shift */
  ~0x1fu,				/* bank_mask */
  (1<<12)-1,				/* core_size */
  id_location_pic12,			/* id_location */
  gp_processor_check_bank_pic12,
  gp_processor_set_bank_pic12,
  gp_processor_check_page_pic12,
  gp_processor_set_page_pic12,
  reloc_call_pic12,
  reloc_goto_pic12,
  reloc_unsupported,
  reloc_f_pic12,
  reloc_tris_pic12,
  op_12c5xx, &num_op_12c5xx,
  find_insn_generic,
  i_memory_get_le, i_memory_put_le,
};

const struct proc_class proc_class_sx = {
  0x800,				/* retlw */
  12,					/* rom_width */
  1,					/* org_to_byte_shift */
  ~0x1fu,				/* bank_mask */
  (1<<12)-1,				/* core_size */
  id_location_pic12,			/* id_location */
  gp_processor_check_bank_pic12,
  gp_processor_set_bank_pic12,
  gp_processor_check_page_pic12,
  gp_processor_set_page_pic12,
  reloc_call_pic12,
  reloc_goto_pic12,
  reloc_unsupported,
  reloc_f_pic12,
  reloc_tris_pic12,
  op_sx, &num_op_sx,
  find_insn_generic,
  i_memory_get_le, i_memory_put_le,
};

const struct proc_class proc_class_pic14 = {
  0x3400,				/* retlw */
  14,					/* rom_width */
  1,					/* org_to_byte_shift */
  ~0x7fu,				/* bank_mask */
  (1<<14)-1,				/* core_size */
  id_location_pic14,			/* id_location */
  gp_processor_check_bank_pic14,
  gp_processor_set_bank_pic14,
  gp_processor_check_page_pic14,
  gp_processor_set_page_pic14,
  reloc_call_pic14,
  reloc_goto_pic14,
  reloc_ibanksel_pic14,
  reloc_f_pic14,
  reloc_tris_pic14,
  op_16cxx, &num_op_16cxx,
  find_insn_generic,
  i_memory_get_le, i_memory_put_le,
};

const struct proc_class proc_class_pic16 = {
  0xb600,				/* retlw */
  16,					/* rom_width */
  1,					/* org_to_byte_shift */
  ~0xffu,				/* bank_mask */
  (1<<16)-1,				/* core_size */
  0,					/* id_location */
  gp_processor_check_bank_pic16,
  gp_processor_set_bank_pic16,
  gp_processor_check_page_pic16,
  gp_processor_set_page_pic16,
  reloc_call_pic16,
  reloc_goto_pic16,
  reloc_ibanksel_pic16,
  reloc_f_pic16,
  reloc_unsupported,
  op_17cxx, &num_op_17cxx,
  find_insn_generic,
  i_memory_get_le, i_memory_put_le,
};

const struct proc_class proc_class_pic16e = {
  0x0c00,				/* retlw */
  8,					/* rom_width */
  0,					/* org_to_byte_shift */
  0,					/* bank_mask */
  (1<<16)-1,				/* core_size */
  id_location_pic16e,			/* id_location */
  gp_processor_check_bank_pic16, 	/* Same as for pic16 */
  gp_processor_set_bank_pic16e,
  gp_processor_check_page_unsupported,
  gp_processor_set_page_unsupported,
  reloc_call_pic16e,
  reloc_goto_pic16e,
  reloc_unsupported,
  reloc_f_pic16, 	/* Same as for pic16 */
  reloc_unsupported,
  op_18cxx, &num_op_18cxx,
  find_insn_pic16e,
  i_memory_get_le, i_memory_put_le,
};

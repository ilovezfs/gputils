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

#include "stdhdr.h"
#include "libgputils.h"

/* XXXPRO: Need to add a line here for any extra processors.  Please
   keep this list sorted! */

static struct px pics[] = {
  { eeprom8,     PROC_CLASS_EEPROM8, "__EEPROM8",  { "eeprom8",     "eeprom8",   "eeprom8"  }, 0x0,    NULL },
  { generic,     PROC_CLASS_GENERIC, "__GEN",      { "generic",     "gen",       "unknown"  }, 0x0,    NULL },
  { pic12c508,   PROC_CLASS_PIC12,   "__12C508",   { "pic12c508",   "p12c508",   "12c508"   }, 0x2508, "12c508.lkr"   },
  { pic12c508a,  PROC_CLASS_PIC12,   "__12C508A",  { "pic12c508a",  "p12c508a",  "12c508a"  }, 0x508a, "12c508a.lkr"  },
  { pic12c509,   PROC_CLASS_PIC12,   "__12C509",   { "pic12c509",   "p12c509",   "12c509"   }, 0x2509, "12c509.lkr"   },
  { pic12c509a,  PROC_CLASS_PIC12,   "__12C509A",  { "pic12c509a",  "p12c509a",  "12c509a"  }, 0x509a, "12c509a.lkr"  },
  { pic12c671,   PROC_CLASS_PIC14,   "__12C671",   { "pic12c671",   "p12c671",   "12c671"   }, 0x2671, "12c671.lkr"   },
  { pic12c672,   PROC_CLASS_PIC14,   "__12C672",   { "pic12c672",   "p12c672",   "12c672"   }, 0x2672, "12c672.lkr"   },
  { pic12ce518,  PROC_CLASS_PIC12,   "__12CE518",  { "pic12ce518",  "p12ce518",  "12ce518"  }, 0x2518, "12ce518.lkr"  },
  { pic12ce519,  PROC_CLASS_PIC12,   "__12CE519",  { "pic12ce519",  "p12ce519",  "12ce519"  }, 0x2519, "12ce519.lkr"  },
  { pic12ce673,  PROC_CLASS_PIC14,   "__12CE673",  { "pic12ce673",  "p12ce673",  "12ce673"  }, 0x2673, "12ce673.lkr"  },
  { pic12ce674,  PROC_CLASS_PIC14,   "__12CE674",  { "pic12ce674",  "p12ce674",  "12ce674"  }, 0x2674, "12ce674.lkr"  },
  { pic12cr509a, PROC_CLASS_PIC12,   "__12CR509A", { "pic12cr509a", "p12cr509a", "12cr509a" }, 0xd09a, "12cr509a.lkr" },
  { pic12f508,   PROC_CLASS_PIC12,   "__12F508",   { "pic12f508",   "p12f508",   "12f508"   }, 0x2508, "12f508.lkr"   },
  { pic12f509,   PROC_CLASS_PIC12,   "__12F509",   { "pic12f509",   "p12f509",   "12f508"   }, 0x2509, "12f509.lkr"   },
  { pic12f629,   PROC_CLASS_PIC14,   "__12F629",   { "pic12f629",   "p12f629",   "12f629"   }, 0x2629, "12f629.lkr"   },
  { pic12f635,   PROC_CLASS_PIC14,   "__12F635",   { "pic12f635",   "p12f635",   "12f635"   }, 0x2635, "12f635.lkr"   },
  { pic12f675,   PROC_CLASS_PIC14,   "__12F675",   { "pic12f675",   "p12f675",   "12f675"   }, 0x2675, "12f675.lkr"   },
  { pic12f683,   PROC_CLASS_PIC14,   "__12F683",   { "pic12f683",   "p12f683",   "12f683"   }, 0x2683, "12f683.lkr"   },
  { pic14000,    PROC_CLASS_PIC14,   "__14000",    { "pic14000",    "p14000",    "14000"    }, 0x4000, "14000.lkr"    },
  { pic16c5x,    PROC_CLASS_PIC12,   "__16C5X",    { "pic16c5x",    "p16c5x",    "16c5x"    }, 0x658a, NULL           },
  { pic16cxx,    PROC_CLASS_PIC14,   "__16CXX",    { "pic16cxx",    "p16cxx",    "16cxx"    }, 0x6c77, NULL           },
  { pic16c432,   PROC_CLASS_PIC14,   "__16C432",   { "pic16c432",   "p16c432",   "16c432"   }, 0x6432, "16c432.lkr"   },
  { pic16c433,   PROC_CLASS_PIC14,   "__16C433",   { "pic16c433",   "p16c433",   "16c433"   }, 0x6433, "16c433.lkr"   },
  { pic16c505,   PROC_CLASS_PIC12,   "__16C505",   { "pic16c505",   "p16c505",   "16c505"   }, 0x2505, "16c505.lkr"   },
  { pic16c52,    PROC_CLASS_PIC12,   "__16C52",    { "pic16c52",    "p16c52",    "16c52"    }, 0x6c52, "16c52.lkr"    },
  { pic16c54,    PROC_CLASS_PIC12,   "__16C54",    { "pic16c54",    "p16c54",    "16c54"    }, 0x6c54, "16c54.lkr"    },
  { pic16c54a,   PROC_CLASS_PIC12,   "__16C54A",   { "pic16c54a",   "p16c54a",   "16c54a"   }, 0x654a, "16c54a.lkr"   },
  { pic16c54b,   PROC_CLASS_PIC12,   "__16C54B",   { "pic16c54b",   "p16c54b",   "16c54b"   }, 0x654b, "16c54b.lkr"   },
  { pic16c54c,   PROC_CLASS_PIC12,   "__16C54C",   { "pic16c54c",   "p16c54c",   "16c54c"   }, 0x654c, "16c54c.lkr"   },
  { pic16c55,    PROC_CLASS_PIC12,   "__16C55",    { "pic16c55",    "p16c55",    "16c55"    }, 0x6c55, "16c55.lkr"    },
  { pic16c55a,   PROC_CLASS_PIC12,   "__16C55A",   { "pic16c55a",   "p16c55a",   "16c55a"   }, 0x655a, "16c55a.lkr"   },
  { pic16c554,   PROC_CLASS_PIC14,   "__16C554",   { "pic16c554",   "p16c554",   "16c554"   }, 0x6554, "16c554.lkr"   },
  { pic16c557,   PROC_CLASS_PIC14,   "__16C557",   { "pic16c557",   "p16c557",   "16c557"   }, 0x6557, "16c557.lkr"   },
  { pic16c558,   PROC_CLASS_PIC14,   "__16C558",   { "pic16c558",   "p16c558",   "16c558"   }, 0x6558, "16c558.lkr"   },
  { pic16c56,    PROC_CLASS_PIC12,   "__16C56",    { "pic16c56",    "p16c56",    "16c56"    }, 0x6c56, "16c56.lkr"    },
  { pic16c56a,   PROC_CLASS_PIC12,   "__16C56A",   { "pic16c56a",   "p16c56a",   "16c56a"   }, 0x656a, "16c56a.lkr"   },
  { pic16c57,    PROC_CLASS_PIC12,   "__16C57",    { "pic16c57",    "p16c57",    "16c57"    }, 0x6c57, "16c57.lkr"    },
  { pic16c57c,   PROC_CLASS_PIC12,   "__16C57C",   { "pic16c57c",   "p16c57c",   "16c57c"   }, 0x657c, "16c57c.lkr"   },
  { pic16c58a,   PROC_CLASS_PIC12,   "__16C58A",   { "pic16c58a",   "p16c58a",   "16c58a"   }, 0x658a, "16c58a.lkr"   },
  { pic16c58b,   PROC_CLASS_PIC12,   "__16C58B",   { "pic16c58b",   "p16c58b",   "16c58b"   }, 0x658b, "16c58b.lkr"   },
  { pic16c61,    PROC_CLASS_PIC14,   "__16C61",    { "pic16c61",    "p16c61",    "16c61"    }, 0x6c61, "16c61.lkr"    },
  { pic16c62,    PROC_CLASS_PIC14,   "__16C62",    { "pic16c62",    "p16c62",    "16c62"    }, 0x6c62, "16c62.lkr"    },
  { pic16c62a,   PROC_CLASS_PIC14,   "__16C62A",   { "pic16c62a",   "p16c62a",   "16c62a"   }, 0x662a, "16c62a.lkr"   },
  { pic16c62b,   PROC_CLASS_PIC14,   "__16C62B",   { "pic16c62b",   "p16c62b",   "16c62b"   }, 0x662b, "16c62b.lkr"   },
  { pic16c620,   PROC_CLASS_PIC14,   "__16C620",   { "pic16c620",   "p16c620",   "16c620"   }, 0x6620, "16c620.lkr"   },
  { pic16c620a,  PROC_CLASS_PIC14,   "__16C620A",  { "pic16c620a",  "p16c620a",  "16c620a"  }, 0x620a, "16c620a.lkr"  },
  { pic16c621,   PROC_CLASS_PIC14,   "__16C621",   { "pic16c621",   "p16c621",   "16c621"   }, 0x6621, "16c621.lkr"   },
  { pic16c621a,  PROC_CLASS_PIC14,   "__16C621A",  { "pic16c621a",  "p16c621a",  "16c621a"  }, 0x621a, "16c621a.lkr"  },
  { pic16c622,   PROC_CLASS_PIC14,   "__16C622",   { "pic16c622",   "p16c622",   "16c622"   }, 0x6622, "16c622.lkr"   },
  { pic16c622a,  PROC_CLASS_PIC14,   "__16C622A",  { "pic16c622a",  "p16c622a",  "16c622a"  }, 0x622a, "16c622a.lkr"  },
  { pic16c63,    PROC_CLASS_PIC14,   "__16C63",    { "pic16c63",    "p16c63",    "16c63"    }, 0x6c63, "16c63.lkr"    },
  { pic16c63a,   PROC_CLASS_PIC14,   "__16C63A",   { "pic16c63a",   "p16c63a",   "16c63a"   }, 0x663a, "16c63a.lkr"   },
  { pic16c64,    PROC_CLASS_PIC14,   "__16C64",    { "pic16c64",    "p16c64",    "16c64"    }, 0x6c64, "16c64.lkr"    },
  { pic16c64a,   PROC_CLASS_PIC14,   "__16C64A",   { "pic16c64a",   "p16c64a",   "16c64a"   }, 0x664a, "16c64a.lkr"   },
  { pic16c642,   PROC_CLASS_PIC14,   "__16C642",   { "pic16c642",   "p16c642",   "16c642"   }, 0x6642, "16c642.lkr"   },
  { pic16c65,    PROC_CLASS_PIC14,   "__16C65",    { "pic16c65",    "p16c65",    "16c65"    }, 0x6c65, "16c65.lkr"    },
  { pic16c65a,   PROC_CLASS_PIC14,   "__16C65A",   { "pic16c65a",   "p16c65a",   "16c65a"   }, 0x665a, "16c65a.lkr"   },
  { pic16c65b,   PROC_CLASS_PIC14,   "__16C65B",   { "pic16c65b",   "p16c65b",   "16c65b"   }, 0x665b, "16c65b.lkr"   },
  { pic16c66,    PROC_CLASS_PIC14,   "__16C66",    { "pic16c66",    "p16c66",    "16c66"    }, 0x6c66, "16c66.lkr"    },
  { pic16c662,   PROC_CLASS_PIC14,   "__16C662",   { "pic16c662",   "p16c662",   "16c662"   }, 0x6662, "16c662.lkr"   },
  { pic16c67,    PROC_CLASS_PIC14,   "__16C67",    { "pic16c67",    "p16c67",    "16c67"    }, 0x6c67, "16c67.lkr"    },
  { pic16c71,    PROC_CLASS_PIC14,   "__16C71",    { "pic16c71",    "p16c71",    "16c71"    }, 0x6c71, "16c71.lkr"    },
  { pic16c710,   PROC_CLASS_PIC14,   "__16C710",   { "pic16c710",   "p16c710",   "16c710"   }, 0x6710, "16c710.lkr"   },
  { pic16c711,   PROC_CLASS_PIC14,   "__16C711",   { "pic16c711",   "p16c711",   "16c711"   }, 0x6711, "16c711.lkr"   },
  { pic16c712,   PROC_CLASS_PIC14,   "__16C712",   { "pic16c712",   "p16c712",   "16c712"   }, 0x6712, "16c712.lkr"   },  
  { pic16c715,   PROC_CLASS_PIC14,   "__16C715",   { "pic16c715",   "p16c715",   "16c715"   }, 0x6715, "16c715.lkr"   },  
  { pic16c716,   PROC_CLASS_PIC14,   "__16C716",   { "pic16c716",   "p16c716",   "16c716"   }, 0x6716, "16c716.lkr"   },
  { pic16c717,   PROC_CLASS_PIC14,   "__16C717",   { "pic16c717",   "p16c717",   "16c717"   }, 0x6717, "16c717.lkr"   },  
  { pic16c72,    PROC_CLASS_PIC14,   "__16C72",    { "pic16c72",    "p16c72",    "16c72"    }, 0x6c72, "16c72.lkr"    },
  { pic16c72a,   PROC_CLASS_PIC14,   "__16C72A",   { "pic16c72a",   "p16c72a",   "16c72a"   }, 0x672a, "16c72a.lkr"   },
  { pic16c73,    PROC_CLASS_PIC14,   "__16C73",    { "pic16c73",    "p16c73",    "16c73"    }, 0x673a, "16c73.lkr"    },
  { pic16c73a,   PROC_CLASS_PIC14,   "__16C73A",   { "pic16c73a",   "p16c73a",   "16c73a"   }, 0x673a, "16c73a.lkr"   },
  { pic16c73b,   PROC_CLASS_PIC14,   "__16C73B",   { "pic16c73b",   "p16c73b",   "16c73b"   }, 0x673b, "16c73b.lkr"   },
  { pic16c74,    PROC_CLASS_PIC14,   "__16C74",    { "pic16c74",    "p16c74",    "16c74"    }, 0x6c74, "16c74.lkr"    },
  { pic16c745,   PROC_CLASS_PIC14,   "__16C745",   { "pic16c745",   "p16c745",   "16c745"   }, 0x6745, "16c745.lkr"   },
  { pic16c747,   PROC_CLASS_PIC14,   "__16C747",   { "pic16c747",   "p16c747",   "16c747"   }, 0x6747, "16c747.lkr"   },
  { pic16c74a,   PROC_CLASS_PIC14,   "__16C74A",   { "pic16c74a",   "p16c74a",   "16c74a"   }, 0x674a, "16c74.lkr"    },
  { pic16c74b,   PROC_CLASS_PIC14,   "__16C74B",   { "pic16c74b",   "p16c74b",   "16c74b"   }, 0x674b, "16c74.lkr"    },
  { pic16c76,    PROC_CLASS_PIC14,   "__16C76",    { "pic16c76",    "p16c76",    "16c76"    }, 0x6c76, "16c76.lkr"    },
  { pic16c765,   PROC_CLASS_PIC14,   "__16C765",   { "pic16c765",   "p16c765",   "16c765"   }, 0x6765, "16c765.lkr"   },
  { pic16c77,    PROC_CLASS_PIC14,   "__16C77",    { "pic16c77",    "p16c77",    "16c77"    }, 0x6c77, "16c77.lkr"    },  
  { pic16c770,   PROC_CLASS_PIC14,   "__16C770",   { "pic16c770",   "p16c770",   "16c770"   }, 0x6770, "16c770.lkr"   },  
  { pic16c771,   PROC_CLASS_PIC14,   "__16C771",   { "pic16c771",   "p16c771",   "16c771"   }, 0x6771, "16c771.lkr"   },  
  { pic16c773,   PROC_CLASS_PIC14,   "__16C773",   { "pic16c773",   "p16c773",   "16c773"   }, 0x6773, "16c773.lkr"   },  
  { pic16c774,   PROC_CLASS_PIC14,   "__16C774",   { "pic16c774",   "p16c774",   "16c774"   }, 0x6774, "16c774.lkr"   },  
  { pic16c781,   PROC_CLASS_PIC14,   "__16C781",   { "pic16c781",   "p16c781",   "16c781"   }, 0x6781, "16c781.lkr"   },  
  { pic16c782,   PROC_CLASS_PIC14,   "__16C782",   { "pic16c782",   "p16c782",   "16c782"   }, 0x6782, "16c782.lkr"   },  
  { pic16c84,    PROC_CLASS_PIC14,   "__16C84",    { "pic16c84",    "p16c84",    "16c84"    }, 0x6c84, "16c84.lkr"    },
  { pic16c923,   PROC_CLASS_PIC14,   "__16C923",   { "pic16c923",   "p16c923",   "16c923"   }, 0x6923, "16c923.lkr"   },  
  { pic16c924,   PROC_CLASS_PIC14,   "__16C924",   { "pic16c924",   "p16c924",   "16c924"   }, 0x6924, "16c924.lkr"   },  
  { pic16c925,   PROC_CLASS_PIC14,   "__16C925",   { "pic16c925",   "p16c925",   "16c925"   }, 0x6925, "16c925.lkr"   },  
  { pic16c926,   PROC_CLASS_PIC14,   "__16C926",   { "pic16c926",   "p16c926",   "16c926"   }, 0x6926, "16c926.lkr"   },  
  { pic16ce623,  PROC_CLASS_PIC14,   "__16CE623",  { "pic16ce623",  "p16ce623",  "16ce623"  }, 0x6623, "16ce623.lkr"  },
  { pic16ce624,  PROC_CLASS_PIC14,   "__16CE624",  { "pic16ce624",  "p16ce624",  "16ce624"  }, 0x6624, "16ce624.lkr"  },
  { pic16ce625,  PROC_CLASS_PIC14,   "__16CE625",  { "pic16ce625",  "p16ce625",  "16ce625"  }, 0x6625, "16ce625.lkr"  },
  { pic16cr54,   PROC_CLASS_PIC12,   "__16CR54",   { "pic16cr54",   "p16cr54",   "16cr54"   }, 0xdc54, "16cr54.lkr"   },
  { pic16cr54a,  PROC_CLASS_PIC12,   "__16CR54A",  { "pic16cr54a",  "p16cr54a",  "16cr54a"  }, 0xd54a, "16cr54a.lkr"  },
  { pic16cr54b,  PROC_CLASS_PIC12,   "__16CR54B",  { "pic16cr54b",  "p16cr54b",  "16cr54b"  }, 0xd54b, "16cr54b.lkr"  },
  { pic16cr54c,  PROC_CLASS_PIC12,   "__16CR54C",  { "pic16cr54c",  "p16cr54c",  "16cr54c"  }, 0xdc54, "16cr54c.lkr"  },
  { pic16cr56a,  PROC_CLASS_PIC12,   "__16CR56A",  { "pic16cr56a",  "p16cr56a",  "16cr56a"  }, 0xd56a, "16cr56a.lkr"  },
  { pic16cr57a,  PROC_CLASS_PIC12,   "__16CR57A",  { "pic16cr57a",  "p16cr57a",  "16cr57a"  }, 0xd57a, "16cr57a.lkr"  },
  { pic16cr57b,  PROC_CLASS_PIC12,   "__16CR57B",  { "pic16cr57b",  "p16cr57b",  "16cr57b"  }, 0xd57b, "16cr57b.lkr"  },
  { pic16cr57c,  PROC_CLASS_PIC12,   "__16CR57C",  { "pic16cr57c",  "p16cr57c",  "16cr57c"  }, 0xd57c, "16cr57c.lkr"  },
  { pic16cr58a,  PROC_CLASS_PIC12,   "__16CR58A",  { "pic16cr58a",  "p16cr58a",  "16cr58a"  }, 0xd58a, "16cr58a.lkr"  },
  { pic16cr58b,  PROC_CLASS_PIC12,   "__16CR58B",  { "pic16cr58b",  "p16cr58b",  "16cr58b"  }, 0xd58b, "16cr58b.lkr"  },
  { pic16cr62,   PROC_CLASS_PIC14,   "__16CR62",   { "pic16cr62",   "p16cr62",   "16cr62"   }, 0xdc62, "16cr62.lkr"   },
  { pic16cr620a, PROC_CLASS_PIC14,   "__16CR620A", { "pic16cr620a", "p16cr620a", "16cr620a" }, 0xd20a, "16cr620a.lkr" },
  { pic16cr63,   PROC_CLASS_PIC14,   "__16CR63",   { "pic16cr63",   "p16cr63",   "16cr63"   }, 0x6d63, "16cr63.lkr"   },
  { pic16cr64,   PROC_CLASS_PIC14,   "__16CR64",   { "pic16cr64",   "p16cr64",   "16cr64"   }, 0xdc64, "16cr64.lkr"   },
  { pic16cr65,   PROC_CLASS_PIC14,   "__16CR65",   { "pic16cr65",   "p16cr65",   "16cr65"   }, 0x6d65, "16cr65.lkr"   },
  { pic16cr72,   PROC_CLASS_PIC14,   "__16CR72",   { "pic16cr72",   "p16cr72",   "16cr72"   }, 0x6d72, "16cr72.lkr"   },
  { pic16cr83,   PROC_CLASS_PIC14,   "__16CR83",   { "pic16cr83",   "p16cr83",   "16cr83"   }, 0xdc83, "16cr83.lkr"   },
  { pic16cr84,   PROC_CLASS_PIC14,   "__16CR84",   { "pic16cr84",   "p16cr84",   "16cr84"   }, 0xdc84, "16cr84.lkr"   },
  { pic16f505,   PROC_CLASS_PIC12,   "__16F505",   { "pic16f505",   "p16f505",   "16f505"   }, 0x654f, "16f505.lkr"    },
  { pic16f54,    PROC_CLASS_PIC14,   "__16F54",    { "pic16f54",    "p16f54",    "16f54"    }, 0x654f, "16f54.lkr"    },
  { pic16f57,    PROC_CLASS_PIC14,   "__16F57",    { "pic16f57",    "p16f57",    "16f57"    }, 0x657f, "16f57.lkr"    },
  { pic16f627,   PROC_CLASS_PIC14,   "__16F627",   { "pic16f627",   "p16f627",   "16f627"   }, 0x6627, "16f627.lkr"   },
  { pic16f627a,  PROC_CLASS_PIC14,   "__16F627A",  { "pic16f627a",  "p16f627a",  "16f627a"  }, 0x627a, "16f627a.lkr"  },
  { pic16f628,   PROC_CLASS_PIC14,   "__16F628",   { "pic16f628",   "p16f628",   "16f628"   }, 0x6628, "16f628.lkr"   },  
  { pic16f628a,  PROC_CLASS_PIC14,   "__16F628A",  { "pic16f628a",  "p16f628a",  "16f628a"  }, 0x628a, "16f628a.lkr"  },
  { pic16f630,   PROC_CLASS_PIC14,   "__16F630",   { "pic16f630",   "p16f630",   "16f630"   }, 0x6630, "16f630.lkr"   },
  { pic16f636,   PROC_CLASS_PIC14,   "__16F636",   { "pic16f636",   "p16f636",   "16f636"   }, 0x6636, "16f636.lkr"   },
  { pic16f648a,  PROC_CLASS_PIC14,   "__16F648A",  { "pic16f648a",  "p16f648a",  "16f648a"  }, 0x648a, "16f648a.lkr"  },
  { pic16f676,   PROC_CLASS_PIC14,   "__16F676",   { "pic16f676",   "p16f676",   "16f676"   }, 0x6676, "16f676.lkr"   },
  { pic16f684,   PROC_CLASS_PIC14,   "__16F684",   { "pic16f684",   "p16f684",   "16f684"   }, 0x6684, "16f684.lkr"   },
  { pic16f688,   PROC_CLASS_PIC14,   "__16F688",   { "pic16f688",   "p16f688",   "16f688"   }, 0x6688, "16f688.lkr"   },
  { pic16f716,   PROC_CLASS_PIC14,   "__16F716",   { "pic16f716",   "p16f716",   "16f716"   }, 0x716f, "16f716.lkr"   },
  { pic16f72,    PROC_CLASS_PIC14,   "__16F72",    { "pic16f72",    "p16f72",    "16f72"    }, 0x672f, "16f72.lkr"    },
  { pic16f73,    PROC_CLASS_PIC14,   "__16F73",    { "pic16f73",    "p16f73",    "16f73"    }, 0x673f, "16f73.lkr"    },
  { pic16f737,   PROC_CLASS_PIC14,   "__16F737",   { "pic16f737",   "p16f737",   "16f737"   }, 0x737f, "16f737.lkr"   },
  { pic16f74,    PROC_CLASS_PIC14,   "__16F74",    { "pic16f74",    "p16f74",    "16f74"    }, 0x674f, "16f74.lkr"    },
  { pic16f76,    PROC_CLASS_PIC14,   "__16F76",    { "pic16f76",    "p16f76",    "16f76"    }, 0x676f, "16f76.lkr"    },
  { pic16f767,   PROC_CLASS_PIC14,   "__16F767",   { "pic16f767",   "p16f767",   "16f767"   }, 0x767f, "16f767.lkr"   },
  { pic16f77,    PROC_CLASS_PIC14,   "__16F77",    { "pic16f77",    "p16f77",    "16f77"    }, 0x677f, "16f77.lkr"    },
  { pic16f777,   PROC_CLASS_PIC14,   "__16F777",   { "pic16f777",   "p16f777",   "16f777"   }, 0x777f, "16f777.lkr"   },
  { pic16f785,   PROC_CLASS_PIC14,   "__16F785",   { "pic16f785",   "p16f785",   "16f785"   }, 0x785f, "16f785.lkr"   },
  { pic16f818,   PROC_CLASS_PIC14,   "__16F818",   { "pic16f818",   "p16f818",   "16f818"   }, 0x818f, "16f818.lkr"   },
  { pic16f819,   PROC_CLASS_PIC14,   "__16F819",   { "pic16f819",   "p16f819",   "16f819"   }, 0x819f, "16f819.lkr"   },
  { pic16f83,    PROC_CLASS_PIC14,   "__16F83",    { "pic16f83",    "p16f83",    "16f83"    }, 0x6c83, "16f83.lkr"    },
  { pic16f84,    PROC_CLASS_PIC14,   "__16F84",    { "pic16f84",    "p16f84",    "16f84"    }, 0x6f84, "16f84.lkr"    },
  { pic16f84a,   PROC_CLASS_PIC14,   "__16F84A",   { "pic16f84a",   "p16f84a",   "16f84a"   }, 0x684a, "16f84a.lkr"   },
  { pic16f87,    PROC_CLASS_PIC14,   "__16F87",    { "pic16f87",    "p16f87",    "16f87"    }, 0x687f, "16f87.lkr"    },
  { pic16f870,   PROC_CLASS_PIC14,   "__16F870",   { "pic16f870",   "p16f870",   "16f870"   }, 0x870f, "16f870.lkr"   },
  { pic16f871,   PROC_CLASS_PIC14,   "__16F871",   { "pic16f871",   "p16f871",   "16f871"   }, 0x871f, "16f871.lkr"   },
  { pic16f872,   PROC_CLASS_PIC14,   "__16F872",   { "pic16f872",   "p16f872",   "16f872"   }, 0x872f, "16f872.lkr"   },
  { pic16f873,   PROC_CLASS_PIC14,   "__16F873",   { "pic16f873",   "p16f873",   "16f873"   }, 0x873f, "16f873.lkr"   },
  { pic16f873a,  PROC_CLASS_PIC14,   "__16F873A",  { "pic16f873a",  "p16f873a",  "16f873a"  }, 0x873a, "16f873a.lkr"  },
  { pic16f874,   PROC_CLASS_PIC14,   "__16F874",   { "pic16f874",   "p16f874",   "16f874"   }, 0x874f, "16f874.lkr"   },
  { pic16f874a,  PROC_CLASS_PIC14,   "__16F874A",  { "pic16f874a",  "p16f874a",  "16f874a"  }, 0x874a, "16f874a.lkr"  },
  { pic16f876,   PROC_CLASS_PIC14,   "__16F876",   { "pic16f876",   "p16f876",   "16f876"   }, 0x876f, "16f876.lkr"   },
  { pic16f876a,  PROC_CLASS_PIC14,   "__16F876A",  { "pic16f876a",  "p16f876a",  "16f876a"  }, 0x876a, "16f876a.lkr"  },
  { pic16f877,   PROC_CLASS_PIC14,   "__16F877",   { "pic16f877",   "p16f877",   "16f877"   }, 0x877f, "16f877.lkr"   },
  { pic16f877a,  PROC_CLASS_PIC14,   "__16F877A",  { "pic16f877a",  "p16f877a",  "16f877a"  }, 0x877a, "16f877a.lkr"  },
  { pic16f88,    PROC_CLASS_PIC14,   "__16F88",    { "pic16f88",    "p16f88",    "16f88"    }, 0x688f, "16f88.lkr"    },
  { pic16f913,   PROC_CLASS_PIC14,   "__16F913",   { "pic16f913",   "p16f913",   "16f913"   }, 0x913f, "16f913.lkr"   },
  { pic16f914,   PROC_CLASS_PIC14,   "__16F914",   { "pic16f914",   "p16f914",   "16f914"   }, 0x914f, "16f914.lkr"   },
  { pic16f916,   PROC_CLASS_PIC14,   "__16F916",   { "pic16f916",   "p16f916",   "16f916"   }, 0x916f, "16f916.lkr"   },
  { pic16f917,   PROC_CLASS_PIC14,   "__16F917",   { "pic16f917",   "p16f917",   "16f917"   }, 0x917f, "16f917.lkr"   },
  { pic16hv540,  PROC_CLASS_PIC12,   "__16HV540",  { "pic16hv540",  "p16hv540",  "16hv540"  }, 0x6540, "16hv540.lkr"  },
  { pic17cxx,    PROC_CLASS_PIC16,   "__17CXX",    { "pic17cxx",    "p17cxx",    "17cxx"    }, 0x7756, NULL           },
  { pic17c42,    PROC_CLASS_PIC16,   "__17C42",    { "pic17c42",    "p17c42",    "17c42"    }, 0x7c42, "17c42.lkr"    },  
  { pic17c42a,   PROC_CLASS_PIC16,   "__17C42A",   { "pic17c42a",   "p17c42a",   "17c42a"   }, 0x742a, "17c42a.lkr"   },  
  { pic17c43,    PROC_CLASS_PIC16,   "__17C43",    { "pic17c43",    "p17c43",    "17c43"    }, 0x7c43, "17c43.lkr"    },  
  { pic17c44,    PROC_CLASS_PIC16,   "__17C44",    { "pic17c44",    "p17c44",    "17c44"    }, 0x7c44, "17c44.lkr"    },  
  { pic17c752,   PROC_CLASS_PIC16,   "__17C752",   { "pic17c752",   "p17c752",   "17c752"   }, 0x7752, "17c752.lkr"   },  
  { pic17c756,   PROC_CLASS_PIC16,   "__17C756",   { "pic17c756",   "p17c756",   "17c756"   }, 0x7756, "17c756.lkr"   },  
  { pic17c756a,  PROC_CLASS_PIC16,   "__17C756A",  { "pic17c756a",  "p17c756a",  "17c756a"  }, 0x756a, "17c756a.lkr"  },  
  { pic17c762,   PROC_CLASS_PIC16,   "__17C762",   { "pic17c762",   "p17c762",   "17c762"   }, 0x7762, "17c762.lkr"   },
  { pic17c766,   PROC_CLASS_PIC16,   "__17C766",   { "pic17c766",   "p17c766",   "17c766"   }, 0x7766, "17c766.lkr"   },
  { pic17cr42,   PROC_CLASS_PIC16,   "__17CR42",   { "pic17cr42",   "p17cr42",   "17cr42"   }, 0xe42a, "17cr42.lkr"   },  
  { pic17cr43,   PROC_CLASS_PIC16,   "__17CR43",   { "pic17cr43",   "p17cr43",   "17cr43"   }, 0xec43, "17cr43.lkr"   },  
  { pic18cxx,    PROC_CLASS_PIC16E,  "__18CXX",    { "pic18cxx",    "p18cxx",    "18cxx"    }, 0x8452, NULL           },
  { pic18c242,   PROC_CLASS_PIC16E,  "__18C242",   { "pic18c242",   "p18c242",   "18c242"   }, 0x8242, "18c242.lkr"   },
  { pic18c252,   PROC_CLASS_PIC16E,  "__18C252",   { "pic18c252",   "p18c252",   "18c252"   }, 0x8252, "18c252.lkr"   },
  { pic18c442,   PROC_CLASS_PIC16E,  "__18C442",   { "pic18c442",   "p18c442",   "18c442"   }, 0x8442, "18c442.lkr"   },
  { pic18c452,   PROC_CLASS_PIC16E,  "__18C452",   { "pic18c452",   "p18c452",   "18c452"   }, 0x8452, "18c452.lkr"   },
  { pic18c601,   PROC_CLASS_PIC16E,  "__18C601",   { "pic18c601",   "p18c601",   "18c601"   }, 0x8601, "18c601.lkr"   },
  { pic18c658,   PROC_CLASS_PIC16E,  "__18C658",   { "pic18c658",   "p18c658",   "18c658"   }, 0x8658, "18c658.lkr"   },
  { pic18c801,   PROC_CLASS_PIC16E,  "__18C801",   { "pic18c801",   "p18c801",   "18c801"   }, 0x8801, "18c801.lkr"   },
  { pic18c858,   PROC_CLASS_PIC16E,  "__18C858",   { "pic18c858",   "p18c858",   "18c858"   }, 0x8858, "18c858.lkr"   },
  { pic18f1220,  PROC_CLASS_PIC16E,  "__18F1220",  { "pic18f1220",  "p18f1220",  "18f1220"  }, 0xa122, "18f1220.lkr"  },
  { pic18f1320,  PROC_CLASS_PIC16E,  "__18F1320",  { "pic18f1320",  "p18f1320",  "18f1320"  }, 0xa132, "18f1320.lkr"  },
  { pic18f2220,  PROC_CLASS_PIC16E,  "__18F2220",  { "pic18f2220",  "p18f2220",  "18f2220"  }, 0xa222, "18f2220.lkr"  },
  { pic18f2320,  PROC_CLASS_PIC16E,  "__18F2320",  { "pic18f2320",  "p18f2320",  "18f2320"  }, 0xa232, "18f2320.lkr"  },
  { pic18f2331,  PROC_CLASS_PIC16E,  "__18F2331",  { "pic18f2331",  "p18f2331",  "18f2331"  }, 0x2331, "18f2331.lkr"  },
  { pic18f2410,  PROC_CLASS_PIC16E,  "__18F2410",  { "pic18f2410",  "p18f2410",  "18f2410"  }, 0x2410, "18f2410.lkr"  },
  { pic18f242,   PROC_CLASS_PIC16E,  "__18F242",   { "pic18f242",   "p18f242",   "18f242"   }, 0x242f, "18f242.lkr"   },
  { pic18f2420,  PROC_CLASS_PIC16E,  "__18F2420",  { "pic18f2420",  "p18f2420",  "18f2420"  }, 0x2420, "18f2420.lkr"  },
  { pic18f2431,  PROC_CLASS_PIC16E,  "__18F2431",  { "pic18f2431",  "p18f2431",  "18f2431"  }, 0x2431, "18f2431.lkr"  },
  { pic18f2439,  PROC_CLASS_PIC16E,  "__18F2439",  { "pic18f2439",  "p18f2439",  "18f2439"  }, 0x2439, "18f2439.lkr"  },
  { pic18f2455,  PROC_CLASS_PIC16E,  "__18F2455",  { "pic18f2455",  "p18f2455",  "18f2455"  }, 0x2455, "18f2455.lkr"  },
  { pic18f248,   PROC_CLASS_PIC16E,  "__18F248",   { "pic18f248",   "p18f248",   "18f248"   }, 0x8248, "18f248.lkr"   },
  { pic18f2480,  PROC_CLASS_PIC16E,  "__18F2480",  { "pic18f2480",  "p18f2480",  "18f2480"  }, 0x2480, "18f2480.lkr"  },
  { pic18f2510,  PROC_CLASS_PIC16E,  "__18F2510",  { "pic18f2510",  "p18f2510",  "18f2510"  }, 0x2510, "18f2510.lkr"  },
  { pic18f2515,  PROC_CLASS_PIC16E,  "__18F2515",  { "pic18f2515",  "p18f2515",  "18f2515"  }, 0x2515, "18f2515.lkr"  },
  { pic18f252,   PROC_CLASS_PIC16E,  "__18F252",   { "pic18f252",   "p18f252",   "18f252"   }, 0x252f, "18f252.lkr"   },
  { pic18f2520,  PROC_CLASS_PIC16E,  "__18F2520",  { "pic18f2520",  "p18f2520",  "18f2520"  }, 0x2520, "18f2520.lkr"  },
  { pic18f2525,  PROC_CLASS_PIC16E,  "__18F2525",  { "pic18f2525",  "p18f2525",  "18f2525"  }, 0x2525, "18f2525.lkr"  },
  { pic18f2539,  PROC_CLASS_PIC16E,  "__18F2539",  { "pic18f2539",  "p18f2539",  "18f2539"  }, 0x2539, "18f2539.lkr"  },
  { pic18f2550,  PROC_CLASS_PIC16E,  "__18F2550",  { "pic18f2550",  "p18f2550",  "18f2550"  }, 0x2550, "18f2550.lkr"  },
  { pic18f258,   PROC_CLASS_PIC16E,  "__18F258",   { "pic18f258",   "p18f258",   "18f258"   }, 0x8258, "18f258.lkr"   },
  { pic18f2580,  PROC_CLASS_PIC16E,  "__18F2580",  { "pic18f2580",  "p18f2580",  "18f2580"  }, 0x2580, "18f2580.lkr"  },
  { pic18f2585,  PROC_CLASS_PIC16E,  "__18F2585",  { "pic18f2585",  "p18f2585",  "18f2585"  }, 0x2585, "18f2585.lkr"  },
  { pic18f2610,  PROC_CLASS_PIC16E,  "__18F2610",  { "pic18f2610",  "p18f2610",  "18f2610"  }, 0xa261, "18f2610.lkr"  },
  { pic18f2620,  PROC_CLASS_PIC16E,  "__18F2620",  { "pic18f2620",  "p18f2620",  "18f2620"  }, 0xa262, "18f2620.lkr"  },
  { pic18f2680,  PROC_CLASS_PIC16E,  "__18F2680",  { "pic18f2680",  "p18f2680",  "18f2680"  }, 0x2680, "18f2680.lkr"  },
  { pic18f2681,  PROC_CLASS_PIC16E,  "__18F2681",  { "pic18f2681",  "p18f2681",  "18f2681"  }, 0x2681, "18f2681.lkr"  },
  { pic18f4220,  PROC_CLASS_PIC16E,  "__18F4220",  { "pic18f4220",  "p18f4220",  "18f4220"  }, 0xa422, "18f4220.lkr"  },
  { pic18f4320,  PROC_CLASS_PIC16E,  "__18F4320",  { "pic18f4320",  "p18f4320",  "18f4320"  }, 0xa432, "18f4320.lkr"  },
  { pic18f4331,  PROC_CLASS_PIC16E,  "__18F4331",  { "pic18f4331",  "p18f4331",  "18f4331"  }, 0x4331, "18f4331.lkr"  },
  { pic18f4410,  PROC_CLASS_PIC16E,  "__18F4410",  { "pic18f4410",  "p18f4410",  "18f4410"  }, 0x4410, "18f4410.lkr"  },
  { pic18f442,   PROC_CLASS_PIC16E,  "__18F442",   { "pic18f442",   "p18f442",   "18f442"   }, 0x442f, "18f442.lkr"   },
  { pic18f4420,  PROC_CLASS_PIC16E,  "__18F4420",  { "pic18f4420",  "p18f4420",  "18f4420"  }, 0x4420, "18f4420.lkr"  },
  { pic18f4431,  PROC_CLASS_PIC16E,  "__18F4431",  { "pic18f4431",  "p18f4431",  "18f4431"  }, 0x4431, "18f4431.lkr"  },
  { pic18f4439,  PROC_CLASS_PIC16E,  "__18F4439",  { "pic18f4439",  "p18f4439",  "18f4439"  }, 0x4439, "18f4439.lkr"  },
  { pic18f4455,  PROC_CLASS_PIC16E,  "__18F4455",  { "pic18f4455",  "p18f4455",  "18f4455"  }, 0x4455, "18f4455.lkr"  },
  { pic18f448,   PROC_CLASS_PIC16E,  "__18F448",   { "pic18f448",   "p18f448",   "18f448"   }, 0x8448, "18f448.lkr"   },
  { pic18f4480,  PROC_CLASS_PIC16E,  "__18F4480",  { "pic18f4480",  "p18f4480",  "18f4480"  }, 0x4480, "18f4480.lkr"  },
  { pic18f4510,  PROC_CLASS_PIC16E,  "__18F4510",  { "pic18f4510",  "p18f4510",  "18f4510"  }, 0x4510, "18f4510.lkr"  },
  { pic18f4515,  PROC_CLASS_PIC16E,  "__18F4515",  { "pic18f4515",  "p18f4515",  "18f4515"  }, 0x4515, "18f4515.lkr"  },
  { pic18f452,   PROC_CLASS_PIC16E,  "__18F452",   { "pic18f452",   "p18f452",   "18f452"   }, 0x452f, "18f452.lkr"   },
  { pic18f4520,  PROC_CLASS_PIC16E,  "__18F4520",  { "pic18f4520",  "p18f4520",  "18f4520"  }, 0x4520, "18f4520.lkr"  },
  { pic18f4525,  PROC_CLASS_PIC16E,  "__18F4525",  { "pic18f4525",  "p18f4525",  "18f4525"  }, 0x4525, "18f4525.lkr"  },
  { pic18f4539,  PROC_CLASS_PIC16E,  "__18F4539",  { "pic18f4539",  "p18f4539",  "18f4539"  }, 0x4539, "18f4539.lkr"  },
  { pic18f4550,  PROC_CLASS_PIC16E,  "__18F4550",  { "pic18f4550",  "p18f4550",  "18f4550"  }, 0x4550, "18f4550.lkr"  },
  { pic18f458,   PROC_CLASS_PIC16E,  "__18F458",   { "pic18f458",   "p18f458",   "18f458"   }, 0x8458, "18f458.lkr"   },
  { pic18f4580,  PROC_CLASS_PIC16E,  "__18F4580",  { "pic18f4580",  "p18f4580",  "18f4580"  }, 0x4580, "18f4580.lkr"  },
  { pic18f4585,  PROC_CLASS_PIC16E,  "__18F4585",  { "pic18f4585",  "p18f4585",  "18f4585"  }, 0x4585, "18f4585.lkr"  },
  { pic18f4610,  PROC_CLASS_PIC16E,  "__18F4610",  { "pic18f4610",  "p18f4610",  "18f4610"  }, 0xa461, "18f4610.lkr"  },
  { pic18f4620,  PROC_CLASS_PIC16E,  "__18F4620",  { "pic18f4620",  "p18f4620",  "18f4620"  }, 0xa462, "18f4620.lkr"  },
  { pic18f4680,  PROC_CLASS_PIC16E,  "__18F4680",  { "pic18f4680",  "p18f4680",  "18f4680"  }, 0x4680, "18f4680.lkr"  },
  { pic18f4681,  PROC_CLASS_PIC16E,  "__18F4681",  { "pic18f4681",  "p18f4681",  "18f4681"  }, 0x4681, "18f4681.lkr"  },
  { pic18f6410,  PROC_CLASS_PIC16E,  "__18F6410",  { "pic18f6410",  "p18f6410",  "18f6410"  }, 0xa641, "18f6410.lkr"  },
  { pic18f6490,  PROC_CLASS_PIC16E,  "__18F6490",  { "pic18f6490",  "p18f6490",  "18f6490"  }, 0xa649, "18f6490.lkr"  },
  { pic18f6520,  PROC_CLASS_PIC16E,  "__18F6520",  { "pic18f6520",  "p18f6520",  "18f6520"  }, 0xa652, "18f6520.lkr"  },
  { pic18f6525,  PROC_CLASS_PIC16E,  "__18F6525",  { "pic18f6525",  "p18f6525",  "18f6525"  }, 0x6525, "18f6525.lkr"  },
  { pic18f6585,  PROC_CLASS_PIC16E,  "__18F6585",  { "pic18f6585",  "p18f6585",  "18f6585"  }, 0x6585, "18f6585.lkr"  },
  { pic18f6620,  PROC_CLASS_PIC16E,  "__18F6620",  { "pic18f6620",  "p18f6620",  "18f6620"  }, 0xa662, "18f6620.lkr"  },
  { pic18f6621,  PROC_CLASS_PIC16E,  "__18F6621",  { "pic18f6621",  "p18f6621",  "18f6621"  }, 0xa621, "18f6621.lkr"  },
  { pic18f6680,  PROC_CLASS_PIC16E,  "__18F6680",  { "pic18f6680",  "p18f6680",  "18f6680"  }, 0x6680, "18f6680.lkr"  },
  { pic18f6720,  PROC_CLASS_PIC16E,  "__18F6720",  { "pic18f6720",  "p18f6720",  "18f6720"  }, 0xa672, "18f6720.lkr"  },
  { pic18f8410,  PROC_CLASS_PIC16E,  "__18F8410",  { "pic18f8410",  "p18f8410",  "18f8410"  }, 0x8410, "18f8410.lkr"  },
  { pic18f8490,  PROC_CLASS_PIC16E,  "__18F8490",  { "pic18f8490",  "p18f8490",  "18f8490"  }, 0x8490, "18f8490.lkr"  },
  { pic18f8520,  PROC_CLASS_PIC16E,  "__18F8520",  { "pic18f8520",  "p18f8520",  "18f8520"  }, 0xa852, "18f8520.lkr"  },
  { pic18f8525,  PROC_CLASS_PIC16E,  "__18F8525",  { "pic18f8525",  "p18f8525",  "18f8525"  }, 0x8525, "18f8525.lkr"  },
  { pic18f8585,  PROC_CLASS_PIC16E,  "__18F8585",  { "pic18f8585",  "p18f8585",  "18f8585"  }, 0x8585, "18f8585.lkr"  },
  { pic18f8620,  PROC_CLASS_PIC16E,  "__18F8620",  { "pic18f8620",  "p18f8620",  "18f8620"  }, 0xa862, "18f8620.lkr"  },
  { pic18f8621,  PROC_CLASS_PIC16E,  "__18F8621",  { "pic18f8621",  "p18f8621",  "18f8621"  }, 0x8621, "18f8621.lkr"  },
  { pic18f8680,  PROC_CLASS_PIC16E,  "__18F8680",  { "pic18f8680",  "p18f8680",  "18f8680"  }, 0x8680, "18f8680.lkr"  },
  { pic18f8720,  PROC_CLASS_PIC16E,  "__18F8720",  { "pic18f8720",  "p18f8720",  "18f8720"  }, 0xa872, "18f8720.lkr"  },
  { rf509af,     PROC_CLASS_PIC12,   "__RF509AF",  { "rf509af",     "rf509af",   "rf509af"  }, 0x6509, "rf509af.lkr"  },
  { rf509ag,     PROC_CLASS_PIC12,   "__RF509AG",  { "rf509ag",     "rf509ag",   "rf509ag"  }, 0x7509, "rf509ag.lkr"  },
  { rf675f,      PROC_CLASS_PIC14,   "__RF675F",   { "rf675f",      "rf675f",    "rf675f"   }, 0x3675, "rf675f.lkr"   },
  { rf675h,      PROC_CLASS_PIC14,   "__RF675H",   { "rf675h",      "rf675h",    "rf675h"   }, 0x4675, "rf675h.lkr"   },
  { rf675k,      PROC_CLASS_PIC14,   "__RF675K",   { "rf675k",      "rf675k",    "rf675k"   }, 0x5675, "rf675k.lkr"   },
  { sx18,        PROC_CLASS_SX,      "__SX18",     { "sx18ac",      "sx18",      "sx18"     }, 0x0018, NULL           },
  { sx20,        PROC_CLASS_SX,      "__SX20",     { "sx20ac",      "sx20",      "sx20"     }, 0x0020, NULL           },
  { sx28,        PROC_CLASS_SX,      "__SX28",     { "sx28ac",      "sx28",      "sx28"     }, 0x0028, NULL           }
};

#define NUM_PICS	(sizeof(pics) / sizeof(pics[0]))

/*
 * Display a list of the processor names
 */
void gp_dump_processor_list(gp_boolean list_all, enum proc_class class)
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
    if (list_all || (pics[i].class == class)) { 
      length = strlen(pics[i].names[FAVORITE]);
      if (length > longest)
        longest = length;
    }
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

struct px *
gp_find_processor(char *name)
{
  int i, j;
  struct px *found = NULL;

  for (i = 0; i < NUM_PICS; i++) {
    for (j = 0; (j < MAX_NAMES) && (pics[i].names[j] != NULL); j++) {
      if (strcasecmp(name, pics[i].names[j]) == 0) {
        found = &pics[i];
      }
    }
  }

  return found;
}

enum proc_class
gp_processor_class(enum pic_processor processor)
{
  int i;
  enum proc_class class = PROC_CLASS_UNKNOWN;

  for (i = 0; i < NUM_PICS; i++) {
    if (pics[i].tag == processor) {
      class = pics[i].class;
    }
  }

  return class;
}

/* 18xx bsr boundary location */

int
gp_processor_bsr_boundary(enum pic_processor processor)
{
  int bsr_boundary = 0;

  switch(processor)
    {
    case pic18cxx:
    case pic18c242:
    case pic18c252:
    case pic18c442:
    case pic18c452:
    case pic18c601:
    case pic18c658:
    case pic18c801:
    case pic18c858:
    case pic18f1220:
    case pic18f1320:
    case pic18f2220:
    case pic18f2320:
    case pic18f2410:
    case pic18f242:
    case pic18f2420:
    case pic18f2439:
    case pic18f2455:
    case pic18f2510:
    case pic18f2515:
    case pic18f252:
    case pic18f2520:
    case pic18f2525:
    case pic18f2539:
    case pic18f2550:
    case pic18f2610:
    case pic18f2620:
    case pic18f4220:
    case pic18f4320:
    case pic18f4410:
    case pic18f442:
    case pic18f4420:
    case pic18f4439:
    case pic18f4455:
    case pic18f4510:
    case pic18f4515:
    case pic18f452:
    case pic18f4520:
    case pic18f4525:
    case pic18f4539:
    case pic18f4550:
    case pic18f4610:
    case pic18f4620:
    case pic18f6525:
    case pic18f6585:
    case pic18f6621:
    case pic18f6680:
    case pic18f8525:
    case pic18f8585:
    case pic18f8621:
    case pic18f8680:
    /* These are future products.  The bsr location isn't known. */
    case pic18f2480:
    case pic18f2580:
    case pic18f2681:
    case pic18f4480:
    case pic18f4580:
    case pic18f4681:
    case pic18f6490:
    case pic18f8490:
      bsr_boundary = 0x80;
      break;
    case pic18f2331:
    case pic18f2431:
    case pic18f248:
    case pic18f258:
    case pic18f2585:
    case pic18f2680:
    case pic18f4331:
    case pic18f4431:
    case pic18f448:
    case pic18f458:
    case pic18f4585:
    case pic18f4680:
    case pic18f6410:
    case pic18f6520:
    case pic18f6620:
    case pic18f6720:
    case pic18f8410:
    case pic18f8520:
    case pic18f8620:
    case pic18f8720:
      bsr_boundary = 0x60;
      break;
    default:
      assert(0);
    }

  return bsr_boundary;
}

unsigned long
gp_processor_coff_type(enum pic_processor processor)
{
  int i;
  unsigned long coff_type = 0;

  for (i = 0; i < NUM_PICS; i++) {
    if (pics[i].tag == processor) {
      coff_type = pics[i].coff_type;
    }
  }

  return coff_type;
}

enum pic_processor
gp_processor_coff_proc(unsigned long coff_type)
{
  int i;
  enum pic_processor processor = no_processor;

  for (i = 0; i < NUM_PICS; i++) {
    if (pics[i].coff_type == coff_type) {
      processor = pics[i].tag;
    }
  }

  return processor;
}

char *
gp_processor_name(enum pic_processor processor, unsigned int choice)
{
  int i;

  assert(!((choice < 0) || (choice > MAX_NAMES - 1)));

  for (i = 0; i < NUM_PICS; i++) {
    if (pics[i].tag == processor) {
      return pics[i].names[choice];
    }
  }

  return NULL;
}

char *
gp_processor_coff_name(unsigned long coff_type, unsigned int choice)
{
  int i;

  if (coff_type == 0)
    return NULL;

  assert(!((choice < 0) || (choice > MAX_NAMES - 1)));

  for (i = 0; i < NUM_PICS; i++) {
    if (pics[i].coff_type == coff_type) {
      return pics[i].names[choice];
    }
  }

  return NULL;
}

char *
gp_processor_script(enum pic_processor processor)
{
  int i;

  for (i = 0; i < NUM_PICS; i++) {
    if (pics[i].tag == processor) {
      return pics[i].script;
    }
  }

  return NULL;
}

int
gp_processor_rom_width(enum proc_class class)
{
  int rom_width;

  switch (class) {
  case PROC_CLASS_GENERIC:
  case PROC_CLASS_PIC12:
  case PROC_CLASS_SX:
    rom_width = 12;
    break;
  case PROC_CLASS_PIC14:
    rom_width = 14;
    break;
  case PROC_CLASS_PIC16:
    rom_width = 16;
    break;
  case PROC_CLASS_PIC16E:
    rom_width = 8;
    break;
  case PROC_CLASS_EEPROM8:
  default:
    assert(0);
  }

  return rom_width;
}

/* determine which page of program memory the address is located */
int 
gp_processor_check_page(enum proc_class class, int address)
{
  int page;

  switch (class) {
  case PROC_CLASS_EEPROM8:
    assert(0);
    break;
  case PROC_CLASS_GENERIC:
  case PROC_CLASS_PIC12:
  case PROC_CLASS_SX:
    if (address < 0x200) {
      page = 0;
    } else if (address < 0x400) {
      page = 1;
    } else if (address < 0x600) {
      page = 2;
    } else {
      page = 3;
    }
    break;
  case PROC_CLASS_PIC14:
    if (address < 0x800) {
      page = 0;
    } else if (address < 0x1000) {
      page = 1;
    } else if (address < 0x1800) {
      page = 2;
    } else {
      page = 3;
    }  
    break;
  case PROC_CLASS_PIC16:
    page = (address >> 8) & 0xff;
    break;
  case PROC_CLASS_PIC16E:
  default:
    assert(0);
  }

  return page;
}

/* determine which bank of data memory the address is located */
int 
gp_processor_check_bank(enum proc_class class, int address)
{
  int bank;

  switch (class) {
  case PROC_CLASS_EEPROM8:
    assert(0);
    break;
  case PROC_CLASS_GENERIC:
  case PROC_CLASS_PIC12:
  case PROC_CLASS_SX:
    bank = (address >> 5) & 0x3;
    break;
  case PROC_CLASS_PIC14:
    bank = (address >> 7) & 0x3;
    break;
  case PROC_CLASS_PIC16:
  case PROC_CLASS_PIC16E:
    bank = (address >> 8) & 0xff;
    break;
  default:
    assert(0);
  }

  return bank;
}

/* Set the page bits, return the number of instructions required. */

int 
gp_processor_set_page(enum proc_class class, 
                      int page,
                      MemBlock *m, 
                      int address)
{
  unsigned int data;
  int bcf_insn;
  int bsf_insn;
  int location;
  int page0;
  int page1;
  int count = 2;

  if ((class == PROC_CLASS_EEPROM8) || (class == PROC_CLASS_PIC16E)) {
    assert(0);
  } else if (class == PROC_CLASS_PIC16) {
    /* movlw <page> */
    data = MEM_USED_MASK | (0xb000 | (page & 0xff));
    i_memory_put(m, address, data);
    /* movwf 0x3 */
    data = MEM_USED_MASK | 0x0100 | 0x3;
    i_memory_put(m, address + 1, data);
  } else {
    if (class == PROC_CLASS_PIC14) {
      bcf_insn = MEM_USED_MASK | 0x1000;
      bsf_insn = MEM_USED_MASK | 0x1400;
      location = 0xa;
      page0 = 3 << 7;
      page1 = 4 << 7;    
    } else {
      bcf_insn = MEM_USED_MASK | 0x400;
      bsf_insn = MEM_USED_MASK | 0x500;
      location = 0x3;
      page0 = 5 << 5;
      page1 = 6 << 5;    
    }

    switch(page) {
    case 0:
      /* bcf location, page0 */
      data = bcf_insn | page0 | location;
      i_memory_put(m, address, data);
      /* bcf location, page1 */
      data = bcf_insn | page1 | location;
      i_memory_put(m, address + 1, data);
      break;
    case 1:
      /* bsf location, page0 */
      data = bsf_insn | page0 | location;
      i_memory_put(m, address, data);
      /* bcf location, page1 */
      data = bcf_insn | page1 | location;
      i_memory_put(m, address + 1, data);
      break;
    case 2:
      /* bcf location, page0 */
      data = bcf_insn | page0 | location;
      i_memory_put(m, address, data);
      /* bsf location, page1 */
      data = bsf_insn | page1 | location;
      i_memory_put(m, address + 1, data);
      break;
    case 3:
      /* bsf location, page0 */
      data = bsf_insn | page0 | location;
      i_memory_put(m, address, data);
      /* bsf location, page1 */
      data = bsf_insn | page1 | location;
      i_memory_put(m, address + 1, data);
      break;
    default:
      assert(0);
      break;  
    }
  }

  return count;
}

/* Set the bank bits, return the number of instructions required. */

int 
gp_processor_set_bank(enum proc_class class, 
                      int bank,
                      MemBlock *m, 
                      int address)
{
  unsigned int data;
  int bcf_insn;
  int bsf_insn;
  int location;
  int bank0;
  int bank1;
  int count;

  if (class == PROC_CLASS_EEPROM8) {
    assert(0);
  } else if (class == PROC_CLASS_PIC16E) {
    /* movlb bank */
    data = MEM_USED_MASK | 0x0100 | (bank & 0xff);
    i_memory_put(m, address, data);
    count = 1;
  } else if (class == PROC_CLASS_PIC16) {
    /* movlb bank */
    data = MEM_USED_MASK | 0xb800 | (bank & 0xff);
    i_memory_put(m, address, data);
    count = 1;
  } else {
    if (class == PROC_CLASS_PIC14) {
      bcf_insn = MEM_USED_MASK | 0x1000;
      bsf_insn = MEM_USED_MASK | 0x1400;
      location = 0x3;
      bank0 = 5 << 7;
      bank1 = 6 << 7;    
    } else {
      bcf_insn = MEM_USED_MASK | 0x400;
      bsf_insn = MEM_USED_MASK | 0x500;
      location = 0x4;
      bank0 = 5 << 5;
      bank1 = 6 << 5;    
    }

    switch(bank) {
    case 0:
      /* bcf location, bank0 */
      data = bcf_insn | bank0 | location;
      i_memory_put(m, address, data);
      /* bcf location, bank1 */
      data = bcf_insn | bank1 | location;
      i_memory_put(m, address + 1, data);
      break;
    case 1:
      /* bsf location, bank0 */
      data = bsf_insn | bank0 | location;
      i_memory_put(m, address, data);
      /* bcf location, bank1 */
      data = bcf_insn | bank1 | location;
      i_memory_put(m, address + 1, data);
      break;
    case 2:
      /* bcf location, bank0 */
      data = bcf_insn | bank0 | location;
      i_memory_put(m, address, data);
      /* bsf location, bank1 */
      data = bsf_insn | bank1 | location;
      i_memory_put(m, address + 1, data);
      break;
    case 3:
      /* bcf location, bank0 */
      data = bsf_insn | bank0 | location;
      i_memory_put(m, address, data);
      /* bcf location, bank1 */
      data = bsf_insn | bank1 | location;
      i_memory_put(m, address + 1, data);
      break;
    default:
      assert(0);
      break;  
    }

    count = 2;
  }

  return count;
}

/* determine the value for retlw */

int 
gp_processor_retlw(enum proc_class class)
{
  int insn;

  switch(class) {
  case PROC_CLASS_PIC12:
  case PROC_CLASS_SX:
    insn = 0x800;
    break;
  case PROC_CLASS_PIC14:
    insn = 0x3400;
    break;
  case PROC_CLASS_PIC16:
    insn = 0xb600;
    break;
  case PROC_CLASS_PIC16E:
    insn = 0x0c00;
    break;
  default:
    assert(0);
  }  

  return insn;
}

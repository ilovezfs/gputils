/*
 * This code is automatically generated from the 8bit_device.info file.
 * This data is in the public domain. Use care in modifying.
 */

/*
 * since caches are enabled, directive and option structures may seem to include
 * references to other PICs. this is done by caching when two or more PICs contain
 * identical data.
 */

#include "stdhdr.h"
#include "gpcfg.h"

static const struct gp_cfg_option mcp25020_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x03 },
};

static const struct gp_cfg_option pic10f200_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0x08 },
};

static const struct gp_cfg_option pic10f200_mclre_opts[] =
{ /* for MCLRE: */
	{ "OFF",       0x00 },
	{ "ON",        0x10 },
};

static const struct gp_cfg_option pic10f200_osc_opts[] =
{ /* for OSC: */
	{ "IntRC",     0x01 },
};

static const struct gp_cfg_option pic10f200_wdte_opts[] =
{ /* for WDTE: */
	{ "OFF",       0x00 },
	{ "ON",        0x04 },
};

static const struct gp_cfg_option pic10f220_ioscfs_opts[] =
{ /* for IOSCFS: */
	{ "4MHZ",      0x00 },
	{ "8MHZ",      0x01 },
};

static const struct gp_cfg_option pic10f220_mcpu_opts[] =
{ /* for MCPU: */
	{ "ON",        0x00 },
	{ "OFF",       0x02 },
};

static const struct gp_cfg_option pic10f320_boren_opts[] =
{ /* for BOREN: */
	{ "OFF",       0x00 },
	{ "SBODEN",    0x02 },
	{ "NSLEEP",    0x04 },
	{ "ON",        0x06 },
};

static const struct gp_cfg_option pic10f320_borv_opts[] =
{ /* for BORV: */
	{ "HI",        0x00 },
	{ "LO",        0x400 },
};

static const struct gp_cfg_option pic10f320_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0x80 },
};

static const struct gp_cfg_option pic10f320_fosc_opts[] =
{ /* for FOSC: */
	{ "INTOSC",    0x00 },
	{ "EC",        0x01 },
};

static const struct gp_cfg_option pic10f320_lpbor_opts[] =
{ /* for LPBOR: */
	{ "OFF",       0x00 },
	{ "ON",        0x200 },
};

static const struct gp_cfg_option pic10f320_lvp_opts[] =
{ /* for LVP: */
	{ "OFF",       0x00 },
	{ "ON",        0x100 },
};

static const struct gp_cfg_option pic10f320_mclre_opts[] =
{ /* for MCLRE: */
	{ "OFF",       0x00 },
	{ "ON",        0x40 },
};

static const struct gp_cfg_option pic10f320_pwrte_opts[] =
{ /* for PWRTE: */
	{ "ON",        0x00 },
	{ "OFF",       0x20 },
};

static const struct gp_cfg_option pic10f320_wdte_opts[] =
{ /* for WDTE: */
	{ "OFF",       0x00 },
	{ "SWDTEN",    0x08 },
	{ "NSLEEP",    0x10 },
	{ "ON",        0x18 },
};

static const struct gp_cfg_option pic10f320_wrt_opts[] =
{ /* for WRT: */
	{ "ALL",       0x00 },
	{ "HALF",      0x800 },
	{ "BOOT",      0x1000 },
	{ "OFF",       0x1800 },
};

static const struct gp_cfg_option pic12c508_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "IntRC",     0x02 },
	{ "ExtRC",     0x03 },
};

static const struct gp_cfg_option pic12c671_cp_opts[] =
{ /* for CP: */
	{ "ALL",       0x00 },
	{ "75",        0x1520 },
	{ "50",        0x2a40 },
	{ "OFF",       0x3f60 },
};

static const struct gp_cfg_option pic12c671_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "INTRCIO",   0x04 },
	{ "INTRCCLK",  0x05 },
	{ "EXTRCIO",   0x06 },
	{ "EXTRCCLK",  0x07 },
};

static const struct gp_cfg_option pic12c671_mclre_opts[] =
{ /* for MCLRE: */
	{ "OFF",       0x00 },
	{ "ON",        0x80 },
};

static const struct gp_cfg_option pic12c671_pwrte_opts[] =
{ /* for PWRTE: */
	{ "ON",        0x00 },
	{ "OFF",       0x10 },
};

static const struct gp_cfg_option pic12c671_wdte_opts[] =
{ /* for WDTE: */
	{ "OFF",       0x00 },
	{ "ON",        0x08 },
};

static const struct gp_cfg_option pic12f1501_boren_opts[] =
{ /* for BOREN: */
	{ "OFF",       0x00 },
	{ "SBODEN",    0x200 },
	{ "NSLEEP",    0x400 },
	{ "ON",        0x600 },
};

static const struct gp_cfg_option pic12f1501_clkouten_opts[] =
{ /* for CLKOUTEN: */
	{ "ON",        0x00 },
	{ "OFF",       0x800 },
};

static const struct gp_cfg_option pic12f1501_fosc_opts[] =
{ /* for FOSC: */
	{ "INTOSC",    0x00 },
	{ "ECL",       0x01 },
	{ "ECM",       0x02 },
	{ "ECH",       0x03 },
};

static const struct gp_cfg_option pic12f1501_lvp_opts[] =
{ /* for LVP: */
	{ "OFF",       0x00 },
	{ "ON",        0x2000 },
};

static const struct gp_cfg_option pic12f1501_wrt_opts[] =
{ /* for WRT: */
	{ "ALL",       0x00 },
	{ "HALF",      0x01 },
	{ "BOOT",      0x02 },
	{ "OFF",       0x03 },
};

static const struct gp_cfg_option pic12f1612_wdtccs_opts[] =
{ /* for WDTCCS: */
	{ "LFINTOSC",  0x00 },
	{ "MFINTOSC",  0x800 },
	{ "SWC",       0x3800 },
};

static const struct gp_cfg_option pic12f1612_wdtcps_opts[] =
{ /* for WDTCPS: */
	{ "WDTCPS0",   0x00 },
	{ "WDTCPS1",   0x01 },
	{ "WDTCPS2",   0x02 },
	{ "WDTCPS3",   0x03 },
	{ "WDTCPS4",   0x04 },
	{ "WDTCPS5",   0x05 },
	{ "WDTCPS6",   0x06 },
	{ "WDTCPS7",   0x07 },
	{ "WDTCPS8",   0x08 },
	{ "WDTCPS9",   0x09 },
	{ "WDTCPSA",   0x0a },
	{ "WDTCPSB",   0x0b },
	{ "WDTCPSC",   0x0c },
	{ "WDTCPSD",   0x0d },
	{ "WDTCPSE",   0x0e },
	{ "WDTCPSF",   0x0f },
	{ "WDTCPS10",  0x10 },
	{ "WDTCPS11",  0x11 },
	{ "WDTCPS12",  0x12 },
	{ "WDTCPS1F",  0x1f },
};

static const struct gp_cfg_option pic12f1612_wdtcws_opts[] =
{ /* for WDTCWS: */
	{ "WDTCWS125", 0x00 },
	{ "WDTCWS25",  0x100 },
	{ "WDTCWS375", 0x200 },
	{ "WDTCWS50",  0x300 },
	{ "WDTCWS625", 0x400 },
	{ "WDTCWS75",  0x500 },
	{ "WDTCWS100", 0x600 },
	{ "WDTCWSSW",  0x700 },
};

static const struct gp_cfg_option pic12f1612_wdte_opts[] =
{ /* for WDTE: */
	{ "OFF",       0x00 },
	{ "SWDTEN",    0x20 },
	{ "NSLEEP",    0x40 },
	{ "ON",        0x60 },
};

static const struct gp_cfg_option pic12f1822_cpd_opts[] =
{ /* for CPD: */
	{ "ON",        0x00 },
	{ "OFF",       0x100 },
};

static const struct gp_cfg_option pic12f1822_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EXTRC",     0x03 },
	{ "INTOSC",    0x04 },
	{ "ECL",       0x05 },
	{ "ECM",       0x06 },
	{ "ECH",       0x07 },
};

static const struct gp_cfg_option pic12f1822_ieso_opts[] =
{ /* for IESO: */
	{ "OFF",       0x00 },
	{ "ON",        0x1000 },
};

static const struct gp_cfg_option pic12f510_ioscfs_opts[] =
{ /* for IOSCFS: */
	{ "OFF",       0x00 },
	{ "ON",        0x20 },
};

static const struct gp_cfg_option pic12f519_cpdf_opts[] =
{ /* for CPDF: */
	{ "ON",        0x00 },
	{ "OFF",       0x40 },
};

static const struct gp_cfg_option pic12f519_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "INTRC",     0x02 },
	{ "EXTRC",     0x03 },
};

static const struct gp_cfg_option pic12f519_ioscfs_opts[] =
{ /* for IOSCFS: */
	{ "4MHz",      0x00 },
	{ "8MHz",      0x20 },
};

static const struct gp_cfg_option pic12f520_ioscfs_opts[] =
{ /* for IOSCFS: */
	{ "4MHZ",      0x00 },
	{ "8MHZ",      0x20 },
};

static const struct gp_cfg_option pic12f529t39a_cp_opts[] =
{ /* for CP: */
	{ "ENABLE",    0x100 },
	{ "DISABLE",   0x580 },
};

static const struct gp_cfg_option pic12f529t39a_parity_opts[] =
{ /* for PARITY: */
	{ "CLEAR",     0x00 },
	{ "SET",       0x08 },
};

static const struct gp_cfg_option pic12f609_boren_opts[] =
{ /* for BOREN: */
	{ "OFF",       0x00 },
	{ "NSLEEP",    0x200 },
	{ "ON",        0x300 },
};

static const struct gp_cfg_option pic12f609_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EC",        0x03 },
	{ "INTOSCIO",  0x04 },
	{ "INTOSCCLK", 0x05 },
	{ "EXTRCIO",   0x06 },
	{ "EXTRCCLK",  0x07 },
};

static const struct gp_cfg_option pic12f609_ioscfs_opts[] =
{ /* for IOSCFS: */
	{ "4MHZ",      0x00 },
	{ "8MHZ",      0x80 },
};

static const struct gp_cfg_option pic12f617_wrt_opts[] =
{ /* for WRT: */
	{ "ALL",       0x00 },
	{ "HALF",      0x400 },
	{ "BOOT",      0x800 },
	{ "OFF",       0xc00 },
};

static const struct gp_cfg_option pic12f629_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EC",        0x03 },
	{ "INTRCIO",   0x04 },
	{ "INTRCCLK",  0x05 },
	{ "EXTRCIO",   0x06 },
	{ "EXTRCCLK",  0x07 },
};

static const struct gp_cfg_option pic12f635_boren_opts[] =
{ /* for BOREN: */
	{ "OFF",       0x00 },
	{ "SBODEN",    0x100 },
	{ "NSLEEP",    0x200 },
	{ "ON",        0x300 },
};

static const struct gp_cfg_option pic12f635_fcmen_opts[] =
{ /* for FCMEN: */
	{ "OFF",       0x00 },
	{ "ON",        0x800 },
};

static const struct gp_cfg_option pic12f635_ieso_opts[] =
{ /* for IESO: */
	{ "OFF",       0x00 },
	{ "ON",        0x400 },
};

static const struct gp_cfg_option pic12f635_wure_opts[] =
{ /* for WURE: */
	{ "ON",        0x00 },
	{ "OFF",       0x1000 },
};

static const struct gp_cfg_option pic12f752_boren_opts[] =
{ /* for BOREN: */
	{ "DIS",       0x00 },
	{ "SLEEP_DIS", 0x200 },
	{ "EN",        0x300 },
};

static const struct gp_cfg_option pic12f752_fosc0_opts[] =
{ /* for FOSC0: */
	{ "INT",       0x00 },
	{ "EC",        0x01 },
};

static const struct gp_cfg_option pic12f752_wrt_opts[] =
{ /* for WRT: */
	{ "ALL",       0x00 },
	{ "HALF",      0x400 },
	{ "FOURTH",    0x800 },
	{ "OFF",       0xc00 },
};

static const struct gp_cfg_option pic16c432_cp_opts[] =
{ /* for CP: */
	{ "ALL",       0x00 },
	{ "75",        0x1510 },
	{ "50",        0x2a20 },
	{ "OFF",       0x3f30 },
};

static const struct gp_cfg_option pic16c432_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "RC",        0x03 },
};

static const struct gp_cfg_option pic16c433_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EXTCLK",    0x03 },
	{ "INTRCIO",   0x04 },
	{ "INTRCCLK",  0x05 },
	{ "EXTRCIO",   0x06 },
	{ "EXTRCCLK",  0x07 },
};

static const struct gp_cfg_option pic16c505_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0xfd0 },
};

static const struct gp_cfg_option pic16c505_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "IntRC_RB4EN", 0x04 },
	{ "IntRC_CLKOUTEN", 0x05 },
	{ "ExtRC_RB4EN", 0x06 },
	{ "ExtRC_CLKOUTEN", 0x07 },
};

static const struct gp_cfg_option pic16c54c_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0xff8 },
};

static const struct gp_cfg_option pic16c554_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0x3f30 },
};

static const struct gp_cfg_option pic16c621_cp_opts[] =
{ /* for CP: */
	{ "ALL",       0x00 },
	{ "50",        0x1510 },
	{ "OFF",       0x3f30 },
};

static const struct gp_cfg_option pic16c710_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0x3fb0 },
};

static const struct gp_cfg_option pic16c717_bodenv_opts[] =
{ /* for BODENV: */
	{ "45",        0x00 },
	{ "42",        0x400 },
	{ "27",        0x800 },
	{ "25",        0xc00 },
};

static const struct gp_cfg_option pic16c717_cp_opts[] =
{ /* for CP: */
	{ "ALL",       0x00 },
	{ "OFF",       0x3300 },
};

static const struct gp_cfg_option pic16c717_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EXTCLK",    0x03 },
	{ "INTRCIO",   0x04 },
	{ "INTRCCLK",  0x05 },
	{ "ER_NOCLKOUT", 0x06 },
	{ "ER_CLKOUT", 0x07 },
};

static const struct gp_cfg_option pic16c745_fosc_opts[] =
{ /* for FOSC: */
	{ "HS",        0x00 },
	{ "EC",        0x01 },
	{ "H4",        0x02 },
	{ "E4",        0x03 },
};

static const struct gp_cfg_option pic16c773_cp_opts[] =
{ /* for CP: */
	{ "ALL",       0x00 },
	{ "75",        0x1110 },
	{ "50",        0x2220 },
	{ "OFF",       0x3330 },
};

static const struct gp_cfg_option pic16c925_cp_opts[] =
{ /* for CP: */
	{ "ALL",       0x00 },
	{ "75",        0x10 },
	{ "50",        0x20 },
	{ "OFF",       0x30 },
};

static const struct gp_cfg_option pic16cr83_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0x3f70 },
};

static const struct gp_cfg_option pic16f1454_cpudiv_opts[] =
{ /* for CPUDIV: */
	{ "NOCLKDIV",  0x00 },
	{ "CLKDIV2",   0x10 },
	{ "CLKDIV3",   0x20 },
	{ "CLKDIV6",   0x30 },
};

static const struct gp_cfg_option pic16f1454_pllen_opts[] =
{ /* for PLLEN: */
	{ "DISABLED",  0x00 },
	{ "ENABLED",   0x100 },
};

static const struct gp_cfg_option pic16f1454_pllmult_opts[] =
{ /* for PLLMULT: */
	{ "4x",        0x00 },
	{ "3x",        0x80 },
};

static const struct gp_cfg_option pic16f1454_usblsclk_opts[] =
{ /* for USBLSCLK: */
	{ "24MHz",     0x00 },
	{ "48MHz",     0x40 },
};

static const struct gp_cfg_option pic16f18313_boren_opts[] =
{ /* for BOREN: */
	{ "OFF",       0x00 },
	{ "SBOREN",    0x40 },
	{ "SLEEP",     0x80 },
	{ "ON",        0xc0 },
};

static const struct gp_cfg_option pic16f18313_borv_opts[] =
{ /* for BORV: */
	{ "HIGH",      0x00 },
	{ "LOW",       0x200 },
};

static const struct gp_cfg_option pic16f18313_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0x01 },
};

static const struct gp_cfg_option pic16f18313_debug_opts[] =
{ /* for DEBUG: */
	{ "ON",        0x00 },
	{ "OFF",       0x2000 },
};

static const struct gp_cfg_option pic16f18313_fextosc_opts[] =
{ /* for FEXTOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "Unimplemented", 0x03 },
	{ "OFF",       0x04 },
	{ "ECL",       0x05 },
	{ "ECM",       0x06 },
	{ "ECH",       0x07 },
};

static const struct gp_cfg_option pic16f18313_mclre_opts[] =
{ /* for MCLRE: */
	{ "OFF",       0x00 },
	{ "ON",        0x01 },
};

static const struct gp_cfg_option pic16f18313_rstosc_opts[] =
{ /* for RSTOSC: */
	{ "HFINT32",   0x00 },
	{ "EXT4X",     0x10 },
	{ "EXT2X",     0x20 },
	{ "SOSC",      0x30 },
	{ "LFINT",     0x40 },
	{ "Unimplemented", 0x50 },
	{ "HFINT1",    0x60 },
	{ "EXT1X",     0x70 },
};

static const struct gp_cfg_option pic16f18313_wdte_opts[] =
{ /* for WDTE: */
	{ "OFF",       0x00 },
	{ "SWDTEN",    0x04 },
	{ "SLEEP",     0x08 },
	{ "ON",        0x0c },
};

static const struct gp_cfg_option pic16f1933_vcapen_opts[] =
{ /* for VCAPEN: */
	{ "RA0",       0x00 },
	{ "RA5",       0x10 },
	{ "RA6",       0x20 },
	{ "OFF",       0x30 },
};

static const struct gp_cfg_option pic16f505_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EC",        0x03 },
	{ "IntRC_RB4EN", 0x04 },
	{ "IntRC_CLKOUTEN", 0x05 },
	{ "ExtRC_RB4EN", 0x06 },
	{ "ExtRC_CLKOUTEN", 0x07 },
};

static const struct gp_cfg_option pic16f526_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EC",        0x03 },
	{ "INTRC_RB4", 0x04 },
	{ "INTRC_CLKOUT", 0x05 },
	{ "ExtRC_RB4", 0x06 },
	{ "ExtRC_CLKOUT", 0x07 },
};

static const struct gp_cfg_option pic16f526_ioscfs_opts[] =
{ /* for IOSCFS: */
	{ "4MHz",      0x00 },
	{ "8MHz",      0x40 },
};

static const struct gp_cfg_option pic16f527_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EC",        0x03 },
	{ "INTRC_IO",  0x04 },
	{ "INTRC_CLKOUT", 0x05 },
	{ "EXTRC_IO",  0x06 },
	{ "EXTRC_CLKOUT", 0x07 },
};

static const struct gp_cfg_option pic16f627_cp_opts[] =
{ /* for CP: */
	{ "ALL",       0x00 },
	{ "75",        0x1400 },
	{ "50",        0x2800 },
	{ "OFF",       0x3c00 },
};

static const struct gp_cfg_option pic16f627_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "ECIO",      0x03 },
	{ "INTOSCIO",  0x10 },
	{ "INTOSCCLK", 0x11 },
	{ "ERIO",      0x12 },
	{ "ERCLK",     0x13 },
};

static const struct gp_cfg_option pic16f627a_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "ECIO",      0x03 },
	{ "INTOSCIO",  0x10 },
	{ "INTOSCCLK", 0x11 },
	{ "EXTRCIO",   0x12 },
	{ "EXTRCCLK",  0x13 },
};

static const struct gp_cfg_option pic16f707_borv_opts[] =
{ /* for BORV: */
	{ "25",        0x00 },
	{ "19",        0x400 },
};

static const struct gp_cfg_option pic16f707_fosc_opts[] =
{ /* for FOSC: */
	{ "LP_OSC",    0x00 },
	{ "XT_OSC",    0x01 },
	{ "HS_OSC",    0x02 },
	{ "EC_OSC",    0x03 },
	{ "INTOSCIO",  0x04 },
	{ "INTOSC",    0x05 },
	{ "EXTRCIO",   0x06 },
	{ "EXTRC",     0x07 },
};

static const struct gp_cfg_option pic16f716_bodenv_opts[] =
{ /* for BODENV: */
	{ "25",        0x00 },
	{ "40",        0x80 },
};

static const struct gp_cfg_option pic16f720_fosc_opts[] =
{ /* for FOSC: */
	{ "INTOSCIO",  0x00 },
	{ "INTOSCCLK", 0x01 },
	{ "ECIO",      0x02 },
	{ "ECCLK",     0x03 },
};

static const struct gp_cfg_option pic16f720_wrten_opts[] =
{ /* for WRTEN: */
	{ "FULL",      0x00 },
	{ "HALF",      0x01 },
	{ "BOOT",      0x02 },
	{ "OFF",       0x03 },
};

static const struct gp_cfg_option pic16f722_vcapen_opts[] =
{ /* for VCAPEN: */
	{ "RA0",       0x00 },
	{ "RA5",       0x10 },
	{ "RA6",       0x20 },
	{ "DIS",       0x30 },
};

static const struct gp_cfg_option pic16f737_borv_opts[] =
{ /* for BORV: */
	{ "45",        0x00 },
	{ "42",        0x80 },
	{ "27",        0x100 },
	{ "20",        0x180 },
};

static const struct gp_cfg_option pic16f737_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "RB3",       0x00 },
	{ "RC1",       0x1000 },
};

static const struct gp_cfg_option pic16f737_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EC",        0x03 },
	{ "INTOSCIO",  0x10 },
	{ "INTOSCCLK", 0x11 },
	{ "EXTRCIO",   0x12 },
	{ "EXTRCCLK",  0x13 },
};

static const struct gp_cfg_option pic16f737_ieso_opts[] =
{ /* for IESO: */
	{ "OFF",       0x00 },
	{ "ON",        0x02 },
};

static const struct gp_cfg_option pic16f818_ccpmx_opts[] =
{ /* for CCPMX: */
	{ "RB3",       0x00 },
	{ "RB2",       0x1000 },
};

static const struct gp_cfg_option pic16f818_wrt_opts[] =
{ /* for WRT: */
	{ "1024",      0x200 },
	{ "512",       0x400 },
	{ "OFF",       0x600 },
};

static const struct gp_cfg_option pic16f819_wrt_opts[] =
{ /* for WRT: */
	{ "1536",      0x00 },
	{ "1024",      0x200 },
	{ "512",       0x400 },
	{ "OFF",       0x600 },
};

static const struct gp_cfg_option pic16f83_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0x3ff0 },
};

static const struct gp_cfg_option pic16f83_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EXTRC",     0x03 },
};

static const struct gp_cfg_option pic16f870_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0x3030 },
};

static const struct gp_cfg_option pic16f870_wrt_opts[] =
{ /* for WRT: */
	{ "OFF",       0x00 },
	{ "ALL",       0x200 },
};

static const struct gp_cfg_option pic16f873_cp_opts[] =
{ /* for CP: */
	{ "ALL",       0x00 },
	{ "HALF",      0x1010 },
	{ "UPPER_256", 0x2020 },
	{ "OFF",       0x3030 },
};

static const struct gp_cfg_option pic16f873a_wrt_opts[] =
{ /* for WRT: */
	{ "HALF",      0x00 },
	{ "1FOURTH",   0x200 },
	{ "256",       0x400 },
	{ "OFF",       0x600 },
};

static const struct gp_cfg_option pic16f87_ccpmx_opts[] =
{ /* for CCPMX: */
	{ "RB3",       0x00 },
	{ "RB0",       0x1000 },
};

static const struct gp_cfg_option pic16f87_wrt_opts[] =
{ /* for WRT: */
	{ "ALL",       0x00 },
	{ "2048",      0x200 },
	{ "256",       0x400 },
	{ "OFF",       0x600 },
};

static const struct gp_cfg_option pic16f882_bor4v_opts[] =
{ /* for BOR4V: */
	{ "BOR21V",    0x00 },
	{ "BOR40V",    0x100 },
};

static const struct gp_cfg_option pic16f882_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EC",        0x03 },
	{ "INTRC_NOCLKOUT", 0x04 },
	{ "INTRC_CLKOUT", 0x05 },
	{ "EXTRC_NOCLKOUT", 0x06 },
	{ "EXTRC_CLKOUT", 0x07 },
};

static const struct gp_cfg_option pic16f882_wrt_opts[] =
{ /* for WRT: */
	{ "HALF",      0x00 },
	{ "1FOURTH",   0x200 },
	{ "OFF",       0x600 },
};

static const struct gp_cfg_option pic18c242_borv_opts[] =
{ /* for BORV: */
	{ "45",        0x00 },
	{ "42",        0x04 },
	{ "27",        0x08 },
	{ "25",        0x0c },
};

static const struct gp_cfg_option pic18c242_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0xff },
};

static const struct gp_cfg_option pic18c242_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "RC",        0x03 },
	{ "EC",        0x04 },
	{ "ECIO",      0x05 },
	{ "HSPLL",     0x06 },
	{ "RCIO",      0x07 },
};

static const struct gp_cfg_option pic18c242_wdtps_opts[] =
{ /* for WDTPS: */
	{ "1",         0x00 },
	{ "2",         0x02 },
	{ "4",         0x04 },
	{ "8",         0x06 },
	{ "16",        0x08 },
	{ "32",        0x0a },
	{ "64",        0x0c },
	{ "128",       0x0e },
};

static const struct gp_cfg_option pic18c601_bw_opts[] =
{ /* for BW: */
	{ "8",         0x00 },
	{ "16",        0x40 },
};

static const struct gp_cfg_option pic18c601_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "EC",        0x01 },
	{ "HS",        0x02 },
	{ "RC",        0x03 },
};

static const struct gp_cfg_option pic18f1220_borv_opts[] =
{ /* for BORV: */
	{ "45",        0x00 },
	{ "42",        0x04 },
	{ "27",        0x08 },
};

static const struct gp_cfg_option pic18f1220_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "EC",        0x04 },
	{ "ECIO",      0x05 },
	{ "HSPLL",     0x06 },
	{ "RCIO",      0x07 },
	{ "INTIO2",    0x08 },
	{ "INTIO1",    0x09 },
	{ "RC",        0x0c },
};

static const struct gp_cfg_option pic18f1220_wdtps_opts[] =
{ /* for WDTPS: */
	{ "1",         0x00 },
	{ "2",         0x02 },
	{ "4",         0x04 },
	{ "8",         0x06 },
	{ "16",        0x08 },
	{ "32",        0x0a },
	{ "64",        0x0c },
	{ "128",       0x0e },
	{ "256",       0x10 },
	{ "512",       0x12 },
	{ "1024",      0x14 },
	{ "2048",      0x16 },
	{ "4096",      0x18 },
	{ "8192",      0x1a },
	{ "16384",     0x1c },
	{ "32768",     0x1e },
};

static const struct gp_cfg_option pic18f1230_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "BB256",     0x00 },
	{ "BB512",     0x30 },
};

static const struct gp_cfg_option pic18f1230_bor_opts[] =
{ /* for BOR: */
	{ "OFF",       0x00 },
	{ "SBORENCTRL", 0x02 },
	{ "BOACTIVE",  0x04 },
	{ "BOHW",      0x06 },
};

static const struct gp_cfg_option pic18f1230_borv_opts[] =
{ /* for BORV: */
	{ "0",         0x00 },
	{ "1",         0x08 },
	{ "2",         0x10 },
	{ "3",         0x18 },
};

static const struct gp_cfg_option pic18f1230_fltamx_opts[] =
{ /* for FLTAMX: */
	{ "RA7",       0x00 },
	{ "RA5",       0x01 },
};

static const struct gp_cfg_option pic18f1230_hpol_opts[] =
{ /* for HPOL: */
	{ "LOW",       0x00 },
	{ "HIGH",      0x08 },
};

static const struct gp_cfg_option pic18f1230_lpol_opts[] =
{ /* for LPOL: */
	{ "LOW",       0x00 },
	{ "HIGH",      0x04 },
};

static const struct gp_cfg_option pic18f1230_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "RC",        0x03 },
	{ "EC",        0x04 },
	{ "ECIO",      0x05 },
	{ "HSPLL",     0x06 },
	{ "RCIO",      0x07 },
	{ "INTIO2",    0x08 },
	{ "INTIO1",    0x09 },
};

static const struct gp_cfg_option pic18f1330_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "BB256",     0x00 },
	{ "BB512",     0x10 },
	{ "BB1K",      0x30 },
};

static const struct gp_cfg_option pic18f13k22_boren_opts[] =
{ /* for BOREN: */
	{ "OFF",       0x00 },
	{ "ON",        0x02 },
	{ "NOSLP",     0x04 },
	{ "SBORDIS",   0x06 },
};

static const struct gp_cfg_option pic18f13k22_borv_opts[] =
{ /* for BORV: */
	{ "30",        0x00 },
	{ "27",        0x08 },
	{ "22",        0x10 },
	{ "19",        0x18 },
};

static const struct gp_cfg_option pic18f13k22_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "ERCCLKOUT", 0x03 },
	{ "ECCLKOUTH", 0x04 },
	{ "ECH",       0x05 },
	{ "ERC",       0x07 },
	{ "IRC",       0x08 },
	{ "IRCCLKOUT", 0x09 },
	{ "ECCLKOUTM", 0x0a },
	{ "ECM",       0x0b },
	{ "ECCLKOUTL", 0x0c },
	{ "ECL",       0x0d },
};

static const struct gp_cfg_option pic18f13k50_cpudiv_opts[] =
{ /* for CPUDIV: */
	{ "NOCLKDIV",  0x00 },
	{ "CLKDIV2",   0x08 },
	{ "CLKDIV3",   0x10 },
	{ "CLKDIV4",   0x18 },
};

static const struct gp_cfg_option pic18f2220_borv_opts[] =
{ /* for BORV: */
	{ "45",        0x00 },
	{ "42",        0x04 },
	{ "27",        0x08 },
	{ "20",        0x0c },
};

static const struct gp_cfg_option pic18f2220_pbad_opts[] =
{ /* for PBAD: */
	{ "DIG",       0x00 },
	{ "ANA",       0x02 },
};

static const struct gp_cfg_option pic18f2221_bor_opts[] =
{ /* for BOR: */
	{ "OFF",       0x00 },
	{ "SOFT",      0x02 },
	{ "NOSLP",     0x04 },
	{ "ON",        0x06 },
};

static const struct gp_cfg_option pic18f2221_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "RB3",       0x00 },
	{ "RC1",       0x01 },
};

static const struct gp_cfg_option pic18f2320_cp2_opts[] =
{ /* for CP2: */
	{ "ON",        0x00 },
	{ "OFF",       0x04 },
};

static const struct gp_cfg_option pic18f2331_hpol_opts[] =
{ /* for HPOL: */
	{ "LOW",       0x00 },
	{ "HIGH",      0x10 },
};

static const struct gp_cfg_option pic18f2331_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "RC2",       0x03 },
	{ "EC",        0x04 },
	{ "ECIO",      0x05 },
	{ "HSPLL",     0x06 },
	{ "RCIO",      0x07 },
	{ "IRCIO",     0x08 },
	{ "IRC",       0x09 },
	{ "RC1",       0x0a },
	{ "RC",        0x0c },
};

static const struct gp_cfg_option pic18f23k20_borv_opts[] =
{ /* for BORV: */
	{ "30",        0x00 },
	{ "27",        0x08 },
	{ "22",        0x10 },
	{ "18",        0x18 },
};

static const struct gp_cfg_option pic18f23k20_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "PORTBE",    0x00 },
	{ "PORTC",     0x01 },
};

static const struct gp_cfg_option pic18f23k20_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "RC",        0x03 },
	{ "EC",        0x04 },
	{ "ECIO6",     0x05 },
	{ "HSPLL",     0x06 },
	{ "RCIO6",     0x07 },
	{ "INTIO67",   0x08 },
	{ "INTIO7",    0x09 },
};

static const struct gp_cfg_option pic18f23k22_borv_opts[] =
{ /* for BORV: */
	{ "285",       0x00 },
	{ "250",       0x08 },
	{ "220",       0x10 },
	{ "190",       0x18 },
};

static const struct gp_cfg_option pic18f23k22_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "PORTB3",    0x00 },
	{ "PORTC1",    0x01 },
};

static const struct gp_cfg_option pic18f23k22_ccp3mx_opts[] =
{ /* for CCP3MX: */
	{ "PORTC6",    0x00 },
	{ "PORTB5",    0x04 },
};

static const struct gp_cfg_option pic18f23k22_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HSHP",      0x02 },
	{ "HSMP",      0x03 },
	{ "ECHP",      0x04 },
	{ "ECHPIO6",   0x05 },
	{ "RC",        0x06 },
	{ "RCIO6",     0x07 },
	{ "INTIO67",   0x08 },
	{ "INTIO7",    0x09 },
	{ "ECMP",      0x0a },
	{ "ECMPIO6",   0x0b },
	{ "ECLP",      0x0c },
	{ "ECLPIO6",   0x0d },
};

static const struct gp_cfg_option pic18f23k22_mclre_opts[] =
{ /* for MCLRE: */
	{ "INTMCLR",   0x00 },
	{ "EXTMCLR",   0x80 },
};

static const struct gp_cfg_option pic18f23k22_p2bmx_opts[] =
{ /* for P2BMX: */
	{ "PORTC0",    0x00 },
	{ "PORTB5",    0x20 },
};

static const struct gp_cfg_option pic18f23k22_t3cmx_opts[] =
{ /* for T3CMX: */
	{ "PORTB5",    0x00 },
	{ "PORTC0",    0x10 },
};

static const struct gp_cfg_option pic18f23k22_wdten_opts[] =
{ /* for WDTEN: */
	{ "OFF",       0x00 },
	{ "NOSLP",     0x01 },
	{ "SWON",      0x02 },
	{ "ON",        0x03 },
};

static const struct gp_cfg_option pic18f23k22_wdtps_opts[] =
{ /* for WDTPS: */
	{ "1",         0x00 },
	{ "2",         0x04 },
	{ "4",         0x08 },
	{ "8",         0x0c },
	{ "16",        0x10 },
	{ "32",        0x14 },
	{ "64",        0x18 },
	{ "128",       0x1c },
	{ "256",       0x20 },
	{ "512",       0x24 },
	{ "1024",      0x28 },
	{ "2048",      0x2c },
	{ "4096",      0x30 },
	{ "8192",      0x34 },
	{ "16384",     0x38 },
	{ "32768",     0x3c },
};

static const struct gp_cfg_option pic18f2420_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "PORTB",     0x00 },
	{ "PORTC",     0x01 },
};

static const struct gp_cfg_option pic18f2439_osc_opts[] =
{ /* for OSC: */
	{ "HS",        0x02 },
	{ "EC",        0x04 },
	{ "ECIO",      0x05 },
	{ "HSPLL",     0x06 },
};

static const struct gp_cfg_option pic18f2450_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "BB1K",      0x00 },
	{ "BB2K",      0x08 },
};

static const struct gp_cfg_option pic18f2450_bor_opts[] =
{ /* for BOR: */
	{ "OFF",       0x00 },
	{ "SOFT",      0x02 },
	{ "ON_ACTIVE", 0x04 },
	{ "ON",        0x06 },
};

static const struct gp_cfg_option pic18f2450_borv_opts[] =
{ /* for BORV: */
	{ "46",        0x00 },
	{ "43",        0x08 },
	{ "28",        0x10 },
	{ "21",        0x18 },
};

static const struct gp_cfg_option pic18f2450_cpudiv_opts[] =
{ /* for CPUDIV: */
	{ "OSC1_PLL2", 0x00 },
	{ "OSC2_PLL3", 0x08 },
	{ "OSC3_PLL4", 0x10 },
	{ "OSC4_PLL6", 0x18 },
};

static const struct gp_cfg_option pic18f2450_fosc_opts[] =
{ /* for FOSC: */
	{ "XT_XT",     0x00 },
	{ "XTPLL_XT",  0x02 },
	{ "ECIO_EC",   0x04 },
	{ "EC_EC",     0x05 },
	{ "ECPLLIO_EC", 0x06 },
	{ "ECPLL_EC",  0x07 },
	{ "INTOSCIO_EC", 0x08 },
	{ "INTOSC_EC", 0x09 },
	{ "INTOSC_XT", 0x0a },
	{ "INTOSC_HS", 0x0b },
	{ "HS",        0x0c },
	{ "HSPLL_HS",  0x0e },
};

static const struct gp_cfg_option pic18f2450_plldiv_opts[] =
{ /* for PLLDIV: */
	{ "1",         0x00 },
	{ "2",         0x01 },
	{ "3",         0x02 },
	{ "4",         0x03 },
	{ "5",         0x04 },
	{ "6",         0x05 },
	{ "10",        0x06 },
	{ "12",        0x07 },
};

static const struct gp_cfg_option pic18f2450_usbdiv_opts[] =
{ /* for USBDIV: */
	{ "1",         0x00 },
	{ "2",         0x20 },
};

static const struct gp_cfg_option pic18f2480_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "1024",      0x00 },
	{ "2048",      0x10 },
};

static const struct gp_cfg_option pic18f2480_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "RC",        0x03 },
	{ "EC",        0x04 },
	{ "ECIO",      0x05 },
	{ "HSPLL",     0x06 },
	{ "RCIO",      0x07 },
	{ "IRCIO67",   0x08 },
	{ "IRCIO7",    0x09 },
};

static const struct gp_cfg_option pic18f24j10_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "ALTERNATE", 0x00 },
	{ "DEFAULT",   0x01 },
};

static const struct gp_cfg_option pic18f24j10_fosc_opts[] =
{ /* for FOSC: */
	{ "HS",        0x00 },
	{ "HSPLL",     0x01 },
	{ "EC",        0x02 },
	{ "ECPLL",     0x03 },
};

static const struct gp_cfg_option pic18f24j10_wdtps_opts[] =
{ /* for WDTPS: */
	{ "1",         0x00 },
	{ "2",         0x01 },
	{ "4",         0x02 },
	{ "8",         0x03 },
	{ "16",        0x04 },
	{ "32",        0x05 },
	{ "64",        0x06 },
	{ "128",       0x07 },
	{ "256",       0x08 },
	{ "512",       0x09 },
	{ "1024",      0x0a },
	{ "2048",      0x0b },
	{ "4096",      0x0c },
	{ "8192",      0x0d },
	{ "16384",     0x0e },
	{ "32768",     0x0f },
};

static const struct gp_cfg_option pic18f24j11_dswdtosc_opts[] =
{ /* for DSWDTOSC: */
	{ "T1OSCREF",  0x00 },
	{ "INTOSCREF", 0x01 },
};

static const struct gp_cfg_option pic18f24j11_dswdtps_opts[] =
{ /* for DSWDTPS: */
	{ "2",         0x00 },
	{ "8",         0x10 },
	{ "32",        0x20 },
	{ "128",       0x30 },
	{ "512",       0x40 },
	{ "2048",      0x50 },
	{ "8192",      0x60 },
	{ "K32",       0x70 },
	{ "K131",      0x80 },
	{ "K524",      0x90 },
	{ "M2",        0xa0 },
	{ "M8",        0xb0 },
	{ "M33",       0xc0 },
	{ "M134",      0xd0 },
	{ "M536",      0xe0 },
	{ "G2",        0xf0 },
};

static const struct gp_cfg_option pic18f24j11_mssp7b_en_opts[] =
{ /* for MSSP7B_EN: */
	{ "MSK5",      0x00 },
	{ "MSK7",      0x08 },
};

static const struct gp_cfg_option pic18f24j11_osc_opts[] =
{ /* for OSC: */
	{ "INTOSC",    0x00 },
	{ "INTOSCO",   0x01 },
	{ "INTOSCPLL", 0x02 },
	{ "INTOSCPLLO", 0x03 },
	{ "HS",        0x04 },
	{ "HSPLL",     0x05 },
	{ "EC",        0x06 },
	{ "ECPLL",     0x07 },
};

static const struct gp_cfg_option pic18f24j11_rtcosc_opts[] =
{ /* for RTCOSC: */
	{ "INTOSCREF", 0x00 },
	{ "T1OSCREF",  0x02 },
};

static const struct gp_cfg_option pic18f24j11_wpend_opts[] =
{ /* for WPEND: */
	{ "PAGE_0",    0x00 },
	{ "PAGE_WPFP", 0x40 },
};

static const struct gp_cfg_option pic18f24j11_wpfp_opts[] =
{ /* for WPFP: */
	{ "PAGE_0",    0x00 },
	{ "PAGE_1",    0x01 },
	{ "PAGE_2",    0x02 },
	{ "PAGE_3",    0x03 },
	{ "PAGE_4",    0x04 },
	{ "PAGE_5",    0x05 },
	{ "PAGE_6",    0x06 },
	{ "PAGE_7",    0x07 },
	{ "PAGE_8",    0x08 },
	{ "PAGE_9",    0x09 },
	{ "PAGE_10",   0x0a },
	{ "PAGE_11",   0x0b },
	{ "PAGE_12",   0x0c },
	{ "PAGE_13",   0x0d },
	{ "PAGE_14",   0x0e },
	{ "PAGE_15",   0x0f },
};

static const struct gp_cfg_option pic18f24j50_cpudiv_opts[] =
{ /* for CPUDIV: */
	{ "OSC4_PLL6", 0x00 },
	{ "OSC3_PLL3", 0x01 },
	{ "OSC2_PLL2", 0x02 },
	{ "OSC1",      0x03 },
};

static const struct gp_cfg_option pic18f24j50_plldiv_opts[] =
{ /* for PLLDIV: */
	{ "12",        0x00 },
	{ "10",        0x02 },
	{ "6",         0x04 },
	{ "5",         0x06 },
	{ "4",         0x08 },
	{ "3",         0x0a },
	{ "2",         0x0c },
	{ "1",         0x0e },
};

static const struct gp_cfg_option pic18f24k50_cpudiv_opts[] =
{ /* for CPUDIV: */
	{ "NOCLKDIV",  0x00 },
	{ "CLKDIV2",   0x08 },
	{ "CLKDIV3",   0x10 },
	{ "CLKDIV6",   0x18 },
};

static const struct gp_cfg_option pic18f24k50_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HSH",       0x02 },
	{ "HSM",       0x03 },
	{ "ECHCLKO",   0x04 },
	{ "ECHIO",     0x05 },
	{ "RCCLKO",    0x06 },
	{ "RCIO",      0x07 },
	{ "INTOSCIO",  0x08 },
	{ "INTOSCCLKO", 0x09 },
	{ "ECMCLKO",   0x0a },
	{ "ECMIO",     0x0b },
	{ "ECLCLKO",   0x0c },
	{ "ECLIO",     0x0d },
};

static const struct gp_cfg_option pic18f24k50_icprt_opts[] =
{ /* for ICPRT: */
	{ "OFF",       0x00 },
};

static const struct gp_cfg_option pic18f24k50_ls48mhz_opts[] =
{ /* for LS48MHZ: */
	{ "SYS24X4",   0x00 },
	{ "SYS48X8",   0x20 },
};

static const struct gp_cfg_option pic18f24k50_pllsel_opts[] =
{ /* for PLLSEL: */
	{ "PLL4X",     0x00 },
	{ "PLL3X",     0x01 },
};

static const struct gp_cfg_option pic18f24k50_sdomx_opts[] =
{ /* for SDOMX: */
	{ "RC7",       0x00 },
	{ "RB3",       0x40 },
};

static const struct gp_cfg_option pic18f24k50_t3cmx_opts[] =
{ /* for T3CMX: */
	{ "RB5",       0x00 },
	{ "RC0",       0x10 },
};

static const struct gp_cfg_option pic18f2585_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "1024",      0x00 },
	{ "2048",      0x10 },
	{ "4096",      0x20 },
};

static const struct gp_cfg_option pic18f25j11_wpfp_opts[] =
{ /* for WPFP: */
	{ "PAGE_0",    0x00 },
	{ "PAGE_1",    0x01 },
	{ "PAGE_2",    0x02 },
	{ "PAGE_3",    0x03 },
	{ "PAGE_4",    0x04 },
	{ "PAGE_5",    0x05 },
	{ "PAGE_6",    0x06 },
	{ "PAGE_7",    0x07 },
	{ "PAGE_8",    0x08 },
	{ "PAGE_9",    0x09 },
	{ "PAGE_10",   0x0a },
	{ "PAGE_11",   0x0b },
	{ "PAGE_12",   0x0c },
	{ "PAGE_13",   0x0d },
	{ "PAGE_14",   0x0e },
	{ "PAGE_15",   0x0f },
	{ "PAGE_16",   0x10 },
	{ "PAGE_17",   0x11 },
	{ "PAGE_18",   0x12 },
	{ "PAGE_19",   0x13 },
	{ "PAGE_20",   0x14 },
	{ "PAGE_21",   0x15 },
	{ "PAGE_22",   0x16 },
	{ "PAGE_23",   0x17 },
	{ "PAGE_24",   0x18 },
	{ "PAGE_25",   0x19 },
	{ "PAGE_26",   0x1a },
	{ "PAGE_27",   0x1b },
	{ "PAGE_28",   0x1c },
	{ "PAGE_29",   0x1d },
	{ "PAGE_30",   0x1e },
	{ "PAGE_31",   0x1f },
};

static const struct gp_cfg_option pic18f25k80_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "BB1K",      0x00 },
	{ "BB2K",      0x10 },
};

static const struct gp_cfg_option pic18f25k80_borpwr_opts[] =
{ /* for BORPWR: */
	{ "LOW",       0x00 },
	{ "MEDIUM",    0x20 },
	{ "HIGH",      0x40 },
	{ "ZPBORMV",   0x60 },
};

static const struct gp_cfg_option pic18f25k80_canmx_opts[] =
{ /* for CANMX: */
	{ "PORTC",     0x00 },
	{ "PORTB",     0x01 },
};

static const struct gp_cfg_option pic18f25k80_fosc_opts[] =
{ /* for FOSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS2",       0x02 },
	{ "HS1",       0x03 },
	{ "EC3IO",     0x04 },
	{ "EC3",       0x05 },
	{ "RC",        0x06 },
	{ "RCIO",      0x07 },
	{ "INTIO2",    0x08 },
	{ "INTIO1",    0x09 },
	{ "EC2IO",     0x0a },
	{ "EC2",       0x0b },
	{ "EC1IO",     0x0c },
	{ "EC1",       0x0d },
};

static const struct gp_cfg_option pic18f25k80_soscsel_opts[] =
{ /* for SOSCSEL: */
	{ "LOW",       0x08 },
	{ "DIG",       0x10 },
	{ "HIGH",      0x18 },
};

static const struct gp_cfg_option pic18f25k80_wdten_opts[] =
{ /* for WDTEN: */
	{ "OFF",       0x00 },
	{ "NOSLP",     0x01 },
	{ "ON",        0x02 },
	{ "SWDTDIS",   0x03 },
};

static const struct gp_cfg_option pic18f25k80_wdtps_opts[] =
{ /* for WDTPS: */
	{ "1",         0x00 },
	{ "2",         0x04 },
	{ "4",         0x08 },
	{ "8",         0x0c },
	{ "16",        0x10 },
	{ "32",        0x14 },
	{ "64",        0x18 },
	{ "128",       0x1c },
	{ "256",       0x20 },
	{ "512",       0x24 },
	{ "1024",      0x28 },
	{ "2048",      0x2c },
	{ "4096",      0x30 },
	{ "8192",      0x34 },
	{ "16384",     0x38 },
	{ "32768",     0x3c },
	{ "65536",     0x40 },
	{ "131072",    0x44 },
	{ "262144",    0x48 },
	{ "524288",    0x4c },
	{ "1048576",   0x7c },
};

static const struct gp_cfg_option pic18f26j11_wpfp_opts[] =
{ /* for WPFP: */
	{ "PAGE_0",    0x00 },
	{ "PAGE_1",    0x01 },
	{ "PAGE_2",    0x02 },
	{ "PAGE_3",    0x03 },
	{ "PAGE_4",    0x04 },
	{ "PAGE_5",    0x05 },
	{ "PAGE_6",    0x06 },
	{ "PAGE_7",    0x07 },
	{ "PAGE_8",    0x08 },
	{ "PAGE_9",    0x09 },
	{ "PAGE_10",   0x0a },
	{ "PAGE_11",   0x0b },
	{ "PAGE_12",   0x0c },
	{ "PAGE_13",   0x0d },
	{ "PAGE_14",   0x0e },
	{ "PAGE_15",   0x0f },
	{ "PAGE_16",   0x10 },
	{ "PAGE_17",   0x11 },
	{ "PAGE_18",   0x12 },
	{ "PAGE_19",   0x13 },
	{ "PAGE_20",   0x14 },
	{ "PAGE_21",   0x15 },
	{ "PAGE_22",   0x16 },
	{ "PAGE_23",   0x17 },
	{ "PAGE_24",   0x18 },
	{ "PAGE_25",   0x19 },
	{ "PAGE_26",   0x1a },
	{ "PAGE_27",   0x1b },
	{ "PAGE_28",   0x1c },
	{ "PAGE_29",   0x1d },
	{ "PAGE_30",   0x1e },
	{ "PAGE_31",   0x1f },
	{ "PAGE_32",   0x20 },
	{ "PAGE_33",   0x21 },
	{ "PAGE_34",   0x22 },
	{ "PAGE_35",   0x23 },
	{ "PAGE_36",   0x24 },
	{ "PAGE_37",   0x25 },
	{ "PAGE_38",   0x26 },
	{ "PAGE_39",   0x27 },
	{ "PAGE_40",   0x28 },
	{ "PAGE_41",   0x29 },
	{ "PAGE_42",   0x2a },
	{ "PAGE_43",   0x2b },
	{ "PAGE_44",   0x2c },
	{ "PAGE_45",   0x2d },
	{ "PAGE_46",   0x2e },
	{ "PAGE_47",   0x2f },
	{ "PAGE_48",   0x30 },
	{ "PAGE_49",   0x31 },
	{ "PAGE_50",   0x32 },
	{ "PAGE_51",   0x33 },
	{ "PAGE_52",   0x34 },
	{ "PAGE_53",   0x35 },
	{ "PAGE_54",   0x36 },
	{ "PAGE_55",   0x37 },
	{ "PAGE_56",   0x38 },
	{ "PAGE_57",   0x39 },
	{ "PAGE_58",   0x3a },
	{ "PAGE_59",   0x3b },
	{ "PAGE_60",   0x3c },
	{ "PAGE_61",   0x3d },
	{ "PAGE_62",   0x3e },
	{ "PAGE_63",   0x3f },
};

static const struct gp_cfg_option pic18f26j13_adcsel_opts[] =
{ /* for ADCSEL: */
	{ "BIT12",     0x00 },
	{ "BIT10",     0x02 },
};

static const struct gp_cfg_option pic18f26j13_pllsel_opts[] =
{ /* for PLLSEL: */
	{ "PLL96",     0x00 },
	{ "PLL4X",     0x04 },
};

static const struct gp_cfg_option pic18f26j13_soscsel_opts[] =
{ /* for SOSCSEL: */
	{ "RESERVED",  0x00 },
	{ "LOW",       0x08 },
	{ "DIG",       0x10 },
	{ "HIGH",      0x18 },
};

static const struct gp_cfg_option pic18f26j13_wpend_opts[] =
{ /* for WPEND: */
	{ "PAGE_0",    0x00 },
	{ "PAGE_WPFP", 0x02 },
};

static const struct gp_cfg_option pic18f26j53_ls48mhz_opts[] =
{ /* for LS48MHZ: */
	{ "SYS24X4",   0x00 },
	{ "SYS48X8",   0x08 },
};

static const struct gp_cfg_option pic18f27j13_wpfp_opts[] =
{ /* for WPFP: */
	{ "PAGE_0",    0x00 },
	{ "PAGE_1",    0x01 },
	{ "PAGE_2",    0x02 },
	{ "PAGE_3",    0x03 },
	{ "PAGE_4",    0x04 },
	{ "PAGE_5",    0x05 },
	{ "PAGE_6",    0x06 },
	{ "PAGE_7",    0x07 },
	{ "PAGE_8",    0x08 },
	{ "PAGE_9",    0x09 },
	{ "PAGE_10",   0x0a },
	{ "PAGE_11",   0x0b },
	{ "PAGE_12",   0x0c },
	{ "PAGE_13",   0x0d },
	{ "PAGE_14",   0x0e },
	{ "PAGE_15",   0x0f },
	{ "PAGE_16",   0x10 },
	{ "PAGE_17",   0x11 },
	{ "PAGE_18",   0x12 },
	{ "PAGE_19",   0x13 },
	{ "PAGE_20",   0x14 },
	{ "PAGE_21",   0x15 },
	{ "PAGE_22",   0x16 },
	{ "PAGE_23",   0x17 },
	{ "PAGE_24",   0x18 },
	{ "PAGE_25",   0x19 },
	{ "PAGE_26",   0x1a },
	{ "PAGE_27",   0x1b },
	{ "PAGE_28",   0x1c },
	{ "PAGE_29",   0x1d },
	{ "PAGE_30",   0x1e },
	{ "PAGE_31",   0x1f },
	{ "PAGE_32",   0x20 },
	{ "PAGE_33",   0x21 },
	{ "PAGE_34",   0x22 },
	{ "PAGE_35",   0x23 },
	{ "PAGE_36",   0x24 },
	{ "PAGE_37",   0x25 },
	{ "PAGE_38",   0x26 },
	{ "PAGE_39",   0x27 },
	{ "PAGE_40",   0x28 },
	{ "PAGE_41",   0x29 },
	{ "PAGE_42",   0x2a },
	{ "PAGE_43",   0x2b },
	{ "PAGE_44",   0x2c },
	{ "PAGE_45",   0x2d },
	{ "PAGE_46",   0x2e },
	{ "PAGE_47",   0x2f },
	{ "PAGE_48",   0x30 },
	{ "PAGE_49",   0x31 },
	{ "PAGE_50",   0x32 },
	{ "PAGE_51",   0x33 },
	{ "PAGE_52",   0x34 },
	{ "PAGE_53",   0x35 },
	{ "PAGE_54",   0x36 },
	{ "PAGE_55",   0x37 },
	{ "PAGE_56",   0x38 },
	{ "PAGE_57",   0x39 },
	{ "PAGE_58",   0x3a },
	{ "PAGE_59",   0x3b },
	{ "PAGE_60",   0x3c },
	{ "PAGE_61",   0x3d },
	{ "PAGE_62",   0x3e },
	{ "PAGE_63",   0x3f },
	{ "PAGE_64",   0x40 },
	{ "PAGE_65",   0x41 },
	{ "PAGE_66",   0x42 },
	{ "PAGE_67",   0x43 },
	{ "PAGE_68",   0x44 },
	{ "PAGE_69",   0x45 },
	{ "PAGE_70",   0x46 },
	{ "PAGE_71",   0x47 },
	{ "PAGE_72",   0x48 },
	{ "PAGE_73",   0x49 },
	{ "PAGE_74",   0x4a },
	{ "PAGE_75",   0x4b },
	{ "PAGE_76",   0x4c },
	{ "PAGE_77",   0x4d },
	{ "PAGE_78",   0x4e },
	{ "PAGE_79",   0x4f },
	{ "PAGE_80",   0x50 },
	{ "PAGE_81",   0x51 },
	{ "PAGE_82",   0x52 },
	{ "PAGE_83",   0x53 },
	{ "PAGE_84",   0x54 },
	{ "PAGE_85",   0x55 },
	{ "PAGE_86",   0x56 },
	{ "PAGE_87",   0x57 },
	{ "PAGE_88",   0x58 },
	{ "PAGE_89",   0x59 },
	{ "PAGE_90",   0x5a },
	{ "PAGE_91",   0x5b },
	{ "PAGE_92",   0x5c },
	{ "PAGE_93",   0x5d },
	{ "PAGE_94",   0x5e },
	{ "PAGE_95",   0x5f },
	{ "PAGE_96",   0x60 },
	{ "PAGE_97",   0x61 },
	{ "PAGE_98",   0x62 },
	{ "PAGE_99",   0x63 },
	{ "PAGE_100",  0x64 },
	{ "PAGE_101",  0x65 },
	{ "PAGE_102",  0x66 },
	{ "PAGE_103",  0x67 },
	{ "PAGE_104",  0x68 },
	{ "PAGE_105",  0x69 },
	{ "PAGE_106",  0x6a },
	{ "PAGE_107",  0x6b },
	{ "PAGE_108",  0x6c },
	{ "PAGE_109",  0x6d },
	{ "PAGE_110",  0x6e },
	{ "PAGE_111",  0x6f },
	{ "PAGE_112",  0x70 },
	{ "PAGE_113",  0x71 },
	{ "PAGE_114",  0x72 },
	{ "PAGE_115",  0x73 },
	{ "PAGE_116",  0x74 },
	{ "PAGE_117",  0x75 },
	{ "PAGE_118",  0x76 },
	{ "PAGE_119",  0x77 },
	{ "PAGE_120",  0x78 },
	{ "PAGE_121",  0x79 },
	{ "PAGE_122",  0x7a },
	{ "PAGE_123",  0x7b },
	{ "PAGE_124",  0x7c },
	{ "PAGE_125",  0x7d },
	{ "PAGE_126",  0x7e },
	{ "PAGE_127",  0x7f },
};

static const struct gp_cfg_option pic18f4331_exclkmx_opts[] =
{ /* for EXCLKMX: */
	{ "RD0",       0x00 },
	{ "RC3",       0x10 },
};

static const struct gp_cfg_option pic18f4331_fltamx_opts[] =
{ /* for FLTAMX: */
	{ "RD4",       0x00 },
	{ "RC1",       0x01 },
};

static const struct gp_cfg_option pic18f4331_pwm4mx_opts[] =
{ /* for PWM4MX: */
	{ "RD5",       0x00 },
	{ "RB5",       0x08 },
};

static const struct gp_cfg_option pic18f4331_sspmx_opts[] =
{ /* for SSPMX: */
	{ "RD1",       0x00 },
	{ "RC7",       0x04 },
};

static const struct gp_cfg_option pic18f43k22_ccp3mx_opts[] =
{ /* for CCP3MX: */
	{ "PORTE0",    0x00 },
	{ "PORTB5",    0x04 },
};

static const struct gp_cfg_option pic18f43k22_p2bmx_opts[] =
{ /* for P2BMX: */
	{ "PORTC0",    0x00 },
	{ "PORTD2",    0x20 },
};

static const struct gp_cfg_option pic18f6310_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "PORTE",     0x00 },
	{ "PORTC",     0x01 },
};

static const struct gp_cfg_option pic18f6310_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "RC",        0x03 },
	{ "EC",        0x04 },
	{ "ECIO",      0x05 },
	{ "HSPLL",     0x06 },
	{ "RCIO",      0x07 },
	{ "INTIO67",   0x08 },
	{ "INTIO7",    0x09 },
};

static const struct gp_cfg_option pic18f6525_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "XT",        0x01 },
	{ "HS",        0x02 },
	{ "RC",        0x03 },
	{ "EC",        0x04 },
	{ "ECIO",      0x05 },
	{ "HSPLL",     0x06 },
	{ "RCIO",      0x07 },
	{ "ECIOPLL",   0x0c },
	{ "ECIOSWPLL", 0x0d },
	{ "HSSWPLL",   0x0e },
};

static const struct gp_cfg_option pic18f6527_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "BB2K",      0x00 },
	{ "BB4K",      0x10 },
	{ "BB8K",      0x20 },
};

static const struct gp_cfg_option pic18f65j94_borv_opts[] =
{ /* for BORV: */
	{ "0",         0x00 },
	{ "1",         0x02 },
};

static const struct gp_cfg_option pic18f65j94_cinasel_opts[] =
{ /* for CINASEL: */
	{ "RA5",       0x00 },
	{ "DEFAULT",   0x02 },
};

static const struct gp_cfg_option pic18f65j94_dswdtosc_opts[] =
{ /* for DSWDTOSC: */
	{ "SOSC",      0x00 },
	{ "LPRC",      0x02 },
};

static const struct gp_cfg_option pic18f65j94_dswdtps_opts[] =
{ /* for DSWDTPS: */
	{ "DSWDTPS0",  0x00 },
	{ "DSWDTPS1",  0x08 },
	{ "DSWDTPS2",  0x10 },
	{ "DSWDTPS3",  0x18 },
	{ "DSWDTPS4",  0x20 },
	{ "DSWDTPS5",  0x28 },
	{ "DSWDTPS6",  0x30 },
	{ "DSWDTPS7",  0x38 },
	{ "DSWDTPS8",  0x40 },
	{ "DSWDTPS9",  0x48 },
	{ "DSWDTPSA",  0x50 },
	{ "DSWDTPSB",  0x58 },
	{ "DSWDTPSC",  0x60 },
	{ "DSWDTPSD",  0x68 },
	{ "DSWDTPSE",  0x70 },
	{ "DSWDTPSF",  0x78 },
	{ "DSWDTPS10", 0x80 },
	{ "DSWDTPS11", 0x88 },
	{ "DSWDTPS12", 0x90 },
	{ "DSWDTPS13", 0x98 },
	{ "DSWDTPS14", 0xa0 },
	{ "DSWDTPS15", 0xa8 },
	{ "DSWDTPS16", 0xb0 },
	{ "DSWDTPS17", 0xb8 },
	{ "DSWDTPS18", 0xc0 },
	{ "DSWDTPS19", 0xc8 },
	{ "DSWDTPS1A", 0xd0 },
	{ "DSWDTPS1B", 0xd8 },
	{ "DSWDTPS1C", 0xe0 },
	{ "DSWDTPS1D", 0xe8 },
	{ "DSWDTPS1E", 0xf0 },
	{ "DSWDTPS1F", 0xf8 },
};

static const struct gp_cfg_option pic18f65j94_fosc_opts[] =
{ /* for FOSC: */
	{ "FRC",       0x00 },
	{ "FRCPLL",    0x01 },
	{ "PRI",       0x02 },
	{ "PRIPLL",    0x03 },
	{ "SOSC",      0x04 },
	{ "LPRC",      0x05 },
	{ "FRC500KHZ", 0x06 },
	{ "FRCDIV",    0x07 },
};

static const struct gp_cfg_option pic18f65j94_fscm_opts[] =
{ /* for FSCM: */
	{ "CSECME",    0x00 },
	{ "CSECMD",    0x10 },
	{ "CSDCMD",    0x20 },
};

static const struct gp_cfg_option pic18f65j94_ls48mhz_opts[] =
{ /* for LS48MHZ: */
	{ "SYSX1",     0x00 },
	{ "SYSX2",     0x02 },
};

static const struct gp_cfg_option pic18f65j94_msspmsk2_opts[] =
{ /* for MSSPMSK2: */
	{ "MSK5",      0x00 },
	{ "MSK7",      0x04 },
};

static const struct gp_cfg_option pic18f65j94_plldiv_opts[] =
{ /* for PLLDIV: */
	{ "NODIV",     0x00 },
	{ "DIV2",      0x01 },
	{ "DIV3",      0x02 },
	{ "DIV4",      0x03 },
	{ "DIV5",      0x04 },
	{ "DIV6",      0x05 },
	{ "DIV10",     0x06 },
	{ "DIV12",     0x07 },
	{ "RESERVED",  0x08 },
	{ "RESERVED",  0x09 },
	{ "RESERVED",  0x0a },
	{ "RESERVED",  0x0b },
	{ "PLL4X",     0x0c },
	{ "PLL6X",     0x0d },
	{ "PLL8X",     0x0e },
	{ "NOPLL",     0x0f },
};

static const struct gp_cfg_option pic18f65j94_poscmd_opts[] =
{ /* for POSCMD: */
	{ "EC",        0x00 },
	{ "MS",        0x01 },
	{ "HS",        0x02 },
	{ "NONE",      0x03 },
};

static const struct gp_cfg_option pic18f65j94_soscsel_opts[] =
{ /* for SOSCSEL: */
	{ "DIG",       0x00 },
	{ "LOW",       0x08 },
};

static const struct gp_cfg_option pic18f65j94_t5gsel_opts[] =
{ /* for T5GSEL: */
	{ "T3G",       0x00 },
	{ "T5G",       0x01 },
};

static const struct gp_cfg_option pic18f65j94_wdtclk_opts[] =
{ /* for WDTCLK: */
	{ "SYS",       0x00 },
	{ "SOSC",      0x04 },
	{ "LPRC",      0x08 },
	{ "FRC",       0x0c },
};

static const struct gp_cfg_option pic18f65j94_wdtps_opts[] =
{ /* for WDTPS: */
	{ "1",         0x00 },
	{ "2",         0x10 },
	{ "4",         0x20 },
	{ "8",         0x30 },
	{ "16",        0x40 },
	{ "32",        0x50 },
	{ "64",        0x60 },
	{ "128",       0x70 },
	{ "256",       0x80 },
	{ "512",       0x90 },
	{ "1024",      0xa0 },
	{ "2048",      0xb0 },
	{ "4096",      0xc0 },
	{ "8192",      0xd0 },
	{ "16384",     0xe0 },
	{ "32768",     0xf0 },
};

static const struct gp_cfg_option pic18f65j94_wdtwin_opts[] =
{ /* for WDTWIN: */
	{ "PS75_0",    0x00 },
	{ "PS50_0",    0x01 },
	{ "PS37_5",    0x02 },
	{ "PS25_0",    0x03 },
};

static const struct gp_cfg_option pic18f65j94_windis_opts[] =
{ /* for WINDIS: */
	{ "WDTWIN",    0x00 },
	{ "WDTSTD",    0x04 },
};

static const struct gp_cfg_option pic18f65j94_wpcfg_opts[] =
{ /* for WPCFG: */
	{ "WPCFGEN",   0x00 },
	{ "WPCFGDIS",  0x04 },
};

static const struct gp_cfg_option pic18f65j94_wpdis_opts[] =
{ /* for WPDIS: */
	{ "WPEN",      0x00 },
	{ "WPDIS",     0x01 },
};

static const struct gp_cfg_option pic18f65j94_wpend_opts[] =
{ /* for WPEND: */
	{ "WPSTARTMEM", 0x00 },
	{ "WPENDMEM",  0x02 },
};

static const struct gp_cfg_option pic18f65j94_wpfp_opts[] =
{ /* for WPFP: */
	{ "WPFP0",     0x00 },
	{ "WPFP1",     0x01 },
	{ "WPFP2",     0x02 },
	{ "WPFP3",     0x03 },
	{ "WPFP4",     0x04 },
	{ "WPFP5",     0x05 },
	{ "WPFP6",     0x06 },
	{ "WPFP7",     0x07 },
	{ "WPFP8",     0x08 },
	{ "WPFP9",     0x09 },
	{ "WPFP10",    0x0a },
	{ "WPFP11",    0x0b },
	{ "WPFP12",    0x0c },
	{ "WPFP13",    0x0d },
	{ "WPFP14",    0x0e },
	{ "WPFP15",    0x0f },
	{ "WPFP16",    0x10 },
	{ "WPFP17",    0x11 },
	{ "WPFP18",    0x12 },
	{ "WPFP19",    0x13 },
	{ "WPFP20",    0x14 },
	{ "WPFP21",    0x15 },
	{ "WPFP22",    0x16 },
	{ "WPFP23",    0x17 },
	{ "WPFP24",    0x18 },
	{ "WPFP25",    0x19 },
	{ "WPFP26",    0x1a },
	{ "WPFP27",    0x1b },
	{ "WPFP28",    0x1c },
	{ "WPFP29",    0x1d },
	{ "WPFP30",    0x1e },
	{ "WPFP31",    0x1f },
	{ "WPFP32",    0x20 },
	{ "WPFP33",    0x21 },
	{ "WPFP34",    0x22 },
	{ "WPFP35",    0x23 },
	{ "WPFP36",    0x24 },
	{ "WPFP37",    0x25 },
	{ "WPFP38",    0x26 },
	{ "WPFP39",    0x27 },
	{ "WPFP40",    0x28 },
	{ "WPFP41",    0x29 },
	{ "WPFP42",    0x2a },
	{ "WPFP43",    0x2b },
	{ "WPFP44",    0x2c },
	{ "WPFP45",    0x2d },
	{ "WPFP46",    0x2e },
	{ "WPFP47",    0x2f },
	{ "WPFP48",    0x30 },
	{ "WPFP49",    0x31 },
	{ "WPFP50",    0x32 },
	{ "WPFP51",    0x33 },
	{ "WPFP52",    0x34 },
	{ "WPFP53",    0x35 },
	{ "WPFP54",    0x36 },
	{ "WPFP55",    0x37 },
	{ "WPFP56",    0x38 },
	{ "WPFP57",    0x39 },
	{ "WPFP58",    0x3a },
	{ "WPFP59",    0x3b },
	{ "WPFP60",    0x3c },
	{ "WPFP61",    0x3d },
	{ "WPFP62",    0x3e },
	{ "WPFP63",    0x3f },
	{ "WPFP64",    0x40 },
	{ "WPFP65",    0x41 },
	{ "WPFP66",    0x42 },
	{ "WPFP67",    0x43 },
	{ "WPFP68",    0x44 },
	{ "WPFP69",    0x45 },
	{ "WPFP70",    0x46 },
	{ "WPFP71",    0x47 },
	{ "WPFP72",    0x48 },
	{ "WPFP73",    0x49 },
	{ "WPFP74",    0x4a },
	{ "WPFP75",    0x4b },
	{ "WPFP76",    0x4c },
	{ "WPFP77",    0x4d },
	{ "WPFP78",    0x4e },
	{ "WPFP79",    0x4f },
	{ "WPFP80",    0x50 },
	{ "WPFP81",    0x51 },
	{ "WPFP82",    0x52 },
	{ "WPFP83",    0x53 },
	{ "WPFP84",    0x54 },
	{ "WPFP85",    0x55 },
	{ "WPFP86",    0x56 },
	{ "WPFP87",    0x57 },
	{ "WPFP88",    0x58 },
	{ "WPFP89",    0x59 },
	{ "WPFP90",    0x5a },
	{ "WPFP91",    0x5b },
	{ "WPFP92",    0x5c },
	{ "WPFP93",    0x5d },
	{ "WPFP94",    0x5e },
	{ "WPFP95",    0x5f },
	{ "WPFP96",    0x60 },
	{ "WPFP97",    0x61 },
	{ "WPFP98",    0x62 },
	{ "WPFP99",    0x63 },
	{ "WPFP100",   0x64 },
	{ "WPFP101",   0x65 },
	{ "WPFP102",   0x66 },
	{ "WPFP103",   0x67 },
	{ "WPFP104",   0x68 },
	{ "WPFP105",   0x69 },
	{ "WPFP106",   0x6a },
	{ "WPFP107",   0x6b },
	{ "WPFP108",   0x6c },
	{ "WPFP109",   0x6d },
	{ "WPFP110",   0x6e },
	{ "WPFP111",   0x6f },
	{ "WPFP112",   0x70 },
	{ "WPFP113",   0x71 },
	{ "WPFP114",   0x72 },
	{ "WPFP115",   0x73 },
	{ "WPFP116",   0x74 },
	{ "WPFP117",   0x75 },
	{ "WPFP118",   0x76 },
	{ "WPFP119",   0x77 },
	{ "WPFP120",   0x78 },
	{ "WPFP121",   0x79 },
	{ "WPFP122",   0x7a },
	{ "WPFP123",   0x7b },
	{ "WPFP124",   0x7c },
	{ "WPFP125",   0x7d },
	{ "WPFP126",   0x7e },
	{ "WPFP127",   0x7f },
	{ "WPFP128",   0x80 },
	{ "WPFP129",   0x81 },
	{ "WPFP130",   0x82 },
	{ "WPFP131",   0x83 },
	{ "WPFP132",   0x84 },
	{ "WPFP133",   0x85 },
	{ "WPFP134",   0x86 },
	{ "WPFP135",   0x87 },
	{ "WPFP136",   0x88 },
	{ "WPFP137",   0x89 },
	{ "WPFP138",   0x8a },
	{ "WPFP139",   0x8b },
	{ "WPFP140",   0x8c },
	{ "WPFP141",   0x8d },
	{ "WPFP142",   0x8e },
	{ "WPFP143",   0x8f },
	{ "WPFP144",   0x90 },
	{ "WPFP145",   0x91 },
	{ "WPFP146",   0x92 },
	{ "WPFP147",   0x93 },
	{ "WPFP148",   0x94 },
	{ "WPFP149",   0x95 },
	{ "WPFP150",   0x96 },
	{ "WPFP151",   0x97 },
	{ "WPFP152",   0x98 },
	{ "WPFP153",   0x99 },
	{ "WPFP154",   0x9a },
	{ "WPFP155",   0x9b },
	{ "WPFP156",   0x9c },
	{ "WPFP157",   0x9d },
	{ "WPFP158",   0x9e },
	{ "WPFP159",   0x9f },
	{ "WPFP160",   0xa0 },
	{ "WPFP161",   0xa1 },
	{ "WPFP162",   0xa2 },
	{ "WPFP163",   0xa3 },
	{ "WPFP164",   0xa4 },
	{ "WPFP165",   0xa5 },
	{ "WPFP166",   0xa6 },
	{ "WPFP167",   0xa7 },
	{ "WPFP168",   0xa8 },
	{ "WPFP169",   0xa9 },
	{ "WPFP170",   0xaa },
	{ "WPFP171",   0xab },
	{ "WPFP172",   0xac },
	{ "WPFP173",   0xad },
	{ "WPFP174",   0xae },
	{ "WPFP175",   0xaf },
	{ "WPFP176",   0xb0 },
	{ "WPFP177",   0xb1 },
	{ "WPFP178",   0xb2 },
	{ "WPFP179",   0xb3 },
	{ "WPFP180",   0xb4 },
	{ "WPFP181",   0xb5 },
	{ "WPFP182",   0xb6 },
	{ "WPFP183",   0xb7 },
	{ "WPFP184",   0xb8 },
	{ "WPFP185",   0xb9 },
	{ "WPFP186",   0xba },
	{ "WPFP187",   0xbb },
	{ "WPFP188",   0xbc },
	{ "WPFP189",   0xbd },
	{ "WPFP190",   0xbe },
	{ "WPFP191",   0xbf },
	{ "WPFP192",   0xc0 },
	{ "WPFP193",   0xc1 },
	{ "WPFP194",   0xc2 },
	{ "WPFP195",   0xc3 },
	{ "WPFP196",   0xc4 },
	{ "WPFP197",   0xc5 },
	{ "WPFP198",   0xc6 },
	{ "WPFP199",   0xc7 },
	{ "WPFP200",   0xc8 },
	{ "WPFP201",   0xc9 },
	{ "WPFP202",   0xca },
	{ "WPFP203",   0xcb },
	{ "WPFP204",   0xcc },
	{ "WPFP205",   0xcd },
	{ "WPFP206",   0xce },
	{ "WPFP207",   0xcf },
	{ "WPFP208",   0xd0 },
	{ "WPFP209",   0xd1 },
	{ "WPFP210",   0xd2 },
	{ "WPFP211",   0xd3 },
	{ "WPFP212",   0xd4 },
	{ "WPFP213",   0xd5 },
	{ "WPFP214",   0xd6 },
	{ "WPFP215",   0xd7 },
	{ "WPFP216",   0xd8 },
	{ "WPFP217",   0xd9 },
	{ "WPFP218",   0xda },
	{ "WPFP219",   0xdb },
	{ "WPFP220",   0xdc },
	{ "WPFP221",   0xdd },
	{ "WPFP222",   0xde },
	{ "WPFP223",   0xdf },
	{ "WPFP224",   0xe0 },
	{ "WPFP225",   0xe1 },
	{ "WPFP226",   0xe2 },
	{ "WPFP227",   0xe3 },
	{ "WPFP228",   0xe4 },
	{ "WPFP229",   0xe5 },
	{ "WPFP230",   0xe6 },
	{ "WPFP231",   0xe7 },
	{ "WPFP232",   0xe8 },
	{ "WPFP233",   0xe9 },
	{ "WPFP234",   0xea },
	{ "WPFP235",   0xeb },
	{ "WPFP236",   0xec },
	{ "WPFP237",   0xed },
	{ "WPFP238",   0xee },
	{ "WPFP239",   0xef },
	{ "WPFP240",   0xf0 },
	{ "WPFP241",   0xf1 },
	{ "WPFP242",   0xf2 },
	{ "WPFP243",   0xf3 },
	{ "WPFP244",   0xf4 },
	{ "WPFP245",   0xf5 },
	{ "WPFP246",   0xf6 },
	{ "WPFP247",   0xf7 },
	{ "WPFP248",   0xf8 },
	{ "WPFP249",   0xf9 },
	{ "WPFP250",   0xfa },
	{ "WPFP251",   0xfb },
	{ "WPFP252",   0xfc },
	{ "WPFP253",   0xfd },
	{ "WPFP254",   0xfe },
	{ "WPFP255",   0xff },
};

static const struct gp_cfg_option pic18f65j94_wpsa_opts[] =
{ /* for WPSA: */
	{ "32",        0x00 },
	{ "128",       0x08 },
};

static const struct gp_cfg_option pic18f65k22_rtcosc_opts[] =
{ /* for RTCOSC: */
	{ "INTOSCREF", 0x00 },
	{ "SOSCREF",   0x01 },
};

static const struct gp_cfg_option pic18f65k80_canmx_opts[] =
{ /* for CANMX: */
	{ "PORTE",     0x00 },
	{ "PORTB",     0x01 },
};

static const struct gp_cfg_option pic18f65k80_t0ckmx_opts[] =
{ /* for T0CKMX: */
	{ "PORTG",     0x00 },
	{ "PORTB",     0x02 },
};

static const struct gp_cfg_option pic18f65k80_t3ckmx_opts[] =
{ /* for T3CKMX: */
	{ "PORTB",     0x00 },
	{ "PORTG",     0x04 },
};

static const struct gp_cfg_option pic18f66j90_osc_opts[] =
{ /* for OSC: */
	{ "INTOSC",    0x00 },
	{ "INTOSCPLL", 0x01 },
	{ "INTOSCO",   0x02 },
	{ "INTOSCPLLO", 0x03 },
	{ "HS",        0x04 },
	{ "HSPLL",     0x05 },
	{ "EC",        0x06 },
	{ "ECPLL",     0x07 },
};

static const struct gp_cfg_option pic18f8310_pm_opts[] =
{ /* for PM: */
	{ "EM",        0x00 },
	{ "MPB",       0x01 },
	{ "MP",        0x02 },
	{ "MC",        0x03 },
};

static const struct gp_cfg_option pic18f83j11_mode_opts[] =
{ /* for MODE: */
	{ "XM20",      0x00 },
	{ "XM16",      0x10 },
	{ "XM12",      0x20 },
	{ "MM",        0x30 },
};

static const struct gp_cfg_option pic18f8525_eccpmx_opts[] =
{ /* for ECCPMX: */
	{ "PORTH",     0x00 },
	{ "PORTE",     0x02 },
};

static const struct gp_cfg_option pic18f8527_addrbw_opts[] =
{ /* for ADDRBW: */
	{ "ADDR8BIT",  0x00 },
	{ "ADDR12BIT", 0x10 },
	{ "ADDR16BIT", 0x20 },
	{ "ADDR20BIT", 0x30 },
};

static const struct gp_cfg_option pic18f8527_databw_opts[] =
{ /* for DATABW: */
	{ "DATA8BIT",  0x00 },
	{ "DATA16BIT", 0x40 },
};

static const struct gp_cfg_option pic18f85j10_eccpmx_opts[] =
{ /* for ECCPMX: */
	{ "ALTERNATE", 0x00 },
	{ "DEFAULT",   0x02 },
};

static const struct gp_cfg_option pic18f85j50_pmpmx_opts[] =
{ /* for PMPMX: */
	{ "ALTERNATE", 0x00 },
	{ "DEFAULT",   0x04 },
};

static const struct gp_cfg_directive mcp19110_002007[] =
{
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "WRT",       0xc00,  4, pic12f617_wrt_opts },
};

static const struct gp_cfg_directive mcp19114_002007[] =
{
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "BOREN",     0x100,  2, pic10f320_lvp_opts },
	{ "WRT",       0xc00,  4, pic12f617_wrt_opts },
};

static const struct gp_cfg_directive mcp25020_002007[] =
{
	{ "OSC",       0x03,  3, mcp25020_osc_opts },
	{ "RSTEN",     0x04,  2, pic10f200_wdte_opts },
};

static const struct gp_cfg_directive pic10f200_000fff[] =
{
	{ "OSC",       0x01,  1, pic10f200_osc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "CP",        0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x10,  2, pic10f200_mclre_opts },
};

static const struct gp_cfg_directive pic10f220_000fff[] =
{
	{ "IOSCFS",    0x01,  2, pic10f220_ioscfs_opts },
	{ "IOFSCS",    0x01,  2, pic10f220_ioscfs_opts },
	{ "MCPU",      0x02,  2, pic10f220_mcpu_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "CP",        0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x10,  2, pic10f200_mclre_opts },
};

static const struct gp_cfg_directive pic10f320_002007[] =
{
	{ "FOSC",      0x01,  2, pic10f320_fosc_opts },
	{ "BOREN",     0x06,  4, pic10f320_boren_opts },
	{ "WDTE",      0x18,  4, pic10f320_wdte_opts },
	{ "PWRTE",     0x20,  2, pic10f320_pwrte_opts },
	{ "MCLRE",     0x40,  2, pic10f320_mclre_opts },
	{ "CP",        0x80,  2, pic10f320_cp_opts },
	{ "LVP",       0x100,  2, pic10f320_lvp_opts },
	{ "LPBOR",     0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "WRT",       0x1800,  4, pic10f320_wrt_opts },
};

static const struct gp_cfg_directive pic12c508_000fff[] =
{
	{ "OSC",       0x03,  4, pic12c508_osc_opts },
	{ "WDT",       0x04,  2, pic10f200_wdte_opts },
	{ "CP",        0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x10,  2, pic10f200_mclre_opts },
};

static const struct gp_cfg_directive pic12c671_002007[] =
{
	{ "FOSC",      0x07,  7, pic12c671_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "CP",        0x3f60,  4, pic12c671_cp_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic12f1501_008007[] =
{
	{ "FOSC",      0x03,  4, pic12f1501_fosc_opts },
	{ "WDTE",      0x18,  4, pic10f320_wdte_opts },
	{ "PWRTE",     0x20,  2, pic10f320_pwrte_opts },
	{ "MCLRE",     0x40,  2, pic10f320_mclre_opts },
	{ "CP",        0x80,  2, pic10f320_cp_opts },
	{ "BOREN",     0x600,  4, pic12f1501_boren_opts },
	{ "CLKOUTEN",  0x800,  2, pic12f1501_clkouten_opts },
};

static const struct gp_cfg_directive pic12f1501_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LPBOR",     0x800,  2, pic12f1501_clkouten_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic12f1571_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "PLLEN",     0x100,  2, pic10f320_lvp_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LPBOREN",   0x800,  2, pic12f1501_clkouten_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic12f1612_008007[] =
{
	{ "FOSC",      0x03,  4, pic12f1501_fosc_opts },
	{ "PWRTE",     0x20,  2, pic10f320_pwrte_opts },
	{ "MCLRE",     0x40,  2, pic10f320_mclre_opts },
	{ "CP",        0x80,  2, pic10f320_cp_opts },
	{ "BOREN",     0x600,  4, pic12f1501_boren_opts },
	{ "CLKOUTEN",  0x800,  2, pic12f1501_clkouten_opts },
};

static const struct gp_cfg_directive pic12f1612_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "ZCDDIS",    0x80,  2, pic12c671_mclre_opts },
	{ "PLLEN",     0x100,  2, pic10f320_lvp_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LPBOR",     0x800,  2, pic12f1501_clkouten_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic12f1612_008009[] =
{
	{ "WDTCPS",    0x1f, 20, pic12f1612_wdtcps_opts },
	{ "WDTE",      0x60,  4, pic12f1612_wdte_opts },
	{ "WDTCWS",    0x700,  8, pic12f1612_wdtcws_opts },
	{ "WDTCCS",    0x3800,  3, pic12f1612_wdtccs_opts },
};

static const struct gp_cfg_directive pic12f1822_008007[] =
{
	{ "FOSC",      0x07,  8, pic12f1822_fosc_opts },
	{ "WDTE",      0x18,  4, pic10f320_wdte_opts },
	{ "PWRTE",     0x20,  2, pic10f320_pwrte_opts },
	{ "MCLRE",     0x40,  2, pic10f320_mclre_opts },
	{ "CP",        0x80,  2, pic10f320_cp_opts },
	{ "CPD",       0x100,  2, pic12f1822_cpd_opts },
	{ "BOREN",     0x600,  4, pic12f1501_boren_opts },
	{ "CLKOUTEN",  0x800,  2, pic12f1501_clkouten_opts },
	{ "IESO",      0x1000,  2, pic12f1822_ieso_opts },
	{ "FCMEN",     0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic12f1822_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "PLLEN",     0x100,  2, pic10f320_lvp_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic12f510_000fff[] =
{
	{ "OSC",       0x03,  4, pic12c508_osc_opts },
	{ "WDT",       0x04,  2, pic10f200_wdte_opts },
	{ "CP",        0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x10,  2, pic10f200_mclre_opts },
	{ "IOSCFS",    0x20,  2, pic12f510_ioscfs_opts },
};

static const struct gp_cfg_directive pic12f519_000fff[] =
{
	{ "FOSC",      0x03,  4, pic12f519_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "CP",        0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x10,  2, pic10f200_mclre_opts },
	{ "IOSCFS",    0x20,  2, pic12f519_ioscfs_opts },
	{ "CPDF",      0x40,  2, pic12f519_cpdf_opts },
};

static const struct gp_cfg_directive pic12f520_000fff[] =
{
	{ "OSC",       0x03,  4, pic12f519_fosc_opts },
	{ "WDT",       0x04,  2, pic10f200_wdte_opts },
	{ "CP",        0x08,  2, pic10f200_cp_opts },
	{ "MCLR",      0x10,  2, pic10f200_mclre_opts },
	{ "IOSCFS",    0x20,  2, pic12f520_ioscfs_opts },
	{ "CPDF",      0x40,  2, pic12f519_cpdf_opts },
};

static const struct gp_cfg_directive pic12f529t39a_000fff[] =
{
	{ "OSC",       0x03,  4, pic12f519_fosc_opts },
	{ "WDT",       0x04,  2, pic10f200_wdte_opts },
	{ "PARITY",    0x08,  2, pic12f529t39a_parity_opts },
	{ "MCLRE",     0x10,  2, pic10f200_mclre_opts },
	{ "IOSCFS",    0x20,  2, pic12f519_ioscfs_opts },
	{ "CPDF",      0x40,  2, pic12f519_cpdf_opts },
	{ "CP",        0x780,  2, pic12f529t39a_cp_opts },
};

static const struct gp_cfg_directive pic12f609_002007[] =
{
	{ "FOSC",      0x07,  8, pic12f609_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "IOSCFS",    0x80,  2, pic12f609_ioscfs_opts },
	{ "BOREN",     0x300,  3, pic12f609_boren_opts },
};

static const struct gp_cfg_directive pic12f617_002007[] =
{
	{ "FOSC",      0x07,  8, pic12f609_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "IOSCFS",    0x80,  2, pic12f609_ioscfs_opts },
	{ "BOREN",     0x300,  3, pic12f609_boren_opts },
	{ "WRT",       0xc00,  4, pic12f617_wrt_opts },
};

static const struct gp_cfg_directive pic12f629_002007[] =
{
	{ "FOSC",      0x07,  8, pic12f629_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "CP",        0x80,  2, pic10f320_cp_opts },
	{ "CPD",       0x100,  2, pic12f1822_cpd_opts },
};

static const struct gp_cfg_directive pic12f635_002007[] =
{
	{ "FOSC",      0x07,  8, pic12f609_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "CPD",       0x80,  2, pic10f320_cp_opts },
	{ "BOREN",     0x300,  4, pic12f635_boren_opts },
	{ "IESO",      0x400,  2, pic12f635_ieso_opts },
	{ "FCMEN",     0x800,  2, pic12f635_fcmen_opts },
	{ "WURE",      0x1000,  2, pic12f635_wure_opts },
};

static const struct gp_cfg_directive pic12f683_002007[] =
{
	{ "FOSC",      0x07,  8, pic12f609_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "CPD",       0x80,  2, pic10f320_cp_opts },
	{ "BOREN",     0x300,  4, pic12f635_boren_opts },
	{ "IESO",      0x400,  2, pic12f635_ieso_opts },
	{ "FCMEN",     0x800,  2, pic12f635_fcmen_opts },
};

static const struct gp_cfg_directive pic12f752_002007[] =
{
	{ "FOSC0",     0x01,  2, pic12f752_fosc0_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "BOREN",     0x300,  3, pic12f752_boren_opts },
	{ "WRT",       0xc00,  4, pic12f752_wrt_opts },
	{ "CLKOUTEN",  0x1000,  2, pic12f635_wure_opts },
};

static const struct gp_cfg_directive pic16c432_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3f30,  4, pic16c432_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic16c433_002007[] =
{
	{ "FOSC",      0x07,  8, pic16c433_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "CP",        0x3f60,  4, pic12c671_cp_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic16c505_000fff[] =
{
	{ "OSC",       0x07,  7, pic16c505_osc_opts },
	{ "WDT",       0x08,  2, pic12c671_wdte_opts },
	{ "CP",        0xfd0,  2, pic16c505_cp_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
};

static const struct gp_cfg_directive pic16c54_000fff[] =
{
	{ "OSC",       0x03,  4, pic16c432_fosc_opts },
	{ "WDT",       0x04,  2, pic10f200_wdte_opts },
	{ "CP",        0x08,  2, pic10f200_cp_opts },
};

static const struct gp_cfg_directive pic16c54c_000fff[] =
{
	{ "OSC",       0x03,  4, pic16c432_fosc_opts },
	{ "WDT",       0x04,  2, pic10f200_wdte_opts },
	{ "CP",        0xff8,  2, pic16c54c_cp_opts },
};

static const struct gp_cfg_directive pic16c554_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3f30,  2, pic16c554_cp_opts },
};

static const struct gp_cfg_directive pic16c557_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3f30,  4, pic16c432_cp_opts },
};

static const struct gp_cfg_directive pic16c620_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3f30,  2, pic16c554_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic16c621_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3f30,  3, pic16c621_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic16c642_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3f30,  4, pic16c432_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "MPEEN",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic16c710_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3fb0,  2, pic16c710_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic16c717_002007[] =
{
	{ "FOSC",      0x07,  8, pic16c717_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "CP",        0x3300,  2, pic16c717_cp_opts },
	{ "BODENV",    0xc00,  4, pic16c717_bodenv_opts },
};

static const struct gp_cfg_directive pic16c71_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic12c671_wdte_opts },
	{ "CP",        0x10,  2, pic12c671_pwrte_opts },
};

static const struct gp_cfg_directive pic16c745_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c745_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3f30,  4, pic16c432_cp_opts },
};

static const struct gp_cfg_directive pic16c773_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3330,  4, pic16c773_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "BODENV",    0xc00,  4, pic16c717_bodenv_opts },
};

static const struct gp_cfg_directive pic16c925_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x30,  4, pic16c925_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic16cr73_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x10,  2, pic12c671_pwrte_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic16cr83_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3f70,  2, pic16cr83_cp_opts },
	{ "DP",        0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic16f1454_008007[] =
{
	{ "FOSC",      0x07,  8, pic12f1822_fosc_opts },
	{ "WDTE",      0x18,  4, pic10f320_wdte_opts },
	{ "PWRTE",     0x20,  2, pic10f320_pwrte_opts },
	{ "MCLRE",     0x40,  2, pic10f320_mclre_opts },
	{ "CP",        0x80,  2, pic10f320_cp_opts },
	{ "BOREN",     0x600,  4, pic12f1501_boren_opts },
	{ "CLKOUTEN",  0x800,  2, pic12f1501_clkouten_opts },
	{ "IESO",      0x1000,  2, pic12f1822_ieso_opts },
	{ "FCMEN",     0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic16f1454_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "CPUDIV",    0x30,  4, pic16f1454_cpudiv_opts },
	{ "USBLSCLK",  0x40,  2, pic16f1454_usblsclk_opts },
	{ "PLLMULT",   0x80,  2, pic16f1454_pllmult_opts },
	{ "PLLEN",     0x100,  2, pic16f1454_pllen_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LPBOR",     0x800,  2, pic12f1501_clkouten_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic16f1512_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "VCAPEN",    0x10,  2, pic12c671_pwrte_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LPBOR",     0x800,  2, pic12f1501_clkouten_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic16f1703_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "PPS1WAY",   0x04,  2, pic10f200_wdte_opts },
	{ "ZCDDIS",    0x80,  2, pic12c671_mclre_opts },
	{ "PLLEN",     0x100,  2, pic10f320_lvp_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LPBOR",     0x800,  2, pic12f1501_clkouten_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic16f1782_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "VCAPEN",    0x20,  2, pic10f320_pwrte_opts },
	{ "PLLEN",     0x100,  2, pic10f320_lvp_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LPBOR",     0x800,  2, pic12f1501_clkouten_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic16f18313_008007[] =
{
	{ "FEXTOSC",   0x07,  8, pic16f18313_fextosc_opts },
	{ "RSTOSC",    0x70,  8, pic16f18313_rstosc_opts },
	{ "CLKOUTEN",  0x100,  2, pic12f1822_cpd_opts },
	{ "CSWEN",     0x800,  2, pic12f635_fcmen_opts },
	{ "FCMEN",     0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic16f18313_008008[] =
{
	{ "MCLRE",     0x01,  2, pic16f18313_mclre_opts },
	{ "PWRTE",     0x02,  2, pic10f220_mcpu_opts },
	{ "WDTE",      0x0c,  4, pic16f18313_wdte_opts },
	{ "LPBOREN",   0x20,  2, pic10f320_pwrte_opts },
	{ "BOREN",     0xc0,  4, pic16f18313_boren_opts },
	{ "BORV",      0x200,  2, pic16f18313_borv_opts },
	{ "PPS1WAY",   0x800,  2, pic12f635_fcmen_opts },
	{ "STVREN",    0x1000,  2, pic12f1822_ieso_opts },
	{ "DEBUG",     0x2000,  2, pic16f18313_debug_opts },
};

static const struct gp_cfg_directive pic16f18313_008009[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic16f18313_00800a[] =
{
	{ "CP",        0x01,  2, pic16f18313_cp_opts },
	{ "CPD",       0x02,  2, pic10f220_mcpu_opts },
};

static const struct gp_cfg_directive pic16f1933_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "VCAPEN",    0x30,  4, pic16f1933_vcapen_opts },
	{ "PLLEN",     0x100,  2, pic10f320_lvp_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic16f1946_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "VCAPEN",    0x10,  2, pic12c671_pwrte_opts },
	{ "PLLEN",     0x100,  2, pic10f320_lvp_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic16f505_000fff[] =
{
	{ "OSC",       0x07,  8, pic16f505_osc_opts },
	{ "WDT",       0x08,  2, pic12c671_wdte_opts },
	{ "CP",        0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
};

static const struct gp_cfg_directive pic16f506_000fff[] =
{
	{ "OSC",       0x07,  8, pic16f505_osc_opts },
	{ "WDT",       0x08,  2, pic12c671_wdte_opts },
	{ "CP",        0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "IOSCFS",    0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic16f526_000fff[] =
{
	{ "FOSC",      0x07,  8, pic16f526_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "CP",        0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "IOSCFS",    0x40,  2, pic16f526_ioscfs_opts },
	{ "CPDF",      0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic16f527_000fff[] =
{
	{ "FOSC",      0x07,  8, pic16f527_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "CP",        0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "IOSCFS",    0x40,  2, pic16f526_ioscfs_opts },
	{ "CPSW",      0x80,  2, pic10f320_cp_opts },
	{ "BOREN",     0x100,  2, pic10f320_lvp_opts },
	{ "DRTEN",     0x200,  2, pic10f320_lpbor_opts },
};

static const struct gp_cfg_directive pic16f570_000fff[] =
{
	{ "FOSC",      0x07,  8, pic16f527_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "CP",        0x10,  2, pic12c671_pwrte_opts },
	{ "IOSCFS",    0x40,  2, pic16f526_ioscfs_opts },
	{ "CPSW",      0x80,  2, pic10f320_cp_opts },
	{ "BOREN",     0x100,  2, pic10f320_lvp_opts },
	{ "DRTEN",     0x200,  2, pic10f320_lpbor_opts },
};

static const struct gp_cfg_directive pic16f627_002007[] =
{
	{ "FOSC",      0x13,  8, pic16f627_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "LVP",       0x80,  2, pic12c671_mclre_opts },
	{ "CPD",       0x100,  2, pic12f1822_cpd_opts },
	{ "CP",        0x3c00,  4, pic16f627_cp_opts },
};

static const struct gp_cfg_directive pic16f627a_002007[] =
{
	{ "FOSC",      0x13,  8, pic16f627a_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "LVP",       0x80,  2, pic12c671_mclre_opts },
	{ "CPD",       0x100,  2, pic12f1822_cpd_opts },
	{ "CP",        0x2000,  2, pic16f18313_debug_opts },
};

static const struct gp_cfg_directive pic16f631_002007[] =
{
	{ "FOSC",      0x07,  8, pic12f629_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "CPD",       0x80,  2, pic10f320_cp_opts },
	{ "BOREN",     0x300,  4, pic12f635_boren_opts },
	{ "IESO",      0x400,  2, pic12f635_ieso_opts },
	{ "FCMEN",     0x800,  2, pic12f635_fcmen_opts },
};

static const struct gp_cfg_directive pic16f707_002007[] =
{
	{ "FOSC",      0x07,  8, pic16f707_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "BOREN",     0x300,  3, pic12f609_boren_opts },
	{ "BORV",      0x400,  2, pic16f707_borv_opts },
	{ "PLLEN",     0x1000,  2, pic12f1822_ieso_opts },
};

static const struct gp_cfg_directive pic16f707_002008[] =
{
	{ "VCAPEN",    0x30,  4, pic16f1933_vcapen_opts },
};

static const struct gp_cfg_directive pic16f716_002007[] =
{
	{ "FOSC",      0x03,  4, pic16c432_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "BODENV",    0x80,  2, pic16f716_bodenv_opts },
	{ "CP",        0x2000,  2, pic16f18313_debug_opts },
};

static const struct gp_cfg_directive pic16f720_002007[] =
{
	{ "FOSC",      0x03,  4, pic16f720_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "BOREN",     0x300,  3, pic12f609_boren_opts },
	{ "PLLEN",     0x1000,  2, pic12f1822_ieso_opts },
};

static const struct gp_cfg_directive pic16f720_002008[] =
{
	{ "WRTEN",     0x03,  4, pic16f720_wrten_opts },
};

static const struct gp_cfg_directive pic16f722_002007[] =
{
	{ "FOSC",      0x07,  8, pic12f609_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "BOREN",     0x300,  3, pic12f609_boren_opts },
	{ "BORV",      0x400,  2, pic16f707_borv_opts },
	{ "PLLEN",     0x1000,  2, pic12f1822_ieso_opts },
	{ "DEBUG",     0x2000,  2, pic16f18313_debug_opts },
};

static const struct gp_cfg_directive pic16f722_002008[] =
{
	{ "VCAPEN",    0x30,  4, pic16f722_vcapen_opts },
};

static const struct gp_cfg_directive pic16f737_002007[] =
{
	{ "FOSC",      0x13,  8, pic16f737_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "BORV",      0x180,  4, pic16f737_borv_opts },
	{ "DEBUG",     0x800,  2, pic12f1501_clkouten_opts },
	{ "CCP2MX",    0x1000,  2, pic16f737_ccp2mx_opts },
	{ "CP",        0x2000,  2, pic16f18313_debug_opts },
};

static const struct gp_cfg_directive pic16f737_002008[] =
{
	{ "FCMEN",     0x01,  2, pic16f18313_mclre_opts },
	{ "IESO",      0x02,  2, pic16f737_ieso_opts },
	{ "BORSEN",    0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic16f818_002007[] =
{
	{ "FOSC",      0x13,  8, pic16f737_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "LVP",       0x80,  2, pic12c671_mclre_opts },
	{ "CPD",       0x100,  2, pic12f1822_cpd_opts },
	{ "WRT",       0x600,  3, pic16f818_wrt_opts },
	{ "DEBUG",     0x800,  2, pic12f1501_clkouten_opts },
	{ "CCPMX",     0x1000,  2, pic16f818_ccpmx_opts },
	{ "CP",        0x2000,  2, pic16f18313_debug_opts },
};

static const struct gp_cfg_directive pic16f819_002007[] =
{
	{ "FOSC",      0x13,  8, pic16f737_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "LVP",       0x80,  2, pic12c671_mclre_opts },
	{ "CPD",       0x100,  2, pic12f1822_cpd_opts },
	{ "WRT",       0x600,  4, pic16f819_wrt_opts },
	{ "DEBUG",     0x800,  2, pic12f1501_clkouten_opts },
	{ "CCPMX",     0x1000,  2, pic16f818_ccpmx_opts },
	{ "CP",        0x2000,  2, pic16f18313_debug_opts },
};

static const struct gp_cfg_directive pic16f83_002007[] =
{
	{ "FOSC",      0x03,  4, pic16f83_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3ff0,  2, pic16f83_cp_opts },
};

static const struct gp_cfg_directive pic16f870_002007[] =
{
	{ "FOSC",      0x03,  4, pic16f83_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3030,  2, pic16f870_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "LVP",       0x80,  2, pic12c671_mclre_opts },
	{ "CPD",       0x100,  2, pic12f1822_cpd_opts },
	{ "WRT",       0x200,  2, pic16f870_wrt_opts },
	{ "DEBUG",     0x800,  2, pic12f1501_clkouten_opts },
};

static const struct gp_cfg_directive pic16f873_002007[] =
{
	{ "FOSC",      0x03,  4, pic16f83_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "CP",        0x3030,  4, pic16f873_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "LVP",       0x80,  2, pic12c671_mclre_opts },
	{ "CPD",       0x100,  2, pic12f1822_cpd_opts },
	{ "WRT",       0x200,  2, pic10f320_lpbor_opts },
	{ "DEBUG",     0x800,  2, pic12f1501_clkouten_opts },
};

static const struct gp_cfg_directive pic16f873a_002007[] =
{
	{ "FOSC",      0x03,  4, pic16f83_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "LVP",       0x80,  2, pic12c671_mclre_opts },
	{ "CPD",       0x100,  2, pic12f1822_cpd_opts },
	{ "WRT",       0x600,  4, pic16f873a_wrt_opts },
	{ "DEBUG",     0x800,  2, pic12f1501_clkouten_opts },
	{ "CP",        0x2000,  2, pic16f18313_debug_opts },
};

static const struct gp_cfg_directive pic16f87_002007[] =
{
	{ "FOSC",      0x13,  8, pic16f737_fosc_opts },
	{ "WDTE",      0x04,  2, pic10f200_wdte_opts },
	{ "PWRTE",     0x08,  2, pic10f200_cp_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "BOREN",     0x40,  2, pic10f320_mclre_opts },
	{ "LVP",       0x80,  2, pic12c671_mclre_opts },
	{ "CPD",       0x100,  2, pic12f1822_cpd_opts },
	{ "WRT",       0x600,  4, pic16f87_wrt_opts },
	{ "DEBUG",     0x800,  2, pic12f1501_clkouten_opts },
	{ "CCPMX",     0x1000,  2, pic16f87_ccpmx_opts },
	{ "CP",        0x2000,  2, pic16f18313_debug_opts },
};

static const struct gp_cfg_directive pic16f87_002008[] =
{
	{ "FCMEN",     0x01,  2, pic16f18313_mclre_opts },
	{ "IESO",      0x02,  2, pic16f737_ieso_opts },
};

static const struct gp_cfg_directive pic16f882_002007[] =
{
	{ "FOSC",      0x07,  8, pic16f882_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "CPD",       0x80,  2, pic10f320_cp_opts },
	{ "BOREN",     0x300,  4, pic12f635_boren_opts },
	{ "IESO",      0x400,  2, pic12f635_ieso_opts },
	{ "FCMEN",     0x800,  2, pic12f635_fcmen_opts },
	{ "LVP",       0x1000,  2, pic12f1822_ieso_opts },
	{ "DEBUG",     0x2000,  2, pic16f18313_debug_opts },
};

static const struct gp_cfg_directive pic16f882_002008[] =
{
	{ "BOR4V",     0x100,  2, pic16f882_bor4v_opts },
	{ "WRT",       0x600,  3, pic16f882_wrt_opts },
};

static const struct gp_cfg_directive pic16f883_002008[] =
{
	{ "BOR4V",     0x100,  2, pic16f882_bor4v_opts },
	{ "WRT",       0x600,  4, pic16f873a_wrt_opts },
};

static const struct gp_cfg_directive pic16f913_002007[] =
{
	{ "FOSC",      0x07,  8, pic12f609_fosc_opts },
	{ "WDTE",      0x08,  2, pic12c671_wdte_opts },
	{ "PWRTE",     0x10,  2, pic12c671_pwrte_opts },
	{ "MCLRE",     0x20,  2, pic12f510_ioscfs_opts },
	{ "CP",        0x40,  2, pic12f519_cpdf_opts },
	{ "CPD",       0x80,  2, pic10f320_cp_opts },
	{ "BOREN",     0x300,  4, pic12f635_boren_opts },
	{ "IESO",      0x400,  2, pic12f635_ieso_opts },
	{ "FCMEN",     0x800,  2, pic12f635_fcmen_opts },
	{ "DEBUG",     0x1000,  2, pic12f635_wure_opts },
};

static const struct gp_cfg_directive pic16lf1782_008008[] =
{
	{ "WRT",       0x03,  4, pic12f1501_wrt_opts },
	{ "PLLEN",     0x100,  2, pic10f320_lvp_opts },
	{ "STVREN",    0x200,  2, pic10f320_lpbor_opts },
	{ "BORV",      0x400,  2, pic10f320_borv_opts },
	{ "LPBOR",     0x800,  2, pic12f1501_clkouten_opts },
	{ "LVP",       0x2000,  2, pic12f1501_lvp_opts },
};

static const struct gp_cfg_directive pic18c242_300000[] =
{
	{ "CP",        0xff,  2, pic18c242_cp_opts },
};

static const struct gp_cfg_directive pic18c242_300001[] =
{
	{ "OSC",       0x07,  8, pic18c242_osc_opts },
	{ "OSCS",      0x20,  2, pic10f320_pwrte_opts },
};

static const struct gp_cfg_directive pic18c242_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BOR",       0x02,  2, pic16f737_ieso_opts },
	{ "BORV",      0x0c,  4, pic18c242_borv_opts },
};

static const struct gp_cfg_directive pic18c242_300003[] =
{
	{ "WDT",       0x01,  2, pic16f18313_mclre_opts },
	{ "WDTPS",     0x0e,  8, pic18c242_wdtps_opts },
};

static const struct gp_cfg_directive pic18c242_300005[] =
{
	{ "CCP2MUX",   0x01,  2, pic16f18313_mclre_opts },
};

static const struct gp_cfg_directive pic18c242_300006[] =
{
	{ "STVR",      0x01,  2, pic16f18313_mclre_opts },
};

static const struct gp_cfg_directive pic18c452_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic16f18313_mclre_opts },
};

static const struct gp_cfg_directive pic18c601_300001[] =
{
	{ "OSC",       0x03,  4, pic18c601_osc_opts },
};

static const struct gp_cfg_directive pic18c601_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BW",        0x40,  2, pic18c601_bw_opts },
};

static const struct gp_cfg_directive pic18f1220_300001[] =
{
	{ "OSC",       0x0f, 10, pic18f1220_osc_opts },
	{ "FSCM",      0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f1220_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BOR",       0x02,  2, pic16f737_ieso_opts },
	{ "BORV",      0x0c,  3, pic18f1220_borv_opts },
};

static const struct gp_cfg_directive pic18f1220_300003[] =
{
	{ "WDT",       0x01,  2, pic16f18313_mclre_opts },
	{ "WDTPS",     0x1e, 16, pic18f1220_wdtps_opts },
};

static const struct gp_cfg_directive pic18f1220_300005[] =
{
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f1220_300006[] =
{
	{ "STVR",      0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f1220_300008[] =
{
	{ "CP0",       0x01,  2, pic16f18313_cp_opts },
	{ "CP1",       0x02,  2, pic10f220_mcpu_opts },
};

static const struct gp_cfg_directive pic18f1220_300009[] =
{
	{ "CPB",       0x40,  2, pic12f519_cpdf_opts },
	{ "CPD",       0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f1220_30000a[] =
{
	{ "WRT0",      0x01,  2, pic16f18313_cp_opts },
	{ "WRT1",      0x02,  2, pic10f220_mcpu_opts },
};

static const struct gp_cfg_directive pic18f1220_30000b[] =
{
	{ "WRTC",      0x20,  2, pic10f320_pwrte_opts },
	{ "WRTB",      0x40,  2, pic12f519_cpdf_opts },
	{ "WRTD",      0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f1220_30000c[] =
{
	{ "EBTR0",     0x01,  2, pic16f18313_cp_opts },
	{ "EBTR1",     0x02,  2, pic10f220_mcpu_opts },
};

static const struct gp_cfg_directive pic18f1220_30000d[] =
{
	{ "EBTRB",     0x40,  2, pic12f519_cpdf_opts },
};

static const struct gp_cfg_directive pic18f1230_300001[] =
{
	{ "OSC",       0x0f, 10, pic18f1230_osc_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f1230_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BOR",       0x06,  4, pic18f1230_bor_opts },
	{ "BORV",      0x18,  4, pic18f1230_borv_opts },
};

static const struct gp_cfg_directive pic18f1230_300004[] =
{
	{ "PWMPIN",    0x02,  2, pic10f220_mcpu_opts },
	{ "LPOL",      0x04,  2, pic18f1230_lpol_opts },
	{ "HPOL",      0x08,  2, pic18f1230_hpol_opts },
};

static const struct gp_cfg_directive pic18f1230_300005[] =
{
	{ "FLTAMX",    0x01,  2, pic18f1230_fltamx_opts },
	{ "T1OSCMX",   0x08,  2, pic18f1230_hpol_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f1230_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "BBSIZ",     0x30,  2, pic18f1230_bbsiz_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f1330_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "BBSIZ",     0x30,  3, pic18f1330_bbsiz_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f13k22_300001[] =
{
	{ "FOSC",      0x0f, 13, pic18f13k22_fosc_opts },
	{ "PLLEN",     0x10,  2, pic10f200_mclre_opts },
	{ "PCLKEN",    0x20,  2, pic12f510_ioscfs_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f13k22_300002[] =
{
	{ "PWRTEN",    0x01,  2, pic16f18313_cp_opts },
	{ "BOREN",     0x06,  4, pic18f13k22_boren_opts },
	{ "BORV",      0x18,  4, pic18f13k22_borv_opts },
};

static const struct gp_cfg_directive pic18f13k22_300003[] =
{
	{ "WDTEN",     0x01,  2, pic16f18313_mclre_opts },
	{ "WDTPS",     0x1e, 16, pic18f1220_wdtps_opts },
};

static const struct gp_cfg_directive pic18f13k22_300005[] =
{
	{ "HFOFST",    0x08,  2, pic12c671_wdte_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f13k22_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "BBSIZ",     0x08,  2, pic12c671_wdte_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f13k50_300000[] =
{
	{ "CPUDIV",    0x18,  4, pic18f13k50_cpudiv_opts },
	{ "USBDIV",    0x20,  2, pic12f510_ioscfs_opts },
};

static const struct gp_cfg_directive pic18f2220_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BOR",       0x02,  2, pic16f737_ieso_opts },
	{ "BORV",      0x0c,  4, pic18f2220_borv_opts },
};

static const struct gp_cfg_directive pic18f2220_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic16f18313_mclre_opts },
	{ "PBAD",      0x02,  2, pic18f2220_pbad_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f2221_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BOR",       0x06,  4, pic18f2221_bor_opts },
	{ "BORV",      0x18,  4, pic18f1230_borv_opts },
};

static const struct gp_cfg_directive pic18f2221_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f2221_ccp2mx_opts },
	{ "PBADEN",    0x02,  2, pic18f2220_pbad_opts },
	{ "LPT1OSC",   0x04,  2, pic10f200_wdte_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f2221_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "BBSIZ",     0x30,  2, pic18f1230_bbsiz_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f2320_300008[] =
{
	{ "CP0",       0x01,  2, pic16f18313_cp_opts },
	{ "CP1",       0x02,  2, pic10f220_mcpu_opts },
	{ "CP2",       0x04,  2, pic18f2320_cp2_opts },
	{ "CP3",       0x08,  2, pic10f200_cp_opts },
};

static const struct gp_cfg_directive pic18f2320_30000a[] =
{
	{ "WRT0",      0x01,  2, pic16f18313_cp_opts },
	{ "WRT1",      0x02,  2, pic10f220_mcpu_opts },
	{ "WRT2",      0x04,  2, pic18f2320_cp2_opts },
	{ "WRT3",      0x08,  2, pic10f200_cp_opts },
};

static const struct gp_cfg_directive pic18f2320_30000c[] =
{
	{ "EBTR0",     0x01,  2, pic16f18313_cp_opts },
	{ "EBTR1",     0x02,  2, pic10f220_mcpu_opts },
	{ "EBTR2",     0x04,  2, pic18f2320_cp2_opts },
	{ "EBTR3",     0x08,  2, pic10f200_cp_opts },
};

static const struct gp_cfg_directive pic18f2321_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "BBSIZ",     0x30,  3, pic18f1330_bbsiz_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f2331_300001[] =
{
	{ "OSC",       0x0f, 12, pic18f2331_osc_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f2331_300002[] =
{
	{ "PWRTEN",    0x01,  2, pic16f18313_cp_opts },
	{ "BOREN",     0x02,  2, pic16f737_ieso_opts },
	{ "BORV",      0x0c,  4, pic18f2220_borv_opts },
};

static const struct gp_cfg_directive pic18f2331_300003[] =
{
	{ "WDTEN",     0x01,  2, pic16f18313_mclre_opts },
	{ "WDPS",      0x1e, 16, pic18f1220_wdtps_opts },
	{ "WINEN",     0x20,  2, pic10f320_pwrte_opts },
};

static const struct gp_cfg_directive pic18f2331_300004[] =
{
	{ "PWMPIN",    0x04,  2, pic18f2320_cp2_opts },
	{ "LPOL",      0x08,  2, pic18f1230_hpol_opts },
	{ "HPOL",      0x10,  2, pic18f2331_hpol_opts },
	{ "T1OSCMX",   0x20,  2, pic12f510_ioscfs_opts },
};

static const struct gp_cfg_directive pic18f2331_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f23k20_300001[] =
{
	{ "FOSC",      0x0f, 10, pic18f23k20_fosc_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f23k20_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BOREN",     0x06,  4, pic18f13k22_boren_opts },
	{ "BORV",      0x18,  4, pic18f23k20_borv_opts },
};

static const struct gp_cfg_directive pic18f23k20_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f23k20_ccp2mx_opts },
	{ "PBADEN",    0x02,  2, pic16f737_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic10f200_wdte_opts },
	{ "HFOFST",    0x08,  2, pic12c671_wdte_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f23k20_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f23k22_300001[] =
{
	{ "FOSC",      0x0f, 14, pic18f23k22_fosc_opts },
	{ "PLLCFG",    0x10,  2, pic10f200_mclre_opts },
	{ "PRICLKEN",  0x20,  2, pic12f510_ioscfs_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f23k22_300002[] =
{
	{ "PWRTEN",    0x01,  2, pic16f18313_cp_opts },
	{ "BOREN",     0x06,  4, pic18f13k22_boren_opts },
	{ "BORV",      0x18,  4, pic18f23k22_borv_opts },
};

static const struct gp_cfg_directive pic18f23k22_300003[] =
{
	{ "WDTEN",     0x03,  4, pic18f23k22_wdten_opts },
	{ "WDTPS",     0x3c, 16, pic18f23k22_wdtps_opts },
};

static const struct gp_cfg_directive pic18f23k22_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f23k22_ccp2mx_opts },
	{ "PBADEN",    0x02,  2, pic16f737_ieso_opts },
	{ "CCP3MX",    0x04,  2, pic18f23k22_ccp3mx_opts },
	{ "HFOFST",    0x08,  2, pic12c671_wdte_opts },
	{ "T3CMX",     0x10,  2, pic18f23k22_t3cmx_opts },
	{ "P2BMX",     0x20,  2, pic18f23k22_p2bmx_opts },
	{ "MCLRE",     0x80,  2, pic18f23k22_mclre_opts },
};

static const struct gp_cfg_directive pic18f2410_300001[] =
{
	{ "OSC",       0x0f, 10, pic18f23k20_fosc_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f2410_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BOREN",     0x06,  4, pic18f13k22_boren_opts },
	{ "BORV",      0x18,  4, pic18f1230_borv_opts },
};

static const struct gp_cfg_directive pic18f2410_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f23k20_ccp2mx_opts },
	{ "PBADEN",    0x02,  2, pic16f737_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic10f200_wdte_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f2410_300009[] =
{
	{ "CPB",       0x40,  2, pic12f519_cpdf_opts },
};

static const struct gp_cfg_directive pic18f2410_30000b[] =
{
	{ "WRTC",      0x20,  2, pic10f320_pwrte_opts },
	{ "WRTB",      0x40,  2, pic12f519_cpdf_opts },
};

static const struct gp_cfg_directive pic18f2420_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f2420_ccp2mx_opts },
	{ "PBADEN",    0x02,  2, pic16f737_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic10f200_wdte_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f2439_300001[] =
{
	{ "OSC",       0x07,  4, pic18f2439_osc_opts },
};

static const struct gp_cfg_directive pic18f2450_300000[] =
{
	{ "PLLDIV",    0x07,  8, pic18f2450_plldiv_opts },
	{ "CPUDIV",    0x18,  4, pic18f2450_cpudiv_opts },
	{ "USBDIV",    0x20,  2, pic18f2450_usbdiv_opts },
};

static const struct gp_cfg_directive pic18f2450_300001[] =
{
	{ "FOSC",      0x0f, 12, pic18f2450_fosc_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f2450_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BOR",       0x06,  4, pic18f2450_bor_opts },
	{ "BORV",      0x18,  4, pic18f2450_borv_opts },
	{ "VREGEN",    0x20,  2, pic12f510_ioscfs_opts },
};

static const struct gp_cfg_directive pic18f2450_300005[] =
{
	{ "PBADEN",    0x02,  2, pic16f737_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic10f200_wdte_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f2450_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "BBSIZ",     0x08,  2, pic18f2450_bbsiz_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f2455_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BOR",       0x06,  4, pic18f2450_bor_opts },
	{ "BORV",      0x18,  4, pic18f1230_borv_opts },
	{ "VREGEN",    0x20,  2, pic12f510_ioscfs_opts },
};

static const struct gp_cfg_directive pic18f2455_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic16f18313_mclre_opts },
	{ "PBADEN",    0x02,  2, pic16f737_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic10f200_wdte_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f2455_300008[] =
{
	{ "CP0",       0x01,  2, pic16f18313_cp_opts },
	{ "CP1",       0x02,  2, pic10f220_mcpu_opts },
	{ "CP2",       0x04,  2, pic18f2320_cp2_opts },
};

static const struct gp_cfg_directive pic18f2455_30000a[] =
{
	{ "WRT0",      0x01,  2, pic16f18313_cp_opts },
	{ "WRT1",      0x02,  2, pic10f220_mcpu_opts },
	{ "WRT2",      0x04,  2, pic18f2320_cp2_opts },
};

static const struct gp_cfg_directive pic18f2455_30000c[] =
{
	{ "EBTR0",     0x01,  2, pic16f18313_cp_opts },
	{ "EBTR1",     0x02,  2, pic10f220_mcpu_opts },
	{ "EBTR2",     0x04,  2, pic18f2320_cp2_opts },
};

static const struct gp_cfg_directive pic18f2480_300001[] =
{
	{ "OSC",       0x0f, 10, pic18f2480_osc_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f2480_300002[] =
{
	{ "PWRT",      0x01,  2, pic16f18313_cp_opts },
	{ "BOREN",     0x06,  4, pic18f1230_bor_opts },
	{ "BORV",      0x18,  4, pic18f1230_borv_opts },
};

static const struct gp_cfg_directive pic18f2480_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "BBSIZ",     0x10,  2, pic18f2480_bbsiz_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f24j10_003ff8[] =
{
	{ "WDTEN",     0x01,  2, pic16f18313_mclre_opts },
	{ "STVREN",    0x20,  2, pic12f510_ioscfs_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f24j10_003ff9[] =
{
	{ "CP0",       0x04,  2, pic18f2320_cp2_opts },
};

static const struct gp_cfg_directive pic18f24j10_003ffa[] =
{
	{ "FOSC",      0x03,  4, pic18f24j10_fosc_opts },
	{ "FOSC2",     0x04,  2, pic10f200_wdte_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f24j10_003ffb[] =
{
	{ "WDTPS",     0x0f, 16, pic18f24j10_wdtps_opts },
};

static const struct gp_cfg_directive pic18f24j10_003ffd[] =
{
	{ "CCP2MX",    0x01,  2, pic18f24j10_ccp2mx_opts },
};

static const struct gp_cfg_directive pic18f24j11_003ffa[] =
{
	{ "OSC",       0x07,  8, pic18f24j11_osc_opts },
	{ "T1DIG",     0x08,  2, pic12c671_wdte_opts },
	{ "LPT1OSC",   0x10,  2, pic12c671_pwrte_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f24j11_003ffc[] =
{
	{ "DSWDTOSC",  0x01,  2, pic18f24j11_dswdtosc_opts },
	{ "RTCOSC",    0x02,  2, pic18f24j11_rtcosc_opts },
	{ "DSBOREN",   0x04,  2, pic10f200_wdte_opts },
	{ "DSWDTEN",   0x08,  2, pic12c671_wdte_opts },
	{ "DSWDTPS",   0xf0, 16, pic18f24j11_dswdtps_opts },
};

static const struct gp_cfg_directive pic18f24j11_003ffd[] =
{
	{ "IOL1WAY",   0x01,  2, pic16f18313_mclre_opts },
	{ "MSSP7B_EN", 0x08,  2, pic18f24j11_mssp7b_en_opts },
};

static const struct gp_cfg_directive pic18f24j11_003ffe[] =
{
	{ "WPFP",      0x0f, 16, pic18f24j11_wpfp_opts },
	{ "WPEND",     0x40,  2, pic18f24j11_wpend_opts },
	{ "WPCFG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f24j11_003fff[] =
{
	{ "WPDIS",     0x01,  2, pic16f18313_cp_opts },
};

static const struct gp_cfg_directive pic18f24j50_003ff8[] =
{
	{ "WDTEN",     0x01,  2, pic16f18313_mclre_opts },
	{ "PLLDIV",    0x0e,  8, pic18f24j50_plldiv_opts },
	{ "STVREN",    0x20,  2, pic12f510_ioscfs_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f24j50_003ff9[] =
{
	{ "CPUDIV",    0x03,  4, pic18f24j50_cpudiv_opts },
	{ "CP0",       0x04,  2, pic18f2320_cp2_opts },
};

static const struct gp_cfg_directive pic18f24k50_300000[] =
{
	{ "PLLSEL",    0x01,  2, pic18f24k50_pllsel_opts },
	{ "CFGPLLEN",  0x02,  2, pic16f737_ieso_opts },
	{ "CPUDIV",    0x18,  4, pic18f24k50_cpudiv_opts },
	{ "LS48MHZ",   0x20,  2, pic18f24k50_ls48mhz_opts },
};

static const struct gp_cfg_directive pic18f24k50_300001[] =
{
	{ "FOSC",      0x0f, 14, pic18f24k50_fosc_opts },
	{ "PCLKEN",    0x20,  2, pic12f510_ioscfs_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f24k50_300002[] =
{
	{ "nPWRTEN",   0x01,  2, pic16f18313_cp_opts },
	{ "BOREN",     0x06,  4, pic18f13k22_boren_opts },
	{ "BORV",      0x18,  4, pic18f23k22_borv_opts },
	{ "nLPBOR",    0x40,  2, pic12f519_cpdf_opts },
};

static const struct gp_cfg_directive pic18f24k50_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f2221_ccp2mx_opts },
	{ "PBADEN",    0x02,  2, pic16f737_ieso_opts },
	{ "T3CMX",     0x10,  2, pic18f24k50_t3cmx_opts },
	{ "SDOMX",     0x40,  2, pic18f24k50_sdomx_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f24k50_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "ICPRT",     0x20,  1, pic18f24k50_icprt_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f2585_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "BBSIZ",     0x30,  3, pic18f2585_bbsiz_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f25j11_007ffe[] =
{
	{ "WPFP",      0x1f, 32, pic18f25j11_wpfp_opts },
	{ "WPEND",     0x40,  2, pic18f24j11_wpend_opts },
	{ "WPCFG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f25k80_300000[] =
{
	{ "RETEN",     0x01,  2, pic16f18313_cp_opts },
	{ "INTOSCSEL", 0x04,  2, pic18f1230_lpol_opts },
	{ "SOSCSEL",   0x18,  3, pic18f25k80_soscsel_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic18f25k80_300001[] =
{
	{ "FOSC",      0x0f, 14, pic18f25k80_fosc_opts },
	{ "PLLCFG",    0x10,  2, pic10f200_mclre_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f25k80_300002[] =
{
	{ "PWRTEN",    0x01,  2, pic16f18313_cp_opts },
	{ "BOREN",     0x06,  4, pic18f13k22_boren_opts },
	{ "BORV",      0x18,  4, pic18f1230_borv_opts },
	{ "BORPWR",    0x60,  4, pic18f25k80_borpwr_opts },
};

static const struct gp_cfg_directive pic18f25k80_300003[] =
{
	{ "WDTEN",     0x03,  4, pic18f25k80_wdten_opts },
	{ "WDTPS",     0x7c, 21, pic18f25k80_wdtps_opts },
};

static const struct gp_cfg_directive pic18f25k80_300005[] =
{
	{ "CANMX",     0x01,  2, pic18f25k80_canmx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f24j11_mssp7b_en_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f25k80_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "BBSIZ",     0x10,  2, pic18f25k80_bbsiz_opts },
};

static const struct gp_cfg_directive pic18f2682_300008[] =
{
	{ "CP0",       0x01,  2, pic16f18313_cp_opts },
	{ "CP1",       0x02,  2, pic10f220_mcpu_opts },
	{ "CP2",       0x04,  2, pic18f2320_cp2_opts },
	{ "CP3",       0x08,  2, pic10f200_cp_opts },
	{ "CP4",       0x10,  2, pic12c671_pwrte_opts },
};

static const struct gp_cfg_directive pic18f2682_30000a[] =
{
	{ "WRT0",      0x01,  2, pic16f18313_cp_opts },
	{ "WRT1",      0x02,  2, pic10f220_mcpu_opts },
	{ "WRT2",      0x04,  2, pic18f2320_cp2_opts },
	{ "WRT3",      0x08,  2, pic10f200_cp_opts },
	{ "WRT4",      0x10,  2, pic12c671_pwrte_opts },
};

static const struct gp_cfg_directive pic18f2682_30000c[] =
{
	{ "EBTR0",     0x01,  2, pic16f18313_cp_opts },
	{ "EBTR1",     0x02,  2, pic10f220_mcpu_opts },
	{ "EBTR2",     0x04,  2, pic18f2320_cp2_opts },
	{ "EBTR3",     0x08,  2, pic10f200_cp_opts },
	{ "EBTR4",     0x10,  2, pic12c671_pwrte_opts },
};

static const struct gp_cfg_directive pic18f2685_300008[] =
{
	{ "CP0",       0x01,  2, pic16f18313_cp_opts },
	{ "CP1",       0x02,  2, pic10f220_mcpu_opts },
	{ "CP2",       0x04,  2, pic18f2320_cp2_opts },
	{ "CP3",       0x08,  2, pic10f200_cp_opts },
	{ "CP4",       0x10,  2, pic12c671_pwrte_opts },
	{ "CP5",       0x20,  2, pic10f320_pwrte_opts },
};

static const struct gp_cfg_directive pic18f2685_30000a[] =
{
	{ "WRT0",      0x01,  2, pic16f18313_cp_opts },
	{ "WRT1",      0x02,  2, pic10f220_mcpu_opts },
	{ "WRT2",      0x04,  2, pic18f2320_cp2_opts },
	{ "WRT3",      0x08,  2, pic10f200_cp_opts },
	{ "WRT4",      0x10,  2, pic12c671_pwrte_opts },
	{ "WRT5",      0x20,  2, pic10f320_pwrte_opts },
};

static const struct gp_cfg_directive pic18f2685_30000c[] =
{
	{ "EBTR0",     0x01,  2, pic16f18313_cp_opts },
	{ "EBTR1",     0x02,  2, pic10f220_mcpu_opts },
	{ "EBTR2",     0x04,  2, pic18f2320_cp2_opts },
	{ "EBTR3",     0x08,  2, pic10f200_cp_opts },
	{ "EBTR4",     0x10,  2, pic12c671_pwrte_opts },
	{ "EBTR5",     0x20,  2, pic10f320_pwrte_opts },
};

static const struct gp_cfg_directive pic18f26j11_00fffe[] =
{
	{ "WPFP",      0x3f, 64, pic18f26j11_wpfp_opts },
	{ "WPEND",     0x40,  2, pic18f24j11_wpend_opts },
	{ "WPCFG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f26j13_00fff8[] =
{
	{ "WDTEN",     0x01,  2, pic16f18313_mclre_opts },
	{ "PLLDIV",    0x0e,  8, pic18f24j50_plldiv_opts },
	{ "CFGPLLEN",  0x10,  2, pic12c671_pwrte_opts },
	{ "STVREN",    0x20,  2, pic12f510_ioscfs_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic18f26j13_00fffa[] =
{
	{ "OSC",       0x07,  8, pic18f24j11_osc_opts },
	{ "SOSCSEL",   0x18,  4, pic18f26j13_soscsel_opts },
	{ "CLKOEC",    0x20,  2, pic12f510_ioscfs_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f26j13_00fffd[] =
{
	{ "IOL1WAY",   0x01,  2, pic16f18313_mclre_opts },
	{ "ADCSEL",    0x02,  2, pic18f26j13_adcsel_opts },
	{ "PLLSEL",    0x04,  2, pic18f26j13_pllsel_opts },
	{ "MSSP7B_EN", 0x08,  2, pic18f24j11_mssp7b_en_opts },
};

static const struct gp_cfg_directive pic18f26j13_00fffe[] =
{
	{ "WPFP",      0x3f, 64, pic18f26j11_wpfp_opts },
	{ "WPCFG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f26j13_00ffff[] =
{
	{ "WPDIS",     0x01,  2, pic16f18313_cp_opts },
	{ "WPEND",     0x02,  2, pic18f26j13_wpend_opts },
};

static const struct gp_cfg_directive pic18f26j53_00fffd[] =
{
	{ "IOL1WAY",   0x01,  2, pic16f18313_mclre_opts },
	{ "ADCSEL",    0x02,  2, pic18f26j13_adcsel_opts },
	{ "MSSP7B_EN", 0x08,  2, pic18f24j11_mssp7b_en_opts },
};

static const struct gp_cfg_directive pic18f26j53_00ffff[] =
{
	{ "WPDIS",     0x01,  2, pic16f18313_cp_opts },
	{ "WPEND",     0x02,  2, pic18f26j13_wpend_opts },
	{ "LS48MHZ",   0x08,  2, pic18f26j53_ls48mhz_opts },
};

static const struct gp_cfg_directive pic18f27j13_01fffe[] =
{
	{ "WPFP",      0x7f, 128, pic18f27j13_wpfp_opts },
	{ "WPCFG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f4331_300005[] =
{
	{ "FLTAMX",    0x01,  2, pic18f4331_fltamx_opts },
	{ "SSPMX",     0x04,  2, pic18f4331_sspmx_opts },
	{ "PWM4MX",    0x08,  2, pic18f4331_pwm4mx_opts },
	{ "EXCLKMX",   0x10,  2, pic18f4331_exclkmx_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f43k22_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f23k22_ccp2mx_opts },
	{ "PBADEN",    0x02,  2, pic16f737_ieso_opts },
	{ "CCP3MX",    0x04,  2, pic18f43k22_ccp3mx_opts },
	{ "HFOFST",    0x08,  2, pic12c671_wdte_opts },
	{ "T3CMX",     0x10,  2, pic18f23k22_t3cmx_opts },
	{ "P2BMX",     0x20,  2, pic18f43k22_p2bmx_opts },
	{ "MCLRE",     0x80,  2, pic18f23k22_mclre_opts },
};

static const struct gp_cfg_directive pic18f4450_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "BBSIZ",     0x08,  2, pic18f2450_bbsiz_opts },
	{ "ICPRT",     0x20,  2, pic12f510_ioscfs_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f4455_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "ICPRT",     0x20,  2, pic12f510_ioscfs_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f6310_300001[] =
{
	{ "OSC",       0x0f, 10, pic18f6310_osc_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f6310_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f6310_ccp2mx_opts },
	{ "LPT1OSC",   0x04,  2, pic10f200_wdte_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f6310_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f6310_300008[] =
{
	{ "CP",        0x01,  2, pic16f18313_cp_opts },
};

static const struct gp_cfg_directive pic18f6390_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f23k20_ccp2mx_opts },
	{ "LPT1OSC",   0x04,  2, pic10f200_wdte_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f6525_300001[] =
{
	{ "OSC",       0x0f, 11, pic18f6525_osc_opts },
	{ "OSCS",      0x20,  2, pic10f320_pwrte_opts },
};

static const struct gp_cfg_directive pic18f6525_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f23k20_ccp2mx_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f6527_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "LVP",       0x04,  2, pic10f200_wdte_opts },
	{ "BBSIZ",     0x30,  3, pic18f6527_bbsiz_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f6585_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic16f18313_mclre_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f65j50_007ffa[] =
{
	{ "FOSC",      0x07,  8, pic18f24j11_osc_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f65j50_007ffd[] =
{
	{ "CCP2MX",    0x01,  2, pic18f24j10_ccp2mx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f24j11_mssp7b_en_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ff0[] =
{
	{ "STVREN",    0x20,  2, pic12f510_ioscfs_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ff1[] =
{
	{ "BOREN",     0x01,  2, pic16f18313_mclre_opts },
	{ "BORV",      0x02,  2, pic18f65j94_borv_opts },
	{ "CP0",       0x04,  2, pic18f2320_cp2_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ff2[] =
{
	{ "FOSC",      0x07,  8, pic18f65j94_fosc_opts },
	{ "SOSCSEL",   0x08,  2, pic18f65j94_soscsel_opts },
	{ "CLKOEN",    0x20,  2, pic12f510_ioscfs_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ff3[] =
{
	{ "PLLDIV",    0x0f, 16, pic18f65j94_plldiv_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ff4[] =
{
	{ "POSCMD",    0x03,  4, pic18f65j94_poscmd_opts },
	{ "FSCM",      0x30,  3, pic18f65j94_fscm_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ff6[] =
{
	{ "WPFP",      0xff, 256, pic18f65j94_wpfp_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ff7[] =
{
	{ "WPDIS",     0x01,  2, pic18f65j94_wpdis_opts },
	{ "WPEND",     0x02,  2, pic18f65j94_wpend_opts },
	{ "WPCFG",     0x04,  2, pic18f65j94_wpcfg_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ff8[] =
{
	{ "T5GSEL",    0x01,  2, pic18f65j94_t5gsel_opts },
	{ "CINASEL",   0x02,  2, pic18f65j94_cinasel_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ff9[] =
{
	{ "IOL1WAY",   0x01,  2, pic16f18313_mclre_opts },
	{ "LS48MHZ",   0x02,  2, pic18f65j94_ls48mhz_opts },
	{ "MSSPMSK2",  0x04,  2, pic18f65j94_msspmsk2_opts },
	{ "MSSPMSK1",  0x08,  2, pic18f24j11_mssp7b_en_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ffa[] =
{
	{ "WDTWIN",    0x03,  4, pic18f65j94_wdtwin_opts },
	{ "WDTCLK",    0x0c,  4, pic18f65j94_wdtclk_opts },
	{ "WDTPS",     0xf0, 16, pic18f65j94_wdtps_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ffb[] =
{
	{ "WDTEN",     0x03,  4, pic18f25k80_wdten_opts },
	{ "WINDIS",    0x04,  2, pic18f65j94_windis_opts },
	{ "WPSA",      0x08,  2, pic18f65j94_wpsa_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ffc[] =
{
	{ "RETEN",     0x01,  2, pic16f18313_cp_opts },
	{ "VBTBOR",    0x04,  2, pic10f200_wdte_opts },
	{ "DSBOREN",   0x08,  2, pic12c671_wdte_opts },
	{ "DSBITEN",   0x10,  2, pic10f200_mclre_opts },
};

static const struct gp_cfg_directive pic18f65j94_007ffe[] =
{
	{ "DSWDTPS",   0xf8, 32, pic18f65j94_dswdtps_opts },
};

static const struct gp_cfg_directive pic18f65j94_007fff[] =
{
	{ "DSWDTEN",   0x01,  2, pic16f18313_mclre_opts },
	{ "DSWDTOSC",  0x02,  2, pic18f65j94_dswdtosc_opts },
};

static const struct gp_cfg_directive pic18f65k22_300000[] =
{
	{ "RETEN",     0x01,  2, pic16f18313_mclre_opts },
	{ "INTOSCSEL", 0x04,  2, pic18f1230_lpol_opts },
	{ "SOSCSEL",   0x18,  3, pic18f25k80_soscsel_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic18f65k22_300004[] =
{
	{ "RTCOSC",    0x01,  2, pic18f65k22_rtcosc_opts },
};

static const struct gp_cfg_directive pic18f65k22_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f23k20_ccp2mx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f24j11_mssp7b_en_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f65k22_300006[] =
{
	{ "STVREN",    0x01,  2, pic16f18313_mclre_opts },
	{ "BBSIZ",     0x10,  2, pic18f25k80_bbsiz_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f65k22_30000c[] =
{
	{ "EBRT0",     0x01,  2, pic16f18313_cp_opts },
	{ "EBRT1",     0x02,  2, pic10f220_mcpu_opts },
	{ "EBRT2",     0x04,  2, pic18f2320_cp2_opts },
	{ "EBRT3",     0x08,  2, pic10f200_cp_opts },
};

static const struct gp_cfg_directive pic18f65k22_30000d[] =
{
	{ "EBRTB",     0x40,  2, pic12f519_cpdf_opts },
};

static const struct gp_cfg_directive pic18f65k80_300005[] =
{
	{ "CANMX",     0x01,  2, pic18f65k80_canmx_opts },
	{ "T0CKMX",    0x02,  2, pic18f65k80_t0ckmx_opts },
	{ "T3CKMX",    0x04,  2, pic18f65k80_t3ckmx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f24j11_mssp7b_en_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f66j60_00fff8[] =
{
	{ "WDT",       0x01,  2, pic16f18313_mclre_opts },
	{ "STVR",      0x20,  2, pic12f510_ioscfs_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
	{ "DEBUG",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f66j60_00fffd[] =
{
	{ "ETHLED",    0x04,  2, pic10f200_wdte_opts },
};

static const struct gp_cfg_directive pic18f66j90_00fffa[] =
{
	{ "OSC",       0x07,  8, pic18f66j90_osc_opts },
	{ "T1DIG",     0x08,  2, pic12c671_wdte_opts },
	{ "LPT1OSC",   0x10,  2, pic12c671_pwrte_opts },
	{ "FCMEN",     0x40,  2, pic10f320_mclre_opts },
	{ "IESO",      0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f66j90_00fffc[] =
{
	{ "RTCSOSC",   0x02,  2, pic18f24j11_rtcosc_opts },
};

static const struct gp_cfg_directive pic18f66j93_00fff8[] =
{
	{ "WDTEN",     0x01,  2, pic16f18313_mclre_opts },
	{ "STVREN",    0x20,  2, pic12f510_ioscfs_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive pic18f6720_300008[] =
{
	{ "CP0",       0x01,  2, pic16f18313_cp_opts },
	{ "CP1",       0x02,  2, pic10f220_mcpu_opts },
	{ "CP2",       0x04,  2, pic18f2320_cp2_opts },
	{ "CP3",       0x08,  2, pic10f200_cp_opts },
	{ "CP4",       0x10,  2, pic12c671_pwrte_opts },
	{ "CP5",       0x20,  2, pic10f320_pwrte_opts },
	{ "CP6",       0x40,  2, pic12f519_cpdf_opts },
	{ "CP7",       0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f6720_30000a[] =
{
	{ "WRT0",      0x01,  2, pic16f18313_cp_opts },
	{ "WRT1",      0x02,  2, pic10f220_mcpu_opts },
	{ "WRT2",      0x04,  2, pic18f2320_cp2_opts },
	{ "WRT3",      0x08,  2, pic10f200_cp_opts },
	{ "WRT4",      0x10,  2, pic12c671_pwrte_opts },
	{ "WRT5",      0x20,  2, pic10f320_pwrte_opts },
	{ "WRT6",      0x40,  2, pic12f519_cpdf_opts },
	{ "WRT7",      0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f6720_30000c[] =
{
	{ "EBTR0",     0x01,  2, pic16f18313_cp_opts },
	{ "EBTR1",     0x02,  2, pic10f220_mcpu_opts },
	{ "EBTR2",     0x04,  2, pic18f2320_cp2_opts },
	{ "EBTR3",     0x08,  2, pic10f200_cp_opts },
	{ "EBTR4",     0x10,  2, pic12c671_pwrte_opts },
	{ "EBTR5",     0x20,  2, pic10f320_pwrte_opts },
	{ "EBTR6",     0x40,  2, pic12f519_cpdf_opts },
	{ "EBTR7",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f67k22_30000c[] =
{
	{ "EBRT0",     0x01,  2, pic16f18313_cp_opts },
	{ "EBRT1",     0x02,  2, pic10f220_mcpu_opts },
	{ "EBRT2",     0x04,  2, pic18f2320_cp2_opts },
	{ "EBRT3",     0x08,  2, pic10f200_cp_opts },
	{ "EBRT4",     0x10,  2, pic12c671_pwrte_opts },
	{ "EBRT5",     0x20,  2, pic10f320_pwrte_opts },
	{ "EBRT6",     0x40,  2, pic12f519_cpdf_opts },
	{ "EBRT7",     0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f8310_300004[] =
{
	{ "PM",        0x03,  4, pic18f8310_pm_opts },
	{ "BW",        0x40,  2, pic18c601_bw_opts },
	{ "WAIT",      0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f8310_30000c[] =
{
	{ "EBTR",      0x01,  2, pic16f18313_cp_opts },
};

static const struct gp_cfg_directive pic18f83j11_001ffc[] =
{
	{ "EASHFT",    0x08,  2, pic12c671_wdte_opts },
	{ "MODE",      0x30,  4, pic18f83j11_mode_opts },
	{ "BW",        0x40,  2, pic18c601_bw_opts },
	{ "WAIT",      0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f8520_300004[] =
{
	{ "MODE",      0x03,  4, pic18f8310_pm_opts },
	{ "WAIT",      0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f8525_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f23k20_ccp2mx_opts },
	{ "ECCPMX",    0x02,  2, pic18f8525_eccpmx_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f8527_300004[] =
{
	{ "MODE",      0x03,  4, pic18f8310_pm_opts },
	{ "ADDRBW",    0x30,  4, pic18f8527_addrbw_opts },
	{ "DATABW",    0x40,  2, pic18f8527_databw_opts },
	{ "WAIT",      0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f8527_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f23k20_ccp2mx_opts },
	{ "ECCPMX",    0x02,  2, pic18f8525_eccpmx_opts },
	{ "LPT1OSC",   0x04,  2, pic10f200_wdte_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f8585_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic16f18313_mclre_opts },
	{ "ECCPMX",    0x02,  2, pic18f8525_eccpmx_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f85j10_007ffd[] =
{
	{ "CCP2MX",    0x01,  2, pic18f24j10_ccp2mx_opts },
	{ "ECCPMX",    0x02,  2, pic18f85j10_eccpmx_opts },
};

static const struct gp_cfg_directive pic18f85j50_007ffd[] =
{
	{ "CCP2MX",    0x01,  2, pic18f24j10_ccp2mx_opts },
	{ "ECCPMX",    0x02,  2, pic18f85j10_eccpmx_opts },
	{ "PMPMX",     0x04,  2, pic18f85j50_pmpmx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f24j11_mssp7b_en_opts },
};

static const struct gp_cfg_directive pic18f85j94_007ff8[] =
{
	{ "T5GSEL",    0x01,  2, pic18f65j94_t5gsel_opts },
	{ "CINASEL",   0x02,  2, pic18f65j94_cinasel_opts },
	{ "EASHFT",    0x08,  2, pic12c671_wdte_opts },
	{ "ABW",       0x30,  4, pic18f83j11_mode_opts },
	{ "BW",        0x40,  2, pic18c601_bw_opts },
	{ "WAIT",      0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f85k22_300004[] =
{
	{ "RTCOSC",    0x01,  2, pic18f65k22_rtcosc_opts },
	{ "EASHFT",    0x08,  2, pic12c671_wdte_opts },
	{ "ABW",       0x30,  4, pic18f83j11_mode_opts },
	{ "BW",        0x40,  2, pic18c601_bw_opts },
	{ "WAIT",      0x80,  2, pic10f320_cp_opts },
};

static const struct gp_cfg_directive pic18f85k22_300005[] =
{
	{ "CCP2MX",    0x01,  2, pic18f23k20_ccp2mx_opts },
	{ "ECCPMX",    0x02,  2, pic18f8525_eccpmx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f24j11_mssp7b_en_opts },
	{ "MCLRE",     0x80,  2, pic12c671_mclre_opts },
};

static const struct gp_cfg_directive pic18f86j60_00fffd[] =
{
	{ "CCP2MX",    0x01,  2, pic16f18313_mclre_opts },
	{ "ECCPMX",    0x02,  2, pic16f737_ieso_opts },
	{ "ETHLED",    0x04,  2, pic10f200_wdte_opts },
};

static const struct gp_cfg_directive pic18lf24j50_003ff8[] =
{
	{ "WDTEN",     0x01,  2, pic16f18313_mclre_opts },
	{ "PLLDIV",    0x0e,  8, pic18f24j50_plldiv_opts },
	{ "STVREN",    0x20,  2, pic12f510_ioscfs_opts },
	{ "XINST",     0x40,  2, pic10f320_mclre_opts },
};

static const struct gp_cfg_directive ps500_300003[] =
{
	{ "WDT",       0x01,  2, pic16f18313_mclre_opts },
};

static const struct gp_cfg_directive ps500_300008[] =
{
	{ "CP01",      0x01,  2, pic16f18313_cp_opts },
	{ "CP23",      0x02,  2, pic10f220_mcpu_opts },
};

static const struct gp_cfg_directive ps500_30000a[] =
{
	{ "WRT01",     0x01,  2, pic16f18313_cp_opts },
	{ "WRT23",     0x02,  2, pic10f220_mcpu_opts },
};

static const struct gp_cfg_directive ps500_30000c[] =
{
	{ "EBTR01",    0x01,  2, pic16f18313_cp_opts },
	{ "EBTR23",    0x02,  2, pic10f220_mcpu_opts },
};

static const struct gp_cfg_directive ps810_300008[] =
{
	{ "CP01",      0x01,  2, pic16f18313_cp_opts },
};

static const struct gp_cfg_directive ps810_30000a[] =
{
	{ "WRT01",     0x01,  2, pic16f18313_cp_opts },
};

static const struct gp_cfg_directive ps810_30000c[] =
{
	{ "EBTR01",    0x01,  2, pic16f18313_cp_opts },
};

static const struct gp_cfg_addr gp_cfg_mcp19110_addrs[] =
{
	{ 0x002007, 0xffff,  5, mcp19110_002007 },
};

static const struct gp_cfg_addr gp_cfg_mcp19114_addrs[] =
{
	{ 0x002007, 0xffff,  6, mcp19114_002007 },
};

static const struct gp_cfg_addr gp_cfg_mcp25020_addrs[] =
{
	{ 0x002007, 0xffff,  2, mcp25020_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic10f200_addrs[] =
{
	{ 0x000fff, 0xffff,  4, pic10f200_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic10f220_addrs[] =
{
	{ 0x000fff, 0xffff,  6, pic10f220_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic10f320_addrs[] =
{
	{ 0x002007, 0x3fff, 10, pic10f320_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic12c508_addrs[] =
{
	{ 0x000fff, 0xffff,  4, pic12c508_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic12c671_addrs[] =
{
	{ 0x002007, 0xffff,  5, pic12c671_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic12f1501_addrs[] =
{
	{ 0x008007, 0xffff,  7, pic12f1501_008007 },
	{ 0x008008, 0xffff,  5, pic12f1501_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic12f1571_addrs[] =
{
	{ 0x008007, 0xffff,  7, pic12f1501_008007 },
	{ 0x008008, 0xffff,  6, pic12f1571_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic12f1612_addrs[] =
{
	{ 0x008007, 0xffff,  6, pic12f1612_008007 },
	{ 0x008008, 0xffff,  7, pic12f1612_008008 },
	{ 0x008009, 0xffff,  4, pic12f1612_008009 },
};

static const struct gp_cfg_addr gp_cfg_pic12f1822_addrs[] =
{
	{ 0x008007, 0xffff, 10, pic12f1822_008007 },
	{ 0x008008, 0xffff,  5, pic12f1822_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic12f510_addrs[] =
{
	{ 0x000fff, 0xffff,  5, pic12f510_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic12f519_addrs[] =
{
	{ 0x000fff, 0xffff,  6, pic12f519_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic12f520_addrs[] =
{
	{ 0x000fff, 0xffff,  6, pic12f520_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic12f529t39a_addrs[] =
{
	{ 0x000fff, 0xdff,  7, pic12f529t39a_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic12f609_addrs[] =
{
	{ 0x002007, 0xffff,  7, pic12f609_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic12f617_addrs[] =
{
	{ 0x002007, 0xffff,  8, pic12f617_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic12f629_addrs[] =
{
	{ 0x002007, 0x31ff,  7, pic12f629_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic12f635_addrs[] =
{
	{ 0x002007, 0xffff, 10, pic12f635_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic12f683_addrs[] =
{
	{ 0x002007, 0xffff,  9, pic12f683_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic12f752_addrs[] =
{
	{ 0x002007, 0xffff,  8, pic12f752_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c432_addrs[] =
{
	{ 0x002007, 0xffff,  5, pic16c432_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c433_addrs[] =
{
	{ 0x002007, 0xffff,  5, pic16c433_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c505_addrs[] =
{
	{ 0x000fff, 0xffff,  4, pic16c505_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic16c54_addrs[] =
{
	{ 0x000fff, 0xffff,  3, pic16c54_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic16c54c_addrs[] =
{
	{ 0x000fff, 0xffff,  3, pic16c54c_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic16c554_addrs[] =
{
	{ 0x002007, 0xffff,  4, pic16c554_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c557_addrs[] =
{
	{ 0x002007, 0xffff,  4, pic16c557_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c620_addrs[] =
{
	{ 0x002007, 0xffff,  5, pic16c620_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c621_addrs[] =
{
	{ 0x002007, 0xffff,  5, pic16c621_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c642_addrs[] =
{
	{ 0x002007, 0xffff,  6, pic16c642_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c710_addrs[] =
{
	{ 0x002007, 0xffff,  5, pic16c710_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c717_addrs[] =
{
	{ 0x002007, 0xffff,  7, pic16c717_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c71_addrs[] =
{
	{ 0x002007, 0xffff,  4, pic16c71_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c745_addrs[] =
{
	{ 0x002007, 0xffff,  4, pic16c745_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c773_addrs[] =
{
	{ 0x002007, 0xffff,  6, pic16c773_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16c925_addrs[] =
{
	{ 0x002007, 0xffff,  5, pic16c925_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16cr73_addrs[] =
{
	{ 0x002007, 0xffff,  5, pic16cr73_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16cr83_addrs[] =
{
	{ 0x002007, 0xffff,  5, pic16cr83_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f1454_addrs[] =
{
	{ 0x008007, 0xffff,  9, pic16f1454_008007 },
	{ 0x008008, 0xffff,  9, pic16f1454_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f1508_addrs[] =
{
	{ 0x008007, 0xffff,  9, pic16f1454_008007 },
	{ 0x008008, 0xffff,  5, pic12f1501_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f1512_addrs[] =
{
	{ 0x008007, 0xffff,  9, pic16f1454_008007 },
	{ 0x008008, 0xffff,  6, pic16f1512_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f1703_addrs[] =
{
	{ 0x008007, 0xffff,  7, pic12f1501_008007 },
	{ 0x008008, 0xffff,  8, pic16f1703_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f1704_addrs[] =
{
	{ 0x008007, 0xffff,  9, pic16f1454_008007 },
	{ 0x008008, 0xffff,  8, pic16f1703_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f1782_addrs[] =
{
	{ 0x008007, 0xffff, 10, pic12f1822_008007 },
	{ 0x008008, 0xffff,  7, pic16f1782_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f18313_addrs[] =
{
	{ 0x008007, 0xffff,  5, pic16f18313_008007 },
	{ 0x008008, 0xffff,  9, pic16f18313_008008 },
	{ 0x008009, 0xffff,  2, pic16f18313_008009 },
	{ 0x00800a, 0xffff,  2, pic16f18313_00800a },
};

static const struct gp_cfg_addr gp_cfg_pic16f1933_addrs[] =
{
	{ 0x008007, 0xffff, 10, pic12f1822_008007 },
	{ 0x008008, 0xffff,  6, pic16f1933_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f1946_addrs[] =
{
	{ 0x008007, 0xffff, 10, pic12f1822_008007 },
	{ 0x008008, 0xffff,  6, pic16f1946_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f505_addrs[] =
{
	{ 0x000fff, 0xffff,  4, pic16f505_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic16f506_addrs[] =
{
	{ 0x000fff, 0xffff,  5, pic16f506_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic16f526_addrs[] =
{
	{ 0x000fff, 0xff,  6, pic16f526_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic16f527_addrs[] =
{
	{ 0x000fff, 0x3ff,  8, pic16f527_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic16f570_addrs[] =
{
	{ 0x000fff, 0x3ff,  7, pic16f570_000fff },
};

static const struct gp_cfg_addr gp_cfg_pic16f627_addrs[] =
{
	{ 0x002007, 0xffff,  8, pic16f627_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f627a_addrs[] =
{
	{ 0x002007, 0xffff,  8, pic16f627a_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f630_addrs[] =
{
	{ 0x002007, 0xffff,  7, pic12f629_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f631_addrs[] =
{
	{ 0x002007, 0xffff,  9, pic16f631_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f707_addrs[] =
{
	{ 0x002007, 0xffff,  8, pic16f707_002007 },
	{ 0x002008, 0xffff,  1, pic16f707_002008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f716_addrs[] =
{
	{ 0x002007, 0xffff,  6, pic16f716_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f720_addrs[] =
{
	{ 0x002007, 0xffff,  7, pic16f720_002007 },
	{ 0x002008, 0xffff,  1, pic16f720_002008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f722_addrs[] =
{
	{ 0x002007, 0xffff,  9, pic16f722_002007 },
	{ 0x002008, 0xffff,  1, pic16f722_002008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f737_addrs[] =
{
	{ 0x002007, 0xffff,  9, pic16f737_002007 },
	{ 0x002008, 0xffff,  3, pic16f737_002008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f818_addrs[] =
{
	{ 0x002007, 0xffff, 11, pic16f818_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f819_addrs[] =
{
	{ 0x002007, 0xffff, 11, pic16f819_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f83_addrs[] =
{
	{ 0x002007, 0xffff,  4, pic16f83_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f870_addrs[] =
{
	{ 0x002007, 0xffff,  9, pic16f870_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f873_addrs[] =
{
	{ 0x002007, 0xffff,  9, pic16f873_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f873a_addrs[] =
{
	{ 0x002007, 0xffff,  9, pic16f873a_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16f87_addrs[] =
{
	{ 0x002007, 0xffff, 11, pic16f87_002007 },
	{ 0x002008, 0xffff,  2, pic16f87_002008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f882_addrs[] =
{
	{ 0x002007, 0xffff, 11, pic16f882_002007 },
	{ 0x002008, 0xffff,  2, pic16f882_002008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f883_addrs[] =
{
	{ 0x002007, 0xffff, 11, pic16f882_002007 },
	{ 0x002008, 0xffff,  2, pic16f883_002008 },
};

static const struct gp_cfg_addr gp_cfg_pic16f913_addrs[] =
{
	{ 0x002007, 0xffff, 10, pic16f913_002007 },
};

static const struct gp_cfg_addr gp_cfg_pic16lf1782_addrs[] =
{
	{ 0x008007, 0xffff, 10, pic12f1822_008007 },
	{ 0x008008, 0xffff,  6, pic16lf1782_008008 },
};

static const struct gp_cfg_addr gp_cfg_pic16lf707_addrs[] =
{
	{ 0x002007, 0xffff,  8, pic16f707_002007 },
	{ 0x002008, 0xffff,  0, NULL },
};

static const struct gp_cfg_addr gp_cfg_pic16lf722_addrs[] =
{
	{ 0x002007, 0xffff,  9, pic16f722_002007 },
	{ 0x002008, 0xffff,  0, NULL },
};

static const struct gp_cfg_addr gp_cfg_pic18c242_addrs[] =
{
	{ 0x300000, 0xff,  1, pic18c242_300000 },
	{ 0x300001, 0xe7,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300005, 0x01,  1, pic18c242_300005 },
	{ 0x300006, 0x03,  1, pic18c242_300006 },
};

static const struct gp_cfg_addr gp_cfg_pic18c452_addrs[] =
{
	{ 0x300000, 0xff,  1, pic18c242_300000 },
	{ 0x300001, 0xe7,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300005, 0x01,  1, pic18c452_300005 },
	{ 0x300006, 0x03,  1, pic18c242_300006 },
};

static const struct gp_cfg_addr gp_cfg_pic18c601_addrs[] =
{
	{ 0x300001, 0x03,  1, pic18c601_300001 },
	{ 0x300002, 0x41,  2, pic18c601_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300006, 0x81,  1, pic18c242_300006 },
};

static const struct gp_cfg_addr gp_cfg_pic18c658_addrs[] =
{
	{ 0x300000, 0xff,  1, pic18c242_300000 },
	{ 0x300001, 0xe7,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300006, 0x03,  1, pic18c242_300006 },
};

static const struct gp_cfg_addr gp_cfg_pic18f1220_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f1220_300001 },
	{ 0x300002, 0x0f,  3, pic18f1220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x80,  1, pic18f1220_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f1230_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f1230_300001 },
	{ 0x300002, 0x1f,  3, pic18f1230_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0x0e,  3, pic18f1230_300004 },
	{ 0x300005, 0x81,  3, pic18f1230_300005 },
	{ 0x300006, 0x81,  4, pic18f1230_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f1330_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f1230_300001 },
	{ 0x300002, 0x1f,  3, pic18f1230_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0x0e,  3, pic18f1230_300004 },
	{ 0x300005, 0x81,  3, pic18f1230_300005 },
	{ 0x300006, 0x81,  4, pic18f1330_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f13k22_addrs[] =
{
	{ 0x300001, 0x27,  5, pic18f13k22_300001 },
	{ 0x300002, 0x1f,  3, pic18f13k22_300002 },
	{ 0x300003, 0x1f,  2, pic18f13k22_300003 },
	{ 0x300005, 0x88,  2, pic18f13k22_300005 },
	{ 0x300006, 0x85,  5, pic18f13k22_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f13k50_addrs[] =
{
	{ 0x300000, 0x00,  2, pic18f13k50_300000 },
	{ 0x300001, 0x27,  5, pic18f13k22_300001 },
	{ 0x300002, 0x3f,  3, pic18f13k22_300002 },
	{ 0x300003, 0x1f,  2, pic18f13k22_300003 },
	{ 0x300005, 0x88,  2, pic18f13k22_300005 },
	{ 0x300006, 0x85,  5, pic18f13k22_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f14k50_addrs[] =
{
	{ 0x300000, 0x00,  2, pic18f13k50_300000 },
	{ 0x300001, 0x27,  5, pic18f13k22_300001 },
	{ 0x300002, 0x1f,  3, pic18f13k22_300002 },
	{ 0x300003, 0x1f,  2, pic18f13k22_300003 },
	{ 0x300005, 0x88,  2, pic18f13k22_300005 },
	{ 0x300006, 0x85,  5, pic18f13k22_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2220_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f1220_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  3, pic18f2220_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2221_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f1230_300001 },
	{ 0x300002, 0x1f,  3, pic18f2221_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2221_300005 },
	{ 0x300006, 0x85,  5, pic18f2221_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2320_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f1220_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  3, pic18f2220_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2321_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f1230_300001 },
	{ 0x300002, 0x1f,  3, pic18f2221_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2221_300005 },
	{ 0x300006, 0x85,  5, pic18f2321_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2331_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f2331_300001 },
	{ 0x300002, 0x0f,  3, pic18f2331_300002 },
	{ 0x300003, 0x3f,  3, pic18f2331_300003 },
	{ 0x300004, 0x3c,  4, pic18f2331_300004 },
	{ 0x300005, 0x9d,  1, pic18f1220_300005 },
	{ 0x300006, 0x85,  3, pic18f2331_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f23k20_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f23k20_300001 },
	{ 0x300002, 0x1f,  3, pic18f23k20_300002 },
	{ 0x300003, 0x1f,  2, pic18f13k22_300003 },
	{ 0x300005, 0x8b,  5, pic18f23k20_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f23k22_addrs[] =
{
	{ 0x300001, 0x25,  5, pic18f23k22_300001 },
	{ 0x300002, 0x1f,  3, pic18f23k22_300002 },
	{ 0x300003, 0x3f,  2, pic18f23k22_300003 },
	{ 0x300005, 0xbf,  7, pic18f23k22_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2410_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2410_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0x40,  1, pic18f2410_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0x60,  2, pic18f2410_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2420_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2420_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f242_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300005, 0x01,  1, pic18c242_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2431_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f2331_300001 },
	{ 0x300002, 0x0f,  3, pic18f2331_300002 },
	{ 0x300003, 0x3f,  3, pic18f2331_300003 },
	{ 0x300004, 0x3c,  4, pic18f2331_300004 },
	{ 0x300005, 0x9d,  1, pic18f1220_300005 },
	{ 0x300006, 0x85,  3, pic18f2331_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2439_addrs[] =
{
	{ 0x300001, 0x22,  1, pic18f2439_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2450_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f2450_300000 },
	{ 0x300001, 0x05,  3, pic18f2450_300001 },
	{ 0x300002, 0x1f,  4, pic18f2450_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x82,  3, pic18f2450_300005 },
	{ 0x300006, 0x85,  5, pic18f2450_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0x40,  1, pic18f2410_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0x60,  2, pic18f2410_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2455_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f2450_300000 },
	{ 0x300001, 0x05,  3, pic18f2450_300001 },
	{ 0x300002, 0x1f,  4, pic18f2455_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2455_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2480_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2480_300001 },
	{ 0x300002, 0x1f,  3, pic18f2480_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x82,  3, pic18f2450_300005 },
	{ 0x300006, 0x85,  5, pic18f2480_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f248_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f24j10_addrs[] =
{
	{ 0x003ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x003ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x003ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x003ffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f24j11_addrs[] =
{
	{ 0x003ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x003ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x003ffa, 0xdf,  5, pic18f24j11_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x003ffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x003ffd, 0xf9,  2, pic18f24j11_003ffd },
	{ 0x003ffe, 0xcf,  3, pic18f24j11_003ffe },
	{ 0x003fff, 0xf1,  1, pic18f24j11_003fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f24j50_addrs[] =
{
	{ 0x003ff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x003ff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x003ffa, 0xdf,  5, pic18f24j11_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x003ffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x003ffd, 0xf9,  2, pic18f24j11_003ffd },
	{ 0x003ffe, 0xcf,  3, pic18f24j11_003ffe },
	{ 0x003fff, 0xf1,  1, pic18f24j11_003fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f24k50_addrs[] =
{
	{ 0x300000, 0x00,  4, pic18f24k50_300000 },
	{ 0x300001, 0x25,  4, pic18f24k50_300001 },
	{ 0x300002, 0x5f,  4, pic18f24k50_300002 },
	{ 0x300003, 0x3f,  2, pic18f23k22_300003 },
	{ 0x300005, 0xd3,  5, pic18f24k50_300005 },
	{ 0x300006, 0x85,  5, pic18f24k50_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2510_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2410_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0x40,  1, pic18f2410_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0x60,  2, pic18f2410_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2515_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2410_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  3, pic18f2455_300008 },
	{ 0x300009, 0x40,  1, pic18f2410_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2455_30000a },
	{ 0x30000b, 0x60,  2, pic18f2410_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2520_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2410_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2523_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2420_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2525_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2410_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f252_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300005, 0x01,  1, pic18c242_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2539_addrs[] =
{
	{ 0x300001, 0x22,  1, pic18f2439_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2550_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f2450_300000 },
	{ 0x300001, 0x05,  3, pic18f2450_300001 },
	{ 0x300002, 0x1f,  4, pic18f2455_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2455_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2580_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2480_300001 },
	{ 0x300002, 0x1f,  3, pic18f2480_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x82,  3, pic18f2450_300005 },
	{ 0x300006, 0x85,  5, pic18f2480_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2585_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2480_300001 },
	{ 0x300002, 0x1f,  3, pic18f2480_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x82,  3, pic18f2450_300005 },
	{ 0x300006, 0x85,  5, pic18f2585_300006 },
	{ 0x300008, 0x0f,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f258_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f25j10_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f25j11_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x007ffa, 0xdf,  5, pic18f24j11_003ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x007ffd, 0xf9,  2, pic18f24j11_003ffd },
	{ 0x007ffe, 0xdf,  3, pic18f25j11_007ffe },
	{ 0x007fff, 0xf1,  1, pic18f24j11_003fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f25j50_addrs[] =
{
	{ 0x007ff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x007ff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x007ffa, 0xdf,  5, pic18f24j11_003ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x007ffd, 0xf9,  2, pic18f24j11_003ffd },
	{ 0x007ffe, 0xdf,  3, pic18f25j11_007ffe },
	{ 0x007fff, 0xf1,  1, pic18f24j11_003fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f25k20_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f23k20_300001 },
	{ 0x300002, 0x1f,  3, pic18f23k20_300002 },
	{ 0x300003, 0x1f,  2, pic18f13k22_300003 },
	{ 0x300005, 0x8b,  5, pic18f23k20_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f25k22_addrs[] =
{
	{ 0x300001, 0x25,  5, pic18f23k22_300001 },
	{ 0x300002, 0x1f,  3, pic18f23k22_300002 },
	{ 0x300003, 0x3f,  2, pic18f23k22_300003 },
	{ 0x300005, 0xbf,  7, pic18f23k22_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f25k50_addrs[] =
{
	{ 0x300000, 0x00,  4, pic18f24k50_300000 },
	{ 0x300001, 0x25,  4, pic18f24k50_300001 },
	{ 0x300002, 0x5f,  4, pic18f24k50_300002 },
	{ 0x300003, 0x3f,  2, pic18f23k22_300003 },
	{ 0x300005, 0xd3,  5, pic18f24k50_300005 },
	{ 0x300006, 0x85,  5, pic18f24k50_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f25k80_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f25k80_300000 },
	{ 0x300001, 0x08,  4, pic18f25k80_300001 },
	{ 0x300002, 0x7f,  4, pic18f25k80_300002 },
	{ 0x300003, 0x7f,  2, pic18f25k80_300003 },
	{ 0x300005, 0x89,  3, pic18f25k80_300005 },
	{ 0x300006, 0x91,  2, pic18f25k80_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2680_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2480_300001 },
	{ 0x300002, 0x1f,  3, pic18f2480_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x82,  3, pic18f2450_300005 },
	{ 0x300006, 0x85,  5, pic18f2585_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2682_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2480_300001 },
	{ 0x300002, 0x1f,  3, pic18f2480_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x82,  3, pic18f2450_300005 },
	{ 0x300006, 0x85,  5, pic18f2585_300006 },
	{ 0x300008, 0x3f,  5, pic18f2682_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x3f,  5, pic18f2682_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x3f,  5, pic18f2682_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f2685_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2480_300001 },
	{ 0x300002, 0x1f,  3, pic18f2480_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x82,  3, pic18f2450_300005 },
	{ 0x300006, 0x85,  5, pic18f2585_300006 },
	{ 0x300008, 0x3f,  6, pic18f2685_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x3f,  6, pic18f2685_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x3f,  6, pic18f2685_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f26j11_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xdf,  5, pic18f24j11_003ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x00fffd, 0xf9,  2, pic18f24j11_003ffd },
	{ 0x00fffe, 0xff,  3, pic18f26j11_00fffe },
	{ 0x00ffff, 0xf1,  1, pic18f24j11_003fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f26j13_addrs[] =
{
	{ 0x00fff8, 0xff,  5, pic18f26j13_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xff,  5, pic18f26j13_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x00fffd, 0xff,  4, pic18f26j13_00fffd },
	{ 0x00fffe, 0xbf,  2, pic18f26j13_00fffe },
	{ 0x00ffff, 0xf3,  2, pic18f26j13_00ffff },
};

static const struct gp_cfg_addr gp_cfg_pic18f26j50_addrs[] =
{
	{ 0x00fff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x00fff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x00fffa, 0xdf,  5, pic18f24j11_003ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x00fffd, 0xf9,  2, pic18f24j11_003ffd },
	{ 0x00fffe, 0xff,  3, pic18f26j11_00fffe },
	{ 0x00ffff, 0xf1,  1, pic18f24j11_003fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f26j53_addrs[] =
{
	{ 0x00fff8, 0xff,  5, pic18f26j13_00fff8 },
	{ 0x00fff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x00fffa, 0xff,  5, pic18f26j13_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x00fffd, 0xfb,  3, pic18f26j53_00fffd },
	{ 0x00fffe, 0xbf,  2, pic18f26j13_00fffe },
	{ 0x00ffff, 0xfb,  3, pic18f26j53_00ffff },
};

static const struct gp_cfg_addr gp_cfg_pic18f27j13_addrs[] =
{
	{ 0x01fff8, 0xff,  5, pic18f26j13_00fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xff,  5, pic18f26j13_00fffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x01fffd, 0xff,  4, pic18f26j13_00fffd },
	{ 0x01fffe, 0xff,  2, pic18f27j13_01fffe },
	{ 0x01ffff, 0xf3,  2, pic18f26j13_00ffff },
};

static const struct gp_cfg_addr gp_cfg_pic18f27j53_addrs[] =
{
	{ 0x01fff8, 0xff,  5, pic18f26j13_00fff8 },
	{ 0x01fff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x01fffa, 0xff,  5, pic18f26j13_00fffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x01fffd, 0xfb,  3, pic18f26j53_00fffd },
	{ 0x01fffe, 0xff,  2, pic18f27j13_01fffe },
	{ 0x01ffff, 0xfb,  3, pic18f26j53_00ffff },
};

static const struct gp_cfg_addr gp_cfg_pic18f4331_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f2331_300001 },
	{ 0x300002, 0x0f,  3, pic18f2331_300002 },
	{ 0x300003, 0x3f,  3, pic18f2331_300003 },
	{ 0x300004, 0x3c,  4, pic18f2331_300004 },
	{ 0x300005, 0x9d,  5, pic18f4331_300005 },
	{ 0x300006, 0x85,  3, pic18f2331_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f43k22_addrs[] =
{
	{ 0x300001, 0x25,  5, pic18f23k22_300001 },
	{ 0x300002, 0x1f,  3, pic18f23k22_300002 },
	{ 0x300003, 0x3f,  2, pic18f23k22_300003 },
	{ 0x300005, 0xbf,  7, pic18f43k22_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f4420_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2410_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  2, pic18f1220_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  2, pic18f1220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f4431_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f2331_300001 },
	{ 0x300002, 0x0f,  3, pic18f2331_300002 },
	{ 0x300003, 0x3f,  3, pic18f2331_300003 },
	{ 0x300004, 0x3c,  4, pic18f2331_300004 },
	{ 0x300005, 0x9d,  5, pic18f4331_300005 },
	{ 0x300006, 0x85,  3, pic18f2331_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f4450_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f2450_300000 },
	{ 0x300001, 0x05,  3, pic18f2450_300001 },
	{ 0x300002, 0x1f,  4, pic18f2450_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x82,  3, pic18f2450_300005 },
	{ 0x300006, 0x85,  6, pic18f4450_300006 },
	{ 0x300008, 0x03,  2, pic18f1220_300008 },
	{ 0x300009, 0x40,  1, pic18f2410_300009 },
	{ 0x30000a, 0x03,  2, pic18f1220_30000a },
	{ 0x30000b, 0x60,  2, pic18f2410_30000b },
	{ 0x30000c, 0x03,  2, pic18f1220_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f4455_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f2450_300000 },
	{ 0x300001, 0x05,  3, pic18f2450_300001 },
	{ 0x300002, 0x1f,  4, pic18f2455_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2455_300005 },
	{ 0x300006, 0x85,  5, pic18f4455_300006 },
	{ 0x300008, 0x0f,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f4550_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f2450_300000 },
	{ 0x300001, 0x05,  3, pic18f2450_300001 },
	{ 0x300002, 0x1f,  4, pic18f2455_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  4, pic18f2455_300005 },
	{ 0x300006, 0x85,  5, pic18f4455_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f45k22_addrs[] =
{
	{ 0x300001, 0x25,  5, pic18f23k22_300001 },
	{ 0x300002, 0x1f,  3, pic18f23k22_300002 },
	{ 0x300003, 0x3f,  2, pic18f23k22_300003 },
	{ 0x300005, 0xbf,  7, pic18f43k22_300005 },
	{ 0x300006, 0x85,  4, pic18f23k20_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f45k50_addrs[] =
{
	{ 0x300000, 0x00,  4, pic18f24k50_300000 },
	{ 0x300001, 0x25,  4, pic18f24k50_300001 },
	{ 0x300002, 0x5f,  4, pic18f24k50_300002 },
	{ 0x300003, 0x3f,  2, pic18f23k22_300003 },
	{ 0x300005, 0xd3,  5, pic18f24k50_300005 },
	{ 0x300006, 0x85,  5, pic18f4455_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f6310_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f6310_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0xc3,  0, NULL },
	{ 0x300005, 0x81,  3, pic18f6310_300005 },
	{ 0x300006, 0x81,  3, pic18f6310_300006 },
	{ 0x300008, 0x01,  1, pic18f6310_300008 },
	{ 0x30000c, 0x01,  0, NULL },
};

static const struct gp_cfg_addr gp_cfg_pic18f6390_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f6310_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x81,  3, pic18f6390_300005 },
	{ 0x300006, 0x81,  3, pic18f6310_300006 },
	{ 0x300008, 0x01,  1, pic18f6310_300008 },
};

static const struct gp_cfg_addr gp_cfg_pic18f63j11_addrs[] =
{
	{ 0x001ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x001ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x001ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x001ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x001ffc, 0xf8,  0, NULL },
	{ 0x001ffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f64j11_addrs[] =
{
	{ 0x003ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x003ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x003ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x003ffc, 0xf8,  0, NULL },
	{ 0x003ffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f6520_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300004, 0x83,  0, NULL },
	{ 0x300005, 0x03,  1, pic18c242_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0xff,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f6525_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f6525_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0x83,  0, NULL },
	{ 0x300005, 0x83,  2, pic18f6525_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f6527_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x81,  3, pic18f6310_300005 },
	{ 0x300006, 0x85,  5, pic18f6527_300006 },
	{ 0x300008, 0xff,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f6585_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f6525_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0x83,  0, NULL },
	{ 0x300005, 0x83,  2, pic18f6585_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f65j10_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffc, 0xf8,  0, NULL },
	{ 0x007ffd, 0xf3,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f65j11_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffc, 0xf8,  0, NULL },
	{ 0x007ffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f65j15_addrs[] =
{
	{ 0x00bff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x00bff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00bffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x00bffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00bffc, 0xf8,  0, NULL },
	{ 0x00bffd, 0xf3,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f65j50_addrs[] =
{
	{ 0x007ff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x007ff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x007ffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffc, 0xf8,  0, NULL },
	{ 0x007ffd, 0xf9,  2, pic18f65j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f65j90_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x007ffa, 0xff,  4, pic18f24j10_003ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffc, 0xf8,  0, NULL },
	{ 0x007ffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f65j94_addrs[] =
{
	{ 0x007ff0, 0xe0,  2, pic18f65j94_007ff0 },
	{ 0x007ff1, 0xf7,  3, pic18f65j94_007ff1 },
	{ 0x007ff2, 0xaf,  4, pic18f65j94_007ff2 },
	{ 0x007ff3, 0xff,  1, pic18f65j94_007ff3 },
	{ 0x007ff4, 0x33,  2, pic18f65j94_007ff4 },
	{ 0x007ff5, 0xf0,  0, NULL },
	{ 0x007ff6, 0xff,  1, pic18f65j94_007ff6 },
	{ 0x007ff7, 0xf7,  3, pic18f65j94_007ff7 },
	{ 0x007ff8, 0x03,  2, pic18f65j94_007ff8 },
	{ 0x007ff9, 0xff,  4, pic18f65j94_007ff9 },
	{ 0x007ffa, 0xff,  3, pic18f65j94_007ffa },
	{ 0x007ffb, 0xff,  3, pic18f65j94_007ffb },
	{ 0x007ffc, 0x1d,  4, pic18f65j94_007ffc },
	{ 0x007ffd, 0xf0,  0, NULL },
	{ 0x007ffe, 0xf8,  1, pic18f65j94_007ffe },
	{ 0x007fff, 0xf3,  2, pic18f65j94_007fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f65k22_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f65k22_300000 },
	{ 0x300001, 0x08,  4, pic18f25k80_300001 },
	{ 0x300002, 0x7f,  4, pic18f25k80_300002 },
	{ 0x300003, 0x7f,  2, pic18f25k80_300003 },
	{ 0x300004, 0x01,  1, pic18f65k22_300004 },
	{ 0x300005, 0x89,  3, pic18f65k22_300005 },
	{ 0x300006, 0x91,  3, pic18f65k22_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f65k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f65k22_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f65k80_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f25k80_300000 },
	{ 0x300001, 0x08,  4, pic18f25k80_300001 },
	{ 0x300002, 0x7f,  4, pic18f25k80_300002 },
	{ 0x300003, 0x7f,  2, pic18f25k80_300003 },
	{ 0x300005, 0x8f,  5, pic18f65k80_300005 },
	{ 0x300006, 0x91,  2, pic18f25k80_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f6621_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f6525_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0x83,  0, NULL },
	{ 0x300005, 0x83,  2, pic18f6525_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f6622_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x81,  3, pic18f6310_300005 },
	{ 0x300006, 0x85,  5, pic18f6527_300006 },
	{ 0x300008, 0xff,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f6627_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  3, pic18f6310_300005 },
	{ 0x300006, 0x85,  5, pic18f6527_300006 },
	{ 0x300008, 0xff,  6, pic18f2685_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  6, pic18f2685_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  6, pic18f2685_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f6680_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f6525_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0x83,  0, NULL },
	{ 0x300005, 0x83,  2, pic18f6585_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j10_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf8,  0, NULL },
	{ 0x00fffd, 0xf3,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j11_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf8,  0, NULL },
	{ 0x00fffd, 0xff,  2, pic18f65j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j15_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x017ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x017ffc, 0xf8,  0, NULL },
	{ 0x017ffd, 0xf3,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j16_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x017ffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x017ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x017ffc, 0xf8,  0, NULL },
	{ 0x017ffd, 0xff,  2, pic18f65j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j50_addrs[] =
{
	{ 0x00fff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x00fff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x00fffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf8,  0, NULL },
	{ 0x00fffd, 0xf9,  2, pic18f65j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j55_addrs[] =
{
	{ 0x017ff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x017ff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x017ffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x017ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x017ffc, 0xf8,  0, NULL },
	{ 0x017ffd, 0xf9,  2, pic18f65j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j60_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf8,  0, NULL },
	{ 0x00fffd, 0xf7,  1, pic18f66j60_00fffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j65_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x017ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x017ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x017ffc, 0xf8,  0, NULL },
	{ 0x017ffd, 0xf7,  1, pic18f66j60_00fffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j90_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xdf,  5, pic18f66j90_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0x02,  1, pic18f66j90_00fffc },
	{ 0x00fffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j93_addrs[] =
{
	{ 0x00fff8, 0xe1,  3, pic18f66j93_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xdf,  5, pic18f66j90_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0x02,  1, pic18f66j90_00fffc },
	{ 0x00fffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j94_addrs[] =
{
	{ 0x00fff0, 0xe0,  2, pic18f65j94_007ff0 },
	{ 0x00fff1, 0xf7,  3, pic18f65j94_007ff1 },
	{ 0x00fff2, 0xaf,  4, pic18f65j94_007ff2 },
	{ 0x00fff3, 0xff,  1, pic18f65j94_007ff3 },
	{ 0x00fff4, 0x33,  2, pic18f65j94_007ff4 },
	{ 0x00fff5, 0xf0,  0, NULL },
	{ 0x00fff6, 0xff,  1, pic18f65j94_007ff6 },
	{ 0x00fff7, 0xf7,  3, pic18f65j94_007ff7 },
	{ 0x00fff8, 0x03,  2, pic18f65j94_007ff8 },
	{ 0x00fff9, 0xff,  4, pic18f65j94_007ff9 },
	{ 0x00fffa, 0xff,  3, pic18f65j94_007ffa },
	{ 0x00fffb, 0xff,  3, pic18f65j94_007ffb },
	{ 0x00fffc, 0x1d,  4, pic18f65j94_007ffc },
	{ 0x00fffd, 0xf0,  0, NULL },
	{ 0x00fffe, 0xf8,  1, pic18f65j94_007ffe },
	{ 0x00ffff, 0xf3,  2, pic18f65j94_007fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f66j99_addrs[] =
{
	{ 0x017ff0, 0xe0,  2, pic18f65j94_007ff0 },
	{ 0x017ff1, 0xf7,  3, pic18f65j94_007ff1 },
	{ 0x017ff2, 0xaf,  4, pic18f65j94_007ff2 },
	{ 0x017ff3, 0xff,  1, pic18f65j94_007ff3 },
	{ 0x017ff4, 0x33,  2, pic18f65j94_007ff4 },
	{ 0x017ff5, 0xf0,  0, NULL },
	{ 0x017ff6, 0xff,  1, pic18f65j94_007ff6 },
	{ 0x017ff7, 0xf7,  3, pic18f65j94_007ff7 },
	{ 0x017ff8, 0x03,  2, pic18f65j94_007ff8 },
	{ 0x017ff9, 0xff,  4, pic18f65j94_007ff9 },
	{ 0x017ffa, 0xff,  3, pic18f65j94_007ffa },
	{ 0x017ffb, 0xff,  3, pic18f65j94_007ffb },
	{ 0x017ffc, 0x1d,  4, pic18f65j94_007ffc },
	{ 0x017ffd, 0xf0,  0, NULL },
	{ 0x017ffe, 0xf8,  1, pic18f65j94_007ffe },
	{ 0x017fff, 0xf3,  2, pic18f65j94_007fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f6720_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300004, 0x83,  0, NULL },
	{ 0x300005, 0x03,  1, pic18c242_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0xff,  8, pic18f6720_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  8, pic18f6720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  8, pic18f6720_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f6722_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300005, 0x83,  3, pic18f6310_300005 },
	{ 0x300006, 0x85,  5, pic18f6527_300006 },
	{ 0x300008, 0xff,  8, pic18f6720_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  8, pic18f6720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  8, pic18f6720_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f67j10_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf8,  0, NULL },
	{ 0x01fffd, 0xf3,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f67j11_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf8,  0, NULL },
	{ 0x01fffd, 0xff,  2, pic18f65j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f67j50_addrs[] =
{
	{ 0x01fff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x01fff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x01fffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf8,  0, NULL },
	{ 0x01fffd, 0xf9,  2, pic18f65j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f67j60_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf8,  0, NULL },
	{ 0x01fffd, 0xf7,  1, pic18f66j60_00fffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f67j90_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xdf,  5, pic18f66j90_00fffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0x02,  1, pic18f66j90_00fffc },
	{ 0x01fffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f67j93_addrs[] =
{
	{ 0x01fff8, 0xe1,  3, pic18f66j93_00fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xdf,  5, pic18f66j90_00fffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0x02,  1, pic18f66j90_00fffc },
	{ 0x01fffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f67j94_addrs[] =
{
	{ 0x01fff0, 0xe0,  2, pic18f65j94_007ff0 },
	{ 0x01fff1, 0xf7,  3, pic18f65j94_007ff1 },
	{ 0x01fff2, 0xaf,  4, pic18f65j94_007ff2 },
	{ 0x01fff3, 0xff,  1, pic18f65j94_007ff3 },
	{ 0x01fff4, 0x33,  2, pic18f65j94_007ff4 },
	{ 0x01fff5, 0xf0,  0, NULL },
	{ 0x01fff6, 0xff,  1, pic18f65j94_007ff6 },
	{ 0x01fff7, 0xf7,  3, pic18f65j94_007ff7 },
	{ 0x01fff8, 0x03,  2, pic18f65j94_007ff8 },
	{ 0x01fff9, 0xff,  4, pic18f65j94_007ff9 },
	{ 0x01fffa, 0xff,  3, pic18f65j94_007ffa },
	{ 0x01fffb, 0xff,  3, pic18f65j94_007ffb },
	{ 0x01fffc, 0x1d,  4, pic18f65j94_007ffc },
	{ 0x01fffd, 0xf0,  0, NULL },
	{ 0x01fffe, 0xf8,  1, pic18f65j94_007ffe },
	{ 0x01ffff, 0xf3,  2, pic18f65j94_007fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f67k22_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f65k22_300000 },
	{ 0x300001, 0x08,  4, pic18f25k80_300001 },
	{ 0x300002, 0x7f,  4, pic18f25k80_300002 },
	{ 0x300003, 0x7f,  2, pic18f25k80_300003 },
	{ 0x300004, 0x01,  1, pic18f65k22_300004 },
	{ 0x300005, 0x89,  3, pic18f65k22_300005 },
	{ 0x300006, 0x91,  3, pic18f65k22_300006 },
	{ 0x300008, 0xff,  8, pic18f6720_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  8, pic18f6720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  8, pic18f67k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f65k22_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f8310_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f6310_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0xc3,  3, pic18f8310_300004 },
	{ 0x300005, 0x81,  3, pic18f6390_300005 },
	{ 0x300006, 0x81,  3, pic18f6310_300006 },
	{ 0x300008, 0x01,  1, pic18f6310_300008 },
	{ 0x30000c, 0x01,  1, pic18f8310_30000c },
};

static const struct gp_cfg_addr gp_cfg_pic18f83j11_addrs[] =
{
	{ 0x001ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x001ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x001ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x001ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x001ffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x001ffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f84j11_addrs[] =
{
	{ 0x003ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x003ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x003ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x003ffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x003ffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f8520_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300004, 0x83,  2, pic18f8520_300004 },
	{ 0x300005, 0x03,  1, pic18c242_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0xff,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f8525_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f6525_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0x83,  2, pic18f8520_300004 },
	{ 0x300005, 0x83,  3, pic18f8525_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f8527_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0xf3,  4, pic18f8527_300004 },
	{ 0x300005, 0x83,  4, pic18f8527_300005 },
	{ 0x300006, 0x85,  5, pic18f6527_300006 },
	{ 0x300008, 0xff,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f8585_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f6525_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0x83,  2, pic18f8520_300004 },
	{ 0x300005, 0x83,  3, pic18f8585_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  3, pic18f2455_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2455_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2455_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f85j10_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x007ffd, 0xf3,  2, pic18f85j10_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f85j11_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x007ffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f85j15_addrs[] =
{
	{ 0x00bff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x00bff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00bffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x00bffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00bffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x00bffd, 0xf3,  2, pic18f85j10_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f85j50_addrs[] =
{
	{ 0x007ff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x007ff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x007ffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x007ffd, 0xff,  4, pic18f85j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f85j94_addrs[] =
{
	{ 0x007ff0, 0xe0,  2, pic18f65j94_007ff0 },
	{ 0x007ff1, 0xf7,  3, pic18f65j94_007ff1 },
	{ 0x007ff2, 0xaf,  4, pic18f65j94_007ff2 },
	{ 0x007ff3, 0xff,  1, pic18f65j94_007ff3 },
	{ 0x007ff4, 0x33,  2, pic18f65j94_007ff4 },
	{ 0x007ff5, 0xf0,  0, NULL },
	{ 0x007ff6, 0xff,  1, pic18f65j94_007ff6 },
	{ 0x007ff7, 0xf7,  3, pic18f65j94_007ff7 },
	{ 0x007ff8, 0xfb,  6, pic18f85j94_007ff8 },
	{ 0x007ff9, 0xff,  4, pic18f65j94_007ff9 },
	{ 0x007ffa, 0xff,  3, pic18f65j94_007ffa },
	{ 0x007ffb, 0xff,  3, pic18f65j94_007ffb },
	{ 0x007ffc, 0x1d,  4, pic18f65j94_007ffc },
	{ 0x007ffd, 0xf0,  0, NULL },
	{ 0x007ffe, 0xf8,  1, pic18f65j94_007ffe },
	{ 0x007fff, 0xf3,  2, pic18f65j94_007fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f85k22_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f65k22_300000 },
	{ 0x300001, 0x08,  4, pic18f25k80_300001 },
	{ 0x300002, 0x7f,  4, pic18f25k80_300002 },
	{ 0x300003, 0x7f,  2, pic18f25k80_300003 },
	{ 0x300004, 0xf9,  5, pic18f85k22_300004 },
	{ 0x300005, 0x8b,  4, pic18f85k22_300005 },
	{ 0x300006, 0x91,  3, pic18f65k22_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f65k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f65k22_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f85k90_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f65k22_300000 },
	{ 0x300001, 0x08,  4, pic18f25k80_300001 },
	{ 0x300002, 0x7f,  4, pic18f25k80_300002 },
	{ 0x300003, 0x7f,  2, pic18f25k80_300003 },
	{ 0x300004, 0x01,  1, pic18f65k22_300004 },
	{ 0x300005, 0x8b,  4, pic18f85k22_300005 },
	{ 0x300006, 0x91,  3, pic18f65k22_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f65k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f65k22_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f8621_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f6525_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0x83,  2, pic18f8520_300004 },
	{ 0x300005, 0x83,  3, pic18f8525_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f8622_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0xf3,  4, pic18f8527_300004 },
	{ 0x300005, 0x83,  4, pic18f8527_300005 },
	{ 0x300006, 0x85,  5, pic18f6527_300006 },
	{ 0x300008, 0xff,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f8627_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0xf3,  4, pic18f8527_300004 },
	{ 0x300005, 0x83,  4, pic18f8527_300005 },
	{ 0x300006, 0x85,  5, pic18f6527_300006 },
	{ 0x300008, 0xff,  6, pic18f2685_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  6, pic18f2685_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  6, pic18f2685_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f8680_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f6525_300001 },
	{ 0x300002, 0x0f,  3, pic18f2220_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0x83,  2, pic18f8520_300004 },
	{ 0x300005, 0x83,  3, pic18f8585_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0x0f,  4, pic18f2320_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2320_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2320_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j10_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x00fffd, 0xf3,  2, pic18f85j10_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j11_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x00fffd, 0xff,  4, pic18f85j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j15_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x017ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x017ffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x017ffd, 0xf3,  2, pic18f85j10_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j16_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x017ffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x017ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x017ffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x017ffd, 0xff,  4, pic18f85j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j50_addrs[] =
{
	{ 0x00fff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x00fff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x00fffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x00fffd, 0xff,  4, pic18f85j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j55_addrs[] =
{
	{ 0x017ff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x017ff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x017ffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x017ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x017ffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x017ffd, 0xff,  4, pic18f85j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j60_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf8,  0, NULL },
	{ 0x00fffd, 0xf7,  3, pic18f86j60_00fffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j65_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x017ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x017ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x017ffc, 0xf8,  0, NULL },
	{ 0x017ffd, 0xf7,  3, pic18f86j60_00fffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j90_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xdf,  5, pic18f66j90_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf2,  1, pic18f66j90_00fffc },
	{ 0x00fffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j93_addrs[] =
{
	{ 0x00fff8, 0xe1,  3, pic18f66j93_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xdf,  5, pic18f66j90_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf2,  1, pic18f66j90_00fffc },
	{ 0x00fffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j94_addrs[] =
{
	{ 0x00fff0, 0xe0,  2, pic18f65j94_007ff0 },
	{ 0x00fff1, 0xf7,  3, pic18f65j94_007ff1 },
	{ 0x00fff2, 0xaf,  4, pic18f65j94_007ff2 },
	{ 0x00fff3, 0xff,  1, pic18f65j94_007ff3 },
	{ 0x00fff4, 0x33,  2, pic18f65j94_007ff4 },
	{ 0x00fff5, 0xf0,  0, NULL },
	{ 0x00fff6, 0xff,  1, pic18f65j94_007ff6 },
	{ 0x00fff7, 0xf7,  3, pic18f65j94_007ff7 },
	{ 0x00fff8, 0xfb,  6, pic18f85j94_007ff8 },
	{ 0x00fff9, 0xff,  4, pic18f65j94_007ff9 },
	{ 0x00fffa, 0xff,  3, pic18f65j94_007ffa },
	{ 0x00fffb, 0xff,  3, pic18f65j94_007ffb },
	{ 0x00fffc, 0x1d,  4, pic18f65j94_007ffc },
	{ 0x00fffd, 0xf0,  0, NULL },
	{ 0x00fffe, 0xf8,  1, pic18f65j94_007ffe },
	{ 0x00ffff, 0xf3,  2, pic18f65j94_007fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f86j99_addrs[] =
{
	{ 0x017ff0, 0xe0,  2, pic18f65j94_007ff0 },
	{ 0x017ff1, 0xf7,  3, pic18f65j94_007ff1 },
	{ 0x017ff2, 0xaf,  4, pic18f65j94_007ff2 },
	{ 0x017ff3, 0xff,  1, pic18f65j94_007ff3 },
	{ 0x017ff4, 0x33,  2, pic18f65j94_007ff4 },
	{ 0x017ff5, 0xf0,  0, NULL },
	{ 0x017ff6, 0xff,  1, pic18f65j94_007ff6 },
	{ 0x017ff7, 0xf7,  3, pic18f65j94_007ff7 },
	{ 0x017ff8, 0xfb,  6, pic18f85j94_007ff8 },
	{ 0x017ff9, 0xff,  4, pic18f65j94_007ff9 },
	{ 0x017ffa, 0xff,  3, pic18f65j94_007ffa },
	{ 0x017ffb, 0xff,  3, pic18f65j94_007ffb },
	{ 0x017ffc, 0x1d,  4, pic18f65j94_007ffc },
	{ 0x017ffd, 0xf0,  0, NULL },
	{ 0x017ffe, 0xf8,  1, pic18f65j94_007ffe },
	{ 0x017fff, 0xf3,  2, pic18f65j94_007fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f8720_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c242_300001 },
	{ 0x300002, 0x0f,  3, pic18c242_300002 },
	{ 0x300003, 0x0f,  2, pic18c242_300003 },
	{ 0x300004, 0x83,  2, pic18f8520_300004 },
	{ 0x300005, 0x03,  1, pic18c242_300005 },
	{ 0x300006, 0x85,  3, pic18f1220_300006 },
	{ 0x300008, 0xff,  8, pic18f6720_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  8, pic18f6720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  8, pic18f6720_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f8722_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2410_300001 },
	{ 0x300002, 0x1f,  3, pic18f2410_300002 },
	{ 0x300003, 0x1f,  2, pic18f1220_300003 },
	{ 0x300004, 0xf3,  4, pic18f8527_300004 },
	{ 0x300005, 0x83,  4, pic18f8527_300005 },
	{ 0x300006, 0x85,  5, pic18f6527_300006 },
	{ 0x300008, 0xff,  8, pic18f6720_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  8, pic18f6720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  8, pic18f6720_30000c },
	{ 0x30000d, 0x40,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f87j10_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x01fffd, 0xf3,  2, pic18f85j10_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f87j11_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x01fffd, 0xff,  4, pic18f85j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f87j50_addrs[] =
{
	{ 0x01fff8, 0xef,  5, pic18f24j50_003ff8 },
	{ 0x01fff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x01fffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x01fffd, 0xff,  4, pic18f85j50_007ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f87j60_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf8,  0, NULL },
	{ 0x01fffd, 0xf7,  3, pic18f86j60_00fffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f87j90_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xdf,  5, pic18f66j90_00fffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf2,  1, pic18f66j90_00fffc },
	{ 0x01fffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f87j93_addrs[] =
{
	{ 0x01fff8, 0xe1,  3, pic18f66j93_00fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xdf,  5, pic18f66j90_00fffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf2,  1, pic18f66j90_00fffc },
	{ 0x01fffd, 0xf1,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f87j94_addrs[] =
{
	{ 0x01fff0, 0xe0,  2, pic18f65j94_007ff0 },
	{ 0x01fff1, 0xf7,  3, pic18f65j94_007ff1 },
	{ 0x01fff2, 0xaf,  4, pic18f65j94_007ff2 },
	{ 0x01fff3, 0xff,  1, pic18f65j94_007ff3 },
	{ 0x01fff4, 0x33,  2, pic18f65j94_007ff4 },
	{ 0x01fff5, 0xf0,  0, NULL },
	{ 0x01fff6, 0xff,  1, pic18f65j94_007ff6 },
	{ 0x01fff7, 0xf7,  3, pic18f65j94_007ff7 },
	{ 0x01fff8, 0xfb,  6, pic18f85j94_007ff8 },
	{ 0x01fff9, 0xff,  4, pic18f65j94_007ff9 },
	{ 0x01fffa, 0xff,  3, pic18f65j94_007ffa },
	{ 0x01fffb, 0xff,  3, pic18f65j94_007ffb },
	{ 0x01fffc, 0x1d,  4, pic18f65j94_007ffc },
	{ 0x01fffd, 0xf0,  0, NULL },
	{ 0x01fffe, 0xf8,  1, pic18f65j94_007ffe },
	{ 0x01ffff, 0xf3,  2, pic18f65j94_007fff },
};

static const struct gp_cfg_addr gp_cfg_pic18f87k22_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f65k22_300000 },
	{ 0x300001, 0x08,  4, pic18f25k80_300001 },
	{ 0x300002, 0x7f,  4, pic18f25k80_300002 },
	{ 0x300003, 0x7f,  2, pic18f25k80_300003 },
	{ 0x300004, 0xf9,  5, pic18f85k22_300004 },
	{ 0x300005, 0x8b,  4, pic18f85k22_300005 },
	{ 0x300006, 0x91,  3, pic18f65k22_300006 },
	{ 0x300008, 0xff,  8, pic18f6720_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  8, pic18f6720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  8, pic18f67k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f65k22_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f87k90_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f65k22_300000 },
	{ 0x300001, 0x08,  4, pic18f25k80_300001 },
	{ 0x300002, 0x7f,  4, pic18f25k80_300002 },
	{ 0x300003, 0x7f,  2, pic18f25k80_300003 },
	{ 0x300004, 0x01,  1, pic18f65k22_300004 },
	{ 0x300005, 0x8b,  4, pic18f85k22_300005 },
	{ 0x300006, 0x91,  3, pic18f65k22_300006 },
	{ 0x300008, 0xff,  8, pic18f6720_300008 },
	{ 0x300009, 0xc0,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  8, pic18f6720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  8, pic18f67k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f65k22_30000d },
};

static const struct gp_cfg_addr gp_cfg_pic18f96j60_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x00fffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x00fffd, 0xf7,  3, pic18f86j60_00fffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f96j65_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x017ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x017ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x017ffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x017ffd, 0xf7,  3, pic18f86j60_00fffd },
};

static const struct gp_cfg_addr gp_cfg_pic18f97j60_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x01fffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x01fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x01fffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x01fffd, 0xf7,  3, pic18f86j60_00fffd },
};

static const struct gp_cfg_addr gp_cfg_pic18lf24j10_addrs[] =
{
	{ 0x003ff8, 0xff,  4, pic18f24j10_003ff8 },
	{ 0x003ff9, 0xf7,  1, pic18f24j10_003ff9 },
	{ 0x003ffa, 0xff,  4, pic18f24j10_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x003ffd, 0xff,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18lf24j50_addrs[] =
{
	{ 0x003ff8, 0xef,  4, pic18lf24j50_003ff8 },
	{ 0x003ff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x003ffa, 0xdf,  5, pic18f24j11_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x003ffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x003ffd, 0xf9,  2, pic18f24j11_003ffd },
	{ 0x003ffe, 0xcf,  3, pic18f24j11_003ffe },
	{ 0x003fff, 0xf1,  1, pic18f24j11_003fff },
};

static const struct gp_cfg_addr gp_cfg_pic18lf25j10_addrs[] =
{
	{ 0x007ff8, 0xff,  4, pic18f24j10_003ff8 },
	{ 0x007ff9, 0xf7,  1, pic18f24j10_003ff9 },
	{ 0x007ffa, 0xff,  4, pic18f24j10_003ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffd, 0xff,  1, pic18f24j10_003ffd },
};

static const struct gp_cfg_addr gp_cfg_pic18lf25j50_addrs[] =
{
	{ 0x007ff8, 0xef,  4, pic18lf24j50_003ff8 },
	{ 0x007ff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x007ffa, 0xdf,  5, pic18f24j11_003ffa },
	{ 0x007ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x007ffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x007ffd, 0xf9,  2, pic18f24j11_003ffd },
	{ 0x007ffe, 0xdf,  3, pic18f25j11_007ffe },
	{ 0x007fff, 0xf1,  1, pic18f24j11_003fff },
};

static const struct gp_cfg_addr gp_cfg_pic18lf26j50_addrs[] =
{
	{ 0x00fff8, 0xef,  4, pic18lf24j50_003ff8 },
	{ 0x00fff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x00fffa, 0xdf,  5, pic18f24j11_003ffa },
	{ 0x00fffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x00fffc, 0xff,  5, pic18f24j11_003ffc },
	{ 0x00fffd, 0xf9,  2, pic18f24j11_003ffd },
	{ 0x00fffe, 0xff,  3, pic18f26j11_00fffe },
	{ 0x00ffff, 0xf1,  1, pic18f24j11_003fff },
};

static const struct gp_cfg_addr gp_cfg_ps500_addrs[] =
{
	{ 0x300003, 0xff,  1, ps500_300003 },
	{ 0x300005, 0xff,  1, pic18f1220_300005 },
	{ 0x300006, 0xff,  1, pic18c242_300006 },
	{ 0x300008, 0xff,  2, ps500_300008 },
	{ 0x300009, 0xff,  2, pic18f1220_300009 },
	{ 0x30000a, 0xff,  2, ps500_30000a },
	{ 0x30000b, 0xff,  3, pic18f1220_30000b },
	{ 0x30000c, 0xff,  2, ps500_30000c },
	{ 0x30000d, 0xff,  1, pic18f1220_30000d },
};

static const struct gp_cfg_addr gp_cfg_ps810_addrs[] =
{
	{ 0x300003, 0xff,  1, ps500_300003 },
	{ 0x300005, 0xff,  1, pic18f1220_300005 },
	{ 0x300006, 0xff,  1, pic18c242_300006 },
	{ 0x300008, 0xff,  1, ps810_300008 },
	{ 0x300009, 0xff,  1, pic18f2410_300009 },
	{ 0x30000a, 0xff,  1, ps810_30000a },
	{ 0x30000b, 0xff,  2, pic18f2410_30000b },
	{ 0x30000c, 0xff,  1, ps810_30000c },
	{ 0x30000d, 0xff,  1, pic18f1220_30000d },
};

/*
 * Devices Table
 */

const unsigned gp_cfg_device_count = 660;
const struct gp_cfg_device gp_cfg_devices[] =
{
	{ "MCP19110",      1, gp_cfg_mcp19110_addrs },
	{ "MCP19111",      1, gp_cfg_mcp19110_addrs },
	{ "MCP19114",      1, gp_cfg_mcp19114_addrs },
	{ "MCP19115",      1, gp_cfg_mcp19114_addrs },
	{ "MCP25020",      1, gp_cfg_mcp25020_addrs },
	{ "MCP25025",      1, gp_cfg_mcp25020_addrs },
	{ "MCP25050",      1, gp_cfg_mcp25020_addrs },
	{ "MCP25055",      1, gp_cfg_mcp25020_addrs },
	{ "MCV08A",        1, gp_cfg_pic12f510_addrs },
	{ "MCV14A",        1, gp_cfg_pic16f526_addrs },
	{ "MCV18A",        1, gp_cfg_pic16c54_addrs },
	{ "MCV18E",        1, gp_cfg_pic16f716_addrs },
	{ "MCV20USB",     12, gp_cfg_pic18f14k50_addrs },
	{ "MCV28A",        1, gp_cfg_pic16c54_addrs },
	{ "PIC10F200",     1, gp_cfg_pic10f200_addrs },
	{ "PIC10F202",     1, gp_cfg_pic10f200_addrs },
	{ "PIC10F204",     1, gp_cfg_pic10f200_addrs },
	{ "PIC10F206",     1, gp_cfg_pic10f200_addrs },
	{ "PIC10F220",     1, gp_cfg_pic10f220_addrs },
	{ "PIC10F222",     1, gp_cfg_pic10f220_addrs },
	{ "PIC10F320",     1, gp_cfg_pic10f320_addrs },
	{ "PIC10F322",     1, gp_cfg_pic10f320_addrs },
	{ "PIC10LF320",    1, gp_cfg_pic10f320_addrs },
	{ "PIC10LF322",    1, gp_cfg_pic10f320_addrs },
	{ "PIC12C508",     1, gp_cfg_pic12c508_addrs },
	{ "PIC12C508A",    1, gp_cfg_pic12c508_addrs },
	{ "PIC12C509",     1, gp_cfg_pic12c508_addrs },
	{ "PIC12C509A",    1, gp_cfg_pic12c508_addrs },
	{ "PIC12C671",     1, gp_cfg_pic12c671_addrs },
	{ "PIC12C672",     1, gp_cfg_pic12c671_addrs },
	{ "PIC12CE518",    1, gp_cfg_pic12c508_addrs },
	{ "PIC12CE519",    1, gp_cfg_pic12c508_addrs },
	{ "PIC12CE673",    1, gp_cfg_pic12c671_addrs },
	{ "PIC12CE674",    1, gp_cfg_pic12c671_addrs },
	{ "PIC12CR509A",   1, gp_cfg_pic12c508_addrs },
	{ "PIC12F1501",    2, gp_cfg_pic12f1501_addrs },
	{ "PIC12F1571",    2, gp_cfg_pic12f1571_addrs },
	{ "PIC12F1572",    2, gp_cfg_pic12f1571_addrs },
	{ "PIC12F1612",    3, gp_cfg_pic12f1612_addrs },
	{ "PIC12F1822",    2, gp_cfg_pic12f1822_addrs },
	{ "PIC12F1840",    2, gp_cfg_pic12f1822_addrs },
	{ "PIC12F508",     1, gp_cfg_pic12c508_addrs },
	{ "PIC12F509",     1, gp_cfg_pic12c508_addrs },
	{ "PIC12F510",     1, gp_cfg_pic12f510_addrs },
	{ "PIC12F519",     1, gp_cfg_pic12f519_addrs },
	{ "PIC12F520",     1, gp_cfg_pic12f520_addrs },
	{ "PIC12F529T39A",  1, gp_cfg_pic12f529t39a_addrs },
	{ "PIC12F529T48A",  1, gp_cfg_pic12f529t39a_addrs },
	{ "PIC12F609",     1, gp_cfg_pic12f609_addrs },
	{ "PIC12F615",     1, gp_cfg_pic12f609_addrs },
	{ "PIC12F617",     1, gp_cfg_pic12f617_addrs },
	{ "PIC12F629",     1, gp_cfg_pic12f629_addrs },
	{ "PIC12F635",     1, gp_cfg_pic12f635_addrs },
	{ "PIC12F675",     1, gp_cfg_pic12f629_addrs },
	{ "PIC12F683",     1, gp_cfg_pic12f683_addrs },
	{ "PIC12F752",     1, gp_cfg_pic12f752_addrs },
	{ "PIC12HV609",    1, gp_cfg_pic12f609_addrs },
	{ "PIC12HV615",    1, gp_cfg_pic12f609_addrs },
	{ "PIC12HV752",    1, gp_cfg_pic12f752_addrs },
	{ "PIC12LF1501",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC12LF1552",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC12LF1571",   2, gp_cfg_pic12f1571_addrs },
	{ "PIC12LF1572",   2, gp_cfg_pic12f1571_addrs },
	{ "PIC12LF1612",   3, gp_cfg_pic12f1612_addrs },
	{ "PIC12LF1822",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC12LF1840",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC12LF1840T39A",  2, gp_cfg_pic12f1822_addrs },
	{ "PIC12LF1840T48A",  2, gp_cfg_pic12f1822_addrs },
	{ "PIC16C432",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C433",     1, gp_cfg_pic16c433_addrs },
	{ "PIC16C505",     1, gp_cfg_pic16c505_addrs },
	{ "PIC16C54",      1, gp_cfg_pic16c54_addrs },
	{ "PIC16C54A",     1, gp_cfg_pic16c54_addrs },
	{ "PIC16C54C",     1, gp_cfg_pic16c54c_addrs },
	{ "PIC16C55",      1, gp_cfg_pic16c54_addrs },
	{ "PIC16C554",     1, gp_cfg_pic16c554_addrs },
	{ "PIC16C557",     1, gp_cfg_pic16c557_addrs },
	{ "PIC16C558",     1, gp_cfg_pic16c557_addrs },
	{ "PIC16C55A",     1, gp_cfg_pic16c54c_addrs },
	{ "PIC16C56",      1, gp_cfg_pic16c54_addrs },
	{ "PIC16C56A",     1, gp_cfg_pic16c54c_addrs },
	{ "PIC16C57",      1, gp_cfg_pic16c54_addrs },
	{ "PIC16C57C",     1, gp_cfg_pic16c54c_addrs },
	{ "PIC16C58A",     1, gp_cfg_pic16c54_addrs },
	{ "PIC16C58B",     1, gp_cfg_pic16c54c_addrs },
	{ "PIC16C620",     1, gp_cfg_pic16c620_addrs },
	{ "PIC16C620A",    1, gp_cfg_pic16c620_addrs },
	{ "PIC16C621",     1, gp_cfg_pic16c621_addrs },
	{ "PIC16C621A",    1, gp_cfg_pic16c621_addrs },
	{ "PIC16C622",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C622A",    1, gp_cfg_pic16c432_addrs },
	{ "PIC16C62A",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C62B",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C63",      1, gp_cfg_pic16c432_addrs },
	{ "PIC16C63A",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C642",     1, gp_cfg_pic16c642_addrs },
	{ "PIC16C64A",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C65A",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C65B",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C66",      1, gp_cfg_pic16c432_addrs },
	{ "PIC16C662",     1, gp_cfg_pic16c642_addrs },
	{ "PIC16C67",      1, gp_cfg_pic16c432_addrs },
	{ "PIC16C71",      1, gp_cfg_pic16c71_addrs },
	{ "PIC16C710",     1, gp_cfg_pic16c710_addrs },
	{ "PIC16C711",     1, gp_cfg_pic16c710_addrs },
	{ "PIC16C712",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C715",     1, gp_cfg_pic16c642_addrs },
	{ "PIC16C716",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C717",     1, gp_cfg_pic16c717_addrs },
	{ "PIC16C72",      1, gp_cfg_pic16c432_addrs },
	{ "PIC16C72A",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C73A",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C73B",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C745",     1, gp_cfg_pic16c745_addrs },
	{ "PIC16C74A",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C74B",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16C76",      1, gp_cfg_pic16c432_addrs },
	{ "PIC16C765",     1, gp_cfg_pic16c745_addrs },
	{ "PIC16C77",      1, gp_cfg_pic16c432_addrs },
	{ "PIC16C770",     1, gp_cfg_pic16c717_addrs },
	{ "PIC16C771",     1, gp_cfg_pic16c717_addrs },
	{ "PIC16C773",     1, gp_cfg_pic16c773_addrs },
	{ "PIC16C774",     1, gp_cfg_pic16c773_addrs },
	{ "PIC16C781",     1, gp_cfg_pic16c717_addrs },
	{ "PIC16C782",     1, gp_cfg_pic16c717_addrs },
	{ "PIC16C923",     1, gp_cfg_pic16c557_addrs },
	{ "PIC16C924",     1, gp_cfg_pic16c557_addrs },
	{ "PIC16C925",     1, gp_cfg_pic16c925_addrs },
	{ "PIC16C926",     1, gp_cfg_pic16c925_addrs },
	{ "PIC16CE623",    1, gp_cfg_pic16c620_addrs },
	{ "PIC16CE624",    1, gp_cfg_pic16c621_addrs },
	{ "PIC16CE625",    1, gp_cfg_pic16c432_addrs },
	{ "PIC16CR54",     1, gp_cfg_pic16c54_addrs },
	{ "PIC16CR54A",    1, gp_cfg_pic16c54c_addrs },
	{ "PIC16CR54C",    1, gp_cfg_pic16c54c_addrs },
	{ "PIC16CR56A",    1, gp_cfg_pic16c54c_addrs },
	{ "PIC16CR57A",    1, gp_cfg_pic16c54c_addrs },
	{ "PIC16CR57B",    1, gp_cfg_pic16c54c_addrs },
	{ "PIC16CR57C",    1, gp_cfg_pic16c54c_addrs },
	{ "PIC16CR58A",    1, gp_cfg_pic16c54c_addrs },
	{ "PIC16CR58B",    1, gp_cfg_pic16c54c_addrs },
	{ "PIC16CR62",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16CR620A",   1, gp_cfg_pic16c620_addrs },
	{ "PIC16CR63",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16CR64",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16CR65",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16CR72",     1, gp_cfg_pic16c432_addrs },
	{ "PIC16CR73",     1, gp_cfg_pic16cr73_addrs },
	{ "PIC16CR74",     1, gp_cfg_pic16cr73_addrs },
	{ "PIC16CR76",     1, gp_cfg_pic16cr73_addrs },
	{ "PIC16CR77",     1, gp_cfg_pic16cr73_addrs },
	{ "PIC16CR83",     1, gp_cfg_pic16cr83_addrs },
	{ "PIC16CR84",     1, gp_cfg_pic16cr83_addrs },
	{ "PIC16CR926",    1, gp_cfg_pic16c925_addrs },
	{ "PIC16F1454",    2, gp_cfg_pic16f1454_addrs },
	{ "PIC16F1455",    2, gp_cfg_pic16f1454_addrs },
	{ "PIC16F1459",    2, gp_cfg_pic16f1454_addrs },
	{ "PIC16F1503",    2, gp_cfg_pic12f1501_addrs },
	{ "PIC16F1507",    2, gp_cfg_pic12f1501_addrs },
	{ "PIC16F1508",    2, gp_cfg_pic16f1508_addrs },
	{ "PIC16F1509",    2, gp_cfg_pic16f1508_addrs },
	{ "PIC16F1512",    2, gp_cfg_pic16f1512_addrs },
	{ "PIC16F1513",    2, gp_cfg_pic16f1512_addrs },
	{ "PIC16F1516",    2, gp_cfg_pic16f1512_addrs },
	{ "PIC16F1517",    2, gp_cfg_pic16f1512_addrs },
	{ "PIC16F1518",    2, gp_cfg_pic16f1512_addrs },
	{ "PIC16F1519",    2, gp_cfg_pic16f1512_addrs },
	{ "PIC16F1526",    2, gp_cfg_pic16f1512_addrs },
	{ "PIC16F1527",    2, gp_cfg_pic16f1512_addrs },
	{ "PIC16F1613",    3, gp_cfg_pic12f1612_addrs },
	{ "PIC16F1703",    2, gp_cfg_pic16f1703_addrs },
	{ "PIC16F1704",    2, gp_cfg_pic16f1704_addrs },
	{ "PIC16F1705",    2, gp_cfg_pic16f1704_addrs },
	{ "PIC16F1707",    2, gp_cfg_pic16f1703_addrs },
	{ "PIC16F1708",    2, gp_cfg_pic16f1704_addrs },
	{ "PIC16F1709",    2, gp_cfg_pic16f1704_addrs },
	{ "PIC16F1713",    2, gp_cfg_pic16f1704_addrs },
	{ "PIC16F1716",    2, gp_cfg_pic16f1704_addrs },
	{ "PIC16F1717",    2, gp_cfg_pic16f1704_addrs },
	{ "PIC16F1718",    2, gp_cfg_pic16f1704_addrs },
	{ "PIC16F1719",    2, gp_cfg_pic16f1704_addrs },
	{ "PIC16F1782",    2, gp_cfg_pic16f1782_addrs },
	{ "PIC16F1783",    2, gp_cfg_pic16f1782_addrs },
	{ "PIC16F1784",    2, gp_cfg_pic16f1782_addrs },
	{ "PIC16F1786",    2, gp_cfg_pic16f1782_addrs },
	{ "PIC16F1787",    2, gp_cfg_pic16f1782_addrs },
	{ "PIC16F1788",    2, gp_cfg_pic16f1782_addrs },
	{ "PIC16F1789",    2, gp_cfg_pic16f1782_addrs },
	{ "PIC16F1823",    2, gp_cfg_pic12f1822_addrs },
	{ "PIC16F1824",    2, gp_cfg_pic12f1822_addrs },
	{ "PIC16F1825",    2, gp_cfg_pic12f1822_addrs },
	{ "PIC16F1826",    2, gp_cfg_pic12f1822_addrs },
	{ "PIC16F1827",    2, gp_cfg_pic12f1822_addrs },
	{ "PIC16F1828",    2, gp_cfg_pic12f1822_addrs },
	{ "PIC16F1829",    2, gp_cfg_pic12f1822_addrs },
	{ "PIC16F1829LIN",  2, gp_cfg_pic12f1822_addrs },
	{ "PIC16F18313",   4, gp_cfg_pic16f18313_addrs },
	{ "PIC16F18323",   4, gp_cfg_pic16f18313_addrs },
	{ "PIC16F1847",    2, gp_cfg_pic12f1822_addrs },
	{ "PIC16F1933",    2, gp_cfg_pic16f1933_addrs },
	{ "PIC16F1934",    2, gp_cfg_pic16f1933_addrs },
	{ "PIC16F1936",    2, gp_cfg_pic16f1933_addrs },
	{ "PIC16F1937",    2, gp_cfg_pic16f1933_addrs },
	{ "PIC16F1938",    2, gp_cfg_pic16f1933_addrs },
	{ "PIC16F1939",    2, gp_cfg_pic16f1933_addrs },
	{ "PIC16F1946",    2, gp_cfg_pic16f1946_addrs },
	{ "PIC16F1947",    2, gp_cfg_pic16f1946_addrs },
	{ "PIC16F505",     1, gp_cfg_pic16f505_addrs },
	{ "PIC16F506",     1, gp_cfg_pic16f506_addrs },
	{ "PIC16F526",     1, gp_cfg_pic16f526_addrs },
	{ "PIC16F527",     1, gp_cfg_pic16f527_addrs },
	{ "PIC16F54",      1, gp_cfg_pic16c54_addrs },
	{ "PIC16F57",      1, gp_cfg_pic16c54_addrs },
	{ "PIC16F570",     1, gp_cfg_pic16f570_addrs },
	{ "PIC16F59",      1, gp_cfg_pic16c54_addrs },
	{ "PIC16F610",     1, gp_cfg_pic12f609_addrs },
	{ "PIC16F616",     1, gp_cfg_pic12f609_addrs },
	{ "PIC16F627",     1, gp_cfg_pic16f627_addrs },
	{ "PIC16F627A",    1, gp_cfg_pic16f627a_addrs },
	{ "PIC16F628",     1, gp_cfg_pic16f627_addrs },
	{ "PIC16F628A",    1, gp_cfg_pic16f627a_addrs },
	{ "PIC16F630",     1, gp_cfg_pic16f630_addrs },
	{ "PIC16F631",     1, gp_cfg_pic16f631_addrs },
	{ "PIC16F636",     1, gp_cfg_pic12f635_addrs },
	{ "PIC16F639",     1, gp_cfg_pic12f635_addrs },
	{ "PIC16F648A",    1, gp_cfg_pic16f627a_addrs },
	{ "PIC16F676",     1, gp_cfg_pic16f630_addrs },
	{ "PIC16F677",     1, gp_cfg_pic16f631_addrs },
	{ "PIC16F684",     1, gp_cfg_pic12f683_addrs },
	{ "PIC16F685",     1, gp_cfg_pic16f631_addrs },
	{ "PIC16F687",     1, gp_cfg_pic16f631_addrs },
	{ "PIC16F688",     1, gp_cfg_pic12f683_addrs },
	{ "PIC16F689",     1, gp_cfg_pic16f631_addrs },
	{ "PIC16F690",     1, gp_cfg_pic16f631_addrs },
	{ "PIC16F707",     2, gp_cfg_pic16f707_addrs },
	{ "PIC16F716",     1, gp_cfg_pic16f716_addrs },
	{ "PIC16F72",      1, gp_cfg_pic16cr73_addrs },
	{ "PIC16F720",     2, gp_cfg_pic16f720_addrs },
	{ "PIC16F721",     2, gp_cfg_pic16f720_addrs },
	{ "PIC16F722",     2, gp_cfg_pic16f722_addrs },
	{ "PIC16F722A",    2, gp_cfg_pic16f722_addrs },
	{ "PIC16F723",     2, gp_cfg_pic16f722_addrs },
	{ "PIC16F723A",    2, gp_cfg_pic16f722_addrs },
	{ "PIC16F724",     2, gp_cfg_pic16f722_addrs },
	{ "PIC16F726",     2, gp_cfg_pic16f722_addrs },
	{ "PIC16F727",     2, gp_cfg_pic16f722_addrs },
	{ "PIC16F73",      1, gp_cfg_pic16cr73_addrs },
	{ "PIC16F737",     2, gp_cfg_pic16f737_addrs },
	{ "PIC16F74",      1, gp_cfg_pic16cr73_addrs },
	{ "PIC16F747",     2, gp_cfg_pic16f737_addrs },
	{ "PIC16F753",     1, gp_cfg_pic12f752_addrs },
	{ "PIC16F76",      1, gp_cfg_pic16cr73_addrs },
	{ "PIC16F767",     2, gp_cfg_pic16f737_addrs },
	{ "PIC16F77",      1, gp_cfg_pic16cr73_addrs },
	{ "PIC16F777",     2, gp_cfg_pic16f737_addrs },
	{ "PIC16F785",     1, gp_cfg_pic12f683_addrs },
	{ "PIC16F818",     1, gp_cfg_pic16f818_addrs },
	{ "PIC16F819",     1, gp_cfg_pic16f819_addrs },
	{ "PIC16F83",      1, gp_cfg_pic16f83_addrs },
	{ "PIC16F84",      1, gp_cfg_pic16f83_addrs },
	{ "PIC16F84A",     1, gp_cfg_pic16f83_addrs },
	{ "PIC16F87",      2, gp_cfg_pic16f87_addrs },
	{ "PIC16F870",     1, gp_cfg_pic16f870_addrs },
	{ "PIC16F871",     1, gp_cfg_pic16f870_addrs },
	{ "PIC16F872",     1, gp_cfg_pic16f870_addrs },
	{ "PIC16F873",     1, gp_cfg_pic16f873_addrs },
	{ "PIC16F873A",    1, gp_cfg_pic16f873a_addrs },
	{ "PIC16F874",     1, gp_cfg_pic16f873_addrs },
	{ "PIC16F874A",    1, gp_cfg_pic16f873a_addrs },
	{ "PIC16F876",     1, gp_cfg_pic16f873_addrs },
	{ "PIC16F876A",    1, gp_cfg_pic16f873a_addrs },
	{ "PIC16F877",     1, gp_cfg_pic16f873_addrs },
	{ "PIC16F877A",    1, gp_cfg_pic16f873a_addrs },
	{ "PIC16F88",      2, gp_cfg_pic16f87_addrs },
	{ "PIC16F882",     2, gp_cfg_pic16f882_addrs },
	{ "PIC16F883",     2, gp_cfg_pic16f883_addrs },
	{ "PIC16F884",     2, gp_cfg_pic16f883_addrs },
	{ "PIC16F886",     2, gp_cfg_pic16f883_addrs },
	{ "PIC16F887",     2, gp_cfg_pic16f883_addrs },
	{ "PIC16F913",     1, gp_cfg_pic16f913_addrs },
	{ "PIC16F914",     1, gp_cfg_pic16f913_addrs },
	{ "PIC16F916",     1, gp_cfg_pic16f913_addrs },
	{ "PIC16F917",     1, gp_cfg_pic16f913_addrs },
	{ "PIC16F946",     1, gp_cfg_pic16f913_addrs },
	{ "PIC16HV540",    1, gp_cfg_pic16c54_addrs },
	{ "PIC16HV610",    1, gp_cfg_pic12f609_addrs },
	{ "PIC16HV616",    1, gp_cfg_pic12f609_addrs },
	{ "PIC16HV753",    1, gp_cfg_pic12f752_addrs },
	{ "PIC16HV785",    1, gp_cfg_pic12f683_addrs },
	{ "PIC16LF1454",   2, gp_cfg_pic16f1454_addrs },
	{ "PIC16LF1455",   2, gp_cfg_pic16f1454_addrs },
	{ "PIC16LF1459",   2, gp_cfg_pic16f1454_addrs },
	{ "PIC16LF1503",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC16LF1507",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC16LF1508",   2, gp_cfg_pic16f1508_addrs },
	{ "PIC16LF1509",   2, gp_cfg_pic16f1508_addrs },
	{ "PIC16LF1512",   2, gp_cfg_pic16f1508_addrs },
	{ "PIC16LF1513",   2, gp_cfg_pic16f1508_addrs },
	{ "PIC16LF1516",   2, gp_cfg_pic16f1508_addrs },
	{ "PIC16LF1517",   2, gp_cfg_pic16f1508_addrs },
	{ "PIC16LF1518",   2, gp_cfg_pic16f1508_addrs },
	{ "PIC16LF1519",   2, gp_cfg_pic16f1508_addrs },
	{ "PIC16LF1526",   2, gp_cfg_pic16f1508_addrs },
	{ "PIC16LF1527",   2, gp_cfg_pic16f1508_addrs },
	{ "PIC16LF1554",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC16LF1559",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC16LF1613",   3, gp_cfg_pic12f1612_addrs },
	{ "PIC16LF1703",   2, gp_cfg_pic16f1703_addrs },
	{ "PIC16LF1704",   2, gp_cfg_pic16f1704_addrs },
	{ "PIC16LF1705",   2, gp_cfg_pic16f1704_addrs },
	{ "PIC16LF1707",   2, gp_cfg_pic16f1703_addrs },
	{ "PIC16LF1708",   2, gp_cfg_pic16f1704_addrs },
	{ "PIC16LF1709",   2, gp_cfg_pic16f1704_addrs },
	{ "PIC16LF1713",   2, gp_cfg_pic16f1704_addrs },
	{ "PIC16LF1716",   2, gp_cfg_pic16f1704_addrs },
	{ "PIC16LF1717",   2, gp_cfg_pic16f1704_addrs },
	{ "PIC16LF1718",   2, gp_cfg_pic16f1704_addrs },
	{ "PIC16LF1719",   2, gp_cfg_pic16f1704_addrs },
	{ "PIC16LF1782",   2, gp_cfg_pic16lf1782_addrs },
	{ "PIC16LF1783",   2, gp_cfg_pic16lf1782_addrs },
	{ "PIC16LF1784",   2, gp_cfg_pic16lf1782_addrs },
	{ "PIC16LF1786",   2, gp_cfg_pic16lf1782_addrs },
	{ "PIC16LF1787",   2, gp_cfg_pic16lf1782_addrs },
	{ "PIC16LF1788",   2, gp_cfg_pic16lf1782_addrs },
	{ "PIC16LF1789",   2, gp_cfg_pic16lf1782_addrs },
	{ "PIC16LF1823",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1824",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1824T39A",  2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1825",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1826",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1827",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1828",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1829",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF18313",  4, gp_cfg_pic16f18313_addrs },
	{ "PIC16LF18323",  4, gp_cfg_pic16f18313_addrs },
	{ "PIC16LF1847",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1902",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC16LF1903",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC16LF1904",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC16LF1906",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC16LF1907",   2, gp_cfg_pic12f1501_addrs },
	{ "PIC16LF1933",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1934",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1936",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1937",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1938",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1939",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1946",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF1947",   2, gp_cfg_pic12f1822_addrs },
	{ "PIC16LF707",    2, gp_cfg_pic16lf707_addrs },
	{ "PIC16LF720",    2, gp_cfg_pic16f720_addrs },
	{ "PIC16LF721",    2, gp_cfg_pic16f720_addrs },
	{ "PIC16LF722",    2, gp_cfg_pic16lf722_addrs },
	{ "PIC16LF722A",   2, gp_cfg_pic16lf722_addrs },
	{ "PIC16LF723",    2, gp_cfg_pic16lf722_addrs },
	{ "PIC16LF723A",   2, gp_cfg_pic16lf722_addrs },
	{ "PIC16LF724",    2, gp_cfg_pic16lf722_addrs },
	{ "PIC16LF726",    2, gp_cfg_pic16lf722_addrs },
	{ "PIC16LF727",    2, gp_cfg_pic16lf722_addrs },
	{ "PIC18C242",     6, gp_cfg_pic18c242_addrs },
	{ "PIC18C252",     6, gp_cfg_pic18c242_addrs },
	{ "PIC18C442",     6, gp_cfg_pic18c242_addrs },
	{ "PIC18C452",     6, gp_cfg_pic18c452_addrs },
	{ "PIC18C601",     4, gp_cfg_pic18c601_addrs },
	{ "PIC18C658",     5, gp_cfg_pic18c658_addrs },
	{ "PIC18C801",     4, gp_cfg_pic18c601_addrs },
	{ "PIC18C858",     5, gp_cfg_pic18c658_addrs },
	{ "PIC18F1220",   11, gp_cfg_pic18f1220_addrs },
	{ "PIC18F1230",   12, gp_cfg_pic18f1230_addrs },
	{ "PIC18F1320",   11, gp_cfg_pic18f1220_addrs },
	{ "PIC18F1330",   12, gp_cfg_pic18f1330_addrs },
	{ "PIC18F13K22",  11, gp_cfg_pic18f13k22_addrs },
	{ "PIC18F13K50",  12, gp_cfg_pic18f13k50_addrs },
	{ "PIC18F14K22",  11, gp_cfg_pic18f13k22_addrs },
	{ "PIC18F14K22LIN", 11, gp_cfg_pic18f13k22_addrs },
	{ "PIC18F14K50",  12, gp_cfg_pic18f14k50_addrs },
	{ "PIC18F2220",   11, gp_cfg_pic18f2220_addrs },
	{ "PIC18F2221",   11, gp_cfg_pic18f2221_addrs },
	{ "PIC18F2320",   11, gp_cfg_pic18f2320_addrs },
	{ "PIC18F2321",   11, gp_cfg_pic18f2321_addrs },
	{ "PIC18F2331",   12, gp_cfg_pic18f2331_addrs },
	{ "PIC18F23K20",  11, gp_cfg_pic18f23k20_addrs },
	{ "PIC18F23K22",  11, gp_cfg_pic18f23k22_addrs },
	{ "PIC18F2410",   11, gp_cfg_pic18f2410_addrs },
	{ "PIC18F242",    11, gp_cfg_pic18f242_addrs },
	{ "PIC18F2420",   11, gp_cfg_pic18f2420_addrs },
	{ "PIC18F2423",   11, gp_cfg_pic18f2420_addrs },
	{ "PIC18F2431",   12, gp_cfg_pic18f2431_addrs },
	{ "PIC18F2439",   10, gp_cfg_pic18f2439_addrs },
	{ "PIC18F2450",   12, gp_cfg_pic18f2450_addrs },
	{ "PIC18F2455",   12, gp_cfg_pic18f2455_addrs },
	{ "PIC18F2458",   12, gp_cfg_pic18f2455_addrs },
	{ "PIC18F248",    10, gp_cfg_pic18f248_addrs },
	{ "PIC18F2480",   11, gp_cfg_pic18f2480_addrs },
	{ "PIC18F24J10",   5, gp_cfg_pic18f24j10_addrs },
	{ "PIC18F24J11",   8, gp_cfg_pic18f24j11_addrs },
	{ "PIC18F24J50",   8, gp_cfg_pic18f24j50_addrs },
	{ "PIC18F24K20",  11, gp_cfg_pic18f23k20_addrs },
	{ "PIC18F24K22",  11, gp_cfg_pic18f23k22_addrs },
	{ "PIC18F24K50",  12, gp_cfg_pic18f24k50_addrs },
	{ "PIC18F2510",   11, gp_cfg_pic18f2510_addrs },
	{ "PIC18F2515",   11, gp_cfg_pic18f2515_addrs },
	{ "PIC18F252",    11, gp_cfg_pic18f252_addrs },
	{ "PIC18F2520",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F2523",   11, gp_cfg_pic18f2523_addrs },
	{ "PIC18F2525",   11, gp_cfg_pic18f2525_addrs },
	{ "PIC18F2539",   10, gp_cfg_pic18f2539_addrs },
	{ "PIC18F2550",   12, gp_cfg_pic18f2550_addrs },
	{ "PIC18F2553",   12, gp_cfg_pic18f2550_addrs },
	{ "PIC18F258",    10, gp_cfg_pic18f258_addrs },
	{ "PIC18F2580",   11, gp_cfg_pic18f2580_addrs },
	{ "PIC18F2585",   11, gp_cfg_pic18f2585_addrs },
	{ "PIC18F25J10",   5, gp_cfg_pic18f25j10_addrs },
	{ "PIC18F25J11",   8, gp_cfg_pic18f25j11_addrs },
	{ "PIC18F25J50",   8, gp_cfg_pic18f25j50_addrs },
	{ "PIC18F25K20",  11, gp_cfg_pic18f25k20_addrs },
	{ "PIC18F25K22",  11, gp_cfg_pic18f25k22_addrs },
	{ "PIC18F25K50",  12, gp_cfg_pic18f25k50_addrs },
	{ "PIC18F25K80",  12, gp_cfg_pic18f25k80_addrs },
	{ "PIC18F2610",   11, gp_cfg_pic18f2510_addrs },
	{ "PIC18F2620",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F2680",   11, gp_cfg_pic18f2680_addrs },
	{ "PIC18F2682",   11, gp_cfg_pic18f2682_addrs },
	{ "PIC18F2685",   11, gp_cfg_pic18f2685_addrs },
	{ "PIC18F26J11",   8, gp_cfg_pic18f26j11_addrs },
	{ "PIC18F26J13",   8, gp_cfg_pic18f26j13_addrs },
	{ "PIC18F26J50",   8, gp_cfg_pic18f26j50_addrs },
	{ "PIC18F26J53",   8, gp_cfg_pic18f26j53_addrs },
	{ "PIC18F26K20",  11, gp_cfg_pic18f25k20_addrs },
	{ "PIC18F26K22",  11, gp_cfg_pic18f25k22_addrs },
	{ "PIC18F26K80",  12, gp_cfg_pic18f25k80_addrs },
	{ "PIC18F27J13",   8, gp_cfg_pic18f27j13_addrs },
	{ "PIC18F27J53",   8, gp_cfg_pic18f27j53_addrs },
	{ "PIC18F4220",   11, gp_cfg_pic18f2220_addrs },
	{ "PIC18F4221",   11, gp_cfg_pic18f2221_addrs },
	{ "PIC18F4320",   11, gp_cfg_pic18f2320_addrs },
	{ "PIC18F4321",   11, gp_cfg_pic18f2321_addrs },
	{ "PIC18F4331",   12, gp_cfg_pic18f4331_addrs },
	{ "PIC18F43K20",  11, gp_cfg_pic18f23k20_addrs },
	{ "PIC18F43K22",  11, gp_cfg_pic18f43k22_addrs },
	{ "PIC18F4410",   11, gp_cfg_pic18f2410_addrs },
	{ "PIC18F442",    11, gp_cfg_pic18f242_addrs },
	{ "PIC18F4420",   11, gp_cfg_pic18f4420_addrs },
	{ "PIC18F4423",   11, gp_cfg_pic18f2420_addrs },
	{ "PIC18F4431",   12, gp_cfg_pic18f4431_addrs },
	{ "PIC18F4439",   10, gp_cfg_pic18f2439_addrs },
	{ "PIC18F4450",   12, gp_cfg_pic18f4450_addrs },
	{ "PIC18F4455",   12, gp_cfg_pic18f4455_addrs },
	{ "PIC18F4458",   12, gp_cfg_pic18f4455_addrs },
	{ "PIC18F448",    10, gp_cfg_pic18f248_addrs },
	{ "PIC18F4480",   11, gp_cfg_pic18f2480_addrs },
	{ "PIC18F44J10",   5, gp_cfg_pic18f24j10_addrs },
	{ "PIC18F44J11",   8, gp_cfg_pic18f24j11_addrs },
	{ "PIC18F44J50",   8, gp_cfg_pic18f24j50_addrs },
	{ "PIC18F44K20",  11, gp_cfg_pic18f23k20_addrs },
	{ "PIC18F44K22",  11, gp_cfg_pic18f43k22_addrs },
	{ "PIC18F4510",   11, gp_cfg_pic18f2510_addrs },
	{ "PIC18F4515",   11, gp_cfg_pic18f2515_addrs },
	{ "PIC18F452",    11, gp_cfg_pic18f252_addrs },
	{ "PIC18F4520",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F4523",   11, gp_cfg_pic18f2523_addrs },
	{ "PIC18F4525",   11, gp_cfg_pic18f2525_addrs },
	{ "PIC18F4539",   10, gp_cfg_pic18f2539_addrs },
	{ "PIC18F4550",   12, gp_cfg_pic18f4550_addrs },
	{ "PIC18F4553",   12, gp_cfg_pic18f4550_addrs },
	{ "PIC18F458",    10, gp_cfg_pic18f258_addrs },
	{ "PIC18F4580",   11, gp_cfg_pic18f2580_addrs },
	{ "PIC18F4585",   11, gp_cfg_pic18f2585_addrs },
	{ "PIC18F45J10",   5, gp_cfg_pic18f25j10_addrs },
	{ "PIC18F45J11",   8, gp_cfg_pic18f25j11_addrs },
	{ "PIC18F45J50",   8, gp_cfg_pic18f25j50_addrs },
	{ "PIC18F45K20",  11, gp_cfg_pic18f25k20_addrs },
	{ "PIC18F45K22",  11, gp_cfg_pic18f45k22_addrs },
	{ "PIC18F45K50",  12, gp_cfg_pic18f45k50_addrs },
	{ "PIC18F45K80",  12, gp_cfg_pic18f25k80_addrs },
	{ "PIC18F4610",   11, gp_cfg_pic18f2510_addrs },
	{ "PIC18F4620",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F4680",   11, gp_cfg_pic18f2680_addrs },
	{ "PIC18F4682",   11, gp_cfg_pic18f2682_addrs },
	{ "PIC18F4685",   11, gp_cfg_pic18f2685_addrs },
	{ "PIC18F46J11",   8, gp_cfg_pic18f26j11_addrs },
	{ "PIC18F46J13",   8, gp_cfg_pic18f26j13_addrs },
	{ "PIC18F46J50",   8, gp_cfg_pic18f26j50_addrs },
	{ "PIC18F46J53",   8, gp_cfg_pic18f26j53_addrs },
	{ "PIC18F46K20",  11, gp_cfg_pic18f25k20_addrs },
	{ "PIC18F46K22",  11, gp_cfg_pic18f45k22_addrs },
	{ "PIC18F46K80",  12, gp_cfg_pic18f25k80_addrs },
	{ "PIC18F47J13",   8, gp_cfg_pic18f27j13_addrs },
	{ "PIC18F47J53",   8, gp_cfg_pic18f27j53_addrs },
	{ "PIC18F6310",    8, gp_cfg_pic18f6310_addrs },
	{ "PIC18F6390",    6, gp_cfg_pic18f6390_addrs },
	{ "PIC18F6393",    6, gp_cfg_pic18f6390_addrs },
	{ "PIC18F63J11",   6, gp_cfg_pic18f63j11_addrs },
	{ "PIC18F63J90",   6, gp_cfg_pic18f63j11_addrs },
	{ "PIC18F6410",    8, gp_cfg_pic18f6310_addrs },
	{ "PIC18F6490",    6, gp_cfg_pic18f6390_addrs },
	{ "PIC18F6493",    6, gp_cfg_pic18f6390_addrs },
	{ "PIC18F64J11",   6, gp_cfg_pic18f64j11_addrs },
	{ "PIC18F64J90",   6, gp_cfg_pic18f64j11_addrs },
	{ "PIC18F6520",   12, gp_cfg_pic18f6520_addrs },
	{ "PIC18F6525",   12, gp_cfg_pic18f6525_addrs },
	{ "PIC18F6527",   11, gp_cfg_pic18f6527_addrs },
	{ "PIC18F6585",   12, gp_cfg_pic18f6585_addrs },
	{ "PIC18F65J10",   6, gp_cfg_pic18f65j10_addrs },
	{ "PIC18F65J11",   6, gp_cfg_pic18f65j11_addrs },
	{ "PIC18F65J15",   6, gp_cfg_pic18f65j15_addrs },
	{ "PIC18F65J50",   6, gp_cfg_pic18f65j50_addrs },
	{ "PIC18F65J90",   6, gp_cfg_pic18f65j90_addrs },
	{ "PIC18F65J94",  16, gp_cfg_pic18f65j94_addrs },
	{ "PIC18F65K22",  13, gp_cfg_pic18f65k22_addrs },
	{ "PIC18F65K80",  12, gp_cfg_pic18f65k80_addrs },
	{ "PIC18F65K90",  13, gp_cfg_pic18f65k22_addrs },
	{ "PIC18F6620",   12, gp_cfg_pic18f6520_addrs },
	{ "PIC18F6621",   12, gp_cfg_pic18f6621_addrs },
	{ "PIC18F6622",   11, gp_cfg_pic18f6622_addrs },
	{ "PIC18F6627",   11, gp_cfg_pic18f6627_addrs },
	{ "PIC18F6628",   11, gp_cfg_pic18f6627_addrs },
	{ "PIC18F6680",   12, gp_cfg_pic18f6680_addrs },
	{ "PIC18F66J10",   6, gp_cfg_pic18f66j10_addrs },
	{ "PIC18F66J11",   6, gp_cfg_pic18f66j11_addrs },
	{ "PIC18F66J15",   6, gp_cfg_pic18f66j15_addrs },
	{ "PIC18F66J16",   6, gp_cfg_pic18f66j16_addrs },
	{ "PIC18F66J50",   6, gp_cfg_pic18f66j50_addrs },
	{ "PIC18F66J55",   6, gp_cfg_pic18f66j55_addrs },
	{ "PIC18F66J60",   6, gp_cfg_pic18f66j60_addrs },
	{ "PIC18F66J65",   6, gp_cfg_pic18f66j65_addrs },
	{ "PIC18F66J90",   6, gp_cfg_pic18f66j90_addrs },
	{ "PIC18F66J93",   6, gp_cfg_pic18f66j93_addrs },
	{ "PIC18F66J94",  16, gp_cfg_pic18f66j94_addrs },
	{ "PIC18F66J99",  16, gp_cfg_pic18f66j99_addrs },
	{ "PIC18F66K22",  13, gp_cfg_pic18f65k22_addrs },
	{ "PIC18F66K80",  12, gp_cfg_pic18f65k80_addrs },
	{ "PIC18F66K90",  13, gp_cfg_pic18f65k22_addrs },
	{ "PIC18F6720",   12, gp_cfg_pic18f6720_addrs },
	{ "PIC18F6722",   11, gp_cfg_pic18f6722_addrs },
	{ "PIC18F6723",   11, gp_cfg_pic18f6722_addrs },
	{ "PIC18F67J10",   6, gp_cfg_pic18f67j10_addrs },
	{ "PIC18F67J11",   6, gp_cfg_pic18f67j11_addrs },
	{ "PIC18F67J50",   6, gp_cfg_pic18f67j50_addrs },
	{ "PIC18F67J60",   6, gp_cfg_pic18f67j60_addrs },
	{ "PIC18F67J90",   6, gp_cfg_pic18f67j90_addrs },
	{ "PIC18F67J93",   6, gp_cfg_pic18f67j93_addrs },
	{ "PIC18F67J94",  16, gp_cfg_pic18f67j94_addrs },
	{ "PIC18F67K22",  13, gp_cfg_pic18f67k22_addrs },
	{ "PIC18F67K90",  13, gp_cfg_pic18f67k22_addrs },
	{ "PIC18F8310",    8, gp_cfg_pic18f8310_addrs },
	{ "PIC18F8390",    6, gp_cfg_pic18f6390_addrs },
	{ "PIC18F8393",    6, gp_cfg_pic18f6390_addrs },
	{ "PIC18F83J11",   6, gp_cfg_pic18f83j11_addrs },
	{ "PIC18F83J90",   6, gp_cfg_pic18f63j11_addrs },
	{ "PIC18F8410",    8, gp_cfg_pic18f8310_addrs },
	{ "PIC18F8490",    6, gp_cfg_pic18f6390_addrs },
	{ "PIC18F8493",    6, gp_cfg_pic18f6390_addrs },
	{ "PIC18F84J11",   6, gp_cfg_pic18f84j11_addrs },
	{ "PIC18F84J90",   6, gp_cfg_pic18f64j11_addrs },
	{ "PIC18F8520",   12, gp_cfg_pic18f8520_addrs },
	{ "PIC18F8525",   12, gp_cfg_pic18f8525_addrs },
	{ "PIC18F8527",   12, gp_cfg_pic18f8527_addrs },
	{ "PIC18F8585",   12, gp_cfg_pic18f8585_addrs },
	{ "PIC18F85J10",   6, gp_cfg_pic18f85j10_addrs },
	{ "PIC18F85J11",   6, gp_cfg_pic18f85j11_addrs },
	{ "PIC18F85J15",   6, gp_cfg_pic18f85j15_addrs },
	{ "PIC18F85J50",   6, gp_cfg_pic18f85j50_addrs },
	{ "PIC18F85J90",   6, gp_cfg_pic18f65j11_addrs },
	{ "PIC18F85J94",  16, gp_cfg_pic18f85j94_addrs },
	{ "PIC18F85K22",  13, gp_cfg_pic18f85k22_addrs },
	{ "PIC18F85K90",  13, gp_cfg_pic18f85k90_addrs },
	{ "PIC18F8620",   12, gp_cfg_pic18f8520_addrs },
	{ "PIC18F8621",   12, gp_cfg_pic18f8621_addrs },
	{ "PIC18F8622",   12, gp_cfg_pic18f8622_addrs },
	{ "PIC18F8627",   12, gp_cfg_pic18f8627_addrs },
	{ "PIC18F8628",   12, gp_cfg_pic18f8627_addrs },
	{ "PIC18F8680",   12, gp_cfg_pic18f8680_addrs },
	{ "PIC18F86J10",   6, gp_cfg_pic18f86j10_addrs },
	{ "PIC18F86J11",   6, gp_cfg_pic18f86j11_addrs },
	{ "PIC18F86J15",   6, gp_cfg_pic18f86j15_addrs },
	{ "PIC18F86J16",   6, gp_cfg_pic18f86j16_addrs },
	{ "PIC18F86J50",   6, gp_cfg_pic18f86j50_addrs },
	{ "PIC18F86J55",   6, gp_cfg_pic18f86j55_addrs },
	{ "PIC18F86J60",   6, gp_cfg_pic18f86j60_addrs },
	{ "PIC18F86J65",   6, gp_cfg_pic18f86j65_addrs },
	{ "PIC18F86J72",   6, gp_cfg_pic18f66j93_addrs },
	{ "PIC18F86J90",   6, gp_cfg_pic18f86j90_addrs },
	{ "PIC18F86J93",   6, gp_cfg_pic18f86j93_addrs },
	{ "PIC18F86J94",  16, gp_cfg_pic18f86j94_addrs },
	{ "PIC18F86J99",  16, gp_cfg_pic18f86j99_addrs },
	{ "PIC18F86K22",  13, gp_cfg_pic18f85k22_addrs },
	{ "PIC18F86K90",  13, gp_cfg_pic18f85k90_addrs },
	{ "PIC18F8720",   12, gp_cfg_pic18f8720_addrs },
	{ "PIC18F8722",   12, gp_cfg_pic18f8722_addrs },
	{ "PIC18F8723",   12, gp_cfg_pic18f8722_addrs },
	{ "PIC18F87J10",   6, gp_cfg_pic18f87j10_addrs },
	{ "PIC18F87J11",   6, gp_cfg_pic18f87j11_addrs },
	{ "PIC18F87J50",   6, gp_cfg_pic18f87j50_addrs },
	{ "PIC18F87J60",   6, gp_cfg_pic18f87j60_addrs },
	{ "PIC18F87J72",   6, gp_cfg_pic18f67j93_addrs },
	{ "PIC18F87J90",   6, gp_cfg_pic18f87j90_addrs },
	{ "PIC18F87J93",   6, gp_cfg_pic18f87j93_addrs },
	{ "PIC18F87J94",  16, gp_cfg_pic18f87j94_addrs },
	{ "PIC18F87K22",  13, gp_cfg_pic18f87k22_addrs },
	{ "PIC18F87K90",  13, gp_cfg_pic18f87k90_addrs },
	{ "PIC18F95J94",  16, gp_cfg_pic18f85j94_addrs },
	{ "PIC18F96J60",   6, gp_cfg_pic18f96j60_addrs },
	{ "PIC18F96J65",   6, gp_cfg_pic18f96j65_addrs },
	{ "PIC18F96J94",  16, gp_cfg_pic18f86j94_addrs },
	{ "PIC18F96J99",  16, gp_cfg_pic18f86j99_addrs },
	{ "PIC18F97J60",   6, gp_cfg_pic18f97j60_addrs },
	{ "PIC18F97J94",  16, gp_cfg_pic18f87j94_addrs },
	{ "PIC18LF13K22", 11, gp_cfg_pic18f13k22_addrs },
	{ "PIC18LF13K50", 12, gp_cfg_pic18f14k50_addrs },
	{ "PIC18LF14K22", 11, gp_cfg_pic18f13k22_addrs },
	{ "PIC18LF14K50", 12, gp_cfg_pic18f14k50_addrs },
	{ "PIC18LF23K22", 11, gp_cfg_pic18f23k22_addrs },
	{ "PIC18LF24J10",  5, gp_cfg_pic18lf24j10_addrs },
	{ "PIC18LF24J11",  8, gp_cfg_pic18f24j11_addrs },
	{ "PIC18LF24J50",  8, gp_cfg_pic18lf24j50_addrs },
	{ "PIC18LF24K22", 11, gp_cfg_pic18f23k22_addrs },
	{ "PIC18LF24K50", 12, gp_cfg_pic18f24k50_addrs },
	{ "PIC18LF25J10",  5, gp_cfg_pic18lf25j10_addrs },
	{ "PIC18LF25J11",  8, gp_cfg_pic18f25j11_addrs },
	{ "PIC18LF25J50",  8, gp_cfg_pic18lf25j50_addrs },
	{ "PIC18LF25K22", 11, gp_cfg_pic18f25k22_addrs },
	{ "PIC18LF25K50", 12, gp_cfg_pic18f25k50_addrs },
	{ "PIC18LF25K80", 12, gp_cfg_pic18f25k80_addrs },
	{ "PIC18LF26J11",  8, gp_cfg_pic18f26j11_addrs },
	{ "PIC18LF26J13",  8, gp_cfg_pic18f26j13_addrs },
	{ "PIC18LF26J50",  8, gp_cfg_pic18lf26j50_addrs },
	{ "PIC18LF26J53",  8, gp_cfg_pic18f26j53_addrs },
	{ "PIC18LF26K22", 11, gp_cfg_pic18f25k22_addrs },
	{ "PIC18LF26K80", 12, gp_cfg_pic18f25k80_addrs },
	{ "PIC18LF27J13",  8, gp_cfg_pic18f27j13_addrs },
	{ "PIC18LF27J53",  8, gp_cfg_pic18f27j53_addrs },
	{ "PIC18LF43K22", 11, gp_cfg_pic18f43k22_addrs },
	{ "PIC18LF44J10",  5, gp_cfg_pic18lf24j10_addrs },
	{ "PIC18LF44J11",  8, gp_cfg_pic18f24j11_addrs },
	{ "PIC18LF44J50",  8, gp_cfg_pic18lf24j50_addrs },
	{ "PIC18LF44K22", 11, gp_cfg_pic18f43k22_addrs },
	{ "PIC18LF45J10",  5, gp_cfg_pic18lf25j10_addrs },
	{ "PIC18LF45J11",  8, gp_cfg_pic18f25j11_addrs },
	{ "PIC18LF45J50",  8, gp_cfg_pic18lf25j50_addrs },
	{ "PIC18LF45K22", 11, gp_cfg_pic18f45k22_addrs },
	{ "PIC18LF45K50", 12, gp_cfg_pic18f45k50_addrs },
	{ "PIC18LF45K80", 12, gp_cfg_pic18f25k80_addrs },
	{ "PIC18LF46J11",  8, gp_cfg_pic18f26j11_addrs },
	{ "PIC18LF46J13",  8, gp_cfg_pic18f26j13_addrs },
	{ "PIC18LF46J50",  8, gp_cfg_pic18lf26j50_addrs },
	{ "PIC18LF46J53",  8, gp_cfg_pic18f26j53_addrs },
	{ "PIC18LF46K22", 11, gp_cfg_pic18f45k22_addrs },
	{ "PIC18LF46K80", 12, gp_cfg_pic18f25k80_addrs },
	{ "PIC18LF47J13",  8, gp_cfg_pic18f27j13_addrs },
	{ "PIC18LF47J53",  8, gp_cfg_pic18f27j53_addrs },
	{ "PIC18LF65K80", 12, gp_cfg_pic18f65k80_addrs },
	{ "PIC18LF66K80", 12, gp_cfg_pic18f65k80_addrs },
	{ "PICRF509AF",    1, gp_cfg_pic12c508_addrs },
	{ "PICRF509AG",    1, gp_cfg_pic12c508_addrs },
	{ "PICRF675F",     1, gp_cfg_pic12f629_addrs },
	{ "PICRF675H",     1, gp_cfg_pic12f629_addrs },
	{ "PICRF675K",     1, gp_cfg_pic12f629_addrs },
	{ "PS500",         9, gp_cfg_ps500_addrs },
	{ "PS810",         9, gp_cfg_ps810_addrs },
};


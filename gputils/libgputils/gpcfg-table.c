/*
 * This code is automatically generated from the MPASMWIN.EXE file.
 * This data is in the public domain. Use care in modifying.
 */

/*
 * since caches are enabled, directive and option structures may seem to include
 * references to other PICs. this is done by caching when two or more PICs contain
 * identical data.
 */

#include "stdhdr.h"
#include "gpcfg.h"

/*
 * PIC18F6410
 */

static struct gp_cfg_option pic18f6410_osc_opts[] =
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

static struct gp_cfg_option pic18f6410_fcmen_opts[] =
{ /* for FCMEN: */
	{ "OFF",       0x00 },
	{ "ON",        0x40 },
};

static struct gp_cfg_option pic18f6410_ieso_opts[] =
{ /* for IESO: */
	{ "OFF",       0x00 },
	{ "ON",        0x80 },
};

static struct gp_cfg_directive pic18f6410_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 10, pic18f6410_osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f6410_pwrt_opts[] =
{ /* for PWRT: */
	{ "ON",        0x00 },
	{ "OFF",       0x01 },
};

static struct gp_cfg_option pic18f6410_boren_opts[] =
{ /* for BOREN: */
	{ "OFF",       0x00 },
	{ "ON",        0x02 },
	{ "NOSLP",     0x04 },
	{ "SBORDIS",   0x06 },
};

static struct gp_cfg_option pic18f6410_borv_opts[] =
{ /* for BORV: */
	{ "0",         0x00 },
	{ "1",         0x08 },
	{ "2",         0x10 },
	{ "3",         0x18 },
};

static struct gp_cfg_directive pic18f6410_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOREN",     0x06,  4, pic18f6410_boren_opts },
	{ "BORV",      0x18,  4, pic18f6410_borv_opts },
};

static struct gp_cfg_option pic18f6410_wdt_opts[] =
{ /* for WDT: */
	{ "OFF",       0x00 },
	{ "ON",        0x01 },
};

static struct gp_cfg_option pic18f6410_wdtps_opts[] =
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

static struct gp_cfg_directive pic18f6410_300003[] =
{ /* for 0x300003 */
	{ "WDT",       0x01,  2, pic18f6410_wdt_opts },
	{ "WDTPS",     0x1e, 16, pic18f6410_wdtps_opts },
};

static struct gp_cfg_option pic18f6410_lpt1osc_opts[] =
{ /* for LPT1OSC: */
	{ "OFF",       0x00 },
	{ "ON",        0x04 },
};

static struct gp_cfg_option pic18f6410_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "PORTE",     0x00 },
	{ "PORTC",     0x01 },
};

static struct gp_cfg_directive pic18f6410_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_ccp2mx_opts },
};

static struct gp_cfg_option pic18f6410_debug_opts[] =
{ /* for DEBUG: */
	{ "ON",        0x00 },
	{ "OFF",       0x80 },
};

static struct gp_cfg_directive pic18f6410_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f6410_300008[] =
{ /* for 0x300008 */
	{ "CP",        0x01,  2, pic18f6410_pwrt_opts },
};

/* config addresses for PIC18F6410: */
static struct gp_cfg_addr gp_cfg_pic18f6410_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f6410_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xc3,  0, NULL },
	{ 0x300005, 0x81,  3, pic18f6410_300005 },
	{ 0x300006, 0x81,  3, pic18f6410_300006 },
	{ 0x300008, 0x01,  1, pic18f6410_300008 },
	{ 0x30000c, 0x01,  0, NULL },
};

/*
 * PIC18F2525
 */

static struct gp_cfg_option pic18f2525_osc_opts[] =
{ /* for OSC: */
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

static struct gp_cfg_directive pic18f2525_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 10, pic18f2525_osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f2525_pbaden_opts[] =
{ /* for PBADEN: */
	{ "OFF",       0x00 },
	{ "ON",        0x02 },
};

static struct gp_cfg_option pic18f2525_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "PORTBE",    0x00 },
	{ "PORTC",     0x01 },
};

static struct gp_cfg_directive pic18f2525_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f2525_ccp2mx_opts },
};

static struct gp_cfg_directive pic18f2525_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_option pic18f2525_cp1_opts[] =
{ /* for CP1: */
	{ "ON",        0x00 },
	{ "OFF",       0x02 },
};

static struct gp_cfg_option pic18f2525_cp2_opts[] =
{ /* for CP2: */
	{ "ON",        0x00 },
	{ "OFF",       0x04 },
};

static struct gp_cfg_directive pic18f2525_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_option pic18f2525_cpb_opts[] =
{ /* for CPB: */
	{ "ON",        0x00 },
	{ "OFF",       0x40 },
};

static struct gp_cfg_directive pic18f2525_300009[] =
{ /* for 0x300009 */
	{ "CPB",       0x40,  2, pic18f2525_cpb_opts },
	{ "CPD",       0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f2525_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_option pic18f2525_wrtc_opts[] =
{ /* for WRTC: */
	{ "ON",        0x00 },
	{ "OFF",       0x20 },
};

static struct gp_cfg_directive pic18f2525_30000b[] =
{ /* for 0x30000b */
	{ "WRTB",      0x40,  2, pic18f2525_cpb_opts },
	{ "WRTC",      0x20,  2, pic18f2525_wrtc_opts },
	{ "WRTD",      0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f2525_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f2525_30000d[] =
{ /* for 0x30000d */
	{ "EBTRB",     0x40,  2, pic18f2525_cpb_opts },
};

/* config addresses for PIC18F2525: */
static struct gp_cfg_addr gp_cfg_pic18f2525_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2525_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4450
 */

static struct gp_cfg_option pic18f4450_plldiv_opts[] =
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

static struct gp_cfg_option pic18f4450_cpudiv_opts[] =
{ /* for CPUDIV: */
	{ "OSC1_PLL2", 0x00 },
	{ "OSC2_PLL3", 0x08 },
	{ "OSC3_PLL4", 0x10 },
	{ "OSC4_PLL6", 0x18 },
};

static struct gp_cfg_option pic18f4450_usbdiv_opts[] =
{ /* for USBDIV: */
	{ "1",         0x00 },
	{ "2",         0x20 },
};

static struct gp_cfg_directive pic18f4450_300000[] =
{ /* for 0x300000 */
	{ "PLLDIV",    0x07,  8, pic18f4450_plldiv_opts },
	{ "CPUDIV",    0x18,  4, pic18f4450_cpudiv_opts },
	{ "USBDIV",    0x20,  2, pic18f4450_usbdiv_opts },
};

static struct gp_cfg_option pic18f4450_fosc_opts[] =
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

static struct gp_cfg_directive pic18f4450_300001[] =
{ /* for 0x300001 */
	{ "FOSC",      0x0f, 12, pic18f4450_fosc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f4450_bor_opts[] =
{ /* for BOR: */
	{ "OFF",       0x00 },
	{ "SOFT",      0x02 },
	{ "ON_ACTIVE", 0x04 },
	{ "ON",        0x06 },
};

static struct gp_cfg_option pic18f4450_borv_opts[] =
{ /* for BORV: */
	{ "46",        0x00 },
	{ "43",        0x08 },
	{ "28",        0x10 },
	{ "21",        0x18 },
};

static struct gp_cfg_option pic18f4450_vregen_opts[] =
{ /* for VREGEN: */
	{ "OFF",       0x00 },
	{ "ON",        0x20 },
};

static struct gp_cfg_directive pic18f4450_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x06,  4, pic18f4450_bor_opts },
	{ "BORV",      0x18,  4, pic18f4450_borv_opts },
	{ "VREGEN",    0x20,  2, pic18f4450_vregen_opts },
};

static struct gp_cfg_directive pic18f4450_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
};

static struct gp_cfg_option pic18f4450_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "BB1K",      0x00 },
	{ "BB2K",      0x08 },
};

static struct gp_cfg_directive pic18f4450_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "BBSIZ",     0x08,  2, pic18f4450_bbsiz_opts },
	{ "ICPRT",     0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f4450_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
};

static struct gp_cfg_directive pic18f4450_300009[] =
{ /* for 0x300009 */
	{ "CPB",       0x40,  2, pic18f2525_cpb_opts },
};

static struct gp_cfg_directive pic18f4450_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
};

static struct gp_cfg_directive pic18f4450_30000b[] =
{ /* for 0x30000b */
	{ "WRTB",      0x40,  2, pic18f2525_cpb_opts },
	{ "WRTC",      0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_directive pic18f4450_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
};

/* config addresses for PIC18F4450: */
static struct gp_cfg_addr gp_cfg_pic18f4450_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f4450_300000 },
	{ 0x300001, 0x05,  3, pic18f4450_300001 },
	{ 0x300002, 0x1f,  4, pic18f4450_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x82,  3, pic18f4450_300005 },
	{ 0x300006, 0x85,  6, pic18f4450_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0x40,  1, pic18f4450_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0x60,  2, pic18f4450_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F67J93
 */

static struct gp_cfg_directive pic18f67j93_01fff8[] =
{ /* for 0x01fff8 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

static struct gp_cfg_directive pic18f67j93_01fff9[] =
{ /* for 0x01fff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_option pic18f67j93_osc_opts[] =
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

static struct gp_cfg_option pic18f67j93_t1dig_opts[] =
{ /* for T1DIG: */
	{ "OFF",       0x00 },
	{ "ON",        0x08 },
};

static struct gp_cfg_option pic18f67j93_lpt1osc_opts[] =
{ /* for LPT1OSC: */
	{ "ON",        0x00 },
	{ "OFF",       0x10 },
};

static struct gp_cfg_directive pic18f67j93_01fffa[] =
{ /* for 0x01fffa */
	{ "OSC",       0x07,  8, pic18f67j93_osc_opts },
	{ "T1DIG",     0x08,  2, pic18f67j93_t1dig_opts },
	{ "LPT1OSC",   0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f67j93_wdtps_opts[] =
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

static struct gp_cfg_directive pic18f67j93_01fffb[] =
{ /* for 0x01fffb */
	{ "WDTPS",     0x0f, 16, pic18f67j93_wdtps_opts },
};

static struct gp_cfg_option pic18f67j93_rtcsosc_opts[] =
{ /* for RTCSOSC: */
	{ "INTOSCREF", 0x00 },
	{ "T1OSCREF",  0x02 },
};

static struct gp_cfg_directive pic18f67j93_01fffc[] =
{ /* for 0x01fffc */
	{ "RTCSOSC",   0x02,  2, pic18f67j93_rtcsosc_opts },
};

static struct gp_cfg_option pic18f67j93_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "ALTERNATE", 0x00 },
	{ "DEFAULT",   0x01 },
};

static struct gp_cfg_directive pic18f67j93_01fffd[] =
{ /* for 0x01fffd */
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F67J93: */
static struct gp_cfg_addr gp_cfg_pic18f67j93_addrs[] =
{
	{ 0x01fff8, 0xe1,  3, pic18f67j93_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j93_01fff9 },
	{ 0x01fffa, 0xdf,  5, pic18f67j93_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffc, 0x02,  1, pic18f67j93_01fffc },
	{ 0x01fffd, 0xf1,  1, pic18f67j93_01fffd },
};

/*
 * PIC18C858
 */

static struct gp_cfg_option pic18c858_cp_opts[] =
{ /* for CP: */
	{ "ON",        0x00 },
	{ "OFF",       0xff },
};

static struct gp_cfg_directive pic18c858_300000[] =
{ /* for 0x300000 */
	{ "CP",        0xff,  2, pic18c858_cp_opts },
};

static struct gp_cfg_option pic18c858_osc_opts[] =
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

static struct gp_cfg_directive pic18c858_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x07,  8, pic18c858_osc_opts },
	{ "OSCS",      0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_option pic18c858_borv_opts[] =
{ /* for BORV: */
	{ "45",        0x00 },
	{ "42",        0x04 },
	{ "27",        0x08 },
	{ "25",        0x0c },
};

static struct gp_cfg_directive pic18c858_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x02,  2, pic18f2525_pbaden_opts },
	{ "BORV",      0x0c,  4, pic18c858_borv_opts },
};

static struct gp_cfg_option pic18c858_wdtps_opts[] =
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

static struct gp_cfg_directive pic18c858_300003[] =
{ /* for 0x300003 */
	{ "WDT",       0x01,  2, pic18f6410_wdt_opts },
	{ "WDTPS",     0x0e,  8, pic18c858_wdtps_opts },
};

static struct gp_cfg_directive pic18c858_300006[] =
{ /* for 0x300006 */
	{ "STVR",      0x01,  2, pic18f6410_wdt_opts },
};

/* config addresses for PIC18C858: */
static struct gp_cfg_addr gp_cfg_pic18c858_addrs[] =
{
	{ 0x300000, 0xff,  1, pic18c858_300000 },
	{ 0x300001, 0xe7,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300006, 0x03,  1, pic18c858_300006 },
};

/*
 * PIC18F2523
 */

static struct gp_cfg_option pic18f2523_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "PORTB",     0x00 },
	{ "PORTC",     0x01 },
};

static struct gp_cfg_directive pic18f2523_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f2523_ccp2mx_opts },
};

static struct gp_cfg_option pic18f2523_cp3_opts[] =
{ /* for CP3: */
	{ "ON",        0x00 },
	{ "OFF",       0x08 },
};

static struct gp_cfg_directive pic18f2523_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
	{ "CP3",       0x08,  2, pic18f2523_cp3_opts },
};

static struct gp_cfg_directive pic18f2523_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
	{ "WRT3",      0x08,  2, pic18f2523_cp3_opts },
};

static struct gp_cfg_directive pic18f2523_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBTR3",     0x08,  2, pic18f2523_cp3_opts },
};

/* config addresses for PIC18F2523: */
static struct gp_cfg_addr gp_cfg_pic18f2523_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2523_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4610
 */

/* config addresses for PIC18F4610: */
static struct gp_cfg_addr gp_cfg_pic18f4610_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2525_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0x40,  1, pic18f4450_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0x60,  2, pic18f4450_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F46K20
 */

static struct gp_cfg_directive pic18f46k20_300001[] =
{ /* for 0x300001 */
	{ "FOSC",      0x0f, 10, pic18f2525_osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f46k20_borv_opts[] =
{ /* for BORV: */
	{ "30",        0x00 },
	{ "27",        0x08 },
	{ "22",        0x10 },
	{ "18",        0x18 },
};

static struct gp_cfg_directive pic18f46k20_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOREN",     0x06,  4, pic18f6410_boren_opts },
	{ "BORV",      0x18,  4, pic18f46k20_borv_opts },
};

static struct gp_cfg_directive pic18f46k20_300003[] =
{ /* for 0x300003 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "WDTPS",     0x1e, 16, pic18f6410_wdtps_opts },
};

static struct gp_cfg_directive pic18f46k20_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "HFOFST",    0x08,  2, pic18f67j93_t1dig_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f2525_ccp2mx_opts },
};

/* config addresses for PIC18F46K20: */
static struct gp_cfg_addr gp_cfg_pic18f46k20_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f46k20_300001 },
	{ 0x300002, 0x1f,  3, pic18f46k20_300002 },
	{ 0x300003, 0x1f,  2, pic18f46k20_300003 },
	{ 0x300005, 0x8b,  5, pic18f46k20_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4458
 */

static struct gp_cfg_directive pic18f4458_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x06,  4, pic18f4450_bor_opts },
	{ "BORV",      0x18,  4, pic18f6410_borv_opts },
	{ "VREGEN",    0x20,  2, pic18f4450_vregen_opts },
};

static struct gp_cfg_directive pic18f4458_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f4458_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ICPRT",     0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F4458: */
static struct gp_cfg_addr gp_cfg_pic18f4458_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f4450_300000 },
	{ 0x300001, 0x05,  3, pic18f4450_300001 },
	{ 0x300002, 0x1f,  4, pic18f4458_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f4458_300005 },
	{ 0x300006, 0x85,  5, pic18f4458_300006 },
	{ 0x300008, 0x0f,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F46K22
 */

static struct gp_cfg_option pic18f46k22_fosc_opts[] =
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

static struct gp_cfg_option pic18f46k22_pllcfg_opts[] =
{ /* for PLLCFG: */
	{ "OFF",       0x00 },
	{ "ON",        0x10 },
};

static struct gp_cfg_directive pic18f46k22_300001[] =
{ /* for 0x300001 */
	{ "FOSC",      0x0f, 14, pic18f46k22_fosc_opts },
	{ "PLLCFG",    0x10,  2, pic18f46k22_pllcfg_opts },
	{ "PRICLKEN",  0x20,  2, pic18f4450_vregen_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f46k22_borv_opts[] =
{ /* for BORV: */
	{ "285",       0x00 },
	{ "250",       0x08 },
	{ "220",       0x10 },
	{ "190",       0x18 },
};

static struct gp_cfg_directive pic18f46k22_300002[] =
{ /* for 0x300002 */
	{ "PWRTEN",    0x01,  2, pic18f6410_pwrt_opts },
	{ "BOREN",     0x06,  4, pic18f6410_boren_opts },
	{ "BORV",      0x18,  4, pic18f46k22_borv_opts },
};

static struct gp_cfg_option pic18f46k22_wdten_opts[] =
{ /* for WDTEN: */
	{ "OFF",       0x00 },
	{ "NOSLP",     0x01 },
	{ "SWON",      0x02 },
	{ "ON",        0x03 },
};

static struct gp_cfg_option pic18f46k22_wdtps_opts[] =
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

static struct gp_cfg_directive pic18f46k22_300003[] =
{ /* for 0x300003 */
	{ "WDTEN",     0x03,  4, pic18f46k22_wdten_opts },
	{ "WDTPS",     0x3c, 16, pic18f46k22_wdtps_opts },
};

static struct gp_cfg_option pic18f46k22_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "PORTB3",    0x00 },
	{ "PORTC1",    0x01 },
};

static struct gp_cfg_option pic18f46k22_ccp3mx_opts[] =
{ /* for CCP3MX: */
	{ "PORTE0",    0x00 },
	{ "PORTB5",    0x04 },
};

static struct gp_cfg_option pic18f46k22_t3cmx_opts[] =
{ /* for T3CMX: */
	{ "PORTB5",    0x00 },
	{ "PORTC0",    0x10 },
};

static struct gp_cfg_option pic18f46k22_p2bmx_opts[] =
{ /* for P2BMX: */
	{ "PORTC0",    0x00 },
	{ "PORTD2",    0x20 },
};

static struct gp_cfg_option pic18f46k22_mclre_opts[] =
{ /* for MCLRE: */
	{ "INTMCLR",   0x00 },
	{ "EXTMCLR",   0x80 },
};

static struct gp_cfg_directive pic18f46k22_300005[] =
{ /* for 0x300005 */
	{ "CCP2MX",    0x01,  2, pic18f46k22_ccp2mx_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP3MX",    0x04,  2, pic18f46k22_ccp3mx_opts },
	{ "HFOFST",    0x08,  2, pic18f67j93_t1dig_opts },
	{ "T3CMX",     0x10,  2, pic18f46k22_t3cmx_opts },
	{ "P2BMX",     0x20,  2, pic18f46k22_p2bmx_opts },
	{ "MCLRE",     0x80,  2, pic18f46k22_mclre_opts },
};

static struct gp_cfg_directive pic18f46k22_30000b[] =
{ /* for 0x30000b */
	{ "WRTC",      0x20,  2, pic18f2525_wrtc_opts },
	{ "WRTB",      0x40,  2, pic18f2525_cpb_opts },
	{ "WRTD",      0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F46K22: */
static struct gp_cfg_addr gp_cfg_pic18f46k22_addrs[] =
{
	{ 0x300001, 0x25,  5, pic18f46k22_300001 },
	{ 0x300002, 0x1f,  3, pic18f46k22_300002 },
	{ 0x300003, 0x3f,  2, pic18f46k22_300003 },
	{ 0x300005, 0xbf,  7, pic18f46k22_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F65J50
 */

static struct gp_cfg_option pic18f65j50_plldiv_opts[] =
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

static struct gp_cfg_directive pic18f65j50_007ff8[] =
{ /* for 0x007ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "PLLDIV",    0x0e,  8, pic18f65j50_plldiv_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_option pic18f65j50_cpudiv_opts[] =
{ /* for CPUDIV: */
	{ "OSC4_PLL6", 0x00 },
	{ "OSC3_PLL3", 0x01 },
	{ "OSC2_PLL2", 0x02 },
	{ "OSC1",      0x03 },
};

static struct gp_cfg_directive pic18f65j50_007ff9[] =
{ /* for 0x007ff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
	{ "CPUDIV",    0x03,  4, pic18f65j50_cpudiv_opts },
};

static struct gp_cfg_option pic18f65j50_fosc_opts[] =
{ /* for FOSC: */
	{ "INTOSC",    0x00 },
	{ "INTOSCO",   0x01 },
	{ "INTOSCPLL", 0x02 },
	{ "INTOSCPLLO", 0x03 },
	{ "HS",        0x04 },
	{ "HSPLL",     0x05 },
	{ "EC",        0x06 },
	{ "ECPLL",     0x07 },
};

static struct gp_cfg_directive pic18f65j50_007ffa[] =
{ /* for 0x007ffa */
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "FOSC",      0x07,  8, pic18f65j50_fosc_opts },
};

static struct gp_cfg_directive pic18f65j50_007ffb[] =
{ /* for 0x007ffb */
	{ "WDTPS",     0x0f, 16, pic18f67j93_wdtps_opts },
};

static struct gp_cfg_option pic18f65j50_msspmsk_opts[] =
{ /* for MSSPMSK: */
	{ "MSK5",      0x00 },
	{ "MSK7",      0x08 },
};

static struct gp_cfg_directive pic18f65j50_007ffd[] =
{ /* for 0x007ffd */
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F65J50: */
static struct gp_cfg_addr gp_cfg_pic18f65j50_addrs[] =
{
	{ 0x007ff8, 0xef,  5, pic18f65j50_007ff8 },
	{ 0x007ff9, 0xf7,  2, pic18f65j50_007ff9 },
	{ 0x007ffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f65j50_007ffb },
	{ 0x007ffc, 0xf8,  0, NULL },
	{ 0x007ffd, 0xff,  2, pic18f65j50_007ffd },
};

/*
 * PIC18F8585
 */

static struct gp_cfg_option pic18f8585_osc_opts[] =
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

static struct gp_cfg_directive pic18f8585_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 11, pic18f8585_osc_opts },
	{ "OSCS",      0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_option pic18f8585_borv_opts[] =
{ /* for BORV: */
	{ "45",        0x00 },
	{ "42",        0x04 },
	{ "27",        0x08 },
	{ "20",        0x0c },
};

static struct gp_cfg_directive pic18f8585_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x02,  2, pic18f2525_pbaden_opts },
	{ "BORV",      0x0c,  4, pic18f8585_borv_opts },
};

static struct gp_cfg_option pic18f8585_mode_opts[] =
{ /* for MODE: */
	{ "EM",        0x00 },
	{ "MPB",       0x01 },
	{ "MP",        0x02 },
	{ "MC",        0x03 },
};

static struct gp_cfg_directive pic18f8585_300004[] =
{ /* for 0x300004 */
	{ "MODE",      0x03,  4, pic18f8585_mode_opts },
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_option pic18f8585_eccpmx_opts[] =
{ /* for ECCPMX: */
	{ "PORTH",     0x00 },
	{ "PORTE",     0x02 },
};

static struct gp_cfg_directive pic18f8585_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "ECCPMX",    0x02,  2, pic18f8585_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f8585_300006[] =
{ /* for 0x300006 */
	{ "STVR",      0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F8585: */
static struct gp_cfg_addr gp_cfg_pic18f8585_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8585_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  2, pic18f8585_300004 },
	{ 0x300005, 0x83,  3, pic18f8585_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4221
 */

static struct gp_cfg_option pic18f4221_osc_opts[] =
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

static struct gp_cfg_directive pic18f4221_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 10, pic18f4221_osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f4221_bor_opts[] =
{ /* for BOR: */
	{ "OFF",       0x00 },
	{ "SOFT",      0x02 },
	{ "NOSLP",     0x04 },
	{ "ON",        0x06 },
};

static struct gp_cfg_directive pic18f4221_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x06,  4, pic18f4221_bor_opts },
	{ "BORV",      0x18,  4, pic18f6410_borv_opts },
};

static struct gp_cfg_option pic18f4221_pbaden_opts[] =
{ /* for PBADEN: */
	{ "DIG",       0x00 },
	{ "ANA",       0x02 },
};

static struct gp_cfg_option pic18f4221_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "RB3",       0x00 },
	{ "RC1",       0x01 },
};

static struct gp_cfg_directive pic18f4221_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "PBADEN",    0x02,  2, pic18f4221_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f4221_ccp2mx_opts },
};

static struct gp_cfg_option pic18f4221_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "BB256",     0x00 },
	{ "BB512",     0x30 },
};

static struct gp_cfg_directive pic18f4221_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ICPORT",    0x08,  2, pic18f67j93_t1dig_opts },
	{ "BBSIZ",     0x30,  2, pic18f4221_bbsiz_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F4221: */
static struct gp_cfg_addr gp_cfg_pic18f4221_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4221_300001 },
	{ 0x300002, 0x1f,  3, pic18f4221_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f4221_300005 },
	{ 0x300006, 0x85,  6, pic18f4221_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4220
 */

static struct gp_cfg_option pic18f4220_osc_opts[] =
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
	{ "RC",        0x0f },
};

static struct gp_cfg_directive pic18f4220_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 10, pic18f4220_osc_opts },
	{ "FSCM",      0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f4220_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "B3",        0x00 },
	{ "OFF",       0x00 },
	{ "C1",        0x01 },
	{ "ON",        0x01 },
};

static struct gp_cfg_directive pic18f4220_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "PBAD",      0x02,  2, pic18f4221_pbaden_opts },
	{ "CCP2MX",    0x01,  4, pic18f4220_ccp2mx_opts },
};

static struct gp_cfg_directive pic18f4220_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
};

static struct gp_cfg_directive pic18f4220_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
};

static struct gp_cfg_directive pic18f4220_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
};

/* config addresses for PIC18F4220: */
static struct gp_cfg_addr gp_cfg_pic18f4220_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f4220_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  3, pic18f4220_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  2, pic18f4220_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f4220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f4220_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F66J93
 */

static struct gp_cfg_directive pic18f66j93_00fff8[] =
{ /* for 0x00fff8 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

static struct gp_cfg_directive pic18f66j93_00fff9[] =
{ /* for 0x00fff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f66j93_00fffa[] =
{ /* for 0x00fffa */
	{ "OSC",       0x07,  8, pic18f67j93_osc_opts },
	{ "T1DIG",     0x08,  2, pic18f67j93_t1dig_opts },
	{ "LPT1OSC",   0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_directive pic18f66j93_00fffb[] =
{ /* for 0x00fffb */
	{ "WDTPS",     0x0f, 16, pic18f67j93_wdtps_opts },
};

static struct gp_cfg_directive pic18f66j93_00fffc[] =
{ /* for 0x00fffc */
	{ "RTCSOSC",   0x02,  2, pic18f67j93_rtcsosc_opts },
};

static struct gp_cfg_directive pic18f66j93_00fffd[] =
{ /* for 0x00fffd */
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F66J93: */
static struct gp_cfg_addr gp_cfg_pic18f66j93_addrs[] =
{
	{ 0x00fff8, 0xe1,  3, pic18f66j93_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j93_00fff9 },
	{ 0x00fffa, 0xdf,  5, pic18f66j93_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0x02,  1, pic18f66j93_00fffc },
	{ 0x00fffd, 0xf1,  1, pic18f66j93_00fffd },
};

/*
 * PIC18F6490
 */

static struct gp_cfg_directive pic18f6490_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "CCP2MX",    0x01,  2, pic18f2525_ccp2mx_opts },
};

/* config addresses for PIC18F6490: */
static struct gp_cfg_addr gp_cfg_pic18f6490_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f6410_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x81,  3, pic18f6490_300005 },
	{ 0x300006, 0x81,  3, pic18f6410_300006 },
	{ 0x300008, 0x01,  1, pic18f6410_300008 },
};

/*
 * PIC18F2520
 */

/* config addresses for PIC18F2520: */
static struct gp_cfg_addr gp_cfg_pic18f2520_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2525_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F26K22
 */

static struct gp_cfg_option pic18f26k22_ccp3mx_opts[] =
{ /* for CCP3MX: */
	{ "PORTC6",    0x00 },
	{ "PORTB5",    0x04 },
};

static struct gp_cfg_option pic18f26k22_p2bmx_opts[] =
{ /* for P2BMX: */
	{ "PORTC0",    0x00 },
	{ "PORTB5",    0x20 },
};

static struct gp_cfg_directive pic18f26k22_300005[] =
{ /* for 0x300005 */
	{ "CCP2MX",    0x01,  2, pic18f46k22_ccp2mx_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP3MX",    0x04,  2, pic18f26k22_ccp3mx_opts },
	{ "HFOFST",    0x08,  2, pic18f67j93_t1dig_opts },
	{ "T3CMX",     0x10,  2, pic18f46k22_t3cmx_opts },
	{ "P2BMX",     0x20,  2, pic18f26k22_p2bmx_opts },
	{ "MCLRE",     0x80,  2, pic18f46k22_mclre_opts },
};

/* config addresses for PIC18F26K22: */
static struct gp_cfg_addr gp_cfg_pic18f26k22_addrs[] =
{
	{ 0x300001, 0x25,  5, pic18f46k22_300001 },
	{ 0x300002, 0x1f,  3, pic18f46k22_300002 },
	{ 0x300003, 0x3f,  2, pic18f46k22_300003 },
	{ 0x300005, 0xbf,  7, pic18f26k22_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8520
 */

static struct gp_cfg_directive pic18f8520_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x07,  8, pic18c858_osc_opts },
	{ "OSCS",      0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_option pic18f8520_ccp2mux_opts[] =
{ /* for CCP2MUX: */
	{ "OFF",       0x00 },
	{ "RE7",       0x00 },
	{ "ON",        0x01 },
	{ "RC1",       0x01 },
};

static struct gp_cfg_directive pic18f8520_300005[] =
{ /* for 0x300005 */
	{ "CCP2MUX",   0x01,  4, pic18f8520_ccp2mux_opts },
};

static struct gp_cfg_directive pic18f8520_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
	{ "CP3",       0x08,  2, pic18f2523_cp3_opts },
};

static struct gp_cfg_directive pic18f8520_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
	{ "WRT3",      0x08,  2, pic18f2523_cp3_opts },
};

static struct gp_cfg_directive pic18f8520_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBTR3",     0x08,  2, pic18f2523_cp3_opts },
};

/* config addresses for PIC18F8520: */
static struct gp_cfg_addr gp_cfg_pic18f8520_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18f8520_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300004, 0x83,  2, pic18f8585_300004 },
	{ 0x300005, 0x03,  1, pic18f8520_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0xff,  4, pic18f8520_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  4, pic18f8520_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  4, pic18f8520_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F242
 */

static struct gp_cfg_directive pic18f242_300005[] =
{ /* for 0x300005 */
	{ "CCP2MUX",   0x01,  2, pic18f6410_wdt_opts },
};

/* config addresses for PIC18F242: */
static struct gp_cfg_addr gp_cfg_pic18f242_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18f8520_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  2, pic18f4220_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f4220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f4220_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18LF45J50
 */

static struct gp_cfg_directive pic18lf45j50_007ff8[] =
{ /* for 0x007ff8 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "PLLDIV",    0x0e,  8, pic18f65j50_plldiv_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

static struct gp_cfg_directive pic18lf45j50_007ff9[] =
{ /* for 0x007ff9 */
	{ "CPUDIV",    0x03,  4, pic18f65j50_cpudiv_opts },
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18lf45j50_007ffa[] =
{ /* for 0x007ffa */
	{ "OSC",       0x07,  8, pic18f65j50_fosc_opts },
	{ "T1DIG",     0x08,  2, pic18f67j93_t1dig_opts },
	{ "LPT1OSC",   0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18lf45j50_dswdtosc_opts[] =
{ /* for DSWDTOSC: */
	{ "T1OSCREF",  0x00 },
	{ "INTOSCREF", 0x01 },
};

static struct gp_cfg_option pic18lf45j50_dswdtps_opts[] =
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

static struct gp_cfg_directive pic18lf45j50_007ffc[] =
{ /* for 0x007ffc */
	{ "DSWDTOSC",  0x01,  2, pic18lf45j50_dswdtosc_opts },
	{ "RTCOSC",    0x02,  2, pic18f67j93_rtcsosc_opts },
	{ "DSBOREN",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "DSWDTEN",   0x08,  2, pic18f67j93_t1dig_opts },
	{ "DSWDTPS",   0xf0, 16, pic18lf45j50_dswdtps_opts },
};

static struct gp_cfg_directive pic18lf45j50_007ffd[] =
{ /* for 0x007ffd */
	{ "IOL1WAY",   0x01,  2, pic18f6410_wdt_opts },
	{ "MSSP7B_EN", 0x08,  2, pic18f65j50_msspmsk_opts },
};

static struct gp_cfg_option pic18lf45j50_wpfp_opts[] =
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

static struct gp_cfg_option pic18lf45j50_wpend_opts[] =
{ /* for WPEND: */
	{ "PAGE_0",    0x00 },
	{ "PAGE_WPFP", 0x40 },
};

static struct gp_cfg_directive pic18lf45j50_007ffe[] =
{ /* for 0x007ffe */
	{ "WPFP",      0x1f, 32, pic18lf45j50_wpfp_opts },
	{ "WPEND",     0x40,  2, pic18lf45j50_wpend_opts },
	{ "WPCFG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18lf45j50_007fff[] =
{ /* for 0x007fff */
	{ "WPDIS",     0x01,  2, pic18f6410_pwrt_opts },
};

/* config addresses for PIC18LF45J50: */
static struct gp_cfg_addr gp_cfg_pic18lf45j50_addrs[] =
{
	{ 0x007ff8, 0xef,  4, pic18lf45j50_007ff8 },
	{ 0x007ff9, 0xf7,  2, pic18lf45j50_007ff9 },
	{ 0x007ffa, 0xdf,  5, pic18lf45j50_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f65j50_007ffb },
	{ 0x007ffc, 0xff,  5, pic18lf45j50_007ffc },
	{ 0x007ffd, 0xf9,  2, pic18lf45j50_007ffd },
	{ 0x007ffe, 0xdf,  3, pic18lf45j50_007ffe },
	{ 0x007fff, 0xf1,  1, pic18lf45j50_007fff },
};

/*
 * PIC18F26J53
 */

static struct gp_cfg_directive pic18f26j53_00fff8[] =
{ /* for 0x00fff8 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "PLLDIV",    0x0e,  8, pic18f65j50_plldiv_opts },
	{ "CFGPLLEN",  0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

static struct gp_cfg_directive pic18f26j53_00fff9[] =
{ /* for 0x00fff9 */
	{ "CPUDIV",    0x03,  4, pic18f65j50_cpudiv_opts },
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_option pic18f26j53_soscsel_opts[] =
{ /* for SOSCSEL: */
	{ "RESERVED",  0x00 },
	{ "LOW",       0x08 },
	{ "DIG",       0x10 },
	{ "HIGH",      0x18 },
};

static struct gp_cfg_directive pic18f26j53_00fffa[] =
{ /* for 0x00fffa */
	{ "OSC",       0x07,  8, pic18f65j50_fosc_opts },
	{ "SOSCSEL",   0x18,  4, pic18f26j53_soscsel_opts },
	{ "CLKOEC",    0x20,  2, pic18f4450_vregen_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_directive pic18f26j53_00fffc[] =
{ /* for 0x00fffc */
	{ "DSWDTOSC",  0x01,  2, pic18lf45j50_dswdtosc_opts },
	{ "RTCOSC",    0x02,  2, pic18f67j93_rtcsosc_opts },
	{ "DSBOREN",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "DSWDTEN",   0x08,  2, pic18f67j93_t1dig_opts },
	{ "DSWDTPS",   0xf0, 16, pic18lf45j50_dswdtps_opts },
};

static struct gp_cfg_option pic18f26j53_adcsel_opts[] =
{ /* for ADCSEL: */
	{ "BIT12",     0x00 },
	{ "BIT10",     0x02 },
};

static struct gp_cfg_directive pic18f26j53_00fffd[] =
{ /* for 0x00fffd */
	{ "IOL1WAY",   0x01,  2, pic18f6410_wdt_opts },
	{ "ADCSEL",    0x02,  2, pic18f26j53_adcsel_opts },
	{ "MSSP7B_EN", 0x08,  2, pic18f65j50_msspmsk_opts },
};

static struct gp_cfg_option pic18f26j53_wpfp_opts[] =
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

static struct gp_cfg_directive pic18f26j53_00fffe[] =
{ /* for 0x00fffe */
	{ "WPFP",      0x3f, 64, pic18f26j53_wpfp_opts },
	{ "WPCFG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_option pic18f26j53_wpend_opts[] =
{ /* for WPEND: */
	{ "PAGE_0",    0x00 },
	{ "PAGE_WPFP", 0x02 },
};

static struct gp_cfg_option pic18f26j53_ls48mhz_opts[] =
{ /* for LS48MHZ: */
	{ "SYS24X4",   0x00 },
	{ "SYS48X8",   0x08 },
};

static struct gp_cfg_directive pic18f26j53_00ffff[] =
{ /* for 0x00ffff */
	{ "WPDIS",     0x01,  2, pic18f6410_pwrt_opts },
	{ "WPEND",     0x02,  2, pic18f26j53_wpend_opts },
	{ "LS48MHZ",   0x08,  2, pic18f26j53_ls48mhz_opts },
};

/* config addresses for PIC18F26J53: */
static struct gp_cfg_addr gp_cfg_pic18f26j53_addrs[] =
{
	{ 0x00fff8, 0xff,  5, pic18f26j53_00fff8 },
	{ 0x00fff9, 0xf7,  2, pic18f26j53_00fff9 },
	{ 0x00fffa, 0xff,  5, pic18f26j53_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xff,  5, pic18f26j53_00fffc },
	{ 0x00fffd, 0xfb,  3, pic18f26j53_00fffd },
	{ 0x00fffe, 0xbf,  2, pic18f26j53_00fffe },
	{ 0x00ffff, 0xfb,  3, pic18f26j53_00ffff },
};

/*
 * PIC18F8722
 */

static struct gp_cfg_option pic18f8722_addrbw_opts[] =
{ /* for ADDRBW: */
	{ "ADDR8BIT",  0x00 },
	{ "ADDR12BIT", 0x10 },
	{ "ADDR16BIT", 0x20 },
	{ "ADDR20BIT", 0x30 },
};

static struct gp_cfg_option pic18f8722_databw_opts[] =
{ /* for DATABW: */
	{ "DATA8BIT",  0x00 },
	{ "DATA16BIT", 0x40 },
};

static struct gp_cfg_directive pic18f8722_300004[] =
{ /* for 0x300004 */
	{ "MODE",      0x03,  4, pic18f8585_mode_opts },
	{ "ADDRBW",    0x30,  4, pic18f8722_addrbw_opts },
	{ "DATABW",    0x40,  2, pic18f8722_databw_opts },
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f8722_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ECCPMX",    0x02,  2, pic18f8585_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f2525_ccp2mx_opts },
};

static struct gp_cfg_option pic18f8722_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "BB2K",      0x00 },
	{ "BB4K",      0x10 },
	{ "BB8K",      0x30 },
};

static struct gp_cfg_directive pic18f8722_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "BBSIZ",     0x30,  3, pic18f8722_bbsiz_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f8722_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
	{ "CP3",       0x08,  2, pic18f2523_cp3_opts },
	{ "CP4",       0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "CP5",       0x20,  2, pic18f2525_wrtc_opts },
	{ "CP6",       0x40,  2, pic18f2525_cpb_opts },
	{ "CP7",       0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f8722_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
	{ "WRT3",      0x08,  2, pic18f2523_cp3_opts },
	{ "WRT4",      0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "WRT5",      0x20,  2, pic18f2525_wrtc_opts },
	{ "WRT6",      0x40,  2, pic18f2525_cpb_opts },
	{ "WRT7",      0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f8722_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBTR3",     0x08,  2, pic18f2523_cp3_opts },
	{ "EBTR4",     0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "EBTR5",     0x20,  2, pic18f2525_wrtc_opts },
	{ "EBTR6",     0x40,  2, pic18f2525_cpb_opts },
	{ "EBTR7",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F8722: */
static struct gp_cfg_addr gp_cfg_pic18f8722_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xf3,  4, pic18f8722_300004 },
	{ 0x300005, 0x83,  4, pic18f8722_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0xff,  8, pic18f8722_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8722_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  8, pic18f8722_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F26J50
 */

static struct gp_cfg_directive pic18f26j50_00fff8[] =
{ /* for 0x00fff8 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "PLLDIV",    0x0e,  8, pic18f65j50_plldiv_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

static struct gp_cfg_directive pic18f26j50_00fffa[] =
{ /* for 0x00fffa */
	{ "OSC",       0x07,  8, pic18f65j50_fosc_opts },
	{ "T1DIG",     0x08,  2, pic18f67j93_t1dig_opts },
	{ "LPT1OSC",   0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_directive pic18f26j50_00fffd[] =
{ /* for 0x00fffd */
	{ "IOL1WAY",   0x01,  2, pic18f6410_wdt_opts },
	{ "MSSP7B_EN", 0x08,  2, pic18f65j50_msspmsk_opts },
};

static struct gp_cfg_directive pic18f26j50_00fffe[] =
{ /* for 0x00fffe */
	{ "WPFP",      0x3f, 64, pic18f26j53_wpfp_opts },
	{ "WPEND",     0x40,  2, pic18lf45j50_wpend_opts },
	{ "WPCFG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f26j50_00ffff[] =
{ /* for 0x00ffff */
	{ "WPDIS",     0x01,  2, pic18f6410_pwrt_opts },
};

/* config addresses for PIC18F26J50: */
static struct gp_cfg_addr gp_cfg_pic18f26j50_addrs[] =
{
	{ 0x00fff8, 0xef,  4, pic18f26j50_00fff8 },
	{ 0x00fff9, 0xf7,  2, pic18f26j53_00fff9 },
	{ 0x00fffa, 0xdf,  5, pic18f26j50_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xff,  5, pic18f26j53_00fffc },
	{ 0x00fffd, 0xf9,  2, pic18f26j50_00fffd },
	{ 0x00fffe, 0xff,  3, pic18f26j50_00fffe },
	{ 0x00ffff, 0xf1,  1, pic18f26j50_00ffff },
};

/*
 * PIC18F83J11
 */

static struct gp_cfg_directive pic18f83j11_001ff8[] =
{ /* for 0x001ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f83j11_001ff9[] =
{ /* for 0x001ff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_option pic18f83j11_fosc_opts[] =
{ /* for FOSC: */
	{ "HS",        0x00 },
	{ "HSPLL",     0x01 },
	{ "EC",        0x02 },
	{ "ECPLL",     0x03 },
};

static struct gp_cfg_directive pic18f83j11_001ffa[] =
{ /* for 0x001ffa */
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f83j11_fosc_opts },
};

static struct gp_cfg_directive pic18f83j11_001ffb[] =
{ /* for 0x001ffb */
	{ "WDTPS",     0x0f, 16, pic18f67j93_wdtps_opts },
};

static struct gp_cfg_option pic18f83j11_bw_opts[] =
{ /* for BW: */
	{ "8",         0x00 },
	{ "16",        0x40 },
};

static struct gp_cfg_option pic18f83j11_mode_opts[] =
{ /* for MODE: */
	{ "XM20",      0x00 },
	{ "XM16",      0x10 },
	{ "XM12",      0x20 },
	{ "MM",        0x30 },
};

static struct gp_cfg_directive pic18f83j11_001ffc[] =
{ /* for 0x001ffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f83j11_bw_opts },
	{ "MODE",      0x30,  4, pic18f83j11_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f67j93_t1dig_opts },
};

static struct gp_cfg_directive pic18f83j11_001ffd[] =
{ /* for 0x001ffd */
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F83J11: */
static struct gp_cfg_addr gp_cfg_pic18f83j11_addrs[] =
{
	{ 0x001ff8, 0xe1,  4, pic18f83j11_001ff8 },
	{ 0x001ff9, 0xf4,  1, pic18f83j11_001ff9 },
	{ 0x001ffa, 0xc7,  4, pic18f83j11_001ffa },
	{ 0x001ffb, 0xff,  1, pic18f83j11_001ffb },
	{ 0x001ffc, 0xf8,  4, pic18f83j11_001ffc },
	{ 0x001ffd, 0xf1,  1, pic18f83j11_001ffd },
};

/*
 * PIC18F248
 */

/* config addresses for PIC18F248: */
static struct gp_cfg_addr gp_cfg_pic18f248_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18f8520_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  2, pic18f4220_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f4220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f4220_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4685
 */

static struct gp_cfg_option pic18f4685_osc_opts[] =
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

static struct gp_cfg_directive pic18f4685_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 10, pic18f4685_osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f4685_boren_opts[] =
{ /* for BOREN: */
	{ "OFF",       0x00 },
	{ "SBORENCTRL", 0x02 },
	{ "BOACTIVE",  0x04 },
	{ "BOHW",      0x06 },
};

static struct gp_cfg_directive pic18f4685_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOREN",     0x06,  4, pic18f4685_boren_opts },
	{ "BORV",      0x18,  4, pic18f6410_borv_opts },
};

static struct gp_cfg_option pic18f4685_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "1024",      0x00 },
	{ "2048",      0x10 },
	{ "4096",      0x20 },
};

static struct gp_cfg_directive pic18f4685_300006[] =
{ /* for 0x300006 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "BBSIZ",     0x30,  3, pic18f4685_bbsiz_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f4685_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
	{ "CP3",       0x08,  2, pic18f2523_cp3_opts },
	{ "CP4",       0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "CP5",       0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_directive pic18f4685_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
	{ "WRT3",      0x08,  2, pic18f2523_cp3_opts },
	{ "WRT4",      0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "WRT5",      0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_directive pic18f4685_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBTR3",     0x08,  2, pic18f2523_cp3_opts },
	{ "EBTR4",     0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "EBTR5",     0x20,  2, pic18f2525_wrtc_opts },
};

/* config addresses for PIC18F4685: */
static struct gp_cfg_addr gp_cfg_pic18f4685_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4685_300001 },
	{ 0x300002, 0x1f,  3, pic18f4685_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x82,  3, pic18f4450_300005 },
	{ 0x300006, 0x85,  5, pic18f4685_300006 },
	{ 0x300008, 0x3f,  6, pic18f4685_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x3f,  6, pic18f4685_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x3f,  6, pic18f4685_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18LF27J13
 */

static struct gp_cfg_directive pic18lf27j13_01fff8[] =
{ /* for 0x01fff8 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "PLLDIV",    0x0e,  8, pic18f65j50_plldiv_opts },
	{ "CFGPLLEN",  0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

static struct gp_cfg_directive pic18lf27j13_01fffa[] =
{ /* for 0x01fffa */
	{ "OSC",       0x07,  8, pic18f65j50_fosc_opts },
	{ "SOSCSEL",   0x18,  4, pic18f26j53_soscsel_opts },
	{ "CLKOEC",    0x20,  2, pic18f4450_vregen_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_directive pic18lf27j13_01fffc[] =
{ /* for 0x01fffc */
	{ "DSWDTOSC",  0x01,  2, pic18lf45j50_dswdtosc_opts },
	{ "RTCOSC",    0x02,  2, pic18f67j93_rtcsosc_opts },
	{ "DSBOREN",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "DSWDTEN",   0x08,  2, pic18f67j93_t1dig_opts },
	{ "DSWDTPS",   0xf0, 16, pic18lf45j50_dswdtps_opts },
};

static struct gp_cfg_option pic18lf27j13_pllsel_opts[] =
{ /* for PLLSEL: */
	{ "PLL96",     0x00 },
	{ "PLL4X",     0x04 },
};

static struct gp_cfg_directive pic18lf27j13_01fffd[] =
{ /* for 0x01fffd */
	{ "IOL1WAY",   0x01,  2, pic18f6410_wdt_opts },
	{ "ADCSEL",    0x02,  2, pic18f26j53_adcsel_opts },
	{ "PLLSEL",    0x04,  2, pic18lf27j13_pllsel_opts },
	{ "MSSP7B_EN", 0x08,  2, pic18f65j50_msspmsk_opts },
};

static struct gp_cfg_option pic18lf27j13_wpfp_opts[] =
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

static struct gp_cfg_directive pic18lf27j13_01fffe[] =
{ /* for 0x01fffe */
	{ "WPFP",      0x7f, 128, pic18lf27j13_wpfp_opts },
	{ "WPCFG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18lf27j13_01ffff[] =
{ /* for 0x01ffff */
	{ "WPDIS",     0x01,  2, pic18f6410_pwrt_opts },
	{ "WPEND",     0x02,  2, pic18f26j53_wpend_opts },
};

/* config addresses for PIC18LF27J13: */
static struct gp_cfg_addr gp_cfg_pic18lf27j13_addrs[] =
{
	{ 0x01fff8, 0xff,  5, pic18lf27j13_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j93_01fff9 },
	{ 0x01fffa, 0xff,  5, pic18lf27j13_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffc, 0xff,  5, pic18lf27j13_01fffc },
	{ 0x01fffd, 0xff,  4, pic18lf27j13_01fffd },
	{ 0x01fffe, 0xff,  2, pic18lf27j13_01fffe },
	{ 0x01ffff, 0xf3,  2, pic18lf27j13_01ffff },
};

/*
 * PIC18F2680
 */

/* config addresses for PIC18F2680: */
static struct gp_cfg_addr gp_cfg_pic18f2680_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4685_300001 },
	{ 0x300002, 0x1f,  3, pic18f4685_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x82,  3, pic18f4450_300005 },
	{ 0x300006, 0x85,  5, pic18f4685_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F2320
 */

/* config addresses for PIC18F2320: */
static struct gp_cfg_addr gp_cfg_pic18f2320_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f4220_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  3, pic18f4220_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F2321
 */

static struct gp_cfg_option pic18f2321_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "BB256",     0x00 },
	{ "BB512",     0x10 },
	{ "BB1K",      0x30 },
};

static struct gp_cfg_directive pic18f2321_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "BBSIZ",     0x30,  3, pic18f2321_bbsiz_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F2321: */
static struct gp_cfg_addr gp_cfg_pic18f2321_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4221_300001 },
	{ 0x300002, 0x1f,  3, pic18f4221_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f4221_300005 },
	{ 0x300006, 0x85,  5, pic18f2321_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F2455
 */

/* config addresses for PIC18F2455: */
static struct gp_cfg_addr gp_cfg_pic18f2455_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f4450_300000 },
	{ 0x300001, 0x05,  3, pic18f4450_300001 },
	{ 0x300002, 0x1f,  4, pic18f4458_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f4458_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4420
 */

/* config addresses for PIC18F4420: */
static struct gp_cfg_addr gp_cfg_pic18f4420_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2525_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  2, pic18f4220_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f4220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f4220_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F66J60
 */

static struct gp_cfg_directive pic18f66j60_00fff8[] =
{ /* for 0x00fff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVR",      0x20,  2, pic18f4450_vregen_opts },
	{ "WDT",       0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f66j60_00fffa[] =
{ /* for 0x00fffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f83j11_fosc_opts },
};

static struct gp_cfg_directive pic18f66j60_00fffd[] =
{ /* for 0x00fffd */
	{ "ETHLED",    0x04,  2, pic18f6410_lpt1osc_opts },
};

/* config addresses for PIC18F66J60: */
static struct gp_cfg_addr gp_cfg_pic18f66j60_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j93_00fff9 },
	{ 0x00fffa, 0xc7,  4, pic18f66j60_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffd, 0xf7,  1, pic18f66j60_00fffd },
};

/*
 * PIC18F8410
 */

static struct gp_cfg_directive pic18f8410_300004[] =
{ /* for 0x300004 */
	{ "PM",        0x03,  4, pic18f8585_mode_opts },
	{ "BW",        0x40,  2, pic18f83j11_bw_opts },
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f8410_30000c[] =
{ /* for 0x30000c */
	{ "EBTR",      0x01,  2, pic18f6410_pwrt_opts },
};

/* config addresses for PIC18F8410: */
static struct gp_cfg_addr gp_cfg_pic18f8410_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f6410_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xc3,  3, pic18f8410_300004 },
	{ 0x300005, 0x81,  3, pic18f6490_300005 },
	{ 0x300006, 0x81,  3, pic18f6410_300006 },
	{ 0x300008, 0x01,  1, pic18f6410_300008 },
	{ 0x30000c, 0x01,  1, pic18f8410_30000c },
};

/*
 * PIC18F84J11
 */

static struct gp_cfg_directive pic18f84j11_003ff8[] =
{ /* for 0x003ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f84j11_003ff9[] =
{ /* for 0x003ff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f84j11_003ffa[] =
{ /* for 0x003ffa */
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f83j11_fosc_opts },
};

static struct gp_cfg_directive pic18f84j11_003ffb[] =
{ /* for 0x003ffb */
	{ "WDTPS",     0x0f, 16, pic18f67j93_wdtps_opts },
};

static struct gp_cfg_directive pic18f84j11_003ffc[] =
{ /* for 0x003ffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f83j11_bw_opts },
	{ "MODE",      0x30,  4, pic18f83j11_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f67j93_t1dig_opts },
};

static struct gp_cfg_directive pic18f84j11_003ffd[] =
{ /* for 0x003ffd */
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F84J11: */
static struct gp_cfg_addr gp_cfg_pic18f84j11_addrs[] =
{
	{ 0x003ff8, 0xe1,  4, pic18f84j11_003ff8 },
	{ 0x003ff9, 0xf4,  1, pic18f84j11_003ff9 },
	{ 0x003ffa, 0xc7,  4, pic18f84j11_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f84j11_003ffb },
	{ 0x003ffc, 0xf8,  4, pic18f84j11_003ffc },
	{ 0x003ffd, 0xf1,  1, pic18f84j11_003ffd },
};

/*
 * PIC18F66J65
 */

static struct gp_cfg_directive pic18f66j65_017ff8[] =
{ /* for 0x017ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVR",      0x20,  2, pic18f4450_vregen_opts },
	{ "WDT",       0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f66j65_017ff9[] =
{ /* for 0x017ff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f66j65_017ffa[] =
{ /* for 0x017ffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f83j11_fosc_opts },
};

static struct gp_cfg_directive pic18f66j65_017ffb[] =
{ /* for 0x017ffb */
	{ "WDTPS",     0x0f, 16, pic18f67j93_wdtps_opts },
};

static struct gp_cfg_directive pic18f66j65_017ffd[] =
{ /* for 0x017ffd */
	{ "ETHLED",    0x04,  2, pic18f6410_lpt1osc_opts },
};

/* config addresses for PIC18F66J65: */
static struct gp_cfg_addr gp_cfg_pic18f66j65_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j65_017ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f66j65_017ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f66j65_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffd, 0xf7,  1, pic18f66j65_017ffd },
};

/*
 * PIC18F85J50
 */

static struct gp_cfg_directive pic18f85j50_007ffc[] =
{ /* for 0x007ffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f83j11_bw_opts },
	{ "MODE",      0x30,  4, pic18f83j11_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f67j93_t1dig_opts },
};

static struct gp_cfg_option pic18f85j50_pmpmx_opts[] =
{ /* for PMPMX: */
	{ "ALTERNATE", 0x00 },
	{ "DEFAULT",   0x04 },
};

static struct gp_cfg_option pic18f85j50_eccpmx_opts[] =
{ /* for ECCPMX: */
	{ "ALTERNATE", 0x00 },
	{ "DEFAULT",   0x02 },
};

static struct gp_cfg_directive pic18f85j50_007ffd[] =
{ /* for 0x007ffd */
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "PMPMX",     0x04,  2, pic18f85j50_pmpmx_opts },
	{ "ECCPMX",    0x02,  2, pic18f85j50_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F85J50: */
static struct gp_cfg_addr gp_cfg_pic18f85j50_addrs[] =
{
	{ 0x007ff8, 0xef,  5, pic18f65j50_007ff8 },
	{ 0x007ff9, 0xf7,  2, pic18f65j50_007ff9 },
	{ 0x007ffa, 0xc7,  3, pic18f65j50_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f65j50_007ffb },
	{ 0x007ffc, 0xf8,  4, pic18f85j50_007ffc },
	{ 0x007ffd, 0xff,  4, pic18f85j50_007ffd },
};

/*
 * PIC18F66K22
 */

static struct gp_cfg_option pic18f66k22_intoscsel_opts[] =
{ /* for INTOSCSEL: */
	{ "LOW",       0x00 },
	{ "HIGH",      0x04 },
};

static struct gp_cfg_option pic18f66k22_soscsel_opts[] =
{ /* for SOSCSEL: */
	{ "LOW",       0x08 },
	{ "DIG",       0x10 },
	{ "HIGH",      0x18 },
};

static struct gp_cfg_directive pic18f66k22_300000[] =
{ /* for 0x300000 */
	{ "RETEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "INTOSCSEL", 0x04,  2, pic18f66k22_intoscsel_opts },
	{ "SOSCSEL",   0x18,  3, pic18f66k22_soscsel_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

static struct gp_cfg_option pic18f66k22_fosc_opts[] =
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

static struct gp_cfg_directive pic18f66k22_300001[] =
{ /* for 0x300001 */
	{ "FOSC",      0x0f, 14, pic18f66k22_fosc_opts },
	{ "PLLCFG",    0x10,  2, pic18f46k22_pllcfg_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f66k22_borpwr_opts[] =
{ /* for BORPWR: */
	{ "LOW",       0x00 },
	{ "MEDIUM",    0x20 },
	{ "HIGH",      0x40 },
	{ "ZPBORMV",   0x60 },
};

static struct gp_cfg_directive pic18f66k22_300002[] =
{ /* for 0x300002 */
	{ "PWRTEN",    0x01,  2, pic18f6410_pwrt_opts },
	{ "BOREN",     0x06,  4, pic18f6410_boren_opts },
	{ "BORV",      0x18,  4, pic18f6410_borv_opts },
	{ "BORPWR",    0x60,  4, pic18f66k22_borpwr_opts },
};

static struct gp_cfg_option pic18f66k22_wdten_opts[] =
{ /* for WDTEN: */
	{ "OFF",       0x00 },
	{ "NOSLP",     0x01 },
	{ "ON",        0x02 },
	{ "SWDTDIS",   0x03 },
};

static struct gp_cfg_option pic18f66k22_wdtps_opts[] =
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

static struct gp_cfg_directive pic18f66k22_300003[] =
{ /* for 0x300003 */
	{ "WDTEN",     0x03,  4, pic18f66k22_wdten_opts },
	{ "WDTPS",     0x7c, 21, pic18f66k22_wdtps_opts },
};

static struct gp_cfg_option pic18f66k22_rtcosc_opts[] =
{ /* for RTCOSC: */
	{ "INTOSCREF", 0x00 },
	{ "T1OSCREF",  0x01 },
};

static struct gp_cfg_directive pic18f66k22_300004[] =
{ /* for 0x300004 */
	{ "RTCOSC",    0x01,  2, pic18f66k22_rtcosc_opts },
};

static struct gp_cfg_directive pic18f66k22_300005[] =
{ /* for 0x300005 */
	{ "CCP2MX",    0x01,  2, pic18f2525_ccp2mx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f66k22_bbsiz0_opts[] =
{ /* for BBSIZ0: */
	{ "BB1K",      0x00 },
	{ "BB2K",      0x10 },
};

static struct gp_cfg_directive pic18f66k22_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "BBSIZ0",    0x10,  2, pic18f66k22_bbsiz0_opts },
};

static struct gp_cfg_directive pic18f66k22_30000c[] =
{ /* for 0x30000c */
	{ "EBRT0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBRT1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBRT2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBRT3",     0x08,  2, pic18f2523_cp3_opts },
};

static struct gp_cfg_directive pic18f66k22_30000d[] =
{ /* for 0x30000d */
	{ "EBRTB",     0x40,  2, pic18f2525_cpb_opts },
};

/* config addresses for PIC18F66K22: */
static struct gp_cfg_addr gp_cfg_pic18f66k22_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f66k22_300000 },
	{ 0x300001, 0x08,  4, pic18f66k22_300001 },
	{ 0x300002, 0x7f,  4, pic18f66k22_300002 },
	{ 0x300003, 0x7f,  2, pic18f66k22_300003 },
	{ 0x300004, 0x01,  1, pic18f66k22_300004 },
	{ 0x300005, 0x89,  3, pic18f66k22_300005 },
	{ 0x300006, 0x91,  2, pic18f66k22_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x0f,  4, pic18f66k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f66k22_30000d },
};

/*
 * PIC18F86J55
 */

static struct gp_cfg_directive pic18f86j55_017ff8[] =
{ /* for 0x017ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "PLLDIV",    0x0e,  8, pic18f65j50_plldiv_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f86j55_017ff9[] =
{ /* for 0x017ff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
	{ "CPUDIV",    0x03,  4, pic18f65j50_cpudiv_opts },
};

static struct gp_cfg_directive pic18f86j55_017ffa[] =
{ /* for 0x017ffa */
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "FOSC",      0x07,  8, pic18f65j50_fosc_opts },
};

static struct gp_cfg_directive pic18f86j55_017ffc[] =
{ /* for 0x017ffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f83j11_bw_opts },
	{ "MODE",      0x30,  4, pic18f83j11_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f67j93_t1dig_opts },
};

static struct gp_cfg_directive pic18f86j55_017ffd[] =
{ /* for 0x017ffd */
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "PMPMX",     0x04,  2, pic18f85j50_pmpmx_opts },
	{ "ECCPMX",    0x02,  2, pic18f85j50_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F86J55: */
static struct gp_cfg_addr gp_cfg_pic18f86j55_addrs[] =
{
	{ 0x017ff8, 0xef,  5, pic18f86j55_017ff8 },
	{ 0x017ff9, 0xf7,  2, pic18f86j55_017ff9 },
	{ 0x017ffa, 0xc7,  3, pic18f86j55_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffc, 0xf8,  4, pic18f86j55_017ffc },
	{ 0x017ffd, 0xff,  4, pic18f86j55_017ffd },
};

/*
 * PIC18LF14K22
 */

static struct gp_cfg_option pic18lf14k22_fosc_opts[] =
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

static struct gp_cfg_directive pic18lf14k22_300001[] =
{ /* for 0x300001 */
	{ "FOSC",      0x0f, 13, pic18lf14k22_fosc_opts },
	{ "PLLEN",     0x10,  2, pic18f46k22_pllcfg_opts },
	{ "PCLKEN",    0x20,  2, pic18f4450_vregen_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18lf14k22_borv_opts[] =
{ /* for BORV: */
	{ "30",        0x00 },
	{ "27",        0x08 },
	{ "22",        0x10 },
	{ "19",        0x18 },
};

static struct gp_cfg_directive pic18lf14k22_300002[] =
{ /* for 0x300002 */
	{ "PWRTEN",    0x01,  2, pic18f6410_pwrt_opts },
	{ "BOREN",     0x06,  4, pic18f6410_boren_opts },
	{ "BORV",      0x18,  4, pic18lf14k22_borv_opts },
};

static struct gp_cfg_directive pic18lf14k22_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "HFOFST",    0x08,  2, pic18f67j93_t1dig_opts },
};

static struct gp_cfg_directive pic18lf14k22_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "BBSIZ",     0x08,  2, pic18f67j93_t1dig_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

/* config addresses for PIC18LF14K22: */
static struct gp_cfg_addr gp_cfg_pic18lf14k22_addrs[] =
{
	{ 0x300001, 0x27,  5, pic18lf14k22_300001 },
	{ 0x300002, 0x1f,  3, pic18lf14k22_300002 },
	{ 0x300003, 0x1f,  2, pic18f46k20_300003 },
	{ 0x300005, 0x88,  2, pic18lf14k22_300005 },
	{ 0x300006, 0x05,  4, pic18lf14k22_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F85K90
 */

static struct gp_cfg_directive pic18f85k90_300005[] =
{ /* for 0x300005 */
	{ "CCP2MX",    0x01,  2, pic18f2525_ccp2mx_opts },
	{ "ECCPMX",    0x02,  2, pic18f8585_eccpmx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
};

/* config addresses for PIC18F85K90: */
static struct gp_cfg_addr gp_cfg_pic18f85k90_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f66k22_300000 },
	{ 0x300001, 0x08,  4, pic18f66k22_300001 },
	{ 0x300002, 0x7f,  4, pic18f66k22_300002 },
	{ 0x300003, 0x7f,  2, pic18f66k22_300003 },
	{ 0x300004, 0x01,  1, pic18f66k22_300004 },
	{ 0x300005, 0x8b,  4, pic18f85k90_300005 },
	{ 0x300006, 0x91,  2, pic18f66k22_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x0f,  4, pic18f66k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f66k22_30000d },
};

/*
 * PIC18F2439
 */

static struct gp_cfg_directive pic18f2439_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x07,  8, pic18c858_osc_opts },
};

/* config addresses for PIC18F2439: */
static struct gp_cfg_addr gp_cfg_pic18f2439_addrs[] =
{
	{ 0x300001, 0x22,  1, pic18f2439_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  2, pic18f4220_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f4220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f4220_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18LF66K80
 */

static struct gp_cfg_option pic18lf66k80_canmx_opts[] =
{ /* for CANMX: */
	{ "PORTE",     0x00 },
	{ "PORTB",     0x01 },
};

static struct gp_cfg_option pic18lf66k80_t0ckmx_opts[] =
{ /* for T0CKMX: */
	{ "PORTG",     0x00 },
	{ "PORTB",     0x02 },
};

static struct gp_cfg_option pic18lf66k80_t3ckmx_opts[] =
{ /* for T3CKMX: */
	{ "PORTG",     0x00 },
	{ "PORTB",     0x04 },
};

static struct gp_cfg_directive pic18lf66k80_300005[] =
{ /* for 0x300005 */
	{ "CANMX",     0x01,  2, pic18lf66k80_canmx_opts },
	{ "T0CKMX",    0x02,  2, pic18lf66k80_t0ckmx_opts },
	{ "T3CKMX",    0x04,  2, pic18lf66k80_t3ckmx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
};

/* config addresses for PIC18LF66K80: */
static struct gp_cfg_addr gp_cfg_pic18lf66k80_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f66k22_300000 },
	{ 0x300001, 0x08,  4, pic18f66k22_300001 },
	{ 0x300002, 0x7f,  4, pic18f66k22_300002 },
	{ 0x300003, 0x7f,  2, pic18f66k22_300003 },
	{ 0x300005, 0x8f,  5, pic18lf66k80_300005 },
	{ 0x300006, 0x91,  2, pic18f66k22_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x0f,  4, pic18f66k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f66k22_30000d },
};

/*
 * PIC18LF25K80
 */

static struct gp_cfg_option pic18lf25k80_canmx_opts[] =
{ /* for CANMX: */
	{ "PORTC",     0x00 },
	{ "PORTB",     0x01 },
};

static struct gp_cfg_directive pic18lf25k80_300005[] =
{ /* for 0x300005 */
	{ "CANMX",     0x01,  2, pic18lf25k80_canmx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
};

/* config addresses for PIC18LF25K80: */
static struct gp_cfg_addr gp_cfg_pic18lf25k80_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f66k22_300000 },
	{ 0x300001, 0x08,  4, pic18f66k22_300001 },
	{ 0x300002, 0x7f,  4, pic18f66k22_300002 },
	{ 0x300003, 0x7f,  2, pic18f66k22_300003 },
	{ 0x300005, 0x89,  3, pic18lf25k80_300005 },
	{ 0x300006, 0x91,  2, pic18f66k22_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x0f,  4, pic18f66k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f66k22_30000d },
};

/*
 * PIC18F86J65
 */

static struct gp_cfg_directive pic18f86j65_017ffd[] =
{ /* for 0x017ffd */
	{ "ETHLED",    0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ECCPMX",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_wdt_opts },
};

/* config addresses for PIC18F86J65: */
static struct gp_cfg_addr gp_cfg_pic18f86j65_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j65_017ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f66j65_017ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f66j65_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffd, 0xf7,  3, pic18f86j65_017ffd },
};

/*
 * PIC18F67K22
 */

static struct gp_cfg_directive pic18f67k22_30000c[] =
{ /* for 0x30000c */
	{ "EBRT0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBRT1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBRT2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBRT3",     0x08,  2, pic18f2523_cp3_opts },
	{ "EBRT4",     0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "EBRT5",     0x20,  2, pic18f2525_wrtc_opts },
	{ "EBRT6",     0x40,  2, pic18f2525_cpb_opts },
	{ "EBRT7",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F67K22: */
static struct gp_cfg_addr gp_cfg_pic18f67k22_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f66k22_300000 },
	{ 0x300001, 0x08,  4, pic18f66k22_300001 },
	{ 0x300002, 0x7f,  4, pic18f66k22_300002 },
	{ 0x300003, 0x7f,  2, pic18f66k22_300003 },
	{ 0x300004, 0x01,  1, pic18f66k22_300004 },
	{ 0x300005, 0x89,  3, pic18f66k22_300005 },
	{ 0x300006, 0x91,  2, pic18f66k22_300006 },
	{ 0x300008, 0xff,  8, pic18f8722_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8722_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0xff,  8, pic18f67k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f66k22_30000d },
};

/*
 * PIC18F86J60
 */

static struct gp_cfg_directive pic18f86j60_00fffd[] =
{ /* for 0x00fffd */
	{ "ETHLED",    0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ECCPMX",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_wdt_opts },
};

/* config addresses for PIC18F86J60: */
static struct gp_cfg_addr gp_cfg_pic18f86j60_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j93_00fff9 },
	{ 0x00fffa, 0xc7,  4, pic18f66j60_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffd, 0xf7,  3, pic18f86j60_00fffd },
};

/*
 * PIC18F85J15
 */

static struct gp_cfg_directive pic18f85j15_00bff8[] =
{ /* for 0x00bff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f85j15_00bff9[] =
{ /* for 0x00bff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f85j15_00bffa[] =
{ /* for 0x00bffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f83j11_fosc_opts },
};

static struct gp_cfg_directive pic18f85j15_00bffb[] =
{ /* for 0x00bffb */
	{ "WDTPS",     0x0f, 16, pic18f67j93_wdtps_opts },
};

static struct gp_cfg_directive pic18f85j15_00bffc[] =
{ /* for 0x00bffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f83j11_bw_opts },
	{ "MODE",      0x30,  4, pic18f83j11_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f67j93_t1dig_opts },
};

static struct gp_cfg_directive pic18f85j15_00bffd[] =
{ /* for 0x00bffd */
	{ "ECCPMX",    0x02,  2, pic18f85j50_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F85J15: */
static struct gp_cfg_addr gp_cfg_pic18f85j15_addrs[] =
{
	{ 0x00bff8, 0xe1,  4, pic18f85j15_00bff8 },
	{ 0x00bff9, 0xf4,  1, pic18f85j15_00bff9 },
	{ 0x00bffa, 0xc7,  4, pic18f85j15_00bffa },
	{ 0x00bffb, 0xff,  1, pic18f85j15_00bffb },
	{ 0x00bffc, 0xf8,  4, pic18f85j15_00bffc },
	{ 0x00bffd, 0xf3,  2, pic18f85j15_00bffd },
};

/*
 * PIC18F85K22
 */

static struct gp_cfg_option pic18f85k22_abw_opts[] =
{ /* for ABW: */
	{ "MM",        0x00 },
	{ "XM12",      0x10 },
	{ "XM16",      0x20 },
	{ "XM20",      0x30 },
};

static struct gp_cfg_directive pic18f85k22_300004[] =
{ /* for 0x300004 */
	{ "RTCOSC",    0x01,  2, pic18f66k22_rtcosc_opts },
	{ "EASHFT",    0x08,  2, pic18f67j93_t1dig_opts },
	{ "ABW",       0x30,  4, pic18f85k22_abw_opts },
	{ "BW",        0x40,  2, pic18f83j11_bw_opts },
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F85K22: */
static struct gp_cfg_addr gp_cfg_pic18f85k22_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f66k22_300000 },
	{ 0x300001, 0x08,  4, pic18f66k22_300001 },
	{ 0x300002, 0x7f,  4, pic18f66k22_300002 },
	{ 0x300003, 0x7f,  2, pic18f66k22_300003 },
	{ 0x300004, 0xf9,  5, pic18f85k22_300004 },
	{ 0x300005, 0x8b,  4, pic18f85k90_300005 },
	{ 0x300006, 0x91,  2, pic18f66k22_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x0f,  4, pic18f66k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f66k22_30000d },
};

/*
 * PIC18F85J10
 */

static struct gp_cfg_directive pic18f85j10_007ff8[] =
{ /* for 0x007ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f85j10_007ff9[] =
{ /* for 0x007ff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f85j10_007ffa[] =
{ /* for 0x007ffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f83j11_fosc_opts },
};

static struct gp_cfg_directive pic18f85j10_007ffd[] =
{ /* for 0x007ffd */
	{ "ECCPMX",    0x02,  2, pic18f85j50_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F85J10: */
static struct gp_cfg_addr gp_cfg_pic18f85j10_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f85j10_007ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f85j10_007ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f85j10_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f65j50_007ffb },
	{ 0x007ffc, 0xf8,  4, pic18f85j50_007ffc },
	{ 0x007ffd, 0xf3,  2, pic18f85j10_007ffd },
};

/*
 * PIC18F85J11
 */

static struct gp_cfg_directive pic18f85j11_007ffa[] =
{ /* for 0x007ffa */
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f83j11_fosc_opts },
};

static struct gp_cfg_directive pic18f85j11_007ffd[] =
{ /* for 0x007ffd */
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F85J11: */
static struct gp_cfg_addr gp_cfg_pic18f85j11_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f85j10_007ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f85j10_007ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f85j11_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f65j50_007ffb },
	{ 0x007ffc, 0xf8,  4, pic18f85j50_007ffc },
	{ 0x007ffd, 0xf1,  1, pic18f85j11_007ffd },
};

/*
 * PIC18F2331
 */

static struct gp_cfg_option pic18f2331_osc_opts[] =
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
	{ "RC1",       0x0b },
	{ "RC",        0x0f },
};

static struct gp_cfg_directive pic18f2331_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 12, pic18f2331_osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_directive pic18f2331_300002[] =
{ /* for 0x300002 */
	{ "PWRTEN",    0x01,  2, pic18f6410_pwrt_opts },
	{ "BOREN",     0x02,  2, pic18f2525_pbaden_opts },
	{ "BORV",      0x0c,  4, pic18f8585_borv_opts },
};

static struct gp_cfg_directive pic18f2331_300003[] =
{ /* for 0x300003 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "WINEN",     0x20,  2, pic18f2525_wrtc_opts },
	{ "WDPS",      0x1e, 16, pic18f6410_wdtps_opts },
};

static struct gp_cfg_option pic18f2331_hpol_opts[] =
{ /* for HPOL: */
	{ "LOW",       0x00 },
	{ "HIGH",      0x10 },
};

static struct gp_cfg_option pic18f2331_lpol_opts[] =
{ /* for LPOL: */
	{ "LOW",       0x00 },
	{ "HIGH",      0x08 },
};

static struct gp_cfg_directive pic18f2331_300004[] =
{ /* for 0x300004 */
	{ "T1OSCMX",   0x20,  2, pic18f4450_vregen_opts },
	{ "HPOL",      0x10,  2, pic18f2331_hpol_opts },
	{ "LPOL",      0x08,  2, pic18f2331_lpol_opts },
	{ "PWMPIN",    0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f2331_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_directive pic18f2331_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F2331: */
static struct gp_cfg_addr gp_cfg_pic18f2331_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f2331_300001 },
	{ 0x300002, 0x0f,  3, pic18f2331_300002 },
	{ 0x300003, 0x3f,  3, pic18f2331_300003 },
	{ 0x300004, 0x3c,  4, pic18f2331_300004 },
	{ 0x300005, 0x9d,  1, pic18f2331_300005 },
	{ 0x300006, 0x85,  3, pic18f2331_300006 },
	{ 0x300008, 0x0f,  2, pic18f4220_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f4220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f4220_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F65J11
 */

/* config addresses for PIC18F65J11: */
static struct gp_cfg_addr gp_cfg_pic18f65j11_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f85j10_007ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f85j10_007ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f85j11_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f65j50_007ffb },
	{ 0x007ffc, 0xf8,  0, NULL },
	{ 0x007ffd, 0xf1,  1, pic18f85j11_007ffd },
};

/*
 * PIC18F8527
 */

static struct gp_cfg_directive pic18f8527_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f8527_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f8527_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
};

/* config addresses for PIC18F8527: */
static struct gp_cfg_addr gp_cfg_pic18f8527_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xf3,  4, pic18f8722_300004 },
	{ 0x300005, 0x83,  4, pic18f8722_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0xff,  3, pic18f8527_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  3, pic18f8527_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  3, pic18f8527_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F47J53
 */

static struct gp_cfg_directive pic18f47j53_01fff9[] =
{ /* for 0x01fff9 */
	{ "CPUDIV",    0x03,  4, pic18f65j50_cpudiv_opts },
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f47j53_01fffd[] =
{ /* for 0x01fffd */
	{ "IOL1WAY",   0x01,  2, pic18f6410_wdt_opts },
	{ "ADCSEL",    0x02,  2, pic18f26j53_adcsel_opts },
	{ "MSSP7B_EN", 0x08,  2, pic18f65j50_msspmsk_opts },
};

static struct gp_cfg_directive pic18f47j53_01ffff[] =
{ /* for 0x01ffff */
	{ "WPDIS",     0x01,  2, pic18f6410_pwrt_opts },
	{ "WPEND",     0x02,  2, pic18f26j53_wpend_opts },
	{ "LS48MHZ",   0x08,  2, pic18f26j53_ls48mhz_opts },
};

/* config addresses for PIC18F47J53: */
static struct gp_cfg_addr gp_cfg_pic18f47j53_addrs[] =
{
	{ 0x01fff8, 0xff,  5, pic18lf27j13_01fff8 },
	{ 0x01fff9, 0xf7,  2, pic18f47j53_01fff9 },
	{ 0x01fffa, 0xff,  5, pic18lf27j13_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffc, 0xff,  5, pic18lf27j13_01fffc },
	{ 0x01fffd, 0xfb,  3, pic18f47j53_01fffd },
	{ 0x01fffe, 0xff,  2, pic18lf27j13_01fffe },
	{ 0x01ffff, 0xfb,  3, pic18f47j53_01ffff },
};

/*
 * PIC18F66J16
 */

static struct gp_cfg_directive pic18f66j16_017ff8[] =
{ /* for 0x017ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f66j16_017ff9[] =
{ /* for 0x017ff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f66j16_017ffd[] =
{ /* for 0x017ffd */
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F66J16: */
static struct gp_cfg_addr gp_cfg_pic18f66j16_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j16_017ff8 },
	{ 0x017ff9, 0xf7,  1, pic18f66j16_017ff9 },
	{ 0x017ffa, 0xc7,  3, pic18f86j55_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffc, 0xf8,  0, NULL },
	{ 0x017ffd, 0xff,  2, pic18f66j16_017ffd },
};

/*
 * PIC18C452
 */

/* config addresses for PIC18C452: */
static struct gp_cfg_addr gp_cfg_pic18c452_addrs[] =
{
	{ 0x300000, 0xff,  1, pic18c858_300000 },
	{ 0x300001, 0xe7,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x03,  1, pic18c858_300006 },
};

/*
 * PIC18F4515
 */

/* config addresses for PIC18F4515: */
static struct gp_cfg_addr gp_cfg_pic18f4515_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2525_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  3, pic18f2525_300008 },
	{ 0x300009, 0x40,  1, pic18f4450_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2525_30000a },
	{ 0x30000b, 0x60,  2, pic18f4450_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F2423
 */

/* config addresses for PIC18F2423: */
static struct gp_cfg_addr gp_cfg_pic18f2423_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2523_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  2, pic18f4220_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f4220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f4220_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F86J50
 */

static struct gp_cfg_directive pic18f86j50_00fff8[] =
{ /* for 0x00fff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "PLLDIV",    0x0e,  8, pic18f65j50_plldiv_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f86j50_00fff9[] =
{ /* for 0x00fff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
	{ "CPUDIV",    0x03,  4, pic18f65j50_cpudiv_opts },
};

static struct gp_cfg_directive pic18f86j50_00fffa[] =
{ /* for 0x00fffa */
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "FOSC",      0x07,  8, pic18f65j50_fosc_opts },
};

static struct gp_cfg_directive pic18f86j50_00fffc[] =
{ /* for 0x00fffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f83j11_bw_opts },
	{ "MODE",      0x30,  4, pic18f83j11_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f67j93_t1dig_opts },
};

static struct gp_cfg_directive pic18f86j50_00fffd[] =
{ /* for 0x00fffd */
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "PMPMX",     0x04,  2, pic18f85j50_pmpmx_opts },
	{ "ECCPMX",    0x02,  2, pic18f85j50_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F86J50: */
static struct gp_cfg_addr gp_cfg_pic18f86j50_addrs[] =
{
	{ 0x00fff8, 0xef,  5, pic18f86j50_00fff8 },
	{ 0x00fff9, 0xf7,  2, pic18f86j50_00fff9 },
	{ 0x00fffa, 0xc7,  3, pic18f86j50_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xf8,  4, pic18f86j50_00fffc },
	{ 0x00fffd, 0xff,  4, pic18f86j50_00fffd },
};

/*
 * PIC18F46J11
 */

/* config addresses for PIC18F46J11: */
static struct gp_cfg_addr gp_cfg_pic18f46j11_addrs[] =
{
	{ 0x00fff8, 0xe1,  3, pic18f66j93_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j93_00fff9 },
	{ 0x00fffa, 0xdf,  5, pic18f26j50_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xff,  5, pic18f26j53_00fffc },
	{ 0x00fffd, 0xf9,  2, pic18f26j50_00fffd },
	{ 0x00fffe, 0xff,  3, pic18f26j50_00fffe },
	{ 0x00ffff, 0xf1,  1, pic18f26j50_00ffff },
};

/*
 * PIC18LF45J11
 */

static struct gp_cfg_directive pic18lf45j11_007ff8[] =
{ /* for 0x007ff8 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

/* config addresses for PIC18LF45J11: */
static struct gp_cfg_addr gp_cfg_pic18lf45j11_addrs[] =
{
	{ 0x007ff8, 0xe1,  3, pic18lf45j11_007ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f85j10_007ff9 },
	{ 0x007ffa, 0xdf,  5, pic18lf45j50_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f65j50_007ffb },
	{ 0x007ffc, 0xff,  5, pic18lf45j50_007ffc },
	{ 0x007ffd, 0xf9,  2, pic18lf45j50_007ffd },
	{ 0x007ffe, 0xdf,  3, pic18lf45j50_007ffe },
	{ 0x007fff, 0xf1,  1, pic18lf45j50_007fff },
};

/*
 * PIC18F96J60
 */

/* config addresses for PIC18F96J60: */
static struct gp_cfg_addr gp_cfg_pic18f96j60_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j93_00fff9 },
	{ 0x00fffa, 0xc7,  4, pic18f66j60_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xf8,  4, pic18f86j50_00fffc },
	{ 0x00fffd, 0xf7,  3, pic18f86j60_00fffd },
};

/*
 * PIC18F64J90
 */

/* config addresses for PIC18F64J90: */
static struct gp_cfg_addr gp_cfg_pic18f64j90_addrs[] =
{
	{ 0x003ff8, 0xe1,  4, pic18f84j11_003ff8 },
	{ 0x003ff9, 0xf4,  1, pic18f84j11_003ff9 },
	{ 0x003ffa, 0xc7,  4, pic18f84j11_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f84j11_003ffb },
	{ 0x003ffc, 0x00,  0, NULL },
	{ 0x003ffd, 0xf1,  1, pic18f84j11_003ffd },
};

/*
 * PIC18F8621
 */

static struct gp_cfg_directive pic18f8621_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "ECCPMX",    0x02,  2, pic18f8585_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f2525_ccp2mx_opts },
};

/* config addresses for PIC18F8621: */
static struct gp_cfg_addr gp_cfg_pic18f8621_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8585_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  2, pic18f8585_300004 },
	{ 0x300005, 0x83,  3, pic18f8621_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8620
 */

/* config addresses for PIC18F8620: */
static struct gp_cfg_addr gp_cfg_pic18f8620_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18f8520_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300004, 0x83,  2, pic18f8585_300004 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0xff,  4, pic18f8520_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  4, pic18f8520_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  4, pic18f8520_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8525
 */

/* config addresses for PIC18F8525: */
static struct gp_cfg_addr gp_cfg_pic18f8525_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8585_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  2, pic18f8585_300004 },
	{ 0x300005, 0x83,  3, pic18f8621_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F2221
 */

static struct gp_cfg_directive pic18f2221_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "BBSIZ",     0x30,  2, pic18f4221_bbsiz_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F2221: */
static struct gp_cfg_addr gp_cfg_pic18f2221_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4221_300001 },
	{ 0x300002, 0x1f,  3, pic18f4221_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f4221_300005 },
	{ 0x300006, 0x85,  5, pic18f2221_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F24J50
 */

static struct gp_cfg_directive pic18f24j50_003ff8[] =
{ /* for 0x003ff8 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "PLLDIV",    0x0e,  8, pic18f65j50_plldiv_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

static struct gp_cfg_directive pic18f24j50_003ff9[] =
{ /* for 0x003ff9 */
	{ "CPUDIV",    0x03,  4, pic18f65j50_cpudiv_opts },
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f24j50_003ffa[] =
{ /* for 0x003ffa */
	{ "OSC",       0x07,  8, pic18f65j50_fosc_opts },
	{ "T1DIG",     0x08,  2, pic18f67j93_t1dig_opts },
	{ "LPT1OSC",   0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_directive pic18f24j50_003ffc[] =
{ /* for 0x003ffc */
	{ "DSWDTOSC",  0x01,  2, pic18lf45j50_dswdtosc_opts },
	{ "RTCOSC",    0x02,  2, pic18f67j93_rtcsosc_opts },
	{ "DSBOREN",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "DSWDTEN",   0x08,  2, pic18f67j93_t1dig_opts },
	{ "DSWDTPS",   0xf0, 16, pic18lf45j50_dswdtps_opts },
};

static struct gp_cfg_directive pic18f24j50_003ffd[] =
{ /* for 0x003ffd */
	{ "IOL1WAY",   0x01,  2, pic18f6410_wdt_opts },
	{ "MSSP7B_EN", 0x08,  2, pic18f65j50_msspmsk_opts },
};

static struct gp_cfg_option pic18f24j50_wpfp_opts[] =
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

static struct gp_cfg_directive pic18f24j50_003ffe[] =
{ /* for 0x003ffe */
	{ "WPFP",      0x0f, 16, pic18f24j50_wpfp_opts },
	{ "WPEND",     0x40,  2, pic18lf45j50_wpend_opts },
	{ "WPCFG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f24j50_003fff[] =
{ /* for 0x003fff */
	{ "WPDIS",     0x01,  2, pic18f6410_pwrt_opts },
};

/* config addresses for PIC18F24J50: */
static struct gp_cfg_addr gp_cfg_pic18f24j50_addrs[] =
{
	{ 0x003ff8, 0xef,  4, pic18f24j50_003ff8 },
	{ 0x003ff9, 0xf7,  2, pic18f24j50_003ff9 },
	{ 0x003ffa, 0xdf,  5, pic18f24j50_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f84j11_003ffb },
	{ 0x003ffc, 0xff,  5, pic18f24j50_003ffc },
	{ 0x003ffd, 0xf9,  2, pic18f24j50_003ffd },
	{ 0x003ffe, 0xcf,  3, pic18f24j50_003ffe },
	{ 0x003fff, 0xf1,  1, pic18f24j50_003fff },
};

/*
 * PIC18F2539
 */

/* config addresses for PIC18F2539: */
static struct gp_cfg_addr gp_cfg_pic18f2539_addrs[] =
{
	{ 0x300001, 0x22,  1, pic18f2439_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4682
 */

static struct gp_cfg_directive pic18f4682_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
	{ "CP3",       0x08,  2, pic18f2523_cp3_opts },
	{ "CP4",       0x10,  2, pic18f67j93_lpt1osc_opts },
};

static struct gp_cfg_directive pic18f4682_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
	{ "WRT3",      0x08,  2, pic18f2523_cp3_opts },
	{ "WRT4",      0x10,  2, pic18f67j93_lpt1osc_opts },
};

static struct gp_cfg_directive pic18f4682_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBTR3",     0x08,  2, pic18f2523_cp3_opts },
	{ "EBTR4",     0x10,  2, pic18f67j93_lpt1osc_opts },
};

/* config addresses for PIC18F4682: */
static struct gp_cfg_addr gp_cfg_pic18f4682_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4685_300001 },
	{ 0x300002, 0x1f,  3, pic18f4685_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x82,  3, pic18f4450_300005 },
	{ 0x300006, 0x85,  5, pic18f4685_300006 },
	{ 0x300008, 0x3f,  5, pic18f4682_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x3f,  5, pic18f4682_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x3f,  5, pic18f4682_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F13K50
 */

static struct gp_cfg_option pic18f13k50_cpudiv_opts[] =
{ /* for CPUDIV: */
	{ "NOCLKDIV",  0x00 },
	{ "CLKDIV2",   0x08 },
	{ "CLKDIV3",   0x10 },
	{ "CLKDIV4",   0x18 },
};

static struct gp_cfg_directive pic18f13k50_300000[] =
{ /* for 0x300000 */
	{ "CPUDIV",    0x18,  4, pic18f13k50_cpudiv_opts },
	{ "USBDIV",    0x20,  2, pic18f4450_vregen_opts },
};

/* config addresses for PIC18F13K50: */
static struct gp_cfg_addr gp_cfg_pic18f13k50_addrs[] =
{
	{ 0x300000, 0x00,  2, pic18f13k50_300000 },
	{ 0x300001, 0x27,  5, pic18lf14k22_300001 },
	{ 0x300002, 0x1f,  3, pic18lf14k22_300002 },
	{ 0x300003, 0x1f,  2, pic18f46k20_300003 },
	{ 0x300005, 0x88,  2, pic18lf14k22_300005 },
	{ 0x300006, 0x05,  4, pic18lf14k22_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F46J13
 */

static struct gp_cfg_directive pic18f46j13_00fffd[] =
{ /* for 0x00fffd */
	{ "IOL1WAY",   0x01,  2, pic18f6410_wdt_opts },
	{ "ADCSEL",    0x02,  2, pic18f26j53_adcsel_opts },
	{ "PLLSEL",    0x04,  2, pic18lf27j13_pllsel_opts },
	{ "MSSP7B_EN", 0x08,  2, pic18f65j50_msspmsk_opts },
};

static struct gp_cfg_directive pic18f46j13_00ffff[] =
{ /* for 0x00ffff */
	{ "WPDIS",     0x01,  2, pic18f6410_pwrt_opts },
	{ "WPEND",     0x02,  2, pic18f26j53_wpend_opts },
};

/* config addresses for PIC18F46J13: */
static struct gp_cfg_addr gp_cfg_pic18f46j13_addrs[] =
{
	{ 0x00fff8, 0xff,  5, pic18f26j53_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j93_00fff9 },
	{ 0x00fffa, 0xff,  5, pic18f26j53_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xff,  5, pic18f26j53_00fffc },
	{ 0x00fffd, 0xff,  4, pic18f46j13_00fffd },
	{ 0x00fffe, 0xbf,  2, pic18f26j53_00fffe },
	{ 0x00ffff, 0xf3,  2, pic18f46j13_00ffff },
};

/*
 * PIC18F67J60
 */

static struct gp_cfg_directive pic18f67j60_01fff8[] =
{ /* for 0x01fff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVR",      0x20,  2, pic18f4450_vregen_opts },
	{ "WDT",       0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f67j60_01fffa[] =
{ /* for 0x01fffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f83j11_fosc_opts },
};

static struct gp_cfg_directive pic18f67j60_01fffd[] =
{ /* for 0x01fffd */
	{ "ETHLED",    0x04,  2, pic18f6410_lpt1osc_opts },
};

/* config addresses for PIC18F67J60: */
static struct gp_cfg_addr gp_cfg_pic18f67j60_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f67j60_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j93_01fff9 },
	{ 0x01fffa, 0xc7,  4, pic18f67j60_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffd, 0xf7,  1, pic18f67j60_01fffd },
};

/*
 * PIC18F83J90
 */

/* config addresses for PIC18F83J90: */
static struct gp_cfg_addr gp_cfg_pic18f83j90_addrs[] =
{
	{ 0x001ff8, 0xe1,  4, pic18f83j11_001ff8 },
	{ 0x001ff9, 0xf4,  1, pic18f83j11_001ff9 },
	{ 0x001ffa, 0xc7,  4, pic18f83j11_001ffa },
	{ 0x001ffb, 0xff,  1, pic18f83j11_001ffb },
	{ 0x001ffc, 0x00,  0, NULL },
	{ 0x001ffd, 0xf1,  1, pic18f83j11_001ffd },
};

/*
 * PIC18F2410
 */

/* config addresses for PIC18F2410: */
static struct gp_cfg_addr gp_cfg_pic18f2410_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2525_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  2, pic18f4220_300008 },
	{ 0x300009, 0x40,  1, pic18f4450_300009 },
	{ 0x30000a, 0x0f,  2, pic18f4220_30000a },
	{ 0x30000b, 0x60,  2, pic18f4450_30000b },
	{ 0x30000c, 0x0f,  2, pic18f4220_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F23K20
 */

/* config addresses for PIC18F23K20: */
static struct gp_cfg_addr gp_cfg_pic18f23k20_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f46k20_300001 },
	{ 0x300002, 0x1f,  3, pic18f46k20_300002 },
	{ 0x300003, 0x1f,  2, pic18f46k20_300003 },
	{ 0x300005, 0x8b,  5, pic18f46k20_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F87J50
 */

static struct gp_cfg_directive pic18f87j50_01fff8[] =
{ /* for 0x01fff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "PLLDIV",    0x0e,  8, pic18f65j50_plldiv_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f87j50_01fff9[] =
{ /* for 0x01fff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
	{ "CPUDIV",    0x03,  4, pic18f65j50_cpudiv_opts },
};

static struct gp_cfg_directive pic18f87j50_01fffa[] =
{ /* for 0x01fffa */
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "FOSC",      0x07,  8, pic18f65j50_fosc_opts },
};

static struct gp_cfg_directive pic18f87j50_01fffc[] =
{ /* for 0x01fffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f83j11_bw_opts },
	{ "MODE",      0x30,  4, pic18f83j11_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f67j93_t1dig_opts },
};

static struct gp_cfg_directive pic18f87j50_01fffd[] =
{ /* for 0x01fffd */
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "PMPMX",     0x04,  2, pic18f85j50_pmpmx_opts },
	{ "ECCPMX",    0x02,  2, pic18f85j50_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F87J50: */
static struct gp_cfg_addr gp_cfg_pic18f87j50_addrs[] =
{
	{ 0x01fff8, 0xef,  5, pic18f87j50_01fff8 },
	{ 0x01fff9, 0xf7,  2, pic18f87j50_01fff9 },
	{ 0x01fffa, 0xc7,  3, pic18f87j50_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffc, 0xf8,  4, pic18f87j50_01fffc },
	{ 0x01fffd, 0xff,  4, pic18f87j50_01fffd },
};

/*
 * PIC18F8627
 */

static struct gp_cfg_directive pic18f8627_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
	{ "CP3",       0x08,  2, pic18f2523_cp3_opts },
	{ "CP4",       0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "CP5",       0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_directive pic18f8627_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
	{ "WRT3",      0x08,  2, pic18f2523_cp3_opts },
	{ "WRT4",      0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "WRT5",      0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_directive pic18f8627_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBTR3",     0x08,  2, pic18f2523_cp3_opts },
	{ "EBTR4",     0x10,  2, pic18f67j93_lpt1osc_opts },
	{ "EBTR5",     0x20,  2, pic18f2525_wrtc_opts },
};

/* config addresses for PIC18F8627: */
static struct gp_cfg_addr gp_cfg_pic18f8627_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xf3,  4, pic18f8722_300004 },
	{ 0x300005, 0x83,  4, pic18f8722_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0xff,  6, pic18f8627_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  6, pic18f8627_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  6, pic18f8627_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8720
 */

/* config addresses for PIC18F8720: */
static struct gp_cfg_addr gp_cfg_pic18f8720_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18f8520_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300004, 0x83,  2, pic18f8585_300004 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0xff,  8, pic18f8722_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8722_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  8, pic18f8722_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F87J11
 */

static struct gp_cfg_directive pic18f87j11_01fff8[] =
{ /* for 0x01fff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f87j11_01fff9[] =
{ /* for 0x01fff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

/* config addresses for PIC18F87J11: */
static struct gp_cfg_addr gp_cfg_pic18f87j11_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f87j11_01fff8 },
	{ 0x01fff9, 0xf7,  1, pic18f87j11_01fff9 },
	{ 0x01fffa, 0xc7,  3, pic18f87j50_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffc, 0xf8,  4, pic18f87j50_01fffc },
	{ 0x01fffd, 0xff,  4, pic18f87j50_01fffd },
};

/*
 * PIC18C801
 */

static struct gp_cfg_option pic18c801_osc_opts[] =
{ /* for OSC: */
	{ "LP",        0x00 },
	{ "EC",        0x01 },
	{ "HS",        0x02 },
	{ "RC",        0x03 },
};

static struct gp_cfg_directive pic18c801_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x03,  4, pic18c801_osc_opts },
};

static struct gp_cfg_directive pic18c801_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BW",        0x40,  2, pic18f83j11_bw_opts },
};

static struct gp_cfg_directive pic18c801_300003[] =
{ /* for 0x300003 */
	{ "WDT",       0x01,  2, pic18f6410_wdt_opts },
	{ "WDTPS",     0x0e,  8, pic18c858_wdtps_opts },
};

static struct gp_cfg_directive pic18c801_300006[] =
{ /* for 0x300006 */
	{ "STVR",      0x01,  2, pic18f6410_wdt_opts },
};

/* config addresses for PIC18C801: */
static struct gp_cfg_addr gp_cfg_pic18c801_addrs[] =
{
	{ 0x300001, 0x03,  1, pic18c801_300001 },
	{ 0x300002, 0x41,  2, pic18c801_300002 },
	{ 0x300003, 0x0e,  2, pic18c801_300003 },
	{ 0x300006, 0x81,  1, pic18c801_300006 },
};

/*
 * PIC18F8680
 */

/* config addresses for PIC18F8680: */
static struct gp_cfg_addr gp_cfg_pic18f8680_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8585_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  2, pic18f8585_300004 },
	{ 0x300005, 0x83,  3, pic18f8585_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6585
 */

static struct gp_cfg_directive pic18f6585_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_wdt_opts },
};

/* config addresses for PIC18F6585: */
static struct gp_cfg_addr gp_cfg_pic18f6585_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8585_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  0, NULL },
	{ 0x300005, 0x83,  2, pic18f6585_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F25J10
 */

/* config addresses for PIC18F25J10: */
static struct gp_cfg_addr gp_cfg_pic18f25j10_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f85j10_007ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f85j10_007ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f85j10_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f65j50_007ffb },
	{ 0x007ffd, 0xf1,  1, pic18f85j11_007ffd },
};

/*
 * PIC18F66J11
 */

static struct gp_cfg_directive pic18f66j11_00fff8[] =
{ /* for 0x00fff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f66j11_00fff9[] =
{ /* for 0x00fff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f66j11_00fffd[] =
{ /* for 0x00fffd */
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F66J11: */
static struct gp_cfg_addr gp_cfg_pic18f66j11_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j11_00fff8 },
	{ 0x00fff9, 0xf7,  1, pic18f66j11_00fff9 },
	{ 0x00fffa, 0xc7,  3, pic18f86j50_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xf8,  0, NULL },
	{ 0x00fffd, 0xff,  2, pic18f66j11_00fffd },
};

/*
 * PIC18F66J10
 */

static struct gp_cfg_directive pic18f66j10_00fffd[] =
{ /* for 0x00fffd */
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F66J10: */
static struct gp_cfg_addr gp_cfg_pic18f66j10_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j11_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j93_00fff9 },
	{ 0x00fffa, 0xc7,  4, pic18f66j60_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xf8,  0, NULL },
	{ 0x00fffd, 0xf3,  1, pic18f66j10_00fffd },
};

/*
 * PIC18F97J60
 */

static struct gp_cfg_directive pic18f97j60_01fffd[] =
{ /* for 0x01fffd */
	{ "ETHLED",    0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ECCPMX",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_wdt_opts },
};

/* config addresses for PIC18F97J60: */
static struct gp_cfg_addr gp_cfg_pic18f97j60_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f67j60_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j93_01fff9 },
	{ 0x01fffa, 0xc7,  4, pic18f67j60_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffc, 0xf8,  4, pic18f87j50_01fffc },
	{ 0x01fffd, 0xf7,  3, pic18f97j60_01fffd },
};

/*
 * PIC18F66J50
 */

/* config addresses for PIC18F66J50: */
static struct gp_cfg_addr gp_cfg_pic18f66j50_addrs[] =
{
	{ 0x00fff8, 0xef,  5, pic18f86j50_00fff8 },
	{ 0x00fff9, 0xf7,  2, pic18f86j50_00fff9 },
	{ 0x00fffa, 0xc7,  3, pic18f86j50_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xf8,  0, NULL },
	{ 0x00fffd, 0xff,  2, pic18f66j11_00fffd },
};

/*
 * PIC18F66J55
 */

/* config addresses for PIC18F66J55: */
static struct gp_cfg_addr gp_cfg_pic18f66j55_addrs[] =
{
	{ 0x017ff8, 0xef,  5, pic18f86j55_017ff8 },
	{ 0x017ff9, 0xf7,  2, pic18f86j55_017ff9 },
	{ 0x017ffa, 0xc7,  3, pic18f86j55_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffc, 0xf8,  0, NULL },
	{ 0x017ffd, 0xff,  2, pic18f66j16_017ffd },
};

/*
 * PIC18F65J10
 */

static struct gp_cfg_directive pic18f65j10_007ffd[] =
{ /* for 0x007ffd */
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F65J10: */
static struct gp_cfg_addr gp_cfg_pic18f65j10_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f85j10_007ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f85j10_007ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f85j10_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f65j50_007ffb },
	{ 0x007ffc, 0xf8,  0, NULL },
	{ 0x007ffd, 0xf3,  1, pic18f65j10_007ffd },
};

/*
 * PIC18F2480
 */

static struct gp_cfg_option pic18f2480_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "1024",      0x00 },
	{ "2048",      0x10 },
};

static struct gp_cfg_directive pic18f2480_300006[] =
{ /* for 0x300006 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "BBSIZ",     0x10,  2, pic18f2480_bbsiz_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
};

/* config addresses for PIC18F2480: */
static struct gp_cfg_addr gp_cfg_pic18f2480_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4685_300001 },
	{ 0x300002, 0x1f,  3, pic18f4685_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x82,  3, pic18f4450_300005 },
	{ 0x300006, 0x85,  5, pic18f2480_300006 },
	{ 0x300008, 0x0f,  2, pic18f4220_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f4220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f4220_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F65J15
 */

static struct gp_cfg_directive pic18f65j15_00bffd[] =
{ /* for 0x00bffd */
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F65J15: */
static struct gp_cfg_addr gp_cfg_pic18f65j15_addrs[] =
{
	{ 0x00bff8, 0xe1,  4, pic18f85j15_00bff8 },
	{ 0x00bff9, 0xf4,  1, pic18f85j15_00bff9 },
	{ 0x00bffa, 0xc7,  4, pic18f85j15_00bffa },
	{ 0x00bffb, 0xff,  1, pic18f85j15_00bffb },
	{ 0x00bffc, 0xf8,  0, NULL },
	{ 0x00bffd, 0xf3,  1, pic18f65j15_00bffd },
};

/*
 * PIC18F6722
 */

static struct gp_cfg_directive pic18f6722_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_ccp2mx_opts },
};

/* config addresses for PIC18F6722: */
static struct gp_cfg_addr gp_cfg_pic18f6722_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  3, pic18f6722_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0xff,  8, pic18f8722_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8722_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  8, pic18f8722_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6720
 */

/* config addresses for PIC18F6720: */
static struct gp_cfg_addr gp_cfg_pic18f6720_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18f8520_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300004, 0x03,  0, NULL },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0xff,  8, pic18f8722_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8722_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  8, pic18f8722_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F452
 */

/* config addresses for PIC18F452: */
static struct gp_cfg_addr gp_cfg_pic18f452_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18f8520_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F87K90
 */

/* config addresses for PIC18F87K90: */
static struct gp_cfg_addr gp_cfg_pic18f87k90_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f66k22_300000 },
	{ 0x300001, 0x08,  4, pic18f66k22_300001 },
	{ 0x300002, 0x7f,  4, pic18f66k22_300002 },
	{ 0x300003, 0x7f,  2, pic18f66k22_300003 },
	{ 0x300004, 0x01,  1, pic18f66k22_300004 },
	{ 0x300005, 0x8b,  4, pic18f85k90_300005 },
	{ 0x300006, 0x91,  2, pic18f66k22_300006 },
	{ 0x300008, 0xff,  8, pic18f8722_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8722_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0xff,  8, pic18f67k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f66k22_30000d },
};

/*
 * PIC18F458
 */

/* config addresses for PIC18F458: */
static struct gp_cfg_addr gp_cfg_pic18f458_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18f8520_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F66K80
 */

static struct gp_cfg_option pic18f66k80_t3ckmx_opts[] =
{ /* for T3CKMX: */
	{ "T3CKI",     0x00 },
	{ "T1CKI",     0x04 },
};

static struct gp_cfg_directive pic18f66k80_300005[] =
{ /* for 0x300005 */
	{ "CANMX",     0x01,  2, pic18lf66k80_canmx_opts },
	{ "T0CKMX",    0x02,  2, pic18lf66k80_t0ckmx_opts },
	{ "T3CKMX",    0x04,  2, pic18f66k80_t3ckmx_opts },
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
};

/* config addresses for PIC18F66K80: */
static struct gp_cfg_addr gp_cfg_pic18f66k80_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f66k22_300000 },
	{ 0x300001, 0x08,  4, pic18f66k22_300001 },
	{ 0x300002, 0x7f,  4, pic18f66k22_300002 },
	{ 0x300003, 0x7f,  2, pic18f66k22_300003 },
	{ 0x300005, 0x8f,  5, pic18f66k80_300005 },
	{ 0x300006, 0x91,  2, pic18f66k22_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x0f,  4, pic18f66k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f66k22_30000d },
};

/*
 * PIC18F66J15
 */

static struct gp_cfg_directive pic18f66j15_017ffd[] =
{ /* for 0x017ffd */
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F66J15: */
static struct gp_cfg_addr gp_cfg_pic18f66j15_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j16_017ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f66j65_017ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f66j65_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffc, 0xf8,  0, NULL },
	{ 0x017ffd, 0xf3,  1, pic18f66j15_017ffd },
};

/*
 * PIC18F2550
 */

/* config addresses for PIC18F2550: */
static struct gp_cfg_addr gp_cfg_pic18f2550_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f4450_300000 },
	{ 0x300001, 0x05,  3, pic18f4450_300001 },
	{ 0x300002, 0x1f,  4, pic18f4458_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f4458_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6520
 */

/* config addresses for PIC18F6520: */
static struct gp_cfg_addr gp_cfg_pic18f6520_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18f8520_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300004, 0x03,  0, NULL },
	{ 0x300005, 0x03,  1, pic18f8520_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0xff,  4, pic18f8520_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  4, pic18f8520_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  4, pic18f8520_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F63J11
 */

/* config addresses for PIC18F63J11: */
static struct gp_cfg_addr gp_cfg_pic18f63j11_addrs[] =
{
	{ 0x001ff8, 0xe1,  4, pic18f83j11_001ff8 },
	{ 0x001ff9, 0xf4,  1, pic18f83j11_001ff9 },
	{ 0x001ffa, 0xc7,  4, pic18f83j11_001ffa },
	{ 0x001ffb, 0xff,  1, pic18f83j11_001ffb },
	{ 0x001ffc, 0xf8,  0, NULL },
	{ 0x001ffd, 0xf1,  1, pic18f83j11_001ffd },
};

/*
 * PIC18F6525
 */

static struct gp_cfg_directive pic18f6525_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "CCP2MX",    0x01,  2, pic18f2525_ccp2mx_opts },
};

/* config addresses for PIC18F6525: */
static struct gp_cfg_addr gp_cfg_pic18f6525_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8585_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  0, NULL },
	{ 0x300005, 0x83,  2, pic18f6525_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F85J90
 */

/* config addresses for PIC18F85J90: */
static struct gp_cfg_addr gp_cfg_pic18f85j90_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f85j10_007ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f85j10_007ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f85j11_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f65j50_007ffb },
	{ 0x007ffc, 0x00,  0, NULL },
	{ 0x007ffd, 0xf1,  1, pic18f85j11_007ffd },
};

/*
 * PIC18F1220
 */

static struct gp_cfg_option pic18f1220_borv_opts[] =
{ /* for BORV: */
	{ "45",        0x00 },
	{ "42",        0x04 },
	{ "27",        0x08 },
};

static struct gp_cfg_directive pic18f1220_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x02,  2, pic18f2525_pbaden_opts },
	{ "BORV",      0x0c,  3, pic18f1220_borv_opts },
};

static struct gp_cfg_directive pic18f1220_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
};

/* config addresses for PIC18F1220: */
static struct gp_cfg_addr gp_cfg_pic18f1220_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f4220_300001 },
	{ 0x300002, 0x0f,  3, pic18f1220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x80,  1, pic18f1220_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4553
 */

/* config addresses for PIC18F4553: */
static struct gp_cfg_addr gp_cfg_pic18f4553_addrs[] =
{
	{ 0x300000, 0x00,  3, pic18f4450_300000 },
	{ 0x300001, 0x05,  3, pic18f4450_300001 },
	{ 0x300002, 0x1f,  4, pic18f4458_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f4458_300005 },
	{ 0x300006, 0x85,  5, pic18f4458_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6680
 */

/* config addresses for PIC18F6680: */
static struct gp_cfg_addr gp_cfg_pic18f6680_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8585_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  0, NULL },
	{ 0x300005, 0x83,  2, pic18f6585_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F96J65
 */

/* config addresses for PIC18F96J65: */
static struct gp_cfg_addr gp_cfg_pic18f96j65_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j65_017ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f66j65_017ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f66j65_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffc, 0xf8,  4, pic18f86j55_017ffc },
	{ 0x017ffd, 0xf7,  3, pic18f86j65_017ffd },
};

/*
 * PIC18LF44J11
 */

static struct gp_cfg_directive pic18lf44j11_003ff8[] =
{ /* for 0x003ff8 */
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
};

/* config addresses for PIC18LF44J11: */
static struct gp_cfg_addr gp_cfg_pic18lf44j11_addrs[] =
{
	{ 0x003ff8, 0xe1,  3, pic18lf44j11_003ff8 },
	{ 0x003ff9, 0xf4,  1, pic18f84j11_003ff9 },
	{ 0x003ffa, 0xdf,  5, pic18f24j50_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f84j11_003ffb },
	{ 0x003ffc, 0xff,  5, pic18f24j50_003ffc },
	{ 0x003ffd, 0xf9,  2, pic18f24j50_003ffd },
	{ 0x003ffe, 0xcf,  3, pic18f24j50_003ffe },
	{ 0x003fff, 0xf1,  1, pic18f24j50_003fff },
};

/*
 * PIC18F6628
 */

/* config addresses for PIC18F6628: */
static struct gp_cfg_addr gp_cfg_pic18f6628_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  3, pic18f6722_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0xff,  6, pic18f8627_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  6, pic18f8627_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  6, pic18f8627_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F1330
 */

static struct gp_cfg_option pic18f1330_borv_opts[] =
{ /* for BORV: */
	{ "0",         0x00 },
	{ "1",         0x04 },
	{ "2",         0x08 },
	{ "3",         0x0c },
};

static struct gp_cfg_directive pic18f1330_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x06,  4, pic18f4685_boren_opts },
	{ "BORV",      0x0c,  4, pic18f1330_borv_opts },
};

static struct gp_cfg_directive pic18f1330_300004[] =
{ /* for 0x300004 */
	{ "HPOL",      0x08,  2, pic18f2331_lpol_opts },
	{ "LPOL",      0x04,  2, pic18f66k22_intoscsel_opts },
	{ "PWMPIN",    0x02,  2, pic18f2525_cp1_opts },
};

static struct gp_cfg_option pic18f1330_fltamx_opts[] =
{ /* for FLTAMX: */
	{ "RA7",       0x00 },
	{ "RA5",       0x01 },
};

static struct gp_cfg_directive pic18f1330_300005[] =
{ /* for 0x300005 */
	{ "FLTAMX",    0x01,  2, pic18f1330_fltamx_opts },
	{ "T1OSCMX",   0x08,  2, pic18f2331_lpol_opts },
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_directive pic18f1330_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "BBSIZ",     0x30,  3, pic18f2321_bbsiz_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F1330: */
static struct gp_cfg_addr gp_cfg_pic18f1330_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4221_300001 },
	{ 0x300002, 0x1f,  3, pic18f1330_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x0e,  3, pic18f1330_300004 },
	{ 0x300005, 0x81,  3, pic18f1330_300005 },
	{ 0x300006, 0x81,  4, pic18f1330_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6622
 */

/* config addresses for PIC18F6622: */
static struct gp_cfg_addr gp_cfg_pic18f6622_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x81,  3, pic18f6410_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0xff,  4, pic18f8520_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  4, pic18f8520_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  4, pic18f8520_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6621
 */

/* config addresses for PIC18F6621: */
static struct gp_cfg_addr gp_cfg_pic18f6621_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8585_300001 },
	{ 0x300002, 0x0f,  3, pic18f8585_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  0, NULL },
	{ 0x300005, 0x83,  2, pic18f6525_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6620
 */

/* config addresses for PIC18F6620: */
static struct gp_cfg_addr gp_cfg_pic18f6620_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18f8520_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300004, 0x03,  0, NULL },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f8585_300006 },
	{ 0x300008, 0xff,  4, pic18f8520_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  4, pic18f8520_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  4, pic18f8520_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F67J10
 */

static struct gp_cfg_directive pic18f67j10_01fffd[] =
{ /* for 0x01fffd */
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F67J10: */
static struct gp_cfg_addr gp_cfg_pic18f67j10_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f87j11_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j93_01fff9 },
	{ 0x01fffa, 0xc7,  4, pic18f67j60_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffc, 0xf8,  0, NULL },
	{ 0x01fffd, 0xf3,  1, pic18f67j10_01fffd },
};

/*
 * PIC18F67J11
 */

static struct gp_cfg_directive pic18f67j11_01fffd[] =
{ /* for 0x01fffd */
	{ "MSSPMSK",   0x08,  2, pic18f65j50_msspmsk_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F67J11: */
static struct gp_cfg_addr gp_cfg_pic18f67j11_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f87j11_01fff8 },
	{ 0x01fff9, 0xf7,  1, pic18f87j11_01fff9 },
	{ 0x01fffa, 0xc7,  3, pic18f87j50_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffc, 0xf8,  0, NULL },
	{ 0x01fffd, 0xff,  2, pic18f67j11_01fffd },
};

/*
 * PIC18F2431
 */

/* config addresses for PIC18F2431: */
static struct gp_cfg_addr gp_cfg_pic18f2431_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f2331_300001 },
	{ 0x300002, 0x0f,  3, pic18f2331_300002 },
	{ 0x300003, 0x3f,  3, pic18f2331_300003 },
	{ 0x300004, 0x3c,  4, pic18f2331_300004 },
	{ 0x300005, 0x9d,  1, pic18f2331_300005 },
	{ 0x300006, 0x85,  3, pic18f2331_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f4220_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f4220_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4321
 */

static struct gp_cfg_directive pic18f4321_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ICPORT",    0x08,  2, pic18f67j93_t1dig_opts },
	{ "BBSIZ",     0x30,  3, pic18f2321_bbsiz_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F4321: */
static struct gp_cfg_addr gp_cfg_pic18f4321_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4221_300001 },
	{ 0x300002, 0x1f,  3, pic18f4221_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f4221_300005 },
	{ 0x300006, 0x85,  6, pic18f4321_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F86J15
 */

static struct gp_cfg_directive pic18f86j15_017ffd[] =
{ /* for 0x017ffd */
	{ "ECCPMX",    0x02,  2, pic18f85j50_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F86J15: */
static struct gp_cfg_addr gp_cfg_pic18f86j15_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j16_017ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f66j65_017ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f66j65_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffc, 0xf8,  4, pic18f86j55_017ffc },
	{ 0x017ffd, 0xf3,  2, pic18f86j15_017ffd },
};

/*
 * PIC18F1230
 */

static struct gp_cfg_directive pic18f1230_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "BBSIZ",     0x30,  2, pic18f4221_bbsiz_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F1230: */
static struct gp_cfg_addr gp_cfg_pic18f1230_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4221_300001 },
	{ 0x300002, 0x1f,  3, pic18f1330_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x0e,  3, pic18f1330_300004 },
	{ 0x300005, 0x81,  3, pic18f1330_300005 },
	{ 0x300006, 0x81,  4, pic18f1230_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F2580
 */

/* config addresses for PIC18F2580: */
static struct gp_cfg_addr gp_cfg_pic18f2580_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4685_300001 },
	{ 0x300002, 0x1f,  3, pic18f4685_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x82,  3, pic18f4450_300005 },
	{ 0x300006, 0x85,  5, pic18f2480_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F86J16
 */

/* config addresses for PIC18F86J16: */
static struct gp_cfg_addr gp_cfg_pic18f86j16_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j16_017ff8 },
	{ 0x017ff9, 0xf7,  1, pic18f66j16_017ff9 },
	{ 0x017ffa, 0xc7,  3, pic18f86j55_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffc, 0xf8,  4, pic18f86j55_017ffc },
	{ 0x017ffd, 0xff,  4, pic18f86j55_017ffd },
};

/*
 * PIC18F86J11
 */

/* config addresses for PIC18F86J11: */
static struct gp_cfg_addr gp_cfg_pic18f86j11_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j11_00fff8 },
	{ 0x00fff9, 0xf7,  1, pic18f66j11_00fff9 },
	{ 0x00fffa, 0xc7,  3, pic18f86j50_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xf8,  4, pic18f86j50_00fffc },
	{ 0x00fffd, 0xff,  4, pic18f86j50_00fffd },
};

/*
 * PIC18F86J10
 */

static struct gp_cfg_directive pic18f86j10_00fffd[] =
{ /* for 0x00fffd */
	{ "ECCPMX",    0x02,  2, pic18f85j50_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F86J10: */
static struct gp_cfg_addr gp_cfg_pic18f86j10_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j11_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j93_00fff9 },
	{ 0x00fffa, 0xc7,  4, pic18f66j60_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j93_00fffb },
	{ 0x00fffc, 0xf8,  4, pic18f86j50_00fffc },
	{ 0x00fffd, 0xf3,  2, pic18f86j10_00fffd },
};

/*
 * PIC18F2585
 */

/* config addresses for PIC18F2585: */
static struct gp_cfg_addr gp_cfg_pic18f2585_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4685_300001 },
	{ 0x300002, 0x1f,  3, pic18f4685_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x82,  3, pic18f4450_300005 },
	{ 0x300006, 0x85,  5, pic18f4685_300006 },
	{ 0x300008, 0x0f,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6527
 */

/* config addresses for PIC18F6527: */
static struct gp_cfg_addr gp_cfg_pic18f6527_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x81,  3, pic18f6410_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0xff,  3, pic18f8527_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  3, pic18f8527_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  3, pic18f8527_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F24J10
 */

static struct gp_cfg_directive pic18f24j10_003ffa[] =
{ /* for 0x003ffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f83j11_fosc_opts },
};

/* config addresses for PIC18F24J10: */
static struct gp_cfg_addr gp_cfg_pic18f24j10_addrs[] =
{
	{ 0x003ff8, 0xe1,  4, pic18f84j11_003ff8 },
	{ 0x003ff9, 0xf4,  1, pic18f84j11_003ff9 },
	{ 0x003ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f84j11_003ffb },
	{ 0x003ffd, 0xf1,  1, pic18f84j11_003ffd },
};

/*
 * PIC18F4331
 */

static struct gp_cfg_option pic18f4331_exclkmx_opts[] =
{ /* for EXCLKMX: */
	{ "RD0",       0x00 },
	{ "RC3",       0x10 },
};

static struct gp_cfg_option pic18f4331_pwm4mx_opts[] =
{ /* for PWM4MX: */
	{ "RD5",       0x00 },
	{ "RB5",       0x08 },
};

static struct gp_cfg_option pic18f4331_sspmx_opts[] =
{ /* for SSPMX: */
	{ "RD1",       0x00 },
	{ "RC7",       0x04 },
};

static struct gp_cfg_option pic18f4331_fltamx_opts[] =
{ /* for FLTAMX: */
	{ "RD4",       0x00 },
	{ "RC1",       0x01 },
};

static struct gp_cfg_directive pic18f4331_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "EXCLKMX",   0x10,  2, pic18f4331_exclkmx_opts },
	{ "PWM4MX",    0x08,  2, pic18f4331_pwm4mx_opts },
	{ "SSPMX",     0x04,  2, pic18f4331_sspmx_opts },
	{ "FLTAMX",    0x01,  2, pic18f4331_fltamx_opts },
};

/* config addresses for PIC18F4331: */
static struct gp_cfg_addr gp_cfg_pic18f4331_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f2331_300001 },
	{ 0x300002, 0x0f,  3, pic18f2331_300002 },
	{ 0x300003, 0x3f,  3, pic18f2331_300003 },
	{ 0x300004, 0x3c,  4, pic18f2331_300004 },
	{ 0x300005, 0x9d,  5, pic18f4331_300005 },
	{ 0x300006, 0x85,  3, pic18f2331_300006 },
	{ 0x300008, 0x0f,  2, pic18f4220_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F87J10
 */

static struct gp_cfg_directive pic18f87j10_01fffd[] =
{ /* for 0x01fffd */
	{ "ECCPMX",    0x02,  2, pic18f85j50_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f67j93_ccp2mx_opts },
};

/* config addresses for PIC18F87J10: */
static struct gp_cfg_addr gp_cfg_pic18f87j10_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f87j11_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j93_01fff9 },
	{ 0x01fffa, 0xc7,  4, pic18f67j60_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffc, 0xf8,  4, pic18f87j50_01fffc },
	{ 0x01fffd, 0xf3,  2, pic18f87j10_01fffd },
};

/*
 * PIC18F87J60
 */

/* config addresses for PIC18F87J60: */
static struct gp_cfg_addr gp_cfg_pic18f87j60_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f67j60_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j93_01fff9 },
	{ 0x01fffa, 0xc7,  4, pic18f67j60_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffd, 0xf7,  3, pic18f97j60_01fffd },
};

/*
 * PIC18F8622
 */

/* config addresses for PIC18F8622: */
static struct gp_cfg_addr gp_cfg_pic18f8622_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xf3,  4, pic18f8722_300004 },
	{ 0x300005, 0x83,  4, pic18f8722_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0xff,  4, pic18f8520_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  4, pic18f8520_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  4, pic18f8520_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F67J50
 */

/* config addresses for PIC18F67J50: */
static struct gp_cfg_addr gp_cfg_pic18f67j50_addrs[] =
{
	{ 0x01fff8, 0xef,  5, pic18f87j50_01fff8 },
	{ 0x01fff9, 0xf7,  2, pic18f87j50_01fff9 },
	{ 0x01fffa, 0xc7,  3, pic18f87j50_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j93_01fffb },
	{ 0x01fffc, 0xf8,  0, NULL },
	{ 0x01fffd, 0xff,  2, pic18f67j11_01fffd },
};

/*
 * PIC18F64J11
 */

/* config addresses for PIC18F64J11: */
static struct gp_cfg_addr gp_cfg_pic18f64j11_addrs[] =
{
	{ 0x003ff8, 0xe1,  4, pic18f84j11_003ff8 },
	{ 0x003ff9, 0xf4,  1, pic18f84j11_003ff9 },
	{ 0x003ffa, 0xc7,  4, pic18f84j11_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f84j11_003ffb },
	{ 0x003ffc, 0xf8,  0, NULL },
	{ 0x003ffd, 0xf1,  1, pic18f84j11_003ffd },
};

/*
 * PIC18F4431
 */

/* config addresses for PIC18F4431: */
static struct gp_cfg_addr gp_cfg_pic18f4431_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f2331_300001 },
	{ 0x300002, 0x0f,  3, pic18f2331_300002 },
	{ 0x300003, 0x3f,  3, pic18f2331_300003 },
	{ 0x300004, 0x3c,  4, pic18f2331_300004 },
	{ 0x300005, 0x9d,  5, pic18f4331_300005 },
	{ 0x300006, 0x85,  3, pic18f2331_300006 },
	{ 0x300008, 0x0f,  4, pic18f2523_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f2523_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f2523_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F87K22
 */

/* config addresses for PIC18F87K22: */
static struct gp_cfg_addr gp_cfg_pic18f87k22_addrs[] =
{
	{ 0x300000, 0x5d,  4, pic18f66k22_300000 },
	{ 0x300001, 0x08,  4, pic18f66k22_300001 },
	{ 0x300002, 0x7f,  4, pic18f66k22_300002 },
	{ 0x300003, 0x7f,  2, pic18f66k22_300003 },
	{ 0x300004, 0xf9,  5, pic18f85k22_300004 },
	{ 0x300005, 0x8b,  4, pic18f85k90_300005 },
	{ 0x300006, 0x91,  2, pic18f66k22_300006 },
	{ 0x300008, 0xff,  8, pic18f8722_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8722_30000a },
	{ 0x30000b, 0xe0,  3, pic18f46k22_30000b },
	{ 0x30000c, 0xff,  8, pic18f67k22_30000c },
	{ 0x30000d, 0x40,  1, pic18f66k22_30000d },
};

/*
 * Devices Table
 */

int gp_cfg_device_count = 272;
struct gp_cfg_device gp_cfg_devices[] =
{
	{ "PIC18C242",     6, gp_cfg_pic18c452_addrs },
	{ "PIC18C252",     6, gp_cfg_pic18c452_addrs },
	{ "PIC18C442",     6, gp_cfg_pic18c452_addrs },
	{ "PIC18C452",     6, gp_cfg_pic18c452_addrs },
	{ "PIC18C601",     4, gp_cfg_pic18c801_addrs },
	{ "PIC18C658",     5, gp_cfg_pic18c858_addrs },
	{ "PIC18C801",     4, gp_cfg_pic18c801_addrs },
	{ "PIC18C858",     5, gp_cfg_pic18c858_addrs },
	{ "PIC18F1220",   11, gp_cfg_pic18f1220_addrs },
	{ "PIC18F1230",   12, gp_cfg_pic18f1230_addrs },
	{ "PIC18F1320",   11, gp_cfg_pic18f1220_addrs },
	{ "PIC18F1330",   12, gp_cfg_pic18f1330_addrs },
	{ "PIC18F13K22",  11, gp_cfg_pic18lf14k22_addrs },
	{ "PIC18F13K50",  12, gp_cfg_pic18f13k50_addrs },
	{ "PIC18F14K22",  11, gp_cfg_pic18lf14k22_addrs },
	{ "PIC18F14K50",  12, gp_cfg_pic18f13k50_addrs },
	{ "PIC18F2220",   11, gp_cfg_pic18f4220_addrs },
	{ "PIC18F2221",   11, gp_cfg_pic18f2221_addrs },
	{ "PIC18F2320",   11, gp_cfg_pic18f2320_addrs },
	{ "PIC18F2321",   11, gp_cfg_pic18f2321_addrs },
	{ "PIC18F2331",   12, gp_cfg_pic18f2331_addrs },
	{ "PIC18F23K20",  11, gp_cfg_pic18f23k20_addrs },
	{ "PIC18F23K22",  11, gp_cfg_pic18f26k22_addrs },
	{ "PIC18F2410",   11, gp_cfg_pic18f2410_addrs },
	{ "PIC18F242",    11, gp_cfg_pic18f242_addrs },
	{ "PIC18F2420",   11, gp_cfg_pic18f4420_addrs },
	{ "PIC18F2423",   11, gp_cfg_pic18f2423_addrs },
	{ "PIC18F2431",   12, gp_cfg_pic18f2431_addrs },
	{ "PIC18F2439",   10, gp_cfg_pic18f2439_addrs },
	{ "PIC18F2450",   12, gp_cfg_pic18f4450_addrs },
	{ "PIC18F2455",   12, gp_cfg_pic18f2455_addrs },
	{ "PIC18F2458",   12, gp_cfg_pic18f2455_addrs },
	{ "PIC18F248",    10, gp_cfg_pic18f248_addrs },
	{ "PIC18F2480",   11, gp_cfg_pic18f2480_addrs },
	{ "PIC18F24J10",   5, gp_cfg_pic18f24j10_addrs },
	{ "PIC18F24J11",   8, gp_cfg_pic18lf44j11_addrs },
	{ "PIC18F24J50",   8, gp_cfg_pic18f24j50_addrs },
	{ "PIC18F24K20",  11, gp_cfg_pic18f23k20_addrs },
	{ "PIC18F24K22",  11, gp_cfg_pic18f26k22_addrs },
	{ "PIC18F2510",   11, gp_cfg_pic18f4610_addrs },
	{ "PIC18F2515",   11, gp_cfg_pic18f4515_addrs },
	{ "PIC18F252",    11, gp_cfg_pic18f452_addrs },
	{ "PIC18F2520",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F2523",   11, gp_cfg_pic18f2523_addrs },
	{ "PIC18F2525",   11, gp_cfg_pic18f2525_addrs },
	{ "PIC18F2539",   10, gp_cfg_pic18f2539_addrs },
	{ "PIC18F2550",   12, gp_cfg_pic18f2550_addrs },
	{ "PIC18F2553",   12, gp_cfg_pic18f2550_addrs },
	{ "PIC18F258",    10, gp_cfg_pic18f458_addrs },
	{ "PIC18F2580",   11, gp_cfg_pic18f2580_addrs },
	{ "PIC18F2585",   11, gp_cfg_pic18f2585_addrs },
	{ "PIC18F25J10",   5, gp_cfg_pic18f25j10_addrs },
	{ "PIC18F25J11",   8, gp_cfg_pic18lf45j11_addrs },
	{ "PIC18F25J50",   8, gp_cfg_pic18lf45j50_addrs },
	{ "PIC18F25K20",  11, gp_cfg_pic18f46k20_addrs },
	{ "PIC18F25K22",  11, gp_cfg_pic18f26k22_addrs },
	{ "PIC18F25K80",  12, gp_cfg_pic18lf25k80_addrs },
	{ "PIC18F2610",   11, gp_cfg_pic18f4610_addrs },
	{ "PIC18F2620",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F2680",   11, gp_cfg_pic18f2680_addrs },
	{ "PIC18F2682",   11, gp_cfg_pic18f4682_addrs },
	{ "PIC18F2685",   11, gp_cfg_pic18f4685_addrs },
	{ "PIC18F26J11",   8, gp_cfg_pic18f46j11_addrs },
	{ "PIC18F26J13",   8, gp_cfg_pic18f46j13_addrs },
	{ "PIC18F26J50",   8, gp_cfg_pic18f26j50_addrs },
	{ "PIC18F26J53",   8, gp_cfg_pic18f26j53_addrs },
	{ "PIC18F26K20",  11, gp_cfg_pic18f46k20_addrs },
	{ "PIC18F26K22",  11, gp_cfg_pic18f26k22_addrs },
	{ "PIC18F26K80",  12, gp_cfg_pic18lf25k80_addrs },
	{ "PIC18F27J13",   8, gp_cfg_pic18lf27j13_addrs },
	{ "PIC18F27J53",   8, gp_cfg_pic18f47j53_addrs },
	{ "PIC18F4220",   11, gp_cfg_pic18f4220_addrs },
	{ "PIC18F4221",   11, gp_cfg_pic18f4221_addrs },
	{ "PIC18F4320",   11, gp_cfg_pic18f2320_addrs },
	{ "PIC18F4321",   11, gp_cfg_pic18f4321_addrs },
	{ "PIC18F4331",   12, gp_cfg_pic18f4331_addrs },
	{ "PIC18F43K20",  11, gp_cfg_pic18f23k20_addrs },
	{ "PIC18F43K22",  11, gp_cfg_pic18f46k22_addrs },
	{ "PIC18F4410",   11, gp_cfg_pic18f2410_addrs },
	{ "PIC18F442",    11, gp_cfg_pic18f242_addrs },
	{ "PIC18F4420",   11, gp_cfg_pic18f4420_addrs },
	{ "PIC18F4423",   11, gp_cfg_pic18f2423_addrs },
	{ "PIC18F4431",   12, gp_cfg_pic18f4431_addrs },
	{ "PIC18F4439",   10, gp_cfg_pic18f2439_addrs },
	{ "PIC18F4450",   12, gp_cfg_pic18f4450_addrs },
	{ "PIC18F4455",   12, gp_cfg_pic18f4458_addrs },
	{ "PIC18F4458",   12, gp_cfg_pic18f4458_addrs },
	{ "PIC18F448",    10, gp_cfg_pic18f248_addrs },
	{ "PIC18F4480",   11, gp_cfg_pic18f2480_addrs },
	{ "PIC18F44J10",   5, gp_cfg_pic18f24j10_addrs },
	{ "PIC18F44J11",   8, gp_cfg_pic18lf44j11_addrs },
	{ "PIC18F44J50",   8, gp_cfg_pic18f24j50_addrs },
	{ "PIC18F44K20",  11, gp_cfg_pic18f23k20_addrs },
	{ "PIC18F44K22",  11, gp_cfg_pic18f46k22_addrs },
	{ "PIC18F4510",   11, gp_cfg_pic18f4610_addrs },
	{ "PIC18F4515",   11, gp_cfg_pic18f4515_addrs },
	{ "PIC18F452",    11, gp_cfg_pic18f452_addrs },
	{ "PIC18F4520",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F4523",   11, gp_cfg_pic18f2523_addrs },
	{ "PIC18F4525",   11, gp_cfg_pic18f2525_addrs },
	{ "PIC18F4539",   10, gp_cfg_pic18f2539_addrs },
	{ "PIC18F4550",   12, gp_cfg_pic18f4553_addrs },
	{ "PIC18F4553",   12, gp_cfg_pic18f4553_addrs },
	{ "PIC18F458",    10, gp_cfg_pic18f458_addrs },
	{ "PIC18F4580",   11, gp_cfg_pic18f2580_addrs },
	{ "PIC18F4585",   11, gp_cfg_pic18f2585_addrs },
	{ "PIC18F45J10",   5, gp_cfg_pic18f25j10_addrs },
	{ "PIC18F45J11",   8, gp_cfg_pic18lf45j11_addrs },
	{ "PIC18F45J50",   8, gp_cfg_pic18lf45j50_addrs },
	{ "PIC18F45K20",  11, gp_cfg_pic18f46k20_addrs },
	{ "PIC18F45K22",  11, gp_cfg_pic18f46k22_addrs },
	{ "PIC18F45K80",  12, gp_cfg_pic18lf25k80_addrs },
	{ "PIC18F4610",   11, gp_cfg_pic18f4610_addrs },
	{ "PIC18F4620",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F4680",   11, gp_cfg_pic18f2680_addrs },
	{ "PIC18F4682",   11, gp_cfg_pic18f4682_addrs },
	{ "PIC18F4685",   11, gp_cfg_pic18f4685_addrs },
	{ "PIC18F46J11",   8, gp_cfg_pic18f46j11_addrs },
	{ "PIC18F46J13",   8, gp_cfg_pic18f46j13_addrs },
	{ "PIC18F46J50",   8, gp_cfg_pic18f26j50_addrs },
	{ "PIC18F46J53",   8, gp_cfg_pic18f26j53_addrs },
	{ "PIC18F46K20",  11, gp_cfg_pic18f46k20_addrs },
	{ "PIC18F46K22",  11, gp_cfg_pic18f46k22_addrs },
	{ "PIC18F46K80",  12, gp_cfg_pic18lf25k80_addrs },
	{ "PIC18F47J13",   8, gp_cfg_pic18lf27j13_addrs },
	{ "PIC18F47J53",   8, gp_cfg_pic18f47j53_addrs },
	{ "PIC18F6310",    8, gp_cfg_pic18f6410_addrs },
	{ "PIC18F6390",    6, gp_cfg_pic18f6490_addrs },
	{ "PIC18F6393",    6, gp_cfg_pic18f6490_addrs },
	{ "PIC18F63J11",   6, gp_cfg_pic18f63j11_addrs },
	{ "PIC18F63J90",   6, gp_cfg_pic18f83j90_addrs },
	{ "PIC18F6410",    8, gp_cfg_pic18f6410_addrs },
	{ "PIC18F6490",    6, gp_cfg_pic18f6490_addrs },
	{ "PIC18F6493",    6, gp_cfg_pic18f6490_addrs },
	{ "PIC18F64J11",   6, gp_cfg_pic18f64j11_addrs },
	{ "PIC18F64J90",   6, gp_cfg_pic18f64j90_addrs },
	{ "PIC18F6520",   12, gp_cfg_pic18f6520_addrs },
	{ "PIC18F6525",   12, gp_cfg_pic18f6525_addrs },
	{ "PIC18F6527",   11, gp_cfg_pic18f6527_addrs },
	{ "PIC18F6585",   12, gp_cfg_pic18f6585_addrs },
	{ "PIC18F65J10",   6, gp_cfg_pic18f65j10_addrs },
	{ "PIC18F65J11",   6, gp_cfg_pic18f65j11_addrs },
	{ "PIC18F65J15",   6, gp_cfg_pic18f65j15_addrs },
	{ "PIC18F65J50",   6, gp_cfg_pic18f65j50_addrs },
	{ "PIC18F65J90",   6, gp_cfg_pic18f85j90_addrs },
	{ "PIC18F65K22",  13, gp_cfg_pic18f66k22_addrs },
	{ "PIC18F65K80",  12, gp_cfg_pic18lf66k80_addrs },
	{ "PIC18F65K90",  13, gp_cfg_pic18f66k22_addrs },
	{ "PIC18F6620",   12, gp_cfg_pic18f6620_addrs },
	{ "PIC18F6621",   12, gp_cfg_pic18f6621_addrs },
	{ "PIC18F6622",   11, gp_cfg_pic18f6622_addrs },
	{ "PIC18F6627",   11, gp_cfg_pic18f6628_addrs },
	{ "PIC18F6628",   11, gp_cfg_pic18f6628_addrs },
	{ "PIC18F6680",   12, gp_cfg_pic18f6680_addrs },
	{ "PIC18F66J10",   6, gp_cfg_pic18f66j10_addrs },
	{ "PIC18F66J11",   6, gp_cfg_pic18f66j11_addrs },
	{ "PIC18F66J15",   6, gp_cfg_pic18f66j15_addrs },
	{ "PIC18F66J16",   6, gp_cfg_pic18f66j16_addrs },
	{ "PIC18F66J50",   6, gp_cfg_pic18f66j50_addrs },
	{ "PIC18F66J55",   6, gp_cfg_pic18f66j55_addrs },
	{ "PIC18F66J60",   5, gp_cfg_pic18f66j60_addrs },
	{ "PIC18F66J65",   5, gp_cfg_pic18f66j65_addrs },
	{ "PIC18F66J90",   6, gp_cfg_pic18f66j93_addrs },
	{ "PIC18F66J93",   6, gp_cfg_pic18f66j93_addrs },
	{ "PIC18F66K22",  13, gp_cfg_pic18f66k22_addrs },
	{ "PIC18F66K80",  12, gp_cfg_pic18f66k80_addrs },
	{ "PIC18F66K90",  13, gp_cfg_pic18f66k22_addrs },
	{ "PIC18F6720",   12, gp_cfg_pic18f6720_addrs },
	{ "PIC18F6722",   11, gp_cfg_pic18f6722_addrs },
	{ "PIC18F6723",   11, gp_cfg_pic18f6722_addrs },
	{ "PIC18F67J10",   6, gp_cfg_pic18f67j10_addrs },
	{ "PIC18F67J11",   6, gp_cfg_pic18f67j11_addrs },
	{ "PIC18F67J50",   6, gp_cfg_pic18f67j50_addrs },
	{ "PIC18F67J60",   5, gp_cfg_pic18f67j60_addrs },
	{ "PIC18F67J90",   6, gp_cfg_pic18f67j93_addrs },
	{ "PIC18F67J93",   6, gp_cfg_pic18f67j93_addrs },
	{ "PIC18F67K22",  13, gp_cfg_pic18f67k22_addrs },
	{ "PIC18F67K90",  13, gp_cfg_pic18f67k22_addrs },
	{ "PIC18F8310",    8, gp_cfg_pic18f8410_addrs },
	{ "PIC18F8390",    6, gp_cfg_pic18f6490_addrs },
	{ "PIC18F8393",    6, gp_cfg_pic18f6490_addrs },
	{ "PIC18F83J11",   6, gp_cfg_pic18f83j11_addrs },
	{ "PIC18F83J90",   6, gp_cfg_pic18f83j90_addrs },
	{ "PIC18F8410",    8, gp_cfg_pic18f8410_addrs },
	{ "PIC18F8490",    6, gp_cfg_pic18f6490_addrs },
	{ "PIC18F8493",    6, gp_cfg_pic18f6490_addrs },
	{ "PIC18F84J11",   6, gp_cfg_pic18f84j11_addrs },
	{ "PIC18F84J90",   6, gp_cfg_pic18f64j90_addrs },
	{ "PIC18F8520",   12, gp_cfg_pic18f8520_addrs },
	{ "PIC18F8525",   12, gp_cfg_pic18f8525_addrs },
	{ "PIC18F8527",   12, gp_cfg_pic18f8527_addrs },
	{ "PIC18F8585",   12, gp_cfg_pic18f8585_addrs },
	{ "PIC18F85J10",   6, gp_cfg_pic18f85j10_addrs },
	{ "PIC18F85J11",   6, gp_cfg_pic18f85j11_addrs },
	{ "PIC18F85J15",   6, gp_cfg_pic18f85j15_addrs },
	{ "PIC18F85J50",   6, gp_cfg_pic18f85j50_addrs },
	{ "PIC18F85J90",   6, gp_cfg_pic18f85j90_addrs },
	{ "PIC18F85K22",  13, gp_cfg_pic18f85k22_addrs },
	{ "PIC18F85K90",  13, gp_cfg_pic18f85k90_addrs },
	{ "PIC18F8620",   12, gp_cfg_pic18f8620_addrs },
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
	{ "PIC18F86J60",   5, gp_cfg_pic18f86j60_addrs },
	{ "PIC18F86J65",   5, gp_cfg_pic18f86j65_addrs },
	{ "PIC18F86J72",   6, gp_cfg_pic18f66j93_addrs },
	{ "PIC18F86J90",   6, gp_cfg_pic18f66j93_addrs },
	{ "PIC18F86J93",   6, gp_cfg_pic18f66j93_addrs },
	{ "PIC18F86K22",  13, gp_cfg_pic18f85k22_addrs },
	{ "PIC18F86K90",  13, gp_cfg_pic18f85k90_addrs },
	{ "PIC18F8720",   12, gp_cfg_pic18f8720_addrs },
	{ "PIC18F8722",   12, gp_cfg_pic18f8722_addrs },
	{ "PIC18F8723",   12, gp_cfg_pic18f8722_addrs },
	{ "PIC18F87J10",   6, gp_cfg_pic18f87j10_addrs },
	{ "PIC18F87J11",   6, gp_cfg_pic18f87j11_addrs },
	{ "PIC18F87J50",   6, gp_cfg_pic18f87j50_addrs },
	{ "PIC18F87J60",   5, gp_cfg_pic18f87j60_addrs },
	{ "PIC18F87J72",   6, gp_cfg_pic18f67j93_addrs },
	{ "PIC18F87J90",   6, gp_cfg_pic18f67j93_addrs },
	{ "PIC18F87J93",   6, gp_cfg_pic18f67j93_addrs },
	{ "PIC18F87K22",  13, gp_cfg_pic18f87k22_addrs },
	{ "PIC18F87K90",  13, gp_cfg_pic18f87k90_addrs },
	{ "PIC18F96J60",   6, gp_cfg_pic18f96j60_addrs },
	{ "PIC18F96J65",   6, gp_cfg_pic18f96j65_addrs },
	{ "PIC18F97J60",   6, gp_cfg_pic18f97j60_addrs },
	{ "PIC18LF13K22", 11, gp_cfg_pic18lf14k22_addrs },
	{ "PIC18LF13K50", 12, gp_cfg_pic18f13k50_addrs },
	{ "PIC18LF14K22", 11, gp_cfg_pic18lf14k22_addrs },
	{ "PIC18LF14K50", 12, gp_cfg_pic18f13k50_addrs },
	{ "PIC18LF23K22", 11, gp_cfg_pic18f26k22_addrs },
	{ "PIC18LF24J11",  8, gp_cfg_pic18lf44j11_addrs },
	{ "PIC18LF24J50",  8, gp_cfg_pic18f24j50_addrs },
	{ "PIC18LF24K22", 11, gp_cfg_pic18f26k22_addrs },
	{ "PIC18LF25J11",  8, gp_cfg_pic18lf45j11_addrs },
	{ "PIC18LF25J50",  8, gp_cfg_pic18lf45j50_addrs },
	{ "PIC18LF25K22", 11, gp_cfg_pic18f26k22_addrs },
	{ "PIC18LF25K80", 12, gp_cfg_pic18lf25k80_addrs },
	{ "PIC18LF26J11",  8, gp_cfg_pic18f46j11_addrs },
	{ "PIC18LF26J13",  8, gp_cfg_pic18f46j13_addrs },
	{ "PIC18LF26J50",  8, gp_cfg_pic18f26j50_addrs },
	{ "PIC18LF26J53",  8, gp_cfg_pic18f26j53_addrs },
	{ "PIC18LF26K22", 11, gp_cfg_pic18f26k22_addrs },
	{ "PIC18LF26K80", 12, gp_cfg_pic18lf25k80_addrs },
	{ "PIC18LF27J13",  8, gp_cfg_pic18lf27j13_addrs },
	{ "PIC18LF27J53",  8, gp_cfg_pic18f47j53_addrs },
	{ "PIC18LF43K22", 11, gp_cfg_pic18f46k22_addrs },
	{ "PIC18LF44J11",  8, gp_cfg_pic18lf44j11_addrs },
	{ "PIC18LF44J50",  8, gp_cfg_pic18f24j50_addrs },
	{ "PIC18LF44K22", 11, gp_cfg_pic18f46k22_addrs },
	{ "PIC18LF45J11",  8, gp_cfg_pic18lf45j11_addrs },
	{ "PIC18LF45J50",  8, gp_cfg_pic18lf45j50_addrs },
	{ "PIC18LF45K22", 11, gp_cfg_pic18f46k22_addrs },
	{ "PIC18LF45K80", 12, gp_cfg_pic18lf25k80_addrs },
	{ "PIC18LF46J11",  8, gp_cfg_pic18f46j11_addrs },
	{ "PIC18LF46J13",  8, gp_cfg_pic18f46j13_addrs },
	{ "PIC18LF46J50",  8, gp_cfg_pic18f26j50_addrs },
	{ "PIC18LF46J53",  8, gp_cfg_pic18f26j53_addrs },
	{ "PIC18LF46K22", 11, gp_cfg_pic18f46k22_addrs },
	{ "PIC18LF46K80", 12, gp_cfg_pic18lf25k80_addrs },
	{ "PIC18LF47J13",  8, gp_cfg_pic18lf27j13_addrs },
	{ "PIC18LF47J53",  8, gp_cfg_pic18f47j53_addrs },
	{ "PIC18LF65K80", 12, gp_cfg_pic18lf66k80_addrs },
	{ "PIC18LF66K80", 12, gp_cfg_pic18lf66k80_addrs },
	{ "PICMCV20USB",  12, gp_cfg_pic18f13k50_addrs },
};


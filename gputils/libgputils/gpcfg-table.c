/*
 * This code is automatically generated from the MPASMWIN.EXE file.
 * This data is in the public domain. Use care in modifying.
 */

/*
 * since caches are enabled, directive and option structures may seem to include
 * references to other PICs. this is done by caching when two or more PICs contain
 * identical data.
 */

#include <stdlib.h>

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
	{ "45",        0x00 },
	{ "42",        0x08 },
	{ "27",        0x10 },
	{ "25",        0x18 },
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
	{ "PORTBE",    0x00 },
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

static struct gp_cfg_directive pic18f6410_30000c[] =
{ /* for 0x30000c */
	{ "EBTR",      0x01,  2, pic18f6410_pwrt_opts },
};

/* config addresses for PIC18F6410: */
static struct gp_cfg_addr gp_cfg_pic18f6410_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f6410_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x81,  3, pic18f6410_300005 },
	{ 0x300006, 0x81,  3, pic18f6410_300006 },
	{ 0x300008, 0x01,  1, pic18f6410_300008 },
	{ 0x30000c, 0x01,  1, pic18f6410_30000c },
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

static struct gp_cfg_option pic18f2525_borv_opts[] =
{ /* for BORV: */
	{ "46",        0x00 },
	{ "43",        0x08 },
	{ "28",        0x10 },
	{ "21",        0x18 },
};

static struct gp_cfg_directive pic18f2525_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOREN",     0x06,  4, pic18f6410_boren_opts },
	{ "BORV",      0x18,  4, pic18f2525_borv_opts },
};

static struct gp_cfg_option pic18f2525_pbaden_opts[] =
{ /* for PBADEN: */
	{ "OFF",       0x00 },
	{ "ON",        0x02 },
};

static struct gp_cfg_directive pic18f2525_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_ccp2mx_opts },
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
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2525_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x07,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x07,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x07,  3, pic18f2525_30000c },
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
	{ "FCMEM",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f4450_bor_opts[] =
{ /* for BOR: */
	{ "OFF",       0x00 },
	{ "SOFT",      0x02 },
	{ "ON_ACTIVE", 0x04 },
	{ "ON",        0x06 },
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
	{ "BORV",      0x18,  4, pic18f2525_borv_opts },
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
	{ "BB2K",      0x00 },
	{ "BB1K",      0x08 },
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
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300006, 0x01,  1, pic18c858_300006 },
};

/*
 * PIC18F4610
 */

static struct gp_cfg_directive pic18f4610_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_ccp2mx_opts },
};

static struct gp_cfg_option pic18f4610_enhcpu_opts[] =
{ /* for ENHCPU: */
	{ "OFF",       0x00 },
	{ "ON",        0x60 },
};

static struct gp_cfg_directive pic18f4610_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ENHCPU",    0x60,  2, pic18f4610_enhcpu_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_option pic18f4610_cp3_opts[] =
{ /* for CP3: */
	{ "ON",        0x00 },
	{ "OFF",       0x08 },
};

static struct gp_cfg_directive pic18f4610_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
	{ "CP3",       0x08,  2, pic18f4610_cp3_opts },
};

static struct gp_cfg_directive pic18f4610_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
	{ "WRT3",      0x08,  2, pic18f4610_cp3_opts },
};

static struct gp_cfg_directive pic18f4610_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBTR3",     0x08,  2, pic18f4610_cp3_opts },
};

/* config addresses for PIC18F4610: */
static struct gp_cfg_addr gp_cfg_pic18f4610_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  3, pic18f4610_300005 },
	{ 0x300006, 0x85,  4, pic18f4610_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0x40,  1, pic18f4450_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0x60,  2, pic18f4450_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F84J15
 */

static struct gp_cfg_directive pic18f84j15_005ff8[] =
{ /* for 0x005ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f84j15_005ff9[] =
{ /* for 0x005ff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_option pic18f84j15_fosc_opts[] =
{ /* for FOSC: */
	{ "HS",        0x00 },
	{ "HSPLL",     0x01 },
	{ "EC",        0x02 },
	{ "ECPLL",     0x03 },
};

static struct gp_cfg_directive pic18f84j15_005ffa[] =
{ /* for 0x005ffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f84j15_fosc_opts },
};

static struct gp_cfg_option pic18f84j15_wdtps_opts[] =
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

static struct gp_cfg_directive pic18f84j15_005ffb[] =
{ /* for 0x005ffb */
	{ "WDTPS",     0x0f, 16, pic18f84j15_wdtps_opts },
};

static struct gp_cfg_option pic18f84j15_bw_opts[] =
{ /* for BW: */
	{ "8",         0x00 },
	{ "16",        0x40 },
};

static struct gp_cfg_option pic18f84j15_mode_opts[] =
{ /* for MODE: */
	{ "MM",        0x00 },
	{ "XM12",      0x10 },
	{ "XM16",      0x20 },
	{ "XM20",      0x30 },
};

static struct gp_cfg_option pic18f84j15_eashft_opts[] =
{ /* for EASHFT: */
	{ "OFF",       0x00 },
	{ "ON",        0x08 },
};

static struct gp_cfg_directive pic18f84j15_005ffc[] =
{ /* for 0x005ffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f84j15_bw_opts },
	{ "MODE",      0x30,  4, pic18f84j15_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f84j15_eashft_opts },
};

static struct gp_cfg_option pic18f84j15_eccpmx_opts[] =
{ /* for ECCPMX: */
	{ "ALTERNATE", 0x00 },
	{ "DEFAULT",   0x02 },
};

static struct gp_cfg_option pic18f84j15_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "ALTERNATE", 0x00 },
	{ "DEFAULT",   0x01 },
};

static struct gp_cfg_directive pic18f84j15_005ffd[] =
{ /* for 0x005ffd */
	{ "ECCPMX",    0x02,  2, pic18f84j15_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F84J15: */
static struct gp_cfg_addr gp_cfg_pic18f84j15_addrs[] =
{
	{ 0x005ff8, 0xe1,  4, pic18f84j15_005ff8 },
	{ 0x005ff9, 0xf4,  1, pic18f84j15_005ff9 },
	{ 0x005ffa, 0xc7,  4, pic18f84j15_005ffa },
	{ 0x005ffb, 0xff,  1, pic18f84j15_005ffb },
	{ 0x005ffc, 0xf8,  4, pic18f84j15_005ffc },
	{ 0x005ffd, 0xf3,  2, pic18f84j15_005ffd },
};

/*
 * PIC18F4221
 */

static struct gp_cfg_option pic18f4221_osc_opts[] =
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

static struct gp_cfg_directive pic18f4221_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 10, pic18f4221_osc_opts },
	{ "FSCM",      0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f4221_bor_opts[] =
{ /* for BOR: */
	{ "OFF",       0x00 },
	{ "SOFT",      0x02 },
	{ "NOSLP",     0x04 },
	{ "ON",        0x06 },
};

static struct gp_cfg_option pic18f4221_borv_opts[] =
{ /* for BORV: */
	{ "45",        0x00 },
	{ "42",        0x08 },
	{ "27",        0x10 },
	{ "20",        0x18 },
};

static struct gp_cfg_directive pic18f4221_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x06,  4, pic18f4221_bor_opts },
	{ "BORV",      0x18,  4, pic18f4221_borv_opts },
};

static struct gp_cfg_option pic18f4221_lpt1osc_opts[] =
{ /* for LPT1OSC: */
	{ "HIGH",      0x00 },
	{ "LOW",       0x04 },
};

static struct gp_cfg_option pic18f4221_pbad_opts[] =
{ /* for PBAD: */
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
	{ "LPT1OSC",   0x04,  2, pic18f4221_lpt1osc_opts },
	{ "PBAD",      0x02,  2, pic18f4221_pbad_opts },
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
	{ "ICPORT",    0x08,  2, pic18f84j15_eashft_opts },
	{ "BBSIZ",     0x30,  2, pic18f4221_bbsiz_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f4221_30000b[] =
{ /* for 0x30000b */
	{ "WRTC",      0x20,  2, pic18f2525_wrtc_opts },
	{ "WRTB",      0x40,  2, pic18f2525_cpb_opts },
	{ "WRTD",      0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F4221: */
static struct gp_cfg_addr gp_cfg_pic18f4221_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4221_300001 },
	{ 0x300002, 0x1f,  3, pic18f4221_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x87,  4, pic18f4221_300005 },
	{ 0x300006, 0xfd,  6, pic18f4221_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f4221_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4220
 */

static struct gp_cfg_directive pic18f4220_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 10, pic18f4221_osc_opts },
	{ "FSCM",      0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f4220_borv_opts[] =
{ /* for BORV: */
	{ "45",        0x00 },
	{ "42",        0x04 },
	{ "27",        0x08 },
	{ "20",        0x0c },
};

static struct gp_cfg_directive pic18f4220_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x02,  2, pic18f2525_pbaden_opts },
	{ "BORV",      0x0c,  4, pic18f4220_borv_opts },
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
	{ "PBAD",      0x02,  2, pic18f4221_pbad_opts },
	{ "CCP2MX",    0x01,  4, pic18f4220_ccp2mx_opts },
};

static struct gp_cfg_directive pic18f4220_300006[] =
{ /* for 0x300006 */
	{ "STVR",      0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F4220: */
static struct gp_cfg_addr gp_cfg_pic18f4220_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f4220_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  3, pic18f4220_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F2520
 */

/* config addresses for PIC18F2520: */
static struct gp_cfg_addr gp_cfg_pic18f2520_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2525_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8525
 */

static struct gp_cfg_option pic18f8525_osc_opts[] =
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

static struct gp_cfg_directive pic18f8525_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 11, pic18f8525_osc_opts },
	{ "OSCS",      0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_option pic18f8525_mode_opts[] =
{ /* for MODE: */
	{ "EM",        0x00 },
	{ "MPB",       0x01 },
	{ "MP",        0x02 },
	{ "MC",        0x03 },
};

static struct gp_cfg_directive pic18f8525_300004[] =
{ /* for 0x300004 */
	{ "MODE",      0x03,  4, pic18f8525_mode_opts },
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_option pic18f8525_eccpmx_opts[] =
{ /* for ECCPMX: */
	{ "PORTH",     0x00 },
	{ "PORTE",     0x02 },
};

static struct gp_cfg_directive pic18f8525_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "ECCPMX",    0x02,  2, pic18f8525_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_ccp2mx_opts },
};

/* config addresses for PIC18F8525: */
static struct gp_cfg_addr gp_cfg_pic18f8525_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8525_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  2, pic18f8525_300004 },
	{ 0x300005, 0x83,  3, pic18f8525_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x07,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x07,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x07,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8520
 */

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

/* config addresses for PIC18F8520: */
static struct gp_cfg_addr gp_cfg_pic18f8520_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300004, 0x83,  2, pic18f8525_300004 },
	{ 0x300005, 0x01,  1, pic18f8520_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
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
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8720
 */

static struct gp_cfg_option pic18f8720_cp4_opts[] =
{ /* for CP4: */
	{ "ON",        0x00 },
	{ "OFF",       0x10 },
};

static struct gp_cfg_directive pic18f8720_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
	{ "CP3",       0x08,  2, pic18f4610_cp3_opts },
	{ "CP4",       0x10,  2, pic18f8720_cp4_opts },
	{ "CP5",       0x20,  2, pic18f2525_wrtc_opts },
	{ "CP6",       0x40,  2, pic18f2525_cpb_opts },
	{ "CP7",       0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f8720_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
	{ "WRT3",      0x08,  2, pic18f4610_cp3_opts },
	{ "WRT4",      0x10,  2, pic18f8720_cp4_opts },
	{ "WRT5",      0x20,  2, pic18f2525_wrtc_opts },
	{ "WRT6",      0x40,  2, pic18f2525_cpb_opts },
	{ "WRT7",      0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f8720_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBTR3",     0x08,  2, pic18f4610_cp3_opts },
	{ "EBTR4",     0x10,  2, pic18f8720_cp4_opts },
	{ "EBTR5",     0x20,  2, pic18f2525_wrtc_opts },
	{ "EBTR6",     0x40,  2, pic18f2525_cpb_opts },
	{ "EBTR7",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F8720: */
static struct gp_cfg_addr gp_cfg_pic18f8720_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300004, 0x83,  2, pic18f8525_300004 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0xff,  8, pic18f8720_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  8, pic18f8720_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
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
	{ "MODE",      0x03,  4, pic18f8525_mode_opts },
	{ "ADDRBW",    0x30,  4, pic18f8722_addrbw_opts },
	{ "DATABW",    0x40,  2, pic18f8722_databw_opts },
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f8722_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ECCPMX",    0x02,  2, pic18f8525_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_ccp2mx_opts },
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

/* config addresses for PIC18F8722: */
static struct gp_cfg_addr gp_cfg_pic18f8722_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xf3,  4, pic18f8722_300004 },
	{ 0x300005, 0x83,  4, pic18f8722_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0xff,  8, pic18f8720_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  8, pic18f8720_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F248
 */

static struct gp_cfg_directive pic18f248_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
};

static struct gp_cfg_directive pic18f248_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
};

static struct gp_cfg_directive pic18f248_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
};

/* config addresses for PIC18F248: */
static struct gp_cfg_addr gp_cfg_pic18f248_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  2, pic18f248_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f248_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f248_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4455
 */

static struct gp_cfg_directive pic18f4455_300000[] =
{ /* for 0x300000 */
	{ "PLLDIV",    0x07,  8, pic18f4450_plldiv_opts },
	{ "CPUDIV",    0x18,  4, pic18f4450_cpudiv_opts },
	{ "USBDIV",    0x20,  2, pic18f4450_usbdiv_opts },
};

static struct gp_cfg_directive pic18f4455_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x06,  4, pic18f4450_bor_opts },
	{ "BORV",      0x18,  4, pic18f2525_borv_opts },
	{ "VREGEN",    0x20,  2, pic18f4450_vregen_opts },
};

static struct gp_cfg_directive pic18f4455_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f4455_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ICPRT",     0x20,  2, pic18f4450_vregen_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F4455: */
static struct gp_cfg_addr gp_cfg_pic18f4455_addrs[] =
{
	{ 0x300000, 0x2f,  3, pic18f4455_300000 },
	{ 0x300001, 0x05,  3, pic18f4450_300001 },
	{ 0x300002, 0x0f,  4, pic18f4455_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x80,  4, pic18f4455_300005 },
	{ 0x300006, 0x85,  5, pic18f4455_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F2680
 */

static struct gp_cfg_option pic18f2680_osc_opts[] =
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
	{ "ERC1",      0x0a },
	{ "ERC",       0x0c },
};

static struct gp_cfg_directive pic18f2680_300001[] =
{ /* for 0x300001 */
	{ "OSC",       0x0f, 12, pic18f2680_osc_opts },
	{ "FCMENB",    0x40,  2, pic18f6410_fcmen_opts },
	{ "IESOB",     0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_option pic18f2680_bor_opts[] =
{ /* for BOR: */
	{ "OFF",       0x00 },
	{ "SBORENCTRL", 0x02 },
	{ "BOACTIVE",  0x04 },
	{ "BOHW",      0x06 },
};

static struct gp_cfg_directive pic18f2680_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x06,  4, pic18f2680_bor_opts },
	{ "BORV",      0x18,  4, pic18f4221_borv_opts },
};

static struct gp_cfg_option pic18f2680_bbsiz_opts[] =
{ /* for BBSIZ: */
	{ "1024",      0x00 },
	{ "2048",      0x10 },
	{ "4096",      0x20 },
};

static struct gp_cfg_directive pic18f2680_300006[] =
{ /* for 0x300006 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "BBSIZ",     0x30,  3, pic18f2680_bbsiz_opts },
	{ "LVP",       0x04,  2, pic18f6410_lpt1osc_opts },
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
};

/* config addresses for PIC18F2680: */
static struct gp_cfg_addr gp_cfg_pic18f2680_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2680_300001 },
	{ 0x300002, 0x1f,  3, pic18f2680_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x82,  3, pic18f4450_300005 },
	{ 0x300006, 0x85,  5, pic18f2680_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F2320
 */

/* config addresses for PIC18F2320: */
static struct gp_cfg_addr gp_cfg_pic18f2320_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f4220_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  3, pic18f4220_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
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
	{ "ICPORT",    0x08,  2, pic18f84j15_eashft_opts },
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
	{ 0x300005, 0x87,  4, pic18f4221_300005 },
	{ 0x300006, 0xfd,  6, pic18f2321_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f4221_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4420
 */

static struct gp_cfg_directive pic18f4420_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
};

static struct gp_cfg_directive pic18f4420_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
};

static struct gp_cfg_directive pic18f4420_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
};

/* config addresses for PIC18F4420: */
static struct gp_cfg_addr gp_cfg_pic18f4420_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  3, pic18f4610_300005 },
	{ 0x300006, 0x85,  4, pic18f4610_300006 },
	{ 0x300008, 0x07,  2, pic18f4420_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x07,  2, pic18f4420_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x07,  2, pic18f4420_30000c },
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

static struct gp_cfg_directive pic18f66j60_00fff9[] =
{ /* for 0x00fff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f66j60_00fffa[] =
{ /* for 0x00fffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f84j15_fosc_opts },
};

static struct gp_cfg_directive pic18f66j60_00fffb[] =
{ /* for 0x00fffb */
	{ "WDTPS",     0x0f, 16, pic18f84j15_wdtps_opts },
};

static struct gp_cfg_directive pic18f66j60_00fffd[] =
{ /* for 0x00fffd */
	{ "ETHLED",    0x04,  2, pic18f6410_lpt1osc_opts },
};

/* config addresses for PIC18F66J60: */
static struct gp_cfg_addr gp_cfg_pic18f66j60_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j60_00fff9 },
	{ 0x00fffa, 0xc7,  4, pic18f66j60_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j60_00fffb },
	{ 0x00fffd, 0xf4,  1, pic18f66j60_00fffd },
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
	{ "FOSC",      0x03,  4, pic18f84j15_fosc_opts },
};

static struct gp_cfg_directive pic18f66j65_017ffb[] =
{ /* for 0x017ffb */
	{ "WDTPS",     0x0f, 16, pic18f84j15_wdtps_opts },
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
	{ 0x017ffd, 0xf4,  1, pic18f66j65_017ffd },
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
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18LF2523
 */

static struct gp_cfg_option pic18lf2523_borv_opts[] =
{ /* for BORV: */
	{ "0",         0x00 },
	{ "1",         0x08 },
	{ "2",         0x10 },
	{ "3",         0x18 },
};

static struct gp_cfg_directive pic18lf2523_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOREN",     0x06,  4, pic18f6410_boren_opts },
	{ "BORV",      0x18,  4, pic18lf2523_borv_opts },
};

static struct gp_cfg_option pic18lf2523_ccp2mx_opts[] =
{ /* for CCP2MX: */
	{ "PORTB",     0x00 },
	{ "PORTC",     0x01 },
};

static struct gp_cfg_directive pic18lf2523_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "PBADEN",    0x02,  2, pic18f2525_pbaden_opts },
	{ "CCP2MX",    0x01,  2, pic18lf2523_ccp2mx_opts },
};

/* config addresses for PIC18LF2523: */
static struct gp_cfg_addr gp_cfg_pic18lf2523_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18lf2523_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18lf2523_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
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
	{ 0x00fff9, 0xf4,  1, pic18f66j60_00fff9 },
	{ 0x00fffa, 0xc7,  4, pic18f66j60_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j60_00fffb },
	{ 0x00fffd, 0xf7,  3, pic18f86j60_00fffd },
};

/*
 * PIC18F64J15
 */

static struct gp_cfg_directive pic18f64j15_005ffd[] =
{ /* for 0x005ffd */
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F64J15: */
static struct gp_cfg_addr gp_cfg_pic18f64j15_addrs[] =
{
	{ 0x005ff8, 0xe1,  4, pic18f84j15_005ff8 },
	{ 0x005ff9, 0xf4,  1, pic18f84j15_005ff9 },
	{ 0x005ffa, 0xc7,  4, pic18f84j15_005ffa },
	{ 0x005ffb, 0xff,  1, pic18f84j15_005ffb },
	{ 0x005ffd, 0xf1,  1, pic18f64j15_005ffd },
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
	{ "BORV",      0x0c,  4, pic18f4220_borv_opts },
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
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18C452
 */

/* config addresses for PIC18C452: */
static struct gp_cfg_addr gp_cfg_pic18c452_addrs[] =
{
	{ 0x300000, 0xff,  1, pic18c858_300000 },
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x01,  1, pic18c858_300006 },
};

/*
 * PIC18F4510
 */

static struct gp_cfg_directive pic18f4510_300009[] =
{ /* for 0x300009 */
	{ "CPB",       0x40,  2, pic18f2525_cpb_opts },
};

static struct gp_cfg_directive pic18f4510_30000b[] =
{ /* for 0x30000b */
	{ "WRTB",      0x40,  2, pic18f2525_cpb_opts },
	{ "WRTC",      0x20,  2, pic18f2525_wrtc_opts },
};

/* config addresses for PIC18F4510: */
static struct gp_cfg_addr gp_cfg_pic18f4510_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18f2525_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  1, pic18f4510_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  2, pic18f4510_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
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
	{ 0x300005, 0x83,  3, pic18f4610_300005 },
	{ 0x300006, 0x85,  4, pic18f4610_300006 },
	{ 0x300008, 0x07,  3, pic18f2525_300008 },
	{ 0x300009, 0x40,  1, pic18f4450_300009 },
	{ 0x30000a, 0x07,  3, pic18f2525_30000a },
	{ 0x30000b, 0x60,  2, pic18f4450_30000b },
	{ 0x30000c, 0x07,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F66J10
 */

static struct gp_cfg_directive pic18f66j10_00fff8[] =
{ /* for 0x00fff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f66j10_00fffd[] =
{ /* for 0x00fffd */
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F66J10: */
static struct gp_cfg_addr gp_cfg_pic18f66j10_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j10_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j60_00fff9 },
	{ 0x00fffa, 0xc7,  4, pic18f66j60_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j60_00fffb },
	{ 0x00fffd, 0xf1,  1, pic18f66j10_00fffd },
};

/*
 * PIC18F96J60
 */

static struct gp_cfg_directive pic18f96j60_00fffc[] =
{ /* for 0x00fffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f84j15_bw_opts },
	{ "MODE",      0x30,  4, pic18f84j15_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f84j15_eashft_opts },
};

/* config addresses for PIC18F96J60: */
static struct gp_cfg_addr gp_cfg_pic18f96j60_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j60_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j60_00fff9 },
	{ 0x00fffa, 0xc7,  4, pic18f66j60_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j60_00fffb },
	{ 0x00fffc, 0xf8,  4, pic18f96j60_00fffc },
	{ 0x00fffd, 0xf7,  3, pic18f86j60_00fffd },
};

/*
 * PIC18F8527
 */

static struct gp_cfg_directive pic18f8527_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "ECCPMX",    0x02,  2, pic18f8525_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18lf2523_ccp2mx_opts },
};

/* config addresses for PIC18F8527: */
static struct gp_cfg_addr gp_cfg_pic18f8527_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xf3,  4, pic18f8722_300004 },
	{ 0x300005, 0x83,  4, pic18f8527_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0x07,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x07,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x07,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8621
 */

/* config addresses for PIC18F8621: */
static struct gp_cfg_addr gp_cfg_pic18f8621_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8525_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  2, pic18f8525_300004 },
	{ 0x300005, 0x83,  3, pic18f8525_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8620
 */

/* config addresses for PIC18F8620: */
static struct gp_cfg_addr gp_cfg_pic18f8620_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300004, 0x83,  2, pic18f8525_300004 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8622
 */

/* config addresses for PIC18F8622: */
static struct gp_cfg_addr gp_cfg_pic18f8622_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xf3,  4, pic18f8722_300004 },
	{ 0x300005, 0x83,  4, pic18f8527_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8585
 */

static struct gp_cfg_directive pic18f8585_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "CCP2MX",    0x01,  2, pic18f6410_wdt_opts },
};

/* config addresses for PIC18F8585: */
static struct gp_cfg_addr gp_cfg_pic18f8585_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8525_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  2, pic18f8525_300004 },
	{ 0x300005, 0x80,  2, pic18f8585_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x07,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x07,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x07,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F2539
 */

static struct gp_cfg_directive pic18f2539_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f2539_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f2539_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
};

/* config addresses for PIC18F2539: */
static struct gp_cfg_addr gp_cfg_pic18f2539_addrs[] =
{
	{ 0x300001, 0x22,  1, pic18f2439_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  3, pic18f2539_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  3, pic18f2539_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  3, pic18f2539_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
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

static struct gp_cfg_directive pic18f67j60_01fff9[] =
{ /* for 0x01fff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f67j60_01fffa[] =
{ /* for 0x01fffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f84j15_fosc_opts },
};

static struct gp_cfg_directive pic18f67j60_01fffb[] =
{ /* for 0x01fffb */
	{ "WDTPS",     0x0f, 16, pic18f84j15_wdtps_opts },
};

static struct gp_cfg_directive pic18f67j60_01fffd[] =
{ /* for 0x01fffd */
	{ "ETHLED",    0x04,  2, pic18f6410_lpt1osc_opts },
};

/* config addresses for PIC18F67J60: */
static struct gp_cfg_addr gp_cfg_pic18f67j60_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f67j60_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j60_01fff9 },
	{ 0x01fffa, 0xc7,  4, pic18f67j60_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j60_01fffb },
	{ 0x01fffd, 0xf4,  1, pic18f67j60_01fffd },
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
	{ 0x300005, 0x83,  3, pic18f4610_300005 },
	{ 0x300006, 0x85,  4, pic18f4610_300006 },
	{ 0x300008, 0x07,  2, pic18f4420_300008 },
	{ 0x300009, 0xc0,  1, pic18f4510_300009 },
	{ 0x30000a, 0x07,  2, pic18f4420_30000a },
	{ 0x30000b, 0xe0,  2, pic18f4510_30000b },
	{ 0x30000c, 0x07,  2, pic18f4420_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18LF2423
 */

/* config addresses for PIC18LF2423: */
static struct gp_cfg_addr gp_cfg_pic18lf2423_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18lf2523_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  4, pic18lf2523_300005 },
	{ 0x300006, 0x85,  4, pic18f2525_300006 },
	{ 0x300008, 0x0f,  2, pic18f248_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f248_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f248_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F66J15
 */

static struct gp_cfg_directive pic18f66j15_017ff8[] =
{ /* for 0x017ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f66j15_017ffd[] =
{ /* for 0x017ffd */
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F66J15: */
static struct gp_cfg_addr gp_cfg_pic18f66j15_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j15_017ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f66j65_017ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f66j65_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffd, 0xf1,  1, pic18f66j15_017ffd },
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
	{ "BW",        0x40,  2, pic18f84j15_bw_opts },
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
 * PIC18F6585
 */

/* config addresses for PIC18F6585: */
static struct gp_cfg_addr gp_cfg_pic18f6585_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8525_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x80,  2, pic18f8585_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x07,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x07,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x07,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F25J10
 */

static struct gp_cfg_directive pic18f25j10_007ff8[] =
{ /* for 0x007ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f25j10_007ff9[] =
{ /* for 0x007ff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f25j10_007ffa[] =
{ /* for 0x007ffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f84j15_fosc_opts },
};

static struct gp_cfg_directive pic18f25j10_007ffb[] =
{ /* for 0x007ffb */
	{ "WDTPS",     0x0f, 16, pic18f84j15_wdtps_opts },
};

static struct gp_cfg_directive pic18f25j10_007ffd[] =
{ /* for 0x007ffd */
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F25J10: */
static struct gp_cfg_addr gp_cfg_pic18f25j10_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f25j10_007ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f25j10_007ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f25j10_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f25j10_007ffb },
	{ 0x007ffd, 0xf1,  1, pic18f25j10_007ffd },
};

/*
 * PIC18F97J60
 */

static struct gp_cfg_directive pic18f97j60_01fffc[] =
{ /* for 0x01fffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f84j15_bw_opts },
	{ "MODE",      0x30,  4, pic18f84j15_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f84j15_eashft_opts },
};

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
	{ 0x01fff9, 0xf4,  1, pic18f67j60_01fff9 },
	{ 0x01fffa, 0xc7,  4, pic18f67j60_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j60_01fffb },
	{ 0x01fffc, 0xf8,  4, pic18f97j60_01fffc },
	{ 0x01fffd, 0xf7,  3, pic18f97j60_01fffd },
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
	{ 0x300001, 0x07,  3, pic18f2680_300001 },
	{ 0x300002, 0x1f,  3, pic18f2680_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x82,  3, pic18f4450_300005 },
	{ 0x300006, 0x85,  5, pic18f2480_300006 },
	{ 0x300008, 0x0f,  2, pic18f248_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  2, pic18f248_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  2, pic18f248_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F65J15
 */

static struct gp_cfg_directive pic18f65j15_00bff8[] =
{ /* for 0x00bff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f65j15_00bff9[] =
{ /* for 0x00bff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f65j15_00bffa[] =
{ /* for 0x00bffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f84j15_fosc_opts },
};

static struct gp_cfg_directive pic18f65j15_00bffb[] =
{ /* for 0x00bffb */
	{ "WDTPS",     0x0f, 16, pic18f84j15_wdtps_opts },
};

static struct gp_cfg_directive pic18f65j15_00bffd[] =
{ /* for 0x00bffd */
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F65J15: */
static struct gp_cfg_addr gp_cfg_pic18f65j15_addrs[] =
{
	{ 0x00bff8, 0xe1,  4, pic18f65j15_00bff8 },
	{ 0x00bff9, 0xf4,  1, pic18f65j15_00bff9 },
	{ 0x00bffa, 0xc7,  4, pic18f65j15_00bffa },
	{ 0x00bffb, 0xff,  1, pic18f65j15_00bffb },
	{ 0x00bffd, 0xf1,  1, pic18f65j15_00bffd },
};

/*
 * PIC18F6722
 */

/* config addresses for PIC18F6722: */
static struct gp_cfg_addr gp_cfg_pic18f6722_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x81,  3, pic18f6410_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0xff,  8, pic18f8720_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  8, pic18f8720_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6720
 */

/* config addresses for PIC18F6720: */
static struct gp_cfg_addr gp_cfg_pic18f6720_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0xff,  8, pic18f8720_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0xff,  8, pic18f8720_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0xff,  8, pic18f8720_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F452
 */

/* config addresses for PIC18F452: */
static struct gp_cfg_addr gp_cfg_pic18f452_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300005, 0x01,  1, pic18f242_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8310
 */

static struct gp_cfg_directive pic18f8310_300004[] =
{ /* for 0x300004 */
	{ "PM",        0x03,  4, pic18f8525_mode_opts },
	{ "BW",        0x40,  2, pic18f84j15_bw_opts },
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F8310: */
static struct gp_cfg_addr gp_cfg_pic18f8310_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f6410_300001 },
	{ 0x300002, 0x1f,  3, pic18f6410_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xc3,  3, pic18f8310_300004 },
	{ 0x300005, 0x81,  3, pic18f6410_300005 },
	{ 0x300006, 0x81,  3, pic18f6410_300006 },
	{ 0x300008, 0x01,  1, pic18f6410_300008 },
	{ 0x30000c, 0x01,  1, pic18f6410_30000c },
};

/*
 * PIC18F458
 */

/* config addresses for PIC18F458: */
static struct gp_cfg_addr gp_cfg_pic18f458_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F8680
 */

/* config addresses for PIC18F8680: */
static struct gp_cfg_addr gp_cfg_pic18f8680_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8525_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x83,  2, pic18f8525_300004 },
	{ 0x300005, 0x80,  2, pic18f8585_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6520
 */

/* config addresses for PIC18F6520: */
static struct gp_cfg_addr gp_cfg_pic18f6520_addrs[] =
{
	{ 0x300001, 0x27,  2, pic18c858_300001 },
	{ 0x300002, 0x0f,  3, pic18c858_300002 },
	{ 0x300003, 0x0f,  2, pic18c858_300003 },
	{ 0x300005, 0x01,  1, pic18f8520_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6525
 */

/* config addresses for PIC18F6525: */
static struct gp_cfg_addr gp_cfg_pic18f6525_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8525_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  3, pic18f8525_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x07,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x07,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x07,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6527
 */

static struct gp_cfg_directive pic18f6527_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
	{ "LPT1OSC",   0x04,  2, pic18f6410_lpt1osc_opts },
	{ "CCP2MX",    0x01,  2, pic18lf2523_ccp2mx_opts },
};

/* config addresses for PIC18F6527: */
static struct gp_cfg_addr gp_cfg_pic18f6527_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x81,  3, pic18f6527_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0x07,  3, pic18f2525_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x07,  3, pic18f2525_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x07,  3, pic18f2525_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F4480
 */

/* config addresses for PIC18F4480: */
static struct gp_cfg_addr gp_cfg_pic18f4480_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2680_300001 },
	{ 0x300002, 0x1f,  3, pic18f2680_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x82,  3, pic18f4450_300005 },
	{ 0x300006, 0x85,  5, pic18f2480_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F1220
 */

static struct gp_cfg_directive pic18f1220_300005[] =
{ /* for 0x300005 */
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
};

/* config addresses for PIC18F1220: */
static struct gp_cfg_addr gp_cfg_pic18f1220_addrs[] =
{
	{ 0x300001, 0xcf,  3, pic18f4220_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x80,  1, pic18f1220_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6680
 */

/* config addresses for PIC18F6680: */
static struct gp_cfg_addr gp_cfg_pic18f6680_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8525_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x80,  2, pic18f8585_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F96J65
 */

static struct gp_cfg_directive pic18f96j65_017ffc[] =
{ /* for 0x017ffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f84j15_bw_opts },
	{ "MODE",      0x30,  4, pic18f84j15_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f84j15_eashft_opts },
};

/* config addresses for PIC18F96J65: */
static struct gp_cfg_addr gp_cfg_pic18f96j65_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j65_017ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f66j65_017ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f66j65_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffc, 0xf8,  4, pic18f96j65_017ffc },
	{ 0x017ffd, 0xf7,  3, pic18f86j65_017ffd },
};

/*
 * PIC18F1330
 */

static struct gp_cfg_directive pic18f1330_300002[] =
{ /* for 0x300002 */
	{ "PWRT",      0x01,  2, pic18f6410_pwrt_opts },
	{ "BOR",       0x06,  4, pic18f2680_bor_opts },
	{ "BORV",      0x0c,  4, pic18f4220_borv_opts },
};

static struct gp_cfg_option pic18f1330_lpol_opts[] =
{ /* for LPOL: */
	{ "LOW",       0x00 },
	{ "HIGH",      0x04 },
};

static struct gp_cfg_directive pic18f1330_300004[] =
{ /* for 0x300004 */
	{ "HPOL",      0x08,  2, pic18f2331_lpol_opts },
	{ "LPOL",      0x04,  2, pic18f1330_lpol_opts },
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
	{ "T1OSCMX",   0x00,  2, pic18f2331_lpol_opts },
	{ "MCLRE",     0x80,  2, pic18f6410_ieso_opts },
};

static struct gp_cfg_directive pic18f1330_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "ENICPORT",  0x08,  2, pic18f84j15_eashft_opts },
	{ "BBSIZ",     0x30,  3, pic18f2321_bbsiz_opts },
	{ "XINST",     0x00,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

static struct gp_cfg_directive pic18f1330_30000b[] =
{ /* for 0x30000b */
	{ "WRTB",      0x40,  2, pic18f2525_cpb_opts },
	{ "WRTC",      0x20,  2, pic18f2525_wrtc_opts },
	{ "WRTD",      0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F1330: */
static struct gp_cfg_addr gp_cfg_pic18f1330_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4221_300001 },
	{ 0x300002, 0x1f,  3, pic18f1330_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x0e,  3, pic18f1330_300004 },
	{ 0x300005, 0x81,  3, pic18f1330_300005 },
	{ 0x300006, 0x81,  5, pic18f1330_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xc0,  3, pic18f1330_30000b },
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
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x81,  3, pic18f6527_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F6621
 */

/* config addresses for PIC18F6621: */
static struct gp_cfg_addr gp_cfg_pic18f6621_addrs[] =
{
	{ 0x300001, 0x2f,  2, pic18f8525_300001 },
	{ 0x300002, 0x0f,  3, pic18f4220_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x83,  3, pic18f8525_300005 },
	{ 0x300006, 0x85,  3, pic18f4220_300006 },
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F67J10
 */

static struct gp_cfg_directive pic18f67j10_01fff8[] =
{ /* for 0x01fff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f67j10_01fffd[] =
{ /* for 0x01fffd */
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F67J10: */
static struct gp_cfg_addr gp_cfg_pic18f67j10_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f67j10_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j60_01fff9 },
	{ 0x01fffa, 0xc7,  4, pic18f67j60_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j60_01fffb },
	{ 0x01fffd, 0xf1,  1, pic18f67j10_01fffd },
};

/*
 * PIC18F86J15
 */

static struct gp_cfg_directive pic18f86j15_017ffd[] =
{ /* for 0x017ffd */
	{ "ECCPMX",    0x02,  2, pic18f84j15_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F86J15: */
static struct gp_cfg_addr gp_cfg_pic18f86j15_addrs[] =
{
	{ 0x017ff8, 0xe1,  4, pic18f66j15_017ff8 },
	{ 0x017ff9, 0xf4,  1, pic18f66j65_017ff9 },
	{ 0x017ffa, 0xc7,  4, pic18f66j65_017ffa },
	{ 0x017ffb, 0xff,  1, pic18f66j65_017ffb },
	{ 0x017ffc, 0xf8,  4, pic18f96j65_017ffc },
	{ 0x017ffd, 0xf3,  2, pic18f86j15_017ffd },
};

/*
 * PIC18F1231
 */

static struct gp_cfg_directive pic18f1231_300006[] =
{ /* for 0x300006 */
	{ "STVREN",    0x01,  2, pic18f6410_wdt_opts },
	{ "ENICPORT",  0x08,  2, pic18f84j15_eashft_opts },
	{ "BBSIZ",     0x30,  2, pic18f4221_bbsiz_opts },
	{ "XINST",     0x00,  2, pic18f6410_fcmen_opts },
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
};

/* config addresses for PIC18F1231: */
static struct gp_cfg_addr gp_cfg_pic18f1231_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f4221_300001 },
	{ 0x300002, 0x1f,  3, pic18f1330_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0x0e,  3, pic18f1330_300004 },
	{ 0x300005, 0x81,  3, pic18f1330_300005 },
	{ 0x300006, 0x81,  5, pic18f1231_300006 },
	{ 0x300008, 0x03,  2, pic18f4450_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x03,  2, pic18f4450_30000a },
	{ 0x30000b, 0xc0,  3, pic18f1330_30000b },
	{ 0x30000c, 0x03,  2, pic18f4450_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F86J10
 */

static struct gp_cfg_directive pic18f86j10_00fffd[] =
{ /* for 0x00fffd */
	{ "ECCPMX",    0x02,  2, pic18f84j15_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F86J10: */
static struct gp_cfg_addr gp_cfg_pic18f86j10_addrs[] =
{
	{ 0x00fff8, 0xe1,  4, pic18f66j10_00fff8 },
	{ 0x00fff9, 0xf4,  1, pic18f66j60_00fff9 },
	{ 0x00fffa, 0xc7,  4, pic18f66j60_00fffa },
	{ 0x00fffb, 0xff,  1, pic18f66j60_00fffb },
	{ 0x00fffc, 0xf8,  4, pic18f96j60_00fffc },
	{ 0x00fffd, 0xf3,  2, pic18f86j10_00fffd },
};

/*
 * PIC18F85J15
 */

static struct gp_cfg_directive pic18f85j15_00bffc[] =
{ /* for 0x00bffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f84j15_bw_opts },
	{ "MODE",      0x30,  4, pic18f84j15_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f84j15_eashft_opts },
};

static struct gp_cfg_directive pic18f85j15_00bffd[] =
{ /* for 0x00bffd */
	{ "ECCPMX",    0x02,  2, pic18f84j15_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F85J15: */
static struct gp_cfg_addr gp_cfg_pic18f85j15_addrs[] =
{
	{ 0x00bff8, 0xe1,  4, pic18f65j15_00bff8 },
	{ 0x00bff9, 0xf4,  1, pic18f65j15_00bff9 },
	{ 0x00bffa, 0xc7,  4, pic18f65j15_00bffa },
	{ 0x00bffb, 0xff,  1, pic18f65j15_00bffb },
	{ 0x00bffc, 0xf8,  4, pic18f85j15_00bffc },
	{ 0x00bffd, 0xf3,  2, pic18f85j15_00bffd },
};

/*
 * PIC18F85J10
 */

static struct gp_cfg_directive pic18f85j10_007ffc[] =
{ /* for 0x007ffc */
	{ "WAIT",      0x80,  2, pic18f6410_debug_opts },
	{ "BW",        0x40,  2, pic18f84j15_bw_opts },
	{ "MODE",      0x30,  4, pic18f84j15_mode_opts },
	{ "EASHFT",    0x08,  2, pic18f84j15_eashft_opts },
};

static struct gp_cfg_directive pic18f85j10_007ffd[] =
{ /* for 0x007ffd */
	{ "ECCPMX",    0x02,  2, pic18f84j15_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F85J10: */
static struct gp_cfg_addr gp_cfg_pic18f85j10_addrs[] =
{
	{ 0x007ff8, 0xe1,  4, pic18f25j10_007ff8 },
	{ 0x007ff9, 0xf4,  1, pic18f25j10_007ff9 },
	{ 0x007ffa, 0xc7,  4, pic18f25j10_007ffa },
	{ 0x007ffb, 0xff,  1, pic18f25j10_007ffb },
	{ 0x007ffc, 0xf8,  4, pic18f85j10_007ffc },
	{ 0x007ffd, 0xf3,  2, pic18f85j10_007ffd },
};

/*
 * PIC18F24J10
 */

static struct gp_cfg_directive pic18f24j10_003ff8[] =
{ /* for 0x003ff8 */
	{ "DEBUG",     0x80,  2, pic18f6410_debug_opts },
	{ "XINST",     0x40,  2, pic18f6410_fcmen_opts },
	{ "STVREN",    0x20,  2, pic18f4450_vregen_opts },
	{ "WDTEN",     0x01,  2, pic18f6410_wdt_opts },
};

static struct gp_cfg_directive pic18f24j10_003ff9[] =
{ /* for 0x003ff9 */
	{ "CP0",       0x04,  2, pic18f2525_cp2_opts },
};

static struct gp_cfg_directive pic18f24j10_003ffa[] =
{ /* for 0x003ffa */
	{ "FCMEN",     0x40,  2, pic18f6410_fcmen_opts },
	{ "IESO",      0x80,  2, pic18f6410_ieso_opts },
	{ "FOSC2",     0x04,  2, pic18f6410_lpt1osc_opts },
	{ "FOSC",      0x03,  4, pic18f84j15_fosc_opts },
};

static struct gp_cfg_directive pic18f24j10_003ffb[] =
{ /* for 0x003ffb */
	{ "WDTPS",     0x0f, 16, pic18f84j15_wdtps_opts },
};

static struct gp_cfg_directive pic18f24j10_003ffd[] =
{ /* for 0x003ffd */
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F24J10: */
static struct gp_cfg_addr gp_cfg_pic18f24j10_addrs[] =
{
	{ 0x003ff8, 0xe1,  4, pic18f24j10_003ff8 },
	{ 0x003ff9, 0xf4,  1, pic18f24j10_003ff9 },
	{ 0x003ffa, 0xc7,  4, pic18f24j10_003ffa },
	{ 0x003ffb, 0xff,  1, pic18f24j10_003ffb },
	{ 0x003ffd, 0xf1,  1, pic18f24j10_003ffd },
};

/*
 * PIC18F6627
 */

static struct gp_cfg_directive pic18f6627_300008[] =
{ /* for 0x300008 */
	{ "CP0",       0x01,  2, pic18f6410_pwrt_opts },
	{ "CP1",       0x02,  2, pic18f2525_cp1_opts },
	{ "CP2",       0x04,  2, pic18f2525_cp2_opts },
	{ "CP3",       0x08,  2, pic18f4610_cp3_opts },
	{ "CP4",       0x10,  2, pic18f8720_cp4_opts },
	{ "CP5",       0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_directive pic18f6627_30000a[] =
{ /* for 0x30000a */
	{ "WRT0",      0x01,  2, pic18f6410_pwrt_opts },
	{ "WRT1",      0x02,  2, pic18f2525_cp1_opts },
	{ "WRT2",      0x04,  2, pic18f2525_cp2_opts },
	{ "WRT3",      0x08,  2, pic18f4610_cp3_opts },
	{ "WRT4",      0x10,  2, pic18f8720_cp4_opts },
	{ "WRT5",      0x20,  2, pic18f2525_wrtc_opts },
};

static struct gp_cfg_directive pic18f6627_30000c[] =
{ /* for 0x30000c */
	{ "EBTR0",     0x01,  2, pic18f6410_pwrt_opts },
	{ "EBTR1",     0x02,  2, pic18f2525_cp1_opts },
	{ "EBTR2",     0x04,  2, pic18f2525_cp2_opts },
	{ "EBTR3",     0x08,  2, pic18f4610_cp3_opts },
	{ "EBTR4",     0x10,  2, pic18f8720_cp4_opts },
	{ "EBTR5",     0x20,  2, pic18f2525_wrtc_opts },
};

/* config addresses for PIC18F6627: */
static struct gp_cfg_addr gp_cfg_pic18f6627_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300005, 0x81,  3, pic18f6410_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0x3f,  6, pic18f6627_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x3f,  6, pic18f6627_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x3f,  6, pic18f6627_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
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
	{ 0x300008, 0x0f,  4, pic18f4610_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x0f,  4, pic18f4610_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x0f,  4, pic18f4610_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * PIC18F87J10
 */

static struct gp_cfg_directive pic18f87j10_01fffd[] =
{ /* for 0x01fffd */
	{ "ECCPMX",    0x02,  2, pic18f84j15_eccpmx_opts },
	{ "CCP2MX",    0x01,  2, pic18f84j15_ccp2mx_opts },
};

/* config addresses for PIC18F87J10: */
static struct gp_cfg_addr gp_cfg_pic18f87j10_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f67j10_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j60_01fff9 },
	{ 0x01fffa, 0xc7,  4, pic18f67j60_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j60_01fffb },
	{ 0x01fffc, 0xf8,  4, pic18f97j60_01fffc },
	{ 0x01fffd, 0xf3,  2, pic18f87j10_01fffd },
};

/*
 * PIC18F87J60
 */

/* config addresses for PIC18F87J60: */
static struct gp_cfg_addr gp_cfg_pic18f87j60_addrs[] =
{
	{ 0x01fff8, 0xe1,  4, pic18f67j60_01fff8 },
	{ 0x01fff9, 0xf4,  1, pic18f67j60_01fff9 },
	{ 0x01fffa, 0xc7,  4, pic18f67j60_01fffa },
	{ 0x01fffb, 0xff,  1, pic18f67j60_01fffb },
	{ 0x01fffd, 0xf7,  3, pic18f97j60_01fffd },
};

/*
 * PIC18F8627
 */

/* config addresses for PIC18F8627: */
static struct gp_cfg_addr gp_cfg_pic18f8627_addrs[] =
{
	{ 0x300001, 0x07,  3, pic18f2525_300001 },
	{ 0x300002, 0x1f,  3, pic18f2525_300002 },
	{ 0x300003, 0x1f,  2, pic18f6410_300003 },
	{ 0x300004, 0xf3,  4, pic18f8722_300004 },
	{ 0x300005, 0x83,  4, pic18f8722_300005 },
	{ 0x300006, 0x85,  5, pic18f8722_300006 },
	{ 0x300008, 0x3f,  6, pic18f6627_300008 },
	{ 0x300009, 0xc0,  2, pic18f2525_300009 },
	{ 0x30000a, 0x3f,  6, pic18f6627_30000a },
	{ 0x30000b, 0xe0,  3, pic18f2525_30000b },
	{ 0x30000c, 0x3f,  6, pic18f6627_30000c },
	{ 0x30000d, 0x40,  1, pic18f2525_30000d },
};

/*
 * Devices Table
 */

int gp_cfg_device_count = 127;
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
	{ "PIC18F1230",   12, gp_cfg_pic18f1231_addrs },
	{ "PIC18F1231",   12, gp_cfg_pic18f1231_addrs },
	{ "PIC18F1320",   11, gp_cfg_pic18f1220_addrs },
	{ "PIC18F1330",   12, gp_cfg_pic18f1330_addrs },
	{ "PIC18F1331",   12, gp_cfg_pic18f1330_addrs },
	{ "PIC18F2220",   11, gp_cfg_pic18f4220_addrs },
	{ "PIC18F2221",   11, gp_cfg_pic18f4221_addrs },
	{ "PIC18F2320",   11, gp_cfg_pic18f2320_addrs },
	{ "PIC18F2321",   11, gp_cfg_pic18f2321_addrs },
	{ "PIC18F2331",   12, gp_cfg_pic18f2331_addrs },
	{ "PIC18F2410",   11, gp_cfg_pic18f2410_addrs },
	{ "PIC18F242",    11, gp_cfg_pic18f242_addrs },
	{ "PIC18F2420",   11, gp_cfg_pic18f4420_addrs },
	{ "PIC18F2431",   12, gp_cfg_pic18f2331_addrs },
	{ "PIC18F2439",   10, gp_cfg_pic18f2439_addrs },
	{ "PIC18F2450",   12, gp_cfg_pic18f4450_addrs },
	{ "PIC18F2455",   12, gp_cfg_pic18f4455_addrs },
	{ "PIC18F248",    10, gp_cfg_pic18f248_addrs },
	{ "PIC18F2480",   11, gp_cfg_pic18f2480_addrs },
	{ "PIC18F24J10",   5, gp_cfg_pic18f24j10_addrs },
	{ "PIC18F2510",   11, gp_cfg_pic18f4510_addrs },
	{ "PIC18F2515",   11, gp_cfg_pic18f4515_addrs },
	{ "PIC18F252",    11, gp_cfg_pic18f452_addrs },
	{ "PIC18F2520",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F2525",   11, gp_cfg_pic18f2525_addrs },
	{ "PIC18F2539",   10, gp_cfg_pic18f2539_addrs },
	{ "PIC18F2550",   12, gp_cfg_pic18f4455_addrs },
	{ "PIC18F258",    10, gp_cfg_pic18f458_addrs },
	{ "PIC18F2580",   11, gp_cfg_pic18f4480_addrs },
	{ "PIC18F2585",   11, gp_cfg_pic18f2680_addrs },
	{ "PIC18F25J10",   5, gp_cfg_pic18f25j10_addrs },
	{ "PIC18F2610",   11, gp_cfg_pic18f4610_addrs },
	{ "PIC18F2620",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F2680",   11, gp_cfg_pic18f2680_addrs },
	{ "PIC18F4220",   11, gp_cfg_pic18f4220_addrs },
	{ "PIC18F4221",   11, gp_cfg_pic18f4221_addrs },
	{ "PIC18F4320",   11, gp_cfg_pic18f2320_addrs },
	{ "PIC18F4321",   11, gp_cfg_pic18f2321_addrs },
	{ "PIC18F4331",   12, gp_cfg_pic18f4331_addrs },
	{ "PIC18F4410",   11, gp_cfg_pic18f2410_addrs },
	{ "PIC18F442",    11, gp_cfg_pic18f242_addrs },
	{ "PIC18F4420",   11, gp_cfg_pic18f4420_addrs },
	{ "PIC18F4431",   12, gp_cfg_pic18f4331_addrs },
	{ "PIC18F4439",   10, gp_cfg_pic18f2439_addrs },
	{ "PIC18F4450",   12, gp_cfg_pic18f4450_addrs },
	{ "PIC18F4455",   12, gp_cfg_pic18f4455_addrs },
	{ "PIC18F448",    10, gp_cfg_pic18f248_addrs },
	{ "PIC18F4480",   11, gp_cfg_pic18f4480_addrs },
	{ "PIC18F44J10",   5, gp_cfg_pic18f24j10_addrs },
	{ "PIC18F4510",   11, gp_cfg_pic18f4510_addrs },
	{ "PIC18F4515",   11, gp_cfg_pic18f4515_addrs },
	{ "PIC18F452",    11, gp_cfg_pic18f452_addrs },
	{ "PIC18F4520",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F4525",   11, gp_cfg_pic18f2525_addrs },
	{ "PIC18F4539",   10, gp_cfg_pic18f2539_addrs },
	{ "PIC18F4550",   12, gp_cfg_pic18f4455_addrs },
	{ "PIC18F458",    10, gp_cfg_pic18f458_addrs },
	{ "PIC18F4580",   11, gp_cfg_pic18f4480_addrs },
	{ "PIC18F4585",   11, gp_cfg_pic18f2680_addrs },
	{ "PIC18F45J10",   5, gp_cfg_pic18f25j10_addrs },
	{ "PIC18F4610",   11, gp_cfg_pic18f4610_addrs },
	{ "PIC18F4620",   11, gp_cfg_pic18f2520_addrs },
	{ "PIC18F4680",   11, gp_cfg_pic18f2680_addrs },
	{ "PIC18F6310",    7, gp_cfg_pic18f6410_addrs },
	{ "PIC18F6390",    7, gp_cfg_pic18f6410_addrs },
	{ "PIC18F6410",    7, gp_cfg_pic18f6410_addrs },
	{ "PIC18F6490",    7, gp_cfg_pic18f6410_addrs },
	{ "PIC18F64J15",   5, gp_cfg_pic18f64j15_addrs },
	{ "PIC18F6520",   11, gp_cfg_pic18f6520_addrs },
	{ "PIC18F6525",   11, gp_cfg_pic18f6525_addrs },
	{ "PIC18F6527",   11, gp_cfg_pic18f6527_addrs },
	{ "PIC18F6585",   11, gp_cfg_pic18f6585_addrs },
	{ "PIC18F65J10",   5, gp_cfg_pic18f25j10_addrs },
	{ "PIC18F65J15",   5, gp_cfg_pic18f65j15_addrs },
	{ "PIC18F6620",   11, gp_cfg_pic18f452_addrs },
	{ "PIC18F6621",   11, gp_cfg_pic18f6621_addrs },
	{ "PIC18F6622",   11, gp_cfg_pic18f6622_addrs },
	{ "PIC18F6627",   11, gp_cfg_pic18f6627_addrs },
	{ "PIC18F6680",   11, gp_cfg_pic18f6680_addrs },
	{ "PIC18F66J10",   5, gp_cfg_pic18f66j10_addrs },
	{ "PIC18F66J15",   5, gp_cfg_pic18f66j15_addrs },
	{ "PIC18F66J60",   5, gp_cfg_pic18f66j60_addrs },
	{ "PIC18F66J65",   5, gp_cfg_pic18f66j65_addrs },
	{ "PIC18F6720",   11, gp_cfg_pic18f6720_addrs },
	{ "PIC18F6722",   11, gp_cfg_pic18f6722_addrs },
	{ "PIC18F67J10",   5, gp_cfg_pic18f67j10_addrs },
	{ "PIC18F67J60",   5, gp_cfg_pic18f67j60_addrs },
	{ "PIC18F8310",    8, gp_cfg_pic18f8310_addrs },
	{ "PIC18F8390",    7, gp_cfg_pic18f6410_addrs },
	{ "PIC18F8410",    8, gp_cfg_pic18f8310_addrs },
	{ "PIC18F8490",    7, gp_cfg_pic18f6410_addrs },
	{ "PIC18F84J15",   6, gp_cfg_pic18f84j15_addrs },
	{ "PIC18F8520",   12, gp_cfg_pic18f8520_addrs },
	{ "PIC18F8525",   12, gp_cfg_pic18f8525_addrs },
	{ "PIC18F8527",   12, gp_cfg_pic18f8527_addrs },
	{ "PIC18F8585",   12, gp_cfg_pic18f8585_addrs },
	{ "PIC18F85J10",   6, gp_cfg_pic18f85j10_addrs },
	{ "PIC18F85J15",   6, gp_cfg_pic18f85j15_addrs },
	{ "PIC18F8620",   12, gp_cfg_pic18f8620_addrs },
	{ "PIC18F8621",   12, gp_cfg_pic18f8621_addrs },
	{ "PIC18F8622",   12, gp_cfg_pic18f8622_addrs },
	{ "PIC18F8627",   12, gp_cfg_pic18f8627_addrs },
	{ "PIC18F8680",   12, gp_cfg_pic18f8680_addrs },
	{ "PIC18F86J10",   6, gp_cfg_pic18f86j10_addrs },
	{ "PIC18F86J15",   6, gp_cfg_pic18f86j15_addrs },
	{ "PIC18F86J60",   5, gp_cfg_pic18f86j60_addrs },
	{ "PIC18F86J65",   5, gp_cfg_pic18f86j65_addrs },
	{ "PIC18F8720",   12, gp_cfg_pic18f8720_addrs },
	{ "PIC18F8722",   12, gp_cfg_pic18f8722_addrs },
	{ "PIC18F87J10",   6, gp_cfg_pic18f87j10_addrs },
	{ "PIC18F87J60",   5, gp_cfg_pic18f87j60_addrs },
	{ "PIC18F96J60",   6, gp_cfg_pic18f96j60_addrs },
	{ "PIC18F96J65",   6, gp_cfg_pic18f96j65_addrs },
	{ "PIC18F97J60",   6, gp_cfg_pic18f97j60_addrs },
	{ "PIC18LF2423",  11, gp_cfg_pic18lf2423_addrs },
	{ "PIC18LF2523",  11, gp_cfg_pic18lf2523_addrs },
	{ "PIC18LF4423",  11, gp_cfg_pic18lf2423_addrs },
	{ "PIC18LF4523",  11, gp_cfg_pic18lf2523_addrs },
};


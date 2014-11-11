#!/usr/bin/perl -w

=back

    Copyright (C) 2012-2014 Molnar Karoly <molnarkaroly@users.sf.net>

    This file is part of gputils.

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

   -----------------------------------------------------------------------------------

    This program prepares the html lists from Config words and other properties
    of the PIC MCU-s. The data reads from the 8bit_device.info called file of MPLAB-X.
    On Linux is usually located on this path:

                /opt/microchip/mplabx/mpasmx/8bit_device.info

    Of course for the program it is necessary to the gputils source from the svn.

    $Id$
=cut

use strict;
use warnings;
no if $] >= 5.018, warnings => "experimental::smartmatch";        # perl 5.16
use 5.12.0;                             # when (regex)
use feature 'switch';
use POSIX 'strftime', 'ULONG_MAX';

use constant FALSE => 0;
use constant TRUE  => 1;

use constant ST_WAIT   => 0;
use constant ST_LISTEN => 1;

my $PROGRAM = 'device-help.pl';

my $verbose = 0;

my $gputils_url = 'gputils.sourceforge.net';
my $mplabx_url  = 'www.microchip.com/pagehandler/en-us/family/mplabx';

my $gputils_path  = "$ENV{HOME}/svn_snapshots/gputils/gputils";
my $gpprocessor_c = 'gpprocessor.c';
my $gpproc_path;
my $svn_rev = -1;

my $dev_info = '/opt/microchip/mplabx/mpasmx/8bit_device.info';
my $dev_info_rev = '';
my $list_file;

my $pic_name_mask = qr/PIC1(2(C[ER]?|HV)\d+\w+|6(C[ER]?|HV)\d+\w+|7C[R]?\d+\w+|[0268](C|L?F)\d+\w+)|PIC(RF\d+\w+|MCV\d+\w+)|PS\d+/;
my $header = ('=' x 70);

my $HTML_Doctype = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">';
my $remote_url = '';
my $out_dir    = 'html-help';
my $css        = 'main.css';
my $conf_tag   = 'conf';
my $feat_tag   = 'feat';
my $ram_tag    = 'ram';
my $sfr_tag    = 'sfr';
my $common_tag = 'common_sfrs';

my @fields;
my $mcu_name;
my $state;
my $config_mask;

my $addr;
my $mask;

use constant PROC_CLASS_PIC12  => 0;
use constant PROC_CLASS_PIC12E => 1;
use constant PROC_CLASS_PIC14  => 2;
use constant PROC_CLASS_PIC14E => 3;
use constant PROC_CLASS_PIC16  => 4;
use constant PROC_CLASS_PIC16E => 5;

use constant PIC12_BANK_SHIFT  => 5;
use constant PIC12_BANK_SIZE   => 2 ** PIC12_BANK_SHIFT;
use constant PIC14_BANK_SHIFT  => 7;
use constant PIC14_BANK_SIZE   => 2 ** PIC14_BANK_SHIFT;
use constant PIC16_BANK_SHIFT  => 8;
use constant PIC16_BANK_SIZE   => 2 ** PIC16_BANK_SHIFT;

my %class_features_p12 =
  (
  CLASS      => PROC_CLASS_PIC12,
  NAME       => '12 bit MCU',
  CSS_CLASS  => 'mcuAttrP12',
  CSS_BGRND  => '#FFB4B4',
  ENHANCED   => FALSE,
  PAGE_SIZE  => 512,
  WORD_SIZE  => 12,
  CONF_SIZE  => 12,
  EE_START   => 0,
  BANK_MAX   => 8,
  BANK_SIZE  => PIC12_BANK_SIZE,
  BANK_MASK  => ~(PIC12_BANK_SIZE - 1),
  BANK_SHIFT => PIC12_BANK_SHIFT,
  CORE_SFRS  => [
                0x00, 0x02, 0x03, 0x04
                ]
  );

my %class_features_p12e =
  (
  CLASS      => PROC_CLASS_PIC12E,
  NAME       => '12 bit enhanced MCU',
  CSS_CLASS  => 'mcuAttrP12E',
  CSS_BGRND  => '#FFB4FF',
  ENHANCED   => TRUE,
  PAGE_SIZE  => 512,
  WORD_SIZE  => 12,
  CONF_SIZE  => 12,
  EE_START   => 0,
  BANK_MAX   => 8,
  BANK_SIZE  => PIC12_BANK_SIZE,
  BANK_MASK  => ~(PIC12_BANK_SIZE - 1),
  BANK_SHIFT => PIC12_BANK_SHIFT,
  CORE_SFRS  => [
                0x00, 0x02, 0x03, 0x04
                ]
  );

my %class_features_p14 =
  (
  CLASS      => PROC_CLASS_PIC14,
  NAME       => '14 bit MCU',
  CSS_CLASS  => 'mcuAttrP14',
  CSS_BGRND  => '#B4B4FF',
  ENHANCED   => FALSE,
  PAGE_SIZE  => 2048,
  WORD_SIZE  => 14,
  CONF_SIZE  => 14,
  EE_START   => 0x2100,
  BANK_MAX   => 4,
  BANK_SIZE  => PIC14_BANK_SIZE,
  BANK_MASK  => ~(PIC14_BANK_SIZE - 1),
  BANK_SHIFT => PIC14_BANK_SHIFT,
  CORE_SFRS  => [
                0x00, 0x02, 0x03, 0x04,
                0x0A, 0x0B
                ]
  );

my %class_features_p14e =
  (
  CLASS      => PROC_CLASS_PIC14E,
  NAME       => '14 bit enhanced MCU',
  CSS_CLASS  => 'mcuAttrP14E',
  CSS_BGRND  => '#9BF0F0',
  ENHANCED   => TRUE,
  PAGE_SIZE  => 2048,
  WORD_SIZE  => 14,
  CONF_SIZE  => 16,
  EE_START   => 0xF000,
  BANK_MAX   => 32,
  BANK_SIZE  => PIC14_BANK_SIZE,
  BANK_MASK  => ~(PIC14_BANK_SIZE - 1),
  BANK_SHIFT => PIC14_BANK_SHIFT,
  CORE_SFRS  => [
                0x00, 0x01, 0x02, 0x03,
                0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B
                ]
  );

my %class_features_p16 =
  (
  CLASS      => PROC_CLASS_PIC16,
  NAME       => '16 bit MCU',
  CSS_CLASS  => 'mcuAttrP16',
  CSS_BGRND  => '#B4FFB4',
  ENHANCED   => FALSE,
  PAGE_SIZE  => 0,
  WORD_SIZE  => 16,
  CONF_SIZE  => 8,
  EE_START   => 0,
  BANK_MAX   => 9,
  BANK_SIZE  => PIC16_BANK_SIZE,
  BANK_MASK  => ~(PIC16_BANK_SIZE - 1),
  BANK_SHIFT => PIC16_BANK_SHIFT,
  CORE_SFRS  => [
                0x00, 0x01, 0x02, 0x03,
                0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B,
                0x0C, 0x0D, 0x0E, 0x0F,
                0x18, 0x19
                ]
  );

my %class_features_p16e =
  (
  CLASS      => PROC_CLASS_PIC16E,
  NAME       => '16 bit extended MCU',
  CSS_CLASS  => 'mcuAttrP16E',
  CSS_BGRND  => '#EBEB81',
  ENHANCED   => TRUE,
  PAGE_SIZE  => 0,
  WORD_SIZE  => 16,
  CONF_SIZE  => 8,
  EE_START   => 0xF00000,
  BANK_MAX   => 16,
  BANK_SIZE  => PIC16_BANK_SIZE,
  BANK_MASK  => ~(PIC16_BANK_SIZE - 1),
  BANK_SHIFT => PIC16_BANK_SHIFT,
  CORE_SFRS  => undef
  );

my @class_features_list =
  (
  \%class_features_p12,         # PROC_CLASS_PIC12
  \%class_features_p12e,        # PROC_CLASS_PIC12E
  \%class_features_p14,         # PROC_CLASS_PIC14
  \%class_features_p14e,        # PROC_CLASS_PIC14E
  \%class_features_p16,         # PROC_CLASS_PIC16
  \%class_features_p16e         # PROC_CLASS_PIC16E
  );

my %class_features_by_mpasmx =
  (
  '16c5x'  => \%class_features_p12,
  '16c5xe' => \%class_features_p12e,
  '16xxxx' => \%class_features_p14,
  '16exxx' => \%class_features_p14e,
  '17xxxx' => \%class_features_p16,
  '18xxxx' => \%class_features_p16e
  );

my @mcu_feat_names = sort {
                          $class_features_by_mpasmx{$a}->{ENHANCED} <=> $class_features_by_mpasmx{$b}->{ENHANCED} ||
                          $class_features_by_mpasmx{$a}->{CLASS}    <=> $class_features_by_mpasmx{$b}->{CLASS}
                          } keys %class_features_by_mpasmx;

#-----------------------------------------------------------------------------------------------------------------------

=back
        The structure of the %pic1.._common_SFRs hash:

        {
        'aaaaa' => {                    # Name of SFR.
                   ADDR   => 0,         # Full address.
                   NUMBER => 0          # The number of occurrences in the family.
                   },

                   .
                   .
                   .

        'zzzzz' => {}
        }
=cut

my %pic12_common_SFRs;
my $pic12_mcu_number = 0;

my %pic12e_common_SFRs;
my $pic12e_mcu_number = 0;

my %pic14_common_SFRs;
my $pic14_mcu_number = 0;

my %pic14e_common_SFRs;
my $pic14e_mcu_number = 0;

my %pic16_common_SFRs;
my $pic16_mcu_number = 0;

my %pic16e_common_SFRs;
my $pic16e_mcu_number = 0;

#-----------------------------------------------------------------------------------------------------------------------

=back
        The structure of one element of the %mcus_by_names hash:

        {
        FEATURES => {
                    MCU_NAME   => '',
                    MCU_CLASS  => 0,    # Class of MCU. (PROC_CLASS_PIC1yy)
                    ROM_SIZE   => 0,    # Size of program memory.

                    COFF       => 0,    # Coff ID of device. (16 bit wide)
                    PAGES      => 0,    # Number of ROM/FLASH pages.
                    MAX_RAM    => 0,    # The highest address of RAM.
                    RAM_SIZE   => 0,    # Full size of all SFR and GPR.
                    GPR_SIZE   => 0,    # Full size of all GPR.
                    SGPR_SIZE  => 0,    # Size of shared GPRs.
                    CONF_START => 0,    # Address of first Configuration byte/word.
                    CONF_END   => 0,    # Address of last Configuration byte/word.
                    STD_HEADER => '',   # Standard header file name of MCU.
                    STD_SCRIPT => '',   # Standard linker script file name of MCU.

                # These addresses relative, compared to the beginning of the blocks.
                    ROM        => 0,    # Last address of ROM/FLASH.
                    FLASHDATA  => 0,    # Last address of FLASH Data.
                    EEPROM     => 0,    # Last address of EEPROM.

                    DIR_COUNT  => 0,    # Number of Configuration bytes/words.
                    DIRECTIVES => [
                                    {
                                    DIR_ADDR    => 0x300005,
                                    DIR_MASK    => 0,
                                    DIR_DEFAULT => 0,
                                    SWITCHES    => [
                                                     {
                                                     HEAD    => 'CCP2MX',
                                                     NAME    => 'CCP2 MUX bit',
                                                     OPTIONS => [
                                                                  {
                                                                  NAME'   => 'OFF',
                                                                  VALUE'  => 0,
                                                                  EXPL'   => 'CCP2 input/output is multiplexed with RB3',
                                                                  DEFAULT => 0
                                                                  },

                                                                  ...

                                                                  {}
                                                                ],
                                                     SW_MASK => 1
                                                     },

                                                     ...

                                                     {}
                                                   ]
                                    },

                                    ...

                                    {}
                                  ],
                    CONF_MASK  => 0,    # Mask of the config words.
                    BANK_MAX   => 0,    # Maximum number of the RAM Banks.
                    BANK_NUM   => 0,    # Number of the RAM Banks.
                    BANK_SIZE  => 0,    # Size of a RAM Banks.
                    BANK_MASK  => 0,    # Mask of the RAM Bank address.
                    BANK_SHIFT => 0,    # Shift value of the RAM Bank address.
                    ACCESS     => 0,    # Last address of lower Access RAM of pic18f series.

                    OSCVAL     => {     # Oscillator Calibration Value.
                                  START => 0,
                                  END   => 0
                                  },

                    USERID     => {     # User ID.
                                  START => 0,
                                  END   => 0
                                  },

                    DEVID      => {     # Device ID.
                                  START => 0,
                                  END   => 0
                                  },

                    LINEARMEM  => {     # Linear RAM of the enhanced pic14 MCUs.
                                  NAME     => '',
                                  START    => 0,
                                  END      => 0,
                                  SEGMENTS => [
                                                {
                                                RSTART => 0,    # Real start of section.
                                                LSTART => 0,    # Logical start of section.
                                                SIZE   => 0     # Size of section.
                                                },

                                                ...

                                                {
                                                }
                                              ]
                                  },

                    SFR_RAM =>    [     # List of SFR sections.
                                    {
                                    START => 0,
                                    END   => 0
                                    },

                                    ...

                                    {
                                    }
                                  ],

                    GPR_RAM =>    [     # List of GPR sections.
                                    {
                                    START => 0,
                                    END   => 0
                                    },

                                    ...

                                    {
                                    }
                                  ],

                    COMMON_RAM => [     # List of common RAM sections.
                                    {
                                    START => 0,
                                    END   => 0
                                    },

                                    ...

                                    {
                                    }
                                  ],

                    MIRROR_RAM => [     # List of mirror RAM sections.
                                    {
                                    START => 0,
                                    END   => 0
                                    },

                                    ...

                                    {
                                    }
                                  ],

                    BAD_RAM    => [     # List of bad RAM sections.
                                    {
                                    START => 0,
                                    END   => 0
                                    },

                                    ...

                                    {
                                    }
                                  ],

                    SFRS       => [     # List of SFRs.
                                    {
                                    NAME => '',
                                    ADDR => 0
                                    },

                                    ...

                                    {
                                    }
                                  ],

                    SFR_NAMES  => {},   # List names of SFRs by addresses.
                    SFR_ADDRS  => {}    # List addresses of SFRs by names.
                    }
        }
=cut

my %mcus_by_names;

my %gp_mcus_by_names;

my $out_handler;

my $only_css = FALSE;

#---------------------------------------------------------------------------------------------------

use constant PRI_MENU_ALL         => 0;
use constant PRI_MENU_ENH         => 1;
use constant PRI_MENU_EXT         => 2;
use constant PRI_MENU_REG         => 3;
use constant PRI_MENU_12_BIT      => 4;
use constant PRI_MENU_14_BIT      => 5;
use constant PRI_MENU_16_BIT      => 6;
use constant PRI_MENU_RAM         => 7;
use constant PRI_MENU_ROM         => 8;
use constant PRI_MENU_EEPROM      => 9;
use constant PRI_COMMON_SFR       => 10;

my @primary_menu =
  (
    {                                   # PRI_MENU_ALL
    HREF  => 'index.html',
    NAME  => 'All',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_ALL
    },

    {                                   # PRI_MENU_ENH  (12 and 14 bit)
    HREF  => 'enhanced-mcus.html',
    NAME  => 'Enhanced',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_ENH
    },

    {                                   # PRI_MENU_EXT  (16 bit)
    HREF  => 'extended-mcus.html',
    NAME  => 'Extended',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_EXT
    },

    {                                   # PRI_MENU_REG
    HREF  => 'regular-mcus.html',
    NAME  => 'Regular',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_REG
    },

    {                                   # PRI_MENU_12_BIT
    HREF  => '12-bits-mcus.html',
    NAME  => '12 bits',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_12_BIT
    },

    {                                   # PRI_MENU_14_BIT
    HREF  => '14-bits-mcus.html',
    NAME  => '14 bits',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_14_BIT
    },

    {                                   # PRI_MENU_16_BIT
    HREF  => '16-bits-mcus.html',
    NAME  => '16 bits',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_16_BIT
    },

    {                                   # PRI_MENU_RAM
    HREF  => 'mcus-by-ram-size.html',
    NAME  => 'RAM<br>size',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_RAM
    },

    {                                   # PRI_MENU_ROM
    HREF  => 'mcus-by-rom-size.html',
    NAME  => 'ROM<br>size',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_ROM
    },

    {                                   # PRI_MENU_EEPROM
    HREF  => 'mcus-by-eeprom-size.html',
    NAME  => 'EEPROM<br>size',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_EEPROM
    },

    {                                   # PRI_COMMON_SFR
    HREF  => "pic12_$common_tag.html",
    NAME  => 'Common<br>SFRs',
    PFUNC => undef,
    CLASS => PRI_COMMON_SFR
    }
  );

#use constant COMMON_SFR_MENU_P12  => 0;
#use constant COMMON_SFR_MENU_P12E => 1;
#use constant COMMON_SFR_MENU_P14  => 2;
#use constant COMMON_SFR_MENU_P14E => 3;
#use constant COMMON_SFR_MENU_P16  => 4;
#use constant COMMON_SFR_MENU_P16E => 5;

my @common_sfr_menu =
  (
    {                                   # PROC_CLASS_PIC12
    HREF   => "pic12_$common_tag.html",
    NAME   => 'PIC12',
    HEAD   => 'PIC12 Common SFRs',
    PFUNC  => \&print_common_sfr_lists,
    PARAM0 => \%pic12_common_SFRs,
    PARAM1 => \$pic12_mcu_number,
    CLASS  => PROC_CLASS_PIC12
    },

    {                                   # PROC_CLASS_P12E
    HREF   => "pic12e_$common_tag.html",
    NAME   => 'PIC12E',
    HEAD   => 'PIC12E Common SFRs',
    PFUNC  => \&print_common_sfr_lists,
    PARAM0 => \%pic12e_common_SFRs,
    PARAM1 => \$pic12e_mcu_number,
    CLASS  => PROC_CLASS_PIC12E
    },

    {                                   # PROC_CLASS_P14
    HREF   => "pic14_$common_tag.html",
    NAME   => 'PIC14',
    HEAD   => 'PIC14 Common SFRs',
    PFUNC  => \&print_common_sfr_lists,
    PARAM0 => \%pic14_common_SFRs,
    PARAM1 => \$pic14_mcu_number,
    CLASS  => PROC_CLASS_PIC14
    },

    {                                   # PROC_CLASS_P14E
    HREF   => "pic14e_$common_tag.html",
    NAME   => 'PIC14E',
    HEAD   => 'PIC14E Common SFRs',
    PFUNC  => \&print_common_sfr_lists,
    PARAM0 => \%pic14e_common_SFRs,
    PARAM1 => \$pic14e_mcu_number,
    CLASS  => PROC_CLASS_PIC14E
    },

    {                                   # COMMON_SFR_P16
    HREF   => "pic16_$common_tag.html",
    NAME   => 'PIC16',
    HEAD   => 'PIC16 Common SFRs',
    PFUNC  => \&print_common_sfr_lists,
    PARAM0 => \%pic16_common_SFRs,
    PARAM1 => \$pic16_mcu_number,
    CLASS  => PROC_CLASS_PIC16
    },

    {                                   # PROC_CLASS_P16E
    HREF   => "pic16e_$common_tag.html",
    NAME   => 'PIC16E',
    HEAD   => 'PIC16E Common SFRs',
    PFUNC  => \&print_common_sfr_lists,
    PARAM0 => \%pic16e_common_SFRs,
    PARAM1 => \$pic16e_mcu_number,
    CLASS  => PROC_CLASS_PIC16E
    }
  );

use constant MCU_MENU_FEAT => 0;
use constant MCU_MENU_CONF => 1;
use constant MCU_MENU_RAM  => 2;
use constant MCU_MENU_SFR  => 3;

my @mcu_menu_elems =
  (
    {
    HREF  => "-$feat_tag.html",
    NAME  => 'Features',
    CLASS => MCU_MENU_FEAT
    },

    {
    HREF  => "-$conf_tag.html",
    NAME  => 'Configuration Bits',
    CLASS => MCU_MENU_CONF
    },

    {
    HREF  => "-$ram_tag.html",
    NAME  => 'RAM map',
    CLASS => MCU_MENU_RAM
    },

    {
    HREF  => "-$sfr_tag.html",
    NAME  => 'SFR map',
    CLASS => MCU_MENU_SFR
    }
  );

use constant RAM_BAD    => 0;
use constant RAM_GPR    => 1;
use constant RAM_SFR    => 2;
use constant RAM_COMMON => 3;
use constant RAM_MIRROR => 4;

my @mcu_missed_debug =
  (
  'PIC12F1571', 'PIC12LF1571', 'PIC12F1572', 'PIC12LF1572',
  'PIC12F1612', 'PIC12LF1612', 'PIC12F1822', 'PIC12LF1822',
  'PIC12F1840', 'PIC12LF1840', 'PIC12LF1840T39A', 'PIC12LF1840T48A',
  'PIC16F1454', 'PIC16LF1454', 'PIC16F1455', 'PIC16LF1455',
  'PIC16F1458', 'PIC16LF1458', 'PIC16F1508', 'PIC16LF1508',
  'PIC16F1509', 'PIC16LF1509', 'PIC16F1512', 'PIC16LF1512',
  'PIC16F1513', 'PIC16LF1513', 'PIC16F1516', 'PIC16LF1516',
  'PIC16F1517', 'PIC16LF1517', 'PIC16F1518', 'PIC16LF1518',
  'PIC16F1519', 'PIC16LF1519', 'PIC16F1526', 'PIC16LF1526',
  'PIC16F1527', 'PIC16LF1527', 'PIC16LF1554', 'PIC16LF1559',
  'PIC16F1613', 'PIC16LF1613', 'PIC16F1703', 'PIC16LF1703',
  'PIC16F1704', 'PIC16LF1704', 'PIC16F1705', 'PIC16LF1705',
  'PIC16F1707', 'PIC16LF1707', 'PIC16F1708', 'PIC16LF1708',
  'PIC16F1709', 'PIC16LF1709', 'PIC16F1713', 'PIC16LF1713',
  'PIC16F1716', 'PIC16LF1716', 'PIC16F1717', 'PIC16LF1717',
  'PIC16F1718', 'PIC16LF1718', 'PIC16F1719', 'PIC16LF1719',
  'PIC16F1764', 'PIC16LF1764', 'PIC16F1765', 'PIC16LF1765',
  'PIC16F1768', 'PIC16LF1768', 'PIC16F1769', 'PIC16LF1769',
  'PIC16F1782', 'PIC16LF1782', 'PIC16F1783', 'PIC16LF1783',
  'PIC16F1784', 'PIC16LF1784', 'PIC16F1786', 'PIC16LF1786',
  'PIC16F1787', 'PIC16LF1787', 'PIC16F1788', 'PIC16LF1788',
  'PIC16F1789', 'PIC16LF1789', 'PIC16F1823', 'PIC16LF1823',
  'PIC16F1824', 'PIC16LF1824', 'PIC16LF1824T39A', 'PIC16F1825',
  'PIC16LF1825', 'PIC16F1826', 'PIC16LF1826', 'PIC16F1827',
  'PIC16LF1827', 'PIC16F1828', 'PIC16LF1828', 'PIC16F1829',
  'PIC16F1829LIN', 'PIC16LF1829', 'PIC16F1847', 'PIC16LF1847',
  'PIC16LF1902', 'PIC16LF1903', 'PIC16LF1904', 'PIC16LF1906',
  'PIC16LF1907', 'PIC16F1933', 'PIC16LF1933', 'PIC16F1934',
  'PIC16LF1934', 'PIC16F1936', 'PIC16LF1936', 'PIC16F1937',
  'PIC16LF1937', 'PIC16F1938', 'PIC16LF1938', 'PIC16F1939',
  'PIC16LF1939', 'PIC16F1946', 'PIC16LF1946', 'PIC16F1947',
  'PIC16LF1947'
  );

# The Microchip created faulty database: 8bit_device.info
# Some parts of it are missing.

my %missed_directive_substitutions =
  (
  'PIC16LF707'  => {
                   DIR_ADDR  => 0x2008,
                   SUBST_MCU => 'PIC16F707'
                   },
  'PIC16LF722'  => {
                   DIR_ADDR  => 0x2008,
                   SUBST_MCU => 'PIC16F722'
                   },
  'PIC16LF722A' => {
                   DIR_ADDR  => 0x2008,
                   SUBST_MCU => 'PIC16F722A'
                   },
  'PIC16LF723'  => {
                   DIR_ADDR  => 0x2008,
                   SUBST_MCU => 'PIC16F723'
                   },
  'PIC16LF723A' => {
                   DIR_ADDR  => 0x2008,
                   SUBST_MCU => 'PIC16F723A'
                   },
  'PIC16LF724'  => {
                   DIR_ADDR  => 0x2008,
                   SUBST_MCU => 'PIC16F724'
                   },
  'PIC16LF726'  => {
                   DIR_ADDR  => 0x2008,
                   SUBST_MCU => 'PIC16F726'
                   },
  'PIC16LF727'  => {
                   DIR_ADDR  => 0x2008,
                   SUBST_MCU => 'PIC16F727'
                   }
  );

my @insufficient_mcus = ();

my @data_sheet_name_exception_list =
  (
  'PIC12LF1552',
  'PIC12LF1840T39A',
  'PIC12LF1840T48A',
  'PIC16HV540',
  'PIC16LF1554',
  'PIC16LF1559',
  'PIC16LF1824T39A',
  'PIC16LF1902',
  'PIC16LF1903',
  'PIC16LF1904',
  'PIC16LF1906',
  'PIC16LF1907'
  );

####################################################################################################
####################################################################################################

my @pp_def_names = ();          # Names of definitions.
my %pp_defines = ();            # Value of definitions.

my @pp_conditions = ();
my @pp_else_conditions = ();
my $pp_level = 0;               # Shows the lowest level.
my $pp_line_number;             # Line number of a lkr file.

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                             @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  This a simple preprocessor.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                             @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  # Examines that the parameter is defined or not defined.

sub _defined($)
  {
  return defined($pp_defines{$_[0]});
  }

#---------------------------------------------------------------------------------------------------

  # Records a definition.

sub define($)
  {
  my ($Name, $Body) = ($_[0] =~ /^(\S+)\s+(.+)$/o);

  die "define(): This definition already exists: \"$Name\"\n" if (_defined($Name));

  push(@pp_def_names, $Name);
        # This the location of name in the @pp_def_names array.
  $pp_defines{$Name}{INDEX} = $#pp_def_names;
        # (The definition is in fact unnecessary.)
  $pp_defines{$Name}{BODY}  = $Body;
  }

#---------------------------------------------------------------------------------------------------

  # Evaluation of the #if give a boolean value. This procedure preserves it.

sub if_condition($)
  {
  my $Val = $_[0];

  push(@pp_conditions, $Val);
  push(@pp_else_conditions, $Val);
  ++$pp_level;
  }

#---------------------------------------------------------------------------------------------------

  # Evaluation of the #else give a boolean value. This procedure preserves it.

sub else_condition($)
  {
  die "else_condition(): The ${pp_line_number}th line of $_[0] there is a #else, but does not belong him #if.\n" if ($pp_level <= 0);

  my $last = $#pp_conditions;

  if ($last > 0 && $pp_conditions[$last - 1])
    {
    $pp_conditions[$last] = ($pp_else_conditions[$#pp_else_conditions]) ? FALSE : TRUE;
    }
  else
    {
    $pp_conditions[$last] = FALSE;
    }
  }

#---------------------------------------------------------------------------------------------------

  # Closes a logical unit which starts with a #if.

sub endif_condition($)
  {
  die "endif_condition(): The ${pp_line_number}th line of $_[0] there is a #endif, but does not belong him #if.\n" if ($pp_level <= 0);

  pop(@pp_conditions);
  pop(@pp_else_conditions);
  --$pp_level;
  }

#---------------------------------------------------------------------------------------------------

sub reset_preprocessor()
  {
  @pp_def_names = ();
  %pp_defines = ();
  @pp_conditions = ();
  push(@pp_conditions, TRUE);
  @pp_else_conditions = ();
  push(@pp_else_conditions, FALSE);
  $pp_line_number = 1;
  $pp_level = 0;
  }

#---------------------------------------------------------------------------------------------------

        # This the preprocessor.

sub run_preprocessor($$$$)
  {
  my ($Fname, $Function, $Line, $Features) = @_;

  if ($Line =~ /^#\s*IFDEF\s+(\S+)$/io)
    {
    if ($pp_conditions[$#pp_conditions])
      {
        # The ancestor is valid, therefore it should be determined that
        # the descendants what kind.
      if_condition(_defined($1));
      }
    else
      {
        # The ancestor is invalid, so the descendants will invalid also.
      if_condition(FALSE);
      }
    }
  elsif ($Line =~ /^#\s*ELSE/io)
    {
    else_condition($Fname);
    }
  elsif ($Line =~ /^#\s*FI/io)
    {
    endif_condition($Fname);
    }
  elsif ($Line =~ /^#\s*DEFINE\s+(.+)$/io)
    {
        # This level is valid, so it should be recorded in the definition.
    define($1) if ($pp_conditions[$#pp_conditions]);
    }
  elsif ($pp_conditions[$#pp_conditions])
    {
        # This is a valid line. (The whole magic is in fact therefore there is.)
    $Function->($Line, $Features);
    }
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  Auxiliary procedures.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

sub basename($)
  {
  return ($_[0] =~ /([^\/]+)$/) ? $1 : '';
  }

#---------------------------------------------------------------------------------------------------

sub str2dec($)
  {
  my $Str = $_[0];

  return hex($1)   if ($Str =~ /^H'([[:xdigit:]]+)'$/io);
  return hex($1)   if ($Str =~ /^0x([[:xdigit:]]+)$/io);
  return int($Str) if ($Str =~ /^-?\d+$/o);

  die "This string not integer: \"$Str\"";
  }

#---------------------------------------------------------------------------------------------------

sub swap_reverse($$)
  {
  my ($Min, $Max) = @_;

  if (${$Min} > ${$Max})
    {
    my $t = ${$Min};

    ${$Min} = ${$Max};
    ${$Max} = $t;
    }
  }

#---------------------------------------------------------------------------------------------------

sub Log
  {
  return if (pop(@_) > $verbose);
  foreach (@_) { print STDERR $_; }
  print STDERR "\n";
  }

#---------------------------------------------------------------------------------------------------

sub Out
  {
  foreach (@_) { print $out_handler $_; }
  }

#---------------------------------------------------------------------------------------------------

sub Outl
  {
  Out(@_);
  print $out_handler "\n";
  }

#---------------------------------------------------------------------------------------------------

sub Outml
  {
  foreach (@_) { print $out_handler "$_\n"; }
  }

#---------------------------------------------------------------------------------------------------

sub aOut
  {
  my $Align = shift(@_);

  if (defined($Align))
    {
    $Align = int($Align);
    print $out_handler (' ' x $Align) if ($Align > 0);
    }

  foreach (@_) { print $out_handler $_; }
  }

#---------------------------------------------------------------------------------------------------

sub aOutl
  {
  aOut(@_);
  print $out_handler "\n";
  }

#---------------------------------------------------------------------------------------------------

sub aOutml
  {
  my $Align = shift(@_);

  if (defined($Align))
    {
    $Align = int($Align);
    $Align = ($Align > 0) ? (' ' x $Align) : '';
    }
  else
    {
    $Align = '';
    }

  foreach (@_) { print $out_handler "$Align$_\n"; }
  }

#---------------------------------------------------------------------------------------------------

sub Outf
  {
  printf $out_handler (shift(@_), @_);
  }

#---------------------------------------------------------------------------------------------------

sub Outfl
  {
  Outf(@_);
  print $out_handler "\n";
  }

#---------------------------------------------------------------------------------------------------

sub aOutf
  {
  my $Align = shift(@_);

  if (defined($Align))
    {
    $Align = int($Align);
    print $out_handler (' ' x $Align) if ($Align > 0);
    }

  printf $out_handler (shift(@_), @_);
  }

#---------------------------------------------------------------------------------------------------

sub aOutfl
  {
  aOutf(@_);
  print $out_handler "\n";
  }

#---------------------------------------------------------------------------------------------------

sub smartCompare($$)
  {
  my ($Str1, $Str2) = @_;

  if (${$Str1} =~ /^\d/o && ${$Str2} =~ /^\d/o)
    {
        # $Str1 number and $Str2 number
    return (int(${$Str1}) <=> int(${$Str2}));
    }

  return (${$Str1} cmp ${$Str2});
  }

#---------------------------------------------------------------------------------------------------

sub smartSort($$)
  {
  my @a_s = ($_[0] =~ /(\d+|\D+)/go);
  my @b_s = ($_[1] =~ /(\d+|\D+)/go);
  my ($i, $k, $end, $ret);

  $i = scalar(@a_s);
  $k = scalar(@b_s);

  if ($i < $k)
    {
    $end = $i;
    $ret = -1;
    }
  elsif ($i == $k)
    {
    $end = $i;
    $ret = 0;
    }
  else
    {
    $end = $k;
    $ret = 1;
    }

  for ($i = 0; $i < $end; ++$i)
    {
    $k = smartCompare(\$a_s[$i], \$b_s[$i]);

    return $k if ($k != 0);
    }

  return $ret;
  }

#---------------------------------------------------------------------------------------------------

        # Reads the content of the $list_file.

sub read_list_file()
  {
  open(LIST, '<', $list_file) || die "read_list_file(): Can not open. -> \"$list_file\"\n";

  %gp_mcus_by_names = ();

  while (<LIST>)
    {
    chomp;
    s/\r$//o;

        # Empty line.
    next if (/^\s*$/o);

    s/^\s*|\s*$//go;

        # Comment line.
    next if (/^#/o);

    my $name = lc($_);
    my $n;

    $name =~ s/^p(ic)?//o;
    $n = $name;

    $n = "p$n" if ($n !~ /^rf/o);
    $name = "pic$name" if ($name =~ /^rf/o);        # rfXXX -> picrfXXX
    $gp_mcus_by_names{uc($name)} = $n;
    }

  close(LIST);
  }

#---------------------------------------------------------------------------------------------------

        # Finds to all the MCU the corresponding inc file.

sub find_inc_files($)
  {
  my $Dir = $_[0];

  opendir(DIR, $Dir) || die "Can not open. -> \"$Dir\"\n";

  foreach (grep(-f "$Dir/$_" && /^\w+\.inc$/io, readdir(DIR)))
    {
    my $name = lc($_);
    my $n;

    $name =~ s/\.inc$//o;
    $n = $name;
    $name =~ s/^p//o;
    $name = uc("pic$name");

        # Remember the name of inc file;
    $gp_mcus_by_names{$name} = $n if (defined($gp_mcus_by_names{$name}));
    }

  closedir(DIR);
  }

#---------------------------------------------------------------------------------------------------

        # Reads SVN revision of the gputils source.

sub read_gp_svn_version()
  {
  chomp($svn_rev = qx/cd $gputils_path; svnversion/);
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  Handle the gpprocessor.c file.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        # Extract the MCU names from gpprocessor.c file.

sub extract_mcu_names()
  {
  open(LIB, '<', $gpproc_path) || die "extract_mcu_names(): Can not open. -> \"$gpproc_path\"\n";

        # static struct px pics[] = {
        # { PROC_CLASS_PIC14E   , "__16F1526"     , { "pic16f1526"     , "p16f1526"       , "16f1526"         }, 0x1526,  4,   32, 0x0F80, { 0x70, 0x7F },    -1, { 0x2000, 0x22EF }, 0x0FFF, 0x001FFF, 0x002000, {       -1,       -1 }, { 0x008000, 0x008003 }, { 0x008007, 0x008008 }, {       -1,       -1 }, "p16f1526.inc"     , "16f1526_g.lkr"     , 0 },
        # { PROC_CLASS_EEPROM8  , "__EEPROM8"     , { "eeprom8"        , "eeprom8"        , "eeprom8"         }, 0x1FFF,  0,    0, 0x0000, {   -1,   -1 },    -1, {     -1,     -1 },     -1, 0x0000FF,       -1, {       -1,       -1 }, {       -1,       -1 }, {       -1,       -1 }, {       -1,       -1 }, NULL               , NULL                , 0 },
        # { PROC_CLASS_PIC14    , "__RF675H"      , { "rf675h"         , "rf675h"         , "rf675h"          }, 0x4675,  1,    2, 0x0080, { 0x20, 0x5F },    -1, {     -1,     -1 }, 0x00DF, 0x00217F, 0x000400, { 0x0003FF, 0x0020FF }, { 0x002000, 0x002003 }, { 0x002007, 0x002007 }, { 0x002100, 0x00217F }, "rf675h.inc"       , "rf675h_g.lkr"      , 0 },

  my $in_table = FALSE;

  %gp_mcus_by_names = ();

  while (<LIB>)
    {
    chomp;

    if (! $in_table)
      {
      $in_table = TRUE if (/^\s*static\s+struct\s+px\s+pics\[\s*\]\s*=\s*\{\s*$/io);
      }
    elsif (/\{\s*PROC_CLASS_\w+\s*,\s*"\w+"\s*,\s*\{\s*"(\w+)"\s*,\s*"\w+"\s*,\s*"\w+"\s*}\s*,\s*[\w-]+\s*,\s*[\w-]+\s*,\s*[\w-]+\s*,\s*[\w-]+\s*,\s*\{\s*\S+\s*,\s*\S+\s*\}\s*,\s*\S+\s*,\s*\{\s*\S+\s*,\s*\S+\s*\}\s*,\s*\S+\s*,\s*\S+\s*,\s*\S+\s*,\s*\{\s*\S+\s*,\s*\S+\s*\}\s*,\s*{\s*\S+\s*,\s*\S+\s*\}\s*,\s*{\s*\S+\s*,\s*\S+\s*\}\s*,\s*{\s*\S+\s*,\s*\S+\s*\}\s*,\s*\"?[\.\w]+\"?\s*,\s*\"?[\.\w]+\"?\s*,\s*\d+\s*\}/io)
      {
      my $name = $1;

      if ($name ne 'pic16c5x' &&
          $name !~ /^pic1[6-8]cxx$/o &&
          $name !~ /^eeprom/o)
        {
        $name = "pic$name" if ($name =~ /^rf/o);        # rfXXX -> picrfXXX
        $gp_mcus_by_names{uc($name)} = '';
        }
      }
    else
      {
      last;
      }
    }

  close(LIB);
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  Handle the .inc files.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

use constant INC_NULL   => 0;
use constant INC_SFR    => 1;
use constant INC_BITS   => 2;
use constant INC_RAM    => 3;
use constant INC_CONFIG => 4;

my %pic17_conf_switch_expl =
  (
  'MODE'  => 'Processor Mode Select bits',
  'BODEN' => 'Brown-out Detect Enable',
  'OSC'   => 'Oscillator Select bits',
  'WDT'   => 'WDT Postscaler Select bits'
  );

        # Read the RAM features from the $Inc file.

sub read_ram_features($$$)
  {
  my ($Inc, $Features, $Class_pic16) = @_;
  my ($inc_path, $line, $full_ram, $state);
  my ($sfrs, $sfr_names, $sfr_addrs, $bad_ram);
  my $config_mask;
  my $directive   = undef;
  my $switch_info = undef;
  my $prev_switch_info_name = '';

  $inc_path = "$gputils_path/header/$Inc";
  open(INC, '<', $inc_path) || die "Could not open for reading: $inc_path\n";

  $Features->{STD_HEADER} = $Inc;
  Log("Read the RAM features from $inc_path.", 4);

  $full_ram    = 0;
  $config_mask = 0;   # For some PIC14E devices.
  $sfrs        = [];
  $sfr_names   = {};
  $sfr_addrs   = {};
  $bad_ram     = [];
  $state = INC_NULL;
  foreach (grep(! /^\s*$/o, <INC>))
    {
    chomp;
    s/\r$//o;
    s/^\s*|\s*$//go;
    $line = $_;

    given ($state)
      {
      when (INC_NULL)
        {
        $state = INC_SFR if ($line =~ /^;-+\s*Register\s+Files\s*-+$/io);
        }

      when (INC_SFR)
        {
        if ($line =~ /^;-+\s*(.+)Bits\s*-+$/io)
          {
          $state = INC_BITS;
          }
        elsif ($line =~ /^(\w+)\s+EQU\s+([\w']+)$/io)  #'
          {
          my ($name, $addr) = ($1, str2dec($2));

          $sfr_names->{$addr} = $name if (! defined($sfr_names->{$addr}));
          $sfr_addrs->{$name} = $addr if (! defined($sfr_names->{$name}));
          push(@{$sfrs}, { NAME => $name, ADDR => $addr });
          }
        }

      when (INC_BITS)
        {
        $state = INC_RAM if ($line =~ /^;\s*RAM\s+Definitions?$/io);
        }

      when (INC_RAM)
        {
        if ($line =~ /^__MAXRAM\s+H'([[:xdigit:]]+)'$/io)
          {
        # __MAXRAM  H'001F'
        # __MAXRAM  H'01FF'

          $full_ram = hex($1);
          $Features->{MAX_RAM} = $full_ram;
          ++$full_ram;
          }
        elsif ($line =~ /^__BADRAM\s+(.+)$/iop)
          {
        # __BADRAM  H'000F'
        # __BADRAM  H'0013'-H'0014'
        # __BADRAM  H'118'-H'1FF', H'218'-H'2FF', H'318'-H'3FF'
        # __BADRAM  H'0F9C'
        # __BADRAM  H'0FA3'-H'0FA5'

          foreach (split(/\s*,\s*/o, ${^POSTMATCH}))
            {
            if (/^H'([[:xdigit:]]+)'\s*-\s*H'([[:xdigit:]]+)'$/io)
              {
              my ($s, $e) = (hex($1), hex($2));

              swap_reverse(\$s, \$e);   # paranoia
              push(@{$bad_ram}, { START => $s, END => $e });
              $full_ram -= $e - $s + 1;
              }
            elsif (/^H'([[:xdigit:]]+)'$/io)
              {
              my $s = hex($1);

              push(@{$bad_ram}, { START => $s, END => $s });
              --$full_ram;
              }
            else
              {
              die "Unknown value in \"$inc_path\" file: $_\n";
              }
            } # foreach (split(/\s*,\s*/op, ${^POSTMATCH}))

          } # elsif ($line =~ /^__BADRAM\s+/io)
        elsif ($line =~ /^;\s*Configuration\s+Bits$/io)
          {
          if ($Class_pic16)
            {
            $directive = {
                         DIR_ADDR => 0xFE00,
                         SWITCHES => [],
                         DIR_MASK => 0
                         };

            push(@{$Features->{DIRECTIVES}}, $directive);
            }

          $state = INC_CONFIG;
          }
        } # when (INC_RAM)

      when (INC_CONFIG)
        {
        if ($line =~ /^_(DEVID\d*)\s+EQU\s+([\w']+)$/io)  #'
          {
          $state = INC_NULL;
          }
        elsif ($line =~ /^_(IDLOC\d*)\s+EQU\s+([\w']+)$/io)  #'
          {
          $state = INC_NULL;
          }
        elsif ($line !~ /^_CONFIG/io && $line =~ /^(\w+)\s+EQU\s+([\w']+)(.+)?$/io)  #'
          {
        # _PMC_MODE                       EQU     H'7FAF'
        # _BODEN_ON                       EQU     H'FFFF'
        # _WDT_256                        EQU     H'FFFB'
        # _XT_OSC                         EQU     H'FFFE'

          my ($name, $value) = ($1, str2dec($2));
          my $expl = '';

          $config_mask |= $value;

          if ($Class_pic16)
            {
        # For the PIC17Cxx devices.

            if (defined($3))
              {
              $expl = $3;
              $expl =~ s/\s*;\s*//;
              }

            foreach my $sw_name (keys %pic17_conf_switch_expl)
              {
              if ($name =~ /$sw_name/)
                {
                if ($prev_switch_info_name ne $sw_name)
                  {
                  $switch_info = {
                                 HEAD    => $sw_name,
                                 NAME    => $pic17_conf_switch_expl{$sw_name},
                                 OPTIONS => [],
                                 SW_MASK => 0xFFFF
                                 };

                  push(@{$directive->{SWITCHES}}, $switch_info);
                  $prev_switch_info_name = $sw_name;
                  }

                push(@{$switch_info->{OPTIONS}}, { NAME => $name, VALUE => $value, EXPL => '' });
                last;
                }
              } # foreach my $sw_name (keys %pic17_conf_switch_expl)
            } # if ($Class_pic16)
          } # elsif ($line =~ /^(\w+)\s+EQU\s+([\w']+)(.+)?$/io)
        } # when (INC_CONFIG)
      } # given ($state)
    } # foreach (grep(! /^\s*$/o, <INC>))

  close(INC);

  $Features->{RAM_SIZE}  = $full_ram;
  $Features->{BAD_RAM}   = $bad_ram;
  $Features->{SFR_NAMES} = $sfr_names;
  $Features->{SFR_ADDRS} = $sfr_addrs;

  @{$sfrs} = sort {$a->{ADDR} <=> $b->{ADDR}} @{$sfrs};
  $Features->{SFRS}      = $sfrs;
  $Features->{CONF_MASK} = $config_mask;
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  Process a lkr file.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        # Process one line of a lkr file.

sub process_lkr_line($$)
  {
  my ($Line, $Features) = @_;
  my ($section, $protected, $name, $start, $end, $size, $tail, $index);

  if ($Line =~ /^(\S+)\s+NAME=(\S+)\s+START=(\S+)\s+END=(\S+)/iop)
    {
    ($section, $name, $start, $end) = ($1, $2, str2dec($3), str2dec($4));
    $size = $end - $start + 1;
    $tail = ${^POSTMATCH};
    $tail =~ s/^\s+//o;
    swap_reverse(\$start, \$end);       # paranoia
    $protected = ($Line =~ /\s+PROTECTED\s*$/io) ? TRUE : FALSE;

    if ($section eq 'CODEPAGE')
      {
        # CODEPAGE   NAME=.config    START=0x8007            END=0x8008         PROTECTED
        # CODEPAGE   NAME=config     START=0xFE00            END=0xFE0F         PROTECTED
        # CODEPAGE   NAME=cfgmem     START=0x300000          END=0x30000D       PROTECTED

        # CODEPAGE   NAME=.oscval    START=0x3FF             END=0x3FF          PROTECTED
        # CODEPAGE   NAME=oscval     START=0x3FF             END=0x3FF          PROTECTED

        # CODEPAGE   NAME=.idlocs    START=0x2000            END=0x2003         PROTECTED
        # CODEPAGE   NAME=.idlocs    START=0x8000            END=0x8003         PROTECTED
        # CODEPAGE   NAME=userid     START=0x640             END=0x643          PROTECTED
        # CODEPAGE   NAME=.usrlocs   START=0x2000            END=0x2003         PROTECTED

        # CODEPAGE   NAME=.device_id START=0x2006            END=0x2006         PROTECTED
        # CODEPAGE   NAME=.devid     START=0x2006            END=0x2006         PROTECTED
        # CODEPAGE   NAME=devid      START=0x2006            END=0x2006         PROTECTED
        # CODEPAGE   NAME=.devid     START=0x8006            END=0x8006         PROTECTED
        # CODEPAGE   NAME=devid      START=0x3FFFFE          END=0x3FFFFF       PROTECTED

      given ($name)
        {
        when (/(config|cfgmem)$/io)
          {
        # For the PIC17Cxx devices.

          $Features->{CONF_START} = $start;
          $Features->{CONF_END}   = $end;
          }

        when (/oscval$/io)
          {
          $Features->{OSCVAL} = { START => $start, END => $end };
          }

        when (/(idlocs|userid|usrlocs)$/io)
          {
          $Features->{USERID} = { START => $start, END => $end };
          }

        when (/dev(ice_)?id$/io)
          {
          $Features->{DEVID} = { START => $start, END => $end };
          }

        when (/^page/io)
          {
          $Features->{ROM_SIZE} += $size;
          }
        } # given ($name)
      } # if ($section eq 'CODEPAGE')
    elsif ($section eq 'LINEARMEM' && $tail eq 'PROTECTED')
      {
        # LINEARMEM  NAME=linear0    START=0x2000            END=0x21EF         PROTECTED
        # LINEARMEM  NAME=linear0    START=0x2000            END=0x23EF         PROTECTED

      $Features->{LINEARMEM} = { NAME => $name, START => $start, END => $end, SEGMENTS => [] };
      }
    elsif ($section eq 'ACCESSBANK' && $name =~ /ram$/io)
      {
      $Features->{GPR_SIZE} += $size;
      $index = ($start & $Features->{BANK_MASK}) >> $Features->{BANK_SHIFT};
      push(@{$Features->{GPR_RAM}->[$index]}, { START => $start, END => $end });
      }
    elsif ($section eq 'DATABANK')
      {
      $index = ($start & $Features->{BANK_MASK}) >> $Features->{BANK_SHIFT};

      if ($name =~ /^sfr/io)
        {
        push(@{$Features->{SFR_RAM}->[$index]}, { START => $start, END => $end });
        }
      elsif ($name =~ /^([dg]pr|reg|usbport)/io)
        {
        # pic16c745
        # DATABANK   NAME=usbport    START=0x1A0             END=0x1DF          PROTECTED

        push(@{$Features->{GPR_RAM}->[$index]}, { START => $start, END => $end });
        $Features->{GPR_SIZE} += $size;

        if ($tail =~ /^SHADOW=(\w+):(\w+)$/io)
          {
          # DATABANK   NAME=gpr0       START=0x20              END=0x6F           SHADOW=linear0:0x2000
          # DATABANK   NAME=gpr4       START=0x220             END=0x26F          SHADOW=linear0:0x2140
          # DATABANK   NAME=gpr12      START=0x620             END=0x64F          SHADOW=linear0:0x23C0

          my ($lname, $lstart) = ($1, str2dec($2));
          my $linear = $Features->{LINEARMEM};

          die "Unknown linearmem name: $lname" if (! defined($linear->{NAME}) || $linear->{NAME} ne $lname);

          push(@{$linear->{SEGMENTS}}, { RSTART => $start, LSTART => $lstart, SIZE => $size });
          }
        }
      }
    elsif ($section eq 'SHAREBANK')
      {
      $index = ($start & $Features->{BANK_MASK}) >> $Features->{BANK_SHIFT};

      if ($name ne 'sfrs')
        {
        if ($protected)
          {
          # SHAREBANK  NAME=gprnobnk   START=0x27            END=0x2F           PROTECTED
          # SHAREBANK  NAME=gprnobank  START=0xF0            END=0xFF           PROTECTED
          # SHAREBANK  NAME=dprnobank  START=0x70              END=0x7F           PROTECTED
          # SHAREBANK  NAME=gpr0       START=0xA0            END=0xDF           PROTECTED
          # SHAREBANK  NAME=gpr0       START=0x8C            END=0xAF           PROTECTED

          # pic17xxx
          # SHAREBANK  NAME=sfrnobnk   START=0x300           END=0x30F          PROTECTED
          # SHAREBANK  NAME=registers START=0x31A    END=0x31F    PROTECTED

          if ($name =~ /^sfr(nobnk|prod)/io)
            {
            push(@{$Features->{SFR_RAM}->[$index]}, { START => $start, END => $end });
            }
          else
            {
            push(@{$Features->{MIRROR_RAM}->[$index]}, { START => $start, END => $end });
            }
          }
        else
          {
          # SHAREBANK  NAME=gprnobnk   START=0x7             END=0xF
          # SHAREBANK  NAME=gprnobank  START=0x70            END=0x7F
          # SHAREBANK  NAME=gpr0       START=0x20            END=0x5F
          # SHAREBANK  NAME=gpr0       START=0xC             END=0x2F
          # SHAREBANK  NAME=gpr0a      START=0xD             END=0xF
          # SHAREBANK  NAME=gprs       START=0x318           END=0x3FF

          # pic17xxx
          # SHAREBANK  NAME=registers START=0x1A     END=0x1F

          push(@{$Features->{COMMON_RAM}->[$index]}, { START => $start, END => $end });
          }
        }
      } # elsif ($section eq 'SHAREBANK')
    } # if ($Line =~ /^(\S+)\s+NAME=(\S+)\s+START=(\S+)\s+END=(\S+)/io)
  }

#---------------------------------------------------------------------------------------------------

sub init_ram_section($$)
  {
  my ($RamRef, $Size) = @_;
  my $i;

  for ($i = 0; $i < $Size; ++$i)
    {
    @{$RamRef->[$i]} = { START => -1, END => -1 };
    }
  }

#---------------------------------------------------------------------------------------------------

        # Read the RAM and ROM features from the $Lkr file.
        # The work was assisted by a very simple preprocessor.

sub read_ram_and_rom_features($$)
  {
  my ($Lkr, $Features) = @_;
  my $lkr_path;

  $lkr_path = "$gputils_path/lkr/$Lkr";
  open(LKR, '<', $lkr_path) || die "Could not open for reading: $lkr_path\n";

  $Features->{STD_SCRIPT} = $Lkr;
  Log("Read the RAM and ROM features from $lkr_path.", 4);

  init_ram_section($Features->{SFR_RAM},    $Features->{BANK_MAX});
  init_ram_section($Features->{GPR_RAM},    $Features->{BANK_MAX});
  init_ram_section($Features->{COMMON_RAM}, $Features->{BANK_MAX});
  init_ram_section($Features->{MIRROR_RAM}, $Features->{BANK_MAX});
  reset_preprocessor();

  while (<LKR>)
    {
    chomp;
    s/\r$//o;

    if ($_ !~ /^\s*$/o && $_ !~ m|^//|o)
      {
      s/^\s*|\s*$//go;
      run_preprocessor($Lkr, \&process_lkr_line, $_, $Features);
      }

    ++$pp_line_number;
    }

  close(LKR);
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                                                   @@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@  Read all informations from the $dev_info file and  @@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@  from the device specific .inc and .lkr files.      @@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                                                   @@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

sub add_missing_debug($)
  {
  my $DirRef = $_[0];
  my $sw_ref =
    {
    HEAD    => 'DEBUG',
    NAME    => 'Debugger enable bit',
    OPTIONS => [
                 {
                 NAME    => 'ON',
                 VALUE   => 0,
                 EXPL    => 'Background debugger enabled',
                 DEFAULT => FALSE
                 },
                 {
                 NAME    => 'OFF',
                 VALUE   => 0x1000,
                 EXPL    => 'Background debugger disabled',
                 DEFAULT => TRUE
                 }
               ],
    SW_MASK => 0x1000
    };

  push(@{$DirRef->{SWITCHES}}, $sw_ref);
  $DirRef->{SWITCH_COUNT} += 1;
  return $sw_ref->{SW_MASK};
  }

#---------------------------------------------------------------------------------------------------

sub add_missing_directives()
  {
  my ($missed_mcu, $complete_mcu, $substitution, $dir_ref);

  foreach (@insufficient_mcus)
    {
    $missed_mcu   = $mcus_by_names{$_};
    $substitution = $missed_mcu->{DIR_SUBST};

    if (! defined($substitution))
      {
      print STDERR "Substitution not exist for the MCU: $_\n";
      exit(1);
      }

    $complete_mcu = $mcus_by_names{$substitution->{SUBST_MCU}};

    if (! defined($complete_mcu))
      {
      print STDERR "The substitute does not exist: $substitution->{SUBST_MCU}\n";
      exit(1);
      }

    foreach $dir_ref (@{$complete_mcu->{DIRECTIVES}})
      {
      if ($dir_ref->{DIR_ADDR} == $substitution->{DIR_ADDR})
        {
        push(@{$missed_mcu->{DIRECTIVES}}, $dir_ref);

        if ($missed_mcu->{DIR_COUNT} > 1)
          {
          @{$missed_mcu->{DIRECTIVES}} = sort {$a->{DIR_ADDR} <=> $b->{DIR_ADDR}} @{$missed_mcu->{DIRECTIVES}};
          }

        last;
        }
      }
    }
  }

#---------------------------------------------------------------------------------------------------

sub find_first_section($$)
  {
  my ($SectRef, $Size) = @_;
  my $i;

  for ($i = 0; $i < $Size; ++$i)
    {
    foreach (@{$SectRef->[$i]})
      {
      if ($_->{START} >= 0 && $_->{END} >= 0)
        {
        return $_;
        }
      }
    }

  return undef;
  }

#---------------------------------------------------------------------------------------------------

sub read_device_informations()
  {
  my ($mcu_name, $inc, $lkr, $class_name);
  my $mcu_features    = undef;
  my $directive       = undef;
  my $switch_info     = undef;
  my $directive_addr  = 0;
  my $directive_count = 0;
  my $switch_count    = 0;
  my $option_count    = 0;
  my $state           = ST_WAIT;
  my ($dir_addr_min, $dir_addr_max, $directive_mask);
  my $debug_present;

  open(INFO, '<', $dev_info) || die "Could not open for reading: $dev_info\n";

  Log("Reads all config options from $dev_info.", 4);

  my $msg = ($list_file ne '') ? 'list file' : 'Gputils';

  while (<INFO>)
    {
    chomp;
    s/\r$//o;
    s/^<|>$//go;

    my @fields = split('><', $_, -1);

    $dev_info_rev = $fields[1] if ($fields[0] eq 'RES_FILE_VERSION_INFO_TYPE');

    next if (@fields < 3);

    if ($fields[0] eq 'PART_INFO_TYPE')
      {
        # <PART_INFO_TYPE><f220><PIC10F220><16c5x><0><0><ff><1><1f><0><0><0><1>
        # <PART_INFO_TYPE><e529><PIC12F529T39A><16c5xe><0><3><5ff><8><1f><0><0><3f><1>
        # <PART_INFO_TYPE><6628><PIC16F628><16xxxx><0><1><7ff><4><7f><7f><0><0><1>
        # <PART_INFO_TYPE><a829><PIC16LF1829><16Exxx><2><4><1fff><20><7f><ff><0><0><2>
        # <PART_INFO_TYPE><7752><PIC17C752><17xxxx><0><1><ffff><9><ff><0><0><0><0>
        # <PART_INFO_TYPE><1330><PIC18F1330><18xxxx><6><1><1fff><10><ff><7f><7f><0><c>

      if ($fields[2] =~ /^$pic_name_mask$/io)
        {
        ($mcu_name, $class_name, $directive_count) = (uc($fields[2]), lc($fields[3]), hex($fields[12]));
        $switch_count   = 0;
        $option_count   = 0;
        $dir_addr_min   = ULONG_MAX;
        $dir_addr_max   = 0;
        $directive_mask = 0;
        $debug_present  = FALSE;

        my $tr = $class_features_by_mpasmx{$class_name};

        die "Unknown class of $mcu_name MCU!" if (! defined($tr));

        $mcu_features =
          {
          MCU_NAME   => $mcu_name,
          MCU_CLASS  => $tr->{CLASS},     # Class of the MCU. (PROC_CLASS_PIC12yy)
          DIR_SUBST  => undef,
          ROM_SIZE   => 0,                # Size of the program memory.

          COFF       => hex($fields[1]),  # Coff ID of the device. (16 bit wide)
          PAGES      => hex($fields[5]),  # Number of the ROM/FLASH pages.
          MAX_RAM    => 0,                # The highest address of the RAM.
          RAM_SIZE   => 0,                # Full size of the all SFR and GPR.
          GPR_SIZE   => 0,                # Full size of the all GPR.
          SGPR_SIZE  => 0,                # Size of the shared GPRs.
          CONF_START => 0,                # Address of the first Configuration byte/word.
          CONF_END   => 0,                # Address of the last Configuration byte/word.
          STD_HEADER => '',               # Standard header file name of the MCU.
          STD_SCRIPT => '',               # Standard linker script file name of the MCU.

        # These addresses relative, compared to the beginning of the blocks.
          ROM        => hex($fields[6]),  # Last address of ROM/FLASH.
          FLASHDATA  => hex($fields[11]), # Last address of FLASH Data.
          EEPROM     => hex($fields[9]),  # Last address of EEPROM.

          DIR_COUNT  => hex($fields[12]), # Number of the Configuration bytes/words.
          DIRECTIVES => [],
          CONF_MASK  => 0,                # Mask of the config words.
          BANK_MAX   => $tr->{BANK_MAX},
          BANK_NUM   => hex($fields[7]),  # Number of the RAM Banks.
          BANK_SIZE  => $tr->{BANK_SIZE},
          BANK_MASK  => $tr->{BANK_MASK},
          BANK_SHIFT => $tr->{BANK_SHIFT},
          ACCESS     => hex($fields[10]), # Last address of the lower Access RAM of pic18f series.
          OSCVAL     => undef,            # Oscillator Calibration Value.
          USERID     => undef,            # User ID.
          DEVID      => undef,            # Device ID.
          LINEARMEM  => undef,            # Linear RAM of enhanced pic14 MCUs.
          SFR_RAM    => [],               # List of the SFR RAM sections.
          GPR_RAM    => [],               # List of the GPR RAM sections.
          COMMON_RAM => [],               # List of the common RAM sections.
          MIRROR_RAM => [],               # List of the mirror RAM sections.
          BAD_RAM    => [],               # List of the bad RAM sections.
          SFRS       => [],               # List of the SFRs.
          SFR_NAMES  => {},               # List names of the SFRs by addresses.
          SFR_ADDRS  => {}                # List addresses of the SFRs by names.
          };

        $inc = $gp_mcus_by_names{$mcu_name};

        if (defined($inc) && $inc ne '')
          {
          $lkr = $inc;
          $lkr =~ s/^p//o;

          my $class_pic16 = ($inc =~ /^p17/o) ? TRUE : FALSE;

          read_ram_and_rom_features("${lkr}_g.lkr", $mcu_features);
          read_ram_features("${inc}.inc", $mcu_features, $class_pic16);

          my ($first, $size);

          $first = find_first_section($mcu_features->{COMMON_RAM}, $mcu_features->{BANK_MAX});

          if (defined($first))
            {
            $size = $first->{END} - $first->{START} + 1;
            $mcu_features->{SGPR_SIZE} = $size;
            $mcu_features->{GPR_SIZE} += $size;
            }

          $mcu_features->{RAM_SIZE} = $mcu_features->{GPR_SIZE} + scalar(keys %{$mcu_features->{SFR_NAMES}});

          if ($class_pic16)
            {
            $mcu_features->{CONF_SIZE} = $mcu_features->{CONF_END} - $mcu_features->{CONF_START} + 1;
            $mcus_by_names{$mcu_name}  = $mcu_features;
            }

          $state = ST_LISTEN;
          $directive_addr = 0;
          }
        else
          {
          print STDERR "This MCU not supported in $msg: $fields[2]\n";
          $state = ST_WAIT;
          }
        }
      else
        {
        print STDERR "This MCU not supported in $PROGRAM: $fields[2]\n";
        $state = ST_WAIT;
        }

      next;
      } # if ($fields[0] eq 'PART_INFO_TYPE')

    if ($state == ST_LISTEN)
      {
      given ($fields[0])
        {
        when ('CONFIGREG_INFO_TYPE')
          {
        # <CONFIGREG_INFO_TYPE><2007><0><ffff><9>
        # <CONFIGREG_INFO_TYPE><2008><0><ffff><0>
        # <CONFIGREG_INFO_TYPE><300001><0><7><3>

          die "Too much the number of \"CONFIGREG_INFO_TYPE\"!\n" if ($directive_count <= 0);

          my ($dir_default, $dir_subst);

          ($directive_addr, $dir_default, $switch_count) = (hex($fields[1]), hex($fields[3]), hex($fields[4]));

          if ($switch_count <= 0)
            {
            printf STDERR "Database error in descriptor of $mcu_name at 0x%06X: This CONFIGREG_INFO_TYPE empty!\n", $directive_addr;
            $dir_subst = $missed_directive_substitutions{$mcu_name};

            if (defined($dir_subst))
              {
              $mcu_features->{DIR_SUBST} = $dir_subst;
              push(@insufficient_mcus, $mcu_name);
              print STDERR "  The possible substitution: $dir_subst->{SUBST_MCU}\n";
              }
            }
          else
            {
            $directive =
              {
              DIR_ADDR    => $directive_addr,
              DIR_DEFAULT => $dir_default,
              DIR_MASK    => 0,
              SWITCHES    => []
              };

            $directive_mask = 0;
            push(@{$mcu_features->{DIRECTIVES}}, $directive);

            $dir_addr_min = $directive_addr if ($dir_addr_min > $directive_addr);
            $dir_addr_max = $directive_addr if ($dir_addr_max < $directive_addr);
            }

          --$directive_count;
          } # when ('CONFIGREG_INFO_TYPE')

        when ('SWITCH_INFO_TYPE')
          {
        # <SWITCH_INFO_TYPE><FOSC><Oscillator Selection bits><7><8>
        # <SWITCH_INFO_TYPE><DEBUG><In-Circuit Debugger Mode bit><2000><2>

          die "Too much the number of \"SWITCH_INFO_TYPE\"!\n" if ($switch_count <= 0);

          my ($sw_name, $sw_mask);

          ($sw_name, $sw_mask, $option_count) = ($fields[1], hex($fields[3]), hex($fields[4]));

          if ($option_count <= 0)
            {
            printf STDERR "Database error in descriptor of $mcu_name at 0x%06X, $sw_name: This SWITCH_INFO_TYPE empty!\n", $directive_addr;
            }
          else
            {
            $switch_info =
              {
              HEAD    => $sw_name,
              NAME    => (defined($fields[2]) ? $fields[2] : ''),
              OPTIONS => [],
              SW_MASK => $sw_mask
              };

            $debug_present = TRUE if ($sw_name eq 'DEBUG');
            push(@{$directive->{SWITCHES}}, $switch_info);
            $directive_mask |= $sw_mask;
            }

          --$switch_count;

          if ($switch_count == 0)
            {
            if ($mcu_features->{MCU_CLASS} == PROC_CLASS_PIC14E && $directive_addr == 0x8008 &&
                ! $debug_present && $mcu_name ~~ @mcu_missed_debug)
              {
              $directive_mask |= add_missing_debug($directive);
              }

            @{$directive->{SWITCHES}} = sort {$a->{SW_MASK} <=> $b->{SW_MASK}} @{$directive->{SWITCHES}};
            $directive->{DIR_MASK}     = $directive_mask;
            # This an ex post correction which unfortunately sometimes necessary.
            $directive->{DIR_DEFAULT} &= $directive_mask;
            }
          } # when ('SWITCH_INFO_TYPE')

        when ('SETTING_VALUE_TYPE')
          {
        # <SETTING_VALUE_TYPE><LP><LP oscillator: Low-power crystal on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN><0>
        # <SETTING_VALUE_TYPE><OFF><WDT disabled and can be enabled by SWDTEN bit of the WDTCON register><0>
        # <SETTING_VALUE_TYPE><HSHP><HS oscillator (high power > 16 MHz)><2>
        # <SETTING_VALUE_TYPE><ECLPIO6><EC oscillator (low power, <500 kHz)><d>
        # <SETTING_VALUE_TYPE><ON><WDT enabled><8>
        # <SETTING_VALUE_TYPE><2><><10>

          die "Too much the number of \"SETTING_VALUE_TYPE\"!\n" if ($option_count <= 0);
          die "There is no actual \"SWITCH_INFO_TYPE\"!\n" if (! defined($switch_info));

          my $value = hex($fields[3]);
          my $is_default = (($directive->{DIR_DEFAULT} & $switch_info->{SW_MASK}) == $value) ? TRUE : FALSE;

          my $option =
            {
            NAME    => $fields[1],
            VALUE   => $value,
            EXPL    => (defined($fields[2]) ? $fields[2] : ''),
            DEFAULT => $is_default
            };

          push(@{$switch_info->{OPTIONS}}, $option);
          --$option_count;

          if ($option_count == 0)
            {
            @{$switch_info->{OPTIONS}} = sort {$a->{VALUE} <=> $b->{VALUE}} @{$switch_info->{OPTIONS}};
            }
          } # when ('SETTING_VALUE_TYPE')
        } # given ($fields[0])

      if ($directive_count == 0 && $switch_count == 0 && $option_count == 0)
        {
        # All information is together.
        die "$mcu_name MCU already exist!" if (defined($mcus_by_names{$mcu_name}));

        $mcu_features->{CONF_START} = $dir_addr_min;
        $mcu_features->{CONF_END}   = $dir_addr_max;
        $mcus_by_names{$mcu_name}   = $mcu_features;
        $state = ST_WAIT;
        }
      } # if ($state == ST_LISTEN)
    } # while (<INFO>)

  close(INFO);
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  Preparation of html files.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        # Print the head of html file.

sub print_html_head($)
  {
  print $out_handler <<EOT
$HTML_Doctype
<html lang="en">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=utf-8">
    <meta name="author" content="Molnár Károly">
    <title>$_[0]</title>
    <link rel="stylesheet" type="text/css" href="${remote_url}$css">
  </head>
EOT
;
  }

#---------------------------------------------------------------------------------------------------

sub print_source_info($)
  {
  my $Align = $_[0];
  my $href = "<a href=\"http://${gputils_url}#Download\">gputils</a> source package";
  my $time = strftime('%F %T UTC', gmtime);

  aOutl($Align, '<div class="legendContainer">');
  aOutml($Align + 2, '<p class="srcInfo">',
                     '  This page generated automatically by the',
                     "  <a href=\"https://sourceforge.net/p/gputils/code/HEAD/tree/trunk/scripts/tools/device-help.pl\"><em>$PROGRAM</em></a>",
                     "  program ($time) from the <em>" . basename($dev_info) . "</em> file (rev: $dev_info_rev) of <em>mpasmx</em> and from the",
                     "  $href (rev: svn $svn_rev). The <em>mpasmx</em>",
                     "  is included in the <a href=\"http://$mplabx_url\">MPLAB X</a>.",
                     "</p>");
  aOutl($Align, '</div>');
  }

#---------------------------------------------------------------------------------------------------

sub mcu_name_correction($)
  {
  my $Name = $_[0];
  
  $Name =~ s/PICRF5/rfPIC12C5/o;
  $Name =~ s/PICRF6/rfPIC12F6/o;
  return $Name;
  }

#---------------------------------------------------------------------------------------------------

        # Prepares multiple list of MCUs.

sub print_mcu_list($$)
  {
  my ($Align, $Element) = @_;
  my $menu_class = $Element->{CLASS};
  my $lst        = '<td class="mcuListHeader">';
  my @array      = ();

  Log("Dump the \"$Element->{NAME}\" page.", 4);

  aOutml($Align + 2, '<div class="mcuList">',
                     '  <table class="mcuTable">',
                     '    <tr>');
  aOutl($Align + 8, "${lst}name</td>");

  given ($menu_class)
    {
    when (PRI_MENU_ALL)
      {
      aOutml($Align + 8, "${lst}instruction size (bit)</td>",
                         "${lst}config word size (bit)</td>",
                         "${lst}class</td>",
                         "${lst}data sheet</td>");
      }

    when ([ PRI_MENU_ENH, PRI_MENU_EXT, PRI_MENU_REG ])
      {
      aOutml($Align + 8, "${lst}instruction size (bit)</td>",
                         "${lst}config word size (bit)</td>",
                         "${lst}data sheet</td>");
      }

    when (PRI_MENU_RAM)
      {
      aOutml($Align + 8, "${lst}RAM size (byte)</th>",
                         "${lst}instruction size (bit)</td>",
                         "${lst}config word size (bit)</td>",
                         "${lst}class</td>",
                         "${lst}data sheet</td>");
      }

    when (PRI_MENU_ROM)
      {
      aOutml($Align + 8, "${lst}ROM size (word/byte)</td>",
                         "${lst}instruction size (bit)</td>",
                         "${lst}config word size (bit)</td>",
                         "${lst}class</td>",
                         "${lst}data sheet</td>");
      }

    when (PRI_MENU_EEPROM)
      {
      aOutml($Align + 8, "${lst}EEPROM size (byte)</td>",
                         "${lst}instruction size (bit)</td>",
                         "${lst}config word size (bit)</td>",
                         "${lst}class</td>",
                         "${lst}data sheet</td>");
      }

    default
      {
      aOutml($Align + 8, "${lst}config word size (bit)</td>",
                         "${lst}class</td>",
                         "${lst}data sheet</td>");
      }
    }

  aOutml($Align + 6, '</tr>',
                     '<tr class="mcuGap"><td></td></tr>');

  if ($menu_class == PRI_MENU_RAM)
    {
    @array = sort { $mcus_by_names{$a}->{RAM_SIZE} <=> $mcus_by_names{$b}->{RAM_SIZE} ||
                    smartSort($a, $b) } keys %mcus_by_names;
    }
  elsif ($menu_class == PRI_MENU_ROM)
    {
    @array = sort { $mcus_by_names{$a}->{ROM_SIZE} <=> $mcus_by_names{$b}->{ROM_SIZE} ||
                    smartSort($a, $b) } keys %mcus_by_names;
    }
  elsif ($menu_class == PRI_MENU_EEPROM)
    {
    foreach (keys %mcus_by_names)
      {
      push(@array, $_) if ($mcus_by_names{$_}->{EEPROM} > 0);
      }

    @array = sort { $mcus_by_names{$a}->{EEPROM} <=> $mcus_by_names{$b}->{EEPROM} ||
                    smartSort($a, $b) } @array;
    }
  else
    {
    @array = sort { smartSort($a, $b) } keys %mcus_by_names;
    }

  foreach my $name (@array)
    {
    my $mcu_features = $mcus_by_names{$name};
    my $mcu_class    = $mcu_features->{MCU_CLASS};
    my $mcu_class_features = $class_features_list[$mcu_class];
    my $css_class    = $mcu_class_features->{CSS_CLASS};
    my $wsize        = $mcu_class_features->{WORD_SIZE};
    my $enh          = ($wsize == 16) ? 'extended' : 'enhanced';
    my $td_wsize     = "<td class=\"$css_class\">$wsize</td>";
    my $td_csize     = "<td class=\"$css_class\">$mcu_class_features->{CONF_SIZE}</td>";
    my $td_class     = "<td class=\"$css_class\">" . (($mcu_class_features->{ENHANCED}) ? $enh : 'regular') . '</td>';

    $name = mcu_name_correction($name);

    my $href_name = $name;

    if (! ($href_name ~~ @data_sheet_name_exception_list))
      {
      # The data sheet not exist with the pic1xLFxxx or the pic1xHVxxx name.
      $href_name =~ s/(LF|HV)/F/o;
      }

    my $td_href    = "<th><a class=\"mcuLink\" href=\"${remote_url}${name}-$feat_tag.html\">$name</a></th>";
    my $td_sh_href = "<td><a class=\"mcuDSheet\" href=\"http://www.microchip.com/TechDoc.aspx?type=datasheet&product=$href_name\">search</a></td>";

    given ($menu_class)
      {
      when (PRI_MENU_ALL)
        {
        aOutl ($Align + 6, '<tr>');
        aOutml($Align + 8, $td_href, $td_wsize, $td_csize, $td_class, $td_sh_href);
        aOutl ($Align + 6, '</tr>');
        }

      when (PRI_MENU_ENH)
        {
        if ($mcu_class == PROC_CLASS_PIC12E || $mcu_class == PROC_CLASS_PIC14E)
          {
          aOutl ($Align + 6, '<tr>');
          aOutml($Align + 8, $td_href, $td_wsize, $td_csize, $td_sh_href);
          aOutl ($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_EXT)
        {
        if ($mcu_class == PROC_CLASS_PIC16E)
          {
          aOutl ($Align + 6, '<tr>');
          aOutml($Align + 8, $td_href, $td_wsize, $td_csize, $td_sh_href);
          aOutl ($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_REG)
        {
        if (! $mcu_class_features->{ENHANCED})
          {
          aOutl ($Align + 6, '<tr>');
          aOutml($Align + 8, $td_href, $td_wsize, $td_csize, $td_sh_href);
          aOutl ($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_12_BIT)
        {
        if ($wsize == 12)
          {
          aOutl ($Align + 6, '<tr>');
          aOutml($Align + 8, $td_href, $td_csize, $td_class, $td_sh_href);
          aOutl ($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_14_BIT)
        {
        if ($wsize == 14)
          {
          aOutl ($Align + 6, '<tr>');
          aOutml($Align + 8, $td_href, $td_csize, $td_class, $td_sh_href);
          aOutl ($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_16_BIT)
        {
        if ($wsize == 16)
          {
          aOutl ($Align + 6, '<tr>');
          aOutml($Align + 8, $td_href, $td_csize, $td_class, $td_sh_href);
          aOutl ($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_RAM)
        {
        aOutl ($Align + 6, '<tr>');
        aOutml($Align + 8, "<th><a class=\"mcuLink\" href=\"${remote_url}${name}-$ram_tag.html\">$name</a></th>",
                           "<td class=\"$css_class\">$mcu_features->{RAM_SIZE}</td>",
                           $td_wsize, $td_csize, $td_class, $td_sh_href);
        aOutl ($Align + 6, '</tr>');
        }

      when (PRI_MENU_ROM)
        {
        aOutl ($Align + 6, '<tr>');
        aOutml($Align + 8, $td_href,
                           "<td class=\"$css_class\">$mcu_features->{ROM_SIZE}</td>",
                           $td_wsize, $td_csize, $td_class, $td_sh_href);
        aOutl ($Align + 6, '</tr>');
        }

      when (PRI_MENU_EEPROM)
        {
        aOutl ($Align + 6, '<tr>');
        aOutl ($Align + 8, $td_href);
        aOutfl($Align + 8, "<td class=\"$css_class\">%u</td>", $mcu_features->{EEPROM} + 1);
        aOutml($Align + 8, $td_wsize, $td_csize, $td_class, $td_sh_href);
        aOutl ($Align + 6, '</tr>');
        }
      } # given ($menu_class)
    } # foreach my $name (@array)

  aOutml($Align + 2, '  </table>', '</div>');

        #------------------------------------

  aOutl($Align + 2, '<div class="legendContainer">');

  my $v = @mcu_feat_names;
  my $level;

  aOutl($Align + 4, '<div class="legend">');
  $level = 1;
  for (my $i = 0; $i < $v;)
    {
    my $cl = $class_features_by_mpasmx{$mcu_feat_names[$i]};

    aOutl($Align + 6, "<p class=\"$cl->{CSS_CLASS} menuEx\">&nbsp;<span class=\"explanation\">$cl->{NAME}</span></p>");
    ++$i;

    if (! ($i % 3))
      {
      aOutl($Align + 4, '</div>');
      --$level;

      if ($i < $v)
        {
        aOutl($Align + 4, '<div class="legend">');
        ++$level;
        }
      }
    }

  aOutl($Align + 4, '</div>') if ($level > 0);  # The closure of last div, if it miss.
  aOutl($Align + 2, '</div>');

  print_source_info($Align + 2);
  }

#---------------------------------------------------------------------------------------------------

sub print_class_menu($$)
  {
  my ($Active, $Features) = @_;
  my $mcu_class = (defined($Features)) ? $Features->{MCU_CLASS} : -1;
  my $link      = ($mcu_class >= 0) ? $common_sfr_menu[$mcu_class]->{HREF} : undef;

  aOutl(4, '<div class="classMenu">');

  foreach (@primary_menu)
    {
        # Hide this active menu element.

    next if ($_->{CLASS} == $Active);

    if (defined($link) && $_->{CLASS} == PRI_COMMON_SFR)
      {
        # Replace this URL.

      aOutl(6, "<a href=\"${remote_url}${link}\">$_->{NAME}</a>");
      }
    else
      {
      aOutl(6, "<a href=\"${remote_url}$_->{HREF}\">$_->{NAME}</a>");
      }
    }

  aOutl(4, '</div>');
  }

#---------------------------------------------------------------------------------------------------

        # Print the local menu of $Name MCU page.

sub print_local_menu($$$)
  {
  my ($Menu, $Name, $Selected) = @_;

  aOutl(4, '<div class="tabs">');

  foreach (@{$Menu})
    {
    my $class = ($_->{CLASS} == $Selected) ? ' class="selected"' : '';

    aOutl(6, "<a$class href=\"${remote_url}${Name}$_->{HREF}\">$_->{NAME}</a>");
    }

  aOutl(4, '</div>');
  }

#---------------------------------------------------------------------------------------------------

sub add_to_sfr_common_list($$)
  {
  my ($Sfr_list, $Features) = @_;

  foreach (@{$Features->{SFRS}})
    {
    my ($name, $addr) = ($_->{NAME}, $_->{ADDR});
    my $sfr = $Sfr_list->{$name};

    if (defined($sfr))
      {
      ++$sfr->{NUMBER} if ($sfr->{ADDR} == $addr);
      }
    else
      {
      $Sfr_list->{$name} = { ADDR => $addr, NUMBER => 1 };
      }
    }
  }

#---------------------------------------------------------------------------------------------------

        # Collects into lists those registers which they exist in the class of all members.

sub make_sfr_common_lists()
  {
  my ($mcu_features, $mcu_class, $menu);

  $pic12_mcu_number  = 0;
  $pic12e_mcu_number = 0;
  $pic14_mcu_number  = 0;
  $pic14e_mcu_number = 0;
  $pic16_mcu_number  = 0;
  $pic16e_mcu_number = 0;

  Log("Make list of common SFRs.", 4);

  foreach (keys %mcus_by_names)
    {
    $mcu_features = $mcus_by_names{$_};
    $mcu_class    = $mcu_features->{MCU_CLASS};
    $menu         = $common_sfr_menu[$mcu_class];

    add_to_sfr_common_list($menu->{PARAM0}, $mcu_features);
    ++${$menu->{PARAM1}};
    }
  }

#---------------------------------------------------------------------------------------------------

        # Dump those registers which they exist in the class of all members.

sub print_common_sfr_lists($$)
  {
  my ($Align, $Element) = @_;
  my ($sfr_list, $number, $name, $addr, $sfr);
  my ($bank, $i, $max_x, $x, $min_y, $max_y, $y, $t);
  my $mcu_class    = $Element->{CLASS};
  my $class_pic16e = ($mcu_class == PROC_CLASS_PIC16E) ? TRUE : FALSE;
  my $mcu_features = $class_features_list[$mcu_class];
  my $bank_size    = $mcu_features->{BANK_SIZE};
  my @bank_array   = ();
  my %array;

  Log("Dump list of $Element->{NAME} common SFRs.", 4);

  print_local_menu(\@common_sfr_menu, '', $mcu_class);

        #------------------------------------

  $sfr_list = $Element->{PARAM0};
  $number   = ${$Element->{PARAM1}};

  foreach (keys %{$sfr_list})
    {
    $sfr = $sfr_list->{$_};

    if ($sfr->{NUMBER} == $number)
      {
      $array{$sfr->{ADDR}} = $_;
      }
    }

        #------------------------------------

  $bank = -1;
  $min_y = ULONG_MAX;
  $max_y = -1;
  $i = -1;
  foreach (sort {$a <=> $b} keys %array)
    {
    $addr = $_;

    my $bn   = int($addr / $bank_size);
    my $offs = $addr % $bank_size;

    if ($bank != $bn)
      {
      $bank = $bn;
      $bank_array[$i]->{MAX_OFFS} = $max_y if ($i >= 0);
      ++$i;
      $max_y = -1;
      $bank_array[$i]->{BANKNUM} = $bank;
      }

    $bank_array[$i]->{BANK}[$offs] = { NAME => $array{$_}, ADDR => $_ };
    $min_y = $offs if ($min_y > $offs);
    $max_y = $offs if ($max_y < $offs);
    }

  $bank_array[$i]->{MAX_OFFS} = $max_y;      # This the last bank.

  $max_x = @bank_array;

  aOutl ($Align, '<table class="sfrCommonTable">');
  aOutfl($Align + 2, "<tr><th colspan=%u class=\"sfrTableName\">$Element->{HEAD}</th></tr>", $max_x * 2 + 1);
  aOutl ($Align + 2, '<tr class="sfrGap"><td></td></tr>');

        #------------------------------------
        # header

  aOutl($Align + 2, '<tr class="sfrHead">');

  for ($x = 0; $x < $max_x; ++$x)
    {
    aOutl($Align + 4, "<th class=\"sfrBank\">Bank&nbsp;$bank_array[$x]->{BANKNUM}</th>");
    }

  aOutml($Align + 2, '</tr>', '<tr class="sfrGap"><td></td></tr>');

  aOutl($Align + 2, '<tr>');

  for ($x = 0; $x < $max_x; ++$x)
    {
    my $bn = $bank_array[$x]->{BANK};

    aOutl($Align + 4, '<td class="sfrColumn">');
    aOutl($Align + 6, '<table>');

    $max_y = $bank_array[$x]->{MAX_OFFS};
    for ($y = $min_y; $y <= $max_y; ++$y)
      {
      $t = $bn->[$y];

      if (defined($t))
        {
        ($name, $addr) = ($t->{NAME}, $t->{ADDR});

        aOutl ($Align + 8, "<tr id=\"$name\">");
        aOutl ($Align + 10, "<th class=\"sfrName\">$name</th>");
        aOutfl($Align + 10, "<td class=\"sfrAddr\">0x%03X</td>", $addr);
        aOutl ($Align + 8, '</tr>');
        }
      else
        {
        aOutl($Align + 8, '<tr><th class="sfrSep">&nbsp;</th></tr>');
        }
      }

    aOutml($Align + 4, '  </table>', '</td>');
    }

  aOutl($Align + 2, '</tr>');
  aOutl($Align, '</table>');

  print_source_info(4);
  }

#---------------------------------------------------------------------------------------------------

        # Dump the device ID.

sub print_devid($$$)
  {
  my ($Align, $Devid, $Length) = @_;

  if (defined($Devid))
    {
    my ($start, $end) = ($Devid->{START}, $Devid->{END});
    my $size = $end - $start + 1;
    my $tail = ($size > 1) ? ' space' : '';

    aOutl($Align, '<tr class="featLine">');
    aOutl($Align + 2, "<th class=\"featName\">Address$tail of Device ID</th>");

    if ($size > 1)
      {
      aOutfl($Align + 2, "<td class=\"featValue\">0x%0${Length}X - 0x%0${Length}X&nbsp;&nbsp;(%u bytes)</td>",
                         $start, $end, $size);
      }
    else
      {
      aOutfl($Align + 2, "<td class=\"featValue\">0x%0${Length}X&nbsp;&nbsp;(%u byte)</td>",
                         $start, $size);
      }

    aOutl($Align, '</tr>');
    }
  }

#---------------------------------------------------------------------------------------------------

        # Dump the features of a MCU.

sub print_features($)
  {
  my $Features = $_[0];
  my ($str, $len, $rom_size, $word_size, $i, $t);
  my $mcu_name      = mcu_name_correction($Features->{MCU_NAME});
  my $mcu_class     = $Features->{MCU_CLASS};
  my $class_pic16   = ($mcu_class == PROC_CLASS_PIC16)  ? TRUE : FALSE;
  my $class_pic16e  = ($mcu_class == PROC_CLASS_PIC16E) ? TRUE : FALSE;
  my $mcu_class_features = $class_features_list[$mcu_class];

  $str = "$out_dir/${mcu_name}-$feat_tag.html";
  open($out_handler, '>', $str) || die "Could not create the \"$str\" file!\n";

  Log("Dump the features of $mcu_name.", 4);

        #------------------------------------

  print_html_head($mcu_name);
  aOutl(2, '<body>');

  print_class_menu(-1, $Features);
  print_local_menu(\@mcu_menu_elems, $mcu_name, MCU_MENU_FEAT);

        #------------------------------------

        # The table of features of MCU.

  $word_size = $mcu_class_features->{WORD_SIZE};
  $len = ($class_pic16e) ? 6 : 4;
  $rom_size = $Features->{ROM} + 1;

  aOutl (4, '<table class="featList">');
  aOutml(6, "<tr><th colspan=4 class=\"featTableName\">$mcu_name</th></tr>",
            '<tr class="featGap"><td></td></tr>');

        #------------------------------------

  aOutl (6, '<tr class="featLine">');
  aOutl (8, '<th class="featName">Coff ID of device</th>');
  aOutfl(8, '<td class="featValue">0x%04X</td>', $Features->{COFF});
  aOutl (6, '</tr>');

        #------------------------------------

  $i = $mcu_class_features->{PAGE_SIZE};
  if ($i > 0)
    {
    aOutl (6, '<tr class="featLine">');
    aOutml(8, '<th class="featName">Number of ROM/FLASH pages</th>',
              "<td class=\"featValue\">$Features->{PAGES}&nbsp;&nbsp;($i words/pages)</td>");
    aOutl (6, '</tr>');
    }

        #------------------------------------

  aOutl (6, '<tr class="featLine">');
  aOutl (8, '<th class="featName">Last address of ROM/FLASH</th>');
  aOutfl(8, "<td class=\"featValue\">0x%0${len}X</td>", $Features->{ROM});
  aOutl (6, '</tr>');

        #------------------------------------

  $i = $Features->{ROM_SIZE};
  if ($i)
    {
    $t = ($word_size == 16) ? 'bytes' : 'words';
    aOutl (6, '<tr class="featLine">');
    aOutml(8, '<th class="featName">Size of ROM</th>',
              "<td class=\"featValue\">$i $t</td>");
    aOutl (6, '</tr>');
    }

        #------------------------------------

  $i = $Features->{OSCVAL};
  if (defined($i))
    {
    my $size = $i->{END} - $i->{START} + 1;

    aOutl(6, '<tr class="featLine">');
    aOutl(8, '<th class="featName">Oscillator calibration value</th>');

    if ($size > 1)
      {
      aOutfl(8, "<td class=\"featValue\">0x%0${len}X - 0x%0${len}X&nbsp;&nbsp;(%u words)</td>",
                 $i->{START}, $i->{END}, $size);
      }
    else
      {
      aOutfl(8, "<td class=\"featValue\">0x%0${len}X&nbsp;&nbsp;(%u word)</td>",
                 $i->{START}, $size);
      }

    aOutl(6, '</tr>');
    }

        #------------------------------------

  $i = $Features->{FLASHDATA};
  if ($i > 0)
    {
    aOutl (6, '<tr class="featLine">');
    aOutl (8, '<th class="featName">Address space of FLASH Data</th>');
    aOutfl(8, "<td class=\"featValue\">0x%0${len}X - 0x%0${len}X&nbsp;&nbsp;(%u words)</td>",
               $rom_size, $rom_size + $i, $i + 1);

    aOutl (6, '</tr>');
    }

        #------------------------------------

  $i = $Features->{USERID};
  if (defined($i))
    {
    my $size = $i->{END} - $i->{START} + 1;
    my $tail = ($size > 1) ? ' space' : '';

    aOutl (6, '<tr class="featLine">');
    aOutl (8, "<th class=\"featName\">Address$tail of User ID</th>");
    aOutfl(8, "<td class=\"featValue\">0x%0${len}X - 0x%0${len}X&nbsp;&nbsp;(%u bytes)</td>",
               $i->{START}, $i->{END}, $size);

    aOutl (6, '</tr>');
    }

        #------------------------------------

  print_devid(6, $Features->{DEVID}, $len) if ($word_size != 16);

        #------------------------------------

  if ($class_pic16)
    {
    $i = $mcu_class_features->{CONF_SIZE};
    $t = 'Bit';

    if ($i > 1)
      {
      $t .= 's';
      $str = ' space';
      }
    else
      {
      $str = '';
      }
    }
  else
    {
    $i = $Features->{DIR_COUNT};
    $t = ($class_pic16e) ? 'Byte' : 'Word';

    if ($i > 1)
      {
      $t .= 's';
      $str = ' space';
      }
    else
      {
      $str = '';
      }
    }

  aOutl(6, '<tr class="featLine">');
  aOutl(8, "<th class=\"featName\">Address$str of Configuration $t</th>");

  if ($i > 1)
    {
    aOutfl(8, "<td class=\"featValue\">0x%0${len}X - 0x%0${len}X&nbsp;&nbsp;($i %s)</td>",
               $Features->{CONF_START}, $Features->{CONF_END}, lc($t));
    }
  else
    {
    aOutfl(8, "<td class=\"featValue\">0x%0${len}X&nbsp;&nbsp;($i %s)</td>",
               $Features->{CONF_START}, lc($t));
    }

  aOutl(6, '</tr>');

        #------------------------------------

  print_devid(6, $Features->{DEVID}, $len) if ($word_size == 16);

        #------------------------------------

  $i = $Features->{EEPROM};
  if ($i > 0)
    {
    aOutl (6, '<tr class="featLine">');
    aOutl (8, '<th class="featName">Address space of EEPROM</th>');
    aOutfl(8, "<td class=\"featValue\">0x%0${len}X - 0x%0${len}X&nbsp;&nbsp;(%u bytes)</td>",
               $mcu_class_features->{EE_START}, $mcu_class_features->{EE_START} + $i, $i + 1);

    aOutl (6, '</tr>');
    }

        #------------------------------------

  aOutl (6, '<tr class="featLine">');
  aOutml(8, '<th class="featName">Number of RAM Banks</th>',
            "<td class=\"featValue\">$Features->{BANK_NUM}&nbsp;&nbsp;($mcu_class_features->{BANK_SIZE} bytes/banks)</td>");
  aOutl (6, '</tr>');

        #------------------------------------

  $i = scalar(keys %{$Features->{SFR_NAMES}});
  if ($i > 0)
    {
    aOutl (6, '<tr class="featLine">');
    aOutml(8, '<th class="featName">Number of SFRs</th>',
              "<td class=\"featValue\">$i</td>");
    aOutl (6, '</tr>');
    }

        #------------------------------------

  $i = $Features->{SGPR_SIZE};
  if ($i > 0)
    {
    aOutl (6, '<tr class="featLine">');
    aOutml(8, '<th class="featName">Size of shared GPRs</th>',
              "<td class=\"featValue\">$i bytes</td>");
    aOutl (6, '</tr>');
    }

        #------------------------------------

  $i = $Features->{GPR_SIZE};
  if ($i > 0)
    {
    aOutl (6, '<tr class="featLine">');
    aOutml(8, '<th class="featName">Size of all GPRs</th>',
              "<td class=\"featValue\">$i bytes</td>");
    aOutl (6, '</tr>');
    }

        #------------------------------------

  $i = $Features->{RAM_SIZE};
  if ($i > 0)
    {
    aOutl (6, '<tr class="featLine">');
    aOutml(8, '<th class="featName">Joint size of SFRs + GPRs</th>',
              "<td class=\"featValue\">$i bytes</td>");
    aOutl (6, '</tr>');
    }

        #------------------------------------

  $i = $Features->{LINEARMEM};          # Only in the enhanced pic14 MCUs.
  if (defined($i))
    {
    aOutl (6, '<tr class="featLine">');
    aOutl (8, '<th class="featName">Start address of Linear RAM</th>');
    aOutfl(8, "<td class=\"featValue\">0x%04X</td>", $i->{START});
    aOutml(6, '</tr>', '<tr class="featLine">');
    aOutl (8, '<th class="featName">Size of Linear RAM</th>');
    aOutfl(8, "<td class=\"featValue\">%u bytes</td>", $i->{END} - $i->{START} + 1);
    aOutml(6, '</tr>', '<tr class="featLine">');
    aOutl (8, '<th class="featName">Number of Linear RAM sections</th>');
    aOutfl(8, "<td class=\"featValue\">%u</td>", scalar @{$i->{SEGMENTS}});
    aOutl (6, '</tr>');
    }

        #------------------------------------

  $i = $Features->{STD_HEADER};
  if ($i ne '')
    {
    aOutl (6, '<tr class="featLine">');
    aOutml(8, '<th class="featName">Standard header</th>',
              "<td class=\"featValue\">$i</td>");
    aOutl (6, '</tr>');
    }

        #------------------------------------

  $i = $Features->{STD_SCRIPT};
  if ($i ne '')
    {
    aOutl (6, '<tr class="featLine">');
    aOutml(8, '<th class="featName">Standard linker script</th>',
              "<td class=\"featValue\">$i</td>");
    aOutl (6, '</tr>');
    }

        #------------------------------------

  if ($class_pic16e)
    {
    aOutl (6, '<tr class="featLine">');
    aOutl (8, '<th class="featName">Last address of lower Access RAM</th>');
    aOutfl(8, "<td class=\"featValue\">0x%02X</td>", $Features->{ACCESS});
    aOutl (6, '</tr>');
    }

        #------------------------------------

  aOutl(4, '</table>');
  print_source_info(4);
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#---------------------------------------------------------------------------------------------------

        # Dump the entire contents of a Config word.

sub print_config_word($$$$$)
  {
  my ($Align, $Config, $Length, $Mask, $Gap) = @_;

  foreach (@{$Config})
    {
    my ($head, $name) = ($_->{HEAD}, $_->{NAME});
    my $mask = ($_->{SW_MASK} ^ $Mask) & $Mask;

    if ($name ne '')
      {
        # Exist a long name.
      $name = "$head -- $name";
      }
    else
      {
      $name = $head;
      }

    $name .= sprintf " (bitmask:0x%0${Length}X)", $_->{SW_MASK};

    aOutml($Align, $Gap, "<tr><th colspan=3 class=\"confOptName\">$name</th></tr>");

    foreach (@{$_->{OPTIONS}})
      {
      my $str        = "$head = $_->{NAME}";
      my $expl       = $_->{EXPL};
      my $is_default = ($_->{DEFAULT}) ? ' confSwDefault' : '';

        # Improve a spelling error: On the end of a sentence a point must be.
      $expl .= '.' if ($expl ne '' && $expl !~ /\.$/o);

      aOutl($Align, '<tr>');
      aOutl($Align + 2, "<td class=\"confSwName$is_default\">$str</td>");
      aOutfl($Align + 2, "<td class=\"confSwValue$is_default\">0x%0${Length}X</td>", $_->{VALUE} | $mask);
      aOutl($Align + 2, "<td class=\"confSwExpl$is_default\">$expl</td>");
      aOutl($Align, '</tr>');
      }
    }
  }

#---------------------------------------------------------------------------------------------------

        # Dump the entire contents of all Config word of a MCU.

sub print_all_config_word($)
  {
  my $Features = $_[0];
  my ($addr, $dir_ref, $str, $len, $i, $head_s, $head_e, $gap);
  my $conf_bits   = $Features->{DIRECTIVES};
  my $mcu_name    = mcu_name_correction($Features->{MCU_NAME});
  my $mcu_class   = $Features->{MCU_CLASS};
  my $mcu_class_features = $class_features_list[$mcu_class];
  my $config_mask = $Features->{CONF_MASK};
  my $count       = @{$conf_bits};
  my @sections;

  return if (! $count);

  $str = "$out_dir/${mcu_name}-$conf_tag.html";
  open($out_handler, '>', $str) || die "Could not create the \"$str\" file!\n";

  Log("Dump the Config Options of $mcu_name.", 4);

  $len = 4;
  $head_s = '<tr><th colspan=4 class="configWord">';
  $head_e = '</th></tr>';
  $gap    = '<tr class="confGap"><td></td></tr>';

        #------------------------------------

  print_html_head($mcu_name);
  aOutl(2, '<body>');

  print_class_menu(-1, $Features);
  print_local_menu(\@mcu_menu_elems, $mcu_name, MCU_MENU_CONF);

        #------------------------------------

        # The table of Config Bits.

  aOutml(4, '<table class="configList">',
            "  <tr><th colspan=5 class=\"confTableName\">$mcu_name</th></tr>");

  if ($count < 2)
    {
        # PIC10F, PIC12, PIC16, PIC17

    $sections[0] = 'CONFIG';
    aOutl(6, $gap);
    $dir_ref = $conf_bits->[0];

    if ($mcu_class == PROC_CLASS_PIC16)
      {
      $addr = sprintf("address:0x%0${len}X-0x%0${len}X", $Features->{CONF_START}, $Features->{CONF_END});
      }
    else
      {
      $addr = sprintf("address:0x%0${len}X", $dir_ref->{DIR_ADDR});
      }

    if ($dir_ref->{DIR_MASK} > 0)
      {
      aOutfl(6, "${head_s}CONFIG ($addr, mask:0x%0${len}X, <span class=\"confSwDefault\">default:0x%0${len}X</span>)$head_e",
                $dir_ref->{DIR_MASK}, $dir_ref->{DIR_DEFAULT});
      }
    else
      {
      aOutfl(6, "${head_s}CONFIG ($addr)$head_e");
      }

    print_config_word(6, $dir_ref->{SWITCHES}, $len, $config_mask, $gap);
    }
  else
    {
    if ($mcu_class == PROC_CLASS_PIC16E)
      {
        # PIC18
      my ($n, $h);

      $len = 2;

      if ($conf_bits->[0]->{DIR_ADDR} < 0x300000)
        {
        # PIC18FxxJ

        for ($i = 0; $i < $count; ++$i)
          {
          $dir_ref = $conf_bits->[$i];
          $n = int(($i & 0x0F) / 2 + 1);
          $h = ($i & 1) ? 'H' : 'L';
          $str = "CONFIG$n$h";
          $sections[$i] = $str;
          aOutl(6, $gap);
          aOutfl(6, "$head_s$str (address:0x%06X, mask:0x%0${len}X, <span class=\"confSwDefault\">default:0x%0${len}X</span>)$head_e",
                    $dir_ref->{DIR_ADDR}, $dir_ref->{DIR_MASK}, $dir_ref->{DIR_DEFAULT});
          print_config_word(6, \@{$dir_ref->{SWITCHES}}, $len, $config_mask, $gap);
          }
        }
      else
        {
        # PIC18Fxx

        for ($i = 0; $i < $count; ++$i)
          {
          $dir_ref = $conf_bits->[$i];
          $addr = $dir_ref->{DIR_ADDR};
          $n = int(($addr & 0x0F) / 2 + 1);
          $h = ($addr & 1) ? 'H' : 'L';
          $str = "CONFIG$n$h";
          $sections[$i] = $str;
          aOutl(6, $gap);
          aOutfl(6, "$head_s$str (address:0x%06X, mask:0x%0${len}X, <span class=\"confSwDefault\">default:0x%0${len}X</span>)$head_e",
                    $addr, $dir_ref->{DIR_MASK}, $dir_ref->{DIR_DEFAULT});
          print_config_word(6, \@{$dir_ref->{SWITCHES}}, $len, $config_mask, $gap);
          }
        }
      }
    else
      {
        # PIC12, PIC16

      for ($i = 0; $i < $count; ++$i)
        {
        $dir_ref = $conf_bits->[$i];
        $str = sprintf "CONFIG%u", $i + 1;
        $sections[$i] = $str;
        aOutl(6, $gap);
        aOutfl(6, "$head_s$str (address:0x%04X, mask:0x%0${len}X, <span class=\"confSwDefault\">default:0x%0${len}X</span>)$head_e",
                  $dir_ref->{DIR_ADDR}, $dir_ref->{DIR_MASK}, $dir_ref->{DIR_DEFAULT});
        print_config_word(6, \@{$dir_ref->{SWITCHES}}, $len, $config_mask, $gap);
        }
      }
    }

  aOutl(4, '</table>');
  print_source_info(4);
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#---------------------------------------------------------------------------------------------------

        # Marks in $Array the GPR regions of RAM.

sub mark_gpr_ram($$)
  {
  my ($Array, $Features) = @_;
  my ($start, $end, $size);
  my $mcu_class_features = $class_features_list[$Features->{MCU_CLASS}];
  my $bank_size = $mcu_class_features->{BANK_SIZE};
  my $bank_max  = $Features->{BANK_MAX};
  my $gpr       = $Features->{GPR_RAM};
  my $ram_size  = $bank_max * $bank_size;
  my $i;

  @{$Array} = ((RAM_BAD) x $ram_size);

  return if (! defined($gpr) || ! @{$gpr});

  for ($i = 0; $i < $bank_max; ++$i)
    {
    foreach (@{$gpr->[$i]})
      {
      $start = $_->{START};
      $end   = $_->{END};
      next if ($start < 0 || $end < 0);

      $size = $end - $start + 1;
      splice(@{$Array}, $start, $size, ((RAM_GPR) x $size));
      }
    }
  }

#---------------------------------------------------------------------------------------------------

        # Marks in $Array the non GPR regions of RAM.

sub mark_non_gpr_ram($$)
  {
  my ($Array, $Features) = @_;
  my ($addr, $bank, $offs, $bank_prev, $i, $k, $max_sfr, $sfr_count, $x);
  my $bank_num  = $Features->{BANK_NUM};
  my $bank_mask = $Features->{BANK_MASK};
  my $bad_ram   = $Features->{BAD_RAM};
  my $sfrs      = $Features->{SFRS};
  my $mcu_class_features = $class_features_list[$Features->{MCU_CLASS}];
  my $bank_size = $mcu_class_features->{BANK_SIZE};
  my $core_sfrs = $mcu_class_features->{CORE_SFRS};

        # Marks the areas that do not exist.

  foreach (@{$bad_ram})
    {
    my $start = $_->{START};
    my $size  = $_->{END} - $start + 1;

    splice(@{$Array}, $start, $size, ((RAM_BAD) x $size));
    }

  if ($mcu_class_features->{WORD_SIZE} < 16)
    {
        # Not exist GPR before the last SFR in bank.

    $bank_prev = -1;
    $max_sfr = 0;
    $sfr_count = @{$sfrs};
    $i = 0;
    while (TRUE)
      {
      $addr = $sfrs->[$i]->{ADDR};
      $bank = $addr & $bank_mask;
      $offs = $addr & ~$bank_mask;

      if ($bank_prev == $bank)
        {
        $max_sfr = $offs if ($max_sfr < $offs);
        }
      else
        {
        if ($bank_prev >= 0)
          {
        # In the previous bank there is no GPR before the last SFR.

          ++$max_sfr;
          splice(@{$Array}, $bank_prev, $max_sfr, ((RAM_BAD) x $max_sfr));
          }

        $bank_prev = $bank;
        $max_sfr = 0;
        }

      ++$i;

      if ($i == $sfr_count)
        {
        # In the current bank there is no GPR before the last SFR.

        ++$max_sfr;
        splice(@{$Array}, $bank, $max_sfr, ((RAM_BAD) x $max_sfr));
        last;
        }
      } # while (TRUE)
    }
  }

#---------------------------------------------------------------------------------------------------

sub is_valid_sfr($$)
  {
  my ($ArrayRef, $SfrAddress) = @_;
  my ($start, $end);

  return FALSE if (! defined($ArrayRef) || ! @{$ArrayRef});

  foreach (@{$ArrayRef})
    {
    $start = $_->{START};
    $end   = $_->{END};
    next if ($start < 0 || $end < 0);

    return TRUE if ($start <= $SfrAddress && $SfrAddress <= $end);
    }

  return FALSE;
  }

#---------------------------------------------------------------------------------------------------

        # Marks in $Array the SFR regions of RAM.

sub mark_sfr_ram($$)
  {
  my ($Array, $Features) = @_;
  my ($bank, $i);
  my $mcu_name  = $Features->{MCU_NAME};
  my $bank_num  = $Features->{BANK_NUM};
  my $sfr_addrs = $Features->{SFR_ADDRS};
  my $mcu_class_features = $class_features_list[$Features->{MCU_CLASS}];
  my $bank_size = $mcu_class_features->{BANK_SIZE};
  my $core_sfrs = $mcu_class_features->{CORE_SFRS};
  my $sfr       = $Features->{SFR_RAM};
  my ($start, $end, $size);

        # Places the core registers. These at the same address there is in the all banks.
        # (12 and 14 bit MCU.)

  if (defined($core_sfrs))
    {
    foreach (@{$core_sfrs})
      {
      last if ($mcu_name eq 'PIC17C42' && $_ > 0x0F);

      $bank = 0;
      for ($i = 0; $i < $bank_num; ++$i)
        {
        $Array->[$bank + $_] = RAM_SFR if (is_valid_sfr($sfr->[$i], $bank + $_));
        $bank += $bank_size;
        }
      }
    }

        # Places the SFRs.

  foreach (values %{$sfr_addrs})
    {
    $Array->[$_] = RAM_SFR;
    }
  }

#---------------------------------------------------------------------------------------------------

        # Marks in $Array the common regions of RAM.

sub mark_common_ram($$)
  {
  my ($Array, $Features) = @_;
  my ($start, $end, $size, $i);
  my $bank_num = $Features->{BANK_NUM};
  my $common   = $Features->{COMMON_RAM};

  return if (! defined($common) || ! @{$common});

  for ($i = 0; $i < $bank_num; ++$i)
    {
    foreach (@{$common->[$i]})
      {
      $start = $_->{START};
      $end   = $_->{END};
      next if ($start < 0 || $end < 0);

      $size = $end - $start + 1;
      splice(@{$Array}, $start, $size, ((RAM_COMMON) x $size));
      }
    }
  }

#---------------------------------------------------------------------------------------------------

        # Marks in $Array the mirrored regions of RAM.

sub mark_mirrored_ram($$)
  {
  my ($Array, $Features) = @_;
  my ($start, $end, $size, $i);
  my $bank_num = $Features->{BANK_NUM};
  my $mirror   = $Features->{MIRROR_RAM};

  return if (! defined($mirror) || ! @{$mirror});

  for ($i = 0; $i < $bank_num; ++$i)
    {
    foreach (@{$mirror->[$i]})
      {
      $start = $_->{START};
      $end   = $_->{END};
      next if ($start < 0 || $end < 0);

      $size = $end - $start + 1;
      splice(@{$Array}, $start, $size, ((RAM_MIRROR) x $size));
      }
    }
  }

#---------------------------------------------------------------------------------------------------

sub print_column_warning($$$)
  {
  my ($Align, $Bank_num, $Mcu16_bit) = @_;

  if ($Bank_num > 1 && ! $Mcu16_bit)
    {
    aOutl($Align, '<tr class="ramGap"><td></td></tr>');
    aOutfl($Align,
           "<tr><td colspan=%u class=\"ramSumEx\">It is possible that this colums is inaccurate due to the mirrored-SFRs.</td></tr>",
           $Bank_num * 2 + 1);
    }
  }

#---------------------------------------------------------------------------------------------------

        # Dump the RAM map of a MCU.

sub print_ram_map($)
  {
  my $Features = $_[0];
  my ($map, $bank, $height, $k, $r, $t, $x, $y);
  my $mcu_name     = mcu_name_correction($Features->{MCU_NAME});
  my $mcu_class    = $Features->{MCU_CLASS};
  my $class_pic16e = ($mcu_class == PROC_CLASS_PIC16E) ? TRUE : FALSE;
  my $mcu_class_features = $class_features_list[$mcu_class];
  my $bank_num     = $Features->{BANK_NUM};
  my $bank_size    = $mcu_class_features->{BANK_SIZE};
  my $sfrs         = $Features->{SFRS};
  my $sfr_names    = $Features->{SFR_NAMES};
  my $linearmem    = $Features->{LINEARMEM};       # Only in the enhanced pic14 MCUs.
  my $lin_name     = '';
  my $segments     = undef;
  my $c_expl       = '<span class="explanation">';
  my @bank_sum     = ();
  my @ram_array;
  my @map_array;

  $t = "$out_dir/${mcu_name}-$ram_tag.html";
  open($out_handler, '>', $t) || die "Could not create the \"$t\" file!\n";

  Log("Dump the RAM map of $mcu_name.", 4);

  if (defined($linearmem))
    {
    $lin_name = $linearmem->{NAME};
    $segments = $linearmem->{SEGMENTS};
    }

  mark_gpr_ram(\@ram_array, $Features);
  mark_non_gpr_ram(\@ram_array, $Features);
  mark_sfr_ram(\@ram_array, $Features);
  mark_common_ram(\@ram_array, $Features);
  mark_mirrored_ram(\@ram_array, $Features);

        #------------------------------------

        # Creates groups from the related fields of RAM.

  $bank = 0;
  for ($x = 0; $x < $bank_num; ++$x)
    {
    my $map_index = 0;
    my $t_prev = -1;
    my $addr = 0;
    my $size = 0;

    $map = $map_array[$x] = [];

    $y = 0;
    while (TRUE)
      {
      $t = $ram_array[$bank + $y];

      if ($t_prev == $t)
        {
        ++$size;
        }
      else
        {
        # The $size for the first time is zero.

        if ($size > 0)
          {
          $map->[$map_index] = { TYPE => $t_prev, ADDR => $addr, SIZE => $size };
          ++$map_index;
          }

        $t_prev = $t;
        $addr = $bank + $y;
        $size = 1;
        }

      ++$y;

      if ($y == $bank_size)
        {
        $map->[$map_index] = { TYPE => $t, ADDR => $addr, SIZE => $size };
        last;
        }
      } # while (TRUE)

    $bank += $bank_size;
    } # for ($x = 0; $x < $bank_num; ++$x)

        #------------------------------------

        # After a lot of work has finally can be written the html code.

  print_html_head($mcu_name);
  aOutl(2, '<body>');

  print_class_menu(-1, $Features);
  print_local_menu(\@mcu_menu_elems, $mcu_name, MCU_MENU_RAM);

        #------------------------------------

        # The table of RAM map of MCU.

  aOutl(4, '<table class="ramMap">');
  aOutfl(6, "<tr><th colspan=%u class=\"ramTableName\">$mcu_name</th></tr>", $bank_num * 2 + 1);
  aOutl(6, '<tr class="ramGap"><td></td></tr>');

        #------------------------------------

        # header

  aOutl(6, '<tr class="ramHead">');

  for ($x = 0; $x < $bank_num; ++$x)
    {
    aOutl(8, "<th class=\"ramBank\">Bank&nbsp;$x</th>");
    }

  aOutml(6, '</tr>', '<tr class="ramGap"><td></td></tr>');

        #------------------------------------

  aOutl(6, '<tr>');

  $k = int(256 / $bank_size) * 3;
  for ($x = 0; $x < $bank_num; ++$x)
    {
    my ($sfr_size, $gpr_size, $common_size, $mirror_size, $bad_size) = (0, 0, 0, 0, 0);

    $map = $map_array[$x];

    aOutml(8, '<td class="ramColumn">', '  <div class="ramColCont">');

    if ($class_pic16e)
      {
      $height = ($Features->{ACCESS} + 1) * $k;

      if ($x == 0)
        {
        aOutl(12, "<div class=\"ramAcc\" style=\"top: 0; height: ${height}px;\"></div>");
        }
      elsif (($x + 1) == $bank_num)
        {
        aOutfl(12, "<div class=\"ramAcc\" style=\"top: ${height}px; height: %upx;\"></div>",
                   ($bank_size * $k) - $height);
        }
      }

    foreach (@{$map})
      {
      my ($addr, $size) = ($_->{ADDR}, $_->{SIZE});
      my $l_e = '';
      my $tt_top;
      my $d;

      $height = $size * $k;

      if ($height < 50)
        {
        $d = 2;
        }
      elsif ($height < 150)
        {
        $d = 3;
        }
      elsif ($height < 450)
        {
        $d = 4;
        }
      else
        {
        $d = 5;
        }

      $tt_top = int($height / $d);

      given ($_->{TYPE})
        {
        when (RAM_SFR)
          {
          $t = 'ramSFR';
          $r = 'SFR';
          $sfr_size += $size;
          }

        when (RAM_GPR)
          {
          $t = 'ramGPR';
          $r = 'GPR';
          $gpr_size += $size;
          }

        when (RAM_COMMON)
          {
          $t = 'ramCOM';
          $r = 'Common';
          $common_size += $size;
          }

        when (RAM_MIRROR)
          {
          $t = 'ramMIR';
          $r = 'Mirrored';
          $mirror_size += $size;
          }

        default
          {
          $t = 'ramBAD';
          $r = 'Unimplemented';
          $bad_size += $size;
          }
        }

      aOut(12, "<div class=\"$t\" style=\"height: ${height}px\"><div class=\"ramTt\" style=\"top: ${tt_top}px\">");

      if ($_->{TYPE} == RAM_SFR)
        {
        my $name = $sfr_names->{$addr};

        if (! $class_pic16e)
          {
        # This a mirror SFR? (12 or 14 bit MCU)

          my $ad = $addr % $bank_size;

          $name = $sfr_names->{$bank_size + $ad} if (! defined($name));
          $name = $sfr_names->{$ad}              if (! defined($name));

          if (! defined($name))
            {
            printf STDERR "SFR not exist in 0x%03X address!\n", $ad;
            exit(1);
            }
          }
        elsif (! defined($name))
          {
          printf STDERR "SFR not exist in 0x%03X address!\n", $addr;
          exit(1);
          }

        Out("<a href=\"${remote_url}${mcu_name}-$sfr_tag.html#$name\">");
        $l_e = '</a>';
        } # if ($_->{TYPE} == RAM_SFR)

      if ($_->{TYPE} == RAM_GPR && defined($segments))
        {
        if ($size > 1)
          {
          my $end = $addr + $size - 1;

          Outf("$r<br>0x%03X - 0x%03X<br>$size bytes", $addr, $end);

        # Linear RAM segments.

          foreach (@{$segments})
            {
            my ($rstart, $lstart, $lsize) = ($_->{RSTART}, $_->{LSTART}, $_->{SIZE});

            last if ($end < $rstart);

            Outf("<br>$lin_name: 0x%04X<br>$lsize bytes", $lstart) if ($addr <= $rstart);
            }

          Outl('</div></div>');
          }
        else
          {
          Outfl("$r<br>0x%03X</div></div>", $addr);
          }
        }
      else
        {
        if ($size > 1)
          {
          Outfl("$r<br>0x%03X - 0x%03X<br>$size bytes$l_e</div></div>", $addr, $addr + $size - 1);
          }
        else
          {
          Outfl("$r<br>0x%03X$l_e</div></div>", $addr);
          }
        }
      } # foreach (@{$map})

    $bank_sum[$x] = { SFR => $sfr_size, GPR => $gpr_size, COMMON => $common_size, MIRROR => $mirror_size, BAD => $bad_size };

    aOutml(8, '  </div>', '</td>');
    }

  aOutl(6, '</tr>');
  print_column_warning(6, $bank_num, $class_pic16e);
  aOutml(4, '</table>', '<p></p>');

        #------------------------------------

        # Summary of banks.

  aOutml(4, '<table class="ramMap">', '  <tr>');

  for ($x = 0; $x < $bank_num; ++$x)
    {
    my $sum = $bank_sum[$x];

    aOutml(8, '<td class="ramColumn">',
              '  <div class="ramColCont">');
    aOutml(12, "<div class=\"ramSFR ramSum\">SFR<br>$sum->{SFR} bytes</div>",
               "<div class=\"ramGPR ramSum\">GPR<br>$sum->{GPR} bytes</div>");

    if ($bank_num > 1 && ! $class_pic16e)
      {
      aOutl(12, "<div class=\"ramCOM ramSum\">Common<br>$sum->{COMMON} bytes</div>");
      aOutl(12, "<div class=\"ramMIR ramSum\">Mirrored<br>$sum->{MIRROR} bytes</div>");
      }

    aOutl(12, "<div class=\"ramBAD ramSum\">Unimplemented<br>$sum->{BAD} bytes</div>");
    aOutl(10, '</div>');
    aOutl(8, '</td>');
    }

  aOutl(6, '</tr>');
  print_column_warning(6, $bank_num, $class_pic16e);
  aOutl(4, '</table>');

        #------------------------------------

  aOutml(4, '<div class="legendContainer">',
            '  <div class="legend">');

  aOutml(8, "<p class=\"ramSFREx\">&nbsp;${c_expl}Special Function Register.</span></p>",
            "<p class=\"ramGPREx\">&nbsp;${c_expl}General-purpose RAM.</span></p>");

  if ($bank_num > 1 && ! $class_pic16e)
    {
    aOutl(8, "<p class=\"ramCOMEx\">&nbsp;${c_expl}Common RAM.</span></p>");
    aOutl(8, "<p class=\"ramMIREx\">&nbsp;${c_expl}Mirrored RAM.</span></p>");
    }

  aOutml(6, "  <p class=\"ramBADEx\">&nbsp;${c_expl}In this place no RAM.</span></p>",
            '</div>');

  if ($class_pic16e)
    {
    aOutl (6, '<div class="legend">');
    aOutml(8, "<p class=\"ramSFREx\"><span class=\"ramAccEx\"></span>&nbsp;${c_expl}Special Function Register on Access Area.</span></p>",
              "<p class=\"ramGPREx\"><span class=\"ramAccEx\"></span>&nbsp;${c_expl}General-purpose RAM on Access Area.</span></p>",
              "<p class=\"ramBADEx\"><span class=\"ramAccEx\"></span>&nbsp;${c_expl}In this place no RAM on Access Area.</span></p>");
    aOutl (6, '</div>');
    }

  aOutl(4, '</div>');

  print_source_info(4);
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#---------------------------------------------------------------------------------------------------

        # Dump the SFR map of $Name MCU.

sub print_sfr_map($)
  {
  my $Features = $_[0];
  my ($bank, $i, $max_x, $x, $min_y, $max_y, $y, $t);
  my $mcu_name     = mcu_name_correction($Features->{MCU_NAME});
  my $mcu_class    = $Features->{MCU_CLASS};
  my $class_pic16e = ($mcu_class == PROC_CLASS_PIC16E) ? TRUE : FALSE;
  my $mcu_class_features = $class_features_list[$mcu_class];
  my $bank_num     = $Features->{BANK_NUM};
  my $bank_size    = $mcu_class_features->{BANK_SIZE};
  my $sfrs         = $Features->{SFRS};
  my $c_expl       = '<span class="explanation">';
  my $accessSfr    = 0xF00 + $Features->{ACCESS} + 1;
  my @bank_array   = ();

  $t = "$out_dir/${mcu_name}-$sfr_tag.html";
  open($out_handler, '>', $t) || die "Could not create the \"$t\" file!\n";

  Log("Dump the SFR map of $mcu_name.", 4);

        #------------------------------------

        # Tabulate the SFRs.

  $bank = -1;
  $min_y = ULONG_MAX;
  $max_y = -1;
  $i = -1;
  foreach (@{$sfrs})
    {
    my $addr = $_->{ADDR};
    my $bn   = int($addr / $bank_size);
    my $offs = $addr % $bank_size;

    if ($bank != $bn)
      {
      $bank = $bn;
      $bank_array[$i]->{MAX_OFFS} = $max_y if ($i >= 0);
      ++$i;
      $max_y = -1;
      $bank_array[$i]->{BANKNUM} = $bank;
      }

    push(@{$bank_array[$i]->{BANK}[$offs]}, $_);

    $min_y = $offs if ($min_y > $offs);
    $max_y = $offs if ($max_y < $offs);
    }

  $bank_array[$i]->{MAX_OFFS} = $max_y;      # This the last bank.

  $max_x = @bank_array;

        #------------------------------------

  print_html_head($mcu_name);
  aOutl(2, '<body>');

  print_class_menu(-1, $Features);
  print_local_menu(\@mcu_menu_elems, $mcu_name, MCU_MENU_SFR);

        #------------------------------------

        # The table of SFR map of the MCU.

  aOutl(4, '<table class="sfrMap">');
  aOutfl(6, "<tr><th colspan=%u class=\"sfrTableName\">$mcu_name</th></tr>", $max_x * 2 + 1);
  aOutl(6, '<tr class="sfrGap"><td></td></tr>');

        #------------------------------------

        # header

  aOutl(6, '<tr class="sfrHead">');

  for ($x = 0; $x < $max_x; ++$x)
    {
    aOutl(8, "<th class=\"sfrBank\">Bank&nbsp;$bank_array[$x]->{BANKNUM}</th>");
    }

  aOutml(6, '</tr>', '<tr class="sfrGap"><td></td></tr>');

  aOutl(6, '<tr>');

  for ($x = 0; $x < $max_x; ++$x)
    {
    my $bn = $bank_array[$x]->{BANK};
    my $last_bank = ($class_pic16e && ($x + 1) == $max_x) ? TRUE : FALSE;

    aOutl(8, '<td class="sfrColumn">');
    aOutl(10, '<table>');

    $max_y = $bank_array[$x]->{MAX_OFFS};
    for ($y = $min_y; $y <= $max_y; ++$y)
      {
      $t = $bn->[$y];

      if (defined($t))
        {
        my $v = @{$t};
        my ($name, $addr) = ($t->[0]{NAME}, $t->[0]{ADDR});
        my $second_class = ($last_bank && $addr >= $accessSfr) ? ' sfrAccess' : '';

        aOutl(12, "<tr id=\"$name\">");

        if ($v > 1)
          {
        # Exist min. one alias.

          aOut(14, "<th class=\"sfrNameX\">$name<span>");

          $i = 1;
          while (TRUE)
            {
            my $j = $i;

            ++$i;

            if ($i < $v)
              {
              Out("$t->[$j]{NAME}<br>");
              }
            else
              {
                # This is the last alias.

              Out("$t->[$j]{NAME}");
              last;
              }
            }

          Outl('</span></th>');
          }
        else
          {
          aOutl(14, "<th class=\"sfrName\">$name</th>");
          }

        aOutfl(14, "<td class=\"sfrAddr$second_class\">0x%03X</td>", $addr);
        aOutl (12, '</tr>');
        } # if (defined($t))
      else
        {
        aOutl(12, '<tr><th class="sfrSep">&nbsp;</th></tr>');
        }
      }

    aOutml(8, '  </table>', '</td>');
    }

  aOutl(6, '</tr>');

        #------------------------------------

  if ($bank_num > 1 && ! $class_pic16e)
    {
    aOutl (6, '<tr class="ramGap"><td></td></tr>');
    aOutfl(6, "<tr><td colspan=%u class=\"ramSumEx\">The mirror of [PCL, PCLATH, ...] are not shown.</td></tr>",
           $bank_num * 2 + 1);
    }

  aOutl(4, '</table>');

        #------------------------------------

  aOutml(4, '<div class="legendContainer">',
            '  <div class="legend">');
  aOutml(8, "<p class=\"sfrNameEx\">&nbsp;${c_expl}SFR</span></p>",
            "<p class=\"sfrNameXEx\">&nbsp;${c_expl}SFR with alias name.</span></p>");
  aOutl (6, '</div>');

  if ($class_pic16e)
    {
    aOutml(6, '<div class="legend">',
              "  <p class=\"sfrNameEx sfrAccess\">&nbsp;${c_expl}SFR on Access Area.</span></p>",
              '</div>');
    }

  aOutl(4, '</div>');

  print_source_info(4);
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#---------------------------------------------------------------------------------------------------

        # Print the menu of class of MCUs.

sub print_primary_menu($)
  {
  my $Selected = $_[0];

  aOutl(6, '<div class="tabs">');

  foreach (@primary_menu)
    {
    my $class = ($Selected == $_) ? ' class="selected"' : '';

    aOutl(8, "<a$class href=\"${remote_url}$_->{HREF}\">$_->{NAME}</a>");
    }

  aOutl(6, '</div>');
  }

#---------------------------------------------------------------------------------------------------

        # Print the class html files of MCUs.

sub create_class_htmls()
  {
  my ($elem, $func, $html);

  foreach $elem (@primary_menu)
    {
    $func = $elem->{PFUNC};

    next if (! defined($func));

    $html = "$out_dir/$elem->{HREF}";

    open($out_handler, '>', $html) || die "Could not create the \"$html\" file!\n";

    print_html_head($elem->{NAME});
    aOutml(2, '<body>',
              '  <div class="headContainer">');
    aOutml(6, '<div class="headSide">&nbsp;</div>',
              '<div class="heading">Informations about the PIC microcontrollers</div>',
              '<div class="headSide">&nbsp;</div>');
    print_primary_menu($elem);

        # print_mcu_list()
    $func->(4, $elem);
    aOutml(2, '  </div>', "</body>\n</html>");
    close($out_handler);
    }

        #-------------------------------

  make_sfr_common_lists();

  foreach $elem (@common_sfr_menu)
    {
    $html = "$out_dir/$elem->{HREF}";

    open($out_handler, '>', $html) || die "Could not create the \"$html\" file!\n";

    print_html_head($elem->{NAME});
    aOutl(2, '<body>');

    print_class_menu(PRI_COMMON_SFR, undef);

        # print_common_sfr_lists()
    $func = $elem->{PFUNC};
    $func->(4, $elem) if (defined($func));
    aOutml(2, "</body>\n</html>");
    close($out_handler);
    }
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                              @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  Preparation of the css file.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                              @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

my $page_width          = 1024;          # px
my $content_background  = '#FFFFFF';
my $tab_color           = '#0000E0';
my $tab_background      = '#E0E0F0';
my $tab_border_color    = '#C0C0C0';
my $tooltip_background1 = '#F0FFA1';
my $tooltip_background2 = '#A1FFC3';
my $border_width        = 1;             # px
my $attr_background1    = '#D7DEB2';
my $attr_background2    = '#B2DECD';
my $header_background   = '#CAB2DE';
my $ramSFR_color        = '#F48282';
my $ramColumn_width     = 130;           # px
my $stripe_color        = '#303030';
my $expl_font_size      = 0.75;          # em
my $expl_y_padding      = 0.2;           # em
my $ramAccEx_height     = $expl_font_size + 2.3 * $expl_y_padding;
my $Ex_color_width      = 50;            # px

#---------------------------------------------------------------------------------------------------

sub css_border_radius($$)
  {
  my ($Align, $Radius) = @_;

  foreach ('   -moz-',
           '-webkit-',
           '        ')
    {
    aOutl($Align, "${_}border-radius: $Radius;");
    }
  }

#---------------------------------------------------------------------------------------------------

sub css_shadow($$)
  {
  my ($Align, $Shadow) = @_;

  foreach ('   -moz-',
           '-webkit-',
           '        ')
    {
    aOutl($Align, "${_}box-shadow: $Shadow;");
    }
  }

#---------------------------------------------------------------------------------------------------

sub css_gradient($$)
  {
  my ($Align, $Gradient) = @_;

  foreach ('   -moz-',          # Firefox 3.6+
           '    -ms-',          # IE10+
           '     -o-',          # Opera 11.10+
           '-webkit-',          # Chrome 10+
           '        ')          # future CSS3 browsers
    {
    aOutl($Align, "background: ${_}linear-gradient($Gradient);");
    }

  aOutl($Align, "-pie-background:    linear-gradient($Gradient);");     # IE 6..8
  }

#---------------------------------------------------------------------------------------------------

sub css_vertical_lines($$$$)
  {
  my ($Align, $Start, $End, $Step) = @_;

  aOutl($Align, "box-shadow: ${Start}px 0 $stripe_color,");

  return if ($Start >= $End);

  while (TRUE)
    {
    aOut($Align + 12, "${Start}px 0 $stripe_color");
    $Start += $Step;

    if ($Start >= $End)
      {
      Outl(';');
      last;
      }

    Outl(',');
    }
  }

#---------------------------------------------------------------------------------------------------

sub create_css()
  {
  my $html = "$out_dir/$css";

  open($out_handler, '>', $html) || die "Could not create the \"$html\" file!\n";
  print $out_handler <<EOT
.headContainer
  {
  position: relative;
  width: ${page_width}px;
  margin: 0 auto;
  }

.headSide
  {
  display: inline-block;
  position: relative;
  float: left;
  }

.heading
  {
  position: relative;
  margin-top: 1em;
  margin-bottom: 1em;
  width: 900px;
  float: left;
  padding: 0.6em;
  text-align: center;
  white-space: nowrap;
  font-size: 2em;
  color: black;
  background: $tab_background;
EOT
;
  css_shadow(2, "0 0 1em $tab_border_color");
  css_gradient(2, 'left, #FF8080, #FFFF80, #80FF80, #8080FF, #FF80FF');
  print $out_handler <<EOT
  background: -webkit-gradient(linear, left top, right top, from(#FF8080), to(#FF80FF));
  filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#FF8080', endColorstr='#FF80FF');
  }

.tabs
  {
  display: inline-block;
  margin: 0;
  padding: 2em 1.5em 0;
  overflow: hidden;
  margin-bottom: -${border_width}px;
  }

.classMenu
  {
  height: 2em;
  margin: 1.5em 0 1.5em 3em;
  }

.tabs a, .classMenu a
  {
  z-index: 1;
  position: relative;
  padding: 0.5em 0.5em;
  display: table-cell;
  list-style: none;
  background: $tab_background;
  color: $tab_color;
  white-space: nowrap;
  text-decoration: none;
  text-align: center;
  vertical-align: middle;
  border: ${border_width}px solid $tab_border_color;
EOT
;
  css_shadow(2, "0 0 1em $tab_border_color");
  print $out_handler <<EOT
  }

.tabs a
  {
EOT
;
  css_border_radius(2, '0.5em 0.5em 0 0');
  print $out_handler <<EOT
  border-bottom: 0;
  }

.classMenu a
  {
EOT
;
  css_shadow(2, "0 0 1em $tab_border_color");
  print $out_handler <<EOT
  }

.tabs a.selected
  {
  z-index: 3;
EOT
;
  css_shadow(2, "0.2em 0.2em 0.8em #000000, inset 0 0 0.5em #C000C0");
  print $out_handler <<EOT
  background: $content_background;
  }

.tabs a:hover, .classMenu a:hover
  {
  z-index: 2;
EOT
;
  css_gradient(2, "$content_background, $tab_background 100%");
  print $out_handler <<EOT
  background: -webkit-gradient(linear, 0 0, 0 100%, from($content_background), to($tab_background));
  filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='$content_background', endColorstr='$tab_background');
  }

.tabs a:hover
  {
EOT
;
  css_shadow(2, "0.2em 0.2em 0.8em #000000");
  print $out_handler <<EOT
  transform: scale(1.1, 1.3) translateY(-4px);
  }

.tabs a.selected:hover
  {
  z-index: 3;
EOT
;
  css_shadow(2, "0.2em 0.2em 0.8em #000000, inset 0 0 0.5em #C000C0");
  print $out_handler <<EOT
  }

.classMenu a:hover
  {
EOT
;
  css_shadow(2, "0.2em 0.2em 0.8em #000000");
  print $out_handler <<EOT
  transform: scale(1.1, 1.3) translateY(6px);
  }

.classMenu a:active
  {
EOT
;
  css_shadow(2, "0 1px 2px $tab_border_color inset");
  print $out_handler <<EOT
  }

.heading,
.mcuListHeader, .mcuList, .classMenu a, .mcuLink, .mcuDSheet,
.featList, .featTableName, .featName,
.configList, .confTableName, .configWord, .confOptName,
.ramMap, .ramTableName, .ramBank,
.sfrMap, .sfrCommonTable, .sfrTableName, .sfrBank, .sfrName, .sfrNameX,
.srcInfo
  {
EOT
;
  css_border_radius(2, '0.5em');
  print $out_handler <<EOT
  }

.mcuList, .featList, .configList, .ramMap, .sfrMap, .sfrCommonTable
  {
  z-index: 2;
  position: relative;
  width: auto;
  padding: 1em 1em;
  background: $content_background;
  border: ${border_width}px solid $tab_border_color;
EOT
;
  css_shadow(2, "0.2em 0.2em 0.8em #000000, inset 0 0 0.5em #C000C0");
  print $out_handler <<EOT
  }

.mcuList
  {
  display: inline-block;
  min-width: 880px;
  }

.mcuTable
  {
  display: inline;
  table-layout: fixed;
  }

/*----------------------------------------------*/

.mcuListHeader, .featTableName, .confTableName, .ramTableName, .sfrTableName
  {
  background: $header_background;
  border-color: $header_background;
  border-style: outset;
  }

EOT
;
  my $i = 0;
  my $v = @mcu_feat_names;
  while (TRUE)
    {
    Out(".$class_features_by_mpasmx{$mcu_feat_names[$i]}->{CSS_CLASS}");
    ++$i;

    if ($i < $v)
      {
      Out(', ');
      }
    else
      {
      Outl();
      last;
      }
    }

  aOutml(2, "{",
            "padding: 0.2em 0.625em;",
            'text-align: center;',
            'font: bold 1em Georgia;',
            "}\n");

  foreach (@mcu_feat_names)
    {
    Outl(".$class_features_by_mpasmx{$_}->{CSS_CLASS}");
    aOutml(2, "{",
              "background: $class_features_by_mpasmx{$_}->{CSS_BGRND};",
              "}\n");
    }

  print $out_handler <<EOT
.mcuListHeader, .configWord,
.featValue,
.confSwName, .confSwValue, .confSwExpl,
.ramBank,
.sfrBank, .sfrAddr, .sfrSep
  {
  padding: 0.2em 0.625em;
  }

.mcuListHeader
  {
  text-align: left;
  white-space: nowrap;
  font-size: 1.2em;
  border-width: 4px;
  }

.mcuLink
  {
  display: block;
  width: 12em;
  padding: 0.14em 0 0.14em 0.6em;
  text-align: left;
  text-decoration: none;
  background: #49DDFF;
  border: 2px outset $attr_background1;
EOT
;
  css_border_radius(2, '0.5em');
  css_shadow(2, "0 0 2px 4px $attr_background1 inset");
  print $out_handler <<EOT
  }

.mcuDSheet
  {
  display: block;
  width: 12em;
  padding: 0.14em 0 0.14em 0.6em;
  text-align: center;
  text-decoration: none;
  background: #FFA949;
  border: 2px outset $attr_background2;
EOT
;
  css_border_radius(2, '0.5em');
  css_shadow(2, "0 0 2px 4px $attr_background2 inset");
  print $out_handler <<EOT
  }

.mcuLink:hover
  {
  background: $tooltip_background1;
EOT
;
  css_shadow(2, "0.2em 0.2em 0.8em #000000, 0 0 2px 4px $attr_background1 inset");
  print $out_handler <<EOT
  transform: scale(1.1, 1.3);
  }

.mcuDSheet:hover
  {
  background: $tooltip_background2;
EOT
;
  css_shadow(2, "0.2em 0.2em 0.8em #000000, 0 0 2px 4px $attr_background2 inset");
  print $out_handler <<EOT
  transform: scale(1.1, 1.3);
  }

.mcuLink:active, .mcuDSheet:active
  {
  border: 2px inset;
EOT
;
  css_shadow(2, "0 0 10px 6px #B7BD97 inset");
  print $out_handler <<EOT
  }

.featValue,
.confSwName, .confSwValue, .confSwExpl,
.sfrAddr
  {
  background: #BAE7B8;
  }

.confSwDefault
  {
  background: #E79C96;
  }

/*----------------------------------------------*/

.featTableName,
.confTableName, .configWord,
.ramBank, .ramTableName,
.sfrBank, .sfrTableName
  {
  text-align: center;
  border-width: 4px;
  }

.featTableName, .confTableName, .ramTableName, .sfrTableName
  {
  font:bold 1.5em Georgia;
  line-height: 1.75em;
  }

.mcuGap, .featGap, .confGap, .ramGap, .sfrGap
  {
  height: 1em;
  background: transparent;
  }

.configWord, .ramBank, .sfrBank
  {
  font-size: 1.3em;
  background: $attr_background1;
  border-color: $attr_background1;
  border-style: ridge;
  }

.featName, .featValue,
.confOptName, .confSwName, .confSwValue, .confSwExpl,
.sfrName, .sfrNameX, .sfrAddr
  {
  text-align: left;
  }

.featName, .confOptName, .sfrName, .sfrNameX, .sfrSep
  {
  padding: 0.5em 0.75em;
  }

.featName, .confOptName, .sfrName, .sfrNameX
  {
  border-style: solid;
  border-width: 0;
  }

.featName, .confOptName, .sfrName, .sfrNameEx
  {
  background: #A5BDE4;
  }

.ramColumn, .sfrColumn
  {
  width: ${ramColumn_width}px;
  padding: 5px;
  background: #DEFFFE;
  border: ${border_width}px solid $tab_border_color;
EOT
;
  css_border_radius(2, '0.3em');
  css_shadow(2, "0 0 0.2em $tab_border_color");
  print $out_handler <<EOT
  }

.ramColCont
  {
  position: relative;
  }

/* This a tooltip. */

.ramTt
  {
  display: none;
  font-size: 0.7em;
  text-align: center;
  position: relative;
  left: 20px;
  background: $tooltip_background1;
  padding: 0.2em 0.8em;
EOT
;
  css_border_radius(2, '0 1em 1em 2em');
  css_shadow(2, "0 0 1.5em $tab_border_color");
  print $out_handler <<EOT
  }

.ramGPR:hover .ramTt,
.ramSFR:hover .ramTt,
.ramCOM:hover .ramTt,
.ramMIR:hover .ramTt,
.ramBAD:hover .ramTt
  {
  z-index: 100;
  display: block;
  }

/* This accessories of the tooltip. This is a beak which point towards the left side. */

.ramGPR:hover .ramTt:before,
.ramSFR:hover .ramTt:before,
.ramCOM:hover .ramTt:before,
.ramMIR:hover .ramTt:before,
.ramBAD:hover .ramTt:before
  {
  display: block;
  position: absolute;
  content: "";
  width: 0;
  height: 0;
  top: 0;
  left: -15px;
  border-right: 15px solid $tooltip_background1;
  border-bottom: 5px solid transparent;
  }

.ramTt a
  {
  display: block;
  background: $tooltip_background1;
  text-decoration: none;
EOT
;
  css_border_radius(2, '0.875em 0.875em 0.875em 1.75em');
  css_shadow(2, "0 0 10px 10px $tooltip_background1 inset");
  print $out_handler <<EOT
  }

.ramTt a:link
  {
  color: #874EAD;
  }

.ramTt a:hover
  {
  background: #49DDFF;
EOT
;
  css_shadow(2, "0.2em 0.2em 0.8em #000000, 0 0 10px 10px $tooltip_background1 inset");
  print $out_handler <<EOT
  transform: scale(1.2, 1.3);
  }

.ramAcc
  {
  z-index: 5;
  position: absolute;
  width: 1px;
  height: 0;
  background: transparent;
EOT
;
  css_vertical_lines(2, 2, $ramColumn_width, 5);
  print $out_handler <<EOT
  }

.ramGPR, .ramGPREx
  {
  background: #76CB76;
  }

.ramSFR, .ramSFREx
  {
  background: $ramSFR_color;
  }

.ramCOM, .ramCOMEx
  {
  background: #DFA9FF;
  }

.ramMIR, .ramMIREx
  {
  background: #76CBFF;
  }

.ramBAD, .ramBADEx
  {
  background: $tab_border_color;
  }

.ramGPR, .ramSFR, .ramCOM, .ramMIR, .ramBAD
  {
  position: relative;
  width: ${ramColumn_width}px;
  }

.ramSFR .ramTt:active
  {
  background: #94A9FF;
  }

.ramMap, .sfrMap
  {
  min-width: 500px;
  }

.sfrCommonTable
  {
  min-width: 480px;
  }

.ramSum
  {
  padding: 0.4em 0;
  font-size: 0.75em;
  text-align: center;
  }

.sfrColumn
  {
  vertical-align: top;
  }

.sfrName, .sfrNameX, .sfrAddr, .sfrSep
  {
  font-size: 0.75em;
  }

.sfrNameX, .sfrNameXEx
  {
  background: #ECA4FC;
  }

.sfrNameX span
  {
  display: none;
  position: absolute;
  text-align: left;
  background: $tooltip_background1;
  padding: 0.2em 0.4em;
EOT
;
  css_border_radius(2, '0 0.75em 0.75em 0.75em');
  css_shadow(2, "0.2em 0.2em 0.8em #000000");
  print $out_handler <<EOT
  }

.sfrNameX:hover span
  {
  z-index: 100;                 /* This will surely be enough. */
  display: block;
  cursor: text;
  }

.ramSFREx, .ramGPREx, .ramCOMEx, .ramMIREx, .ramBADEx,
.sfrNameEx, .sfrNameXEx, .sfrAccess, .menuEx
  {
  padding: 0;
  width: ${Ex_color_width}px;
  margin-left: 20px;
  border: 1px solid #A0A0A0;
EOT
;
  css_border_radius(2, '3px');
  css_shadow(2, "0 0 0.5em $tab_border_color");
  print $out_handler <<EOT
  }

.sfrAccess
  {
  background: #C87548;
  color: #FDFFC7;
  }

.legendContainer
  {
  position: relative;
  width: ${page_width}px;
  margin: 1.5em auto 0 auto;
  }

.legend
  {
  display: inline-block;
  position: relative;
  vertical-align: top;
  width: 22em;
  }

.ramAccEx
  {
  position: absolute;
  width: 1px;
  height: ${ramAccEx_height}em;         /* .explanation->(font-size + padding-top + padding-bottom) */
  background: transparent;
EOT
;
  css_vertical_lines(2, 2, $Ex_color_width, 5);
  print $out_handler <<EOT
  }

.explanation, .ramSumEx
  {
  font-size: ${expl_font_size}em;
  background: #BAA78F;
EOT
;
  css_border_radius(2, '0.5em');
  print $out_handler <<EOT
  }

.explanation
  {
  position: relative;
  padding: ${expl_y_padding}em 0.4em;
  top: -1px;
  left: 55px;
  white-space: nowrap;
EOT
;
  css_shadow(2, "0 0 0.5em $tab_border_color");
  print $out_handler <<EOT
  }

.ramSumEx
  {
  padding: 0.3em 0.4em;
  }

.srcInfo
  {
  width: 92%;
  font-size: 0.8em;
  padding: 0.3em 0.5em;
  background: #BA8F94;
EOT
;
  css_shadow(2, "0 0 0.5em $tab_border_color");
  print $out_handler <<EOT
  }
EOT
;
  close($out_handler);
  }

#---------------------------------------------------------------------------------------------------

sub usage()
  {
  print <<EOT
Usage: $PROGRAM [options]

    Options are:

        -gp <path> or --gputils-path <path>

            Path to gputils source files.

        -mi <path> or --mp-info-file <path>

            The program on this path looks for 8bit_device.info file.

        -od <path> or --out-dir <path>

            The program on this path creates the all html files.

        -ru <url> or --remote-url <url>

            Remote URL to the all html files. For example: www.desert-ice.com
            This option enable the online use the generated html files.

        -l <file> or --list-file <file>

            The name of the MCUs, reads from the file. In this way independently
            from the gputils, you can specify the name of MCUs. (The file construction
            of is very simple: One name per line. The name of MCUs, full length
            shall be described. For example: pic16f1933)

        -oc or --only-css

            Only make the $out_dir/$css file.

        -v <level> or --verbose <level>

            It provides information on from the own operation.
            Possible value of the level between 0 and 10. (default: 0)

        -h or --help

            This text.
EOT
;
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  The main program.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

$PROGRAM = basename($0);
$remote_url = '';
$list_file  = '';

for (my $i = 0; $i < scalar(@ARGV); )
  {
  my $opt = $ARGV[$i++];

  given ($opt)
    {
    when (/^-(gp|-gputils-path)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $gputils_path = $ARGV[$i++];
      }

    when (/^-(mi|-mp-info-file)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $dev_info = $ARGV[$i++];
      }

    when (/^-(od|-out-dir)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $out_dir = $ARGV[$i++];
      }

    when (/^-(ru|-remote-url)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $remote_url = "http://$ARGV[$i++]/";
      }

    when (/^-(l|-list-file)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $list_file = $ARGV[$i++];
      }

    when (/^-(oc|-only-css)$/o)
      {
      $only_css = TRUE;
      }

    when (/^-(v|-verbose)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $verbose = int($ARGV[$i++]);
      $verbose = 0 if (! defined($verbose) || $verbose < 0);
      $verbose = 10 if ($verbose > 10);
      }

    when (/^-(h|-help)$/o)
      {
      usage();
      exit(0);
      }

    default
      {
      die "Unknown option: \"$opt\"!\n";
      }
    } # given ($opt)
  }

die "This directory - $gputils_path - not exist!" if (! -d $gputils_path);

read_gp_svn_version();

$gpproc_path  = "$gputils_path/libgputils/$gpprocessor_c";

if ($list_file ne '')
  {
  read_list_file();
  }
else
  {
  extract_mcu_names();
  }

if (! -e $out_dir)
  {
  mkdir($out_dir) || die "Could not create $out_dir directory!";
  }

die "This file - $out_dir - is not directory!" if (! -d $out_dir);

if ($only_css)
  {
  create_css();
  exit(0);
  }

find_inc_files("$gputils_path/header");
read_device_informations();
add_missing_directives();
create_css();
create_class_htmls();

foreach (sort { smartSort($a, $b) } keys %mcus_by_names)
  {
  print_all_config_word($mcus_by_names{$_});
  print_features($mcus_by_names{$_});
  print_ram_map($mcus_by_names{$_});
  print_sfr_map($mcus_by_names{$_});
  }

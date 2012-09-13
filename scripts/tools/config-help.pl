#!/usr/bin/perl -w

=back

    Copyright (C) 2012, Molnár Károly <proton7@freemail.hu>

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

   -------------------------------------------------------------------

    This program prepares the html lists from Config words and other properties
    of the PIC MCU-s. The data reads from the 8bit_device.info called file of MPLAB-X.
    On Linux is usually located on this path:

                /opt/microchip/mplabx/mpasmx/8bit_device.info

    Of course for the program it is necessary to the gputils source from the svn.

    $Id$
=cut

use strict;
use warnings;
use 5.12.0;                     # when (regex)
use feature 'switch';
use POSIX 'ULONG_MAX';

use constant FALSE => 0;
use constant TRUE  => 1;

use constant ST_WAIT   => 0;
use constant ST_LISTEN => 1;

my $PROGRAM = 'config-help.pl';

my $verbose = 0;

my $gputils_url = 'gputils.sourceforge.net';
my $mplabx_url  = 'www.microchip.com/pagehandler/en-us/family/mplabx';

my $gputils_local_repo = "$ENV{HOME}/svn_snapshots/gputils";
my $gputils_path       = "$gputils_local_repo/gputils";
my $gpprocessor_c      = 'gpprocessor.c';
my $gpproc_path;
my $svn_rev = -1;

my $dev_info = '/opt/microchip/mplabx/mpasmx/8bit_device.info';
my $dev_info_rev = '';
my $list_file;

my $pic_name_mask = qr/PIC1(2(C[ER]?|HV)\d+\w+|6(C[ER]?|HV)\d+\w+|[0268](C|L?F)\d+\w+)|PIC(RF\d+\w+|MCV\d+\w+)|PS\d+/;
my $header = ('=' x 70);

my $HTML_1_Frameset = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">';
my $remote_url = '';
my $out_dir  = 'html-help';
my $css      = 'main.css';
my $conf_tag = 'conf';
my $feat_tag = 'feat';
my $ram_tag  = 'ram';
my $sfr_tag  = 'sfr';

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

my %mcu_features =
  (
  '16c5x'  => {
              CLASS     => PROC_CLASS_PIC12,
              NAME      => '12 bit MCU',
              CSS_CLASS => 'mcuAttrP12',
              CSS_BGRND => '#FFB4B4',
              ENHANCED  => FALSE,
              PAGE_SIZE => 512,
              WORD_SIZE => 12,
              CONF_SIZE => 12,
              EE_START  => 0,
              BANK_SIZE => 32,
              CORE_SFRS => [
                           0x00, 0x02, 0x03, 0x04
                           ]
              },

  '16c5xe' => {
              CLASS     => PROC_CLASS_PIC12E,
              NAME      => '12 bit enhanced MCU',
              CSS_CLASS => 'mcuAttrP12E',
              CSS_BGRND => '#FFB4FF',
              ENHANCED  => TRUE,
              PAGE_SIZE => 512,
              WORD_SIZE => 12,
              CONF_SIZE => 12,
              EE_START  => 0,
              BANK_SIZE => 32,
              CORE_SFRS => [
                           0x00, 0x02, 0x03, 0x04
                           ]
              },

  '16xxxx' => {
              CLASS     => PROC_CLASS_PIC14,
              NAME      => '14 bit MCU',
              CSS_CLASS => 'mcuAttrP14',
              CSS_BGRND => '#B4B4FF',
              ENHANCED  => FALSE,
              PAGE_SIZE => 2048,
              WORD_SIZE => 14,
              CONF_SIZE => 14,
              EE_START  => 0x2100,
              BANK_SIZE => 128,
              CORE_SFRS => [
                           0x00, 0x02, 0x03, 0x04,
                           0x0A, 0x0B
                           ]
              },

  '16exxx' => {
              CLASS     => PROC_CLASS_PIC14E,
              NAME      => '14 bit enhanced MCU',
              CSS_CLASS => 'mcuAttrP14E',
              CSS_BGRND => '#B4FFFF',
              ENHANCED  => TRUE,
              PAGE_SIZE => 2048,
              WORD_SIZE => 14,
              CONF_SIZE => 16,
              EE_START  => 0xF000,
              BANK_SIZE => 128,
              CORE_SFRS => [
                           0x00, 0x01, 0x02, 0x03,
                           0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B
                           ]
              },

  '17xxxx' => {
              CLASS     => PROC_CLASS_PIC16,
              NAME      => '16 bit MCU',
              CSS_CLASS => 'mcuAttrP16',
              CSS_BGRND => '#B4FFB4',
              ENHANCED  => FALSE,
              PAGE_SIZE => 0,
              WORD_SIZE => 16,
              CONF_SIZE => 8,
              EE_START  => 0,
              BANK_SIZE => 256,
              CORE_SFRS => [
                           0x00, 0x01, 0x02, 0x03,
                           0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B,
                           0x0C, 0x0D, 0x0E, 0x0F
                           ]
              },

  '18xxxx' => {
              CLASS     => PROC_CLASS_PIC16E,
              NAME      => '16 bit extended MCU',
              CSS_CLASS => 'mcuAttrP16E',
              CSS_BGRND => '#FFFFB4',
              ENHANCED  => TRUE,
              PAGE_SIZE => 0,
              WORD_SIZE => 16,
              CONF_SIZE => 8,
              EE_START  => 0xF00000,
              BANK_SIZE => 256,
              CORE_SFRS => undef
              }
  );

my @mcu_feat_names = sort {
                          $mcu_features{$a}->{ENHANCED} <=> $mcu_features{$b}->{ENHANCED} ||
                          $mcu_features{$a}->{CLASS}    <=> $mcu_features{$b}->{CLASS}
                          } keys(%mcu_features);

#-----------------------------------------------

=back
        The structure of one element of the %mcus_by_names hash:

        {
        FEATURES => {
                    CLASS     => PROC_CLASS_PIC1XX,
                    CSS_CLASS => 'mcuAttrP1xx',
                    ENHANCED  => FALSE,
                    PAGE_SIZE => 0,
                    WORD_SIZE => 0,     # Instruction size of MCU.
                    CONF_SIZE => 0,     # Width of a config word.
                    EE_START  => 0,     # Start address of EEPROM.
                    BANK_SIZE => 0,     # Size of RAM Banks.
                    CORE_SFRS => [],

                    COFF      => 0,     # Coff ID of device. (16 bit wide)
                    PAGES     => 0,     # Number of ROM/FLASH pages.
                    MAX_RAM   => 0,     # The highest address of RAM.
                    RAM_SIZE  => 0,     # Full size of all SFR and GPR.
                    CF_START  => 0,     # Address of first Configuration byte/word.
                    CF_END    => 0,     # Address of last Configuration byte/word.

                # These addresses relative, compared to the beginning of the blocks.
                    ROM       => 0,     # Last address of ROM/FLASH.
                    FLASHDATA => 0,     # Last address of FLASH Data.
                    EEPROM    => 0,     # Last address of EEPROM.

                    CONFIGS   => 0,     # Number of Configuration bytes/words.
                    BANKS     => 0,     # Number of RAM Banks.
                    ACCESS    => 0,     # Last address of lower Access RAM of pic18f series.

                    OSCVAL    => {      # Oscillator Calibration Value.
                                 START => 0,
                                 END   => 0
                                 },

                    USERID    => {      # User ID.
                                 START => 0,
                                 END   => 0
                                 },

                    DEVID     => {      # Device ID.
                                 START => 0,
                                 END   => 0
                                 },

                    BAD_RAM   => [      # List of bad RAM sectors.
                                   {
                                   START => 0,
                                   END   => 0
                                   },

                                   ...

                                   {
                                   }
                                 ],

                    SFRS      => [      # List of SFRs.
                                   {
                                   NAME => '',
                                   ADDR => 0
                                   },

                                   ...

                                   {
                                   }
                                 ],

                    SFR_NAMES => {},    # List names of SFRs by addresses.
                    SFR_ADDRS => {}     # List addresses of SFRs by names.
                    },

        CONFIGS  => {
                    '300000' => [],

                    ...

                    '300005' => [
                                  {
                                  'HEAD' => 'CCP2MX',
                                  'NAME' => 'CCP2 MUX bit',
                                  'BITS' => [
                                              {
                                              'NAME'  => 'OFF',
                                              'VALUE' => 0,
                                              'EXPL'  => 'CCP2 input/output is multiplexed with RB3'
                                              },

                                              ...

                                              {}
                                            ],
                                  'MASK' => 1
                                  },

                                  ...

                                  {}
                                ],

                                ...

                                []
                    }
        }
=cut

my %mcus_by_names;

my %gp_mcus_by_names;

my $out_handler;

my $only_css = FALSE;

#-------------------------------------------------------------------------------

use constant PRI_MENU_ALL    => 0;
use constant PRI_MENU_ENH    => 1;
use constant PRI_MENU_EXT    => 2;
use constant PRI_MENU_REG    => 3;
use constant PRI_MENU_12_BIT => 4;
use constant PRI_MENU_14_BIT => 5;
use constant PRI_MENU_16_BIT => 6;
use constant PRI_MENU_RAM    => 7;
use constant PRI_MENU_ROM    => 8;
use constant PRI_MENU_EEPROM => 9;

my @pri_menu_elems =
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
    NAME  => 'by RAM size',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_RAM
    },

    {                                   # PRI_MENU_ROM
    HREF  => 'mcus-by-rom-size.html',
    NAME  => 'by ROM size',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_ROM
    },

    {                                   # PRI_MENU_EEPROM
    HREF  => 'mcus-by-eeprom-size.html',
    NAME  => 'by EEPROM size',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_EEPROM
    }
  );

use constant MCU_MENU_CONF => 0;
use constant MCU_MENU_FEAT => 1;
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

use constant RAM_BAD => 0;
use constant RAM_GPR => 1;
use constant RAM_SFR => 2;

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@                       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@  Auxiliary procedures.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@                       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

sub basename($)
  {
  return ($_[0] =~ /([^\/]+)$/) ? $1 : '';
  }

#-------------------------------------------------------------------------------

sub str2dec($)
  {
  my $Str = $_[0];

  return hex($1)   if ($Str =~ /^H'([[:xdigit:]]+)'$/io);
  return hex($1)   if ($Str =~ /^0x([[:xdigit:]]+)$/io);
  return int($Str) if ($Str =~ /^-?\d+$/o);

  die "This string not integer: \"$Str\"";
  }

#-------------------------------------------------------------------------------

sub Log
  {
  return if (pop(@_) > $verbose);
  foreach (@_) { print STDERR $_; }
  print STDERR "\n";
  }

#-------------------------------------------------------------------------------

sub Out
  {
  foreach (@_) { print $out_handler $_; }
  }

#-------------------------------------------------------------------------------

sub Outl
  {
  Out(@_);
  print $out_handler "\n";
  }

#-------------------------------------------------------------------------------

sub Outml
  {
  foreach (@_) { print $out_handler "$_\n"; }
  }

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

sub aOutl
  {
  aOut(@_);
  print $out_handler "\n";
  }

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

sub Outf
  {
  printf $out_handler (shift(@_), @_);
  }

#-------------------------------------------------------------------------------

sub Outfl
  {
  Outf(@_);
  print $out_handler "\n";
  }

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

sub aOutfl
  {
  aOutf(@_);
  print $out_handler "\n";
  }

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

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
    next if ($_ =~ /^\s*$/o);

    s/^\s*|\s*$//go;

        # Comment line.
    next if ($_ =~ /^#/o);

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

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

        # Reads SVN revision of the gputils source.

sub read_gp_svn_revision()
  {
  $svn_rev = -1;

  foreach (qx/cd $gputils_local_repo; svn info/)
    {
    if ($_ =~ /^\s*Revision\s*:\s*(\d+)\s*$/io)
      {
      $svn_rev = int($1);
      last;
      }
    }
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@  Handle the gpprocessor.c file.  @@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        # Extract the MCU names from gpprocessor.c file.

sub extract_mcu_names()
  {
  open(LIB, '<', $gpproc_path) || die "extract_mcu_names(): Can not open. -> \"$gpproc_path\"\n";

        # static struct px pics[] = {
        # { PROC_CLASS_PIC14E   , "__16F1526"     , { "pic16f1526"     , "p16f1526"       , "16f1526"         }, 0x1526,  4,   32, 0x001FFF, {       -1,       -1 }, { 0x008007, 0x008008 }, "16f1526_g.lkr"      },
        # { PROC_CLASS_EEPROM8  , "__EEPROM8"     , { "eeprom8"        , "eeprom8"        , "eeprom8"         }, 0x1FFF,  0,    0, 0x0000FF, {       -1,       -1 }, {       -1,       -1 }, NULL                 },
        # { PROC_CLASS_PIC14    , "__RF675H"      , { "rf675h"         , "rf675h"         , "rf675h"          }, 0x4675,  1,    2, 0x00217F, { 0x0003FF, 0x0020FF }, { 0x002007, 0x002007 }, "rf675h_g.lkr"       },

  my $in_table = FALSE;

  %gp_mcus_by_names = ();

  while (<LIB>)
    {
    chomp;

    if (! $in_table)
      {
      $in_table = TRUE if ($_ =~ /^\s*static\s+struct\s+px\s+pics\[\s*\]\s*=\s*\{\s*$/io);
      }
    elsif ($_ =~ /\{\s*PROC_CLASS_\w+\s*,\s*"\w+"\s*,\s*\{\s*"(\w+)"\s*,\s*"\w+"\s*,\s*"\w+"\s*}\s*,\s*([\w-]+)\s*,\s*([\w-]+)\s*,\s*([\w-]+)\s*,\s*\S+\s*,\s*\{\s*\S+\s*,\s*\S+\s*\}\s*,\s*{\s*\S+\s*,\s*\S+\s*\}\s*,\s*"?[\.\w]+"?\s*\}/io)
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

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@                        @@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@  Handle the .inc files.  @@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@                        @@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

use constant INC_NULL => 0;
use constant INC_SFR  => 1;
use constant INC_BITS => 2;
use constant INC_RAM  => 3;

        # Read the RAM features from the $Inc file.

sub read_ram_features($$)
  {
  my ($Inc, $Features) = @_;
  my ($line, $full_ram, $state);
  my ($sfrs, $sfr_names, $sfr_addrs, $bad_ram);

  open(INC, '<', $Inc) || die "Could not open for reading: $Inc\n";

  Log("Read the RAM features from $Inc.", 4);

  $full_ram = 0;
  $sfrs      = [];
  $sfr_names = {};
  $sfr_addrs = {};
  $bad_ram   = [];
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
          my $name = $1;
          my $addr = str2dec($2);

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
        elsif ($line =~ /^__BADRAM\s+/io)
          {

        # __BADRAM  H'000F'
        # __BADRAM  H'0013'-H'0014'
        # __BADRAM  H'118'-H'1FF', H'218'-H'2FF', H'318'-H'3FF'
        # __BADRAM  H'0F9C'
        # __BADRAM  H'0FA3'-H'0FA5'

          foreach (split(/\s*,\s*/o, ${^POSTMATCH}))
            {
            if ($_ =~ /^H'([[:xdigit:]]+)'\s*-\s*H'([[:xdigit:]]+)'$/io)
              {
              my ($s, $e) = (hex($1), hex($2));

              if ($s > $e)
                {
                # It is unlikely that the values in reverse order there are,
                # but it is advisable to pay attention.

                my $t = $s;

                $s = $e;
                $e = $t;
                }

              push(@{$bad_ram}, { START => $s, END => $e });
              $full_ram -= $e - $s + 1;
              }
            elsif ($_ =~ /^H'([[:xdigit:]]+)'$/io)
              {
              my $s = hex($1);

              push(@{$bad_ram}, { START => $s, END => $s });
              --$full_ram;
              }
            else
              {
              die "Unknown value in \"$Inc\" file: $_\n";
              }
            } # foreach (split(/\s*,\s*/o, ${^POSTMATCH}))
          }
        } # when (INC_RAM)
      } # given ($state)
    } # foreach (grep(! /^\s*$/o, <INC>))

  close(INC);

  $Features->{RAM_SIZE}  = $full_ram;
  $Features->{BAD_RAM}   = $bad_ram;
  $Features->{SFR_NAMES} = $sfr_names;
  $Features->{SFR_ADDRS} = $sfr_addrs;

  @{$sfrs} = sort {$a->{ADDR} <=> $b->{ADDR}} @{$sfrs};
  $Features->{SFRS}      = $sfrs;
  }

#-------------------------------------------------------------------------------

        # Read the ROM features from the $Lkr file.

sub read_rom_features($$)
  {
  my ($Lkr, $Features) = @_;
  my $line;

  open(LKR, '<', $Lkr) || die "Could not open for reading: $Lkr\n";

  Log("Read the ROM features from $Lkr.", 4);

  foreach (grep(! /^\s*$/o, <LKR>))
    {
    chomp;
    s/\r$//o;
    s/^\s*|\s*$//go;
    $line = $_;

    if ($line =~ /^CODEPAGE\s+NAME=(\S+)\s+START=(\w+)\s+END=(\w+)\s+PROTECTED$/io)
      {
        # CODEPAGE   NAME=.oscval    START=0x3FF             END=0x3FF          PROTECTED
        # CODEPAGE   NAME=oscval     START=0x3FF             END=0x3FF          PROTECTED

        # CODEPAGE   NAME=.idlocs    START=0x2000            END=0x2003         PROTECTED
        # CODEPAGE   NAME=.idlocs    START=0x8000            END=0x8003         PROTECTED
        # CODEPAGE   NAME=userid     START=0x200000          END=0x200007       PROTECTED

        # CODEPAGE   NAME=.device_id START=0x2006            END=0x2006         PROTECTED
        # CODEPAGE   NAME=.devid     START=0x2006            END=0x2006         PROTECTED
        # CODEPAGE   NAME=devid      START=0x2006            END=0x2006         PROTECTED
        # CODEPAGE   NAME=.devid     START=0x8006            END=0x8006         PROTECTED
        # CODEPAGE   NAME=devid      START=0x3FFFFE          END=0x3FFFFF       PROTECTED

      my ($name, $start, $end) = ($1, $2, $3);

      given ($name)
        {
        when (/oscval$/io)
          {
          $Features->{OSCVAL} = { START => str2dec($start), END => str2dec($end) };
          }

        when (/(idlocs|^userid)$/io)
          {
          $Features->{USERID} = { START => str2dec($start), END => str2dec($end) };
          }

        when (/dev(ice_)?id$/io)
          {
          $Features->{DEVID} = { START => str2dec($start), END => str2dec($end) };
          }
        }
      }
    } # foreach (grep(! /^\s*$/o, <LKR>))

  close(LKR);
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@                                                  @@@@@@@@@@@@@@@@
#@@@@@@@@@@@@  Read all config options from the $dev_info file.  @@@@@@@@@@@@@@@
#@@@@@@@@@@@@@                                                  @@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

sub read_all_config_bits()
  {
  my ($configs, $name, $inc, $lkr, $class);
  my $features = undef;
  my $addr = 0;
  my $config_count = 0;
  my $switch_count = 0;
  my $setting_count = 0;
  my $switch_info = undef;
  my $state = ST_WAIT;
  my ($cf_addr_min, $cf_addr_max);

  open(INFO, '<', $dev_info) || die "Could not open for reading: $dev_info\n";

  Log("Reads all config options from $dev_info.", 4);

  my $msg = ($list_file ne '') ? 'list file' : 'Gputils';

  while (<INFO>)
    {
    my @fields = ($_ =~ /<([^<>]*)>/go);

    if ($fields[0] eq 'RES_FILE_VERSION_INFO_TYPE')
      {
      $dev_info_rev = $fields[1];
      }

    next if (@fields < 3);

    if ($fields[0] eq 'PART_INFO_TYPE')
      {
        # <PART_INFO_TYPE><f220><PIC10F220><16c5x><0><0><ff><1><1f><0><0><0><1>
        # <PART_INFO_TYPE><e529><PIC12F529T39A><16c5xe><0><3><5ff><8><1f><0><0><3f><1>
        # <PART_INFO_TYPE><6628><PIC16F628><16xxxx><0><1><7ff><4><7f><7f><0><0><1>
        # <PART_INFO_TYPE><a829><PIC16LF1829><16Exxx><2><4><1fff><20><7f><ff><0><0><2>
        # <PART_INFO_TYPE><1330><PIC18F1330><18xxxx><6><1><1fff><10><ff><7f><7f><0><c>

      if ($fields[2] =~ /^$pic_name_mask$/io)
        {
        $name          = uc($fields[2]);
        $class         = lc($fields[3]);
        $config_count  = hex($fields[12]);
        $switch_count  = 0;
        $setting_count = 0;
        $cf_addr_min   = ULONG_MAX;
        $cf_addr_max   = 0;

        my $tr = $mcu_features{$class};

        die "Unknown class of $name MCU!" if (! defined($tr));

        $features = {
                    CLASS     => $tr->{CLASS},
                    CSS_CLASS => $tr->{CSS_CLASS}, # Css class name.
                    CSS_BGRND => $tr->{CSS_BGRND}, # Background color.
                    ENHANCED  => $tr->{ENHANCED},
                    PAGE_SIZE => $tr->{PAGE_SIZE}, # Size of program memory pages.
                    WORD_SIZE => $tr->{WORD_SIZE}, # Size of instructions.
                    CONF_SIZE => $tr->{CONF_SIZE}, # Size of Config Words.
                    EE_START  => $tr->{EE_START},  # Start address of EEPROM.
                    BANK_SIZE => $tr->{BANK_SIZE}, # Size of RAM Banks.
                    CORE_SFRS => $tr->{CORE_SFRS}, # Core SFRs of 12 and 14 bit MCUs.

                    COFF      => hex($fields[1]),  # Coff ID of device. (16 bit wide)
                    PAGES     => hex($fields[5]),  # Number of ROM/FLASH pages.
                    MAX_RAM   => 0,                # The highest address of RAM.
                    RAM_SIZE  => 0,                # Full size of all SFR and GPR.
                    CF_START  => 0,                # Address of first Configuration byte/word.
                    CF_END    => 0,                # Address of last Configuration byte/word.

                # These addresses relative, compared to the beginning of the blocks.
                    ROM       => hex($fields[6]),  # Last address of ROM/FLASH.
                    FLASHDATA => hex($fields[11]), # Last address of FLASH Data.
                    EEPROM    => hex($fields[9]),  # Last address of EEPROM.

                    CONFIGS   => hex($fields[12]), # Number of Configuration bytes/words.
                    BANKS     => hex($fields[7]),  # Number of RAM Banks.
                    ACCESS    => hex($fields[10]), # Last address of lower Access RAM of pic18f series.
                    OSCVAL    => undef,            # Oscillator Calibration Value.
                    USERID    => undef,            # User ID.
                    DEVID     => undef,            # Device ID.
                    BAD_RAM   => [],               # List of bad RAM sectors.
                    SFRS      => [],               # List of SFRs.
                    SFR_NAMES => {},               # List names of SFRs by addresses.
                    SFR_ADDRS => {}                # List addresses of SFRs by names.
                    };

        $inc = $gp_mcus_by_names{$name};

        if (defined($inc) && $inc ne '')
          {
          $lkr = $inc;
          $lkr =~ s/^p//o;
          read_ram_features("$gputils_path/header/${inc}.inc", $features);
          read_rom_features("$gputils_path/lkr/${lkr}_g.lkr", $features);
          $state = ST_LISTEN;
          $addr = 0;
          $configs = {};
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

          die "Too much the number of \"CONFIGREG_INFO_TYPE\"!\n" if ($config_count <= 0);

          $switch_count = hex($fields[4]);
          $addr = hex($fields[1]);
          $cf_addr_min = $addr if ($cf_addr_min > $addr);
          $cf_addr_max = $addr if ($cf_addr_max < $addr);
          --$config_count;
          } # when ('CONFIGREG_INFO_TYPE')

        when ('SWITCH_INFO_TYPE')
          {
        # <SWITCH_INFO_TYPE><FOSC><Oscillator Selection bits><7><8>
        # <SWITCH_INFO_TYPE><DEBUG><In-Circuit Debugger Mode bit><2000><2>

          die "Too much the number of \"SWITCH_INFO_TYPE\"!\n" if ($switch_count <= 0);

          $switch_info = {
                         HEAD => $fields[1],
                         NAME => (defined($fields[2]) ? $fields[2] : ''),
                         BITS => [],
                         MASK => hex($fields[3])
                         };

          $setting_count = hex($fields[4]);
          push(@{$configs->{$addr}}, $switch_info);
          --$switch_count;
          } # when ('SWITCH_INFO_TYPE')

        when ('SETTING_VALUE_TYPE')
          {
        # <SETTING_VALUE_TYPE><LP><LP oscillator: Low-power crystal on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN><0>
        # <SETTING_VALUE_TYPE><OFF><WDT disabled and can be enabled by SWDTEN bit of the WDTCON register><0>
        # <SETTING_VALUE_TYPE><ON><WDT enabled><8>
        # <SETTING_VALUE_TYPE><2><><10>

          die "Too much the number of \"SETTING_VALUE_TYPE\"!\n" if ($setting_count <= 0);
          die "There is no actual \"SWITCH_INFO_TYPE\"!\n" if (! defined($switch_info));

          my $setting = {
                        NAME  => $fields[1],
                        VALUE => hex($fields[3]),
                        EXPL  => (defined($fields[2]) ? $fields[2] : '')
                        };

          push(@{$switch_info->{BITS}}, $setting);
          --$setting_count;

          if (! $setting_count && ! $switch_count && ! $config_count)
            {
        # All information is together.
            die "$name MCU already exist!" if (defined($mcus_by_names{$name}));

            $features->{CF_START} = $cf_addr_min;
            $features->{CF_END}   = $cf_addr_max;
            $mcus_by_names{$name}{FEATURES} = $features;
            $mcus_by_names{$name}{CONFIGS}  = $configs;
            $configs = {};
            }
          } # when ('SETTING_VALUE_TYPE')
        } # given ($fields[0])
      } # if ($state == ST_LISTEN)
    } # while (<INFO>)

  close(INFO);
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@                            @@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@  Preparation of html files.  @@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@                            @@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        # Print the head of html file.

sub print_html_head($)
  {
  print $out_handler <<EOT
$HTML_1_Frameset
<html lang="en">
  <head>
    <title>$_[0]</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <link rel="stylesheet" type="text/css" href="$remote_url$css">
  </head>
EOT
;
  }

#-------------------------------------------------------------------------------

sub src_info($)
  {
  my $Align = $_[0];
  my $href = "<a href=\"http://${gputils_url}#Download\">gputils</a> source package";

  aOutl($Align, '<div class="legendContainer">');
  aOutl($Align + 2, "<p class=\"srcInfo\">This page generated automatically by the " .
                    "<em>$PROGRAM</em> program from the <em>" . basename($dev_info) .
                    "</em> file (rev: $dev_info_rev) of <em>mpasmx</em> and from the $href " .
                    "(rev: svn $svn_rev). The <em>mpasmx</em> is included in the " .
                    "<a href=\"http://$mplabx_url\">MPLAB X</a>. The <em>$PROGRAM</em> " .
                    "is included in the $href.</p>");
  aOutl($Align, '</div>');
  }

#-------------------------------------------------------------------------------

        # Prepares multiple list of MCUs.

sub print_mcu_list($$)
  {
  my ($Align, $Class) = @_;
  my $lst = '<td class="mcuListHeader">';
  my @array = ();

  Log("Print list of MCUs.", 4);

  aOutml($Align + 2, '<div class="mcuList">',
                     '  <table class="mcuTable">',
                     '    <tr>');
  aOutml($Align + 8, '<td class="vMargin"></td>',
                     "${lst}name</th>");

  given ($Class)
    {
    when (PRI_MENU_ALL)
      {
      aOutml($Align + 8, "${lst}instruction size (bit)</td>",
                         "${lst}config word size (bit)</td>",
                         "${lst}class</th>");
      }

    when ([ PRI_MENU_ENH, PRI_MENU_EXT, PRI_MENU_REG ])
      {
      aOutml($Align + 8, "${lst}instruction size (bit)</td>",
                         "${lst}config word size (bit)</td>");
      }

    when (PRI_MENU_RAM)
      {
      aOutml($Align + 8, "${lst}RAM size (byte)</th>",
                         "${lst}instruction size (bit)</td>",
                         "${lst}config word size (bit)</td>",
                         "${lst}class</th>");
      }

    when (PRI_MENU_ROM)
      {
      aOutml($Align + 8, "${lst}ROM size (word/byte)</td>",
                         "${lst}instruction size (bit)</td>",
                         "${lst}config word size (bit)</td>",
                         "${lst}class</td>");
      }

    when (PRI_MENU_EEPROM)
      {
      aOutml($Align + 8, "${lst}EEPROM size (byte)</td>",
                         "${lst}instruction size (bit)</td>",
                         "${lst}config word size (bit)</td>",
                         "${lst}class</td>");
      }

    default
      {
      aOutml($Align + 8, "${lst}config word size (bit)</td>",
                         "${lst}class</td>");
      }
    }

  aOutml($Align + 6, '  <td class="vMargin"></td>',
                     '</tr>',
                     '<tr class="mcuGap"><td></td></tr>');

  if ($Class == PRI_MENU_RAM)
    {
    @array = sort { $mcus_by_names{$a}->{FEATURES}{RAM_SIZE} <=> $mcus_by_names{$b}->{FEATURES}{RAM_SIZE} ||
                    smartSort($a, $b) } keys %mcus_by_names;
    }
  elsif ($Class == PRI_MENU_ROM)
    {
    @array = sort { $mcus_by_names{$a}->{FEATURES}{ROM} <=> $mcus_by_names{$b}->{FEATURES}{ROM} ||
                    smartSort($a, $b) } keys %mcus_by_names;
    }
  elsif ($Class == PRI_MENU_EEPROM)
    {
    foreach (keys %mcus_by_names)
      {
      push(@array, $_) if ($mcus_by_names{$_}->{FEATURES}{EEPROM} > 0);
      }

    @array = sort { $mcus_by_names{$a}->{FEATURES}{EEPROM} <=> $mcus_by_names{$b}->{FEATURES}{EEPROM} ||
                    smartSort($a, $b) } @array;
    }
  else
    {
    @array = sort { smartSort($a, $b) } keys %mcus_by_names;
    }

  foreach my $name (@array)
    {
    my $margin    = '<td class="vMargin"></td>';
    my $td_href   = "<th class=\"mcuLink\"><a href=\"$remote_url${name}-$feat_tag.html\">$name</a></th>";
    my $features  = $mcus_by_names{$name}->{FEATURES};
    my $class     = $features->{CLASS};
    my $css_class = $features->{CSS_CLASS};
    my $wsize     = $features->{WORD_SIZE};
    my $enh       = ($wsize == 16) ? 'extended' : 'enhanced';
    my $td_wsize  = "<td class=\"$css_class\">$wsize</td>";
    my $td_csize  = "<td class=\"$css_class\">$features->{CONF_SIZE}</td>";
    my $td_class  = "<td class=\"$css_class\">" . (($features->{ENHANCED}) ? $enh : 'regular') . '</td>';

    given ($Class)
      {
      when (PRI_MENU_ALL)
        {
        aOutl($Align + 6, '<tr>');
        aOutml($Align + 8, $margin, $td_href, $td_wsize, $td_csize, $td_class, $margin);
        aOutl($Align + 6, '</tr>');
        }

      when (PRI_MENU_ENH)
        {
        if ($class == PROC_CLASS_PIC12E || $class == PROC_CLASS_PIC14E)
          {
          aOutl($Align + 6, '<tr>');
          aOutml($Align + 8, $margin, $td_href, $td_wsize, $td_csize, $margin);
          aOutl($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_EXT)
        {
        if ($class == PROC_CLASS_PIC16E)
          {
          aOutl($Align + 6, '<tr>');
          aOutml($Align + 8, $margin, $td_href, $td_wsize, $td_csize, $margin);
          aOutl($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_REG)
        {
        if (! $features->{ENHANCED})
          {
          aOutl($Align + 6, '<tr>');
          aOutml($Align + 8, $margin, $td_href, $td_wsize, $td_csize, $margin);
          aOutl($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_12_BIT)
        {
        if ($wsize == 12)
          {
          aOutl($Align + 6, '<tr>');
          aOutml($Align + 8, $margin, $td_href, $td_csize, $td_class, $margin);
          aOutl($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_14_BIT)
        {
        if ($wsize == 14)
          {
          aOutl($Align + 6, '<tr>');
          aOutml($Align + 8, $margin, $td_href, $td_csize, $td_class, $margin);
          aOutl($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_16_BIT)
        {
        if ($wsize == 16)
          {
          aOutl($Align + 6, '<tr>');
          aOutml($Align + 8, $margin, $td_href, $td_csize, $td_class, $margin);
          aOutl($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_RAM)
        {
        aOutl($Align + 6, '<tr>');
        aOutml($Align + 8, $margin,
                           "<th class=\"mcuLink\"><a href=\"$remote_url${name}-$ram_tag.html\">$name</a></th>",
                           "<td class=\"$css_class\">$features->{RAM_SIZE}</td>",
                           $td_wsize, $td_csize, $td_class, $margin);
        aOutl($Align + 6, '</tr>');
        }

      when (PRI_MENU_ROM)
        {
        aOutl($Align + 6, '<tr>');
        aOutml($Align + 8, $margin, $td_href);
        aOutfl($Align + 8, "<td class=\"$css_class\">%u</td>", $features->{ROM} + 1);
        aOutml($Align + 8, $td_wsize, $td_csize, $td_class, $margin);
        aOutl($Align + 6, '</tr>');
        }

      when (PRI_MENU_EEPROM)
        {
        aOutl($Align + 6, '<tr>');
        aOutml($Align + 8, $margin, $td_href);
        aOutfl($Align + 8, "<td class=\"$css_class\">%u</td>", $features->{EEPROM} + 1);
        aOutml($Align + 8, $td_wsize, $td_csize, $td_class, $margin);
        aOutl($Align + 6, '</tr>');
        }
      }
    }

  aOutml($Align + 2, '  </table>', '</div>');

        #------------------------------------

  aOutl($Align + 2, '<div class="legendContainer">');

  my $v = @mcu_feat_names;
  my $level;

  aOutl($Align + 4, '<div class="legend">');
  $level = 1;
  for (my $i = 0; $i < $v;)
    {
    my $cl = $mcu_features{$mcu_feat_names[$i]};

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

  src_info($Align + 2);
  }

#-------------------------------------------------------------------------------

        # Print the local menu of $Name MCU page.

sub print_mcu_menu($$)
  {
  my ($Name, $Selected) = @_;

  aOutl(4, '<ul class="tabs">');

  foreach (@mcu_menu_elems)
    {
    my $class = ($_->{CLASS} == $Selected) ? ' class="selected"' : '';

    aOutl(6, "<li$class><a href=\"$remote_url${Name}$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(4, '</ul>');
  }

#-------------------------------------------------------------------------------

sub print_devid($$$$)
  {
  my ($Align, $Devid, $Margin, $Length) = @_;

  if (defined($Devid))
    {
    my ($start, $end) = ($Devid->{START}, $Devid->{END});

    aOutl($Align, '<tr class="featLine">');
    aOutml($Align + 2, $Margin, '<th class="featName">Device ID</th>');
    aOutfl($Align + 2, "<td class=\"featValue\">0x%0${Length}X - 0x%0${Length}X   (%u bytes)</td>",
                       $start, $end, $end - $start + 1);

    aOutl($Align, '</tr>');
    }
  }

#-------------------------------------------------------------------------------

        # Dump the features of $Name MCU.

sub dump_features($$)
  {
  my ($Name, $Configs) = @_;
  my $features = $Configs->{FEATURES};
  my $margin   = '<td class="vMargin"></td>';
  my ($str, $len, $rom_size, $word_size, $i, $t);

  $str = "$out_dir/${Name}-$feat_tag.html";
  open($out_handler, '>', $str) || die "Could not create the \"$str\" file!\n";

  Log("Dump the features of $Name.", 4);

        #------------------------------------

  print_html_head($Name);
  aOutl(2, '<body>');

        # The main menu.

  aOutl(4, '<ul class="classMenu">');

  foreach (@pri_menu_elems)
    {
    aOutl(6, "<li><a href=\"$remote_url$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(4, '</ul>');

  print_mcu_menu($Name, MCU_MENU_FEAT);

        #------------------------------------

        # The table of features of MCU.

  $word_size = $features->{WORD_SIZE};
  $len = ($word_size == 16) ? 6 : 4;
  $rom_size = $features->{ROM} + 1;

  aOutl(4, '<table class="featList">');
  aOutml(6, "<tr><th colspan=4 class=\"featTableName\">$Name</th></tr>",
            '<tr class="featGap"><td></td></tr>');

        #------------------------------------

  aOutl(6, '<tr class="featLine">');
  aOutml(8, $margin, '<th class="featName">Coff ID of device</th>');
  aOutfl(8, '<td class="featValue">0x%04X</td>', $features->{COFF});
  aOutl(8, $margin);
  aOutl(6, '</tr>');

        #------------------------------------

  $i = $features->{PAGE_SIZE};
  if ($i > 0)
    {
    aOutl(6, '<tr class="featLine">');
    aOutml(8, $margin,
              '<th class="featName">Number of ROM/FLASH pages</th>',
              "<td class=\"featValue\">$features->{PAGES} ($i words/pages)</td>");
    aOutl(6, '</tr>');
    }

        #------------------------------------

  aOutl(6, '<tr class="featLine">');
  aOutml(8, $margin, '<th class="featName">Last address of ROM/FLASH</th>');

  $t = ($word_size == 16) ? 'bytes' : 'words';
  aOutfl(8, "<td class=\"featValue\">0x%0${len}X   ($rom_size $t)</td>", $features->{ROM});
  aOutl(6, '</tr>');

        #------------------------------------

  $i = $features->{OSCVAL};
  if (defined($i))
    {
    aOutl(6, '<tr class="featLine">');
    aOutml(8, $margin, '<th class="featName">Oscillator calibration value</th>');

    aOutfl(8, "<td class=\"featValue\">0x%0${len}X - 0x%0${len}X   (%u words)</td>",
               $i->{START}, $i->{END}, $i->{END} - $i->{START} + 1);

    aOutl(6, '</tr>');
    }

        #------------------------------------

  $i = $features->{FLASHDATA};
  if ($i > 0)
    {
    aOutl(6, '<tr class="featLine">');
    aOutml(8, $margin, '<th class="featName">Address space of FLASH Data</th>');

    aOutfl(8, "<td class=\"featValue\">0x%0${len}X - 0x%0${len}X   (%u words)</td>",
               $rom_size, $rom_size + $i, $i + 1);

    aOutl(6, '</tr>');
    }

        #------------------------------------

  $i = $features->{USERID};
  if (defined($i))
    {
    aOutl(6, '<tr class="featLine">');
    aOutml(8, $margin, '<th class="featName">User ID</th>');

    aOutfl(8, "<td class=\"featValue\">0x%0${len}X - 0x%0${len}X   (%u bytes)</td>",
               $i->{START}, $i->{END}, $i->{END} - $i->{START} + 1);

    aOutl(6, '</tr>');
    }

        #------------------------------------

  print_devid(6, $features->{DEVID}, $margin, $len) if ($word_size != 16);

        #------------------------------------

  $i = $features->{CONFIGS};
  $t = ($word_size == 16) ? 'Byte' : 'Word';
  $t .= 's' if ($i > 1);
  aOutl(6, '<tr class="featLine">');
  aOutml(8, $margin, "<th class=\"featName\">Address space of Configuration $t</th>");
  aOutfl(8, "<td class=\"featValue\">0x%0${len}X - 0x%0${len}X   ($i %s)</td>",
             $features->{CF_START}, $features->{CF_END}, lc($t));
  aOutl(6, '</tr>');

        #------------------------------------

  print_devid(6, $features->{DEVID}, $margin, $len) if ($word_size == 16);

        #------------------------------------

  $i = $features->{EEPROM};
  if ($i > 0)
    {
    aOutl(6, '<tr class="featLine">');
    aOutml(8, $margin, '<th class="featName">Address space of EEPROM</th>');

    aOutfl(8, "<td class=\"featValue\">0x%0${len}X - 0x%0${len}X   (%u bytes)</td>",
               $features->{EE_START}, $features->{EE_START} + $i, $i + 1);

    aOutl(6, '</tr>');
    }

        #------------------------------------

  aOutl(6, '<tr class="featLine">');
  aOutml(8, $margin,
            '<th class="featName">Number of RAM Banks</th>',
            "<td class=\"featValue\">$features->{BANKS} ($features->{BANK_SIZE} bytes/banks)</td>");
  aOutl(6, '</tr>');

        #------------------------------------

  $i = scalar(keys %{$features->{SFR_NAMES}});
  if ($i > 0)
    {
    aOutl(6, '<tr class="featLine">');
    aOutml(8, $margin,
              '<th class="featName">Number of SFRs</th>',
              "<td class=\"featValue\">$i</td>");
    aOutl(6, '</tr>');

    if ($features->{RAM_SIZE} > 0)
      {
      $i = $features->{RAM_SIZE} - $i;
      aOutl(6, '<tr class="featLine">');
      aOutml(8, $margin,
                '<th class="featName">Size of GPRs</th>',
                "<td class=\"featValue\">$i bytes</td>");
      aOutl(6, '</tr>');
      }
    }

        #------------------------------------

  $i = $features->{RAM_SIZE};
  if ($i > 0)
    {
    aOutl(6, '<tr class="featLine">');
    aOutml(8, $margin,
              '<th class="featName">Full size of SFRs + GPRs</th>',
              "<td class=\"featValue\">$i bytes</td>");
    aOutl(6, '</tr>');
    }

        #------------------------------------

  if ($word_size == 16)
    {
    aOutl(6, '<tr class="featLine">');
    aOutml(8, $margin, '<th class="featName">Last address of lower Access RAM</th>');
    aOutfl(8, "<td class=\"featValue\">0x%02X</td>", $features->{ACCESS});
    aOutl(6, '</tr>');
    }

        #------------------------------------

  aOutl(4, '</table>');
  src_info(4);
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # Dump the entire contents of a Config word.

sub dump_config_word($$$$$)
  {
  my ($Align, $Config, $Length, $Mask, $Gap) = @_;
  my $margin = '<td class="vMargin"></td>';

  foreach (@{$Config})
    {
    my $head = $_->{HEAD};
    my $name = $_->{NAME};
    my $mask = ($_->{MASK} ^ $Mask) & $Mask;

    if ($name ne '')
      {
        # Exist a long name.
      $name = "$head -- $name";
      }
    else
      {
      $name = $head;
      }

    aOutml($Align, $Gap, "<tr><th colspan=3 class=\"confOptName\">$name</th></tr>");

    foreach (@{$_->{BITS}})
      {
      my $str  = "$head = $_->{NAME}";
      my $expl = $_->{EXPL};

        # Improve a spelling error: On the end of a sentence a point must be.
      $expl .= '.' if ($expl ne '' && $expl !~ /\.$/o);

      aOutl($Align, '<tr>');
      aOutml($Align + 2, $margin, "<td class=\"confSwName\">$str</td>");
      aOutfl($Align + 2, "<td class=\"confSwValue\">0x%0${Length}X</td>", $_->{VALUE} | $mask);
      aOutml($Align + 2, "<td class=\"confSwExpl\">$expl</td>", $margin);
      aOutl($Align, '</tr>');
      }
    }
  }

#-------------------------------------------------------------------------------

        # Dump the entire contents of all Config word of $Name MCU.

sub dump_all_config_word($$)
  {
  my ($Name, $Configs) = @_;
  my $conf_bits = $Configs->{CONFIGS};
  my @addresses = sort {$a <=> $b} keys(%{$conf_bits});
  my @sections;
  my $config_mask = (ULONG_MAX << $Configs->{FEATURES}->{CONF_SIZE}) ^ ULONG_MAX;
  my $count = @addresses;
  my ($str, $len, $i, $head_s, $head_e, $gap);

  return if (! $count);

  $str = "$out_dir/${Name}-$conf_tag.html";
  open($out_handler, '>', $str) || die "Could not create the \"$str\" file!\n";

  Log("Dump the Config Options of $Name.", 4);

  $len = 4;
  $head_s = '<tr><th colspan=4 class="configWord">';
  $head_e = '</th></tr>';
  $gap    = '<tr class="confGap"><td></td></tr>';

        #------------------------------------

  print_html_head($Name);
  aOutl(2, '<body>');

        # The main menu.

  aOutl(4, '<ul class="classMenu">');

  foreach (@pri_menu_elems)
    {
    aOutl(6, "<li><a href=\"$remote_url$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(4, '</ul>');

  print_mcu_menu($Name, MCU_MENU_CONF);

        #------------------------------------

        # The table of Config Bits.

  aOutml(4, '<table class="configList">',
            "  <tr><th colspan=5 class=\"confTableName\">$Name</th></tr>");

  if ($count < 2)
    {
        # PIC10F, PIC12, PIC16

    $sections[0] = 'CONFIG';
    aOutl(6, $gap);
    aOutfl(6, "${head_s}CONFIG (0x%0${len}X)$head_e", $addresses[0]);
    dump_config_word(6, \@{$conf_bits->{$addresses[0]}}, $len, $config_mask, $gap);
    }
  else
    {
    if ($config_mask == 0x00FF)
      {
        # PIC18
      my ($v, $n, $h);

      $len = 2;

      if ($addresses[0] < 0x300000)
        {
        # PIC18FxxJ

        for ($i = 0; $i < $count; ++$i)
          {
          $v = $addresses[$i];
          $n = int(($i & 0x0F) / 2 + 1);
          $h = ($i & 1) ? 'H' : 'L';
          $str = "CONFIG$n$h";
          $sections[$i] = $str;
          aOutl(6, $gap);
          aOutfl(6, "$head_s$str (0x%06X)$head_e", $v);
          dump_config_word(6, \@{$conf_bits->{$addresses[$i]}}, $len, $config_mask, $gap);
          }
        }
      else
        {
        # PIC18Fxx

        for ($i = 0; $i < $count; ++$i)
          {
          $v = $addresses[$i];
          $n = int(($v & 0x0F) / 2 + 1);
          $h = ($v & 1) ? 'H' : 'L';
          $str = "CONFIG$n$h";
          $sections[$i] = $str;
          aOutl(6, $gap);
          aOutfl(6, "$head_s$str (0x%06X)$head_e", $v);
          dump_config_word(6, \@{$conf_bits->{$addresses[$i]}}, $len, $config_mask, $gap);
          }
        }
      }
    else
      {
        # PIC12, PIC16

      for ($i = 0; $i < $count; ++$i)
        {
        $str = sprintf "CONFIG%u", $i + 1;
        $sections[$i] = $str;
        aOutl(6, $gap);
        aOutfl(6, "$head_s$str (0x%04X)$head_e", $addresses[$i]);
        dump_config_word(6, \@{$conf_bits->{$addresses[$i]}}, $len, $config_mask, $gap);
        }
      }
    }

  aOutl(4, '</table>');
  src_info(4);
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # Marks in $Array the non GPR regions of RAM.

sub mark_non_gpr_ram($$)
  {
  my ($Array, $Features) = @_;
  my $bank_num  = $Features->{BANKS};
  my $bank_size = $Features->{BANK_SIZE};
  my $ram_size  = $Features->{MAX_RAM} + 1;
  my $bad_ram   = $Features->{BAD_RAM};
  my $core_sfrs = $Features->{CORE_SFRS};
  my $sfrs      = $Features->{SFRS};
  my ($bank, $bank_prev, $i, $k, $max_sfr, $sfr_count, $x);

        # Prepares the RAM map.

  @{$Array} = ((RAM_GPR) x $ram_size);
  $k = $bank_num * $bank_size;
        # Another deletion if the real RAM less than the possible max RAM.
  push(@{$Array}, (RAM_BAD) x ($k - $ram_size)) if ($ram_size < $k);

        # Marks the areas that do not exist.

  foreach (@{$bad_ram})
    {
    my $start = $_->{START};
    my $size  = $_->{END} - $start + 1;

    splice(@{$Array}, $start, $size, ((RAM_BAD) x $size));
    }

  if ($Features->{WORD_SIZE} < 16)
    {
        # Not exist GPR before the last SFR in bank.

    $bank_prev = -1;
    $max_sfr = 0;
    $sfr_count = @{$sfrs};
    $i = 0;
    while (TRUE)
      {
      my $addr = $sfrs->[$i]->{ADDR};
      my $offs = $addr % $bank_size;

      $bank = int($addr / $bank_size);

      if ($bank_prev == $bank)
        {
        $max_sfr = $offs if ($max_sfr < $offs);
        }
      else
        {
        if ($bank_prev >= 0)
          {
        # In the previous bank there is no GPR before the last SFR.

          $bank_prev *= $bank_size;
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

        $bank *= $bank_size;
        ++$max_sfr;
        splice(@{$Array}, $bank, $max_sfr, ((RAM_BAD) x $max_sfr));
        last;
        }
      }

        # None of in bank does not exists GPR before the last mirror SFR.

    if (defined($core_sfrs))
      {
      $k = $core_sfrs->[$#{$core_sfrs}] + 1;
      $bank = 0;
      for ($x = 0; $x < $bank_num; ++$x)
        {
        splice(@{$Array}, $bank, $k, ((RAM_BAD) x $k));
        $bank += $bank_size;
        }
      }
    }
  }

#-------------------------------------------------------------------------------

        # Marks in $Array the SFR regions of RAM.

sub mark_sfr_ram($$)
  {
  my ($Array, $Features) = @_;
  my $bank_num  = $Features->{BANKS};
  my $bank_size = $Features->{BANK_SIZE};
  my $core_sfrs = $Features->{CORE_SFRS};
  my $sfr_addrs = $Features->{SFR_ADDRS};
  my ($bank, $x);

        # Places the core registers. These at the same address there is in the all banks.
        # (12 and 14 bit MCU.)

  if (defined($core_sfrs))
    {
    foreach (@{$core_sfrs})
      {
      $bank = 0;
      for ($x = 0; $x < $bank_num; ++$x)
        {
        $Array->[$bank + $_] = RAM_SFR;
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

#-------------------------------------------------------------------------------

        # Dump the RAM map of $Name MCU.

sub dump_ram_map($$)
  {
  my ($Name, $Configs) = @_;
  my $features  = $Configs->{FEATURES};
  my $bank_num  = $features->{BANKS};
  my $bank_size = $features->{BANK_SIZE};
  my $sfrs      = $features->{SFRS};
  my $sfr_names = $features->{SFR_NAMES};
  my @ram_array;
  my @map_array;
  my @bank_sum = ();
  my $margin = '<td class="vMargin"></td>';
  my ($map, $bank, $height, $k, $r, $t, $x, $y);
  my $mcu16_bit = ($features->{WORD_SIZE} == 16) ? TRUE : FALSE;

  $t = "$out_dir/${Name}-$ram_tag.html";
  open($out_handler, '>', $t) || die "Could not create the \"$t\" file!\n";

  Log("Dump the RAM map of $Name.", 4);

  mark_non_gpr_ram(\@ram_array, $features);
  mark_sfr_ram(\@ram_array, $features);

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
      }

    $bank += $bank_size;
    }
        #------------------------------------

        # After a lot of work has finally can be written the html code.

  print_html_head($Name);
  aOutl(2, '<body>');

        # The main menu.

  aOutl(4, '<ul class="classMenu">');

  foreach (@pri_menu_elems)
    {
    aOutl(6, "<li><a href=\"$remote_url$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(4, '</ul>');

  print_mcu_menu($Name, MCU_MENU_RAM);

        #------------------------------------

        # The table of RAM map of MCU.

  aOutl(4, '<table class="ramMap">');
  aOutfl(6, "<tr><th colspan=%u class=\"ramTableName\">$Name</th></tr>", $bank_num * 2 + 1);
  aOutl(6, '<tr class="ramGap"><td></td></tr>');

        #------------------------------------

        # header

  aOutl(6, '<tr class="ramHead">');
  aOutl(8, $margin);

  for ($x = 0; $x < $bank_num; ++$x)
    {
    aOutml(8, "<th class=\"ramBank\">Bank&nbsp;$x</th>", $margin);
    }

  aOutml(6, '</tr>', '<tr class="ramGap"><td></td></tr>');

        #------------------------------------

  aOutl(6, '<tr>');
  aOutl(8, $margin);

  $k = int(256 / $bank_size) * 3;
  for ($x = 0; $x < $bank_num; ++$x)
    {
    my ($sfr_size, $gpr_size, $bad_size) = (0, 0, 0);

    $map = $map_array[$x];

    aOutml(8, '<td class="ramColumn">', '  <div class="ramColCont">');

    if ($mcu16_bit)
      {
      $height = ($features->{ACCESS} + 1) * $k;

      if ($x == 0)
        {
        aOutfl(12, '<div class="ramAcc" style="top: 0; height: %upx;"></div>',
                   $height);
        }
      elsif (($x + 1) == $bank_num)
        {
        aOutfl(12, '<div class="ramAcc" style="top: %upx; height: %upx;"></div>',
                   $height, ($bank_size * $k) - $height);
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

        default
          {
          $t = 'ramBAD';
          $r = 'Unimplemented';
          $bad_size += $size;
          }
        }

      aOutf(12, "<div class=\"$t\" style=\"height: %upx\"><div class=\"ramTt\" style=\"top: %upx\">",
                $height, $tt_top);

      if ($_->{TYPE} == RAM_SFR)
        {
        my $name = $sfr_names->{$addr};

        if (! $mcu16_bit)
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

        Out("<a href=\"$remote_url${Name}-$sfr_tag.html#$name\">");
        $l_e = '</a>';
        }

      if ($size > 1)
        {
        Outfl("$r<br>0x%03X - 0x%03X<br>$size bytes$l_e</div></div>", $addr, $addr + $size - 1);
        }
      else
        {
        Outfl("$r<br>0x%03X$l_e</div></div>", $addr);
        }
      } # foreach (@{$map})

    $bank_sum[$x] = { SFR => $sfr_size, GPR => $gpr_size, BAD => $bad_size };

    aOutml(8, '  </div>', '</td>', $margin);
    }

  aOutml(4, '  </tr>', '</table>');

        #------------------------------------

        # Summary of banks.

  aOutml(4, '<table class="ramMap">', '  <tr>');
  aOutl(8, $margin);

  for ($x = 0; $x < $bank_num; ++$x)
    {
    my $sum = $bank_sum[$x];

    aOutl(8, '<td class="ramColumn">');
    aOutl(10, '<div class="ramColCont">');
    aOutml(12, "<div class=\"ramSFR ramSum\">SFR<br>$sum->{SFR} bytes</div>",
               "<div class=\"ramGPR ramSum\">GPR<br>$sum->{GPR} bytes</div>",
               "<div class=\"ramBAD ramSum\">Unimplemented<br>$sum->{BAD} bytes</div>");
    aOutl(10, '</div>');
    aOutml(8, '</td>', $margin);
    }

  aOutl(6, '</tr>');

  if ($bank_num > 1 && ! $mcu16_bit)
    {
    aOutl(6, '<tr class="ramGap"><td></td></tr>');
    aOutfl(6, "<tr><td colspan=%u class=\"ramSumEx\">It is possible that the tables is inaccurate due to the mirror-SFRs.</td></tr>", $bank_num * 2 + 1);
    }

  aOutl(4, '</table>');

        #------------------------------------

  aOutml(4, '<div class="legendContainer">',
            '  <div class="legend">');
  aOutml(8, '<p class="ramSFREx">&nbsp;<span class="explanation">Special Function Register.</span></p>',
            '<p class="ramGPREx">&nbsp;<span class="explanation">General-purpose RAM.</span></p>',
            '<p class="ramBADEx">&nbsp;<span class="explanation">In this place no RAM.</span></p>');
  aOutl(6, '</div>');

  if ($mcu16_bit)
    {
    aOutl(6, '<div class="legend">');
    aOutml(8, '<p class="ramSFREx"><span class="ramAccEx"></span>&nbsp;<span class="explanation">Special Function Register on Access Area.</span></p>',
              '<p class="ramGPREx"><span class="ramAccEx"></span>&nbsp;<span class="explanation">General-purpose RAM on Access Area.</span></p>',
              '<p class="ramBADEx"><span class="ramAccEx"></span>&nbsp;<span class="explanation">In this place no RAM on Access Area.</span></p>');
    aOutl(6, '</div>');
    }

  aOutl(4, '</div>');

  src_info(4);
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # Dump the SFR map of $Name MCU.

sub dump_sfr_map($$)
  {
  my ($Name, $Configs) = @_;
  my $features  = $Configs->{FEATURES};
  my $bank_num  = $features->{BANKS};
  my $bank_size = $features->{BANK_SIZE};
  my $sfrs      = $features->{SFRS};
  my @bank_array = ();
  my $margin = '<td class="vMargin"></td>';
  my ($bank, $i, $max_x, $x, $min_y, $max_y, $y, $t);
  my $mcu16_bit = ($features->{WORD_SIZE} == 16) ? TRUE : FALSE;
  my $accessSfr = 0xF00 + $features->{ACCESS} + 1;

  $t = "$out_dir/${Name}-$sfr_tag.html";
  open($out_handler, '>', $t) || die "Could not create the \"$t\" file!\n";

  Log("Dump the SFR map of $Name.", 4);

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

  print_html_head($Name);
  aOutl(2, '<body>');

        # The main menu.

  aOutl(4, '<ul class="classMenu">');

  foreach (@pri_menu_elems)
    {
    aOutl(6, "<li><a href=\"$remote_url$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(4, '</ul>');

  print_mcu_menu($Name, MCU_MENU_SFR);

        #------------------------------------

        # The table of SFR map of MCU.

  aOutl(4, '<table class="sfrMap">');
  aOutfl(6, "<tr><th colspan=%u class=\"sfrTableName\">$Name</th></tr>", $max_x * 2 + 1);
  aOutl(6, '<tr class="sfrGap"><td></td></tr>');

        #------------------------------------

        # header

  aOutl(6, '<tr class="sfrHead">');
  aOutl(8, $margin);

  for ($x = 0; $x < $max_x; ++$x)
    {
    aOutml(8, "<th class=\"sfrBank\">Bank&nbsp;$bank_array[$x]->{BANKNUM}</th>", $margin);
    }

  aOutml(6, '</tr>', '<tr class="sfrGap"><td></td></tr>');

  aOutl(6, '<tr>');
  aOutl(8, $margin);

  for ($x = 0; $x < $max_x; ++$x)
    {
    my $bn = $bank_array[$x]->{BANK};
    my $last_bank = ($mcu16_bit && ($x + 1) == $max_x) ? TRUE : FALSE;

    aOutl(8, '<td class="sfrColumn">');
    aOutl(10, '<table>');

    $max_y = $bank_array[$x]->{MAX_OFFS};
    for ($y = $min_y; $y <= $max_y; ++$y)
      {
      $t = $bn->[$y];

      if (defined($t))
        {
        my $v = @{$t};
        my $name = $t->[0]{NAME};
        my $addr = $t->[0]{ADDR};
        my $second_class = '';

        $second_class = ' sfrAccess' if ($last_bank && $addr >= $accessSfr);

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
        aOutl(12, '</tr>');
        } # if (defined($t))
      else
        {
        aOutl(12, '<tr><th class="sfrSep">&nbsp;</th></tr>');
        }
      }

    aOutml(8, '  </table>', '</td>', $margin);
    }

  aOutl(6, '</tr>');

        #------------------------------------

  if ($bank_num > 1 && ! $mcu16_bit)
    {
    aOutl(6, '<tr class="ramGap"><td></td></tr>');
    aOutfl(6, "<tr><td colspan=%u class=\"ramSumEx\">The mirror of [PCL, STATUS, ...] are not shown.</td></tr>", $bank_num * 2 + 1);
    }

  aOutl(4, '</table>');

        #------------------------------------

  aOutml(4, '<div class="legendContainer">',
            '  <div class="legend">');
  aOutml(8, '<p class="sfrNameEx">&nbsp;<span class="explanation">SFR</span></p>',
            '<p class="sfrNameXEx">&nbsp;<span class="explanation">SFR with alias name.</span></p>');
  aOutl(6, '</div>');

  if ($mcu16_bit)
    {
    aOutml(6, '<div class="legend">',
              '  <p class="sfrNameEx sfrAccess">&nbsp;<span class="explanation">SFR on Access Area.</span></p>',
              '</div>');
    }

  aOutl(4, '</div>');

  src_info(4);
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # Print the menu of class of MCUs.

sub print_pri_menu($)
  {
  my $Menu = $_[0];

  print $out_handler <<EOT
      <ul class="tabs">
EOT
;
  foreach (@pri_menu_elems)
    {
    my $class = ($Menu == $_) ? ' class="selected"' : '';

    aOutl(8, "<li$class><a href=\"$remote_url$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(6, '</ul>');
  }

#-------------------------------------------------------------------------------

        # Print the class html files of MCUs.

sub create_class_htmls()
  {
  foreach (@pri_menu_elems)
    {
    my $html = "$out_dir/$_->{HREF}";

    open($out_handler, '>', $html) || die "Could not create the \"$html\" file!\n";

    print_html_head($_->{NAME});
    aOutml(2, '<body>',
              '  <div class="headContainer">');
    aOutml(6, '<div class="headSide">&nbsp;</div>',
              '<div class="heading">Informations about the PIC microcontrollers</div>',
              '<div class="headSide">&nbsp;</div>');
    print_pri_menu($_);

        # print_mcu_list()
    my $func = $_->{PFUNC};

    $func->(4, $_->{CLASS}) if (defined($func));

    aOutml(2, '  </div>', "</body>\n</html>");

    close($out_handler);
    }
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                              @@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@  Preparation of the css file.  @@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                              @@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

my $page_width         = 1024;          # px
my $content_background = '#FFFFFF';
my $tab_color          = '#000000';
my $tab_background     = '#E0E0F0';
my $tab_border_color   = '#C0C0C0';
my $tooltip_background = '#F0FFA1';
my $border_width       = 1;             # px
my $attr_background    = '#D7DEB2';
my $header_background  = '#CAB2DE';
my $ramSFR_color       = '#F48282';
my $ramColumn_width    = 120;           # px
my $stripe_color       = '#303030';
my $expl_font_size     = 0.75;          # em
my $expl_y_padding     = 0.2;           # em
my $ramAccEx_height    = $expl_font_size + 2 * $expl_y_padding;
my $Ex_color_width     = 50;            # px

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

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
  padding: 1em 1em 0;
  overflow: hidden;
  margin-bottom: -${border_width}px;
  }

.tabs li, .classMenu li
  {
  z-index: 1;
  position: relative;
  padding: 0.5em 0.5em;
  display: table-cell;
  list-style: none;
  background: $tab_background;
  color: $tab_color;
  border: ${border_width}px solid $tab_border_color;
EOT
;
  css_shadow(2, "0 0 1em $tab_border_color");
  print $out_handler <<EOT
  }

.tabs li
  {
EOT
;
  css_border_radius(2, '0.5em 0.5em 0 0');
  print $out_handler <<EOT
  border-bottom: 0;
  }

.tabs a
  {
  white-space: nowrap;
  text-decoration: none;
  }

.tabs li:hover, .classMenu li:hover
  {
EOT
;
  css_gradient(2, "$content_background, $tab_background 100%");
  print $out_handler <<EOT
  background: -webkit-gradient(linear, 0 0, 0 100%, from($content_background), to($tab_background));
  filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='$content_background', endColorstr='$tab_background');
  }

.tabs li.selected
  {
  z-index: 3;
  background: $content_background;
  }

.classMenu li:active
  {
EOT
;
  css_shadow(2, "0 1px 2px $tab_border_color inset");
  print $out_handler <<EOT
  }

.heading,
.mcuListHeader, .mcuList, .classMenu li, .mcuLink,
.featList, .featTableName, .featName,
.configList, .confTableName, .configWord, .confOptName,
.ramMap, .ramTableName, .ramBank,
.sfrMap, .sfrTableName, .sfrBank, .sfrName, .sfrNameX,
.srcInfo
  {
EOT
;
  css_border_radius(2, '0.5em');
  print $out_handler <<EOT
  }

.mcuList, .featList, .configList, .ramMap, .sfrMap
  {
  z-index: 2;
  position: relative;
  width: auto;
  padding: 1em 1em;
  background: $content_background;
  border: ${border_width}px solid $tab_border_color;
EOT
;
  css_shadow(2, "0 0 1em $tab_border_color");
  print $out_handler <<EOT
  }

.mcuList
  {
  display: inline-block;
  min-width: 95%;
  }

.mcuTable
  {
  display: inline;
  table-layout: fixed;
  }

/*----------------------------------------------*/

.classMenu
  {
  white-space: nowrap;
  height: 2em;
  margin-bottom: 1.5em;
  }

.classMenu li
  {
EOT
;
  css_shadow(2, "0 0 1em $tab_border_color");
  print $out_handler <<EOT
  }

.classMenu a
  {
  text-decoration: none;
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
    Out(".$mcu_features{$mcu_feat_names[$i]}->{CSS_CLASS}");
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
    Outl(".$mcu_features{$_}->{CSS_CLASS}");
    aOutml(2, "{",
              "background: $mcu_features{$_}->{CSS_BGRND};",
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
  width: 12em;
  padding: 0.2em;
  text-align: left;
  background: $attr_background;
  border: 2px outset $attr_background;
  }

.mcuLink:active
  {
  border: 2px inset;
EOT
;
  css_shadow(2, "0 0 10px 6px #B7BD97 inset");
  print $out_handler <<EOT
  }

.mcuLink a
  {
  display: block;
  padding: 0 0.5em 0 0.5em;
  text-decoration: none;
EOT
;
  css_border_radius(2, '0.3em');
  css_shadow(2, "0 0 10px 6px $tooltip_background inset");
  print $out_handler <<EOT
  }

.mcuLink a:hover
  {
  background: #49DDFF;
  }

.featValue,
.confSwName, .confSwValue, .confSwExpl,
.sfrAddr
  {
  background: #BAE7B8;
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
  background: $attr_background;
  border-color: $attr_background;
  border-style: ridge;
  }

.featName, .featValue,
.confOptName, .confSwName, .confSwValue, .confSwExpl,
.sfrName, .sfrNameX, .sfrAddr
  {
  text-align: left;
  }

.vMargin
  {
  width: auto;
  background: transparent;
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

.ramColCont a
  {
  display: block;
  background: $tooltip_background;
  text-decoration: none;
EOT
;
  css_border_radius(2, '0.75em 0.75em 0.75em 1.5em');
  css_shadow(2, "0 0 10px 6px $tooltip_background inset");
  print $out_handler <<EOT
  }

.ramColCont a:link
  {
  color: #874EAD;
  }

.ramColCont a:hover
  {
  background: #49DDFF;
  }

/* This a tooltip. */

.ramTt
  {
  display: none;
  font-size: 0.7em;
  text-align: center;
  position: relative;
  left: 20px;
  background: $tooltip_background;
  padding: 0.2em 0.8em;
EOT
;
  css_border_radius(2, '0 1em 1em 2em');
  css_shadow(2, "0 0 1.5em $tab_border_color");
  print $out_handler <<EOT
  }

.ramGPR:hover .ramTt, .ramSFR:hover .ramTt, .ramBAD:hover .ramTt
  {
  z-index: 100;
  display: block;
  }

/* This accessories of the tooltip. This is a beak which point towards the left side. */

.ramGPR:hover .ramTt:before, .ramSFR:hover .ramTt:before, .ramBAD:hover .ramTt:before
  {
  display: block;
  position: absolute;
  content: "";
  width: 0;
  height: 0;
  top: 0;
  left: -15px;
  border-right: 15px solid $tooltip_background;
  border-bottom: 5px solid transparent;
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

.ramBAD, .ramBADEx
  {
  background: $tab_border_color;
  }

.ramGPR, .ramSFR, .ramBAD
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
  min-width: 55%;
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
  background: $tooltip_background;
  padding: 0.2em 0.4em;
EOT
;
  css_border_radius(2, '0 0.75em 0.75em 0.75em');
  css_shadow(2, "0 0 1.5em $tab_border_color");
  print $out_handler <<EOT
  }

.sfrNameX:hover span
  {
  z-index: 100;                 /* This will surely be enough. */
  display: block;
  cursor: text;
  }

.ramSFREx, .ramGPREx, .ramBADEx,
.sfrNameEx, .sfrNameXEx, .menuEx
  {
  padding: 0;
  width: ${Ex_color_width}px;
  margin-left: 20px;
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
EOT
;
  css_border_radius(2, '0');
  print $out_handler <<EOT
  }

.legendContainer
  {
  position: relative;
  width: ${page_width}px;
  margin: 0 auto;
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
  }
EOT
;
  close($out_handler);
  }

#-------------------------------------------------------------------------------

sub usage()
  {
  print <<EOT
Usage: $PROGRAM [options]

    Options are:

        -gp <path> or --gputils-local-repo <path>

            The program on this path looks for the gputils local repository.

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

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@  The main program.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

$PROGRAM = basename($0);
$remote_url = '';
$list_file  = '';

for (my $i = 0; $i < scalar(@ARGV); )
  {
  my $opt = $ARGV[$i++];

  given ($opt)
    {
    when (/^-(gp|-gputils-local-repo)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $gputils_local_repo = $ARGV[$i++];
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

die "This directory - $gputils_local_repo - not exist!" if (! -d $gputils_local_repo);

read_gp_svn_revision();

$gputils_path = "$gputils_local_repo/gputils";
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
read_all_config_bits();
create_css();
create_class_htmls();

foreach (sort { smartSort($a, $b) } keys %mcus_by_names)
  {
  dump_all_config_word($_, $mcus_by_names{$_});
  dump_features($_, $mcus_by_names{$_});
  dump_ram_map($_, $mcus_by_names{$_});
  dump_sfr_map($_, $mcus_by_names{$_});
  }

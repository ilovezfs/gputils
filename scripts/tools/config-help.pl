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

    This program prepares the html lists from Config words of PIC MCU-s.
    The data reads from the 8bit_device.info called file of MPLAB-X.
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

my $gputils_local_repo  = "$ENV{HOME}/svn_snapshots/gputils";
my $gputils_path        = "$gputils_local_repo/gputils";
my $gpprocessor_c = 'gpprocessor.c';
my $gpproc_path;
my $svn_rev = -1;

my $dev_info = '/opt/microchip/mplabx/mpasmx/8bit_device.info';
my $dev_info_rev = '';
my $list_file;

my $pic_name_mask = qr/PIC1(2(C[ER]?|HV)\d+\w+|6(C[ER]?|HV)\d+\w+|[0268](C|L?F)\d+\w+)|PIC(RF\d+\w+|MCV\d+\w+)|PS\d+/;
my $header = ('=' x 70);

my $XHTML_1_Frameset = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Frameset//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd">';

my $out_dir   = 'html-help';
my $css       = 'main.css';
my $conf_tag  = 'conf';
my $trait_tag = 'trait';

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

my %mcu_traits =
  (
  '16c5x'  => {
              CLASS     => PROC_CLASS_PIC12,
              ENHANCED  => FALSE,
              PAGE_SIZE => 512,
              WORD_SIZE => 12,
              CONF_SIZE => 12,
              EE_START  => 0,
              BANK_SIZE => 32
              },

  '16c5xe' => {
              CLASS     => PROC_CLASS_PIC12E,
              ENHANCED  => TRUE,
              PAGE_SIZE => 512,
              WORD_SIZE => 12,
              CONF_SIZE => 12,
              EE_START  => 0,
              BANK_SIZE => 32
              },

  '16xxxx' => {
              CLASS     => PROC_CLASS_PIC14,
              ENHANCED  => FALSE,
              PAGE_SIZE => 2048,
              WORD_SIZE => 14,
              CONF_SIZE => 14,
              EE_START  => 0x2100,
              BANK_SIZE => 128
              },

  '16exxx' => {
              CLASS     => PROC_CLASS_PIC14E,
              ENHANCED  => TRUE,
              PAGE_SIZE => 2048,
              WORD_SIZE => 14,
              CONF_SIZE => 16,
              EE_START  => 0xF000,
              BANK_SIZE => 128
              },

  '17xxxx' => {
              CLASS     => PROC_CLASS_PIC16,
              ENHANCED  => FALSE,
              PAGE_SIZE => 0,
              WORD_SIZE => 16,
              CONF_SIZE => 8,
              EE_START  => 0,
              BANK_SIZE => 256
              },

  '18xxxx' => {
              CLASS     => PROC_CLASS_PIC16E,
              ENHANCED  => TRUE,
              PAGE_SIZE => 0,
              WORD_SIZE => 16,
              CONF_SIZE => 8,
              EE_START  => 0xF00000,
              BANK_SIZE => 256
              }
  );

#-----------------------------------------------

=back
        The structure of one element of the %mcus_by_names hash:

        {
        TRAITS   => {
                    CLASS     => PROC_CLASS_PIC1XX,
                    ENHANCED  => FALSE,
                    WORD_SIZE => 0,     # Instruction size of MCU.
                    CONF_SIZE => 0      # Width of a config word.
                    EE_START  => 0      # Start address of EEPROM.
                    BANK_SIZE => 0      # Size of RAM Banks.

                    COFF      => 0,     # Coff ID of device. (16 bit wide)
                    PAGES     => 0,     # Number of ROM/FLASH pages.
                    CF_START  => 0,     # Address of first Configuration byte/word.
                    CF_END    => 0,     # Address of last Configuration byte/word.

                # These addresses relative, compared to the beginning of the blocks.
                    ROM       => 0,     # Last address of ROM/FLASH.
                    FLASHDATA => 0,     # Last address of FLASH Data.
                    EEPROM    => 0,     # Last address of EEPROM.

                    CONFIGS   => 0,     # Number of Configuration bytes/words.
                    BANKS     => 0,     # Number of RAM Banks.
                    ACCESS    => 0      # Last address of lower Access RAM of pic18f series.
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
use constant PRI_MENU_REG    => 2;
use constant PRI_MENU_12_BIT => 3;
use constant PRI_MENU_14_BIT => 4;
use constant PRI_MENU_16_BIT => 5;

my @pri_menu_elems =
  (
    {                                   # PRI_MENU_ALL
    HREF  => 'index.html',
    NAME  => 'All MCUs',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_ALL
    },

    {                                   # PRI_MENU_ENH
    HREF  => 'enhanced-mcus.html',
    NAME  => 'Enhanced MCUs',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_ENH
    },

    {                                   # PRI_MENU_REG
    HREF  => 'regular-mcus.html',
    NAME  => 'Regular MCUs',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_REG
    },

    {                                   # PRI_MENU_12_BIT
    HREF  => '12-bits-mcus.html',
    NAME  => '12 bits MCUs',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_12_BIT
    },

    {                                   # PRI_MENU_14_BIT
    HREF  => '14-bits-mcus.html',
    NAME  => '14 bits MCUs',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_14_BIT
    },

    {                                   # PRI_MENU_16_BIT
    HREF  => '16-bits-mcus.html',
    NAME  => '16 bits MCUs',
    PFUNC => \&print_mcu_list,
    CLASS => PRI_MENU_16_BIT
    }
  );

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

sub smartSort
  {
  my @a_s = ($a =~ /(\d+|\D+)/go);
  my @b_s = ($b =~ /(\d+|\D+)/go);
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

    my $name = $_;

    $name = "pic$name" if ($name =~ /^rf/o);        # rfXXX -> picrfXXX

    $gp_mcus_by_names{uc($name)} = TRUE;
    }

  close(LIST);
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
  open(LIB, '<', $gpproc_path) || die "read_gpproc_c_content(): Can not open. -> \"$gpproc_path\"\n";

        # static struct px pics[] = {
        # { PROC_CLASS_EEPROM8, "__EEPROM8",  { "eeprom8",     "eeprom8",   "eeprom8"  }, 0x1fff, 0, 0, 0xff,     { -1, -1 },            { -1, -1 },             NULL                 },
        # { PROC_CLASS_PIC14E,  "__16LF1526", { "pic16lf1526", "p16lf1526", "16lf1526" }, 0xa526, 4, 0x1f, 0x1fff,{     -1,     -1 },    { 0x8007, 0x8008 },     "16lf1526_g.lkr"     },

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

        $gp_mcus_by_names{uc($name)} = TRUE;
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
#@@@@@@@@@@@@@                                                  @@@@@@@@@@@@@@@@
#@@@@@@@@@@@@  Read all config options from the $dev_info file.  @@@@@@@@@@@@@@@
#@@@@@@@@@@@@@                                                  @@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

sub read_all_config_bits()
  {
  my ($configs, $name, $class);
  my $traits = undef;
  my $addr = 0;
  my $config_count = 0;
  my $switch_count = 0;
  my $setting_count = 0;
  my $switch_info = undef;
  my $state = ST_WAIT;
  my ($cf_addr_min, $cf_addr_max);

  open(IN, '<', $dev_info) || die "Could not open for reading: $dev_info\n";

  my $msg = 'Gputils';

  $msg = 'list file' if ($list_file ne '');

  while (<IN>)
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

        my $tr = $mcu_traits{$class};

        die "Unknown class of $name MCU!" if (! defined($tr));

        $traits = {
                  CLASS     => $tr->{CLASS},
                  ENHANCED  => $tr->{ENHANCED},
                  PAGE_SIZE => $tr->{PAGE_SIZE}, # Size of program memory pages.
                  WORD_SIZE => $tr->{WORD_SIZE}, # Size of instructions.
                  CONF_SIZE => $tr->{CONF_SIZE}, # Size of Config Words.
                  EE_START  => $tr->{EE_START},  # Start address of EEPROM.
                  BANK_SIZE => $tr->{BANK_SIZE}, # Size of RAM Banks.

                  COFF      => hex($fields[1]),  # Coff ID of device. (16 bit wide)
                  PAGES     => hex($fields[5]),  # Number of ROM/FLASH pages.
                  CF_START  => 0,                # Address of first Configuration byte/word.
                  CF_END    => 0,                # Address of last Configuration byte/word.

                # These addresses relative, compared to the beginning of the blocks.
                  ROM       => hex($fields[6]),  # Last address of ROM/FLASH.
                  FLASHDATA => hex($fields[11]), # Last address of FLASH Data.
                  EEPROM    => hex($fields[9]),  # Last address of EEPROM.

                  CONFIGS   => hex($fields[12]), # Number of Configuration bytes/words.
                  BANKS     => hex($fields[7]),  # Number of RAM Banks.
                  ACCESS    => hex($fields[10])  # Last address of lower Access RAM of pic18f series.
                  };

        if (defined($gp_mcus_by_names{$name}))
          {
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

        # All information is together.
          if (! $setting_count && ! $switch_count && ! $config_count)
            {
            die "$name MCU already exist!" if (defined($mcus_by_names{$name}));

            $traits->{CF_START} = $cf_addr_min;
            $traits->{CF_END}   = $cf_addr_max;
            $mcus_by_names{$name}{TRAITS}  = $traits;
            $mcus_by_names{$name}{CONFIGS} = $configs;
            $configs = {};
            }
          } # when ('SETTING_VALUE_TYPE')
        } # given ($fields[0])
      } # if ($state == ST_LISTEN)
    } # while (<IN>)

  close(IN);
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@                            @@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@  Preparation of html files.  @@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@                            @@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

sub src_info($)
  {
  my $Align = $_[0];

  aOutl($Align, '<p id=confGap></p>');
  aOutl($Align, "<p id=srcInfo>This page generated automatically by the " .
                "$PROGRAM program from the MPASM " . basename($dev_info) .
                " file (rev: $dev_info_rev) " .
                "and from the gputils source package (rev: svn $svn_rev).</p>");
  }

#-------------------------------------------------------------------------------

        # Prepares multiple list of MCUs.

sub print_mcu_list($$)
  {
  my ($Align, $Class) = @_;
  my $lst = '<th id=mcuListHeader>';

  aOutl($Align + 2, '<div id=mcuList>');
  aOutl($Align + 4, '<table>');
  aOutl($Align + 6, '<tr>');
  aOutl($Align + 8, "${lst}name</th>");

  given ($Class)
    {
    when (PRI_MENU_ALL)
      {
      aOutl($Align + 8, "${lst}instruction size (bit)</th>");
      aOutl($Align + 8, "${lst}config word size (bit)</th>");
      aOutl($Align + 8, "${lst}class</th>");
      }

    when ([ PRI_MENU_ENH, PRI_MENU_REG ])
      {
      aOutl($Align + 8, "${lst}instruction size (bit)</th>");
      aOutl($Align + 8, "${lst}config word size (bit)</th>");
      }

    default
      {
      aOutl($Align + 8, "${lst}config word size (bit)</th>");
      aOutl($Align + 8, "${lst}class</th>");
      }
    }

  aOutl($Align + 6, '</tr>');

  foreach (sort smartSort keys %mcus_by_names)
    {
    my $td_href  = "<td id=mcuLinkName><a id=mcuLink href=\"${_}-$conf_tag.html\">$_</a></td>";
    my $traits   = $mcus_by_names{$_}->{TRAITS};
    my $td_wsize = "<td id=mcuAttr>$traits->{WORD_SIZE}</td>";
    my $td_csize = "<td id=mcuAttr>$traits->{CONF_SIZE}</td>";
    my $td_class = ($traits->{ENHANCED}) ? '<td id=mcuAttrEnh>enhanced</td>' : '<td id=mcuAttrReg>regular</td>';

    given ($Class)
      {
      when (PRI_MENU_ALL)
        {
        aOutl($Align + 6, '<tr>');
        aOutl($Align + 8, $td_href);
        aOutl($Align + 8, $td_wsize);
        aOutl($Align + 8, $td_csize);
        aOutl($Align + 8, $td_class);
        aOutl($Align + 6, '</tr>');
        }

      when (PRI_MENU_ENH)
        {
        if ($traits->{ENHANCED})
          {
          aOutl($Align + 6, '<tr>');
          aOutl($Align + 8, $td_href);
          aOutl($Align + 8, $td_wsize);
          aOutl($Align + 8, $td_csize);
          aOutl($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_REG)
        {
        if (! $traits->{ENHANCED})
          {
          aOutl($Align + 6, '<tr>');
          aOutl($Align + 8, $td_href);
          aOutl($Align + 8, $td_wsize);
          aOutl($Align + 8, $td_csize);
          aOutl($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_12_BIT)
        {
        if ($traits->{WORD_SIZE} == 12)
          {
          aOutl($Align + 6, '<tr>');
          aOutl($Align + 8, $td_href);
          aOutl($Align + 8, $td_csize);
          aOutl($Align + 8, $td_class);
          aOutl($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_14_BIT)
        {
        if ($traits->{WORD_SIZE} == 14)
          {
          aOutl($Align + 6, '<tr>');
          aOutl($Align + 8, $td_href);
          aOutl($Align + 8, $td_csize);
          aOutl($Align + 8, $td_class);
          aOutl($Align + 6, '</tr>');
          }
        }

      when (PRI_MENU_16_BIT)
        {
        if ($traits->{WORD_SIZE} == 16)
          {
          aOutl($Align + 6, '<tr>');
          aOutl($Align + 8, $td_href);
          aOutl($Align + 8, $td_csize);
          aOutl($Align + 8, $td_class);
          aOutl($Align + 6, '</tr>');
          }
        }
      }
    }

  aOutl($Align + 4, '</table>');
  src_info($Align + 4);
  aOutl($Align + 2, '</div>');
  }

#-------------------------------------------------------------------------------

        # Dump the entire contents of a Config word.

sub dump_config_word($$$$$)
  {
  my ($Align, $Config, $Length, $Mask, $Gap) = @_;

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

    aOutl($Align, $Gap);
    aOutl($Align, "<tr><th colspan=3 id=confOptName>$name</th></tr>");

    foreach (@{$_->{BITS}})
      {
      my $str  = "$head = $_->{NAME}";
      my $expl = $_->{EXPL};

        # Improve a spelling error: On the end of a sentence a point must be.
      $expl .= '.' if ($expl ne '' && $expl !~ /\.$/o);

      aOutl ($Align, '<tr id=confLine>');
      aOutl ($Align + 2, '<td id=confVertMargin></td>');
      aOutl ($Align + 2, "<td id=confSwName>$str</td>");
      aOutfl($Align + 2, "<td id=confSwValue>0x%0${Length}X</td>", $_->{VALUE} | $mask);
      aOutl ($Align + 2, "<td id=confSwExpl>$expl</td>");
      aOutl ($Align + 2, '<td id=confVertMargin></td>');
      aOutl ($Align, '</tr>');
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
  my $config_mask = (ULONG_MAX << $Configs->{TRAITS}->{CONF_SIZE}) ^ ULONG_MAX;
  my $count = @addresses;
  my ($str, $len, $i, $head_s, $head_e, $gap);

  return if (! $count);

  $str = "$out_dir/${Name}-$conf_tag.html";
  open($out_handler, '>', $str) || die "Could not create the \"$str\" file!\n";

  Outml($XHTML_1_Frameset, '<html>');
  print_head($Name, undef);
  aOutl(2, '<body>');

  $len = 4;
  $head_s = '<tr><th colspan=4 id=configWord>';
  $head_e = '</th></tr>';
  $gap    = '<tr id=confGap></tr>';

        #------------------------------------

        # The main menu.

  aOutl(4, '<div>');
  aOutl(6, '<ul id=classMenu>');

  foreach (@pri_menu_elems)
    {
    aOutl(8, "<li><a href=\"$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(6, '</ul>');
  aOutl(4, '</div>');

        #------------------------------------

        # The tabs.

  aOutl(4, '<ul id=tabs>');
  aOutl(6, "<li id=selected><a href=\"${Name}-$conf_tag.html\">Configuration Bits</a></li>");
  aOutl(6, "<li><a href=\"${Name}-$trait_tag.html\">MCU traits</a></li>");
  aOutl(4, '</ul>');

        #------------------------------------

        # The table of Config Bits.

  aOutl(4, '<div id=configList>');
  aOutl(6, '<table>');
  aOutl(8, "<caption id=confTableName>$Name</caption>");

  if ($count < 2)
    {
        # PIC10F, PIC12, PIC16

    $sections[0] = 'CONFIG';
    aOutl(8, $gap);
    aOutfl(8, "${head_s}CONFIG (0x%0${len}X)$head_e", $addresses[0]);
    dump_config_word(8, \@{$conf_bits->{$addresses[0]}}, $len, $config_mask, $gap);
    }
  else
    {
    if ($config_mask == 0x00FF)
      {
        # PIC18
      my $v;
      my $n;
      my $h;

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
          aOutl(8, $gap);
          aOutfl(8, "$head_s$str (0x%06X)$head_e", $v);
          dump_config_word(8, \@{$conf_bits->{$addresses[$i]}}, $len, $config_mask, $gap);
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
          aOutl(8, $gap);
          aOutfl(8, "$head_s$str (0x%06X)$head_e", $v);
          dump_config_word(8, \@{$conf_bits->{$addresses[$i]}}, $len, $config_mask, $gap);
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
        aOutl(8, $gap);
        aOutfl(8, "$head_s$str (0x%04X)$head_e", $addresses[$i]);
        dump_config_word(8, \@{$conf_bits->{$addresses[$i]}}, $len, $config_mask, $gap);
        }
      }
    }

  aOutl(6, '</table>');
  src_info(6);
  aOutl(4, '</div>');
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # Dump the traits of $Name MCU.

sub dump_traits($$)
  {
  my ($Name, $Configs) = @_;
  my $traits = $Configs->{TRAITS};
  my ($str, $len, $rom_size, $word_size, $i, $t);

  $str = "$out_dir/${Name}-$trait_tag.html";
  open($out_handler, '>', $str) || die "Could not create the \"$str\" file!\n";

  Outml($XHTML_1_Frameset, '<html>');
  print_head($Name, undef);
  aOutl(2, '<body>');

        #------------------------------------

        # The main menu.

  aOutl(4, '<div>');
  aOutl(6, '<ul id=classMenu>');

  foreach (@pri_menu_elems)
    {
    aOutl(8, "<li><a href=\"$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(6, '</ul>');
  aOutl(4, '</div>');

        #------------------------------------

        # The tabs.

  aOutl(4, '<ul id=tabs>');
  aOutl(6, "<li><a href=\"${Name}-$conf_tag.html\">Configuration Bits</a></li>");
  aOutl(6, "<li id=selected><a href=\"${Name}-$trait_tag.html\">MCU traits</a></li>");
  aOutl(4, '</ul>');

        #------------------------------------

        # The table of traits of MCU.

  $word_size = $traits->{WORD_SIZE};
  $len = ($word_size == 16) ? 6 : 4;
  $rom_size = $traits->{ROM} + 1;

  aOutl(4, '<div id=traitList>');
  aOutl(6, '<table>');
  aOutl(8, "<caption id=traitTableName>$Name</caption>");

  aOutl(8, '<tr id=traitGap></tr>');

        #------------------------------------

  aOutl(8, '<tr id=traitLine>');
  aOutl(10, '<td id=traitVertMargin></td>');
  aOutl(10, '<th id=traitName>Coff ID of device</th>');
  aOutfl(10, '<td id=traitValue>0x%04X</td>', $traits->{COFF});
  aOutl(10, '<td id=traitVertMargin></td>');
  aOutl(8, '</tr>');

        #------------------------------------

  aOutl(8, '<tr id=traitLine>');
  aOutl(10, '<td id=traitVertMargin></td>');
  aOutl(10, '<th id=traitName>Number of ROM/FLASH pages</th>');
  aOutl(10, "<td id=traitValue>$traits->{PAGES} ($traits->{PAGE_SIZE} words/pages)</td>");
  aOutl(8, '</tr>');

        #------------------------------------

  aOutl(8, '<tr id=traitLine>');
  aOutl(10, '<td id=traitVertMargin></td>');
  aOutl(10, '<th id=traitName>Last address of ROM/FLASH</th>');

  $t = ($word_size == 16) ? 'bytes' : 'words';
  aOutfl(10, "<td id=traitValue>0x%0${len}X   ($rom_size $t)</td>", $traits->{ROM});
  aOutl(8, '</tr>');

        #------------------------------------

  if ($traits->{FLASHDATA} > 0)
    {
    aOutl(8, '<tr id=traitLine>');
    aOutl(10, '<td id=traitVertMargin></td>');
    aOutl(10, '<th id=traitName>Address space of FLASH Data</th>');

    aOutfl(10, "<td id=traitValue>0x%0${len}X - 0x%0${len}X   (%i words)</td>",
               $rom_size,
               $rom_size + $traits->{FLASHDATA},
               $traits->{FLASHDATA} + 1);

    aOutl(8, '</tr>');
    }

        #------------------------------------

  $i = $traits->{CONFIGS};
  $t = ($word_size == 16) ? 'Byte' : 'Word';
  $t .= 's' if ($i > 1);
  aOutl(8, '<tr id=traitLine>');
  aOutl(10, '<td id=traitVertMargin></td>');
  aOutl(10, "<th id=traitName>Address space of Configuration $t</th>");
  aOutfl(10, "<td id=traitValue>0x%0${len}X - 0x%0${len}X   ($i %s)</td>",
             $traits->{CF_START}, $traits->{CF_END}, lc($t));
  aOutl(8, '</tr>');

        #------------------------------------

  if ($traits->{EEPROM} > 0)
    {
    aOutl(8, '<tr id=traitLine>');
    aOutl(10, '<td id=traitVertMargin></td>');
    aOutl(10, '<th id=traitName>Address space of EEPROM</th>');

    aOutfl(10, "<td id=traitValue>0x%0${len}X - 0x%0${len}X   (%i bytes)</td>",
               $traits->{EE_START},
               $traits->{EE_START} + $traits->{EEPROM},
               $traits->{EEPROM} + 1);

    aOutl(8, '</tr>');
    }

        #------------------------------------

  aOutl(8, '<tr id=traitLine>');
  aOutl(10, '<td id=traitVertMargin></td>');
  aOutl(10, '<th id=traitName>Number of RAM Banks</th>');
  aOutl(10, "<td id=traitValue>$traits->{BANKS} ($traits->{BANK_SIZE} bytes/banks)</td>");
  aOutl(8, '</tr>');

        #------------------------------------

  if ($word_size == 16)
    {
    aOutl(8, '<tr id=traitLine>');
    aOutl(10, '<td id=traitVertMargin></td>');
    aOutl(10, '<th id=traitName>Last address of lower Access RAM</th>');
    aOutfl(10, "<td id=traitValue>0x%02X</td>", $traits->{ACCESS});
    aOutl(8, '</tr>');
    }

        #------------------------------------

  aOutl(6, '</table>');
  src_info(6);
  aOutl(4, '</div>');
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # Print the head of html file.

sub print_head($)
  {
  print $out_handler <<EOT
  <head>
    <title>$_[0]</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <link rel="stylesheet" type="text/css" href="$css">
  </head>
EOT
;
  }

#-------------------------------------------------------------------------------

        # Print the menu of class of MCUs.

sub print_menu($)
  {
  my $Menu = $_[0];

  print $out_handler <<EOT
    <div id=tabBox>
      <ul id=tabs>
EOT
;
  foreach (@pri_menu_elems)
    {
    my $id = ($Menu == $_) ? ' id=selected' : '';

    aOutl(8, "<li$id><a href=\"$_->{HREF}\">$_->{NAME}</a></li>");
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

    Outml($XHTML_1_Frameset, '<html>');
    print_head($_->{NAME});
    aOutl(2, '<body>');
    print_menu($_);

        # print_mcu_list()
    my $func = $_->{PFUNC};

    $func->(4, $_->{CLASS}) if (defined($func));

    aOutl(4, '</div>');
    aOutl(2, "</body>\n</html>");

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

my $content_background = '#FFFFFF';
my $tab_color          = '#000000';
my $tab_background     = '#E0E0F0';
my $tab_border_color   = '#C0C0C0';
my $border_width       = 1;
my $attr_background    = '#D7DEB2';
my $header_background  = '#CAB2DE';

#-------------------------------------------------------------------------------

sub css_border_radius($$)
  {
  my ($Align, $Radius) = @_;

  aOutl($Align, "-moz-border-radius: $Radius;");
  aOutl($Align, "-webkit-border-radius: $Radius;");
  aOutl($Align, "border-radius: $Radius;");
  }

#-------------------------------------------------------------------------------

sub css_shadow($$)
  {
  my ($Align, $Shadow) = @_;

  aOutl($Align, "-moz-box-shadow: $Shadow;");
  aOutl($Align, "-webkit-box-shadow: $Shadow;");
  aOutl($Align, "box-shadow: $Shadow;");
  }

#-------------------------------------------------------------------------------

sub create_css()
  {
  my $html = "$out_dir/$css";

  open($out_handler, '>', $html) || die "Could not create the \"$html\" file!\n";
  print $out_handler <<EOT
#tabs
  {
  margin: 0;
  padding: 0 0.625em;
  overflow: hidden;
  margin-bottom: -${border_width}px;
  }

#tabs li, #classMenu li
  {
  margin: 0 0.25em;
  padding: 0.5em 0.5em;
  float: left;
  list-style: none;
  color: $tab_color;
  background: $tab_background;
  border: ${border_width}px solid $tab_border_color;
  }

#tabs li
  {
  z-index: 1;
  position: relative;
EOT
;
  css_border_radius(2, '0.5em 0.5em 0 0');
  print $out_handler <<EOT
  border-bottom: 0;
  }

#tabs li#selected
  {
  z-index: 3;
  background: $content_background;
  }

#tabs a
  {
  text-decoration: none;
  }

#tabs li:hover, #classMenu li:hover
  {
  background: -moz-linear-gradient($content_background, $tab_background 100%);
  background: -ms-linear-gradient($content_background, $tab_background 100%);
  background: -o-linear-gradient($content_background, $tab_background 100%);
  background: -webkit-gradient(linear, 0 0, 0 100%, from($content_background), to($tab_background));
  background: -webkit-linear-gradient($content_background, $tab_background 100%);
  background: linear-gradient($content_background, $tab_background 100%);
  -pie-background: linear-gradient($content_background, $tab_background 100%);
  }

#mcuListHeader, #mcuList, #classMenu li, #mcuLinkName, #mcuLink, #mcuAttr, #mcuAttrEnh, #mcuAttrReg,
#configList, #confTableName, #configWord, #confOptName, #traitList, #traitTableName, #traitName, #srcInfo
  {
EOT
;
  css_border_radius(2, '0.5em');
  print $out_handler <<EOT
  }

#mcuList, #configList, #traitList
  {
  z-index: 2;
  padding: 1em 1em;
  position: relative;
  clear: left;
  border: ${border_width}px solid $tab_border_color;
  background: $content_background;
EOT
;
  css_shadow(2, "$tab_border_color 0 0 1em");
  print $out_handler <<EOT
  }

/*----------------------------------------------*/

#classMenu
  {
  height: 2em;
  margin-bottom: 2.5em;
  }

#classMenu li
  {
EOT
;
  css_shadow(2, "$tab_border_color 0 0 1em");
  print $out_handler <<EOT
  }

#classMenu a
  {
  text-decoration: none;
  }

/*----------------------------------------------*/

#mcuListHeader, #confTableName, #traitTableName
  {
  background: $header_background;
  border-color: $header_background;
  border-style: outset;
  }

#mcuListHeader
  {
  text-align: left;
  padding: 0.2em 0.625em;
  font-size: 1.2em;
  border-width: 4px;
  }

#mcuLinkName, #mcuAttr, #mcuAttrEnh, #mcuAttrReg
  {
  text-align: center;
  background: $attr_background;
  border-color: $attr_background;
  border-style: outset;
  border-width: 2px;
  }

#mcuLinkName
  {
  text-align: left;
  padding: 0.2em 0.625em;
  }

#mcuLink:active
  {
  border-style: inset;
  border-width: 2px;
  }

#mcuAttrEnh
  {
  color: #D24E4E;
  padding: 0.2em 0.625em;
  font:bold 1em Georgia;
  }

#mcuAttrReg
  {
  color: #318C31;
  padding: 0.2em 0.625em;
  font:bold 1em Georgia;
  }

/*----------------------------------------------*/

#confTableName, #configWord, #traitTableName
  {
  text-align: center;
  border-width: 4px;
  }

#confTableName, #traitTableName
  {
  font:bold 1.5em Georgia;
  line-height: 1.75em;
  }

#confGap, #traitGap
  {
  height: 1em;
  }

#configWord
  {
  font-size: 1.3em;
  padding: 0.2em 0.625em;
  background: $attr_background;
  border-color: $attr_background;
  border-style: ridge;
  }

#confLine, #traitValue
  {
  background: #BAE7B8;
  }

#confSwName, #confSwValue, #confSwExpl, #traitValue
  {
  padding: 0.2em 0.625em;
  text-align: left;
  }

#confVertMargin, #traitVertMargin
  {
  width: 2%;
  background: $content_background;
  }

#confOptName, #traitName
  {
  padding: 0.5em 0.75em;
  text-align: left;
  background: #A5BDE4;
  border-style: solid;
  border-width: 0;
  }

#srcInfo
  {
  font-size: 0.5em;
  padding: 0.3em 0.5em;
  background: #A5BDE4;
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

            The program on this path looks for the gputils local repopsitory.

        -mi <path> or --mp-info-file <path>

            The program on this path looks for 8bit_device.info file.

        -od <path> or --out-dir <path>

            The program on this path creates the all html files.

        -l <file> or --list-file <file>

            The name of the MCUs, reads from the file. In this way independently
            from the gputils, you can specify the name of MCUs. (The file construction
            of is very simple: One name per line. The name of MCUs, full length
            shall be described. For example: pic16f1933)

        -oc or --only-css

            Only make the $out_dir/$css file.

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
$list_file = '';

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

    when (/^-(l|-list-file)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $list_file = $ARGV[$i++];
      }

    when (/^-(oc|-only-css)$/o)
      {
      $only_css = TRUE;
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

read_all_config_bits();
create_css();
create_class_htmls();

foreach (sort smartSort keys %mcus_by_names)
  {
  dump_all_config_word($_, $mcus_by_names{$_});
  dump_traits($_, $mcus_by_names{$_});
  }

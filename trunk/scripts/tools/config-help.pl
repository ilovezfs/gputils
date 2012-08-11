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

my $verbose = 0;

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
              ENHANCED  => FALSE,
              PAGE_SIZE => 512,
              WORD_SIZE => 12,
              CONF_SIZE => 12,
              EE_START  => 0,
              BANK_SIZE => 32,
              CORE_REGS => [
                           0x00, 0x02, 0x03, 0x04
                           ]
              },

  '16c5xe' => {
              CLASS     => PROC_CLASS_PIC12E,
              ENHANCED  => TRUE,
              PAGE_SIZE => 512,
              WORD_SIZE => 12,
              CONF_SIZE => 12,
              EE_START  => 0,
              BANK_SIZE => 32,
              CORE_REGS => [
                           0x00, 0x02, 0x03, 0x04
                           ]
              },

  '16xxxx' => {
              CLASS     => PROC_CLASS_PIC14,
              ENHANCED  => FALSE,
              PAGE_SIZE => 2048,
              WORD_SIZE => 14,
              CONF_SIZE => 14,
              EE_START  => 0x2100,
              BANK_SIZE => 128,
              CORE_REGS => [
                           0x00, 0x02, 0x03, 0x04,
                           0x0A, 0x0B
                           ]
              },

  '16exxx' => {
              CLASS     => PROC_CLASS_PIC14E,
              ENHANCED  => TRUE,
              PAGE_SIZE => 2048,
              WORD_SIZE => 14,
              CONF_SIZE => 16,
              EE_START  => 0xF000,
              BANK_SIZE => 128,
              CORE_REGS => [
                           0x00, 0x01, 0x02, 0x03,
                           0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B
                           ]
              },

  '17xxxx' => {
              CLASS     => PROC_CLASS_PIC16,
              ENHANCED  => FALSE,
              PAGE_SIZE => 0,
              WORD_SIZE => 16,
              CONF_SIZE => 8,
              EE_START  => 0,
              BANK_SIZE => 256,
              CORE_REGS => [
                           0x00, 0x01, 0x02, 0x03,
                           0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B,
                           0x0C, 0x0D, 0x0E, 0x0F
                           ]
              },

  '18xxxx' => {
              CLASS     => PROC_CLASS_PIC16E,
              ENHANCED  => TRUE,
              PAGE_SIZE => 0,
              WORD_SIZE => 16,
              CONF_SIZE => 8,
              EE_START  => 0xF00000,
              BANK_SIZE => 256,
              CORE_REGS => undef
              }
  );

#-----------------------------------------------

=back
        The structure of one element of the %mcus_by_names hash:

        {
        FEATURES => {
                    CLASS     => PROC_CLASS_PIC1XX,
                    ENHANCED  => FALSE,
                    PAGE_SIZE => 0,
                    WORD_SIZE => 0,     # Instruction size of MCU.
                    CONF_SIZE => 0,     # Width of a config word.
                    EE_START  => 0,     # Start address of EEPROM.
                    BANK_SIZE => 0,     # Size of RAM Banks.
                    CORE_REGS => [],

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
                    ACCESS    => 0      # Last address of lower Access RAM of pic18f series.
                    BAD_RAM   => [
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
                                 ]
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
  return int($Str) if ($Str =~ /^\d+$/o);

  die "This string not number: \"$Str\"";
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

    my $name = $_;

    $name = "pic$name" if ($name =~ /^rf/o);        # rfXXX -> picrfXXX

    $gp_mcus_by_names{uc($name)} = '';
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

    $name =~ s/^p//o;
    $name =~ s/\.inc$//o;
    $name = uc("pic$name");

        # Remember the name of inc file;

    $gp_mcus_by_names{$name} = $_ if (defined($gp_mcus_by_names{$name}));
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

        # Read the RAM features from $Inc file.

sub read_ram_features($$)
  {
  my ($Inc, $Features) = @_;
  my ($line, $full_ram, $state);
  my ($sfrs, $bad_ram);

  open(INC, '<', $Inc) || die "Could not open for reading: $Inc\n";

  Log("Read the RAM features from $Inc.", 4);

  $full_ram = 0;
  $sfrs = [];
  $bad_ram = [];
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
          push(@{$sfrs}, { NAME => $1, ADDR => str2dec($2) });
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

  $Features->{RAM_SIZE} = $full_ram;
  $Features->{BAD_RAM}  = $bad_ram;

  @{$sfrs} = sort {$a->{ADDR} <=> $b->{ADDR}} @{$sfrs};
  $Features->{SFRS}     = $sfrs;
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
  my ($configs, $name, $inc, $class);
  my $features = undef;
  my $addr = 0;
  my $config_count = 0;
  my $switch_count = 0;
  my $setting_count = 0;
  my $switch_info = undef;
  my $state = ST_WAIT;
  my ($cf_addr_min, $cf_addr_max);

  open(IN, '<', $dev_info) || die "Could not open for reading: $dev_info\n";

  Log("Reads all config options from $dev_info.", 4);

  my $msg = ($list_file ne '') ? 'list file' : 'Gputils';

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

        my $tr = $mcu_features{$class};

        die "Unknown class of $name MCU!" if (! defined($tr));

        $features = {
                    CLASS     => $tr->{CLASS},
                    ENHANCED  => $tr->{ENHANCED},
                    PAGE_SIZE => $tr->{PAGE_SIZE}, # Size of program memory pages.
                    WORD_SIZE => $tr->{WORD_SIZE}, # Size of instructions.
                    CONF_SIZE => $tr->{CONF_SIZE}, # Size of Config Words.
                    EE_START  => $tr->{EE_START},  # Start address of EEPROM.
                    BANK_SIZE => $tr->{BANK_SIZE}, # Size of RAM Banks.
                    CORE_REGS => $tr->{CORE_REGS},

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
                    ACCESS    => hex($fields[10]),  # Last address of lower Access RAM of pic18f series.
                    BAD_RAM   => [],
                    SFRS      => []
                    };

        $inc = $gp_mcus_by_names{$name};

        if (defined($inc) && $inc ne '')
          {
          read_ram_features("$gputils_path/header/$inc", $features);
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

            $features->{CF_START} = $cf_addr_min;
            $features->{CF_END}   = $cf_addr_max;
            $mcus_by_names{$name}{FEATURES}  = $features;
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

        # Print the head of html file.

sub print_html_head($)
  {
  print $out_handler <<EOT
$XHTML_1_Frameset
<html>
  <head>
    <title>$_[0]</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <link rel="stylesheet" type="text/css" href="$css">
  </head>
EOT
;
  }

#-------------------------------------------------------------------------------

sub src_info($$)
  {
  my ($Align, $GapId) = @_;

  aOutl($Align, "<p id=$GapId></p>");
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

  Log("Print list of MCUs.", 4);

  aOutl($Align + 2, '<table id=mcuList>');
  aOutl($Align + 4, '<tr>');
  aOutl($Align + 6, "${lst}name</th>");

  given ($Class)
    {
    when (PRI_MENU_ALL)
      {
      aOutl($Align + 6, "${lst}instruction size (bit)</th>");
      aOutl($Align + 6, "${lst}config word size (bit)</th>");
      aOutl($Align + 6, "${lst}class</th>");
      }

    when ([ PRI_MENU_ENH, PRI_MENU_REG ])
      {
      aOutl($Align + 6, "${lst}instruction size (bit)</th>");
      aOutl($Align + 6, "${lst}config word size (bit)</th>");
      }

    default
      {
      aOutl($Align + 6, "${lst}config word size (bit)</th>");
      aOutl($Align + 6, "${lst}class</th>");
      }
    }

  aOutl($Align + 4, '</tr>');
  aOutl($Align + 4, '<tr id=mcuGap></tr>');

  foreach (sort { smartSort($a, $b) } keys %mcus_by_names)
    {
    my $td_href  = "<td id=mcuLinkName><a id=mcuLink href=\"${_}-$feat_tag.html\">$_</a></td>";
    my $features = $mcus_by_names{$_}->{FEATURES};
    my $td_wsize = "<td id=mcuAttr>$features->{WORD_SIZE}</td>";
    my $td_csize = "<td id=mcuAttr>$features->{CONF_SIZE}</td>";
    my $td_class = ($features->{ENHANCED}) ? '<td id=mcuAttrEnh>enhanced</td>' : '<td id=mcuAttrReg>regular</td>';

    given ($Class)
      {
      when (PRI_MENU_ALL)
        {
        aOutl($Align + 4, '<tr>');
        aOutl($Align + 6, $td_href);
        aOutl($Align + 6, $td_wsize);
        aOutl($Align + 6, $td_csize);
        aOutl($Align + 6, $td_class);
        aOutl($Align + 4, '</tr>');
        }

      when (PRI_MENU_ENH)
        {
        if ($features->{ENHANCED})
          {
          aOutl($Align + 4, '<tr>');
          aOutl($Align + 6, $td_href);
          aOutl($Align + 6, $td_wsize);
          aOutl($Align + 6, $td_csize);
          aOutl($Align + 4, '</tr>');
          }
        }

      when (PRI_MENU_REG)
        {
        if (! $features->{ENHANCED})
          {
          aOutl($Align + 4, '<tr>');
          aOutl($Align + 6, $td_href);
          aOutl($Align + 6, $td_wsize);
          aOutl($Align + 6, $td_csize);
          aOutl($Align + 4, '</tr>');
          }
        }

      when (PRI_MENU_12_BIT)
        {
        if ($features->{WORD_SIZE} == 12)
          {
          aOutl($Align + 4, '<tr>');
          aOutl($Align + 6, $td_href);
          aOutl($Align + 6, $td_csize);
          aOutl($Align + 6, $td_class);
          aOutl($Align + 4, '</tr>');
          }
        }

      when (PRI_MENU_14_BIT)
        {
        if ($features->{WORD_SIZE} == 14)
          {
          aOutl($Align + 4, '<tr>');
          aOutl($Align + 6, $td_href);
          aOutl($Align + 6, $td_csize);
          aOutl($Align + 6, $td_class);
          aOutl($Align + 4, '</tr>');
          }
        }

      when (PRI_MENU_16_BIT)
        {
        if ($features->{WORD_SIZE} == 16)
          {
          aOutl($Align + 4, '<tr>');
          aOutl($Align + 6, $td_href);
          aOutl($Align + 6, $td_csize);
          aOutl($Align + 6, $td_class);
          aOutl($Align + 4, '</tr>');
          }
        }
      }
    }

  aOutl($Align + 2, '</table>');
  src_info($Align + 2, 'confGap');
  }

#-------------------------------------------------------------------------------

        # Print the local menu of $Name MCU page.

sub print_mcu_menu($$)
  {
  my ($Name, $Selected) = @_;

  aOutl(4, '<ul id=tabs>');

  foreach (@mcu_menu_elems)
    {
    my $id = ($_->{CLASS} == $Selected) ? ' id=selected' : '';

    aOutl(6, "<li$id><a href=\"${Name}$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(4, '</ul>');
  }

#-------------------------------------------------------------------------------

        # Dump the features of $Name MCU.

sub dump_features($$)
  {
  my ($Name, $Configs) = @_;
  my $features = $Configs->{FEATURES};
  my ($str, $len, $rom_size, $word_size, $i, $t);

  $str = "$out_dir/${Name}-$feat_tag.html";
  open($out_handler, '>', $str) || die "Could not create the \"$str\" file!\n";

  Log("Dump the features of $Name.", 4);

        #------------------------------------

  print_html_head($Name);
  aOutl(2, '<body>');

        # The main menu.

  aOutl(4, '<ul id=classMenu>');

  foreach (@pri_menu_elems)
    {
    aOutl(6, "<li><a href=\"$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(4, '</ul>');

  print_mcu_menu($Name, MCU_MENU_FEAT);

        #------------------------------------

        # The table of features of MCU.

  $word_size = $features->{WORD_SIZE};
  $len = ($word_size == 16) ? 6 : 4;
  $rom_size = $features->{ROM} + 1;

  aOutl(4, '<table id=featList>');
  aOutl(6, "<tr><th colspan=4 id=featTableName>$Name</th></tr>");
  aOutl(6, '<tr id=featGap></tr>');

        #------------------------------------

  aOutl(6, '<tr id=featLine>');
  aOutl(8, '<td id=featVertMargin></td>');
  aOutl(8, '<th id=featName>Coff ID of device</th>');
  aOutfl(8, '<td id=featValue>0x%04X</td>', $features->{COFF});
  aOutl(8, '<td id=featVertMargin></td>');
  aOutl(6, '</tr>');

        #------------------------------------

  $i = $features->{PAGE_SIZE};
  if ($i > 0)
    {
    aOutl(6, '<tr id=featLine>');
    aOutl(8, '<td id=featVertMargin></td>');
    aOutl(8, '<th id=featName>Number of ROM/FLASH pages</th>');
    aOutl(8, "<td id=featValue>$features->{PAGES} ($i words/pages)</td>");
    aOutl(6, '</tr>');
    }

        #------------------------------------

  aOutl(6, '<tr id=featLine>');
  aOutl(8, '<td id=featVertMargin></td>');
  aOutl(8, '<th id=featName>Last address of ROM/FLASH</th>');

  $t = ($word_size == 16) ? 'bytes' : 'words';
  aOutfl(8, "<td id=featValue>0x%0${len}X   ($rom_size $t)</td>", $features->{ROM});
  aOutl(6, '</tr>');

        #------------------------------------

  $i = $features->{FLASHDATA};
  if ($i > 0)
    {
    aOutl(6, '<tr id=featLine>');
    aOutl(8, '<td id=featVertMargin></td>');
    aOutl(8, '<th id=featName>Address space of FLASH Data</th>');

    aOutfl(8, "<td id=featValue>0x%0${len}X - 0x%0${len}X   (%i words)</td>",
               $rom_size, $rom_size + $i, $i + 1);

    aOutl(6, '</tr>');
    }

        #------------------------------------

  $i = $features->{CONFIGS};
  $t = ($word_size == 16) ? 'Byte' : 'Word';
  $t .= 's' if ($i > 1);
  aOutl(6, '<tr id=featLine>');
  aOutl(8, '<td id=featVertMargin></td>');
  aOutl(8, "<th id=featName>Address space of Configuration $t</th>");
  aOutfl(8, "<td id=featValue>0x%0${len}X - 0x%0${len}X   ($i %s)</td>",
             $features->{CF_START}, $features->{CF_END}, lc($t));
  aOutl(6, '</tr>');

        #------------------------------------

  $i = $features->{EEPROM};
  if ($i > 0)
    {
    aOutl(6, '<tr id=featLine>');
    aOutl(8, '<td id=featVertMargin></td>');
    aOutl(8, '<th id=featName>Address space of EEPROM</th>');

    aOutfl(8, "<td id=featValue>0x%0${len}X - 0x%0${len}X   (%i bytes)</td>",
               $features->{EE_START}, $features->{EE_START} + $i, $i + 1);

    aOutl(6, '</tr>');
    }

        #------------------------------------

  aOutl(6, '<tr id=featLine>');
  aOutl(8, '<td id=featVertMargin></td>');
  aOutl(8, '<th id=featName>Number of RAM Banks</th>');
  aOutl(8, "<td id=featValue>$features->{BANKS} ($features->{BANK_SIZE} bytes/banks)</td>");
  aOutl(6, '</tr>');

        #------------------------------------

  $i = @{$features->{SFRS}};
  if ($i > 0)
    {
    aOutl(6, '<tr id=featLine>');
    aOutl(8, '<td id=featVertMargin></td>');
    aOutl(8, '<th id=featName>Number of SFRs</th>');
    aOutl(8, "<td id=featValue>$i</td>");
    aOutl(6, '</tr>');

    if ($features->{RAM_SIZE} > 0)
      {
      $i = $features->{RAM_SIZE} - $i;
      aOutl(6, '<tr id=featLine>');
      aOutl(8, '<td id=featVertMargin></td>');
      aOutl(8, '<th id=featName>Size of GPRs</th>');
      aOutl(8, "<td id=featValue>$i bytes</td>");
      aOutl(6, '</tr>');
      }
    }

        #------------------------------------

  $i = $features->{RAM_SIZE};
  if ($i > 0)
    {
    aOutl(6, '<tr id=featLine>');
    aOutl(8, '<td id=featVertMargin></td>');
    aOutl(8, '<th id=featName>Full size of SFRs + GPRs</th>');
    aOutl(8, "<td id=featValue>$i bytes</td>");
    aOutl(6, '</tr>');
    }

        #------------------------------------

  if ($word_size == 16)
    {
    aOutl(6, '<tr id=featLine>');
    aOutl(8, '<td id=featVertMargin></td>');
    aOutl(8, '<th id=featName>Last address of lower Access RAM</th>');
    aOutfl(8, "<td id=featValue>0x%02X</td>", $features->{ACCESS});
    aOutl(6, '</tr>');
    }

        #------------------------------------

  aOutl(4, '</table>');
  src_info(4, 'featGap');
  aOutl(2, "</body>\n</html>");
  close($out_handler);
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
  my $config_mask = (ULONG_MAX << $Configs->{FEATURES}->{CONF_SIZE}) ^ ULONG_MAX;
  my $count = @addresses;
  my ($str, $len, $i, $head_s, $head_e, $gap);

  return if (! $count);

  $str = "$out_dir/${Name}-$conf_tag.html";
  open($out_handler, '>', $str) || die "Could not create the \"$str\" file!\n";

  Log("Dump the Config Options of $Name.", 4);

  $len = 4;
  $head_s = '<tr><th colspan=4 id=configWord>';
  $head_e = '</th></tr>';
  $gap    = '<tr id=confGap></tr>';

        #------------------------------------

  print_html_head($Name);
  aOutl(2, '<body>');

        # The main menu.

  aOutl(4, '<ul id=classMenu>');

  foreach (@pri_menu_elems)
    {
    aOutl(6, "<li><a href=\"$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(4, '</ul>');

  print_mcu_menu($Name, MCU_MENU_CONF);

        #------------------------------------

        # The table of Config Bits.

  aOutl(4, '<table id=configList>');
  aOutl(4, "<tr><th colspan=5 id=confTableName>$Name</th></tr>");

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
  src_info(4, 'confGap');
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
  my $core_regs = $Features->{CORE_REGS};
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

  if (defined($core_regs))
    {
    $k = $core_regs->[$#{$core_regs}] + 1;
    $bank = 0;
    for ($x = 0; $x < $bank_num; ++$x)
      {
      splice(@{$Array}, $bank, $k, ((RAM_BAD) x $k));
      $bank += $bank_size;
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
  my $core_regs = $Features->{CORE_REGS};
  my $sfrs      = $Features->{SFRS};
  my ($bank, $x);

        # Places the core registers. These at the same address there is in the all banks.

  if (defined($core_regs))
    {
    foreach (@{$core_regs})
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

  foreach (@{$sfrs})
    {
    $Array->[$_->{ADDR}] = RAM_SFR;
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
  my $core_regs = $features->{CORE_REGS};
  my $sfrs      = $features->{SFRS};
  my @ram_array;
  my @map_array;
  my ($map, $bank, $k, $x, $y, $t);

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
          $map->[$map_index] = { TYPE => $t_prev, SIZE => $size };
          ++$map_index;
          }

        $t_prev = $t;
        $size = 1;
        }

      ++$y;

      if ($y == $bank_size)
        {
        $map->[$map_index] = { TYPE => $t, SIZE => $size };
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

  aOutl(4, '<ul id=classMenu>');

  foreach (@pri_menu_elems)
    {
    aOutl(6, "<li><a href=\"$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(4, '</ul>');

  print_mcu_menu($Name, MCU_MENU_RAM);

        #------------------------------------

        # The table of RAM map of MCU.

  aOutl(4, '<table id=ramMap>');
  aOutfl(6, "<tr><th colspan=%i id=ramTableName>$Name</th></tr>", $bank_num * 3);
  aOutl(6, '<tr id=ramGap></tr>');

        #------------------------------------

        # header

  aOutl(6, '<tr id=ramHead>');

  for ($x = 0; $x < $bank_num; ++$x)
    {
    aOutl(8, '<td id=ramVertMargin>');
    aOutl(8, "<th id=ramBank>Bank $x</th>");
    aOutl(8, '<td id=ramVertMargin>');
    }

  aOutl(6, '</tr>');
  aOutl(6, '<tr id=ramGap></tr>');

        #------------------------------------

  aOutl(6, '<tr>');

  $k = int(256 / $bank_size);
  for ($x = 0; $x < $bank_num; ++$x)
    {
    $map = $map_array[$x];

    aOutl(8, '<td id=ramVertMargin></td>');
    aOutl(8, '<td id=ramColumn>');

    foreach (@{$map})
      {
      given ($_->{TYPE})
        {
        when (RAM_GPR) { $t = 'ramGPR'; }
        when (RAM_SFR) { $t = 'ramSFR'; }
        default        { $t = 'ramBAD'; }
        }

      aOutfl(10, "<a id=$t style=\"height: %ipx\"></a>", $_->{SIZE} * $k);
      }

    aOutl(8, '</td>');
    aOutl(8, '<td id=ramVertMargin></td>');
    }

  aOutl(6, '</tr>');

        #------------------------------------

  aOutl(4, '</table>');
  src_info(4, 'ramGap');
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # Dump the SFR map of $Name MCU.

sub dump_sfr_map($$)
  {
  my ($Name, $Configs) = @_;
  my $features  = $Configs->{FEATURES};
  my $bank_size = $features->{BANK_SIZE};
  my $sfrs      = $features->{SFRS};
  my @bank_array;
  my ($bank, $i, $max_x, $max_y, $x, $y, $t);

  $t = "$out_dir/${Name}-$sfr_tag.html";
  open($out_handler, '>', $t) || die "Could not create the \"$t\" file!\n";

  Log("Dump the SFR map of $Name.", 4);

        #------------------------------------

        # Tabulate the SFRs.

  $i = -1;
  $bank = -1;
  $y = -1;
  $max_y = 0;
  foreach (@{$sfrs})
    {
    my $b = int($_->{ADDR} / $bank_size);

    if ($bank != $b)
      {
      $bank = $b;
      ++$i;
      $bank_array[$i]->{BANKNUM} = $bank;
      $max_y = $y if ($max_y < $y);
      $y = -1;
      }

    push(@{$bank_array[$i]->{BANK}}, $_);
    ++$y;
    }

  $max_y = $y if ($max_y < $y);
  $max_x = @bank_array;

        #------------------------------------

  print_html_head($Name);
  aOutl(2, '<body>');

        # The main menu.

  aOutl(4, '<ul id=classMenu>');

  foreach (@pri_menu_elems)
    {
    aOutl(6, "<li><a href=\"$_->{HREF}\">$_->{NAME}</a></li>");
    }

  aOutl(4, '</ul>');

  print_mcu_menu($Name, MCU_MENU_SFR);

        #------------------------------------

        # The table of SFR map of MCU.

  aOutl(4, '<table id=sfrMap>');
  aOutfl(6, "<tr><th colspan=%i id=sfrTableName>$Name</th></tr>", $max_x * 4);
  aOutl(6, '<tr id=sfrGap></tr>');

        #------------------------------------

        # header

  aOutl(6, '<tr id=sfrHead>');

  for ($x = 0; $x < $max_x; ++$x)
    {
    aOutl(8, '<td id=sfrVertMargin>');
    aOutl(8, "<th colspan=2 id=sfrBank>Bank $bank_array[$x]->{BANKNUM}</th>");
    aOutl(8, '<td id=sfrVertMargin>');
    }

  aOutl(6, '</tr>');
  aOutl(6, '<tr id=sfrGap></tr>');

        #------------------------------------

  for ($y = 0; $y < $max_y; ++$y)
    {
    aOutl(6, '<tr id=sfrLine>');

    for ($x = 0; $x < $max_x; ++$x)
      {
      $t = $bank_array[$x]->{BANK}[$y];

      aOutl(8, '<td id=sfrVertMargin></td>');

      if (defined($t))
        {
        aOutl(8, "<th id=sfrName>$t->{NAME}</th>");
        aOutfl(8, '<td id=sfrAddr>0x%04X</td>', $t->{ADDR});
        }
      else
        {
        aOutl(8, '<th id=sfrName></th>');
        aOutl(8, '<td id=sfrAddr></td>');
        }

      aOutl(8, '<td id=sfrVertMargin></td>');
      }

    aOutl(6, '</tr>');
    }

        #------------------------------------

  aOutl(4, '</table>');
  src_info(4, 'sfrGap');
  aOutl(2, "</body>\n</html>");
  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # Print the menu of class of MCUs.

sub print_pri_menu($)
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

    print_html_head($_->{NAME});
    aOutl(2, '<body>');
    print_pri_menu($_);

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
  padding: 1em 1em 0;
  overflow: hidden;
  margin-bottom: -${border_width}px;
  }

#tabs li, #classMenu li
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
  css_shadow(2, "$tab_border_color 0 0 1em");
  print $out_handler <<EOT
  }

#tabs li
  {
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
#configList, #confTableName, #configWord, #confOptName,
#featList, #featTableName, #featName,
#ramMap, #ramTableName, #ramBank,
#sfrMap, #sfrTableName, #sfrBank, #sfrName,
#srcInfo
  {
EOT
;
  css_border_radius(2, '0.5em');
  print $out_handler <<EOT
  }

#mcuList, #configList, #featList, #ramMap, #sfrMap
  {
  z-index: 2;
  position: relative;
  width: auto;
  padding: 1em 1em;
  background: $content_background;
  border: ${border_width}px solid $tab_border_color;
EOT
;
  css_shadow(2, "$tab_border_color 0 0 1em");
  print $out_handler <<EOT
  }

#mcuList
  {
  min-width: 95%;
  }

/*----------------------------------------------*/

#classMenu
  {
  height: 2em;
  margin-bottom: 1.5em;
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

#mcuListHeader, #confTableName, #featTableName, #ramTableName, #sfrTableName
  {
  background: $header_background;
  border-color: $header_background;
  border-style: outset;
  }

#mcuListHeader, #mcuLinkName, #mcuAttrEnh, #mcuAttrReg, #configWord,
#confSwName, #confSwValue, #confSwExpl,
#featValue,
#ramBank,
#sfrBank, #sfrAddr
  {
  padding: 0.2em 0.625em;
  }

#mcuListHeader
  {
  text-align: left;
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
  }

#mcuLink:active
  {
  border-style: inset;
  border-width: 2px;
  }

#mcuAttrEnh, #mcuAttrReg
  {
  font: bold 1em Georgia;
  }

#mcuAttrEnh
  {
  color: #D24E4E;
  }

#mcuAttrReg
  {
  color: #318C31;
  }

/*----------------------------------------------*/

#confTableName, #configWord, #featTableName,
#ramBank, #ramTableName,
#sfrBank, #sfrTableName
  {
  text-align: center;
  border-width: 4px;
  }

#confTableName, #featTableName, #ramTableName, #sfrTableName
  {
  font:bold 1.5em Georgia;
  line-height: 1.75em;
  }

#mcuGap, #confGap, #featGap, #ramGap, #sfrGap
  {
  height: 1em;
  }

#configWord, #ramBank, #sfrBank
  {
  font-size: 1.3em;
  background: $attr_background;
  border-color: $attr_background;
  border-style: ridge;
  }

#confLine, #featValue, #sfrAddr
  {
  background: #BAE7B8;
  }

#confOptName, #confSwName, #confSwValue, #confSwExpl,
#featName, #featValue,
#sfrName, #sfrAddr
  {
  text-align: left;
  }

#confVertMargin, #featVertMargin, #sfrVertMargin
  {
  width: 2%;
  background: $content_background;
  }

#ramVertMargin
  {
  width: auto;
  background: $content_background;
  }

#confOptName, #featName, #sfrName
  {
  padding: 0.5em 0.75em;
  background: #A5BDE4;
  border-style: solid;
  border-width: 0;
  }

#ramColumn
  {
  width: 100px;
  }

#ramColumn a
  {
  float: left;
  width: 100px;
  }

#ramBAD
  {
  background: $tab_border_color;
  }

#ramGPR
  {
  background: #4EB34E;
  }

#ramSFR
  {
  background: #C54141;
  }

#ramMap, #sfrMap
  {
  min-width: 60%;
  }

#sfrLine
  {
  font-size: 0.75em;
  }

#srcInfo
  {
  font-size: 0.75em;
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

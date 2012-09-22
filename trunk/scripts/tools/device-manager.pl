#!/usr/bin/perl -w

=back

   Copyright (C) 2012, Molnár Károly <molnarkaroly@users.sf.net>

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   ------------------------------------------------------------------

   This program simplifies the addition of MCUs to the gputils.
   In addition can be removed also a MCU from the gputils.
   This program has two main applications area. Can you have different
   lists and comparisons to prepare.

   The program operation is necessary to:

        1.) An installed mplabx package.
        2.) The source code of gputils from the svn repository.
            (Both paths can be given on the command line.)

        3.) Of course does not trouble, if you have a functional Perl interpreter. ;-)

   The end of the addition and deletion operations, the program creates
   the following files:

        gpprocessor.c.gen
        p16f1xxx.inc.gen
        p18cxxx.inc.gen

   If the operation of the addition was, then still creates two files:

        DEVICE.inc.gen
        pDEVICE_g.lkr.gen

   If the program get the '-h' command line switch then you give a help on
   how to be used.

   $Id$
=cut

use strict;
use warnings;
use 5.10.1;                     # Because of her need: ${^POSTMATCH}
use feature 'switch';           # Starting from 5.10.1.
use POSIX qw(strftime);

use constant FALSE => 0;
use constant TRUE  => 1;

use constant ST_WAIT   => 0;
use constant ST_LISTEN => 1;

use constant AL_NAME => 17;
use constant AL_ADDR => 22;

use constant OP_NULL         => 0;
use constant OP_ADD          => 1;
use constant OP_REMOVE       => 2;
use constant OP_REGENERATE   => 3;
use constant OP_FABR_INC     => 4;
use constant OP_FABR_LKR     => 5;
use constant OP_LIST_GP      => 6;
use constant OP_LIST_MP      => 7;
use constant OP_SURVEY       => 8;
use constant OP_SHOWS_CDIFF  => 9;
use constant OP_SHOWS_PXDIFF => 10;

use constant SORT_DEFINED_AS => 0;
use constant SORT_NAME0      => 1;
use constant SORT_NAME1      => 2;
use constant SORT_NAME2      => 3;

use constant RP_ADD   => 0;
use constant RP_PRINT => 1;

use constant E_COFF_COLL => 0;
use constant E_COFF_DIFF => 1;
use constant E_ROM_DIFF  => 2;
use constant E_NAME_COLL => 3;
use constant E_LKR_NAME  => 4;

my $PROGRAM = 'device-manager.pl';

my $verbose = 0;

my $border0 = ('-' x 70);
my $border1 = ('=' x 60);

use constant PROC_CLASS_EEPROM8  => 0;
use constant PROC_CLASS_EEPROM16 => 1;
use constant PROC_CLASS_GENERIC  => 2;
use constant PROC_CLASS_PIC12    => 3;
use constant PROC_CLASS_PIC14    => 4;
use constant PROC_CLASS_PIC14E   => 5;
use constant PROC_CLASS_PIC16    => 6;
use constant PROC_CLASS_PIC16E   => 7;
use constant PROC_CLASS_SX       => 8;

my @classnames =
  (
  'PROC_CLASS_EEPROM8',
  'PROC_CLASS_EEPROM16',
  'PROC_CLASS_GENERIC',
  'PROC_CLASS_PIC12',
  'PROC_CLASS_PIC14',
  'PROC_CLASS_PIC14E',
  'PROC_CLASS_PIC16',
  'PROC_CLASS_PIC16E',
  'PROC_CLASS_SX'
  );

my %exist_classes =
  (
  'eeprom8'  => PROC_CLASS_EEPROM8,
  'eeprom16' => PROC_CLASS_EEPROM16,
  'generic'  => PROC_CLASS_GENERIC,
  '16c5x'    => PROC_CLASS_PIC12,
  '16c5xe'   => PROC_CLASS_PIC12,
  '16xxxx'   => PROC_CLASS_PIC14,
  '16exxx'   => PROC_CLASS_PIC14E,
  '17xxxx'   => PROC_CLASS_PIC16,
  '18xxxx'   => PROC_CLASS_PIC16E,
  'sx'       => PROC_CLASS_SX
  );

my @eeprom_starts =
  (
        -1,     # PROC_CLASS_EEPROM8
        -1,     # PROC_CLASS_EEPROM16
        -1,     # PROC_CLASS_GENERIC
        -1,     # PROC_CLASS_PIC12
  0x002100,     # PROC_CLASS_PIC14
  0x00F000,     # PROC_CLASS_PIC14E
        -1,     # PROC_CLASS_PIC16
  0xF00000,     # PROC_CLASS_PIC16E
        -1      # PROC_CLASS_SX
  );

my %config_word_masks =
  (
  '16c5x'  => 0x0FFF,
  '16c5xe' => 0x0FFF,
  '16xxxx' => 0x3FFF,
  '16exxx' => 0xFFFF,
  '17xxxx' => 0x00FF,
  '18xxxx' => 0x00FF
  );

my $name_filter = qr/10l?f\d+[a-z]*|1[26]((c(e|r)?)|hv)\d+[a-z]*|17c\d+[a-z]*|1[268]l?f\d+([a-z]*|[a-z]+\d+[a-z]*)/i;

my $gputils_path = "$ENV{HOME}/svn_snapshots/gputils/gputils";
my $mplabx_path  = '/opt/microchip/mplabx';
my $mplabx_dev_info = '8bit_device.info';

my $gpprocessor_c = 'gpprocessor.c';
my $p16f1xxx_inc  = 'p16f1xxx.inc';
my $p18cxxx_inc   = 'p18cxxx.inc';

my $out_tail = '.gen';

my $gpproc_path;

my $gputils_inc;
my $gputils_lkr;

my $mplabx_inc;
my $mplabx_lkr;

my $list_file;
my %list_file_members;
my $mcu;
my $operation;
my $add_config_bits = FALSE;
my $extended_list = FALSE;
my $examine_exist_device = TRUE;
my $time_str = '';
my $out_handler;
my $timestamp = FALSE;

#-------------------------------------------------------------------------------

my $gp_border = ';' . ('=' x 74);
my $igh = ';;;;';
my $gp_added_start = "$igh Begin: Added in gputils";
my $gp_added_end   = "$igh End: Added in gputils";
my $gp_spec_hist   = 'Gputils Specific Revision History';
my $conf_sect      = 'Configuration Bits';

my $gp_rev_hist_begin;  # The begin of the "Gputils Specific Revision History" the in the include file.
my $gp_rev_hist_end;    # The end of the "Gputils Specific Revision History" in the include file.

my $config_mask = 0xFFFF;

################################################################################
################################################################################

my @pp_def_names = ();          # Names of definitions.
my %pp_defines = ();            # Value of definitions.

my @pp_conditions = ();
my @pp_else_conditions = ();
my $pp_level = 0;   # Shows the lowest level.
my $pp_line_number;             # Line number of a lkr file.

################################################################################
################################################################################

my @gpproc_c_content;           # Full content of gpprocessor.c file.
my $px_struct_begin;            # The begin of the px structure in the gpprocessor.c file.
my $px_struct_end;              # The end of the px structure in the gpprocessor.c file.

#-----------------------------------------------

=back
        The structure of one element of the @gp_px_struct and @mp_px_struct arrays:

        [
        IGNORED      => FALSE,          True if line is commented.
        CLASS        => '',
        DEFINED_AS   => '',
        NAMES        => ['', '', ''],
        COFF_TYPE    => 0,
        NUM_PAGES    => 0,
        NUM_BANKS    => 0,
        MAXROM       => 0,
        BADROM       => [0, 0],
        CONFIG_ADDRS => [0, 0],
        SCRIPT       => '',
        COMMENT      => ''              Comment on the end of line.
        ]
=cut

my @gp_px_struct;
my @mp_px_struct;

#-----------------------------------------------

my %gp_px_rows_by_name;
my %mp_px_rows_by_name;

my %gp_px_rows_by_coff;
my %mp_px_rows_by_coff;

my @lkr_pages;
my $lkr_config_start;
my $lkr_config_end;

#-----------------------------------------------

=back
        The structure of one element of the @mp_mcus arrays:
        [
        NAME      => '',
        CLASS     => 0,
        COFF      => 0,
        PAGES     => 0,
        ROM       => 0,
        FLASHDATA => 0,
        EEPROM    => 0,
        CONFIGS   => 0,
        BANKS     => 0,
        ACCESS    => 0
        ]
=cut

my @mp_mcus;

#-----------------------------------------------

my %mp_mcus_by_name;

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@                             @@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@  This a simple preprocessor.  @@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@                             @@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  # Examines that the parameter is defined or not defined.

sub _defined($)
  {
  return defined($pp_defines{$_[0]});
  }

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

  # Evaluation of the #if give a boolean value. This procedure preserves it.

sub if_condition($)
  {
  my $Val = $_[0];

  push(@pp_conditions, $Val);
  push(@pp_else_conditions, $Val);
  ++$pp_level;
  }

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

  # Closes a logical unit which starts with a #if.

sub endif_condition($)
  {
  die "endif_condition(): The ${pp_line_number}th line of $_[0] there is a #endif, but does not belong him #if.\n" if ($pp_level <= 0);

  pop(@pp_conditions);
  pop(@pp_else_conditions);
  --$pp_level;
  }

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

        # This the preprocessor.

sub run_preprocessor($$$)
  {
  my ($Fname, $Function, $Line) = @_;

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
    $Function->($Line);
    }
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@  Some auxiliary function.  @@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

sub basename($)
  {
  return ($_[0] =~ /([^\/]+)$/) ? $1 : '';
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

sub align($$)
  {
  my $Text = $_[0];
  my $al   = $_[1] - length($Text);

        # One space will surely becomes behind it.
  $al = 1 if ($al < 1);

  return ($Text . ' ' x $al);
  }

#-------------------------------------------------------------------------------

sub str2dec($)
  {
  my $Str = $_[0];

  return hex($1)   if ($Str =~ /^0x([[:xdigit:]]+)$/io);
  return int($Str) if ($Str =~ /^-?\d+$/o);

  die "str2dec(): This string is not integer: \"$Str\"";
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

sub smartSort($$$)
  {
  my @a_s = ($_[0] =~ /(\d+|\D+)/go);
  my @b_s = ($_[1] =~ /(\d+|\D+)/go);
  my ($i, $k, $end, $ret);

  if ($_[2])
    {
        # Ignore case.
    @a_s = map { lc($_); } @a_s;
    @b_s = map { lc($_); } @b_s;
    }

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

sub str2class($)
  {
  my $Str = $_[0];
  my $class = $exist_classes{lc($Str)};

  return $class if (defined($class));

  die "str2class(): Unknown class: $Str\n";
  }

#-------------------------------------------------------------------------------

        # Creates a sorted list of the files which are located
        # in the $Dir directory. The end of file names can be the $Tail only.
        # (The small letters and capital letters are not different.)

sub create_dirlist($$$)
  {
  my ($Array, $Dir, $Tail) = @_;

  opendir(DIR, $Dir) || die "create_dirlist(): Can not open. -> \"$Dir\"\n";
  @{$Array} = sort { smartSort($a, $b, TRUE) } grep(-f "$Dir/$_" && /^.+$Tail$/i, readdir(DIR));
  closedir(DIR);
  }

#-------------------------------------------------------------------------------

        # Reads the $Name text file into the $Array.
        # In the readed file to removes the DOS end of line characters from.

sub read_file($$)
  {
  my ($Name, $Array) = @_;

  open(IN, '<', $Name) || die "read_file(): Can not open. -> \"$Name\"\n";
  Log("Reads the $Name file.", 4);
  @{$Array} = map { chomp($_); $_ =~ s/\r$//o; $_; } <IN>;
  close(IN);
  }

#-------------------------------------------------------------------------------

        # Searches for the possible location of the gputils history.
        # Also went out looking section of the Configuration Bits as well.

sub assess_include($)
  {
  my $n = 0;
  my $start = -1;

  $gp_rev_hist_begin = -1;
  $gp_rev_hist_end   = -1;
  foreach (@{$_[0]})
    {
    if ($_ =~ /^\s*;\s*=+\s*$/io)
      {
      $start = $n;
      $gp_border = $_;
      }
    elsif ($start > 0 && $_ =~ /^\s*;\s*Verify\s+Processor\s*$/io)
      {
      $gp_rev_hist_begin = $start - 1;
      $gp_rev_hist_end   = $start;
      }
    elsif ($_ =~ /^\s*;\s*Configuration\s+Bits\s*$/io)
      {
      return TRUE;
      }

    ++$n;
    } # foreach (@{$_[0]})

  return FALSE;
  }

#-------------------------------------------------------------------------------

        # Delete the last LIST line.

sub delete_last_list($)
  {
  my $Array = $_[0];
  my $i;

  for ($i = $#{$Array}; $i >= 0; --$i)
    {
    if ($Array->[$i] =~ /^\s*LIST\s*$/io)
      {
      splice(@{$Array}, $i, 1);
      last;
      }
    }
  }

#-------------------------------------------------------------------------------

        # Reads config bits of $Mcu from the $Info.

sub read_config_bits($$$)
  {
  my ($Info, $Mcu, $Configs) = @_;
  my $addr = 0;
  my $config_count = 0;
  my $switch_count = 0;
  my $setting_count = 0;
  my $switch_info = undef;
  my $state = ST_WAIT;

  open(IN, '<', $Info) || die "Could not open for reading: $Info\n";

=back
        The structure of one element of the %{$Configs} hash:

        '300005' => [
                      {
                      'HEAD' => 'CCP2MX',
                      'BITS' => [
                                  {
                                  'NAME'  => 'OFF',
                                  'VALUE' => 0,
                                  'EXPL'  => 'CCP2 input/output is multiplexed with RB3'
                                  },

                                  ...

                                  {}
                                ],
                      'MASK' => 1,
                      'EXPL' => 'CCP2 MUX bit'
                      },

                      ...

                      {}
                    ]
=cut

  %{$Configs} = ();

  while (<IN>)
    {
    my @fields = ($_ =~ /<([^<>]*)>/go);

    next if (@fields < 3);

    if ($fields[0] eq 'PART_INFO_TYPE')
      {
        # <PART_INFO_TYPE><f220><PIC10F220><16c5x><0><0><ff><1><1f><0><0><0><1>
        # <PART_INFO_TYPE><e529><PIC12F529T39A><16c5xe><0><3><5ff><8><1f><0><0><3f><1>
        # <PART_INFO_TYPE><6628><PIC16F628><16xxxx><0><1><7ff><4><7f><7f><0><0><1>
        # <PART_INFO_TYPE><a829><PIC16LF1829><16Exxx><2><4><1fff><20><7f><ff><0><0><2>
        # <PART_INFO_TYPE><1330><PIC18F1330><18xxxx><6><1><1fff><10><ff><7f><7f><0><c>

      if (lc($fields[2]) eq $Mcu)
        {
        $config_count = hex($fields[12]);
        $switch_count = 0;
        $setting_count = 0;
        $config_mask = $config_word_masks{$fields[3]};
        $config_mask = 0xFFFF if (! defined($config_mask));
        $state = ST_LISTEN;
        $addr = 0;
        %{$Configs} = ();
        }
      else
        {
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
          --$config_count;
          } # when ('CONFIGREG_INFO_TYPE')

        when ('SWITCH_INFO_TYPE')
          {
        # <SWITCH_INFO_TYPE><FOSC><Oscillator Selection bits><7><8>
        # <SWITCH_INFO_TYPE><DEBUG><In-Circuit Debugger Mode bit><2000><2>

          die "Too much the number of \"SWITCH_INFO_TYPE\"!\n" if ($switch_count <= 0);

          $switch_info = {
                         HEAD => $fields[1],
                         BITS => [],
                         MASK => hex($fields[3]),
                         EXPL => (defined($fields[2]) ? $fields[2] : '')
                         };

          $setting_count = hex($fields[4]);
          push(@{$Configs->{$addr}}, $switch_info);
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
          last if (! $setting_count && ! $switch_count && ! $config_count);
          } # when ('SETTING_VALUE_TYPE')
        } # given ($fields[0])
      } # if ($state == ST_LISTEN)
    } # while (<IN>)

  close(IN);
  }

#-------------------------------------------------------------------------------

        # Writes out the $Configs.

sub dump_config($)
  {
  my $Configs = $_[0];
  my @addresses = sort {$a <=> $b} keys(%{$Configs});
  my @sections;
  my $count = @addresses;
  my $str;
  my $len;
  my $i;

  return if (! $count);

  $len = 4;

  if ($count < 2)
    {
        # PIC10F, PIC12, PIC16

    $sections[0] = 'CONFIG';
    $str = align('_CONFIG', AL_NAME);
    Outfl(align("${str}EQU", AL_ADDR) . "H'%0${len}X'", $addresses[0]);
    }
  else
    {
    if ($config_mask == 0x00FF)
      {
        # PIC18
      my $v;
      my $n;
      my $h;

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
          $str = align("_$str", AL_NAME);
          Outfl(align("${str}EQU", AL_ADDR) . "H'%06X'", $v);
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
          $str = align("_$str", AL_NAME);
          Outfl(align("${str}EQU", AL_ADDR) . "H'%06X'", $v);
          }
        }

      $len = 2;
      }
    else
      {
      # PIC12, PIC16

      for ($i = 0; $i < $count; ++$i)
        {
        $str = sprintf "CONFIG%u", $i + 1;
        $sections[$i] = $str;
        $str = align("_$str", AL_NAME);
        Outfl(align("${str}EQU", AL_ADDR) . "H'%04X'", $addresses[$i]);
        }
      }
    }

  for ($i = 0; $i < $count; ++$i)
    {
    Outl("\n;----- $sections[$i] options ------------------------------------------------\n");

    foreach (@{$Configs->{$addresses[$i]}})
      {
      my $head = "_$_->{HEAD}";
      my $mask = ($_->{MASK} ^ $config_mask) & $config_mask;

      Outl("        ; $_->{EXPL}");

      foreach (@{$_->{BITS}})
        {
        $str = align("${head}_$_->{NAME}", 28);
        $str = sprintf(align("${str}EQU", 33) . "H'%0${len}X'", $_->{VALUE} | $mask);

        if ($_->{EXPL} ne '')
          {
          Outl(align($str, 42), "; $_->{EXPL}");
          }
        else
          {
          Outl($str);
          }
        }

      Outl();
      }
    }
  }

#-------------------------------------------------------------------------------

        # Reads a '.inc' or a '.lkr' file from $Source_dir. In the readed
        # file to removes the DOS end of line characters from, after that
        # from it creates a new file.

sub convert_file($$$$)
  {
  my ($Source_dir, $Name, $Tail, $Lkr) = @_;
  my @dir_list;
  my @array;
  my $in_file;
  my $out_file;

  $out_file = ($Lkr) ? "${Name}_g$Tail" : "p$Name$Tail";
  $in_file  = $out_file;
  $out_file .= $out_tail;
  create_dirlist(\@dir_list, $Source_dir, $Tail);

        # This solution eliminates the confusion what inside the file names
        # cause the different small and capital letters.

  $in_file = $1 if (/^($in_file)$/i ~~ @dir_list);

  read_file("$Source_dir/$in_file", \@array);
  open($out_handler, '>', $out_file) || die "convert_file(): Could not create the \"$out_file\" file!\n";
  Log("Create the $out_file file.", 6);

  if ($Lkr || assess_include(\@array))
    {
    Outl(join("\n", @array));
    }
  elsif (! $add_config_bits)
    {
    Outl(join("\n", @array));
    }
  else
    {
        # There are, so inc files (eg p18f25j11.inc) from which is missing in the Configuration Bits section.
        # This section complement such a deficiency.

    die "In $in_file missing the \"Verify Processor\" header." if ($gp_rev_hist_begin < 0 || $gp_rev_hist_begin > $gp_rev_hist_end);

    my %configs;

    delete_last_list(\@array);
    read_config_bits("$Source_dir/$mplabx_dev_info", "pic$Name", \%configs);

    Outl(join("\n", @array[ 0 .. $gp_rev_hist_begin ]));

    Outl("$gp_added_start\n\n$gp_border\n;\n;       $gp_spec_hist\n;\n$gp_border\n");
    Outl(';       ', strftime('%F', gmtime), "  Added entire $conf_sect section.");
    Outl("$gp_added_end\n");

    Out(join("\n", @array[ $gp_rev_hist_end .. $#array ]));

    print $out_handler <<EOT
$gp_added_start\n
; This section is generated automatically by the $PROGRAM.
\n$gp_border\n;\n;       $conf_sect\n;\n$gp_border\n
EOT
;
    dump_config(\%configs);
    Outl($gp_added_end);

    Outl("\n        LIST");
    }

  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # According to the value of $Key sorts the $Px.

sub sort_px_struct($$)
  {
  my ($Px, $Key) = @_;

  given ($Key)
    {
    when (SORT_DEFINED_AS)
      {
        # According to the DEFINED_AS sorts the table.
      @{$Px} = sort { smartSort($a->{DEFINED_AS}, $b->{DEFINED_AS}, FALSE) } @{$Px};
      }

    when (SORT_NAME0)
      {
        # According to the NAMES0 sorts the table.
      @{$Px} = sort { smartSort($a->{NAMES}->[0], $b->{NAMES}->[0], FALSE) } @{$Px};
      }

    when (SORT_NAME1)
      {
        # According to the NAMES1 sorts the table.
      @{$Px} = sort { smartSort($a->{NAMES}->[1], $b->{NAMES}->[1], FALSE) } @{$Px};
      }

    when (SORT_NAME2)
      {
        # According to the NAMES2 sorts the table.
      @{$Px} = sort { smartSort($a->{NAMES}->[2], $b->{NAMES}->[2], FALSE) } @{$Px};
      }

    default
      {
      die "sort_px_struct(): Internal error: Unknown sort key.\n";
      }
    }
  }

#-------------------------------------------------------------------------------

        # One capability: Creates a one-line report and puts into the $Array.
        # Another capability: Prints the whole report from $Array.

sub report
  {
  die "report(): The action code, error type, array and one parameter must be specified." if (@_ < 4);

  my $Action = shift(@_);
  my $Type   = shift(@_);
  my $Array  = shift(@_);

  given ($Type)
    {
    when (E_COFF_COLL)
      {
      given ($Action)
        {
        when (RP_ADD)
          {
          ${$Array} .= sprintf "| 0x%04X    | %-17s | %-17s |\n", @_;
          }

        when (RP_PRINT)
          {
          return if (${$Array} eq '');      # No report.

          my ($Location, $Out) = @_;
          my $border = '+-----------+-------------------+-------------------+';

          print $Out <<EOT
\nThe following coff_type collisions exist in the $Location:\n
$border\n| coff_type |   owner device    |  usurper device   |\n$border\n${$Array}$border
EOT
;
          }
        } # given ($Action)
      } # when (E_COFF_COLL)

    when (E_COFF_DIFF)
      {
      given ($Action)
        {
        when (RP_ADD)
          {
          ${$Array} .= sprintf "| %-17s | 0x%04X | 0x%04X  |\n", @_;
          }

        when (RP_PRINT)
          {
          return if (${$Array} eq '');      # No report.

          my ($Location, $Out) = @_;
          my $border = '+-------------------+--------+---------+';

          print $Out <<EOT
\nThere are differences in the coff_types:\n
$border\n|      device       | mplabx | gputils |\n$border\n${$Array}$border
EOT
;
          }
        } # given ($Action)
      } # when (E_COFF_DIFF)

    when (E_ROM_DIFF)
      {
      my $border = '+----------------------+----------+';

      given ($Action)
        {
        when (RP_ADD)
          {
          ${$Array} .= sprintf "| %-20s | 0x%06X |\n| %-20s | 0x%06X |\n$border\n", @_;
          }

        when (RP_PRINT)
          {
          return if (${$Array} eq '');      # No report.

          my ($Location, $Out) = @_;

          print $Out <<EOT
\nThe 'max_rom' differs in the $Location:\n
$border\n|        place         | max_rom  |\n$border\n${$Array}
EOT
;
          }
        } # given ($Action)
      } # when (E_ROM_DIFF)

    when (E_NAME_COLL)
      {
      given ($Action)
        {
        when (RP_ADD)
          {
          ${$Array} .= sprintf "| %-17s |\n", @_;
          }

        when (RP_PRINT)
          {
          return if (${$Array} eq '');      # No report.

          my ($Location, $Out) = @_;
          my $border = '+-------------------+';

          print $Out <<EOT
\nThe following names repeated in the $Location:\n
$border\n|      device       |\n$border\n${$Array}$border
EOT
;
          }
        } # given ($Action)
      } # when (E_NAME_COLL)

    when (E_LKR_NAME)
      {
      given ($Action)
        {
        when (RP_ADD)
          {
          ${$Array} .= sprintf "| %-17s | %-20s |\n", @_;
          }

        when (RP_PRINT)
          {
          return if (${$Array} eq '');      # No report.

          my ($Location, $Out) = @_;
          my $border = '+-------------------+----------------------+';

          print $Out <<EOT
\nThe following lkr name differ from the device name in the $Location:\n
$border\n|      device       |      lkr script      |\n$border\n${$Array}$border
EOT
;
          }
        } # given ($Action)
      } # when (E_LKR_NAME)
    } # given ($Type)
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@                     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@  Process a lkr file.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@                     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        # Process one line of a lkr file.

sub process_lkr_line($)
  {
  my $Line = $_[0];

  if ($Line =~ /^(\w+)\s+NAME=(\S+)\s+START=(\w+)\s+END=(\w+)/io)
    {
    my $sect  = uc($1);         # uc() <- paranoia
    my $name  = $2;
    my $start = str2dec($3);
    my $end   = str2dec($4);

    if ($sect eq 'CODEPAGE')
      {
      if ($name =~ /page\d*$/io)
        {
        # CODEPAGE   NAME=page       START=0x0               END=0x7FFF
        # CODEPAGE   NAME=page2      START=0x400             END=0x5FF

        push(@lkr_pages, { START => $start, END => $end });
        }
      elsif ($name =~ /(config|cfgmem)$/io)
        {
        # CODEPAGE   NAME=.config    START=0xFFF             END=0xFFF          PROTECTED
        # CODEPAGE   NAME=config     START=0x300000          END=0x30000D       PROTECTED
        # CODEPAGE   NAME=cfgmem     START=0x300000          END=0x30000D       PROTECTED

        $lkr_config_start = $start;
        $lkr_config_end   = $end;
        }
      }
    }
  }

#-------------------------------------------------------------------------------

        # Reads informations from a lkr file.
        # The work was assisted by a very simple preprocessor.

sub read_lkr($)
  {
  open(IN, '<', $_[0]) || die "read_lkr(): Could not open the $_[0] file!\n";

  Log("Reads the $_[0] file.", 4);

  my $name = basename($_[0]);

  reset_preprocessor();

  @lkr_pages = ();
  $lkr_config_start = -1;
  $lkr_config_end   = -1;

  while (<IN>)
    {
    chomp;
    s/\r$//o;

    if ($_ !~ /^\s*$/o)
      {
      s/^\s*|\s*$//go;
      run_preprocessor($name, \&process_lkr_line, $_) if ($_ !~ m|^//|o);
      }

    ++$pp_line_number;
    }

  die "read_lkr(): There is no one program memory page either. ($name)\n" if (! @lkr_pages);
  close(IN);
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@  The mplabx related procedures.  @@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        # Reads characteristics of the $Name MCU from the $mplabx_dev_info.

sub read_mcu_info_from_mplabx($)
  {
  my $Name = $_[0];
  my $path = "$mplabx_inc/$mplabx_dev_info";

  open(IN, '<', $path) || die "read_mcu_info_from_mplabx(): Could not open for reading: $path\n";

  Log("Reads from the $path file.", 4);

  my $proc = 'PIC' . uc($Name);
  my $info = undef;

  while (<IN>)
    {
    if ($_ =~ /^<PART_INFO_TYPE><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)>/io && $2 eq $proc)
      {
      $info = {
              NAME      => $Name,
              CLASS     => str2class($3),
              COFF      => hex($1),             # Coff ID of device. (16 bit wide)
              PAGES     => hex($5),             # Number of ROM/FLASH pages.

                # These addresses relative, compared to the beginning of the blocks.
              ROM       => hex($6),             # Last address of ROM/FLASH.
              FLASHDATA => hex($11),            # Last address of FLASH Data.
              EEPROM    => hex($9),             # Last address of EEPROM.

              CONFIGS   => hex($12),            # Number of Configuration bytes/words.
              BANKS     => hex($7),             # Number of RAM Banks.
              ACCESS    => hex($10)             # Last address of lower Access RAM of pic18f series.
              };

      last;
      }
    }

  close(IN);
  return $info;
  }

#-------------------------------------------------------------------------------

        # Reads characteristics of all MCUs from the $mplabx_dev_info.

sub read_all_mcu_info_from_mplabx()
  {
  my $path = "$mplabx_inc/$mplabx_dev_info";
  my $prev;
  my $name_error = '';

  open(IN, '<', $path) || die "read_all_mcu_info_from_mplabx(): Could not open for reading: $path\n";

  Log("Reads the $path file.", 4);
  @mp_mcus = ();
  %mp_mcus_by_name = ();

  while (<IN>)
    {
        # <PART_INFO_TYPE><e529><PIC12F529T39A><16c5xe><0><3><5ff><8><1f><0><0><3f><1>
        # <PART_INFO_TYPE><1840><PIC12F1840><16Exxx><2><2><fff><20><7f><ff><0><0><2>
        # <PART_INFO_TYPE><a454><PIC16LF1454><16Exxx><2><4><1fff><20><7f><0><0><0><2>
        # <PART_INFO_TYPE><2620><PIC18F2620><18xxxx><6><1><ffff><10><ff><3ff><7f><0><b>
        # <PART_INFO_TYPE><2685><PIC18F2685><18xxxx><6><1><17fff><10><ff><3ff><5f><0><b>

    if ($_ =~ /^<PART_INFO_TYPE><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)>/io)
      {
      my ($coff, $name, $class, $pages, $rom) = (hex($1), lc($2), str2class($3), hex($5), hex($6));
      my ($banks, $eeprom, $split, $fdata, $configs) = (hex($7), hex($9), hex($10), hex($11), hex($12));

      $name =~ s/^pic//o;

        # With these does not deal with.

      next if ($name eq '16c5x'  || $name =~ /^1[6-8]cxx$/o ||
               $name eq 'ap7675' || $name =~ /^eeprom/o ||
               $name =~ /^mcp25/o);

      my $info = {
                 NAME      => $name,
                 CLASS     => $class,
                 COFF      => $coff,            # Coff ID of device. (16 bit wide)
                 PAGES     => $pages,           # Number of ROM/FLASH pages.

                # These addresses relative, compared to the beginning of the blocks.
                 ROM       => $rom,             # Last address of ROM/FLASH.
                 FLASHDATA => $fdata,           # Last address of FLASH Data.
                 EEPROM    => $eeprom,          # Last address of EEPROM.

                 CONFIGS   => $configs,         # Number of Configuration bytes/words.
                 BANKS     => $banks,           # Number of RAM Banks.
                 ACCESS    => $split            # Last address of lower Access RAM of pic18f series.
                 };

      $prev = $mp_mcus_by_name{$name};

      if (defined($prev))
        {
        report(RP_ADD, E_NAME_COLL, \$name_error, $name);
        }
      else
        {
        $mp_mcus_by_name{$name} = $info;
        }

      push(@mp_mcus, $info);
      }
    }

  close(IN);
  report(RP_PRINT, E_NAME_COLL, \$name_error, 'mplabx', *STDERR);
  }

#-------------------------------------------------------------------------------

        # The basis of available data creates a new row to the px structure.

sub new_px_row($$$)
  {
  my ($Error, $Info, $Script) = @_;
  my $name = $Info->{NAME};
  my $rom_end = $Info->{ROM};
  my $lkr_rom_end = $lkr_pages[$#lkr_pages]->{END};

  $Script = "${name}_g.lkr" if (! defined($Script));

  Log('Create a px struct row.', 6);

  if ($rom_end != $lkr_rom_end)
    {
    report(RP_ADD, E_ROM_DIFF, $Error, $Script, $lkr_rom_end, $mplabx_dev_info, $rom_end);
    }

  my $p18j = ($name =~ /^18l?f\d+j\d+/o);
  my $class = $Info->{CLASS};
  my $eeprom_start = $eeprom_starts[$class];
  my $bad_start = -1;
  my $bad_end   = -1;

  if (! $p18j && $eeprom_start > 0 && $Info->{EEPROM} > 0)
    {
    $bad_start = $rom_end + 1;
    $bad_end = $eeprom_start - 1;
    $rom_end = $bad_end + $Info->{EEPROM} + 1;
    }

  my $num_pages = $Info->{PAGES};
  $num_pages = 1 if (! $num_pages);
  my $p16e = ($class == PROC_CLASS_PIC16E);
  my $no_head = ($name =~ /^(mc(p|v)|rf)/o);
  my $px = {
           IGNORED      => FALSE,
           CLASS        => $classnames[$class],
           DEFINED_AS   => '__' . uc($name),
           NAMES        => [
                           ($no_head) ? $name : "pic$name",
                           ($no_head) ? $name : "p$name",
                           $name
                           ],
           COFF_TYPE    => $Info->{COFF},
           NUM_PAGES    => ($p16e) ? 0 : $num_pages,
           NUM_BANKS    => ($p16e) ? ($Info->{ACCESS} + 1) : $Info->{BANKS},
           MAXROM       => $rom_end,
           BADROM       => [ $bad_start, $bad_end ],
           CONFIG_ADDRS => [ $lkr_config_start, $lkr_config_end ],
           SCRIPT       => $Script,
           COMMENT      => ''
           };

  return $px;
  }

#-------------------------------------------------------------------------------

        # Create struct px from the known MCU-s in the mplabx.

sub create_px_struct_from_mplabx()
  {
  my @dir_list;
  my $prev;
  my $coff_error = '';
  my $mem_error = '';

  read_all_mcu_info_from_mplabx();
  create_dirlist(\@dir_list, $mplabx_lkr, '.lkr');

  Log('Creates the @mp_px_struct.', 4);
  @mp_px_struct = ();
  %mp_px_rows_by_name = ();
  %mp_px_rows_by_coff = ();

  foreach my $info (@mp_mcus)
    {
    my ($name, $coff) = ($info->{NAME}, $info->{COFF});
    my $script = "${name}_g.lkr";

        # The Microchip is quite has unfortunately been ported to Linux the mplabx package.
        # It seems that they do not care the fact, that on Linux in the name of files there
        # is difference between the small and capital letters.
        # For example: 16C58a_g.lkr, 16c58b_g.lkr, P16F1823.INC, p16f1824.inc
        # For this reason more difficult may be find a lkr file.

    $script = $1 if (/^($script)$/i ~~ @dir_list);

    read_lkr("$mplabx_lkr/$script");

    my $px = new_px_row(\$mem_error, $info, $script);

    $mp_px_rows_by_name{$name} = $px;

        # Watches the coff_type collisions.

    $prev = $mp_px_rows_by_coff{$coff};

    if (defined($prev))
      {
      report(RP_ADD, E_COFF_COLL, \$coff_error, $coff, $prev->{NAMES}->[0], $name);
      }
    else
      {
      $mp_px_rows_by_coff{$coff} = $px;
      }

    push(@mp_px_struct, $px);
    }

  report(RP_PRINT, E_ROM_DIFF, \$mem_error, 'mplabx', *STDERR);
  report(RP_PRINT, E_COFF_COLL, \$coff_error, 'mplabx', *STDERR);
  sort_px_struct(\@mp_px_struct, SORT_NAME0);
  }

################################################################################
################################################################################

        # Creates a unique coff_type identifier.
=back
sub generate_coff_type()
  {
  for (my $i = 1; $i <= 0xFFFF; ++$i)
    {
    return $i if (! defined($gp_px_rows_by_coff{$i}));
    }

  return -1;
  }
=cut
#-------------------------------------------------------------------------------

        # Reads the content of the list file.

sub read_list_file($)
  {
  open(LIST, '<', $_[0]) || die "read_list_file(): Can not open. -> \"$_[0]\"\n";

  Log("Reads the $_[0] file.", 4);
  %list_file_members = ();

  while (<LIST>)
    {
    chomp;
    s/\r$//o;

        # Empty line.
    next if ($_ =~ /^\s*$/o);

    s/^\s*|\s*$//go;

        # Comment line.
    next if ($_ =~ /^#/o);

    $list_file_members{$_} = TRUE;
    }

  close(LIST);
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@  Handle the gpprocessor.c file.  @@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        # Reads entire content of the gpprocessor.c file.

sub read_gpproc_c_content()
  {
  open(LIB, '<', $gpproc_path) || die "read_gpproc_c_content(): Can not open. -> \"$gpproc_path\"\n";
  Log("Reads the $gpproc_path file.", 4);
  @gpproc_c_content = <LIB>;
  close(LIB);
  chomp(@gpproc_c_content);
  }

#-------------------------------------------------------------------------------

        # Processes the px struct table.

sub extract_px_struct()
  {
  my $n;
  my $in_table;
  my $coff_error = '';
  my $name_error = '';
  my $lkr_error = '';

        # static struct px pics[] = {
        # { PROC_CLASS_EEPROM8, "__EEPROM8",  { "eeprom8",     "eeprom8",   "eeprom8"  }, 0x1fff, 0, 0, 0xff,     { -1, -1 },            { -1, -1 },             NULL                 },
        # { PROC_CLASS_PIC14E,  "__16LF1526", { "pic16lf1526", "p16lf1526", "16lf1526" }, 0xa526, 4, 0x1f, 0x1fff,{     -1,     -1 },    { 0x8007, 0x8008 },     "16lf1526_g.lkr"     },

  Log('Extract the table of px struct.', 4);
  $in_table = FALSE;
  $n = 0;
  $px_struct_begin = -1;
  $px_struct_end = -1;
  @gp_px_struct = ();
  %gp_px_rows_by_name = ();
  %gp_px_rows_by_coff = ();

  foreach (@gpproc_c_content)
    {
    if (! $in_table)
      {
      if ($_ =~ /^\s*static\s+struct\s+px\s+pics\[\s*\]\s*=\s*\{\s*$/io)
        {
        $in_table = TRUE;
        $px_struct_begin = $n;
        }
      }
    elsif ($_ =~ /\{\s*(PROC_CLASS_\w+)\s*,\s*"(\w+)"\s*,\s*\{\s*"(\w+)"\s*,\s*"(\w+)"\s*,\s*"(\S+)"\s*}\s*,\s*([\w-]+)\s*,\s*([\w-]+)\s*,\s*([\w-]+)\s*,\s*(\S+)\s*,\s*\{\s*(\S+)\s*,\s*(\S+)\s*\}\s*,\s*{\s*(\S+)\s*,\s*(\S+)\s*\}\s*,\s*\"?([\.\w]+)\"?\s*\}/io)
      {
      my $name0 = $3;
      my $name2 = $5;
      my $coff = str2dec($6);
      my $script = $14;
      my $prev;
      my $px = {
               IGNORED      => FALSE,
               CLASS        => $1,
               DEFINED_AS   => $2,
               NAMES        => [ $name0, $4, $name2 ],
               COFF_TYPE    => $coff,
               NUM_PAGES    => str2dec($7),
               NUM_BANKS    => str2dec($8),
               MAXROM       => str2dec($9),
               BADROM       => [ str2dec($10), str2dec($11) ],
               CONFIG_ADDRS => [ str2dec($12), str2dec($13) ],
               SCRIPT       => $script,
               COMMENT      => ''
               };

      my $tail = ${^POSTMATCH};         # Maybe there is a comment at the end of the line.

      $tail =~ s/^\s*,?\s*//o;
      $tail =~ s/^\*\/\s*$//o;

      $px->{IGNORED} = TRUE if ($_ =~ /^\s*\/\*/o);
      $px->{COMMENT} = $1 if ($tail =~ /\/\*(.+)\*\/\s*$/o);

        # Watches the name collisions.

      $prev = $gp_px_rows_by_name{$name2};

      if (defined($prev))
        {
        report(RP_ADD, E_NAME_COLL, \$name_error, $name2);
        }
      else
        {
        $gp_px_rows_by_name{$name2} = $px;
        }

        # Does not deal with the generic devices. Otherwise this would cause a false error message.


      if ($name0 ne 'pic16c5x' &&
          $name0 !~ /^pic1[6-8]cxx$/o &&
          $name0 !~ /^eeprom/o)
        {
        # Watches the coff_type collisions.

        $prev = $gp_px_rows_by_coff{$coff};

        if (defined($prev))
          {
          report(RP_ADD, E_COFF_COLL, \$coff_error, $coff, $prev->{NAMES}->[0], $name0);
          }
        else
          {
          $gp_px_rows_by_coff{$coff} = $px;
          }
        }

        # Watches the script name error.

      if ($script ne 'NULL' && $script !~ /^${name2}(_g)?.lkr$/)
        {
        report(RP_ADD, E_LKR_NAME, \$lkr_error, $name0, $script);
        }

      push(@gp_px_struct, $px);
      }
    else
      {
      $px_struct_end = $n;
      last;
      }

    ++$n;
    }

  report(RP_PRINT, E_COFF_COLL, \$coff_error, 'gputils', *STDERR);
  report(RP_PRINT, E_NAME_COLL, \$name_error, 'gputils', *STDERR);
  report(RP_PRINT, E_LKR_NAME, \$lkr_error, 'gputils', *STDERR);
  sort_px_struct(\@gp_px_struct, SORT_DEFINED_AS);
  }

#-------------------------------------------------------------------------------

sub neg_form($)
  {
  sprintf((($_[0] < 0) ? '%8i' : '0x%06X'), $_[0]);
  }

#-------------------------------------------------------------------------------

        # Prints the known MCU-s.

sub list_known_mcu($)
  {
  my $exist_list = scalar(keys(%list_file_members));
  my $i;

  foreach (@{$_[0]})
    {
    if ($extended_list)
      {
      my $name = $_->{NAMES}->[2];

      next if ($exist_list && ! defined($list_file_members{$name}));

      print <<EOT
$border1\n        $_->{NAMES}->[0]
class       : $_->{CLASS}
defined_as  : $_->{DEFINED_AS}
names       : $_->{NAMES}->[0], $_->{NAMES}->[1], $name
EOT
;
      printf "coff_type   : 0x%04X\n", $_->{COFF_TYPE};
      print  "num_pages   : $_->{NUM_PAGES}\n";

      print  'num_banks   : ';
      $i = $_->{NUM_BANKS};
      printf((($i <= 32) ? "%u\n"  : "0x%02X\n"), $i);
      print ('maxrom      : ' . neg_form($_->{MAXROM}) . "\n");
      print ('badrom      : ' . neg_form($_->{BADROM}->[0]) . ', ' . neg_form($_->{BADROM}->[1]) . "\n");
      print ('config_addrs: ' . neg_form($_->{CONFIG_ADDRS}->[0]) . ', ' . neg_form($_->{CONFIG_ADDRS}->[1]) . "\n");
      print  "script      : $_->{SCRIPT}\n";
      }
    else
      {
      print  "$_->{NAMES}->[0]\n";
      }
    }

  print "$border1\n" if ($extended_list);
  }

#-------------------------------------------------------------------------------

        # Deletes the $Name MCU from the @gp_px_struct.

sub delete_mcu($)
  {
  my $Name = $_[0];
  my $v = @gp_px_struct;

  for (my $i = 0; $i < $v; ++$i)
    {
    if ($gp_px_struct[$i]->{NAMES}->[2] eq $Name)
      {
      splice(@gp_px_struct, $i, 1);
      return;
      }
    }

  print STDERR "This MCU: $Name not exist on the gputils.\n";
  }

#-------------------------------------------------------------------------------

sub create_one_px_row($$)
  {
  my ($Row, $Full) = @_;
  my ($i, $line);

  Log('Shows a px struct row.', 6);

  if ($Full)
    {
    $line = sprintf("{ %-20s, %-16s, { %-17s, %-17s, %-17s }, 0x%04X, %2u, ",
                    $Row->{CLASS},
                    "\"$Row->{DEFINED_AS}\"",
                    "\"$Row->{NAMES}->[0]\"", "\"$Row->{NAMES}->[1]\"", "\"$Row->{NAMES}->[2]\"",
                    $Row->{COFF_TYPE}, $Row->{NUM_PAGES});
    }
  else
    {
    $line = sprintf("{ %-20s, %-16s, {...}, 0x%04X, %2u, ",
                    $Row->{CLASS},
                    "\"$Row->{DEFINED_AS}\"",
                    $Row->{COFF_TYPE}, $Row->{NUM_PAGES});
    }

  $i = $Row->{NUM_BANKS};
  $line .= sprintf((($i <= 32) ? '%4u, ' : '0x%02X, '), $i);

  $line .= neg_form($Row->{MAXROM});
  $line .= ', ';

  $line .= '{ ';
  $line .= neg_form($Row->{BADROM}->[0]);
  $line .= ', ';
  $line .= neg_form($Row->{BADROM}->[1]);
  $line .= ' }, ';

  $line .= '{ ';
  $line .= neg_form($Row->{CONFIG_ADDRS}->[0]);
  $line .= ', ';
  $line .= neg_form($Row->{CONFIG_ADDRS}->[1]);
  $line .= ' }, ';

  $i = $Row->{SCRIPT};
  $line .= sprintf('%-20s },', (($i ne 'NULL') ? "\"$i\"" : $i));

  $line = "/*$line*/" if ($Row->{IGNORED});

  $line .= " /*$_->{COMMENT}*/" if ($Row->{COMMENT} ne '');
  return $line;
  }

#-------------------------------------------------------------------------------

        # Creates the "$gpprocessor_c.gen" file.

sub create_gpprocessor_c()
  {
  my $name = "$gpprocessor_c$out_tail";

  open($out_handler, '>', $name) || die "create_gpprocessor_c(): Could not create the \"$name\" file!\n";

        # This the part of the gpprocessor.c file before the table.

  Outl(join("\n", @gpproc_c_content[ 0 .. $px_struct_begin ]));

        # This the px struct table.

  foreach (@gp_px_struct)
    {
    Outl('  ', create_one_px_row($_, TRUE));
    }

        # This the part of the gpprocessor.c file after the table.

  Outl(join("\n", @gpproc_c_content[ $px_struct_end .. $#gpproc_c_content ]));
  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # Performs one deletion.

sub remove_mcu($)
  {
  my $Name = $_[0];

  die "remove_mcu(): Miss the name of MCU!\n" if ($Name eq '');
  die "remove_mcu(): This name is wrong: \"$Name\"\n" if ($Name !~ /^(p(ic)?)?$name_filter$/io);

  $Name =~ s/^p(ic)?//io;
  $Name = lc($Name);
  delete_mcu($Name);
  Log("The $Name MCU removed of gputils.", 2);
  }

#-------------------------------------------------------------------------------

        # Performs more deletion from a list.

sub remove_more_mcu()
  {
  return if (! scalar(keys(%list_file_members)));

  foreach (sort { smartSort($a, $b, FALSE) } keys(%list_file_members))
    {
    remove_mcu($_);
    }
  }

#-------------------------------------------------------------------------------

        # Fabricate .inc file of the $Name MCU.

sub fabricate_inc($)
  {
  my $Name = $_[0];

  die "fabricate_inc(): Miss the name of MCU!\n" if ($Name eq '');
  die "fabricate_inc(): This name is wrong: \"$Name\"\n" if ($Name !~ /^(p(ic)?)?$name_filter$/io);

  $Name =~ s/^p(ic)?//io;
  Log("Fabricate the p${Name}.inc file.", 2);
  convert_file($mplabx_inc, lc($Name), '.inc', FALSE);
  }

#-------------------------------------------------------------------------------

        # Fabricate more .inc file of MCUs from a list.

sub fabricate_more_inc()
  {
  return if (! scalar(keys(%list_file_members)));

  foreach (sort { smartSort($a, $b, FALSE) } keys(%list_file_members))
    {
    fabricate_inc($_);
    }
  }

#-------------------------------------------------------------------------------

        # Fabricate .lkr file of the $Name MCU.

sub fabricate_lkr($)
  {
  my $Name = $_[0];

  die "fabricate_lkr(): Miss the name of MCU!\n" if ($Name eq '');
  die "fabricate_lkr(): This name is wrong: \"$Name\"\n" if ($Name !~ /^(p(ic)?)?$name_filter$/io);

  $Name =~ s/^p(ic)?//io;
  Log("Fabricate the ${Name}_g.lkr file.", 2);
  convert_file($mplabx_lkr, lc($Name), '.lkr', TRUE);
  }

#-------------------------------------------------------------------------------

        # Fabricate more .lkr file of MCUs from a list.

sub fabricate_more_lkr()
  {
  return if (! scalar(keys(%list_file_members)));

  foreach (sort { smartSort($a, $b, FALSE) } keys(%list_file_members))
    {
    fabricate_lkr($_);
    }
  }

#-------------------------------------------------------------------------------

        # This the 1th helper of addition.

sub addition_helper1($)
  {
  my $Name = $_[0];

  die "addition_helper1(): Miss the name of MCU!\n" if ($Name eq '');
  die "addition_helper1(): This name is wrong: \"$Name\"\n" if ($Name !~ /^(p(ic)?)?$name_filter$/io);

  $Name =~ s/^p(ic)?//io;
  $Name = lc($Name);

  if ($examine_exist_device && defined($gp_px_rows_by_name{$Name}))
    {
    print STDERR "This MCU: $Name already exist on the gputils.\n";
    return undef;
    }

  read_lkr("$mplabx_lkr/${Name}_g.lkr");
  return $Name;
  }

#-------------------------------------------------------------------------------

        # This the 2th helper of addition.

sub addition_helper2($$$$)
  {
  my ($Coff_error, $Mem_error, $Name, $Info) = @_;

  die "addition_helper2(): This device: $Name not exist in $mplabx_dev_info!\n" if (! defined($Info));

  my $coff = $Info->{COFF};
  my $px   = $gp_px_rows_by_coff{$coff};

  report(RP_ADD, E_COFF_COLL, $Coff_error, $coff, $px->{NAMES}->[0], "pic$Name") if (defined($px));

  $px = new_px_row($Mem_error, $Info, undef);
  push(@gp_px_struct, $px);
  $gp_px_rows_by_name{$Name} = $px;
  $gp_px_rows_by_coff{$coff} = $px;
  Log("The $Name MCU added to gputils.", 2);

  convert_file($mplabx_inc, $Name, '.inc', FALSE);
  convert_file($mplabx_lkr, $Name, '.lkr', TRUE);
  }

#-------------------------------------------------------------------------------

        # Performs one addition.

sub add_mcu($)
  {
  my $Name = $_[0];
  my $coff_error = '';
  my $mem_error = '';

  $Name = addition_helper1($Name);
  return if (! defined($Name));

  addition_helper2(\$coff_error, \$mem_error, $Name, read_mcu_info_from_mplabx($Name));

  report(RP_PRINT, E_COFF_COLL, \$coff_error, 'gputils', *STDERR);
  report(RP_PRINT, E_ROM_DIFF, \$mem_error, 'mplabx', *STDERR);
  sort_px_struct(\@gp_px_struct, SORT_DEFINED_AS);
  }

#-------------------------------------------------------------------------------

        # Performs more addition from a list.

sub add_more_mcu()
  {
  return if (! scalar(keys(%list_file_members)));

  my $coff_error = '';
  my $mem_error = '';

  read_all_mcu_info_from_mplabx();

  foreach my $name (sort { smartSort($a, $b, FALSE) } keys(%list_file_members))
    {
    $name = addition_helper1($name);

    return if (! defined($name));

    addition_helper2(\$coff_error, \$mem_error, $name, $mp_mcus_by_name{$name});
    }

  report(RP_PRINT, E_COFF_COLL, \$coff_error, 'gputils', *STDERR);
  report(RP_PRINT, E_ROM_DIFF, \$mem_error, 'mplabx', *STDERR);
  sort_px_struct(\@gp_px_struct, SORT_DEFINED_AS);
  }

#-------------------------------------------------------------------------------

        # Create the p16f1xxx.inc file.

sub create_p16f1xxx_inc()
  {
  my $name = "$p16f1xxx_inc$out_tail";

  open($out_handler, '>', $name) || die "create_p16f1xxx_inc(): Could not create the \"$name\" file!\n";

  Log("Create the $name file.", 6);

  print $out_handler <<EOT
;$border0\n;\n; p16f1xxx.inc - Common include file to PIC16XXXX enhanced families.
;\n; This file is has been generated using $PROGRAM${time_str}.
;\n;$border0\n\nifdef __16F1xxx\n  include <p16f1937.inc>\nendif
EOT
;

  foreach (@gp_px_struct)
    {
    my $name = $_->{DEFINED_AS};

    next if ($name !~ /^__1[26]L?F\d{4}/o);

    Outl("ifdef $name\n  include <$_->{NAMES}->[1].inc>\nendif");
    }

  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # Create the p18cxxx.inc file.

sub create_p18cxxx_inc()
  {
  my $name = "$p18cxxx_inc$out_tail";

  open($out_handler, '>', $name) || die "create_p18cxxx_inc(): Could not create the \"$name\" file!\n";

  Log("Create the $name file.", 6);

  print $out_handler <<EOT
;$border0\n;\n; p18cxxx.inc - Common include file to PIC18XXXX families.
;\n; This file is has been generated using $PROGRAM${time_str}.
;\n;$border0\n
EOT
;
  foreach (@gp_px_struct)
    {
    my $name = $_->{DEFINED_AS};

    next if ($name !~ /^__18/o);

    my $inc = $_->{NAMES}->[1];

    $inc = 'p18c452' if ($name eq '__18CXX');

    Outl("ifdef $name\n  include <$inc.inc>\nendif");
    }

  close($out_handler);
  }

#-------------------------------------------------------------------------------

        # The devices have a binary identification number. It happens that
        # the number is different in the mplabx and in the gputils.
        # This procedure shows these differences (if any).
        # Only those device deals which exist both (mplabx and gputils) in package.

sub show_ctype_differences()
  {
  my $diffs = '';

  read_all_mcu_info_from_mplabx();
  Log('Shows the differences of coff_types.', 6);

  foreach (@gp_px_struct)
    {
    my $info = $mp_mcus_by_name{$_->{NAMES}->[2]};

    next if (! defined($info) || $info->{COFF} == $_->{COFF_TYPE});

    report(RP_ADD, E_COFF_DIFF, \$diffs, $_->{NAMES}->[0], $info->{COFF}, $_->{COFF_TYPE});
    }

  if ($diffs ne '')
    {
    report(RP_PRINT, E_COFF_DIFF, \$diffs, undef, *STDOUT);
    }
  else
    {
    print "No differences in the coff_types.\n";
    }
  }

#-------------------------------------------------------------------------------

        # Shows differences of the px struct, between the mplabx and the gputils.
        # The starting point of the gpprocessor.c file. In this in turn
        # the px structure. From the data of mplabx, temporarily creates
        # a similar structure. The elements of the two structures it compares.
        # Only those MCU deals which exist both (mplabx and gputils) in package.

sub show_diff_px_struct()
  {
  my $border = '-' x 156;
  my $out = '';

  create_px_struct_from_mplabx();
  Log('Shows the differences between the mplabx and the gputils.', 6);

  foreach (@gp_px_struct)
    {
    my $name = $_->{NAMES}->[2];         # Based on the element of gputils ...
    my $mp = $mp_px_rows_by_name{$name}; # looking at the appropriate mplabx item.

    next if (! defined($mp));           # There is no such element.

        # These important values.

    if ($_->{COFF_TYPE}         != $mp->{COFF_TYPE} ||
        $_->{NUM_PAGES}         != $mp->{NUM_PAGES} ||
        $_->{NUM_BANKS}         != $mp->{NUM_BANKS} ||
        $_->{MAXROM}            != $mp->{MAXROM} ||
        $_->{BADROM}->[0]       != $mp->{BADROM}->[0] ||
        $_->{BADROM}->[1]       != $mp->{BADROM}->[1] ||
        $_->{CONFIG_ADDRS}->[0] != $mp->{CONFIG_ADDRS}->[0] ||
        $_->{CONFIG_ADDRS}->[1] != $mp->{CONFIG_ADDRS}->[1])
      {
      $out .= "$border\n";
      $out .= 'mplabx  ' . create_one_px_row($mp, FALSE) . "\n";
      $out .= 'gputils ' . create_one_px_row($_, FALSE)  . "\n";
      }
    }

  print (($out ne '') ? "$out$border\n" : "No differences.\n");
  }

#-------------------------------------------------------------------------------

sub usage()
  {
  print <<EOT
Usage: $PROGRAM [options]

    Options are:

        -gp <path> or --gputils-path <path>

            The program on this path looks for the gputils.

        -mp <path> or --mplabx-path <path>

            The program on this path looks for the mplabx.

        -p <p12f1822> or --processor <p12f1822>

            The name of MCU. The prefix of name can be: 'pic', 'p' or nothing

        -l <file> or --list-file <file>

            The name of the MCUs, reads from the file. In this way
            at the same time, may be given more MCUs to the gputils.
            (The file construction of is very simple: One name per line. The names
             of processors in a form to be described as the '-p' switch is case.)

        -a or --add

            Adds the MCU to the gputils.
            If the program received a list file, then each member adds.
            (In this case the '-p' switch does not matter.)
            See: '-ac' option.

        -r or --remove

            Removes the MCU from the gputils.
            If the program received a list file, then each member removes.
            (In this case the '-p' switch does not matter.)

        -fi or --fabricate-inc

            Fabricate the .inc files, but only those. See: -p <p12f1822> or -l <file>
            This is especially useful together with the '-ac' option then
            if the $conf_sect section missing from the inc file. (eg: p18f25j11.inc)

        -fl or --fabricate-lkr

            Fabricate the .lkr files, but only those. See: -p <p12f1822> or -l <file>

        --regenerate

            Regenerates the px struct in the gpprocessor.c file. This means
            that sorts the table rows, uniformly formatted the whole table and
            make a gpprocessor.c.gen file.

        -os or --only-survey

            Only the surveying section run. Surveys the errors in databases
            of mplabx and gputils.

        -lg or --list-gputils

            Lists all MCU is what gputils know.

        -lm or --list-mplabx

            Lists all MCU is what mplabx know.

        -dc or --show-diff-coff-types

            Shows differences of the coff_type, between the mplabx and gputils.

        -dp or --show-diff-px-struct

            Shows differences of the px struct, between the mplabx and gputils.
            (The case of mplabx, this structure only exists in theory.)
            (Only those MCU deals which exist both in package.)

        -ac or --add-config-bits

            Adds the missing $conf_sect section. See: '-a' or '-fi' options.

        -el or --extended-list

            Shows a detailed list from all MCU. (The '-lm' and the '-lg' options relates.)
            If the program received a list file, the it works like a filter.

        -ne or --no-examine-exist

            Does not examine the existence of the MCU, what you want to add.
            This switch, only to test recommended to used.

        -t or --timestamp

            There will be timestamp in the header files. (default: no)

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
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@  The main program.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

$PROGRAM = basename($0);
$list_file = '';
%list_file_members = ();
$operation = OP_NULL;
$mcu = '';

for (my $i = 0; $i < @ARGV; )
  {
  my $opt = $ARGV[$i++];

  given ($opt)
    {
    when (/^-(gp|-gputils-path)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $gputils_path = $ARGV[$i++];
      }

    when (/^-(mp|-mplabx-path)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $mplabx_path = $ARGV[$i++];
      }

    when (/^-(p|-processor)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $mcu = $ARGV[$i++];
      }

    when (/^-(l|-list-file)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $list_file = $ARGV[$i++];
      }

    when (/^-(a|-add)$/o)                   { $operation = OP_ADD; }
    when (/^-(r|-remove)$/o)                { $operation = OP_REMOVE; }
    when (/^-(fi|-fabricate-inc)$/o)        { $operation = OP_FABR_INC; }
    when (/^-(fl|-fabricate-lkr)$/o)        { $operation = OP_FABR_LKR; }
    when ('--regenerate')                   { $operation = OP_REGENERATE; }
    when (/^-(os|-only-survey)$/o)          { $operation = OP_SURVEY; }
    when (/^-(lg|-list-gputils)$/o)         { $operation = OP_LIST_GP; }
    when (/^-(lm|-list-mplabx)$/o)          { $operation = OP_LIST_MP; }
    when (/^-(dc|-show-diff-coff-types)$/o) { $operation = OP_SHOWS_CDIFF; }
    when (/^-(dp|-show-diff-px-struct)$/o)  { $operation = OP_SHOWS_PXDIFF; }
    when (/^-(ac|-add-config-bits)$/o)      { $add_config_bits = TRUE; }
    when (/^-(el|-extended-list)$/o)        { $extended_list = TRUE; }
    when (/^-(ne|-no-examine-exist)$/o)     { $examine_exist_device = FALSE; }
    when (/^-(t|-timestamp)$/o)             { $timestamp = TRUE; }

    when (/^-(v|-verbose)$/o)
      {
      die "This option \"$opt\" requires a parameter.\n" if ($i > $#ARGV);

      $verbose = int($ARGV[$i++]);
      $verbose = 0 if (! defined($verbose) || $verbose < 0);
      $verbose = 10 if ($verbose > 10);
      }

    when (/^-(h|-help)$/)
      {
      usage();
      exit(0);
      }

    default
      {
      die "This option \"$opt\" is unknown.\n";
      }
    } # given ($opt)
  } # for (my $i = 0; $i < @ARGV; )

if ($operation == OP_NULL)
  {
  usage();
  exit(0);
  }

$time_str = strftime(', %F %T UTC', gmtime) if ($timestamp);

$gpproc_path = "$gputils_path/libgputils/$gpprocessor_c";
$gputils_inc = "$gputils_path/header";
$gputils_lkr = "$gputils_path/lkr";
$mplabx_inc  = "$mplabx_path/mpasmx";
$mplabx_lkr  = "$mplabx_path/mpasmx/LKR";

if ($operation == OP_SURVEY)
  {
  create_px_struct_from_mplabx();
  read_gpproc_c_content();
  extract_px_struct();
  exit(0);
  }

read_list_file($list_file) if ($list_file ne '');

if ($operation == OP_LIST_MP)
  {
  create_px_struct_from_mplabx();
  list_known_mcu(\@mp_px_struct);
  exit(0);
  }

read_gpproc_c_content();
extract_px_struct();

given ($operation)
  {
  when (OP_REGENERATE)
    {
    create_gpprocessor_c();
    exit(0);
    }

  when (OP_LIST_GP)
    {
    sort_px_struct(\@gp_px_struct, SORT_NAME0);
    list_known_mcu(\@gp_px_struct);
    exit(0);
    }

  when (OP_SHOWS_CDIFF)
    {
    show_ctype_differences();
    exit(0);
    }

  when (OP_SHOWS_PXDIFF)
    {
    show_diff_px_struct();
    exit(0);
    }
  }

if (scalar(keys(%list_file_members)))
  {
  given ($operation)
    {
    when (OP_ADD)
      {
      add_more_mcu();
      }

    when (OP_FABR_INC)
      {
      fabricate_more_inc();
      exit(0);
      }

    when (OP_FABR_LKR)
      {
      fabricate_more_lkr();
      exit(0);
      }

    when (OP_REMOVE)
      {
      remove_more_mcu();
      }
    }
  }
elsif ($mcu ne '')
  {
  given ($operation)
    {
    when (OP_ADD)
      {
      add_mcu($mcu);
      }

    when (OP_FABR_INC)
      {
      fabricate_inc($mcu);
      exit(0);
      }

    when (OP_FABR_LKR)
      {
      fabricate_lkr($mcu);
      exit(0);
      }

    when (OP_REMOVE)
      {
      remove_mcu($mcu);
      }
    }
  }
else
  {
  exit(0);
  }

create_gpprocessor_c();
create_p16f1xxx_inc();
create_p18cxxx_inc();

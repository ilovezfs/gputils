#!/usr/bin/perl -w

=back

   Copyright (C) 2012-2016 Molnar Karoly <molnarkaroly@users.sf.net>

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
no if $] >= 5.018, warnings => "experimental::smartmatch";        # perl 5.16
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
use constant OP_SHOWS_SPDIFF => 9;
use constant OP_SHOWS_CDIFF  => 10;
use constant OP_SHOWS_PXDIFF => 11;

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
use constant E_INC_NAME  => 4;
use constant E_LKR_NAME  => 5;

my $PROGRAM = 'device-manager.pl';

my $verbose = 0;

my $border0 = ('-' x 70);
my $border1 = ('=' x 60);

use constant PIC_BANK_NUMBER_MAX => 32;

use constant PROC_CLASS_EEPROM8  =>  0;
use constant PROC_CLASS_EEPROM16 =>  1;
use constant PROC_CLASS_GENERIC  =>  2;
use constant PROC_CLASS_PIC12    =>  3;
use constant PROC_CLASS_PIC12E   =>  4;
use constant PROC_CLASS_PIC14    =>  5;
use constant PROC_CLASS_PIC14E   =>  6;
use constant PROC_CLASS_PIC14EX  =>  7;
use constant PROC_CLASS_PIC16    =>  8;
use constant PROC_CLASS_PIC16E   =>  9;
use constant PROC_CLASS_SX       => 10;

use constant PIC12_BANK_SHIFT => 5;
use constant PIC12_BANK_SIZE  => 2 ** PIC12_BANK_SHIFT;
use constant PIC14_BANK_SHIFT => 7;
use constant PIC14_BANK_SIZE  => 2 ** PIC14_BANK_SHIFT;
use constant PIC16_BANK_SHIFT => 8;
use constant PIC16_BANK_SIZE  => 2 ** PIC16_BANK_SHIFT;

my %class_features_eeprom8 =
  (
  CLASS         => PROC_CLASS_EEPROM8,
  ENHANCED      => FALSE,
  PAGE_SIZE     => -1,
  WORD_SIZE     => 8,
  CONF_SIZE     => -1,
  CONF_MASK     => -1,
  EE_START      => -1,
  BANK_MAX      => -1,
  BANK_SIZE     => -1,
  BANK_MASK     => -1,
  BANK_SHIFT    => -1,
  IDLOCS_MASK   => 0,
  ACC_SPLIT_MIN => -1,
  ACC_SPLIT_MAX => -1
  );

my %class_features_eeprom16 =
  (
  CLASS         => PROC_CLASS_EEPROM16,
  ENHANCED      => FALSE,
  PAGE_SIZE     => -1,
  WORD_SIZE     => 16,
  CONF_SIZE     => -1,
  CONF_MASK     => -1,
  EE_START      => -1,
  BANK_MAX      => -1,
  BANK_SIZE     => -1,
  BANK_MASK     => -1,
  BANK_SHIFT    => -1,
  IDLOCS_MASK   => 0,
  ACC_SPLIT_MIN => -1,
  ACC_SPLIT_MAX => -1
  );

my %class_features_generic =
  (
  CLASS         => PROC_CLASS_GENERIC,
  ENHANCED      => FALSE,
  PAGE_SIZE     => 512,
  WORD_SIZE     => 12,
  CONF_SIZE     => 12,
  CONF_MASK     => 0x0FFF,
  EE_START      => -1,
  BANK_MAX      => 4,
  BANK_SIZE     => PIC12_BANK_SIZE,
  BANK_MASK     => ~(PIC12_BANK_SIZE - 1),
  BANK_SHIFT    => PIC12_BANK_SHIFT,
  IDLOCS_MASK   => 0x0FF0,
  ACC_SPLIT_MIN => -1,
  ACC_SPLIT_MAX => -1
  );

my %class_features_p12 =
  (
  CLASS         => PROC_CLASS_PIC12,
  ENHANCED      => FALSE,
  PAGE_SIZE     => 512,
  WORD_SIZE     => 12,
  CONF_SIZE     => 12,
  CONF_MASK     => 0x0FFF,
  EE_START      => -1,
  BANK_MAX      => 8,
  BANK_SIZE     => PIC12_BANK_SIZE,
  BANK_MASK     => ~(PIC12_BANK_SIZE - 1),
  BANK_SHIFT    => PIC12_BANK_SHIFT,
  IDLOCS_MASK   => 0x0FF0,
  ACC_SPLIT_MIN => -1,
  ACC_SPLIT_MAX => -1
  );

my %class_features_p12e =
  (
  CLASS         => PROC_CLASS_PIC12E,
  ENHANCED      => TRUE,
  PAGE_SIZE     => 512,
  WORD_SIZE     => 12,
  CONF_SIZE     => 12,
  CONF_MASK     => 0x0FFF,
  EE_START      => -1,
  BANK_MAX      => 8,
  BANK_SIZE     => PIC12_BANK_SIZE,
  BANK_MASK     => ~(PIC12_BANK_SIZE - 1),
  BANK_SHIFT    => PIC12_BANK_SHIFT,
  IDLOCS_MASK   => 0x0FF0,
  ACC_SPLIT_MIN => -1,
  ACC_SPLIT_MAX => -1
  );

my %class_features_p14 =
  (
  CLASS         => PROC_CLASS_PIC14,
  ENHANCED      => FALSE,
  PAGE_SIZE     => 2048,
  WORD_SIZE     => 14,
  CONF_SIZE     => 14,
  CONF_MASK     => 0x3FFF,
  EE_START      => 0x2100,
  BANK_MAX      => 4,
  BANK_SIZE     => PIC14_BANK_SIZE,
  BANK_MASK     => ~(PIC14_BANK_SIZE - 1),
  BANK_SHIFT    => PIC14_BANK_SHIFT,
  IDLOCS_MASK   => 0x3F80,
  ACC_SPLIT_MIN => -1,
  ACC_SPLIT_MAX => -1
  );

my %class_features_p14e =
  (
  CLASS         => PROC_CLASS_PIC14E,
  ENHANCED      => TRUE,
  PAGE_SIZE     => 2048,
  WORD_SIZE     => 14,
  CONF_SIZE     => 16,
  CONF_MASK     => 0xFFFF,
  EE_START      => 0xF000,
  BANK_MAX      => 32,
  BANK_SIZE     => PIC14_BANK_SIZE,
  BANK_MASK     => ~(PIC14_BANK_SIZE - 1),
  BANK_SHIFT    => PIC14_BANK_SHIFT,
  IDLOCS_MASK   => 0x3F80,
  ACC_SPLIT_MIN => -1,
  ACC_SPLIT_MAX => -1
  );

my %class_features_p14ex =
  (
  CLASS         => PROC_CLASS_PIC14EX,
  ENHANCED      => TRUE,
  PAGE_SIZE     => 2048,
  WORD_SIZE     => 14,
  CONF_SIZE     => 16,
  CONF_MASK     => 0xFFFF,
  EE_START      => 0xF000,
  BANK_MAX      => 32,
  BANK_SIZE     => PIC14_BANK_SIZE,
  BANK_MASK     => ~(PIC14_BANK_SIZE - 1),
  BANK_SHIFT    => PIC14_BANK_SHIFT,
  IDLOCS_MASK   => 0x3F80,
  ACC_SPLIT_MIN => -1,
  ACC_SPLIT_MAX => -1
  );

my %class_features_p16 =
  (
  CLASS         => PROC_CLASS_PIC16,
  ENHANCED      => FALSE,
  PAGE_SIZE     => -1,
  WORD_SIZE     => 16,
  CONF_SIZE     => 8,
  CONF_MASK     => 0x00FF,
  EE_START      => -1,
  BANK_MAX      => 9,
  BANK_SIZE     => PIC16_BANK_SIZE,
  BANK_MASK     => ~(PIC16_BANK_SIZE - 1),
  BANK_SHIFT    => PIC16_BANK_SHIFT,
  IDLOCS_MASK   => 0x00,
  ACC_SPLIT_MIN => -1,
  ACC_SPLIT_MAX => -1
  );

my %class_features_p16e =
  (
  CLASS         => PROC_CLASS_PIC16E,
  ENHANCED      => TRUE,
  PAGE_SIZE     => -1,
  WORD_SIZE     => 16,
  CONF_SIZE     => 8,
  CONF_MASK     => 0x00FF,
  EE_START      => 0xF00000,
  BANK_MAX      => 16,
  BANK_SIZE     => PIC16_BANK_SIZE,
  BANK_MASK     => ~(PIC16_BANK_SIZE - 1),
  BANK_SHIFT    => PIC16_BANK_SHIFT,
  IDLOCS_MASK   => 0x00,
  ACC_SPLIT_MIN => 0x5F,
  ACC_SPLIT_MAX => 0x7F
  );

my %class_features_sx =
  (
  CLASS         => PROC_CLASS_SX,
  ENHANCED      => FALSE,
  PAGE_SIZE     => 512,
  WORD_SIZE     => 12,
  CONF_SIZE     => 12,
  CONF_MASK     => 0x0FFF,
  EE_START      => -1,
  BANK_MAX      => -1,
  BANK_SIZE     => PIC12_BANK_SIZE,
  BANK_MASK     => ~(PIC12_BANK_SIZE - 1),
  BANK_SHIFT    => PIC12_BANK_SHIFT,
  IDLOCS_MASK   => 0x3F80,
  ACC_SPLIT_MIN => -1,
  ACC_SPLIT_MAX => -1
  );

my @classnames =
  (
  'PROC_CLASS_EEPROM8',
  'PROC_CLASS_EEPROM16',
  'PROC_CLASS_GENERIC',
  'PROC_CLASS_PIC12',
  'PROC_CLASS_PIC12E',
  'PROC_CLASS_PIC14',
  'PROC_CLASS_PIC14E',
  'PROC_CLASS_PIC14EX',
  'PROC_CLASS_PIC16',
  'PROC_CLASS_PIC16E',
  'PROC_CLASS_SX'
  );

my @class_features_list =
  (
  \%class_features_eeprom8,     # PROC_CLASS_EEPROM8
  \%class_features_eeprom16,    # PROC_CLASS_EEPROM16
  \%class_features_generic,     # PROC_CLASS_GENERIC
  \%class_features_p12,         # PROC_CLASS_PIC12
  \%class_features_p12e,        # PROC_CLASS_PIC12E
  \%class_features_p14,         # PROC_CLASS_PIC14
  \%class_features_p14e,        # PROC_CLASS_PIC14E
  \%class_features_p14ex,       # PROC_CLASS_PIC14EX
  \%class_features_p16,         # PROC_CLASS_PIC16
  \%class_features_p16e,        # PROC_CLASS_PIC16E
  \%class_features_sx           # PROC_CLASS_SX
  );

my %class_features_by_gputils =
  (
  'PROC_CLASS_EEPROM8'  => \%class_features_eeprom8,
  'PROC_CLASS_EEPROM16' => \%class_features_eeprom16,
  'PROC_CLASS_GENERIC'  => \%class_features_generic,
  'PROC_CLASS_PIC12'    => \%class_features_p12,
  'PROC_CLASS_PIC12E'   => \%class_features_p12e,
  'PROC_CLASS_PIC14'    => \%class_features_p14,
  'PROC_CLASS_PIC14E'   => \%class_features_p14e,
  'PROC_CLASS_PIC14EX'  => \%class_features_p14ex,
  'PROC_CLASS_PIC16'    => \%class_features_p16,
  'PROC_CLASS_PIC16E'   => \%class_features_p16e,
  'PROC_CLASS_SX'       => \%class_features_sx
  );

my %class_features_by_mpasmx =
  (
  'EEPROM8'  => \%class_features_eeprom8,
  'EEPROM16' => \%class_features_eeprom16,
  'generic'  => \%class_features_generic,
  '16c5x'    => \%class_features_p12,
  '16c5ie'   => \%class_features_p12e,
  '16c5xe'   => \%class_features_p12e,
  '16xxxx'   => \%class_features_p14,
  '16Exxx'   => \%class_features_p14e,
  '16EXxx'   => \%class_features_p14ex,
  '17xxxx'   => \%class_features_p16,
  '18xxxx'   => \%class_features_p16e,
  'sx'       => \%class_features_sx
  );

my %lost_devices =
  (
  '16c52'    => \%class_features_p12,
  '16c54b'   => \%class_features_p12,
  '16c61'    => \%class_features_p14,
  '16c62'    => \%class_features_p14,
  '16c64'    => \%class_features_p14,
  '16c65'    => \%class_features_p14,
  '16c73'    => \%class_features_p14,
  '16c74'    => \%class_features_p14,
  '16c84'    => \%class_features_p14,
  '16cr54b'  => \%class_features_p12,
  '16f1458'  => \%class_features_p14e,
  '16lf1458' => \%class_features_p14e,
  '17cr43'   => \%class_features_p16,
  'hcs1365'  => \%class_features_p12,
  'hcs1370'  => \%class_features_p12
  );

# From the gputils/libgputils/gpprocessor.h header.
use constant PIC16E_FLAG_HAVE_EXTINST => (1 << 0);
use constant PIC16E_FLAG_J_SUBFAMILY  => (1 << 1);

my $name_filter = qr/10l?f\d+[a-z]*|1[26]((c(e|r)?)|hv)\d+[a-z]*|17c\d+[a-z]*|1[268]l?f\d+([a-z]*|[a-z]+\d+[a-z]*)/i;

my $gputils_path     = "$ENV{HOME}/svn_snapshots/gputils/gputils";
my $mplabx_root_path = '/opt/microchip/mplabx';
my $mplabx_path      = '';
my $mplabx_dev_info  = '8bit_device.info';

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
my $pp_level = 0;               # Shows the lowest level.
my $pp_line_number;             # Line number of a lkr file.

################################################################################
################################################################################

my @gpproc_c_content;           # Full content of gpprocessor.c file.
my $px_struct_begin;            # The begin of the px structure in the gpprocessor.c file.
my $px_struct_end;              # The end of the px structure in the gpprocessor.c file.

#-----------------------------------------------

=back
        The structure of one element of the @gp_px_struct and @mp_px_struct arrays:

        {
        IGNORED      => FALSE,          True if line is commented.
        CLASS        => '',
        DEFINED_AS   => '',
        NAMES        => ['', '', ''],
        COFF_TYPE    => 0,
        NUM_PAGES    => 0,
        NUM_BANKS    => 0,
        BANK_BITS    => 0,
        COMMON_RAM   => [0, 0],
        COMMON_MAX   => 0,
        LINEAR_RAM   => [0, 0],
        MAXRAM       => 0,
        MAXROM       => 0,
        PROGSIZE     => 0,              Size of program memory.
        BADROM       => [0, 0],
        IDLOCS_ADDRS => [0, 0],
        CONFIG_ADDRS => [0, 0],
        EEPROM_ADDRS => [0, 0],
        IDLOCS_MASK  => 0,
        HEADER       => '',
        SCRIPT       => '',
        P16E_FLAGS   => 0,
        COMMENT      => ''              Comment on the end of line.
        }
=cut

my @gp_px_struct;
my @mp_px_struct;

#-----------------------------------------------

my %gp_px_rows_by_name;
my %mp_px_rows_by_name;

my %gp_px_rows_by_coff;
my %mp_px_rows_by_coff;

my $lkr_bank_mask;
my $lkr_bank_shift;
my $lkr_common_start;
my $lkr_common_end;
my $lkr_common_max;
my $lkr_data_max;
my @lkr_common_banks;

my $lkr_linear_start;
my $lkr_linear_end;
my $lkr_ram_max;

my $lkr_idlocs_start;
my $lkr_idlocs_end;

my @lkr_pages;
my $lkr_config_start;
my $lkr_config_end;

my $lkr_eeprom_start;
my $lkr_eeprom_end;

#-----------------------------------------------

=back
        The structure of one element of the @mp_mcus arrays:
        {
        NAME       => '',
        CLASS      => 0,
        COFF       => 0,
        PAGES      => 0,
        ROM        => 0,
        FLASHDATA  => 0,
        EEPROM     => 0,
        CONFIGS    => 0,
        BANKS      => 0,
        ACCESS     => 0,
        P16E_FLAGS => 0
        }
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

sub param_exist($$)
  {
  die "This option \"$_[0]\" requires a parameter.\n" if ($_[1] > $#ARGV);
  }

#-------------------------------------------------------------------------------

sub Log
  {
  return if (pop(@_) > $verbose);
  foreach (@_) { print STDERR $_; }
  print STDERR "\n";
  }

#-------------------------------------------------------------------------------

sub Open($$)
  {
  my ($Name, $Function) = @_;
  my $handle;

  open($handle, '<', $Name) || die "${Function}(): Could not open the \"$Name\" file.\n";
  return $handle;
  }

#-------------------------------------------------------------------------------

sub Create($$)
  {
  my ($Name, $Function) = @_;

  open($out_handler, '>', $Name) || die "${Function}(): Could not create the \"$Name\" file.\n";
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

sub versionCompare($$)
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

sub versionSort($$$)
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
    $k = versionCompare(\$a_s[$i], \$b_s[$i]);

    return $k if ($k != 0);
    }

  return $ret;
  }

#-------------------------------------------------------------------------------

sub create_mask($)
  {
  my $Value = $_[0];
  my ($i, $mask);

  $mask = 0x7FFFFFFF;
  for ($i = 32; $i > 0; --$i)
    {
    if ($Value & (~$mask))
      {
      return (($mask << 1) | 1);
      }

    $mask >>= 1;
    }

  return 0;
  }

#-------------------------------------------------------------------------------

sub str2class($)
  {
  my $Str = $_[0];
  my $class = $class_features_by_mpasmx{$Str};

  return $class->{CLASS} if (defined($class));

  die "str2class(): Unknown class: $Str\n";
  }

#-------------------------------------------------------------------------------

	# Finds path of the latest version of mplabx.

sub find_latest_version($)
  {
  my $Dir = $_[0];
  my @dir_list;

  opendir(DIR, $Dir) || die "find_latest_version(): Can not open. -> \"$Dir\"\n";
  @dir_list = sort { versionSort($a, $b, TRUE) } grep(-d "$Dir/$_" && $_ ne '.' && $_ ne '..', readdir(DIR));
  closedir(DIR);
  return ((scalar(@dir_list) > 0) ? "$Dir/$dir_list[$#dir_list]" : $Dir);
  }

#-------------------------------------------------------------------------------

        # Creates a sorted list of the files which are located
        # in the $Dir directory. The end of file names can be the $Tail only.
        # (The small letters and capital letters are not different.)

sub create_dirlist($$$)
  {
  my ($Array, $Dir, $Tail) = @_;

  opendir(DIR, $Dir) || die "create_dirlist(): Can not open. -> \"$Dir\"\n";
  @{$Array} = sort { versionSort($a, $b, TRUE) } grep(-f "$Dir/$_" && /^.+$Tail$/i, readdir(DIR));
  closedir(DIR);
  }

#-------------------------------------------------------------------------------

        # Reads the $Name text file into the $Array.
        # In the readed file to removes the DOS end of line characters from.

sub read_file($$)
  {
  my ($Name, $Array) = @_;

  Log("Reads the $Name file.", 4);

  my $in = Open($Name, 'read_file');

  @{$Array} = map { chomp($_); $_ =~ s/\r$//o; $_; } <$in>;
  close($in);
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
  my $switch_name;
  my $setting_count = 0;
  my $switch_info = undef;
  my $state = ST_WAIT;
  my $in = Open($Info, 'read_config_bits');

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

  while (<$in>)
    {
    my @fields = ($_ =~ /<([^<>]*)>/go);

    next if (@fields < 3);

    if ($fields[0] eq 'PART_INFO_TYPE')
      {
        # <PART_INFO_TYPE><f220><PIC10F220><16c5x><0><0><ff><1><1f><0><0><0><1>
        # <PART_INFO_TYPE><f527><PIC16F527><16c5ie><0><2><3ff><4><1f><0><0><3f><1>
        # <PART_INFO_TYPE><e529><PIC12F529T39A><16c5xe><0><3><5ff><8><1f><0><0><3f><1>
        # <PART_INFO_TYPE><6628><PIC16F628><16xxxx><0><1><7ff><4><7f><7f><0><0><1>
        # <PART_INFO_TYPE><a829><PIC16LF1829><16Exxx><2><4><1fff><20><7f><ff><0><0><2>
        # <PART_INFO_TYPE><8857><PIC16F18857><16EXxx><2><10><7fff><40><7f><0><0><ff><5>
        # <PART_INFO_TYPE><1330><PIC18F1330><18xxxx><6><1><1fff><10><ff><7f><7f><0><c>

      if (lc($fields[2]) eq $Mcu)
        {
        $config_count = hex($fields[12]);
        $switch_count = 0;
        $setting_count = 0;
        $config_mask = str2class($fields[3])->{CONF_MASK};
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

        # Microchip bug of the 18f47k40 and 18lf47k40 devices: nDEBUG, nXINST
          $switch_name = $fields[1];
          $switch_name =~ s/^n//o;

          $switch_info = {
                         HEAD => $switch_name,
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
    } # while (<$in>)

  close($in);
  }

#-------------------------------------------------------------------------------

        # Writes out the $Configs.

sub dump_config($)
  {
  my $Configs = $_[0];
  my @addresses = sort {$a <=> $b} keys(%{$Configs});
  my @sections;
  my $count = @addresses;
  my ($str, $len, $i);

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
  my ($in_file, $out_file);

  $out_file = ($Lkr) ? "${Name}_g$Tail" : "p$Name$Tail";
  $in_file  = $out_file;
  $out_file .= $out_tail;
  create_dirlist(\@dir_list, $Source_dir, $Tail);

        # This solution eliminates the confusion what inside the file names
        # cause the different small and capital letters.

  $in_file = $1 if (/^($in_file)$/i ~~ @dir_list);

  read_file("$Source_dir/$in_file", \@array);
  Log("Create the $out_file file.", 6);
  Create($out_file, 'convert_file');

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
      @{$Px} = sort { versionSort($a->{DEFINED_AS}, $b->{DEFINED_AS}, FALSE) } @{$Px};
      }

    when (SORT_NAME0)
      {
        # According to the NAMES0 sorts the table.
      @{$Px} = sort { versionSort($a->{NAMES}->[0], $b->{NAMES}->[0], FALSE) } @{$Px};
      }

    when (SORT_NAME1)
      {
        # According to the NAMES1 sorts the table.
      @{$Px} = sort { versionSort($a->{NAMES}->[1], $b->{NAMES}->[1], FALSE) } @{$Px};
      }

    when (SORT_NAME2)
      {
        # According to the NAMES2 sorts the table.
      @{$Px} = sort { versionSort($a->{NAMES}->[2], $b->{NAMES}->[2], FALSE) } @{$Px};
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

    when (E_INC_NAME)
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
\nThe following header name differ from the device name in the $Location:\n
$border\n|      device       |     header file      |\n$border\n${$Array}$border
EOT
;
          }
        } # given ($Action)
      } # when (E_INC_NAME)

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
  my ($sect, $name, $start, $end, $size, $tail, $index);

  if ($Line =~ /^(\w+)\s+NAME=(\S+)\s+START=(\w+)\s+END=(\w+)/iop)
    {
    # uc($1) <- paranoia
    ($sect, $name, $start, $end) = (uc($1), $2, str2dec($3), str2dec($4));
    $size = $end - $start + 1;
    $tail = ${^POSTMATCH};
    $tail =~ s/^\s+//o;

    if ($sect eq 'CODEPAGE')
      {
      if ($name =~ /page\d*$/io)
        {
        # CODEPAGE   NAME=page       START=0x0               END=0x7FFF
        # CODEPAGE   NAME=page2      START=0x400             END=0x5FF

        push(@lkr_pages, { START => $start, END => $end });
        }
      elsif ($name =~ /(idlocs|userid|usrlocs)$/io)
        {
        # CODEPAGE   NAME=.idlocs    START=0x200             END=0x203          PROTECTED 
        # CODEPAGE   NAME=.idlocs    START=0x400             END=0x403          PROTECTED 
        # CODEPAGE   NAME=.idlocs    START=0x440             END=0x443          PROTECTED 
        # CODEPAGE   NAME=userid     START=0x640             END=0x643          PROTECTED 
        # CODEPAGE   NAME=.usrlocs   START=0x2000            END=0x2003         PROTECTED 
        # CODEPAGE   NAME=.idlocs    START=0x2000            END=0x2003         PROTECTED 
        # CODEPAGE   NAME=.idlocs    START=0x8000            END=0x8003         PROTECTED 
        # CODEPAGE   NAME=idlocs     START=0x200000          END=0x200007       PROTECTED

        $lkr_idlocs_start = $start;
        $lkr_idlocs_end   = $end;
        }
      elsif ($name =~ /(config|cfgmem)$/io)
        {
        # CODEPAGE   NAME=.config    START=0xFFF             END=0xFFF          PROTECTED
        # CODEPAGE   NAME=config     START=0x300000          END=0x30000D       PROTECTED
        # CODEPAGE   NAME=cfgmem     START=0x300000          END=0x30000D       PROTECTED

        $lkr_config_start = $start;
        $lkr_config_end   = $end;
        }
      elsif ($name =~ /(eedata|flashdata)$/io)
        {
        # CODEPAGE   NAME=flashdata  START=0x400             END=0x43F          PROTECTED 
        # CODEPAGE   NAME=eedata     START=0x2100            END=0x213F         PROTECTED
        # CODEPAGE   NAME=eedata     START=0x2100            END=0x217F         PROTECTED 
        # CODEPAGE   NAME=eedata     START=0x2100            END=0x21FF         PROTECTED 
        # CODEPAGE   NAME=eedata     START=0xF000            END=0xF0FF         PROTECTED 
        # CODEPAGE   NAME=eedata     START=0xF00000          END=0xF000FF       PROTECTED
        # CODEPAGE   NAME=eedata     START=0xF00000          END=0xF003FF       PROTECTED

        $lkr_eeprom_start = $start;
        $lkr_eeprom_end   = $end;
        }
      } # if ($sect eq 'CODEPAGE')
    elsif ($sect eq 'LINEARMEM' && $tail eq 'PROTECTED')
      {
        # LINEARMEM  NAME=linear0    START=0x2000            END=0x21EF         PROTECTED
        # LINEARMEM  NAME=linear0    START=0x2000            END=0x23EF         PROTECTED

      $lkr_linear_start = $start;
      $lkr_linear_end   = $end;
      }
    elsif ($sect eq 'DATABANK')
      {
      if ($name =~ /^gpr/io)
        {
        $lkr_data_max = $end if ($lkr_data_max < $end);
        }

      $lkr_ram_max = $end if ($lkr_ram_max < $end);
      }
    elsif ($sect eq 'SHAREBANK')
      {
      if ($name =~ /^[dg]prnoba?nk/io)
        {
        # SHAREBANK  NAME=gprnobnk   START=0x7             END=0xF
        # SHAREBANK  NAME=gprnobnk   START=0x27            END=0x2F           PROTECTED
        # SHAREBANK  NAME=gprnobank  START=0x70            END=0x7F
        # SHAREBANK  NAME=gprnobank  START=0xF0            END=0xFF           PROTECTED
        # SHAREBANK  NAME=dprnobank  START=0x70              END=0x7F           PROTECTED

        if ($tail ne 'PROTECTED')
          {
          $lkr_common_max = $end if ($lkr_common_max < $end);
          }

        $lkr_data_max = $end if ($lkr_data_max < $end);
        $index = ($start & $lkr_bank_mask) >> $lkr_bank_shift;
        $lkr_common_banks[$index] = {START => $start, END => $end};
        }
      elsif ($name =~ /^gpr0/io)
        {
        # SHAREBANK  NAME=gpr0       START=0x20            END=0x5F
        # SHAREBANK  NAME=gpr0       START=0xA0            END=0xDF           PROTECTED
        # SHAREBANK  NAME=gpr0       START=0xC             END=0x2F
        # SHAREBANK  NAME=gpr0       START=0x8C            END=0xAF           PROTECTED
        # SHAREBANK  NAME=gpr0a      START=0xD             END=0xF

        $index = ($start & $lkr_bank_mask) >> $lkr_bank_shift;
        $lkr_common_banks[$index] = {START => $start, END => $end};
        }

      $lkr_ram_max = $end if ($lkr_ram_max < $end);
      }
    elsif ($sect eq 'ACCESSBANK')
      {
      $lkr_ram_max = $end if ($lkr_ram_max < $end);
      }
    }
  }

#-------------------------------------------------------------------------------

        # Reads informations from a lkr file.
        # The work was assisted by a very simple preprocessor.

sub read_lkr($$$)
  {
  my ($McuName, $Lkr, $Info) = @_;
  my ($class_feature, $first, $prev_start, $prev_end);

  $lkr_common_start = -1;
  $lkr_common_end   = -1;
  $lkr_data_max     = -1;
  $lkr_common_max   = -1;

  $lkr_linear_start = -1;
  $lkr_linear_end   = -1;
  $lkr_ram_max      = -1;

  $lkr_idlocs_start = -1;
  $lkr_idlocs_end   = -1;

  @lkr_pages = ();
  $lkr_config_start = -1;
  $lkr_config_end   = -1;

  $lkr_eeprom_start = -1;
  $lkr_eeprom_end   = -1;

  return FALSE if (! -e $Lkr);

  Log("Reads the $Lkr file.", 4);

  if (defined($Info))
    {
    $class_feature = $class_features_list[$Info->{CLASS}];
    }
  else
    {
    print STDERR "The $McuName device not exist in the \"$mplabx_inc/$mplabx_dev_info\" file.\n";
    $class_feature = $lost_devices{$McuName};

    if (! defined($class_feature))
      {
      print STDERR "The $McuName is unknown device!\n";
      return FALSE;
      }
    }

  my $in = Open($Lkr, 'read_lkr');
  my $name = basename($Lkr);

  $lkr_bank_mask    = $class_feature->{BANK_MASK};
  $lkr_bank_shift   = $class_feature->{BANK_SHIFT};
  @lkr_common_banks = (({START => -1, END => -1}) x PIC_BANK_NUMBER_MAX);

  reset_preprocessor();

  while (<$in>)
    {
    chomp;
    s/\r$//o;

    if ($_ !~ /^\s*$/o && $_ !~ /^\/\//o)
      {
      s/^\s*|\s*$//go;
      run_preprocessor($name, \&process_lkr_line, $_) if ($_ !~ m|^//|o);
      }

    ++$pp_line_number;
    }

  die "read_lkr(): There is no one program memory page either. ($name)\n" if (! @lkr_pages);
  close($in);

  Log($Lkr, 8);
  $prev_start = -1;
  $prev_end   = -1;
  $first      = TRUE;

  foreach (@lkr_common_banks)
    {
    my ($start, $end) = ($_->{START}, $_->{END});

    if ($start < 0 || $end < 0)
      {
      Log("    --------", 8);

      if ($first)
        {
        # The common area not exist the first bank, therefore it does not matter
        # that later exist or not.
        $lkr_common_start = -1;
        $lkr_common_end   = -1;
        }
      }
    else
      {
      Log(sprintf("    0x%03X - 0x%03X", $start, $end), 8);

      $start &= ~$lkr_bank_mask;
      $end   &= ~$lkr_bank_mask;

      if ($first)
        {
        $lkr_common_start = $start;
        $lkr_common_end   = $end;
        }
      else
        {
        if ($prev_start != $start)
          {
          # This area in the previous bank starts on a different address.
          Log(sprintf("    prev_start{0x%02X} != start{0x%02X}", $prev_start, $start), 8);
          $lkr_common_start = -1;
          $lkr_common_end   = -1;
          }

        if ($prev_end != $end)
          {
          # This area in the previous bank ends on a different address.
          Log(sprintf("    prev_end{0x%02X}   != end{0x%02X}", $prev_end, $end), 8);
          $lkr_common_start = -1;
          $lkr_common_end   = -1;
          }
        }
      }

    $prev_start = $start;
    $prev_end   = $end;
    $first      = FALSE;
    } # foreach (@lkr_common_banks)

  $lkr_common_max = -1 if ($lkr_common_max == ~$lkr_bank_mask);

  return TRUE;
  }

#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@  The mplabx related procedures.  @@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        # Reads characteristics of all MCUs from the $mplabx_dev_info.

sub read_all_mcu_info_from_mplabx()
  {
  my $path = "$mplabx_inc/$mplabx_dev_info";
  my $prev;
  my $name_error = '';
  my $tmp;
  my $class_feat;
  my $split_min;
  my $split_max;

  Log("Reads the $path file.", 4);

  my $in = Open($path, 'read_all_mcu_info_from_mplabx');
  my $info = undef;

  @mp_mcus = ();
  %mp_mcus_by_name = ();

  while (<$in>)
    {
        # <PART_INFO_TYPE><f220><PIC10F220><16c5x><0><0><ff><1><1f><0><0><0><1>
        # <PART_INFO_TYPE><f527><PIC16F527><16c5ie><0><2><3ff><4><1f><0><0><3f><1>
        # <PART_INFO_TYPE><e529><PIC12F529T39A><16c5xe><0><3><5ff><8><1f><0><0><3f><1>
        # <PART_INFO_TYPE><6628><PIC16F628><16xxxx><0><1><7ff><4><7f><7f><0><0><1>
        # <PART_INFO_TYPE><a829><PIC16LF1829><16Exxx><2><4><1fff><20><7f><ff><0><0><2>
        # <PART_INFO_TYPE><8857><PIC16F18857><16EXxx><2><10><7fff><40><7f><0><0><ff><5>
        # <PART_INFO_TYPE><1330><PIC18F1330><18xxxx><6><1><1fff><10><ff><7f><7f><0><c>

    if (/^<PART_INFO_TYPE><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)><(\w+)>/io)
      {
      if (defined($info))
        {
	my $n = $info->{NAME};

	$prev = $mp_mcus_by_name{$n};

	if (defined($prev))
	  {
	  report(RP_ADD, E_NAME_COLL, \$name_error, $n);
	  }
        else
          {
          $mp_mcus_by_name{$n} = $info;
          }

	push(@mp_mcus, $info);
	$info = undef;
        }

      my $scl = $3;
      my ($coff,  $name,   $class, $pages, $rom)     = (hex($1), lc($2),  str2class($scl), hex($5),  hex($6));
      my ($banks, $eeprom, $split, $fdata, $configs) = (hex($7), hex($9), hex($10),        hex($11), hex($12));

      $name =~ s/^pic//o;

        # With these does not deal with.

      next if ($name eq '16c5x'  || $name =~ /^1[6-8]cxx$/o ||
               $name eq 'ap7675' || $name =~ /^eeprom/o ||
               $name =~ /^mcp25/o);

      $class_feat = $class_features_by_mpasmx{$scl};
      $tmp = $class_feat->{BANK_MAX};
      Log("Read info of the $name MCU.", 9);

      if ($banks > $tmp)
        {
        # This is a Microchip bug in the database.
        print STDERR "Too much the Bank number of $name device: $banks, May at most $tmp.\n";
        $banks = $tmp;
        }

      if (($class_feat->{ACC_SPLIT_MIN} > 0) && ($class_feat->{ACC_SPLIT_MAX} > $class_feat->{ACC_SPLIT_MIN}))
        {
        $split_min = $class_feat->{ACC_SPLIT_MIN};
        $split_max = $class_feat->{ACC_SPLIT_MAX};

        if ($split < $split_min)
          {
          printf STDERR "The Access Bank split address (0x%03X) of $name device less than 0x%03X. Restated value: 0x%03X\n",
                        $split, $split_min, $split_min;
          $split = $split_min;
          }
        elsif ($split > $split_max)
          {
          printf STDERR "The Access Bank split address (0x%03X) of $name device greater than 0x%03X. Restated value: 0x%03X\n",
                        $split, $split_max, $split_max;
          $split = $split_max;
          }
        }

      $info = {
              NAME       => $name,
              CLASS      => $class,
              COFF       => $coff,            # Coff ID of device. (16 bit wide)
              PAGES      => $pages,           # Number of ROM/FLASH pages.

                # These addresses relative, compared to the beginning of the blocks.
              ROM        => $rom,             # Last address of ROM/FLASH.
              FLASHDATA  => $fdata,           # Last address of FLASH Data.
              EEPROM     => $eeprom,          # Last address of EEPROM.

              CONFIGS    => $configs,         # Number of Configuration bytes/words.
              BANKS      => $banks,           # Number of RAM Banks.
              ACCESS     => $split,           # Last address of lower Access RAM of pic18f series.
              P16E_FLAGS => (($name =~ /^18l?f\d+j\d+/o) ? PIC16E_FLAG_J_SUBFAMILY : 0)
              };
      }
    elsif (/^<SWITCH_INFO_TYPE><n?XINST>/io)
      {
      $info->{P16E_FLAGS} |= PIC16E_FLAG_HAVE_EXTINST if (defined($info));
      }
    }

  if (defined($info))
    {
    my $n = $info->{NAME};

    $prev = $mp_mcus_by_name{$n};

    if (defined($prev))
      {
      report(RP_ADD, E_NAME_COLL, \$name_error, $n);
      }
    else
      {
      $mp_mcus_by_name{$n} = $info;
      }

    push(@mp_mcus, $info);
    }

  close($in);
  report(RP_PRINT, E_NAME_COLL, \$name_error, 'mplabx', *STDERR);
  }

#-------------------------------------------------------------------------------

        # The basis of available data creates a new row to the px structure.

sub new_px_row($$$$)
  {
  my ($Error, $Info, $Header, $Script) = @_;
  my ($name, $rom_end) = ($Info->{NAME}, $Info->{ROM});
  my $lkr_rom_end = $lkr_pages[$#lkr_pages]->{END};

  $Script = "${name}_g.lkr" if (! defined($Script));

  Log('Create a px struct row.', 6);

  if ($rom_end != $lkr_rom_end)
    {
    report(RP_ADD, E_ROM_DIFF, $Error, $Script, $lkr_rom_end, $mplabx_dev_info, $rom_end);
    }

  my $p18j = ($name =~ /^18l?f\d+j\d+/o);
  my $class = $Info->{CLASS};
  my $eeprom_start = $class_features_list[$class]->{EE_START};
  my $bad_start = -1;
  my $bad_end   = -1;

  if ((! $p18j) && ($eeprom_start > 0) && ($Info->{EEPROM} > 0))
    {
    $bad_start = $rom_end + 1;
    $bad_end   = $lkr_eeprom_start - 1;
    $rom_end   = $lkr_eeprom_start + $Info->{EEPROM};
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
           BANK_BITS    => ($lkr_ram_max > 0) ? (create_mask($lkr_ram_max) & $class_features_list[$class]->{BANK_MASK}) : 0,
           COMMON_RAM   => [ $lkr_common_start, $lkr_common_end ],
           COMMON_MAX   => $lkr_common_max,
           LINEAR_RAM   => [ $lkr_linear_start, $lkr_linear_end ],
           MAXRAM       => $lkr_ram_max,
           MAXROM       => $rom_end,
           PROGSIZE     => $lkr_rom_end + 1,
           BADROM       => [ $bad_start, $bad_end ],
           IDLOCS_ADDRS => [ $lkr_idlocs_start, $lkr_idlocs_end ],
           CONFIG_ADDRS => [ $lkr_config_start, $lkr_config_end ],
           EEPROM_ADDRS => [ $lkr_eeprom_start, $lkr_eeprom_end ],
           IDLOCS_MASK  => $class_features_list[$class]->{IDLOCS_MASK},
           HEADER       => $Header,
           SCRIPT       => $Script,
    	   P16E_FLAGS   => $Info->{P16E_FLAGS},
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

  create_dirlist(\@dir_list, $mplabx_lkr, '.lkr');

  Log('Creates the @mp_px_struct.', 4);
  @mp_px_struct = ();
  %mp_px_rows_by_name = ();
  %mp_px_rows_by_coff = ();

  foreach my $info (@mp_mcus)
    {
    my ($name, $coff) = ($info->{NAME}, $info->{COFF});
    my $header = ($name =~ /^1/o) ? "p${name}.inc" : "${name}.inc";
    my $script = "${name}_g.lkr";

        # The Microchip is quite has unfortunately been ported to Linux the mplabx package.
        # It seems that they do not care the fact, that on Linux in the name of files there
        # is difference between the small and capital letters.
        # For example: 16C58a_g.lkr, 16c58b_g.lkr, P16F1823.INC, p16f1824.inc
        # For this reason more difficult may be find a lkr file.

    $script = $1 if (/^($script)$/i ~~ @dir_list);

    next if (! read_lkr($name, "$mplabx_lkr/$script", $info));

    my $px = new_px_row(\$mem_error, $info, $header, $script);

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

  report(RP_PRINT, E_ROM_DIFF,  \$mem_error,  'mplabx', *STDERR);
  report(RP_PRINT, E_COFF_COLL, \$coff_error, 'mplabx', *STDERR);
  sort_px_struct(\@mp_px_struct, SORT_NAME0);
  }

################################################################################
################################################################################

        # Reads the content of the list file.

sub read_list_file($)
  {
  my $List = $_[0];

  Log("Reads the $List file.", 4);

  my $in = Open($List, 'read_list_file');

  %list_file_members = ();

  while (<$in>)
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

  close($in);
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
  Log("Reads the $gpproc_path file.", 4);

  my $in = Open($gpproc_path, 'read_gpproc_c_content');

  @gpproc_c_content = <$in>;
  close($in);
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
  my $inc_error = '';
  my $lkr_error = '';

        # static struct px pics[] = {
        #   { PROC_CLASS_PIC12E   , "__12F529T39A"  , { "pic12f529t39a"  , "p12f529t39a"    , "12f529t39a"      }, 0xE529,  3,    8, 0x00E0, { 0x07, 0x0F }, 0x06F, {     -1,     -1 }, 0x00FF, 0x0005FF, 0x000600, {       -1,       -1 }, { 0x000640, 0x000643 }, { 0x000FFF, 0x000FFF }, { 0x000600, 0x00063F }, 0x0FF0, "p12f529t39a.inc"  , "12f529t39a_g.lkr"  , 0 },
        #   { PROC_CLASS_PIC14E   , "__16LF1517"    , { "pic16lf1517"    , "p16lf1517"      , "16lf1517"        }, 0xA517,  4,   32, 0x0F80, { 0x70, 0x7F },    -1, { 0x2000, 0x21EF }, 0x0FFF, 0x001FFF, 0x002000, {       -1,       -1 }, { 0x008000, 0x008003 }, { 0x008007, 0x008008 }, {       -1,       -1 }, 0x3F80, "p16lf1517.inc"    , "16lf1517_g.lkr"    , 0 },

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
      if (/^\s*static\s+struct\s+px\s+pics\[\s*\]\s*=\s*\{\s*$/io)
        {
        $in_table = TRUE;
        $px_struct_begin = $n;
        }
      }
        #               $1                $2                 $3            $4            $5                 $6             $7             $8             $9                $10         $11              $12              $13         $14              $15         $16         $17              $18         $19                  $20         $21                  $22         $23                  $24         $25              $26              $27                   $28              $29
    elsif (/\{\s*(PROC_CLASS_\w+)\s*,\s*"(\w+)"\s*,\s*\{\s*"(\w+)"\s*,\s*"(\w+)"\s*,\s*"(\w+)"\s*}\s*,\s*([\w-]+)\s*,\s*([\w-]+)\s*,\s*([\w-]+)\s*,\s*([\w-]+)\s*,\s*\{\s*(\S+)\s*,\s*(\S+)\s*\}\s*,\s*(\S+)\s*,\s*\{\s*(\S+)\s*,\s*(\S+)\s*\}\s*,\s*(\S+)\s*,\s*(\S+)\s*,\s*(\S+)\s*,\s*\{\s*(\S+)\s*,\s*(\S+)\s*\}\s*,\s*{\s*(\S+)\s*,\s*(\S+)\s*\}\s*,\s*{\s*(\S+)\s*,\s*(\S+)\s*\}\s*,\s*{\s*(\S+)\s*,\s*(\S+)\s*\}\s*,\s*(\w+)\s*,\s*\"?([\.\w]+)\"?\s*,\s*\"?([\.\w]+)\"?\s*,\s*(\d+)\s*\}/iop)
      {
      my ($class, $long_name, $middle_name, $short_name, $coff, $header, $script) = ($1, $3, $4, $5, str2dec($6), $27, $28);
        # Maybe there is a comment at the end of the line.
      my $tail = ${^POSTMATCH};
      my $bank_mask = $class_features_by_gputils{$class}->{BANK_MASK};
      my $prev;
      my $px = {
               IGNORED      => FALSE,
               CLASS        => $class,
               DEFINED_AS   => $2,
               NAMES        => [ $long_name, $middle_name, $short_name ],
               COFF_TYPE    => $coff,
               NUM_PAGES    => str2dec($7),
               NUM_BANKS    => str2dec($8),
               BANK_BITS    => str2dec($9),
               COMMON_RAM   => [ str2dec($10), str2dec($11) ],
               COMMON_MAX   => str2dec($12),
               LINEAR_RAM   => [ str2dec($13), str2dec($14) ],
               MAXRAM       => str2dec($15),
               MAXROM       => str2dec($16),
               PROGSIZE     => str2dec($17),
               BADROM       => [ str2dec($18), str2dec($19) ],
               IDLOCS_ADDRS => [ str2dec($20), str2dec($21) ],
               CONFIG_ADDRS => [ str2dec($22), str2dec($23) ],
               EEPROM_ADDRS => [ str2dec($24), str2dec($25) ],
               IDLOCS_MASK  => str2dec($26),
               HEADER       => $header,
               SCRIPT       => $script,
               P16E_FLAGS   => str2dec($29),
               COMMENT      => ''
               };

      $tail =~ s/^\s*,?\s*//o;
      $tail =~ s/^\*\/\s*$//o;

      $px->{IGNORED} = TRUE if ($_ =~ /^\s*\/\*/o);
      $px->{COMMENT} = $1 if ($tail =~ /\/\*(.+)\*\/\s*$/o);

        # Watches the name collisions.

      $prev = $gp_px_rows_by_name{$short_name};

      if (defined($prev))
        {
        report(RP_ADD, E_NAME_COLL, \$name_error, $short_name);
        }
      else
        {
        $gp_px_rows_by_name{$short_name} = $px;
        }

        # Does not deal with the generic devices. Otherwise this would cause a false error message.

      if ($long_name ne 'pic16c5x' &&
          $long_name !~ /^pic1[6-8]cxx$/o &&
          $long_name !~ /^eeprom/o)
        {
        # Watches the coff_type collisions.

        $prev = $gp_px_rows_by_coff{$coff};

        if (defined($prev))
          {
          report(RP_ADD, E_COFF_COLL, \$coff_error, $coff, $prev->{NAMES}->[0], $long_name);
          }
        else
          {
          $gp_px_rows_by_coff{$coff} = $px;
          }
        }

        # Watches the header name error.

      if ($header ne 'NULL' && $header !~ /^${middle_name}.inc$/)
        {
        report(RP_ADD, E_INC_NAME, \$inc_error, $middle_name, $header);
        }

        # Watches the script name error.

      if ($script ne 'NULL' && $script !~ /^${short_name}(_g)?.lkr$/)
        {
        report(RP_ADD, E_LKR_NAME, \$lkr_error, $long_name, $script);
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
  report(RP_PRINT, E_INC_NAME,  \$inc_error,  'gputils', *STDERR);
  report(RP_PRINT, E_LKR_NAME,  \$lkr_error,  'gputils', *STDERR);
  sort_px_struct(\@gp_px_struct, SORT_DEFINED_AS);
  }

#-------------------------------------------------------------------------------

sub neg_form($$)
  {
  my ($Number, $Width) = @_;

  if ($Number < 0)
    {
    sprintf('%*i', $Width + 2, $Number);
    }
  else
    {
    sprintf('0x%0*X', $Width, $Number);
    }
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
      print ('common_ram  : ' . neg_form($_->{COMMON_RAM}->[0], 2) . ', ' . neg_form($_->{COMMON_RAM}->[1], 2) . "\n");
      print ('common_max  : ' . neg_form($_->{COMMON_MAX}, 3) . "\n");
      print ('linear_ram  : ' . neg_form($_->{LINEAR_RAM}->[0], 4) . ', ' . neg_form($_->{LINEAR_RAM}->[1], 4) . "\n");
      print ('maxram      : ' . neg_form($_->{MAXRAM}, 4) . "\n");
      print ('maxrom      : ' . neg_form($_->{MAXROM}, 6) . "\n");
      print ('prog_size   : ' . neg_form($_->{PROGSIZE}, 6) . "\n");
      print ('badrom      : ' . neg_form($_->{BADROM}->[0], 6) . ', ' . neg_form($_->{BADROM}->[1], 6) . "\n");
      print ('idlocs_addrs: ' . neg_form($_->{IDLOCS_ADDRS}->[0], 6) . ', ' . neg_form($_->{IDLOCS_ADDRS}->[1], 6) . "\n");
      print ('config_addrs: ' . neg_form($_->{CONFIG_ADDRS}->[0], 6) . ', ' . neg_form($_->{CONFIG_ADDRS}->[1], 6) . "\n");
      print ('eeprom_addrs: ' . neg_form($_->{EEPROM_ADDRS}->[0], 6) . ', ' . neg_form($_->{EEPROM_ADDRS}->[1], 6) . "\n");
      printf "idlocs_mask : 0x%04X\n", $_->{IDLOCS_MASK};
      print  "header      : $_->{HEADER}\n";
      print  "script      : $_->{SCRIPT}\n";
      print  "pic16e_flags: $_->{P16E_FLAGS}\n";
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
  $line .= sprintf((($i <= PIC_BANK_NUMBER_MAX) ? '%4u, ' : '0x%02X, '), $i);
  $line .= neg_form($Row->{BANK_BITS}, 4);
  $line .= ', ';

  $line .= '{ ';
  $line .= neg_form($Row->{COMMON_RAM}->[0], 2);
  $line .= ', ';
  $line .= neg_form($Row->{COMMON_RAM}->[1], 2);
  $line .= ' }, ';
  $line .= neg_form($Row->{COMMON_MAX}, 3);
  $line .= ', ';

  $line .= '{ ';
  $line .= neg_form($Row->{LINEAR_RAM}->[0], 4);
  $line .= ', ';
  $line .= neg_form($Row->{LINEAR_RAM}->[1], 4);
  $line .= ' }, ';

  $line .= neg_form($Row->{MAXRAM}, 4);
  $line .= ', ';
  $line .= neg_form($Row->{MAXROM}, 6);
  $line .= ', ';
  $line .= neg_form($Row->{PROGSIZE}, 6);
  $line .= ', ';

  $line .= '{ ';
  $line .= neg_form($Row->{BADROM}->[0], 6);
  $line .= ', ';
  $line .= neg_form($Row->{BADROM}->[1], 6);
  $line .= ' }, ';

  $line .= '{ ';
  $line .= neg_form($Row->{IDLOCS_ADDRS}->[0], 6);
  $line .= ', ';
  $line .= neg_form($Row->{IDLOCS_ADDRS}->[1], 6);
  $line .= ' }, ';

  $line .= '{ ';
  $line .= neg_form($Row->{CONFIG_ADDRS}->[0], 6);
  $line .= ', ';
  $line .= neg_form($Row->{CONFIG_ADDRS}->[1], 6);
  $line .= ' }, ';

  $line .= '{ ';
  $line .= neg_form($Row->{EEPROM_ADDRS}->[0], 6);
  $line .= ', ';
  $line .= neg_form($Row->{EEPROM_ADDRS}->[1], 6);
  $line .= ' }, ';

  $line .= sprintf("0x%04X, ", $Row->{IDLOCS_MASK});

  $i = $Row->{HEADER};
  $line .= sprintf('%-19s, ', (($i ne 'NULL') ? "\"$i\"" : $i));
  $i = $Row->{SCRIPT};
  $line .= sprintf('%-20s, ', (($i ne 'NULL') ? "\"$i\"" : $i));
  $i = $Row->{P16E_FLAGS};
  $line .= sprintf('%i },', $i);

  $line = "/*$line*/" if ($Row->{IGNORED});

  $line .= " /*$_->{COMMENT}*/" if ($Row->{COMMENT} ne '');
  return $line;
  }

#-------------------------------------------------------------------------------

        # Creates the "$gpprocessor_c.gen" file.

sub create_gpprocessor_c()
  {
  my $name = "$gpprocessor_c$out_tail";

  Log("Create the $name file.", 6);
  Create($name, 'create_gpprocessor_c');

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

  foreach (sort { versionSort($a, $b, FALSE) } keys(%list_file_members))
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

  foreach (sort { versionSort($a, $b, FALSE) } keys(%list_file_members))
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

  foreach (sort { versionSort($a, $b, FALSE) } keys(%list_file_members))
    {
    fabricate_lkr($_);
    }
  }

#-------------------------------------------------------------------------------

        # This the 1th helper of addition.

sub addition_helper1($)
  {
  my ($Name, $Info) = @_;

  die "addition_helper1(): Miss the name of MCU!\n" if ($Name eq '');
  die "addition_helper1(): This name is wrong: \"$Name\"\n" if ($Name !~ /^(p(ic)?)?$name_filter$/io);

  $Name =~ s/^p(ic)?//io;
  $Name = lc($Name);

  if ($examine_exist_device && defined($gp_px_rows_by_name{$Name}))
    {
    print STDERR "This MCU: $Name already exist in the gputils.\n";
    return undef;
    }

  read_lkr($Name, "$mplabx_lkr/${Name}_g.lkr", $mp_mcus_by_name{$Name});
  return $Name;
  }

#-------------------------------------------------------------------------------

        # This the 2th helper of addition.

sub addition_helper2($$$$)
  {
  my ($Coff_error, $Mem_error, $Name, $Info) = @_;

  die "addition_helper2(): This device: $Name not exist in the $mplabx_dev_info!\n" if (! defined($Info));

  my $coff = $Info->{COFF};
  my $px   = $gp_px_rows_by_coff{$coff};

  report(RP_ADD, E_COFF_COLL, $Coff_error, $coff, $px->{NAMES}->[0], "pic$Name") if (defined($px));

  $px = new_px_row($Mem_error, $Info, "p${Name}.inc", "${Name}_g.lkr");
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

  addition_helper2(\$coff_error, \$mem_error, $Name, $mp_mcus_by_name{$Name});

  report(RP_PRINT, E_COFF_COLL, \$coff_error, 'gputils', *STDERR);
  report(RP_PRINT, E_ROM_DIFF,  \$mem_error,  'mplabx',  *STDERR);
  sort_px_struct(\@gp_px_struct, SORT_DEFINED_AS);
  }

#-------------------------------------------------------------------------------

        # Performs more addition from a list.

sub add_more_mcu()
  {
  return if (! scalar(keys(%list_file_members)));

  my $coff_error = '';
  my $mem_error = '';

  foreach my $name (sort { versionSort($a, $b, FALSE) } keys(%list_file_members))
    {
    $name = addition_helper1($name);

    return if (! defined($name));

    addition_helper2(\$coff_error, \$mem_error, $name, $mp_mcus_by_name{$name});
    }

  report(RP_PRINT, E_COFF_COLL, \$coff_error, 'gputils', *STDERR);
  report(RP_PRINT, E_ROM_DIFF,  \$mem_error,  'mplabx',  *STDERR);
  sort_px_struct(\@gp_px_struct, SORT_DEFINED_AS);
  }

#-------------------------------------------------------------------------------

        # Create the p16f1xxx.inc file.

sub create_p16f1xxx_inc()
  {
  my $name = "$p16f1xxx_inc$out_tail";

  Log("Create the $name file.", 6);
  Create($name, 'create_p16f1xxx_inc');

  print $out_handler <<EOT
;$border0
;
; p16f1xxx.inc - Common include file to PIC16XXXX enhanced families.
;
; This file is has been generated using $PROGRAM${time_str}.
;
;    To the usage of the PIC16F1503 MCU an example:
;
;	1. Command line switch:
;	    gpasm -p pic16f1503 source.asm
;
;	2. The LIST directive in the source file:
;	    list p=16f1503
;		(It does not matter that lowercase or uppercase.)
;
;	3. The PROCESSOR directive in the source file:
;	    processor 16f1503
;		(It does not matter that lowercase or uppercase.)
;
;$border0

ifdef __16F1xxx
  include <p16f1937.inc>
endif
EOT
;

  foreach (@gp_px_struct)
    {
    $name = $_->{DEFINED_AS};

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

  Log("Create the $name file.", 6);
  Create($name, 'create_p18cxxx_inc');

  print $out_handler <<EOT
;$border0
;
; p18cxxx.inc - Common include file to PIC18XXXX families.
;
; This file is has been generated using $PROGRAM${time_str}.
;
;    To the usage of the PIC18F4520 MCU an example:
;
;	1. Command line switch:
;	    gpasm -p pic18f4520 source.asm
;
;	2. The LIST directive in the source file:
;	    list p=18f4520
;		(It does not matter that lowercase or uppercase.)
;
;	3. The PROCESSOR directive in the source file:
;	    processor 18f4520
;		(It does not matter that lowercase or uppercase.)
;
;$border0

ifdef __18CXX
  include <p18c452.inc>
endif
EOT
;
  foreach (@gp_px_struct)
    {
    $name = $_->{DEFINED_AS};

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

    if (($_->{COFF_TYPE}         != $mp->{COFF_TYPE})         ||
        ($_->{NUM_PAGES}         != $mp->{NUM_PAGES})         ||
        ($_->{NUM_BANKS}         != $mp->{NUM_BANKS})         ||
        ($_->{BANK_BITS}         != $mp->{BANK_BITS})         ||
        ($_->{COMMON_RAM}->[0]   != $mp->{COMMON_RAM}->[0])   ||
        ($_->{COMMON_MAX}        != $mp->{COMMON_MAX})        ||
        ($_->{LINEAR_RAM}->[0]   != $mp->{LINEAR_RAM}->[0])   ||
        ($_->{MAXRAM}            != $mp->{MAXRAM})            ||
        ($_->{MAXROM}            != $mp->{MAXROM})            ||
        ($_->{PROGSIZE}          != $mp->{PROGSIZE})          ||
        ($_->{BADROM}->[0]       != $mp->{BADROM}->[0])       ||
        ($_->{BADROM}->[1]       != $mp->{BADROM}->[1])       ||
        ($_->{IDLOCS_ADDRS}->[0] != $mp->{IDLOCS_ADDRS}->[0]) ||
        ($_->{IDLOCS_ADDRS}->[1] != $mp->{IDLOCS_ADDRS}->[1]) ||
        ($_->{CONFIG_ADDRS}->[0] != $mp->{CONFIG_ADDRS}->[0]) ||
        ($_->{CONFIG_ADDRS}->[1] != $mp->{CONFIG_ADDRS}->[1]) ||
        ($_->{EEPROM_ADDRS}->[0] != $mp->{EEPROM_ADDRS}->[0]) ||
        ($_->{EEPROM_ADDRS}->[1] != $mp->{EEPROM_ADDRS}->[1]) ||
        ($_->{IDLOCS_MASK}       != $mp->{IDLOCS_MASK}))
      {
      $out .= "$border\n";
      $out .= 'mplabx  ' . create_one_px_row($mp, FALSE) . "\n";
      $out .= 'gputils ' . create_one_px_row($_, FALSE)  . "\n";
      }

#    print "  " . create_one_px_row($mp, TRUE) . "\n";
    }

  print (($out ne '') ? "$out$border\n" : "No differences.\n");
  }

#-------------------------------------------------------------------------------

sub show_differences_mp_gp()
  {
  my $out = '';

  Log('Shows the supported MCUs in the mplabx and the gputils.', 6);

  foreach (sort { versionSort($a, $b, FALSE) } keys(%mp_mcus_by_name))
    {
    $out .= "$_\n" if (! defined($gp_px_rows_by_name{$_}));
    }

  print (($out ne '') ? "The mplabx supports, but the gputils not:\n$out" : "No differences.\n");
  }

#-------------------------------------------------------------------------------

sub show_differences_gp_mp()
  {
  my $out = '';

  Log('Shows the supported MCUs in the gputils and the mplabx.', 6);

  foreach (sort { versionSort($a, $b, FALSE) } keys(%gp_px_rows_by_name))
    {
    $out .= "$_\n" if (! defined($mp_mcus_by_name{$_}));
    }

  print (($out ne '') ? "The gputils supports, but the mplabx not:\n$out" : "No differences.\n");
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

        -ld or --list-support-differences

            Shows differences between the gputils and the mplabx MCU support.

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
my $user_mplabx_path = FALSE;

for (my $i = 0; $i < @ARGV; )
  {
  my $opt = $ARGV[$i++];

  given ($opt)
    {
    when (/^-(gp|-gputils-path)$/o)
      {
      param_exist($opt, $i);
      $gputils_path = $ARGV[$i++];
      }

    when (/^-(mp|-mplabx-path)$/o)
      {
      param_exist($opt, $i);
      $mplabx_path = $ARGV[$i++];
      $user_mplabx_path = TRUE;
      }

    when (/^-(p|-processor)$/o)
      {
      param_exist($opt, $i);
      $mcu = $ARGV[$i++];
      }

    when (/^-(l|-list-file)$/o)
      {
      param_exist($opt, $i);
      $list_file = $ARGV[$i++];
      }

    when (/^-(a|-add)$/o)                       { $operation = OP_ADD; }
    when (/^-(r|-remove)$/o)                    { $operation = OP_REMOVE; }
    when (/^-(fi|-fabricate-inc)$/o)            { $operation = OP_FABR_INC; }
    when (/^-(fl|-fabricate-lkr)$/o)            { $operation = OP_FABR_LKR; }
    when ('--regenerate')                       { $operation = OP_REGENERATE; }
    when (/^-(os|-only-survey)$/o)              { $operation = OP_SURVEY; }
    when (/^-(lg|-list-gputils)$/o)             { $operation = OP_LIST_GP; }
    when (/^-(lm|-list-mplabx)$/o)              { $operation = OP_LIST_MP; }
    when (/^-(ld|-list-support-differences)$/o) { $operation = OP_SHOWS_SPDIFF; }
    when (/^-(dc|-show-diff-coff-types)$/o)     { $operation = OP_SHOWS_CDIFF; }
    when (/^-(dp|-show-diff-px-struct)$/o)      { $operation = OP_SHOWS_PXDIFF; }
    when (/^-(ac|-add-config-bits)$/o)          { $add_config_bits = TRUE; }
    when (/^-(el|-extended-list)$/o)            { $extended_list = TRUE; }
    when (/^-(ne|-no-examine-exist)$/o)         { $examine_exist_device = FALSE; }
    when (/^-(t|-timestamp)$/o)                 { $timestamp = TRUE; }

    when (/^-(v|-verbose)$/o)
      {
      param_exist($opt, $i);
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

$mplabx_path = find_latest_version($mplabx_root_path) if (!$user_mplabx_path);

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

read_all_mcu_info_from_mplabx();
read_gpproc_c_content();
extract_px_struct();

given ($operation)
  {
  when (OP_REGENERATE)
    {
    create_gpprocessor_c();
    create_p16f1xxx_inc();
    create_p18cxxx_inc();
    exit(0);
    }

  when (OP_LIST_GP)
    {
    sort_px_struct(\@gp_px_struct, SORT_NAME0);
    list_known_mcu(\@gp_px_struct);
    exit(0);
    }

  when (OP_SHOWS_SPDIFF)
    {
    show_differences_mp_gp();
    show_differences_gp_mp();
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

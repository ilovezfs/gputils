#!/usr/bin/perl -w

=back

    Copyright (C) 2014-2015 Molnar Karoly <molnarkaroly@users.sf.net>

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

    This program prepares the gpcfg-table.c and gpcfg.h files from 8bit_device.info.

    $Id$
=cut

use strict;
use warnings;
use 5.12.0;                     # when (regex)
no if $] >= 5.018, warnings => "experimental::smartmatch";        # perl 5.16
use feature 'switch';
use POSIX 'strftime';

use constant FALSE => 0;
use constant TRUE  => 1;

use constant ST_WAIT   => 0;
use constant ST_LISTEN => 1;

use constant FNV1A32_INIT  => 0x811C9DC5;
use constant FNV1A32_PRIME => 0x01000193;

my $PROGRAM = 'cfg-import.pl';
my $limit_defaults = FALSE;
my $verbose = 0;

my $mplabx_root_path = '/opt/microchip/mplabx';
my $mplabx_dev_info  = '8bit_device.info';
my $mplabx_path      = '';
my $dev_info_path    = '';

my $pic_name_mask = qr/PIC1(2(C[ER]?|HV)\d+\w+|6(C[ER]?|HV)\d+\w+|7C[R]?\d+\w+|[0268](C|L?F)\d+\w+)|PICRF\d+\w+|MCP\d+|MCV\d+\w+|PS\d+/;

my $time_str;

use constant PROC_CLASS_PIC12   => 0;
use constant PROC_CLASS_PIC12E  => 1;
use constant PROC_CLASS_PIC12I  => 2;
use constant PROC_CLASS_PIC14   => 3;
use constant PROC_CLASS_PIC14E  => 4;
use constant PROC_CLASS_PIC14EX => 5;
use constant PROC_CLASS_PIC16   => 6;
use constant PROC_CLASS_PIC16E  => 7;

my %class_features_p12 =
  (
  CLASS      => PROC_CLASS_PIC12,
  NAME       => '12 bit MCU',
  DIR_DIGITS => 3,
  ENHANCED   => FALSE
  );

my %class_features_p12e =
  (
  CLASS      => PROC_CLASS_PIC12E,
  NAME       => '12 bit enhanced MCU',
  DIR_DIGITS => 3,
  ENHANCED   => TRUE
  );

my %class_features_p12i =
  (
  CLASS      => PROC_CLASS_PIC12I,
  NAME       => '12 bit enhanced MCU',
  DIR_DIGITS => 3,
  ENHANCED   => TRUE
  );

my %class_features_p14 =
  (
  CLASS      => PROC_CLASS_PIC14,
  NAME       => '14 bit MCU',
  DIR_DIGITS => 4,
  ENHANCED   => FALSE
  );

my %class_features_p14e =
  (
  CLASS      => PROC_CLASS_PIC14E,
  NAME       => '14 bit enhanced MCU',
  DIR_DIGITS => 4,
  ENHANCED   => TRUE
  );

my %class_features_p14ex =
  (
  CLASS      => PROC_CLASS_PIC14EX,
  NAME       => '14 bit enhanced MCU',
  DIR_DIGITS => 4,
  ENHANCED   => TRUE
  );

my %class_features_p16 =
  (
  CLASS      => PROC_CLASS_PIC16,
  NAME       => '16 bit MCU',
  DIR_DIGITS => 2,
  ENHANCED   => FALSE
  );

my %class_features_p16e =
  (
  CLASS      => PROC_CLASS_PIC16E,
  NAME       => '16 bit extended MCU',
  DIR_DIGITS => 2,
  ENHANCED   => TRUE
  );

my %class_features_by_mpasmx =
  (
  '16c5x'  => \%class_features_p12,
  '16c5xe' => \%class_features_p12e,
  '16c5ie' => \%class_features_p12i,
  '16xxxx' => \%class_features_p14,
  '16Exxx' => \%class_features_p14e,
  '16EXxx' => \%class_features_p14ex,
  '17xxxx' => \%class_features_p16,
  '18xxxx' => \%class_features_p16e
  );

my @mcu_classes =
  (
  \%class_features_p12,
  \%class_features_p12e,
  \%class_features_p12i,
  \%class_features_p14,
  \%class_features_p14e,
  \%class_features_p14ex,
  \%class_features_p16,
  \%class_features_p16e
  );

my @mcu_feat_names = sort {
                          $class_features_by_mpasmx{$a}->{ENHANCED} <=> $class_features_by_mpasmx{$b}->{ENHANCED} ||
                          $class_features_by_mpasmx{$a}->{CLASS}    <=> $class_features_by_mpasmx{$b}->{CLASS}
                          } keys %class_features_by_mpasmx;

=back
        MCU_NAME     => 'PIC18F25K22',
        DIR_ADDR     => 0x300001,
        DIR_DEFAULT  => 0,
        DIR_MASK     => 0x25,
        DIR_DIGITS   => 2,
        SWITCH_COUNT => 5,
        DIR_HASH     => 0,
        SWITCHES     => [
                          {
                          MCU_NAME    => 'PIC18F25K22',
                          DIR_ADDR    => 0x300001,
                          SW_NAME     => 'FCMEN',
                          SW_MASK     => 0x40,
                          SW_DIGITS   => 2,
                          OPT_COUNT   => 2,
                          SW_HASH     => 0,
                          OPTIONS     => [
                                           {
                                           MCU_NAME    => 'PIC12F1822',
                                           SW_NAME     => 'FCMEN',
                                           OPT_NAME    => 'OFF',
                                           OPT_VALUE   => 0,
                                           OPT_DIGITS  => 2,
                                           OPT_HASH    => 0x276914DB,
                                           STRUCT_NAME => 'PIC12F1822_FCMEN_OFF_0000'
                                           },
                                           {
                                           MCU_NAME    => 'PIC18F1230',
                                           SW_NAME     => 'FCMEN',
                                           OPT_NAME    => 'ON',
                                           OPT_VALUE   => 0x40,
                                           OPT_DIGITS  => 2,
                                           OPT_HASH    => 0xE8E18517,
                                           STRUCT_NAME => 'PIC18F1230_FCMEN_ON_0040',
                                           }
                                         ],
                          STRUCT_NAME => 'PIC18F25K22_FCMEN_options'
                          },
                          {
                          MCU_NAME    => 'PIC18F25K22',
                          DIR_ADDR    => 3145729,
                          SW_NAME     => 'FOSC',
                          SW_MASK     => 0x0F,
                          SW_DIGITS   => 2,
                          OPT_COUNT   => 14,
                          SW_HASH     => 0,
                          OPTIONS     => [
                                           {
                                           MCU_NAME    => 'PIC18F23K22',
                                           SW_NAME     => 'FOSC',
                                           OPT_NAME    => 'ECHP',
                                           OPT_VALUE   => 0x04,
                                           OPT_DIGITS  => 2,
                                           OPT_HASH    => 396302728,
                                           STRUCT_NAME => 'PIC18F23K22_FOSC_ECHP_0004'
                                           },
                                           {
                                           MCU_NAME    => 'PIC18F23K22',
                                           SW_NAME     => 'FOSC',
                                           OPT_NAME    => 'ECHPIO6',
                                           OPT_VALUE   => 0x05,
                                           OPT_DIGITS  => 2,
                                           OPT_HASH    => 3369969319,
                                           STRUCT_NAME => 'PIC18F23K22_FOSC_ECHPIO6_0005'
                                           },
                                           {
                                           MCU_NAME    => 'PIC18F23K22',
                                           SW_NAME     => 'FOSC',
                                           OPT_NAME    => 'ECLP',
                                           OPT_VALUE   => 0x0C,
                                           OPT_DIGITS  => 2,
                                           OPT_HASH    => 2810990724,
                                           STRUCT_NAME => 'PIC18F23K22_FOSC_ECLP_000C'
                                           },
                                           {
                                           MCU_NAME    => 'PIC18F23K22',
                                           SW_NAME     => 'FOSC',
                                           OPT_NAME    => 'ECLPIO6',
                                           OPT_VALUE   => 0x0D,
                                           OPT_DIGITS  => 2,
                                           OPT_HASH    => 825191163,
                                           STRUCT_NAME => 'PIC18F23K22_FOSC_ECLPIO6_000D'
                                           },
                                           {
                                           MCU_NAME    => 'PIC18F23K22',
                                           SW_NAME     => 'FOSC',
                                           OPT_NAME    => 'ECMP',
                                           OPT_VALUE   => 0x0A,
                                           OPT_DIGITS  => 2,
                                           OPT_HASH    => 828687115,
                                           STRUCT_NAME => 'PIC18F23K22_FOSC_ECMP_000A'
                                           },
                                           .
                                           .
                                           .
                                           {
                                           MCU_NAME    => 'PIC12C671',
                                           SW_NAME     => 'FOSC',
                                           OPT_NAME    => 'XT',
                                           OPT_VALUE   => 0x01,
                                           OPT_DIGITS  => 2,
                                           OPT_HASH    => 0x0644FCA5,
                                           STRUCT_NAME => 'PIC12C671_FOSC_XT_0001',
                                           }
                                         ],
                          STRUCT_NAME => 'PIC18F25K22_FOSC_options'
                          },
                          .
                          .
                          .
                          {}
                        ],
        STRUCT_NAME => 'PIC18F25K22_300001_conf'
=cut

my %mcus_by_names = ();
my @sorted_mcu_names = ();

my %option_prototypes = ();
my @sorted_option_prototypes = ();
my $option_name_width = 0;

my %switch_prototypes = ();
my @sorted_switch_prototypes = ();

my %directive_prototypes = ();
my @sorted_directive_prototypes = ();
my $directive_name_width = 0;

my %mcu_prototypes = ();
my @sorted_mcu_prototypes = ();
my $feature_name_width = 0;

my $output = 'gpcfg';
my $output_c = "${output}-table.c";
my $output_h = "${output}.h";
my $name_head = 'gp_cfg_';

my $cfg_option_t        = "${name_head}option_t";
my $cfg_directive_t     = "${name_head}directive_t";
my $cfg_addr_t          = "${name_head}addr_t";
my $cfg_device_t        = "${name_head}device_t";
my $cfg_addr_hit_pair_t = "${name_head}addr_hit_pair_t";
my $cfg_addr_hit_t      = "${name_head}addr_hit_t";
my $cfg_addr_pack_t     = "${name_head}addr_pack_t";

my $out_handler;

my @mcu_missed_debug_0x8008_0x1000 =
  (
  'PIC12F1571', 'PIC12F1572', 'PIC12F1612', 'PIC12F1822',
  'PIC12F1840', 'PIC12LF1571', 'PIC12LF1572', 'PIC12LF1612',
  'PIC12LF1822', 'PIC12LF1840', 'PIC12LF1840T39A', 'PIC12LF1840T48A',
  'PIC16F1454', 'PIC16F1455', 'PIC16F1458', 'PIC16F1459',
  'PIC16F1508', 'PIC16F1509', 'PIC16F1512', 'PIC16F1513',
  'PIC16F1516', 'PIC16F1517', 'PIC16F1518', 'PIC16F1519',
  'PIC16F1526', 'PIC16F1527', 'PIC16F1574', 'PIC16F1575',
  'PIC16F1578', 'PIC16F1579', 'PIC16F1613', 'PIC16F1614',
  'PIC16F1615', 'PIC16F1618', 'PIC16F1619', 'PIC16F1703',
  'PIC16F1704', 'PIC16F1705', 'PIC16F1707', 'PIC16F1708',
  'PIC16F1709', 'PIC16F1713', 'PIC16F1716', 'PIC16F1717',
  'PIC16F1718', 'PIC16F1719', 'PIC16F1764', 'PIC16F1765',
  'PIC16F1768', 'PIC16F1769', 'PIC16F1773', 'PIC16F1776',
  'PIC16F1777', 'PIC16F1778', 'PIC16F1779',
  'PIC16F1782', 'PIC16F1783', 'PIC16F1784', 'PIC16F1786',
  'PIC16F1787', 'PIC16F1788', 'PIC16F1789', 'PIC16F1823',
  'PIC16F1824', 'PIC16F1825', 'PIC16F1826', 'PIC16F1827',
  'PIC16F1828', 'PIC16F1829', 'PIC16F1829LIN', 'PIC16F1847',
  'PIC16F1933', 'PIC16F1934', 'PIC16F1936', 'PIC16F1937',
  'PIC16F1938', 'PIC16F1939', 'PIC16F1946', 'PIC16F1947',
  'PIC16LF1454', 'PIC16LF1455', 'PIC16LF1458', 'PIC16LF1459',
  'PIC16LF1508', 'PIC16LF1509', 'PIC16LF1512', 'PIC16LF1513',
  'PIC16LF1516', 'PIC16LF1517', 'PIC16LF1518', 'PIC16LF1519',
  'PIC16LF1526', 'PIC16LF1527', 'PIC16LF1554', 'PIC16LF1559',
  'PIC16LF1574', 'PIC16LF1575', 'PIC16LF1578', 'PIC16LF1579',
  'PIC16LF1613', 'PIC16LF1614', 'PIC16LF1615', 'PIC16LF1618',
  'PIC16LF1619', 'PIC16LF1703', 'PIC16LF1704', 'PIC16LF1705',
  'PIC16LF1707', 'PIC16LF1708', 'PIC16LF1709', 'PIC16LF1713',
  'PIC16LF1716', 'PIC16LF1717', 'PIC16LF1718', 'PIC16LF1719',
  'PIC16LF1764', 'PIC16LF1765', 'PIC16LF1768', 'PIC16LF1769',
  'PIC16LF1773', 'PIC16LF1776', 'PIC16LF1777', 'PIC16LF1778',
  'PIC16LF1779', 'PIC16LF1782', 'PIC16LF1783',
  'PIC16LF1784', 'PIC16LF1786', 'PIC16LF1787', 'PIC16LF1788',
  'PIC16LF1789', 'PIC16LF1823', 'PIC16LF1824', 'PIC16LF1824T39A',
  'PIC16LF1825', 'PIC16LF1826', 'PIC16LF1827', 'PIC16LF1828',
  'PIC16LF1829', 'PIC16LF1847', 'PIC16LF1902', 'PIC16LF1903',
  'PIC16LF1904', 'PIC16LF1906', 'PIC16LF1907', 'PIC16LF1933',
  'PIC16LF1934', 'PIC16LF1936', 'PIC16LF1937', 'PIC16LF1938',
  'PIC16LF1939', 'PIC16LF1946', 'PIC16LF1947'
  );

my @mcu_missed_debug_0x8008_0x2000 =
  (
  'PIC16F18855', 'PIC16F18875',
  'PIC16LF18855', 'PIC16LF18875'
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

####################################################################################################
####################################################################################################

sub param_exist($$)
  {
  die "This option \"$_[0]\" requires a parameter.\n" if ($_[1] > $#ARGV);
  }

#---------------------------------------------------------------------------------------------------

sub Log
  {
  return if (pop(@_) > $verbose);
  foreach (@_) { print STDERR $_; }
  print STDERR "\n";
  }

#---------------------------------------------------------------------------------------------------

sub fnv1a32_str($$)
  {
  my ($String, $Hash) = @_;

  foreach (unpack('C*', $String))
    {
    $Hash ^= $_;
    $Hash *= FNV1A32_PRIME;
    $Hash &= 0xFFFFFFFF;
    }

  return $Hash;
  }

#---------------------------------------------------------------------------------------------------

sub fnv1a32_int32($$)
  {
  my ($Int, $Hash) = @_;
  my $i;

  for ($i = 4; $i; --$i)
    {
    $Hash ^= $Int & 0xFF;
    $Hash *= FNV1A32_PRIME;
    $Hash &= 0xFFFFFFFF;
    $Int >>= 8;
    }

  return $Hash;
  }

#---------------------------------------------------------------------------------------------------

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

#---------------------------------------------------------------------------------------------------

sub versionSort($$)
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
    $k = versionCompare(\$a_s[$i], \$b_s[$i]);

    return $k if ($k != 0);
    }

  return $ret;
  }

#---------------------------------------------------------------------------------------------------

	# Finds path of the latest version of mplabx.

sub find_latest_version($)
  {
  my $Dir = $_[0];
  my @dir_list;

  opendir(DIR, $Dir) || die "find_latest_version(): Can not open. -> \"$Dir\"\n";
  @dir_list = sort { versionSort($a, $b) } grep(-d "$Dir/$_" && $_ ne '.' && $_ ne '..', readdir(DIR));
  closedir(DIR);
  return ((scalar(@dir_list) > 0) ? "$Dir/$dir_list[$#dir_list]" : $Dir);
  }

#---------------------------------------------------------------------------------------------------

sub add_missing_debug($$$$)
  {
  my ($McuName, $DirRef, $DirAddr, $Mask) = @_;
  my $sw_ref =
    {
    MCU_NAME    => $McuName,
    DIR_ADDR    => $DirAddr,
    SW_NAME     => 'DEBUG',
    SW_MASK     => $Mask,
    SW_DIGITS   => 4,
    OPT_COUNT   => 2,
    OPTIONS     => [
                     {
                     MCU_NAME    => $McuName,
                     SW_NAME     => 'DEBUG',
                     OPT_NAME    => 'ON',
                     OPT_VALUE   => 0,
                     OPT_DIGITS  => 4,
                     OPT_HASH    => 0,
                     STRUCT_NAME => "${McuName}_DEBUG_ON_0000"
                     },
                     {
                     MCU_NAME    => $McuName,
                     SW_NAME     => 'DEBUG',
                     OPT_NAME    => 'OFF',
                     OPT_VALUE   => $Mask,
                     OPT_DIGITS  => 4,
                     OPT_HASH    => 0,
                     STRUCT_NAME => sprintf("${McuName}_DEBUG_OFF_%04u", $Mask)
                     }
                   ],
    SW_HASH     => 0,
    STRUCT_NAME => "${McuName}_DEBUG"
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

sub read_device_informations()
  {
  my ($mcu_name, $inc, $lkr, $class_name);
  my $mcu_features    = undef;
  my $directive       = undef;
  my $directive_addr  = 0;
  my $directive_count = 0;
  my $switch_info     = undef;
  my $switch_count    = 0;
  my $option_count    = 0;
  my $state           = ST_WAIT;
  my ($directive_mask, $digits, $debug_present);

  open(INFO, '<', $dev_info_path) || die "Could not open for reading: $dev_info_path\n";

  Log("Reads the config options from $dev_info_path file.", 4);

  while (<INFO>)
    {
    chomp;
    s/\r$//o;
    s/^<|>$//go;

    my @fields = split('><', $_, -1);

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

      if ($fields[2] =~ /^$pic_name_mask$/io)
        {
        ($mcu_name, $class_name, $directive_count) = (uc($fields[2]), $fields[3], hex($fields[12]));
        $switch_count  = 0;
        $option_count  = 0;
        $debug_present = FALSE;

        my $tr = $class_features_by_mpasmx{$class_name};

        $mcu_name =~ s/^PICRF/RF/o;
        die "Unknown class of $mcu_name MCU!" if (! defined($tr));

        $digits = $tr->{DIR_DIGITS};

        $mcu_features =
          {
          MCU_CLASS   => $tr->{CLASS},     # Class of MCU. (PROC_CLASS_PIC12yy)
          MCU_NAME    => $mcu_name,
          DIR_COUNT   => $directive_count, # Number of Configuration bytes/words.
          DIR_SUBST   => undef,
          DIR_DIGITS  => $digits,
          DIRECTIVES  => [],
          MCU_HASH    => 0,
          STRUCT_NAME => "${mcu_name}_addrs"
          };

        $state = ST_LISTEN;
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
            else
              {
              $mcu_features->{DIR_COUNT} -= 1;
              }
            }
          else
            {
            $directive =
              {
              MCU_NAME     => $mcu_name,
              DIR_ADDR     => $directive_addr,
              DIR_DEFAULT  => $dir_default,
              DIR_MASK     => 0,
              DIR_DIGITS   => $digits,
              SWITCH_COUNT => $switch_count,
              SWITCHES     => [],
              DIR_HASH     => 0,
              STRUCT_NAME  => sprintf("${mcu_name}_%0*X", $digits, $directive_addr)
              };

            $directive_mask = 0;
            push(@{$mcu_features->{DIRECTIVES}}, $directive);
            }

          --$directive_count;

          if ($directive_count == 0)
            {
            if ($mcu_features->{DIR_COUNT} > 1)
              {
              @{$mcu_features->{DIRECTIVES}} = sort {$a->{DIR_ADDR} <=> $b->{DIR_ADDR}} @{$mcu_features->{DIRECTIVES}};
              }
            }
          } # when ('CONFIGREG_INFO_TYPE')

        when ('SWITCH_INFO_TYPE')
          {
          # <SWITCH_INFO_TYPE><FOSC><Oscillator Selection bits><7><8>
          # <SWITCH_INFO_TYPE><DEBUG><In-Circuit Debugger Mode bit><2000><2>

          die "Too much the number of \"SWITCH_INFO_TYPE\"!\n" if ($switch_count <= 0);

          my ($sw_name, $sw_mask);

          ($sw_name, $sw_mask, $option_count) = ($fields[1], hex($fields[3]), hex($fields[4]));

          # Microchip bug of the 18f47k40 and 18lf47k40 devices: nDEBUG, nXINST
          $sw_name =~ s/^n//o;

          if ($option_count <= 0)
            {
            $directive->{SWITCH_COUNT} -= 1;
            printf STDERR "Database error in descriptor of $mcu_name at 0x%06X, $sw_name: This SWITCH_INFO_TYPE empty!\n", $directive_addr;
            }
          else
            {
            $switch_info =
              {
              MCU_NAME    => $mcu_name,
              DIR_ADDR    => $directive_addr,
              SW_NAME     => $sw_name,
              SW_MASK     => $sw_mask,
              SW_DIGITS   => $digits,
              OPT_COUNT   => $option_count,
              OPTIONS     => [],
              SW_HASH     => 0,
              STRUCT_NAME => "${mcu_name}_${sw_name}"
              };

            $debug_present = TRUE if ($sw_name eq 'DEBUG');
            push(@{$directive->{SWITCHES}}, $switch_info);
            $directive_mask |= $sw_mask;
            }

          --$switch_count;

          if ($switch_count == 0)
            {
            if ($directive_addr == 0x8008 && ! $debug_present &&$mcu_features->{MCU_CLASS} == PROC_CLASS_PIC14E)
              {
              if ($mcu_name ~~ @mcu_missed_debug_0x8008_0x1000)
                {
                $directive_mask |= add_missing_debug($mcu_name, $directive, $directive_addr, 0x1000);
                }
              elsif ($mcu_name ~~ @mcu_missed_debug_0x8008_0x2000)
                {
                $directive_mask |= add_missing_debug($mcu_name, $directive, $directive_addr, 0x2000);
                }
              }

            if ($directive->{SWITCH_COUNT} > 1)
              {
              @{$directive->{SWITCHES}} = sort {$a->{SW_MASK} <=> $b->{SW_MASK}} @{$directive->{SWITCHES}};
              }

            $directive->{DIR_MASK}     = $directive_mask;
            # This an ex post correction which unfortunately sometimes necessary. (e.g. PIC18C242)
            $directive->{DIR_DEFAULT} &= $directive_mask if ($limit_defaults);
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

          my ($opt_name, $opt_value) = ($fields[1], hex($fields[3]));
          my $sw_name = $switch_info->{SW_NAME};

          if ($opt_name =~ /^reserved$/io)
            {
            $opt_name = 'RESERVED';
            }

          my $option =
            {
            MCU_NAME    => $mcu_name,
            SW_NAME     => $sw_name,
            OPT_NAME    => $opt_name,
            OPT_VALUE   => $opt_value,
            OPT_DIGITS  => $digits,
            OPT_HASH    => 0,
            STRUCT_NAME => sprintf("${mcu_name}_${sw_name}_${opt_name}_%0*X", $digits, $opt_value)
            };

          push(@{$switch_info->{OPTIONS}}, $option);
          --$option_count;

          if ($option_count == 0)
            {
            if ($switch_info->{OPT_COUNT} > 1)
              {
              @{$switch_info->{OPTIONS}} = sort {$a->{OPT_VALUE} <=> $b->{OPT_VALUE}} @{$switch_info->{OPTIONS}};
              }
            }
          } # when ('SETTING_VALUE_TYPE')
        } # given ($fields[0])

      if ($directive_count == 0 && $switch_count == 0 && $option_count == 0)
        {
        # All information is together.
        die "The $mcu_name MCU already exist!" if (defined($mcus_by_names{$mcu_name}));

        $mcus_by_names{$mcu_name} = $mcu_features;
        $state = ST_WAIT;
        }
      } # if ($state == ST_LISTEN)
    } # while (<INFO>)

  close(INFO);
  @sorted_mcu_names = sort { $a cmp $b } keys %mcus_by_names;
  }

#---------------------------------------------------------------------------------------------------

sub make_dir_hash($$)
  {
  my ($DirRef, $Hash) = @_;

  $Hash = fnv1a32_int32($DirRef->{DIR_ADDR}, $Hash);
  $Hash = fnv1a32_int32($DirRef->{DIR_DEFAULT}, $Hash);
  $Hash = fnv1a32_int32($DirRef->{DIR_MASK}, $Hash);
  return fnv1a32_int32($DirRef->{SWITCH_COUNT}, $Hash);
  }

#---------------------------------------------------------------------------------------------------

sub make_sw_hash($$)
  {
  my ($SwRef, $Hash) = @_;

  $Hash = fnv1a32_str($SwRef->{SW_NAME}, $Hash);
  $Hash = fnv1a32_str($SwRef->{SW_MASK}, $Hash);
  return fnv1a32_int32($SwRef->{OPT_COUNT}, $Hash);
  }

#---------------------------------------------------------------------------------------------------

sub make_opt_hash($$)
  {
  my ($OptRef, $Hash) = @_;

  $Hash = fnv1a32_str($OptRef->{OPT_NAME}, $Hash);
  return fnv1a32_int32($OptRef->{OPT_VALUE}, $Hash);
  }

#---------------------------------------------------------------------------------------------------

sub collect_prototypes()
  {
  my ($mcu_name, $mcu_features, $mcu_hash);
  my ($directives, $dir_count, $dir_ref, $dir_hash);
  my ($switches,   $sw_count,  $sw_ref,  $sw_hash);
  my ($options,    $opt_count, $opt_ref, $opt_hash);
  my ($i, $j, $k, $len, $proto_ref);

  Log('Collect prototypes.', 4);

  $feature_name_width = 0;
  $directive_name_width = 0;
  $option_name_width = 0;
  foreach $mcu_name (@sorted_mcu_names)
    {
    $mcu_features = $mcus_by_names{$mcu_name};
    $dir_count    = $mcu_features->{DIR_COUNT};
    $mcu_hash     = fnv1a32_int32($dir_count, FNV1A32_INIT);

    $directives = $mcu_features->{DIRECTIVES};

    for ($i = 0; $i < $dir_count; ++$i)
      {
      $dir_ref  = $directives->[$i];
      $mcu_hash = make_dir_hash($dir_ref, $mcu_hash);
      $dir_hash = make_dir_hash($dir_ref, FNV1A32_INIT);

      $switches = $dir_ref->{SWITCHES};
      $sw_count = $dir_ref->{SWITCH_COUNT};

      for ($j = 0; $j < $sw_count; ++$j)
        {
        $sw_ref   = $switches->[$j];
        $mcu_hash = make_sw_hash($sw_ref, $mcu_hash);
        $dir_hash = make_sw_hash($sw_ref, $dir_hash);
        $sw_hash  = make_sw_hash($sw_ref, FNV1A32_INIT);

        $options   = $sw_ref->{OPTIONS};
        $opt_count = $sw_ref->{OPT_COUNT};

        for ($k = 0; $k < $opt_count; ++$k)
          {
          $opt_ref  = $options->[$k];
          $mcu_hash = make_opt_hash($opt_ref, $mcu_hash);
          $dir_hash = make_opt_hash($opt_ref, $dir_hash);
          $sw_hash  = make_opt_hash($opt_ref, $sw_hash);
          $opt_hash = make_opt_hash($opt_ref, FNV1A32_INIT);

          $proto_ref = $option_prototypes{$opt_hash};

          if (defined($proto_ref))
            {
            $options->[$k] = $proto_ref;
            }
          else
            {
            #
            # A new prototype.
            #
            $option_prototypes{$opt_hash} = $opt_ref;
            $len = length($opt_ref->{STRUCT_NAME});
            $option_name_width = $len if ($option_name_width < $len);
            }

          $opt_ref->{OPT_HASH} = $opt_hash;
          } # for ($k = 0; $k < $opt_count; ++$k)

        $sw_ref->{SW_HASH} = $sw_hash;

        $proto_ref = $switch_prototypes{$sw_hash};

        if (defined($proto_ref))
          {
          $switches->[$j] = $proto_ref;
          }
        else
          {
          #
          # A new prototype.
          #
          $switch_prototypes{$sw_hash} = $sw_ref;
          }
        } # for ($j = 0; $j < $sw_count; ++$j)

      $dir_ref->{DIR_HASH} = $dir_hash;

      $proto_ref = $directive_prototypes{$dir_hash};

      if (defined($proto_ref))
        {
        $directives->[$i] = $proto_ref;
        }
      else
        {
        #
        # A new prototype.
        #
        $directive_prototypes{$dir_hash} = $dir_ref;
        $len = length($dir_ref->{STRUCT_NAME});
        $directive_name_width = $len if ($directive_name_width < $len);
        }
      } # for ($i = 0; $i < $dir_count; ++$i)

    $mcu_features->{MCU_HASH} = $mcu_hash;

    $proto_ref = $mcu_prototypes{$mcu_hash};

    if (defined($proto_ref))
      {
      $mcus_by_names{$mcu_name} = $proto_ref;
      }
    else
      {
      #
      # A new prototype.
      #
      $mcu_prototypes{$mcu_hash} = $mcu_features;
      $len = length($mcu_features->{STRUCT_NAME});
      $feature_name_width = $len if ($feature_name_width < $len);
      }
    } # foreach $mcu_name (@sorted_mcu_names)

  @sorted_mcu_prototypes = sort {$mcu_prototypes{$a}->{MCU_NAME} cmp $mcu_prototypes{$b}->{MCU_NAME}} (keys %mcu_prototypes);

  @sorted_directive_prototypes = sort {versionSort($directive_prototypes{$a}->{MCU_NAME}, $directive_prototypes{$b}->{MCU_NAME}) ||
                                       $directive_prototypes{$a}->{DIR_ADDR} <=> $directive_prototypes{$b}->{DIR_ADDR}} (keys %directive_prototypes);

  @sorted_switch_prototypes = sort {versionSort($switch_prototypes{$a}->{MCU_NAME}, $switch_prototypes{$b}->{MCU_NAME}) ||
                                    versionSort($switch_prototypes{$a}->{SW_MASK}, $switch_prototypes{$b}->{SW_MASK}) ||
                                    $switch_prototypes{$a}->{DIR_ADDR} <=> $switch_prototypes{$b}->{DIR_ADDR}} (keys %switch_prototypes);

  @sorted_option_prototypes = sort {versionSort($option_prototypes{$a}->{SW_NAME}, $option_prototypes{$b}->{SW_NAME}) ||
                                    versionSort($option_prototypes{$a}->{OPT_NAME}, $option_prototypes{$b}->{OPT_NAME}) ||
                                    $option_prototypes{$a}->{OPT_VALUE} <=> $option_prototypes{$b}->{OPT_VALUE} ||
                                    $option_prototypes{$a}->{MCU_NAME} cmp $option_prototypes{$b}->{MCU_NAME}} (keys %option_prototypes);

  }

#---------------------------------------------------------------------------------------------------

sub print_option_prototypes()
  {
  my $opt_ref;

  Log('Prints option prototypes.', 4);

  foreach (@sorted_option_prototypes)
    {
    $opt_ref = $option_prototypes{$_};
    printf $out_handler "static const $cfg_option_t %-*s = { %-16s, 0x%0*X };\n",
                        $option_name_width, $opt_ref->{STRUCT_NAME}, "\"$opt_ref->{OPT_NAME}\"",
                        $opt_ref->{OPT_DIGITS}, $opt_ref->{OPT_VALUE};
    }

  print $out_handler "\n";
  }

#---------------------------------------------------------------------------------------------------

sub print_switch_prototypes()
  {
  my ($i, $sw_ref, $opt_count, $options, $opt_ref);

  Log('Prints switch prototypes.', 4);

  foreach (@sorted_switch_prototypes)
    {
    $sw_ref = $switch_prototypes{$_};
    print $out_handler "static const $cfg_option_t *$sw_ref->{STRUCT_NAME}\[\] = {\n";

    $options   = $sw_ref->{OPTIONS};
    $opt_count = $sw_ref->{OPT_COUNT};
    $i = 0;
    while (1)
      {
      $opt_ref = $options->[$i];
      ++$i;
      print $out_handler "  \&$opt_ref->{STRUCT_NAME}";

      if ($i >= $opt_count)
        {
        print $out_handler "\n";
        last;
        }

      print $out_handler ",\n";
      }

    print $out_handler "};\n\n";
    }
  }

#---------------------------------------------------------------------------------------------------

sub print_directive_prototypes()
  {
  my ($dir_ref, $sw_count, $switches, $sw_ref, $max_sw, $max_st, $len, $i);

  Log('Prints directive prototypes.', 4);

  foreach (@sorted_directive_prototypes)
    {
    $dir_ref  = $directive_prototypes{$_};
    $switches = $dir_ref->{SWITCHES};
    $sw_count = $dir_ref->{SWITCH_COUNT};

    print $out_handler "static const $cfg_directive_t $dir_ref->{STRUCT_NAME}\[\] = {\n";

    $max_sw = 0;
    $max_st = 0;
    foreach (@{$switches})
      {
      $len = length($_->{SW_NAME});
      $max_sw = $len if ($max_sw < $len);
      $len = length($_->{STRUCT_NAME});
      $max_st = $len if ($max_st < $len);
      }

    $max_sw += 2;       # For quotation marks.
    $i = 0;
    while (1)
      {
      $sw_ref = $switches->[$i];
      ++$i;
      printf $out_handler "  { %-*s, 0x%04X, %2u, %-*s }", $max_sw, "\"$sw_ref->{SW_NAME}\"",
                          $sw_ref->{SW_MASK}, $sw_ref->{OPT_COUNT}, $max_st, $sw_ref->{STRUCT_NAME};

      if ($i >= $sw_count)
        {
        print $out_handler "\n";
        last;
        }

      print $out_handler ",\n";
      }

    print $out_handler "};\n\n";
    }
  }

#---------------------------------------------------------------------------------------------------

sub print_mcu_prototypes()
  {
  my ($mcu_features, $directives, $dir_count, $i, $dir_ref);

  Log('Prints mcu prototypes.', 4);

  foreach (@sorted_mcu_prototypes)
    {
    $mcu_features = $mcu_prototypes{$_};
    $directives   = $mcu_features->{DIRECTIVES};
    $dir_count    = $mcu_features->{DIR_COUNT};

    print $out_handler "static const $cfg_addr_t $mcu_features->{STRUCT_NAME}\[\] = {\n";

    $i = 0;
    while (1)
      {
      $dir_ref = $directives->[$i];
      ++$i;

      printf $out_handler "  { 0x%06X, 0x%0*X, %2u, %-*s }", $dir_ref->{DIR_ADDR}, $dir_ref->{DIR_DIGITS},
                          $dir_ref->{DIR_DEFAULT}, $dir_ref->{SWITCH_COUNT}, $directive_name_width, $dir_ref->{STRUCT_NAME};

      if ($i >= $dir_count)
        {
        print $out_handler "\n";
        last;
        }

      print $out_handler ",\n";
      }

    print $out_handler "};\n\n";
    }
  }

#---------------------------------------------------------------------------------------------------

sub print_device_table()
  {
  my ($mcu_count, $i, $mcu_name, $mcu_features);

  Log('Prints device table.', 4);

  print $out_handler "const $cfg_device_t ${name_head}devices\[\] = {\n";

  $mcu_count = @sorted_mcu_names;
  $i = 0;
  while (1)
    {
    $mcu_name = $sorted_mcu_names[$i];
    ++$i;
    $mcu_features = $mcus_by_names{$mcu_name};
    printf $out_handler "  { %-17s, %2u, %-*s }", "\"$mcu_name\"", $mcu_features->{DIR_COUNT},
                        $feature_name_width, $mcu_features->{STRUCT_NAME};

    if ($i >= $mcu_count)
      {
      print $out_handler "\n";
      last;
      }

    print $out_handler ",\n";
    }

  print $out_handler "};\n\nconst int ${name_head}device_count = $mcu_count;\n";
  }

#---------------------------------------------------------------------------------------------------

sub print_license()
  {
  print $out_handler <<EOT
/*  Copyright (C) 2014-2015 Molnar Karoly <molnarkaroly\@users.sf.net>

This file is part of gputils.

Gputils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Gputils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with gputils; see the file COPYING. If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA. */

EOT
;
  }

#---------------------------------------------------------------------------------------------------

sub print_header()
  {
  my $guard = '__' . uc($output) . '_H__';

  print $out_handler <<EOT

\#ifndef $guard
\#define $guard

/* This file is generated automatically by the $PROGRAM ${time_str}. */

/* $output_h - header file for pic configurations
   Copyright (C) 2006
   Michael Ballbach */

EOT
;
  print_license();
  print $out_handler <<EOT
/* A directive value. */
typedef struct {
  const char *name;                         /* Name of the value. */
  unsigned short value;                     /* The value. */
} $cfg_option_t;

/* A directive, i.e., FOSC. */
typedef struct {
  const char *name;                         /* Name of the directive. */
  unsigned short mask;                      /* Mask of words in the config address that apply to its value. */
  unsigned int option_count;                /* Number of possible values. */
  const $cfg_option_t **options;          /* Array of addresses of values. */
} $cfg_directive_t;

/* One particular configuration address, i.e., 0x300001. */
typedef struct {
  unsigned int address;                     /* The address. */
  unsigned short def_value;                 /* Its default value. */
  unsigned int directive_count;             /* Count of relevant directives. */
  const $cfg_directive_t *directives;     /* Array of directives. */
} $cfg_addr_t;

/* A device - that is, a collection of configuration addresses. */
typedef struct {
  const char *name;                         /* Name of the device. */
  unsigned int address_count;               /* Number of configuration addresses. */
  const $cfg_addr_t *addresses;           /* Array of configuration addresses. */
} $cfg_device_t;

\#define GP_CFG_ADDR_HIT_MAX         16
\#define GP_CFG_ADDR_PACK_MAX        16

typedef struct {
  const $cfg_directive_t *directive;
  const $cfg_option_t *option;
} $cfg_addr_hit_pair_t;

typedef struct {
  unsigned int max_dir_width;               /* The size of the longest directive name. */
  unsigned short def_value;                 /* Default value of this $cfg_addr_t. */
  unsigned int pair_count;                  /* Number of the pairs. */
  $cfg_addr_hit_pair_t pairs[GP_CFG_ADDR_HIT_MAX];
} $cfg_addr_hit_t;

typedef struct {
  unsigned int max_dir_width;               /* The size of the longest directive name. */
  unsigned int hit_count;                   /* Number of the hits. */
  $cfg_addr_hit_t hits[GP_CFG_ADDR_PACK_MAX];
} $cfg_addr_pack_t;

extern const $cfg_device_t ${name_head}devices[];
extern const int ${name_head}device_count;

const $cfg_device_t *${name_head}find_pic(const char *Pic);

const $cfg_device_t *${name_head}find_pic_multi_name(unsigned int Count, const char *const *Pics);

const $cfg_directive_t *${name_head}find_directive(const $cfg_device_t *Device, const char *Directive,
                                                unsigned int *Out_config_addr, unsigned short *Out_def_value);

const $cfg_option_t *${name_head}find_option(const $cfg_directive_t *Directive, const char *Option);

const $cfg_addr_t *${name_head}find_config(const $cfg_device_t *Device, unsigned int Address);

unsigned short ${name_head}get_default(const $cfg_device_t *Device, unsigned int Address);

unsigned int ${name_head}decode_directive(const $cfg_device_t *Device, unsigned int Address, unsigned int Value,
                                     $cfg_addr_hit_t *Hit);

\#endif /* $guard */
EOT
;
  }

#---------------------------------------------------------------------------------------------------

sub usage()
  {
  print <<EOT
Usage: $PROGRAM [options]

    Options are:

        -i <path> or --info-path <path>

            The program on this path looks for the 8bit_device.info file.
            (Default: $dev_info_path)

        -l or --limit-defaults

            Limits the default value of the directives according to the existing config bits.
            (Default: No limitation, remains the original values.)

        -v <level> or --verbose <level>

            It provides information on from the own operation.
            Possible value of the level between 0 and 10. (Default: 0)

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

my $user_info_path = FALSE;

for (my $i = 0; $i < @ARGV; )
  {
  my $opt = $ARGV[$i++];

  given ($opt)
    {
    when (/^-(i|-info-path)$/o)
      {
      param_exist($opt, $i);
      $dev_info_path = $ARGV[$i++];
      $user_info_path = TRUE;
      }

    when (/^-(l|-limit-defaults)$/o)
      {
      $limit_defaults = TRUE;
      }

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

if (!$user_info_path)
  {
  $mplabx_path   = find_latest_version($mplabx_root_path);
  $dev_info_path = "$mplabx_path/mpasmx/$mplabx_dev_info";
  }

read_device_informations();
add_missing_directives();
collect_prototypes();

$time_str = strftime('%F %T UTC', gmtime);

open($out_handler, '>', $output_h) || die "Could not create the \"$output_h\" file!\n";
Log("Creates the header of register database.", 1);
print_header();
close($out_handler);

open($out_handler, '>', $output_c) || die "Could not create the \"$output_c\" file!\n";
Log("Creates the register database.", 1);

print $out_handler "\n/* This file is generated automatically by the $PROGRAM $time_str. */\n\n";
print_license();
print $out_handler "#include \"$output_h\"\n\n";
print_option_prototypes();
print_switch_prototypes();
print_directive_prototypes();
print_mcu_prototypes();
print_device_table();
close($out_handler);

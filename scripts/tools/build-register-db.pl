#!/usr/bin/perl -w

=back

    Copyright (C) 2014 Molnar Karoly <molnarkaroly@users.sf.net>

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

    This program prepares the gpreg-table.c and gpregister.h files.

    $Id$
=cut

use strict;
use warnings;
use 5.12.0;                     # when (regex)
use feature 'switch';           # Starting from 5.10.1.
use POSIX 'strftime', 'UINT_MAX';

use constant FALSE => 0;
use constant TRUE  => 1;

use constant ST_NONE     => 0;
use constant ST_REG_ADDR => 1;
use constant ST_REG1_DEF => 2;
use constant ST_REG2_DEF => 3;
use constant ST_RAM_DEF  => 4;

use constant REG_LINE_LENGTH => 26;

my $PROGRAM  = 'build-register-db.pl';
my $time_str = '';

my $reg_line_t  = 'gp_register_t';
my $reg_table_t = 'gp_register_table_t';
my $reg_table   = 'gp_register_db';
my $output_c    = 'gpreg-table.c';
my $output_h    = 'gpregister.h';

my $gputils_path   = "$ENV{HOME}/svn_snapshots/gputils/gputils";
my $gp_header_path = '';
my $gpprocessor_c  = 'gpprocessor.c';
my $gpproc_path;

my $gputils_inc;
my $gputils_lkr;

my $verbose = 0;

my $out_handler;

=back
        The structure of one element of the %mcus_by_name hash:

        {
        NAME      => '',
        INCLUDE   => '',
        REGISTERS => {},
        ADDR_NUM  => 0
        }
=cut

my %mcus_by_name;
my @mcu_name_array;

        # Here those names to be entered that are defective.
        #  BAD  => 'GOOD'  or
        # 'BAD' => 'GOOD'

my %correction_of_names =
  (
  OPTION => 'OPTION_REG'
  );

#-----------------------------------------------

=back
        The structure of one element of the %registers hash:

        {
        MCU_NAME  => '',      The name of owner MCU.
        REG_NAME  => '',      The name of register.
        REG_ADDR  => 0,       The address of register.
        USE_COUNT => 0        Shows the register get reference how many times.
        }
=cut

my %registers = ();

####################################################################################################
####################################################################################################
####################################################################################################
####################################################################################################

sub basename($)
  {
  return ($_[0] =~ /([^\/]+)$/) ? $1 : '';
  }

#---------------------------------------------------------------------------------------------------

sub param_exist($$)
  {
  die "This option \"$_[0]\" requires a parameter.\n" if ($_[1] > $#ARGV);
  }

#---------------------------------------------------------------------------------------------------

sub str2int($)
  {
  my $Str = $_[0];

  return hex($1)   if ($Str =~ /^H'([[:xdigit:]]+)'$/io);
  return hex($1)   if ($Str =~ /^0x([[:xdigit:]]+)$/io);
  return int($Str) if ($Str =~ /^-?\d+$/o);

  die "str2int(): This string not integer: \"$Str\"";
  }

#---------------------------------------------------------------------------------------------------

sub Log
  {
  return if (pop(@_) > $verbose);
  foreach (@_) { print $_; }
  print "\n";
  }

#-------------------------------------------------------------------------------

sub align($$)
  {
  my $text = $_[0];
  my $al   = $_[1] - length($text);

        # One space will surely becomes behind it.
  $al = 1 if ($al < 1);

  return ($text . ' ' x $al);
  }

#---------------------------------------------------------------------------------------------------

        # If the $Word included in a list then it will replace.

sub correct_name($$)
  {
  my ($Word, $McuName) = @_;
  my $corr = $correction_of_names{$Word};

  if (defined($corr))
    {
    Log("$Word --> $corr ($McuName)", 7);
    return $corr;
    }

  return $Word;
  }

#---------------------------------------------------------------------------------------------------

        # Adds to the list the $Name register.

sub new_register($$$)
  {
  my ($RegName, $Address, $refMcu) = @_;
  my $mcu_name  = $refMcu->{NAME};
  my $reg_name  = correct_name($RegName, $mcu_name);
  my $reg_array = $registers{$reg_name};
  my $reg;

  if (defined($reg_array))
    {
    foreach $reg (@{$registers{$reg_name}})
      {
      if ($reg->{REG_ADDR} == $Address)
        {
        push(@{$refMcu->{REGISTERS}{$Address}}, $reg) if ($Address >= 0);
        return;
        }
      }
    }

  $reg = {
         MCU_NAME  => $mcu_name,
         REG_NAME  => $reg_name,
         REG_ADDR  => $Address,
         USE_COUNT => 0
         };

  push(@{$refMcu->{REGISTERS}{$Address}}, $reg) if ($Address >= 0);
  push(@{$registers{$reg_name}}, $reg);
  }

#---------------------------------------------------------------------------------------------------

        # In the $Queue are in register's names.

sub add_registers($$)
  {
  my ($Queue, $refMcu) = @_;

  return if (! @{$Queue});

  foreach (@{$Queue})
    {
    next if ($_ eq 'and');      # This here easiest to filter out.

    new_register($_, -1, $refMcu);
    }

  @{$Queue} = ();
  }

#---------------------------------------------------------------------------------------------------

        # Reads the entire content of the $File.

sub read_content_from_header($)
  {
  my $McuName = $_[0];
  my ($mcu_data, $state, $name, $addr);
  my @queue;

  $mcu_data = $mcus_by_name{$McuName};
  $name     = $mcu_data->{INCLUDE};

  print STDERR "Header file not exist! ($mcu_data->{NAME})\n" if ($name eq '');
  
  $name = "$gputils_path/header/$name";

  open(IN, '<', $name) || die "Can not open the $name header file!\n";

  $state = ST_NONE;
  @queue = ();

  foreach (grep(! /^\s*$/o, <IN>))
    {
    chomp;
    s/\r$//o;
    s/^\s*|\s*$//go;

    my $line = $_;

    Log("#### \"$line\"", 8);

    given ($state)
      {
      when (ST_NONE)
        {
        Log(":::: ST_NONE ($line) ($McuName)", 4);

        $state = ST_REG_ADDR if ($line =~ /^;-+\s*Register\s+Files\s*-+$/io);
        }

     when (ST_REG_ADDR)
        {
        Log(":::: ST_REG_ADDR ($line) ($McuName)", 4);

        if ($line =~ /^;-+\s*(.+)Bits\s*-+$/io)
          {
        # ;----- STKPTR Bits --------------------------------------------------------
        # ;----- UIR/UIE Bits -------------------------------------------------------
        # ;----- TXSTA, TXSTA1 and TXSTA2 Bits --------------------------------------
        #

        # Therefore need the queue because more register names can be on one line.

          @queue = ($1 =~ /([^\s,\/]+)/go);
          $state = ST_REG1_DEF;
          }
        elsif ($line =~ /^(\w+)\s+EQU\s+([\w']+)$/io)  #'
          {
        # PORTC     EQU  H'0007'
        #

          new_register($1, str2int($2), $mcu_data);
          }
        } # when (ST_REG_ADDR)

      when (ST_REG1_DEF)
        {
        Log(":::: ST_REG1_DEF ($line) ($McuName)", 4);

        if ($line =~ /^;\s*I\/O\s+Pin\s+Name\s+Definitions?$/io)
          {
        # ;       I/O Pin Name Definitions
        #

          Log("1 +++ add_register()", 6);
          add_registers(\@queue, $mcu_data);
          $state = ST_REG2_DEF;
          }
        elsif ($line =~ /^;\s*RAM\s+Definitions?$/io)
          {
        # ;       RAM Definition
        # ;       RAM Definitions
        #

          Log("2 +++ add_register()", 6);
          add_registers(\@queue, $mcu_data);
          $state = ST_RAM_DEF;
          }
        elsif ($line =~ /^;-+\s*(.+)Bits\s*-+$/io)
          {
          my $name = $1;

          Log("3 +++ add_register()", 6);
          add_registers(\@queue, $mcu_data);

        # Therefore need the queue because more register names can be on one line.

          @queue = ($name =~ /([^\s,\/]+)/go);
          }
        } # when (ST_REG1_DEF)

      when (ST_REG2_DEF)
        {
        Log(":::: ST_REG2_DEF ($line) ($McuName)", 4);

        if ($line =~ /^;\s*RAM\s+Definitions?$/io)
          {
        # ;       RAM Definition
        # ;       RAM Definitions
        #

          Log("4 +++ add_register()", 6);
          add_registers(\@queue, $mcu_data);
          $state = ST_RAM_DEF;
          }
        elsif ($line =~ /^;-+\s*([^-]+)\s*-+$/io)
          {
          my $name = $1;

          Log("5 +++ add_register()", 6);
          add_registers(\@queue, $mcu_data);

        # Therefore need the queue because more register names can be on one line.

          @queue = ($name =~ /([^\s,\/]+)/go);
          }
        } # when (ST_REG2_DEF)

      when (ST_RAM_DEF)
        {
        Log(":::: ST_RAM_DEF ($line) ($McuName)", 4);
        }
      } # given ($state)
    } # foreach (grep(! /^\s*$/o, <IN>))

  close(IN);
  }

#---------------------------------------------------------------------------------------------------

        # Extract the MCU names from gpprocessor.c file and read the register names
        # from specific header file.

sub read_all_mcu_registers($)
  {
  my $Dir = $_[0];

  open(LIB, '<', $gpproc_path) || die "read_all_mcu_registers(): Can not open. -> \"$gpproc_path\"\n";

        # static struct px pics[] = {
        # { PROC_CLASS_PIC14E   , "__16F1526"     , { "pic16f1526"     , "p16f1526"       , "16f1526"         }, 0x1526,  4,   32, 0x001FFF, 0x002000, {       -1,       -1 }, { 0x008000, 0x008003 }, { 0x008007, 0x008008 }, {       -1,       -1 }, "p16f1526.inc"     , "16f1526_g.lkr"     , 0 },
        # { PROC_CLASS_EEPROM8  , "__EEPROM8"     , { "eeprom8"        , "eeprom8"        , "eeprom8"         }, 0x1FFF,  0,    0, 0x0000FF,       -1, {       -1,       -1 }, {       -1,       -1 }, {       -1,       -1 }, {       -1,       -1 }, NULL               , NULL                , 0 },
        # { PROC_CLASS_PIC14    , "__RF675H"      , { "rf675h"         , "rf675h"         , "rf675h"          }, 0x4675,  1,    2, 0x00217F, 0x000400, { 0x0003FF, 0x0020FF }, { 0x002000, 0x002003 }, { 0x002007, 0x002007 }, { 0x002100, 0x00217F }, "rf675h.inc"       , "rf675h_g.lkr"      , 0 },

  my $in_table = FALSE;

  %mcus_by_name = ();

  while (<LIB>)
    {
    chomp;

    if (! $in_table)
      {
      $in_table = TRUE if (/^\s*static\s+struct\s+px\s+pics\[\s*\]\s*=\s*\{\s*$/io);
      }
    elsif (/\{\s*PROC_CLASS_\w+\s*,\s*"\w+"\s*,\s*\{\s*"\w+"\s*,\s*"(\w+)"\s*,\s*"\w+"\s*}\s*,\s*[\w-]+\s*,\s*[\w-]+\s*,\s*[\w-]+\s*,\s*\S+\s*,\s*\S+\s*,\s*\{\s*\S+\s*,\s*\S+\s*\}\s*,\s*{\s*\S+\s*,\s*\S+\s*\}\s*,\s*{\s*\S+\s*,\s*\S+\s*\}\s*,\s*{\s*\S+\s*,\s*\S+\s*\}\s*,\s*\"?([\.\w]+)\"?\s*,\s*\"?[\.\w]+\"?\s*,\s*\d+\s*\}/io)
      {
      my $middle_name = $1;
      my $mcu_data = {
                     NAME      => $middle_name,
                     INCLUDE   => $2,
                     REGISTERS => {},
                     ADDR_NUM  => 0
                     };

      if ($middle_name eq 'p16c52')
        {
	if (! -e "$Dir/$mcu_data->{INCLUDE}")
	  {
          printf STDERR "This header not exist: \"$Dir/$mcu_data->{INCLUDE}\"\n";
	  $mcu_data->{INCLUDE} = '';
	  }

        $mcus_by_name{$middle_name} = $mcu_data;
        }
      elsif ($middle_name ne 'p16c5x' &&
             $middle_name !~ /^p1[6-8]cxx$/o &&
             $middle_name !~ /^p14000/o &&
             $middle_name !~ /^eeprom/o &&
             $middle_name =~ /^(p1|ps|rf)/o)
        {
	if (! -e "$Dir/$mcu_data->{INCLUDE}")
	  {
          printf STDERR "This header not exist: \"$Dir/$mcu_data->{INCLUDE}\"\n";
	  $mcu_data->{INCLUDE} = '';
	  }

        $mcus_by_name{$middle_name} = $mcu_data;
        read_content_from_header($middle_name);
        }
      }
    else
      {
      last;
      }
    }

  close(LIB);
  @mcu_name_array = sort(keys %mcus_by_name);
  }

#---------------------------------------------------------------------------------------------------

sub filter_registers_of_mcu($)
  {
  my $McuName = $_[0];
  my $mcu_data = $mcus_by_name{$McuName};
  my @addresses;
  my ($addr, $i, $j, $reg, $addr_num, $reg_num);
  my ($registers, $min, $reg_shortest, $regL, $reg1, $len);

  @addresses = sort {$a <=> $b} keys %{$mcu_data->{REGISTERS}};
  $addr_num = @addresses;
  $mcu_data->{ADDR_NUM} = $addr_num;

  return if ($addr_num == 0);

  for ($i = 0; $i < $addr_num; ++$i)
    {
    $addr = $addresses[$i];
    $registers = \@{$mcu_data->{REGISTERS}{$addr}};

    $reg_num = scalar(@{$registers});

    if ($reg_num == 1)
      {
      $reg = $registers->[0];
      $reg->{USE_COUNT} += 1;
      next;
      }

    $min  = UINT_MAX;
    $regL = undef;
    $reg1 = undef;
    for ($j = 0; $j < $reg_num; ++$j)
      {
      $reg  = $registers->[$j];
      $len  = length($reg->{REG_NAME});

      if ($reg->{REG_NAME} =~ /.+L$/io)
        {
        $regL = $reg;
        }
      elsif ($reg->{REG_NAME} =~ /.+1$/io)
        {
        $reg1 = $reg;
        }

      if ($min > $len)
        {
        #
        # The "DDR[A-Z]" is obsolete name.
        #
        if ($reg->{REG_NAME} !~ /^DDR[A-Z]/io)
          {
          $min = $len;
          $reg_shortest = $reg;
          }
        }
      }

    if (defined($regL) && $regL->{REG_NAME} eq "$reg_shortest->{REG_NAME}L")
      {
      #
      # PROD + L = PRODL
      #
      $regL->{USE_COUNT} += 1;
      #
      # Only one name remain.
      #
      @{$mcu_data->{REGISTERS}{$addr}} = ( $regL );
      }
    elsif (defined($reg1) && $reg1->{REG_NAME} eq "$reg_shortest->{REG_NAME}1")
      {
      #
      # RCREG + 1 = RCREG1
      #
      $reg1->{USE_COUNT} += 1;
      #
      # Only one name remain.
      #
      @{$mcu_data->{REGISTERS}{$addr}} = ( $reg1 );
      }
    else
      {
      $reg_shortest->{USE_COUNT} += 1;
      #
      # Only one name remain.
      #
      @{$mcu_data->{REGISTERS}{$addr}} = ( $reg_shortest );
      }
    }
  }

#---------------------------------------------------------------------------------------------------

        # Prints prototype of registers.

sub print_prototypes_of_registers()
  {
  my ($reg_name, $reg_array, $reg, $line);

  foreach $reg_name (sort(keys %registers))
    {
    $reg_array = $registers{$reg_name};

    next if (! defined($reg_array) || scalar(@{$reg_array}) == 0);

    foreach $reg (@{$reg_array})
      {
      my ($mcu_name, $addr) = ($reg->{MCU_NAME}, $reg->{REG_ADDR});

      if ($addr >= 0 && $reg->{USE_COUNT} > 0)
        {
        $line = "static const $reg_line_t ${mcu_name}_${reg_name}";
        printf $out_handler (align($line, 49) . "= { " . align("\"$reg_name\"", 15) . ", 0x%04X };\n", $addr);
        }
      }
    }

  print $out_handler "\n";
  }

#---------------------------------------------------------------------------------------------------

sub print_mcu($)
  {
  my $McuName = $_[0];
  my $mcu_data = $mcus_by_name{$McuName};
  my @addresses;
  my ($addr, $i, $reg, $addr_num, $reg_num, $line);

  @addresses = sort {$a <=> $b} keys %{$mcu_data->{REGISTERS}};
  $addr_num = @addresses;
  $mcu_data->{ADDR_NUM} = $addr_num;

  return if ($addr_num == 0);

  print $out_handler "/*--------------------- $McuName ---------------------*/\n\n";

  print $out_handler "static const $reg_line_t *${McuName}_registers[] = {\n";

  $line = '';
  $i = 0;
  while (1)
    {
    $addr = $addresses[$i++];

    $reg_num = scalar(@{$mcu_data->{REGISTERS}{$addr}});

    if ($reg_num == 0)
      {
      printf STDERR "$McuName: On this address no name! 0x%04X\n", $addr;
      }
    elsif ($reg_num > 1)
      {
      printf STDERR "$McuName: On this address more than one name there are! 0x%04X\n", $addr;
      }

    $reg  = ${$mcu_data->{REGISTERS}{$addr}}[0];
    $line = "  &$reg->{MCU_NAME}_$reg->{REG_NAME}";

    if ($i >= $addr_num)
      {
      printf $out_handler (align($line, REG_LINE_LENGTH) . "/* 0x%04X */\n", $addr);
      last;
      }

    printf $out_handler (align("${line},", REG_LINE_LENGTH) . "/* 0x%04X */\n", $addr);
    }

  print $out_handler "};\n\n";
  }

#---------------------------------------------------------------------------------------------------

sub print_mcu_array()
  {
  my ($mcu_name, $mcu_data, $line, $i, $mcu_num, $mcu_real_num, $reg_num);

  print $out_handler "const $reg_table_t $reg_table\[\] = {\n";

  $i = 0;
  $mcu_num = @mcu_name_array;
  $reg_num = 0;
  $mcu_real_num = 0;

  while (1)
    {
    $mcu_name = $mcu_name_array[$i++];

    next if (! defined($mcu_name));

    $mcu_data = $mcus_by_name{$mcu_name};

    if (! defined($mcu_data))
      {
      print STDERR "The $mcu_name MCU not exist !\n";
      next;
      }

    $reg_num  = $mcu_data->{ADDR_NUM};

    next if ($reg_num == 0);

    $line = align("  { \"$mcu_name\"", 20) . ", %3u, " . align("${mcu_name}_registers", 24) . '}';
    ++$mcu_real_num;

    if ($i >= $mcu_num)
      {
      printf $out_handler "$line\n", $reg_num;
      last;
      }

    printf $out_handler "${line},\n", $reg_num;
    }

  print $out_handler "};\n\n";
  print $out_handler "const int ${reg_table}_size = $mcu_real_num;\n";
  }

#---------------------------------------------------------------------------------------------------

sub print_header()
  {
  print $out_handler <<EOT

\#ifndef __GPREGISTER_H__
\#define __GPREGISTER_H__

/* This file is generated automatically by the $PROGRAM${time_str}. */

typedef struct {
  const char *name;
  unsigned int address;
} $reg_line_t;

typedef struct {
  const char *name;
  unsigned int register_number;
  const $reg_line_t **registers;
} $reg_table_t;

extern const $reg_table_t $reg_table\[\];
extern const int ${reg_table}_size;

const $reg_table_t *gp_register_find_mcu(const $reg_table_t *Table, int Table_size, const char *McuName);
const $reg_line_t *gp_register_find_reg(const $reg_table_t *Mcu, unsigned int Address);
const char *gp_register_find_reg_name(const $reg_table_t *Mcu, unsigned int Address);

\#endif /* __GPREGISTER_H__ */
EOT
;
  }

#---------------------------------------------------------------------------------------------------

sub usage()
  {
  print <<EOT
Usage: $PROGRAM [options]

    Options are:

        -gp <path> or --gputils-path <path>

            Path to gputils source files.

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
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  The main program.  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

$PROGRAM = basename($0);

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
$gpproc_path = "$gputils_path/libgputils/$gpprocessor_c";

read_all_mcu_registers("$gputils_path/header");

foreach my $mcu_name (@mcu_name_array)
  {
  filter_registers_of_mcu($mcu_name)
  }

$time_str = strftime(', %F %T UTC', gmtime);

open($out_handler, '>', $output_h) || die "Could not create the \"$output_h\" file!\n";
Log("Creates the header of register database", 1);
print_header();
close($out_handler);

open($out_handler, '>', $output_c) || die "Could not create the \"$output_c\" file!\n";
Log("Creates the register database", 1);

print $out_handler "\n/* This file is generated automatically by the $PROGRAM${time_str}. */\n\n" .
                   "#include \"$output_h\"\n\n";
print_prototypes_of_registers();

foreach my $mcu_name (@mcu_name_array)
  {
  print_mcu($mcu_name);
  }

print_mcu_array();
close($out_handler);

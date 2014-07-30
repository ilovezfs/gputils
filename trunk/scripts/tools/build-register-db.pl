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

use constant FNV1A32_INIT  => 0x811C9DC5;
use constant FNV1A32_PRIME => 0x01000193;

my $PROGRAM  = 'build-register-db.pl';
my $time_str = '';

my $bit_line_t  = 'gp_bit_t';
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
        OWNER_MCU  => '',      The name of owner MCU.
        REG_NAME   => '',      The name of register.
        REG_ADDR   => 0,       The address of register.
        REG_HASH   => 0,       A hash number of entire register: Register name and address, bit names and addresses.
        BIT_COUNT  => 0,       Number of existing bits.
        BIT_NAMES  => [        The bits of register.
                        [],    The names of 0th bit.
                        [],
                        [],
                        [],
                        [],
                        [],
                        [],
                        []     The names of 7th bit.
                      ]
        BIT_STRUCT => '',      The name of this bit structure.
        USE_COUNT  => 0        Shows the register get reference how many times.
        }
=cut

my %registers = ();

my %reg_prototypes = ();
my @sorted_hashes_of_prototypes = ();

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

sub fnv1a32_str($$)
  {
  my ($String, $Hash) = @_;

  foreach my $byte (unpack('C*', $String))
    {
    $Hash ^= $byte;
    $Hash *= FNV1A32_PRIME;
    $Hash &= 0xFFFFFFFF;
    }

  return $Hash;
  }

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

sub align($$)
  {
  my $text = $_[0];
  my $al   = $_[1] - length($text);

  return (($al < 1) ? $text : ($text . ' ' x $al));
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
  my ($refMcu, $RegName, $Address) = @_;
  my $mcu_name = $refMcu->{NAME};
  my $reg_name = correct_name($RegName, $mcu_name);
  my $ref_reg  = $refMcu->{REG_BY_NAME}->{$reg_name};

  return $ref_reg if (defined($ref_reg));

  Log("New register: $mcu_name, $reg_name", 6);

  $ref_reg = {
             OWNER_MCU  => $mcu_name,
             REG_NAME   => $reg_name,
             REG_ADDR   => $Address,
             REG_HASH   => 0,
             BIT_COUNT  => 0,
             BIT_NAMES  => undef,
             BIT_STRUCT => undef,
             USE_COUNT  => 0
             };

  $refMcu->{REG_BY_NAME}->{$reg_name} = $ref_reg;
  push(@{$refMcu->{REG_BY_ADDR}->{$Address}}, $ref_reg) if ($Address >= 0);
  return $ref_reg;
  }

#---------------------------------------------------------------------------------------------------

        # In the $Queue are in register's names.

sub add_registers($$$)
  {
  my ($refMcu, $refRegs, $refBits) = @_;
  my $ref_reg;

  return if (@{$refRegs} == 0 || @{$refBits} == 0);

  foreach (@{$refRegs})
    {
    next if ($_ eq 'and');      # This here easiest to filter out.

    $ref_reg = new_register($refMcu, $_, -1);
    $ref_reg->{BIT_NAMES} = [ @{$refBits} ];
    }

  @{$refRegs} = ();
  @{$refBits} = ();
  }

#---------------------------------------------------------------------------------------------------

        # Reads the entire content of an include file.

sub read_content_from_header($)
  {
  my $refMcu = $_[0];
  my ($mcu_name, $state, $name, $addr);
  my @reg_names;
  my @reg_bits;

  $mcu_name = $refMcu->{NAME};
  $name     = $refMcu->{INCLUDE};

  Log("Read header: $mcu_name -- $name", 4);
  print STDERR "Header file not exist! ($mcu_name)\n" if ($name eq '');
  
  $name = "$gp_header_path/$name";

  open(IN, '<', $name) || die "Can not open the $name header file!\n";

  $state = ST_NONE;
  @reg_names = ();
  @reg_bits  = ();

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
        Log(":::: ST_NONE ($line) ($mcu_name)", 7);

        $state = ST_REG_ADDR if ($line =~ /^;-+\s*Register\s+Files\s*-+$/io);
        }

     when (ST_REG_ADDR)
        {
        Log(":::: ST_REG_ADDR ($line) ($mcu_name)", 7);

        if ($line =~ /^;-+\s*(.+)Bits\s*-+$/io)
          {
          # ;----- STKPTR Bits --------------------------------------------------------
          # ;----- UIR/UIE Bits -------------------------------------------------------
          # ;----- TXSTA, TXSTA1 and TXSTA2 Bits --------------------------------------
          #

          # Therefore need the reg_names because more register names can be on one line.
          #

          @reg_names = ($1 =~ /([^\s,\/]+)/go);
          $state = ST_REG1_DEF;
          }
        elsif ($line =~ /^(\w+)\s+EQU\s+([\w']+)$/io)  #'
          {
          # PORTC     EQU  H'0007'
          #

          new_register($refMcu, $1, str2int($2));
          }
        } # when (ST_REG_ADDR)

      when (ST_REG1_DEF)
        {
        Log(":::: ST_REG1_DEF ($line) ($mcu_name)", 7);

        if ($line =~ /^;\s*I\/O\s+Pin\s+Name\s+Definitions?$/io)
          {
          # ;       I/O Pin Name Definitions
          #

          Log("1 +++ add_register()", 8);
          add_registers($refMcu, \@reg_names, \@reg_bits);
          $state = ST_REG2_DEF;
          }
        elsif ($line =~ /^;\s*RAM\s+Definitions?$/io)
          {
          # ;       RAM Definition
          # ;       RAM Definitions
          #

          Log("2 +++ add_register()", 8);
          add_registers($refMcu, \@reg_names, \@reg_bits);
          $state = ST_RAM_DEF;
          }
        elsif ($line =~ /^;-+\s*(.+)Bits\s*-+$/io)
          {
          $name = $1;
          Log("3 +++ add_register()", 8);
          add_registers($refMcu, \@reg_names, \@reg_bits);

          # Therefore need the reg_names because more register names can be on one line.

          @reg_names = ($name =~ /([^\s,\/]+)/go);
          }
        elsif ($line =~ /^(\w+)\s+EQU\s+([\w']+)$/io)  #'
          {
          # VR2       EQU  H'0002'
          #

          push(@{$reg_bits[str2int($2)]}, $1);
          }
        } # when (ST_REG1_DEF)

      when (ST_REG2_DEF)
        {
        Log(":::: ST_REG2_DEF ($line) ($mcu_name)", 7);

        if ($line =~ /^;\s*RAM\s+Definitions?$/io)
          {
          # ;       RAM Definition
          # ;       RAM Definitions
          #

          Log("4 +++ add_register()", 8);
          add_registers($refMcu, \@reg_names, \@reg_bits);
          $state = ST_RAM_DEF;
          }
        elsif ($line =~ /^;-+\s*([^-]+)\s*-+$/io)
          {
          $name = $1;
          Log("5 +++ add_register()", 8);
          add_registers($refMcu, \@reg_names, \@reg_bits);

          # Therefore need the reg_names because more register names can be on one line.
          #

          @reg_names = ($name =~ /([^\s,\/]+)/go);
          }
        elsif ($line =~ /^(\w+)\s+EQU\s+([\w']+)$/io)  #'
          {
          # RE3      EQU  3
          #

          push(@{$reg_bits[int($2)]}, $1);
          }
        } # when (ST_REG2_DEF)

      when (ST_RAM_DEF)
        {
        Log(":::: ST_RAM_DEF ($line) ($mcu_name)", 7);
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

  Log("Read all MCU registers.", 4);
  open(LIB, '<', $gpproc_path) || die "read_all_mcu_registers(): Can not open. -> \"$gpproc_path\"\n";

        # static struct px pics[] = {
        # { PROC_CLASS_PIC14E   , "__16F1526"     , { "pic16f1526"     , "p16f1526"       , "16f1526"         }, 0x1526,  4,   32, { 0x70, 0x7F }, 0x001FFF, 0x002000, {       -1,       -1 }, { 0x008000, 0x008003 }, { 0x008007, 0x008008 }, {       -1,       -1 }, "p16f1526.inc"     , "16f1526_g.lkr"     , 0 },
        # { PROC_CLASS_EEPROM8  , "__EEPROM8"     , { "eeprom8"        , "eeprom8"        , "eeprom8"         }, 0x1FFF,  0,    0, {   -1,   -1 }, 0x0000FF,       -1, {       -1,       -1 }, {       -1,       -1 }, {       -1,       -1 }, {       -1,       -1 }, NULL               , NULL                , 0 },
        # { PROC_CLASS_PIC14    , "__RF675H"      , { "rf675h"         , "rf675h"         , "rf675h"          }, 0x4675,  1,    2, { 0x20, 0x5F }, 0x00217F, 0x000400, { 0x0003FF, 0x0020FF }, { 0x002000, 0x002003 }, { 0x002007, 0x002007 }, { 0x002100, 0x00217F }, "rf675h.inc"       , "rf675h_g.lkr"      , 0 },

  my $in_table = FALSE;

  %mcus_by_name = ();

  while (<LIB>)
    {
    chomp;

    if (! $in_table)
      {
      $in_table = TRUE if (/^\s*static\s+struct\s+px\s+pics\[\s*\]\s*=\s*\{\s*$/io);
      }
    elsif (/\{\s*PROC_CLASS_\w+\s*,\s*"\w+"\s*,\s*\{\s*"\w+"\s*,\s*"(\w+)"\s*,\s*"\w+"\s*}\s*,\s*[\w-]+\s*,\s*[\w-]+\s*,\s*[\w-]+\s*,\s*\{\s*\S+\s*,\s*\S+\s*\}\s*,\s*\S+\s*,\s*\S+\s*,\s*\{\s*\S+\s*,\s*\S+\s*\}\s*,\s*{\s*\S+\s*,\s*\S+\s*\}\s*,\s*{\s*\S+\s*,\s*\S+\s*\}\s*,\s*{\s*\S+\s*,\s*\S+\s*\}\s*,\s*\"?([\.\w]+)\"?\s*,\s*\"?[\.\w]+\"?\s*,\s*\d+\s*\}/io)
      {
      my $middle_name = $1;
      my $ref_mcu = {
                    NAME        => $middle_name,
                    INCLUDE     => $2,
                    REG_BY_NAME => {},
                    REG_BY_ADDR => {},
                    ADDR_NUMBER => 0
                    };

      if ($middle_name eq 'p16c52')
        {
	if (! -e "$Dir/$ref_mcu->{INCLUDE}")
	  {
          printf STDERR "This header not exist: \"$Dir/$ref_mcu->{INCLUDE}\"\n";
	  $ref_mcu->{INCLUDE} = '';
	  }

        $mcus_by_name{$middle_name} = $ref_mcu;
        }
      elsif ($middle_name ne 'p16c5x' &&
             $middle_name !~ /^p1[6-8]cxx$/o &&
             $middle_name !~ /^p14000/o &&
             $middle_name !~ /^eeprom/o &&
             $middle_name =~ /^(p1|ps|rf)/o)
        {
	if (! -e "$Dir/$ref_mcu->{INCLUDE}")
	  {
          printf STDERR "This header not exist: \"$Dir/$ref_mcu->{INCLUDE}\"\n";
	  $ref_mcu->{INCLUDE} = '';
	  }

        $mcus_by_name{$middle_name} = $ref_mcu;
        read_content_from_header($ref_mcu);
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

        # Sometimes it happens that more names are at disposal of a bit.
        # This procedure choose one of them and delete the rest.

sub filter_bits_of_register($)
  {
  my $refReg   = $_[0];
  my $ref_bits = $refReg->{BIT_NAMES};
  my ($bits, $bit_num, $bit_name_num, $i, $j, $bit, $len, $min, $bit_shortest, $hash);

  Log("    Filter bits of the $refReg->{REG_NAME}.", 6);

  $hash = $refReg->{REG_HASH};
  $bit_num = 0;

  for ($i = 0; $i < 8; ++$i)
    {
    $bits = $ref_bits->[$i];
    next if (! defined(@{$bits}));

    ++$bit_num;
    $bit_name_num = scalar(@{$bits});

    if ($bit_name_num == 1)
      {
      $bit_shortest = $bits->[0];
      }
    else
      {
      $min = UINT_MAX;

      for ($j = 0; $j < $bit_name_num; ++$j)
        {
        $bit = $bits->[$j];
        $len = length($bit);

        if ($min > $len)
          {
          $min = $len;
          $bit_shortest = $bit;
          }
        }
      }

    #
    # The type of "$refReg->{BIT_NAMES}->[]" thereafter no array, but also scalar.
    #
    $ref_bits->[$i] = $bit_shortest;
    $hash = fnv1a32_str($bit_shortest, $hash);
    $hash = fnv1a32_int32($i, $hash);
    }

  $refReg->{REG_HASH}  = $hash;
  $refReg->{BIT_COUNT} = $bit_num;
  }

#---------------------------------------------------------------------------------------------------

        # Sometimes it happens that more names are at disposal of a register.
        # This procedure choose one of them and delete the rest.

sub filter_registers_of_mcus()
  {
  my ($mcu_name, $ref_mcu);
  my ($addr, $i, $j, $ref_reg, $addr_num, $reg_num, $hash);
  my ($registers, $min, $ref_reg_shortest, $ref_regL, $ref_reg1, $ref_reg_one, $len);
  my @addresses;

  Log("Filter the registers.", 4);

  foreach $mcu_name (@mcu_name_array)
    {
    Log("  Filter registers of the $mcu_name.", 5);

    $ref_mcu = $mcus_by_name{$mcu_name};
    @addresses = sort {$a <=> $b} keys %{$ref_mcu->{REG_BY_ADDR}};
    $addr_num = @addresses;
    $ref_mcu->{ADDR_NUMBER} = $addr_num;

    next if ($addr_num == 0);

    for ($i = 0; $i < $addr_num; ++$i)
      {
      $addr = $addresses[$i];
      $registers = $ref_mcu->{REG_BY_ADDR}->{$addr};
      $reg_num = scalar(@{$registers});

      if ($reg_num == 1)
        {
        $ref_reg = $registers->[0];
        #
        # The type of "$ref_mcu->{REG_BY_ADDR}->{$addr}" thereafter no array, but also scalar.
        #
        $ref_mcu->{REG_BY_ADDR}->{$addr} = $ref_reg;
        $ref_reg->{USE_COUNT} += 1;
        $hash = fnv1a32_str($ref_reg->{REG_NAME}, FNV1A32_INIT);
        $ref_reg->{REG_HASH} = fnv1a32_int32($ref_reg->{REG_ADDR}, $hash);
        filter_bits_of_register($ref_reg);
        next;
        }

      $min = UINT_MAX;
      $ref_regL = undef;
      $ref_reg1 = undef;

      for ($j = 0; $j < $reg_num; ++$j)
        {
        $ref_reg = $registers->[$j];
        $hash = fnv1a32_str($ref_reg->{REG_NAME}, FNV1A32_INIT);
        $ref_reg->{REG_HASH} = fnv1a32_int32($ref_reg->{REG_ADDR}, $hash);
        $len = length($ref_reg->{REG_NAME});

        if ($ref_reg->{REG_NAME} =~ /.+L$/io)
          {
          $ref_regL = $ref_reg;
          }
        elsif ($ref_reg->{REG_NAME} =~ /.+1$/io)
          {
          $ref_reg1 = $ref_reg;
          }

        if ($min > $len)
          {
          #
          # The "DDR[A-Z]" is obsolete name.
          #
          if ($ref_reg->{REG_NAME} !~ /^DDR[A-Z]/io)
            {
            $min = $len;
            $ref_reg_shortest = $ref_reg;
            }
          }
        } # for ($j = 0; $j < $reg_num; ++$j)

      if (defined($ref_regL) && $ref_regL->{REG_NAME} eq "$ref_reg_shortest->{REG_NAME}L")
        {
        #
        # PROD + L = PRODL
        # Only one name remain.
        #
        $ref_reg_one = $ref_regL;
        }
      elsif (defined($ref_reg1) && $ref_reg1->{REG_NAME} eq "$ref_reg_shortest->{REG_NAME}1")
        {
        #
        # RCREG + 1 = RCREG1
        # Only one name remain.
        #
        $ref_reg_one = $ref_reg1;
        }
      else
        {
        #
        # Only one name remain.
        #
        $ref_reg_one = $ref_reg_shortest;
        }

      for ($j = 0; $j < $reg_num; ++$j)
        {
        $ref_reg = $registers->[$j];
        next if ($ref_reg->{REG_HASH} == $ref_reg_one->{REG_HASH});

        delete($ref_mcu->{REG_BY_NAME}->{$ref_reg->{REG_NAME}});
        }

      #
      # The type of "$ref_mcu->{REG_BY_ADDR}->{$addr}" thereafter no array, but also scalar.
      #
      $ref_mcu->{REG_BY_ADDR}->{$addr} = $ref_reg_one;
      $ref_reg_one->{USE_COUNT} += 1;
      filter_bits_of_register($ref_reg_one);
      } # for ($i = 0; $i < $addr_num; ++$i)
    }
  }

#---------------------------------------------------------------------------------------------------

        # Collect prototypes of registers.

sub collect_prototypes_of_registers()
  {
  my ($mcu_name, $ref_mcu, $addr_num, $addr, $reg_num, $ref_reg, $ref_proto);

  Log("Collect prototypes of registers.", 4);

  foreach $mcu_name (@mcu_name_array)
    {
    $ref_mcu = $mcus_by_name{$mcu_name};

    foreach $addr (sort {$a <=> $b} keys %{$ref_mcu->{REG_BY_ADDR}})
      {
      $ref_reg   = $ref_mcu->{REG_BY_ADDR}->{$addr};
      $ref_proto = $reg_prototypes{$ref_reg->{REG_HASH}};

      if (defined($ref_proto))
        {
        $ref_mcu->{REG_BY_ADDR}->{$addr} = $ref_proto;
        }
      else
        {
        #
        # A new prototype.
        #
        $reg_prototypes{$ref_reg->{REG_HASH}} = $ref_reg;
        }
      }
    }

  @sorted_hashes_of_prototypes = sort {$reg_prototypes{$a}->{REG_NAME}  cmp $reg_prototypes{$b}->{REG_NAME} ||
                                       $reg_prototypes{$a}->{OWNER_MCU} cmp $reg_prototypes{$b}->{OWNER_MCU}} (keys %reg_prototypes);
  }

#---------------------------------------------------------------------------------------------------

        # Prints prototypes of bits.

sub print_prototypes_of_bits()
  {
  my ($hash, $ref_reg, $mcu_name, $reg_name, $addr, $line);
  my ($ref_bits, $bit, $bits_num, $i, $len, $max);

  Log("Prints prototypes of bits.", 4);

  foreach $hash (@sorted_hashes_of_prototypes)
    {
    $ref_reg = $reg_prototypes{$hash};
    ($mcu_name, $reg_name, $addr) = ($ref_reg->{OWNER_MCU}, $ref_reg->{REG_NAME}, $ref_reg->{REG_ADDR});

    if ($addr >= 0 && $ref_reg->{USE_COUNT} > 0)
      {
      $bits_num = $ref_reg->{BIT_COUNT};
      next if ($bits_num <= 0);

      $ref_bits = $ref_reg->{BIT_NAMES};
      $max = 0;

      for ($i = 0; $i < 8; ++$i)
        {
        $bit = $ref_bits->[$i];
        next if (! defined($bit));

        $len = length($bit);
        $max = $len if ($max < $len);
        }

      $ref_reg->{BIT_STRUCT} = "${mcu_name}_${reg_name}_bits";
      $line = "static const $bit_line_t $ref_reg->{BIT_STRUCT}\[\]";
      print $out_handler (align($line, 51) . "= {\n");
      $max += 2;

      for ($i = 0; $i < 8; ++$i)
        {
        $bit = $ref_bits->[$i];
        next if (! defined($bit));

        print $out_handler ("  { " . align("\"$bit\"", $max) . ", $i }");
        --$bits_num;
        print($out_handler ',') if ($bits_num > 0);
        print $out_handler "\n";
        }

      print $out_handler "};\n\n";
      }
    }

  print $out_handler "\n";
  }

#---------------------------------------------------------------------------------------------------

        # Prints prototypes of registers.

sub print_prototypes_of_registers()
  {
  my ($hash, $ref_reg, $mcu_name, $reg_name, $addr, $line, $name);

  Log("Prints prototypes of registers.", 4);

  foreach $hash (@sorted_hashes_of_prototypes)
    {
    $ref_reg = $reg_prototypes{$hash};
    ($mcu_name, $reg_name, $addr) = ($ref_reg->{OWNER_MCU}, $ref_reg->{REG_NAME}, $ref_reg->{REG_ADDR});
    $name = "${mcu_name}_${reg_name}";

    Log("  Prints register prototypes of the $name.", 5);

    if ($addr >= 0 && $ref_reg->{USE_COUNT} > 0)
      {
      $line = "static const $reg_line_t $name";
      printf $out_handler (align($line, 48) . " = {\n  \"$reg_name\",\n  0x%04X,\n", $addr);

      if ($ref_reg->{BIT_COUNT} > 0)
        {
        print $out_handler "  $ref_reg->{BIT_COUNT},\n  $ref_reg->{BIT_STRUCT}\n";
        }
      else
        {
        print $out_handler "  0,\n  NULL\n";
        }

      print $out_handler "};\n\n";
      }
    }

  print $out_handler "\n";
  }

#---------------------------------------------------------------------------------------------------

        # Prints descriptors of all MCU.

sub print_all_mcu()
  {
  my ($mcu_name, $ref_mcu, $addr, $i, $ref_reg, $addr_num, $reg_num, $line);
  my @addresses;

  Log("Prints descriptors of all MCUs.", 4);

  foreach $mcu_name (@mcu_name_array)
    {
    Log("  Prints descriptor of $mcu_name.", 5);

    $ref_mcu = $mcus_by_name{$mcu_name};
    @addresses = sort {$a <=> $b} keys %{$ref_mcu->{REG_BY_ADDR}};
    $addr_num = @addresses;
    $ref_mcu->{ADDR_NUMBER} = $addr_num;

    next if ($addr_num == 0);

    print $out_handler "/*--------------------- $mcu_name ---------------------*/\n\n" .
                       "static const $reg_line_t *${mcu_name}_registers\[\] = {\n";

    $line = '';
    $i = 0;

    while (1)
      {
      $addr    = $addresses[$i++];
      $ref_reg = $ref_mcu->{REG_BY_ADDR}->{$addr};
      $line    = "  \&$ref_reg->{OWNER_MCU}_$ref_reg->{REG_NAME}";

      if ($i >= $addr_num)
        {
        printf $out_handler (align($line, REG_LINE_LENGTH) . "/* 0x%04X */\n", $addr);
        last;
        }

      printf $out_handler (align("${line},", REG_LINE_LENGTH) . "/* 0x%04X */\n", $addr);
      }

    print $out_handler "};\n\n";
    }
  }

#---------------------------------------------------------------------------------------------------

sub print_mcu_array()
  {
  my ($mcu_name, $ref_mcu, $line, $i, $mcu_num, $mcu_real_num, $reg_num);

  $reg_num = 0;
  $mcu_real_num = 0;

  Log("Prints array of MCUs.", 4);

  print $out_handler "const $reg_table_t $reg_table\[\] = {\n";

  $i = 0;
  $mcu_num = @mcu_name_array;

  while (1)
    {
    $mcu_name = $mcu_name_array[$i++];

    next if (! defined($mcu_name));

    $ref_mcu = $mcus_by_name{$mcu_name};

    if (! defined($ref_mcu))
      {
      print STDERR "The $mcu_name MCU not exist !\n";
      next;
      }

    $reg_num = $ref_mcu->{ADDR_NUMBER};

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
} $bit_line_t;

typedef struct {
  const char *name;
  unsigned int address;
  unsigned int bit_number;
  const $bit_line_t *bits;
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

const $bit_line_t *gp_register_find_bit(const $reg_line_t *Reg, unsigned int Address);
const char *gp_register_find_bit_name(const $reg_line_t *Reg, unsigned int Address);

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
$gpproc_path    = "$gputils_path/libgputils/$gpprocessor_c";
$gp_header_path = "$gputils_path/header";

read_all_mcu_registers($gp_header_path);
filter_registers_of_mcus();
collect_prototypes_of_registers();

$time_str = strftime(', %F %T UTC', gmtime);

open($out_handler, '>', $output_h) || die "Could not create the \"$output_h\" file!\n";
Log("Creates the header of register database.", 1);
print_header();
close($out_handler);

open($out_handler, '>', $output_c) || die "Could not create the \"$output_c\" file!\n";
Log("Creates the register database.", 1);

print $out_handler "\n/* This file is generated automatically by the $PROGRAM${time_str}. */\n\n" .
                   "#include \"stdhdr.h\"\n#include \"$output_h\"\n\n";
print_prototypes_of_bits();
print_prototypes_of_registers();
print_all_mcu();
print_mcu_array();
close($out_handler);

#!/bin/sh

# test.sh tests gpal
# Copyright (C) 2004
# Craig Franklin
#
# This file is part of gputils.
#
# gputils is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# gputils is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with gputils; see the file COPYING.  If not, write to
# the Free Software Foundation, 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.  

# FIXME: This is a poorly written script. This is temporary.  In 
# the future, dejagnu will be used to replace this script.

# Defines
version=1.0
TESTDIR=./test
HEADER=../../header
LKR=../../lkr
GPASMBIN=../../gpasm/gpasm
GPLINKBIN=../../gplink/gplink
GPALBIN=../gpal
GPALFLAGS="-p16f877 -k\"-m\""
GPVOBIN=../../gputils/gpvo

# general functions

function testfailed {
  echo "TEST FAILED"
  exit 1
}

create_stc() {

  if [ $# -lt 2 ] ; then
    echo "Usage: create_stc {COD_FILE} {GPSIM_SCRIPT}"
    exit 1
  fi

  echo "load s $1" > $2
  echo "break e test.done" >> $2
  echo "run" >> $2
  echo "x test.failed" >> $2
  echo "abort_gpsim_now" >> $2

}

simulate() {

  if [ $# -lt 2 ] ; then
    echo "Usage: simulate {GPSIM_SCRIPT} {OUTPUT}"
    exit 1
  fi

  echo "Simulate $1" >> $2
  gpsim --cli -c $1 >> $2
  grep "test.failed" $2 | grep "= 0x0"
  if [ $? -eq 0 ] ; then
    echo "PASSED" >> $2
    return 0
  else
    echo "FAILED" >> $2
    return 1
  fi

}

binexists() {

  # Test syntax.
  if [ $# = 0 ] ; then
    echo "Usage: binexists {program}"
  fi
  echo "testing for $1"
  if !(test -e $1); then
    echo "$1 not found.  Aborting."
    exit 1
  else
    $1 -v
    echo "executable found."
    echo
  fi

}

printbanner() {
  # Test syntax.
  if [ $# = 0 ] ; then
    echo "Usage: printbanner {message}"
    return 1
  fi
  echo "-----------------------------------------------------------"
  echo "$1"
  echo "-----------------------------------------------------------"
  echo
  return 0
}

printversion() {
  printbanner " test v$version - gpal test script."
  return 0
}

printheader () {
  printversion
  echo "NAME:      $NAME" 
  echo "DATE:      $(date +%x)"
  echo "TIME:      $(date +%r)"
  echo "HOST:      $HOSTNAME"
  echo "HOST TYPE: $HOSTTYPE"
  echo "HOST OS:   $OSTYPE"
  echo
  return 0
}

test_gpal_sub() {

  let tested=0
  let compiled=0
  let passed=0

  #Test syntax.
  if [ $# = 0 ] ; then
    echo "Usage: test_gpal_sub {subdirectory}"
    return 1
  fi

  if !(test -d $1); then
    echo "$1 not found.  Aborting."
    return 1
  fi

  printbanner "Running ./$1 gpal tests"

  cd $1

  # create the test directory if it doesn't already exist
  test -d $TESTDIR || mkdir $TESTDIR || exit 1
  cd $TESTDIR
  rm -f *

  # compile and simulate all of the test files using the scripts
  cp ../srcfiles/* .
  for x in `ls *.pal`
    do
      # split the base file name from the extension
      for basefilename in `echo $x | tr -s "." " "`
        do
          if [ $basefilename = "pal" ]; then
            # do nothing for the extension
            echo
          else
            let tested=tested+1
            echo "testing $basefilename.pal"
            echo "../../$GPALBIN $GPALFLAGS -o $basefilename.hex $basefilename.pal"
            ../../$GPALBIN $GPALFLAGS -o $basefilename.hex $basefilename.pal
            if (test -e $basefilename.hex); then 
              let compiled=compiled+1
              create_stc $basefilename.cod $basefilename.stc
              simulate $basefilename.stc $basefilename.log
              RETVAL=$?
              if [ $RETVAL -eq 1 ]; then
                echo "$basefilename.cod failed simulation"	
              else
                let passed=passed+1
              fi
            else
              echo "$basefilename.pal failed to compile"
            fi
          fi
        done
    done
  cd ..
  cd ..
  printbanner "./$1 testing complete"
  echo "$tested files tested"
  echo "$compiled compiled without errors"  
  echo "$passed passed simulation"  
  echo
}

test_gpal() {
  printbanner "Start of gpal testing"
  binexists $GPASMBIN
  binexists $GPLINKBIN
  binexists $GPALBIN
  test_gpal_sub gpal.project
}

# top level
if [ ! $1 ]; then
  echo "Not enough arguments. Try \"$0 help\" for help."
  exit 1
fi

case "$1" in

  all)
    printheader
    test_gpal
    exit
    ;;
  clean)
    rm -Rf ./gpal.project/test
    ;;
  help)
    printversion
    echo "Usage: $0 <option>"
    echo
    echo "Options:"
    echo "       all                = run all tests"
    echo "       clean              = clean all test directories"
    echo "       help               = display this help message"
    echo
    echo
    exit
    ;;
  *)
    echo "$0: Invalid argument. Try \"$0 help\" for help."
    exit 1  

esac

exit


#! /bin/sh

# cmp_gp_mp.sh - compare gputils and mplabx .inc and .lkr files
#
# Copyright (c) 2012 Borut Razem
#
# This file is part of gputils.
#
#  This software is provided 'as-is', without any express or implied
#  warranty.  In no event will the authors be held liable for any damages
#  arising from the use of this software.
#
#  Permission is granted to anyone to use this software for any purpose,
#  including commercial applications, and to alter it and redistribute it
#  freely, subject to the following restrictions:
#
#  1. The origin of this software must not be misrepresented; you must not
#     claim that you wrote the original software. If you use this software
#     in a product, an acknowledgment in the product documentation would be
#     appreciated but is not required.
#  2. Altered source versions must be plainly marked as such, and must not be
#     misrepresented as being the original software.
#  3. This notice may not be removed or altered from any source distribution.
#
#  Borut Razem
#  borut.razem@gmail.com

#set -vx

if [ "$(uname)" = "CYGWIN_NT-5.1" ]
then
  MPLABX_PATH=/cygdrive/c/Program\ Files/Microchip/MPLABX
  GPUTILS_PATH=/cygdrive/c/svn_snapshots/gputils/gputils/gputils
else
  MPLABX_PATH=/opt/microchip/mplabx
  GPUTILS_PATH=~/svn_snapshots/gputils/gputils
fi

MPLABX_INC=$MPLABX_PATH/mpasmx
MPLABX_LKR=$MPLABX_PATH/mpasmx/LKR

GPUTILS_INC=$GPUTILS_PATH/header
GPUTILS_LKR=$GPUTILS_PATH/lkr

#usage
usage()
{
  echo "Usage: $(basename $0)"
  echo "Compare gputils and mplabx .inc and .lkr files."
  exit 1
}

# convert to lower case
lowercase()
{
  if [ -n "$1" ]
  then
    echo "$1" | tr "[A-Z]" "[a-z]"
  else
    tr "[A-Z]" "[a-z]"
  fi
}

# remove trailing CRs
fixnl()
{
  if [ -n "$1" ]
  then
    cat "$1" | tr -d '\r'
  else
    tr -d '\r'
  fi
}

# gputils specific processing:
# process ";;;; Begin: - ;;;; End:" blocks
gpspec()
{
  local sed_cmd='/^;;;; Begin: Removed in gputils/,/^;;;; End: Removed in gputils/ {
  /^;;;; / d
  /^;;;; /! {
    s/^;;;;//
  }
}
/^;;;; Begin: Added in gputils/,/^;;;; End: Added in gputils/ {
  d
}
/^;;;; Begin: Changed in gputils/,/^;;;; End: Changed in gputils/ {
  /^;;;; / d
  /^;;;; /! {
    /;;;;/! d
    /;;;;/ s/^;;;;//
  }
}'

  if [ -n "$1" ]
  then
    cat "$1" | sed -s "$sed_cmd"
  else
    sed -s "$sed_cmd"
  fi
}

# main compare procedure
do_cmp()
{
  # compare .inc files
  local equ=0 equ_spec=0 diff=0 no_ex=0
  local mpp mpf gpp gpf

  echo ".inc files:"
  for mpp in "$MPLABX_INC"/*.[Ii][Nn][Cc]
  do
    mpf=$(basename "$mpp")
    gpf=$(lowercase "$mpf")
    gpp="$GPUTILS_INC"/$(lowercase "$gpf")

    #echo "=== $mpf"

    if [ -e "$gpp" ]
    then
      if diff --strip-trailing-cr --brief "$gpp" "$mpp" > /dev/null
      then
        echo "+++ $gpf is up to date."
        equ=$(expr $equ + 1)
      else
        if gpspec "$gpp" | diff --strip-trailing-cr --brief - "$mpp" > /dev/null
        then
          echo "### $gpf is up to date with gputils specifics."
          equ_spec=$(expr $equ_spec + 1)
        else
          echo "--- $gpf differs."
          diff=$(expr $diff + 1)
        fi
      fi
    else
      echo "!!! $gpf does not exist in gputils."
      no_ex=$(expr $no_ex + 1)
    fi
  done

  echo "-----------------------------------------------------------------------"
  echo "Up to date: $equ"
  echo "Up to date gp spec: $equ_spec"
  echo "Different: $diff"
  echo "Non existing: $no_ex"
  echo

  # compare .lkr files
  equ=0
  diff=0
  no_ex=0
  equ_spec=0

  echo ".lkr files:"
  for mpp in "$MPLABX_LKR"/*.[Ll][Kk][Rr]
  do
    mpf=$(basename "$mpp")
    gpf=$(lowercase "$mpf")
    gpp="$GPUTILS_LKR"/$(lowercase "$gpf")

    #echo "=== $mpf"

    if [ -e "$gpp" ]
    then
      if diff --strip-trailing-cr --brief "$gpp" "$mpp" > /dev/null
      then
        echo "+++ $gpf is up to date."
        equ=$(expr $equ + 1)
      else
        echo "--- $gpf differs."
        diff=$(expr $diff + 1)
      fi
    else
      if ls ${gpp%_g.lkr}.lkr > /dev/null 2>&1
      then
        echo "??? old version: $(basename $(ls ${gpp%_g.lkr}.lkr))"
        equ_spec=$(expr $equ_spec + 1)
      else
        echo "!!! $gpf does not exist in gputils."
        no_ex=$(expr $no_ex + 1)
      fi
    fi
  done

  echo "-----------------------------------------------------------------------"
  echo "Up to date: $equ"
  echo "Different: $diff"
  echo "Non existing: $no_ex"
  echo "Old version: $equ_spec"
}

test $# = 0 || usage

# execute main compare procedure
do_cmp

exit 0

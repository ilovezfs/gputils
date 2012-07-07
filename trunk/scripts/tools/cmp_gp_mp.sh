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
  GPUTILS_PATH=$HOME/svn_snapshots/gputils/gputils
fi

MPLABX_INC=$MPLABX_PATH/mpasmx
MPLABX_LKR=$MPLABX_PATH/mpasmx/LKR

GPUTILS_INC=$GPUTILS_PATH/header
GPUTILS_LKR=$GPUTILS_PATH/lkr

#usage
usage()
{
  echo "Usage: $(basename $0) [-mi] [-ml] [-pl] [-lp] [-pi]"
  echo "Compare gputils and mplabx .inc and .lkr files."
  echo "Options:"
  echo "  -mi     mplabx .inc files in header folder"
  echo "  -ml     mplabx .lkr files in lkr folder"
  echo "  -pl     devices from gpprocessor.c in lkr folder"
  echo "  -lp     .lkr from lkr folder in gpprocessor.c"
  echo "  -pi     devices from gpprocessor.c in header folder"
  echo "  -all    same as -mi -ml -pl -lp -pi"
  echo "  <none>  same as -mi -ml"
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

# extract .lkr files from gpprocessor.c
lkr_from_proc()
{
  sed -n -e 's/^.*,  *"\([^"]*\)" *},$/\1/p' < $GPUTILS_PATH/libgputils/gpprocessor.c
}

# extract device name from gpprocessor.c
dev_from_proc()
{
  local devs='' dev names i

#  { PROC_CLASS_PIC12,   "__12F529T48A", { "pic12f529t48a", "p12f529t48a", "12f529t48a" }, 0xa548, 2, 8, 0x5ff, { -1, -1 },       { 0xFFF, 0xFFF },       "12f529t48a_g.lkr"   },
  case $1 in
  0) sed -n -e 's/  *{[^,]*,[^,]*, *{ *"\([^"]*\)" *, *"[^"]*" *, *"[^"]*" *}.*/\1/p' < $GPUTILS_PATH/libgputils/gpprocessor.c;;
  1) sed -n -e 's/  *{[^,]*,[^,]*, *{ *"[^"]*" *, *"\([^"]*\)" *, *"[^"]*" *}.*/\1/p' < $GPUTILS_PATH/libgputils/gpprocessor.c;;
  2) sed -n -e 's/  *{[^,]*,[^,]*, *{ *"[^"]*" *, *"[^"]*" *, *"\([^"]*\)" *}.*/\1/p' < $GPUTILS_PATH/libgputils/gpprocessor.c;;
  esac
}

# compare .inc with mplabx
cmp_mp_inc()
{
  local equ=0 equ_spec=0 diff=0 diff_spec=0 no_ex=0
  local mpp mpf gpp gpf

  echo "mplabx .inc files in header folder:"
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
          if grep '^;;;; Begin:' $gpf > /dev/null 2>&1
          then
            echo "@@@ $gpf differs with gputils specifics."
            diff_spec=$(expr $diff_spec + 1)
          else
            echo "--- $gpf differs."
            diff=$(expr $diff + 1)
          fi
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
  echo "Different gp spec: $diff_spec"
  echo "Non existing: $no_ex"
  echo
}

# compare .lkr with mplabx
cmp_mp_lkr()
{
  local equ=0 old=0 diff=0 no_ex=0
  local mpp mpf gpp gpf

  echo "mplabx .lkr files in lkr folder:"
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
        old=$(expr $old + 1)
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
  echo "Old version: $old"
  echo
}

# is elemant in the list
is_in()
{
  local e=$1 l

  shift
  l=$*

  for i in $l
  do
    if [ $i = $e ]
    then
      return 0
    fi
  done
  return 1
}
    
# compare gpproccessor.c  with lkr
cmp_pl()
{
  local f n

  echo "devices from gpprocessor.c in lkr folder:"
  for f in $(lkr_from_proc)
  do
    if [ ! -e $GPUTILS_LKR/$f ]
    then
      n=$(echo $f | sed -e 's/^\([^.]*\)\.lkr/\1/' -e 's/^\([^_]*\)_.*/\1/' -e 's/^\([^_]*\)_.*/\1/' -e 's/^\([^i]*\)i.*/\1/')
      echo "%%% $GPUTILS_LKR/$f does not exist. Similar: " $(ls $GPUTILS_LKR/$n* 2> /dev/null)
    fi
  done
  echo
}

# compare lkr with gpproccessor.c
cmp_lp()
{
  local f

  echo ".lkr from lkr folder in gpprocessor.c:"
  for f in $GPUTILS_LKR/*.lkr
  do
    if ! is_in $(basename $f) $(lkr_from_proc)
    then
      echo "$f not in gpprocessor.c!"
    fi
  done
  echo
}

# compare gpproccessor.c with header
cmp_pi()
{
  local f n

  # check .inc files
  echo "devices from gpprocessor.c in header folder:"
  for f in $(dev_from_proc 1)
  do
    n=${f}.inc
    if [ ! -e $GPUTILS_INC/$n ]
    then
      echo "*** $GPUTILS_INC/$n does not exist."
    fi
  done
  echo
}

# main procrdure
for arg in $*
do
  case $arg
  in
  -mi) mi=1; has_opt=1;;
  -ml) ml=1; has_opt=1;;
  -pl) pl=1; has_opt=1;;
  -lp) lp=1; has_opt=1;;
  -pi) pi=1; has_opt=1;;
  -all) mi=1; ml=1; pl=1; lp=1; pi=1;;
  -*) echo "Unknown option $arg!"; usage; exit 1;;
  *) usage; exit 1;;
  esac
done

test -z "$has_opt" && ml=1 && mp=1

# execute main compare procedure
test -n "$mi" && cmp_mp_inc
test -n "$ml" && cmp_mp_lkr
test -n "$pl" && cmp_pl
test -n "$lp" && cmp_lp
test -n "$pi" && cmp_pi

exit 0

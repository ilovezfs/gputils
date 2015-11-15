#!/bin/bash

# cmp_gp_mp.sh - compare gputils and mplabx .inc and .lkr files
#
# Copyright (c) 2012 Borut Razem
# Copyright (c) 2014-2015 Molnar Karoly
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
#
#    $Id$
#

#set -vx

MPLABX_PATH=
DIFF_OUT_PATH=

if [ "$(uname)" = "CYGWIN_NT-5.1" ]; then
  MPLABX_PATH="/cygdrive/c/Program Files/Microchip/MPLABX"
  GPUTILS_PATH="/cygdrive/c/svn_snapshots/gputils/gputils/gputils"
else
  for d in "/opt/microchip/mplabx/*"; do
    if [ -z "$MPLABX_PATH" ]; then
      MPLABX_PATH="$d"
    fi
  done

  if [ -z "$MPLABX_PATH" ]; then
    MPLABX_PATH="/opt/microchip/mplabx"
  fi

  GPUTILS_PATH="$HOME/svn_snapshots/gputils/gputils"
fi

MPLABX_INC="$MPLABX_PATH/mpasmx"
MPLABX_LKR="$MPLABX_PATH/mpasmx/LKR"

GPUTILS_INC="$GPUTILS_PATH/header"
GPUTILS_LKR="$GPUTILS_PATH/lkr"

# diff ignore trailing space and dos newline char
DIFF_BASE_OPTS='--strip-trailing-cr'

if [ -n "$TMP" ]; then
  BASH_TMPHEAD="$TMP/$BASHPID"
else
  BASH_TMPHEAD="/tmp/$BASHPID"
fi

INC_GPSIGN=';;;;'
LKR_GPSIGN='\/\/\/\/'
BI_GPUTILS="^\s*$INC_GPSIGN\s*Begin\s*:"
BL_GPUTILS="^\s*$LKR_GPSIGN\s*Begin\s*:"
AD_GPUTILS='Added\s+in\s+gputils\.?'
CH_GPUTILS='Changed\s+in\s+gputils\.?'
RM_GPUTILS='Removed\s+in\s+gputils\.?'

# usage
usage()
{
  echo "Usage: $(basename $0) [-mi] [-ml] [-pl] [-lp] [-pi] [-p <path>]"
  echo "Compare gputils and mplabx .inc and .lkr files."
  echo "Options:"
  echo "  -mi                            The mplabx .inc files in header folder."
  echo "  -ml                            The mplabx .lkr files in lkr folder."
  echo "  -pl                            Devices from gpprocessor.c in lkr folder."
  echo "  -lp                            The *.lkr from lkr folder in gpprocessor.c."
  echo "  -pi                            Devices from gpprocessor.c in header folder."
  echo "  -all                           Same as -mi -ml -pl -lp -pi."
  echo "  <none>                         Same as -mi -ml."
  echo "  --strict                       Don't ignore blank line and spaces during comparison."
  echo "  -p <path>, --diff-path <path>  Path of diff and new or to be modified files."
  echo "  -h, --help                     Print this usage and exit."
  exit 1
}

# convert to lower case
lowercase()
{
  if [ -n "$1" ]; then
    echo "$1" | tr "[A-Z]" "[a-z]"
  else
    tr "[A-Z]" "[a-z]"
  fi
}

# remove trailing CRs
fixnl()
{
  if [ -n "$1" ]; then
    cat "$1" | tr -d '\r'
  else
    tr -d '\r'
  fi
}

inc_build_date_and_copyright_filter()
{
  local sed_cmd="s/^\s*;\s*Build\s+date\s*:.+$//
s/^\s*;\s*\(c\)\s+Copyright\s+[0-9]+\s*-\s*[0-9]+\s+Microchip\s+.+$//"

  sed -r -s "$sed_cmd" "$1" > "$2"
}

lkr_build_date_filter()
{
  sed -r -s "s/^\s*\/\/\s*Build\s+date\s*:.+$//" "$1" > "$2"
}

# gputils .inc specific processing:
# process ";;;; Begin: - ;;;; End:" blocks
inc_spec()
{
  local sed_cmd="/${BI_GPUTILS}\s*${RM_GPUTILS}$/,/^\s*${INC_GPSIGN}\s*End\s*:\s*${RM_GPUTILS}$/ {
  /^\s*${INC_GPSIGN}\s*(Begin|End)\s*:\s*${RM_GPUTILS}$/ d
  /^\s*${INC_GPSIGN}\s*(Begin|End)\s*:\s*${RM_GPUTILS}$/! {
    s/^\s*${INC_GPSIGN}//
  }
}
s/^\s*;\s*Build\s+date\s*:.+$//
s/^\s*;\s*\(c\)\s+Copyright\s+[0-9]+\s*-\s*[0-9]+\s+Microchip\s+.+$//
/${BI_GPUTILS}\s*${AD_GPUTILS}$/,/^\s*${INC_GPSIGN}\s*End\s*:\s*${AD_GPUTILS}$/ {
  d
}
/${BI_GPUTILS}\s*${CH_GPUTILS}$/,/^\s*${INC_GPSIGN}\s*End\s*:\s*${CH_GPUTILS}$/ {
  /^\s*${INC_GPSIGN}\s*(Begin|End)\s*:\s*${CH_GPUTILS}$/ d
  /^\s*${INC_GPSIGN}\s*(Begin|End)\s*:\s*${CH_GPUTILS}$/! {
    /${INC_GPSIGN}/! d
    /${INC_GPSIGN}/ s/^\s*${INC_GPSIGN}//
  }
}"

  if [ -n "$1" ]; then
    cat "$1" | sed -r -s "$sed_cmd"
  else
    sed -r -s "$sed_cmd"
  fi
}

# gputils .lkr specific processing:
# process "//// Begin: - //// End:" blocks
lkr_spec()
{
  local sed_cmd="/${BL_GPUTILS}\s*${RM_GPUTILS}$/,/^\s*${LKR_GPSIGN}\s*End\s*:\s*${RM_GPUTILS}$/ {
  /^\s*${LKR_GPSIGN}\s*(Begin|End)\s*:\s*${RM_GPUTILS}$/ d
  /^\s*${LKR_GPSIGN}\s*(Begin|End)\s*:\s*${RM_GPUTILS}$/! {
    s/^\s*${LKR_GPSIGN}//
  }
}
s/^\s*\/\/\s*Build\s+date\s*:.+$//
/${BL_GPUTILS}\s*${AD_GPUTILS}$/,/^\s*${LKR_GPSIGN}\s*End\s*:\s*${AD_GPUTILS}$/ {
  d
}
/${BL_GPUTILS}\s*${CH_GPUTILS}$/,/^\s*${LKR_GPSIGN}\s*End\s*:\s*${CH_GPUTILS}$/ {
  /^\s*${LKR_GPSIGN}\s*(Begin|End)\s*:\s*${CH_GPUTILS}$/ d
  /^\s*${LKR_GPSIGN}\s*(Begin|End)\s*:\s*${CH_GPUTILS}$/! {
    /${LKR_GPSIGN}/! d
    /${LKR_GPSIGN}/ s/^\s*${LKR_GPSIGN}//
  }
}"

  if [ -n "$1" ]; then
    cat "$1" | sed -r -s "$sed_cmd"
  else
    sed -r -s "$sed_cmd"
  fi
}

# extract .lkr files from gpprocessor.c
lkr_from_proc()
{
  sed -n -e 's/^.*,  *"\([^"]*\)" *},$/\1/p' < "$GPUTILS_PATH/libgputils/gpprocessor.c"
}

# extract device name from gpprocessor.c
dev_from_proc()
{
  local devs='' dev names i

#  { PROC_CLASS_PIC12E   , "__12F529T48A"  , { "pic12f529t48a"  , "p12f529t48a"    , "12f529t48a"      }, 0xD529,  3,    8, 0x00E0, { 0x07, 0x0F }, 0x06F, {     -1,     -1 }, 0x00FF, 0x0005FF, 0x000600, {       -1,       -1 }, { 0x000640, 0x000643 }, { 0x000FFF, 0x000FFF }, { 0x000600, 0x00063F }, "p12f529t48a.inc"  , "12f529t48a_g.lkr"  , 0 },
  case $1 in
    0) sed -n -e 's/  *{[^,]*,[^,]*, *{ *"\([^"]*\)" *, *"[^"]*" *, *"[^"]*" *}.*/\1/p' < "$GPUTILS_PATH/libgputils/gpprocessor.c";;
    1) sed -n -e 's/  *{[^,]*,[^,]*, *{ *"[^"]*" *, *"\([^"]*\)" *, *"[^"]*" *}.*/\1/p' < "$GPUTILS_PATH/libgputils/gpprocessor.c";;
    2) sed -n -e 's/  *{[^,]*,[^,]*, *{ *"[^"]*" *, *"[^"]*" *, *"\([^"]*\)" *}.*/\1/p' < "$GPUTILS_PATH/libgputils/gpprocessor.c";;
  esac
}

# compare .inc with mplabx
cmp_mp_inc()
{
  local equ=0 equ_bdate=0 equ_spec=0 diff_base=0 diff_spec=0 no_ex=0
  local mplabx_inc_file_path mplabx_inc_file_name
  local gputils_inc_file_path gputils_inc_file_name
  local gputils_inc gputils_spec_inc mplabx_inc
  local inc_path
  local diff_path diff_date_path diff_mod_path
  local gputils_path gputils_date_path gputils_mod_path
  local mplabx_path mplabx_mod_path mplabx_new_path
  local gpdiff

  if [ -n "$DIFF_OUT_PATH" ]; then
    inc_path="$DIFF_OUT_PATH/inc"
    diff_path="$inc_path/diff"
    diff_date_path="$inc_path/diff-date"
    diff_mod_path="$inc_path/diff-unto-gputils-modified"
    gputils_path="$inc_path/gputils/source-unmodified"
    gputils_date_path="$inc_path/gputils/source-date-modified"
    gputils_mod_path="$inc_path/gputils/source-unto-gputils-modified"
    mplabx_path="$inc_path/mplabx/source-original"
    mplabx_mod_path="$inc_path/mplabx/source-unto-gputils-modified"
    mplabx_new_path="$inc_path/mplabx/source-new"
  fi

  echo "mplabx .inc files in header folder:"
  for mplabx_inc_file_path in $MPLABX_INC/*.[Ii][Nn][Cc]; do
    mplabx_inc_file_name=$(basename "$mplabx_inc_file_path")
    gputils_inc_file_name=$(lowercase "$mplabx_inc_file_name")
    gputils_inc_file_path="$GPUTILS_INC/$gputils_inc_file_name"

    #echo "=== $mplabx_inc_file_name"

    if [ -e "$gputils_inc_file_path" ]; then
      if diff $DIFF_BASE_OPTS --brief "$gputils_inc_file_path" "$mplabx_inc_file_path" > /dev/null; then
        echo "+++ $gputils_inc_file_name is up to date."
        equ=$(expr $equ + 1)
      else
        gputils_inc="$BASH_TMPHEAD-gputils-$gputils_inc_file_name"
        mplabx_inc="$BASH_TMPHEAD-mplabx-$mplabx_inc_file_name"
        inc_build_date_and_copyright_filter "$gputils_inc_file_path" "$gputils_inc"
        inc_build_date_and_copyright_filter "$mplabx_inc_file_path" "$mplabx_inc"
        if diff $DIFF_BASE_OPTS --brief "$gputils_inc" "$mplabx_inc" > /dev/null; then
          echo "/// $gputils_inc_file_name is up to date, but differ with build date or copyright."
          equ_bdate=$(expr $equ_bdate + 1)
	  mkdir -p "$diff_date_path" "$gputils_date_path"
          gpdiff="$diff_date_path/$gputils_inc_file_name.diff"
          diff $DIFF_BASE_OPTS -uN "$gputils_inc_file_path" "$mplabx_inc_file_path" > "$gpdiff"
          cp "$gpdiff" "$gputils_date_path"
          cp "$gputils_inc_file_path" "$gputils_date_path"
        else
          gputils_spec_inc="$BASH_TMPHEAD-gputils-spec-$gputils_inc_file_name"
          inc_spec "$gputils_inc_file_path" > "$gputils_spec_inc"
          if diff $DIFF_BASE_OPTS --brief "$gputils_spec_inc" "$mplabx_inc" > /dev/null; then
            echo "### $gputils_inc_file_name is up to date with gputils specifics."
            equ_spec=$(expr $equ_spec + 1)
          else
            if grep -qE "^;;;;\s*Begin\s*:" "$gputils_inc_file_path" > /dev/null 2>&1; then
              echo "@@@ $gputils_inc_file_name differs with gputils specifics."
              diff_spec=$(expr $diff_spec + 1)
              if [ -n "$DIFF_OUT_PATH" ]; then
                gpdiff="$diff_mod_path/$gputils_inc_file_name.diff"
                mkdir -p "$diff_mod_path"
                diff $DIFF_BASE_OPTS -uN "$gputils_spec_inc" "$mplabx_inc" > "$gpdiff"
                if [ -s "$gpdiff" ]; then
                  # If the size of $gpdiff is not zero.
                  mkdir -p "$gputils_mod_path" "$mplabx_mod_path"
                  cp "$gpdiff" "$gputils_mod_path"
                  cp "$gputils_inc_file_path" "$gputils_mod_path"
                  cp "$mplabx_inc_file_path" "$mplabx_mod_path"
                else
                  rm "$gpdiff"
                fi
              fi
            else
              echo "--- $gputils_inc_file_name differs."
              diff_base=$(expr $diff_base + 1)
              if [ -n "$DIFF_OUT_PATH" ]; then
                gpdiff="$diff_path/$gputils_inc_file_name.diff"
                mkdir -p "$diff_path"
                diff $DIFF_BASE_OPTS -uN "$gputils_inc" "$mplabx_inc" > "$gpdiff"
                if [ -s "$gpdiff" ]; then
                  # If the size of $gpdiff is not zero.
                  mkdir -p "$gputils_path" "$mplabx_path"
                  cp "$mplabx_inc_file_path" "$gputils_path"
                  cp "$mplabx_inc_file_path" "$mplabx_path"
                else
                  rm "$gpdiff"
                fi
              fi
            fi
          fi
          rm -f "$gputils_spec_inc"
        fi
        rm -f "$gputils_inc" "$mplabx_inc"
      fi
    else
      echo "!!! $gputils_inc_file_name does not exist in gputils."
      no_ex=$(expr $no_ex + 1)
      if [ -n "$DIFF_OUT_PATH" ]; then
        mkdir -p "$mplabx_new_path"
        cp "$mplabx_inc_file_path" "$mplabx_new_path"
      fi
    fi
  done

  echo "-----------------------------------------------------------------------"
  echo "Up to date                       : $equ"
  echo "Up to date, but different"
  echo "  the build date or copyright    : $equ_bdate"
  echo "Up to date with gputils specifics: $equ_spec"
  echo "Different                        : $diff_base"
  echo "Different with gputils specifics : $diff_spec"
  echo "Non existing                     : $no_ex"
  echo
}

# compare .lkr with mplabx
cmp_mp_lkr()
{
  local equ=0 equ_bdate=0 equ_spec=0 diff_base=0 diff_spec=0 old=0 no_ex=0
  local mplabx_lkr_file_path mplabx_lkr_file_name
  local gputils_lkr_file_path gputils_lkr_file_name
  local gputils_lkr gputils_spec_lkr mplabx_lkr
  local lkr_path
  local diff_path diff_date_path diff_mod_path
  local gputils_path gputils_date_path gputils_mod_path
  local mplabx_path mplabx_mod_path mplabx_new_path
  local gpdiff

  if [ -n "$DIFF_OUT_PATH" ]; then
    lkr_path="$DIFF_OUT_PATH/lkr"
    diff_path="$lkr_path/diff"
    diff_date_path="$lkr_path/diff-date"
    diff_mod_path="$lkr_path/diff-unto-gputils-modified"
    gputils_path="$lkr_path/gputils/source-unmodified"
    gputils_date_path="$lkr_path/gputils/source-date-modified"
    gputils_mod_path="$lkr_path/gputils/source-unto-gputils-modified"
    mplabx_path="$lkr_path/mplabx/source-original"
    mplabx_mod_path="$lkr_path/mplabx/source-unto-gputils-modified"
    mplabx_new_path="$lkr_path/mplabx/source-new"
  fi

  echo "mplabx .lkr files in lkr folder:"
  for mplabx_lkr_file_path in $MPLABX_LKR/*.[Ll][Kk][Rr]; do
    mplabx_lkr_file_name=$(basename "$mplabx_lkr_file_path")
    gputils_lkr_file_name=$(lowercase "$mplabx_lkr_file_name")
    gputils_lkr_file_path="$GPUTILS_LKR/$gputils_lkr_file_name"

    #echo "=== $mplabx_lkr_file_name"

    if [ -e "$gputils_lkr_file_path" ]; then
      if diff $DIFF_BASE_OPTS --brief "$gputils_lkr_file_path" "$mplabx_lkr_file_path" > /dev/null; then
        echo "+++ $gputils_lkr_file_name is up to date."
        equ=$(expr $equ + 1)
      else
        gputils_lkr="$BASH_TMPHEAD-gputils-$gputils_lkr_file_name"
        mplabx_lkr="$BASH_TMPHEAD-mplabx-$mplabx_lkr_file_name"
        lkr_build_date_filter "$gputils_lkr_file_path" "$gputils_lkr"
        lkr_build_date_filter "$mplabx_lkr_file_path" "$mplabx_lkr" >&2
        if diff $DIFF_BASE_OPTS --brief "$gputils_lkr" "$mplabx_lkr" > /dev/null; then
          echo "/// $gputils_lkr_file_name is up to date, but differ with build date."
          equ_bdate=$(expr $equ_bdate + 1)
	  mkdir -p "$diff_date_path" "$gputils_date_path"
          gpdiff="$diff_date_path/$gputils_lkr_file_name.diff"
          diff $DIFF_BASE_OPTS -uN "$gputils_lkr_file_path" "$mplabx_lkr_file_path" > "$gpdiff"
          cp "$gpdiff" "$gputils_date_path"
          cp "$gputils_lkr_file_path" "$gputils_date_path"
        else
          gputils_spec_lkr="$BASH_TMPHEAD-gputils-spec-$gputils_lkr_file_name"
          lkr_spec "$gputils_lkr_file_path" > "$gputils_spec_lkr"
          if diff $DIFF_BASE_OPTS --brief "$gputils_spec_lkr" "$mplabx_lkr" > /dev/null; then
            echo "### $gputils_lkr_file_name is up to date with gputils specifics."
            equ_spec=$(expr $equ_spec + 1)
          else
            if grep -qE "^////\s*Begin\s*:" "$gputils_lkr_file_path" > /dev/null 2>&1; then
              echo "@@@ $gputils_lkr_file_name differs with gputils specifics."
              diff_spec=$(expr $diff_spec + 1)
              if [ -n "$DIFF_OUT_PATH" ]; then
                gpdiff="$diff_mod_path/$gputils_lkr_file_name.diff"
                mkdir -p "$diff_mod_path"
                diff $DIFF_BASE_OPTS -uN "$gputils_spec_lkr" "$mplabx_lkr" > "$gpdiff"
                if [ -s "$gpdiff" ]; then
                  # If the size of $gpdiff is not zero.
                  mkdir -p "$gputils_mod_path" "$mplabx_mod_path"
                  cp "$gpdiff" "$gputils_mod_path"
                  cp "$gputils_lkr_file_path" "$gputils_mod_path"
                  cp "$mplabx_lkr_file_path" "$mplabx_mod_path"
                else
                  rm "$gpdiff"
                fi
              fi
            else
              echo "--- $gputils_lkr_file_name differs."
              diff_base=$(expr $diff_base + 1)
              if [ -n "$DIFF_OUT_PATH" ]; then
                gpdiff="$diff_path/$gputils_lkr_file_name.diff"
                mkdir -p "$diff_path"
                diff $DIFF_BASE_OPTS -uN "$gputils_lkr" "$mplabx_lkr" > "$gpdiff"
                if [ -s "$gpdiff" ]; then
                  # If the size of $gpdiff is not zero.
                  mkdir -p "$gputils_path" "$mplabx_path"
                  cp "$mplabx_lkr_file_path" "$gputils_path"
                  cp "$mplabx_lkr_file_path" "$mplabx_path"
                else
                  rm "$gpdiff"
                fi
              fi
            fi
          fi
          rm -f "$gputils_spec_lkr"
        fi
        rm -f "$gputils_lkr" "$mplabx_lkr"
      fi
    else
      if ls "${gputils_lkr_file_path%_g.lkr}.lkr" > /dev/null 2>&1; then
        echo "??? old version: $(basename $(ls ${gputils_lkr_file_path%_g.lkr}.lkr))"
        old=$(expr $old + 1)
      else
        echo "!!! $gputils_lkr_file_name does not exist in gputils."
        no_ex=$(expr $no_ex + 1)
        if [ -n "$DIFF_OUT_PATH" ]; then
          mkdir -p "$mplabx_new_path"
          cp "$mplabx_lkr_file_path" "$mplabx_new_path"
        fi
      fi
    fi
  done

  echo "-----------------------------------------------------------------------"
  echo "Up to date                       : $equ"
  echo "Up to date, but different"
  echo "  the build date                 : $equ_bdate"
  echo "Up to date with gputils specifics: $equ_spec"
  echo "Different                        : $diff_base"
  echo "Different with gputils specifics : $diff_spec"
  echo "Non existing                     : $no_ex"
  echo "Old version                      : $old"
  echo
}

# is element in the list
is_in()
{
  local e=$1 l

  shift
  l=$*

  for i in $l; do
    if [ $i = $e ]; then
      return 0
    fi
  done
  return 1
}
    
# compare gpproccessor.c  with lkr
cmp_pl()
{
  local f n

  echo "Devices from gpprocessor.c in lkr folder:"
  for f in $(lkr_from_proc); do
    if [ ! -e "$GPUTILS_LKR/$f" ]; then
      n=$(echo $f | sed -e 's/^\([^.]*\)\.lkr/\1/' -e 's/^\([^_]*\)_.*/\1/' -e 's/^\([^_]*\)_.*/\1/' -e 's/^\([^i]*\)i.*/\1/') #'
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
  for f in $GPUTILS_LKR/*.lkr; do
    if ! is_in "$(basename $f)" "$(lkr_from_proc)"; then
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
  echo "Devices from gpprocessor.c in header folder:"
  for f in $(dev_from_proc 1); do
    n="${f}.inc"
    if [ ! -e "$GPUTILS_INC/$n" ]; then
      echo "*** $GPUTILS_INC/$n does not exist."
    fi
  done
  echo
}

# main procedure
while [ $# -gt 0 ]; do
  opt="$1"
  case "$opt" in
    -mi)
      mi=1
      has_opt=1
      ;;

    -ml)
      ml=1
      has_opt=1
      ;;

    -pl)
      pl=1
      has_opt=1
      ;;

    -lp)
      lp=1
      has_opt=1
      ;;

    -pi)
      pi=1
      has_opt=1
      ;;

    -all)
      mi=1
      ml=1
      pl=1
      lp=1
      pi=1
      ;;

    --strict)
      strict=1
      ;;

    -p|--diff-path)
      shift
      if [ $# -gt 0 ]; then
        DIFF_OUT_PATH="$1"
      else
        echo "Option $opt has no argument!" >&2
        exit 1
      fi
      ;;

    -h|--help)
      usage
      exit 0
      ;;

    -*)
      echo "Unknown option $opt!";
      usage
      exit 1
      ;;

    *)
      usage
      exit 1
      ;;
  esac
  shift
done

test -z "$has_opt" && mi=1 && ml=1

test -z "$strict" && DIFF_BASE_OPTS="$DIFF_BASE_OPTS --ignore-all-space --ignore-blank-lines"

# execute main compare procedure
test -n "$mi" && cmp_mp_inc
test -n "$ml" && cmp_mp_lkr
test -n "$pl" && cmp_pl
test -n "$lp" && cmp_lp
test -n "$pi" && cmp_pi

exit 0

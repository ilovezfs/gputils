#!/bin/bash

# generate Makefile.am from the directory contents

echo "## Process this file with automake to produce Makefile.in"
echo
echo "pkgdatadir = @GPLINK_LKR_PATH@"
echo
echo "EXTRA_DIST =\\"

# count the number of items in the list
let count=0
for x in `ls *.lkr | sort`
  do
    let count=count+1
  done

# output the file list
let pass=0
for x in `ls *.lkr | sort`
  do
    let pass=pass+1
    if [ $pass -eq $count ]; then
      echo "	$x"
    else
      echo "	$x \\"
    fi
  done
 
echo
echo "pkgdata_DATA = \$(EXTRA_DIST)"

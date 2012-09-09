#!/bin/sh

if test -n "$1"
then
  srcfilename=$1
else
  ver=$(sed -n 's/AC_INIT(\[gputils\],[ \t]*\[\([^\]*\)\],[ \t]*\[[^\]*\])$/\1/p' ../../../gputils/configure.ac)
  srcfilename=gputils-${ver}.tar.gz
fi

if test -n "$2"
then
  packager=$2
else
  packager="Borut Razem"
fi

if test -n "$3"
then
  email=$3
else
  email=borut.razem@siol.net
fi

# gcc-3
#CC="gcc -mno-cygwin" ./make-mingw --email=$email --name="$packager" --source=$srcfilename --save-temps
#gcc-4
CC="i686-pc-mingw32-gcc" ./make-mingw --email=$email --name="$packager" --source=$srcfilename --save-temps
# on mingw32:
#CC=gcc ./make-mingw --email=$email --name="$packager" --source=$srcfilename --save-temps

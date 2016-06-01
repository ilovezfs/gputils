#!/bin/sh

# do_setup.sh - front end for the make-mingw script
#
# Copyright (c) 2009-2013 Borut Razem
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
#  borut.razem@users.sourceforge.net

if [ -n "$1" ]
then
  srcfilename="$1"
else
  ver=$(sed -n 's/AC_INIT(\[gputils\],[ \t]*\[\([^\]*\)\],[ \t]*\[[^\]*\])$/\1/p' ../../../gputils/configure.ac)
  srcfilename="gputils-${ver}.tar.bz2"
fi

if [ -n "$2" ]
then
  packager="$2"
else
  packager="Molnar Karoly"
fi

if [ -n "$3" ]
then
  email="$3"
else
  email="molnarkaroly@users.sourceforge.net"
fi

./make-mingw --email=$email --name="$packager" --source="$srcfilename" --save-temps

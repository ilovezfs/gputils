# Makefile.am
# Copyright (C) 2000, 2001, 2002, 2003
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

bin_PROGRAMS = gpdasm gplib gpvc gpvo

CPPFLAGS = @CPPFLAGS@ -I${top_srcdir}/libgputils -I${top_srcdir}/include

gpdasm_SOURCES=\
	gpdasm.c\
	gpdasm.h

gpdasm_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

gplib_SOURCES=\
	gplib.c \
	gplib.h

gplib_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

gpvc_SOURCES=\
	block.c dump.c gpvc.c \
	block.h dump.h gpvc.h

gpvc_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

gpvo_SOURCES=\
	gpvo.c \
	gpvo.h

gpvo_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

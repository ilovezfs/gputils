# Makefile.am
# Copyright (C) 1998,1999,2000,2001 James Bowman, Craig Franklin
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

bin_PROGRAMS = gpasm

CPPFLAGS = @CPPFLAGS@ -I${top_srcdir}/libgputils -I${top_srcdir}/include -DGPASM_HEADER_PATH=\"@GPASM_HEADER_PATH@\"

gpasm_SOURCES=\
	cod.c \
	directive.c \
	evaluate.c \
	gpasm.c \
	parse.y \
	scan.l \
	gperror.c \
	lst.c \
	macro.c \
	processor.c \
	special.c \
	util.c \
	cod.h \
	directive.h \
	evaluate.h \
	gpasm.h \
	gperror.h \
	lst.h \
	macro.h \
	processor.h \
	special.h

gpasm_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

YFLAGS = -dvt
LFLAGS = -i

EXTRA_DIST = scan.h parse.h

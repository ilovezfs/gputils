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

SUBDIRS = @MAKE_SUBDIRS@

# generate the rpm spec file when "make spec" or "make dist" is commanded.

SPECFILE = gputils-@VERSION@.spec

dist-hook: $(SPECFILE)

spec: $(SPECFILE)

$(SPECFILE): ${top_srcdir}/gputils.spec
	sed \
	-e 's%\@VERSION\@%$(VERSION)%g' \
	-e 's%\@NAME\@%$(NAME)%g' \
	-e 's%\@EMAIL\@%$(EMAIL)%g' \
	${top_srcdir}/gputils.spec > $(SPECFILE)

CLEANFILES = $(SPECFILE)

EXTRA_DIST =\
	./include/stdhdr.h \
	./include/getopt.h \
	config.h.in \
	gputils.spec \
	stamp-h.in \
	TODO

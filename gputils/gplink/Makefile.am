## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gplink

AM_CPPFLAGS = -I${top_srcdir}/libgputils -I${top_srcdir}/include

gplink_SOURCES=\
	parse.y \
	cod.c \
	gplink.c \
	lst.c \
	map.c \
	script.c \
	scan.l \
	cod.h \
	gplink.h \
	lst.h \
	map.h \
	scan.h \
	script.h

gplink_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

AM_YFLAGS = -dvt
AM_LFLAGS = -i

EXTRA_DIST = parse.h

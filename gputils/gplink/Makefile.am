## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gplink

CPPFLAGS = @CPPFLAGS@ -I${top_srcdir}/libgputils -I${top_srcdir}/include

gplink_SOURCES=\
	cod.c \
	gplink.c \
	map.c \
	script.c \
	parse.y \
	scan.l \
	cod.h \
	gplink.h \
	map.h \
	script.h

gplink_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

YFLAGS = -dvt
LFLAGS = -i

EXTRA_DIST = scan.h parse.h

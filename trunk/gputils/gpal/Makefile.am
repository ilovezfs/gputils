## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gpal

AM_CPPFLAGS = -I${top_srcdir}/libgputils -I${top_srcdir}/include

gpal_SOURCES=\
	parse.y \
	analyze.c \
	codegen.c \
	codegen14.c \
	codegen16e.c \
	gpal.c \
	optimize.c \
	scan.l \
	symbol.c \
	tree.c \
	optimize.h \
	analyze.h \
	codegen.h \
	codegen14.h \
	codegen16e.h \
	gpal.h \
	scan.h \
	symbol.h \
	tree.h

gpal_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

AM_YFLAGS = -dvt
AM_LFLAGS = -i

EXTRA_DIST = parse.h

CLEANFILES = parse.output

MAINTAINERCLEANFILES = parse.c parse.h scan.c

## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gpasm

noinst_LIBRARIES = libgpasm.a

AM_CPPFLAGS = -I${top_srcdir}/libgputils -I${top_srcdir}/include

libgpasm_a_SOURCES=\
	parse.y \
	cod.c \
	coff.c \
	deps.c \
	directive.c \
	evaluate.c \
	gpasm.c \
	scan.l \
	gperror.c \
	lst.c \
	macro.c \
	processor.c \
	special.c \
	util.c \
	cod.h \
	coff.h \
	deps.h \
	directive.h \
	evaluate.h \
	gpasm.h \
	gperror.h \
	libgpasm.h \
	lst.h \
	macro.h \
	processor.h \
	scan.h \
	special.h

gpasm_SOURCES = main.c

gpasm_LDADD = libgpasm.a ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

# Need to have -d alone (not -dvt) for automake magic
AM_YFLAGS = -d -vt
AM_LFLAGS = -i

# Make sure we have parse.h when compiling other sources
BUILT_SOURCES = parse.h

CLEANFILES = parse.output

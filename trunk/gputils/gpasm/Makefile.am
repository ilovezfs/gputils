## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gpasm

noinst_LIBRARIES = libgpasm.a

CPPFLAGS = @CPPFLAGS@ -I${top_srcdir}/libgputils -I${top_srcdir}/include -DGPASM_HEADER_PATH=\"@GPASM_HEADER_PATH@\"

libgpasm_a_SOURCES=\
	cod.c \
	coff.c \
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
	coff.h \
	directive.h \
	evaluate.h \
	gpasm.h \
	gperror.h \
	libgpasm.h \
	lst.h \
	macro.h \
	processor.h \
	special.h

gpasm_SOURCES = main.c

gpasm_LDADD = libgpasm.a ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

YFLAGS = -dvt
LFLAGS = -i

EXTRA_DIST = scan.h parse.h

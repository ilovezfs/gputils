## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gpal

CPPFLAGS = @CPPFLAGS@ -I${top_srcdir}/libgputils -I${top_srcdir}/include -DGPASM_HEADER_PATH=\"@GPASM_HEADER_PATH@\"

gpal_SOURCES=\
	parse.y \
	analyze.c \
	codegen.c \
	codegen14.c \
	gpal.c \
	optimize.c \
	scan.l \
	symbol.c \
	tree.c \
	optimize.h \
	analyze.h \
	codegen.h \
	codegen14.h \
	gpal.h \
	scan.h \
	symbol.h \
	tree.h

gpal_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

YFLAGS = -dvt
LFLAGS = -i

EXTRA_DIST = scan.h parse.h

## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gpasm

noinst_LIBRARIES = libgpasm.a

AM_CPPFLAGS = -I${top_srcdir}/libgputils -I${top_srcdir}/include

libgpasm_a_SOURCES = \
	cod.c \
	cod.h \
	coff.c \
	coff.h \
	deps.c \
	deps.h \
	directive.c \
	directive.h \
	evaluate.c \
	evaluate.h \
	file.c \
	file.h \
	gpasm.c \
	gpasm.h.in \
	gpmsg.c \
	gpmsg.h \
	libgpasm.h \
	lst.c \
	lst.h \
	macro.c \
	macro.h \
	parse.y \
	ppparse.y \
	ppscan.l \
	preprocess.c \
	preprocess.h \
	processor.c \
	processor.h \
	scan.h \
	scan.l \
	special.c \
	special.h \
	util.c

gpasm_SOURCES = main.c

gpasm_LDADD = libgpasm.a ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

# Need to have -d alone (not -dvt) for automake magic
AM_YFLAGS = -d -vt
AM_LFLAGS = -i

# Make sure we have parse.h when compiling other sources
BUILT_SOURCES = parse.c parse.h ppparse.c ppparse.h scan.c ppscan.c gpasm.h

CLEANFILES = parse.output parse.c parse.h ppparse.output ppparse.c ppparse.h scan.c ppscan.c gpasm.h

gpasm.h: gpasm.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "${srcdir}/gpasm.h.in" > "$@"

ppparse.c: ppparse.y
	$(SHELL) $(YLWRAP) ${srcdir}/ppparse.y y.tab.c $@ y.tab.h ppparse.h y.output ppparse.output -- $(YACCCOMPILE) -p pp

#ppscan.c: LFLAGS += -P pp

ppscan.c: ppscan.l
	$(LEX) $(AM_LFLAGS) -P pp -o $@ ${srcdir}/ppscan.l

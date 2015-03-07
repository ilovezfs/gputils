## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gpasm

noinst_LIBRARIES = libgpasm.a

AM_CPPFLAGS = -I${top_srcdir}/libgputils -I${top_srcdir}/include

libgpasm_a_SOURCES = \
	cod.c \
	coff.c \
	deps.c \
	directive.c \
	evaluate.c \
	gpasm.c \
	gperror.c \
	lst.c \
	macro.c \
	preprocess.c \
	processor.c \
	special.c \
	util.c \
	cod.h \
	coff.h \
	deps.h \
	directive.h \
	evaluate.h \
	gperror.h \
	libgpasm.h \
	lst.h \
	macro.h \
	preprocess.h \
	processor.h \
	scan.h \
	special.h \
	gpasm.h.in \
	ppscan.l \
	scan.l \
	parse.y \
	ppparse.y

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

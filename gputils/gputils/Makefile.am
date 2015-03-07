## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gpdasm gplib gpstrip gpvc gpvo

AM_CPPFLAGS = -I${top_srcdir}/libgputils -I${top_srcdir}/include

LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

gpdasm_SOURCES= \
	labelset.c \
	labelset.h \
	gpdasm.c \
	gpdasm.h.in \
	parse.y \
	scan.h \
	scan.l

gplib_SOURCES= \
	gplib.c \
	gplib.h.in

gpstrip_SOURCES= \
	gpstrip.c \
	gpstrip.h.in

gpvc_SOURCES= \
	block.c dump.c gpvc.c \
	block.h dump.h gpvc.h.in

gpvo_SOURCES= \
	gpvo.c \
	gpvo.h.in

# Need to have -d alone (not -dvt) for automake magic
AM_YFLAGS = -d -vt
AM_LFLAGS = -i

BUILT_SOURCES = gpdasm.h gplib.h gpstrip.h gpvc.h gpvo.h parse.h scan.c

CLEANFILES = gpdasm.h gplib.h gpstrip.h gpvc.h gpvo.h parse.c parse.h parse.output scan.c

gpdasm.h: gpdasm.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "${srcdir}/gpdasm.h.in" > "$@"

gplib.h: gplib.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "${srcdir}/gplib.h.in" > "$@"

gpstrip.h: gpstrip.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "${srcdir}/gpstrip.h.in" > "$@"

gpvc.h: gpvc.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "${srcdir}/gpvc.h.in" > "$@"

gpvo.h: gpvo.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "${srcdir}/gpvo.h.in" > "$@"

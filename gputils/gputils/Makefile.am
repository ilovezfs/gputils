## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gpdasm gplib gpstrip gpvc gpvo

AM_CPPFLAGS = -I${top_srcdir}/libgputils -I${top_srcdir}/include

LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

gpdasm_SOURCES=\
	gpdasm.c\
	gpdasm.h.in

gplib_SOURCES=\
	gplib.c \
	gplib.h.in

gpstrip_SOURCES=\
	gpstrip.c \
	gpstrip.h.in

gpvc_SOURCES=\
	block.c dump.c gpvc.c \
	block.h dump.h gpvc.h.in

gpvo_SOURCES=\
	gpvo.c \
	gpvo.h.in

BUILT_SOURCES = gpdasm.h gplib.h gpstrip.h gpvc.h gpvo.h

CLEANFILES = gpdasm.h gplib.h gpstrip.h gpvc.h gpvo.h

gpdasm.h: gpdasm.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "$<" > "$@"

gplib.h: gplib.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "$<" > "$@"

gpstrip.h: gpstrip.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "$<" > "$@"

gpvc.h: gpvc.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "$<" > "$@"

gpvo.h: gpvo.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "$<" > "$@"

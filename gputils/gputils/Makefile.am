## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gpdasm gplib gpvc gpvo

AM_CPPFLAGS = -I${top_srcdir}/libgputils -I${top_srcdir}/include

gpdasm_SOURCES=\
	gpdasm.c\
	gpdasm.h

gpdasm_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

gplib_SOURCES=\
	gplib.c \
	gplib.h

gplib_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

gpvc_SOURCES=\
	block.c dump.c gpvc.c \
	block.h dump.h gpvc.h

gpvc_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

gpvo_SOURCES=\
	gpvo.c \
	gpvo.h

gpvo_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

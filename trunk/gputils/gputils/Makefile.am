## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gpdasm gplib gpstrip gpvc gpvo

AM_CPPFLAGS = -I${top_srcdir}/libgputils -I${top_srcdir}/include

LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

gpdasm_SOURCES=\
	gpdasm.c\
	gpdasm.h

gplib_SOURCES=\
	gplib.c \
	gplib.h

gpstrip_SOURCES=\
	gpstrip.c \
	gpstrip.h

gpvc_SOURCES=\
	block.c dump.c gpvc.c \
	block.h dump.h gpvc.h

gpvo_SOURCES=\
	gpvo.c \
	gpvo.h

## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I${top_srcdir}/include

noinst_LIBRARIES = libiberty.a

libiberty_a_SOURCES =\
	getopt.c \
	getopt1.c

libiberty_a_LIBADD = $(IBERTYOBJS)
libiberty_a_DEPENDENCIES = $(IBERTYOBJS)

EXTRA_DIST = snprintf.c strndup.c vasprintf.c vsnprintf.c README

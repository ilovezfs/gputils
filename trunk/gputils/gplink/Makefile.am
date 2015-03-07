## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gplink

AM_CPPFLAGS = -I${top_srcdir}/libgputils -I${top_srcdir}/include

gplink_SOURCES=\
	parse.y \
	cod.c \
	gplink.c \
	lst.c \
	map.c \
	script.c \
	scan.l \
	cod.h \
	gplink.h.in \
	lst.h \
	map.h \
	scan.h \
	script.h

gplink_LDADD = ${top_builddir}/@LIBGPUTILS@ ${top_builddir}/@LIBIBERTY@

# Need to have -d alone (not -dvt) for automake magic
AM_YFLAGS = -d -vt
AM_LFLAGS = -i

# Make sure we have parse.h when compiling other sources
BUILT_SOURCES = gplink.h parse.h

CLEANFILES = gplink.h parse.c parse.h parse.output scan.c

gplink.h: gplink.h.in
	sed -e "s/@REVISION@/$$(${top_srcdir}\/get_cl_revision.sh -s ${top_srcdir}\/ChangeLog)/g" "${srcdir}/gplink.h.in" > "$@"

## Process this file with automake to produce Makefile.in

AM_CPPFLAGS =\
	-I$(top_srcdir)/include \
	-DHEADER_PATH=\"@GPUTILS_HEADER_PATH@\" \
	-DLKR_PATH=\"@GPUTILS_LKR_PATH@\" \
	-DLIB_PATH=\"@GPUTILS_LIB_PATH@\" \
	-DPUB_PATH=\"@GPUTILS_PUB_PATH@\"

noinst_LIBRARIES = libgputils.a

libgputils_a_SOURCES =\
	gparchive.c \
	gpcfg.c \
	gpcfg-table.c \
	gpcod.c \
	gpcoffgen.c \
	gpcofflink.c \
	gpcoffopt.c \
	gpdis.c \
	gphash.c \
	gpmemory.c \
	gpmessage.c \
	gpopcode.c \
	gpprocessor.c \
	gpreadhex.c \
	gpreadobj.c \
	gpreg-table.c \
	gpregister.c \
	gpsymbol.c \
	gpsystem.c \
	gpwritehex.c \
	gpwriteobj.c \
	gparchive.h \
	gpcfg.h \
	gpcod.h \
	gpcoff.h \
	gpcoffgen.h \
	gpcofflink.h \
	gpcoffopt.h \
	gpdis.h \
	gphash.h \
	gpmemory.h \
	gpmessage.h \
	gpopcode.h \
	gpprocessor.h \
	gpreadhex.h \
	gpreadobj.h \
	gpregister.h \
	gpsymbol.h \
	gpsystem.h \
	gptypes.h \
	gpwritehex.h \
	gpwriteobj.h \
	libgputils.h

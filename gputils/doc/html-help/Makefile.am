## Process this file with automake to produce Makefile.in

all: index.html

index.html: $(DEFAULT_MPLABX_PATH)/mpasmx/8bit_device.info
	$(top_srcdir)/../scripts/tools/config-help.pl -gp $(top_srcdir)/.. -od ./ -v 4

EXTRA_DIST = *.css *.html

CLEANFILES = *.css *.html

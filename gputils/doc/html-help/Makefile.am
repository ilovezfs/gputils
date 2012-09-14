## Process this file with automake to produce Makefile.in

pkgdatadir = @GPUTILS_HTMLDOC_PATH@

all: html-doc

html-doc: $(DEFAULT_MPLABX_PATH)/mpasmx/8bit_device.info
	@if [ "$(ENABLE_HTML_DOC)" == "yes" ]; then \
	  $(top_srcdir)/../scripts/tools/device-help.pl -gp $(top_srcdir)/.. -od ./ -v 4; \
	fi

install-html:
	@if [ "$(ENABLE_HTML_DOC)" == "yes" ]; then \
	  $(install_sh) -d "$(DESTDIR)$(pkgdatadir)"; \
	  $(install_sh) -c -m 644 *.css *.html "$(DESTDIR)$(pkgdatadir)"; \
	fi

EXTRA_DIST = *.css *.html
CLEANFILES = *.css *.html

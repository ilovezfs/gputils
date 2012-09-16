## Process this file with automake to produce Makefile.in

# HTML help is built only when building the distribution
# (make dist is invoked at ato level). It can be built explicitely by
# make html-doc from html-help directory.

pkgdatadir = @GPUTILS_HTMLDOC_PATH@

html-doc: $(DEFAULT_MPLABX_PATH)/mpasmx/8bit_device.info
	@if [ "$(ENABLE_HTML_DOC)" == "yes" ]; then \
	  $(top_srcdir)/../scripts/tools/device-help.pl -gp $(top_srcdir)/.. -od ./ -v 4; \
	fi

install: install-html

install-html:
	@if [ "$(ENABLE_HTML_DOC)" == "yes" ]; then \
	  $(install_sh) -d "$(DESTDIR)$(pkgdatadir)"; \
	  $(install_sh) -c -m 644 *.css *.html "$(DESTDIR)$(pkgdatadir)"; \
	fi

dist-hook: html-doc
	cp *.html *.css $(distdir)

MAINTAINERCLEANFILES = *.css *.html

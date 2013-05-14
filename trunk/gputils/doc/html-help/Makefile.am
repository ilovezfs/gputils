## Process this file with automake to produce Makefile.in

# HTML help is built only when building the distribution
# (make dist is invoked at ato level). It can be built explicitely by
# make html-doc from html-help directory.

pkgdatadir = @GPUTILS_HTMLDOC_PATH@

html-doc:
	if [ "$(ENABLE_HTML_DOC)" == "yes" ]; then \
	  if [ ! -e $(DEFAULT_MPLABX_PATH)/mpasmx/8bit_device.info ]; then \
	    @echo "Can't find mplabx installation; HTML documentation will not be built."; \
	  else \
	    $(top_srcdir)/../scripts/tools/device-help.pl -gp $(top_srcdir) -od ./ -v 4; \
	  fi; \
	fi

install: install-html

install-html:
	if [ "$(ENABLE_HTML_DOC)" == "yes" ]; then \
	  if [ -n "$$(ls *.html 2>/dev/null)" -a -n "$$(ls *.css 2>/dev/null)" ]; then \
	    $(install_sh) -d "$(DESTDIR)$(pkgdatadir)"; \
	    $(install_sh) -c -m 644 *.css *.html "$(DESTDIR)$(pkgdatadir)"; \
	  fi; \
	fi

dist-hook: html-doc
	if [ -n "$$(ls *.html 2>/dev/null)" -a -n "$$(ls *.css 2>/dev/null)" ]; then \
	  cp *.html *.css $(distdir); \
	fi

MAINTAINERCLEANFILES = *.css *.html

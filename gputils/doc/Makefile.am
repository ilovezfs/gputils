## Process this file with automake to produce Makefile.in

gputils.ps: gputils.lyx
	if [ "$(srcdir)" != "." ] ; then \
	  cp -f $(srcdir)/$(notdir $<) . ; \
	fi
	lyx -e ps gputils.lyx

gputils.pdf: gputils.lyx
	if [ "$(srcdir)" != "." ] ; then \
	  cp -f $(srcdir)/$(notdir $<) . ; \
	fi
	lyx -e pdf gputils.lyx

EXTRA_DIST = gputils.lyx gputils.ps gputils.pdf

MAINTAINERCLEANFILES = gputils.ps gputils.pdf


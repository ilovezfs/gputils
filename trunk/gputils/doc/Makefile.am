## Process this file with automake to produce Makefile.in

gputils.ps: gputils.lyx
	lyx -e ps gputils.lyx

gputils.pdf: gputils.lyx
	lyx -e pdf gputils.lyx

EXTRA_DIST = gputils.lyx gputils.ps gputils.pdf

MAINTAINERCLEANFILES = gputils.ps gputils.pdf


## Process this file with automake to produce Makefile.in

MAN_PAGES =\
	gpal.1 \
	gpasm.1 \
	gpdasm.1 \
	gplib.1 \
	gplink.1 \
	gputils.1 \
	gpvc.1 \
	gpvo.1

gputils.pdf: gputils.ps
	ps2pdf gputils.ps gputils.pdf

man_MANS = $(MAN_PAGES)

EXTRA_DIST = $(MAN_PAGES) gputils.lyx gputils.ps gputils.pdf


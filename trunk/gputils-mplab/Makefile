PACKAGE_NAME = gputils-mplab
PACKAGE_VERSION = 0.3

DIST_NAME=$(PACKAGE_NAME)-$(PACKAGE_VERSION)

DISTFILES=\
	COPYING.txt\
	ChangeLog.txt\
	README.txt\
	gputils-mplab.nsi\
	tlgputils.ini\
	gpasm.mtc\
	gplink.mtc

all:

gputils-mplab.nsi: gputils-mplab.nsi.in
	sed -e s/@VERSION@/$(PACKAGE_VERSION)/ gputils-mplab.nsi.in > gputils-mplab.nsi

dist: $(DISTFILES)
	-rm -Rf $(DIST_NAME)
	-rm -f $(DIST_NAME).zip
	mkdir $(DIST_NAME)
	cp $(DISTFILES) $(DIST_NAME)
	zip -rq $(DIST_NAME).zip $(DIST_NAME)
	rm -Rf $(DIST_NAME)

clean:
	rm -f gputils-mplab.nsi

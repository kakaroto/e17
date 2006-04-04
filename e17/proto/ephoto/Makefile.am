## Process this file with automake to produce Makefile.in

SUBDIRS = src data

EXTRA_DIST = \
	README AUTHORS COPYING ephoto.spec \
	debian/changelog \
	debian/changelog.in \
	debian/compat \
        debian/control \
        debian/copyright \
	debian/rules


MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in  ephoto.spec \
		       debian/changelog

dist-hook:
	if test -d data; then \
		(cd $(srcdir); tar -cf - data) | (cd $(distdir); tar -xf -; rm -rf `find . -name CVS`) \
	fi

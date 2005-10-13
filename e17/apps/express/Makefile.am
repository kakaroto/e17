## Process this file with automake to produce Makefile.in

SUBDIRS = src data

EXTRA_DIST = \
	README AUTHORS COPYING \
	debian/changelog.in debian/changelog \
	debian/control debian/rules debian/docs \
	debian/copyright

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in debian/changelog

dist-hook:
	if test -d data; then \
	  (cd $(srcdir); tar -cf - data) | (cd $(distdir); tar -xf -; rm -rf `find . -name CVS`) \
	fi

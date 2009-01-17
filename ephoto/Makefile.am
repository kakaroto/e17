## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in Makefile aclocal.m4 config.* \
                   configure compile depcomp install-sh \
                   ltconfig ltmain.sh missing mkinstalldirs \
                   stamp-h.in

SUBDIRS = src data po

EXTRA_DIST = \
	README AUTHORS COPYING ephoto.spec

ACLOCAL_AMFLAGS = -I m4

dist-hook:
	if test -d data; then \
		(cd $(srcdir); tar -cf - data) | (cd $(distdir); tar -xf -; rm -rf `find . -name CVS`) \
	fi

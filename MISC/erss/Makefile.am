## Process this file with automake to produce Makefile.in

SUBDIRS = src data

EXTRA_DIST = \
	autogen.sh README AUTHORS COPYING

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in

ACLOCAL_AMFLAGS = -I m4

dist-hook:
	if test -d data; then \
	  (cd $(srcdir); tar -cf - data) | (cd $(distdir); tar -xf -) \
	fi

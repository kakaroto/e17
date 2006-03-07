## Process this file with automake to produce Makefile.in

SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess e_utils.spec \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in debian/changelog

dist-hook:
	./gendoc; \
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for file in data/*; do \
	    cp -pR $$file $(distdir)/data; \
	  done \
	fi

EXTRA_DIST = AUTHORS COPYING COPYING-PLAIN INSTALL NEWS README e_utils.spec \
	     debian/control debian/copyright \
	     debian/changelog debian/rules debian/eutils.files \
	     debian/docs e_utils.spec.in

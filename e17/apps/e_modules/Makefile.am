## Process this file with automake to produce Makefile.in

SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in debian/changelog e_modules.spec

dist-hook:
	./gendoc; \
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for file in data/*; do \
	    cp -pR $$file $(distdir)/data; \
	  done \
	fi
							  

EXTRA_DIST = AUTHORS COPYING COPYING-PLAIN INSTALL NEWS README e_modules.spec \
	     debian/changelog.in debian/changelog debian/copyright \
	     debian/rules debian/control debian/docs e_modules.spec.in

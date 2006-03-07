## Process this file with automake to produce Makefile.in

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in epsilon_docs.tar epsilon.c \
		       epsilon.spec epsilon.pc

install-data-local:
        @$(NORMAL_INSTALL)
	if test -d $(srcdir)/data; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/data; \
	  for d in $(srcdir)/data/*; do \
	    cp -pR $$d $(DESTDIR)$(pkgdatadir)/data; \
          done \
	fi

bin_SCRIPTS = epsilon-config

EXTRA_DIST = README AUTHORS COPYING epsilon.spec.in epsilon.spec gendoc \
	     Doxyfile debian/changelog debian/rules \
	     debian/copyright debian/control debian/epsilon0-bin.files \
	     debian/libepsilon0.files debian/libepsilon0-dev.files \
	     epsilon.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = epsilon.pc

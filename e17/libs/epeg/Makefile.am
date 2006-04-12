## Process this file with automake to produce Makefile.in

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in epeg.c \
		       epeg.spec debian/changelog

install-data-local:
        @$(NORMAL_INSTALL)
	if test -d $(srcdir)/data; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/data; \
	  for d in $(srcdir)/data/*; do \
	    cp -pR $$d $(DESTDIR)$(pkgdatadir)/data; \
          done \
	fi

dist-hook:
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for file in data/*; do \
	    cp -pR $$file $(distdir)/data; \
	  done \
	fi

bin_SCRIPTS = epeg-config

EXTRA_DIST = README AUTHORS COPYING COPYING-PLAIN \
	     epeg.spec epeg.spec.in epeg.c.in gendoc \
	     Doxyfile \
             debian/changelog \
             debian/changelog.in \
             debian/control \
             debian/copyright \
             debian/libepeg0-dev.files \
    	     debian/epeg0-bin.files \
             debian/libepeg0.files \
             debian/rules

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = epeg.pc

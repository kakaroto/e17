## Process this file with automake to produce Makefile.in

SUBDIRS = src test

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
					   config.h.in config.sub configure install-sh \
					   ltconfig ltmain.sh missing mkinstalldirs \
					   stamp-h.in

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/data; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir); \
	  for d in $(srcdir)/data/*; do \
		  cp -afr $$d $(DESTDIR)$(pkgdatadir); \
	  done \
	fi

dist-hook:
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for d in data/*; do \
		  cp -ar $$d $(distdir)/data; \
	  done \
	fi

bin_SCRIPTS = ewl-config

EXTRA_DIST = README AUTHORS COPYING ewl.m4 ewl.spec

m4datadir = $(datadir)/aclocal
m4data_DATA = ewl.m4

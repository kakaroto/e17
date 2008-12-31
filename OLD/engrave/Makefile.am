## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = 1.5 foreign

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess engrave.spec \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in depcomp ylwrap

bin_SCRIPTS =

#install-data-local:
#	@$(NORMAL_INSTALL)
#	if test -d $(srcdir)/data; then \
#	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/fonts; \
#	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/themes; \
#	  for d in $(srcdir)/data/fonts/*.ttf; do \
#	      cp -afr $$d $(DESTDIR)$(pkgdatadir)/fonts; \
#	  done \
#	  for d in $(srcdir)/data/themes/*/*.eet; do \
#	      cp -afr $$d $(DESTDIR)$(pkgdatadir)/themes; \
#	  done \
#	fi

dist-hook:
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for d in data/*; do \
	      cp -ar $$d $(distdir)/data; \
	  done \
	fi

EXTRA_DIST = README AUTHORS COPYING engrave.spec engrave.spec.in engrave.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = engrave.pc

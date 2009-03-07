## Process this file with automake to produce Makefile.in

SUBDIRS = src resources

ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in

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

#dist-hook:
#	if test -d data; then \
#	  mkdir $(distdir)/data; \
#	  for d in data/*; do \
#	      cp -ar $$d $(distdir)/data; \
#	  done \
#	fi

EXTRA_DIST = README AUTHORS COPYING TODO

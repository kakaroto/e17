## Process this file with automake to produce Makefile.in

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
	    fi \
	  done \
	fi
	if test -d $(srcdir)/examples/bits; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/examples; \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/examples/bits; \
	  for file in $(srcdir)/examples/bits/*; do \
	    if test -f $$file; then \
	      $(INSTALL_DATA) $$file $(DESTDIR)$(pkgdatadir)/examples/bits; \
	    fi \
	  done \
	fi
	if test -d $(srcdir)/examples/images; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/examples; \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/examples/images; \
	  for file in $(srcdir)/examples/images/*; do \
	    if test -f $$file; then \
	      $(INSTALL_DATA) $$file $(DESTDIR)$(pkgdatadir)/examples/images; \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi
	if test -d examples/bits; then \
	  mkdir $(distdir)/examples; \
	  mkdir $(distdir)/examples/bits; \
	  for file in examples/bits/*; do \
	    if test -f $$file; then \
	      cp -p $$file $(distdir)/examples/bits; \
	    fi \
	  done \
	fi
	if test -d examples/images; then \
	  mkdir $(distdir)/examples; \
	  mkdir $(distdir)/examples/images; \
	  for file in examples/images/*; do \
	    if test -f $$file; then \
	      cp -p $$file $(distdir)/examples/images; \
	    fi \
	  done \
	fi

EXTRA_DIST = config.rpath  README AUTHORS COPYING etcher.spec etcher.spec.in

ACLOCAL_AMFLAGS = -I m4

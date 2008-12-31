## Process this file with automake to produce Makefile.in

MYAPPNAME=etk_server
SUBDIRS = scripts src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs depcomp \
		       stamp-h.in $(MYAPPNAME).c

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/data; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/data; \
	  for d in $(srcdir)/data/*; do \
	    cp -pR $$d $(DESTDIR)$(pkgdatadir)/data; \
          done \
	fi

dist-hook:
	./gendoc; \
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for file in data/*; do \
	    cp -pR $$file $(distdir)/data; \
	  done \
	fi

EXTRA_DIST = README \
AUTHORS \
COPYING \
COPYING-PLAIN \
$(MYAPPNAME).spec.in \
$(MYAPPNAME).c.in \
gendoc \
Doxyfile

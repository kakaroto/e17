## Process this file with automake to produce Makefile.in

MYAPPNAME=estickies
SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs depcomp \
		       stamp-h.in $(MYAPPNAME)_docs.tar $(MYAPPNAME)_docs.tar.gz $(MYAPPNAME).c

ACLOCAL_AMFLAGS = -I m4

install-data-local:
        @$(NORMAL_INSTALL)
	if test -d $(srcdir)/data; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/data; \
	  for d in $(srcdir)/data/*; do \
	    cp -pR $$d $(DESTDIR)$(pkgdatadir)/data; \
          done \
	fi

EXTRA_DIST = README \
AUTHORS \
COPYING \
COPYING-PLAIN \
$(MYAPPNAME).c.in

## Process this file with automake to produce Makefile.in

MYAPPNAME=enterminus
SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in $(MYAPPNAME)_docs.tar $(MYAPPNAME)_docs.tar.gz $(MYAPPNAME).c

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
	tar cvf $(MYAPPNAME)_docs.tar doc; \
	rm -f $(MYAPPNAME)_docs.tar.gz; \
	gzip -9 $(MYAPPNAME)_docs.tar; \
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for file in data/*; do \
	    cp -pR $$file $(distdir)/data; \
	  done \
	fi
							  

bin_SCRIPTS = $(MYAPPNAME)-config

EXTRA_DIST = README AUTHORS COPYING COPYING-PLAIN $(MYAPPNAME).spec $(MYAPPNAME).c.in gendoc Doxyfile $(MYAPPNAME)_docs.tar.gz

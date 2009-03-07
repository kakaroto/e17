## Process this file with automake to produce Makefile.in

SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in examine_docs.tar examine_docs.tar.gz examine.c

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
	tar cvf examine_docs.tar doc; \
	rm -f examine_docs.tar.gz; \
	gzip -9 examine_docs.tar; \
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for file in data/*; do \
	    cp -pR $$file $(distdir)/data; \
	  done \
	fi
							  

EXTRA_DIST = README AUTHORS COPYING COPYING-PLAIN examine.spec \
	     examine.c.in gendoc Doxyfile examine_docs.tar.gz

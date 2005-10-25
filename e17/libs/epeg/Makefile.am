## Process this file with automake to produce Makefile.in

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in epeg_docs.tar epeg_docs.tar.gz epeg.c \
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
	./gendoc; \
	tar cvf epeg_docs.tar doc; \
	rm -f epeg_docs.tar.gz; \
	gzip -9 epeg_docs.tar; \
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for file in data/*; do \
	    cp -pR $$file $(distdir)/data; \
	  done \
	fi
							  

bin_SCRIPTS = epeg-config

EXTRA_DIST = README AUTHORS COPYING COPYING-PLAIN \
	     epeg.spec epeg.spec.in epeg.c.in gendoc \
	     Doxyfile epeg_docs.tar.gz \
             debian/changelog  debian/changelog.in \
             debian/control \
             debian/copyright \
             debian/libepeg0-dev.files \
    	     debian/epeg0-bin.files \
             debian/libepeg0.files \
             debian/rules

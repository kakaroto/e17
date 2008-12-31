## Process this file with automake to produce Makefile.in

SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in iconbar_docs.tar iconbar_docs.tar.gz iconbar.c

dist-hook:
	./gendoc; \
	tar cvf iconbar_docs.tar doc; \
	rm -f iconbar_docs.tar.gz; \
	gzip -9 iconbar_docs.tar; \
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for file in data/*; do \
	    cp -pR $$file $(distdir)/data; \
	  done \
	fi
							  

EXTRA_DIST = README AUTHORS COPYING iconbar.spec

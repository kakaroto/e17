## Process this file with automake to produce Makefile.in

SUBDIRS = src data doc debian

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in imlib2_docs.tar imlib2_docs.tar.gz imlib2.c

dist-hook:
	./gendoc; \
	tar cvf imlib2_docs.tar doc; \
	rm -f imlib2_docs.tar.gz; \
	gzip -9 imlib2_docs.tar;
							  

bin_SCRIPTS = imlib2-config

EXTRA_DIST = README AUTHORS COPYING COPYING-PLAIN TODO imlib2.spec imlib2.c.in imlib2.pc.in gendoc Doxyfile imlib2_docs.tar.gz

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = imlib2.pc

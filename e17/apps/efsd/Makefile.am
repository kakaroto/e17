## Process this file with automake to produce Makefile.in

SUBDIRS = efsd demo tools doc 

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in

bin_SCRIPTS = efsd-config

EXTRA_DIST = README AUTHORS COPYING

docs:
	$(MAKE) -C $(top_srcdir)/doc docs

if HAVE_JADE
html-docs:
	$(MAKE) -C $(top_srcdir)/doc html-docs
endif


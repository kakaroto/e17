## Process this file with automake to produce Makefile.in

SUBDIRS = efsd tools doc

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in

bin_SCRIPTS = efsd-config

EXTRA_DIST = README AUTHORS COPYING

if HAVE_DOCGEN
manual:
	$(MAKE) -C $(top_srcdir)/doc manual

if HAVE_JADE
html-manual:
	$(MAKE) -C $(top_srcdir)/doc html-manual

endif
endif

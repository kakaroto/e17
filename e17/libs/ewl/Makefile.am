
## Process this file with automake to produce Makefile.in

SUBDIRS = src test tools doc

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		   config.h.in config.sub configure install-sh \
		   ltconfig ltmain.sh missing mkinstalldirs \
		   stamp-h.in

install-data-local:
	@$(NORMAL_INSTALL)
	if [ -d $(srcdir)/data ]; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir); \
	    (cd $(srcdir)/data; tar -cf - *) | (cd $(DESTDIR)$(pkgdatadir); tar -xf -) \
	fi

dist-hook:
	if [ -d data ]; then \
	  (cd $(srcdir)/data; tar -cf - *) | (cd $(distdir); tar -xf -) \
	fi

bin_SCRIPTS = ewl-config

EXTRA_DIST = README AUTHORS COPYING ewl.m4 ewl.spec

m4datadir = $(datadir)/aclocal
m4data_DATA = ewl.m4

docs:
	$(MAKE) -C $(top_srcdir)/doc docs

if HAVE_JADE
html-docs:
	$(MAKE) -C $(top_srcdir)/doc html-docs
endif


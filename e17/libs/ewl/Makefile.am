
## Process this file with automake to produce Makefile.in

SUBDIRS = src test tools data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		   config.h.in config.sub configure install-sh \
		   ltconfig ltmain.sh missing mkinstalldirs \
		   stamp-h.in

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


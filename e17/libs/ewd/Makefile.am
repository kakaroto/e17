## Process this file with automake to produce Makefile.in

SUBDIRS = src test doc

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess ewd-config.h.in \
			config.sub configure install-sh ltconfig ltmain.sh \
			missing mkinstalldirs stamp-h.in config.h.in

dist-hook:
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for file in data/*; do \
	    if test -f $$file; then \
	      cp -p $$file $(distdir)/data; \
	    fi \
	  done \
	fi

docs:
	$(MAKE) -C $(top_srcdir)/doc docs

if HAVE_JADE
html-docs:
	$(MAKE) -C $(top_srcdir)/doc html-docs
endif

bin_SCRIPTS = ewd-config

EXTRA_DIST = README AUTHORS COPYING ewd.m4 ewd.spec

m4datadir = $(datadir)/aclocal
m4data_DATA = ewd.m4

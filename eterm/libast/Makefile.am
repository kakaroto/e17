# $Id$

AUTOMAKE_OPTIONS = foreign
SUBDIRS = include src test

bin_SCRIPTS = libast-config

MACROS = libast.m4
macrodir = $(datadir)/aclocal

EXTRA_DIST = DESIGN README $(MACROS)

test: all
	cd test && $(MAKE) test

perf: all
	cd test && $(MAKE) perf

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(macrodir)
	for i in $(MACROS) ; do \
	    $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(macrodir)/ ; \
	done

.PHONY: test

MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = default.edc
SUBDIRS = icons
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/icons/default/icons

filesdir = $(pkgdatadir)/icons
files_DATA = default.edj

default.edj: Makefile default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/icons/default/default.edc \
	$(top_builddir)/data/icons/default/default.edj

clean-local:
	rm -f *.edj

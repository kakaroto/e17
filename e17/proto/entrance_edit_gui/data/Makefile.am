MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = icons.edc
SUBDIRS = icons
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/icons/

filesdir = $(pkgdatadir)/icons
files_DATA = default.edj

default.edj: Makefile default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/icons.edc \
	$(top_builddir)/data/icons.edj

clean-local:
	rm -f *.edj

MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = edje_viewer.edc \
	     edje_viewer.desktop

SUBDIRS = images

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/images

filesdir = $(pkgdatadir)
files_DATA = edje_viewer.edj

desktopdir   = $(datadir)/applications
desktop_DATA = edje_viewer.desktop

edje_viewer.edj: Makefile
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/edje_viewer.edc \
	$(top_builddir)/data/edje_viewer.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache

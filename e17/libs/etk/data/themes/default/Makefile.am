MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = default.edc
SUBDIRS = fonts \
	  images \
	  macros \
	  widgets
	  
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/default/images \
-fd $(top_srcdir)/data/themes/default/fonts

filesdir = $(pkgdatadir)/themes
files_DATA = default.edj

default.edj: Makefile default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default/default.edc \
	$(top_builddir)/data/themes/default/default.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache

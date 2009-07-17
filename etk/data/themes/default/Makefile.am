MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = main.edc
SUBDIRS = img
	  
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/default/img \
-fd $(top_srcdir)/data/themes/default

filesdir = $(pkgdatadir)/themes
files_DATA = default.edj

THEME_DEP = main.edc

default.edj: Makefile $(THEME_DEP)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default/main.edc \
	$(top_builddir)/data/themes/default/default.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache

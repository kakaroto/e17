MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = default.edc
SUBDIRS = images

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/default/images

filesdir = $(pkgdatadir)/themes
files_DATA = default.edj

THEME_DEP = \
default.edc

default.edj: Makefile $(THEME_DEP)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default/default.edc \
	$(top_builddir)/data/themes/default/default.edj
			
clean-local:
	rm -rf *.edj
	rm -rf autom4te.cache

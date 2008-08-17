MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = mc.edc
SUBDIRS = images

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/mc/images

filesdir = $(pkgdatadir)/themes
files_DATA = mc.edj

THEME_DEP = \
mc.edc

mc.edj: Makefile $(THEME_DEP)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/mc/mc.edc \
	$(top_builddir)/data/themes/mc/mc.edj
			
clean-local:
	rm -rf *.edj
	rm -rf autom4te.cache

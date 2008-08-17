MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = default.edc \
             borders.edc \
             borders_macros.edc \
             background.edc \
             pointers.edc
SUBDIRS = fonts \
	  images
	  
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/wm/default/images \
-fd $(top_srcdir)/data/wm/default/fonts

filesdir = $(pkgdatadir)/wm
files_DATA = default.edj

THEME_DEP = \
default.edc \
borders.edc \
borders_macros.edc \
background.edc \
pointers.edc

default.edj: Makefile $(THEME_DEP)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/wm/default/default.edc \
	$(top_builddir)/data/wm/default/default.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache

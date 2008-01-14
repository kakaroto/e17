MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/theme/default/images \
-fd $(top_srcdir)/data/theme/default/fonts

defaultdir = $(pkgdatadir)/theme
default_DATA = default.edj 

EXTRA_DIST = $(wildcard fonts/*.ttf) \
               $(wildcard images/*.png) \
               $(wildcard images/animation/*.png) \
               $(wildcard *.edc) 

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/theme/default/default.edc \
	$(top_builddir)/data/theme/default/default.edj

clean-local:
	rm -f *.edj


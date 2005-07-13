MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = default.edc

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/widget_themes/default/images \
-fd $(top_srcdir)/data/widget_themes/default/fonts

defaultdir = $(pkgdatadir)/widget_themes
default_DATA = default.edj

default.edj: Makefile default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/widget_themes/default/default.edc \
	$(top_builddir)/data/widget_themes/default/default.edj

clean-local:
	rm -f *.edj

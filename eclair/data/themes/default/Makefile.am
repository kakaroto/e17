MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = default.edc

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/default/images \
-fd $(top_srcdir)/data/themes/default/fonts

defaultdir = $(pkgdatadir)/themes
default_DATA = default.edj

default.edj: Makefile default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default/default.edc \
	$(top_builddir)/data/themes/default/default.edj

clean-local:
	rm -f *.edj

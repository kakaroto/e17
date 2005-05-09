MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = gant.edc

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/gant/images \
-fd $(top_srcdir)/data/themes/gant/fonts

gantdir = $(pkgdatadir)/themes
gant_DATA = gant.edj

gant.edj: Makefile gant.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/gant/gant.edc \
	$(top_builddir)/data/themes/gant/gant.edj

clean-local:
	rm -f *.edj

MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = swallow_me.edc

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/swallow_me/images \
-fd $(top_srcdir)/data/themes/swallow_me/fonts

swallow_medir = $(pkgdatadir)/themes
swallow_me_DATA = swallow_me.edj

swallow_me.edj: Makefile swallow_me.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/swallow_me/swallow_me.edc \
	$(top_builddir)/data/themes/swallow_me/swallow_me.edj

clean-local:
	rm -f *.edj

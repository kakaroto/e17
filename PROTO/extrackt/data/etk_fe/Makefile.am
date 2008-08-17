MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = etk_fe.edc extrackt.glade $(wildcard images/*.png)

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/etk_fe/images

etk_fedir = $(pkgdatadir)/etk_fe
etk_fe_DATA = etk_fe.edj extrackt.glade

etk_fe.edj: Makefile etk_fe.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/etk_fe/etk_fe.edc \
	$(top_builddir)/data/etk_fe/etk_fe.edj

clean-local:
	rm -f *.edj


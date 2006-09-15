MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = gui.edc

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/images

guidir = $(pkgdatadir)
gui_DATA = gui.edj

gui.edj: Makefile gui.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/gui.edc \
	$(top_builddir)/data/gui.edj

clean-local:
	rm -f *.edj

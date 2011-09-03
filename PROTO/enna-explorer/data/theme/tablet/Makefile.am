MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc

EDJE_FLAGS = \
-id $(top_srcdir)/data/theme/tablet/images -id $(top_srcdir)/data/theme/tablet/gadgets/date/images -id $(top_srcdir)/data/theme/tablet/images/icons -id $(top_srcdir)/data/theme/tablet/elm \
-fd $(top_srcdir)/data/theme/tablet/fonts

tabletdir = $(pkgdatadir)/theme

tablet_DATA = tablet.edj


FONTS =

EDC = \
icons.edc \
tablet.edc \
elm/*.edc

IMGS = \
elm/*.png \
images/*.png \
images/icons/*.png

EXTRA_DIST = \
	$(FONTS) \
	$(EDC) \
	$(IMGS)

tablet.edj: Makefile $(EXTRA_DIST)
	$(AM_V_GEN)$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/theme/tablet/tablet.edc \
	$(top_builddir)/data/theme/tablet/tablet.edj

clean-local:
	rm -f *.edj

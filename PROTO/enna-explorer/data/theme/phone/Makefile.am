MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc

EDJE_FLAGS = \
-id $(top_srcdir)/data/theme/phone/images -id $(top_srcdir)/data/theme/phone/gadgets/date/images -id $(top_srcdir)/data/theme/phone/images/icons -id $(top_srcdir)/data/theme/phone/elm \
-fd $(top_srcdir)/data/theme/phone/fonts

phonedir = $(pkgdatadir)/theme

phone_DATA = phone.edj


FONTS =

EDC = \
icons.edc \
phone.edc \
elm/*.edc

IMGS = \
images/*.png \
images/icons/*.png \
elm/*.png

EXTRA_DIST = \
	$(FONTS) \
	$(EDC) \
	$(IMGS)

phone.edj: Makefile $(EXTRA_DIST)
	$(AM_V_GEN)$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/theme/phone/phone.edc \
	$(top_builddir)/data/theme/phone/phone.edj

clean-local:
	rm -f *.edj

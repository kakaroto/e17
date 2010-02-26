THEME = default
EDC = $(THEME).edc
EDJ = $(EDC:$.edc=$.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EDJ)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJ)

all: $(EDJ)

.edc.edj: $(EDC)
	edje_cc -v \
	 -id $(top_srcdir)/data/themes/$(THEME)/img \
	 -fd $(top_srcdir)/data/themes/$(THEME)/fonts \
	 $< $@

EXTRA_DIST = fonts img groups default.edc

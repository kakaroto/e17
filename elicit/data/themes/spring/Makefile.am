THEME = spring
EDC = $(THEME).edc
EET = $(EDC:%.edc=%.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EET)
EXTRA_DIST = $(EDC)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EET)

all: $(EDC)

.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(THEME)/img -fd $(top_srcdir)/data/themes/$(THEME)/fonts $< $@

preview: $(EET)
	edje $(EET)


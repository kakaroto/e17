THEME = winter
EDC = $(THEME).edc
EET = $(EDC:%.edc=%.eet)
SUFFIXES = .edc .eet

CLEANFILES = $(EET)
EXTRA_DIST = $(EDC)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EET)

all: $(EDC)

.edc.eet:
	edje_cc -v -id $(top_srcdir)/data/themes/$(THEME)/img -fd $(top_srcdir)/data/themes/$(THEME)/fonts $< $@

preview: $(EET)
	edje $(EET)


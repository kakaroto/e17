THEME = summer
EDC = $(THEME).edc
EET = $(EDC:%.edc=%.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EET)
EXTRA_DIST = $(EDC)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EET)

all: $(EDC)

.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(THEME)/img -fd $(top_srcdir)/data/themes/$(THEME) $< $@

preview: $(EET)
	edje $(EET) 


THEME = spring
EDC = $(THEME).edc
EET = $(EDC:%.edc=%.eet)
SUFFIXES = .edc .eet

CLEANFILES = $(EET)
EXTRA_DIST = $(EDC)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EET)

all: $(EDC)

.edc.eet:
	edje_cc -v -id $(top_builddir)/data/themes/$(<:%.edc=%)/img -fd $(top_builddir)/data/themes/$(<:%.edc=%)/fonts $< $@

preview: $(EET)
	edje $(EET) eplayer


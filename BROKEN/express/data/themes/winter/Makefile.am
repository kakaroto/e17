THEME = express
EDC = $(THEME).edc
EDJ = $(EDC:%.edc=%.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EDJ)
EXTRA_DIST = $(EDC)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJ)

all: $(EDC)

.edc.edj:
	edje_cc -v -id $(srcdir)/img -fd $(srcdir)/fonts $< $@

preview: $(EDJ)
	edje $(EDJ)


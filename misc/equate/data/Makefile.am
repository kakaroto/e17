
EDCS = equate.edc
EETS = $(EDCS:%.edc=%.eet)
SUFFIXES = .edc .eet

CLEANFILES = $(EETS)
EXTRA_DIST = $(EDCS) README

themedir = $(pkgdatadir)/themes
theme_DATA = $(EETS)

all: $(EDCS)

.edc.eet:
	edje_cc -v -id images $< $@

preview: equate.eet
	edje equate.eet equate
	

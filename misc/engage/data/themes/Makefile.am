THEMES = gentoo
EDCS = $(THEMES:%=%.edc)
EETS = $(THEMES:%=%.eet)
SUFFIXES = .edc .eet

CLEANFILES = $(EETS)
EXTRA_DIST = $(EDCS)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EETS)

all: $(EDCS)

.edc.eet:
	edje_cc -v -id $(THEMES)/images $< $@



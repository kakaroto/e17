THEMES = gentoo none module
EDCS = $(THEMES:%=%.edc)
EDJS = $(THEMES:%=%.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EDJS)
EXTRA_DIST = $(EDCS)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJS)

all: $(EDCS)

.edc.edj:
	edje_cc -v -id $*/images -fd $*/fonts $< $@


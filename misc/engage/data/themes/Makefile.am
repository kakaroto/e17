THEMES = gentoo none module
EDCS = $(THEMES:%=%.edc)
EDJS = $(THEMES:%=%.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EDJS)
EXTRA_DIST = $(EDCS)

themedir = $(datadir)/themes
theme_DATA = $(EDJS)

all: $(EDCS)

.edc.edj:
	edje_cc -v -id $(<:%.edc=%)/images -fd $(<:%.edc=%)/fonts $< $@




EDCS = equate.edc text.edc
EDJS = $(EDCS:%.edc=%.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EDJS)
EXTRA_DIST = $(EDCS) README

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJS)

all: $(EDCS)

.edc.edj:
	edje_cc -v -id images $< $@

preview: equate.edj
	edje equate.edj equate
	

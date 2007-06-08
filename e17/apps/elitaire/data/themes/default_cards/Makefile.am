SUBDIRS = images
THEME = default_cards

EDC = $(THEME).edc
EDJ = $(EDC:%.edc=%.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EDJ)
EXTRA_DIST = $(EDC) AUTHORS COPYING

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJ)

all: $(EDC)

.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(THEME)/images  $< $@


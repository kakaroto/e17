SUBDIRS = images fonts
THEME = e17

EDC = $(THEME).edc
EDJ = $(EDC:%.edc=%.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EDJ)
EXTRA_DIST = $(EDC) element.grp main.grp e17_macros.edc

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJ)

all: $(EDC)

.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(THEME)/images -fd $(top_srcdir)/data/themes/$(THEME)/fonts $< $@


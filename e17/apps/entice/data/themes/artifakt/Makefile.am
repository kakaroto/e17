SUBDIRS = parts programs images fonts

EXTRA_DIST= \
artifakt.edc \
images.edc \
fonts.edc \
macros/macros.edc \
embryo/globals.edc

THEME = artifakt
EDC = $(THEME).edc
EDJ = $(EDC:%.edc=%.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EDJ)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJ)

.PHONY: all
all: $(EDC)

.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(THEME)/images -fd $(top_srcdir)/data/themes/$(THEME) $< $@

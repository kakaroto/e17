SUBDIRS = parts programs images fonts

EXTRA_DIST= \
artifakt.edc \
images.edc \
fonts.edc \
macros/macros.edc \
embryo/globals.edc

THEME = artifakt
EDC = $(THEME).edc
EET = $(EDC:%.edc=%.eet)
SUFFIXES = .edc .eet

themedir = $(pkgdatadir)/themes
theme_DATA = $(EET)

all: $(EDC)

.edc.eet:
	edje_cc -v -id $(top_srcdir)/data/themes/$(THEME)/images -fd $(top_srcdir)/data/themes/$(THEME) $< $@

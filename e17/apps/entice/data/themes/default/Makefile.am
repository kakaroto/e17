SUBDIRS = parts programs

EXTRA_DIST= \
default.edc \
images.edc \
fonts.edc \
macros/macros.edc \
embryo/globals.edc

THEME = default
EDC = $(THEME).edc
EET = $(EDC:%.edc=%.eet)
SUFFIXES = .edc .eet

themedir = $(pkgdatadir)/themes
theme_DATA = $(EET)

all: $(EDC)

.edc.eet:
	edje_cc -v -id $(top_srcdir)/data/images -fd $(top_srcdir)/data/fonts $< $@

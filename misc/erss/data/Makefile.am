SUBDIRS = fonts config

EDCS =  default.edc white.edc darkgray.edc

EETS = $(EDCS:%.edc=%.eet)

EXTRA_DIST = $(EDCS)
CLEANFILES = $(EETS)

themedir = $(pkgdatadir)
theme_DATA = $(EETS)

all: $(EETS)

SUFFIXES = .edc .eet
.edc.eet:
	edje_cc -v $< $@

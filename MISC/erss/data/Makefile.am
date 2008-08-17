SUBDIRS = fonts config

EDCS =  default.edc white.edc darkgray.edc

EDJS = $(EDCS:%.edc=%.edj)

EXTRA_DIST = $(EDCS)
CLEANFILES = $(EDJS)

themedir = $(pkgdatadir)
theme_DATA = $(EDJS)

all: $(EDJS)

SUFFIXES = .edc .edj
.edc.edj:
	edje_cc -v $< $@

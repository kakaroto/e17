SUBDIRS = ewler
MAINTAINERCLEANFILES = Makefile.in

EDCS =  ewler.edc

EETS = $(EDCS:%.edc=%.eet)

EXTRA_DIST = $(EDCS)
CLEANFILES = $(EETS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EETS)

all: $(EETS)

SUFFIXES = .edc .eet
.edc.eet:
	edje_cc -v -id $(@:%.eet=%)/bits/images $< $@

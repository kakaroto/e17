MAINTAINERCLEANFILES = Makefile.in

EDCS =  elapse.edc

EETS = $(EDCS:%.edc=%.eet)

EXTRA_DIST = $(EDCS)
CLEANFILES = $(EETS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EETS)

all: $(EETS)

SUFFIXES = .edc .eet
.edc.eet:
	edje_cc -v -id $(@:%.eet=%)/images $< $@

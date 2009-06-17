MAINTAINERCLEANFILES = Makefile.in

EDCS =  elapse.edc

EETS = $(EDCS:%.edc=%.edj)

EXTRA_DIST = $(EDCS)
CLEANFILES = $(EETS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EETS)

all: $(EETS)

SUFFIXES = .edc .edj
.edc.edj:
	edje_cc -v -id $(@:%.edj=%)/images $< $@

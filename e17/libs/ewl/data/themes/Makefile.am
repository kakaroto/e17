MAINTAINERCLEANFILES = Makefile.in

EDCS =  default.edc skeleton.edc

EETS = $(EDCS:%.edc=%.eet)

EXTRA_DIST = $(EDCS)
CLEANFILES = $(EETS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EETS)

all: $(EETS)

SUFFIXES = .edc .eet
.edc.eet:
	edje_cc -v -id $(top_builddir)/data/themes/$(<:%.edc=%)/bits/images `basename $<` $@

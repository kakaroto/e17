MAINTAINERCLEANFILES = Makefile.in

EDCS =  bg.edc

EETS = $(EDCS:%.edc=%.eet)

IMAGES = e-logo.png lilbottom.png

themedir = $(pkgdatadir)/images
theme_DATA = $(IMAGES) $(EETS)

EXTRA_DIST = $(IMAGES) $(EDCS)

CLEANFILES = $(EETS)

all: $(EETS)

SUFFIXES = .edc .eet
.edc.eet:
	edje_cc -v -id $(top_builddir)/data/images `basename $<` $@

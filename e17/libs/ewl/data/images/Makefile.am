MAINTAINERCLEANFILES = Makefile.in

EDCS =  bg.edc

EETS = $(EDCS:%.edc=%.eet)

IMAGES = arrow_up.png e-logo.png lilbottom.png banner-top.png elicit.png \
	 elicit1.png elicit2.png elicit3.png entrance.png entrance1.png \
	 entrance2.png evidence.png evidence1.png evidence2.png evidence3.png \
	 evidence4.png entrance3.png home.png lilgrad.png \
	 entice.png entice1.png entice2.png entice3.png

themedir = $(pkgdatadir)/images
theme_DATA = $(IMAGES) $(EETS)

EXTRA_DIST = $(IMAGES) $(EDCS)

CLEANFILES = $(EETS)

all: $(EETS)

SUFFIXES = .edc .eet
.edc.eet:
#	edje_cc -v -id $(top_builddir)/data/images `basename $<` $@
	(cd $(top_srcdir)/data/images ; edje_cc -v -id . $(notdir $<) $(top_builddir)/data/images/$(notdir $@))

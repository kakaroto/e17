MAINTAINERCLEANFILES = Makefile.in

EDCS =  bg.edc

EDJS = $(EDCS:%.edc=%.edj)

IMAGES = arrow_up.png e-logo.png lilbottom.png banner-top.png elicit.png \
	 elicit1.png elicit2.png elicit3.png entrance.png entrance1.png \
	 entrance2.png evidence.png evidence1.png evidence2.png evidence3.png \
	 evidence4.png entrance3.png home.png lilgrad.png \
	 entice.png entice1.png entice2.png entice3.png

themedir = $(pkgdatadir)/images
theme_DATA = $(IMAGES) $(EDJS)

EXTRA_DIST = $(IMAGES) $(EDCS)

CLEANFILES = $(EDJS)

all: $(EDJS)

SUFFIXES = .edc .edj
.edc.edj:
#	edje_cc -v -id $(top_builddir)/data/images `basename $<` $@
	(cd $(top_srcdir)/data/images ; edje_cc -v -id . $(notdir $<) $(top_builddir)/data/images/$(notdir $@))

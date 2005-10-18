MAINTAINERCLEANFILES = Makefile.in

EDCS =  bg.edc

EDJS = $(EDCS:%.edc=%.edj)

IMAGES = Draw.png End.png NewBCard.png Open.png Package.png World.png \
	banner-top.png e-logo.png elicit.png elicit1.png elicit2.png \
	elicit3.png entice.png entice1.png entice2.png entice3.png \
	entrance.png entrance1.png entrance2.png entrance3.png evidence.png \
	evidence1.png evidence2.png evidence3.png evidence4.png \
	lilbottom.png lilgrad.png

themedir = $(pkgdatadir)/images
theme_DATA = $(IMAGES) $(EDJS)

EXTRA_DIST = $(IMAGES) $(EDCS)

CLEANFILES = $(EDJS)

all: $(EDJS)

SUFFIXES = .edc .edj
.edc.edj:
#	edje_cc -v -id $(top_builddir)/data/images `basename $<` $@
	(cd $(top_srcdir)/data/images ; edje_cc -v -id . $(notdir $<) $(top_builddir)/data/images/$(notdir $@))

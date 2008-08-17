MAINTAINERCLEANFILES = Makefile.in

EDCS =  bg.edc

EDJS = $(EDCS:%.edc=%.edj)

IMAGES = Draw.png End.png NewBCard.png Open.png Package.png World.png \
	banner-top.png e-logo.png elicit.png elicit1.png elicit2.png \
	elicit3.png entice.png entice1.png entice2.png entice3.png \
	entrance.png entrance1.png entrance2.png entrance3.png evidence.png \
	evidence1.png evidence2.png evidence3.png evidence4.png \
	lilbottom.png lilgrad.png e-logo-0.png e-logo-1.png e-logo-2.png \
	e-logo-3.png e-logo-4.png e-logo-5.png e-logo-6.png e-logo-7.png \
	e-logo-8.png e-logo-9.png e-logo-10.png e-logo-11.png e-logo-12.png \
	e-logo-13.png e-logo-14.png

themedir = $(pkgdatadir)/images
theme_DATA = $(IMAGES) $(EDJS)

EXTRA_DIST = $(IMAGES) $(EDCS)

CLEANFILES = $(EDJS)

all: $(EDJS)

SUFFIXES = .edc .edj
.edc.edj:
	edje_cc -v -id $(srcdir) $< $@

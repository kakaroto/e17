SUBDIRS = fonts images

EDCS = eplayer.edc
EETS = $(EDCS:%.edc=%.eet)

CLEANFILES = $(EETS)
EXTRA_DIST = $(EDCS) README

themedir = $(pkgdatadir)/themes
theme_DATA = $(EETS)

all: $(EDCS)

.edc.eet:
	edje_cc -v -id images $< $@

preview: eplayer.eet
	edje eplayer.eet eplayer
	




  ####   #    #  #####   #####   #       ######   #####  ######   ####   #    #
 #    #  #    #  #    #  #    #  #       #          #    #       #    #  #    #
 #       #    #  #    #  #    #  #       #####      #    #####   #       ######
 #       #    #  #    #  #    #  #       #          #    #       #       #    #
 #    #  #    #  #    #  #    #  #       #          #    #       #    #  #    #
  ####    ####   #####   #####   ######  ######     #    ######   ####   #    #


MAINTAINERCLEANFILES = Makefile.in

EDCS =	image_bg.edc

EDJS = $(EDCS:%.edc=%.edj)

FILES = add.png \
	dialog-close.png \
        emblem-favorite.png \
	emblem-photos.png \
	exit.png \
	folder.png \
	go-up.png \
	go-home.png \
	go-down.png \
	go-next.png \
	image.png \
	preferences-system.png \
	redo.png \
	remove.png \
	search.png \
        stock_save.png \
	undo.png \
	x-office-presentation.png
imagedir = $(pkgdatadir)/images
edjedir = $(pkgdatadir)/images
edje_DATA = $(EDJS)
all: $(EDJS)
image_bg.edj: ${EDCS}
SUFFIXES = .edc .edj
.edc.edj:
	edje_cc image_bg.edc image_bg.edj
image_DATA = $(FILES)
EXTRA_DIST = $(FILES)

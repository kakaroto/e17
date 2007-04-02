SUBDIRS = parts programs

EXTRA_DIST= \
default.edc \
images.edc \
macros/exifpanel.edc macros/slider.edc \
embryo/globals.edc \
hashes/exif.edc \
parts/bg.edc \
parts/buttons.edc \
parts/delete_dialog.edc \
parts/enticeimage.edc \
parts/enticeimagepreview.edc \
parts/enticeimagescroller.edc \
parts/enticethumb_group.edc \
parts/exifpanel.edc \
parts/exifpanel_itemlist.edc \
parts/panel.edc \
parts/status.edc \
parts/tooltip.edc

CLEANFILES = $(EDJ)

THEME = default
EDC = $(THEME).edc
EDJ = $(EDC:%.edc=%.edj)
SUFFIXES = .edc .edj

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJ)

.PHONY: all
all: $(EDC)

.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/images -fd $(top_srcdir)/data/fonts $< $@

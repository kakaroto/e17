SUBDIRS = images

THEME = default
EDC = $(THEME).edc
EET = $(EDC:%.edc=%.eet)
SUFFIXES = .edc .eet

CLEANFILES = $(EET)
EXTRA_DIST = $(EDC) default_images.edc default_parts.edc \
             default_playlist_parts.edc default_playlist_progs.edc \
             default_progs.edc default_small_parts.edc default_small_progs.edc

themedir = $(pkgdatadir)/themes
theme_DATA = $(EET)

all: $(EDC)

.edc.eet:
	edje_cc -v -id images $< $@

preview: $(EET)
	edje $(EET) eplayer


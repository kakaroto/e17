SUBDIRS = images

THEME = streamline
EDC = $(THEME).edc
EET = $(EDC:%.edc=%.eet)
SUFFIXES = .edc .eet

CLEANFILES = $(EET)
EXTRA_DIST = $(EDC) streamline_images.edc streamline_parts.edc  \
             streamline_playlist_parts.edc                      \
             streamline_playlist_progs.edc streamline_progs.edc \
             streamline_small_parts.edc streamline_small_progs.edc

themedir = $(pkgdatadir)/themes
theme_DATA = $(EET)

all: $(EDC)

.edc.eet:
	edje_cc -v -id images $< $@

preview: $(EET)
	edje $(EET) eplayer


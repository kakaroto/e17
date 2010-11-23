MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/themes -fd $(top_srcdir)/data/themes -id $(top_srcdir)/data/themes/images

filesdir = $(datadir)/$(PACKAGE)
files_DATA = default.edj

images = \
images/base.png \
images/bottom_bg.png \
images/main.png \
images/menu_separation.png \
images/rating_stars_highlight.png \
images/rating_stars.png \
images/scrollbar-bg.png \
images/scrollbar-knob.png \
images/sound_max.png \
images/sound_min.png \
images/sound_mute.png \
images/volume_bg.png

fonts =

EXTRA_DIST = default.edc $(images) $(fonts)

default.edj: Makefile $(images) $(fonts) default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

clean-local:
	rm -f default.edj

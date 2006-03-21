MAINTAINERCLEANFILES=Makefile.in
EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
	     -id images \
	     -fd fonts 

filesdir = @E_MODULES@/$(PACKAGE)

files_DATA = volume_icon.png \
	     volume.edj

EXTRA_DIST = ./default.edc \
	     ./groups/mixer_entry.edc \
	     ./groups/volume_bar.edc \
	     ./fonts/Vera.ttf \
	     ./images/volume_bar_drag_down.png \
	     ./images/volume_bar_drag_mute.png \
	     ./images/handle_thumb.png \
	     ./images/handle_thumb_mute.png \
	     ./images/volume_bar_drag.png \
	     ./images/volume_bar_horiz.png \
	     ./images/volume_bar_vert.png \
	     ./images/bg.png \
	     ./images/over.png \
	     ./volume_icon.png


volume.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default/default.edc \
	$(top_builddir)/data/themes/default/volume.edj


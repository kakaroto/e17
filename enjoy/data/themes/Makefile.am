MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/themes -fd $(top_srcdir)/data/themes -id $(top_srcdir)/data/themes/images

filesdir = $(datadir)/$(PACKAGE)
files_DATA = default.edj

images = \
images/back_bg.png \
images/back-button.png \
images/bottom_bg.png \
images/bt_list_normal.png \
images/bt_list_selected.png \
images/bt_more_normal.png \
images/bt_more_selected.png \
images/bt_next_normal.png \
images/bt_next_selected.png \
images/bt_nowplaying_normal.png \
images/bt_nowplaying_selected.png \
images/bt_pause_normal.png \
images/bt_pause_selected.png \
images/bt_play_normal.png \
images/bt_play_selected.png \
images/bt_prev_normal.png \
images/bt_prev_selected.png \
images/list-highlight.png \
images/no-cover.png \
images/scrollbar-bg.png \
images/scrollbar-knob.png \
images/top_bg.png

fonts =

EXTRA_DIST = default.edc $(images) $(fonts)

default.edj: Makefile $(images) $(fonts) default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

clean-local:
	rm -f default.edj

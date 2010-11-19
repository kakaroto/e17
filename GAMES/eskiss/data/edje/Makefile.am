MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/edje -fd $(top_srcdir)/data/edje

filesdir = $(datadir)/eskiss/theme
files_DATA = default.edj

EXTRA_DIST = \
default.edc \
background_02.png \
ball_01.png \
ball_02.png \
ball_03.png \
bt_close.png \
cloud1.png \
cloud2.png \
editor_bg.png \
editor_bt_background.png \
editor_bt_color.png \
editor_bt_glow.png \
editor_bt_menu.png \
editor_bt_move.png \
editor_bt_open.png \
editor_bt_pencil.png \
editor_bt_play.png \
editor_bt.png \
editor_bt_previous.png \
editor_bt_save_new.png \
editor_bt_save.png \
editor_bt_static.png \
editor_bt_stop.png \
game_level_next.png \
goal_01.png \
goal_02.png \
goal_03.png \
load_anim_01.png \
load_anim_02.png \
load_anim_03.png \
load_anim_04.png \
load_anim_05.png \
load_anim_06.png \
load_anim_07.png \
menu_item.png \
pin.png \
play_menu.png \
title.png \
tree.png \
calaos_content-bold.ttf \
calaos_content-oblique.ttf \
calaos_content.ttf

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/edje/default.edc \
	$(top_builddir)/data/edje/default.edj

clean-local:
	rm -f *.edj

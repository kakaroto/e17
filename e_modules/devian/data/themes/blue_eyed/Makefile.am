EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
	-id images \
	-fd fonts

EXTRA_DIST =    ./blue_eyed.edc \
		./readme.edc \
		./fonts/VeraBd.ttf \
		./fonts/Vera.ttf \
		./images/border.png \
		./images/button_link_open.png \
		./images/button_pause_clicked.png \
		./images/button_pause.png \
		./images/button_refresh_clicked.png \
		./images/button_refresh.png \
		./images/button_set_bg_clicked.png \
		./images/button_set_bg.png \
		./images/hbar.png \
		./images/infos_border.png \
		./images/loading_state_bg.png \
		./images/loading_state.png \
		./images/logo.png \
		./images/menu_title2.png \
		./images/popup_background.png \
		./images/popup_close.png \
		./images/popup_disable_clicked.png \
		./images/popup_disable.png \
		./images/rss_background.png \
		./images/rss_background_selected.png

filesdir = $(datadir)/themes/

files_DATA = blue_eyed.edj

blue_eyed.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/blue_eyed/blue_eyed.edc \
	$(top_builddir)/data/themes/blue_eyed/blue_eyed.edj

clean:
	rm blue_eyed.edj
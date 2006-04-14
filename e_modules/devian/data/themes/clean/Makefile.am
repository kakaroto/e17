EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
	-id images \
	-fd fonts

EXTRA_DIST =    ./clean.edc \
		./clean_popup/clean_popup.edc \
		./clean_popup/clean_popup_parts.edc \
		./clean_popup/clean_popup_programs.edc \
		./clean_rss/clean_rss.edc \
		./clean_rss/clean_rss_parts.edc \
		./clean_rss/clean_rss_programs.edc \
		./clean_box/clean_box.edc \
		./clean_box/clean_box_parts.edc \
		./clean_box/clean_box_programs.edc \
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
		./images/infos_border.png \
		./images/loading_state_bg.png \
		./images/loading_state.png \
		./images/logo.png \
		./images/popup_background.png \
		./images/popup_close.png \
		./images/popup_disable_clicked.png \
		./images/popup_disable.png \
		./images/rss_background.png \
		./images/rss_background_selected.png

filesdir = $(datadir)/themes/

files_DATA = clean.edj

clean.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/clean/clean.edc \
	$(top_builddir)/data/themes/clean/clean.edj

clean:
	rm -f clean.edj

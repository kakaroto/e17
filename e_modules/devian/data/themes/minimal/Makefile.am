EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
	-id images \
	-fd fonts

EXTRA_DIST =    ./minimal.edc \
		./minimal_popup/minimal_popup.edc \
		./minimal_popup/minimal_popup_parts.edc \
		./minimal_popup/minimal_popup_programs.edc \
		./minimal_rss/minimal_rss.edc \
		./minimal_rss/minimal_rss_parts.edc \
		./minimal_rss/minimal_rss_programs.edc \
		./minimal_box/minimal_box.edc \
		./minimal_box/minimal_box_parts.edc \
		./minimal_box/minimal_box_programs.edc \
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

files_DATA = minimal.edj

minimal.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/minimal/minimal.edc \
	$(top_builddir)/data/themes/minimal/minimal.edj

clean:
	rm minimal.edj
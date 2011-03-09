
EDJE_FLAGS = -v \
	     -id $(top_srcdir)/data/themes/default/images

filesdir = $(datadir)
files_DATA = default.edj

IMGS=images/module_icon.png  \
	images/sel_bg.png \
	images/music.png \
	images/video.png \
	images/photo.png \
	images/apps.png \
	images/web.png \
	images/weather.png \
	images/settings.png \
	images/menu_bg.png \
	images/bg.png \
	images/bg_tile.png \
	images/windows.png \
	images/inwin_bg.png \
	images/separator.png \
	images/scrollbar-knob.png \
	images/scrollbar-bg.png \
	images/app_text_bg.png \
	images/desk_unselected.png \
	images/desk_selected.png 

EXTRA_DIST = default.edc \
	elm.edc \
	$(IMGS)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf default.edj *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)


EDJE_FLAGS = -v \
	     -id $(top_srcdir)/data/themes/default/images

filesdir = $(datadir)
files_DATA = default.edj

IMGS= \
	images/application-default-icon-128.png \
	images/application-default-icon-16.png \
	images/application-default-icon-24.png \
	images/application-default-icon-48.png \
	images/application-default-icon-64.png \
	images/application-default-icon-96.png \
	images/apps.png \
	images/app_text_bg.png \
	images/badge_delete.png \
	images/badge-delete.svg \
	images/bg.png \
	images/bg_tile.png \
	images/desk_selected.png \
	images/desk_unselected.png \
	images/icon_application.png \
	images/icon_configuration.png \
	images/icon_widgets.png \
	images/inwin_bg.png \
	images/mainmenu_bg_bottom.png \
	images/mainmenu_bg_reflect.png \
	images/mainmenu_bg_top.png \
	images/menu_bg.png \
	images/module_icon.png \
	images/music.png \
	images/photo.png \
	images/scrollbar-bg.png \
	images/scrollbar-knob.png \
	images/sel_bg.png \
	images/separator.png \
	images/settings.png \
	images/video.png \
	images/weather.png \
	images/web.png \
	images/windows.png

EXTRA_DIST = default.edc \
	elm.edc \
	$(IMGS)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf default.edj *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)

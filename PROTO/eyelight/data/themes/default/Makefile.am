MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = transition fonts

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/default \
-fd $(top_srcdir)/data/themes/default

theme.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default/theme.edc \
	$(top_builddir)/data/themes/default/theme.edj

icondir = $(pkgdatadir)/themes/default
icon_DATA = theme.edj

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)/themes/default

EXTRA_DIST = \
background_bottom.png \
background.png \
background_top.png \
black.png \
border_glow_2.png \
border_glow.png \
cadre_glow_2.png \
cadre_glow.png \
cadre.png \
expose.edc \
expose_next.png \
expose_previous.png \
expose_selection.png \
expose_shadow.png \
glow.png \
gotoslide.edc \
item_00.png \
item_01.png \
item_02.png \
item_03.png \
layout.edc \
pause_glow.png \
pause.png \
pause_select.png \
play_glow.png \
play.png \
play_select.png \
slider_back.png \
slider_back_select.png \
slider_glow.png \
slider.png \
slider_select.png \
slideshow.edc \
stop_glow.png \
stop.png \
stop_select.png \
subtitle_background.png \
tableofcontents.edc \
tableofcontents_selection.png \
theme.edc \
title_background.png

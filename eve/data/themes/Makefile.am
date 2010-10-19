MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/themes -fd $(top_srcdir)/data/themes

filesdir = $(datadir)/$(PACKAGE)
files_DATA = default.edj

images = \
    more-list-back-button.png \
    more-list-title.png \
    bt_back_normal.png \
    bt_back_selected.png \
    bt_more_normal.png \
    bt_more_selected.png \
    bt_fav_normal.png \
    bt_fav_selected.png \
    bt_forward_normal.png \
    bt_forward_selected.png \
    bt_home_normal.png \
    bt_home_selected.png \
    bt_reload_normal.png \
    bt_reload_selected.png \
    bt_tab_normal.png \
    bt_tab_selected.png \
    bt_addtab_normal.png \
    bt_addtab_selected.png \
    bt_stop_normal.png \
    bt_stop_selected.png \
    panel-top-triangle.png \
    scrollbar-bg.png \
    scrollbar-knob.png \
    shadow-bottom.png \
    tab-bg.png \
    tab-bg-selected.png \
    tab-close.png \
    tab-header.png \
    toolbar-background-bottom.png \
    toolbar-background-top.png \
    url-entry.png \
    list-highlight.png \
    cur_box.png \
    cur_hi.png \
    cur_shine.png \
    cur_glow.png \
    cur_shad.png \
    progress-bar-background.png \
    arrow_right.png \
    toggle_bg.png  \
    toggle_off.png \
    toggle_on.png \
    check.png \
    index.png \
    index_selected.png

edcdeps = elm-gengrid.edc \
    elm-notify.edc \
    elm-scroller.edc \
    elm-entry.edc \
    elm-list.edc \
    elm-pager.edc \
    elm-toggle.edc

fonts = 

EXTRA_DIST = default.edc $(images) $(fonts) $(edcdeps)

default.edj: Makefile $(images) $(fonts) default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

clean-local:
	rm -f default.edj

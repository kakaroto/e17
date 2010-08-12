MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/themes -fd $(top_srcdir)/data/themes

filesdir = $(datadir)/$(PACKAGE)
files_DATA = ewebkit.edj default.edj

images = \
    bookmark-list-back-button.png \
    bookmark-list-title.png \
    bt_back_normal.png \
    bt_back_selected.png \
    bt_bookmark_normal.png \
    bt_bookmark_selected.png \
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
    panel-top-triangle.png \
    scrollbar-bg.png \
    scrollbar-knob.png \
    shadow-bottom.png \
    tab-add.png \
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
    progress-bar-background.png

fonts = 

EXTRA_DIST = ewebkit.edj default.edc $(images) $(fonts)

default.edj: Makefile $(images) $(fonts) default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

clean-local:
	rm -f default.edj

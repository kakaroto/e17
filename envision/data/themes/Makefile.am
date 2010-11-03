MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/themes -fd $(top_srcdir)/data/themes

images = \
	bt_clear_normal.png \
	bt_clear_selected.png \
	bt_down_normal.png \
	bt_down_selected.png \
	bt_more_normal.png \
	bt_more_selected.png \
	bt_up_normal.png \
	bt_up_selected.png \
	bt_zoomin_normal.png \
	bt_zoomin_selected.png \
	bt_zoomout_normal.png \
	bt_zoomout_selected.png \
	found_page_selected.png \
	fullscreen.png \
	more-list-title.png \
	open.png \
	panel-top-triangle.png \
	search_normal.png \
	search_selected.png \
	search-entry.png \
	toolbar-background-bottom.png \
	toolbar-background-top.png \
	cur_box.png \
	cur_glow.png \
	cur_hi.png \
	cur_shad.png \
	cur_shine.png \
	bt_sm_base1.png \
        bt_sm_shine.png \
	found_page_selected.png \
	tab-bg.png \
	scrollbar-bg.png \
	scrollbar-knob.png
	toggle_bg.png \
	icon_arrow_left.png \
	spinner_left_selected.png \
	icon_arrow_right.png \
	spinner_right_selected.png


filesdir = $(datadir)/$(PACKAGE)
files_DATA = default.edj

edcdeps = elm-genlist.edc \
	elm-scroller.edc \
	elm-entry.edc \
	elm-gengrid.edc \
	elm-spinner.edc

fonts =

EXTRA_DIST = default.edc $(images) $(fonts) $(edcdeps)

default.edj: Makefile $(images) $(fonts) default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

clean-local:
	rm -f default.edj

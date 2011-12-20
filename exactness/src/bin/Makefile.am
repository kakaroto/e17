
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DLOCALE_DIR=\"$(datadir)/locale\" \
@EFL_CFLAGS@

bin_PROGRAMS = exactness_raw

exactness_raw_SOURCES = \
		    tsuite.c \
		    ../lib/tsuite_file_data.c \
		    test_3d.c \
		    test_actionslider.c \
		    test_panel.c \
		    test_anchorblock.c \
		    test_panes.c \
		    test_anchorview.c \
		    test_focus.c \
		    test_bg.c \
		    test_gengrid.c \
		    test_box.c \
		    test_genlist.c \
		    test_progressbar.c \
		    test_bubble.c \
		    test_radio.c \
		    test_button.c \
		    test_scaling.c \
		    test_calendar.c \
		    test_grid.c \
		    test_segment_control.c \
		    test_check.c \
		    test_hover.c \
		    test_separator.c \
		    test_hoversel.c \
		    test_slider.c \
		    test_colorselector.c \
		    test_conform.c \
		    test_spinner.c \
		    test_ctxpopup.c \
		    test_index.c \
		    test_table.c \
		    test_inwin.c \
		    test_diskselector.c \
		    test_label.c \
		    test_toggle.c \
		    test_entry.c \
		    test_toolbar.c \
		    test_fileselector_button.c \
		    test_tooltip.c \
		    test_fileselector.c \
		    test_list.c \
		    test_fileselector_entry.c \
		    test_menu.c \
		    test_naviframe.c \
		    test_notify.c \
		    test_scroller.c \
		    test_pager.c



exactness_raw_LDADD = @EFL_LIBS@

EXTRA_DIST = \
	     tsuite.h


## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_builddir) \
-I$(top_srcdir) \
-DPACKAGE_DATA_DIR=\"$(datadir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_SYSCONF_DIR=\"$(sysconfdir)\" \
$(GCOV_CFLAGS) \
@EDJE_CFLAGS@ @ECORE_CFLAGS@ @EVAS_CFLAGS@ @EMOTION_CFLAGS@ @EPSILON_CFLAGS@ @EFREET_CFLAGS@ @EVIL_CFLAGS@

lib_LTLIBRARIES = libewl.la

EWLHEADERS = \
	Ewl.h \
	ewl_attach.h \
	ewl_base.h \
	ewl_border.h \
	ewl_box.h \
	ewl_label.h \
	ewl_button.h \
	ewl_calendar.h \
	ewl_callback.h \
	ewl_cell.h \
	ewl_check.h \
	ewl_checkbutton.h \
	ewl_colordialog.h \
	ewl_colorpicker.h \
	ewl_combo.h \
	ewl_config.h \
	ewl_container.h \
	ewl_context_menu.h \
	ewl_cursor.h \
	ewl_datepicker.h \
	ewl_debug.h \
	ewl_dialog.h \
	ewl_dnd.h \
	ewl_embed.h \
	ewl_engines.h \
	ewl_entry.h \
	ewl_enums.h \
	ewl_events.h \
	ewl_expansion.h \
	ewl_floater.h \
	ewl_filelist.h \
	ewl_filelist_model.h \
	ewl_filelist_view.h \
	ewl_filepicker.h \
	ewl_filedialog.h \
	ewl_freebox.h \
	ewl_freebox_mvc.h \
	ewl_grid.h \
	ewl_histogram.h \
	ewl_icon.h \
	ewl_icon_theme.h \
	ewl_icondialog.h \
	ewl_io_manager.h \
	ewl_image.h \
	ewl_list.h \
	ewl_macros.h \
	ewl_media.h \
	ewl_menu.h \
	ewl_menubar.h \
	ewl_menu_item.h \
	ewl_misc.h \
	ewl_model.h \
	ewl_mvc.h \
	ewl_notebook.h \
	ewl_object.h \
	ewl_overlay.h \
	ewl_paned.h \
	ewl_popup.h \
	ewl_progressbar.h \
	ewl_radiobutton.h \
	ewl_range.h \
	ewl_row.h \
	ewl_scrollbar.h \
	ewl_scrollpane.h \
	ewl_seeker.h \
	ewl_separator.h \
	ewl_shadow.h \
	ewl_spacer.h \
	ewl_spectrum.h \
	ewl_spinner.h \
	ewl_statusbar.h \
	ewl_stock.h \
	ewl_text.h \
	ewl_text_context.h \
	ewl_text_fmt.h \
	ewl_text_trigger.h \
	ewl_table.h \
	ewl_tree.h \
	ewl_tree_view.h \
	ewl_tree_view_plain.h \
	ewl_tree_view_scrolled.h \
	ewl_theme.h \
	ewl_toolbar.h \
	ewl_view.h \
	ewl_widget.h \
	ewl_window.h

EWLSOURCES= \
	ewl_attach.c \
	ewl_border.c \
	ewl_box.c \
	ewl_label.c \
	ewl_button.c \
	ewl_calendar.c \
	ewl_callback.c \
	ewl_cell.c \
	ewl_check.c \
	ewl_checkbutton.c \
	ewl_colordialog.c \
	ewl_colorpicker.c \
	ewl_combo.c \
	ewl_config.c \
	ewl_container.c \
	ewl_context_menu.c \
	ewl_cursor.c \
	ewl_datepicker.c \
	ewl_dialog.c \
	ewl_dnd.c \
	ewl_embed.c \
	ewl_engines.c \
	ewl_entry.c \
	ewl_events.c \
	ewl_expansion.c \
	ewl_floater.c \
	ewl_filelist.c \
	ewl_filelist_model.c \
	ewl_filelist_view.c \
	ewl_filepicker.c \
	ewl_filedialog.c \
	ewl_freebox.c \
	ewl_freebox_mvc.c \
	ewl_grid.c \
	ewl_histogram.c \
	ewl_icon.c \
	ewl_icon_theme.c \
	ewl_icondialog.c \
	ewl_io_manager.c \
	ewl_image.c \
	ewl_list.c \
	ewl_media.c \
	ewl_menu.c \
	ewl_menubar.c \
	ewl_menu_item.c \
	ewl_misc.c \
	ewl_model.c \
	ewl_mvc.c \
	ewl_notebook.c \
	ewl_object.c \
	ewl_overlay.c \
	ewl_paned.c \
	ewl_popup.c \
	ewl_progressbar.c \
	ewl_radiobutton.c \
	ewl_range.c \
	ewl_row.c \
	ewl_scrollbar.c \
	ewl_scrollpane.c \
	ewl_seeker.c \
	ewl_separator.c \
	ewl_shadow.c \
	ewl_spacer.c \
	ewl_spectrum.c \
	ewl_spinner.c \
	ewl_statusbar.c \
	ewl_stock.c \
	ewl_table.c \
	ewl_text.c \
	ewl_text_context.c \
	ewl_text_fmt.c \
	ewl_text_trigger.c \
	ewl_theme.c \
	ewl_tree.c \
	ewl_tree_view.c \
	ewl_tree_view_plain.c \
	ewl_tree_view_scrolled.c \
	ewl_toolbar.c \
	ewl_view.c \
	ewl_widget.c \
	ewl_window.c \
	ewl_private.h \
	$(EWLHEADERS)

libewl_la_SOURCES = \
	$(EWLSOURCES)

installed_headersdir = $(prefix)/include/ewl
installed_headers_DATA = $(EWLHEADERS)

libewl_la_LIBADD = $(GCOV_LIBS) @EMOTION_LIBS@ @EPSILON_LIBS@ @EFREET_LIBS@ @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EVIL_LIBS@ @fnmatch_libs@ -lm
libewl_la_LDFLAGS = @lt_no_undefined@ @lt_enable_auto_import@ -version-info @INTERFACE_VERSION@
libewl_la_LIBTOOLFLAGS =

clean-local:
	@rm -f *.gcno

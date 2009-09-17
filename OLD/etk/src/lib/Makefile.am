MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# Gettext
datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

INCLUDES	= \
-I. -I../.. -I$(top_srcdir)/src/lib -Wall -g \
@EVAS_CFLAGS@ @ECORE_CFLAGS@ @EDJE_CFLAGS@

lib_LTLIBRARIES = libetk.la

EXTRA_DIST = 

ETKHEADERS = \
Etk.h \
etk_alignment.h \
etk_argument.h \
etk_bin.h \
etk_box.h \
etk_button.h \
etk_cache.h \
etk_canvas.h \
etk_check_button.h \
etk_colorpicker.h \
etk_combobox.h \
etk_combobox_entry.h \
etk_config.h \
etk_container.h \
etk_dialog.h \
etk_dnd.h \
etk_drag.h \
etk_editable.h \
etk_embed.h \
etk_engine.h \
etk_entry.h \
etk_evas_object.h \
etk_event.h \
etk_filechooser_widget.h \
etk_frame.h \
etk_iconbox.h \
etk_image.h \
etk_label.h \
etk_main.h \
etk_marshallers.h \
etk_mdi_area.h \
etk_mdi_window.h \
etk_menu.h \
etk_menu_bar.h \
etk_menu_item.h \
etk_menu_shell.h \
etk_message_dialog.h \
etk_notebook.h \
etk_object.h \
etk_paned.h \
etk_popup_window.h \
etk_progress_bar.h \
etk_property.h \
etk_radio_button.h \
etk_range.h \
etk_scrollbar.h \
etk_scrolled_view.h \
etk_selection.h \
etk_separator.h \
etk_shadow.h \
etk_signal.h \
etk_signal_callback.h \
etk_slider.h \
etk_spinner.h \
etk_statusbar.h \
etk_stock.h \
etk_string.h \
etk_table.h \
etk_text_view.h \
etk_text_view2.h \
etk_text_view3.h \
etk_textblock.h \
etk_textblock2.h \
etk_theme.h \
etk_toggle_button.h \
etk_tool_button.h \
etk_toolbar.h \
etk_tooltips.h \
etk_toplevel.h \
etk_tree.h \
etk_tree_model.h \
etk_type.h \
etk_types.h \
etk_utils.h \
etk_viewport.h \
etk_widget.h \
etk_window.h

libetk_la_SOURCES = \
etk_alignment.c \
etk_argument.c \
etk_bin.c \
etk_box.c \
etk_button.c \
etk_cache.c \
etk_canvas.c \
etk_check_button.c \
etk_colorpicker.c \
etk_combobox.c \
etk_combobox_entry.c \
etk_config.c \
etk_container.c \
etk_dialog.c \
etk_dnd.c \
etk_drag.c \
etk_editable.c \
etk_embed.c \
etk_engine.c \
etk_entry.c \
etk_evas_object.c \
etk_event.c \
etk_filechooser_widget.c \
etk_frame.c \
etk_iconbox.c \
etk_image.c \
etk_label.c \
etk_main.c \
etk_marshallers.c \
etk_mdi_area.c \
etk_mdi_window.c \
etk_menu.c \
etk_menu_bar.c \
etk_menu_item.c \
etk_menu_shell.c \
etk_message_dialog.c \
etk_notebook.c \
etk_object.c \
etk_paned.c \
etk_popup_window.c \
etk_progress_bar.c \
etk_property.c \
etk_radio_button.c \
etk_range.c \
etk_scrollbar.c \
etk_scrolled_view.c \
etk_selection.c \
etk_separator.c \
etk_shadow.c \
etk_signal.c \
etk_signal_callback.c \
etk_slider.c \
etk_spinner.c \
etk_statusbar.c \
etk_stock.c \
etk_string.c \
etk_table.c \
etk_text_view.c \
etk_text_view2.c \
etk_text_view3.c \
etk_textblock.c \
etk_textblock2.c \
etk_theme.c \
etk_toggle_button.c \
etk_tool_button.c \
etk_toolbar.c \
etk_tooltips.c \
etk_toplevel.c \
etk_tree.c \
etk_tree_model.c \
etk_type.c \
etk_utils.c \
etk_viewport.c \
etk_widget.c \
etk_window.c \
$(ETKHEADERS)

installed_headersdir = $(prefix)/include/etk
installed_headers_DATA = $(ETKHEADERS)

libetk_la_LIBADD  = @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@

libetk_la_DEPENDENCIES = 

libetk_la_LDFLAGS = -version-info 1:0:0


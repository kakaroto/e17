MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# Gettext
datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

INCLUDES	= \
-I. -I$(top_srcdir)/src/lib -Wall -g \
@EVAS_CFLAGS@ @ECORE_CFLAGS@ @EDJE_CFLAGS@

lib_LTLIBRARIES = libetk.la

EXTRA_DIST = 

ETKHEADERS = \
Etk.h etk_types.h etk_main.h etk_utils.h \
etk_type.h etk_object.h etk_property.h \
etk_signal.h etk_signal_callback.h etk_marshallers.h \
etk_theme.h etk_cache.h \
etk_widget.h etk_toplevel_widget.h \
etk_container.h etk_bin.h etk_table.h etk_alignment.h \
etk_box.h etk_hbox.h etk_vbox.h \
etk_paned.h etk_frame.h \
etk_scrolled_view.h etk_viewport.h \
etk_window.h etk_dialog.h \
etk_image.h \
etk_stock.h \
etk_label.h \
etk_string.h etk_text_buffer.h etk_text_iter.h etk_editable_text_object.h \
etk_button.h etk_toggle_button.h etk_check_button.h etk_radio_button.h \
etk_entry.h etk_text_view.h \
etk_canvas.h \
etk_colorpicker.h etk_colorpicker_vertical.h etk_colorpicker_square.h \
etk_tree.h etk_tree_model.h \
etk_range.h etk_slider.h etk_scrollbar.h \
etk_separator.h \
etk_menu_shell.h etk_menu.h etk_menu_bar.h etk_menu_item.h \
etk_statusbar.h \
etk_filechooser_widget.h \
etk_notebook.h \
etk_progress_bar.h \
etk_spin_button.h \
etk_dnd.h \
etk_clipboard.h \
etk_selection.h \
etk_drag.h

libetk_la_SOURCES = \
etk_main.c etk_utils.c \
etk_type.c etk_object.c etk_property.c \
etk_signal.c etk_signal_callback.c etk_marshallers.c \
etk_theme.c etk_cache.c \
etk_widget.c etk_toplevel_widget.c \
etk_container.c etk_bin.c etk_table.c etk_alignment.c \
etk_box.c etk_hbox.c etk_vbox.c \
etk_paned.c etk_frame.c \
etk_scrolled_view.c etk_viewport.c \
etk_window.c etk_dialog.c \
etk_image.c \
etk_stock.c \
etk_label.c \
etk_string.c etk_text_buffer.c etk_text_iter.c etk_editable_text_object.c \
etk_button.c etk_toggle_button.c etk_check_button.c etk_radio_button.c \
etk_entry.c etk_text_view.c \
etk_canvas.c \
etk_colorpicker.c etk_colorpicker_vertical.c etk_colorpicker_square.c \
etk_tree.c etk_tree_model.c \
etk_range.c etk_slider.c etk_scrollbar.c \
etk_separator.c \
etk_menu_shell.c etk_menu.c etk_menu_bar.c etk_menu_item.c \
etk_statusbar.c \
etk_filechooser_widget.c \
etk_notebook.c \
etk_progress_bar.c \
etk_spin_button.c \
etk_dnd.c \
etk_clipboard.c \
etk_selection.c \
etk_drag.c \
$(ETKHEADERS)

installed_headersdir = $(prefix)/include/etk
installed_headers_DATA = $(ETKHEADERS)

libetk_la_LIBADD  = @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@

libetk_la_DEPENDENCIES = 

libetk_la_LDFLAGS = -version-info 1:0:0

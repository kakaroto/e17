MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# Gettext
datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST =

INCLUDES	= \
-I. -I../.. -I$(top_srcdir)/src/lib -Wall -g \
@EVAS_CFLAGS@ @ECORE_CFLAGS@ @EDJE_CFLAGS@

bin_PROGRAMS = etk_test

etk_test_DEPENDENCIES = $(top_builddir)/src/lib/libetk.la

etk_test_SOURCES = \
etk_test.h \
etk_test.c \
etk_button_test.c \
etk_entry_test.c \
etk_table_test.c \
etk_image_test.c \
etk_slider_test.c \
etk_canvas_test.c \
etk_colorpicker_test.c \
etk_tree_test.c \
etk_paned_test.c \
etk_scrolled_view_test.c \
etk_menu_test.c \
etk_mdi_window_test.c \
etk_filechooser_test.c \
etk_notebook_test.c \
etk_progress_bar_test.c \
etk_dnd_test.c \
etk_combobox_test.c \
etk_iconbox_test.c \
etk_text_view_test.c \
etk_embed_test.c \
etk_evas_object_test.c \
etk_shadow_test.c \
etk_stock.c

etk_test_LDADD = $(top_builddir)/src/lib/libetk.la \
		 @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@

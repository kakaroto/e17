## Process this file with automake to produce Makefile.in

INCLUDES = @ebits_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@ @ewd_cflags@ @etox_cflags@ @imlib2_cflags@

lib_LTLIBRARIES = libewl.la

installed_headersdir = $(prefix)/include/ewl

installed_headers_DATA = \
	Ewl.h \
	ewl_box.h \
	ewl_button.h \
	ewl_callback.h \
	ewl_checkbutton.h \
	ewl_config.h \
	ewl_container.h \
	ewl_cursor.h \
	ewl_debug.h \
	ewl_entry.h \
	ewl_enums.h \
	ewl_events.h \
	ewl_fx.h \
	ewl_image.h \
	ewl_list.h \
	ewl_macros.h \
	ewl_misc.h \
	ewl_object.h \
	ewl_radiobutton.h \
	ewl_seeker.h \
	ewl_spinner.h \
	ewl_text.h \
	ewl_table.h \
	ewl_theme.h \
	ewl_widget.h \
	ewl_window.h

libewl_la_SOURCES = \
	ewl_box.c \
	ewl_button.c \
	ewl_callback.c \
	ewl_checkbutton.c \
	ewl_config.c \
	ewl_container.c \
	ewl_cursor.c \
	ewl_entry.c \
	ewl_events.c \
	ewl_fx.c \
	ewl_image.c \
	ewl_list.c \
	ewl_misc.c \
	ewl_object.c \
	ewl_radiobutton.c \
	ewl_seeker.c \
	ewl_spinner.c \
	ewl_table.c \
	ewl_text.c \
	ewl_theme.c \
	ewl_widget.c \
	ewl_window.c \
	getopt.c \
	getopt1.c

libewl_la_LIBADD = @ebits_libs@ @ecore_libs@ @edb_libs@ @evas_libs@ @ewd_libs@ @etox_libs@ @imlib2_libs@ -lm
libewl_la_LDFLAGS = -version-info 0:0:0

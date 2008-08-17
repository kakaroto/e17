MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES=libentrance_widgets.la

libentrance_widgets_la_HEADERS = \
	Entrance_Widgets.h \
	ew.h \
	\
	ew_dialog.h \
	ew_notice.h \
	ew_fileselector.h \
	\
	ew_group.h \
	\
	_ew_list.h \
	ew_textlist.h \
	ew_edjelist.h \
	\
	ew_messagebox.h \
	\
	ew_preview.h\
	\
	ew_image.h \
	ew_label.h \
	ew_entry.h \
	ew_button.h \
	ew_toggle_button.h \
	ew_radio_button.h \
	ew_checkbox.h

libentrance_widgets_ladir = $(prefix)/include


libentrance_widgets_la_SOURCES = \
	ew.c \
	ew_dialog.c \
	ew_notice.c \
	ew_fileselector.c \
	ew_group.c \
	_ew_list.c \
	ew_textlist.c \
	ew_edjelist.c \
	ew_image.c \
	ew_messagebox.c \
	ew_preview.c \
	ew_label.c \
	ew_entry.c \
	ew_button.c \
	ew_toggle_button.c \
	ew_radio_button.c \
	ew_checkbox.c \
	$(libentrance_widgets_la_HEADERS)
	
libentrance_widgets_la_LIBADD  = \
	@evas_libs@  @etk_libs@

libentrance_widgets_la_CFLAGS = \
	-I$(top_srcdir)/src/smarts \
	-I$(top_srcdir)/src/widgets \
	@evas_cflags@ @etk_cflags@

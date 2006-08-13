MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES=libentrance_widgets.la

libentrance_widgets_la_HEADERS = \
	Entrance_Widgets.h \
	ew.h \
	ew_dialog.h \
	ew_group.h \
	_ew_list.h \
	ew_textlist.h \
	ew_edjelist.h \
	ew_image.h \
	ew_notice.h

libentrance_widgets_ladir = $(prefix)/include


libentrance_widgets_la_SOURCES = \
	ew.c \
	ew_dialog.c \
	ew_group.c \
	_ew_list.c \
	ew_textlist.c \
	ew_edjelist.c \
	ew_image.c \
	ew_notice.c \
	$(libentrance_widgets_la_HEADERS)
	
libentrance_widgets_la_LIBADD  = @etk_libs@
libentrance_widgets_la_CFLAGS = @etk_cflags@

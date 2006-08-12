
lib_LTLIBRARIES=libentrance_widgets.la


libentrance_widgets_la_SOURCES = ew.c ew.h \
	ew_dialog.c ew_dialog.h \
	ew_group.c ew_group.h \
	_ew_list.c _ew_list.h \
	ew_textlist.c ew_textlist.h \
	ew_edjelist.c ew_edjelist.h \
	ew_image.c ew_image.h
	
libentrance_widgets_la_HEADERS = Entrance_Widgets.h \
	ew.h \
	ew_dialog.h \
	ew_group.h \
	_ew_list.h \
	ew_textlist.h \
	ew_edjelist.h \
	ew_image.h
libentrance_widgets_ladir = $(prefix)/include
libentrance_widgets_la_LDFLAGS=`etk-config --libs`
libentrance_widgets_la_CFLAGS=`etk-config --cflags` 

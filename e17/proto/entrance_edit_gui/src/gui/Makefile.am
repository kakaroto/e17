MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS=entrance_edit_gui

entrance_edit_gui_SOURCES=main.c \
						  background.c \
						  behavior.c \
						  layout.c \
						  sessions.c \
						  theme.c \
						  x_settings.c \
						  egui_graphics_selector.c \
						  Egui.h

entrance_edit_gui_CFLAGS = \
 -I$(top_srcdir)/src/widgets \
@etk_cflags@ 
##@entrance_cflags@ 

entrance_edit_gui_LDFLAGS = \
 -L$(top_srcdir)/src/widgets

entrance_edit_gui_LDADD = \
 -lentrance_widgets \
 @entrance_libs@

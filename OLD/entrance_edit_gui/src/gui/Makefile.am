MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS=entrance_edit_gui

entrance_edit_gui_SOURCES=main.c \
						  background.c \
						  behavior.c \
						  layout.c \
						  sessions.c \
						  theme.c \
						  x_settings.c \
						  egui_graphics_dialog.c \
						  egui_graphics_dialog.h \
						  Egui.h egui.c


entrance_edit_gui_CFLAGS = \
	-I$(top_srcdir)/src/widgets \
	-I$(top_srcdir)/src/smarts \
	@etk_cflags@ \
	@entrance_edit_cflags@ 

entrance_edit_gui_LDFLAGS = \
	-L$(top_srcdir)/src/widgets \
	-L$(top_srcdir)/src/smarts

entrance_edit_gui_LDADD = \
	-lentrance_widgets \
	-lentrance_smarts \
	@entrance_edit_libs@

## Process this file with automake to produce Makefile.in

INCLUDES = \
	@GTK_CFLAGS@

bin_PROGRAMS = flipbook

flipbook_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	init.c init.h \
	idle.c idle.h \
	loader.c loader.h \
	callbacks.c callbacks.h \
	hooks.c hooks.h \
	menus.c menus.h

flipbook_LDADD = @GTK_LIBS@ -lgthread -ljpeg


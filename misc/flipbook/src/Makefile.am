## Process this file with automake to produce Makefile.in

INCLUDES = \
	@GTK_CFLAGS@

bin_PROGRAMS = flipbook

flipbook_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	hooks.c hooks.h \
	menus.c menus.h

flipbook_LDADD = @GTK_LIBS@


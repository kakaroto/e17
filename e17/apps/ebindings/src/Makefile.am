# Process this file with automake to produce Makefile.in

INCLUDES = \
	@BONUS_INCS@ \
	@GTK_CFLAGS@ \
	@evas_cflags@ \
	@edb_cflags@

bin_PROGRAMS = ebindings

ebindings_SOURCES = \
	eaction_item.c eaction_item.h \
	emenu_item.c emenu_item.h \
	util.c	util.h \
	menu_interface.c menu_interface.h \
	keybind_interface.c keybind_interface.h \
	focus_interface.c focus_interface.h \
	callbacks.c callbacks.h \
	interface.c interface.h \
	desktops.c desktops.h \
	main.c

ebindings_LDADD = \
	@BONUS_LIBS@ \
	@GTK_LIBS@ \
	@evas_libs@ \
	@edb_libs@

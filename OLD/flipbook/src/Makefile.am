## Process this file with automake to produce Makefile.in

INCLUDES = \
	@GTK_CFLAGS@

bin_PROGRAMS = flipbook

flipbook_SOURCES = \
	main.c loadfiles.h \
	support.c support.h \
	interface.c interface.h \
	init.c init.h \
	idle.c idle.h \
	loader.c loader.h \
	callbacks.c callbacks.h \
	hooks.c hooks.h \
	textload.c textload.h \
	menus.c menus.h \
	controls.h

flipbook_LDADD = -L/usr/lib -L/usr/X11R6/lib -lgnomeui -lart_lgpl \
-lgdk_imlib -lSM -lICE -lgtk -lgdk -lgmodule -lXext -lX11 -lgnome \
-lgnomesupport -lesd -laudiofile -lm -ldb1 -lglib -ldl -lgthread -ljpeg \
-lgdk_pixbuf



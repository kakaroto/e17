## Process this file with automake to produce Makefile.in

INCLUDES = @ebg_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@

bin_PROGRAMS = entrance

entrance_SOURCES = \
	sys_callbacks.c sys_callbacks.h \
	entrance_auth.c entrance_auth.h \
	entrance_session.c entrance_session.h \
	entrance_config.c entrance_config.h \
	entrance_theme.c entrance_theme.h \
	entrance_fx.c entrance_fx.h \
	ui_callbacks.c ui_callbacks.h \
	events.c events.h \
	util.c util.h \
	entrance.h \
	intro.c \
	main.c 

entrance_LDADD = @ebg_libs@ @ecore_libs@ @edb_libs@ @evas_libs@

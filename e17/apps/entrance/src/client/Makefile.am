## Process this file with automake to produce Makefile.in

INCLUDES = @edje_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@

bin_PROGRAMS = entrance

entrance_SOURCES = \
	entrance_auth.c entrance_auth.h \
	entrance_session.c entrance_session.h \
	entrance_config.c entrance_config.h \
	EvasTextEntry.c EvasTextEntry.h \
	util.c util.h \
	main.c 

entrance_LDADD = @edje_libs@ @ecore_libs@ @edb_libs@ @evas_libs@

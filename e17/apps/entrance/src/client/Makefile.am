## Process this file with automake to produce Makefile.in

INCLUDES = @edje_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@ \
@esmart_cflags@

bin_PROGRAMS = entrance entrance_login

entrance_SOURCES = \
	entrance_auth.c entrance_auth.h \
	entrance_session.c entrance_session.h \
	entrance_x_session.c entrance_x_session.h \
	entrance_config.c entrance_config.h \
	entrance_user.c entrance_user.h \
	EvasTextEntry.c EvasTextEntry.h \
	util.c util.h \
	main.c 

entrance_LDADD = @edje_libs@ @ecore_libs@ @edb_libs@ @evas_libs@ \
@esmart_libs@

entrance_login_SOURCES = entrance_login.c
entrance_login_LDADD = @edje_libs@ @ecore_libs@ @edb_libs@ @evas_libs@ \
@esmart_libs@

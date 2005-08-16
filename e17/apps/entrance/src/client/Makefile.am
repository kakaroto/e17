## Process this file with automake to produce Makefile.in

INCLUDES = @edje_cflags@ @ecore_cflags@ @evas_cflags@ \
@esmart_cflags@

bin_PROGRAMS = entrance entrance_login entrance_edit

entrance_SOURCES = \
	entrance_auth.c entrance_auth.h \
	entrance_session.c entrance_session.h \
	entrance_config.c entrance_config.h \
	entrance_user.c entrance_user.h \
	entrance_x_session.c entrance_x_session.h \
	entrance_ipc.c entrance_ipc.h \
	entrance_smart.c entrance_smart.h \
	util.c util.h \
	entrance.h main.c 

entrance_LDADD = @edje_libs@ @ecore_libs@ @evas_libs@ \
@esmart_libs@ -lesmart_container -lesmart_text_entry

entrance_login_SOURCES = \
	entrance_login.c \
	entrance_auth.c entrance_auth.h \
	util.c util.h

entrance_edit_SOURCES = \
	entrance_edit.c \
	entrance_config.c entrance_user.c \
	util.c entrance_x_session.c entrance_smart.c
entrance_edit_LDADD = @edje_libs@ @ecore_libs@ @evas_libs@ \
@esmart_libs@ -lesmart_container -lesmart_text_entry

## Process this file with automake to produce Makefile.in

INCLUDES = @EDJE_CFLAGS@ @ECORE_CFLAGS@ @EVAS_CFLAGS@ \
@ESMART_CFLAGS@

entrancelibdir = $(libdir)/$(PACKAGE)

bin_PROGRAMS = entrance entrance_edit

entrancelib_PROGRAMS = entrance_login

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

entrance_LDADD = @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ \
@ESMART_LIBS@ -lesmart_container -lesmart_text_entry

entrance_login_SOURCES = \
	entrance_login.c \
	entrance_auth.c entrance_auth.h \
	util.c util.h

entrance_edit_SOURCES = \
	entrance_edit.c \
	entrance_config.c entrance_user.c \
	util.c entrance_x_session.c entrance_smart.c
entrance_edit_LDADD = @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ \
@ESMART_LIBS@ -lesmart_container -lesmart_text_entry

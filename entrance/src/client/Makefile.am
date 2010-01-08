## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = @EDJE_CFLAGS@ @EFREET_CFLAGS@ @ECORE_CFLAGS@ @EVAS_CFLAGS@ \
@ESMART_CFLAGS@ @ELEMENTARY_CFLAGS@

entrancelibexecdir = $(libexecdir)/$(PACKAGE)

#bin_PROGRAMS = entrance entrance_edit
bin_PROGRAMS = entrance

entrancelibexec_PROGRAMS = entrance_login

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

entrance_LDADD = @EDJE_LIBS@ @EFREET_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ \
@ESMART_LIBS@ @ELEMENTARY_LIBS@

entrance_login_LDADD = @ECORE_LIBS@
entrance_login_SOURCES = \
	entrance_login.c \
	entrance_auth.c entrance_auth.h \
	util.c util.h

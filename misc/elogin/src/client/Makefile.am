## Process this file with automake to produce Makefile.in

INCLUDES = \
	@evas_cflags@ \
	@ebg_cflags@ \
	@edb_cflags@ \
	@ecore_cflags@ \
	@estyle_cflags@

bin_PROGRAMS = elogin

elogin_SOURCES = \
	callbacks.c callbacks.h \
	e_login_auth.c e_login_auth.h \
	e_login_session.c e_login_session.h \
	e_login_config.c e_login_config.h \
	session_bar.c session_bar.h \
	events.c events.h \
	util.c util.h \
	elogin.h \
	intro.c \
	main.c 

elogin_LDADD = \
	@evas_libs@ \
	@ebg_libs@ \
	@edb_libs@ \
	@ecore_libs@ \
	@estyle_libs@

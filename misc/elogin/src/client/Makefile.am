## Process this file with automake to produce Makefile.in

INCLUDES = \
	@evas_cflags@ \
	@ebg_cflags@ \
	@edb_cflags@ \
	@ecore_cflags@

bin_PROGRAMS = elogin

elogin_SOURCES = \
	callbacks.c callbacks.h \
	e_login_auth.c e_login_auth.h \
	e_login_session.c e_login_session.h \
	events.c events.h \
	elogin.h \
	intro.c \
	main.c 

elogin_LDADD = \
	@evas_libs@ \
	@ebg_libs@ \
	@edb_libs@ \
	@ecore_libs@


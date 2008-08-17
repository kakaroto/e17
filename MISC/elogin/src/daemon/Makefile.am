## Process this file with automake to produce Makefile.in

INCLUDES = \
	@evas_cflags@ \
	@ebg_cflags@ \
	@edb_cflags@ \
	@ecore_cflags@

sbin_PROGRAMS = elogind
bin_SCRIPTS = elogin_wrapper

elogind_SOURCES = \
	spawner.c spawner.h    

elogind_LDADD = \
	@evas_libs@ \
	@ebg_libs@ \
	@edb_libs@ \
	@ecore_cflags@


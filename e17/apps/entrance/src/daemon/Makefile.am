## Process this file with automake to produce Makefile.in

INCLUDES = \
	@evas_cflags@ \
	@ebg_cflags@ \
	@edb_cflags@ \
	@ecore_cflags@

sbin_PROGRAMS = entranced
bin_SCRIPTS = entrance_wrapper

entranced_SOURCES = \
	spawner.c spawner.h    

entranced_LDADD = \
	@evas_libs@ \
	@ebg_libs@ \
	@edb_libs@ \
	@ecore_cflags@


## Process this file with automake to produce Makefile.in

INCLUDES = \
	@BONUS_INCS@ \
	@gtk_cflags@ \
	@glib_cflags@ \
	@evas_cflags@ \
	@imlib2_cflags@ \
	@ebg_cflags@ \
	@edb_cflags@ 

bin_PROGRAMS = ebony

ebony_SOURCES = \
	main.c  ebony.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	gtk_util.c gtk_util.h  \
	util.c util.h

ebony_LDADD = \
	@BONUS_LIBS@ \
	@gtk_libs@ \
	@glib_libs@ \
	@evas_libs@ \
	@imlib2_libs@ \
	@ebg_libs@ \
	@edb_libs@ 


# Process this file with automake to produce Makefile.in

INCLUDES = \
	@BONUS_INCS@ \
	@GTK_CFLAGS@ \
	@GLIB_CFLAGS@ \
	@evas_cflags@ \
	@imlib2_cflags@ \
	@ebg_cflags@ \
	@edb_cflags@ 

bin_PROGRAMS = ebony

ebony_SOURCES = \
    advanced.c advanced.h \
    callbacks.c callbacks.h \
    interface.c interface.h \
    util.c util.h \
    main.c

ebony_LDADD = \
	@BONUS_LIBS@ \
	@GTK_LIBS@ \
	@GLIB_CFLAGS@ \
	@evas_libs@ \
	@imlib2_libs@ \
	@ebg_libs@ \
	@edb_libs@ 

## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I../lib \
	@esmart_cflags@ \
	@evas_cflags@ \
	@edb_cflags@ \
	@ecore_cflags@ \
	@edje_cflags@

bin_PROGRAMS = entice

entice_SOURCES = \
entice.c entice.h \
image.c image.h \
ipc.c ipc.h \
keys.c keys.h \
prefs.c prefs.h \
signals_image.c signals_image.h \
signals_thumb.c signals_thumb.h \
main.c

entice_LDADD = \
@esmart_libs@ \
@evas_libs@ \
@edb_libs@ \
@ecore_libs@ \
@edje_libs@ \
-lm -lc

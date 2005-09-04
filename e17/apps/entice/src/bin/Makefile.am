## Process this file with automake to produce Makefile.in

INCLUDES = \
	-Wall -I../lib \
	@IMLIB2_CFLAGS@ \
	@ESMART_CFLAGS@ \
	@EVAS_CFLAGS@ \
	@EDB_CFLAGS@ \
	@ECORE_CFLAGS@ \
	@EPSILON_CFLAGS@ \
	@EDJE_CFLAGS@

bin_PROGRAMS = entice

entice_SOURCES = \
entice.c entice.h \
image.c image.h \
ipc.c ipc.h \
keys.c keys.h \
prefs.c prefs.h \
signals_image.c signals_image.h \
signals_thumb.c signals_thumb.h \
exif.c exif.h \
main.c

entice_LDADD = \
@ESMART_LIBS@ \
-lesmart_container \
-lesmart_draggies \
-lesmart_thumb \
-lesmart_trans_x11 \
@EVAS_LIBS@ \
@EDB_LIBS@ \
@ECORE_LIBS@ \
@EDJE_LIBS@ \
-lm -lc

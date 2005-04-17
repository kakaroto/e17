## Process this file with automake to produce Makefile.in

INCLUDES = \
@gtk_cflags@ \
@evas_cflags@ \
@ecore_cflags@ \
@edje_cflags@ \
@emotion_cflags@ \
@esmart_cflags@ \
@taglib_cflags@

AM_CFLAGS = \
-Wall \
-O1 \
-g

bin_PROGRAMS = eclair

eclair_SOURCES = \
eclair.c eclair.h \
eclair_args.c eclair_args.h \
eclair_utils.c eclair_utils.h \
eclair_callbacks.c eclair_callbacks.h \
eclair_playlist.c eclair_playlist.h \
eclair_private.h \
eclair_subtitles.c eclair_subtitles.h

eclair_LDADD = \
@gtk_libs@ \
@evas_libs@ \
@ecore_libs@ \
@edje_libs@ \
@emotion_libs@ \
@esmart_libs@ -lesmart_container -lesmart_draggies \
@taglib_libs@ -ltag_c

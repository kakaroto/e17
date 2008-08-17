## Process this file with automake to produce Makefile.in

SUBDIRS = eclair_wsz2edj

INCLUDES = \
@ECLAIR_CFLAGS@ \
@taglib_cflags@

AM_CFLAGS = \
-Wall \
-O1

bin_PROGRAMS = eclair

eclair_SOURCES = \
eclair_types.h \
eclair.c eclair.h \
eclair_args.c eclair_args.h \
eclair_utils.c eclair_utils.h \
eclair_callbacks.c eclair_callbacks.h \
eclair_media_file.c eclair_media_file.h \
eclair_playlist.c eclair_playlist.h \
eclair_playlist_container.c eclair_playlist_container.h \
eclair_subtitles.c eclair_subtitles.h \
eclair_cover.c eclair_cover.h \
eclair_meta_tag.c eclair_meta_tag.h \
eclair_config.c eclair_config.h \
eclair_dialogs.c eclair_dialogs.h \
eclair_window.c eclair_window.h \
eclair_database.c eclair_database.h \
eclair_video.c eclair_video.h \
eclair_menu.c eclair_menu.h

eclair_LDADD = \
@ECLAIR_LIBS@ \
-ltag_c \
-lpthread

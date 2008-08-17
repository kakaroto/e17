## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" 

AM_CFLAGS =\
	@ECORE_CFLAGS@ \
	@EVAS_CFLAGS@ \
	@EDJE_CFLAGS@ \
	@EMOTION_CFLAGS@ \
	@LIBINTL@ \
	@MUSICBRAINZ_CFLAGS@ \
	@EDBUS_CFLAGS@\
	@DBUS_CFLAGS@ \
	@EET_CFLAGS@ \
	@SQLITE3_CFLAGS@ \
	@IPOD_CFLAGS@ \
	@BLUETOOTH_CFLAGS@\
	@LIGHTMEDIASCANNER_CFLAGS@

bin_PROGRAMS = enna

enna_SOURCES = \
	enna_mainmenu.c\
	enna_mainmenu.h\
	enna_box.c\
	enna_box.h\
	enna.h\
	enna.c\
	enna_module.c\
	enna_module_music.c\
	enna_module_video.c\
	enna_module_photo.c\
	enna_module_playlist.c\
	enna_mediaplayer.c\
	enna_emusic.c\
	enna_emotion.c\
	enna_miniplayer.c\
	enna_slideshow.c\
	enna_fm.c\
	enna_volume_manager.c\
	enna_event.c\
	enna_lirc.c\
	enna_list.c\
	enna_image.c\
	enna_config.c\
	enna_cdda.c\
	enna_util.c \
	enna_list.h\
	enna_image.h\
	enna_module.h\
	enna_module_music.h\
	enna_module_video.h\
	enna_module_photo.h\
	enna_module_playlist.h\
	enna_mediaplayer.h\
	enna_emusic.h\
	enna_emotion.h\
	enna_miniplayer.h\
	enna_config.h\
	enna_fm.h\
	enna_event.h\
	enna_list.h\
	enna_volume_manager.h\
	enna_cdda.h\
	enna_util.h\
	enna_lirc.h\
	enna_slideshow.h\
	gettext.h \
	enna_fm_db.c\
	enna_fm_db.h \
	enna_bluetooth.c\
	enna_bluetooth.h \
	enna_popup.c\
	enna_popup.h\
	enna_reflection.c\
	enna_reflection.h \
	enna_scanner.c\
	enna_scanner.h

enna_LDADD =  	@EVAS_LIBS@ \
		@ECORE_LIBS@ \
		@EDJE_LIBS@ \
		@EDBUS_LIBS@ \
		@EMOTION_LIBS@\
		@DBUS_LIBS@ \
		@MUSICBRAINZ_LIBS@ \
		@EET_LIBS@ \
		@LIGHTMEDIASCANNER_LIBS@\
		-ltag_c\
		-lehal\
		@lirc_libs@ \
		@SQLITE3_LIBS@ \	
		@IPOD_LIBS@ \
      		@BLUETOOTH_LIBS@



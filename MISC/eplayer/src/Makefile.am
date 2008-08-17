# $Id$

SUBDIRS = input output

AM_CFLAGS = @PTHREAD_CFLAGS@ \
	@ECORE_CFLAGS@ @EVAS_CFLAGS@ @EDJE_CFLAGS@ @ESMART_CFLAGS@ \
	@EWL_CFLAGS@

bin_PROGRAMS = eplayer

eplayer_SOURCES = \
	eplayer.c       eplayer.h \
	callbacks.c     callbacks.h \
	interface.c     interface.h \
	playlist.c      playlist.h \
	playlist_item.c playlist_item.h \
	plugin.c        plugin.h \
	track.c         track.h \
	utils.c         utils.h

eplayer_LDADD = @PTHREAD_LIBS@ @DL_LIBS@ \
	@ECORE_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@ @ESMART_LIBS@ @EWL_LIBS@

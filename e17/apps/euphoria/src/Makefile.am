## $Id$

AM_CFLAGS = \
	@ECORE_CFLAGS@ @EVAS_CFLAGS@ @EDJE_CFLAGS@ @ESMART_CFLAGS@ \
	@EWL_CFLAGS@

bin_PROGRAMS = euphoria

euphoria_SOURCES = \
	euphoria.c      euphoria.h \
	callbacks.c     callbacks.h \
	interface.c     interface.h \
	playlist.c      playlist.h \
	playlist_item.c playlist_item.h \
	utils.c         utils.h

euphoria_LDADD = \
	@ECORE_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@ @ESMART_LIBS@ @EWL_LIBS@ \
	-lxmmsclient -lxmmsclient-ecore

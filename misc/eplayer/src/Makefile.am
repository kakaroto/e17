AM_CFLAGS = @AO_CFLAGS@ \
	@ECORE_CFLAGS@ @EVAS_CFLAGS@ @EDJE_CFLAGS@ @ESMART_CFLAGS@

bin_PROGRAMS = eplayer

eplayer_SOURCES = eplayer.c eplayer.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	vorbis.c vorbis.h \
	playlist.c playlist.h \
	mixer.c mixer.h

eplayer_LDADD = -lvorbis -lvorbisfile @AO_LIBS@ \
	@ECORE_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@ @ESMART_LIBS@

## ePlayer Makefile.am
## gcc -g -lao -lvorbis -lvorbisfile -lecore -lecore_evas e_player.c -o e_player

INCLUDES = -I$(top_srcdir) -I$(includedir) \
	@ECORE_CFLAGS@ @EVAS_CFLAGS@ @EDJE_CFLAGS@ @ESMART_CFLAGS@

bin_PROGRAMS = eplayer

### I think e_player_HEADER is more correct but not doc'ed.
eplayer_SOURCES = eplayer.c eplayer.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	vorbis.c vorbis.h \
	playlist.c playlist.h \
	mixer.c mixer.h

eplayer_LDADD = -lao -lvorbis -lvorbisfile \
	@ECORE_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@ @ESMART_LIBS@

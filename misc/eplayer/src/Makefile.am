## ePlayer Makefile.am
## gcc -g -lao -lvorbis -lvorbisfile -lecore -lecore_evas e_player.c -o e_player

INCLUDES = -I$(top_srcdir) -I$(includedir) $(ecore_cflags) $(edje_cflags)

bin_PROGRAMS = eplayer


### I think e_player_HEADER is more correct but not doc'ed.
eplayer_SOURCES = eplayer.c interface.c callbacks.c vorbis.c eplayer.h
eplayer_LDADD = -lao -lvorbis -lvorbisfile $(edje_libs) $(ecore_libs)

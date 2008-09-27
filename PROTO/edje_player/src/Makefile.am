## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(EDJE_PLAYER_CFLAGS)

AM_CFLAGS =\
	 -Wall

bin_PROGRAMS = edje_player

edje_player_SOURCES = \
	edje_player.c\
	option_pool.c \
	option_pool.h

edje_player_LDFLAGS = 

edje_player_LDADD = $(EDJE_PLAYER_LIBS)


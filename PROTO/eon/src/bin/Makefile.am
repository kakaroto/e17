MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

EXTRA_DIST =

INCLUDES = \
-I. -I../.. -I$(top_srcdir)/src/include -Wall -g \
@ETK_CFLAGS@

bin_PROGRAMS = eon_player

## Testsuite
eon_player_DEPENDENCIES = $(top_builddir)/src/lib/libeon.la

eon_player_SOURCES = \
eon_player.c

eon_player_LDADD = $(top_builddir)/src/lib/libeon.la \
@ETK_LIBS@ \
-lm

if BUILD_ENGINE_SDL
eon_player_LDADD += @ECORE_SDL_LIBS@ @SDL_LIBS@ @EXML_LIBS@
endif

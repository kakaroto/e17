# $Id$

if HAVE_VORBIS

plugin_LTLIBRARIES = libVorbis.la
plugindir = @PLUGIN_DIR@/input

libVorbis_la_SOURCES = vorbis.c
libVorbis_la_LIBADD = @VORBIS_LIBS@

endif

libVorbis_la_LDFLAGS = -module -avoid-version

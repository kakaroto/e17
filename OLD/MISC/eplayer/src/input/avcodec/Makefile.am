# $Id$

if HAVE_AVCODEC

AM_CFLAGS = @AVCODEC_CFLAGS@

plugin_LTLIBRARIES = libAVCodec.la
plugindir = @PLUGIN_DIR@/input

libAVCodec_la_SOURCES = avcodec.c
libAVCodec_la_LIBADD = @AVCODEC_LIBS@

endif

libAVCodec_la_LDFLAGS = -module -avoid-version

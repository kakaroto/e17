# $Id$

if HAVE_ALSA

AM_CFLAGS = @ALSA_CFLAGS@

plugin_LTLIBRARIES = libALSA.la
plugindir = @PLUGIN_DIR@/output

libALSA_la_SOURCES = alsa.c
libALSA_la_LIBADD = @ALSA_LIBS@
libALSA_la_LDFLAGS = -module -avoid-version

endif

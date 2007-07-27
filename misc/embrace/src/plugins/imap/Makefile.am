## $Id$

if BUILD_IMAP

AM_CFLAGS = -I$(top_srcdir)/src/include \
            @EMBRACE_CFLAGS@

plugin_LTLIBRARIES = libimap.la
plugindir = @PLUGIN_DIR@

libimap_la_SOURCES = imap.c

endif

libimap_la_LDFLAGS = -module -avoid-version

## $Id$

if BUILD_IMAP

AM_CFLAGS = -I$(top_srcdir)/src \
            @EDB_CFLAGS@        \
            @EVAS_CFLAGS@       \
            @ECORE_CFLAGS@

plugin_LTLIBRARIES = libimap.la
plugindir = @PLUGIN_DIR@

libimap_la_SOURCES = imap.c

endif

libimap_la_LDFLAGS = -module -avoid-version

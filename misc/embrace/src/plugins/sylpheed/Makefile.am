## $Id$

if BUILD_SYLPHEED

AM_CFLAGS = -I$(top_srcdir)/src/include \
            @XML_CFLAGS@   \
            @EDB_CFLAGS@   \
            @EVAS_CFLAGS@  \
            @ECORE_CFLAGS@ \
            @EDJE_CFLAGS@

plugin_LTLIBRARIES = libsylpheed.la
plugindir = @PLUGIN_DIR@

libsylpheed_la_SOURCES = sylpheed.c
libsylpheed_la_LIBADD = @XML_LIBS@

endif

libsylpheed_la_LDFLAGS = -module -avoid-version

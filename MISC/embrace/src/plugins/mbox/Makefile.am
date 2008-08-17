## $Id$

if BUILD_MBOX

AM_CFLAGS = -I$(top_srcdir)/src/include \
            @EMBRACE_CFLAGS@

plugin_LTLIBRARIES = libmbox.la
plugindir = @PLUGIN_DIR@

libmbox_la_SOURCES = mbox.c

endif

libmbox_la_LDFLAGS = -module -avoid-version

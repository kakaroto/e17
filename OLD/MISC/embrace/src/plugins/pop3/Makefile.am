## $Id$

if BUILD_POP3

AM_CFLAGS = -I$(top_srcdir)/src/include \
            @EMBRACE_CFLAGS@

plugin_LTLIBRARIES = libpop3.la
plugindir = @PLUGIN_DIR@

libpop3_la_SOURCES = pop3.c

endif

libpop3_la_LDFLAGS = -module -avoid-version

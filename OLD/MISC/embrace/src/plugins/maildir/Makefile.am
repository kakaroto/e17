## $Id$

if BUILD_MAILDIR

AM_CFLAGS = -I$(top_srcdir)/src/include \
            @EMBRACE_CFLAGS@

plugin_LTLIBRARIES = libmaildir.la
plugindir = @PLUGIN_DIR@

libmaildir_la_SOURCES = maildir.c

endif

libmaildir_la_LDFLAGS = -module -avoid-version

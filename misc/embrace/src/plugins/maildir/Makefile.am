## $Id$

if BUILD_MAILDIR

INCLUDES = -I$(top_srcdir)/src

plugin_LTLIBRARIES = libmaildir.la
plugindir = @PLUGIN_DIR@

libmaildir_la_SOURCES = maildir.c

endif

libmaildir_la_LDFLAGS = -module -avoid-version

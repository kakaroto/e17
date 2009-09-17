MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# Gettext
datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST =

INCLUDES	= \
-I. -I../.. -I$(top_srcdir)/src/lib -Wall -g \
@EVAS_CFLAGS@ @ECORE_CFLAGS@ @EDJE_CFLAGS@

bin_PROGRAMS = etk_prefs

etk_prefs_DEPENDENCIES = $(top_builddir)/src/lib/libetk.la

etk_prefs_SOURCES = \
etk_prefs.c

etk_prefs_LDADD = $(top_builddir)/src/lib/libetk.la \
		 @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@

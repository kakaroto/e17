MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DGETTEXT_PACKAGE=\"$(PACKAGE)\" \
-DLOCALEDIR=\"$(localedir)\" \
@ELEMENTARY_CFLAGS@ \
@EMOTION_CFLAGS@ \
@LMS_CFLAGS@ \
@SQLITE3_CFLAGS@ \
@EDBUS_CFLAGS@

bin_PROGRAMS = enjoy
if BUILD_QUICKLAUNCH
bin_PROGRAMS += enjoy_ql
endif

enjoy_LDADD = @ELEMENTARY_LIBS@ @EMOTION_LIBS@ @LMS_LIBS@ @SQLITE3_LIBS@ @EDBUS_LIBS@
enjoy_CFLAGS = -rdynamic
enjoy_SOURCES = main.c win.c db.c list.c page.c cover.c nowplaying.c libmanager.c coverart-lastfm.c

if BUILD_QUICKLAUNCH
############################################################################
## Build quick launch binary, needs elementary_quicklaunch to be enabled. ##
##                                                                        ##
## It is composed of a library with actual code and a binary that talks   ##
## to server that will then fork() + dlopen() such library.               ##
############################################################################
enjoy_qldir = $(quicklauncher_libdir)
enjoy_ql_LTLIBRARIES = enjoy_ql.la
enjoy_ql_la_SOURCES = main.c win.c db.c list.c page.c cover.c nowplaying.c libmanager.c coverart-lastfm.c
enjoy_ql_la_LIBADD = @ELEMENTARY_LIBS@ @EMOTION_LIBS@ @LMS_LIBS@ @SQLITE3_LIBS@ @EDBUS_LIBS@
enjoy_ql_la_CFLAGS =
enjoy_ql_la_LDFLAGS = -module -avoid-version -no-undefined
enjoy_ql_SOURCES = main.c
enjoy_ql_LDADD = @ELEMENTARY_LIBS@
enjoy_ql_CFLAGS = -DELM_LIB_QUICKLAUNCH=1
enjoy_ql_LDFLAGS =
endif

noinst_HEADERS = gettext.h private.h coverart-lastfm.h
EXTRA_DIST = gettext.h private.h coverart-lastfm.h

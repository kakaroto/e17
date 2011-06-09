MAINTAINERCLEANFILES = Makefile.in

EWEBKIT_DATA_DIR=@ewebkit_datadir@

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DEWEBKIT_DATA_DIR=\"$(EWEBKIT_DATA_DIR)\" \
-DGETTEXT_PACKAGE=\"$(PACKAGE)\" \
-DLOCALEDIR=\"$(localedir)\" \
@ELEMENTARY_CFLAGS@ \
@EWEBKIT_CFLAGS@ \
@EDBUS_CFLAGS@

bin_PROGRAMS = eve
if BUILD_QUICKLAUNCH
bin_PROGRAMS += eve_ql
endif

eve_LDADD = @ELEMENTARY_LIBS@ @EWEBKIT_LIBS@ @EDBUS_LIBS@ -lm
eve_SOURCES = main.c view.c chrome.c eve_state.c

if BUILD_QUICKLAUNCH
############################################################################
## Build quick launch binary, needs elementary_quicklaunch to be enabled. ##
##                                                                        ##
## It is composed of a library with actual code and a binary that talks   ##
## to server that will then fork() + dlopen() such library.               ##
############################################################################
eve_qldir = $(quicklauncher_libdir)
eve_ql_LTLIBRARIES = eve_ql.la
eve_ql_la_SOURCES = main.c view.c chrome.c eve_state.h
eve_ql_la_LIBADD = @ELEMENTARY_LIBS@ @EWEBKIT_LIBS@ @EDBUS_LIBS@ -lm
eve_ql_la_CFLAGS =
eve_ql_la_LDFLAGS = -module -avoid-version -no-undefined
eve_ql_SOURCES = main.c
eve_ql_LDADD = @ELEMENTARY_LIBS@
eve_ql_CFLAGS = -DELM_LIB_QUICKLAUNCH=1
eve_ql_LDFLAGS =
endif

noinst_HEADERS = gettext.h private.h eve_state.h
EXTRA_DIST = gettext.h private.h eve_state.h

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DGETTEXT_PACKAGE=\"$(PACKAGE)\" \
-DLOCALEDIR=\"$(localedir)\" \
@ELEMENTARY_CFLAGS@

bin_PROGRAMS = @PROJECT@
if BUILD_QUICKLAUNCH
bin_PROGRAMS += @PROJECT@_ql
endif

@PROJECT_@_LDADD = @ELEMENTARY_LIBS@
@PROJECT_@_SOURCES = main.c

if BUILD_QUICKLAUNCH
############################################################################
## Build quick launch binary, needs elementary_quicklaunch to be enabled. ##
##                                                                        ##
## It is composed of a library with actual code and a binary that talks   ##
## to server that will then fork() + dlopen() such library.               ##
############################################################################
@PROJECT_@_qldir = $(quicklauncher_libdir)
@PROJECT_@_ql_LTLIBRARIES = @PROJECT@_ql.la
@PROJECT_@_ql_la_SOURCES = main.c
@PROJECT_@_ql_la_LIBADD = @ELEMENTARY_LIBS@
@PROJECT_@_ql_la_CFLAGS =
@PROJECT_@_ql_la_LDFLAGS = -module -avoid-version -no-undefined
@PROJECT_@_ql_SOURCES = main.c
@PROJECT_@_ql_LDADD = @ELEMENTARY_LIBS@
@PROJECT_@_ql_CFLAGS = -DELM_LIB_QUICKLAUNCH=1
@PROJECT_@_ql_LDFLAGS =
endif

noinst_HEADERS = gettext.h
EXTRA_DIST = gettext.h

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DGETTEXT_PACKAGE=\"$(PACKAGE)\" \
-DLOCALEDIR=\"$(localedir)\" \
@ELEMENTARY_CFLAGS@

bin_PROGRAMS = envision
if BUILD_QUICKLAUNCH
bin_PROGRAMS += envision_ql
endif

envision_LDADD = @ELEMENTARY_LIBS@
envision_SOURCES = main.c

if BUILD_QUICKLAUNCH
############################################################################
## Build quick launch binary, needs elementary_quicklaunch to be enabled. ##
##                                                                        ##
## It is composed of a library with actual code and a binary that talks   ##
## to server that will then fork() + dlopen() such library.               ##
############################################################################
envision_qldir = $(quicklauncher_libdir)
envision_ql_LTLIBRARIES = envision_ql.la
envision_ql_la_SOURCES = main.c
envision_ql_la_LIBADD = @ELEMENTARY_LIBS@
envision_ql_la_CFLAGS =
envision_ql_la_LDFLAGS = -module -avoid-version -no-undefined
envision_ql_SOURCES = main.c
envision_ql_LDADD = @ELEMENTARY_LIBS@
envision_ql_CFLAGS = -DELM_LIB_QUICKLAUNCH=1
envision_ql_LDFLAGS =
endif

noinst_HEADERS = gettext.h
EXTRA_DIST = gettext.h

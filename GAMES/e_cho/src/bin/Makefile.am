MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DGETTEXT_PACKAGE=\"$(PACKAGE)\" \
-DLOCALEDIR=\"$(localedir)\" \
@ELEMENTARY_CFLAGS@ \
@LIBCANBERRA_CFLAGS@ \
@EVAS_CFLAGS@ \
@ECORE_CFLAGS@ \
@EDJE_CFLAGS@

bin_PROGRAMS = e_cho
if BUILD_QUICKLAUNCH
bin_PROGRAMS += e_cho_ql
endif

e_cho_LDADD = @ELEMENTARY_LIBS@ @LIBCANBERRA_LIBS@ @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@
e_cho_SOURCES = main.c sound.c

if BUILD_QUICKLAUNCH
############################################################################
## Build quick launch binary, needs elementary_quicklaunch to be enabled. ##
##                                                                        ##
## It is composed of a library with actual code and a binary that talks   ##
## to server that will then fork() + dlopen() such library.               ##
############################################################################
e_cho_qldir = $(quicklauncher_libdir)
e_cho_ql_LTLIBRARIES = e_cho_ql.la
e_cho_ql_la_SOURCES = main.c sound.c
e_cho_ql_la_LIBADD = @ELEMENTARY_LIBS@ @LIBCANBERRA_LIBS@ @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@
e_cho_ql_la_CFLAGS =
e_cho_ql_la_LDFLAGS = -module -avoid-version -no-undefined
e_cho_ql_SOURCES = main.c sound.c
e_cho_ql_LDADD = @ELEMENTARY_LIBS@ @LIBCANBERRA_LIBS@ @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@
e_cho_ql_CFLAGS = -DELM_LIB_QUICKLAUNCH=1
e_cho_ql_LDFLAGS =
endif

noinst_HEADERS = gettext.h sound.h
EXTRA_DIST = gettext.h sound.h

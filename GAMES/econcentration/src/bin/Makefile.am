MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ELEMENTARY_CFLAGS@ \
@ETROPHY_CFLAGS@

bin_PROGRAMS = econcentration
if BUILD_QUICKLAUNCH
bin_PROGRAMS += econcentration_ql
endif

econcentration_LDADD = @ELEMENTARY_LIBS@ @ETROPHY_LIBS@
econcentration_SOURCES = main.c

if BUILD_QUICKLAUNCH
############################################################################
## Build quick launch binary, needs elementary_quicklaunch to be enabled. ##
##                                                                        ##
## It is composed of a library with actual code and a binary that talks   ##
## to server that will then fork() + dlopen() such library.               ##
############################################################################
econcentration_qldir = $(quicklauncher_libdir)
econcentration_ql_LTLIBRARIES = econcentration_ql.la
econcentration_ql_la_SOURCES = main.c
econcentration_ql_la_LIBADD = @ELEMENTARY_LIBS@ @ETROPHY_LIBS@
econcentration_ql_la_CFLAGS =
econcentration_ql_la_LDFLAGS = -module -avoid-version -no-undefined
econcentration_ql_SOURCES = main.c
econcentration_ql_LDADD = @ELEMENTARY_LIBS@ @ETROPHY_LIBS@
econcentration_ql_CFLAGS = -DELM_LIB_QUICKLAUNCH=1
econcentration_ql_LDFLAGS =
endif

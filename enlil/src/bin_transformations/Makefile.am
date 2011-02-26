
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/enlil \
-I$(top_builddir)/src/enlil \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ENLIL_CFLAGS@

bin_PROGRAMS = enlil_transformations

enlil_transformations_SOURCES = main.c
enlil_transformations_LDADD = $(top_builddir)/src/enlil/libenlil.la @ENLIL_LIBS@

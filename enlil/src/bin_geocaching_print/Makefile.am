
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-Wall -g -I$(top_srcdir)/src/enlil @ENLIL_CFLAGS@ @EABZU_CFLAGS@

bin_PROGRAMS = enlil_geocaching_print

enlil_geocaching_print_SOURCES = bin_print.c
enlil_geocaching_print_LDADD = $(top_builddir)/src/enlil/libenlil.la @ENLIL_LIBS@ @EABZU_LIBS@


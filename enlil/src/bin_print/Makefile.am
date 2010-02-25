
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-Wall -g -I$(top_srcdir)/src/enlil @ENLIL_CFLAGS@ @FLICKR_CFLAGS@

bin_PROGRAMS = enlil_db_print

enlil_db_print_SOURCES = bin_print.c
enlil_db_print_LDADD = $(top_builddir)/src/enlil/libenlil.la @ENLIL_LIBS@ @FLICKR_LIBS@


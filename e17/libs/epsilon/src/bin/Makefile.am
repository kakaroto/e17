## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/src/lib \
           @IMLIB2_CFLAGS@ \
           @EVAS_CFLAGS@ \
           @ECORE_CFLAGS@ \
           @EDJE_CFLAGS@

bin_PROGRAMS = epsilon

epsilon_SOURCES = \
epsilon_main.c 

epsilon_LDADD = \
$(top_builddir)/src/lib/libepsilon.la @IMLIB2_LIBS@ @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@

epsilon_DEPENDENCIES = $(top_builddir)/src/lib/libepsilon.la

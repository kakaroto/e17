## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@my_includes@

bin_PROGRAMS = epeg

epeg_SOURCES = \
epeg_main.c \
epeg_main.h

epeg_LDADD = \
$(top_builddir)/src/lib/libepeg.la

epeg_DEPENDENCIES = $(top_builddir)/src/lib/libepeg.la

## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@ewl_cflags@ \
@ecore_cflags@

bin_PROGRAMS = equate

equate_SOURCES = \
gui.c \
calc_parse.c \
edje.c \
main.c

equate_LDADD = @ewl_libs@ @ecore_libs@


## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@EQUATE_CFLAGS@

bin_PROGRAMS = equate

equate_SOURCES = \
gui.c \
calc_parse.c \
edje.c \
main.c

equate_LDADD = @EQUATE_LIBS@


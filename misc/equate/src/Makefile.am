## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@ewl_cflags@

bin_PROGRAMS = equate

equate_SOURCES = \
gui.c \
calc.tab.c \
main.c

equate_LDADD = @ewl_libs@ 

equate_CFLAGS = @ewl_cflags@


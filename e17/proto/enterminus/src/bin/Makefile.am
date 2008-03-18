## Process this file with automake to produce Makefile.in

INCLUDES = \
-I$(top_srcdir)/lib \
@evas_CFLAGS@ @ecore_CFLAGS@

bin_PROGRAMS = enterminus

enterminus_SOURCES = \
enterm.h \
term.h \
main.c \
misc.c \
pty.c  \
term.c \
ui.c \
handlers.c \
smart.c \
enterm.c

enterminus_LDADD = @evas_LIBS@ @ecore_LIBS@

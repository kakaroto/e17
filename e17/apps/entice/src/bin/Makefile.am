## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I../lib \
	@imlib2_cflags@ @evas_cflags@ @edb_cflags@ @ecore_cflags@

bin_PROGRAMS = entice

entice_SOURCES = \
buttons.c \
event.c \
fade.c \
file.c \
handler.c \
image.c \
list.c \
main.c \
misc.c \
panel.c \
thumb.c \
buttons.h \
entice.h \
event.h \
fade.h \
file.h \
globals.h \
handler.h \
image.h \
list.h \
misc.h \
panel.h \
thumb.h

entice_LDADD = @imlib2_libs@ @evas_libs@ @edb_libs@ @ecore_libs@ -lm -lc

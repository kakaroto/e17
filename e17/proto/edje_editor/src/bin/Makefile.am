## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-Wall -g \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@ECORE_CFLAGS@ \
@ETK_CFLAGS@ \
@EVAS_CFLAGS@ \
@ENGRAVE_CFLAGS@ \
@EDJE_CFLAGS@

bin_PROGRAMS = edje_editor

edje_editor_INCLUDES = \
callbacks.h \
evas.h \
interface.h \
main.h \
parse.h

edje_editor_SOURCES = \
callbacks.c \
evas.c \
interface.c \
main.c \
parse.c \
$(edje_editor_INCLUDES)

edje_editor_LDADD = @ECORE_LIBS@ @ETK_LIBS@ @EVAS_LIBS@ @ENGRAVE_LIBS@ @EDJE_LIBS@

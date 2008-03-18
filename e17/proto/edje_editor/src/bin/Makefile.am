## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-Wall -g \
-I$(top_srcdir)/lib \
@ECORE_CFLAGS@ \
@ETK_CFLAGS@ \
@EVAS_CFLAGS@ \
@EDJE_CFLAGS@

bin_PROGRAMS = edje_editor

edje_editor_INCLUDES = \
callbacks.h \
evas.h \
interface.h \
main.h

edje_editor_SOURCES = \
callbacks.c \
evas.c \
interface.c \
main.c \
$(edje_editor_INCLUDES)

edje_editor_LDADD = @ECORE_LIBS@ @ETK_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@

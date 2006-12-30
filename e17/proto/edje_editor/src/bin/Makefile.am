## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-Wall -g \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@ETK_CFLAGS@ \
@GLIB2_CFLAGS@ \
@EVAS_CFLAGS@ \
@EDJE_CFLAGS@

bin_PROGRAMS = edje_editor

edje_editor_INCLUDES = \
callbacks.h \
evas.h \
nterface.h \
main.h \
parse.h

edje_editor_SOURCES = \
callbacks.c \
evas.c \
interface.c \
main.c \
parse.c \
$(edje_editor_INCLUDES)

edje_editor_LDADD = @ETK_LIBS@ @GLIB2_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@

## Process this file with automake to produce Makefile.in

INCLUDES = @evas_CFLAGS@ @ecore_CFLAGS@ @edje_CFLAGS@ @etk_CFLAGS@

bin_PROGRAMS = edje_viewer

edje_viewer_INCLUDES = \
edje_viewer.h edje_etk.h etk_gui.h conf.h

edje_viewer_SOURCES = \
edje_etk.c etk_gui.c main.c conf.c \
$(edje_viewer_INCLUDES)

edje_viewer_LDADD = @evas_LIBS@ @ecore_LIBS@ @edje_LIBS@ @etk_LIBS@

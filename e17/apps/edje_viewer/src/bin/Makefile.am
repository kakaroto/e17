## Process this file with automake to produce Makefile.in

INCLUDES = @EDJEVIEWER_CFLAGS@ @e_cflags@

bin_PROGRAMS = edje_viewer

edje_viewer_INCLUDES = \
edje_viewer.h edje_etk.h etk_gui.h conf.h

edje_viewer_SOURCES = \
edje_etk.c etk_gui.c main.c conf.c \
$(edje_viewer_INCLUDES)

edje_viewer_LDADD = @EDJEVIEWER_LIBS@ @E_LIBS@


MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EDJE_VIEWER_CFLAGS@

bin_PROGRAMS = edje_viewer

edje_viewer_SOURCES = \
edje_viewer_conf.c \
edje_viewer_edje.c \
edje_viewer_gui.c \
edje_viewer_main.c

edje_viewer_LDADD = @EDJE_VIEWER_LIBS@

EXTRA_DIST = \
edje_viewer_conf.h \
edje_viewer_edje.h \
edje_viewer_gui.h \
edje_viewer_main.h


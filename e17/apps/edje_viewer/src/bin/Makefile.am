
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EDJE_VIEWER_CFLAGS@

bin_PROGRAMS = edje_viewer

edje_viewer_SOURCES = \
conf.c \
edje_etk.c \
etk_gui.c \
main.c

edje_viewer_LDADD = @EDJE_VIEWER_LIBS@

EXTRA_DIST = \
conf.h \
edje_etk.h \
etk_gui.h

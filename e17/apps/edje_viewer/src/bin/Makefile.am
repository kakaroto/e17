## Process this file with automake to produce Makefile.in

INCLUDES = \
-Wall -g \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@my_includes@

bin_PROGRAMS = edje_viewer

edje_viewer_INCLUDES = \
edje_viewer.h edje_etk.h etk_gui.h

edje_viewer_SOURCES = \
edje_etk.c etk_gui.c main.c \
$(edje_etk_INCLUDES)

edje_viewer_LDADD = @my_libs@

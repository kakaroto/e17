## Process this file with automake to produce Makefile.in

INCLUDES = \
@edb_cflags@ \
@evas_cflags@ \
@edje_cflags@ \
@ecore_cflags@ \
@esmart_cflags@ \
@ewl_cflags@ 

bin_PROGRAMS = iconbar

iconbar_SOURCES = \
iconbar.c iconbar.h \
iconbar_smart.c \
icon.c icon.h \
util.c util.h \
prefs.c prefs.h \
icon_editor.c icon_editor.h \
main.c

iconbar_LDADD = \
@edb_libs@ \
@evas_libs@ \
@edje_libs@ \
@ecore_libs@ \
@esmart_libs@ \
@ewl_libs@ \
-ledje_edit



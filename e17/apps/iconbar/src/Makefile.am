## Process this file with automake to produce Makefile.in

INCLUDES = \
@edb_cflags@ \
@evas_cflags@ \
@edje_cflags@ \
@ecore_cflags@ \
@esmart_cflags@

bin_PROGRAMS = iconbar

iconbar_SOURCES = \
iconbar.c iconbar.h \
util.c util.h \
main.c

iconbar_LDADD = \
@edb_libs@ \
@evas_libs@ \
@edje_libs@ \
@ecore_libs@ \
@esmart_libs@



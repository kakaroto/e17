## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@evas_cflags@ \
@ecore_cflags@ \
@esmart_cflags@ \
@imlib_cflags@

bin_PROGRAMS = engage

engage_SOURCES = \
main.c \
config.c \
dock.c \
icon.c \
wm.c \
window.c \
userconfig.c


engage_LDADD = @evas_libs@ @ecore_libs@ @esmart_libs@ @imlib_libs@


## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@ @imlib_cflags@

installed_headersdir = $(prefix)/include/Esmart/

noinst_LTLIBRARIES = libethumb.la

installed_headers_DATA = \
	E_Thumb.h 

libethumb_la_SOURCES = \
	md5.c md5.h \
	E_Thumb.c E_Thumb.h

libethumb_la_LIBADD = @evas_libs@ @imlib_libs@ -lm
libethumb_la_LDFLAGS = -version-info 1:0:0
libethumb_la_CFLAGS = -g

## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@ @epsilon_cflags@ @imlib_cflags@

installed_headersdir = $(prefix)/include/Esmart/

noinst_LTLIBRARIES = libethumb.la

installed_headers_DATA = \
	Esmart_Thumb.h 

libethumb_la_SOURCES = \
	E_Thumb.c Esmart_Thumb.h

libethumb_la_LIBADD = @evas_libs@ @epsilon_libs@ @imlib_libs@ -lm
libethumb_la_LDFLAGS = -version-info 1:0:0
libethumb_la_CFLAGS = -g

## Process this file with automake to produce Makefile.in

INCLUDES = @ecore_cflags@ @evas_cflags@ @imlib_cflags@

installed_headersdir = $(prefix)/include/Esmart/

noinst_LTLIBRARIES = libepseudotrans.la

installed_headers_DATA = \
	X11_Trans.h 

libepseudotrans_la_SOURCES = \
	X11_Trans.c X11_Trans.h

libepseudotrans_la_LIBADD = @evas_libs@ @imlib_libs@ -lm
libepseudotrans_la_LDFLAGS = -version-info 1:0:0
libepseudotrans_la_CFLAGS = -g

## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@ @ecore_cflags@

installed_headersdir = $(prefix)/include/Esmart/

noinst_LTLIBRARIES = libecore_evas_dragable.la

installed_headers_DATA = \
	dragable.h 

libecore_evas_dragable_la_SOURCES = \
	dragable.c dragable.h

libecore_evas_dragable_la_LIBADD = @evas_libs@ @ecore_cflags@
libecore_evas_dragable_la_LDFLAGS = -version-info 1:0:0
libecore_evas_dragable_la_CFLAGS = -g

## Process this file with automake to produce Makefile.in

INCLUDES = @ecore_cflags@ @evas_cflags@ @imlib_cflags@

installed_headersdir = $(prefix)/include/Esmart/

noinst_LTLIBRARIES = libesmart_trans.la

installed_headers_DATA = \
	Esmart_Trans.h

libesmart_trans_la_SOURCES = \
	esmart_trans.c

libesmart_trans_la_LIBADD = @evas_libs@ @imlib_libs@ -lm
libesmart_trans_la_LDFLAGS = -version-info 1:0:0
libesmart_trans_la_CFLAGS = -g

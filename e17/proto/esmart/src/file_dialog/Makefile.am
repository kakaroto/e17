## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@ @ecore_cflags@ @edje_cflags@

installed_headersdir = $(prefix)/include/Esmart/

noinst_LTLIBRARIES = libesmart_file_dialog.la

installed_headers_DATA = \
	Esmart_File_Dialog.h 

libesmart_file_dialog_la_SOURCES = \
	esmart_file_dialog.c Esmart_File_Dialog.h

libesmart_file_dialog_la_LIBADD = @evas_libs@ @ecore_libs@ @edje_libs@
libesmart_file_dialog_la_LDFLAGS = -version-info 1:0:0
libesmart_file_dialog_la_CFLAGS = -g

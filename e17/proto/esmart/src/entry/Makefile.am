## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@ @ecore_cflags@ @edje_cflags@

installed_headersdir = $(prefix)/include/Esmart/

noinst_LTLIBRARIES = libesmart_text_entry.la

installed_headers_DATA = \
	Esmart_Text_Entry.h 

libesmart_text_entry_la_SOURCES = \
	Esmart_Text_Entry.c Esmart_Text_Entry.h

libesmart_text_entry_la_LIBADD = @evas_libs@ @ecore_libs@ @edje_libs@
libesmart_text_entry_la_LDFLAGS = -version-info 1:0:0
libesmart_text_entry_la_CFLAGS = -g

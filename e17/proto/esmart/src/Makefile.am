## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@ @imlib_cflags@

SUBDIRS = container thumb

lib_LTLIBRARIES = libesmart.la

include_HEADERS= 

libesmart_la_SOURCES = esmart.c

libesmart_la_LIBADD 	= container/libecontainer.la \
			  thumb/libethumb.la 
			  @evas_libs@ @imlib_libs@ -lm

libesmart_la_DEPENDENCIES = container/libecontainer.la \
			    thumb/libethumb.la 
			  
#libesmart_la_LDFLAGS = -version-info 1:0:0
libesmart_la_CFLAGS = -g

## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@ @imlib_cflags@ @ecore_cflags@

SUBDIRS = container thumb trans draggies

lib_LTLIBRARIES = libesmart.la

include_HEADERS= 

libesmart_la_SOURCES = esmart.c

libesmart_la_LIBADD 	= container/libecontainer.la \
			  thumb/libethumb.la  trans/libesmart_trans.la \
			  draggies/libecore_evas_dragable.la \
			  @evas_libs@ @imlib_libs@ @ecore_libs@ -lm

libesmart_la_DEPENDENCIES = container/libecontainer.la \
			    thumb/libethumb.la \
			    draggies/libecore_evas_dragable.la \
			    trans/libesmart_trans.la
			  
#libesmart_la_LDFLAGS = -version-info 1:0:0
libesmart_la_CFLAGS = -g

## Process this file with automake to produce Makefile.in

SUBDIRS = layout

INCLUDES = @evas_cflags@ @imlib_cflags@

installed_headersdir = $(prefix)/include/Esmart/

noinst_LTLIBRARIES = libecontainer.la

installed_headers_DATA = \
	Esmart_Container.h 

libecontainer_la_SOURCES = \
	container.c Esmart_Container.h \
	container_element.c \
	container_smart.c \
	container_plugin.c container_plugin.h \
	container_util.c

libecontainer_la_LIBADD = @evas_libs@ @imlib_libs@ -lm
libecontainer_la_LDFLAGS = -version-info 1:0:0
libecontainer_la_CFLAGS = -g -Wall

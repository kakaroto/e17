## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@

lib_LTLIBRARIES = libestyle.la

installed_headersdir = $(prefix)/include

installed_headers_DATA = \
Estyle.h

libestyle_la_SOURCES = \
Estyle.h \
estyle.c \
estyle_style.c \
estyle_color.c \
estyle_heap.c


libestyle_la_LIBADD       = @evas_libs@
libestyle_la_DEPENDENCIES = $(top_builddir)/estyle-config.h
libestyle_la_LDFLAGS      = -version-info 0:1:0

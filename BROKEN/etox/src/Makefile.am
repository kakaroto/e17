## Process this file with automake to produce Makefile.in

SUBDIRS = style

INCLUDES = @evas_cflags@ @ecore_cflags@ @edb_cflags@

lib_LTLIBRARIES = libetox.la

installed_headersdir = $(prefix)/include

installed_headers_DATA = \
Etox.h

libetox_la_SOURCES = \
Etox.h \
Etox_private.h \
etox_line.h \
etox_obstacle.h \
etox.c \
etox_line.c \
etox_context.c \
etox_obstacle.c \
etox_selection.c


libetox_la_LIBADD       = style/libetox_style.la @evas_libs@ @ecore_libs@
libetox_la_DEPENDENCIES = $(top_builddir)/config.h style/libetox_style.la
libetox_la_LDFLAGS      = -version-info 0:9:0


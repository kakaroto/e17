## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@ @ewd_cflags@

lib_LTLIBRARIES = libetox.la

installed_headersdir = $(prefix)/include

installed_headers_DATA = \
Etox.h

libetox_la_SOURCES = \
Etox.h \
etox.c \
etox_line.c \
etox_context.c \
etox_obstacle.c


libetox_la_LIBADD       = @evas_libs@ @ewd_libs@
libetox_la_DEPENDENCIES = $(top_builddir)/etox-config.h
libetox_la_LDFLAGS      = -version-info 0:2:0

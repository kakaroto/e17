## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@ @ewd_cflags@

lib_LTLIBRARIES = libetox.la

installed_headersdir = $(prefix)/include

installed_headers_DATA = \
Etox.h

libetox_la_SOURCES = \
Etox.h \
Etox_private.h \
etox.c \
etox_bit.c \
etox_color.c \
etox_create.c \
etox_get.c \
etox_loadfile.c \
etox_object.c \
etox_obstacle.c \
etox_set.c \
etox_str.c \
etox_style.c \
etox_align.c \
etox_sort.c


libetox_la_LIBADD       = @evas_libs@ @ewd_libs@
libetox_la_DEPENDENCIES = $(top_builddir)/etox-config.h
libetox_la_LDFLAGS      = -version-info 0:1:0

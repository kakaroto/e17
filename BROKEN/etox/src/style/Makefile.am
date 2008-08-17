## Process this file with automake to produce Makefile.in

INCLUDES = @evas_cflags@ @ecore_cflags@ @edb_cflags@

noinst_LTLIBRARIES = libetox_style.la

libetox_style_la_SOURCES = \
Etox_Style.h \
etox_style.c \
etox_style_style.c \
etox_style_color.c \
etox_style_heap.c \
etox_style_color.h \
etox_style_heap.h \
etox_style_private.h \
etox_style_style.h

libetox_style_la_LIBADD       = @evas_libs@ @ecore_libs@
libetox_style_la_DEPENDENCIES = $(top_builddir)/config.h

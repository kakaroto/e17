## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I. -I$(top_srcdir) \
-DPREFIX=\""$(prefix)"\" @IMLIB_CFLAGS@

lib_LTLIBRARIES = libgiblib.la

installed_headers_DATA = \
giblib.h gib_list.h gib_utils.h gib_style.h gib_imlib.h giblib_config.h

installed_headersdir = $(prefix)/include/giblib

libgiblib_la_SOURCES = \
    gib_debug.h \
    giblib.h \
    gib_list.c gib_list.h \
    gib_style.c gib_style.h \
    gib_imlib.c gib_imlib.h \
    gib_utils.c gib_utils.h

libgiblib_la_LIBADD = @IMLIB_LIBS@
libgiblib_la_DEPENDENCIES = $(top_builddir)/config.h
libgiblib_la_LDFLAGS = -version-info 1:0:1


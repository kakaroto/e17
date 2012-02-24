MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
@POINT_CORE_CFLAGS@

noinst_LTLIBRARIES = libexotic_coyote.la

libexotic_coyote_la_SOURCES = \
exotic_time.cpp \
exotic_main.c \
exotic_stdlib.c \
exotic_mman.cpp \
exotic_inet.c \
exotic_alloc.c \
exotic_stdio.c \
exotic_blanck.c \
exotic_private.h \
exotic_assert.cpp

libexotic_coyote_la_LIBADD =
libexotic_coyote_la_CFLAGS = @POINT_CORE_CFLAGS@


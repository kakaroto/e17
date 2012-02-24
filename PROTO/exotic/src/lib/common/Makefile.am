MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib

noinst_LTLIBRARIES = libexotic_common.la

libexotic_common_la_SOURCES = \
exotic_strings.c

if EXOTIC_FNMATCH
libexotic_common_la_SOURCES += \
exotic_fnmatch.c \
exotic_fnmatch_list_of_states.c
endif

if EXOTIC_BASENAME
libexotic_common_la_SOURCES += exotic_basename.c
endif

if EXOTIC_DIRNAME
libexotic_common_la_SOURCES += exotic_dirname.c
endif

EXTRA_DIST=exotic_fnmatch_private.h

libexotic_common_la_LIBADD =
libexotic_common_la_CFLAGS =


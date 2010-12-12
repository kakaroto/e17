AM_CFLAGS = \
 -I$(top_srcdir) \
 -I$(top_srcdir)/src/include \
 -I$(top_srcdir)/src/include/extras
AM_CXXFLAGS = $(AM_CFLAGS)
LIBADD = -lm

noinst_LTLIBRARIES = \
libcJSON.la \
libb64.la \
libpugixml.la

libpugixml_la_SOURCES = pugixml.cpp
libpugixml_la_LIBADD =

libcJSON_la_SOURCES = cJSON.c

libb64_la_SOURCES = cencode.c cdecode.c

MAINTAINERCLEANFILES = Makefile.in

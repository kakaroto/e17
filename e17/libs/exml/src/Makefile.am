## Process this file with automake to produce Makefile.in

INCLUDES = \
@ecore_cflags@ \
@xml2_cflags@ \
@xslt_cflags@

lib_LTLIBRARIES = libexml.la
include_HEADERS = \
EXML.h

libexml_la_SOURCES = \
exml.c

libexml_la_LIBADD = -lm @xml2_libs@
libexml_la_LDFLAGS = -version-info 1:0:0

MAINTAINERCLEANFILES = Makefile.in config.h.in

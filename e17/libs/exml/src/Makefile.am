## Process this file with automake to produce Makefile.in

INCLUDES = \
@ECORE_CFLAGS@ \
@XML2_CFLAGS@ \
@XSLT_CFLAGS@

lib_LTLIBRARIES = libexml.la
include_HEADERS = \
EXML.h

libexml_la_SOURCES = \
exml.c

libexml_la_LIBADD = @XSL_LIBS@ @XML2_LIBS@ @ECORE_LIBS@ -lm
libexml_la_LDFLAGS = -version-info 1:0:0

MAINTAINERCLEANFILES = Makefile.in config.h.in

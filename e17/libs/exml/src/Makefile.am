
MAINTAINERCLEANFILES = Makefile.in config.h.in

AM_CPPFLAGS = @EXML_CFLAGS@

lib_LTLIBRARIES = libexml.la
include_HEADERS = \
EXML.h

libexml_la_SOURCES = \
exml.c

libexml_la_LIBADD = @EXML_LIBS@ -lm
libexml_la_LDFLAGS = -version-info @version_info@

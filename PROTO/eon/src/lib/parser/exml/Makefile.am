MAINTAINERCLEANFILES = Makefile.in

if BUILD_PARSER_EXML

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ETK_CFLAGS@ \
@EXML_CFLAGS@


noinst_LTLIBRARIES = libeon_parser_exml.la

libeon_parser_exml_la_SOURCES = \
exml_main.c

libeon_parser_exml_la_LIBADD = \
@EXML_LIBS@

endif
SUBDIRS = common

MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

noinst_LTLIBRARIES = libeon_parser.la

EXTRA_DIST = 

libeon_parser_la_SOURCES =
libeon_parser_la_DEPENDENCIES = $(top_builddir)/src/lib/parser/common/libeon_parser_common.la
libeon_parser_la_LIBADD = $(top_builddir)/src/lib/parser/common/libeon_parser_common.la 


if BUILD_PARSER_EXPAT

SUBDIRS += expat

libeon_parser_la_DEPENDENCIES += $(top_builddir)/src/lib/parser/expat/libeon_parser_expat.la
libeon_parser_la_LIBADD += $(top_builddir)/src/lib/parser/expat/libeon_parser_expat.la

endif

if BUILD_PARSER_EXML

SUBDIRS += exml

libeon_parser_la_DEPENDENCIES += $(top_builddir)/src/lib/parser/exml/libeon_parser_exml.la
libeon_parser_la_LIBADD += $(top_builddir)/src/lib/parser/exml/libeon_parser_exml.la

endif
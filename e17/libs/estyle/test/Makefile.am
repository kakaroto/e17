AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

CFLAGS			=	-g -W -Wall -Wno-implicit
INCLUDES		=	-I$(top_srcdir)/src @evas_cflags@ @ewd_cflags@ @ecore_cflags@ @edb_cflags@

bin_PROGRAMS			=	estyle_test_simple

estyle_test_simple_DEPENDENCIES	=	$(top_builddir)/src/libestyle.la
estyle_test_simple_SOURCES	=	estyle_test_simple.c
estyle_test_simple_LDFLAGS	=	-static
estyle_test_simple_LDADD		=	$(top_builddir)/src/libestyle.la @evas_libs@ @evas_libs@ @ecore_libs@ @edb_libs@

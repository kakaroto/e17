AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

CFLAGS			=	-g -W -Wall -Wno-implicit
INCLUDES		=	-I$(top_srcdir)/src @evas_cflags@ @ewd_cflags@ @estyle_cflags@ @ecore_cflags@ @edb_cflags@

bin_PROGRAMS = etox_test

etox_test_DEPENDENCIES = $(top_builddir)/src/libetox.la
etox_test_SOURCES = \
etox_test.c \
tests.c\
Etox_test.h \
tests.h
etox_test_LDFLAGS	=	-static
etox_test_LDADD = $(top_builddir)/src/libetox.la @evas_libs@ @ewd_libs@ @estyle_libs@ @ecore_libs@ @edb_libs@

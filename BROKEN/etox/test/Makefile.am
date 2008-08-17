AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS		=	-W -Wall -Wno-implicit
INCLUDES		=	-I$(top_srcdir) -I$(top_srcdir)/src @evas_cflags@ @ecore_cflags@ @edb_cflags@
LIBS 			=   -lfreetype
if BUILD_ETOX_TEST
bin_PROGRAMS = etox_test etox_selections

etox_test_DEPENDENCIES = $(top_builddir)/src/libetox.la
etox_test_SOURCES = \
etox_test.c \
tests.c \
panel.c \
panel.h \
Etox_test.h \
tests.h
#etox_test_LDFLAGS	=	-static
etox_test_LDADD = $(top_builddir)/src/libetox.la @evas_libs@ @ecore_libs@ @edb_libs@

etox_selections_DEPENDENCIES = $(top_builddir)/src/libetox.la
etox_selections_SOURCES = selections.c 
#etox_selections_LDFLAGS	=	-static
etox_selections_LDADD = $(top_builddir)/src/libetox.la @evas_libs@ @ecore_libs@ @edb_libs@
endif

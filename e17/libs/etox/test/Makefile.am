AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

CFLAGS			=	-g -W -Wall -Wno-implicit
INCLUDES		=	-I$(top_srcdir)/src @evas_cflags@ @ewd_cflags@

bin_PROGRAMS			=	etox_test_simple etox_test_drag

etox_test_simple_DEPENDENCIES	=	$(top_builddir)/src/libetox.la
etox_test_simple_SOURCES	=	etox_test_simple.c
etox_test_simple_LDFLAGS	=	-static
etox_test_simple_LDADD		=	$(top_builddir)/src/libetox.la @evas_libs@ @evas_libs@ -lecore

etox_test_drag_DEPENDENCIES	=	$(top_builddir)/src/libetox.la
etox_test_drag_SOURCES		=	etox_test_drag.c
etox_test_drag_LDFLAGS		=	-static
etox_test_drag_LDADD		=	$(top_builddir)/src/libetox.la @evas_libs@ @ewd_libs@

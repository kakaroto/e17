MAINTAINERCLEANFILES 		= 	Makefile.in

SUBDIRS				=	tests

if EWL_TESTS_BUILD
EWLTESTPROGRAMS                 =	ewl_test ewl_embed_test ewl_simple_test
else
EWLTESTPROGRAMS                 =
endif

AM_CPPFLAGS			=	-I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib -DPACKAGE_DATA_DIR=\"$(datadir)\" -DPACKAGE_LIB_DIR=\"$(libdir)\" @EMOTION_CFLAGS@ @EPSILON_CFLAGS@ @EDJE_CFLAGS@ @ECORE_EVAS_CFLAGS@ @ECORE_CFLAGS@ @EVAS_CFLAGS@

bin_PROGRAMS			=	$(EWLTESTPROGRAMS)

ewl_test_DEPENDENCIES		=	$(top_builddir)/src/lib/libewl.la
ewl_test_SOURCES		=	Ewl_Test.h ewl_test_private.h main.c

ewl_test_LDADD			=	$(top_builddir)/src/lib/libewl.la @EMOTION_LIBS@ @EPSILON_LIBS@ @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@

ewl_embed_test_DEPENDENCIES	=	$(top_builddir)/src/lib/libewl.la
ewl_embed_test_SOURCES		=	ewl_embed_test.c

# ewl_embed_test_LDFLAGS	=
ewl_embed_test_LDADD		=	$(top_builddir)/src/lib/libewl.la @EMOTION_LIBS@ @EPSILON_LIBS@ @EDJE_LIBS@ @ECORE_EVAS_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@

ewl_simple_test_DEPENDENCIES	=	$(top_builddir)/src/lib/libewl.la
ewl_simple_test_SOURCES		=	ewl_simple_test.c

# ewl_simple_test_LDFLAGS	=
ewl_simple_test_LDADD		=	$(top_builddir)/src/lib/libewl.la @EMOTION_LIBS@ @EPSILON_LIBS@ @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@

EXTRA_DIST 			= 	$(ewl_test_SOURCES) \
					$(ewl_embed_test_SOURCES) \
					$(ewl_simple_test_SOURCES)

exampledir 			=	$(pkgdatadir)/examples/
example_DATA 			=	$(ewl_test_SOURCES)

installed_headersdir = $(prefix)/include/ewl
installed_headers_DATA = Ewl_Test.h


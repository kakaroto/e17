AUTOMAKE_OPTIONS 		= 	1.4 foreign
MAINTAINERCLEANFILES 		= 	Makefile.in

SUBDIRS				=	tests

INCLUDES			=	-I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib @EDJE_CFLAGS@ @ECORE_CFLAGS@ @EVAS_CFLAGS@ @EMOTION_CFLAGS@ @EPSILON_CFLAGS@

EWLTESTPROGRAMS                 =	ewl_test ewl_embed_test ewl_simple_test 

bin_PROGRAMS			=	$(EWLTESTPROGRAMS)

ewl_test_DEPENDENCIES		=	$(top_builddir)/src/lib/libewl.la
ewl_test_SOURCES		=	Ewl_Test.h main.c

ewl_test_LDADD			=	$(top_builddir)/src/lib/libewl.la @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EMOTION_LIBS@ @EPSILON_LIBS@

ewl_embed_test_DEPENDENCIES	=	$(top_builddir)/src/lib/libewl.la
ewl_embed_test_SOURCES		=	ewl_embed_test.c

# ewl_embed_test_LDFLAGS	=	
ewl_embed_test_LDADD		=	$(top_builddir)/src/lib/libewl.la @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EMOTION_LIBS@ @EPSILON_LIBS@

ewl_simple_test_DEPENDENCIES	=	$(top_builddir)/src/lib/libewl.la
ewl_simple_test_SOURCES		=	ewl_simple_test.c

# ewl_simple_test_LDFLAGS	=	
ewl_simple_test_LDADD		=	$(top_builddir)/src/lib/libewl.la @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EMOTION_LIBS@ @EPSILON_LIBS@

EXTRA_DIST 			= 	$(ewl_test_SOURCES) \
					$(ewl_embed_test_SOURCES) \
					$(ewl_simple_test_SOURCES)

exampledir 			=	$(pkgdatadir)/examples/
example_DATA 			=	$(ewl_test_SOURCES)

installed_headersdir = $(prefix)/include/ewl
installed_headers_DATA = Ewl_Test.h


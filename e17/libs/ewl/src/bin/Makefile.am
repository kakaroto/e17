AUTOMAKE_OPTIONS 		= 	1.4 foreign
MAINTAINERCLEANFILES 		= 	Makefile.in

SUBDIRS				=	tests

AM_CFLAGS			=	-W -Wall -Wno-implicit
INCLUDES			=	-I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib @EDJE_CFLAGS@ @ECORE_CFLAGS@ @EVAS_CFLAGS@ @EMOTION_CFLAGS@

EWLTESTPROGRAMS                 =	ewl_test ewl_test2 ewl_embed_test ewl_simple_test ewl_media_test

bin_PROGRAMS			=	$(EWLTESTPROGRAMS)

ewl_test_DEPENDENCIES		=	$(top_builddir)/src/lib/libewl.la
ewl_test_SOURCES		=	ewl_test.h \
					ewl_test.c \
					ewl_imenu_test.c \
					ewl_menu_test.c \
					ewl_theme_test.c 

# ewl_test_LDFLAGS		=	
ewl_test_LDADD			=	$(top_builddir)/src/lib/libewl.la @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EMOTION_LIBS@ 

ewl_test2_DEPENDENCIES		=	$(top_builddir)/src/lib/libewl.la
ewl_test2_SOURCES		=	Ewl_Test2.h main.c

ewl_test2_LDADD			=	$(top_builddir)/src/lib/libewl.la @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EMOTION_LIBS@ 

ewl_embed_test_DEPENDENCIES	=	$(top_builddir)/src/lib/libewl.la
ewl_embed_test_SOURCES		=	ewl_embed_test.c

# ewl_embed_test_LDFLAGS	=	
ewl_embed_test_LDADD		=	$(top_builddir)/src/lib/libewl.la @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EMOTION_LIBS@

ewl_simple_test_DEPENDENCIES	=	$(top_builddir)/src/lib/libewl.la
ewl_simple_test_SOURCES		=	ewl_simple_test.c

# ewl_simple_test_LDFLAGS	=	
ewl_simple_test_LDADD		=	$(top_builddir)/src/lib/libewl.la @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EMOTION_LIBS@

ewl_media_test_DEPENDENCIES		=	$(top_builddir)/src/lib/libewl.la
ewl_media_test_SOURCES			=	ewl_media_test.c

# ewl_media_test_LDFLAGS		=	
ewl_media_test_LDADD			= $(top_builddir)/src/lib/libewl.la @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EMOTION_LIBS@

EXTRA_DIST 			= 	$(ewl_test_SOURCES) \
					$(ewl_embed_test_SOURCES) \
					$(ewl_simple_test_SOURCES)

exampledir 			=	$(pkgdatadir)/examples/
example_DATA 			=	$(ewl_test_SOURCES)

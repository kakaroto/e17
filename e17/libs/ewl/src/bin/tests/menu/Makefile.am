AUTOMAKE_OPTIONS 	= 	1.4 foreign
MAINTAINERCLEANFILES 	= 	Makefile.in

INCLUDES		=	-I$(top_srcdir)/src/lib \
				-I$(top_builddir)/src/lib \
				-I$(top_srcdir)/src/bin \
				-I$(top_builddir)/src/bin \
				@EDJE_CFLAGS@ \
				@ECORE_CFLAGS@ \
				@EVAS_CFLAGS@ \
				@EMOTION_CFLAGS@

pkgdir                  = $(libdir)/ewl/tests

pkg_LTLIBRARIES		= ewl_menu.la

ewl_menu_la_SOURCES   = ewl_menu.c
ewl_menu_la_LIBADD    = $(top_builddir)/src/lib/libewl.la
ewl_menu_la_LDFLAGS	= -module -avoid-version \
			  -L$(top_builddir)/src/lib \
			  -L$(top_builddir)/src/lib/.libs
ewl_menu_la_DEPENDENCIES = 

FILES = $(ewl_menu_la_SOURCES)

EXTRA_DIST = $(FILES)

exampledir 			=	$(pkgdatadir)/examples/
example_DATA 			=	$(FILES)


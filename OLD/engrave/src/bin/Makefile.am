
MAINTAINERCLEANFILES 	= Makefile.in

AM_CFLAGS	= -W -Wall -Wno-implicit
AM_CPPFLAGS	= -I$(top_srcdir)/src/lib @ECORE_CFLAGS@

bin_PROGRAMS	= engrave_test engrave_canvas_test

engrave_test_DEPENDENCIES 		= $(top_builddir)/src/lib/libengrave.la
engrave_test_SOURCES 			= engrave_test_main.c
engrave_test_LDADD			= $(top_builddir)/src/lib/libengrave.la @ECORE_LIBS@

engrave_canvas_test_DEPENDENCIES 	= $(top_builddir)/src/lib/libengrave.la
engrave_canvas_test_SOURCES 		= engrave_canvas_test_main.c
engrave_canvas_test_LDADD		= $(top_builddir)/src/lib/libengrave.la @ECORE_LIBS@


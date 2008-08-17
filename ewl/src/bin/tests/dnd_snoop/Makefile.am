
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/bin \
-I$(top_builddir)/src/bin \
@EDJE_CFLAGS@ \
@ECORE_CFLAGS@ \
@ECORE_X_CFLAGS@ \
@EVAS_CFLAGS@ \
@EMOTION_CFLAGS@

if EWL_TESTS_BUILD

pkgdir = $(libdir)/ewl/tests

pkg_LTLIBRARIES = ewl_dnd_snoop_test.la

ewl_dnd_snoop_test_la_SOURCES = ewl_dnd_snoop_test.c
ewl_dnd_snoop_test_la_LIBADD = $(top_builddir)/src/lib/libewl.la @ECORE_X_LIBS@
ewl_dnd_snoop_test_la_LDFLAGS = -module @lt_no_undefined@ -avoid-version

FILES = $(ewl_dnd_snoop_test_la_SOURCES)

exampledir = $(pkgdatadir)/examples/
example_DATA = $(FILES)

endif

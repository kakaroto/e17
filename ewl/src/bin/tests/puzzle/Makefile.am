
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/bin \
-I$(top_builddir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)\" \
@EDJE_CFLAGS@ \
@ECORE_CFLAGS@ \
@EVAS_CFLAGS@ \
@EMOTION_CFLAGS@

if EWL_TESTS_BUILD

pkgdir = $(libdir)/ewl/tests

pkg_LTLIBRARIES = ewl_puzzle_test.la

ewl_puzzle_test_la_SOURCES = ewl_puzzle_test.c
ewl_puzzle_test_la_LIBADD = $(top_builddir)/src/lib/libewl.la
ewl_puzzle_test_la_LDFLAGS = -module @lt_no_undefined@ -avoid-version

FILES = $(ewl_puzzle_test_la_SOURCES)

exampledir = $(pkgdatadir)/examples/
example_DATA = $(FILES)

endif

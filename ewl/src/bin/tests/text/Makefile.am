
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/bin \
-I$(top_builddir)/src/bin \
@EDJE_CFLAGS@ \
@ECORE_CFLAGS@ \
@EVAS_CFLAGS@ \
@EMOTION_CFLAGS@

if EWL_TESTS_BUILD

pkgdir = $(libdir)/ewl/tests

pkg_LTLIBRARIES = ewl_text_test.la

ewl_text_test_la_SOURCES = ewl_text_test.c \
			   ewl_text_unit_test.c
ewl_text_test_la_LIBADD = $(top_builddir)/src/lib/libewl.la
ewl_text_test_la_LDFLAGS = -module @lt_no_undefined@ -avoid-version

FILES = ewl_text_test.c

exampledir = $(pkgdatadir)/examples/
example_DATA = $(FILES)

endif

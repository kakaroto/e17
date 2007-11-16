
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

pkg_LTLIBRARIES = ewl_object.la

ewl_object_la_SOURCES = ewl_object.c
ewl_object_la_LIBADD = $(top_builddir)/src/lib/libewl.la
ewl_object_la_LDFLAGS = -module @create_shared_lib@ -avoid-version 

FILES = $(ewl_object_la_SOURCES)

exampledir = $(pkgdatadir)/examples/
example_DATA = $(FILES)

endif

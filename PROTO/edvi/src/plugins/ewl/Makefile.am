
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EDVI_EWL
EDVI_EWL_PLUGIN = libewl_dvi.la
EDVI_EWL_PLUGIN_SOURCE = ewl_dvi.c
else
EDVI_EWL_PLUGIN =
EDVI_EWL_PLUGIN_SOURCE =
endif

pkgdir = @EWL_TEST_LIBS_DIR@

pkg_LTLIBRARIES = $(EDVI_EWL_PLUGIN)

libewl_dvi_la_SOURCES = $(EDVI_EWL_PLUGIN_SOURCE)

libewl_dvi_la_CPPFLAGS = \
-I$(top_builddir)/src/lib \
@EWL_CFLAGS@

libewl_dvi_la_LIBADD  = \
$(top_builddir)/src/lib/libedvi.la \
@EWL_LIBS@

libewl_dvi_la_LDFLAGS = \
-module \
-avoid-version

libewl_dvi_la_DEPENDENCIES = $(top_builddir)/src/lib/libedvi.la

EXTRA_DIST = $(libewl_dvi_la_SOURCES)

exampledir = @EWL_TEST_SOURCE_DIR@

example_DATA = $(libewl_dvi_la_SOURCES)

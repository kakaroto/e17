
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPS_EWL
EPS_EWL_PLUGIN = libewl_ps.la
EPS_EWL_PLUGIN_SOURCE = ewl_ps.c
else
EPS_EWL_PLUGIN =
EPS_EWL_PLUGIN_SOURCE =
endif

pkgdir = @EWL_TEST_LIBS_DIR@
pkg_LTLIBRARIES = $(EPS_EWL_PLUGIN)

libewl_ps_la_SOURCES = $(EPS_EWL_PLUGIN_SOURCE)
libewl_ps_la_CPPFLAGS  = @EWL_CFLAGS@ -I$(top_builddir)/src/lib
libewl_ps_la_LIBADD  = $(top_builddir)/src/lib/libeps.la @EWL_LIBS@
libewl_ps_la_LDFLAGS = -module -avoid-version
libewl_ps_la_DEPENDENCIES = $(top_builddir)/src/lib/libeps.la

EXTRA_DIST = $(libewl_ps_la_SOURCES)
exampledir = @EWL_TEST_SOURCE_DIR@
example_DATA = $(libewl_ps_la_SOURCES)

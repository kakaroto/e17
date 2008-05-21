
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPDF_EPSILON
EPDF_EPSILON_PLUGIN = epdf_thumbnailer.la
EPDF_EPSILON_PLUGIN_SOURCES = epsilon_thumbnailer.c
else
EPDF_EPSILON_PLUGIN =
EPDF_EPSILON_PLUGIN_SOURCES =
endif

pkgdir = @EPSILON_PLUGINS_DIR@

pkg_LTLIBRARIES = $(EPDF_EPSILON_PLUGIN)

epdf_thumbnailer_la_SOURCES = $(EPDF_EPSILON_PLUGIN_SOURCES)

epdf_thumbnailer_la_CPPFLAGS = -I$(top_builddir)/src/lib @EPSILON_CFLAGS@

epdf_thumbnailer_la_LDFLAGS = @create_shared_lib@ -module -avoid-version

epdf_thumbnailer_la_LIBADD = $(top_builddir)/src/lib/libepdf.la @EPSILON_LIBS@

epdf_thumbnailer_la_DEPENDENCIES = $(top_builddir)/src/lib/libepdf.la

EXTRA_DIST = $(epdf_thumbnailer_la_SOURCES)

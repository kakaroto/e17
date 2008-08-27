
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPS_EPSILON
EPS_EPSILON_PLUGIN = eps_thumbnailer.la
EPS_EPSILON_PLUGIN_SOURCES = epsilon_thumbnailer.c
else
EPS_EPSILON_PLUGIN =
EPS_EPSILON_PLUGIN_SOURCES =
endif

pkgdir = @EPSILON_PLUGINS_DIR@
pkg_LTLIBRARIES = $(EPS_EPSILON_PLUGIN)

eps_thumbnailer_la_SOURCES = $(EPS_EPSILON_PLUGIN_SOURCES)
eps_thumbnailer_la_LDFLAGS = -module -avoid-version
eps_thumbnailer_la_LIBADD = @EPSILON_LIBS@ $(top_builddir)/src/lib/libeps.la
eps_thumbnailer_la_CFLAGS = -I$(top_builddir)/src/lib @EPSILON_CFLAGS@
eps_thumbnailer_la_DEPENDENCIES = $(top_builddir)/src/lib/libeps.la

EXTRA_DIST = $(eps_thumbnailer_la_SOURCES)

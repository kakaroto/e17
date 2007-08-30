
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EDVI_EPSILON
EDVI_EPSILON_PLUGIN = edvi_thumbnailer.la
else
EDVI_EPSILON_PLUGIN =
endif

pkgdir                  = @EPSILON_PLUGINS_DIR@
pkg_LTLIBRARIES         = $(EDVI_EPSILON_PLUGIN)

edvi_thumbnailer_la_SOURCES = epsilon_thumbnailer.c
edvi_thumbnailer_la_LDFLAGS = -module -avoid-version
edvi_thumbnailer_la_LIBADD = @EPSILON_LIBS@ $(top_builddir)/src/lib/libedvi.la
edvi_thumbnailer_la_CFLAGS = -I$(top_builddir)/src/lib @EPSILON_CFLAGS@

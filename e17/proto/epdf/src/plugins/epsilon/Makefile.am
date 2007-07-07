
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPDF_EPSILON
EPDF_EPSILON_PLUGIN = epdf_thumbnailer.la
else
EPDF_EPSILON_PLUGIN =
endif

pkgdir                  = @EPSILON_PLUGINS_DIR@
pkg_LTLIBRARIES         = $(EPDF_EPSILON_PLUGIN)

epdf_thumbnailer_la_SOURCES = pdf_thumbnailer.c
epdf_thumbnailer_la_LDFLAGS = -module -avoid-version
epdf_thumbnailer_la_LIBADD = @EPDF_LIBS@ @EPSILON_LIBS@
epdf_thumbnailer_la_CFLAGS = -I$(top_builddir)/src/lib @EPSILON_CFLAGS@


MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\"

INCLUDES = \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@E_CFLAGS@


include_HEADERS = EMap.h 

lib_LTLIBRARIES = libemap.la

libemap_la_SOURCES  = \
emap.c \
route.c \
route_node.c \
route_gpx.c 

libemap_la_DEPENDENCIES = $(top_builddir)/config.h
libemap_la_LDFLAGS = @E_LIBS@

clean-local:
	@rm -rf *.gcno

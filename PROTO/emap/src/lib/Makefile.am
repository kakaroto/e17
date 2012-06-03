MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = @EMAP_CFLAGS@

include_HEADERS = EMap.h 

lib_LTLIBRARIES = libemap.la

libemap_la_SOURCES  = \
emap.c \
route.c \
route_node.c \
route_gpx.c 

libemap_la_LIBADD = @EMAP_LIBS@
libemap_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@

EXTRA_DIST = emap_private.h

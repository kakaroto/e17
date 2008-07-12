
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_container \
@CONTAINER_CFLAGS@

plugin_LTLIBRARIES = default.la
plugindir = @libdir@/@PACKAGE@/layout

default_la_SOURCES = default.c
default_la_LIBADD = $(top_builddir)/src/lib/esmart_container/libesmart_container.la @CONTAINER_LIBS@
default_la_LDFLAGS = -no-undefined -module -avoid-version

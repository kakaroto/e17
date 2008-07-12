
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_container \
@CONTAINER_CFLAGS@

plugin_LTLIBRARIES = entice.la
plugindir = @libdir@/@PACKAGE@/layout

entice_la_SOURCES = entice.c
entice_la_LIBADD = $(top_builddir)/src/lib/esmart_container/libesmart_container.la @CONTAINER_LIBS@
entice_la_LDFLAGS = -no-undefined -module -avoid-version

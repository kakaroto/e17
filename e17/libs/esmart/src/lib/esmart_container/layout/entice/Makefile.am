
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_container \
@CONTAINER_CFLAGS@

plugin_LTLIBRARIES = entice.la
plugindir = @libdir@/@PACKAGE@/layout

entice_la_SOURCES = entice.c
entice_la_LDFLAGS = -module -avoid-version


MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_container \
@CONTAINER_CFLAGS@

plugin_LTLIBRARIES = default.la
plugindir = @libdir@/@PACKAGE@/layout

default_la_SOURCES = default.c
default_la_LDFLAGS = -module -avoid-version

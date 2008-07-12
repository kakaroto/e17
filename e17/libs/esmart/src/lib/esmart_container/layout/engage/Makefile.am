
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_container \
@CONTAINER_CFLAGS@

plugindir = @libdir@/@PACKAGE@/layout
plugin_LTLIBRARIES = engage.la

engage_la_SOURCES = engage.c
engage_la_LIBADD = $(top_builddir)/src/lib/esmart_container/libesmart_container.la @CONTAINER_LIBS@
engage_la_LDFLAGS = -no-undefined -module -avoid-version

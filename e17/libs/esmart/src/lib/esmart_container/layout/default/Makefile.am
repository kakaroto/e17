plugin_LTLIBRARIES = default.la
plugindir = @libdir@/@PACKAGE@/layout

INCLUDES = -I$(top_srcdir)/src/lib/esmart_container \
	   @imlib_cflags@ \
	   @evas_cflags@


default_la_SOURCES = default.c
default_la_LDFLAGS = -module -avoid-version

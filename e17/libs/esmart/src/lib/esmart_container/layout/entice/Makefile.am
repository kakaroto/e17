plugin_LTLIBRARIES = entice.la
plugindir = @libdir@/@PACKAGE@/layout

INCLUDES = -I$(top_srcdir)/src/lib/esmart_container \
	   @IMLIB2_CFLAGS@ \
	   @EVAS_CFLAGS@

entice_la_SOURCES = entice.c
entice_la_LDFLAGS = -module -avoid-version

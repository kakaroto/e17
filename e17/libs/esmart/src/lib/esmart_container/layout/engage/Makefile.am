plugindir = @libdir@/@PACKAGE@/layout
plugin_LTLIBRARIES = engage.la

INCLUDES = -I$(top_srcdir)/src/lib/esmart_container \
	   @IMLIB2_CFLAGS@ \
	   @EVAS_CFLAGS@


engage_la_SOURCES = engage.c
engage_la_LDFLAGS = -module -avoid-version

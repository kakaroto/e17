AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

INCLUDES			= -I$(top_srcdir)/src \
				  -I$(top_srcdir)/src/thumb \
				  -I$(top_srcdir)/src/container \
				    @evas_cflags@ \
				    @imlib_cflags@ \
				    @ecore_cflags@

bin_PROGRAMS			= esmart_test

esmart_test_CFLAGS		= -g
esmart_test_DEPENDENCIES	= $(top_builddir)/src/libesmart.la
esmart_test_SOURCES		= esmart_main.c 

esmart_test_LDADD		= $(top_builddir)/src/libesmart.la \
				    @evas_libs@ \
				    @imlib_libs@ \
				    @ecore_libs@

AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

INCLUDES			= -I$(top_srcdir)/src \
				  -I$(top_srcdir)/src/thumb \
				  -I$(top_srcdir)/src/container \
				  -I$(top_srcdir)/src/trans \
				  -I$(top_srcdir)/src/draggies \
				  -I$(top_srcdir)/src/entry \
				  -I$(top_srcdir)/src/file_dialog \
				    @evas_cflags@ \
				    @imlib_cflags@ \
				    @ecore_cflags@

bin_PROGRAMS			= esmart_test esmart_file_dialog_test

esmart_test_CFLAGS		= -g
esmart_test_DEPENDENCIES	= $(top_builddir)/src/libesmart.la
esmart_test_SOURCES		= esmart_main.c 

esmart_test_LDADD		= $(top_builddir)/src/libesmart.la \
				    @evas_libs@ \
				    @imlib_libs@ \
				    @ecore_libs@

esmart_file_dialog_test_CFLAGS		= -g
esmart_file_dialog_test_DEPENDENCIES	= $(top_builddir)/src/libesmart.la
esmart_file_dialog_test_SOURCES		= esmart_file_dialog_main.c 

esmart_file_dialog_test_LDADD		= $(top_builddir)/src/libesmart.la \
				    @evas_libs@ \
				    @imlib_libs@ \
				    @ecore_libs@

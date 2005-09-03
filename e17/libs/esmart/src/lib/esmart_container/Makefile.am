SUBDIRS = layout

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_container.la

INCLUDES = -I$(top_srcdir)/src/lib/esmart_container \
	   @IMLIB2_CFLAGS@ \
	   @EVAS_CFLAGS@

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Container.h

libesmart_container_la_SOURCES = esmart_container.c \
				 Esmart_Container.h \
				 esmart_container_element.c \
				 esmart_container_plugin.c \
				 esmart_container_private.h \
				 esmart_container_smart.c \
				 esmart_container_util.c

libesmart_container_la_LDFLAGS = -version-info 0:9:0
libesmart_container_la_LIBADD = @IMLIB2_LIBS@ @EVAS_LIBS@ -lltdl -lm

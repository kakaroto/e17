SUBDIRS = layout

pkgdir = @libdir@/@PACKAGE@
pkg_LTLIBRARIES = libesmart_container.la

INCLUDES = -I. \
			  @imlib_cflags@ \
			  @evas_cflags@

installed_headersdir = @prefix@/include/Esmart
installed_headers_DATA = Esmart_Container.h

libesmart_container_la_SOURCES = esmart_container.c \
											Esmart_Container.h \
											esmart_container_element.c \
											esmart_container_plugin.c \
											esmart_container_plugin.h \
											esmart_container_smart.c \
											esmart_container_util.c

libesmart_container_la_LDFLAGS = -shared -version-info 0:9:0
libesmart_container_la_LIBADD = @imlib_libs@ @evas_libs@ -lm


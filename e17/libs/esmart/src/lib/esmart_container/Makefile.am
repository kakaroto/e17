
MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = . layout

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_container \
-DCONTAINER_PLUGIN_DIR=\"$(libdir)/$(PACKAGE)/layout\" \
@CONTAINER_CFLAGS@

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_container.la

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Container.h

libesmart_container_la_SOURCES = \
esmart_container.c \
Esmart_Container.h \
esmart_container_element.c \
esmart_container_plugin.c \
esmart_container_private.h \
esmart_container_smart.c \
esmart_container_util.c

libesmart_container_la_LDFLAGS = -no-undefined -version-info @version_info@
libesmart_container_la_LIBADD = @CONTAINER_LIBS@ -lltdl -lm

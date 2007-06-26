pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_resize.la

INCLUDES = -I$(top_srcdir)/src/lib/esmart_resize \
			  @RESIZE_CFLAGS@

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Resize.h

libesmart_resize_la_SOURCES = esmart_resize.c \
				Esmart_Resize.h

libesmart_resize_la_LDFLAGS = -version-info 0:9:0
libesmart_resize_la_LIBADD = @RESIZE_LIBS@


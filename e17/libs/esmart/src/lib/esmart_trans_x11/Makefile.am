pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_trans_x11.la

INCLUDES = -I. \
			  @imlib_cflags@ \
			  @evas_cflags@ \
			  @ecore_cflags@

installed_headersdir = @prefix@/include/Esmart
installed_headers_DATA = Esmart_Trans_X11.h

libesmart_trans_x11_la_SOURCES = esmart_trans_x11.c \
											Esmart_Trans_X11.h

libesmart_trans_x11_la_LDFLAGS = -shared -version-info 0:9:0 \
											-L/usr/X11R6/lib
libesmart_trans_x11_la_LIBADD = @imlib_libs@ @evas_libs@ -lecore -lecore_x


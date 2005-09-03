pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_trans_x11.la

INCLUDES = -I$(top_srcdir)/src/lib/esmart_trans_x11 \
			  @IMLIB2_CFLAGS@ \
			  @EVAS_CFLAGS@ \
			  @ECORE_CFLAGS@

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Trans_X11.h

libesmart_trans_x11_la_SOURCES = esmart_trans_x11.c \
				 Esmart_Trans_X11.h

libesmart_trans_x11_la_LDFLAGS = -version-info 0:9:0 \
				 -L/usr/X11R6/lib
libesmart_trans_x11_la_LIBADD = @IMLIB2_LIBS@ @EVAS_LIBS@ @ECORE_LIBS_X@


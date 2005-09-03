pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_thumb.la

INCLUDES = -I$(top_srcdir)/src/lib/esmart_thumb \
			  @IMLIB2_CFLAGS@ \
			  @EVAS_CFLAGS@ \
			  @EPSILON_CFLAGS@

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Thumb.h

libesmart_thumb_la_SOURCES = esmart_thumb.c \
			     Esmart_Thumb.h

libesmart_thumb_la_LDFLAGS = -version-info 0:9:0
libesmart_thumb_la_LIBADD = @IMLIB2_LIBS@ @EVAS_LIBS@ @EPSILON_LIBS@


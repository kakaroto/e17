pkgdir = @libdir@/@PACKAGE@
pkg_LTLIBRARIES = libesmart_thumb.la

INCLUDES = -I. \
			  @imlib_cflags@ \
			  @evas_cflags@ \
			  @epsilon_cflags@

installed_headersdir = @prefix@/include/Esmart
installed_headers_DATA = Esmart_Thumb.h

libesmart_thumb_la_SOURCES = esmart_thumb.c \
											Esmart_Thumb.h

libesmart_thumb_la_LDFLAGS = -shared -version-info 0:9:0
libesmart_thumb_la_LIBADD = @imlib_libs@ @evas_libs@ @epsilon_libs@


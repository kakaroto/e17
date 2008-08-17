MAINTAINERCLEANFILES = Makefile.in

pkgdir = @libdir@

if HAVE_CAIRO

pkg_LTLIBRARIES = libetk_cairo.la

INCLUDES = -I$(top_srcdir)/src/lib/etk_cairo \
			  @ETK_CFLAGS@ \
			  @CAIRO_CFLAGS@

installed_headersdir = $(prefix)/include/etk_extra
installed_headers_DATA = etk_cairo.h

libetk_cairo_la_SOURCES = etk_cairo.c \
			  etk_cairo.h

libetk_cairo_la_LDFLAGS = -version-info 0:9:0
libetk_cairo_la_LIBADD = @ETK_LIBS@ @CAIRO_LIBS@

endif

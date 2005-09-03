pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_draggies.la

INCLUDES = -I$(top_srcdir)/src/lib/esmart_draggies \
			  @EVAS_CFLAGS@ \
			  @ECORE_CFLAGS@

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Draggies.h

libesmart_draggies_la_SOURCES = esmart_draggies.c \
				Esmart_Draggies.h

libesmart_draggies_la_LDFLAGS = -version-info 0:9:0
libesmart_draggies_la_LIBADD = @EVAS_LIBS@ @ECORE_LIBS_X@ @ECORE_LIBS_EVAS@


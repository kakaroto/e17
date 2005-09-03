pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_textarea.la

INCLUDES = -I$(top_srcdir)/src/lib/esmart_textarea \
	   @EVAS_CFLAGS@ \
	   @ECORE_CFLAGS@ \
	   @EDJE_CFLAGS@

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Textarea.h

libesmart_textarea_la_SOURCES = esmart_textarea_callbacks.c \
				esmart_textarea_cursor.c \
				esmart_textarea_private.c \
				esmart_textarea_smart.c \
				esmart_textarea_public.c \
				Esmart_Textarea.h \
				esmart_textarea_private.h

libesmart_textarea_la_LDFLAGS = -version-info 0:9:0
libesmart_textarea_la_LIBADD = @ECORE_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@

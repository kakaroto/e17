pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_textarea.la

INCLUDES = -I$(top_srcdir)/src/lib/esmart_textarea \
	   @ecore_cflags@ \
	   @evas_cflags@

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Textarea.h

libesmart_textarea_la_SOURCES = esmart_textarea_callbacks.c \
				esmart_textarea_cursor.c \
				esmart_textarea_private.c \
				esmart_textarea_smart.c \
				esmart_textarea_public.c
				 

libesmart_textarea_la_LDFLAGS = -version-info 0:9:0
libesmart_textarea_la_LIBADD = @ecore_libs@ @evas_libs@

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_text_entry.la

INCLUDES = -I$(top_srcdir)/src/lib/esmart_text_entry \
			  @EVAS_CFLAGS@ \
			  @ECORE_CFLAGS@ \
			  @EDJE_CFLAGS@

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_Text_Entry.h

libesmart_text_entry_la_SOURCES = esmart_text_entry.c \
											Esmart_Text_Entry.h

libesmart_text_entry_la_LDFLAGS = -version-info 0:9:0
libesmart_text_entry_la_LIBADD = @EVAS_LIBS@ -lecore @EDJE_LIBS@


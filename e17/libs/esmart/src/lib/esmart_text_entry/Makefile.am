pkgdir = @libdir@/@PACKAGE@
pkg_LTLIBRARIES = libesmart_text_entry.la

INCLUDES = -I. \
			  @evas_cflags@ \
			  @ecore_cflags@ \
			  @edje_cflags@

installed_headersdir = @prefix@/include/Esmart
installed_headers_DATA = Esmart_Text_Entry.h

libesmart_text_entry_la_SOURCES = esmart_text_entry.c \
											Esmart_Text_Entry.h

libesmart_text_entry_la_LDFLAGS = -shared -version-info 0:9:0
libesmart_text_entry_la_LIBADD = @evas_libs@ -lecore -lecore_x @edje_libs@

